#include "precompiled_core.h"

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_


#ifndef _ITF_VLDMANAGER_H_
#include "core/vldManager.h"
#endif //_ITF_VLDMANAGER_H_

namespace ITF
{


    TLS ThreadLocalStorage ThreadInfo::m_threadEngineIndex ;


    i32 Thread::getAffinityHW(Thread_HardwareThreadId _affinity)
    {
        return NN_OS_CORE_NO_USE_PROCESS_VALUE;
    }

    i32 Thread::getPriorityHW(Thread_Priority ePriority)
    {
        i32 iPriority;
        switch ( ePriority )
        {
            case Thread_Priority_Idle:
                iPriority = NN_OS_LOWEST_THREAD_PRIORITY;
                break;
            case Thread_Priority_Lowest:
                iPriority = NN_OS_LOWEST_THREAD_PRIORITY-1;
                break;
            case Thread_Priority_BelowNormal:
                iPriority = NN_OS_LOWEST_THREAD_PRIORITY-2;
                break;
            case Thread_Priority_AboveNormal:
                iPriority = NN_OS_DEFAULT_THREAD_PRIORITY-1;
                break;
            case Thread_Priority_Highest:
                iPriority = NN_OS_DEFAULT_THREAD_PRIORITY-2;
                break;
            case Thread_Priority_TimeCritical:
                iPriority = NN_OS_HIGHEST_THREAD_PRIORITY;
                break;
            case Thread_Priority_Normal:
            default:
                iPriority = NN_OS_DEFAULT_THREAD_PRIORITY;
                break;
        }
       
        return iPriority;
    }

    void funcCaller(void* user)
    {
        sFuncParameter* pFuncParameter = (sFuncParameter*) user;
        if (pFuncParameter->m_engineIndex != U32_INVALID)
        {
            if (ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_useTLS)
                ThreadInfo::m_threadEngineIndex.setValue(pFuncParameter->m_engineIndex);
            ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_threadID = Synchronize::getCurrentThreadId();
        }

        ((u32 (*)(void*)) (pFuncParameter->m_pThreadFunc))(pFuncParameter->m_pThreadFuncParam);
        Thread* pThread = (Thread*)pFuncParameter->m_pThread;
        THREADMANAGER->unregisterThread(pThread);
        NN_LOG("----------WARNING : TODO : m_systhread/m_stack OBJECTs ARE NOT DELETED YET \n");
    }

    void Thread::create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
    {
        setSettings(&_settings);

        m_Parameter.m_pThreadFunc = pThreadFunc;
        m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
        m_Parameter.m_pThread = this;
        m_Parameter.m_engineIndex = _settings.m_engineIndex;         

        ITF_ASSERT(!m_systhread);
        m_systhread = new nn::os::Thread();
        m_stack = new nn::os::StackMemoryBlock(65536);

        m_systhread->Start<void*,void*,nn::os::StackMemoryBlock>(&funcCaller, &m_Parameter, *m_stack, getPriorityHW(_settings.m_priority), getAffinityHW(_settings.m_hwId));
        m_Handle = m_systhread->GetHandle();

        ITF_ASSERT(m_Handle.IsValid());
    }


    void Thread::setPriority(Thread_Priority ePriority)
    {
        m_ePriority = ePriority;
        //m_systhread.ChangeCurrentPriority(getPriorityHW(ePriority);
    }

    void Thread::setAffinity(Thread_HardwareThreadId eThreadId)
    {
        m_eThreadAffinity = eThreadId;
        //SetThreadIdealProcessor( m_Handle, getAffinityHW(eThreadId) );
    }
    

} // namespace ITF

