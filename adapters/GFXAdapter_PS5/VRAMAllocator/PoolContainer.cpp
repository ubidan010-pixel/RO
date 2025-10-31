#include "precompiled_GFXAdapter_PS5.h"

#include <algorithm>
#include "core/math/MathTools.h"
#include "core/math/BitTweak.h"
#include "core/utility.h"
#include "core/memory/MemMng/MemMngPage_PS5.h"
#include "VRAMMemoryTypes.h"
#include "ChunkContainer.h"
#include "PoolContainer.h"

namespace ITF::Gnmp::VRAMAllocator
{
#ifdef VRAM_PRINT_INFO
    // set to 1 in the debugger to output VRAM info
    int g_printVramInfo = 0;
#endif

    //////////////////////////////////////////////////////////////////////////
    //  Pool container
    //////////////////////////////////////////////////////////////////////////

    template <typename Fct>
    void PoolContainer::forEachContainer(Fct _fct)
    {
        for(auto & memBusPools : m_pools)
        {
            for(PoolVector & resPools : memBusPools)
            {
                for(ChunkContainer * container : resPools)
                {
                    _fct(container);
                }
            }
        }
    }

    template <VRAMMemoryBus memBus, typename Fct>
    void PoolContainer::forEachContainer(Fct _fct)
    {
        for(PoolVector & resPools : m_pools[ux(memBus)])
        {
            for(ChunkContainer * container : resPools)
            {
                _fct(container);
            }
        }
    }

    PoolContainer::PoolContainer(MemoryPageManager & _pageMgr, bbool _autoAllocPool, bbool _forceOOM)
        :   m_pageMgr(_pageMgr)
        ,   m_autoAllocPool(_autoAllocPool)
    {
        std::fill(std::begin(m_forceOOMBreak), std::end(m_forceOOMBreak), _forceOOM);
    }

    PoolContainer::~PoolContainer()
    {
        // not implemented, we may need to free every pool ?
    }

    class ChunkContainer * PoolContainer::createPool(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus)
    {
        ScopeLockMutex lock(m_mutex);
        ChunkContainer * container = newAlloc(mId_VRAMPool, ChunkContainer());
        m_pools[ux(_memBus)][_resType].push_back(container);
        container->Init(_startOfBlock, _blockSize, _memBus, m_forceOOMBreak[ux(_memBus)], false, true);
        return container;
    }

