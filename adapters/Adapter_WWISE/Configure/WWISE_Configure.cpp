#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Configure/WWISE_Configure.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>		// Memory Manager
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>			// Default memory and stream managers

#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkQueryParameters.h>
#include "adapters/Adapter_WWISE/WwiseHooks/WWISE_Hooks.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "adapters/Adapter_WWISE/WwiseHooks/Files/Common/AkJobWorkerMgr.h"
#if defined(AK_PS3)
#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"
#include "core/system/PS3/Purs/Purs_PS3.h"
#endif

#include "../Common/AkPlatformProfilerHooks.h"

namespace	ITF
{
    extern void* AllocHook(AkMemPoolId poolId, size_t in_size);
    extern void* AllocAlignedHook(AkMemPoolId poolId, size_t in_size, AkUInt32 uAlignment);
    extern void* ReallocHook(AkMemPoolId poolId, void* pAddress, size_t in_size);
    extern void* ReallocAlignedHook(AkMemPoolId poolId, void* pAddress, size_t in_size, AkUInt32 uAlignment);
    extern void FreeHook(AkMemPoolId poolId, void* in_ptr);
#if defined(ITF_ANDROID)
extern JavaVM* s_javaVM;
#endif

namespace	Wwise
{


#define WWISE_TARGET_BUFFER_LENGTH_DEFAULT   2000.f  // in milliseconds
#define WWISE_MAX_CONCURRENT_IO              4
#define WWISE_MAX_CONCURRENT_IO_WRITE_FILES  16

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Configure::getDefaultSettings( AkMemSettings           &out_memSettings,
                                        AkStreamMgrSettings		&out_stmSettings,
						                AkDeviceSettings		&out_deviceSettings,
						                AkInitSettings			&out_initSettings,
						                AkPlatformInitSettings	&out_platformInitSettings,
						                AkMusicSettings			&out_musicInit,
                                        AK::JobWorkerMgr::InitSettings  & _jobWorkerSettings
					                   )
	{

        AK::MemoryMgr::GetDefaultSettings(out_memSettings);

        out_memSettings.pfMalloc = &AllocHook;
        out_memSettings.pfMalign = &AllocAlignedHook;
        out_memSettings.pfRealloc = &ReallocHook;
        out_memSettings.pfFree = &FreeHook;
        out_memSettings.pfReallocAligned = &ReallocAlignedHook;

//         out_memSettings.pfInitForThread = NULL; //todoisap
//         out_memSettings.pfTermForThread = NULL;
// 
        // streaming
        AK_StreamMgr_GetDefaultSettings(out_stmSettings);

        // streaming device
        AK_StreamMgr_GetDefaultDeviceSettings(out_deviceSettings);
        out_deviceSettings.uIOMemorySize = 3 * 1024 * 1024;
#ifdef ITF_X360
        out_deviceSettings.uGranularity = 16 * 1024;
#else
        out_deviceSettings.uGranularity = 32 * 1024;
#endif
        out_deviceSettings.bUseStreamCache = true;


        out_deviceSettings.uIOMemoryAlignment = 4096; //2048 ?
        out_deviceSettings.uGranularity = 32 * 1024;
        out_deviceSettings.fTargetAutoStmBufferLength = WWISE_TARGET_BUFFER_LENGTH_DEFAULT;

        out_deviceSettings.uMaxConcurrentIO = WWISE_MAX_CONCURRENT_IO;



        AK_SoundEngine_GetDefaultInitSettings(out_initSettings);
        out_initSettings.pfnAssertHook = Wwise::Hooks::assertHook;
#ifndef ITF_XBOX_SERIES
        AkSetPlatformProfilerHooks(out_initSettings);
#endif
        
        

        AK_SoundEngine_GetDefaultPlatformInitSettings(out_platformInitSettings);
        AK_MusicEngine_GetDefaultInitSettings(out_musicInit);

        // The default job worker manager provided in the samples will generate more appropriate settings for the internal job manager of the sound engine.
        AK::JobWorkerMgr::GetDefaultInitSettings(_jobWorkerSettings);
        _jobWorkerSettings.uNumWorkerThreads = AK_MAX_WORKER_THREADS;
        out_initSettings.settingsJobManager = _jobWorkerSettings.GetJobMgrSettings();
        for (AkUInt32 uJobType = 0; uJobType < AK_NUM_JOB_TYPES; ++uJobType)
        {
            out_initSettings.settingsJobManager.uMaxActiveWorkers[uJobType] = 2;
        }








	}
}
}
#endif
