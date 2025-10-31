#ifndef _ITF_THREADMANAGER_H_
#define _ITF_THREADMANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_


namespace ITF
{
    class ThreadManager:public TemplateSingleton<ThreadManager>
    {
    public:
        
        ThreadManager(){};

        virtual ~ThreadManager() {};

        //Dump all thread created by itself
        virtual void dumpThreadsActivity() = 0;

        virtual void unregisterThread(Thread* _pThread) = 0;

        virtual Thread* createThread(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings) = 0;

#ifdef ITF_WII
        virtual void    CheckActiveThreads() = 0;
#endif // ITF_WII

    };

#define THREADMANAGER ThreadManager::getptr()

} //namespace ITF



#endif  //_ITF_FILEMANAGER_H_