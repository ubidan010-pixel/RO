#include "precompiled_engine.h"
#include <cell/spurs/task.h>
#include <edge/zlib/edgezlib_ppu.h>
#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"
#include "EdgeUncompress_PS3.h"
#include "Compress.h"

#ifndef ZLIB_H
#include "ZLib/zlib.h"
#endif //ZLIB_H

#ifndef ZUTIL_H
#include "ZLib/zutil.h"
#endif //ZUTIL_H


namespace ITF
{
    static CellSpursTaskset s_taskSet;			//Cannot be on stack

    static EdgeZlibInflateQHandle s_inflateQueue;
    Vector<void*> s_vecTaskContext;
    void* s_pInflateQueueBuffer;

    // Test compatibility between our zlib and edgeZLib
    static void TestEdgeUnCompress()
    {
        const ux srcSize = 1024*1024; // 1 Meg of data
        u8 * srcData = new u8[srcSize];
        // build it with pseudo random data
        u32 seed = 12345;
        // Stressing the compressor -> the compressed version is bigger
        for(u32 i = 0; i<srcSize/4; ++i)
        {
            seed = seed * 214013u + 2531011u;
            ((u32*)srcData)[i] = seed;
        }

        ux compMaxSize = Compress::computeSizeRequired(srcSize);
        u8 * compData = new u8[compMaxSize];
        i32 compSize = Compress::compressBuffer(compData, compMaxSize, srcData, srcSize);

        u8 * refData = new u8[srcSize];
        EdgeUnCompress::uncompressBuffer(refData, srcSize, compData, compSize);
        
        if (memcmp(refData, srcData, srcSize) != 0)
        {
            ITF_DEBUG_BREAK();
        }

        delete [] srcData;
        delete [] compData;
        delete [] refData;
    }

    // Test the edge copy buffer
    static void TestEdgeCopyBuffer(ux sizeTest)
    {
        const ux srcSize = sizeTest;
        u8 * srcData = new u8[srcSize];

        // build it with pseudo random data
        u32 seed = 12345;
        // Stressing the compressor -> the compressed version is bigger
        for(u32 i = 0; i<srcSize/4; ++i)
        {
            seed = seed * 214013u + 2531011u;
            ((u32*)srcData)[i] = seed;
        }

        u8 * destData = new u8[srcSize+2];
        destData[0] = 0xDB;
        destData[srcSize+1] = 0xDE;
        i32 copySize = EdgeUnCompress::copyBuffer(destData+1, srcSize, srcData);

        if (destData[0] != 0xDB || destData[srcSize+1] != 0xDE)
        {
            ITF_DEBUG_BREAK();
        }

        if (memcmp(srcData, destData+1, srcSize) != 0)
        {
            ITF_DEBUG_BREAK();
        }

        delete [] srcData;
        delete [] destData;
    }

