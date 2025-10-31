#ifndef ITF_CORE_MEMMNGPAGE_PS5_H_
#define ITF_CORE_MEMMNGPAGE_PS5_H_

#pragma once

#include "Core/math/BitTweak.h"
#include "MemMngPrivate.h"

namespace ITF
{

// basic page management for direct memory
class MemoryPageManager
{
public:

    enum : uPtr
    {
        DEFAULT_VIRTUAL_SPACE_SIZE = 8ul*1024ul*1024ul*1024ul, // 8GB at each page system
        VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT = 16*1024,
        VIRTUAL_SPACE_PAGE_SIZE = SCE_KERNEL_PAGE_SIZE
    };


    struct Protection
    {
        constexpr explicit Protection(int sceProt)
            : sceInt(sceProt)
        {
        }

        constexpr Protection& operator += (const Protection& _rhs)
        {
            sceInt |= _rhs.sceInt;
            return *this;
        }

        friend constexpr Protection operator + (const Protection& _lhs, const Protection& _rhs)
        {
            return Protection{ _lhs.sceInt | _rhs.sceInt };
        }

        static constexpr Protection cpuR()
        {
            return Protection{ SCE_KERNEL_PROT_CPU_READ };
        }

        static constexpr Protection cpuW()
        {
            return Protection{ SCE_KERNEL_PROT_CPU_WRITE };
        }

        static constexpr Protection cpuRW()
        {
            return cpuR() + cpuW();
        }

        static constexpr Protection gpuR()
        {
            return Protection{ SCE_KERNEL_PROT_GPU_READ };
        }

        static constexpr Protection gpuW()
        {
            return Protection{ SCE_KERNEL_PROT_GPU_WRITE };
        }

        static constexpr Protection gpuRW()
        {
            return gpuR() + gpuW();
        }

        static constexpr Protection amprR()
        {
            return Protection{ SCE_KERNEL_PROT_AMPR_READ };
        }

        static constexpr Protection amprW()
        {
            return Protection{ SCE_KERNEL_PROT_AMPR_WRITE };
        }

        static constexpr Protection amprRW()
        {
            return amprR() + amprW();
        }

        static constexpr Protection acpR()
        {
            return Protection{ SCE_KERNEL_PROT_ACP_READ };
        }

        static constexpr Protection acpW()
        {
            return Protection{ SCE_KERNEL_PROT_ACP_WRITE };
        }

        static constexpr Protection acpRW()
        {
            return acpR() + acpW();
        }

        static constexpr Protection allRW()
        {
            return cpuRW() + gpuRW() + amprRW() + acpRW();
        }


        int sceInt = SCE_KERNEL_PROT_CPU_READ | SCE_KERNEL_PROT_CPU_WRITE;
    };

    MemoryPageManager();
    ~MemoryPageManager();

    // Chainable param for init
    // exemple : memoryPageMgr.init(InitParams().setPageAlignment(32*1024));
    struct InitParams
    {
        InitParams() {}
        InitParams & setPageAlignment(uPtr _pageAlignment) { pageAlignment = _pageAlignment; return *this; }
        InitParams & setReserveVirtualSpace(bbool _reserve) { reserveVirtualSpace = _reserve; return *this; }
        InitParams & setSceMemoryType(int _memoryType) { sceMemoryType = _memoryType; return *this; }
        InitParams & setAreaSize(uPtr _areaSize) { areaSize = _areaSize; return *this; }
        uPtr pageAlignment = VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT;
        bbool reserveVirtualSpace = btrue;
        int sceMemoryType = SCE_KERNEL_MTYPE_C_SHARED;
        uPtr areaSize = DEFAULT_VIRTUAL_SPACE_SIZE;
    };
    
    // Calling init is optional but recommended.
    // By not calling it, _reserveArea is false, it's using onion and the allocated area is unknown.
    void Init(const InitParams & _initParams = InitParams());
    void Close();

    uPtr GetPageSize() const { return m_pageAlignment; } 
    u8 * GetAreaBegin() const { return m_pAreaBegin; } 
    u8 * GetAreaEnd() const { return m_pAreaEnd; } 

    bbool isInRange(u8 * pBlock) const
    {
        return pBlock>=m_pAreaBegin && pBlock<m_pAreaEnd;
    }

    // NB: There is a way to reduce oom issues by removing the constraint "The physical memory is guaranteed to be contiguous".
    // It would require an API change as some allocations, in particular some GPU ones, has to be contiguous.

    // Assign physical memory to a range of addresses (by page size granularity)
    // The physical memory is guaranteed to be contiguous.
    // _pageBegin and size has to be aligned on the page alignment specified at init
    // If the virtual space is reserved (init parameter), it's guaranteed to return _pageBegin or nullptr (failure).
    // If the virtual space is not reserved, it returns the next available following _pageBegin
    u8* CommitPages(u8 * _pageBegin, uPtr _size, Protection _sceProt = Protection::allRW());
    
    // Release all the physical memory pages assigned to [_pageBegin, _pageBegin +_size-1]
    // If the virtual space is reserved, the decommit virtual space is kept reserved.
    bbool DecommitPages(u8 * _pageBegin, uPtr _size);

    // change the memory protection of previously allocated pages
    bbool setMemoryProtection(u8 * _pageBegin, uPtr _size, Protection _sceProt);

    // Alloc area without giving specific addresses
    // It cannot be called if the virtual space is reserved (The address delivering being based on the virtual space reservation).
    // The physical memory assigned to the block is guaranteed to be contiguous.
    u8* AllocArea(uPtr _size, Protection _sceProt = Protection::allRW(), uPtr _alignment = 0);
    // Release the physical memory assigned to the given address.
    // _pageBegin has to be aligned on the page alignment specified at init
    // if _pageBegin is not the start of the physical mapping, only the area of the mapping is freed:
    // Starting at _pageBegin and ending at _pageBegin + BitTweak::GetAboveAlign(_size, VIRTUAL_SPACE_PAGE_SIZE) - 1
    // if the virtual space is reserved, the area will still be reserved (thus, this method can be used in pair with CommitPages with the same _pageBegin)
    // This method is very similar to DecommitPages except it is assuming only one physical area is assigned to the page.
    // Use only in parity with AllocArea as CommitPages could be implemented by aggregation of several physical memory areas.
    bbool FreeArea(u8* _pageBegin, uPtr _size);

protected:

    void checkCommitParameters(u8* _pageBegin, uPtr _size, Protection _sceProt = Protection{ SCE_KERNEL_PROT_CPU_WRITE } + Protection{ SCE_KERNEL_PROT_CPU_READ });

    u8*                 m_pAreaBegin;
    u8*                 m_pAreaEnd;

    uPtr                m_pageAlignment;
    int                 m_sceMemoryType;
    bbool               m_virtualMemReserved;
};

} // namespace ITF

#endif // ITF_CORE_MEMMNGPAGE_PS5_H_
