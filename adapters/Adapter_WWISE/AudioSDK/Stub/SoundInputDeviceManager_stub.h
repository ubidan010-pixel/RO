#ifndef __SOUNDINPUTDEVICEMANAGER_STUB_H__
#define __SOUNDINPUTDEVICEMANAGER_STUB_H__

#include "adapters/Adapter_WWISE/AudioSDK/AudioStructs.h"

namespace	AudioSDK
{
    class SoundRecorder_stub;
	class	SoundInputDeviceManager_stub
	{
			static SoundInputDeviceManager_stub *ms_singleton;


	public:
			inline static SoundInputDeviceManager_stub	*s_getSingleton()	{ return ms_singleton; }
			static void									s_createSingleton() { ms_singleton = WWISE_NEW(SoundInputDeviceManager_stub) (); }
			static void									s_destroySingleton(){ if(ms_singleton){ WWISE_DELETE(ms_singleton); } }

            u32											getDefaultDeviceID() const {return 0;}
            SoundRecorder_stub							*createRecorder();
            void										destroyRecorder(SoundRecorder_stub *_recoreder);

    private:
			~										SoundInputDeviceManager_stub()	{ }
			SoundInputDeviceManager_stub()											{ }
			SoundInputDeviceManager_stub(const SoundInputDeviceManager_stub &);					// Prevent copy-construction
			SoundInputDeviceManager_stub &operator	=(const SoundInputDeviceManager_stub &);	// Prevent assignment
	};
}
#endif // __SOUNDINPUTDEVICEMANAGER_STUB_H__
