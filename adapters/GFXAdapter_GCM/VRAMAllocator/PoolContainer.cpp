#include "precompiled_GFXAdapter_GCM.h"

#include "../GCM_API.h"

#include <sys/fixed_addr.h>

#include "core/math/vec2d.h"
#include "core/math/MathTools.h"
#include "PoolContainer.h"
#include "VRAMMemoryTypes.h"
#include "ChunkContainer.h"
#include "VRAMTools.h"

namespace ITF
{
namespace GCM
{
namespace VRAMAllocator
{
#ifndef ITF_FINAL
    // set to 1 in the debugger to output VRAM info
    int g_printVramInfo = 0;
#endif
    //////////////////////////////////////////////////////////////////////////
    //  Pool container
    //////////////////////////////////////////////////////////////////////////

    PoolContainer::PoolContainer(bool _forceOOMOverallBreak, bool _forceOOMOnLocalBreak, bool _forceOOMOnMainBreak)
    : m_forceOOMOnLocalBreak(_forceOOMOnLocalBreak)
    , m_forceOOMOnMainBreak(_forceOOMOnMainBreak)
    , m_forceOOMOverallBreak(_forceOOMOverallBreak)
    , m_mainMemoryPoolGranularity(10*1024*1024)
    {
    }

    PoolContainer::~PoolContainer()
    {
        // not implemented, we may need to free every pool ?
    }

    void PoolContainer::CreatePool(VRAMAddress startOfBlock, ChunkSize blockSize, VRAMOffset offsetStart, u32 location)
    {
        ScopeLockMutex lock(m_mutex);
        ChunkContainer * container = new ChunkContainer();
        bool forceOOM;
        if (location == CELL_GCM_LOCATION_LOCAL)
        {
            forceOOM = m_forceOOMOnLocalBreak;
            VRAM_ALLOC_ASSERT(u32(offsetStart)<256*1024*1024); // local = 0 .. 256M
            VRAM_ALLOC_ASSERT(u32(startOfBlock) < RSX_FB_BASE_ADDR + 256u*1024u*1024u); // Start at 0xC0000000
            VRAM_ALLOC_ASSERT(u32(startOfBlock) >= RSX_FB_BASE_ADDR);
            VRAM_ALLOC_ASSERT(u32(startOfBlock) - RSX_FB_BASE_ADDR == offsetStart); // Correct offset according to startOfBlock
            m_localPools.push_back(container);
        }
        else
        {
            forceOOM = m_forceOOMOnMainBreak;
            if (offsetStart>=ReportIOOffsetStart && offsetStart<ReportIOOffsetEnd)
            {
                m_mainReportPools.push_back(container);
            }
            else
            {
                m_mainPools.push_back(container);
            }
        }
        container->Init(startOfBlock, blockSize, offsetStart, location, forceOOM, true, true);
    }

    // allocate main memory and create a ChunkContainer to handle at least the given size
    // if staticBuffer, the pool will never be deleted
    ChunkContainer * PoolContainer::AllocAndCreateMainPool(ChunkSize _blockSize, bool _staticBuffer, bool _reportArea)
    {
        ScopeLockMutex lock(m_mutex);
        VRAM_ALLOC_ASSERT(_blockSize<256u*1024u*1024u);
        VRAM_ALLOC_ASSERT(!_reportLocation || _staticBuffer);
        _blockSize = GetAboveAlign(_blockSize, 1024u*1024u);
        VRAMAddress block = 0;
        VRAMOffset offsetOfBlock = 0;
        AllocRSXMappedMainMemory(_blockSize, block, offsetOfBlock, _reportArea);
        if (!block)
        {
            VRAM_ALLOC_TRACE("**************************************************************************\n");
            VRAM_ALLOC_TRACE("Out of RAM in the VRAM pool container : requested size %d bytes\n", _blockSize);
            VRAM_ALLOC_TRACE("**************************************************************************\n");
            return 0;
        }
        ChunkContainer * container = new ChunkContainer();
        if(_reportArea)
        {
            m_mainReportPools.push_back(container);
        }
        else
        {
            m_mainPools.push_back(container);
        }
        container->Init(block, _blockSize, offsetOfBlock, CELL_GCM_LOCATION_MAIN, m_forceOOMOnMainBreak, true, _staticBuffer);
        return container;
    }

    // do a bit for defragmentation
    void PoolContainer::Update(u32 cpuCurrentFrame, u32 gpuLastFrameCount, bool bForceErase)
    {
        ScopeLockMutex lock(m_mutex);
        #ifdef VRAM_PRINT_INFO
        if (g_printVramInfo!=0)
        {
            PrintInfo();
            g_printVramInfo = 0;
        }
        #endif
        for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
        {
            (*it)->Update(cpuCurrentFrame, gpuLastFrameCount, bForceErase);
        }
        for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
        {
            (*it)->Update(cpuCurrentFrame, gpuLastFrameCount, bForceErase);
        }
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            (*it)->Update(cpuCurrentFrame, gpuLastFrameCount, bForceErase);
        }
    }

