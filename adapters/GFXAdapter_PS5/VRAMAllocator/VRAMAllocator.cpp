#include "precompiled_GFXAdapter_PS5.h"

#include "VRAMAllocator.h"
#include "PoolContainer.h"
#include "VRAMRelocHdl.h"

namespace ITF::Gnmp::VRAMAllocator
{
    // Use VRAMAllocator::PoolContainer::get() to get this object.
    static VRAMAllocator::PoolContainer* g_poolContainer = nullptr;

    //////////////////////////////////////////////////////////////////////////
    // allocator impl
    //////////////////////////////////////////////////////////////////////////
    void init(MemoryPageManager& _pageMgr, bbool _autoAllocPool, bbool _forceOOM)
    {
        // create singleton
        g_poolContainer = newAlloc(mId_GfxAdapter, VRAMAllocator::PoolContainer(_pageMgr, _autoAllocPool, _forceOOM));
    }

    void createPool(VRAMAddress startOfBlock, ChunkSize blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus)
    {
        VRAMAllocator::PoolContainer::getptr()->createPool(startOfBlock, blockSize, _resType, _memBus);
    }

    void createPool(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus, VRAMRelocHandle & _allocHdl)
    {
        class ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->createPool(_startOfBlock, _blockSize, _resType, _memBus);
        _allocHdl.allocInContainer(_blockSize, chunkContainer);
    }

    void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus)
    {
        VRAMAllocator::PoolContainer::getptr()->allocAndCreatePool(_blockSize, _resType, _memBus, btrue);
    }

    void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus, VRAMRelocHandle & _allocHdl)
    {
        class ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->allocAndCreatePool(_blockSize, _resType, _memBus, btrue);
        _allocHdl.allocInContainer(_blockSize, chunkContainer);
    }

    void releaseUnusedDynamicPools()
    {
        VRAMAllocator::PoolContainer::getptr()->releaseUnusedDynamicPools();
    }

    // do a bit for defragmentation
    void update(u64 cpuCurrentFrame, u64 gpuLastFrameCount, bbool bForceFlush)
    {
        VRAMAllocator::PoolContainer::getptr()->update(cpuCurrentFrame, gpuLastFrameCount, bForceFlush);
    }

    void printInfo()
    {
        VRAMAllocator::PoolContainer::getptr()->printInfo();
    }

    void getStats(VRAMStats & stats)
    {
        VRAMAllocator::PoolContainer::getptr()->getStats(stats);
    }

    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    bbool canHandleAllocation(ChunkSize _blockSize, u32 _alignment)
    {
        return VRAMAllocator::PoolContainer::getptr()->canHandleAllocation<resType, memBus>(_blockSize, _alignment);
    }

    template bbool canHandleAllocation<READ_WRITE, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize, u32 _alignment);
    template bbool canHandleAllocation<READ_ONLY, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize, u32 _alignment);


    template <VRAMResourceType resType, VRAMMemoryBus memBus>
    bbool canHandleAllocation(ChunkSize _blockSize)
    {
        return VRAMAllocator::PoolContainer::getptr()->canHandleAllocation<resType, memBus>(_blockSize);
    }

    template bbool canHandleAllocation<READ_WRITE, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize);
    template bbool canHandleAllocation<READ_ONLY, VRAMMemoryBus::DIRECT>(ChunkSize _blockSize);

} // namespace ITF::Gnmp::VRAMAllocator
