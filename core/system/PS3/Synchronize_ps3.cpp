#include "precompiled_core.h"

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

namespace ITF
{
    #define GetCS(cs)   (cs)

    void Synchronize::createCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        sys_lwmutex_attribute_t attr;
        attr.attr_protocol = SYS_SYNC_PRIORITY;
        attr.attr_recursive = SYS_SYNC_RECURSIVE;
		sys_lwmutex_t * mutex = (sys_lwmutex_t *) Memory::malloc(sizeof(sys_lwmutex_t));
        sys_lwmutex_create(mutex, &attr);
        *cs = (i32)mutex;
    }

    void Synchronize::destroyCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        sys_lwmutex_destroy((sys_lwmutex_t *)(*cs));
        Memory::free((void*)*cs);
        cs = NULL;
    }

    void Synchronize::enterCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        sys_lwmutex_lock((sys_lwmutex_t *)*cs, 0);
    }

    void Synchronize::leaveCriticalSection(ITF_THREAD_CRITICAL_SECTION *cs)
    {
        sys_lwmutex_unlock((sys_lwmutex_t *)*cs);
    }

    void Synchronize::createSemaphore(ITF_THREAD_SEMAPHORE * pSemaphore, const char * szName, u32 uInitialCount, u32 uMaximunCount)
    {
        if (szName == NULL)
            szName = "no_name";
        sys_semaphore_attribute_t attr;

        sys_semaphore_attribute_initialize(attr);
        sys_semaphore_attribute_name_set(attr.name, szName);
        sys_semaphore_create(pSemaphore, &attr, uInitialCount, uMaximunCount);
    }

    void Synchronize::increaseSemaphore(ITF_THREAD_SEMAPHORE *pSemaphore, u32 cpt)
    {
        sys_semaphore_post(*pSemaphore, cpt);
    }

    void Synchronize::destroySemaphore(ITF_THREAD_SEMAPHORE *pSemaphore)
    {
        sys_semaphore_destroy(*pSemaphore);
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
        sys_ppu_thread_t thread_id;
        sys_ppu_thread_get_id(&thread_id);
        return thread_id;
    }
 
    void ITF_THREAD_EVENT::init( const char *szName, bbool bAutoReset)
    {
        if (szName==NULL)
            szName = "no_name";
        sys_lwmutex_attribute_t mutex_attr;
        sys_lwmutex_attribute_initialize(mutex_attr);

        strncpy( mutex_attr.name, szName, sizeof( mutex_attr.name ) - 2 );
        mutex_attr.name[ sizeof( mutex_attr.name ) - 1 ] = 0;

        mutex_attr.attr_protocol = SYS_SYNC_PRIORITY;
        mutex_attr.attr_recursive = SYS_SYNC_RECURSIVE;
        i32 err = sys_lwmutex_create(&_mutex, &mutex_attr);
        ITF_ASSERT( err == CELL_OK);
        sys_lwcond_attribute_t cond_attr;
        sys_lwcond_attribute_initialize(cond_attr);

        strncpy( cond_attr.name, szName, sizeof( cond_attr.name ) - 2 );
        cond_attr.name[ sizeof( cond_attr.name ) - 1 ] = 0;

        m_signaled  = btrue;
        m_autoreset = bAutoReset;
        err = sys_lwcond_create(&_cond, &_mutex, &cond_attr ); 
        ITF_ASSERT( err == CELL_OK);
    }

    void ITF_THREAD_EVENT::set()
    { 
        i32 err = sys_lwmutex_lock( &_mutex, 0 );
        ITF_ASSERT( err == CELL_OK);
        m_signaled = btrue;
        if( m_autoreset )
            sys_lwcond_signal( &_cond );
        else
            sys_lwcond_signal_all( &_cond );

        err = sys_lwmutex_unlock( &_mutex );
        ITF_ASSERT( err == CELL_OK);
    }

    void ITF_THREAD_EVENT::reset()
    { 
        i32 err = sys_lwmutex_lock( &_mutex, 0 );
        ITF_ASSERT( err == CELL_OK);
        m_signaled = bfalse;

        err = sys_lwmutex_unlock( &_mutex );
        ITF_ASSERT( err == CELL_OK);
    }

    void ITF_THREAD_EVENT::wait()
    {
        i32 err = sys_lwmutex_lock( &_mutex, 0 );
        ITF_ASSERT( err == CELL_OK);
        ITF_MemoryBarrier();
        while( !m_signaled )
        {
            i32 err = sys_lwcond_wait( &_cond, 0 );
            ITF_ASSERT( err == CELL_OK);
        }
        if( m_autoreset )
            m_signaled = bfalse;

         err = sys_lwmutex_unlock( &_mutex );
         ITF_ASSERT( err == CELL_OK);
    }

    // VThread

    VThread::VThread()
        : m_thread(u64(-1))
    {
    }

    VThread::~VThread()
    {
        if (m_thread != u64(-1))
        {
            waitForExit();
        }
    }

    void VThread::threadStub(u64 _param)
    {
        VThread * tHis = (VThread *)_param;
        u32 ret = tHis->run();
        sys_ppu_thread_exit(u64(ret));
    }

    bool VThread::start(const char * _name, u32 _prio, u32 _stackSize)
    {
        return CELL_OK == sys_ppu_thread_create(&m_thread,
                                                threadStub,
                                                u64(this),
                                                _prio,
                                                _stackSize,
                                                SYS_PPU_THREAD_CREATE_JOINABLE,
                                                _name?_name:"No Name");
    }

    u32 VThread::waitForExit()
    {
        ITF_ASSERT(m_thread != u64(-1));
        if (m_thread != u64(-1))
        {
            uint64_t exitStatus;
            ITF_VERIFY(CELL_OK == sys_ppu_thread_join(m_thread, &exitStatus));
            m_thread = u64(-1);
            return u32(exitStatus);
        }
        return u32(-1);
    }
}



