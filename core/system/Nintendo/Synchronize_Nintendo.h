#ifndef ITF_SYNCHRONIZE_NX_H_
#define ITF_SYNCHRONIZE_NX_H_

#pragma once

#include "core/types.h"

#include "Atomic_Nintendo.h"

#include <nn/os/os_MemoryFenceApi.h>
#include <nn/os/os_MutexTypes.h>
#include <nn/os/os_SemaphoreTypes.h>
#include <nn/os/os_EventTypes.h>
#include <nn/os/os_Mutex.h>
#include <nn/os/os_ThreadTypes.h>
#include <nn/os/os_ThreadLocalStorage.h>
#include <nn/os/os_ConditionVariable.h>
#include <nn/os.h>

namespace ITF
{
    class Mutex
    {
    public:
        Mutex(const char* _debugName = "unset")
            : m_osMutex(true) // recursive so not so fast
        {
            ITF_UNUSED(_debugName); // unfortunately no debug names on mutex
        }

        ~Mutex() = default;

        void lock()
        {
            m_osMutex.lock();
        }

        bool tryLock()
        {
            return m_osMutex.try_lock();
        }

        void unlock()
        {
            m_osMutex.unlock();
        }

        nn::os::Mutex & GetOSMutexObject() { return m_osMutex; }

    private:
        nn::os::Mutex m_osMutex;
    };

    // Do not use ITF_THREAD_FASTMUTEX anymore, use Mutex instead
    using ITF_THREAD_FASTMUTEX = Mutex;

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

    using ITF_THREAD_ID = uPtr;

    using ITF_THREAD_CRITICAL_SECTION = nn::os::MutexType;

    using ITF_THREAD_EVENT = nn::os::EventType;

    inline void ITF_MemoryBarrier()
    {
        nn::os::FenceMemoryAnyAny();
    }

