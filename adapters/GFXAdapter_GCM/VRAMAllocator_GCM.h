#ifndef ITF_VRAMALLOCATOR_GCM_H_
#define ITF_VRAMALLOCATOR_GCM_H_

#include "VRAMAllocator/VRAMMemoryTypes.h"

namespace ITF
{
    namespace GCM
    {
        namespace VRAMAllocator
        {
            // Interface for management of the allocator
            // Everyday allocations/free has to be done through the VRAMRelocHandle class

            // call at startup
            void Init(bool forceOOMBreak = true);

            // call to create memory pools (typically, initial local memory block)
            void CreatePool(VRAMAddress startOfBlock, ChunkSize blockSize, u32 location);

            // call to create static memory pools in main ram (by allocating it). blockSize is aligned on 1MB
            // Set _reportArea to get a static pool where RSX reports can be set
            void AllocAndCreateStaticMainPool(ChunkSize blockSize, bool _reportArea = false);

            // call update regularly to do incremental defragmentation (typically once per frame)
            void Update(u32 cpuCurrentFrame, u32 gpuLastFrameCount, bool bForceFlush = false);

            void PrintInfo();

            void GetStats(VRAMStats & stats);
        } // VRAMAllocator
    } // GCM
} // ITF

#endif // ITF_VRAMALLOCATOR_GCM_H_
