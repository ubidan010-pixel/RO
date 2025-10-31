#include "precompiled_core.h"
#include "MemMngPage.h"

namespace ITF
{

    // tools

    static u8 * ms_referencebaseAddress = (u8*)(SCE_KERNEL_APP_MAP_AREA_START_ADDR + 8 * MemoryPageManager::DEFAULT_VIRTUAL_SPACE_SIZE); // let of the first 64 Gio of virtual space

    static u8 * getNextVirtualSpaceAdress()
    {
        return ms_referencebaseAddress;
    }

    static void setNextVirtualSpaceAddress(u8 * _nextAddress)
    {
        ms_referencebaseAddress = _nextAddress;
    }

    static u8 * ReserveVirtualSpace(u8 * _baseAddress, uPtr _size, uPtr _pageAlignment)
    {
        void * baseAddress = _baseAddress;
        int ret = sceKernelReserveVirtualRange( &baseAddress, _size, 0, _pageAlignment);
        MEMMNG_ASSERT(ret == SCE_OK); // Out of virtual space !!
        return (u8 *)baseAddress;
    }

    static void ReleaseVirtualSpace(u8 * pageAdressBegin, uPtr _size)
    {
        int ret = sceKernelMunmap(pageAdressBegin, _size);
        MEMMNG_ASSERT(ret==SCE_OK); // Problem reported by OS
    }


    // This class is designed as a scope helper to handle the release of direct memory in the decommitPages method.
    // In order to call sceKernelReserveVirtualRange as less as possible, it's keeping a list of physical blocks.
    // As we are in an allocator, no memory allocation can be done, so a static number of blocks is kept.
    // NbPhysBlockEntry is the number of blocks kept until the virtual memory reserve is performed and the physical blocks are freed.
    // The size should be reasonably low in order to get the object on stack.
    // Init with a null ptr to not perform virtual memory reservation.
    template<ux NbPhysBlockEntry>
    class PhysicalFreeHelper
    {
    public:
        PhysicalFreeHelper(u8 * _virtualStart)
            : m_virtualStart(_virtualStart)
        {
            memset(m_physBlocks, 0, sizeof(m_physBlocks));
        }

        ~PhysicalFreeHelper()
        {
            flush();
        }

        bbool add(uPtr _physOffset, uPtr _size)
        {
            if (m_newPhysBlockEntry == NbPhysBlockEntry)
            {
                bbool success = flush();
                if (!success)
                    return bfalse;
            }
            PhysBlock & physBlock = m_physBlocks[m_newPhysBlockEntry];
            physBlock.offset = _physOffset;
            physBlock.size = _size;
            m_virtualSize += _size;
            m_newPhysBlockEntry++;
            return btrue;
        }

        bbool flush()
        {
            if (m_newPhysBlockEntry != 0)
            {
                if (m_virtualStart)
                {
                    void * reservedPage = m_virtualStart;
                    int ret = sceKernelReserveVirtualRange(&reservedPage, m_virtualSize, SCE_KERNEL_MAP_FIXED, 0);
                    MEMMNG_ASSERT(reservedPage==m_virtualStart);
                    MEMMNG_ASSERT(ret==SCE_OK);
                    if (ret!=SCE_OK)
                    {
                        m_newPhysBlockEntry = 0;
                        m_virtualSize = 0;
                        return bfalse;
                    }
                }

                for(ux i = 0, n = m_newPhysBlockEntry; i<n; ++i)
                {
                    PhysBlock & physBlock = m_physBlocks[i];
                    int ret = sceKernelReleaseDirectMemory(physBlock.offset, physBlock.size); // and unmap
                    MEMMNG_ASSERT(ret==SCE_OK); // continue, anyway the memory is remapped or forget...
                }

                m_newPhysBlockEntry = 0;
                if (m_virtualStart)
                    m_virtualStart += m_virtualSize;
                m_virtualSize = 0;
            }
            return btrue;
        }

