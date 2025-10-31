#ifndef ITF_SYNCHRONIZE_PS5_H_
#define ITF_SYNCHRONIZE_PS5_H_

#pragma once

#include <kernel.h>
#include <sce_atomic.h>

#include "core/types.h"

namespace ITF
{
    #define ITF_MemoryBarrier() _mm_mfence()

    class ITF_THREAD_FASTMUTEX
    {
    public:
        ITF_THREAD_FASTMUTEX();

        ~ITF_THREAD_FASTMUTEX();

        void lock();
        void unlock();

    private:
        ScePthreadMutex		m_lwMutex;
    };

    class Mutex
    {
    public:
        explicit Mutex(const char* _debugName = "unset");

        ~Mutex();

        void lock();
        bool tryLock();
        void unlock();

        ScePthreadMutex & GetOSMutexObject()
        {
            return m_lwMutex;
        }

    private:
        friend class CondVariable; // the condition variable need to access the platform specific mutex object
        ScePthreadMutex m_lwMutex;
    };

    // Generic, could go in a common header
    template <class Locker>
    class ScopeLock
    {
    public:
        ScopeLock(Locker & _locker)
            : m_locker(_locker)
        {
            m_locker.lock();
        }

        ~ScopeLock()
        {
            m_locker.unlock();
        }
    private:
        Locker & m_locker;
    };

    template <class Locker>
    class ScopeUnLock
    {
    public:
        ScopeUnLock(Locker & _locker)
            : m_locker(_locker)
        {
            m_locker.unlock();
        }

        ~ScopeUnLock()
        {
            m_locker.lock();
        }
    private:
        Locker & m_locker;
    };

    using ScopeLockFastMutex = ScopeLock<ITF_THREAD_FASTMUTEX>;
    using ScopeUnLockFastMutex = ScopeUnLock<ITF_THREAD_FASTMUTEX>;

    using ScopeLockMutex = ScopeLock<Mutex>;
    using ScopeUnLockMutex = ScopeUnLock<Mutex>;


    using ITF_THREAD_ID = uPtr;

    using ITF_THREAD_CRITICAL_SECTION = ScePthreadMutex;

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
        static bbool tryEnterCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs);

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
        /// @param [in] szName  If non-null, name of the event. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        static void createEvent(ITF_THREAD_EVENT * pEvent, const char* szName, bbool bAutoReset=bfalse);

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

        void init( const char *szName, bbool bAutoReset);
        void destroy();

        void wait();
        bbool wait(i64 _timeoutMs);
        bbool tryWait();
        void reset();
        void set();

    protected:
        friend class Synchronize;
		ScePthreadMutex	m_mutex;
        ScePthreadCond _cond;

        volatile bbool m_signaled;
        bbool m_autoreset;
    };

    // simple condition variable wrapping
    class CondVariable
    {
    public:
        explicit CondVariable(const char * _dbgName = NULL);
        ~CondVariable();

        // Locking can be done by getting the mutex
        Mutex & getMutex() { return m_mutex; }
        
        // Or direct call to lock/unlock methods (thus a condition variable can be used with a ScopeLock)
        void lock();
        void unlock();

        void signal(); // wake up only 1 waiting thread
        void broadcast(); // wake up all waiting threads and let them fight on the mutex

        // Returns false in case of error or timeout
        bool wait(u32 _ulMicroSec = 0);

    private:
        Mutex m_mutex;
        ScePthreadCond  m_cond;
    };

    inline void CondVariable::lock()
    {
        m_mutex.lock();
    }

    inline void CondVariable::unlock()
    {
        m_mutex.unlock();
    }

    // Atomic stuff
    // right now, only fcts used by the scheduler are put there

    ITF_FORCE_INLINE int64_t InterlockedAnd(volatile int64_t* Target, int64_t Set)
    {
        return sceAtomicAnd64(Target, Set);
    }

    ITF_FORCE_INLINE int64_t InterlockedOr(volatile int64_t* Target, int64_t Set)
    {
        return sceAtomicOr64(Target, Set);
    }

    ITF_FORCE_INLINE long InterlockedIncrement(long volatile *lpAddend)
    {
        return long(sceAtomicIncrement64(lpAddend));
    }

    ITF_FORCE_INLINE long InterlockedDecrement(long volatile *lpAddend)
    {
        return long(sceAtomicDecrement64(lpAddend));
    }

    // Thread class to inherit in order to implement a thread in oop style with a virtual Run method
    class VThread
    {
    public:
        VThread();
        VThread(const VThread &) = delete;

        virtual ~VThread();

        VThread & operator=(const VThread&) = delete;

        enum
        {
            DefaultPriority = 1001,
            DefaultStackSize = 8*1024
        };

        virtual bool start(const char * _name, u32 _prio = DefaultPriority, u32 _stackSize = DefaultStackSize);

        virtual u32 run() = 0;
        
        u32 waitForExit();

        virtual void requestExit() {} // overload this method to give a way to stop the current thread

    private:
        static void* threadStub(void* _param);
		ScePthread m_thread;
    };


    // Lock free single list stuff
    struct SLIST_ENTRY
    {
        SLIST_ENTRY *Next;
    };

    typedef SLIST_ENTRY * PSLIST_ENTRY;

    union SLIST_HEADER
    {
        int64_t allMembers;
        struct  
        {
            SLIST_ENTRY	* Next;
            union
            {
                u32 depthSeq;
                struct  
                {
                    u16		            Depth;
                    u16		            Sequence;
                };
            };
        };
    };

    typedef  SLIST_HEADER *  PSLIST_HEADER;
    typedef double DOUBLE;

    inline void InitializeSListHead(PSLIST_HEADER head)
    {
        head->allMembers = 0u;
    }


    // Locked versions for reference
