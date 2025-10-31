#include "precompiled_InputAdapter_xboxseries.h"

#include "adapters/InputAdapter_xboxseries/InputPad_xboxseries.h"

#include "core/math/MathTools.h"

namespace ITF
{
    namespace 
    {
        constexpr f32 GAMEINPUT_AXIS_DEADZONE = 0.24f; // 24% of the axis range taken from XInput historical dead zone, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, so 7864 / 32767
        constexpr f32 GAMEINPUT_TRIGGER_DEADZONE = 0.118f; // 11.8% of the trigger range, taken from XInput historical dead zone, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, so 30 / 255
        constexpr f32 GAMEINPUT_TRIGGER_BUTTON_THRESHOLD = 0.5f; // Threshold for trigger press detection button emulation, 50% of the trigger range
        constexpr f64 GAMEINPUT_MAX_VIBRATION_TIME = 10.0; // 10 seconds max
    }

    InputPad_GameInput::InputPad_GameInput(IGameInputDevice& _gameInputDevice)
        : m_gameInputDevice(&_gameInputDevice)
    {
        m_axes.fill(0.0f);
        m_buttons.fill(InputAdapter::PressStatus::Released);
    }

    InputPad_GameInput::InputPad_GameInput(InputPad_GameInput&& _other)
        : m_gameInputDevice(_other.m_gameInputDevice)
        , m_axes(std::move(_other.m_axes))
        , m_buttons(std::move(_other.m_buttons))
        , m_vibrationEndTime(_other.m_vibrationEndTime)
    {
        _other.m_gameInputDevice = nullptr;
        _other.m_axes.fill(0.0f);
        _other.m_buttons.fill(InputAdapter::Released);
        _other.m_vibrationEndTime = 0.0;
    }

    InputPad_GameInput::~InputPad_GameInput()
    {
        m_axes.fill(0.0f);
        m_buttons.fill(InputAdapter::Released);

        m_vibrationEndTime = 0.0;

        if (m_gameInputDevice != nullptr)
        {
            stopVibration();
            m_gameInputDevice->Release();
            m_gameInputDevice = nullptr;
        }
    }

    InputPad_GameInput& InputPad_GameInput::operator=(InputPad_GameInput&& _other)
    {
        if (this != &_other)
        {
            // Release current resources
            if (m_gameInputDevice != nullptr)
            {
                stopVibration();
                m_gameInputDevice->Release();
                m_gameInputDevice = nullptr;
            }

            // Move resources from _other
            m_gameInputDevice = _other.m_gameInputDevice;
            m_axes = std::move(_other.m_axes);
            m_buttons = std::move(_other.m_buttons);
            m_vibrationEndTime = _other.m_vibrationEndTime;

            // Reset _other
            _other.m_gameInputDevice = nullptr;
            _other.m_axes.fill(0.0f);
            _other.m_buttons.fill(InputAdapter::Released);
            _other.m_vibrationEndTime = 0.0;
        }

        return *this;
    }

    void InputPad_GameInput::setAxis(u32 _axisId, f32 _axisVal)
    {
        ITF_ASSERT_CRASH(_axisId < m_axes.size(), "[InputPad_Scarlett::setAxis] invalid axis id");
        m_axes[_axisId] = _axisVal;
    }

    void InputPad_GameInput::updateVibration()
    {
        if (isVibrating() && SYSTEM_ADAPTER->getTime() > m_vibrationEndTime)
        {
            stopVibration();
        }
    }

