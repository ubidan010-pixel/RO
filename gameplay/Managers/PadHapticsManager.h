#ifndef _ITF_HATPICMANAGER_H_
#define _ITF_HATPICMANAGER_H_
#ifdef USE_PAD_HAPTICS
#ifdef ITF_WINDOWS
#include "libScePadforPCGames/include/pad.h"
#endif
#include "engine/AdaptersInterfaces/InputAdapter.h"
#define  JOY_MAX_COUNT      4
namespace ITF
{
    class PadHapticsManager
    {
        static PadHapticsManager* s_instance;
    public:
        static void create() { s_instance = newAlloc(mId_Singleton, PadHapticsManager()); }
        static void destroy() { SF_DEL(s_instance); }
        static PadHapticsManager* get() { return s_instance; }
        void initialize();
        void update(float dt);
        void onSoundBankLoaded();
        void onControllerConnected(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,InputAdapter::PadType _padType);
        void onControllerDisconnected(uint32_t _pad);
        void enableHaptics(bbool enable);
        void enableControllerSpeaker(bbool enable);
    private:
        struct  DeviceInfo
        {
            u32  deviceID;
            u32  deviceOutputID;
            InputAdapter::PadType padType;
            bbool regHaptics;
            bbool regSpeaker;
        };
        ITF_MAP<u32,DeviceInfo> m_devices;
        PadHapticsManager();
        ~PadHapticsManager();
        bbool m_enableHaptics;
        bbool m_enableControllerSpeaker;
        bbool registerHaptics(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,InputAdapter::PadType _padType);
        bbool unregisterHaptics(uint32_t _pad);
        bbool registerControllerSpeaker(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,InputAdapter::PadType _padType);
        bbool unregisterControllerSpeaker(uint32_t _pad);

    };
#define HAPTICS_MANAGER PadHapticsManager::get()
}
#endif
#endif