    // allocate main memory and create a ChunkContainer to handle at least the given size
    // if staticBuffer, the pool will never be deleted
    ChunkContainer * PoolContainer::allocAndCreatePool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus, bbool _staticBuffer)
    {
        ScopeLockMutex lock(m_mutex);

        MemoryPageManager* memPageMgr = &m_pageMgr;

        MemoryPageManager::Protection sceProt = MemoryPageManager::Protection::cpuRW() + MemoryPageManager::Protection::gpuR();
        if (_resType != READ_ONLY)
            sceProt += MemoryPageManager::Protection::gpuW();

        _blockSize = BitTweak::GetAboveAlign(_blockSize, ChunkSize(memPageMgr->GetPageSize()));
        VRAMAddress block = memPageMgr->AllocArea(_blockSize, sceProt, Min(1*1024*1024u, BitTweak::GetCeilPow2(m_memoryPoolGranularity))); // align on pool granularity, and at max 1Meg

        if (!block)
        {
            VRAM_ALLOC_TRACE("*******************************************************************************************\n");
            VRAM_ALLOC_TRACE("Out of RAM when creating a pool in the VRAM pool container : requested size %d bytes\n", _blockSize);
            VRAM_ALLOC_TRACE("*******************************************************************************************\n");
            return nullptr;
        }
        ChunkContainer * container =  newAlloc(mId_VRAMPool,ChunkContainer());
        m_pools[ux(_memBus)][_resType].push_back(container);
        container->Init(block, _blockSize, _memBus, false, _staticBuffer);
        return container;
    }

    // do a bit for defragmentation
    void PoolContainer::update(u64 cpuCurrentFrame, u64 gpuLastFrameCount, bbool bForceErase)
    {
        ScopeLockMutex lock(m_mutex);
        #ifdef VRAM_PRINT_INFO
        if (g_printVramInfo!=0)
        {
            printInfo();
            g_printVramInfo = 0;
        }
        #endif

        forEachContainer(
            [=] (ChunkContainer * container)
            {
                container->Update(cpuCurrentFrame, gpuLastFrameCount, bForceErase);
            }
            );
    }

    void PoolContainer::printInfo()
    {
    #ifdef VRAM_ALLOC_TRACE_ENABLED
        ScopeLockMutex lock(m_mutex);

        int count = 0;

        for (auto& memBusPools : m_pools)
        {
            for (ChunkContainer* container : memBusPools[READ_ONLY])
            {
                VRAM_ALLOC_TRACE("$$$$$ BusType %zu - RO Pool %d $$$$$\n", distInRange(m_pools, memBusPools), count);
                count++;
                container->PrintInfo();
            }
        }

        for (auto& memBusPools : m_pools)
        {
            for (ChunkContainer* container : memBusPools[READ_WRITE])
            {
                VRAM_ALLOC_TRACE("$$$$$ BusType %zu - RW Pool %d $$$$$\n", distInRange(m_pools, memBusPools), count);
                count++;
                container->PrintInfo();
            }
        }
    #endif // VRAM_ALLOC_TRACE_ENABLED
    }

    void PoolContainer::getStats(VRAMStats & stats)
    {
        ScopeLockMutex lock(m_mutex);

        stats = VRAMStats{};

        forEachContainer<VRAMMemoryBus::DIRECT>
        (
            [&stats] (ChunkContainer * container)
            {
                u32 freeSize = 0;
                u32 freeBiggestSize = 0;
                u32 initialSize = 0;
                container->GetStats(freeSize, freeBiggestSize, initialSize);
                stats.directBiggestBlockSize = Max(freeBiggestSize, stats.directBiggestBlockSize);
                stats.directFreeSize += freeSize;
                stats.directTotalSize += initialSize;
            }
        );
    }

    // find the ChunkContainer where the VRAMHandleLinks is pointing
    ChunkContainer * PoolContainer::findPoolFromLink(const VRAMHandleLinks & link)
    {
        ScopeLockMutex lock(m_mutex);

        for(auto & memBusPools : m_pools)
        {
            for(PoolVector & resPools : memBusPools)
            {
                for(ChunkContainer * container : resPools)
                {
                    if (container->IsOwner(link))
                    {
                        return container;
                    }
                }
            }
        }
        return nullptr;
    }
        
    // Force a break when OOM
    void PoolContainer::setForceOOMBreak(bbool _on)
    {
        ScopeLockMutex lock(m_mutex);

        std::fill(std::begin(m_forceOOMBreak), std::end(m_forceOOMBreak), _on);

        for(ux memBus = 0; memBus<ux(VRAMMemoryBus::Count); ++memBus)
        {
            for(PoolVector & pools: m_pools[memBus])
            {
                for(ChunkContainer * container : pools)
                {
                    container->SetForceOOMBreak(m_forceOOMBreak[memBus]);
                }
            }
        }
    }

    void PoolContainer::setMemoryPoolGranularity(ChunkSize _granularity)
    {
        m_memoryPoolGranularity = _granularity;
    }

    void PoolContainer::releaseUnusedDynamicPools()
    {
        ScopeLockMutex lock(m_mutex);

        Vector<ChunkContainer *> chunkContainerToRelease;

        forEachContainer
        (
            [&chunkContainerToRelease] (ChunkContainer * chunkContainer)
            {
                if (chunkContainer->IsEmpty()
                    && !chunkContainer->IsStatic())
                {
                    chunkContainerToRelease.push_back(chunkContainer);
                }
            }
        );

        for(auto chunkContainer : chunkContainerToRelease)
        {
            eraseContainer(chunkContainer);
        }
    }

    // destroy an empty container
    void PoolContainer::eraseContainer(ChunkContainer * chunkContainer)
    {
        ScopeLockMutex lock(m_mutex);

        VRAM_ALLOC_ASSERT(chunkContainer);
        VRAM_ALLOC_ASSERT(chunkContainer->IsEmpty()); // hmm, that's not correct to delete a non empty container as RelocHdl can still be pointing on it

        for(ux memBus = 0; memBus<ux(VRAMMemoryBus::Count); ++memBus)
        {
            for(PoolVector & pools : m_pools[memBus])
            {
                for(auto it = pools.begin(); it!=pools.end(); ++it)
                {
                    if ((*it)==chunkContainer)
                    {
                        pools.erase(it);
                        VRAMAddress buffer = chunkContainer->GetStartAddress();
                        ChunkSize size = chunkContainer->GetContainerSize();
                        MemoryPageManager * memPageMgr = &m_pageMgr;
                        ITF_VERIFY(memPageMgr->FreeArea(buffer, size));
                        delete chunkContainer;
                        return;
                    }   
                }
            }
        }
    }

    // find the best ChunkContainer for a size, return NULL if no container found
    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    void PoolContainer::alloc(ChunkSize _size, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx)
    {
        ScopeLockMutex lock(m_mutex);

        for(ChunkContainer * container : m_pools[ux(memBus)][resType])
        {
            auto idx = container->Alloc(_size, _category);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                _resIdx = idx;
                _resContainer = container;
                return;
            }
        }
        // failure, create a new pool
        if (m_autoAllocPool)
        {
            ChunkContainer * container = allocAndCreatePool(Max(m_memoryPoolGranularity, _size), resType, memBus);
            if (container)
            {
                _resIdx = container->Alloc(_size, _category);
                _resContainer = container;
                ITF_ASSERT(_resIdx != ChunkContainer::InvalidIndex);
                return;
            }
        }
        _resContainer = nullptr;
        _resIdx = ChunkContainer::InvalidIndex;
    }

    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    void PoolContainer::allocAligned(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx)
    {
        ScopeLockMutex lock(m_mutex);

        for(ChunkContainer * container : m_pools[ux(memBus)][resType])
        {
            auto idx = container->AllocAligned(_size, _align, _category);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                _resIdx = idx;
                _resContainer = container;
                return;
            }
        }
        // failure, create a new pool
        if (m_autoAllocPool)
        {
            ITF_ASSERT(m_memoryPoolGranularity > _align);
            ChunkContainer * container = allocAndCreatePool(Max(m_memoryPoolGranularity, _size), resType, memBus);
            if (container)
            {
                _resIdx = container->AllocAligned(_size, _align, _category);
                _resContainer = container;
                ITF_ASSERT(_resIdx != ChunkContainer::InvalidIndex);
                return;
            }
        }
        _resContainer = nullptr;
        _resIdx = ChunkContainer::InvalidIndex;
    }

    // explicit instantiations
    template void PoolContainer::alloc<READ_WRITE, VRAMMemoryBus::DIRECT>(ChunkSize _size, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::alloc<READ_ONLY, VRAMMemoryBus::DIRECT>(ChunkSize _size, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx);

    template void PoolContainer::allocAligned<READ_WRITE, VRAMMemoryBus::DIRECT>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::allocAligned<READ_ONLY, VRAMMemoryBus::DIRECT>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx);

    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    bbool PoolContainer::canHandleAllocation(ChunkSize _blockSize, ChunkSize _alignment)
    {
        ScopeLockMutex lock(m_mutex);

        for(ChunkContainer * container : m_pools[ux(memBus)][resType])
        {
            if (container->CanHandleAllocation(_blockSize, _alignment))
            {
                return btrue;
            }
        }
        return bfalse;
    }

    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    bbool PoolContainer::canHandleAllocation(ChunkSize _blockSize)
    {
        return canHandleAllocation<resType, memBus>(_blockSize, MinimalAlignment);
    }

    template bbool PoolContainer::canHandleAllocation<READ_WRITE, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize);
    template bbool PoolContainer::canHandleAllocation<READ_ONLY, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize);

    template bbool PoolContainer::canHandleAllocation<READ_WRITE, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize, ChunkSize _alignment);
    template bbool PoolContainer::canHandleAllocation<READ_ONLY, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize, ChunkSize _alignment);

} // namespace ITF::Gnmp::VRAMAllocator
