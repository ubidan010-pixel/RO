#ifndef __ERROR_H__
#define __ERROR_H__

#include "adapters/Adapter_WWISE/AudioSDK/types.h"
#include "core/system/Synchronize.h"


namespace ITF
{
    class BaseObject;
}

namespace AudioSDK
{
#ifndef ITF_FINAL
	struct Error
	{
        static void						init();
        static void						term();

		static void log(const char *_format, ...);
		static void fatal_error(const char *_format, ...);
        static void warning(const char *_format, ...);
		static void warning(const ITF::BaseObject *_pOwner, const char *_format, ...);
        
    private:
		static const u32					c_strBufferSize = 512;	////< log for debug/production
		static char							s_strbuffer[c_strBufferSize];
#if defined (ITF_PS5) || (ITF_NX) || (ITF_OUNCE) 
        static ITF::ITF_THREAD_CRITICAL_SECTION* m_csLog;
#else
		static ITF_THREAD_CRITICAL_SECTION	*m_csLog;
#endif
	};
#endif
}


#ifndef ITF_FINAL
#define AUDIO_LOG(...)		            AudioSDK::Error::log(__VA_ARGS__)
#define AUDIO_WARNING(...)	            AudioSDK::Error::warning(__VA_ARGS__)
#define AUDIO_WARNING_OBJ(__obj, ...)   AudioSDK::Error::warning(__obj, __VA_ARGS__)
#define AUDIO_ERROR(...)	            AudioSDK::Error::fatal_error(__VA_ARGS__)
#define AUDIO_ASSERT(__cond, ...)       { if(!(__cond)) AudioSDK::Error::fatal_error(__VA_ARGS__); } 
#define AUDIO_VERIFY(__cond, ...)       { if(!(__cond)) AudioSDK::Error::fatal_error(__VA_ARGS__); } 
#define WWISE_GLUE2( a, b)              a##b
#define WWISE_GLUE( a, b)               WWISE_GLUE2( a, b)
#define AUDIO_STATIC_ASSERT(__expr__, __msg__)	typedef char   WWISE_GLUE(staticassert_, __LINE__)[(__expr__) ? 1 : -1]
#else
#define AUDIO_LOG(...)				    ((void)0)
#define AUDIO_WARNING(...)			    ((void)0)
#define AUDIO_WARNING_OBJ(__obj, ...)   ((void)0)
#define AUDIO_ERROR(...)			    ((void)0)
#define AUDIO_ASSERT(__cond, ...)	    ((void)0)
#define AUDIO_VERIFY(__cond, ...)       (__cond)
#define AUDIO_STATIC_ASSERT(__expr__, __msg__)	
#endif

#endif // __ERROR_H__
