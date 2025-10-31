#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/AudioSDK/error.h"

#include "core/error/ErrorHandler.h"
#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

namespace	AudioSDK
{
#ifndef ITF_FINAL
#if defined (ITF_PS5) || (ITF_NX) || (ITF_OUNCE) 
	ITF::ITF_THREAD_CRITICAL_SECTION *Error::	m_csLog = NULL;
#else
    ITF_THREAD_CRITICAL_SECTION* Error::m_csLog = NULL;
#endif
	char Error::							s_strbuffer[c_strBufferSize];

#if _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
#define MY_VSPRINTF(__error_str_buffer, __ErrorBufferSize, __format,  __args)	::_vsprintf_s_l(__error_str_buffer, __ErrorBufferSize, __format, NULL, __args)
#else
#define MY_VSPRINTF(__error_str_buffer, __ErrorBufferSize, __format, __args)	::vsprintf(__error_str_buffer, __format, __args)
#endif

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Error::init()
	{
#if defined (ITF_PS5) || (ITF_NX) || (ITF_OUNCE)
        m_csLog = WWISE_NEW(ITF::ITF_THREAD_CRITICAL_SECTION);
#else
        m_csLog = WWISE_NEW(ITF_THREAD_CRITICAL_SECTION);
#endif

		ITF::Synchronize::createCriticalSection(m_csLog);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Error::term()
	{
		ITF::Synchronize::destroyCriticalSection(m_csLog);
		WWISE_DELETE(m_csLog);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Error::log(const char *_format, ...)
	{
		{
			ITF::csAutoLock	cs(m_csLog);

			va_list		args;
			va_start(args, _format);
			MY_VSPRINTF(s_strbuffer, c_strBufferSize, _format, args);
			va_end(args);
		}

		LOG(s_strbuffer);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Error::fatal_error(const char *_format, ...)
	{
		{
			ITF::csAutoLock	cs(m_csLog);

			va_list		args;
			va_start(args, _format);
			MY_VSPRINTF(s_strbuffer, c_strBufferSize, _format, args);
			va_end(args);
		}

		ITF_ERROR(s_strbuffer);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Error::warning(const char *_format, ...)
	{
		{
			ITF::csAutoLock	cs(m_csLog);

			va_list		args;
			va_start(args, _format);
			MY_VSPRINTF(s_strbuffer, c_strBufferSize, _format, args);
			va_end(args);
		}

        using namespace ITF;
		ITF_WARNING(NULL, 0, s_strbuffer);
	}

    void Error::warning(const ITF::BaseObject *_pOwner, const char *_format, ...)
    {
        {
            ITF::csAutoLock	cs(m_csLog);

            va_list		args;
            va_start(args, _format);
            MY_VSPRINTF(s_strbuffer, c_strBufferSize, _format, args);
            va_end(args);
        }

        using namespace ITF;
        ITF_WARNING_CATEGORY(Sound, _pOwner, 0, s_strbuffer);
    }
#endif
}
#endif
