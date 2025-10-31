#ifndef __WWISE_HELPER_H__
#define __WWISE_HELPER_H__

#ifdef ITF_SUPPORT_WWISE
#include "AK/SoundEngine/Common/AkTypes.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "core/system/Synchronize.h"
#include "AK/Tools/Common/AkMonitorError.h"

namespace	ITF
{
	class		ObjectRef;

	namespace	Wwise
    {
	    namespace	Helper
        {

	        //////////////////////////////////////////////////////////////////////////
	        //
	        const char					*getAkErrorCodeString(AK::Monitor::ErrorCode _errorCode);
	        const char					*getAkResultString(AKRESULT _akResult);

	        //////////////////////////////////////////////////////////////////////////
	        //
	        const AkOSChar				*getUAFDataRoot();
	        const AkOSChar				*getTempDir();

	        //////////////////////////////////////////////////////////////////////////
	        //
	        enum Language
	        {
		        ARABIC				= 0,
		        BULGARIAN,
		        CHINESE_HK,CHINESE_PRC,CHINESE_TAIWAN,
		        CZECH,
		        DANISH,
		        DUTCH,
		        ENGLISH_AUSTRALIA,ENGLISH_INDIA,ENGLISH_UK,ENGLISH_US,
		        FINNISH,
		        FRENCH_CANADA,FRENCH_FRANCE,
                GERMAN,
		        GREEK,
		        HEBREW,
		        HUNGARIAN,
		        INDONESIAN,
		        ITALIAN,
		        JAPANESE,
		        KOREAN,
		        LATIN,
		        NORWEGIAN,
		        POLISH,
		        PORTUGUESE_BRAZIL,PORTUGUESE_PORTUGAL,
		        ROMANIAN,
		        RUSSIAN,
		        SLOVENIAN,
		        SPANISH_MEXICO,SPANISH_SPAIN,SPANISH_US,
		        SWEDISH,
		        TURKISH,
		        UKRAINIAN,
		        VIETNAMESE,

		        LANGUAGE_COUNT,
		        INVALID_LANGUAGE
	        };

            const AkOSChar	*getWwiseLanguageName(Language _language);
            Language        getLanguageFromWwiseLanguageName(const AkOSChar* _languageName);
            Language	    staticCast(ITF_LANGUAGE _itfLang);
            ITF_LANGUAGE	staticCast(Language _wwiseLang);


	        //////////////////////////////////////////////////////////////////////////
	        //
	        enum Platform
	        {
		        WINDOWS				= 0,
		        MAC,
		        IOS,
		        XBOX360,
		        PS3,
		        WII,
		        WIIU,
		        ANDROIDS,
		        VITASW,
		        VITAHW,
		        NINTENDO3DS,
                PS4,
                XBOXONE,
                PS5,
                NX,
                OUNCE,
                XBOXSX,
		        PLATFORM_COUNT,
		        INVALID_PLATFORM,

                #if defined ITF_PS3
		        CURRENT_PLATFORM = PS3
                #elif defined ITF_X360
		        CURRENT_PLATFORM = XBOX360
                #elif defined ITF_CAFE
		        CURRENT_PLATFORM = WIIU
                #elif defined ITF_WII
		        CURRENT_PLATFORM = WII
                #elif defined ITF_VITA
		        CURRENT_PLATFORM = VITASW
                #elif defined ITF_CTR
		        CURRENT_PLATFORM = NINTENDO3DS
                #elif defined ITF_WINDOWS
		        CURRENT_PLATFORM = WINDOWS
                #elif defined ITF_IOS
		        CURRENT_PLATFORM = IOS
                #elif defined ITF_ANDROID
		        CURRENT_PLATFORM = ANDROIDS
                #elif defined ITF_ORBIS
		        CURRENT_PLATFORM = PS4
                #elif defined ITF_DURANGO
		        CURRENT_PLATFORM = XBOXONE
                #elif defined ITF_PS5
                CURRENT_PLATFORM = PS5
                #elif defined ITF_NX
                CURRENT_PLATFORM = NX
                #elif defined ITF_OUNCE
                CURRENT_PLATFORM = OUNCE
                #elif defined ITF_XBOX_SERIES
                CURRENT_PLATFORM = XBOXSX
                #else
                #pragma message("ERROR : CURRENT_PLATFORM undefined")
                #endif
	        };

	        const AkOSChar	*getWwisePlatformName(Platform _platform);
        }
    }
}

#endif // #ifdef ITF_SUPPORT_WWISE
#endif // __WWISE_HELPER_H__
