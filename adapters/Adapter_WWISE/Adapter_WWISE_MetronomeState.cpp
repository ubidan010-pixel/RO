#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE_MetronomeState.h"
#include "AK/MusicEngine/Common/AkMusicEngine.h"
#include "engine/sound/MetronomeManager.h"
#include "AK/SoundEngine/Common/AkCallback.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_Helper.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"
#include "adapters/AudioSerializedData/Wwise/WwiseEvents.h"
#include "engine/singleton/Singletons.h"
#include "engine/events/EventManager.h"
#include "AK\SoundEngine\Common\AkCallback.h"
#include "engine/gameplay/musicmanager.h"

namespace	ITF
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	inline bool Adapter_WWISE::MetronomeState::MusicalSegmentSignature::operator!=(const MusicalSegmentSignature &_r) const
	{
		if(m_beatsPerBar != _r.m_beatsPerBar) return true;
		if(m_segmentDuration_ms != _r.m_segmentDuration_ms) return true;
		if(m_barDuration_ms != _r.m_barDuration_ms) return true;
		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::MetronomeState::MusicalSegmentSignature::MusicalSegmentSignature() :
		m_beatsPerBar(0),
		m_segmentDuration_ms(0),
		m_barDuration_ms(0)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	inline bool Adapter_WWISE::MetronomeState::MusicCallbackInfo::operator!=(const MusicCallbackInfo &_r) const
	{
		if(f32_Abs(m_beatDuration - _r.m_beatDuration) > .0001f) return true;
		if(f32_Abs(m_barDuration - _r.m_barDuration) > .0001f) return true;
		if(f32_Abs(m_gridDuration - _r.m_gridDuration) > .000001f) return true;
		if(f32_Abs(m_gridOffset - _r.m_gridOffset) > .000001f) return true;

		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::MetronomeState::MusicCallbackInfo::MusicCallbackInfo() :
		m_beatDuration(0.f),
		m_barDuration(0.f),
		m_gridDuration(0.f),
		m_gridOffset(0.f)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::MetronomeState::MusicCallbackInfo::MusicCallbackInfo(f32 _beatDuration, f32 _barDuration, f32 _gridDuration, f32 _gridOffset) :
		m_beatDuration(_beatDuration),
		m_barDuration(_barDuration),
		m_gridDuration(_gridDuration),
		m_gridOffset(_gridOffset)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::MetronomeState::MetronomeState(MetronomeType _metronomeID) :
		c_metronomeID(_metronomeID),
		m_measureInPart(0),
		m_measureSinceStart(0),
		m_isPause(false),
		m_syncPlayStackSize(0),
		m_syncSendEventStackSize(0)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::MetronomeState::attach(SoundHandle _playingID)
	{
		if(m_playingID.isValid() == bfalse)
		{
			m_currentSegment.m_beatsPerBar = 0;
			m_currentSegment.m_segmentDuration_ms = 0;
			m_currentSegment.m_barDuration_ms = 0;
			m_measureInPart = 0;
			m_measureSinceStart = 0;
//             METRONOME_MANAGER->reset(c_metronomeID);
// 			METRONOME_MANAGER->start(c_metronomeID);
// 			METRONOME_MANAGER->setTimeSignature(METRONOME_DEFAULT_BPM, 4, 4, c_metronomeID);
		}

		m_playingID = _playingID;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::MetronomeState::detach(SoundHandle _playingID)
	{
		if(m_playingID == _playingID)
		{
			m_playingID.invalidate();
			//if(METRONOME_MANAGER) METRONOME_MANAGER->reset(c_metronomeID);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::MetronomeState::update()
	{

		//$1- update the clock ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if (m_playingID.isValid() && !METRONOME_MANAGER->isPaused(c_metronomeID) && m_musicDuration_msec!=0.f)
        {

            f32 duration = (f32) (m_musicDuration_msec * 0.001);
            AudioPlayRequest request{};
            bool result = AUDIO_ADAPTER->getPlayRequestFromPlayingID(m_playingID, request);
            if (result)
                MUSICMANAGER->notifyDuration(c_metronomeID, request.m_musicPart, duration);

        }

/*

			AkSegmentInfo	akSegmentInfo;
            AKRESULT akResul = AK_MusicEngine_GetPlayingSegmentInfo(m_playingID.getValueAsU32(), akSegmentInfo, true);
			MusicCallbackInfo lastMusicCallbackInfo;
			lastMusicCallbackInfo=m_lastCallbackInfo.read();
			if(akResul == AK_Success && akSegmentInfo.iActiveDuration)
			{
                if(lastMusicCallbackInfo.m_barDuration)
				{
					MusicalSegmentSignature musicalSegmentSignature;

					musicalSegmentSignature.m_beatsPerBar = static_cast<u32>(lastMusicCallbackInfo.m_barDuration / lastMusicCallbackInfo.m_beatDuration);
					musicalSegmentSignature.m_segmentDuration_ms = akSegmentInfo.iActiveDuration;
					musicalSegmentSignature.m_barDuration_ms = static_cast<u32>(lastMusicCallbackInfo.m_barDuration * 1000.f);

					if(m_currentSegment != musicalSegmentSignature) // new segment/signature ?
					{
						m_currentSegment = musicalSegmentSignature;

						const f32	partDurationSeconds = static_cast<f32>(m_currentSegment.m_segmentDuration_ms) * 0.001f;
						const f32	fsizeInBars = partDurationSeconds / lastMusicCallbackInfo.m_barDuration;
						const u32	nbOfBarInThispart = static_cast<u32>(fsizeInBars + 0.5f);

						//METRONOME_MANAGER->setParameters(nbOfBarInThispart, musicalSegmentSignature.m_beatsPerBar, lastMusicCallbackInfo.m_barDuration, lastMusicCallbackInfo.m_beatDuration, c_metronomeID);
                        METRONOME_MANAGER->setParameters(nbOfBarInThispart, musicalSegmentSignature.m_beatsPerBar, partDurationSeconds, c_metronomeID);

						if(m_measureInPart)
						{
							++m_measureSinceStart;
						}

						m_measureInPart = 0;
					}

					if(akSegmentInfo.iCurrentPosition > 0)
					{
						const i32	bar_ms = m_currentSegment.m_barDuration_ms;
						i32			position_ms = akSegmentInfo.iCurrentPosition;
						u32			measureInPart = 0;

						while(position_ms > bar_ms)
						{
							position_ms -= bar_ms;
							++measureInPart;
						}

						const f32	barPercentage = static_cast<f32>(position_ms) / static_cast<f32>(m_currentSegment.m_barDuration_ms);

						if(measureInPart != m_measureInPart) ++m_measureSinceStart;
						m_measureInPart = measureInPart;

						METRONOME_MANAGER->update(m_measureInPart, m_measureSinceStart, barPercentage, c_metronomeID);
					}
				}
			}
		}

		//$1- synchro play ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		MusicCallbackEvent	musicCallbackEvent;
        AudioPlayRequest syncplayStack[Wwise::Configure::METRONOME_SYNC_PLAY_STACK_SIZE];
		u32					syncplayStackSize ;
		AudioSendEventRequest syncSendEventStack[Wwise::Configure::METRONOME_SYNC_PLAY_STACK_SIZE];
		u32					syncSendEventStackSize ;
        ITF::EventMusicCustomCue eventMusicCustomCue ;
        


		while(m_callbackEventRingBuffer.read(musicCallbackEvent))
		{
			const i32 lastCallbackFlags = musicCallbackEvent.m_eventFlag;
            
            // send event
            if(lastCallbackFlags & AK_MusicSyncUserCue)
            {
                eventMusicCustomCue.setCue(musicCallbackEvent.m_cueName);
                eventMusicCustomCue.setMetronomeID(c_metronomeID);
                EVENTMANAGER->broadcastEvent(&eventMusicCustomCue);
            }
			

            // treat sync play commands
            syncplayStackSize = 0;
			for(ux i = 0; i < m_syncPlayStackSize; ++i)
			{
				AudioPlayRequest &playOnNext = m_syncPlayStack[i];

				switch(playOnNext.m_playAt)
				{
				case AUDIO_SYNC_PLAY_IMMEDIATE:
					playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
					playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
					Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_GRID:
					if(lastCallbackFlags & AK_MusicSyncGrid)
					{
						playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
					}
					else
					{
						syncplayStack[syncplayStackSize++] = playOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_BAR:
					if(lastCallbackFlags & AK_MusicSyncBar)
					{
						playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
					}
					else
					{
						syncplayStack[syncplayStackSize++] = playOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_BEAT:
					if(lastCallbackFlags & AK_MusicSyncBeat)
					{
						playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
					}
					else
					{
						syncplayStack[syncplayStackSize++] = playOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_CUE:
					if(lastCallbackFlags & (AK_MusicSyncUserCue | AK_MusicSyncEntry | AK_MusicSyncExit))
					{
						playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
					}
					else
					{
						syncplayStack[syncplayStackSize++] = playOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_CUSTOM_CUE:
					if(lastCallbackFlags & AK_MusicSyncUserCue)
					{
						if(musicCallbackEvent.m_cueName == playOnNext.m_playAtCue)
						{
							playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
							playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
							Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
						}
					}
					else
					{
						syncplayStack[syncplayStackSize++] = playOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_ENTRY_CUE:
					if(lastCallbackFlags & AK_MusicSyncEntry)
					{
						playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
					}
					else
					{
						syncplayStack[syncplayStackSize++] = playOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_EXIT_CUE:
					if(lastCallbackFlags & AK_MusicSyncExit)
					{
						playOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						playOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						Adapter_WWISE::s_getSingleton()->priv_play(playOnNext);
					}
					else
					{
						syncplayStack[syncplayStackSize++] = playOnNext;
					}
					break;
				}
			}

			// report
			m_syncPlayStackSize = syncplayStackSize;
			for(ux i = 0; i < syncplayStackSize; ++i) m_syncPlayStack[i] = syncplayStack[i];


			// treat sync SendEvent commands
			syncSendEventStackSize = 0;
			for(ux i = 0; i < m_syncSendEventStackSize; ++i)
			{
				AudioSendEventRequest &sendEventOnNext = m_syncSendEventStack[i];

				switch(sendEventOnNext.m_playAt)
				{
				case AUDIO_SYNC_PLAY_IMMEDIATE:
					sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
					sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
					sendEvent(sendEventOnNext);
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_GRID:
					if(lastCallbackFlags & AK_MusicSyncGrid)
					{
						sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						sendEvent(sendEventOnNext);
					}
					else
					{
						syncSendEventStack[syncSendEventStackSize++] = sendEventOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_BAR:
					if(lastCallbackFlags & AK_MusicSyncBar)
					{
						sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						sendEvent(sendEventOnNext);
					}
					else
					{
						syncSendEventStack[syncSendEventStackSize++] = sendEventOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_BEAT:
					if(lastCallbackFlags & AK_MusicSyncBeat)
					{
						sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						sendEvent(sendEventOnNext);
					}
					else
					{
						syncSendEventStack[syncSendEventStackSize++] = sendEventOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_CUE:
					if(lastCallbackFlags & (AK_MusicSyncUserCue | AK_MusicSyncEntry | AK_MusicSyncExit))
					{
						sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						sendEvent(sendEventOnNext);
					}
					else
					{
						syncSendEventStack[syncSendEventStackSize++] = sendEventOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_NEXT_CUSTOM_CUE:
					if(lastCallbackFlags & AK_MusicSyncUserCue)
					{
						if(musicCallbackEvent.m_cueName == sendEventOnNext.m_playAtCue)
						{
							sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
							sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
							sendEvent(sendEventOnNext);
						}
					}
					else
					{
						syncSendEventStack[syncSendEventStackSize++] = sendEventOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_ENTRY_CUE:
					if(lastCallbackFlags & AK_MusicSyncEntry)
					{
						sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						sendEvent(sendEventOnNext);
					}
					else
					{
						syncSendEventStack[syncSendEventStackSize++] = sendEventOnNext;
					}
					break;

				case AUDIO_SYNC_PLAY_AT_EXIT_CUE:
					if(lastCallbackFlags & AK_MusicSyncExit)
					{
						sendEventOnNext.m_playAt = AUDIO_SYNC_PLAY_IMMEDIATE;
						sendEventOnNext.m_metronomeID = METRONOME_TYPE_INVALID;
						sendEvent(sendEventOnNext);
					}
					else
					{
						syncSendEventStack[syncSendEventStackSize++] = sendEventOnNext;
					}
					break;
				}
			}

			// report
			m_syncSendEventStackSize = syncSendEventStackSize;
			for(ux i = 0; i < syncSendEventStackSize; ++i) m_syncSendEventStack[i] = syncSendEventStack[i];
		}
        */
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::MetronomeState::sendEvent(const AudioSendEventRequest &_sendEventRequest) const
	{
		if(_sendEventRequest.getOnPlayEvent())
		{
			// callback event
			Actor	*sendTo = static_cast<Actor *>(GETOBJECT(_sendEventRequest.m_sentToObjectRef));
			if(sendTo) sendTo->onEvent(_sendEventRequest.getOnPlayEvent());
		}
	}

    //
// ===================================================================================================================
// ===================================================================================================================
//
    bool Adapter_WWISE::MetronomeState::durationCallBack(const AkDurationCallbackInfo* _akInfo, SoundHandle _soundHandle)
    {
        if (m_playingID == _soundHandle && _akInfo )
        {

            m_musicDuration_msec = ((f32)_akInfo->fDuration);
            return true;
        }
        return false;
    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool Adapter_WWISE::MetronomeState::musicCallBack(const AkMusicSyncCallbackInfo *_akMusicSyncCallbackInfo)
	{
		if(m_playingID == SoundHandle(_akMusicSyncCallbackInfo->playingID))
		{
			if(_akMusicSyncCallbackInfo->musicSyncType & AK_MusicSyncBeat)
			{
                auto& segmentInfo = _akMusicSyncCallbackInfo->segmentInfo;
				const MusicCallbackInfo musicCallbackInfo(segmentInfo.fBeatDuration, segmentInfo.fBarDuration, segmentInfo.fGridDuration, segmentInfo.fGridOffset);
                
				m_lastCallbackInfo.write(musicCallbackInfo);
			}

			m_callbackEventRingBuffer.write(MusicCallbackEvent(_akMusicSyncCallbackInfo->musicSyncType, _akMusicSyncCallbackInfo->pszUserCueName));
			return true;
		}

		return false;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	ITF::SoundHandle Adapter_WWISE::MetronomeState::playOnNext(const AudioPlayRequest &_internalPlayRequest)
	{
		if(m_syncPlayStackSize < Wwise::Configure::METRONOME_SYNC_PLAY_STACK_SIZE)
		{
			m_syncPlayStack[m_syncPlayStackSize++] = _internalPlayRequest;
		}
		else
		{
			AUDIO_WARNING("There is no more space on metronome %d stack to \"play-on-next\"", c_metronomeID);
		}

		return SoundHandle(0);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	ITF::bbool Adapter_WWISE::MetronomeState::sendEventOnNext(const AudioSendEventRequest &_internalSendEventRequest)
	{
		if(m_syncSendEventStackSize < Wwise::Configure::METRONOME_SYNC_PLAY_STACK_SIZE)
		{
			m_syncSendEventStack[m_syncSendEventStackSize++] = _internalSendEventRequest;
			return btrue;
		}
		else
		{
			AUDIO_WARNING("There is no more space on metronome %d stack to \"SendEvent-on-next\"", c_metronomeID);
		}

		return bfalse;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::MetronomeState::MusicCallbackEvent::MusicCallbackEvent() :
		m_eventFlag(0),
		m_cueName()
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	Adapter_WWISE::MetronomeState::MusicCallbackEvent::MusicCallbackEvent(i32 _eventFlag, const char *_cueName) :
		m_eventFlag(_eventFlag),
		m_cueName(_cueName)
	{
	}
}
#endif // ITF_SUPPORT_WWISE
