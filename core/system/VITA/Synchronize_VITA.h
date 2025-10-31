
#ifndef _ITF_SYNCHRONIZE_VITA_H_
#define _ITF_SYNCHRONIZE_VITA_H_
/*
#include <ppu_intrinsics.h>
#include <sys/ppu_thread.h>
#include <sys/spinlock.h>
#include <sys/synchronization.h>
#include <sys/event.h>*/
#include "core/types.h"

namespace ITF
{

#define ITF_MemoryBarrier()    __lwsync()

    class ITF_THREAD_FASTMUTEX
    {
        public:    
            ITF_THREAD_FASTMUTEX() {}
            void lock() 
            {
                ITF_ASSERT(0);
                //NOT_IMPLEMENTED_ON_VITA 
            } 

            void unlock() 
            { 
                  ITF_ASSERT(0); 
                  //NOT_IMPLEMENTED_ON_VITA 
            }


        private:
    };


#define ITF_THREAD_SEMAPHORE        HANDLE
#define ITF_THREAD_ID                DWORD

#define ITF_THREAD_CRITICAL_SECTION u32 //TODO PS3


    class ITF_THREAD_EVENT;

    class Synchronize
    {
        public:

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static ITF_THREAD_ID getCurrentThreadId(void)
        /// @brief  Gets the current thread identifier. 
        /// @return The current thread identifier. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static ITF_THREAD_ID getCurrentThreadId(void);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName,
        ///     u32 uInitialCount= 0, u32 uMaximumCount= 666666)
        /// @param [in,out] pSemaphore  If non-null, the semaphore. 
        /// @param [in,out] szName      If non-null, name of the size. 
        /// @param  uInitialCount       The initial count for the semaphore object.. 
        /// @param  uMaximumCount       The maximum count for the semaphore object. This value must be greater than zero. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, char * szName, u32 uInitialCount= 0, u32 uMaximumCount= 666666);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount)
        /// @brief  Increases the count of the specified semaphore object by a specified amount.
        /// @param [in,out] pSemaphore  If non-null, the semaphore. 
        /// @param  uCount              The amount by which the semaphore object's current count is to be increased.
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 uCount);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore)
        /// @brief  Destroys the semaphore described by pSemaphore. 
        /// @param [in,out] pSemaphore  If non-null, the semaphore. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createCriticalSection(ITF_THREAD_CRITICAL_SECTION * pCriticalSection)
        /// @brief  Creates a critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createCriticalSection(ITF_THREAD_CRITICAL_SECTION * pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Destroys the critical section described by pCriticalSection. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Enter critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Leave critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createEvent(ITF_THREAD_EVENT * pEvent, char* szName)
        /// @brief  Creates an event. 
        /// @param [in,out] pEvent  If non-null, the event. 
        /// @param [in,out] szName  If non-null, name of the size. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createEvent(ITF_THREAD_EVENT * pEvent, char* szName, bbool bAutoReset=bfalse);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void resetEvent(ITF_THREAD_EVENT *evt)
        /// @brief  Resets the event described by evt. 
        /// @param [in,out] evt If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void  resetEvent(ITF_THREAD_EVENT *evt);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void setEvent(ITF_THREAD_EVENT *pEvent)
        /// @brief  Sets the specified event object to the signaled state.
        /// @param [in,out] pEvent  If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void setEvent(ITF_THREAD_EVENT *pEvent);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroyEvent(ITF_THREAD_EVENT *pEvent)
        /// @brief  Destroys the event described by pEvent. 
        /// @param [in,out] pEvent  If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void destroyEvent(ITF_THREAD_EVENT *pEvent);


        static void waitEvent(ITF_THREAD_EVENT* pEvent);
    };

    class ITF_THREAD_EVENT
    {
        public :
            ITF_THREAD_EVENT():m_signaled(bfalse),m_autoreset(bfalse) {};

        void init( char *szName, bbool bAutoReset);
        void destroy()
        {/*
            sys_lwcond_destroy( &_cond );
            sys_lwmutex_destroy( &_mutex );*/
        }

        void wait();
        void reset();
        void set();

    protected:
        friend class Synchronize;
       /* sys_lwmutex_t _mutex;
        sys_lwcond_t _cond;*/

        volatile bbool m_signaled;
        bbool m_autoreset;
    };

}

#endif // _ITF_SYNCHRONIZE_VITA_H_

