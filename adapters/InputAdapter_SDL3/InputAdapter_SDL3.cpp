#include "precompiled_InputAdapter_SDL3.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#ifdef ITF_USE_SDL
#ifndef _ITF_INPUTADAPTER_SDL3_H_
#include "InputAdapter_SDL3.h"
#endif

#ifndef ITF_SYSTEMADAPTER_WIN_H_
#include "adapters/SystemAdapter_win/SystemAdapter_win.h"
#endif
#include <pad.h>
#include <pad_types.h>
#include <pad_windows_static.h>
#include <mmdeviceapi.h>
#include <propkey.h>

namespace ITF
{
    static constexpr f32 SDL_AXIS_MAX_VALUE = 32767.0f;
    static constexpr f32 SDL_RUMBLE_MAX_VALUE = 65535.0f;
    static constexpr f64 MS_PER_SECOND = 1000.0;
    static constexpr u32 MAX_EVENTS_PER_FRAME = 100;
    static constexpr f32 AXIS_INPUT_THRESHOLD = 0.3f;
    static constexpr uint32_t DUALSENSE_ID_MARKER = 0x80000000;
    static constexpr int32_t GUID_STRING_BUFFER_SIZE = 64;
    static constexpr f32 INPUT_DEADZONE = 0.2f;
    static InputAdapter::PadType MapSDLGamepadTypeToPadType(const SDL_Gamepad* gamepad)
    {
        if (!gamepad)
            return InputAdapter::Pad_Other;

        SDL_GamepadType type = SDL_GetGamepadType(const_cast<SDL_Gamepad*>(gamepad));
        switch (type)
        {
        case SDL_GAMEPAD_TYPE_PS5:
            return InputAdapter::Pad_PS5;
        case SDL_GAMEPAD_TYPE_PS4:
            return InputAdapter::Pad_PS5;
        case SDL_GAMEPAD_TYPE_PS3:
            return InputAdapter::Pad_PS3;
        case SDL_GAMEPAD_TYPE_XBOX360:
            return InputAdapter::Pad_X360;
        case SDL_GAMEPAD_TYPE_XBOXONE:
            return InputAdapter::Pad_GenericXBox;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
            return InputAdapter::Pad_NX_Pro;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
            return InputAdapter::Pad_NX_Joycon;
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
            return InputAdapter::Pad_NX_Joycon_Dual;
        default:
            return InputAdapter::Pad_Other;
        }
    }



    SDLGamepad::SDLGamepad()
        : m_gamepad(nullptr)
          , m_joystickId(0)
          , m_id(-1)
          , m_connected(false)
          , deviceID(0)
          , deviceOutputID(0)
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