    void EdgeUnCompress::Init(CellSpurs * _spurs, ux _nbTasks)
    {
        CellSpursTasksetAttribute taskSetAttribute;
        uint8_t prios[8] = {1, 1, 1, 1, 1, 1, 1, 1};
        ITF_VERIFY(CELL_OK == cellSpursTasksetAttributeInitialize(&taskSetAttribute, 0, prios, 8));
        ITF_VERIFY(CELL_OK == cellSpursTasksetAttributeSetName(&taskSetAttribute, "edgeZlibTaskSet"));
        ITF_VERIFY(CELL_OK == cellSpursCreateTasksetWithAttribute(_spurs, &s_taskSet, &taskSetAttribute));

        //////////////////////////////////////////////////////////////////////////
        //
        //	Initialize Inflate Queue.
        //	This will hold the queue of work that the Inflate Task(s) on SPU will
        //	pull work from.
        //
        //////////////////////////////////////////////////////////////////////////
        const uint32_t kMaxNumInflateQueueEntries = 16;
        uint32_t inflateQueueBuffSize = edgeZlibGetInflateQueueSize(kMaxNumInflateQueueEntries);
        s_pInflateQueueBuffer = Memory::alignedMalloc(inflateQueueBuffSize, EDGE_ZLIB_INFLATE_QUEUE_ALIGN);
        s_inflateQueue = edgeZlibCreateInflateQueue(_spurs, kMaxNumInflateQueueEntries, s_pInflateQueueBuffer, inflateQueueBuffSize);

        //////////////////////////////////////////////////////////////////////////
        //
        //	Build Inflate Tasks.
        //	We want the decompression to be able to run in parallel on multiple
        //	SPUs so we create as many tasks as SPUs that we want it to run
        //
        //////////////////////////////////////////////////////////////////////////

        s_vecTaskContext.resize(_nbTasks);
        for( uint32_t taskNum = 0 ; taskNum < _nbTasks; taskNum++ )
        {
            uint32_t contextSaveSize = edgeZlibGetInflateTaskContextSaveSize();
            s_vecTaskContext[taskNum] = Memory::alignedMalloc(contextSaveSize, CELL_SPURS_TASK_CONTEXT_ALIGN);
            edgeZlibCreateInflateTask(&s_taskSet, s_vecTaskContext[taskNum], s_inflateQueue);
        }

#if defined(_DEBUG)
        TestEdgeUnCompress();
        TestEdgeCopyBuffer(0);
        TestEdgeCopyBuffer(1);
        TestEdgeCopyBuffer(7);
        TestEdgeCopyBuffer(16);
        TestEdgeCopyBuffer(64*1024);
        TestEdgeCopyBuffer(256*1024);
        TestEdgeCopyBuffer(256*1024+7);
        TestEdgeCopyBuffer(1024*1024+64*1024);
        TestEdgeCopyBuffer(1*1024*1024 + 512*1024 + 137); // arbitrary unaligned and over 16*64K
#endif
    }

    void EdgeUnCompress::Destroy()
    {
        //////////////////////////////////////////////////////////////////////////
        //
        //	Shutdown Inflate Queue, and taskset.
        //
        //////////////////////////////////////////////////////////////////////////

        edgeZlibShutdownInflateQueue(s_inflateQueue);

        // shutdown taskSet
        ITF_VERIFY(CELL_OK == cellSpursShutdownTaskset(&s_taskSet));

        // wait for all tasks to finish
        ITF_VERIFY(CELL_OK == cellSpursJoinTaskset(&s_taskSet));

        // free alloc'd buffers
        for(uint32_t taskNum = 0 ; taskNum < s_vecTaskContext.size(); taskNum++)
        {
            Memory::alignedFree(s_vecTaskContext[taskNum]);
        }
        s_vecTaskContext.clear();
        Memory::alignedFree(s_pInflateQueueBuffer);
    }

    //----------------------------------------------------------------------------------------------------
    // uncompress using edge zlib
    // should be multi-thread safe (not tested)
    //----------------------------------------------------------------------------------------------------
    i32 EdgeUnCompress::uncompressBuffer(u8 *target, u32 target_size, const u8 *source, u32 source_size) 
    {
        ITF_ASSERT(source_size>=4u);

        //////////////////////////////////////////////////////////////////////////
        //	Add one item to the Inflate Queue.
        //	The Inflate Task will wake up and process this work.
        //////////////////////////////////////////////////////////////////////////

        // To get the method thread safe, the event is allocated for each decompression
        // (unfortunately events cannot be on stack)
        CellSpursEventFlag  * eventFlag = (CellSpursEventFlag *)Memory::alignedMalloc(sizeof(CellSpursEventFlag), CELL_SPURS_EVENT_FLAG_ALIGN) ; //Cannot be on stack
        ITF_VERIFY(CELL_OK == cellSpursEventFlagInitialize(&s_taskSet, eventFlag, CELL_SPURS_EVENT_FLAG_CLEAR_AUTO, CELL_SPURS_EVENT_FLAG_SPU2PPU));
        ITF_VERIFY(CELL_OK == cellSpursEventFlagAttachLv2EventQueue(eventFlag));
 
        u32 * taskCount = NULL;
    #ifndef ITF_FINAL
        taskCount = new u32;
        (*taskCount) = 1;
    #endif

        // if the queue is full, the add will wait
        uint16_t eventFlagBits = 1;
        edgeZlibAddInflateQueueElement(
            s_inflateQueue,
            source+2u, source_size-4u, // EdgeZLib strangely ask to remove a header and footer from the classical zlib data
            target, target_size,
            taskCount,
            eventFlag,
            eventFlagBits,
            kEdgeZlibInflateTask_Inflate
            );

        //////////////////////////////////////////////////////////////////////////
        //	Wait for the event flag
        //////////////////////////////////////////////////////////////////////////

        ITF_VERIFY(CELL_OK == cellSpursEventFlagWait( eventFlag, &eventFlagBits, CELL_SPURS_EVENT_FLAG_AND));

        ITF_VERIFY(CELL_OK == cellSpursEventFlagDetachLv2EventQueue(eventFlag));
    #ifndef ITF_FINAL
        ITF_ASSERT_CRASH((*taskCount) == 0, "Error during edge zlib decompression\n");
    #endif
        
        Memory::alignedFree(eventFlag);
        delete taskCount;

        return target_size;
    }

