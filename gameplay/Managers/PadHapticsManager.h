#ifndef _ITF_HATPICMANAGER_H_
#define _ITF_HATPICMANAGER_H_
#ifdef USE_PAD_HAPTICS
#ifdef ITF_WINDOWS
#include "libScePadforPCGames/include/pad.h"
#endif
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
        void onControllerConnected(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,bool _isSony);
        void onControllerDisconnected(uint32_t _pad);
        void enableHaptics(bbool enable);
        void enableControllerSpeaker(bbool enable);
    private:
        struct  DeviceInfo
        {
            u32  deviceID;
            u32  deviceOutputID;
            bbool isSony;
            bbool regHaptics;
            bbool regSpeaker;
        };
        ITF_MAP<u32,DeviceInfo> m_devices;
        PadHapticsManager();
        ~PadHapticsManager();
        bbool m_enableHaptics;
        bbool m_enableControllerSpeaker;
        void registerHaptics(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,bool isSony);
        void unregisterHaptics(uint32_t _pad);
        void registerControllerSpeaker(u32 _padIndex,u32 _deviceID,u32 _deviceOutputID,bool isSony);
        void unregisterControllerSpeaker(uint32_t _pad);

    };
#define HAPTICS_MANAGER PadHapticsManager::get()
}
#endif
#endif
