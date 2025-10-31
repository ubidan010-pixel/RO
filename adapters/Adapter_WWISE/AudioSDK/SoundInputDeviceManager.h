#ifndef __SOUNDINPUTDEVICEMANAGER_H__
#define __SOUNDINPUTDEVICEMANAGER_H__


#if defined ITF_WINDOWS
    #include "adapters/Adapter_WWISE/AudioSDK/Win32/SoundInputDeviceManager_win.h"
    namespace AudioSDK
    {
        typedef SoundInputDeviceManager_win32 SoundInputDeviceManager;
    }

#elif defined ITF_ANDROID || defined ITF_X360 || defined ITF_DURANGO || defined ITF_PS3 || defined ITF_ORBIS
    #include "adapters/Adapter_WWISE/AudioSDK/Stub/SoundInputDeviceManager_stub.h"
    namespace AudioSDK
    {
        typedef SoundInputDeviceManager_stub SoundInputDeviceManager;
    }
#else
#error Unknown plateform!
#endif


#endif // __SOUNDINPUTDEVICEMANAGER_H__