    i32 EdgeUnCompress::copyBuffer(u8 * _target, u32 _size, const u8 * _source)
    {
        // 0 size case
        if (_size == 0)
            return 0;

        //////////////////////////////////////////////////////////////////////////
        //	Add one item to the Inflate Queue.
        //	The Inflate Task will wake up and process this work.
        //////////////////////////////////////////////////////////////////////////

        // To get the method thread safe, the event is allocated for each decompression
        // (unfortunately events cannot be on stack)
        CellSpursEventFlag  * eventFlag = (CellSpursEventFlag *)Memory::alignedMalloc(sizeof(CellSpursEventFlag), CELL_SPURS_EVENT_FLAG_ALIGN) ; //Cannot be on stack
        ITF_VERIFY(CELL_OK == cellSpursEventFlagInitialize(&s_taskSet, eventFlag, CELL_SPURS_EVENT_FLAG_CLEAR_AUTO, CELL_SPURS_EVENT_FLAG_SPU2PPU));
        ITF_VERIFY(CELL_OK == cellSpursEventFlagAttachLv2EventQueue(eventFlag));

        u32 nbChunk = _size / u32(kEdgeZlibInflateInputBufferMaxSize);
        nbChunk += nbChunk * u32(kEdgeZlibInflateInputBufferMaxSize)<_size?1:0;

        u32 * taskCount = new u32;
        (*taskCount) = nbChunk;
        u32 sizeLeft = _size;

        uint16_t eventFlagBits = 1;

        for(ux i = 0; i<nbChunk-1; ++i)
        {
            // if the queue is full, the add will wait
            edgeZlibAddInflateQueueElement(
                s_inflateQueue,
                _source, kEdgeZlibInflateInputBufferMaxSize,
                _target, kEdgeZlibInflateInputBufferMaxSize,
                taskCount,
                eventFlag,
                eventFlagBits,
                kEdgeZlibInflateTask_Memcpy
                );
            sizeLeft -= kEdgeZlibInflateInputBufferMaxSize;
            _source += kEdgeZlibInflateInputBufferMaxSize;
            _target += kEdgeZlibInflateInputBufferMaxSize;
        }

        ITF_ASSERT(sizeLeft>0);
        ITF_ASSERT(sizeLeft<=kEdgeZlibInflateInputBufferMaxSize);
        edgeZlibAddInflateQueueElement(
            s_inflateQueue,
            _source, sizeLeft,
            _target, sizeLeft,
            taskCount,
            eventFlag,
            eventFlagBits,
            kEdgeZlibInflateTask_Memcpy
            );

        //////////////////////////////////////////////////////////////////////////
        //	Wait for the event flag
        //////////////////////////////////////////////////////////////////////////
        ITF_VERIFY(CELL_OK == cellSpursEventFlagWait( eventFlag, &eventFlagBits, CELL_SPURS_EVENT_FLAG_AND));
        ITF_VERIFY(CELL_OK == cellSpursEventFlagDetachLv2EventQueue(eventFlag));
        ITF_ASSERT_CRASH((*taskCount) == 0, "Error during edge zlib memcpy\n");

        Memory::alignedFree(eventFlag);
        delete taskCount;
        return _size;
    }

} // namespace ITF

