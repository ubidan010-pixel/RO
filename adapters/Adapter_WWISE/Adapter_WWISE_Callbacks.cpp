#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_MetronomeState.h"
#include "AK/SoundEngine/Common/AkCallback.h"
#include "AK/SoundEngine/Common/AkQueryParameters.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"

namespace	ITF
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::onEndOfEvent(SoundHandle _SoundHandle)
	{
		m_endedEvents.write(_SoundHandle);
	}

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //

    void Adapter_WWISE::onDurationEvent(const AkDurationCallbackInfo *_akInfo, SoundHandle _soundHandle)
    {
        for (u32 i = 0; i < METRONOME_TYPE_COUNT; ++i)
        {
            if (m_metronomeState[i]->durationCallBack(_akInfo, _soundHandle))
            {
                break;
            }
        }
    }
   
        //
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::onMusicEvent(const AkMusicSyncCallbackInfo *_akMusicSyncCallbackInfo)
	{
		if(_akMusicSyncCallbackInfo->musicSyncType & AK_MusicSyncAll)
		{
			for(u32 i = 0; i < METRONOME_TYPE_COUNT; ++i)
			{
				if(m_metronomeState[i]->musicCallBack(_akMusicSyncCallbackInfo))
				{
					break;
				}
			}
		}
	}



#define DATA_SUMMARY_REFRESH_COOLDOWN 7; // Refresh cooldown affecting the refresh rate of the resource monitor data summary
    AkResourceMonitorDataSummary Adapter_WWISE::ResourceDataSummary;
    void Adapter_WWISE::ResourceMonitorDataCallback(const AkResourceMonitorDataSummary* in_pdataSummary)
    {
        static int ResourceMonitorUpdateCooldown = 0;
        if (ResourceMonitorUpdateCooldown <= 0)
        {
            Adapter_WWISE::ResourceDataSummary = *in_pdataSummary;
            ResourceMonitorUpdateCooldown = DATA_SUMMARY_REFRESH_COOLDOWN;
        }
        else
            ResourceMonitorUpdateCooldown--;
    }
    void Adapter_WWISE::LocalErrorCallback(AK::Monitor::ErrorCode in_eErrorCode, const AkOSChar* in_pszError, AK::Monitor::ErrorLevel in_eErrorLevel, AkPlayingID in_playingID, AkGameObjectID in_gameObjID)
    {
        char* pszErrorStr;
        CONVERT_OSCHAR_TO_CHAR(in_pszError, pszErrorStr);

        if (in_eErrorLevel == AK::Monitor::ErrorLevel_Error)
        {

        }
        AKPLATFORM::OutputDebugMsgV("%s: %s\n", in_eErrorLevel == AK::Monitor::ErrorLevel_Message ? "AK Message" : "AK Error", pszErrorStr);
    }

    void Adapter_WWISE::WwiseAudioThreadCallbackFunc(AK::IAkGlobalPluginContext*, AkGlobalCallbackLocation in_eLocation, void* in_pCookie)
    {

        if (!AK::SoundEngine::IsInitialized())
        {
            return;
        }

        if (in_eLocation == AkGlobalCallbackLocation_BeginRender)
        {
//            Adapter_WWISE* adapter = (Adapter_WWISE*)in_pCookie;
// 
//             constexpr ubiS8 updateEvery = 5;
//             ubiS8& speakerConfigUpdateCount = soundWwiseManager->m_SpeakerConfigUpdateCount;
// 
//             speakerConfigUpdateCount = (speakerConfigUpdateCount + 1) % updateEvery;
// 
//             if (speakerConfigUpdateCount != 0)
//                 return;
// 
//             ubiS32& speakerConfigStatus = soundWwiseManager->m_SpeakerConfigStatus;
//             if (G4::Atomic::ExchangeIfEqual(speakerConfigStatus, SpeakerConfig_Unchanged, SpeakerConfig_Reading) == SpeakerConfig_Unchanged)
//             {
//                 //called from a wwise callback since GetSpeakerConfiguration locks the AK main lock
//                 AkChannelConfig newConfig = AK::SoundEngine::GetSpeakerConfiguration();
//                 if (newConfig.uNumChannels != soundWwiseManager->m_CurrentNumberOfChannels)
//                 {
//                     soundWwiseManager->m_CurrentNumberOfChannels = newConfig.uNumChannels;
//                     G4::Atomic::Exchange(speakerConfigStatus, SpeakerConfig_Modified);
//                     return;
//                 }
//                 G4::Atomic::Exchange(speakerConfigStatus, SpeakerConfig_Unchanged);
//             }
        }
    }


}
#endif // ITF_SUPPORT_WWISE
