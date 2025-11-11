#include "precompiled_InputAdapter_SDL3.h"
#ifdef ITF_USE_SDL
#ifndef _ITF_INPUTADAPTER_SDL3_H_
#include "InputAdapter_SDL3.h"
#endif

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef ITF_SYSTEMADAPTER_WIN_H_
#include "adapters/SystemAdapter_win/SystemAdapter_win.h"
#endif

namespace
{
    bool ContainsIgnoreCase(const char* haystack, const char* needle)
    {
        if (!haystack || !needle || *needle == '\0')
            return false;

        const size_t needleLen = SDL_strlen(needle);
        if (needleLen == 0)
            return false;

        for (const char* current = haystack; *current; ++current)
        {
            size_t matched = 0;
            while (matched < needleLen && current[matched] &&
                SDL_tolower(static_cast<unsigned char>(current[matched])) ==
                    SDL_tolower(static_cast<unsigned char>(needle[matched])))
            {
                ++matched;
            }

            if (matched == needleLen)
                return true;
        }

        return false;
    }

    const char* PadTypeToString(ITF::InputAdapter::PadType padType)
    {
        using PadType = ITF::InputAdapter::PadType;
        switch (padType)
        {
        case PadType::Pad_X360:              return "Pad_X360";
        case PadType::Pad_PS3:               return "Pad_PS3";
        case PadType::Pad_WiiSideWay:        return "Pad_WiiSideWay";
        case PadType::Pad_WiiNunchuk:        return "Pad_WiiNunchuk";
        case PadType::Pad_WiiClassic:        return "Pad_WiiClassic";
        case PadType::Pad_Vita:              return "Pad_Vita";
        case PadType::Pad_CTR:               return "Pad_CTR";
        case PadType::Pad_PS5:               return "Pad_PS5";
        case PadType::Pad_NX_Joycon:         return "Pad_NX_Joycon";
        case PadType::Pad_NX_Joycon_Dual:    return "Pad_NX_Joycon_Dual";
        case PadType::Pad_NX_Pro:            return "Pad_NX_Pro";
        case PadType::Pad_GenericXBox:       return "Pad_GenericXBox";
        case PadType::Pad_Other:             return "Pad_Other";
        case PadType::PadType_Count:        return "PadType_Count";
        case PadType::Pad_Invalid:           return "Pad_Invalid";
        default:                             return "Pad_Unknown";
        }
    }
}

namespace ITF
{
    const f32 SDLGamepad::INPUT_DEADZONE = 0.2f;

    SDLGamepad::SDLGamepad()
        : m_gamepad(nullptr)
          , m_joystickId(0)
          , m_id(-1)
          , m_connected(false)
    {
        memset(m_axes, 0, sizeof(m_axes));
        memset(m_buttons, 0, sizeof(m_buttons));

        m_vibrationState.duration = 0.0;
        m_vibrationState.startTime = 0.0;
        m_vibrationState.lastEndTime = 0.0;
        m_vibrationState.active = false;
    }

    SDLGamepad::~SDLGamepad()
    {
        cleanup();
    }

    bool SDLGamepad::initialize(u32 playerIndex)
    {
        m_id = playerIndex;

        int numJoysticks;
        SDL_JoystickID* joysticks = SDL_GetJoysticks(&numJoysticks);

        if (joysticks && playerIndex < static_cast<u32>(numJoysticks))
        {
            SDL_JoystickID joystickId = joysticks[playerIndex];

            if (SDL_IsGamepad(joystickId))
            {
                m_gamepad = SDL_OpenGamepad(joystickId);
                if (m_gamepad)
                {
                    m_joystickId = joystickId;
                    m_connected = true;
                    SDL_free(joysticks);
                    return true;
                }
            }
        }

        if (joysticks)
            SDL_free(joysticks);

        return false;
    }

