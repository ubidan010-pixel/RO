#include "precompiled_Adapter_WWISE.h"
#include "adapters/Adapter_WWISE/AudioSDK/tools.h"
#include "AK/Tools/Common/AkPlatformFuncs.h"

namespace	AudioSDK
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void safeStringCopy(wchar_t *_out, size_t _SIZE, const char *_in)
	{
		AKPLATFORM::AkCharToWideChar(_in, (AkUInt32)_SIZE, _out);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void safeStringCopy(char *_out, size_t _SIZE, const wchar_t *_in)
	{
		AKPLATFORM::AkWideCharToChar(_in, (AkUInt32)_SIZE, _out);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void safeStringCat(wchar_t *_out, size_t _SIZE, const char *_in)
	{
		const size_t	outSize = wcslen(_out);
		const size_t	remainSize = _SIZE - outSize;
		if(remainSize > 0)
		{
			AKPLATFORM::AkCharToWideChar(_in, (AkUInt32)remainSize, (_out + outSize));
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void safeStringCat(char *_out, size_t _SIZE, const wchar_t *_in)
	{
		const size_t outSize = strlen(_out);
		const size_t remainSize = _SIZE - outSize;
		if(remainSize > 0)
		{
			AKPLATFORM::AkWideCharToChar(_in, (AkUInt32)remainSize, (_out + outSize));
		}
	}
}
