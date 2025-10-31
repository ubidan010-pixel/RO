#ifndef _ITF_THREAD_PS3_H_
#define _ITF_THREAD_PS3_H_

#include <sys/timer.h>
#include <sys/ppu_thread.h>
#include <cell/atomic.h>


namespace ITF
{
    class ThreadSettings;

    ITF_INLINE void sleep( u32 uMilliseconds )
    {
        if (uMilliseconds == 0)
            sys_ppu_thread_yield();
        else
            sys_timer_usleep(uMilliseconds *1000); //translate in micro;
    }

    ITF_INLINE void YieldProcessor()
    {
        sys_ppu_thread_yield();
    }

    typedef sys_ppu_thread_t  THREAD_HANDLE;
    typedef DWORD   THREAD_INDEX;

    typedef DWORD (*PTHREAD_START_ROUTINE)(void* lpThreadParameter);
    typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

    struct sFuncParameter
    {
    public:
        LPTHREAD_START_ROUTINE  m_pThreadFunc;
        void*                   m_pThreadFuncParam;
        class Thread *          m_pThread;
        u32                     m_engineIndex;
    };

    class Thread
    {
    public:
        
    void create(u32 (* pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings);


    u32 getPriorityHW(Thread_Priority ePriority);

    void setAffinity(Thread_HardwareThreadId eThreadId) {m_eThreadAffinity = eThreadId;};
    Thread_HardwareThreadId getAffinity()  const   {return m_eThreadAffinity;}

    Thread_Priority getPriority() const  {return m_ePriority;} 

    const ThreadSettings* getSettings() const {return m_settings;}

    protected:
        THREAD_HANDLE   m_Handle;
        THREAD_INDEX    m_Index;

        sFuncParameter  m_Parameter;
        Thread_Priority m_ePriority;
        Thread_HardwareThreadId m_eThreadAffinity;

    private:
        void setSettings(ThreadSettings* _settings) {m_settings = _settings;}
        const ThreadSettings*  m_settings;
    };


    ///////////////////////////////////////
    // Thread local storage functionalities
    ///////////////////////////////////////

    class ThreadLocalStorage
    {
    private:
        enum
        {
            Max_TLS_Slots = 20
        };

        static __thread void * m_threadLocalStorage[Max_TLS_Slots];
        static u32 m_currentIndex;
    public:

        ThreadLocalStorage()
        {
            m_index = cellAtomicIncr32(&m_currentIndex);
            ITF_ASSERT( m_index < Max_TLS_Slots );
        }

        ThreadLocalStorage(void* _value)
        {
            m_index = cellAtomicIncr32(&m_currentIndex);
            ITF_ASSERT( m_index < Max_TLS_Slots );
            setValue(_value);
        }


        ThreadLocalStorage(u32 _value)
        {
            m_index = cellAtomicIncr32(&m_currentIndex);
            ITF_ASSERT( m_index < Max_TLS_Slots );
            setValue(_value);
        }

        ~ThreadLocalStorage()
        {
            // TODO: do a proper implementation and reuse unused slots
            ITF_ASSERT( m_index < Max_TLS_Slots );
        }


        const void *pGetValue() const
        {
            ITF_ASSERT( m_index < Max_TLS_Slots );
            return m_threadLocalStorage[m_index];
        }

        void * pGetPtrValue() const
        {
            ITF_ASSERT( m_index < Max_TLS_Slots );
            return m_threadLocalStorage[m_index];
        }

        u32 getValue() const
        {
            ITF_ASSERT( m_index < Max_TLS_Slots );
            return u32(m_threadLocalStorage[m_index]);
        }

        void setValue( void *_pValue )
        {
            ITF_ASSERT( m_index < Max_TLS_Slots );
            m_threadLocalStorage[m_index] = _pValue;
        } 

        void setNULL()
        {
            ITF_ASSERT( m_index < Max_TLS_Slots );
            m_threadLocalStorage[m_index] = 0;
        }

        void setValue( u32 _value )
        {
            ITF_ASSERT( m_index < Max_TLS_Slots );
            m_threadLocalStorage[m_index] = reinterpret_cast<void*>(_value);
        }

    private:

        i32	m_index;
    };
    
} // namespace ITF



#endif // _ITF_THREAD_PS3_H_
