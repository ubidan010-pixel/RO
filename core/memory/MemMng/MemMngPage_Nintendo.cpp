#include "precompiled_core.h"
#include "MemMngPage.h"

#include <nn/os.h>

namespace ITF
{

    static u8 * ReserveVirtualSpace(uPtr _size)
    {
        uPtr p = 0;
        ITF_VERIFY_NN_CALL(::nn::os::AllocateAddressRegion(&p, _size));
        return reinterpret_cast<u8*>(p);
    }

    static void ReleaseVirtualSpace(u8 * pageAdressBegin)
    {
        ITF_VERIFY_NN_CALL(::nn::os::FreeAddressRegion(reinterpret_cast<uPtr>(pageAdressBegin)));
    }

    // MemoryPageManager

    MemoryPageManager::MemoryPageManager()
        : m_pAreaBegin(nullptr)
        , m_pAreaEnd(nullptr)
        , m_pageAlignment(VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT)
    {
    }

    MemoryPageManager::~MemoryPageManager()
    {
        Close();
    }

    void MemoryPageManager::Init(const InitParams & _initParams)
    {
        m_pageAlignment = BitTweak::GetCeilPow2(u32(_initParams.pageAlignment));
        MEMMNG_ASSERT(m_pageAlignment == _initParams.pageAlignment); // not a pow2 alignment !
        m_pageAlignment = (m_pageAlignment<VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT)?VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT:m_pageAlignment;

        uPtr areaSize = BitTweak::GetAboveAlign(_initParams.areaSize, m_pageAlignment);

        m_pAreaBegin = ReserveVirtualSpace(areaSize);
        m_pAreaEnd = m_pAreaBegin + areaSize;
    }

    void MemoryPageManager::Close()
    {
        ReleaseVirtualSpace(m_pAreaBegin);
    }

    void MemoryPageManager::checkCommitParameters(u8 * _pageBegin, uPtr _size)
    {
        MEMMNG_ASSERT(m_pAreaEnd > m_pAreaBegin); // not init ?
        MEMMNG_ASSERT(_pageBegin >= m_pAreaBegin && _pageBegin < m_pAreaEnd); // not in range ?
        MEMMNG_ASSERT(_pageBegin+_size > m_pAreaBegin && _pageBegin+_size <= m_pAreaEnd); // not in range ?
        MEMMNG_ASSERT(_size == BitTweak::GetAboveAlign(_size, m_pageAlignment)); // probably a caller error
        MEMMNG_ASSERT(_size != 0); // probably a caller error
        MEMMNG_ASSERT(_size < (m_pAreaEnd - m_pAreaBegin)); // probably a caller error
    }

    // Assign physical memory to a range of addresses (by page size granularity)
    // It's guaranteed to return _pageBegin or nullptr (failure).
    u8* MemoryPageManager::CommitPages(u8 * _pageBegin, uPtr _size)
    {
        checkCommitParameters(_pageBegin, _size);

        uPtr size = BitTweak::GetAboveAlign(_size, m_pageAlignment);

        auto result = nn::os::AllocateMemoryPages(reinterpret_cast<uintptr_t>(_pageBegin), size);

        ITF_ASSERT_NN_RESULT_NAMED(result, "AllocateMemoryPages");

        if (result.IsFailure())
        {
            return nullptr;
        }

        return _pageBegin;
    }

    // Release all the physical memory pages assigned to [_pageBegin, _pageBegin +_size-1]
    bbool MemoryPageManager::DecommitPages(u8 * _pageBegin, uPtr _size)
    {
        checkCommitParameters(_pageBegin, _size);

        uPtr size = BitTweak::GetBelowAlign(_size, m_pageAlignment);
        auto result = nn::os::FreeMemoryPages(reinterpret_cast<uintptr_t>(_pageBegin), size);
        ITF_ASSERT_NN_RESULT_NAMED(result, "FreeMemoryPages");

        return result.IsSuccess();
    }

    bbool MemoryPageManager::setMemoryProtection(u8 * _pageBegin, uPtr _size, nn::os::MemoryPermission _permission)
    {
        checkCommitParameters(_pageBegin, _size);
        nn::os::SetMemoryPermission(reinterpret_cast<uintptr_t>(_pageBegin), _size, _permission);
        return btrue;
    }

    //////////////////////////////////////////////////////////////////////////
    //  Page Area Allocator
    //////////////////////////////////////////////////////////////////////////

    u8* MemoryPageAreaAllocator::allocArea(uPtr _blockSize)
    {
        MEMMNG_ASSERT(_blockSize >= MemoryPageSizeGranularity);
        MEMMNG_ASSERT((_blockSize % MemoryPageSizeGranularity) == 0);

        uPtr address = 0;
        auto result = nn::os::AllocateAddressRegion(&address, _blockSize);
        ITF_ASSERT_NN_RESULT_NAMED(result, "AllocateAddressRegion");

        if (result.IsFailure() || address == 0)
        {
            return nullptr;
        }

        result = nn::os::AllocateMemoryPages(address, _blockSize);
        ITF_ASSERT_NN_RESULT_NAMED(result, "AllocateMemoryPages");

        if (result.IsFailure())
        {
            return nullptr;
        }

        return reinterpret_cast<u8*>(address);
    }

    void MemoryPageAreaAllocator::freeArea(u8* _address, uPtr _blockSize)
    {
        MEMMNG_ASSERT(_blockSize >= MemoryPageSizeGranularity);
        MEMMNG_ASSERT((_blockSize % MemoryPageSizeGranularity) == 0);

        uPtr addressAsUPtr = reinterpret_cast<uPtr>(_address);
        MEMMNG_ASSERT(addressAsUPtr >= MemoryPageSizeGranularity);
        MEMMNG_ASSERT((addressAsUPtr % MemoryPageSizeGranularity) == 0);

        ITF_VERIFY_NN_CALL(nn::os::FreeMemoryPages(addressAsUPtr, _blockSize));
        ITF_VERIFY_NN_CALL(nn::os::FreeAddressRegion(addressAsUPtr));
    }

} // namespace ITF
