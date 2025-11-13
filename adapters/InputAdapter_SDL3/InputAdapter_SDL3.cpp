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
#include <pad.h>
#include <pad_types.h>
#include <pad_windows_static.h>

#ifndef _ITF_INPUT_MANAGER_H_
#include "engine/input/InputManager.h"
#endif //_ITF_INPUT_MANAGER_H_

#ifndef _ITF_KEYBOARD_INPUT_SOURCE_H_
#include "engine/input/KeyboardInputSource.h"
#endif //_ITF_KEYBOARD_INPUT_SOURCE_H_

#ifndef _ITF_CONTROLLER_INPUT_SOURCE_H_
#include "engine/input/ControllerInputSource.h"
#endif //_ITF_CONTROLLER_INPUT_SOURCE_H_

#ifndef _ITF_INPUT_MAPPING_DEFAULTS_H_
#include "engine/input/InputMappingDefaults.h"
#endif //_ITF_INPUT_MAPPING_DEFAULTS_H_

namespace ITF
{
    const f32 SDLGamepad::INPUT_DEADZONE = 0.2f;

    SDLGamepad::SDLGamepad()
        : m_gamepad(nullptr)
          , m_joystickId(0)
          , m_id(-1)
          , m_connected(false)
          ,deviceID(0)
          ,deviceOutputID(0)
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
        for (int i = 0; i < JOY_MAX_COUNT; i++)
        {
            m_scePadHandles[i] = -1;
        }
        InitScePad(m_scePadHandles);
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
            m_gamepadCount++;
        }
        else
        {
            m_gamepadCount = (m_gamepadCount > 0) ? m_gamepadCount - 1 : 0;
        }
        NotifyDeviceConnectEvent(index,padType,connected);
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
                setGamepadConnected(i, true, InputAdapter::Pad_X360);
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
                    setGamepadConnected(i, true, InputAdapter::Pad_X360);
                    LOG("[SDL] - Gamepad connected");
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
                m_gamepads[i].cleanup();
                setGamepadConnected(i, false, InputAdapter::Pad_Invalid);
                LOG("[SDL] - Gamepad disconnected");
                break;
            }
        }
    }
    void SDLInput::NotifyDeviceConnectEvent(u32 _padIndex,InputAdapter::PadType _type,bbool isConnected)
    {
        SDLGamepad& gamepad = m_gamepads[_padIndex];
        bool isSonyController = false;
        if (isConnected)
        {
            auto type = SDL_GetGamepadTypeForID(gamepad.getJoystickId());
            if (type == SDL_GAMEPAD_TYPE_PS5 || type == SDL_GAMEPAD_TYPE_PS4)
            {
                isSonyController = true;
                uint32_t resolvedId;
                for (int iPadHandleIdx = 0; iPadHandleIdx < JOY_MAX_COUNT; iPadHandleIdx++)
                {
                    if (gamepad.deviceID !=0) continue;
                    int iScePadHandle = m_scePadHandles[iPadHandleIdx];
                    GetScePadDeviceId(iScePadHandle, resolvedId);
                    if (resolvedId !=0)
                    {
                        gamepad.deviceID = iScePadHandle;
                    }
                    ScePadContainerIdInfo padContainerInfo;
                    {
                        int ret = scePadGetContainerIdInformation(iScePadHandle, &padContainerInfo);
                        if (ret >= 0)
                        {
                            IMMDevice* io_pMmDevice;
                            if (GetMMDeviceFromPadHandle(padContainerInfo.containerInfo,io_pMmDevice))
                            {
#ifdef USE_PAD_HAPTICS
                                gamepad.deviceOutputID = AUDIO_ADAPTER->getDeviceId(io_pMmDevice);
#endif
                            }

                        }
                    }
                }
            }
            else
            {
                gamepad.deviceID = _padIndex;
            }
        }
        if (isConnected)
        {
            m_adapter->OnControllerConnected(_padIndex,gamepad.deviceID,gamepad.deviceOutputID,isSonyController);
        }
        else
        {
            m_adapter->OnControllerDisconnected(_padIndex);
        }
    }
    void SDLInput::InitScePad(int* out_pScePadHandles)
    {
        ScePadInit3Param param;
        memset(&param.reserve, 0x00, sizeof(uint8_t) * 16);
        param.supportBusType = SCE_PAD_SUPPORT_BUS_TYPE_USB_BT;
        int ret = scePadInit();
        if (ret == SCE_OK)
        {
            for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
            {
                out_pScePadHandles[i] = scePadOpen(SCE_USER_SERVICE_STATIC_USER_ID_1 + i, SCE_PAD_PORT_TYPE_STANDARD, 0, nullptr);
            }
        }
    }
    void SDLInput::GetScePadDeviceId(int in_padHandle, uint32_t& out_resolvedId)
    {
        ScePadControllerType padType;
        int ret = scePadGetControllerType(in_padHandle, &padType);
        if (ret == SCE_OK && padType != SCE_PAD_CONTROLLER_TYPE_NOT_CONNECTED)
        {
            out_resolvedId = padType == SCE_PAD_CONTROLLER_TYPE_DUALSENSE ? (in_padHandle | 0x80000000) : in_padHandle;
        }
        else
        {
            out_resolvedId = 0;
        }
    }
    bool SDLInput::GetMMDeviceFromPadHandle(wchar_t const* containerInfo, IMMDevice*& io_pMmDevice)
    {
        IMMDeviceEnumerator* pEnumerator = nullptr;
        HRESULT hr;
        hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr,
            CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
            (void**)&pEnumerator);

        if (hr != S_OK || !pEnumerator)
        {
            return false;
        }

        IMMDeviceCollection* pDevices = nullptr;
        pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATEMASK_ALL, &pDevices);
        bool bDeviceFound = false;
        if (pDevices)
        {
            UINT dwCount = 0;
            pDevices->GetCount(&dwCount);

            for (int32_t i = 0; i < (int32_t)dwCount; i++)
            {
                IMMDevice* pDevice = nullptr;
                pDevices->Item(i, &pDevice);
                if (!pDevice)
                {
                    continue;
                }
                IPropertyStore* pProps = NULL;
                hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
                if (hr != S_OK)
                {
                    pDevice->Release();
                    continue;
                }
                PROPVARIANT propVar;
                PropVariantInit(&propVar);

                hr = pProps->GetValue(PKEY_Devices_ContainerId, &propVar);
                if (hr != S_OK)
                {
                    pProps->Release();
                    pDevice->Release();
                    continue;
                }
                pProps->Release();

                const int32_t MmContainerIdStrLength = 64;
                wchar_t mmContainerIdStr[MmContainerIdStrLength];
                int ret = StringFromGUID2(*propVar.puuid, mmContainerIdStr, MmContainerIdStrLength);
                PropVariantClear(&propVar);

                if (ret == 0 || _wcsnicmp(containerInfo, mmContainerIdStr, MmContainerIdStrLength) != 0)
                {
                    pDevice->Release();
                    continue;
                }

                io_pMmDevice = pDevice;
                bDeviceFound = true;
                break;
            }
            pDevices->Release();
        }
        pEnumerator->Release();

        return bDeviceFound;
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
        InitializeInputManager();
        InputAdapter::LoadPlayerControlSettings();
    }

    InputAdapter_SDL3::~InputAdapter_SDL3()
    {
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

        InputManager* inputManager = m_inputManager;
        if (inputManager && inputManager->IsInitialized())
        {
            for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
            {
                if (m_sdlInput.m_gamepads[i].isConnected())
                {
                    IInputSource* existingSource = inputManager->GetInputSource(i);
                    ControllerInputSource* controllerSource = nullptr;

                    if (existingSource && existingSource->GetInputType() == PhysicalInput::ControllerButton)
                    {
                        controllerSource = static_cast<ControllerInputSource*>(existingSource);
                    }
                    else
                    {
                        auto newSource = std::make_unique<ControllerInputSource>(i);
                        controllerSource = newSource.get();
                        inputManager->RegisterInputSource(std::move(newSource));

                        InitializeDefaultControllerMappings(inputManager->GetInputMapping(), i);
                    }

                    const SDLGamepad& gamepad = m_sdlInput.m_gamepads[i];
                    controllerSource->SetConnected(true);

                    for (u32 button = 0; button < JOY_MAX_BUT; ++button)
                    {
                        PressStatus buttonStatus = gamepad.getButton(button);
                        bool pressed = (buttonStatus == Pressed || buttonStatus == JustPressed);
                        controllerSource->SetButtonState(button, pressed);
                    }

                    for (u32 axis = 0; axis < JOY_MAX_AXES; ++axis)
                    {
                        f32 axisValue = gamepad.getAxis(axis);
                        controllerSource->SetAxisState(axis, axisValue);
                    }
                }
                else
                {
                    IInputSource* existingSource = inputManager->GetInputSource(i);
                    if (existingSource && existingSource->GetInputType() == PhysicalInput::ControllerButton)
                    {
                        ControllerInputSource* controllerSource = static_cast<ControllerInputSource*>(existingSource);
                        controllerSource->SetConnected(false);
                    }
                }
            }
        }

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
