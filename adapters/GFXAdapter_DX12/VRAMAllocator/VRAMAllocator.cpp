#include "precompiled_GFXAdapter_DX12.h"

#include "VRAMAllocator.h"
#include "PoolContainer.h"
#include "VRAMRelocHdl.h"
#include <limits>

namespace ITF::DX12::VRAMAllocator
{
    // Use VRAMAllocator::PoolContainer::get() to get this object.
    static VRAMAllocator::PoolContainer* g_poolContainer = nullptr;

    //////////////////////////////////////////////////////////////////////////
    // allocator impl
    //////////////////////////////////////////////////////////////////////////
    void init(ComPtr<ID3D12Device> _device, bool _autoAllocPool, bool _forceOOM)
    {
        // create singleton
        g_poolContainer = new (MemoryId::mId_GfxAdapter) VRAMAllocator::PoolContainer(_device, _autoAllocPool, _forceOOM);
    }

    ComPtr<ID3D12Device> getDevice()
    {
        ITF_ASSERT(g_poolContainer != nullptr);
        if (g_poolContainer == nullptr)
        {
            return nullptr;
        }
        return g_poolContainer->getDevice();
    }

    void createPool(ComPtr<ID3D12Heap> _heap, VRAMResourceType _resType)
    {
        VRAMAllocator::PoolContainer::getptr()->createPool(std::move(_heap), _resType);
    }

    void createPool(ComPtr<ID3D12Heap> _heap, VRAMResourceType _resType, VRAMRelocHandle & _allocHdl)
    {
        D3D12_HEAP_DESC headDesc = _heap->GetDesc();
        ITF_ASSERT(headDesc.SizeInBytes < static_cast<UINT64>(std::numeric_limits<ChunkSize>::max())); // max 4GiB per pool
        ChunkSize blockSize = ChunkSize(headDesc.SizeInBytes);

        class ChunkContainer * chunkContainer = VRAMAllocator::PoolContainer::getptr()->createPool(std::move(_heap), _resType);
        _allocHdl.allocInContainer(blockSize, chunkContainer);
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

    template bbool canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize, u32 _alignment);
    template bbool canHandleAllocation<VRAMResourceType::RESOURCE_UPLOAD>(ChunkSize _blockSize, u32 _alignment);


    template <VRAMResourceType resType>
    bbool canHandleAllocation(ChunkSize _blockSize)
    {
        return VRAMAllocator::PoolContainer::getptr()->canHandleAllocation<resType>(_blockSize);
    }

    template bbool canHandleAllocation<VRAMResourceType::RESOURCE>(ChunkSize _blockSize);
    template bbool canHandleAllocation<VRAMResourceType::RESOURCE_UPLOAD>(ChunkSize _blockSize);

} // namespace ITF::DX12::VRAMAllocator
