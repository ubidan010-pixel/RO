#include "precompiled_core.h"

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

namespace ITF
{
    #define GetCS(cs)   (cs)

    void Synchronize::createCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        cs->Initialize();
    }

    void Synchronize::destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        cs->Finalize();
    }

    void Synchronize::enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        cs->Enter();
    }

    void Synchronize::leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        cs->Leave();
    }

    void Synchronize::createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * /*szName*/, u32 uInitialCount, u32 uMaximunCount)
    {
        pSemaphore->Initialize((s32)uInitialCount, (s32)uMaximunCount);
    }

    void Synchronize::increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 cpt)
    {
        ITF_ASSERT(cpt==1);
        pSemaphore->Acquire();
    }

    void Synchronize::destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore)
    {
        pSemaphore->Finalize();
    }

    void Synchronize::createEvent(ITF_THREAD_EVENT *evt, char *szName, bbool bAutoReset)
    {
        evt->Initialize(!bAutoReset);
    }

    void Synchronize::resetEvent(ITF_THREAD_EVENT *evt)
    {
        evt->ClearSignal();
    }

    void Synchronize::setEvent(ITF_THREAD_EVENT *evt)
    {
        ITF_ASSERT(evt);
        evt->Signal();
    }

    void Synchronize::destroyEvent(ITF_THREAD_EVENT *evt)
    {
        evt->Finalize();
    }

    void Synchronize::waitEvent(ITF_THREAD_EVENT* evt)
    {
        ITF_ASSERT(evt);
        evt->Wait();
    }

    ITF_THREAD_ID Synchronize::getCurrentThreadId(void)
    {
        return nn::os::Thread::GetCurrentId();
    }
}



