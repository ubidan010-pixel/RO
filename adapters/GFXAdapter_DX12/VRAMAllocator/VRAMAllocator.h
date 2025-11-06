#pragma once

#include "VRAMMemoryTypes.h"

namespace ITF::DX12
{
    class VRAMRelocHandle;
}

namespace ITF::DX12::VRAMAllocator
{
    // Interface for management of the allocator
    // Everyday allocations/free has to be done through the VRAMRelocHandle class

    // call at startup
    void init(ComPtr<ID3D12Device> _device, bool _autoAllocPool = true, bool _forceOOM = false);

    // Just return the device given at init
    ComPtr<ID3D12Device> getDevice();

    // call to create memory pools (typically, initial memory block)
    // The _heap size is limited to 4GiB
    void createPool(ComPtr<ID3D12Heap> _heap, VRAMResourceType _resType);

    // call to create memory pool and alloc a memory handle to the complete block
    // The _heap size is limited to 4GiB
    void createPool(ComPtr<ID3D12Heap> _heap, VRAMResourceType _resType, VRAMRelocHandle& _allocHdl);

    // call to create static memory pools (by allocating it). blockSize is rounded on the next 64K at least.
    void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType);

    // Same as previous with the given hdl being allocated in this area
    void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMRelocHandle & _allocHdl);

    // Release unused dynamic pools.
    void releaseUnusedDynamicPools();

    // call update regularly to do incremental defragmentation (typically once per frame)
    void update(u64 cpuCurrentFrame, u64 gpuLastFrameCount, bbool bForceFlush = false);

    void printInfo();

    void getStats(VRAMStats & stats);

    // this method can be used to build a loading policy according to the amount of memory available
    // It return true if the allocation can be performed without allocating an additional pool
    template <VRAMResourceType = VRAMResourceType::RESOURCE>
    bbool canHandleAllocation(ChunkSize _blockSize, u32 _alignment);
    template <VRAMResourceType = VRAMResourceType::RESOURCE>
    bbool canHandleAllocation(ChunkSize _blockSize);
} // VRAMAllocator
