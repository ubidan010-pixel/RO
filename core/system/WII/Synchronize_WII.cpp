#include "precompiled_core.h"

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

namespace ITF
{
    #define GetCS(cs)   (cs)

	volatile bbool Synchronize::bg_Interrupt = bfalse;

    void Synchronize::createCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
		OSInitMutex(cs);
    }

    void Synchronize::destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
    }

    void Synchronize::enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        OSLockMutex(cs);
    }

    void Synchronize::leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        OSUnlockMutex(cs);
    }

    void Synchronize::createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * /*szName*/, u32 uInitialCount, u32 uMaximunCount)
    {
        OSInitSemaphore(pSemaphore, (s32)uInitialCount);
    }

    void Synchronize::increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount)
    {
		while(uCount--)
			OSSignalSemaphore(pSemaphore);
    }

	void Synchronize::waitSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount)
	{
		while(uCount--)
			OSWaitSemaphore(pSemaphore);
	}

    void Synchronize::destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore)
    {
    }

    void Synchronize::createEvent(ITF_THREAD_EVENT *evt, char *szName, bbool bAutoReset)
    {
        evt->init(szName, bAutoReset);
    }

    void Synchronize::resetEvent(ITF_THREAD_EVENT *evt)
    {
        evt->reset();
    }

    void Synchronize::setEvent(ITF_THREAD_EVENT *evt)
    {
        evt->set();
    }

    void Synchronize::destroyEvent(ITF_THREAD_EVENT *evt)
    {
        evt->destroy();
    }

    void Synchronize::waitEvent(ITF_THREAD_EVENT* evt)
    {
        evt->wait();
    }

    ITF_THREAD_ID Synchronize::getCurrentThreadId(void)
    {
		return (ITF_THREAD_ID)(bg_Interrupt ? -1 : OSGetCurrentThread());
    }

	bbool Synchronize::IsInterupt()
	{
	    return bg_Interrupt;
	}
	
	bbool Synchronize::SetInterupt(bbool _int)
	{
	    bbool old = bg_Interrupt;
	    bg_Interrupt = _int;
	    return old;
	}
    
    void ITF_THREAD_EVENT::init(char *szName, bbool bAutoReset)
    {
		OSInitCond(&m_cond);
		OSInitMutex(&m_mutex);
        m_signaled  = btrue;
        m_autoreset = bAutoReset;
    }

    void ITF_THREAD_EVENT::set()
    { 
    	OSLockMutex(&m_mutex);
    	m_signaled = btrue;
   		OSSignalCond(&m_cond);
    	OSUnlockMutex(&m_mutex);
    }

    void ITF_THREAD_EVENT::reset()
    { 
		OSLockMutex(&m_mutex);
		m_signaled = bfalse;
		OSUnlockMutex(&m_mutex);
    }

    void ITF_THREAD_EVENT::wait()
    {
		OSLockMutex(&m_mutex);

		while (!m_signaled)
		{
			OSWaitCond(&m_cond, &m_mutex);
		}

		if(m_autoreset)
			m_signaled = bfalse;

		OSUnlockMutex(&m_mutex);   
    }    
}
