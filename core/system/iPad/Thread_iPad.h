#ifndef _ITF_THREAD_IPAD_H_
#define _ITF_THREAD_IPAD_H_

namespace ITF
{
	class ThreadSettings;
	class String;

	ITF_INLINE void sleep(u32 uMilliseconds)
	{
		
	}


	typedef pthread_t	THREAD_HANDLE;
	typedef int			THREAD_INDEX;

	class FuncParameter
    {
		public:
			u32		(*m_pThreadFunc)(void*);
			void*	m_pThreadFuncParam;
    };

	class Thread
	{
		public:
			void	create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings);
			void	setPriority(Thread_Priority ePriority);
		   
			Thread_HardwareThreadId getAffinity()  const
									{
										return m_eThreadAffinity;
									}
		
			void	setAffinity(Thread_HardwareThreadId eThreadId) 
					{
						m_eThreadAffinity = eThreadId;
					}
		
			const Thread_Priority	getPriority() const  
									{
										return m_ePriority;
									}
 		
			const ThreadSettings*	getSettings() const 
									{
										return m_settings;
									}

		protected:
			THREAD_HANDLE			m_Handle;
			THREAD_INDEX			m_Index;
			FuncParameter			m_Parameter;
			Thread_Priority			m_ePriority;
			Thread_HardwareThreadId	m_eThreadAffinity;

		private:
			const	ThreadSettings*  m_settings;
	};
} // namespace ITF

#endif // _ITF_THREAD_IPAD_H_