        updateButtonState(m_joyButton_A, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_SOUTH));
        updateButtonState(m_joyButton_B, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_EAST));
        updateButtonState(m_joyButton_X, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_WEST));
        updateButtonState(m_joyButton_Y, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_NORTH));

        updateButtonState(m_joyButton_LB, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER));
        updateButtonState(m_joyButton_RB, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER));

        updateButtonState(m_joyButton_Back, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_BACK));
        updateButtonState(m_joyButton_Start, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_START));

        updateButtonState(m_joyButton_ThumbLeft, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK));
        updateButtonState(m_joyButton_ThumbRight, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK));

        updateButtonState(m_joyButton_DPadU, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP));
        updateButtonState(m_joyButton_DPadD, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN));
        updateButtonState(m_joyButton_DPadL, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT));
        updateButtonState(m_joyButton_DPadR, SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT));

        updateAxisState(
            JOY_AXIS_LX, static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTX)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            JOY_AXIS_LY, static_cast<f32>(-SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTY)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            JOY_AXIS_RX, static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTX)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            JOY_AXIS_RY,
            static_cast<f32>(-SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTY)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            JOY_AXIS_LT,
            static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            JOY_AXIS_RT,
            static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)) / SDL_AXIS_MAX_VALUE);

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

    void SDLGamepad::updateButtonState(u32 buttonIndex, bool pressed)
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
            if (previousStatus == InputAdapter::Pressed || previousStatus == InputAdapter::JustPressed)
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

        Uint16 leftMotor = static_cast<Uint16>(_leftMotorSpeed * SDL_RUMBLE_MAX_VALUE);
        Uint16 rightMotor = static_cast<Uint16>(_rightMotorSpeed * SDL_RUMBLE_MAX_VALUE);
        Uint32 durationMs = (_duration > 0.0) ? static_cast<Uint32>(_duration * MS_PER_SECOND) : 0;

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
        for (int i = 0; i < JOY_MAX_COUNT; ++i)
        {
            m_scePadHandles[i] = -1;
        }
        initScePad(m_scePadHandles);
    }

    SDLInput::~SDLInput()
    {
        cleanup();
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

    void SDLInput::cleanup()
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

    void SDLInput::updateInputState()
    {
        if (!m_initialized)
            return;

        SDL_Event event;
        u32 eventCount = 0;

        while (SDL_PollEvent(&event) && eventCount < MAX_EVENTS_PER_FRAME)
        {
            eventCount++;

            switch (event.type)
            {
            case SDL_EVENT_GAMEPAD_ADDED:
                handleGamepadConnected(event.gdevice.which);
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                handleGamepadDisconnected(event.gdevice.which);
                break;
            default:
                break;
            }
        }

        for (auto& gamepad : m_gamepads)
        {
            gamepad.updateInputState();
        }
    }

    void SDLInput::setGamepadConnected(u32 index, bool connected, InputAdapter::PadType padType)
    {
        m_adapter->setPadConnected(index, connected);
        m_adapter->setPadType(index, padType);

        if (connected)
        {
            ++m_gamepadCount;
        }
        else if (m_gamepadCount > 0)
        {
            --m_gamepadCount;
        }

        notifyDeviceConnectEvent(index, padType, connected);
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
                InputAdapter::PadType padType = MapSDLGamepadTypeToPadType(m_gamepads[i].getGamepad());
                setGamepadConnected(i, true, padType);
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
                    InputAdapter::PadType padType = MapSDLGamepadTypeToPadType(m_gamepads[i].getGamepad());
                    setGamepadConnected(i, true, padType);
                    LOG("[SDL] - Gamepad connected at slot %u", i);
                    break;
                }
            }
        }
    }

    void SDLInput::handleGamepadDisconnected(SDL_JoystickID instanceId)
    {
        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            if (m_gamepads[i].getJoystickId() == instanceId && m_gamepads[i].isConnected())
            {
                m_gamepads[i].cleanup();
                setGamepadConnected(i, false, InputAdapter::Pad_Invalid);
                LOG("[SDL] - Gamepad disconnected at slot %u", i);
                break;
            }
        }
    }

    void SDLInput::notifyDeviceConnectEvent(u32 padIndex, InputAdapter::PadType type, bbool isConnected)
    {
        if (!isConnected)
        {
            m_adapter->OnControllerDisconnected(padIndex);
            return;
        }

        SDLGamepad& gamepad = m_gamepads[padIndex];

        SDL_GamepadType sdlType = SDL_GetGamepadTypeForID(gamepad.getJoystickId());
        bool isSonyController = (sdlType == SDL_GAMEPAD_TYPE_PS5 || sdlType == SDL_GAMEPAD_TYPE_PS4);

        if (isSonyController)
        {
            initSonyControllerDeviceIds(gamepad);
        }
        else
        {
            gamepad.deviceID = padIndex;
        }

        m_adapter->OnControllerConnected(padIndex, gamepad.deviceID, gamepad.deviceOutputID, isSonyController);
    }

    void SDLInput::initSonyControllerDeviceIds(SDLGamepad& gamepad)
    {
        if (gamepad.deviceID != 0)
            return;

        for (int i = 0; i < JOY_MAX_COUNT; ++i)
        {
            int scePadHandle = m_scePadHandles[i];
            uint32_t resolvedId = 0;
            getScePadDeviceId(scePadHandle, resolvedId);

            if (resolvedId != 0)
            {
                gamepad.deviceID = scePadHandle;

                ScePadContainerIdInfo padContainerInfo;
                int ret = scePadGetContainerIdInformation(scePadHandle, &padContainerInfo);
                if (ret >= 0)
                {
                    IMMDevice* pMmDevice = nullptr;
                    if (getMMDeviceFromPadHandle(padContainerInfo.containerInfo, pMmDevice))
                    {
#ifdef USE_PAD_HAPTICS
                        gamepad.deviceOutputID = AUDIO_ADAPTER->getDeviceId(pMmDevice);
#endif
                        if (pMmDevice)
                            pMmDevice->Release();
                    }
                }
                break;
            }
        }
    }

    void SDLInput::initScePad(int* outScePadHandles)
    {
        int ret = scePadInit();
        if (ret != SCE_OK)
        {
            LOG("[SDL] Failed to initialize ScePad: %d", ret);
            return;
        }

        for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; ++i)
        {
            outScePadHandles[i] = scePadOpen(
                SCE_USER_SERVICE_STATIC_USER_ID_1 + i, SCE_PAD_PORT_TYPE_STANDARD, 0, nullptr);
        }
    }

    void SDLInput::getScePadDeviceId(int padHandle, uint32_t& outResolvedId)
    {
        outResolvedId = 0;

        ScePadControllerType padType;
        int ret = scePadGetControllerType(padHandle, &padType);

        if (ret != SCE_OK || padType == SCE_PAD_CONTROLLER_TYPE_NOT_CONNECTED)
            return;

        outResolvedId = (padType == SCE_PAD_CONTROLLER_TYPE_DUALSENSE)
                            ? (padHandle | DUALSENSE_ID_MARKER)
                            : padHandle;
    }

    bool SDLInput::getMMDeviceFromPadHandle(wchar_t const* containerInfo, IMMDevice*& outMmDevice)
    {
        outMmDevice = nullptr;

        IMMDeviceEnumerator* pEnumerator = nullptr;
        HRESULT hr = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr,
            CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(&pEnumerator));

        if (FAILED(hr) || !pEnumerator)
            return false;

        IMMDeviceCollection* pDevices = nullptr;
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATEMASK_ALL, &pDevices);

        bool deviceFound = false;

        if (SUCCEEDED(hr) && pDevices)
        {
            UINT deviceCount = 0;
            pDevices->GetCount(&deviceCount);

            for (UINT i = 0; i < deviceCount; ++i)
            {
                IMMDevice* pDevice = nullptr;
                if (FAILED(pDevices->Item(i, &pDevice)) || !pDevice)
                    continue;

                IPropertyStore* pProps = nullptr;
                hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
                if (FAILED(hr) || !pProps)
                {
                    pDevice->Release();
                    continue;
                }

                PROPVARIANT propVar;
                PropVariantInit(&propVar);
                hr = pProps->GetValue(PKEY_Devices_ContainerId, &propVar);
                pProps->Release();

                if (FAILED(hr))
                {
                    pDevice->Release();
                    continue;
                }

                // Convert GUID to string for comparison
                wchar_t mmContainerIdStr[GUID_STRING_BUFFER_SIZE];
                int ret = StringFromGUID2(*propVar.puuid, mmContainerIdStr, GUID_STRING_BUFFER_SIZE);
                PropVariantClear(&propVar);

                // Case-insensitive comparison (container IDs are lowercase)
                if (ret == 0 || _wcsnicmp(containerInfo, mmContainerIdStr, GUID_STRING_BUFFER_SIZE) != 0)
                {
                    pDevice->Release();
                    continue;
                }

                // Found matching device
                outMmDevice = pDevice;
                deviceFound = true;
                break;
            }
            pDevices->Release();
        }
        pEnumerator->Release();

        return deviceFound;
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
        memset(m_connectedPlayers, 0, JOY_MAX_COUNT * sizeof(PlayerState));
        m_connectedPlayers[0] = ePlaying;

        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            m_slotGamepad[i] = -1;
        }
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
        m_sdlInput.updateInputState();

        bbool share = IsKeyboardControllerSharingEnabled();
        bbool connectedGamepad[JOY_MAX_COUNT];
        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            connectedGamepad[i] = m_sdlInput.m_gamepads[i].isConnected();
        }

        for (u32 slot = 0; slot < JOY_MAX_COUNT; ++slot)
        {
            i32 idx = m_slotGamepad[slot];
            if (idx < 0 || idx >= JOY_MAX_COUNT || !connectedGamepad[idx])
            {
                m_slotGamepad[slot] = -1;
            }
        }

        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            if (!connectedGamepad[i])
                continue;

            bbool alreadyMapped = bfalse;
            for (u32 s = 0; s < JOY_MAX_COUNT; ++s)
            {
                if (m_slotGamepad[s] == static_cast<i32>(i))
                {
                    alreadyMapped = btrue;
                    break;
                }
            }
            if (alreadyMapped)
                continue;

            if (share)
            {
                if (m_slotGamepad[0] == -1)
                {
                    m_slotGamepad[0] = static_cast<i32>(i);
                }
                else
                {
                    for (u32 s = 1; s < JOY_MAX_COUNT; ++s)
                    {
                        if (m_slotGamepad[s] == -1)
                        {
                            m_slotGamepad[s] = static_cast<i32>(i);
                            break;
                        }
                    }
                }
            }
            else
            {
                for (u32 s = 1; s < JOY_MAX_COUNT; ++s)
                {
                    if (m_slotGamepad[s] == -1)
                    {
                        m_slotGamepad[s] = static_cast<i32>(i);
                        break;
                    }
                }
            }
        }
        for (u32 t = 0; t < JOY_MAX_COUNT; ++t)
        {
            for (u32 axis = 0; axis < JOY_MAX_AXES; ++axis)
            {
                m_axes[t][axis] = 0.0f;
            }
            for (u32 button = 0; button < JOY_MAX_BUT; ++button)
            {
                m_buttons[t][button] = Released;
            }

            if (t > 0)
            {
                setPadConnected(t, bfalse);
            }
        }

        for (u32 slot = 0; slot < JOY_MAX_COUNT; ++slot)
        {
            i32 idx = m_slotGamepad[slot];
            if (idx < 0 || idx >= JOY_MAX_COUNT)
                continue;
            if (!connectedGamepad[idx])
                continue;

            const SDLGamepad& gamepad = m_sdlInput.m_gamepads[idx];

            bbool hasGamepadInput = bfalse;

            for (u32 button = 0; button < JOY_MAX_BUT; ++button)
            {
                PressStatus status = gamepad.getButton(button);
                m_buttons[slot][button] = status;

                if (status == Pressed || status == JustPressed)
                {
                    hasGamepadInput = btrue;
                }
            }

            for (u32 axis = 0; axis < JOY_MAX_AXES; ++axis)
            {
                f32 axisValue = gamepad.getAxis(axis);
                m_axes[slot][axis] = axisValue;

                if (!hasGamepadInput && (axisValue > AXIS_INPUT_THRESHOLD || axisValue < -AXIS_INPUT_THRESHOLD))
                {
                    hasGamepadInput = btrue;
                }
            }

            if (hasGamepadInput)
            {
                setLastUsedInputDevice(slot, InputDevice_Gamepad);
            }

            if (m_connectedPlayers[slot] == eNotConnected)
            {
                m_connectedPlayers[slot] = ePlaying;
            }
            setPadConnected(slot, btrue);
            setPadType(slot, MapSDLGamepadTypeToPadType(gamepad.getGamepad()));
        }

        for (u32 slot = 1; slot < JOY_MAX_COUNT; ++slot)
        {
            if (m_slotGamepad[slot] == -1)
            {
                if (m_connectedPlayers[slot] != eNotConnected)
                    m_connectedPlayers[slot] = eNotConnected;
            }
        }

        if (m_slotGamepad[0] == -1)
        {
            m_connectedPlayers[0] = ePlaying;
        }
        setPadConnected(0, btrue);
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
}
#endif
