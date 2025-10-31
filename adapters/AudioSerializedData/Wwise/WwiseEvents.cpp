#include "precompiled_AudioSerializedData.h"

#include "adapters/AudioSerializedData/Wwise/WwiseEvents.h"

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_



namespace ITF
{
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(EventLoadWwiseBank)
IMPLEMENT_OBJECT_RTTI(EventUnloadWwiseBank)
IMPLEMENT_OBJECT_RTTI(EventSetWwiseState)
IMPLEMENT_OBJECT_RTTI(EventSetWwiseSwitch)
IMPLEMENT_OBJECT_RTTI(EventPlayWwiseEvent)
IMPLEMENT_OBJECT_RTTI(SendMetronomedEvent)
IMPLEMENT_OBJECT_RTTI(EventSetWwiseAuxBusEffect)
IMPLEMENT_OBJECT_RTTI(EventResetWwiseAuxBusEffect)
IMPLEMENT_OBJECT_RTTI(EventMusicCustomCue)
IMPLEMENT_OBJECT_RTTI(EventHeadphonesPlugged)
IMPLEMENT_OBJECT_RTTI(EventHeadphonesUnplugged)

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventLoadWwiseBank)
SERIALIZE_CONTAINER("WwiseBankList", m_wwiseBankList);
END_SERIALIZATION()

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventUnloadWwiseBank)
    SERIALIZE_CONTAINER("WwiseBankList", m_wwiseBankList);
END_SERIALIZATION()

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventSetWwiseState)
    SERIALIZE_MEMBER("WwiseStateGroupGUID", m_wwiseStateGroupGUID);
    SERIALIZE_MEMBER("WwiseStateGUID", m_wwiseStateGUID);
END_SERIALIZATION()

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventSetWwiseSwitch)
    SERIALIZE_MEMBER("WwiseSwitchGroupGUID", m_wwiseSwitchGroupGUID);
    SERIALIZE_MEMBER("WwiseSwitchGUID", m_wwiseSwitchGUID);
END_SERIALIZATION() 

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventPlayWwiseEvent)
    SERIALIZE_MEMBER("WwiseEventGUID", m_wwiseEventGUID);
    SERIALIZE_MEMBER("soundPlayAfterdestroy", m_soundPlayAfterdestroy);
END_SERIALIZATION() 

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(SendMetronomedEvent)
    SERIALIZE_METRONOMETYPE("WwiseMetronomeID", m_wwiseMetronomeID);
    SERIALIZE_SYNCPLAY_TYPE("WwisePlayAt", m_wwisePlayAt);
	if(m_wwisePlayAt == AUDIO_SYNC_PLAY_AT_NEXT_CUSTOM_CUE)
	{ 
		SERIALIZE_MEMBER("WwisePlayAtCue", m_wwisePlayAtCue);
	}
    SERIALIZE_OBJECT_WITH_FACTORY("onPlayEvent",m_onPlayEvent, ACTORSMANAGER->getEventFactory());
    SERIALIZE_MEMBER("SendTo", m_onPlayEventObjectPath);
END_SERIALIZATION() 

void SendMetronomedEvent::setOnPlayEvent(Event *_val)
{
	if(m_onPlayEvent) SF_DEL(m_onPlayEvent);
	if(_val)
	{
		m_onPlayEvent = static_cast<Event *>(_val->CreateNewObject());
		BinaryClone(_val, m_onPlayEvent);
	}
	else
		m_onPlayEvent = NULL;
}

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventSetWwiseAuxBusEffect)
    SERIALIZE_MEMBER("WwiseBusGUID", m_wwiseBusGUID);
    SERIALIZE_MEMBER("WwiseEffectGUID", m_wwiseEffectGUID);
    SERIALIZE_ENUM_BEGIN("WwiseBusSlotID", m_wwiseBusSlotID);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_0);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_1);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_2);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_3);
    SERIALIZE_ENUM_END();
END_SERIALIZATION() 

    //////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventResetWwiseAuxBusEffect)
    SERIALIZE_MEMBER("WwiseBusGUID", m_wwiseBusGUID);
    SERIALIZE_ENUM_BEGIN("WwiseBusSlotID", m_wwiseBusSlotID);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_0);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_1);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_2);
        SERIALIZE_ENUM_VAR(AUDIO_BUS_SLOT_3);
    SERIALIZE_ENUM_END();
END_SERIALIZATION() 

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventMusicCustomCue)
    SERIALIZE_METRONOMETYPE("Metronome", m_metronomeID);
    SERIALIZE_MEMBER("Cue", m_cue);
END_SERIALIZATION() 

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventHeadphonesPlugged)
END_SERIALIZATION() 

//////////////////////////////////////////////////////////////////////////
BEGIN_SERIALIZATION_CHILD(EventHeadphonesUnplugged)
END_SERIALIZATION() 



//////////////////////////////////////////////////////////////////////////


} //namespace ITF
