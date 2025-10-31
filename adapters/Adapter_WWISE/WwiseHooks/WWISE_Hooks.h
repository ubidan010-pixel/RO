#ifndef __WWISE_HOOKS_H__
#define __WWISE_HOOKS_H__

#include "AK/SoundEngine/Common/AkTypes.h"
#include "AK/Tools/Common/AkMonitorError.h"


#ifdef ITF_SUPPORT_WWISE

namespace ITF
{
    namespace Wwise 
    {
        namespace Hooks
        {
	        void	logHook(AK::Monitor::ErrorCode in_eErrorCode, const AkOSChar *in_pszError, AK::Monitor::ErrorLevel in_eErrorLevel, AkPlayingID in_playingID, AkGameObjectID in_gameObjID);
	        void	assertHook(const char *in_pszExpression, const char *in_pszFileName, int in_lineNumber);
        }
    }
}


#endif
#endif // __WWISE_HOOKS_H__
