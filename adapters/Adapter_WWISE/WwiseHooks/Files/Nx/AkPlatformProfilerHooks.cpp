/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkTypes.h>

// These are sample implementations of the profiler hooks, to tie into the platform-specific profiler functionality.

#include <nn/profiler.h>
#pragma comment( lib, "nn_profiler" )

void AkPlatformProfilerPushTimer(AkPluginID in_uPluginID, const char* in_pszZoneName)
{
	nn::profiler::PushCodeBlock(in_pszZoneName);
}

void AkPlatformProfilerPopTimer()
{
	nn::profiler::PopCodeBlock();
}

void AkPlatformProfilerPostMarker(AkPluginID in_uPluginID, const char* in_pszMarkerName)
{
	// no marker fn, no op
}

void AkSetPlatformProfilerHooks(AkInitSettings& initSettings)
{
	initSettings.fnProfilerPushTimer = AkPlatformProfilerPushTimer;
	initSettings.fnProfilerPopTimer = AkPlatformProfilerPopTimer;
	initSettings.fnProfilerPostMarker = AkPlatformProfilerPostMarker;
}

void AkClearPlatformProfilerHooks(AkInitSettings& initSettings)
{
	initSettings.fnProfilerPushTimer = nullptr;
	initSettings.fnProfilerPopTimer = nullptr;
	initSettings.fnProfilerPostMarker = nullptr;
}
#endif
