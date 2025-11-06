#include "precompiled_core.h"

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

namespace ITF
{
    #define GetCS(cs)   (cs)

    void Synchronize::createCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ::InitializeCriticalSection(GetCS(cs));
    }

    void Synchronize::destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ::DeleteCriticalSection(GetCS(cs));
    }

    void Synchronize::enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ::EnterCriticalSection(GetCS(cs));
    }

    bool Synchronize::tryEnterCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs)
    {
        return 0 != ::TryEnterCriticalSection(GetCS(cs));
    }

    void Synchronize::leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ::LeaveCriticalSection(GetCS(cs));
    }

    void Synchronize::createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName, u32 uInitialCount, u32 uMaximunCount)
    {
        *pSemaphore = ::CreateSemaphoreA(NULL, uInitialCount, uMaximunCount, szName);
    }

    void Synchronize::increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 cpt)
    {
        ::ReleaseSemaphore(*pSemaphore, cpt, NULL);
    }

    void Synchronize::destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore)
    {
        ::CloseHandle(*pSemaphore);
    }

    void Synchronize::createEvent(ITF_THREAD_EVENT *evt, char *szName, bbool bAutoReset)
    {
        *evt = CreateEventA(NULL, (bAutoReset?FALSE:TRUE), FALSE, NULL);
        ITF_ASSERT(GetLastError()!=ERROR_ALREADY_EXISTS);
    }

    void Synchronize::resetEvent(ITF_THREAD_EVENT *evt)
    {
        ITF_VERIFY(ResetEvent(*evt));
    }

    void Synchronize::setEvent(ITF_THREAD_EVENT *evt)
    {
        ITF_ASSERT(evt);
        ITF_ASSERT(*evt);
        SetEvent(*evt);
    }

    void Synchronize::destroyEvent(ITF_THREAD_EVENT *evt)
    {
        CloseHandle(*evt);
    }

    void Synchronize::waitEvent(ITF_THREAD_EVENT* evt)
    {
        ITF_ASSERT(evt);
        ITF_ASSERT(*evt);
        WaitForSingleObject(*evt, INFINITE);
    }

    ITF_THREAD_ID Synchronize::getCurrentThreadId(void)
    {
        return ::GetCurrentThreadId();
    }
}



