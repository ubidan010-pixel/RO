#ifndef _ITF_SYNCHRONIZE_WII_H_
#define _ITF_SYNCHRONIZE_WII_H_

namespace ITF
{
	#define ITF_MemoryBarrier()

	#define ITF_THREAD_CRITICAL_SECTION		OSMutex
	#define ITF_THREAD_SEMAPHORE			OSSemaphore
	#define ITF_THREAD_ID					u32

	class ITF_THREAD_FASTMUTEX
	{
		public:    
			ITF_THREAD_FASTMUTEX() {m_lock = 0;}
			void lock() {} 
			void unlock() {}

		private:
			volatile long m_lock;
	};

 	class ITF_THREAD_EVENT;

    class Synchronize
    {
        public:
			static ITF_THREAD_ID	getCurrentThreadId(void);

			static void				createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName, u32 uInitialCount= 0, u32 uMaximumCount= 666666);
			static void				increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount);
			static void				waitSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount);
			static void				destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore);

			static void				createCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);
			static void				destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);
			static void				enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);
			static void				leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);

			static void             createEvent(ITF_THREAD_EVENT * pEvent, char* szName, bbool bAutoReset=bfalse);
            static void				resetEvent(ITF_THREAD_EVENT *evt);
			static void				setEvent(ITF_THREAD_EVENT *pEvent);
			static void				destroyEvent(ITF_THREAD_EVENT *pEvent);
			static void				waitEvent(ITF_THREAD_EVENT* pEvent);

			static bbool			IsInterupt();
			static bbool			SetInterupt(bbool _int);

		private:
			static volatile bbool	bg_Interrupt;
    };

    class ITF_THREAD_EVENT
    {

        public :
            ITF_THREAD_EVENT():m_signaled(bfalse),m_autoreset(bfalse) {};

        void init( char *szName, bbool bAutoReset);
        void destroy()
        {
        }

        void wait();
        void reset();
        void set();

		protected:
			friend class		Synchronize;
			OSMutex				m_mutex;
			OSCond				m_cond;
			
        	volatile bbool m_signaled;
        	bbool m_autoreset;
    };

    ITF_FORCE_INLINE long InterlockedIncrement(volatile register long *lpAddend)
    {
        register u32	reg0;
        register u32	reg1;
        asm volatile
        {
        lockFailedLoop:
            lwarx  reg0, 0, lpAddend
            addi   reg1, reg0, 1
            stwcx. reg1, 0, lpAddend
            bne-   lockFailedLoop
        }

        return reg1;
    }

    ITF_FORCE_INLINE long InterlockedDecrement(volatile register long *lpAddend)
    {
        register u32	reg0;
        register u32	reg1;
        asm volatile
        {
        lockFailedLoop:
            lwarx  reg0, 0, lpAddend
            addi   reg1, reg0, -1
            stwcx. reg1, 0, lpAddend
            bne-   lockFailedLoop
        }

        return reg1;
    }
}

#endif // _ITF_SYNCHRONIZE_WII_H_