    // The speed given is between 0.f - 1.f
    // For XBox pads, the left motor handles low-frequency vibration and the right motor handles high-frequency vibration.
    void InputPad_GameInput::setVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _duration)
    {
        _leftMotorSpeed = Clamp<f32>(_leftMotorSpeed, 0.f, 1.f);
        _rightMotorSpeed = Clamp<f32>(_rightMotorSpeed, 0.f, 1.f);

        ITF_ASSERT(_duration >= 0.0);
        ITF_ASSERT_MSG(_duration <= GAMEINPUT_MAX_VIBRATION_TIME, "Requiring pad vibration for a too long time. Clamped to 10s.");

        m_vibrationEndTime = SYSTEM_ADAPTER->getTime() + Clamp(_duration, 0.0, 10.0);

        GameInputRumbleParams vibration = {};
        vibration.lowFrequency = _leftMotorSpeed;
        vibration.highFrequency = _rightMotorSpeed;

        // Missing handling of the trigger motors, leftTrigger and rightTrigger.
        // Consider for a future added feature.

        m_gameInputDevice->SetRumbleState(&vibration);
    }

    void InputPad_GameInput::getGamePadPos(float* _pos, u32 _nbAxis) const
    {
        ITF_ASSERT(_nbAxis < m_axes.size());

        // Limit the number of axis to the max supported, set the rest to 0
        u32 nbEffectiveAxis = _nbAxis;
        if (nbEffectiveAxis > u32(m_axes.size()))
            nbEffectiveAxis = u32(m_axes.size());

        std::copy(m_axes.begin(), m_axes.begin() + nbEffectiveAxis, _pos);
        std::fill(_pos + nbEffectiveAxis, _pos + _nbAxis, 0.0f);
    }

    InputAdapter::PressStatus InputPad_GameInput::getGamePadButton(u32 _buttonID) const
    {
        ITF_ASSERT(_buttonID <= m_buttons.size());

        if (_buttonID < m_buttons.size())
        {
            return m_buttons[_buttonID];
        }

        return InputAdapter::Released;
    }

    void InputPad_GameInput::getGamePadButtons(InputAdapter::PressStatus* _buttons, u32 _nbButtons) const
    {
        ITF_ASSERT(_nbButtons <= m_buttons.size());

        // Limit the number of buttons to the max supported, set the rest to PressStatus::Released
        u32 nbEffectiveButtons = _nbButtons;
        if (nbEffectiveButtons > u32(m_buttons.size()))
            nbEffectiveButtons = u32(m_buttons.size());

        std::copy(m_buttons.begin(), m_buttons.begin() + nbEffectiveButtons, _buttons);
        std::fill(_buttons + nbEffectiveButtons, _buttons + _nbButtons, InputAdapter::Released);
    }

    InputAdapter::PadType InputPad_GameInput::getType() const
    {
        return InputAdapter::PadType::Pad_GenericXBox;
    }

    const char* InputPad_GameInput::getTypeName() const
    {
        return "Pad_GenericXBox";
    }

    void InputPad_GameInput::update(const GameInputGamepadState& state)
    {
        updateControls(state);
        updateVibration();
    }

    void InputPad_GameInput::updateButton(u32 _buttonId, bool _isPressed)
    {
        InputAdapter::PressStatus& buttonStatus = m_buttons[_buttonId];

        if (_isPressed)
        {
            if (buttonStatus == InputAdapter::JustPressed || buttonStatus == InputAdapter::Pressed)
            {
                buttonStatus = InputAdapter::Pressed;
            }
            else
            {
                buttonStatus = InputAdapter::JustPressed;
            }
        }
        else
        {
            if (buttonStatus == InputAdapter::JustReleased || buttonStatus == InputAdapter::Released)
            {
                buttonStatus = InputAdapter::Released;
            }
            else
            {
                buttonStatus = InputAdapter::JustReleased;
            }
        }
    }

    void InputPad_GameInput::updateAxis(u32 _axisId, f32 _newValue)
    {
        auto axis = m_axes[_axisId];
        if (_newValue > -GAMEINPUT_AXIS_DEADZONE && _newValue < GAMEINPUT_AXIS_DEADZONE)
            axis = 0.0f;
        else
        {
            axis = _newValue;
        }
    }

    void InputPad_GameInput::updateTriggerAxis(u32 _axisId, f32 _newValue)
    {
        auto axis = m_axes[_axisId];
        if (_newValue < GAMEINPUT_TRIGGER_DEADZONE)
            axis = 0.0f;
        else
            axis = _newValue;
    }

    void InputPad_GameInput::updateControls(const GameInputGamepadState& _state)
    {
        auto isButtonPressed = [&](GameInputGamepadButtons _button)
        {
            return (_state.buttons & _button) != 0;
        };

        updateButton(m_joyButton_A, isButtonPressed(GameInputGamepadA));
        updateButton(m_joyButton_B, isButtonPressed(GameInputGamepadB));
        updateButton(m_joyButton_X, isButtonPressed(GameInputGamepadX));
        updateButton(m_joyButton_Y, isButtonPressed(GameInputGamepadY));
        updateButton(m_joyButton_LB, isButtonPressed(GameInputGamepadLeftShoulder));
        updateButton(m_joyButton_RB, isButtonPressed(GameInputGamepadRightShoulder));
        updateButton(m_joyButton_Start, isButtonPressed(GameInputGamepadMenu));
        updateButton(m_joyButton_Back, isButtonPressed(GameInputGamepadView));
        updateButton(m_joyButton_DPadL, isButtonPressed(GameInputGamepadDPadLeft));
        updateButton(m_joyButton_DPadR, isButtonPressed(GameInputGamepadDPadRight));
        updateButton(m_joyButton_DPadU, isButtonPressed(GameInputGamepadDPadUp));
        updateButton(m_joyButton_DPadD, isButtonPressed(GameInputGamepadDPadDown));
        updateButton(m_joyButton_ThumbLeft, isButtonPressed(GameInputGamepadLeftThumbstick));
        updateButton(m_joyButton_ThumbRight, isButtonPressed(GameInputGamepadRightThumbstick));
        updateButton(m_joyButton_TriggerLeft, _state.leftTrigger >= GAMEINPUT_TRIGGER_BUTTON_THRESHOLD);
        updateButton(m_joyButton_TriggerRight, _state.rightTrigger >= GAMEINPUT_TRIGGER_BUTTON_THRESHOLD);

        updateAxis(m_joyStickLeft_X, _state.leftThumbstickX);
        updateAxis(m_joyStickLeft_Y, _state.leftThumbstickY);
        updateAxis(m_joyStickRight_X, _state.rightThumbstickX);
        updateAxis(m_joyStickRight_Y, _state.rightThumbstickY);
        updateTriggerAxis(m_joyTrigger_Left, _state.leftTrigger);
        updateTriggerAxis(m_joyTrigger_Right, _state.rightTrigger);
    }

} // namespace ITF
