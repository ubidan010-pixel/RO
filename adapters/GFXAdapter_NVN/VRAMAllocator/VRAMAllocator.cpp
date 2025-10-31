#include "precompiled_GFXAdapter_NVN.h"

#include "VRAMAllocator.h"
#include "PoolContainer.h"
#include "VRAMRelocHdl.h"

namespace ITF::NVN::VRAMAllocator
{
    // Use VRAMAllocator::PoolContainer::get() to get this object.
    static VRAMAllocator::PoolContainer* g_poolContainer = nullptr;

    //////////////////////////////////////////////////////////////////////////
    // allocator impl
    //////////////////////////////////////////////////////////////////////////
    void init(nvn::Device& _device, bbool _autoAllocPool, bbool _forceOOM)
    {
        // create singleton
        g_poolContainer = new (MemoryId::mId_GfxAdapter) VRAMAllocator::PoolContainer(_device, _autoAllocPool, _forceOOM);
    }

    nvn::Device * getDevice()
    {
        ITF_ASSERT(g_poolContainer != nullptr);
        if (g_poolContainer == nullptr)
        {
            return nullptr;
        }
        return &g_poolContainer->getDevice();
    }

    int getDeviceProperty(nvn::DeviceInfo _info)
    {
        int value;
        getDevice()->GetInteger(_info, &value);
        return value;
    }

    void createPool(VRAMAddress startOfBlock, ChunkSize blockSize, VRAMResourceType _resType)
    {
        VRAMAllocator::PoolContainer::getptr()->createPool(startOfBlock, blockSize, _resType);
    }

    void createPool(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType, VRAMRelocHandle & _allocHdl)
    {
        class ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->createPool(_startOfBlock, _blockSize, _resType);
        _allocHdl.allocInContainer(_blockSize, chunkContainer);
    }

    void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType)
    {
        VRAMAllocator::PoolContainer::getptr()->allocAndCreatePool(_blockSize, _resType, btrue);
    }

    void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMRelocHandle & _allocHdl)
    {
        class ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->allocAndCreatePool(_blockSize, _resType, btrue);
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

    template <VRAMResourceType resType>
    bbool canHandleAllocation(ChunkSize _blockSize, u32 _alignment)
    {
        return VRAMAllocator::PoolContainer::getptr()->canHandleAllocation<resType>(_blockSize, _alignment);
    }

    template bbool canHandleAllocation<VRAMResourceType::RENDER_TARGET>(ChunkSize _blockSize, u32 _alignment);
    template bbool canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize, u32 _alignment);
    template bbool canHandleAllocation<VRAMResourceType::TEXTURE>(ChunkSize _blockSize, u32 _alignment);
    template bbool canHandleAllocation<VRAMResourceType::SHADER>(ChunkSize _blockSize, u32 _alignment);


    template <VRAMResourceType resType>
    bbool canHandleAllocation(ChunkSize _blockSize)
    {
        return VRAMAllocator::PoolContainer::getptr()->canHandleAllocation<resType>(_blockSize);
    }

    template bbool canHandleAllocation<VRAMResourceType::RENDER_TARGET>(ChunkSize _blockSize);
    template bbool canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize);
    template bbool canHandleAllocation<VRAMResourceType::TEXTURE>(ChunkSize _blockSize);
    template bbool canHandleAllocation<VRAMResourceType::SHADER>(ChunkSize _blockSize);

} // namespace ITF::NVN::VRAMAllocator
