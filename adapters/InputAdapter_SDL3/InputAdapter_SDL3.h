#ifndef _ITF_INPUTADAPTER_SDL3_H_
#define _ITF_INPUTADAPTER_SDL3_H_
#ifdef ITF_USE_SDL
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
        void setVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _duration = 0.0);
        bool isConnected() const;
        uint32_t deviceID;
        uint32_t deviceOutputID;

    private:
        void updateButtonState(u32 buttonIndex, bool pressed);
        void updateAxisState(u32 axisIndex, f32 value);

        SDL_Gamepad* m_gamepad;
        SDL_JoystickID m_joystickId;
        u32 m_id;
        bool m_connected;
        float m_axes[JOY_MAX_AXES];
        InputAdapter::PressStatus m_buttons[JOY_MAX_BUT];

        struct VibrationState
        {
            f64 duration;
            f64 startTime;
            f64 lastEndTime;
            bool active;
        } m_vibrationState;
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
        void notifyDeviceConnectEvent(u32 padIndex, InputAdapter::PadType type, bbool isConnected);
        void initSonyControllerDeviceIds(SDLGamepad& gamepad);
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
        void padVibration(u32 _numPad, f32 _rightMotorSpeed, f32 _leftMotorSpeed) override;
        void startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed) override;
        void stopRumble(u32 _numPad) override;
        const char* GetControllerTypeName(u32 padIndex) const override;
        void UpdatePads() override;
    private:
        SDLInput m_sdlInput;
        i32 m_slotGamepad[JOY_MAX_COUNT];
    };
}
#endif
#endif //_ITF_INPUTADAPTER_SDL3_H_
