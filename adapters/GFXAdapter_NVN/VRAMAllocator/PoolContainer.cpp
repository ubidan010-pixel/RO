#include "precompiled_GFXAdapter_NVN.h"

#include <algorithm>
#include "core/math/MathTools.h"
#include "core/math/BitTweak.h"
#include "core/utility.h"
#include "core/memory/MemMng/MemMngPage_Nintendo.h"
#include "VRAMMemoryTypes.h"
#include "ChunkContainer.h"
#include "PoolContainer.h"
#include "NVN/Helpers.h"

namespace ITF::NVN::VRAMAllocator
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

    PoolContainer::PoolContainer(nvn::Device& _device, bbool _autoAllocPool, bbool _forceOOM)
        : m_device(&_device)
        , m_autoAllocPool(_autoAllocPool)
        , m_forceOOMBreak(_forceOOM)
    {
    }

    PoolContainer::~PoolContainer()
    {
        // not implemented, we may need to free every pool ?
    }

    nvn::MemoryPoolFlags PoolContainer::toPoolFlag(VRAMResourceType _resType)
    {
        // GPU cached for everyone. Only the command buffer should not be, but its allocated in another way.
        nvn::MemoryPoolFlags memPoolFlags{ nvn::MemoryPoolFlags::GPU_CACHED };
        if (_resType == VRAMResourceType::RENDER_TARGET)
        {
            memPoolFlags |= nvn::MemoryPoolFlags::CPU_NO_ACCESS; // We assume to never touch to Render targets in CPU
            memPoolFlags |= nvn::MemoryPoolFlags::COMPRESSIBLE;
        }
        else
        {
        #ifdef ITF_NX
            // Not CPU cached: Filling a VB or Texture in CPU will be efficient anyway as soon as we fill it linearly, and don't need to read it back.
            memPoolFlags |= nvn::MemoryPoolFlags::CPU_UNCACHED;
        #else
            // On Ounce, we get coherency guarantied, so use CPU cached is recommended
            memPoolFlags |= nvn::MemoryPoolFlags::CPU_CACHED;
        #endif
            if (_resType == VRAMResourceType::SHADER)
                memPoolFlags |= nvn::MemoryPoolFlags::SHADER_CODE;
        }
        return memPoolFlags;
    }

    nvn::MemoryPoolBuilder PoolContainer::createMemPoolBuilder(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType)
    {
        nvn::MemoryPoolBuilder builder;
        builder.SetDefaults()
            .SetDevice(m_device)
            .SetFlags(toPoolFlag(_resType))
            .SetStorage(reinterpret_cast<void*>(_startOfBlock), size_t(_blockSize));
        return builder;
    }

    class ChunkContainer * PoolContainer::createPool(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType)
    {
        ScopeLockMutex lock(m_mutex);
        ChunkContainer * container = newAlloc(mId_VRAMPool, ChunkContainer());
        m_pools[toUnderlying(_resType)].push_back(container);

        container->Init(createMemPoolBuilder(_startOfBlock, _blockSize, _resType), m_forceOOMBreak, /* verboseOutOfMem = */ false, /* staticBuffer = */ true);
        return container;
    }

    // allocate main memory and create a ChunkContainer to handle at least the given size
    // if staticBuffer, the pool will never be deleted
    ChunkContainer * PoolContainer::allocAndCreatePool(ChunkSize _blockSize, VRAMResourceType _resType, bbool _staticBuffer)
    {
        ScopeLockMutex lock(m_mutex);

        _blockSize = BitTweak::GetAboveAlign(_blockSize, ChunkSize(MemoryPageAreaAllocator::MemoryPageSizeGranularity));
        VRAMAddress block = MemoryPageAreaAllocator::allocArea(_blockSize);

        if (!block)
        {
            VRAM_ALLOC_TRACE("*******************************************************************************************\n");
            VRAM_ALLOC_TRACE("Out of RAM when creating a pool in the VRAM pool container : requested size %d bytes\n", _blockSize);
            VRAM_ALLOC_TRACE("*******************************************************************************************\n");
            return nullptr;
        }
        ChunkContainer * container =  newAlloc(mId_VRAMPool,ChunkContainer());
        m_pools[toUnderlying(_resType)].push_back(container);
        container->Init(createMemPoolBuilder(block, _blockSize, _resType), false, _staticBuffer);
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
            case VRAMResourceType::RENDER_TARGET: return "RenderTarget";
            case VRAMResourceType::RESOURCE: return "Resource";
            case VRAMResourceType::TEXTURE: return "Texture";
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
                    VRAMAddress buffer = chunkContainer->GetStartAddress();
                    ChunkSize size = chunkContainer->GetContainerSize();
                    MemoryPageAreaAllocator::freeArea(buffer, size);
                    delete chunkContainer;
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
    template void PoolContainer::alloc<VRAMResourceType::RENDER_TARGET>(ChunkSize _size, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::alloc<VRAMResourceType::RESOURCE>(ChunkSize _size, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::alloc<VRAMResourceType::TEXTURE>(ChunkSize _size, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx& _resIdx);
    template void PoolContainer::alloc<VRAMResourceType::SHADER>(ChunkSize _size, VramMemoryCategory _category, ChunkContainer*& _resContainer, ChunkIdx& _resIdx);

    template void PoolContainer::allocAligned<VRAMResourceType::RENDER_TARGET>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::allocAligned<VRAMResourceType::RESOURCE>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::allocAligned<VRAMResourceType::TEXTURE>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, ChunkContainer * & _resContainer, ChunkIdx & _resIdx);
    template void PoolContainer::allocAligned<VRAMResourceType::SHADER>(ChunkSize _size, ChunkSize _align, VramMemoryCategory _category, ChunkContainer*& _resContainer, ChunkIdx& _resIdx);

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

    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RENDER_TARGET>(ChunkSize _blockSize);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::TEXTURE>(ChunkSize _blockSize);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::SHADER>(ChunkSize _blockSize);

    template <VRAMResourceType resType>
    bbool PoolContainer::canHandleAllocation(ChunkSize _blockSize)
    {
        return canHandleAllocation<resType>(_blockSize, MinimalAlignment);
    }

    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RENDER_TARGET>(ChunkSize _blockSize, ChunkSize _alignment);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize, ChunkSize _alignment);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::TEXTURE>(ChunkSize _blockSize, ChunkSize _alignment);
    template bbool PoolContainer::canHandleAllocation<VRAMResourceType::SHADER>(ChunkSize _blockSize, ChunkSize _alignment);

} // namespace ITF::NVN::VRAMAllocator