    private:
        struct PhysBlock
        {
            uPtr offset;
            uPtr size;
        };
        uPtr m_virtualSize = 0;
        ux m_newPhysBlockEntry = 0;
        PhysBlock m_physBlocks[NbPhysBlockEntry];
        u8 * m_virtualStart = nullptr;
    };


    // MemoryPageManager

    MemoryPageManager::MemoryPageManager()
        : m_pAreaBegin((u8*)SCE_KERNEL_APP_MAP_AREA_START_ADDR)
        , m_pAreaEnd((u8*)SCE_KERNEL_APP_MAP_AREA_END_ADDR)
        , m_sceMemoryType(SCE_KERNEL_MTYPE_C_SHARED)
        , m_pageAlignment(VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT)
        , m_virtualMemReserved(bfalse)
    {
    }

    MemoryPageManager::~MemoryPageManager()
    {
        Close();
    }

    void MemoryPageManager::Init(const InitParams & _initParams)
    {
        m_sceMemoryType = _initParams.sceMemoryType;
        m_virtualMemReserved = _initParams.reserveVirtualSpace;

        m_pageAlignment = BitTweak::GetCeilPow2(u32(_initParams.pageAlignment));
        MEMMNG_ASSERT(m_pageAlignment == _initParams.pageAlignment); // not a pow2 alignment !
        m_pageAlignment = (m_pageAlignment<VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT)?VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT:m_pageAlignment;

        uPtr areaSize = BitTweak::GetAboveAlign(_initParams.areaSize, m_pageAlignment);

        m_pAreaBegin = getNextVirtualSpaceAdress();
        if (m_virtualMemReserved)
        {
            m_pAreaBegin = ReserveVirtualSpace(m_pAreaBegin, areaSize, m_pageAlignment);
        }
        m_pAreaEnd = m_pAreaBegin + areaSize;
        setNextVirtualSpaceAddress(m_pAreaEnd);
    }

    void MemoryPageManager::Close()
    {
        if (m_virtualMemReserved) // do not unmap
            ReleaseVirtualSpace(m_pAreaBegin, m_pAreaEnd - m_pAreaBegin);
        m_virtualMemReserved = bfalse;
    }

    void MemoryPageManager::checkCommitParameters(u8 * _pageBegin, uPtr _size, Protection _sceProt)
    {
        MEMMNG_ASSERT(m_pAreaEnd > m_pAreaBegin); // not init ?
        MEMMNG_ASSERT(_pageBegin >= m_pAreaBegin && _pageBegin < m_pAreaEnd); // not in range ?
        MEMMNG_ASSERT(_pageBegin+_size > m_pAreaBegin && _pageBegin+_size <= m_pAreaEnd); // not in range ?
        MEMMNG_ASSERT(_size == BitTweak::GetAboveAlign(_size, m_pageAlignment)); // probably a caller error
        MEMMNG_ASSERT(_size != 0); // probably a caller error
        MEMMNG_ASSERT(_size < (m_pAreaEnd - m_pAreaBegin)); // probably a caller error
        MEMMNG_ASSERT(_sceProt.sceInt <= Protection::allRW().sceInt); // probably a caller error
    }


