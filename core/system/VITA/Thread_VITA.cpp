#include "precompiled_core.h"

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

//#include <sys/ppu_thread.h>

namespace ITF
{
    ThreadLocalStorage ThreadInfo::m_threadEngineIndex;


    #define THREAD_PRIORITY_IDLE 3300
    #define THREAD_PRIORITY_LOWEST 2500	
    #define THREAD_PRIORITY_BELOW_NORMAL 2000
    #define THREAD_PRIORITY_NORMAL 1001
    #define THREAD_PRIORITY_ABOVE_NORMAL 1000
    #define THREAD_PRIORITY_HIGHEST 500
    #define THREAD_PRIORITY_TIME_CRITICAL 100
    
    u32 Thread::getPriorityHW(Thread_Priority ePriority)
    {
        i32 iPriority;
        switch ( ePriority )
        {
        case Thread_Priority_Idle:
            iPriority = THREAD_PRIORITY_IDLE;
            break;
        case Thread_Priority_Lowest:
            iPriority = THREAD_PRIORITY_LOWEST;
            break;
        case Thread_Priority_BelowNormal:
            iPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case Thread_Priority_AboveNormal:
            iPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case Thread_Priority_Highest:
            iPriority = THREAD_PRIORITY_HIGHEST;
            break;
        case Thread_Priority_TimeCritical:
            iPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
        case Thread_Priority_Normal:
        default:
            iPriority = THREAD_PRIORITY_NORMAL;
            break;
        }

        return iPriority;
    }

    typedef DWORD (*PTHREAD_START_ROUTINE)(void* lpThreadParameter);

    typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

    static void funcCaller( uint64_t lpParameter )
    {/*
        sFuncParameter* pFuncParameter = (sFuncParameter*) lpParameter;
        ThreadInfo::m_threadEngineIndex = pFuncParameter->m_engineIndex;
        ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_threadID = Synchronize::getCurrentThreadId();
        ((LPTHREAD_START_ROUTINE)(pFuncParameter->m_pThreadFunc))(pFuncParameter->m_pThreadFuncParam);
        THREADMANAGER->unregisterThread((Thread*)pFuncParameter->m_pThread);
        sys_ppu_thread_exit(0);*/
    }


    void Thread::create(u32 (*pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
    {/*
        sys_ppu_thread_t ppu_thread;

        setSettings(&_settings);
        
        m_Parameter.m_pThreadFunc = (void*)pThreadFunc;
        m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
        m_Parameter.m_pThread = this;
        m_Parameter.m_engineIndex = _settings.m_engineIndex;

        m_ePriority =  _settings.m_priority;

        i32 ret = sys_ppu_thread_create(&ppu_thread,funcCaller, (uint64_t) &m_Parameter, getPriorityHW(m_ePriority), _settings.m_stackSize,0 , _settings.m_name);
        if( ret != CELL_OK)
        {   
            ITF_ASSERT(0);
        }

        sys_ppu_thread_yield();*/
    }

} // namespace ITF

