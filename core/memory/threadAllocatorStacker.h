#ifndef _ITF_THREADALLOCATIONTRACKER_H_
#define _ITF_THREADALLOCATIONTRACKER_H_

#ifndef _ITF_STACKWALKER_H_
#include "core/StackWalker.h"
#endif //_ITF_STACKWALKER_H_

#ifndef _ITF_MAPMEMORYTRACKER_H_
#include "core/memory/mapMemoryTracker.h"
#endif //_ITF_MAPMEMORYTRACKER_H_

#ifndef ITF_MEMORYTRACKINGVECTOR_H_
#include "core/memory/memoryTrackingVector.h"
#endif //ITF_MEMORYTRACKINGVECTOR_H_

#ifndef ITF_MEMORYEVENT_H_
#include "core/memory/memoryEvent.h"
#endif //ITF_MEMORYEVENT_H_

#include "IcePairManager.h"

#ifdef ITF_TRACKING_MEMORY_ENABLE

namespace ITF
{

    class   Blob;
    class   TraceInfo;

    extern u32 taggerEvent;
    extern u64 currentContextTag;

    struct frameAllocationTracking
    {
        bool    alive;
        u32     crc;
        uPtr    size;
        u32     attributes;

    };
   
    class threadAllocatorTracker
    {
    public:
        threadAllocatorTracker();
       
        virtual ~threadAllocatorTracker()
        {
            dumpAllocationShutdownInternal();
        }

        void * operator new( size_t size )
        {
            return malloc( size );
        }

        void operator delete(void* ptr)
        {
            ::free(ptr);
        }

        static void destroyInstance()
        {
            if (m_TreadAllocatorTracker)
                delete m_TreadAllocatorTracker;

            m_TreadAllocatorTracker = NULL;
        }

        static const u32 MAX_STACK_DEPTH = 40;


    #define MAKE_MEMCHUNCK(size, crc) (u64)((((u64)(size) << 32) | crc))
    #define GET_SIZECHUNCK(chunck)     (uPtr)((chunck >> 32))
    #define GET_CRCCHUNCK(chunck)     (uPtr)((chunck & 0xFFFFFFFF))

        void dumpMemoryCapture();

        void flushFrameAllocation();
     
        static void         dumpAllocationShutdown() {getInstance()->dumpAllocationShutdownInternal();};

        void dumpAllocationShutdownInternal();

        void dumpCaptureTopAllocation(Blob& blob,u32 _allocatorID,u32 _countMax);

        void dumpCaptureFrameAllocation(Blob& blob);

        void requestCaptureFrameAllocation(u32 _allocatorID,u32 _countMax);
        
        void dumpAllocatorID(Blob& blob,u32 _allocatorID);

        bbool hasCaptureFrameAllocationDone() {return m_trackFrameEnable;}
        void  disableCaptureFrameAllocation() {m_trackFrameEnable = bfalse;}

        void enableEvents(const SafeArray<u32>& _events);

        void addAllocation(ITF_AllocatorAttributes attributes, uPtr _size, uPtr _memoryAddress, u_long _crc, TraceInfo& _trace);

        bbool removeAllocation(uPtr _memoryAddress, u32 _dwAllocAttributes);//memory address used by instance,(not real address allocated)

        static void createInstance();
       
        void recordEvent(memoryEvent* _memEvent);
        void pushEvent(memoryEvent* _memEvent);
        void popBackEvent(memoryEvent* _event,u64 _currentcontext);

        static threadAllocatorTracker* m_TreadAllocatorTracker;
        static threadAllocatorTracker*  getInstance() {return  m_TreadAllocatorTracker;}

    #if defined (ITF_WINDOWS)
        StackWalker& getStackWalker()   {return m_stackWalker;}
    #endif //ITF_WINDOWS

        ITF_INLINE bbool isEnable() const   {return m_bEnable;}

        const memoryTrackingVector<memoryEvent>& getEventsCaptured()    {return  m_previousRecordingOneFrame;}

        bbool eventSelected(memoryEvent* _event);

        void started(bbool _val)  {m_started = _val;}

     protected:

        ITF_THREAD_CRITICAL_SECTION             m_cs;
        mapMemoryTracker<u32,u64>               m_map;
        UserPairManager2                        m_allocationMap;
        memoryTrackingVector<memoryEvent>       m_recordingOneFrame;
        memoryTrackingVector<memoryEvent>       m_previousRecordingOneFrame;
	
        u64                                     m_CountAllocation;
        bbool                                   m_bEnable;

        mapMemoryTracker<u_long,TraceInfo*>     m_callstack;

        //allocation tracking frame
        bbool                                   m_trackFrameEnable;
        bbool                                   m_trackFrameRequested;
        bbool                                   m_started;
        u32                                     m_trackingAllocatorID;
        u32                                     m_trackingMaxCount;

        memoryTrackingVector<frameAllocationTracking>   m_allocationFrame;

        SafeArray<u32>                          m_eventTracker;
        u32                                     m_currentPos;
        u64                                     m_captureContextTag;

    #if defined (ITF_WINDOWS)
        StackWalker m_stackWalker;
    #endif //ITF_WINDOWS

        UserPair*   copyAllocations(u32& _retCountAllocation);
        
    };

    class  TraceInfo
    {
    public:
        u32 m_attributes;
        uPtr m_stack[threadAllocatorTracker::MAX_STACK_DEPTH];
    };

    class threadAllocatorTrackerScope
    {
    public:
        threadAllocatorTrackerScope()
        {
            threadAllocatorTracker::createInstance();
        }

        ~threadAllocatorTrackerScope()
        {
            threadAllocatorTracker::destroyInstance();
        }
    };
}


#endif //ITF_TRACKING_MEMORY_ENABLE
#endif //_ITF_THREADALLOCATIONTRACKER_H_