    // Assign physical memory to a range of addresses (by page size granularity)
    // The physical memory is guaranteed to be contiguous.
    // _pageBegin and size has to be aligned on the page alignment specified at init
    // If the virtual space is reserved (init parameter), it's guaranteed to return _pageBegin or nullptr (failure).
    // If the virtual space is not reserved, it returns the next available following _pageBegin
    u8* MemoryPageManager::CommitPages(u8 * _pageBegin, uPtr _size, Protection  _sceProt)
    {
        checkCommitParameters(_pageBegin, _size, _sceProt);

        uPtr size = BitTweak::GetAboveAlign(_size, m_pageAlignment);
        off_t physMemory = 0;
        int ret = sceKernelAllocateDirectMemory(0, SCE_KERNEL_MAIN_DMEM_SIZE, size, m_pageAlignment, m_sceMemoryType, &physMemory);
        MEMMNG_ASSERT(ret == SCE_OK);
        if (ret!=SCE_OK)
            return nullptr;

        void * pageStart = _pageBegin;

        if (m_virtualMemReserved)
        {
            ret = sceKernelMapDirectMemory( &pageStart, size, _sceProt.sceInt, SCE_KERNEL_MAP_FIXED, physMemory, m_pageAlignment);
            MEMMNG_ASSERT(pageStart == _pageBegin);
        }
        else
        {
            ret = sceKernelMapDirectMemory( &pageStart, size, _sceProt.sceInt, 0, physMemory, m_pageAlignment);
            if ((u8*)pageStart<m_pAreaBegin || (u8*)pageStart>+m_pAreaEnd)
            {
                ret = SCE_KERNEL_ERROR_ENOMEM; // release the memory and return nullptr, as in any other failure case...
            }
        }

        if (ret != SCE_OK)
        {
            sceKernelReleaseDirectMemory(physMemory, size);
            return nullptr;
        }
        return (u8*)pageStart;
    }

    // Release all the physical memory pages assigned to [_pageBegin, _pageBegin +_size-1]
    // If the virtual space is reserved, the decommit virtual space is kept reserved.
    bbool MemoryPageManager::DecommitPages(u8 * _pageBegin, uPtr _size)
    {
        checkCommitParameters(_pageBegin, _size);
        uPtr size = BitTweak::GetBelowAlign(_size, m_pageAlignment);

        // loop on every block of physical
        u8 * currentPageAddress = _pageBegin;
        uPtr sizeLeft = size;

        // get the physical memory offset
        SceKernelVirtualQueryInfo info;
        int ret = sceKernelVirtualQuery((void *)currentPageAddress, 0, &info, sizeof(info));
        MEMMNG_ASSERT(ret==SCE_OK);
        MEMMNG_ASSERT(info.isDirectMemory!=0);
        if (ret!=SCE_OK || !info.isDirectMemory)
            return bfalse;

        // compute adjustement of the physical area to free
        uPtr physSize = uPtr(info.end) - uPtr(info.start);
        MEMMNG_ASSERT((u8*)info.start+physSize <= m_pAreaEnd);

        uPtr physOffset = _pageBegin - (u8*)info.start;
        uPtr physStart = info.offset + physOffset; 
        uPtr physSizeToFree = physSize - physOffset; 
        physSizeToFree = std::min(physSizeToFree, sizeLeft);
        MEMMNG_ASSERT(physSizeToFree>0);

        // We use sceKernelBatchMap to be more efficient.
        PhysicalFreeHelper<100> physFreeHelper(m_virtualMemReserved?_pageBegin:nullptr);
        if (!physFreeHelper.add(physStart, physSizeToFree))
            return bfalse;

        currentPageAddress += physSizeToFree;
        sizeLeft -= physSizeToFree;

        while(sizeLeft!=0)
        {
            ret = sceKernelVirtualQuery((void *)currentPageAddress, 0, &info, sizeof(info));
            MEMMNG_ASSERT(ret == SCE_OK);
            if (ret != SCE_OK || !info.isDirectMemory)
                return bfalse; // we cannot leave in a coherent state...
            physStart = info.offset;
            physSize = uPtr(info.end) - uPtr(info.start);
            physSizeToFree = std::min(physSize, sizeLeft);
            if (!physFreeHelper.add(physStart, physSizeToFree))
                return bfalse;
            currentPageAddress += physSizeToFree;
            MEMMNG_ASSERT(sizeLeft>=physSizeToFree);
            sizeLeft -= physSizeToFree;
        }

        return physFreeHelper.flush();
    }

