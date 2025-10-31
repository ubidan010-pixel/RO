#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "engine/display/View.h"
#include "engine/display/Camera.h"
#include "AK/SoundEngine/Common/AkQueryParameters.h"
#include "engine/sound/SoundConfigTemplate.h"
#include "adapters/Adapter_WWISE/Configure/WWISE_Configure.h"
#include "adapters/Adapter_WWISE/Wrapper/WWISE_Wrapper.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_MetronomeState.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_Listener.h"
#include "adapters/Adapter_WWISE/Helper/WWISE_Helper.h"
#include "engine/actors/actor.h"
#ifdef ITF_SUPPORT_EDITOR
#include "editor/Editor.h"
//#include "adapters/AudioSerializedData/Wwise/WwiseSoundComponent.h"
#endif
#include "adapters/Adapter_WWISE/Helper/WWISE_AkGameObject.h"
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_INSTANCEMANAGER_H_
#include "engine/sound/InstanceManager.h"
#endif // _ITF_INSTANCEMANAGER_H_


#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#include "engine/sound/routing/SoftwareBus.h"

namespace	ITF
{

    SourceHandle Adapter_WWISE::createSource(SourceInfo* _params, SoundInstance* _inst)
    {
        if (m_instanceManager == NULL || _params == NULL || _params->m_sound == NULL)
            return 0xFFFFFFFF;

        return m_instanceManager->createSource(_params->m_bus, _inst);
    }

    void Adapter_WWISE::getPauseSensitivePlayingVoices(SafeArray<SourceHandle>& _playingVoices)
    {
        return m_instanceManager->getPauseSensitivePlayingVoices(_playingVoices);
    }


	void Adapter_WWISE::setBusVolume(StringID _bus, Volume _volume, f32 _time)
	{
        if (isRunning() == false)
            return;

        ITF_MAP<StringID, SoftwareBus*>::const_iterator it = m_buses.find(_bus);
        if (it != m_buses.end())
        {
                //LOG("Adapter_WWISE Set [%s] volume : %f", it->first.getDebugString(), _volume.ratio());
                if (AUDIO_ADAPTER)
                {
                    SoftwareBus* bus = it->second;
                    if (bus->getBusDef()->getVolumeRtpc().isValid())
                    {
                        SoundRtpcID	rtpcVolumeID = AUDIO_ADAPTER->getIDFromGUID(bus->getBusDef()->getVolumeRtpc());
                        if (rtpcVolumeID != ITF_INVALID_SOUND_RTPC_ID)
                            AUDIO_ADAPTER->setRtpc(rtpcVolumeID, _volume.ratio(), ITF_INVALID_OBJREF); // or volume in db ?
                        else
                            AUDIO_ERROR("Sound : rtpcVolumeID \"%s\" is not defined.", bus->getBusDef()->getVolumeRtpc().getDebugString());
                    }
                }
        }
	}

