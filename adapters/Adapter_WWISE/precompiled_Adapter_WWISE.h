#ifndef __PRECOMPILED_WWISE_H__
#define __PRECOMPILED_WWISE_H__

#include "core/EngineCommon.h"
#ifdef ITF_PS5
#if (SCE_PROSPERO_SDK_VERSION < 0x11000040u)
#undef AK_PROSPERO_SDK_1100_OR_LATER
#endif
#endif

#include "AK/AkPlatforms.h"
#include "adapters/Adapter_WWISE/AudioSDK/memory.h"
#include "adapters/Adapter_WWISE/AudioSDK/macros.h"


#ifdef AK_WIN
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

// Windows Header Files:
#include <windows.h>
#endif


#endif // __PRECOMPILED_WWISE_H__
