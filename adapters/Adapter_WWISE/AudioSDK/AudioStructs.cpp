#include "precompiled_Adapter_WWISE.h"
#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"

namespace	AudioSDK
{
    const AudioStructs::Guid AudioStructs::c_ksDataFormatSubtypeGuidBase{ (u32)AudioStructs::FORMAT_PCM, (u16)0x0000, (u16)0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71} };
	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	AudioStructs::WaveFormatTag AudioStructs::s_extractWaveFormatTag(const Guid &_guid)
	{
		if
		(
			(_guid.Data2 == c_ksDataFormatSubtypeGuidBase.Data2)
		&&	(_guid.Data3 == c_ksDataFormatSubtypeGuidBase.Data3)
		&&	(_guid.Data4[0] == c_ksDataFormatSubtypeGuidBase.Data4[0])
		&&	(_guid.Data4[1] == c_ksDataFormatSubtypeGuidBase.Data4[1])
		&&	(_guid.Data4[2] == c_ksDataFormatSubtypeGuidBase.Data4[2])
		&&	(_guid.Data4[3] == c_ksDataFormatSubtypeGuidBase.Data4[3])
		&&	(_guid.Data4[4] == c_ksDataFormatSubtypeGuidBase.Data4[4])
		&&	(_guid.Data4[5] == c_ksDataFormatSubtypeGuidBase.Data4[5])
		&&	(_guid.Data4[6] == c_ksDataFormatSubtypeGuidBase.Data4[6])
		&&	(_guid.Data4[7] == c_ksDataFormatSubtypeGuidBase.Data4[7])
		)
		{
			switch(_guid.Data1)
			{
			case AudioStructs::FORMAT_PCM:			return AudioStructs::FORMAT_PCM;
			case AudioStructs::FORMAT_IEEE_FLOAT:	return AudioStructs::FORMAT_IEEE_FLOAT;
			default:									break;
			}
		}

		return AudioStructs::FORMAT_UNKNOWN;
	}
}
