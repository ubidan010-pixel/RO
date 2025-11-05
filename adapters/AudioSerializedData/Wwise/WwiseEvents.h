#ifndef __WWISEEVENTS_H__
#define __WWISEEVENTS_H__

#include "engine/events/Events.h"


namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
#define EventLoadWwiseBank_CRC ITF_GET_STRINGID_CRC(EventLoadWwiseBank ,3664813256)
    class EventLoadWwiseBank : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventLoadWwiseBank,Event,EventLoadWwiseBank_CRC);
        DECLARE_SERIALIZE()

    public :
        EventLoadWwiseBank() {}
        ~EventLoadWwiseBank() {}

        inline const ITF_VECTOR<Path>& getBankList() const { return m_wwiseBankList; }

    private:
        ITF_VECTOR<Path>	m_wwiseBankList;
    };

    //////////////////////////////////////////////////////////////////////////
#define EventUnloadWwiseBank_CRC ITF_GET_STRINGID_CRC(EventUnloadWwiseBank ,3411715416)
    class EventUnloadWwiseBank : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventUnloadWwiseBank,Event,EventUnloadWwiseBank_CRC);
        DECLARE_SERIALIZE()

    public :
        EventUnloadWwiseBank() {}
        ~EventUnloadWwiseBank() {}

        inline const ITF_VECTOR<Path>& getBankList() const { return m_wwiseBankList; }

    private:
        ITF_VECTOR<Path>	m_wwiseBankList;
    };

    //////////////////////////////////////////////////////////////////////////
#define EventSetWwiseState_CRC ITF_GET_STRINGID_CRC(EventSetWwiseState,3353127953)
    class EventSetWwiseState : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetWwiseState,Event,EventSetWwiseState_CRC);
        DECLARE_SERIALIZE()

    public :
        EventSetWwiseState() {}
        ~EventSetWwiseState() {}
        
        inline const StringID& getStateGroupGUID() const { return m_wwiseStateGroupGUID; }
        inline const StringID& getStateGUID() const { return m_wwiseStateGUID; }

    private:
        StringID    m_wwiseStateGroupGUID;
        StringID    m_wwiseStateGUID;
    };

    //////////////////////////////////////////////////////////////////////////
#define EventSetWwiseSwitch_CRC ITF_GET_STRINGID_CRC(EventSetWwiseSwitch,2278317649)
    class EventSetWwiseSwitch : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetWwiseSwitch,Event,EventSetWwiseSwitch_CRC);
        DECLARE_SERIALIZE()

    public :
        EventSetWwiseSwitch() {}
        ~EventSetWwiseSwitch() {}

        inline const StringID& getSwitchGroupGUID() const { return m_wwiseSwitchGroupGUID; }
        inline const StringID& getSwitchGUID() const { return m_wwiseSwitchGUID; }

    private:
        StringID    m_wwiseSwitchGroupGUID;
        StringID    m_wwiseSwitchGUID;
    };

	//////////////////////////////////////////////////////////////////////////
#define SendMetronomedEvent_CRC ITF_GET_STRINGID_CRC(SendMetronomedEvent,3358737088)
	class SendMetronomedEvent : public Event
	{
		DECLARE_OBJECT_CHILD_RTTI(SendMetronomedEvent,Event,SendMetronomedEvent_CRC);
		DECLARE_SERIALIZE()

	public :
		SendMetronomedEvent(MetronomeType _metroType = METRONOME_TYPE_INVALID, AudioSyncPlayAt _playAt = AUDIO_SYNC_PLAY_IMMEDIATE, const StringID &_playAtCue = StringID::Invalid) : 
			m_wwiseMetronomeID(_metroType), m_wwisePlayAt(_playAt), m_wwisePlayAtCue(_playAtCue), m_onPlayEvent(NULL)
		{}

		virtual ~SendMetronomedEvent() 
		{
			SF_DEL(m_onPlayEvent);
		}

		inline MetronomeType    getMetronomeID() const { return m_wwiseMetronomeID; }
		inline AudioSyncPlayAt  getPlayAt() const       { return m_wwisePlayAt; }
		inline StringID         getPlayAtCue() const    { return m_wwisePlayAtCue; }
		inline Event *          getOnPlayEvent() const    { return m_onPlayEvent; }
		inline const ObjectPath&getOnPlayEventObjectPath() const { return m_onPlayEventObjectPath; }

		void					setOnPlayEvent(Event *_val);

	private:
		MetronomeType    m_wwiseMetronomeID;
		AudioSyncPlayAt  m_wwisePlayAt;
		StringID         m_wwisePlayAtCue;

		Event		*m_onPlayEvent;
		ObjectPath	m_onPlayEventObjectPath;
	};

	//////////////////////////////////////////////////////////////////////////
