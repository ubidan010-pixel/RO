#ifndef __WWISE_CONFIGURE_H__
#define __WWISE_CONFIGURE_H__

#ifdef ITF_SUPPORT_WWISE
 #include <AK/SoundEngine/Common/AkTypes.h>
 #include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "adapters/Adapter_WWISE/WwiseHooks/Files/Common/AkJobWorkerMgr.h"


struct AkStreamMgrSettings;
struct AkDeviceSettings;
struct AkInitSettings;
struct AkPlatformInitSettings;
struct AkMusicSettings;
struct AkMemSettings;


namespace ITF
{
    namespace	Wwise
    {
        namespace	Configure
        {
            enum Constant
            {
#ifdef ITF_DURANGO
                MEMORY_MAX_NUMBER_OF_POOL = 256,				///< Maximum number of memory pools.//USED TO BE 20
#else
                MEMORY_MAX_NUMBER_OF_POOL = 100,				///< Maximum number of memory pools
#endif

                LISTENER_COUNT = 1,
#ifdef ITF_PROJECT_WW1
                MEMORY_DEFAULT_POOL_SIZE = 10 * 1024 * 1024,	///< Size of the default memory pool, in bytes
#else
                MEMORY_DEFAULT_POOL_SIZE = 2 * 1024 * 1024,	///< Size of the default memory pool, in bytes
#endif

#ifdef ITF_PROJECT_POP
                LOWER_ENGINE_DEFAULT_POOL_SIZE = 5 * 1024 * 1024,	///< Lower Engine default memory pool size
#else
                LOWER_ENGINE_DEFAULT_POOL_SIZE = 10 * 1024 * 1024,	///< Lower Engine default memory pool size
#endif

                INSTANCE_POOL_SIZE = 256,				///< max of simultaneous instance
                METRONOME_SYNC_PLAY_STACK_SIZE = 32,				///< max number of deferred play for sync features
                METRONOME_RINGBUFFER_SIZE = 128,

#if defined ITF_PROJECT_WW1
                X360_OUTPUT_CHANNEL_MASK = AK_SPEAKER_SETUP_STEREO, ///< force stereo output
#else
                //  X360_OUTPUT_CHANNEL_MASK		= AK_CHANNEL_MASK_DETECT,	
#endif

                IOS_SAMPLING_RATE = 32000,
                IOS_APPLI_MUTE_OTHER_APPS = 0,                ///< Will mute other applications playing audio in the background. If set to false, hardware sampling rate is not guaranteed (resampling might happen, limitation of iOS). Default is true.
                IOS_APPLI_HANDLE_INTERRUPTIONS = 0,                ///< If set to true, the app will be responsible for handling interruptions such as incoming phone calls and alarms by calling AK::SoundEngine::iOS::ListenToAudioSessionInterruption() in iOS Objective-C delegates applicationWillResignActive() and applicationDidBecomeActive() (required by Wwise Unity Integration). If set to false, the sound engine will handle it internally. Default to false.


                PS3_ENABLE_BGM = 0,                                 ///< Enable support for BGM auto-muting. This will cause the Sound Engine to take one Sysutil callback slot, specified below
                PS3_SYSUTIL_CB_SLOT = 0,                            ///< Sysutil callback slot (range 0-3) used for BGM auto-muting ONLY when bGMEnable==true. 
            };

            static const AkOSChar* c_bankRootPath = AKTEXT("WWise/RaymanOrigins/GeneratedSoundBanks/");

            static const AkOSChar* c_packageRootPath = AKTEXT("Sound/WwisePackage/");
            static const AkOSChar* c_packageExt = AKTEXT(".pck");
#ifdef ITF_PROJECT_WW1
            static const AkOSChar* c_defaultPackageName = AKTEXT("Intro");
            static const AkOSChar* c_globalBankName = AKTEXT("bootstrap");
#else
            static const AkOSChar* c_defaultPackageName = AKTEXT("SFX");
#endif

            static const char* c_configFileName = "soundconfig";
            static const char* c_configFileExtension = "isg";

            static const char* c_wwiseGenerateFileName = "generatebank";
            static const char* c_wwiseGenerateFileExtension = "wis";

            static AkThreadProperties arThreadProperties[AK_MAX_WORKER_THREADS];


            void				    getDefaultSettings(AkMemSettings& out_memSettings,
                AkStreamMgrSettings& out_stmSettings,
                AkDeviceSettings& out_deviceSettings,
                AkInitSettings& out_initSettings,
                AkPlatformInitSettings& out_platformInitSettings,
                AkMusicSettings& out_musicInit,
                AK::JobWorkerMgr::InitSettings& _jobWorkerSettings);
        }

    }

}
#endif // #ifdef ITF_SUPPORT_WWISE
#endif // __WWISE_CONFIGURE_H__
