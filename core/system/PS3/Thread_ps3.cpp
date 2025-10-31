#include "precompiled_core.h"

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#include <sys/ppu_thread.h>

namespace ITF
{
    ThreadLocalStorage ThreadInfo::m_threadEngineIndex;

    __thread void * ThreadLocalStorage::m_threadLocalStorage[ThreadLocalStorage::Max_TLS_Slots];
    u32 ThreadLocalStorage::m_currentIndex = 0;

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

    static void funcCaller( uint64_t lpParameter )
    {
        sFuncParameter* pFuncParameter = reinterpret_cast<sFuncParameter*>(lpParameter);
        if (pFuncParameter->m_engineIndex != U32_INVALID)
        {
            if (ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_useTLS)
            {
                ThreadInfo::m_threadEngineIndex.setValue(pFuncParameter->m_engineIndex);
            }
            ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_threadID = Synchronize::getCurrentThreadId();
        }
        u32 res = (*pFuncParameter->m_pThreadFunc)(pFuncParameter->m_pThreadFuncParam);
        THREADMANAGER->unregisterThread((Thread*)pFuncParameter->m_pThread);
        sys_ppu_thread_exit(res);
    }


    void Thread::create(u32 (*pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
    {
        sys_ppu_thread_t ppu_thread;

        setSettings(&_settings);
        
        m_Parameter.m_pThreadFunc = pThreadFunc;
        m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
        m_Parameter.m_pThread = this;
        m_Parameter.m_engineIndex = _settings.m_engineIndex;

        m_ePriority =  _settings.m_priority;

        i32 ret = sys_ppu_thread_create(&ppu_thread, funcCaller, reinterpret_cast<uint64_t>(&m_Parameter), getPriorityHW(m_ePriority), _settings.m_stackSize, 0, _settings.m_name);
        if( ret != CELL_OK)
        {   
            ITF_ASSERT(0);
        }

        sys_ppu_thread_yield();
    }

} // namespace ITF

