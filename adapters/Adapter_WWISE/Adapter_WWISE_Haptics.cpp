#include "precompiled_Adapter_WWISE.h"

#include "AudioSDK/error.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#ifdef USE_PAD_HAPTICS
#include <AK/Plugin/AkMotionSinkFactory.h>
#pragma comment(lib, "AkMotionSink")
// Link statically with AkMotionSource
#include <AK/Plugin/AkMotionSourceSourceFactory.h>
#pragma comment(lib, "AkMotionSourceSource")
#ifdef ITF_WINDOWS
#include <mmdeviceapi.h>
#include <propkey.h>
#include <tchar.h>
#include <libScePadforPCGames/include/pad.h>
#include <libScePadforPCGames/include/pad_audio.h>
#include <libScePadforPCGames/include/pad_windows_static.h>
#endif
#include <AK/Plugin/AkMotionSinkScePadHelpers.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
namespace	ITF
{
    void Adapter_WWISE::initMotionLib()
    {
#ifdef ITF_WINDOWS
        AKMOTIONSINK_STATIC_LINK_SCEPAD_FUNCTIONS
#endif
    }
    bbool Adapter_WWISE::registerHaptics(u32 _pad, u32 _deviceId, u32 _deviceOutputId, bbool _isSony)
    {
        u32 padHandle = _deviceId;
#ifdef ITF_WINDOWS
        if (_isSony)
        {
            ScePadControllerType padType;
            int ret = scePadGetControllerType(_deviceId, &padType);
            if (ret == SCE_OK && padType != SCE_PAD_CONTROLLER_TYPE_NOT_CONNECTED)
            {
                padHandle = padType == SCE_PAD_CONTROLLER_TYPE_DUALSENSE
                                ? (_deviceId | AKMOTION_SCEPAD_HAPTICS_MODE)
                                : _deviceId;
            }
        }
#endif
        AkOutputSettings montionSetting(CONTROLLER_MONTION, padHandle);
        auto res = AK::SoundEngine::AddOutput(montionSetting, &m_PlayerMotionOutput[_pad]);
        if (res != AK_Success)
        {
            AUDIO_WARNING("Register Controller Speaker error %d with pad %d",res,_pad);
            return bfalse;
        }
        else
        {
            return btrue;
        }
    }

    bbool Adapter_WWISE::unregisterHaptics(u32 _pad)
    {
        // AkOutputSettings montionSetting(CONTROLLER_MONTION, pad);
        if (m_PlayerMotionOutput[_pad] != AK_INVALID_OUTPUT_DEVICE_ID)
        {
            auto res = AK::SoundEngine::RemoveOutput(m_PlayerMotionOutput[_pad]);
            m_PlayerMotionOutput[_pad] = AK_INVALID_OUTPUT_DEVICE_ID;
            if (res != AK_Success)
            {
                AUDIO_WARNING("Unregister Controller Speaker error %d with pad %d",res,_pad);
                return bfalse;
            }
            else {
                return btrue;
            }
        }
        else
        {
            AUDIO_WARNING("Register Controller Speaker error. Cant find device output %d",_pad);
            return bfalse;
        }
    }

    bbool Adapter_WWISE::registerControllerSpeaker(u32 _pad, u32 _deviceId, u32 _deviceOutputId, bbool _isSony)
    {
        AkOutputSettings padSpkOutputSettings;
        padSpkOutputSettings.audioDeviceShareset = CONTROLLER_SPEAKER_SHARESET;
        padSpkOutputSettings.idDevice = _deviceOutputId; //AK::GetDeviceID();
        padSpkOutputSettings.channelConfig.SetStandard(AK_SPEAKER_SETUP_STEREO);
#ifdef ITF_WINDOWS
        if (_isSony)
        {
            scePadSetAudioOutPath(static_cast<int32_t>(_deviceId), SCE_PAD_AUDIO_OUT_PATH_SPEAKER);
            ScePadVolumeGain padGain = {127, 127, 0, 127};
            scePadSetVolumeGain(static_cast<int32_t>(_deviceId), &padGain);
        }

#endif
        AKRESULT res = AK::SoundEngine::AddOutput(padSpkOutputSettings, &m_PlayerAudioOutput[_pad]);
        if (res != AK_Success)
        {
            AUDIO_WARNING("Register Controller Speaker error %d with pad %d",res,_pad);
            return bfalse;
        }
        else
        {
            return btrue;
        }

    }

    bbool Adapter_WWISE::unregisterControllerSpeaker(u32 _pad)
    {
        if (m_PlayerAudioOutput[_pad] != AK_INVALID_OUTPUT_DEVICE_ID)
        {
            auto res = AK::SoundEngine::RemoveOutput(m_PlayerAudioOutput[_pad]);
            m_PlayerAudioOutput[_pad] = AK_INVALID_OUTPUT_DEVICE_ID;
            if (res != AK_Success)
            {
                AUDIO_WARNING("Unregister Controller Speaker error %d with pad %d",res,_pad);
                return bfalse;
            }
            else {
                return btrue;
            }
        }
        else
        {
            AUDIO_WARNING("Register Controller Speaker error. Cant find device output %d",_pad);
            return bfalse;
        }
    }
#ifdef ITF_WINDOWS
    u32 Adapter_WWISE::getDeviceId(IMMDevice* _imDevice)
    {
        if (_imDevice == nullptr)
        {
            return 0;
        }
        else
        {
            return AK::GetDeviceID(_imDevice);
        }

    }
#endif
}
#endif
#endif
