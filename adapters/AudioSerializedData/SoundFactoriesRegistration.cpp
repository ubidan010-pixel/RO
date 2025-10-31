#include "precompiled_AudioSerializedData.h"

#include "adapters/AudioSerializedData/SoundFactoriesRegistration.h"
#if defined ITF_SUPPORT_RAKI
#include "engine/events/Events.h"
#endif
#include "engine/actors/components/SoundComponent.h"
#include "engine/actors/components/UIMenuSoundComponent.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/AudioSerializedData/Wwise/WwiseEvents.h"

#endif
namespace	ITF
{

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void AudioSerializedData::registerEvents(ObjectFactory &_objectFactory)
	{
#ifdef ITF_SUPPORT_WWISE
		_objectFactory.RegisterObject<EventLoadWwiseBank> (EventLoadWwiseBank_CRC);
		_objectFactory.RegisterObject<EventUnloadWwiseBank> (EventUnloadWwiseBank_CRC);
		_objectFactory.RegisterObject<EventSetWwiseState> (EventSetWwiseState_CRC);
		_objectFactory.RegisterObject<EventSetWwiseSwitch> (EventSetWwiseSwitch_CRC);
		_objectFactory.RegisterObject<EventPlayWwiseEvent> (EventPlayWwiseEvent_CRC);		
		_objectFactory.RegisterObject<SendMetronomedEvent> (SendMetronomedEvent_CRC);
		_objectFactory.RegisterObject<EventSetWwiseAuxBusEffect> (EventSetWwiseAuxBusEffect_CRC);
		_objectFactory.RegisterObject<EventResetWwiseAuxBusEffect> (EventResetWwiseAuxBusEffect_CRC);
		_objectFactory.RegisterObject<EventMusicCustomCue> (EventMusicCustomCue_CRC);
		_objectFactory.RegisterObject<EventHeadphonesPlugged> (EventHeadphonesPlugged_CRC);
		_objectFactory.RegisterObject<EventHeadphonesUnplugged> (EventHeadphonesUnplugged_CRC);

#endif
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void AudioSerializedData::registerComponents(ObjectFactory &_objectFactory)
	{
		_objectFactory.RegisterObject<SoundComponent> (ITF_GET_STRINGID_CRC(SoundComponent, 2111333436));
		_objectFactory.RegisterObject<UIMenuSoundComponent> (ITF_GET_STRINGID_CRC(UIMenuSoundComponent, 3788816759));


	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void AudioSerializedData::registerTemplateComponents(ObjectFactory &_objectFactory)
	{
		_objectFactory.RegisterObject<SoundComponent_Template> (ITF_GET_STRINGID_CRC(SoundComponent_Template, 3645729875));
		_objectFactory.RegisterObject<UIMenuSoundComponent_Template> (ITF_GET_STRINGID_CRC(UIMenuSoundComponent_Template, 473615840));

	}
}	// namespace ITF
