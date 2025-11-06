#include "precompiled_GFXAdapter_DX12.h"

#include <algorithm>
#include "core/math/MathTools.h"
#include "core/math/BitTweak.h"
#include "core/utility.h"
#include "VRAMMemoryTypes.h"
#include "ChunkContainer.h"
#include "PoolContainer.h"
#include "DX12/Helpers.h"

namespace ITF::DX12::VRAMAllocator
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
        for(PoolVector & resPools : m_pools)
        {
            for(ChunkContainer * container : resPools)
            {
                _fct(container);
            }
        }
    }

    PoolContainer::PoolContainer(ComPtr<ID3D12Device> _device, bbool _autoAllocPool, bbool _forceOOM)
        : m_device(std::move(_device))
        , m_autoAllocPool(_autoAllocPool)
        , m_forceOOMBreak(_forceOOM)
    {
    }

    PoolContainer::~PoolContainer()
    {
        // not implemented, we may need to free every pool ?
    }

    class ChunkContainer * PoolContainer::createPool(ComPtr<ID3D12Heap> _heap, VRAMResourceType _resType)
    {
        ChunkContainer* container = newAlloc(mId_VRAMPool, ChunkContainer());

        ScopeLockMutex lock(m_mutex);
        m_pools[toUnderlying(_resType)].push_back(container);
        container->Init(_heap, m_forceOOMBreak, /* verboseOutOfMem = */ false, /* staticBuffer = */ true);
        return container;
    }

    // allocate main memory and create a ChunkContainer to handle at least the given size
    // if staticBuffer, the pool will never be deleted
    ChunkContainer * PoolContainer::allocAndCreatePool(ChunkSize _blockSize, VRAMResourceType _resType, bbool _staticBuffer)
    {
        _blockSize = BitTweak::GetAboveAlign(_blockSize, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
        D3D12_HEAP_DESC heapDesc = {};
        heapDesc.SizeInBytes = _blockSize;
        heapDesc.Properties.Type = _resType == VRAMResourceType::RESOURCE_UPLOAD ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
        heapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
        heapDesc.Flags = D3D12_HEAP_FLAG_NONE;

        ComPtr<ID3D12Heap> heap;
        m_device->CreateHeap(&heapDesc, DX12_IID_COMPTR_ARGS(heap));

        if (!heap)
        {
            VRAM_ALLOC_TRACE("*******************************************************************************************\n");
            VRAM_ALLOC_TRACE("Out of RAM when creating a pool in the VRAM pool container : requested size %d bytes\n", _blockSize);
            VRAM_ALLOC_TRACE("*******************************************************************************************\n");
            return nullptr;
        }
        ChunkContainer* container = newAlloc(mId_VRAMPool, ChunkContainer());

        ScopeLockMutex lock(m_mutex);
        m_pools[toUnderlying(_resType)].push_back(container);
        container->Init(std::move(heap), /* verboseOutOfMem = */ false, _staticBuffer);
        return container;
    }

    // Let a chance to do a bit for defragmentation
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
        auto poolTypeNames = [](VRAMResourceType resType) -> const char*
        {
            switch (resType)
            {
            case VRAMResourceType::RESOURCE: return "Resource";
            case VRAMResourceType::RESOURCE_UPLOAD: return "ResourceUpload";
            default: return "Unknown";
            }
        };

        ScopeLockMutex lock(m_mutex);

        ux count = 0;
        for (PoolVector& resPools : m_pools)
        {
            auto idx = distInRange(m_pools, resPools);
            VRAMResourceType resType = static_cast<VRAMResourceType>(idx);
            for (ChunkContainer*& container : resPools)
            {
                VRAM_ALLOC_TRACE("[PoolContainer] Pool %d of type %s \n", count, poolTypeNames(resType));
                container->PrintInfo();
                count++;
            }
        }
    #endif // VRAM_ALLOC_TRACE_ENABLED
    }

    void PoolContainer::getStats(VRAMStats & stats)
    {
        ScopeLockMutex lock(m_mutex);

        stats = VRAMStats{};

        forEachContainer<>
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

        for(PoolVector & resPools : m_pools)
        {
            for(ChunkContainer * container : resPools)
            {
                if (container->IsOwner(link))
                {
                    return container;
                }
            }
        }
        return nullptr;
    }
        
    // Force a break when OOM
    void PoolContainer::setForceOOMBreak(bbool _on)
    {
        ScopeLockMutex lock(m_mutex);

        m_forceOOMBreak = _on;

        for(PoolVector & pools: m_pools)
        {
            for(ChunkContainer * container : pools)
            {
                container->SetForceOOMBreak(m_forceOOMBreak);
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

        for(PoolVector & pools : m_pools)
        {
            for(auto it = pools.begin(); it!=pools.end(); ++it)
            {
                if ((*it)==chunkContainer)
                {
                    pools.erase(it);
                    delete chunkContainer; // should release the heap through the refcount system
                    return;
                }
            }
        }
    }

    // find the best ChunkContainer for a size, return NULL if no container found
    template <VRAMResourceType resType>
    void PoolContainer::alloc(ChunkSize _size, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx)
    {
        ScopeLockMutex lock(m_mutex);

        for(ChunkContainer * container : m_pools[toUnderlying(resType)])
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
            ChunkContainer * container = allocAndCreatePool(Max(m_memoryPoolGranularity, _size), resType);
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

    template <VRAMResourceType resType>
    void PoolContainer::allocAligned(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, class ChunkContainer * & _resContainer, ChunkIdx & _resIdx)
    {
        ScopeLockMutex lock(m_mutex);

        for(ChunkContainer * container : m_pools[toUnderlying(resType)])
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
            ChunkContainer * container = allocAndCreatePool(Max(m_memoryPoolGranularity, _size), resType);
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
    template void PoolContainer::alloc<VRAMResourceType::RESOURCE>(ChunkSize _size, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::alloc<VRAMResourceType::RESOURCE_UPLOAD>(ChunkSize _size, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);

    template void PoolContainer::allocAligned<VRAMResourceType::RESOURCE>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::allocAligned<VRAMResourceType::RESOURCE_UPLOAD>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);

    template <VRAMResourceType resType>
    bbool PoolContainer::canHandleAllocation(ChunkSize _blockSize, ChunkSize _alignment)
    {
        ScopeLockMutex lock(m_mutex);

        for(ChunkContainer * container : m_pools[toUnderlying(resType)])
        {
            if (container->CanHandleAllocation(_blockSize, _alignment))
            {
                return btrue;
            }
        }
        return bfalse;
    }

    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RESOURCE_UPLOAD>(ChunkSize _blockSize);

    template <VRAMResourceType resType>
    bbool PoolContainer::canHandleAllocation(ChunkSize _blockSize)
    {
        return canHandleAllocation<resType>(_blockSize, MinimalAlignment);
    }

    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize, ChunkSize _alignment);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RESOURCE_UPLOAD>(ChunkSize _blockSize, ChunkSize _alignment);

} // namespace ITF::DX12::VRAMAllocator
