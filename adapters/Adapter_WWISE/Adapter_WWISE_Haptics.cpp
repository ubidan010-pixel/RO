#include "precompiled_Adapter_WWISE.h"

#include "AudioSDK/error.h"
#include "Wrapper/WWISE_Wrapper.h"
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
#include <AK/Plugin/AkMotionSinkScePadHelpers.h>
#endif
#include <AK/SoundEngine/Common/AkSoundEngine.h>

namespace ITF
{
    void Adapter_WWISE::initMotionLib()
    {
#ifdef ITF_WINDOWS
        AKMOTIONSINK_STATIC_LINK_SCEPAD_FUNCTIONS
#endif
    }

    bbool Adapter_WWISE::registerHaptics(u32 _pad, u32 _deviceId, u32 _deviceOutputId, InputAdapter::PadType _padType)
    {
        if (_deviceId == AK_INVALID_DEVICE_ID)
        {
            AUDIO_WARNING("Invalid Register Controller Speaker pad %d with device id %d", _pad, _deviceId);
            return bfalse;
        }
        u32 padHandle = _deviceId;
#ifdef ITF_WINDOWS
        bbool isDualSense = _padType == InputAdapter::Pad_PS5;
        if (isDualSense)
        {
            padHandle = _deviceId | AKMOTION_SCEPAD_HAPTICS_MODE;
        }

#endif
        AkOutputSettings montionSetting(CONTROLLER_MONTION, padHandle);
        auto res = AK::SoundEngine::AddOutput(montionSetting, &m_PlayerMotionOutput[_pad]);
        if (res != AK_Success)
        {
            AUDIO_WARNING("Register Controller Speaker error %d with pad %d", res, _pad);
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
        if (m_PlayerMotionOutput[_pad] != AK_INVALID_DEVICE_ID)
        {
            auto res = AK::SoundEngine::RemoveOutput(m_PlayerMotionOutput[_pad]);
            m_PlayerMotionOutput[_pad] = AK_INVALID_DEVICE_ID;
            if (res != AK_Success)
            {
                AUDIO_WARNING("Unregister Controller Speaker error %d with pad %d", res, _pad);
                return bfalse;
            }
            else
            {
                return btrue;
            }
        }
        else
        {
            AUDIO_WARNING("Register Controller Speaker error. Cant find device output %d", _pad);
            return bfalse;
        }
    }

    bbool Adapter_WWISE::registerControllerSpeaker(u32 _pad, u32 _deviceId, u32 _deviceOutputId, InputAdapter::PadType _padType)
    {
        if (_deviceOutputId == AK_INVALID_OUTPUT_DEVICE_ID)
        {
            return false;
        }
        AkOutputSettings padSpkOutputSettings;
        padSpkOutputSettings.audioDeviceShareset = CONTROLLER_SPEAKER_SHARESET;
        padSpkOutputSettings.idDevice = _deviceOutputId; //AK::GetDeviceID();
        padSpkOutputSettings.channelConfig.SetStandard(AK_SPEAKER_SETUP_STEREO);
#ifdef ITF_WINDOWS
         bbool isSonyController = _padType == InputAdapter::Pad_PS4 || _padType == InputAdapter::Pad_PS5;
        if (isSonyController)
        {
            scePadSetAudioOutPath(static_cast<int32_t>(_deviceId), SCE_PAD_AUDIO_OUT_PATH_SPEAKER);
            ScePadVolumeGain padGain = {127, 127, 0, 127};
            scePadSetVolumeGain(static_cast<int32_t>(_deviceId), &padGain);
        }

#endif
        AKRESULT res = AK::SoundEngine::AddOutput(padSpkOutputSettings, &m_PlayerAudioOutput[_pad]);
        if (res != AK_Success)
        {
            AUDIO_WARNING("Register Controller Speaker error %d with pad %d", res, _pad);
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
                AUDIO_WARNING("Unregister Controller Speaker error %d with pad %d", res, _pad);
                return bfalse;
            }
            else
            {
                return btrue;
            }
        }
        else
        {
            AUDIO_WARNING("Register Controller Speaker error. Cant find device output %d", _pad);
            return bfalse;
        }
    }
    void Adapter_WWISE::setMotionVolume( Volume _volume)
    {
        if (isRunning() == false)
            return;

        StringID guidRTPCVol = m_guidRtpc.m_MenuMotionGUID;
        if (guidRTPCVol.isValid())
        {
            SoundRtpcID	rtpcVolumeID = AUDIO_ADAPTER->getIDFromGUID(guidRTPCVol);
            if (rtpcVolumeID != ITF_INVALID_SOUND_RTPC_ID)
                AUDIO_ADAPTER->setRtpc(rtpcVolumeID, _volume.ratio(), ITF_INVALID_OBJREF);
            else
                AUDIO_ERROR("Sound : rtpcMotionVolume on Motion Master is not defined.");

        }
        else
            AUDIO_ERROR("Sound : rtpcMotionVolume on Motion Master is not defined.");

    }
#if defined(ITF_WINDOWS)
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
#elif defined(ITF_XBOX_SERIES)
    u32 Adapter_WWISE::getDeviceId(IGameInputDevice* _device)
    {
        if (_device) {
            return AK::SoundEngine::GetGameInputDeviceID(_device);
        }
        else  {
            return AK_INVALID_DEVICE_ID;
        }

    }
    u32 Adapter_WWISE::getDeviceIdFromName(String& _device)
    {
        wchar_t buffer[512];
        AudioSDK::safeStringCopy(buffer,_device.wcharCStr());
        return  AK::GetDeviceIDFromName(buffer);
    }
#endif
}
#endif
#endif
