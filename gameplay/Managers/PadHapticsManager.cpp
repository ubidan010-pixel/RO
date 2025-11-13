
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
    ,m_enableControllerSpeaker(btrue)
    {
    }

    PadHapticsManager::~PadHapticsManager()
    {
    }

    void PadHapticsManager::initialize()
    {

    }

    void PadHapticsManager::update(float dt)
    {
    }

    void PadHapticsManager::onControllerConnected(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,bool _isSony)
    {
        m_devices[_padIndex] = {_deviceID,_deviceOutputID,_isSony};
        if (m_enableHaptics)
        {
            registerHaptics(_padIndex,_deviceID,_deviceOutputID,_isSony);
        }
        if (m_enableControllerSpeaker)
        {
            registerControllerSpeaker(_padIndex,_deviceID,_deviceOutputID,_isSony);
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
            for (const auto& pair : m_devices)
            {
                const u32 pad = pair.first;
                const DeviceInfo& info = pair.second;
                if (enable)
                {
                    registerHaptics(pad,info.deviceID,info.deviceOutputID,info.isSony);
                }
                else
                {
                    unregisterHaptics(pad);
                }
            }
    }

    void PadHapticsManager::enableControllerSpeaker(bbool enable)
    {
        for (const auto& pair : m_devices)
        {
            const u32 pad = pair.first;
            const DeviceInfo& info = pair.second;
            if (enable)
            {
                registerControllerSpeaker(pad,info.deviceID,info.deviceOutputID,info.isSony);
            }
            else
            {
                unregisterControllerSpeaker(pad);
            }
        }
    }

    void PadHapticsManager::onSoundBankLoaded()
    {

    }
    void PadHapticsManager::registerHaptics(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,bool isSony)
    {
        AUDIO_ADAPTER->registerHaptics(_padIndex,_deviceID,_deviceOutputID,isSony);
    }

    void PadHapticsManager::unregisterHaptics(uint32_t _pad)
    {
        AUDIO_ADAPTER->unregisterHaptics(_pad);
    }

    void PadHapticsManager::registerControllerSpeaker(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,bool isSony)
    {
        AUDIO_ADAPTER->registerControllerSpeaker(_padIndex,_deviceID,_deviceOutputID,isSony);
    }

    void PadHapticsManager::unregisterControllerSpeaker(uint32_t _pad)
    {
        AUDIO_ADAPTER->unregisterControllerSpeaker(_pad);
    }
    PadHapticsManager* PadHapticsManager::s_instance = NULL;
}
#endif //USE_PAD_HAPTICS
