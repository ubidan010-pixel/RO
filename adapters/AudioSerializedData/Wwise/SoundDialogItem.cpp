#include "precompiled_AudioSerializedData.h"
#include "adapters/AudioSerializedData/Wwise/SoundDialogItem.h"



namespace	ITF
{
	BEGIN_SERIALIZATION(SoundDialogItem)
	    SERIALIZE_MEMBER("Sound", m_waveFile);
        SERIALIZE_MEMBER("WwiseEventGuid", m_WWiseguid);
        SERIALIZE_MEMBER("VolumedB", m_volumedB);
        String8 idAsStr;
        idAsStr.setTextFormat("%u", m_lineId);
        SERIALIZE_MEMBER("lineId", idAsStr);
        sscanf(idAsStr.cStr(), "%u", &m_lineId);
	END_SERIALIZATION()
}

