#include "precompiled_core.h"

#include "core/system/Synchronize.h"
#include "core/system/Thread.h"
#include <nn/os.h>

namespace ITF
{
    // Synchronize static methods

    ITF_THREAD_ID Synchronize::getCurrentThreadId()
    {
        return reinterpret_cast<ITF_THREAD_ID>(nn::os::GetCurrentThread());
    }

    void Synchronize::createCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs)
    {
        nn::os::InitializeMutex(cs, true, 0);
    }

    void Synchronize::destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs)
    {
        nn::os::FinalizeMutex(cs);
    }

    void Synchronize::enterCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs)
    {
        nn::os::LockMutex(cs);
    }

    bbool Synchronize::tryEnterCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs)
    {
        return nn::os::TryLockMutex(cs);
    }

    void Synchronize::leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION* cs)
    {
        nn::os::UnlockMutex(cs);
    }

    void Synchronize::createEvent(ITF_THREAD_EVENT* evt, const char* /*szName*/, bbool bAutoReset)
    {
        nn::os::InitializeEvent(evt, false, bAutoReset ? nn::os::EventClearMode_AutoClear : nn::os::EventClearMode_ManualClear);
    }

    void Synchronize::resetEvent(ITF_THREAD_EVENT* evt)
    {
        nn::os::ClearEvent(evt);
    }

    void Synchronize::setEvent(ITF_THREAD_EVENT* evt)
    {
        nn::os::SignalEvent(evt);
    }

    void Synchronize::destroyEvent(ITF_THREAD_EVENT* evt)
    {
        nn::os::FinalizeEvent(evt);
    }

    void Synchronize::waitEvent(ITF_THREAD_EVENT* evt)
    {
        nn::os::WaitEvent(evt);
    }

    // Condition Variable

    CondVariable::CondVariable(const char* _dbgName)
    {
    }

    CondVariable::~CondVariable() = default;

    void CondVariable::lock()
    {
        m_mutex.lock();
    }

    void CondVariable::unlock()
    {
        m_mutex.unlock();
    }

    void CondVariable::signal() // wake up only 1 waiting thread
    {
        m_cond.Signal();
    }

    void CondVariable::broadcast() // wake up all waiting threads and let them fight on the mutex
    {
        m_cond.Broadcast();
    }

    // Returns false in case of error or timeout
    bool CondVariable::wait(u32 _ulMicroSec)
    {
        if (_ulMicroSec == 0)
        {
            m_cond.Wait(m_mutex.GetOSMutexObject());
            return true; // no error
        }
        else
        {
            nn::TimeSpan timeout = nn::TimeSpan::FromMicroSeconds(u64(_ulMicroSec));
            nn::os::ConditionVariableStatus status = m_cond.TimedWait(m_mutex.GetOSMutexObject(), timeout);
            return status != nn::os::ConditionVariableStatus_Timeout;
        }
    }

    // VThread

    VThread::VThread()
    {
        m_name[0] = '\0';
    }
    
    VThread::~VThread()
    {
        if (m_stack != nullptr)
        {
            waitForExit();
        }
    }

    bool VThread::start(const char* _name, u32 _prio, u32 _stackSize)
    {
        m_returnValue = U32_INVALID;

        size_t alignedStackSize = nn::util::align_up(_stackSize, nn::os::ThreadStackAlignment);
        m_stack = Memory::alignedMallocCategory(alignedStackSize, nn::os::ThreadStackAlignment, MemoryId::mId_System);

        int osPriority = static_cast<int>(_prio);

        nn::Result created = nn::os::CreateThread(&m_osThread, threadStub, this, m_stack, alignedStackSize, osPriority);

        ITF_ASSERT(created.IsSuccess());

        if (created.IsSuccess())
        {
        #ifndef ITF_FINAL
            ::strncpy(m_name, _name, ITF_ARRAY_SIZE(m_name));
            m_name[ITF_ARRAY_SIZE(m_name) - 1] = '\0';
            nn::os::SetThreadNamePointer(&m_osThread, m_name);
        #endif // ITF_FINAL
            nn::os::StartThread(&m_osThread);
        }

        return created.IsSuccess();
    }

    u32 VThread::waitForExit()
    {
        if (m_stack != nullptr)
        {
            nn::os::WaitThread(&m_osThread);
            nn::os::DestroyThread(&m_osThread);
            Memory::alignedFree(m_stack);
            m_stack = nullptr;
        }
        return m_returnValue;
    }

    void VThread::threadStub(void* _param)
    {
        VThread* thread = static_cast<VThread*>(_param);
        if (thread)
        {
            thread->m_returnValue = thread->run();
        }
    }


    // LIST_ENTRY

    PSLIST_ENTRY InterlockedPushEntrySList(PSLIST_HEADER head, SLIST_ENTRY *entry)
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
            //	} while (!OSCompareAndSwapAtomic(&head->allMembers, oldHead.allMembers.u.u32, newHead.allMembers.u.u32));
        } while (oldHead.allMembers != ITF::Atomic::exchangeIfEqual(head->allMembers, oldHead.allMembers, newHead.allMembers));

        return oldHead.Next;
    }

    PSLIST_ENTRY InterlockedPopEntrySList(PSLIST_HEADER head)
    {
        // ITF_PASTA
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
            //	} while (!OSCompareAndSwapAtomic(&head->allMembers, oldHead.allMembers.u.u32, newHead.allMembers.u.u32));
        } while (oldHead.allMembers != ITF::Atomic::exchangeIfEqual(head->allMembers, oldHead.allMembers, newHead.allMembers));
        return entryPoped;
    }

}
