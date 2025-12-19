#include "precompiled_gameplay.h"
#ifdef USE_PAD_HAPTICS
#ifndef __AUDIOMIDDLEWAREADAPTER_H__
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#endif // __AUDIOMIDDLEWAREADAPTER_H__
#ifndef _ITF_HATPICMANAGER_H_
#include "PadHapticsManager.h"
#endif //_ITF_HATPICMANAGER_H_
namespace ITF
{
    PadHapticsManager::PadHapticsManager() : m_enableHaptics(btrue)
                                             , m_enableControllerSpeaker(btrue)
    {
    }

    PadHapticsManager::~PadHapticsManager()
    {
    }

    void PadHapticsManager::init()
    {
    }

    void PadHapticsManager::update(float dt)
    {
    }

    void PadHapticsManager::onControllerConnected(u32 _padIndex, u32 _deviceID, u32 _deviceOutputID,InputAdapter::PadType _padType)
    {
        m_devices[_padIndex] = {_deviceID, _deviceOutputID, _padType};
        if (m_enableHaptics)
        {
           m_devices[_padIndex].regHaptics = registerHaptics(_padIndex, _deviceID, _deviceOutputID, _padType);
        }
        if (m_enableControllerSpeaker)
        {
           m_devices[_padIndex].regSpeaker =  registerControllerSpeaker(_padIndex, _deviceID, _deviceOutputID, _padType);
        }
    }

    void PadHapticsManager::onControllerDisconnected(u32 pad)
    {
        unregisterHaptics(pad);
        unregisterControllerSpeaker(pad);
        m_devices.erase(pad);
    }

    void PadHapticsManager::enableHaptics(bbool enable)
    {
        m_enableHaptics = enable;
        for (auto& pair : m_devices)
        {
            const u32 pad = pair.first;
            DeviceInfo& info = pair.second;
            if (enable && !info.regHaptics)
            {
                bbool isSuccess = registerHaptics(pad, info.deviceID, info.deviceOutputID, info.padType);
                info.regHaptics = isSuccess;
            }
            else if (!enable && info.regHaptics)
            {
                bbool isSuccess = unregisterHaptics(pad);
                info.regHaptics = !isSuccess;
            }
        }
        int k = 0;
        // TODO, wait for new option, but for now it's alongs with haptics
        enableControllerSpeaker(m_enableHaptics);
    }

    void PadHapticsManager::enableControllerSpeaker(bbool enable)
    {
        m_enableControllerSpeaker = enable;
        for (auto& pair : m_devices)
        {
            const u32 pad = pair.first;
            DeviceInfo& info = pair.second;
            if (enable && !info.regSpeaker)
            {
                bbool isSuccess = registerControllerSpeaker(pad, info.deviceID, info.deviceOutputID, info.padType);
                info.regSpeaker = isSuccess;
            }
            else if (!enable && info.regSpeaker)
            {
                bbool isSuccess = unregisterControllerSpeaker(pad);
                info.regSpeaker = !isSuccess;
            }
        }
    }

    void PadHapticsManager::onSoundBankLoaded()
    {
    }

    bbool PadHapticsManager::registerHaptics(u32 _padIndex, u32 _deviceID, u32 _deviceOutputID, InputAdapter::PadType _padType)
    {
        return AUDIO_ADAPTER->registerHaptics(_padIndex, _deviceID, _deviceOutputID, _padType);
    }

    bbool PadHapticsManager::unregisterHaptics(uint32_t _pad)
    {
        return AUDIO_ADAPTER->unregisterHaptics(_pad);
    }

    bbool PadHapticsManager::registerControllerSpeaker(u32 _padIndex, u32 _deviceID, u32 _deviceOutputID, InputAdapter::PadType _padType)
    {
        return AUDIO_ADAPTER->registerControllerSpeaker(_padIndex, _deviceID, _deviceOutputID, _padType);
    }

    bbool PadHapticsManager::unregisterControllerSpeaker(uint32_t _pad)
    {
        return AUDIO_ADAPTER->unregisterControllerSpeaker(_pad);
    }

    PadHapticsManager* PadHapticsManager::s_instance = NULL;
}
#endif //USE_PAD_HAPTICS
