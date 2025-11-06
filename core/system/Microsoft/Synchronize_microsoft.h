#pragma once

#include "core/system/Synchronize.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "core/types.h"

namespace ITF
{


#define ITF_MemoryBarrier()    MemoryBarrier()


#define ITF_THREAD_SEMAPHORE        HANDLE
#define ITF_THREAD_EVENT            HANDLE
#define ITF_THREAD_ID                DWORD

#define ITF_THREAD_INVALID_EVENT    NULL

class ITF_THREAD_FASTMUTEX
{
public:    
    ITF_THREAD_FASTMUTEX() {m_lock = 0;}
    void lock() {while (InterlockedCompareExchangeAcquire(&m_lock, 1, 0)==1) {} } 
    void unlock() {InterlockedExchange(&m_lock, 0); }

private:
    volatile long m_lock;
};


#define ITF_THREAD_CRITICAL_SECTION CRITICAL_SECTION

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
        /// @fn static bool tryEnterCriticalSection(VOY_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Try to enter critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static bool tryEnterCriticalSection(ITF_THREAD_CRITICAL_SECTION* pCriticalSection);

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

    // TODO : The name is confusing, some people will mistake this with the higher overhead "real" mutex provided by Windows, the one we call "ProcessMutex"
    class Mutex
    {
    public:

        Mutex()
        {
            Synchronize::createCriticalSection(&m_cs);
        }

        explicit Mutex(const char* /*_dbgName*/) // ignored
        {
            Synchronize::createCriticalSection(&m_cs);
        }

        Mutex(const Mutex&) = delete;
        Mutex& operator=(const Mutex&) = delete;

        ~Mutex()
        {
            Synchronize::destroyCriticalSection(&m_cs);
        }

        void lock()
        {
            Synchronize::enterCriticalSection(&m_cs);
        }

        bool tryLock()
        {
            return Synchronize::tryEnterCriticalSection(&m_cs);
        }

        void unlock()
        {
            Synchronize::leaveCriticalSection(&m_cs);
        }

        ITF_THREAD_CRITICAL_SECTION * getCS()
        {
            return &m_cs;
        }

    private:
        ITF_THREAD_CRITICAL_SECTION m_cs;
    };


    // Generic, could go in a common header
    template <class Locker>
    class ScopeLock
    {
    public:
        ScopeLock(Locker& _locker)
            : m_locker(_locker)
        {
            m_locker.lock();
        }

        ~ScopeLock()
        {
            m_locker.unlock();
        }
    private:
        Locker& m_locker;
    };

    template <class Locker>
    class ScopeUnLock
    {
    public:
        ScopeUnLock(Locker& _locker)
            : m_locker(_locker)
        {
            m_locker.unlock();
        }

        ~ScopeUnLock()
        {
            m_locker.lock();
        }
    private:
        Locker& m_locker;
    };

    using ScopeLockFastMutex = ScopeLock<ITF_THREAD_FASTMUTEX>;
    using ScopeUnLockFastMutex = ScopeUnLock<ITF_THREAD_FASTMUTEX>;

    using ScopeLockMutex = ScopeLock<Mutex>;
    using ScopeUnLockMutex = ScopeUnLock<Mutex>;

    // simple condition variable + its 'mutex' wrapping
    class CondVariable
    {
    public:
        explicit CondVariable(const char* _dbgName = nullptr)
        {
            ITF_UNUSED(_dbgName); // ignored (or the condvar would be a global object)
            ::InitializeConditionVariable(&m_cond);
        }

        CondVariable(const CondVariable&) = delete;
        CondVariable& operator=(const CondVariable&) = delete;

        Mutex& getMutex() { return m_mutex; }

        void signal() // wake up only 1 waiting thread
        {
            ::WakeConditionVariable(&m_cond);
        }

        void broadcast() // wake up all waiting threads and let them fight on the mutex
        {
            ::WakeAllConditionVariable(&m_cond);
        }

        enum : u32 { Infinite = INFINITE };

        // Returns false in case of error or timeout
        bool wait(u32 _ulMicroSec = Infinite)
        {
            auto msTimeout = (_ulMicroSec == Infinite) ? INFINITE : (_ulMicroSec / 1000);
            return 0 != ::SleepConditionVariableCS(&m_cond, m_mutex.getCS(), msTimeout);
        }

        template<class Predicate>
        void wait(Predicate _pred)
        {   // wait for signal and test predicate
            while (!_pred())
                wait();
        }

    private:
        Mutex m_mutex;
        CONDITION_VARIABLE  m_cond{};
    };

}

