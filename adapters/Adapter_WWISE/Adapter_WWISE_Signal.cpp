#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "engine/sound/SoundConfigTemplate.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
#ifdef ITF_SUPPORT_EDITOR
#include "editor/Editor.h"
//#include "adapters/AudioSerializedData/Wwise/WwiseSoundComponent.h"
#endif


namespace	ITF
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::signal(AudioSignal _signal)
	{
		AudioEngineEvent	audioEngineEvent = AUDIO_ENGEVT_INVALID;

		switch(_signal)
		{
        case AUDIO_SIG_STOP_ALL:
            audioEngineEvent = AUDIO_ENGEVT_WWISE_STOP_ALL;
            break;
			
		case AUDIO_SIG_STOP_ALL_QUITFROMGAME:
			audioEngineEvent = AUDIO_ENGEVT_WWISE_QUITFROMGAME;
			break;

		case AUDIO_SIG_START_MAP:
			audioEngineEvent = AUDIO_ENGEVT_WWISE_START_MAP;
			initDefaultAuxBusSettings();
			break;

		case AUDIO_SIG_SKIP_CINE:
			audioEngineEvent = AUDIO_ENGEVT_WWISE_SKIP_CINE;
			break;

		case AUDIO_SIG_CHECK_POINT_LOAD:	// F5
#ifdef ITF_SUPPORT_EDITOR
			m_playingEventsBeforeGenerateBanks.clear();
			m_auxEffectsBeforeGenerateBanks.clear();
#endif
			audioEngineEvent = AUDIO_ENGEVT_WWISE_CHECK_POINT_LOAD;

			// fall back
			if(m_engineEventIDs[audioEngineEvent] == ITF_INVALID_SOUND_EVENT_ID) audioEngineEvent = AUDIO_ENGEVT_WWISE_STOP_ALL;

			resetAllActiveAuxBus();
			initDefaultAuxBusSettings();
			break;

		case AUDIO_SIG_RELOAD_MAP:			// ctrl+F5
#ifdef ITF_SUPPORT_EDITOR
			m_playingEventsBeforeGenerateBanks.clear();
			m_auxEffectsBeforeGenerateBanks.clear();
#endif
			audioEngineEvent = AUDIO_ENGEVT_WWISE_RELOAD_MAP;

			// fall back
			if(m_engineEventIDs[audioEngineEvent] == ITF_INVALID_SOUND_EVENT_ID) audioEngineEvent = AUDIO_ENGEVT_WWISE_STOP_ALL;
			resetAllActiveAuxBus();
			break;

		case AUDIO_SIG_HOT_RELOAD_MAP:
#ifdef ITF_SUPPORT_EDITOR
			m_playingEventsBeforeGenerateBanks.clear();
			m_auxEffectsBeforeGenerateBanks.clear();
#endif
			audioEngineEvent = AUDIO_ENGEVT_WWISE_HOT_RELOAD_MAP;

			// fall back
			if(m_engineEventIDs[audioEngineEvent] == ITF_INVALID_SOUND_EVENT_ID) audioEngineEvent = AUDIO_ENGEVT_WWISE_STOP_ALL;
			resetAllActiveAuxBus();
			break;

#ifdef ITF_SUPPORT_EDITOR

		case AUDIO_SIG_CONFIG_HAS_CHANGED:
			signal(AUDIO_SIG_BEGIN_GENERATE_BANK);
			signal(AUDIO_SIG_END_GENERATE_BANK);
			break;

		case AUDIO_SIG_BEGIN_GENERATE_BANK:
            break;
         
		case AUDIO_SIG_END_GENERATE_BANK:
            
			//$1- reload bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            reloadAllBanks();

			//$1- try to restore playing events ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			for(ITF_VECTOR<AudioPlayRequest>::const_iterator it = m_playingEventsBeforeGenerateBanks.begin(), itEnd = m_playingEventsBeforeGenerateBanks.end(); it != itEnd; ++it)
			{
				priv_play(*it);
			}

			m_playingEventsBeforeGenerateBanks.clear();

			for(ITF_VECTOR<ActiveAuxBus>::const_iterator it = m_auxEffectsBeforeGenerateBanks.begin(), itEnd = m_auxEffectsBeforeGenerateBanks.end(); it != itEnd; ++it)
			{
				setBusEffect(it->first, it->second.first, it->second.second);
			}

			m_auxEffectsBeforeGenerateBanks.clear();

            //$1- restore states ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            for(u32 i=0; i<m_stateBeforeGenerateBanks.size(); ++i)
            {
                std::pair<SoundRtpcID,SoundRtpcID>&pair=m_stateBeforeGenerateBanks[i];
				setState(pair.first, pair.second);
            }
            m_stateBeforeGenerateBanks.clear();

			//$1- resume the game engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            
		//	EDITOR->popPause(btrue); //todoisap

            m_ignorePauseDuringBankGenerations = false;
			break;
#endif

		default:
			break;
		}

		if((audioEngineEvent != AUDIO_ENGEVT_INVALID) && (m_engineEventIDs[audioEngineEvent] != ITF_INVALID_SOUND_EVENT_ID))
		{
			play(AudioPlayRequest(m_engineEventIDs[audioEngineEvent]));

			if(_signal == AUDIO_SIG_RELOAD_MAP) update(0);
		}
	}
}
#endif // ITF_SUPPORT_WWISE
