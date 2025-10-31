#ifndef ITF_CORE_MEMMNGPAGE_NINTENDO_H_
#define ITF_CORE_MEMMNGPAGE_NINTENDO_H_

#pragma once

#include "Core/math/BitTweak.h"
#include "MemMngPrivate.h"

#include <nn/os.h>

namespace ITF
{

    // Basic page management for memory allocation
    // This class is allocating a virtual space of InitParams.areaSize bytes at init and allow committing/decommitting of physical memory on it at provided addresses.
    // As the handling of addresses is not done here, a higher level class has to handle the addresses.
    class MemoryPageManager
    {
    public:

        enum : uPtr
        {
            DEFAULT_VIRTUAL_SPACE_SIZE = 4ul*1024ul*1024ul*1024ul, // 8GB at each page system
            VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT = 16*1024,
            VIRTUAL_SPACE_PAGE_SIZE = 16 * 1024
        };

        MemoryPageManager();
        MemoryPageManager(const MemoryPageManager&) = delete;
        MemoryPageManager & operator=(const MemoryPageManager&) = delete;
        ~MemoryPageManager();

        // Chainable param for init
        // exemple : memoryPageMgr.init(InitParams().setPageAlignment(32*1024));
        struct InitParams
        {
            InitParams() {}
            InitParams & setPageAlignment(uPtr _pageAlignment) { pageAlignment = _pageAlignment; return *this; }
            InitParams & setAreaSize(uPtr _areaSize) { areaSize = _areaSize; return *this; }
            uPtr pageAlignment = VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT;
            uPtr areaSize = DEFAULT_VIRTUAL_SPACE_SIZE;
        };
    
        void Init(const InitParams & _initParams = InitParams());
        void Close();

        uPtr GetPageSize() const { return m_pageAlignment; } 
        u8 * GetAreaBegin() const { return m_pAreaBegin; } 
        u8 * GetAreaEnd() const { return m_pAreaEnd; } 

        bbool isInRange(u8 * pBlock) const
        {
            return pBlock>=m_pAreaBegin && pBlock<m_pAreaEnd;
        }

        // Assign physical memory to a range of addresses (by page size granularity)
        // _pageBegin and size has to be aligned on the page alignment specified at init
        // It's guaranteed to return _pageBegin or nullptr (failure).
        u8* CommitPages(u8 * _pageBegin, uPtr _size);
    
        // Release all the physical memory pages assigned to [_pageBegin, _pageBegin +_size-1]
        // The decommit virtual space is kept reserved.
        bbool DecommitPages(u8 * _pageBegin, uPtr _size);

        // change the memory protection of previously committed pages
        bbool setMemoryProtection(u8 * _pageBegin, uPtr _size, nn::os::MemoryPermission _permission);

    protected:

        void checkCommitParameters(u8* _pageBegin, uPtr _size);

        u8*                 m_pAreaBegin = nullptr;
        u8*                 m_pAreaEnd = nullptr;
        uPtr                m_pageAlignment = VIRTUAL_SPACE_PAGE_MINIMAL_ALIGNMENT;
    };

    // Simple class, wrapping nn::os allocations functions for virtual and physical memory.
    class MemoryPageAreaAllocator
    {
    public:

        static constexpr uPtr MemoryPageSizeGranularity = nn::os::MemoryPageSize;

        // Alloc a virtual space and map physical memory to it.
        // _blockSize : size of the block to allocate, must be a multiple of MemoryPageSizeGranularity
        static u8* allocArea(uPtr _blockSize);

        // Free a virtual space and unmap physical memory assigned to it.
        // _address : address of the block to free, must be a multiple of MemoryPageSizeGranularity, and must be one returned by allocArea.
        // _blockSize : The same size as the one provided at allocArea. As the API do not allow it, there is no check of the size validity so be aware.
        static void freeArea(u8* _address, uPtr _blockSize);
    };

} // namespace ITF

#endif // ITF_CORE_MEMMNGPAGE_NX_H_
