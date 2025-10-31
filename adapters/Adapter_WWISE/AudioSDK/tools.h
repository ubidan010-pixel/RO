#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "adapters/Adapter_WWISE/AudioSDK/types.h"
#include <string.h>

namespace	AudioSDK
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	template<u32 SIZE> inline u32 roundUp(u32 _x)
	{
		return((_x + SIZE - 1) &~(SIZE - 1));
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	template<class T, size_t N> inline size_t getArraySize(T(&)[N]) {
			return N;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	inline void safeStringCopy(char *_out, size_t _SIZE, const char *_in)
	{

#if defined(ITF_WINDOWS) || defined(ITF_X360)|| defined(ITF_XBOX_SERIES)
        strcpy_s(_out, _SIZE, _in);
#else
        strncpy(_out, _in, _SIZE);
#endif
	}

	void	safeStringCopy(wchar_t *_out, size_t _SIZE, const char *_in);

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	inline void safeStringCopy(wchar_t *_out, size_t _SIZE, const wchar_t *_in)
	{

#if defined(ITF_WINDOWS) || defined(ITF_X360)|| defined(ITF_XBOX_SERIES)
        wcscpy_s(_out, _SIZE, _in);
#else
        wcscpy(_out, _in);
#endif
    }

	void	safeStringCopy(char *_out, size_t _SIZE, const wchar_t *_in);

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	inline void safeStringCat(char *_out, size_t _SIZE, const char *_in)
	{

#if defined(ITF_WINDOWS) || defined(ITF_X360)|| defined(ITF_XBOX_SERIES)
        strcat_s(_out, _SIZE, _in);
#else
        strcat(_out, _in);
#endif

	}

	void	safeStringCat(wchar_t *_out, size_t _SIZE, const char *_in);

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	inline void safeStringCat(wchar_t *_out, size_t _SIZE, const wchar_t *_in)
	{

#if defined(ITF_WINDOWS) || defined(ITF_X360)|| defined(ITF_XBOX_SERIES)
        wcscat_s(_out, _SIZE, _in);
#else
        wcscat(_out, _in);
#endif
	}

	void	safeStringCat(char *_out, size_t _SIZE, const wchar_t *_in);

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	template<size_t _SIZE, typename _OUT_CHAR_TYPE, typename _IN_CHAR_TYPE> inline void safeStringCopy(_OUT_CHAR_TYPE(&_out)[_SIZE], const _IN_CHAR_TYPE * _in)
	{


		safeStringCopy(_out, _SIZE, (const wchar_t*)_in);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	template<size_t _SIZE, typename _OUT_CHAR_TYPE, typename _IN_CHAR_TYPE> inline void safeStringCat(_OUT_CHAR_TYPE(&_out)[_SIZE], const _IN_CHAR_TYPE * _in)
	{
		safeStringCat(_out, _SIZE, _in);
	}

    template<typename T> inline void stringReplace( T* in_pstring, T _find, T _replace )
    {
        while( *in_pstring != 0 )
        {
            if( *in_pstring == _find )
            {
                *in_pstring = _replace;
            }
            ++in_pstring;
        }
    }
}
#endif // __TOOLS_H__
