#ifndef VRAM_POOLCONTAINER_H_
#define VRAM_POOLCONTAINER_H_

#include "core/templateSingleton.h"
#include "VRAMMemoryTypes.h"
#include "core/system/PS3/Synchronize_ps3.h"

namespace ITF
{
namespace GCM
{

struct VRAMHandleLinks;
class VRAMRelocHandle;

namespace VRAMAllocator
{
    class PoolContainer : public TemplateSingleton<PoolContainer>
    {
        // Pool = ChunkContainer
    public:

        PoolContainer(bool _forceOOMOverallBreak = true, bool _forceOOMOnLocalBreak = false, bool _forceOOMOnMainBreak = false);
        ~PoolContainer();
        
        // create a pool 
        void CreatePool(VRAMAddress startOfBlock, ChunkSize blockSize, VRAMOffset offset, u32 location);

        // call update to give a chance to defragmentation
        void Update(u32 cpuCurrentFrame, u32 gpuLastFrameCount, bool bForceErase);
        
        // print stats
        void PrintInfo();

        // get stats
        void GetStats(VRAMStats & stats);

        // Force a break during OOM
        void SetForceOOMBreak(bool _onLocal, bool _onMain, bool _overal);

        // Force a break during OOM
        void SetMainMemoryPoolGranularity(u32 _granularity);

        // allocate main memory and create a ChunkContainer to handle at least the given size
        // _staticBuffer : freeing the last block is not freeing the pool.
        // _reportArea : alloc in IO memory area for RSX report/timestamp (up to 16MB, 0x0e000000 to 0x0f000000, can be used for anything)
        // Note that _reportArea imply _staticBuffer
        class ChunkContainer * AllocAndCreateMainPool(ChunkSize _blockSize, bool _staticBuffer = false, bool _reportArea = false);

    private:
        friend class ::ITF::GCM::VRAMRelocHandle;
        //////////////////////////////////////////////////////////////////////////
        // RelocHdl access
        //////////////////////////////////////////////////////////////////////////
        
        // find the ChunkContainer where the VRAMHandleLinks is pointing
        class ChunkContainer * FindPoolFromLink(const VRAMHandleLinks & link);
        void EraseContainer(class ChunkContainer * chunkContainer); // delete an empty container

        // find the best ChunkContainer for a size, return NULL if no container found
        void Alloc(ChunkSize size, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx);
        void AllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx);
        
        void LocalAlloc(ChunkSize size, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx);
        void LocalAllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx);

        void MainAlloc(ChunkSize size, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx) { MainAlloc(size, type, container, idx, true); }
        void MainAllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx) { MainAllocAligned(size, align, type, container, idx, true); }

        void MainReportAlloc(ChunkSize size, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx);
        void MainReportAllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx);

        // alloc versions using _allocOnFail
        void MainAlloc(ChunkSize size, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx, bool _allocOnFail);
        void MainAllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx, bool _allocOnFail);

        typedef Vector<class ChunkContainer *> PoolVector;
        PoolVector m_localPools;
        PoolVector m_mainPools;
        PoolVector m_mainReportPools;
        
        bool m_forceOOMOnLocalBreak;
        bool m_forceOOMOnMainBreak;
        bool m_forceOOMOverallBreak;

        u32 m_mainMemoryPoolGranularity;
        Mutex m_mutex; // accessed in the VRAMRelocHandle system
    };
} // namespace VRAMAllocator
} // namespace GCM
} // ITF

#endif // #define VRAM_POOLCONTAINER_H_
