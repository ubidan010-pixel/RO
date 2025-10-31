#include "precompiled_core.h"

#include "core/system/thread.h"

#include <kernel.h>
#include "core/AdaptersInterfaces/ThreadManager.h"

namespace ITF
{

    // Should probably be moved to a more appropriate place
    ThreadLocalStorage ThreadInfo::m_threadEngineIndex;

    __thread void * ThreadLocalStorage::ms_threadLocalStorage[ThreadLocalStorage::Max_TLS_Slots + 1];
    volatile int64_t ThreadLocalStorage::ms_currentIndex = 0;

    // utilities
    void sleep(u32 uMilliseconds)
    {
        if (uMilliseconds == 0)
            scePthreadYield();
        else
            sceKernelUsleep(uMilliseconds * 1000); //translate in micro;
    }

    void YieldProcessor()
    {
        scePthreadYield();
    }


    #define THREAD_PRIORITY_IDLE			(SCE_KERNEL_PRIO_FIFO_LOWEST + 100)
    #define THREAD_PRIORITY_LOWEST			SCE_KERNEL_PRIO_FIFO_LOWEST
    #define THREAD_PRIORITY_BELOW_NORMAL	(SCE_KERNEL_PRIO_FIFO_LOWEST + SCE_KERNEL_PRIO_FIFO_DEFAULT) / 2
    #define THREAD_PRIORITY_NORMAL			SCE_KERNEL_PRIO_FIFO_DEFAULT
    #define THREAD_PRIORITY_ABOVE_NORMAL	(SCE_KERNEL_PRIO_FIFO_DEFAULT + SCE_KERNEL_PRIO_FIFO_HIGHEST) / 2
    #define THREAD_PRIORITY_HIGHEST			SCE_KERNEL_PRIO_FIFO_HIGHEST
    #define THREAD_PRIORITY_TIME_CRITICAL	0
    
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

    u64 Thread::getAffinityHW(Thread_HardwareThreadId eAffinity)
    {
        if (eAffinity == Thread_HardwareThread_Unknown)
            return SCE_KERNEL_CPUMASK_13CPU; // 6 cores available
        u64 coreNum = u64(eAffinity - Thread_HardwareThread0);
        // Basic assignment rules. See 5. CPU Management in the documentation.
        // Test should be done by returning SCE_KERNEL_CPUMASK_13CPU and let the OS do its job.
        // Repeat for each core above the number of HW threads.
        coreNum = coreNum % 13;
        // Favor HW threads with parity first, then odd HW threads.        
        if (coreNum <= 6)
            return u64(1) << (coreNum * 2); // 0, 2, 4, 6, 8, 10, 12
        else
            return u64(1) << ((coreNum - 7) * 2 + 1); // 1, 3, 5, 7, 9, 11
    }

    static void * funcCaller(void * parameter)
    {
        FuncParameter* pFuncParameter = reinterpret_cast<FuncParameter*>(parameter);
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
        return reinterpret_cast<void*>(static_cast<uPtr>(res));
    }


    void Thread::create(FuncParameter::PThreadFunc pThreadFunc, void *pThreadFuncParam, const ThreadSettings& _settings)
    {
        ITF_ASSERT(m_handle == NULL);

        setSettings(_settings);
        
        m_Parameter.m_pThreadFunc = pThreadFunc;
        m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
        m_Parameter.m_pThread = this;
        m_Parameter.m_engineIndex = _settings.m_engineIndex;

        m_ePriority = _settings.m_priority;

        if (m_eThreadAffinity == Thread_HardwareThread_Unknown)
            m_eThreadAffinity = _settings.m_hwId;

        ScePthreadAttr thread_atributes;
        ITF_VERIFY_SCE_CALL(scePthreadAttrInit(&thread_atributes));
        ITF_VERIFY_SCE_CALL(scePthreadAttrSetstacksize(&thread_atributes, _settings.m_stackSize));
        ITF_VERIFY_SCE_CALL(scePthreadAttrSetaffinity(&thread_atributes,  getAffinityHW(m_eThreadAffinity)));
        ITF_VERIFY_SCE_CALL(scePthreadAttrSetdetachstate(&thread_atributes, SCE_PTHREAD_CREATE_DETACHED));
        ITF_VERIFY_SCE_CALL(scePthreadAttrSetinheritsched(&thread_atributes, SCE_PTHREAD_EXPLICIT_SCHED));
        SceKernelSchedParam schedParam;
        schedParam.sched_priority = getPriorityHW(m_ePriority);
        ITF_VERIFY_SCE_CALL(scePthreadAttrSetschedparam(&thread_atributes, &schedParam));

        char truncatedName[32];
        strncpy(truncatedName, _settings.m_name, ITF_ARRAY_SIZE(truncatedName));
        truncatedName[ITF_ARRAY_SIZE(truncatedName)-1] = 0;

        ITF_VERIFY_SCE_CALL(scePthreadCreate(&m_handle, &thread_atributes, funcCaller, (void*)&m_Parameter, truncatedName));
        ITF_ASSERT(m_handle != nullptr);

        ITF_VERIFY_SCE_CALL(scePthreadAttrDestroy(&thread_atributes));
    }

    void Thread::setAffinity(Thread_HardwareThreadId eThreadId)
    {
        m_eThreadAffinity = eThreadId;
        if (m_handle != nullptr)
        {
            ITF_VERIFY_SCE_CALL(scePthreadSetaffinity(m_handle, getAffinityHW(eThreadId)));
        }
    }

} // namespace ITF

