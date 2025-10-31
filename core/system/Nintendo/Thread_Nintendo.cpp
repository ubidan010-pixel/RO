#include "precompiled_core.h"
#include "core/system/thread.h"
#include "core/math/BitTweak.h"

#include <nn/os/os_Thread.h>

#include <nn/nn_TimeSpan.h>

namespace ITF
{
    ThreadLocalStorage ThreadInfo::m_threadEngineIndex;

    void sleep(u32 uMilliseconds)
    {
        nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(uMilliseconds));
    }

    void YieldProcessor()
    {
        nn::os::YieldThread();
    }

    Thread::Thread() = default;

    Thread::~Thread()
    {
        destroyThread();
    }

    void Thread::destroyThread()
    {
        join();

        if (m_created)
        {
            nn::os::DestroyThread(&m_osThread);
        }

        if (m_stack != nullptr)
        {
            Memory::alignedFree(m_stack);
            m_stack = nullptr;
        }
    }

    void Thread::create(ThreadFunc _threadFunc, void* _threadFuncParam, const ThreadSettings& _settings)
    {
        ITF_ASSERT_CRASH(!m_created || m_finished, "Creating a thread that is already running");
        destroyThread();

        size_t stackSize = nn::util::align_up(_settings.m_stackSize, nn::os::ThreadStackAlignment);
        m_stack = Memory::alignedMallocCategory(stackSize, nn::os::ThreadStackAlignment, MemoryId::mId_System);

        int priority = getPriorityHW(_settings.m_priority);
        Thread_HardwareThreadId hwThreadId = m_eThreadAffinity == Thread_HardwareThread_Unknown ? _settings.m_hwId : m_eThreadAffinity;
        int coreNumber = getCoreIdHW(hwThreadId);

        static auto fctStub = [](void* _arg)
        {
            (reinterpret_cast<Thread *>(_arg))->threadMain();
        };

        nn::Result created = nn::os::CreateThread(&m_osThread, fctStub, this, m_stack, stackSize, priority, coreNumber);
        ITF_ASSERT(created.IsSuccess());
        m_created = created.IsSuccess();
        if (!m_created)
        {
            destroyThread();
            return;
        }

        m_threadFunc = _threadFunc;
        m_threadFuncParam = _threadFuncParam;
        m_settings = _settings;
        m_eThreadAffinity = hwThreadId;
        m_finished = false;
        m_waited = false;
        nn::os::SetThreadNamePointer(&m_osThread, m_settings.m_name);
        nn::os::StartThread(&m_osThread);
    }

    int Thread::getPriorityHW(Thread_Priority ePriority)
    {
        int iPriority;
        switch (ePriority)
        {
        case Thread_Priority_Idle:
            iPriority = nn::os::LowestThreadPriority;
            break;
        case Thread_Priority_Lowest:
            iPriority = nn::os::LowestThreadPriority - 1;
            break;
        case Thread_Priority_BelowNormal:
            iPriority = nn::os::DefaultThreadPriority + 2;
            break;
        case Thread_Priority_Normal:
            iPriority = nn::os::DefaultThreadPriority;
            break;
        case Thread_Priority_AboveNormal:
            iPriority = nn::os::DefaultThreadPriority - 2;
            break;
        case Thread_Priority_Highest:
            iPriority = nn::os::HighestThreadPriority + 1;
            break;
        case Thread_Priority_TimeCritical:
            iPriority = nn::os::HighestThreadPriority;
            break;
        default:
            ITF_ASSERT(false);
            iPriority = nn::os::DefaultThreadPriority;
            break;
        }
        return iPriority;
    }

    int Thread::getCoreIdHW(Thread_HardwareThreadId _affinity)
    {
        u32 coreId = nn::os::IdealCoreDontCare;
    #ifdef ITF_NX
        switch (_affinity)
        {
        case Thread_HardwareThread0:
            coreId = 0;
            break;
        case Thread_HardwareThread1:
        case Thread_HardwareThread2:
            coreId = 1;
            break;
        case Thread_HardwareThread3:
        case Thread_HardwareThread4:
        case Thread_HardwareThread5:
        case Thread_HardwareThread6:
        case Thread_HardwareThread7:
            coreId = 2;
            break;

        default:
            coreId = nn::os::IdealCoreDontCare;
            break;
        }
    #elif defined(ITF_OUNCE)
        if (_affinity != Thread_HardwareThread_Unknown)
        {
            coreId = u32(_affinity - Thread_HardwareThread0);
            coreId = coreId % 6;

        }
    #endif

        return coreId;
    }

    u32 Thread::join()
    {
        if (m_created && !m_waited)
        {
            nn::os::WaitThread(&m_osThread);
            m_waited = true;
            return m_returnValue;
        }
        return U32_INVALID;
    }

    static nn::Bit64 computeValidAffinity(u32 affinity)
    {
        static nn::Bit64 availableCores = nn::os::GetThreadAvailableCoreMask();
        if (affinity == 0)
            return availableCores;
        nn::Bit64 adjustedAffinity = static_cast<nn::Bit64>(affinity) & availableCores;
        if (adjustedAffinity == 0)
            adjustedAffinity = 1 << 1;  // Use Core 1 by default (secondary core)
        return adjustedAffinity;
    }

    void Thread::setAffinity(Thread_HardwareThreadId eThreadId)
    {
        m_eThreadAffinity = eThreadId;
        if (m_created)
        {
            int newCore = getCoreIdHW(eThreadId);
            nn::Bit64 mask = computeValidAffinity(newCore);
            nn::os::SetThreadCoreMask(&m_osThread, newCore, mask);
        }
    }

    void Thread::threadMain()
    {
        if (m_settings.m_engineIndex != U32_INVALID)
        {
            if (ThreadSettings::m_settings[m_settings.m_engineIndex].m_useTLS)
            {
                ThreadInfo::m_threadEngineIndex.setValue(m_settings.m_engineIndex);
            }
            ThreadSettings::m_settings[m_settings.m_engineIndex].m_threadID = Synchronize::getCurrentThreadId();
        }

        m_returnValue = m_threadFunc(m_threadFuncParam);
        m_finished = true;
    }
}
