#pragma once

#include "engine/AdaptersInterfaces/InputAdapter.h"

#include <GameInput.h>
#include <array>

namespace ITF
{
    class InputPad_GameInput
    {
    public:

        explicit InputPad_GameInput(IGameInputDevice& _gameInputDevice);
        InputPad_GameInput(const InputPad_GameInput& _gameInputDevice) = delete;
        InputPad_GameInput(InputPad_GameInput&& _gameInputDevice);
        ~InputPad_GameInput();

        InputPad_GameInput& operator=(const InputPad_GameInput& _gameInputDevice) = delete;
        InputPad_GameInput & operator=(InputPad_GameInput&& _gameInputDevice);

        void update(const GameInputGamepadState& state);

        IGameInputDevice* getDevice() const { return m_gameInputDevice; }

        InputAdapter::PadType getType() const;
        const char* getTypeName() const;

        void getGamePadPos(float* _pos, u32 _nbAxis) const;
        InputAdapter::PressStatus getGamePadButton(u32 _buttonID) const;
        void getGamePadButtons(InputAdapter::PressStatus* _buttons, u32 _nbButtons) const;
                
        void setVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _duration);
        void stopVibration() { setVibration(0.0f, 0.0f, 0.0); }

    private:
        void updateControls(const GameInputGamepadState& state); // update buttons and axes states
        void updateButton(u32 _buttonId, bool _isPressed);
        void updateAxis(u32 _axisId, f32 _newValue);
        void updateTriggerAxis(u32 _axisId, f32 _newValue);
        void updateVibration();

        void setAxis(u32 _axisId, f32 _axisVal);

        bool isVibrating() const { return (m_vibrationEndTime > 0.0); }

        f64 m_vibrationEndTime = 0.0;
        IGameInputDevice* m_gameInputDevice = nullptr;
        std::array<f32, JOY_MAX_AXES> m_axes{};
        std::array<InputAdapter::PressStatus, JOY_MAX_BUT> m_buttons{};
    };

} // namespace ITF
