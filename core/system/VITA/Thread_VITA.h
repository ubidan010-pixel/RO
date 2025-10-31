#ifndef _ITF_THREAD_PS3_H_
#define _ITF_THREAD_PS3_H_
/*
#include <sys/timer.h>
#include <sys/ppu_thread.h>
*/
namespace ITF
{
    class ThreadSettings;

    ITF_INLINE void sleep( u32 uMilliseconds )
    {
		ITF_ASSERT(0);
      // sys_timer_usleep(uMilliseconds *1000); //translate in micro;
    }

    //typedef sys_ppu_thread_t  THREAD_HANDLE;
	
	typedef u32  THREAD_HANDLE;
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

    
//CASA::TODO::class copied from wii version
	class ThreadLocalStorage
	{
	public:

		ThreadLocalStorage()
		{
			/*this->mi_Index = Thread_LocalStorageManager::GetManager()->u32_GetFreeIndex();
			setNULL();*/
		}


		ThreadLocalStorage(void* _value)
		{
			/*this->mi_Index = Thread_LocalStorageManager::GetManager()->u32_GetFreeIndex();
			setValue(_value);*/
		}


		ThreadLocalStorage(u32 _value)
		{
			/*this->mi_Index = Thread_LocalStorageManager::GetManager()->u32_GetFreeIndex();
			setValue(_value);*/
		}

		~ThreadLocalStorage()
		{
			//Thread_LocalStorageManager::GetManager()->ReleaseIndex(mi_Index);

		}

		const void *pGetValue() const
		{
			/*u32* array = (u32*)OSGetThreadSpecific(0);
			u32 returned_value = array[mi_Index];
			return (void*) returned_value;*/
			return(void*) NULL;
		}

		void *pGetPtrValue() const
		{
			/*u32* array = (u32*)OSGetThreadSpecific(0);
			u32 returned_value = array[mi_Index];
			return (void*) returned_value;*/
			return(void*) NULL;
		}

		const u32 getValue() const
		{
			/*u32* array = (u32*)OSGetThreadSpecific(0);
			u32 returned_value = array[mi_Index];
			return (const u32) returned_value; 		*/
			return 0;
		}

		void setValue( void *_pValue )
		{
			/*u32* apu32 = (u32*)OSGetThreadSpecific(0);
			apu32[mi_Index] = (u32) _pValue;*/
		} 


		void setNULL()
		{
			/*u32* apu32 = (u32*)OSGetThreadSpecific(0);
			apu32[mi_Index] = 0;*/
		}

		void setValue( u32 _Value )
		{
			/*u32* apu32 = (u32*)OSGetThreadSpecific(0);
			apu32[mi_Index] = _Value;*/
		}

	private:

		u32 mi_Index;
	};

} // namespace ITF



#endif // _ITF_THREAD_PS3_H_
