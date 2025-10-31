#include "precompiled_GFXAdapter_GCM.h"

#include "GCM_API.h"

#include "VRAMAllocator_GCM.h"
#include "VRAMAllocator/PoolContainer.h"

namespace ITF
{
    namespace GCM
    {

        namespace VRAMAllocator
        {

        //////////////////////////////////////////////////////////////////////////
        // allocator impl
        //////////////////////////////////////////////////////////////////////////
        void Init(bool forceOOMBreak)
        {
            VRAMAllocator::PoolContainer * pool = new VRAMAllocator::PoolContainer(forceOOMBreak);
        }

        void CreatePool(VRAMAddress startOfBlock, VRAMChunkSize blockSize, u32 location)
        {
            VRAMOffset offset = 0;
            cellGcmAddressToOffset(startOfBlock, &offset);
            VRAMAllocator::PoolContainer::getptr()->CreatePool(startOfBlock, blockSize, offset, location);
        }

        void AllocAndCreateStaticMainPool(VRAMChunkSize blockSize, bool _reportArea)
        {
            VRAMAllocator::PoolContainer::getptr()->AllocAndCreateMainPool(blockSize, true, _reportArea);
        }

        // do a bit for defragmentation
        void Update(u32 cpuCurrentFrame, u32 gpuLastFrameCount, bool bForceFlush)
        {
            VRAMAllocator::PoolContainer::getptr()->Update(cpuCurrentFrame, gpuLastFrameCount, bForceFlush);
        }

        void PrintInfo()
        {
            VRAMAllocator::PoolContainer::getptr()->PrintInfo();
        }

        void GetStats(VRAMStats & stats)
        {
            VRAMAllocator::PoolContainer::getptr()->GetStats(stats);
        }
        } // namespace VRAMAllocator
    }
}
