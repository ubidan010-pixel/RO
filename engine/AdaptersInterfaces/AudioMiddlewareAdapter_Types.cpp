#include "precompiled_engine.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "engine/events/Events.h"

namespace	ITF
{
    
	// ===================================================================================================================
	AudioSendEventRequest::AudioSendEventRequest() :
		m_metronomeID(METRONOME_TYPE_INVALID),
		m_playAt(AUDIO_SYNC_PLAY_IMMEDIATE),
		m_playAtCue(),
		m_onPlayEvent(NULL),
		m_sentToObjectRef(ITF_INVALID_OBJREF)
	{
	}

	// ===================================================================================================================
	AudioSendEventRequest::AudioSendEventRequest(MetronomeType _metronomeID, AudioSyncPlayAt _playAt, StringID _playAtCue) :
		m_metronomeID(_metronomeID),
		m_playAt(_playAt),
		m_playAtCue(_playAtCue),
		m_onPlayEvent(NULL),
		m_sentToObjectRef(ITF_INVALID_OBJREF)
	{
	}

	// ===================================================================================================================
	AudioSendEventRequest::AudioSendEventRequest(const AudioSendEventRequest &_ref) :
		m_metronomeID(_ref.m_metronomeID),
		m_playAt(_ref.m_playAt),
		m_playAtCue(_ref.m_playAtCue),
		m_sentToObjectRef(_ref.m_sentToObjectRef)
	{
		if(_ref.m_onPlayEvent)
		{
			m_onPlayEvent = static_cast<Event *>(_ref.m_onPlayEvent->CreateNewObject());
			BinaryClone(_ref.m_onPlayEvent, m_onPlayEvent);
		}
		else
		{
			m_onPlayEvent = NULL;
		}
	}

	// ===================================================================================================================
	AudioSendEventRequest::~AudioSendEventRequest()
	{
		if(m_onPlayEvent)
		{
			SF_DEL(m_onPlayEvent);
		}
	}

	// ===================================================================================================================
	const AudioSendEventRequest &AudioSendEventRequest::operator=(const AudioSendEventRequest &_ref)
	{
		m_metronomeID = _ref.m_metronomeID;
		m_playAt = _ref.m_playAt;
		m_playAtCue = _ref.m_playAtCue;
		m_sentToObjectRef = _ref.m_sentToObjectRef;

		setOnPlayEvent(_ref.m_onPlayEvent);

		return *this;
	}

	// ===================================================================================================================
	void AudioSendEventRequest::setOnPlayEvent(Event *_val)
	{
		if(m_onPlayEvent) SF_DEL(m_onPlayEvent);

		if(_val)
		{
			m_onPlayEvent = static_cast<Event *>(_val->CreateNewObject());
			BinaryClone(_val, m_onPlayEvent);
		}
		else
		{
			m_onPlayEvent = NULL;
		}
	}


	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AudioPlayRequest::AudioPlayRequest(SoundEventID _soundEventID) : AudioSendEventRequest(),
		m_soundEventID(_soundEventID),
		m_objectRef(ITF_INVALID_OBJREF),
		m_stopOnObjecDestroy(false),
		m_sourcePlayPosition(false)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AudioPlayRequest::AudioPlayRequest(SoundEventID _soundEventID, ObjectRef _objectRef) : AudioSendEventRequest(),
		m_soundEventID(_soundEventID),
		m_objectRef(_objectRef),
		m_stopOnObjecDestroy(false),
		m_sourcePlayPosition(false)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AudioPlayRequest::AudioPlayRequest(SoundEventID _soundEventID, ObjectRef _objectRef, bool _stopOnObjecDestroy, MetronomeType _metronomeID, AudioSyncPlayAt _playAt, StringID _playAtCue, bool _sourcePlayPosition) 
		: AudioSendEventRequest(_metronomeID, _playAt, _playAtCue),
		m_soundEventID(_soundEventID),
		m_objectRef(_objectRef),
		m_stopOnObjecDestroy(_stopOnObjecDestroy),
		m_sourcePlayPosition(_sourcePlayPosition)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AudioPlayRequest::AudioPlayRequest(const AudioPlayRequest &_ref) : AudioSendEventRequest(_ref),
		m_soundEventID(_ref.m_soundEventID),
		m_objectRef(_ref.m_objectRef),
		m_stopOnObjecDestroy(_ref.m_stopOnObjecDestroy),
		m_sourcePlayPosition(_ref.m_sourcePlayPosition)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	AudioPlayRequest::~AudioPlayRequest()
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	const AudioPlayRequest &AudioPlayRequest::operator=(const AudioPlayRequest &_ref)
	{
		m_soundEventID = _ref.m_soundEventID;
		m_objectRef = _ref.m_objectRef;
		m_stopOnObjecDestroy = _ref.m_stopOnObjecDestroy;
		m_metronomeID = _ref.m_metronomeID;
		m_playAt = _ref.m_playAt;
		m_playAtCue = _ref.m_playAtCue;
		m_sentToObjectRef = _ref.m_sentToObjectRef;
		m_sourcePlayPosition = _ref.m_sourcePlayPosition;
                m_musicPart = _ref.m_musicPart;

        setOnPlayEvent(_ref.getOnPlayEvent());

		return *this;
	}
    
}
