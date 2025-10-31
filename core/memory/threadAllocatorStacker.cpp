#include "precompiled_core.h"

#ifndef _ITF_THREADALLOCATIONTRACKER_H_
#include "core/memory/threadAllocatorStacker.h"
#endif //_ITF_THREADALLOCATIONTRACKER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifdef ITF_TRACKING_MEMORY_ENABLE

#ifdef ITF_X360
#include <xbdm.h>
#endif //ITF_X360


namespace ITF
{

    threadAllocatorTracker* threadAllocatorTracker::m_TreadAllocatorTracker = NULL;

    threadAllocatorTracker::threadAllocatorTracker() 
    {
        m_bEnable = ISDEBUGGER_PRESENT;
#if defined (ITF_WINDOWS) || defined (ITF_X360)
        LPSTR cmdLine =  GetCommandLineA();
        if (strstr(cmdLine,"leakdetectoroff"))
            m_bEnable = bfalse;
#endif //defined (ITF_WINDOWS) || defined (ITF_X360)

        m_trackFrameEnable          = bfalse;
        m_trackFrameRequested       = bfalse;
    
        m_CountAllocation           = 0;
        m_trackingMaxCount          = 0;
        Synchronize::createCriticalSection(&m_cs);
		ITF_MemoryBarrier();
#if defined (ITF_WINDOWS)
        if (m_bEnable)
        {
            m_stackWalker.initialize();
        }
#endif //ITF_WINDOWS

        m_started = bfalse;
    }

    void threadAllocatorTracker::createInstance()
    {
        threadAllocatorTracker* pThis = new threadAllocatorTracker();
        ITF_MemoryBarrier();
        m_TreadAllocatorTracker = pThis;
    }

    void threadAllocatorTracker::recordEvent(memoryEvent* _memEvent)
    {
        //record a copy;
        m_recordingOneFrame.push_back(*_memEvent);
    }

    void threadAllocatorTracker::pushEvent(memoryEvent* _memEvent)
    {
        ITF_ASSERT(0);
    }

    bbool threadAllocatorTracker::eventSelected(memoryEvent* _event)
    {
        if (m_eventTracker.size())
        {
            if (m_eventTracker[m_currentPos] == _event->m_Tag)
            {
                m_currentPos++;
                if (m_currentPos == m_eventTracker.size())
                {
                    m_captureContextTag = _event->m_CurrentContextTag;
                    m_currentPos = 0;
                    m_eventTracker.clear();
                    //start recording...
                }
                return btrue;
            }

        }

        return bfalse;
    }


    void threadAllocatorTracker::popBackEvent(memoryEvent* _event,u64 _currentcontext)
    {
       ITF_ASSERT(0);
    }

void threadAllocatorTracker::enableEvents(const SafeArray<u32>& _events)
{
    m_eventTracker  = _events;
    m_currentPos    = 0;
}


void threadAllocatorTracker::flushFrameAllocation()
{
    csAutoLock cs(m_cs);

    if (m_trackFrameRequested)
    {
        m_trackFrameEnable = btrue;
        m_trackFrameRequested = bfalse; 
    }

}
#ifdef ITF_WINDOWS
#ifdef _DEBUG
#pragma optimize ("g", on) 
#endif //_DEBUG
#endif // ITF_WINDOWS

void threadAllocatorTracker::addAllocation(ITF_AllocatorAttributes attributes, uPtr _size, uPtr _memoryAddress, u_long _crc, TraceInfo& _trace)
{
    if (!m_started)
        return;

    u64 memchk = MAKE_MEMCHUNCK(_size, _crc);

    csAutoLock cs(m_cs);
    ++m_CountAllocation;
    /*
#ifdef ITF_X360
    if (attributes.dwMemoryType == XALLOC_MEMTYPE_HEAP)
    {
        DWORD allowAttribute = *( DWORD* )& attributes;
        XSetAttributesOnHeapAlloc((void*) _memoryAddress,allowAttribute);
    }
#endif //ITF_X360
    **/

    /* debug purpose
    u32 ss  = GET_SIZECHUNCK(memchk);
    u32 cc  = GET_CRCCHUNCK(memchk);
    */

    const u8 allocID = attributes.dwAllocatorId;
    if (m_trackFrameEnable && (m_trackingAllocatorID == U32_INVALID || m_trackingAllocatorID == allocID)) 
    {
        frameAllocationTracking allocTracking;
        allocTracking.alive          = btrue;
        allocTracking.attributes     = _trace.m_attributes;
        allocTracking.crc            = _crc;
        allocTracking.size           = _size;
        m_allocationFrame.push_back(allocTracking);
    }

    mapMemoryTracker<u_long,TraceInfo*>::iterator iterCallstack =  m_callstack.find(_crc);

    if (iterCallstack == m_callstack.end())
    {
        TraceInfo* pT = (TraceInfo*)::malloc(sizeof(TraceInfo));
        pT->m_attributes = _trace.m_attributes;
        ITF_Memcpy(pT->m_stack, _trace.m_stack, sizeof(pT->m_stack[0]) * MAX_STACK_DEPTH);
        m_callstack[_crc] = pT;
    }

    UserPair* pair = m_allocationMap.AddPair((u16)(_memoryAddress>>16),(u16)(_memoryAddress&0x0000FFFF));
    pair->mUserData = memchk;
}

bbool threadAllocatorTracker::removeAllocation(uPtr _memoryAddress,u32 _dwAllocAttributes)
{
    //fast eject for Physical Resource
#ifdef ITF_X360
    if (_memoryAddress>=GPU_CPU_VIRTUAL_ADDRESS)
    {
        return bfalse;
    }
#endif //ITF_X360

    csAutoLock cs(m_cs);
    u64 userData;
    if (m_allocationMap.RemovePair((u16)(_memoryAddress>>16),(u16)(_memoryAddress&0x0000FFFF),userData))
    {
        return btrue;
    }
  
    return bfalse;

}

}

#endif //ITF_TRACKING_MEMORY_ENABLE