    void SDLGamepad::updateInputState()
    {
        if (!m_connected || !m_gamepad)
            return;

        UpdateButtonState(JOY_BUT_A, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_SOUTH));
        UpdateButtonState(JOY_BUT_B, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_EAST));
        UpdateButtonState(JOY_BUT_X, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_WEST));
        UpdateButtonState(JOY_BUT_Y, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_NORTH));

        UpdateButtonState(JOY_BUT_LB, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER));
        UpdateButtonState(JOY_BUT_RB, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER));

        UpdateButtonState(JOY_BUT_BACK, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_BACK));
        UpdateButtonState(JOY_BUT_START, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_START));

        UpdateButtonState(JOY_BUT_LS, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK));
        UpdateButtonState(JOY_BUT_RS, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK));

        UpdateButtonState(JOY_BUT_DPAD_UP, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP));
        UpdateButtonState(JOY_BUT_DPAD_DOWN, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN));
        UpdateButtonState(JOY_BUT_DPAD_LEFT, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT));
        UpdateButtonState(JOY_BUT_DPAD_RIGHT, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT));

        // Update axes
        updateAxisState(
            JOY_AXIS_LX, static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTX)) / 32767.0f);
        updateAxisState(
            JOY_AXIS_LY,
            static_cast<f32>(-SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTY)) / 32767.0f); // Invert Y
        updateAxisState(
            JOY_AXIS_RX, static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTX)) / 32767.0f);
        updateAxisState(
            JOY_AXIS_RY,
            static_cast<f32>(-SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTY)) / 32767.0f); // Invert Y

        // Triggers (0.0 to 1.0)
        updateAxisState(
            JOY_AXIS_LT, static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)) / 32767.0f);
        updateAxisState(
            JOY_AXIS_RT, static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)) / 32767.0f);

        // Update vibration
        if (m_vibrationState.active)
        {
            f64 currentTime = SYSTEM_ADAPTER->getTime();
            if (m_vibrationState.duration > 0.0 &&
                (currentTime - m_vibrationState.startTime) >= m_vibrationState.duration)
            {
                // Stop vibration
                SDL_RumbleGamepad(m_gamepad, 0, 0, 0);
                m_vibrationState.active = false;
                m_vibrationState.lastEndTime = currentTime;
            }
        }
    }

    void SDLGamepad::UpdateButtonState(u32 buttonIndex, bool pressed)
    {
        if (buttonIndex >= JOY_MAX_BUT)
            return;

        InputAdapter::PressStatus previousStatus = m_buttons[buttonIndex];

        if (pressed)
        {
            if (previousStatus == InputAdapter::Released)
            {
                m_buttons[buttonIndex] = InputAdapter::JustPressed;
            }
            else
            {
                m_buttons[buttonIndex] = InputAdapter::Pressed;
            }
        }
        else
        {
            if (previousStatus == InputAdapter::Pressed ||
                previousStatus == InputAdapter::JustPressed)
            {
                m_buttons[buttonIndex] = InputAdapter::JustReleased;
            }
            else
            {
                m_buttons[buttonIndex] = InputAdapter::Released;
            }
        }
    }

    void SDLGamepad::updateAxisState(u32 axisIndex, f32 value)
    {
        if (axisIndex >= JOY_MAX_AXES)
            return;
        if (fabs(value) < INPUT_DEADZONE)
            value = 0.0f;
        m_axes[axisIndex] = value;
    }

    void SDLGamepad::cleanup()
    {
        if (m_gamepad)
        {
            SDL_CloseGamepad(m_gamepad);
            m_gamepad = nullptr;
        }
        m_connected = false;
        m_joystickId = 0;
    }

    float SDLGamepad::getAxis(u32 _axis) const
    {
        if (_axis >= JOY_MAX_AXES)
            return 0.0f;
        return m_axes[_axis];
    }

    InputAdapter::PressStatus SDLGamepad::getButton(u32 _button) const
    {
        if (_button >= JOY_MAX_BUT)
            return InputAdapter::Released;
        return m_buttons[_button];
    }

    void SDLGamepad::setVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _duration)
    {
        if (!m_connected || !m_gamepad)
            return;

        Uint16 leftMotor = static_cast<Uint16>(_leftMotorSpeed * 65535.0f);
        Uint16 rightMotor = static_cast<Uint16>(_rightMotorSpeed * 65535.0f);
        Uint32 durationMs = (_duration > 0.0) ? static_cast<Uint32>(_duration * 1000.0) : 0;

        if (SDL_RumbleGamepad(m_gamepad, leftMotor, rightMotor, durationMs) == 0)
        {
            m_vibrationState.active = true;
            m_vibrationState.duration = _duration;
            m_vibrationState.startTime = SYSTEM_ADAPTER->getTime();
        }
    }

    bool SDLGamepad::isConnected() const
    {
        return m_connected && m_gamepad;
    }

    SDLInput::SDLInput()
        : m_gamepadCount(0)
          , m_initialized(false)
          , m_adapter(nullptr)
    {
    }

    SDLInput::~SDLInput()
    {
        Cleanup();
    }

    bool SDLInput::initialize(InputAdapter* adapter)
    {
        if (m_initialized)
            return true;
        m_adapter = adapter;
        if (!SDL_Init(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC))
        {
            LOG("[SDL] - could not initialize! Error: %s\n", SDL_GetError());
        }
        else
        {
            LOG("[SDL] - initialized successfully!\n");
        }
        refreshGamepadList();
        m_initialized = true;
        return true;
    }

    void SDLInput::Cleanup()
    {
        if (!m_initialized)
            return;

        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            m_gamepads[i].cleanup();
        }

        SDL_Quit();
        m_initialized = false;
    }

    void SDLInput::UpdateInputState()
    {
        if (!m_initialized)
            return;

        SDL_Event event;
        u32 eventCount = 0;
        const u32 MAX_EVENTS_PER_FRAME = 100;

        while (SDL_PollEvent(&event) && eventCount < MAX_EVENTS_PER_FRAME)
        {
            eventCount++;

            try
            {
                switch (event.type)
                {
                case SDL_EVENT_GAMEPAD_ADDED:
                    handleGamepadConnected(event.gdevice.which);
                    break;
                case SDL_EVENT_GAMEPAD_REMOVED:
                    HandleGamepadDisconnected(event.gdevice.which);
                    break;
                default:
                    break;
                }
            }
            catch (...)
            {
                // Silently ignore SDL event errors to prevent crash during hot-plug
                LOG("[SDL] Warning: Exception during gamepad event handling");
            }
        }

        for (auto& m_gamepad : m_gamepads)
        {
            try
            {
                m_gamepad.updateInputState();
            }
            catch (...)
            {
                // Cleanup on exception to prevent invalid state
                LOG("[SDL] Warning: Exception during gamepad update, cleaning up");
                m_gamepad.cleanup();
            }
        }
    }

    void SDLInput::setGamepadConnected(u32 index, bool connected, InputAdapter::PadType padType)
    {
        m_adapter->setPadConnected(index, connected);
        m_adapter->setPadType(index, padType);

        if (connected)
        {
            m_adapter->OnControllerConnected(index);
            m_gamepadCount++;
        }
        else
        {
            m_adapter->OnControllerDisconnected(index);
            m_gamepadCount = (m_gamepadCount > 0) ? m_gamepadCount - 1 : 0;
        }
    }

    InputAdapter::PadType SDLInput::detectPadType(const SDLGamepad& pad) const
    {
        SDL_Gamepad* handle = const_cast<SDL_Gamepad*>(pad.getGamepad());
        if (!handle)
            return InputAdapter::Pad_Other;

        const SDL_GamepadType type = SDL_GetGamepadType(handle);
        switch (type)
        {
        case SDL_GAMEPAD_TYPE_XBOX360:
            return InputAdapter::Pad_X360;
        case SDL_GAMEPAD_TYPE_XBOXONE:
        case SDL_GAMEPAD_TYPE_STANDARD:
            return InputAdapter::Pad_GenericXBox;
        case SDL_GAMEPAD_TYPE_PS5:
        case SDL_GAMEPAD_TYPE_PS4:
            return InputAdapter::Pad_PS5;
        case SDL_GAMEPAD_TYPE_PS3:
            return InputAdapter::Pad_PS3;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
            return InputAdapter::Pad_NX_Pro;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
            return InputAdapter::Pad_NX_Joycon;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
            return InputAdapter::Pad_NX_Joycon_Dual;
        default:
            break;
        }

        const char* controllerName = SDL_GetGamepadName(handle);
        if (ContainsIgnoreCase(controllerName, "JOYCON") || ContainsIgnoreCase(controllerName, "SWITCH") ||
            ContainsIgnoreCase(controllerName, "NX"))
        {
            return InputAdapter::Pad_NX_Pro;
        }
        if (ContainsIgnoreCase(controllerName, "PS5") || ContainsIgnoreCase(controllerName, "DUALSENSE"))
        {
            return InputAdapter::Pad_PS5;
        }
        if (ContainsIgnoreCase(controllerName, "PS4") || ContainsIgnoreCase(controllerName, "DUALSHOCK") ||
            ContainsIgnoreCase(controllerName, "PLAYSTATION"))
        {
            return InputAdapter::Pad_PS3;
        }
        if (ContainsIgnoreCase(controllerName, "XBOX") || ContainsIgnoreCase(controllerName, "MICROSOFT") ||
            ContainsIgnoreCase(controllerName, "XINPUT"))
        {
            return InputAdapter::Pad_GenericXBox;
        }
        if (ContainsIgnoreCase(controllerName, "OUNCE"))
        {
            return InputAdapter::Pad_Other;
        }

        return InputAdapter::Pad_Other;
    }

    void SDLInput::refreshGamepadList()
    {
        for (auto& m_gamepad : m_gamepads)
        {
            m_gamepad.cleanup();
        }

        m_gamepadCount = 0;

        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            if (m_gamepads[i].initialize(i))
            {
                const InputAdapter::PadType padType = detectPadType(m_gamepads[i]);
                setGamepadConnected(i, true, padType);
                const SDL_Gamepad* handle = m_gamepads[i].getGamepad();
                const char* padName = handle ? SDL_GetGamepadName(const_cast<SDL_Gamepad*>(handle)) : "Unknown";
                LOG("[SDL]   Slot %u connected during refresh (type=%s, name=%s)", i, PadTypeToString(padType),
                    padName ? padName : "Unknown");
            }
            else
            {
                m_adapter->setPadConnected(i, false);
                m_adapter->setPadType(i, InputAdapter::Pad_Invalid);
            }
        }
        LOG("[SDL] - Refreshed gamepad list, found %u gamepads", m_gamepadCount);
    }

    void SDLInput::handleGamepadConnected(SDL_JoystickID instanceId)
    {
        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            if (!m_gamepads[i].isConnected())
            {
                if (m_gamepads[i].initialize(i))
                {
                    const InputAdapter::PadType padType = detectPadType(m_gamepads[i]);
                    setGamepadConnected(i, true, padType);
                    const SDL_Gamepad* handle = m_gamepads[i].getGamepad();
                    const char* padName = handle ? SDL_GetGamepadName(const_cast<SDL_Gamepad*>(handle)) : "Unknown";
                    LOG("[SDL] - Gamepad connected (slot=%u, id=%d, type=%s, name=%s)", i,
                        static_cast<int>(m_gamepads[i].getJoystickId()), PadTypeToString(padType),
                        padName ? padName : "Unknown");
                    break;
                }
            }
        }
    }

    void SDLInput::HandleGamepadDisconnected(SDL_JoystickID instanceId)
    {
        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            if (m_gamepads[i].getJoystickId() == instanceId && m_gamepads[i].isConnected())
            {
                const SDL_Gamepad* handle = m_gamepads[i].getGamepad();
                const char* padName = handle ? SDL_GetGamepadName(const_cast<SDL_Gamepad*>(handle)) : "Unknown";
                const InputAdapter::PadType padType = m_adapter ? m_adapter->getPadType(i) : InputAdapter::Pad_Invalid;
                LOG("[SDL] - Gamepad disconnected (slot=%u, id=%d, type=%s, name=%s)", i,
                    static_cast<int>(instanceId), PadTypeToString(padType), padName ? padName : "Unknown");
                m_gamepads[i].cleanup();
                setGamepadConnected(i, false, InputAdapter::Pad_Invalid);
                break;
            }
        }
    }

    InputAdapter_SDL3::InputAdapter_SDL3()
    {
        auto adapter = static_cast<SystemAdapter_win*>(SYSTEM_ADAPTER);
        adapter->SetCallbackKeyboard(KeyCB);
        adapter->SetCallbackMousePos(MousePosCB);
        adapter->SetCallbackMouseWheel(MouseWheelCB);
        adapter->SetCallbackMouseButton(MouseButtonCB);
        m_sdlInput.initialize(this);
        setPadConnected(0, btrue);

        InitializeActionStrings();
        memset(m_keyStatus, 0, KEY_COUNT * sizeof(PressStatus));
        memset(m_keyPressTime, 0, KEY_COUNT * sizeof(u32));
        memset(m_connectedPlayers, 0, JOY_MAX_COUNT * sizeof(PlayerState));
        m_connectedPlayers[0] = ePlaying;
        InputAdapter::LoadPlayerControlSettings();
    }

    InputAdapter_SDL3::~InputAdapter_SDL3()
    {
    }

    void InputAdapter_SDL3::getGamePadPosStatus(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const
    {
        if ((m_environmentInput & _environment) != 0 && _pad < JOY_MAX_COUNT)
        {
            const SDLGamepad& gamepad = m_sdlInput.m_gamepads[_pad];
            if (gamepad.isConnected())
            {
                for (u32 i = 0; i < _numAxes && i < JOY_MAX_AXES; i++)
                    _pos[i] = gamepad.getAxis(i);
            }
            else
            {
                for (u32 i = 0; i < _numAxes; i++)
                    _pos[i] = 0.0f;
            }
        }
        else
        {
            for (u32 i = 0; i < _numAxes; i++)
                _pos[i] = 0.0f;
        }
    }

    void InputAdapter_SDL3::getGamePadButtonsStatus(u32 _environment, u32 _pad, PressStatus* _buttons,
                                                    u32 _numButtons) const
    {
        ITF_ASSERT(_numButtons <= JOY_MAX_BUT);

        if ((m_environmentInput & _environment) != 0 && _pad < JOY_MAX_COUNT)
        {
            const SDLGamepad& gamepad = m_sdlInput.m_gamepads[_pad];
            if (gamepad.isConnected())
            {
                for (u32 i = 0; i < _numButtons && i < JOY_MAX_BUT; i++)
                    _buttons[i] = gamepad.getButton(i);
            }
            else
            {
                for (u32 i = 0; i < _numButtons; i++)
                    _buttons[i] = Released;
            }
        }
        else
        {
            for (u32 i = 0; i < _numButtons; i++)
                _buttons[i] = Released;
        }
    }

    u32 InputAdapter_SDL3::getGamePadCount()
    {
        u32 count = 0;
        for (u32 i = 0; i < JOY_MAX_COUNT; i++)
        {
            if (m_sdlInput.m_gamepads[i].isConnected())
                count++;
        }
        return count;
    }

    void InputAdapter_SDL3::UpdatePads()
    {
        m_sdlInput.UpdateInputState();
        u32 connectedCount = 0;
        for (u32 i = 0; i < JOY_MAX_COUNT && connectedCount < m_sdlInput.m_gamepadCount; ++i)
        {
            if (m_sdlInput.m_gamepads[i].isConnected())
            {
                connectedCount++;
                const SDLGamepad& gamepad = m_sdlInput.m_gamepads[i];
                for (u32 axis = 0; axis < JOY_MAX_AXES; ++axis)
                {
                    m_axes[i][axis] = gamepad.getAxis(axis);
                }
                for (u32 button = 0; button < JOY_MAX_BUT; ++button)
                {
                    m_buttons[i][button] = gamepad.getButton(button);
                }

                if (m_connectedPlayers[i] == eNotConnected)
                {
                    m_connectedPlayers[i] = ePlaying;
                }
            }
            else if (m_connectedPlayers[i] != eNotConnected)
            {
                // Don't disconnect player 0 - keep available for keyboard input on PC
                if (i != 0)
                    m_connectedPlayers[i] = eNotConnected;
                else
                    m_connectedPlayers[0] = ePlaying; // Keep player 0 for keyboard

                for (u32 axis = 0; axis < JOY_MAX_AXES; ++axis)
                {
                    m_axes[i][axis] = 0.0f;
                }
                for (u32 button = 0; button < JOY_MAX_BUT; ++button)
                {
                    m_buttons[i][button] = Released;
                }
            }
        }
    }

    InputAdapter::PressStatus InputAdapter_SDL3::GetButtonStatus(InputValue inputValue)
    {
        if (inputValue.inputIndex >= JOY_MAX_COUNT)
            return Released;
        return m_sdlInput.m_gamepads[inputValue.inputIndex].getButton(inputValue.inputValue);
    }

    float InputAdapter_SDL3::GetAxe(InputValue inputValue)
    {
        if (inputValue.inputIndex >= JOY_MAX_COUNT)
            return 0.0f;
        return m_sdlInput.m_gamepads[inputValue.inputIndex].getAxis(inputValue.inputValue);
    }

    bbool InputAdapter_SDL3::IsButtonPressed(InputValue inputValue)
    {
        if (inputValue.inputIndex >= JOY_MAX_COUNT)
            return bfalse;
        auto state = m_sdlInput.m_gamepads[inputValue.inputIndex].getButton(inputValue.inputValue);
        return state == JustPressed || state == Pressed;
    }

    void InputAdapter_SDL3::padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        if (_numPad < JOY_MAX_COUNT)
        {
            m_sdlInput.m_gamepads[_numPad].setVibration(_leftMotorSpeed, _rightMotorSpeed, 0.0);
        }
    }

    void InputAdapter_SDL3::startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        if (_numPad >= JOY_MAX_COUNT)
            return;
        m_sdlInput.m_gamepads[_numPad].setVibration(_leftMotorSpeed, _rightMotorSpeed, _time);
    }

    void InputAdapter_SDL3::stopRumble(u32 _numPad)
    {
        if (_numPad >= JOY_MAX_COUNT)
            return;
        m_sdlInput.m_gamepads[_numPad].setVibration(0.0f, 0.0f, 0.0);
    }

    const char* InputAdapter_SDL3::GetControllerTypeName(u32 padIndex) const
    {
        if (padIndex >= JOY_MAX_COUNT) return "Unknown";

        if (m_sdlInput.m_gamepads[padIndex].isConnected())
        {
            const SDL_Gamepad* gamepad = m_sdlInput.m_gamepads[padIndex].getGamepad();
            if (gamepad)
            {
                const char* name = SDL_GetGamepadName(const_cast<SDL_Gamepad*>(gamepad));
                if (name && strlen(name) > 0)
                    return name;
                else
                    return "SDL Gamepad";
            }
        }
        return "Disconnected";
    }

    ControllerType InputAdapter_SDL3::GetControllerType(InputValue& value)
    {
        return value.inputType;
    }

}
#endif
