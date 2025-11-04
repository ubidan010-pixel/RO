#ifndef _ITF_INPUTADAPTER_SDL3_H_
#define _ITF_INPUTADAPTER_SDL3_H_
#ifdef ITF_USE_SDL
#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>

namespace ITF
{
    class SDLGamepad
    {
    public:
        static const u32 MAX_PAD = JOY_MAX_COUNT;
        static const f32 INPUT_DEADZONE;

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

    private:
        void UpdateButtonState(u32 buttonIndex, bool pressed);
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
        void Cleanup();
        void UpdateInputState();
        void refreshGamepadList();

        SDLGamepad m_gamepads[JOY_MAX_COUNT];
        u32 m_gamepadCount;
        bool m_initialized;

    private:
        void handleGamepadConnected(SDL_JoystickID instanceId);
        void HandleGamepadDisconnected(SDL_JoystickID instanceId);
        void setGamepadConnected(u32 index, bool connected, InputAdapter::PadType padType);
        InputAdapter* m_adapter;
    };

    class InputAdapter_SDL3 : public InputAdapter
    {
    public:
        InputAdapter_SDL3();
        ~InputAdapter_SDL3() override;

        void getGamePadPosStatus(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const override;
        void getGamePadButtonsStatus(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const override;
        u32 getGamePadCount() override;
        void padVibration(u32 _numPad, f32 _rightMotorSpeed, f32 _leftMotorSpeed) override;
        void startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed) override;
        void stopRumble(u32 _numPad) override;
        const char* GetControllerTypeName(u32 padIndex) const override;
        PressStatus GetButtonStatus(InputValue) override;
        float GetAxe(InputValue) override;
        bbool IsButtonPressed(InputValue) override;
        bbool isX360Pad(u32 padIndex) override
        {
            if (padIndex < JOY_MAX_COUNT && m_sdlInput.m_gamepads[padIndex].isConnected())
            {
                if (SDL_Gamepad* gamepad = m_sdlInput.m_gamepads[padIndex].getGamepad())
                {
                    const char* name = SDL_GetGamepadName(gamepad);
                    return name && (strstr(name, "Xbox") != nullptr || strstr(name, "XInput") != nullptr);
                }
            }
            return false;
        }

        void UpdatePads() override;
        ControllerType GetControllerType(InputValue& value) override;
        
        void setFocus() override;
        void unsetFocus() override;
        
    private:
        SDLInput m_sdlInput;
    };
}
#endif
#endif //_ITF_INPUTADAPTER_SDL3_H_
