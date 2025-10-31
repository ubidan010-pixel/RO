#ifndef _ITF_THREAD_CTR_H_
#define _ITF_THREAD_CTR_H_


//////////////////////////////////////////////////////////////////////////
///
/// Thread manipulation class
///

namespace ITF
{
    class ThreadSettings;
    class String;

    //////////////////////////////////////////////////////////////////////////
    ///
    /// Wait for the specific time
    /// @param u32 specific the time you want to sleep in milliseconds
    //////////////////////////////////////////////////////////////////////////

    ITF_INLINE void sleep( u32 uMilliseconds )
    {
        nnosThreadSleep( uMilliseconds*1000000 );
    }


    typedef nn::Handle  THREAD_HANDLE;
    //typedef DWORD   THREAD_INDEX;

    struct sFuncParameter
    {
    public:
        u32                     (*m_pThreadFunc)(void*);
        void*                   m_pThreadFuncParam;
        void*                   m_pThread;
        u32                     m_engineIndex;
    };

    class Thread
    {
    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        Thread() : m_stack(NULL),m_systhread(NULL)  {}

        //////////////////////////////////////////////////////////////////////////
        ///
        /// Create and run a thread 
        /// @param pThreadFunc the function called after the thread creation
        /// @param psz_Name name of the thread
        /// @param pThreadFuncParam A pointer to a variable to be passed to the thread
        /// @param _settings The thread's engine settings
        ///
        void create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings);


        i32 getPriorityHW(Thread_Priority ePriority);
        i32 getAffinityHW(Thread_HardwareThreadId _affinity);

        void setPriority(Thread_Priority ePriority);
        const Thread_Priority getPriority() const  {return m_ePriority;} 

        void setAffinity(Thread_HardwareThreadId eThreadId);
        const Thread_HardwareThreadId getAffinity() const {return m_eThreadAffinity;}

        const ThreadSettings* getSettings() const {return m_settings;}


    protected:
        nn::os::StackMemoryBlock *m_stack;
        nn::os::Thread  *m_systhread;

        THREAD_HANDLE   m_Handle;
        //THREAD_INDEX    m_Index;
        Thread_Priority m_ePriority;
        Thread_HardwareThreadId m_eThreadAffinity;
        sFuncParameter  m_Parameter;

    private:
        void setSettings(ThreadSettings* _settings) {m_settings = _settings;}
        const ThreadSettings*  m_settings;
    };


//CASA::TODO::class copied from wii version
	class ThreadLocalStorage
	{
	public:

		ThreadLocalStorage()
		{
			setNULL();
		}


		ThreadLocalStorage(void* _value)
		{
			setValue(_value);
		}


		ThreadLocalStorage(u32 _value)
		{
			setValue(_value);
		}

		~ThreadLocalStorage()
		{
		}

		const void *pGetValue() const
		{
            u32 returned_value = reinterpret_cast<u32>(m_tls.GetValue());
			return(void*) returned_value;
		}

		void *pGetPtrValue() const
		{
            u32 returned_value = reinterpret_cast<u32>(m_tls.GetValue());
			return(void*) returned_value;
		}

		const u32 getValue() const
		{
            u32 returned_value = reinterpret_cast<u32>(m_tls.GetValue());
			return (const u32) returned_value;
		}

		void setValue( void *_pValue )
		{
            m_tls.SetValue(reinterpret_cast<uptr>(_pValue));
		} 


		void setNULL()
		{
            m_tls.SetValue(NULL);
		}

		void setValue( u32 _Value )
		{
            m_tls.SetValue(reinterpret_cast<uptr>(_Value));
		}

	private:

		u32 mi_Index;
        nn::os::ThreadLocalStorage m_tls;
	};


    
} // namespace ITF



#endif // _ITF_THREAD_CTR_H_
