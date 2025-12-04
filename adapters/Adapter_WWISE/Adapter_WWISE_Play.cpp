#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_MetronomeState.h"
#include "engine/actors/actor.h"
#include "engine/events/Events.h"
#include "core/serializer/ZSerializerObjectBinary.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_AkGameObject.h"
#ifdef ITF_WINDOWS
//#define TRACE_ALL_PLAY_STOP
#endif

namespace	ITF
{

    u32 Wwise_dbg_BreakOnEventID = 0;
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	SoundHandle Adapter_WWISE::priv_play(const AudioPlayRequest &_playRequest)
	{

#ifdef ITF_DEBUG_STRINGID
        {
            if(Wwise_dbg_BreakOnEventID)
            {
                if(_playRequest.m_soundEventID == Wwise_dbg_BreakOnEventID)
                {
                    // AUDIO_LOG("HIT Play Wwise event \"%s\".", getWwiseNameFromID(_playRequest.m_soundEventID));
                }

                // AUDIO_LOG("Play Wwise event \"%d\".", _playRequest.m_soundEventID);
            }
        }
#ifdef ITF_LOG_SOUND_EVENTS
	    // AUDIO_LOG("HIT Play Wwise event \"%s\".", getWwiseNameFromID(_playRequest.m_soundEventID));
#endif
#endif

		struct CallbackWrapper
		{

			//
			// ===========================================================================================================
			// ===========================================================================================================
			//
			static void s_eventCallback(AkCallbackType _type, AkCallbackInfo *_callbackInfo)
			{
				if(_type == AK_EndOfEvent)
				{
					Adapter_WWISE		*thisPointer = static_cast<Adapter_WWISE *>(_callbackInfo->pCookie);
					AkEventCallbackInfo *akEventCallbackInfo = static_cast<AkEventCallbackInfo *>(_callbackInfo);
					thisPointer->onEndOfEvent(SoundHandle(akEventCallbackInfo->playingID));
				}
			}

			//
			// ===========================================================================================================
			// ===========================================================================================================
			//
			static void s_musicCallback(AkCallbackType _type, AkCallbackInfo *_callbackInfo)
			{
				if(_type & AK_EndOfEvent)
				{
					Adapter_WWISE				*thisPointer = static_cast<Adapter_WWISE *>(_callbackInfo->pCookie);
					const AkEventCallbackInfo	*akEventCallbackInfo = static_cast<AkEventCallbackInfo *>(_callbackInfo);
					thisPointer->onEndOfEvent(SoundHandle(akEventCallbackInfo->playingID));
				}
				else if(_type & AK_MusicSyncAll)
				{
					Adapter_WWISE					*thisPointer = static_cast<Adapter_WWISE *>(_callbackInfo->pCookie);
					const AkMusicSyncCallbackInfo	*akMusicSyncCallbackInfo = static_cast<AkMusicSyncCallbackInfo *>(_callbackInfo);
					thisPointer->onMusicEvent(akMusicSyncCallbackInfo);
				}
                else if (_type & AK_Duration)
                {
                    Adapter_WWISE* thisPointer = static_cast<Adapter_WWISE*>(_callbackInfo->pCookie);
                    const AkEventCallbackInfo* akEventCallbackInfo = static_cast<AkEventCallbackInfo*>(_callbackInfo);
                    const AkDurationCallbackInfo* akDurationCallbackInfo = static_cast<AkDurationCallbackInfo*>(_callbackInfo);
                    thisPointer->onDurationEvent(akDurationCallbackInfo, SoundHandle(akEventCallbackInfo->playingID));
                }
			}
		};


        if(_playRequest.m_objectRef.isValid())
            Wwise::AkGameObjectFactory::s_getSingleton()->create(_playRequest.m_objectRef);

        const AkGameObjectID gameObjectID = Wwise::AkGameObject::s_getAkGameObjectID(_playRequest.m_objectRef) ;
		AkUInt32		uFlags = AK_EndOfEvent; // EndOfFile callback
		AkCallbackFunc	callback = &CallbackWrapper::s_eventCallback;

		if(_playRequest.m_metronomeID != METRONOME_TYPE_INVALID)
		{
			// this is a music => play is differed
			if(_playRequest.m_playAt == AUDIO_SYNC_PLAY_IMMEDIATE)
			{
 				uFlags |= AK_MusicSyncBeat;
 				uFlags |= AK_MusicSyncEntry;
				//uFlags |= AK_EnableGetMusicPlayPosition;
                uFlags |= AK_EnableGetSourcePlayPosition;
                uFlags |= AK_Duration;

				// for sync
				uFlags |= AK_MusicSyncGrid;
				uFlags |= AK_MusicSyncBar;
				uFlags |= AK_MusicSyncUserCue;
				uFlags |= AK_MusicSyncEntry;
				uFlags |= AK_MusicSyncExit;
				callback = &CallbackWrapper::s_musicCallback;
			}
			else
			{
				// this is a music slave
				return m_metronomeState[_playRequest.m_metronomeID]->playOnNext(_playRequest);
			}
		}
		else if(_playRequest.getOnPlayEvent())
		{
			// callback event
			Actor	*sendTo = static_cast<Actor *>(GETOBJECT(_playRequest.m_sentToObjectRef));
			if(sendTo) sendTo->onEvent(_playRequest.getOnPlayEvent());
		}
		else if(_playRequest.m_sourcePlayPosition)
		{
            uFlags |= AK_EnableGetSourcePlayPosition;
		}

#ifdef TRACE_ALL_PLAY_STOP
        {
            const char*name=getWwiseNameFromID(_playRequest.m_soundEventID);
            AUDIO_LOG("WWISE play %s", name);
        }
#endif

		const AkPlayingID	playingID = AK_SoundEngine_PostEvent(_playRequest.m_soundEventID, gameObjectID, uFlags, callback, this);
		SoundHandle		SoundHandle(playingID);

		if(SoundHandle.isValid())
		{
			m_playingIDs[SoundHandle.getValueAsU32()] = _playRequest;
		    if(_playRequest.m_metronomeID != METRONOME_TYPE_INVALID) m_metronomeState[_playRequest.m_metronomeID]->attach(SoundHandle);
        }

		return SoundHandle;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unregisterObjectRef(ObjectRef _objRef)
	{
		if(isRunning()==false)
			return;
		const AkGameObjectID gameObjectID = Wwise::AkGameObject::s_getAkGameObjectID(_objRef) ;
		Wwise::AkGameObjectFactory::s_getSingleton()->destroy(gameObjectID);
	}
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	SoundHandle Adapter_WWISE::play(const AudioPlayRequest &_playRequest)
	{
		if(isRunning()==false)
			return SoundHandle::getInvalidHandle();
		if(_playRequest.m_soundEventID == 0) return SoundHandle::getInvalidHandle();
		return priv_play(_playRequest);
	}

    AKRESULT Adapter_WWISE::AK_SoundEngine_PrepareEvent(AkUniqueID				 in_eventID)
    {
        AUDIO_ASSERT(in_eventID != AK_INVALID_UNIQUE_ID, "you are trying to prepare an invalid event");
        AK::SoundEngine::PreparationType		in_PreparationType = AK::SoundEngine::Preparation_Load;
        AkUniqueID in_events[1];
        in_events[0] = in_eventID;
        const AKRESULT res = AK::SoundEngine::PrepareEvent(in_PreparationType, in_events, 1, nullptr, nullptr);
        return res;
    }
    AKRESULT Adapter_WWISE::AK_SoundEngine_UnPrepareEvent(AkUniqueID				 in_eventID)
    {
        AUDIO_ASSERT(in_eventID != AK_INVALID_UNIQUE_ID, "you are trying to unprepare an invalid event");
        AK::SoundEngine::PreparationType		in_PreparationType = AK::SoundEngine::Preparation_Unload;
        const AKRESULT res = AK::SoundEngine::PrepareEvent(in_PreparationType, &in_eventID, 1);
        return res;
    }

    bbool Adapter_WWISE::prepareEvent(const SoundEventID    _soundEventID)
    {
        if (isRunning() == false)
            return false;

        const AKRESULT	eResult = AK_SoundEngine_PrepareEvent(_soundEventID);
        return eResult == AK_Success;

    }


    bbool Adapter_WWISE::unPrepareEvent(const SoundEventID _soundEventID)
    {
        if (isRunning() == false)
            return false;

        const AKRESULT	eResult = AK_SoundEngine_UnPrepareEvent(_soundEventID);
        return eResult == AK_Success;
    }

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    SoundEventID    Adapter_WWISE::getEventIDFromPlayingID(SoundHandle _playingID) const
    {
        PlayingIDs::const_iterator it =m_playingIDs.find(_playingID.getValueAsU32());
        if(it!=m_playingIDs.end())
        {
            return it->second.m_soundEventID;
        }

        return ITF_INVALID_SOUND_EVENT_ID;
    }


    bbool    Adapter_WWISE::getPlayRequestFromPlayingID(SoundHandle _playingID, AudioPlayRequest& request) const
    {
        PlayingIDs::const_iterator it = m_playingIDs.find(_playingID.getValueAsU32());
        if (it != m_playingIDs.end())
        {
            request = it->second;
            return btrue;
        }

        return bfalse;
    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::stop(SoundHandle _SoundHandle)
	{
		if(_SoundHandle.isValid())
		{
#ifdef TRACE_ALL_PLAY_STOP
            {
                const AkUniqueID eventID= AK::SoundEngine::Query::GetEventIDFromPlayingID(_SoundHandle.getValueAsU32());
                const char*name=getWwiseNameFromID(eventID);
                AUDIO_LOG("WWISE stop %s", name);
            }
#endif
            AK_SoundEngine_StopPlayingID(_SoundHandle.getValueAsU32(), 200, AkCurveInterpolation_Linear);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bbool Adapter_WWISE::playInstanceAfterDestroy(SoundHandle _SoundHandle)
	{
		PlayingIDs::const_iterator	it = m_playingIDs.find(_SoundHandle.getValueAsU32());
		if(it != m_playingIDs.end())
		{
			return !(it->second.m_stopOnObjecDestroy);

		}
		return bfalse;
	}


	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	ITF::bbool Adapter_WWISE::isPlaying(SoundHandle _SoundHandle) const
	{
		PlayingIDs::const_iterator	it = m_playingIDs.find(_SoundHandle.getValueAsU32());

		return(it != m_playingIDs.end());
	}

}
#endif // ITF_SUPPORT_WWISE
