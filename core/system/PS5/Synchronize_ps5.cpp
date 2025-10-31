#include "precompiled_core.h"

#include "core/system/Synchronize.h"

namespace ITF
{
    // FastMutex
    ITF_THREAD_FASTMUTEX::ITF_THREAD_FASTMUTEX()
    {
        ScePthreadMutexattr mutex_atributes;
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrInit(&mutex_atributes));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSettype(&mutex_atributes, SCE_PTHREAD_MUTEX_RECURSIVE));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSetprotocol(&mutex_atributes, SCE_PTHREAD_PRIO_INHERIT));
        ITF_VERIFY_SCE_CALL(scePthreadMutexInit(&m_lwMutex, &mutex_atributes, nullptr));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrDestroy(&mutex_atributes));
    }

    ITF_THREAD_FASTMUTEX::~ITF_THREAD_FASTMUTEX()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexDestroy(&m_lwMutex));
    }

    void ITF_THREAD_FASTMUTEX::lock()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexLock(&m_lwMutex));
    }

    void ITF_THREAD_FASTMUTEX::unlock()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexUnlock(&m_lwMutex));
    }


    // Mutex
    Mutex::Mutex(const char* _debugName)
    {
        ScePthreadMutexattr mutex_atributes;
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrInit(&mutex_atributes));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSettype(&mutex_atributes, SCE_PTHREAD_MUTEX_RECURSIVE));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSetprotocol(&mutex_atributes, SCE_PTHREAD_PRIO_INHERIT));

        char truncatedName[32] = "no_name";
        if (_debugName != nullptr)
        {
            strncpy(truncatedName, _debugName, ITF_ARRAY_SIZE(truncatedName));
            truncatedName[ITF_ARRAY_SIZE(truncatedName) - 1] = 0;
        }

        ITF_VERIFY_SCE_CALL(scePthreadMutexInit(&m_lwMutex, &mutex_atributes, truncatedName));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrDestroy(&mutex_atributes));
    }

    Mutex::~Mutex()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexDestroy(&m_lwMutex));
    }

    void Mutex::lock()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexLock(&m_lwMutex));
    }

    bool Mutex::tryLock()
    {
        int ret = scePthreadMutexTrylock(&m_lwMutex);
        ITF_ASSERT(ret == SCE_OK || ret == SCE_KERNEL_ERROR_EBUSY);
        return SCE_OK == ret;
    }

    void Mutex::unlock()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexUnlock(&m_lwMutex));
    }

    // Synchronize
    void Synchronize::createCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ScePthreadMutexattr mutex_atributes;
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrInit(&mutex_atributes));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSettype(&mutex_atributes, SCE_PTHREAD_MUTEX_RECURSIVE));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSetprotocol(&mutex_atributes, SCE_PTHREAD_PRIO_INHERIT));
        ITF_VERIFY_SCE_CALL(scePthreadMutexInit(cs, &mutex_atributes, nullptr));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrDestroy(&mutex_atributes));
    }

    void Synchronize::destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexDestroy(cs));
        *cs = nullptr;
    }

    void Synchronize::enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexLock(cs));
    }
    
    bbool Synchronize::tryEnterCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        i32 ret = SCE_OK;
        ITF_VERIFY_SCE_CALL(ret = scePthreadMutexTrylock(cs));
        return ret == SCE_OK;
    }

    void Synchronize::leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexUnlock(cs));
    }

    void Synchronize::createEvent(ITF_THREAD_EVENT *pEvent, const char *szName, bbool bAutoReset)
    {
        pEvent->init(szName,  bAutoReset);
    }

    void Synchronize::resetEvent(ITF_THREAD_EVENT *pEvent)
    {
        pEvent->reset();
    }

    void Synchronize::setEvent(ITF_THREAD_EVENT *pEvent)
    {
        pEvent->set();
    }

    void Synchronize::destroyEvent(ITF_THREAD_EVENT *pEvent)
    {
        pEvent->destroy();
    }

    void Synchronize::waitEvent(ITF_THREAD_EVENT* pEvent)
    {
         pEvent->wait();
    }

    ITF_THREAD_ID Synchronize::getCurrentThreadId(void)
    {
        ITF_THREAD_ID threadId = reinterpret_cast<ITF_THREAD_ID>(scePthreadSelf());
        return threadId;
    }
 
    void ITF_THREAD_EVENT::init(const char *szName, bbool bAutoReset)
    {
        ITF_VERIFY_SCE_CALL(scePthreadCondInit(&_cond, nullptr, nullptr));

        m_autoreset = bAutoReset;

        ScePthreadMutexattr mutex_atributes;
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrInit(&mutex_atributes));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSettype(&mutex_atributes, SCE_PTHREAD_MUTEX_RECURSIVE));
        ITF_VERIFY_SCE_CALL(scePthreadMutexattrSetprotocol(&mutex_atributes, SCE_PTHREAD_PRIO_INHERIT));

        char truncatedName[32] = "no_name";
        if (szName != nullptr)
        {
            strncpy(truncatedName, szName, ITF_ARRAY_SIZE(truncatedName));
            truncatedName[ITF_ARRAY_SIZE(truncatedName) - 1] = 0;
        }
        ITF_VERIFY_SCE_CALL(scePthreadMutexInit(&m_mutex, &mutex_atributes, truncatedName));

        ITF_VERIFY_SCE_CALL(scePthreadMutexattrDestroy(&mutex_atributes));
    }

    void ITF_THREAD_EVENT::destroy()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexDestroy(&m_mutex));
    }

    void ITF_THREAD_EVENT::set()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexLock(&m_mutex));
        m_signaled = btrue;
        if (m_autoreset)
        {
            ITF_VERIFY_SCE_CALL(scePthreadCondSignal(&_cond));
        }
        else
        {
            ITF_VERIFY_SCE_CALL(scePthreadCondBroadcast(&_cond));
        }

        ITF_VERIFY_SCE_CALL(scePthreadMutexUnlock(&m_mutex));
    }

    void ITF_THREAD_EVENT::reset()
    { 
        ITF_VERIFY_SCE_CALL(scePthreadMutexLock(&m_mutex));
        m_signaled = bfalse;

        ITF_VERIFY_SCE_CALL(scePthreadMutexUnlock(&m_mutex));
    }

    void ITF_THREAD_EVENT::wait()
    {
        ITF_VERIFY_SCE_CALL(scePthreadMutexLock(&m_mutex));
        ITF_MemoryBarrier();
        while (!m_signaled)
        {
            ITF_VERIFY_SCE_CALL(scePthreadCondWait(&_cond, &m_mutex));
        }
        if (m_autoreset)
            m_signaled = bfalse;

        ITF_VERIFY_SCE_CALL(scePthreadMutexUnlock(&m_mutex));
    }

    // CondVariable
    CondVariable::CondVariable(const char* _dbgName)
    {
        ScePthreadCondattr condition_attr;
        scePthreadCondattrInit(&condition_attr);
        char truncatedName[32] = "no_name";
        if (_dbgName != nullptr)
        {
            strncpy(truncatedName, _dbgName, ITF_ARRAY_SIZE(truncatedName));
            truncatedName[ITF_ARRAY_SIZE(truncatedName) - 1] = 0;
        }
        ITF_VERIFY(SCE_OK == scePthreadCondInit(&m_cond, &condition_attr, truncatedName));
    }

    CondVariable::~CondVariable()
    {
        ITF_VERIFY(SCE_OK == scePthreadCondDestroy(&m_cond));
    }

    void CondVariable::signal() // wake up only 1 waiting thread
    {
        ITF_VERIFY(SCE_OK == scePthreadCondSignal(&m_cond));
    }

    void CondVariable::broadcast() // wake up all waiting threads and let them fight on the mutex
    {
        ITF_VERIFY(SCE_OK == scePthreadCondBroadcast(&m_cond));
    }

    bool CondVariable::wait(u32 _ulMicroSec)
    {
        int ret = scePthreadCondWait(&m_cond, &m_mutex.GetOSMutexObject());
        ITF_ASSERT((ret == SCE_OK) || (ret == SCE_KERNEL_ERROR_ETIMEDOUT));
        return ret == SCE_OK;
    }


    // VThread
    VThread::VThread()
        : m_thread(nullptr)
    {
    }

    VThread::~VThread()
    {
        if (nullptr != m_thread)
        {
            waitForExit();
        }
    }

    void* VThread::threadStub(void* _param)
    {
        VThread * threadObj = reinterpret_cast<VThread *>(_param);
        u32 ret = threadObj->run();
        
        return reinterpret_cast<void*>(ret);
    }

    bool VThread::start(const char * _name, u32 _prio, u32 _stackSize)
    {
        ScePthreadAttr thread_atributes;
        ITF_VERIFY_SCE_CALL(scePthreadAttrInit(&thread_atributes));
        ITF_VERIFY_SCE_CALL(scePthreadAttrSetstacksize(&thread_atributes, _stackSize));
        ITF_VERIFY_SCE_CALL(scePthreadSetprio(m_thread, (i32)_prio));

        char truncatedName[32] = "no_name";
        if (_name != nullptr)
        {
            strncpy(truncatedName, _name, ITF_ARRAY_SIZE(truncatedName));
            truncatedName[ITF_ARRAY_SIZE(truncatedName)-1] = 0;
        }

        int res = SCE_OK;
        ITF_VERIFY_SCE_CALL(res = scePthreadCreate(&m_thread, &thread_atributes, threadStub, reinterpret_cast<void*>(this), truncatedName));

        ITF_VERIFY_SCE_CALL(scePthreadAttrDestroy(&thread_atributes));

        return res == SCE_OK;
    }

    u32 VThread::waitForExit()
    {
        ITF_ASSERT(nullptr != m_thread);
        if (nullptr != m_thread)
        {
            void* returnValue = nullptr;
            ITF_VERIFY_SCE_CALL(scePthreadJoin(m_thread, &returnValue));
            m_thread = nullptr;
            return static_cast<u32>(reinterpret_cast<uPtr>(returnValue));
        }
        return u32(-1);
    }
}



