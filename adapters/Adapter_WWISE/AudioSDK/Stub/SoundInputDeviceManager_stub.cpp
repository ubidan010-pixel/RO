#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/AudioSDK/Stub/SoundInputDeviceManager_stub.h"
#include "adapters/Adapter_WWISE/AudioSDK/Stub/SoundRecorder_stub.h"

namespace	AudioSDK
{
	SoundInputDeviceManager_stub *AudioSDK::SoundInputDeviceManager_stub::	ms_singleton = NULL;

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	SoundRecorder_stub *SoundInputDeviceManager_stub::createRecorder()
	{
		return WWISE_NEW(SoundRecorder_stub);
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void SoundInputDeviceManager_stub::destroyRecorder(SoundRecorder_stub *_recorder)
	{
		WWISE_DELETE(_recorder);
	}
}
#endif
