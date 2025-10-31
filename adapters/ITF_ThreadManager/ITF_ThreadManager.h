#ifndef _ITF_ITF_THREADMANAGER_H_
#define _ITF_ITF_THREADMANAGER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_


namespace ITF
{
    
class ITF_ThreadManager : public ThreadManager
{
public:
    ITF_ThreadManager();
    virtual ~ITF_ThreadManager();

    virtual Thread* createThread(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings);
    virtual void dumpThreadsActivity();
    virtual void unregisterThread(Thread* _pThread);

#ifdef ITF_WII
    virtual void    CheckActiveThreads();
#endif // ITF_WII

protected:

    ITF_VECTOR<Thread*> m_vThreads;

    ITF_THREAD_CRITICAL_SECTION m_csThreadCreation;
};


} //namespace ITF


#endif  //_ITF_ITF_THREADMANAGER_H_