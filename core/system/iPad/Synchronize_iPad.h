#ifndef _ITF_SYNCHRONIZE_IPAD_H_
#define _ITF_SYNCHRONIZE_IPAD_H_

#include <pthread.h>

namespace ITF
{
	class ITF_THREAD_FASTMUTEX
	{
		public:    
			ITF_THREAD_FASTMUTEX() {m_lock = 0;}
			void lock() {} 
			void unlock() {}

		private:
			volatile long m_lock;
	};

	#define ITF_MemoryBarrier()				


	#define ITF_THREAD_SEMAPHORE			u32
	#define ITF_THREAD_ID					u32

	#define ITF_THREAD_CRITICAL_SECTION		pthread_mutex_t


    class ITF_THREAD_EVENT;

    class Synchronize
    {
        public:
			static ITF_THREAD_ID	getCurrentThreadId(void);
			static void				createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName, u32 uInitialCount= 0, u32 uMaximumCount= 666666);
			static void				increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount);
			static void				destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore);
			static void				createCriticalSection(ITF_THREAD_CRITICAL_SECTION * pCriticalSection);
			static void				destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);
			static void				enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);
			static void				leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);
			static void				createEvent(ITF_THREAD_EVENT * pEvent, char* szName);
			static void				resetEvent(ITF_THREAD_EVENT *evt);
			static void				setEvent(ITF_THREAD_EVENT *pEvent);
			static void				destroyEvent(ITF_THREAD_EVENT *pEvent);
			static void				waitEvent(ITF_THREAD_EVENT* pEvent);
    };

    class ITF_THREAD_EVENT
    {
		protected:
			friend class		Synchronize;
			pthread_mutex_t		_mutex;
			pthread_cond_t		_cond;
    };

}

#endif // _ITF_SYNCHRONIZE_IPAD_H_

