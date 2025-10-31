#ifndef ITF_SYNCHRONIZE_PS3_H_
#define ITF_SYNCHRONIZE_PS3_H_

#include <ppu_intrinsics.h>
#include <sys/ppu_thread.h>
#include <sys/spinlock.h>
#include <sys/synchronization.h>
#include <sys/event.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include "core/types.h"

namespace ITF
{

#define ITF_MemoryBarrier()    __lwsync()

    class ITF_THREAD_FASTMUTEX
    {
        public:    
            ITF_THREAD_FASTMUTEX()
            {
                sys_lwmutex_attribute_t lwMutexAttr;
                lwMutexAttr.attr_protocol = SYS_SYNC_PRIORITY; // or SYS_SYNC_FIFO ?
                lwMutexAttr.attr_recursive = SYS_SYNC_NOT_RECURSIVE;
                lwMutexAttr.name[0] = 0;
                ITF_VERIFY(CELL_OK == sys_lwmutex_create( &m_lwMutex, &lwMutexAttr));
            }

            ~ITF_THREAD_FASTMUTEX()
            {
                ITF_VERIFY(CELL_OK == sys_lwmutex_destroy(&m_lwMutex));
            }

            void lock() 
            {
                ITF_VERIFY(CELL_OK == sys_lwmutex_lock(&m_lwMutex, 0));
            } 

            void unlock() 
            { 
                ITF_VERIFY(CELL_OK == sys_lwmutex_unlock(&m_lwMutex));
            }

        private:
            sys_lwmutex_t m_lwMutex;
    };

    class Mutex
    {
    public:    
        Mutex()
        {
            sys_lwmutex_attribute_t lwMutexAttr;
            lwMutexAttr.attr_protocol = SYS_SYNC_PRIORITY; // or SYS_SYNC_FIFO ?
            lwMutexAttr.attr_recursive = SYS_SYNC_RECURSIVE;
            lwMutexAttr.name[0] = 0;
            ITF_VERIFY(CELL_OK == sys_lwmutex_create( &m_lwMutex, &lwMutexAttr));
        }

        ~Mutex()
        {
            ITF_VERIFY(CELL_OK == sys_lwmutex_destroy(&m_lwMutex));
        }

        void lock() 
        {
            ITF_VERIFY(CELL_OK == sys_lwmutex_lock(&m_lwMutex, 0));
        } 

        void unlock() 
        { 
            ITF_VERIFY(CELL_OK == sys_lwmutex_unlock(&m_lwMutex));
        }

    private:
        friend class CondVariable; // the condition variable need to access the platform specific mutex object
        sys_lwmutex_t m_lwMutex;
    };

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


    typedef ScopeLock<ITF_THREAD_FASTMUTEX> ScopeLockFastMutex;
    typedef ScopeUnLock<ITF_THREAD_FASTMUTEX> ScopeUnLockFastMutex;

    typedef ScopeLock<Mutex> ScopeLockMutex;
    typedef ScopeUnLock<Mutex> ScopeUnLockMutex;

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
        /// @param [in] szName      If non-null, name of the size. 
        /// @param  uInitialCount       The initial count for the semaphore object.. 
        /// @param  uMaximumCount       The maximum count for the semaphore object. This value must be greater than zero. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        static void createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, const char * szName, u32 uInitialCount= 0, u32 uMaximumCount= 666666);

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
        void destroy()
        {
            sys_lwcond_destroy( &_cond );
            sys_lwmutex_destroy( &_mutex );
        }

        void wait();
        void reset();
        void set();

    protected:
        friend class Synchronize;
        sys_lwmutex_t _mutex;
        sys_lwcond_t _cond;

