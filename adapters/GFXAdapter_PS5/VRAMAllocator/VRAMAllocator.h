#ifndef ITF_VRAMALLOCATOR_PS5_H_
#define ITF_VRAMALLOCATOR_PS5_H_

#pragma once

#include "VRAMMemoryTypes.h"

namespace ITF
{
    class MemoryPageManager;

    namespace Gnmp
    {
        class VRAMRelocHandle;

        namespace VRAMAllocator
        {
            // Interface for management of the allocator
            // Everyday allocations/free has to be done through the VRAMRelocHandle class

            // call at startup
            void init(MemoryPageManager & _pageMgr, bbool _autoAllocPool = btrue, bbool _forceOOM = bfalse);

            // call to create memory pools (typically, initial memory block)
            // The pool size is limited to 4GiB
            void createPool(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus = VRAMMemoryBus::DIRECT);

            // call to create memory pool and alloc a memory handle to the complete block
            void createPool(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus, VRAMRelocHandle& _allocHdl);
            inline void createPool(VRAMAddress _startOfBlock, ChunkSize _blockSize, VRAMResourceType _resType, VRAMRelocHandle& _allocHdl)
            {
                createPool(_startOfBlock, _blockSize, _resType, VRAMMemoryBus::DIRECT, _allocHdl);
            }

            // call to create static memory pools (by allocating it). blockSize is rounded on the next 64K at least.
            void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus = VRAMMemoryBus::DIRECT);

            // Same as previous with the given hdl being allocated in this area
            void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMMemoryBus _memBus, VRAMRelocHandle & _allocHdl);
            inline void allocAndCreateStaticPool(ChunkSize _blockSize, VRAMResourceType _resType, VRAMRelocHandle& _allocHdl)
            {
                allocAndCreateStaticPool(_blockSize, _resType, VRAMMemoryBus::DIRECT, _allocHdl);
            }

            // Release unused dynamic pools.
            void releaseUnusedDynamicPools();

            // call update regularly to do incremental defragmentation (typically once per frame)
            void update(u64 cpuCurrentFrame, u64 gpuLastFrameCount, bbool bForceFlush = false);

            void printInfo();

            void getStats(VRAMStats & stats);

            // this method can be used to build a loading policy according to the amount of memory available
            // It return true if the allocation can be performed without allocating an additional pool
            template <VRAMResourceType = READ_ONLY, VRAMMemoryBus = VRAMMemoryBus::DIRECT>
            bbool canHandleAllocation(ChunkSize _blockSize, u32 _alignment);
            template <VRAMResourceType = READ_ONLY, VRAMMemoryBus = VRAMMemoryBus::DIRECT>
            bbool canHandleAllocation(ChunkSize _blockSize);

        } // VRAMAllocator
    } // Gnmp
} // ITF

#endif // ITF_VRAMALLOCATOR_PS5_H_
