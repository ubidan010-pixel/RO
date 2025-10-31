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

#include "core/memory/WII/MEM_consts_WII.h"
#include "core/memory/WII/MEM_macros_WII.h"

namespace ITF
{
	ThreadLocalStorage ThreadInfo::m_threadEngineIndex;
	Thread_LocalStorageManager* Thread_LocalStorageManager::mp_Instance = NULL;
	Thread_LocalStorageManager  Thread_LocalStorageManager::m_Instance;

    #define THREAD_PRIORITY_IDLE 31//OS_PRIORITY_IDLE
    #define THREAD_PRIORITY_LOWEST 30//OS_PRIORITY_MAX
    #define THREAD_PRIORITY_BELOW_NORMAL 17//20
    #define THREAD_PRIORITY_NORMAL 16
    #define THREAD_PRIORITY_ABOVE_NORMAL 15//10
    #define THREAD_PRIORITY_HIGHEST 1//OS_PRIORITY_MIN
    #define THREAD_PRIORITY_TIME_CRITICAL 0//OS_PRIORITY_MIN

    /*i32 Thread::getAffinityHW(Thread_HardwareThreadId _affinity)
    {
        return NN_OS_CORE_NO_USE_PROCESS_VALUE;
    }*/

    i32 Thread::getPriorityHW(Thread_Priority ePriority)
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

    void* funcCaller(void* user)
    {
		MEM_RegisterThread();

        sFuncParameter* pFuncParameter = (sFuncParameter*) user;

        if (pFuncParameter->m_engineIndex != U32_INVALID)
        {
            if(ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_useTLS)
                ThreadInfo::m_threadEngineIndex.setValue(pFuncParameter->m_engineIndex);
            ThreadSettings::m_settings[pFuncParameter->m_engineIndex].m_threadID = Synchronize::getCurrentThreadId();
        }

        ((u32 (*)(void*)) (pFuncParameter->m_pThreadFunc))(pFuncParameter->m_pThreadFuncParam);

		MEM_UnregisterThread();

        ((Thread*)pFuncParameter->m_pThread)->SetActive(bfalse);

        return 0;
    }

    Thread::~Thread()
    {
        if(!OSIsThreadTerminated(&m_systhread))
            OSCancelThread(&m_systhread);

        if(m_stack)
		{
			Memory::alignedFree(m_stack);
			m_stack = NULL;
		}
    }
    
    void Thread::create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
    {
        setSettings(&_settings);

        m_Parameter.m_pThreadFunc = pThreadFunc;
        m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
        m_Parameter.m_pThread = this;
        m_Parameter.m_engineIndex = _settings.m_engineIndex;         

        //ITF_ASSERT(!m_systhread);
                
		MEM_M_PushExt(MEM_C_MEM1_ALLOC);
		m_stack = (u8 *)Memory::alignedMallocCategory(sizeof(u8) * _settings.m_stackSize, 32, MemoryId::mId_System);
		MEM_M_PopExt();

        SetActive(btrue);

		BOOL bSuccess = OSCreateThread(
		&m_systhread, // pointer to the thread to initialize
		funcCaller, // pointer to the start routine
		&m_Parameter, // parameter passed to the start routine
		m_stack + _settings.m_stackSize, // initial stack address
		_settings.m_stackSize, // stack size
		getPriorityHW(_settings.m_priority), // scheduling priority - lowest
		OS_THREAD_ATTR_DETACH); // detached by default   
    
    	ITF_MemSet(mau32_LocalStorages, 0 , sizeof(u32) * Thread_LocalStorageManager::mcu32_MaxLocalStorageData);
    	m_systhread.specific[0] = &mau32_LocalStorages;
    	
    	// Starts the thread
		OSResumeThread(&m_systhread);
		//OSReport("start thread succes: %d", bSuccess);
    }

    void Thread::setPriority(Thread_Priority ePriority)
    {
    	m_ePriority = ePriority;
    }

    void Thread::setAffinity(Thread_HardwareThreadId eThreadId)
    {
    	m_eThreadAffinity = eThreadId;
    }

} // namespace ITF