        volatile bbool m_signaled;
        bbool m_autoreset;
    };

    // simple condition variable wrapping
    class CondVariable
    {
    public:
        CondVariable(const char * _dbgName = NULL);
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
        sys_lwcond_t m_cond;
    };

    inline CondVariable::CondVariable(const char * _dbgName)
    {
        sys_lwcond_attribute_t cond_attr;
        if (_dbgName)
        {
            strncpy(cond_attr.name, _dbgName, sizeof(cond_attr.name-1u));
            cond_attr.name[sizeof(cond_attr.name-1u)] = 0;
        }
        else
            sys_lwcond_attribute_initialize(cond_attr);
        ITF_VERIFY(CELL_OK == sys_lwcond_create(&m_cond, &m_mutex.m_lwMutex, &cond_attr));
    }

    inline CondVariable::~CondVariable()
    {
        ITF_VERIFY(CELL_OK == sys_lwcond_destroy(&m_cond));
    }

    inline void CondVariable::lock()
    {
        m_mutex.lock();
    }

    inline void CondVariable::unlock()
    {
        m_mutex.unlock();
    }

    inline void CondVariable::signal() // wake up only 1 waiting thread
    {
        ITF_VERIFY(CELL_OK == sys_lwcond_signal(&m_cond));
    }

    inline void CondVariable::broadcast() // wake up all waiting threads and let them fight on the mutex
    {
        ITF_VERIFY(CELL_OK == sys_lwcond_signal_all(&m_cond));
    }

    // Returns false in case of error or timeout
    inline bool CondVariable::wait(u32 _ulMicroSec)
    {
        int ret = sys_lwcond_wait(&m_cond, _ulMicroSec);
        ITF_ASSERT((ret == CELL_OK) || (ret == ETIMEDOUT));
        return ret == CELL_OK;
    }

    typedef ScopeLock<CondVariable> ScopeLockCondVar;
    typedef ScopeUnLock<CondVariable> ScopeUnLockCondVar;

    // Atomic stuff
    // right now, only fcts used by the scheduler are put there
    ITF_FORCE_INLINE long _InterlockedAnd(long volatile *Target, long Set)
    {
        return long(cellAtomicAnd32((uint32_t *)Target, uint32_t(Set)));
    }

    ITF_FORCE_INLINE long _InterlockedOr(long volatile *Target, long Set)
    {
        return long(cellAtomicOr32((uint32_t *)Target, uint32_t(Set)));
    }

    ITF_FORCE_INLINE long InterlockedIncrement(long volatile *lpAddend)
    {
        return long(cellAtomicIncr32((uint32_t *)lpAddend));
    }

    ITF_FORCE_INLINE long InterlockedDecrement(long volatile *lpAddend)
    {
        return long(cellAtomicDecr32((uint32_t *)lpAddend));
    }

    // Thread class to inherit in order to implement a thread in oop style with a virtual Run method
    class VThread
    {
    public:
        VThread();
        virtual ~VThread();

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
        static void threadStub(u64 _param);
        sys_ppu_thread_t m_thread;
    };


    // Lock free single list stuff
    struct SLIST_ENTRY
    {
        SLIST_ENTRY *Next;
    };

    typedef SLIST_ENTRY * PSLIST_ENTRY;

    union SLIST_HEADER
    {
        u64 allMembers;
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
        } while(cellAtomicCompareAndSwap64(&head->allMembers, oldHead.allMembers, newHead.allMembers)
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
        } while(cellAtomicCompareAndSwap64(&head->allMembers, oldHead.allMembers, newHead.allMembers)
                != oldHead.allMembers);
        return entryPoped;
    }

    ITF_FORCE_INLINE void QueryPerformanceCounter(LARGE_INTEGER * _counter)
    {
        uint64_t cur_tb;
        SYS_TIMEBASE_GET(cur_tb);
        _counter->QuadPart = cur_tb;
    }

    ITF_FORCE_INLINE void QueryPerformanceFrequency(LARGE_INTEGER * _freq)
    {
        //it will return 79.8MHz on PS3
        _freq->QuadPart = sys_time_get_timebase_frequency();
    }
}

#endif // ITF_SYNCHRONIZE_PS3_H_