#define EventPlayWwiseEvent_CRC ITF_GET_STRINGID_CRC(EventPlayWwiseEvent,4259299401)
	class EventPlayWwiseEvent : public SendMetronomedEvent
	{
		DECLARE_OBJECT_CHILD_RTTI(EventPlayWwiseEvent,SendMetronomedEvent,EventPlayWwiseEvent_CRC);
		DECLARE_SERIALIZE()

	public :
		EventPlayWwiseEvent() : Super(), m_soundPlayAfterdestroy(bfalse)
		{}

		virtual ~EventPlayWwiseEvent() 
		{}

		inline const StringID&  getEventGUID() const	{ return m_wwiseEventGUID; }
		inline bbool            stopOnDestroy() const   { return !m_soundPlayAfterdestroy; }

	private:
		StringID         m_wwiseEventGUID;
		bbool            m_soundPlayAfterdestroy;
	};


    //////////////////////////////////////////////////////////////////////////
#define EventSetWwiseAuxBusEffect_CRC ITF_GET_STRINGID_CRC(EventSetWwiseAuxBusEffect,2899118355)
    class EventSetWwiseAuxBusEffect : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetWwiseAuxBusEffect,Event,EventSetWwiseAuxBusEffect_CRC);
        DECLARE_SERIALIZE()

    public :
        EventSetWwiseAuxBusEffect() : m_wwiseBusSlotID(AUDIO_BUS_SLOT_0) {}
        ~EventSetWwiseAuxBusEffect() {}

        inline const StringID& getBusGUID() const { return m_wwiseBusGUID; }
        inline const StringID& getEffectGUID() const { return m_wwiseEffectGUID; }
        inline  AudioBusSlotID getBusSlotID() const { return m_wwiseBusSlotID; }

    private:
        StringID         m_wwiseBusGUID;
        StringID         m_wwiseEffectGUID;
        AudioBusSlotID   m_wwiseBusSlotID;             
    };

    //////////////////////////////////////////////////////////////////////////
#define EventResetWwiseAuxBusEffect_CRC ITF_GET_STRINGID_CRC(EventResetWwiseAuxBusEffect,77853971)
    class EventResetWwiseAuxBusEffect : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventResetWwiseAuxBusEffect,Event,EventResetWwiseAuxBusEffect_CRC);
        DECLARE_SERIALIZE()

    public :
        EventResetWwiseAuxBusEffect() : m_wwiseBusSlotID(AUDIO_BUS_SLOT_0) {}
        ~EventResetWwiseAuxBusEffect() {}

        inline const StringID& getBusGUID() const { return m_wwiseBusGUID; }
        inline AudioBusSlotID getBusSlotID() const { return m_wwiseBusSlotID; }
		void  setBusGUID( const StringID&  _id) { m_wwiseBusGUID = _id; }
		void setBusSlotID( AudioBusSlotID  _id) { m_wwiseBusSlotID = _id; }
    private:
        StringID         m_wwiseBusGUID;
        AudioBusSlotID   m_wwiseBusSlotID;             
    };


    //////////////////////////////////////////////////////////////////////////
#define EventMusicCustomCue_CRC ITF_GET_STRINGID_CRC(EventMusicCustomCue,3460096081)
    class EventMusicCustomCue : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventMusicCustomCue,Event,EventMusicCustomCue_CRC);
        DECLARE_SERIALIZE()

    public :
        EventMusicCustomCue() : m_metronomeID(METRONOME_TYPE_INVALID) {}
        ~EventMusicCustomCue() {}

        inline ITF::MetronomeType getMetronomeID() const { return m_metronomeID; }
        inline void setMetronomeID(ITF::MetronomeType val) { m_metronomeID = val; }
        
        inline ITF::StringID getCue() const { return m_cue; }
        inline void setCue(ITF::StringID val) { m_cue = val; }

    private:
       MetronomeType    m_metronomeID;
       StringID         m_cue;
    };

    //////////////////////////////////////////////////////////////////////////
#define EventHeadphonesPlugged_CRC ITF_GET_STRINGID_CRC(EventHeadphonesPlugged,429751742)
    class EventHeadphonesPlugged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventHeadphonesPlugged,Event,EventHeadphonesPlugged_CRC);
        DECLARE_SERIALIZE()

    public :
        EventHeadphonesPlugged()  {}
        ~EventHeadphonesPlugged() {}
    };

    //////////////////////////////////////////////////////////////////////////
#define EventHeadphonesUnplugged_CRC ITF_GET_STRINGID_CRC(EventHeadphonesUnplugged,3289978366)
    class EventHeadphonesUnplugged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventHeadphonesUnplugged,Event,EventHeadphonesUnplugged_CRC);
        DECLARE_SERIALIZE()

    public :
        EventHeadphonesUnplugged()  {}
        ~EventHeadphonesUnplugged() {}
    };

    //////////////////////////////////////////////////////////////////////////
} // namespace ITF

#endif // __WWISEEVENTS_H__
