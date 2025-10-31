#include "precompiled_AudioSerializedData.h"
#include "adapters/AudioSerializedData/Wwise/WwiseItem.h"



namespace	ITF
{
	BEGIN_SERIALIZATION(WwiseItem)
	   SERIALIZE_MEMBER("WwiseEventName", m_name);
        SERIALIZE_MEMBER("WwiseEventGuid", m_guid);
        SERIALIZE_MEMBER("WwiseDuration", m_duration);

        String8 idAsStr;
        idAsStr.setTextFormat("%u", m_id);
        SERIALIZE_MEMBER("WwiseEventId", idAsStr);
        sscanf(idAsStr.cStr(), "%u", &m_id);
	END_SERIALIZATION()
}

// 	SERIALIZE_ENUM_BEGIN("Type", m_type);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_EVENT);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_GAME_PARAMETER);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_STATE_GROUP);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_STATE);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_SWITCH_GROUP);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_SWITCH);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_TRIGGER);
//         SERIALIZE_ENUM_VAR(AUDIO_ITEM_WWISE_DIALOGUE_EVENT);
// 	SERIALIZE_ENUM_END();
