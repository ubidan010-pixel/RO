#include "precompiled_core.h"

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

namespace ITF
{
    TLS u32 ThreadInfo::m_threadEngineIndex  = 0;
	
	static void *ThreadMain(void* _params)
	{
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		FuncParameter* params = reinterpret_cast<FuncParameter*>(_params);
		params->m_pThreadFunc(params->m_pThreadFuncParam);
		[pool release];
		pthread_exit(NULL);
	}
    
	void Thread::create(u32 (WINAPI *pThreadFunc)(void*), void *pThreadFuncParam, ThreadSettings& _settings)
	{
		m_Parameter.m_pThreadFunc = pThreadFunc;
		m_Parameter.m_pThreadFuncParam = pThreadFuncParam;
		
		pthread_create(&m_Handle, NULL, ThreadMain, &m_Parameter);
		
		m_settings = &_settings;
		m_ePriority = _settings.m_priority;
	}
	
	void Thread::setPriority(Thread_Priority ePriority)
	{
		ITF_ASSERT_MSG(0, "Not implemented on iPad");
	}
} // namespace ITF

