#include "precompiled_core.h"

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

namespace ITF
{
    ThreadLocalStorage ThreadInfo::m_threadEngineIndex;

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


    u32 WINAPI funcCaller(void* user)
    {
        sFuncParameter* pFuncParameter = (sFuncParameter*) user;
        ThreadInfo::m_threadEngineIndex.setValue(pFuncParameter->m_engineIndex);
        if (pFuncParameter->m_engineIndex != U32_INVALID)
        {
            if (ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_useTLS)
                ThreadInfo::m_threadEngineIndex.setValue(pFuncParameter->m_engineIndex);
            
            ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_threadID = Synchronize::getCurrentThreadId();
        }
        u32 res = ((LPTHREAD_START_ROUTINE)(pFuncParameter->m_pThreadFunc))(pFuncParameter->m_pThreadFuncParam);
        THREADMANAGER->unregisterThread((Thread*)pFuncParameter->m_pThread);
        return res;
    }
#ifdef _DEBUG
    typedef struct tagTHREADNAME_INFO {
        DWORD dwType;     // Must be 0x1000
        LPCSTR szName;    // Pointer to name (in user address space)
        DWORD dwThreadID; // Thread ID (-1 for caller thread)
        DWORD dwFlags;    // Reserved for future use; must be zero
    } THREADNAME_INFO;

    void SetThreadName(  LPCSTR szThreadName )
    {
        THREADNAME_INFO info;

        info.dwType = 0x1000;
        info.szName = szThreadName;
        info.dwThreadID = U32_INVALID;
        info.dwFlags = 0;

        __try
        {
            RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD *)&info );
        }
        __except( GetExceptionCode()==0x406D1388 ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_EXECUTE_HANDLER )
        {
        }
    }
#endif //_DEBUG

    void Thread::create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
    {
        setSettings(&_settings);
        m_Parameter.m_pThreadFunc = pThreadFunc;
        m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
        m_Parameter.m_pThread = this;
        m_Parameter.m_engineIndex = _settings.m_engineIndex;

        m_Handle = ::CreateThread( NULL,_settings.m_stackSize, (LPTHREAD_START_ROUTINE)funcCaller,&m_Parameter,CREATE_SUSPENDED,&m_Index);

        ITF_ASSERT(m_Handle!=NULL);

#ifdef _DEBUG
        SetThreadName((LPCSTR)_settings.m_name);
#endif // DEBUG

        setPriority(_settings.m_priority);

        setAffinity(_settings.m_hwId);

        ResumeThread( m_Handle );
    }



    void Thread::setPriority(Thread_Priority ePriority)
    {   
        m_ePriority = ePriority;
        SetThreadPriority( m_Handle, getPriorityHW(ePriority) );
    }

    void Thread::setAffinity(Thread_HardwareThreadId eThreadId)
    {
        m_eThreadAffinity = eThreadId;
        XSetThreadProcessor( m_Handle, eThreadId );
    }

} // namespace ITF

