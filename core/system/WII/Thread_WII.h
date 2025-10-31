#ifndef _ITF_THREAD_WII_H_
#define _ITF_THREAD_WII_H_

#include "core/system/WII/Synchronize_WII.h"

//////////////////////////////////////////////////////////////////////////
///
/// Thread manipulation class
///
#define OS_THREAD_SPECIFIC_MAX 2

namespace ITF
{
    class ThreadSettings;

    class String;

	typedef OSThread*  THR_ID;

    ITF_INLINE void sleep( u32 uMilliseconds )
    {
    	OSSleepMilliseconds( uMilliseconds );
    }

    typedef void*  THREAD_HANDLE;
    typedef DWORD   THREAD_INDEX;

    struct sFuncParameter
    {
    public:
        void*                   m_pThreadFunc;
        void*                   m_pThreadFuncParam;
        void*                   m_pThread;
        u32                     m_engineIndex;
   };

	class Thread_LocalStorageManager
	{

	private:
		void CreateLocalStorageManager()
		{
			ITF_MemSet(mab_IndexUsed, 0, mcu32_MaxLocalStorageData * sizeof(u32));
			ITF_MemSet(mau32_DefaultTable, 0, mcu32_MaxLocalStorageData * sizeof(u32));
			OSSetThreadSpecific(0,mau32_DefaultTable); //Main thread table
			Synchronize::createCriticalSection(&LSMutex);		
		}

		void DestroyLocalStorageManager()
		{
			Synchronize::destroyCriticalSection(&LSMutex);
		}

	public:	
		static void CreateSingleton()
		{
			if(mp_Instance == NULL)
			{
				mp_Instance = &m_Instance;
				m_Instance.CreateLocalStorageManager();
			}
		}
		
		static void DestroySingleton()
		{
			if(mp_Instance != NULL)
			{
				m_Instance.DestroyLocalStorageManager();
				mp_Instance = NULL;
			}
		}
		
		static Thread_LocalStorageManager* GetManager()
		{
			ITF_ASSERT_CRASH(mp_Instance != NULL, "Must create singleton first");
			return mp_Instance;
		}
		
		u32 u32_GetFreeIndex()
		{		
			u32 index = (u32)-1;
			
			Synchronize::enterCriticalSection(&LSMutex);
			
			for(u32 i =0; i < mcu32_MaxLocalStorageData; i++)
			{
				if(mab_IndexUsed[i] == bfalse)
				{
					mab_IndexUsed[i] = btrue;
					index = i;
					break;
				}
			}
			
			Synchronize::leaveCriticalSection(&LSMutex);
			
			ITF_ASSERT_MSG(index != (u32)-1, "[Thread Local Storage Manager] Invalid Index");
			return index;
		}
		
		void ReleaseIndex(u32 _index)
		{
			ITF_ASSERT_MSG(_index >= 0 && _index < mcu32_MaxLocalStorageData, "[Thread Local Storage Manager] Invalid Index");
			Synchronize::enterCriticalSection(&LSMutex);
			mab_IndexUsed[_index] = bfalse;
			Synchronize::leaveCriticalSection(&LSMutex);
		}
		
	public:
		static const u32		mcu32_MaxLocalStorageData = 20;

	private:
		static Thread_LocalStorageManager*	mp_Instance;
		static Thread_LocalStorageManager	m_Instance; // The instance cannot be dynamically allocated because allocation manager uses local storage.

		OSMutex					LSMutex;
		bbool 					mab_IndexUsed[mcu32_MaxLocalStorageData];
		u32						mau32_DefaultTable[mcu32_MaxLocalStorageData]; //= Main thread table, should be moved
		
	};


    class Thread
    {
    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
 
        //////////////////////////////////////////////////////////////////////////
        ///
        /// Create and run a thread 
        /// @param pThreadFunc the function called after the thread creation
        /// @param psz_Name name of the thread
        /// @param pThreadFuncParam A pointer to a variable to be passed to the thread
        /// @param _settings The thread's engine settings
        ///
        void create(u32 (WINAPI* pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings);

		~Thread();

        ITF_INLINE  bbool   isActive() const            { return m_Active; }
        ITF_INLINE  void    SetActive(bbool _active)    { m_Active = _active; }

        i32 getPriorityHW(Thread_Priority ePriority);
        i32 getAffinityHW(Thread_HardwareThreadId _affinity);

        void setPriority(Thread_Priority ePriority);
        const Thread_Priority getPriority() const  {return m_ePriority;} 

        void setAffinity(Thread_HardwareThreadId eThreadId);
        const Thread_HardwareThreadId getAffinity() const {return m_eThreadAffinity;}

        const ThreadSettings* getSettings() const {return m_settings;}

	    THR_ID GetThreadId()
	    {
	        return &m_systhread;
	    }

		u32	mau32_LocalStorages[Thread_LocalStorageManager::mcu32_MaxLocalStorageData];
		
    protected:
        u8*                     m_stack;
        OSThread                m_systhread;

        Thread_Priority         m_ePriority;
        Thread_HardwareThreadId m_eThreadAffinity;
        sFuncParameter          m_Parameter;

        bbool                   m_Active;

    private:
        void setSettings(ThreadSettings* _settings) {m_settings = _settings;}
        const ThreadSettings*  m_settings;
    };

    class ThreadLocalStorage
    {
    public:

        ThreadLocalStorage()
        {
            this->mi_Index = Thread_LocalStorageManager::GetManager()->u32_GetFreeIndex();
     		setNULL();
        }


        ThreadLocalStorage(void* _value)
        {
            this->mi_Index = Thread_LocalStorageManager::GetManager()->u32_GetFreeIndex();
            setValue(_value);
        }


        ThreadLocalStorage(u32 _value)
        {
            this->mi_Index = Thread_LocalStorageManager::GetManager()->u32_GetFreeIndex();
            setValue(_value);
        }

        ~ThreadLocalStorage()
        {
            Thread_LocalStorageManager::GetManager()->ReleaseIndex(mi_Index);
            
        }

        const void *pGetValue() const
        {
			u32* array = (u32*)OSGetThreadSpecific(0);
			u32 returned_value = array[mi_Index];
			return (void*) returned_value;
        }

        void *pGetPtrValue() const
        {
 			u32* array = (u32*)OSGetThreadSpecific(0);
			u32 returned_value = array[mi_Index];
			return (void*) returned_value;
        }

        const u32 getValue() const
        {
			u32* array = (u32*)OSGetThreadSpecific(0);
			u32 returned_value = array[mi_Index];
			return (const u32) returned_value; 		
        }

        void setValue( void *_pValue )
        {
			u32* apu32 = (u32*)OSGetThreadSpecific(0);
			apu32[mi_Index] = (u32) _pValue;
        } 


        void setNULL()
        {
			u32* apu32 = (u32*)OSGetThreadSpecific(0);
			apu32[mi_Index] = 0;
        }

        void setValue( u32 _Value )
        {
			u32* apu32 = (u32*)OSGetThreadSpecific(0);
			apu32[mi_Index] = _Value;
        }

    private:

        u32 mi_Index;
    };

} // namespace ITF



#endif // _ITF_THREAD_WII_H_
