#include "precompiled_ITF_ThreadManager.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#include "core/system/Synchronize.h"

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{

ITF_ThreadManager::ITF_ThreadManager()
{
    Synchronize::createCriticalSection(&m_csThreadCreation);
    ThreadSettings::m_settings[eThreadId_mainThread].m_threadID = Synchronize::getCurrentThreadId();
}


ITF_ThreadManager::~ITF_ThreadManager()
{
#ifndef ITF_WII
    //We are exiting the application with thread pending;please kill them before exiting
    if (m_vThreads.size())
    {
        dumpThreadsActivity();
        //ITF_ASSERT(0);
    }
#endif //ITF_WII

    Synchronize::destroyCriticalSection(&m_csThreadCreation);
}

#define SETNAME(val) case val: {return #val;}

const String getPriorityName(Thread_Priority _priority)
{
    switch (_priority)
    {
        SETNAME(Thread_Priority_Idle);
        SETNAME(Thread_Priority_Lowest);
        SETNAME(Thread_Priority_BelowNormal);
        SETNAME(Thread_Priority_Normal);
        SETNAME(Thread_Priority_AboveNormal);
        SETNAME(Thread_Priority_Highest);
        SETNAME(Thread_Priority_TimeCritical);
    default:
        ITF_ASSERT(0);
        break;
    };

    ITF_ASSERT(0);
    return "";
}


const String getAffinityName(Thread_HardwareThreadId _priority)
{
    switch (_priority)
    {
        SETNAME(Thread_HardwareThread0);
        SETNAME(Thread_HardwareThread1);
        SETNAME(Thread_HardwareThread2);
        SETNAME(Thread_HardwareThread3);
        SETNAME(Thread_HardwareThread4);
        SETNAME(Thread_HardwareThread5);
        SETNAME(Thread_HardwareThread6);
        SETNAME(Thread_HardwareThread7);
        default:
            ITF_ASSERT(0);
        break;
    };

    ITF_ASSERT(0);
    return "";
}

void ITF_ThreadManager::dumpThreadsActivity()
{
    for (ITF_VECTOR<Thread*>::const_iterator iter = m_vThreads.begin();iter!=m_vThreads.end();++iter)
    {
       const Thread* pThread = (*iter);

       LOG("----------------------------------------------------------------------");
       LOG("Dump Thread Activity\n");
       LOG("%25s %25s %25s","-----Name-----","-----Priority-----","-----Affinity-----");
       const String szPrioName      = getPriorityName(pThread->getPriority());
       const String szAffinityName  = getAffinityName(pThread->getAffinity());       
       LOG("%25s %25S %25S",pThread->getSettings()->m_name,szPrioName.cStr(),szAffinityName.cStr());
       LOG("\n----------------------------------------------------------------------");
    }
}

void ITF_ThreadManager::unregisterThread(Thread* _pThread)
{
    csAutoLock cs(m_csThreadCreation);
    ITF_VECTOR<Thread*>::iterator iter = std::find(m_vThreads.begin(),m_vThreads.end(),_pThread);
    
    if (iter!=m_vThreads.end())
    {
        m_vThreads.erase(iter);
        SF_DEL(_pThread);
    }
    else
    {
       ITF_ASSERT(0);
    }
}

#ifdef ITF_WII
void ITF_ThreadManager::CheckActiveThreads()
{
    #ifndef ITF_FINAL
    OSCheckActiveThreads();
    #endif // ITF_FINAL

    // Remove one thread at a time
    csAutoLock cs(m_csThreadCreation);
    for (ITF_VECTOR<Thread*>::iterator iter = m_vThreads.begin() ; iter!=m_vThreads.end() ; ++iter)
    {
        Thread* pThread = (*iter);

        if(!pThread->isActive())
        {
            m_vThreads.erase(iter);
            SF_DEL(pThread);
            break;
        }
    }
}
#endif // ITF_WII

Thread* ITF_ThreadManager::createThread(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
{
    csAutoLock cs(m_csThreadCreation);
    Thread* pThread = newAlloc(mId_System,Thread());
    pThread->create(pThreadFunc, pThreadFuncParam, _settings);
    m_vThreads.push_back(pThread);
    //Register the thread
    return pThread;
}


}
