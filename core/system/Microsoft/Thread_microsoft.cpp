#include "precompiled_core.h"

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_VLDMANAGER_H_
#include "core/vldManager.h"
#endif //_ITF_VLDMANAGER_H_

#include "core/UnicodeTools.h"

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
        vldManager::apply();

        sFuncParameter* pFuncParameter = (sFuncParameter*) user;
        if (pFuncParameter->m_engineIndex != U32_INVALID)
        {
            if (ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_useTLS)
            {
                ThreadInfo::m_threadEngineIndex.setValue(pFuncParameter->m_engineIndex);
                ITF_ASSERT_CRASH( ThreadInfo::m_threadEngineIndex.getValue()==pFuncParameter->m_engineIndex,"TLS assignation failed ");
            }
            else
            {
                ITF_ASSERT(0);
            }

            ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_threadID = Synchronize::getCurrentThreadId();
        }

        u32 res = ((LPTHREAD_START_ROUTINE)(pFuncParameter->m_pThreadFunc))(pFuncParameter->m_pThreadFuncParam);
        Thread* pThread = (Thread*)pFuncParameter->m_pThread;
        THREADMANAGER->unregisterThread(pThread);
        return res;
    }
    

    bool Thread::SetThreadName(THREAD_HANDLE dwThreadID, const char* szThreadName)
    {
        // This is a best effort function.
        // We do not assume the function is available in windows.h
        // We search it at first run in Kernel32.dll and assign the static.
        // If not found, we just ignore the set and return false.
        using SetThreadDescriptionFunc = HRESULT(WINAPI*)(HANDLE hThread, PCWSTR lpThreadDescription);
        static auto setThreadDescriptionFunc = reinterpret_cast<SetThreadDescriptionFunc>(::GetProcAddress(::GetModuleHandle(L"Kernel32.dll"), "SetThreadDescription"));

        if (setThreadDescriptionFunc)
        {
            setThreadDescriptionFunc(dwThreadID, UTF8ToUTF16(szThreadName).getAsWChar());
            return true;
        }

        return false;
    }

    bool Thread::SetCurrentThreadName(const char* szThreadName)
    {
        return SetThreadName(::GetCurrentThread(), szThreadName);
    }

    void Thread::create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
    {
        setSettings(&_settings);
        m_Parameter.m_pThreadFunc = pThreadFunc;
        m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
        m_Parameter.m_pThread = this;
        m_Parameter.m_engineIndex = _settings.m_engineIndex;
         
        m_Handle = ::CreateThread( NULL, _settings.m_stackSize, (LPTHREAD_START_ROUTINE)funcCaller,&m_Parameter,CREATE_SUSPENDED,&m_Index);


        ITF_ASSERT(m_Handle!=NULL);

        setPriority(_settings.m_priority);

        setAffinity(_settings.m_hwId);

        SetThreadName(m_Handle, _settings.m_name);

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
        SetThreadIdealProcessor( m_Handle, eThreadId );
    }
    

} // namespace ITF

