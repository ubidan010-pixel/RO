#include "precompiled_core.h"

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef _ITF_AUTOLOCK_H_
#include "core/system/autolock.h"
#endif //_ITF_AUTOLOCK_H_

namespace ITF
{
    #define GetCS(cs)   (cs)

    void Synchronize::createCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
		pthread_mutex_init(cs, NULL);
    }

    void Synchronize::destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
		pthread_mutex_destroy(cs);
    }

    void Synchronize::enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
		pthread_mutex_lock(cs);
    }

    void Synchronize::leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
		pthread_mutex_unlock(cs);
    }

    void Synchronize::createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName, u32 uInitialCount, u32 uMaximunCount)
    {
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
    }

    void Synchronize::increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 cpt)
    {
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
    }

    void Synchronize::destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore)
    {
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
    }

    void Synchronize::createEvent(ITF_THREAD_EVENT *evt, char *szName)
    {
		pthread_mutex_init(&evt->_mutex, NULL);
		
		pthread_condattr_t cond_attr;
		pthread_condattr_init(&cond_attr);
		pthread_cond_init(&evt->_cond, &cond_attr);
    }

    void Synchronize::resetEvent(ITF_THREAD_EVENT *evt)
    {
		destroyEvent(evt);
		createEvent(evt, NULL);
    }

    void Synchronize::setEvent(ITF_THREAD_EVENT *evt)
    {
		pthread_cond_signal(&evt->_cond);
    }

    void Synchronize::destroyEvent(ITF_THREAD_EVENT *evt)
    {
		pthread_cond_destroy(&evt->_cond);
    }

    void Synchronize::waitEvent(ITF_THREAD_EVENT* pEvent)
    {
		pthread_cond_wait(&pEvent->_cond, &pEvent->_mutex);
    }

    ITF_THREAD_ID Synchronize::getCurrentThreadId(void)
    {
		return 0;
    }
}