    bbool MemoryPageManager::setMemoryProtection(u8 * _pageBegin, uPtr _size, Protection _sceProt)
    {
        checkCommitParameters(_pageBegin, _size);
        uPtr size = BitTweak::GetBelowAlign(_size, m_pageAlignment);
        int ret = sceKernelMprotect( _pageBegin, size, _sceProt.sceInt);
        return ret >= SCE_OK;
    }


    // Alloc area without giving specific addresses
    // It cannot be called if the virtual space is reserved (The address delivering being based on the virtual space reservation).
    // The physical memory assigned to the block is guaranteed to be contiguous.
    u8 * MemoryPageManager::AllocArea(uPtr _size, Protection _sceProt, uPtr _alignment)
    {
        MEMMNG_ASSERT(m_pAreaEnd > m_pAreaBegin);
        MEMMNG_ASSERT(!m_virtualMemReserved);
        _alignment = (_alignment < m_pageAlignment) ? m_pageAlignment : _alignment;
        _size = BitTweak::GetAboveAlign(_size, _alignment);
        off_t physMemory = 0;
        int ret = sceKernelAllocateDirectMemory(0, SCE_KERNEL_MAIN_DMEM_SIZE, _size, _alignment, m_sceMemoryType, &physMemory);
        MEMMNG_ASSERT(ret == SCE_OK);
        if (ret!=SCE_OK)
            return nullptr;
        void * pageStart = m_pAreaBegin;
        ret = sceKernelMapDirectMemory( &pageStart, _size, _sceProt.sceInt, 0, physMemory, _alignment);
        MEMMNG_ASSERT(ret == SCE_OK);
        if ((u8*)pageStart < m_pAreaBegin || (u8*)pageStart >= m_pAreaEnd)
        {
            ret = SCE_KERNEL_ERROR_ENOMEM;
        }
        if (ret != SCE_OK)
        {
            sceKernelReleaseDirectMemory(physMemory, _size);
            return nullptr;
        }
        return (u8*)pageStart;
    }

    bbool MemoryPageManager::FreeArea(u8 * _pageBegin, uPtr _size)
    {
        _size = BitTweak::GetAboveAlign(_size, m_pageAlignment);
        checkCommitParameters(_pageBegin, _size);

        // get the physical memory offset
        SceKernelVirtualQueryInfo info;
        int ret = sceKernelVirtualQuery(_pageBegin, 0, &info, sizeof(info));
        MEMMNG_ASSERT(_pageBegin+_size<=info.end); // Page freeing of multiple physical pages, use DecommitPages
        MEMMNG_ASSERT(ret==SCE_OK);
        MEMMNG_ASSERT(info.isDirectMemory!=0);
        if (ret!=SCE_OK || !info.isDirectMemory)
            return bfalse;

        if (m_virtualMemReserved) // we re-reserve before unmapping
        {
            void * pageReserved = _pageBegin;
            ret = sceKernelReserveVirtualRange(&pageReserved, _size, SCE_KERNEL_MAP_FIXED, m_pageAlignment);
            MEMMNG_ASSERT(pageReserved==_pageBegin);
            MEMMNG_ASSERT(ret==SCE_OK);
            if (ret!=SCE_OK)
                return bfalse;
        }

        // compute adjustment of the physical area to free
        uPtr pagePhysSize = uPtr(info.end) - uPtr(info.start);
        MEMMNG_ASSERT((u8*)info.start + pagePhysSize <= m_pAreaEnd);
        MEMMNG_ASSERT(pagePhysSize >= _size);

        uPtr physAreaOffset = _pageBegin - (u8*)info.start;
        uPtr physAreaStart = info.offset + physAreaOffset;

        ret = sceKernelReleaseDirectMemory(physAreaStart, _size); // and unmap
        MEMMNG_ASSERT(ret==SCE_OK);
        return ret==SCE_OK;
    }

} // namespace ITF