#if 0
    inline PSLIST_ENTRY InterlockedPushEntrySList(PSLIST_HEADER head, SLIST_ENTRY *entry)
    {
        ScopeLock locker(mutex);
        PSLIST_ENTRY old = head->Next;
        entry->Next = old;
        head->Next = entry;
        head->Depth++;
        head->Sequence++;
        return old;
    }

    inline PSLIST_ENTRY InterlockedPopEntrySList(PSLIST_HEADER head)
    {
        ScopeLock locker(mutex);
        PSLIST_ENTRY entry;
        PSLIST_ENTRY old;
        old = head->Next;
        entry = old;
        ITF_MemoryBarrier();
        if( !entry )
            return NULL;
        head->Next = entry->Next;
        head->Depth--;
        head->Sequence++;
        return entry;
    }
#endif

    inline PSLIST_ENTRY InterlockedPushEntrySList(PSLIST_HEADER head, SLIST_ENTRY *entry)
    {
        SLIST_HEADER oldHead;
        SLIST_HEADER newHead;
        newHead.Next = entry;

        do
        {
            oldHead = *head;
            entry->Next = oldHead.Next;
            // We need a memory barrier to be sure that the new entry is correctly linked
            // to the previously first element before effectively modifying the list head
            ITF_MemoryBarrier();
            newHead.Depth = oldHead.Depth + 1;
            newHead.Sequence = oldHead.Sequence + 1;
        } while(sceAtomicCompareAndSwap64(&head->allMembers, oldHead.allMembers, newHead.allMembers)
                != oldHead.allMembers);
        return oldHead.Next;
    }

    inline PSLIST_ENTRY InterlockedPopEntrySList(PSLIST_HEADER head)
    {
        SLIST_HEADER oldHead;
        SLIST_HEADER newHead;
        PSLIST_ENTRY entryPoped;

        do
        {
            // We need a memory barrier here to be sure that the *head is effectively
            // coherent with *entryPoped (*entryPoped could have been written before *head)
            ITF_MemoryBarrier();
            oldHead = *head;
            entryPoped = oldHead.Next;
            if (!entryPoped) // empty head, just return...
            {
                return NULL;
            }
            newHead.Next = entryPoped->Next;
            newHead.Depth = oldHead.Depth - 1;
            newHead.Sequence = oldHead.Sequence + 1;
        } while(sceAtomicCompareAndSwap64(&head->allMembers, oldHead.allMembers, newHead.allMembers)
                != oldHead.allMembers);
        return entryPoped;
    }

    ITF_FORCE_INLINE void QueryPerformanceCounter(LARGE_INTEGER * _counter)
    {
		_counter->QuadPart = sceKernelReadTsc();
    }

    ITF_FORCE_INLINE void QueryPerformanceFrequency(LARGE_INTEGER * _freq)
    {
		_freq->QuadPart = sceKernelGetTscFrequency();
    }
}

#endif // ITF_SYNCHRONIZE_PS5_H_