    void Adapter_WWISE::setMasterVolume( Volume _volume)
    {
        if (isRunning() == false)
            return;

        StringID guidRTPCVol = m_guidRtpc.m_MenuGlobalGUID;
        if (guidRTPCVol.isValid())
        {
            SoundRtpcID	rtpcVolumeID = AUDIO_ADAPTER->getIDFromGUID(guidRTPCVol);
            if (rtpcVolumeID != ITF_INVALID_SOUND_RTPC_ID)
                AUDIO_ADAPTER->setRtpc(rtpcVolumeID, _volume.ratio(), ITF_INVALID_OBJREF); // or volume in db ?
            else
                AUDIO_ERROR("Sound : rtpcMenuVolume on MASTER is not defined.");

        }
        else
            AUDIO_ERROR("Sound : rtpcMenuVolume on MASTER is not defined.");

    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::setRtpc(SoundRtpcID _inputName, f32 _inputValue, ObjectRef _objectRef)
	{
		if(isRunning()==false)
			return;

		if(_objectRef.isValid())
		{
			const AkGameObjectID	akGameObjectID = Wwise::AkGameObjectFactory::s_getSingleton()->create(_objectRef);
			AK_SoundEngine_SetRTPCValue(_inputName, _inputValue, akGameObjectID);
		}
		else
		{
			// global scope
			AK_SoundEngine_SetRTPCValue(_inputName, _inputValue);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::setRtpc(SoundRtpcID _inputName, SoundRtpcID _inputValue, ObjectRef _objectRef)
	{
		if(isRunning()==false)
			return;

		if(_objectRef.isValid())
		{
			const AkGameObjectID	akGameObjectID = Wwise::AkGameObjectFactory::s_getSingleton()->create(_objectRef);
			AK_SoundEngine_SetSwitch(_inputName, _inputValue, akGameObjectID);
		}
		else
		{
			// this is a global scope => this is a "state"
			AK_SoundEngine_SetState(_inputName, _inputValue);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bool Adapter_WWISE::getRtpc(SoundRtpcID _inputName, f32 &_inputValue, ObjectRef _objectRef)
	{
		if(isRunning()==false)
			return false;

		AK::SoundEngine::Query::RTPCValue_type valueType;

		if(_objectRef.isValid())
		{
			// object scope
			const AKRESULT	res = AK_SoundEngine_Query_GetRTPCValue(_inputName, Wwise::AkGameObject::s_getAkGameObjectID(_objectRef), AK_INVALID_PLAYING_ID, _inputValue, valueType);
			if(res == AK_Success) return true;
		}
		else
		{
			valueType = AK::SoundEngine::Query::RTPCValue_Global;
			const AKRESULT	res = AK_SoundEngine_Query_GetRTPCValue(_inputName, AK_INVALID_GAME_OBJECT, AK_INVALID_PLAYING_ID, _inputValue, valueType);
			if(res == AK_Success) return true;
		}

		return false;
	}

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    bool Adapter_WWISE::getState(SoundRtpcID _inputName, SoundRtpcID &_value)
    {
		if(isRunning()==false)
			return false;

        AkStateID akStateID;
        if(AK_Success == AK_SoundEngine_Query_GetState(_inputName, akStateID))
        {
            _value = akStateID;
            return true;
        }

        return false;
    }

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    void Adapter_WWISE::setState(SoundRtpcID _inputName, SoundRtpcID _value)
    {
		if(isRunning()==false)
			return;
        AK_SoundEngine_SetState(_inputName, _value);
    }

    
    void Adapter_WWISE::setVolume(const ObjectRef _objectRef, const f32 _volume)
    {
        if (isRunning() == false)
            return;

        //get volume rtpc
        StringID guidRTPCVol = m_guidRtpc.m_volumeGUID;
        if (guidRTPCVol.isValid())
        {
            const AkGameObjectID	akGameObjectID = Wwise::AkGameObjectFactory::s_getSingleton()->create(_objectRef);

            ITF::SoundEventID  rtpcId = getIDFromGUID(guidRTPCVol);
            if(rtpcId != ITF_INVALID_SOUND_RTPC_ID)
                AK_SoundEngine_SetRTPCValue(rtpcId, _volume, akGameObjectID);


        }

    }

    void Adapter_WWISE::setPan(const ObjectRef _objectRef, const f32 _pan)
    {
        if (isRunning() == false)
            return;
        StringID guidRTPCPAN = m_guidRtpc.m_panGUID;
        if (guidRTPCPAN.isValid())
        {
            const AkGameObjectID	akGameObjectID = Wwise::AkGameObjectFactory::s_getSingleton()->create(_objectRef);

            ITF::SoundEventID  rtpcId = getIDFromGUID(guidRTPCPAN);
            if (rtpcId != ITF_INVALID_SOUND_RTPC_ID)
                AK_SoundEngine_SetRTPCValue(rtpcId, _pan, akGameObjectID);


        }

    }



    //const f32 value = m_filterHelper->computeHpfValue(_frequency);

    void Adapter_WWISE::setPitch(const ObjectRef _objectRef, f32 _pitch)
    {
        StringID guidRTPCpitch = m_guidRtpc.m_pitchGUID;
        if (guidRTPCpitch.isValid())
        {
            const AkGameObjectID	akGameObjectID = Wwise::AkGameObjectFactory::s_getSingleton()->create(_objectRef);

            ITF::SoundEventID  rtpcId = getIDFromGUID(guidRTPCpitch);
            if (rtpcId != ITF_INVALID_SOUND_RTPC_ID)
                AK_SoundEngine_SetRTPCValue(rtpcId, _pitch, akGameObjectID);


        }

    }

    ITF::ObjectRef Adapter_WWISE::createWwiseActor(const char* _name)
    {
        return   Wwise::AkGameObjectFactory::s_getSingleton()->createAkActor( _name);
    }

    //
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::pause(AudioPauseID _pauselevel)
	{
		if(isRunning()==false)
			return;

#ifdef ITF_SUPPORT_EDITOR
        if(m_ignorePauseDuringBankGenerations) return;
#endif
        AudioEngineEvent	audioEngineEvent = AUDIO_ENGEVT_INVALID;

		switch(_pauselevel)
		{
		case AUDIOPAUSE_NONE:				return;
		case AUDIOPAUSE_GAMEPLAY:			audioEngineEvent = AUDIO_ENGEVT_WWISE_PAUSE_GAMEPLAY; break;
		case AUDIOPAUSE_CUTSCENE:			audioEngineEvent = AUDIO_ENGEVT_WWISE_PAUSE_CUTSCENE; break;
		case AUDIOPAUSE_MENU:				audioEngineEvent = AUDIO_ENGEVT_WWISE_PAUSE_MENU; break;
		case AUDIOPAUSE_TRC:				audioEngineEvent = AUDIO_ENGEVT_WWISE_PAUSE_TRC; break;
		case AUDIOPAUSE_VIDEO:				audioEngineEvent = AUDIO_ENGEVT_WWISE_PAUSE_VIDEO; break;
		case AUDIOPAUSE_DEBUG:				audioEngineEvent = AUDIO_ENGEVT_WWISE_PAUSE_DEBUG; break;
		case AUDIOPAUSE_WIIU_BACKGROUND:	audioEngineEvent = AUDIO_ENGEVT_WWISE_PAUSE_WIIU; break;
		default:							AUDIO_WARNING("Sound : Pause ID \"%d\" is not defined.", _pauselevel); return;
		}

		if((audioEngineEvent != AUDIO_ENGEVT_INVALID) && (m_engineEventIDs[audioEngineEvent] != ITF_INVALID_SOUND_EVENT_ID))
		{
			play(AudioPlayRequest(m_engineEventIDs[audioEngineEvent]));
		}
	}


	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::pauseImmediate(AudioPauseID _pauselevel)
	{
		if(isRunning()==false)
			return;
		pause(_pauselevel);
		AK_SoundEngine_RenderAudio();//Process all events in the sound engine's queue

	}
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::resume(AudioPauseID _pauselevel)
	{

		if(isRunning()==false)
			return;

#ifdef ITF_SUPPORT_EDITOR
        if(m_ignorePauseDuringBankGenerations) return;
#endif
		AudioEngineEvent	audioEngineEvent = AUDIO_ENGEVT_INVALID;

		switch(_pauselevel)
		{
		case AUDIOPAUSE_NONE:				return;
		case AUDIOPAUSE_GAMEPLAY:			audioEngineEvent = AUDIO_ENGEVT_WWISE_RESUME_GAMEPLAY; break;
		case AUDIOPAUSE_CUTSCENE:			audioEngineEvent = AUDIO_ENGEVT_WWISE_RESUME_CUTSCENE; break;
		case AUDIOPAUSE_MENU:				audioEngineEvent = AUDIO_ENGEVT_WWISE_RESUME_MENU; break;
		case AUDIOPAUSE_TRC:				audioEngineEvent = AUDIO_ENGEVT_WWISE_RESUME_TRC; break;
		case AUDIOPAUSE_VIDEO:				audioEngineEvent = AUDIO_ENGEVT_WWISE_RESUME_VIDEO; break;
		case AUDIOPAUSE_DEBUG:				audioEngineEvent = AUDIO_ENGEVT_WWISE_RESUME_DEBUG; break;
		case AUDIOPAUSE_WIIU_BACKGROUND:	audioEngineEvent = AUDIO_ENGEVT_WWISE_RESUME_WIIU; break;
		default:							AUDIO_WARNING("Pause ID \"%d\" is not defined.", _pauselevel); return;
		}

		if((audioEngineEvent != AUDIO_ENGEVT_INVALID) && (m_engineEventIDs[audioEngineEvent] != ITF_INVALID_SOUND_EVENT_ID))
		{
			play(AudioPlayRequest(m_engineEventIDs[audioEngineEvent]));
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::muteDevice(AudioDeviceID _device)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unmuteDevice(AudioDeviceID _device)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::mute(bbool _isMute)
	{
		AK_SoundEngine_SetVolumeThreshold(_isMute ? MIN_VOLUME_THRESHOLD : MAX_VOLUME_THRESHOLD);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	ITF::SoundEventID Adapter_WWISE::getIDFromGUID(const StringID &_guid) const
	{
		if(m_config)
		{
			GUIDMap::const_iterator it = m_guidToID.find(_guid);

			if(it != m_guidToID.end())
			{
				return it->second.getID();
			}
		}

		return ITF_INVALID_SOUND_EVENT_ID;
	}

    f32 Adapter_WWISE::getDurationFromGUID(const StringID& _guid) const
    {
        if (m_config)
        {
            GUIDMap::const_iterator it = m_guidToID.find(_guid);

            if (it != m_guidToID.end())
            {
                return it->second.getDuration();
            }
        }

        return 0.f;
    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

#ifdef ITF_DEBUG_STRINGID

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const char *Adapter_WWISE::getWwiseNameFromGUID(const StringID &_guid) const
	{
		if(m_config)
		{
			WwiseGUIDNameMap::const_iterator	it = m_wwiseGUID2NameMap.find(_guid);

			if(it != m_wwiseGUID2NameMap.end())
			{
				return it->second.getDebugString();
			}
		}

		return _guid.isValid() ? _guid.getDebugString() : "???";
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const char *Adapter_WWISE::getWwiseNameFromID(SoundEventID _id) const
	{

		if(m_config)
		{
			WwiseIDNameMap::const_iterator	it = m_wwiseID2NameMap.find(_id);

			if(it != m_wwiseID2NameMap.end())
			{
				return it->second.getDebugString();
			}
		}
        String displayMsg;
        displayMsg.setTextFormat("%d", _id);
        StringConverter str(displayMsg);

        return str.getChar();
	}
#endif

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::registerActiveAuxBus(SoundBusID _busID, AudioBusSlotID _slotID, SoundEffectID _effectID)
	{
		if(isRunning()==false)
			return;

		for(ITF_VECTOR<ActiveAuxBus>::iterator it = m_activeAuxBuses.begin(), itEnd = m_activeAuxBuses.end(); it != itEnd; ++it)
		{
			if((it->first == _busID) && (it->second.first == _slotID)) 
            {
                it->second.second = _effectID;
                return;	// yet registered
            }
		}

        std::pair<AudioBusSlotID,SoundEffectID> eff(_slotID, _effectID);
		ActiveAuxBus	pair(_busID, eff);
		m_activeAuxBuses.push_back(pair);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::initDefaultAuxBusSettings()
	{
		if(isRunning()==false)
			return;
		if(m_config)
		{
// 			const ITF_VECTOR<EventSetWwiseAuxBusEffect>	&auxList = m_config->getWwiseDefaultAuxEffectList();
// 			for(ITF_VECTOR<EventSetWwiseAuxBusEffect>::const_iterator it = auxList.begin(), itEnd = auxList.end(); it != itEnd; ++it)
// 			{
// 				SoundRtpcID busID= getIDFromGUID(it->getBusGUID());
// 				SoundEffectID		auxBusID = getIDFromGUID(it->getEffectGUID());
// 				setBusEffect(busID,it->getBusSlotID(),auxBusID);
// 			}
		}

	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::resetAllActiveAuxBus()
	{
		if(isRunning()==false)
			return;
		for(ITF_VECTOR<ActiveAuxBus>::iterator it = m_activeAuxBuses.begin(), itEnd = m_activeAuxBuses.end(); it != itEnd; ++it)
		{
			AK_SoundEngine_SetBusEffect(it->first, it->second.first, AK_INVALID_UNIQUE_ID);
		}

		m_activeAuxBuses.clear();
	}

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    void Adapter_WWISE::forceActiveAuxBus()
    {
        static int count=0;
        if(count++ > 10)
        {
            count = 0;
            for(ITF_VECTOR<ActiveAuxBus>::iterator it = m_activeAuxBuses.begin(), itEnd = m_activeAuxBuses.end(); it != itEnd; ++it)
            {
                AK_SoundEngine_SetBusEffect(it->first, it->second.first, it->second.second);
            }
        }
    }

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::unregisterActiveAuxBus(SoundBusID _busID, AudioBusSlotID _slotID)
	{
		for(ITF_VECTOR<ActiveAuxBus>::iterator it = m_activeAuxBuses.begin(), itEnd = m_activeAuxBuses.end(); it != itEnd; ++it)
		{
			if((it->first == _busID) && (it->second.first == _slotID))
			{
				m_activeAuxBuses.erase(it);
				break;
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::setBusEffect(SoundBusID _busID, AudioBusSlotID _slotID, SoundEffectID _effectID)
	{
		AK_SoundEngine_SetBusEffect(_busID, _slotID, _effectID);
		registerActiveAuxBus(_busID, _slotID, _effectID);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::resetBusEffect(SoundBusID _busID, AudioBusSlotID _slotID)
	{
		if(isRunning()==false)
			return;
		AK_SoundEngine_SetBusEffect(_busID, _slotID, AK_INVALID_UNIQUE_ID);
		unregisterActiveAuxBus(_busID, _slotID);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::reInitBusEffect(SoundBusID _busID, AudioBusSlotID _slotID)
	{
		if(isRunning()==false)
			return;
		
		if(m_config)
		{
// 			const ITF_VECTOR<EventSetWwiseAuxBusEffect>	&auxList = m_config->getWwiseDefaultAuxEffectList();
// 			for(ITF_VECTOR<EventSetWwiseAuxBusEffect>::const_iterator it = auxList.begin(), itEnd = auxList.end(); it != itEnd; ++it)
// 			{
// 				SoundRtpcID busID= getIDFromGUID(it->getBusGUID());
// 				if( busID == _busID)
// 				{
// 					SoundEffectID		auxBusID = getIDFromGUID(it->getEffectGUID());
// 					setBusEffect(busID,it->getBusSlotID(),auxBusID);
// 				}
// 
// 			}
		}

	}
	

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::treatEndOfEvent()
	{
		//SoundHandle	audioInput = AudioInput::s_getSingleton() ? AudioInput::s_getSingleton()->getSoundHandle() : ITF_INVALID_SOUND_PLAYING_ID;
		SoundHandle	endedSoundHandle = SoundHandle::getInvalidHandle();
		while(m_endedEvents.read(endedSoundHandle))
		{
			PlayingIDs::iterator	it = m_playingIDs.find(endedSoundHandle.getValueAsU32());

			if(it != m_playingIDs.end())
            {
                AudioPlayRequest& request = it->second;
				const MetronomeType metronomeID = request.m_metronomeID;
               

				// delete metronome ref
				if(metronomeID < METRONOME_TYPE_COUNT) 
                {
                    m_metronomeState[metronomeID]->detach(endedSoundHandle);
                    MUSICMANAGER->notifyEnd(request.m_metronomeID, request.m_musicPart);
                 }


                // erase m_playingIDs ref
                m_playingIDs.erase(it);

				//if(audioInput == endedSoundHandle) AudioInput::s_getSingleton()->autostopped();
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::setListenerPosition(const Vec3d &_position, int _listenerIndex /* 0 */ )
	{
		if(_listenerIndex < Wwise::Configure::LISTENER_COUNT)
		{
			m_listenersState[_listenerIndex].m_lockedOnCameraPosition = false;
            m_listenersState[_listenerIndex].m_akPosition.SetPosition(_position.getX() , _position.getY(), _position.getZ());

		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::getListenerPosition(Vec3d *_position, int _listenerIndex /* 0 */ ) const
	{
		if(_listenerIndex < Wwise::Configure::LISTENER_COUNT)
		{
			_position->setX((f32)m_listenersState[_listenerIndex].m_akPosition.Position().X);
			_position->setY((f32)m_listenersState[_listenerIndex].m_akPosition.Position().Y);
			_position->setZ((f32)m_listenersState[_listenerIndex].m_akPosition.Position().Z);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::setListenerOrientation(const Vec3d &_front, const Vec3d &_top, int _listenerIndex /* 0 */ )
	{
		if(_listenerIndex < Wwise::Configure::LISTENER_COUNT)
		{
			m_listenersState[_listenerIndex].m_lockedOnCameraOrientation = false;

            m_listenersState[_listenerIndex].m_akPosition.SetOrientation(_top.getX() , _top.getY(), _top.getZ(), _front.getX(), _front.getY(), _front.getZ());

		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::getListenerOrientation(Vec3d *_front, Vec3d *_top, int _listenerIndex /* 0 */ ) const
	{
		if(_listenerIndex < Wwise::Configure::LISTENER_COUNT)
		{
			_top->setX(m_listenersState[_listenerIndex].m_akPosition.OrientationTop().X);
			_top->setY(m_listenersState[_listenerIndex].m_akPosition.OrientationTop().Y);
			_top->setZ(m_listenersState[_listenerIndex].m_akPosition.OrientationTop().Z);

			_front->setX(m_listenersState[_listenerIndex].m_akPosition.OrientationFront().X);
			_front->setY(m_listenersState[_listenerIndex].m_akPosition.OrientationFront().Y);
			_front->setZ(m_listenersState[_listenerIndex].m_akPosition.OrientationFront().Z);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::setActorPosition(ObjectRef _objectRef, const ITF_VECTOR<Vec3d> &_positions, AudioMultiPositionMode _mode)
	{
		const AkGameObjectID	akGameObjectID = Wwise::AkGameObjectFactory::s_getSingleton()->create(_objectRef);

		Wwise::AkGameObject * gao = Wwise::AkGameObjectFactory::s_getSingleton()->getAkGameObject(akGameObjectID);

		gao->setPosition(_positions, _mode);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::seek(SoundHandle _SoundHandle, f32 _seekMsec)
	{
		const AkGameObjectID	gameObjectID = AK_SoundEngine_Query_GetGameObjectFromPlayingID(_SoundHandle.getValueAsU32());
		const AkUniqueID		eventID = AK_SoundEngine_Query_GetEventIDFromPlayingID(_SoundHandle.getValueAsU32());

		AK_SoundEngine_SeekOnEvent(eventID, gameObjectID, AkTimeMs(_seekMsec));
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	i32 Adapter_WWISE::getSourcePlayPosition(SoundHandle _SoundHandle)
	{
		AkTimeMs sourcePosition = -1;
		if(AK_SoundEngine_GetSourcePlayPosition(_SoundHandle.getValueAsU32(), &sourcePosition) != AK_Success)
            return -1;

		return sourcePosition;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::pause(SoundHandle _SoundHandle)
	{
		PlayingIDs::const_iterator	it = m_playingIDs.find(_SoundHandle.getValueAsU32());
		if(it != m_playingIDs.end())
		{
			AK_SoundEngine_ExecuteActionOnEvent(it->second.m_soundEventID, AK::SoundEngine::AkActionOnEventType_Pause);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::resume(SoundHandle _SoundHandle)
	{
		PlayingIDs::const_iterator	it = m_playingIDs.find(_SoundHandle.getValueAsU32());
		if(it != m_playingIDs.end())
		{
			AK_SoundEngine_ExecuteActionOnEvent(it->second.m_soundEventID, AK::SoundEngine::AkActionOnEventType_Resume);
		}
	}


    f32 Adapter_WWISE::getLocalizationVolumeFromLocId( u32 _locId) const
    {
        if (m_config)
        {
            LocIdMap::const_iterator it = m_locIdToSound.find(_locId);

            if (it != m_locIdToSound.end())
            {
                return it->second.getVolume();
            }
        }

        return 0.f;
    }

    ITF::SoundEventID Adapter_WWISE::getLocalizationWWiseIdFromLocId( u32 _locId) const
    {
        if (m_config)
        {
            LocIdMap::const_iterator it = m_locIdToSound.find(_locId);

            if (it != m_locIdToSound.end())
            {
                return AUDIO_ADAPTER->getIDFromGUID(it->second.getGUID());

            }
        }

        return ITF_INVALID_SOUND_EVENT_ID;
    }

    StringID Adapter_WWISE::getLocalizationWWiseGUIdFromLocId( u32 _locId) const
    {
        if (m_config)
        {
            LocIdMap::const_iterator it = m_locIdToSound.find(_locId);

            if (it != m_locIdToSound.end())
            {
                return it->second.getGUID();

            }
        }

        return StringID::Invalid;
    }



#ifdef ITF_SUPPORT_PLUGIN

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::registerPlugins() const
	{
	}
#endif
#ifdef ITF_SUPPORT_EDITOR

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//


	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void Adapter_WWISE::visitAllPlayingInstances(void (*_visitor) (void *, const AudioPlayRequest &), void *_userParam) const
	{
		for(PlayingIDs::const_iterator it = m_playingIDs.begin(), itEnd = m_playingIDs.end(); it != itEnd; ++it)
		{
			_visitor(_userParam, it->second);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	void Adapter_WWISE::debugEffects()
	{
		for(ITF_VECTOR<ActiveAuxBus>::iterator it = m_activeAuxBuses.begin(), itEnd = m_activeAuxBuses.end(); it != itEnd; ++it)
		{
			 SoundEffectID effectID = it->second.second;
			 static char azID[32];

			 if(m_config)
			 {
				 WwiseIDNameMap::const_iterator	iterator = m_wwiseID2NameMap.find(effectID);

				 if(iterator != m_wwiseID2NameMap.end())
				 {
					 DebugDraw::text(ITF_POS2D_AUTO, ITF_POS2D_AUTO, Color::white(), "Current Effect : %s", iterator->second.getDebugString());

				 }
			 }

		}

	}
#endif

#if defined ITF_X360 || defined ITF_DURANGO
    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    ::IXAudio2* Adapter_WWISE::getXAudio2Interface()
    {
        return AK::GetWwiseXAudio2Interface();
    }
#endif

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
//     AudioCapture*   Adapter_WWISE::getGameAudioCaptureInterface() 
//     {
//         return m_gameOutputRecorder;
//     }

    ITF::ITF_LANGUAGE Adapter_WWISE::getCurrentLanguage()
    {
        return ITF::ITF_LANGUAGE_ENGLISH; //todoisap
       // return Wwise::Helper::staticCast(Wwise::Helper::getLanguageFromWwiseLanguageName(AK_StreamMgr_GetCurrentLanguage()));
    }




}
#endif // ITF_SUPPORT_WWISE