    class Synchronize
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static ITF_THREAD_ID getCurrentThreadId()
        /// @brief  Gets the current thread identifier. 
        /// @return The current thread identifier. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static ITF_THREAD_ID getCurrentThreadId();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createCriticalSection(ITF_THREAD_CRITICAL_SECTION * pCriticalSection)
        /// @brief  Creates a critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void createCriticalSection(ITF_THREAD_CRITICAL_SECTION* pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Destroys the critical section described by pCriticalSection. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION* pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Enter critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void enterCriticalSection(ITF_THREAD_CRITICAL_SECTION* pCriticalSection);
        static bbool tryEnterCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *pCriticalSection)
        /// @brief  Leave critical section. 
        /// @param [in,out] pCriticalSection    If non-null, the critical section. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION* pCriticalSection);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void createEvent(ITF_THREAD_EVENT * pEvent, char* szName)
        /// @brief  Creates an event. 
        /// @param [in,out] pEvent  If non-null, the event. 
        /// @param [in] szName  If non-null, name of the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void createEvent(ITF_THREAD_EVENT* pEvent, const char* szName, bbool bAutoReset = bfalse);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void resetEvent(ITF_THREAD_EVENT *evt)
        /// @brief  Resets the event described by evt. 
        /// @param [in,out] evt If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void  resetEvent(ITF_THREAD_EVENT* evt);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void setEvent(ITF_THREAD_EVENT *pEvent)
        /// @brief  Sets the specified event object to the signaled state.
        /// @param [in,out] pEvent  If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setEvent(ITF_THREAD_EVENT* pEvent);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn static void destroyEvent(ITF_THREAD_EVENT *pEvent)
        /// @brief  Destroys the event described by pEvent. 
        /// @param [in,out] pEvent  If non-null, the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void destroyEvent(ITF_THREAD_EVENT* pEvent);

        static void waitEvent(ITF_THREAD_EVENT* pEvent);
    };


    // Simple condition variable wrapping, with its mutex embedded.
    class CondVariable
    {
    public:
        explicit CondVariable(const char* _dbgName = NULL);
        CondVariable(const CondVariable &) = delete;
        ~CondVariable();

        CondVariable & operator = (const CondVariable&) = delete;

        // Locking can be done by getting the mutex
        Mutex& getMutex() { return m_mutex; }

        // Or direct call to lock/unlock methods (thus a condition variable can be used with a ScopeLock)
        void lock();
        void unlock();

        void signal(); // wake up only 1 waiting thread
        void broadcast(); // wake up all waiting threads and let them fight on the mutex

        // Returns false in case of error or timeout
        bool wait(u32 _ulMicroSec = 0);

    private:
        Mutex m_mutex;
        nn::os::ConditionVariable m_cond;
    };


    ITF_FORCE_INLINE long InterlockedAnd(volatile long *Target, long Set)
    {
        return  __atomic_and_fetch(Target, Set, __ATOMIC_SEQ_CST);
    }

    ITF_FORCE_INLINE long InterlockedOr(volatile long *Target, long Set)
    {
        return  __atomic_or_fetch(Target, Set, __ATOMIC_SEQ_CST);
    }

    ITF_FORCE_INLINE long InterlockedIncrement(volatile long *lpAddend)
    {
        // ITF_PASTA
        return  __atomic_add_fetch(lpAddend, 1, __ATOMIC_SEQ_CST);
    }

    ITF_FORCE_INLINE long InterlockedDecrement(volatile long *lpAddend)
    {
        return  __atomic_sub_fetch(lpAddend, 1, __ATOMIC_SEQ_CST);
        //		return OSDecAtomic((volatile OSAtomicVar *)lpAddend);
    }

    ITF_FORCE_INLINE long InterlockedSwap(volatile long *lpAddend, long val)
    {
        return  __atomic_exchange_n(lpAddend, val, __ATOMIC_SEQ_CST);
        //		return OSSwapAtomic((volatile OSAtomicVar *)lpAddend, u32(val));
    }

    ITF_FORCE_INLINE void QueryPerformanceCounter(LARGE_INTEGER* counter)
    {
        counter->QuadPart = nn::os::GetSystemTick().GetInt64Value();
    }

    ITF_FORCE_INLINE void QueryPerformanceFrequency(LARGE_INTEGER* freq)
    {
        freq->QuadPart = nn::os::GetSystemTickFrequency();
    }


    // Thread class to inherit in order to implement a thread in oop style with a virtual Run method
    class VThread
    {
    public:
        VThread();
        VThread(const VThread &) = delete;

        virtual ~VThread();

        VThread & operator=(const VThread&) = delete;

        static constexpr u32 DefaultPriority = nn::os::DefaultThreadPriority;
        static constexpr u32 DefaultStackSize = 64 * 1024;

        virtual bool start(const char * _name, u32 _prio = DefaultPriority, u32 _stackSize = DefaultStackSize);

        virtual u32 run() = 0;
        
        u32 waitForExit();

        virtual void requestExit() {} // overload this method to give a way to stop the current thread

    private:
        static void threadStub(void* _param);
        nn::os::ThreadType m_osThread;
        void* m_stack = nullptr;
        char m_name[64];
        u32 m_returnValue = U32_INVALID;
    };

    // Lock free single list stuff
    struct SLIST_ENTRY
    {
	    SLIST_ENTRY *Next;
    };

    typedef SLIST_ENTRY * PSLIST_ENTRY;

    union SLIST_HEADER
    {
        volatile i64 allMembers;
        struct
        {
            SLIST_ENTRY	* Next;
            union
            {
                u64 depthSeq;
                struct
                {
                    u32 Depth;
                    u32 Sequence;
                };
            };
        };
    };

    using PSLIST_HEADER = SLIST_HEADER *;

    inline void InitializeSListHead(PSLIST_HEADER head)
    {
        head->allMembers = 0;
    }

    PSLIST_ENTRY InterlockedPushEntrySList(PSLIST_HEADER head, SLIST_ENTRY *entry);

    PSLIST_ENTRY InterlockedPopEntrySList(PSLIST_HEADER head);
}

#endif // ITF_SYNCHRONIZE_NX_H_
