#pragma once

#include "core/system/Thread.h"

#include "core/types.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
///
/// Thread manipulation class
///

namespace ITF
{


    class ThreadLocalStorage
    {
    public:

        ThreadLocalStorage()
        {
            this->mi_Index = TlsAlloc();
            ITF_ASSERT( mi_Index != TLS_OUT_OF_INDEXES );
            ::TlsSetValue( mi_Index, NULL );
        }


        ThreadLocalStorage(void* _value)
        {
            this->mi_Index = TlsAlloc();
            ITF_ASSERT( mi_Index != TLS_OUT_OF_INDEXES );
            setValue(_value);
        }


        ThreadLocalStorage(u32 _value)
        {
            this->mi_Index = TlsAlloc();
            ITF_ASSERT( mi_Index != TLS_OUT_OF_INDEXES );
            setValue(_value);
        }

        ~ThreadLocalStorage()
        {
            ITF_ASSERT( mi_Index != TLS_OUT_OF_INDEXES );
            ::TlsFree( mi_Index );
        }

        const void *pGetValue() const
        {
            return ::TlsGetValue( mi_Index );
        }

        void *pGetPtrValue() const
        {
            return ::TlsGetValue( mi_Index );
        }

        const u32 getValue() const
        {
            return u32(uPtr(::TlsGetValue( mi_Index)));
        }

        void setValue( void *_pValue )
        {
            ::TlsSetValue( mi_Index, _pValue );
        } 


        void setNULL()
        {
            ::TlsSetValue( mi_Index, 0 );
        }

        void setValue( u32 _Value )
        {
            ::TlsSetValue( mi_Index, (void*)uPtr(_Value));
        }

    private:

        u32	mi_Index;
    };



    class ThreadSettings;
    class String;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Wait for the specific time
    /// @param u32 specific the time you want to sleep in milliseconds
    //////////////////////////////////////////////////////////////////////////

    ITF_INLINE void sleep( u32 uMilliseconds )
    {
        Sleep( uMilliseconds );
    }


    typedef HANDLE  THREAD_HANDLE;
    typedef DWORD   THREAD_INDEX;

    struct sFuncParameter
    {
    public:
        void*                   m_pThreadFunc;
        void*                   m_pThreadFuncParam;
        void*                   m_pThread;
        u32                     m_engineIndex;
    };

    class Thread
    {
    public:

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Create and run a thread 
        /// @param pThreadFunc the function called after the thread creation
        /// @param psz_Name name of the thread
        /// @param pThreadFuncParam A pointer to a variable to be passed to the thread
        /// @param _settings The thread's engine settings
        ///
        void create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings);


        u32 getPriorityHW(Thread_Priority ePriority);

        void setPriority(Thread_Priority ePriority);
        const Thread_Priority getPriority() const  {return m_ePriority;} 

        void setAffinity(Thread_HardwareThreadId eThreadId);
        const Thread_HardwareThreadId getAffinity() const {return m_eThreadAffinity;}

        const ThreadSettings* getSettings() const {return m_settings;}

        // Windows specific
        static bool SetThreadName(THREAD_HANDLE dwThreadID, const char* szThreadName);
        static bool SetCurrentThreadName(const char* szThreadName);

    protected:
        THREAD_HANDLE   m_Handle;
        THREAD_INDEX    m_Index;
        Thread_Priority m_ePriority;
        Thread_HardwareThreadId m_eThreadAffinity;
        sFuncParameter  m_Parameter;

    private:
        void setSettings(ThreadSettings* _settings) {m_settings = _settings;}
        const ThreadSettings*  m_settings;
    };

    
} // namespace ITF