    void PoolContainer::PrintInfo()
    {
        ScopeLockMutex lock(m_mutex);

        int count = 0;
        for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
        {
            VRAM_ALLOC_TRACE("$$$$$ Local Pool %d $$$$$\n", count);
            count++;
            (*it)->PrintInfo();
        }
        for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
        {
            VRAM_ALLOC_TRACE("$$$$$ Main Pool %d $$$$$\n", count);
            count++;
            (*it)->PrintInfo();
        }
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            VRAM_ALLOC_TRACE("$$$$$ Main Report Pool %d $$$$$\n", count);
            count++;
            (*it)->PrintInfo();
        }
    }

    void PoolContainer::GetStats(VRAMStats & stats)
    {
        ScopeLockMutex lock(m_mutex);

        memset(&stats, 0, sizeof(stats));

        int count = 0;
        for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
        {
            u32 freeSize = 0;
            u32 freeBiggestSize = 0;
            u32 initialSize = 0;
            (*it)->GetStats(freeSize, freeBiggestSize, initialSize);
            stats.vramBiggestBlockSize = std::max(freeBiggestSize, stats.vramBiggestBlockSize);
            stats.vramFreeSize += freeSize;
            stats.vramTotalSize += initialSize;
        }
        for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
        {
            u32 freeSize = 0;
            u32 freeBiggestSize = 0;
            u32 initialSize = 0;
            (*it)->GetStats(freeSize, freeBiggestSize, initialSize);
            stats.mainBiggestBlockSize = std::max(freeBiggestSize, stats.mainBiggestBlockSize);
            stats.mainFreeSize += freeSize;
            stats.mainTotalSize += initialSize;
        }
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            u32 freeSize = 0;
            u32 freeBiggestSize = 0;
            u32 initialSize = 0;
            (*it)->GetStats(freeSize, freeBiggestSize, initialSize);
            stats.mainBiggestBlockSize = std::max(freeBiggestSize, stats.mainBiggestBlockSize);
            stats.mainFreeSize += freeSize;
            stats.mainTotalSize += initialSize;
        }
    }

    // find the ChunkContainer where the VRAMHandleLinks is pointing
    ChunkContainer * PoolContainer::FindPoolFromLink(const VRAMHandleLinks & link)
    {
        ScopeLockMutex lock(m_mutex);

        for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
        {
            if ((*it)->IsOwner(link))
            {
                return *it;
            }
        }
        for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
        {
            if ((*it)->IsOwner(link))
            {
                return *it;
            }
        }
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            if ((*it)->IsOwner(link))
            {
                return *it;
            }
        }
        return 0;
    }
        
    // Force a break when OOM
    void PoolContainer::SetForceOOMBreak(bool _onLocal, bool _onMain, bool _overal)
    {
        ScopeLockMutex lock(m_mutex);

        m_forceOOMOnLocalBreak = _onLocal;
        m_forceOOMOnMainBreak = _onMain;
        m_forceOOMOverallBreak = _overal;
        
        for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
        {
            (*it)->SetForceOOMBreak(m_forceOOMOnLocalBreak);
        }
        for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
        {
            (*it)->SetForceOOMBreak(m_forceOOMOnMainBreak);
        }
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            (*it)->SetForceOOMBreak(m_forceOOMOnMainBreak);
        }
    }

    void PoolContainer::SetMainMemoryPoolGranularity(u32 _granularity)
    {
        m_mainMemoryPoolGranularity = _granularity;
    }

    // destroy an empty container
    void PoolContainer::EraseContainer(class ChunkContainer * chunkContainer)
    {
        ScopeLockMutex lock(m_mutex);

        VRAM_ALLOC_ASSERT(chunkContainer);
        VRAM_ALLOC_ASSERT(chunkContainer->GetLocation() == CELL_GCM_LOCATION_MAIN); // right now, we never supposed to delete a VRAM container
        VRAM_ALLOC_ASSERT(chunkContainer->IsEmpty()); // hmm, that's not correct to delete a non empty container as RelocHdl can still be pointing on it

        if (chunkContainer->GetLocation() == CELL_GCM_LOCATION_MAIN)
        {
            for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
            {
                if ((*it)==chunkContainer)
                {
                    m_mainPools.erase(it);
                    return;
                }
            }
            VRAMAddress buffer = chunkContainer->GetStartAddress();
            delete chunkContainer;
            FreeRSXMappedMainMemory(buffer);
        }
        else
        {
            for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
            {
                if ((*it)==chunkContainer)
                {
                    m_localPools.erase(it);
                    break;
                }
            }
            delete chunkContainer;
        }
    }

    // find the best ChunkContainer for a size, return NULL if no container found
    void PoolContainer::Alloc(ChunkSize size, VramMemoryType type, ChunkContainer * & container, ChunkIdx & idx)
    {
        ScopeLockMutex lock(m_mutex);
        // Policy can be changed here
#if 0
        if (type == VRAM_Texture) // Texture -> Main first
        {
            MainAlloc(size, type, container, idx, false);
            if (container!=0)
            {
                return;
            }
        }
#endif
        LocalAlloc(size, type, container, idx);
        if (container==0)
        {
            VRAM_ALLOC_ASSERT(idx == ChunkContainer::InvalidIndex);
            MainAlloc(size, type, container, idx);
        }
    }

    void PoolContainer::AllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, ChunkContainer * & container, ChunkIdx & idx)
    {
        ScopeLockMutex lock(m_mutex);
        // Policy can be changed here
#if 0
        if (type == VRAM_Texture) // Texture -> Main first
        {
            MainAllocAligned(size, align, type, container, idx, false);
            if (container!=0)
            {
                return;
            }
        }
#endif
        LocalAllocAligned(size, align, type, container, idx);
        if (container==0)
        {
            VRAM_ALLOC_ASSERT(idx == ChunkContainer::InvalidIndex);
            MainAllocAligned(size, align, type, container, idx);
        }
    }

    void PoolContainer::LocalAlloc(ChunkSize size, VramMemoryType type, ChunkContainer * & container, ChunkIdx & idx)
    {
        ScopeLockMutex lock(m_mutex);
        for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
        {
            idx = (*it)->Alloc(size, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        container = 0;
        idx = ChunkContainer::InvalidIndex;
    }

    void PoolContainer::LocalAllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, ChunkContainer * & container, ChunkIdx & idx)
    {
        ScopeLockMutex lock(m_mutex);
        for(PoolVector::iterator it = m_localPools.begin(); it!=m_localPools.end(); ++it)
        {
            idx = (*it)->AllocAligned(size, align, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        container = 0;
        idx = ChunkContainer::InvalidIndex;
    }

    void PoolContainer::MainAlloc(ChunkSize size, VramMemoryType type, ChunkContainer * & container, ChunkIdx & idx, bool _allocOnFail)
    {
        ScopeLockMutex lock(m_mutex);
        for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
        {
            idx = (*it)->Alloc(size, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            idx = (*it)->Alloc(size, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        if (_allocOnFail)
        {
            container = AllocAndCreateMainPool(Max(m_mainMemoryPoolGranularity, size));
            if (container)
            {
                idx = container->Alloc(size, type);
                return;
            }
        }
        // Force a break ?
        if (m_forceOOMOverallBreak)
        {
            PrintInfo();
            // Call mem dump callback ?
            // Write to null ptr to force a crash.
            *(static_cast<char *>(0)) = 0;
        }
        container = NULL;
        idx = ChunkContainer::InvalidIndex;
    }

    void PoolContainer::MainAllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, ChunkContainer * & container, ChunkIdx & idx, bool _allocOnFail)
    {
        ScopeLockMutex lock(m_mutex);
        for(PoolVector::iterator it = m_mainPools.begin(); it!=m_mainPools.end(); ++it)
        {
            idx = (*it)->AllocAligned(size, align, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            idx = (*it)->AllocAligned(size, align, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        VRAM_ALLOC_ASSERT(align <= 1024*1024); // lets be sure the user is not asking us a non realistic alignment
        if (_allocOnFail)
        {
            container = AllocAndCreateMainPool(Max(m_mainMemoryPoolGranularity, size));
            if (container)
            {
                idx = container->AllocAligned(size, align, type);
                return;
            }
        }
        // Force a break ?
        if (m_forceOOMOverallBreak)
        {
            PrintInfo();
            // Call mem dump callback ?
            // Write to null ptr to force a crash.
            *(static_cast<char *>(0)) = 0;
        }
        container = NULL;
        idx = ChunkContainer::InvalidIndex;
    }

    void PoolContainer::MainReportAlloc(ChunkSize size, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx)
    {
        ScopeLockMutex lock(m_mutex);
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            idx = (*it)->Alloc(size, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        // should have been allocated
        idx = ChunkContainer::InvalidIndex;
    }

    void PoolContainer::MainReportAllocAligned(ChunkSize size, ChunkSize align, VramMemoryType type, class ChunkContainer * & container, ChunkIdx & idx)
    {
        ScopeLockMutex lock(m_mutex);
        for(PoolVector::iterator it = m_mainReportPools.begin(); it!=m_mainReportPools.end(); ++it)
        {
            idx = (*it)->AllocAligned(size, align, type);
            if (idx!=ChunkContainer::InvalidIndex)
            {
                container = *it;
                return;
            }
        }
        idx = ChunkContainer::InvalidIndex;
    }

} // namespace VRAMAllocator
} // namespace GCM
} // namespace ITF
