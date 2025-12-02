#ifndef _ITF_INPUTADAPTER_SDL3_H_
#define _ITF_INPUTADAPTER_SDL3_H_
#ifndef _ITF_WININPUTADAPTER_H_
#include "engine/AdaptersInterfaces/WinInputAdapter.h"
#endif //_ITF_WININPUTADAPTER_H_
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>
#include <mmdeviceapi.h>

namespace ITF
{
    class SDLGamepad
    {
    public:
        SDLGamepad();
        ~SDLGamepad();

        ITF_INLINE u32 getId() { return m_id; }
        ITF_INLINE void setId(u32 _id) { m_id = _id; }
        ITF_INLINE SDL_JoystickID getJoystickId() const { return m_joystickId; }
        ITF_INLINE SDL_Gamepad* getGamepad() { return m_gamepad; }
        ITF_INLINE const SDL_Gamepad* getGamepad() const { return m_gamepad; }

        bool initialize(u32 playerIndex);
        void updateInputState();
        void cleanup();
        float getAxis(u32 _axis) const;
        InputAdapter::PressStatus getButton(u32 _button) const;
        bool isConnected() const;
        uint32_t m_deviceID;
        uint32_t m_deviceOutputID;
        InputAdapter::PadType m_padType;
        bbool isSonyController() { return m_padType == InputAdapter::Pad_PS4 || m_padType == InputAdapter::Pad_PS5; }

    private:
        void updateButtonState(u32 buttonIndex, bool pressed);
        void updateAxisState(u32 axisIndex, f32 value);

        SDL_Gamepad* m_gamepad;
        SDL_JoystickID m_joystickId;
        u32 m_id;
        bool m_connected;
        float m_axes[JOY_MAX_AXES];
        InputAdapter::PressStatus m_buttons[JOY_MAX_BUT];
    };

    class SDLInput
    {
    public:
        SDLInput();
        ~SDLInput();

        bool initialize(InputAdapter*);
        void cleanup();
        void updateInputState();
        void refreshGamepadList();

        SDLGamepad m_gamepads[JOY_MAX_COUNT];
        u32 m_gamepadCount;
        bool m_initialized;
        int m_scePadHandles[JOY_MAX_COUNT];

    private:
        void handleGamepadConnected(SDL_JoystickID instanceId);
        void handleGamepadDisconnected(SDL_JoystickID instanceId);
        void setGamepadConnected(u32 index, bool connected, InputAdapter::PadType padType);
        void notifyDeviceConnectEvent(u32 padIndex, InputAdapter::PadType padType, bbool isConnected);
        bbool checkExistDeviceId(u32 deviceId);
        void updateSonyControllerDeviceIds(u32 padIndex, SDLGamepad& gamepad);
        void initScePad(int* outScePadHandles);
        void getScePadDeviceId(int padHandle, uint32_t& outResolvedId);
        bool getMMDeviceFromPadHandle(wchar_t const* containerInfo, IMMDevice*& outMmDevice);
        InputAdapter* m_adapter;
    };

    class InputAdapter_SDL3 : public WinInputAdapter
    {
    public:
        InputAdapter_SDL3();
        ~InputAdapter_SDL3() override;
        u32 getGamePadCount() override;
        const char* GetControllerTypeName(u32 padIndex) const override;
        void UpdatePads() override;

    protected:
        void OnPCControlModeChanged(PCControlMode previous, PCControlMode current) override;

    private:
        SDLInput m_sdlInput;
        i32 m_slotGamepad[JOY_MAX_COUNT];
    };
}
#endif //_ITF_INPUTADAPTER_SDL3_H_
