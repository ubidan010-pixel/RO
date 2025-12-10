#include "precompiled_InputAdapter_SDL3.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
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

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#include "engine/singleton/Singletons.h"

namespace ITF
{
    static constexpr f32 SDL_AXIS_MAX_VALUE = 32767.0f;
    static constexpr u32 MAX_EVENTS_PER_FRAME = 100;
    static constexpr f32 AXIS_INPUT_THRESHOLD = 0.3f;
    static constexpr int32_t GUID_STRING_BUFFER_SIZE = 64;
    static constexpr f32 INPUT_DEADZONE = 0.2f;
    static f64 s_lastLeftMouseClick = 0.;

    ITF_INLINE i32 TranslateSDLKey(SDL_Keycode keycode)
    {
        switch (keycode)
        {
        case SDLK_ESCAPE: return KEY_ESC;
        case SDLK_RETURN: return KEY_ENTER;
        case SDLK_BACKSPACE: return KEY_BACKSPACE;
        case SDLK_TAB: return KEY_TAB;
        case SDLK_SPACE: return KEY_SPACE;
        case SDLK_PAGEUP: return KEY_PAGEUP;
        case SDLK_PAGEDOWN: return KEY_PAGEDOWN;
        case SDLK_END: return KEY_END;
        case SDLK_HOME: return KEY_HOME;
        case SDLK_LEFT: return KEY_LEFT;
        case SDLK_UP: return KEY_UP;
        case SDLK_RIGHT: return KEY_RIGHT;
        case SDLK_DOWN: return KEY_DOWN;
        case SDLK_INSERT: return KEY_INSERT;
        case SDLK_DELETE: return KEY_DEL;
        case SDLK_LCTRL: return KEY_LCTRL;
        case SDLK_RCTRL: return KEY_RCTRL;
        case SDLK_LSHIFT: return KEY_LSHIFT;
        case SDLK_RSHIFT: return KEY_RSHIFT;
        case SDLK_LALT: return KEY_LALT;
        case SDLK_RALT: return KEY_RALT;
        case SDLK_KP_0: return KEY_KP_0;
        case SDLK_KP_1: return KEY_KP_1;
        case SDLK_KP_2: return KEY_KP_2;
        case SDLK_KP_3: return KEY_KP_3;
        case SDLK_KP_4: return KEY_KP_4;
        case SDLK_KP_5: return KEY_KP_5;
        case SDLK_KP_6: return KEY_KP_6;
        case SDLK_KP_7: return KEY_KP_7;
        case SDLK_KP_8: return KEY_KP_8;
        case SDLK_KP_9: return KEY_KP_9;
        case SDLK_KP_MULTIPLY: return KEY_KP_MULTIPLY;
        case SDLK_KP_PLUS: return KEY_KP_ADD;
        case SDLK_KP_MINUS: return KEY_KP_SUBTRACT;
        case SDLK_KP_PERIOD: return KEY_KP_DECIMAL;
        case SDLK_KP_DIVIDE: return KEY_KP_DIVIDE;
        case SDLK_KP_EQUALS: return KEY_KP_EQUAL;
        case SDLK_KP_ENTER: return KEY_KP_ENTER;
        case SDLK_PRINTSCREEN: return KEY_PRINT;
        case SDLK_F1: return KEY_F1;
        case SDLK_F2: return KEY_F2;
        case SDLK_F3: return KEY_F3;
        case SDLK_F4: return KEY_F4;
        case SDLK_F5: return KEY_F5;
        case SDLK_F6: return KEY_F6;
        case SDLK_F7: return KEY_F7;
        case SDLK_F8: return KEY_F8;
        case SDLK_F9: return KEY_F9;
        case SDLK_F10: return KEY_F10;
        case SDLK_F11: return KEY_F11;
        case SDLK_F12: return KEY_F12;
        case SDLK_F13: return KEY_F13;
        case SDLK_F14: return KEY_F14;
        case SDLK_F15: return KEY_F15;
        case SDLK_F16: return KEY_F16;
        case SDLK_F17: return KEY_F17;
        case SDLK_F18: return KEY_F18;
        case SDLK_F19: return KEY_F19;
        case SDLK_F20: return KEY_F20;
        case SDLK_F21: return KEY_F21;
        case SDLK_F22: return KEY_F22;
        case SDLK_F23: return KEY_F23;
        case SDLK_F24: return KEY_F24;
        default:
            if (keycode >= 32 && keycode < 256)
                return static_cast<i32>(keycode);
            return KEY_SPECIAL;
        }
    }

    ITF_INLINE InputAdapter::MouseButton TranslateSDLMouseButton(u8 button)
    {
        switch (button)
        {
        case SDL_BUTTON_LEFT: return InputAdapter::MB_Left;
        case SDL_BUTTON_RIGHT: return InputAdapter::MB_Right;
        case SDL_BUTTON_MIDDLE: return InputAdapter::MB_Middle;
        default: return static_cast<InputAdapter::MouseButton>(-1);
        }
    }

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
            return InputAdapter::Pad_PS4;
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
          , m_deviceID(-1)
          , m_deviceOutputID(0)
    {
        memset(m_axes, 0, sizeof(m_axes));
        memset(m_buttons, 0, sizeof(m_buttons));
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
            m_joyStickLeft_X,
            static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTX)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            m_joyStickLeft_Y,
            static_cast<f32>(-SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTY)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            m_joyStickRight_X,
            static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTX)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            m_joyStickRight_Y,
            static_cast<f32>(-SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTY)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            m_joyTrigger_Left,
            static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER)) / SDL_AXIS_MAX_VALUE);
        updateAxisState(
            m_joyTrigger_Right,
            static_cast<f32>(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)) / SDL_AXIS_MAX_VALUE);
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
        m_deviceID = (u32)-1;
        m_deviceOutputID = 0;
        m_padType = InputAdapter::Pad_Invalid;
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

    bool SDLGamepad::isConnected() const
    {
        return m_connected && m_gamepad;
    }

    SDLInput::SDLInput()
        : m_gamepadCount(0)
          , m_initialized(false)
          , m_adapter(nullptr)
          , m_hiddenWindow(nullptr)
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
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS) != 0)
        {
            LOG("[SDL] - SDL_Init(video|gamepad|events) failed: %s", SDL_GetError());
            SDL_InitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
        }

        const bool videoReady = SDL_WasInit(SDL_INIT_VIDEO) != 0;
        if (videoReady)
        {
            auto* sysAdapter = static_cast<SystemAdapter_win*>(SYSTEM_ADAPTER);
            if (sysAdapter && sysAdapter->m_hwnd)
            {
                SDL_PropertiesID props = SDL_CreateProperties();
                SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER, sysAdapter->m_hwnd);
                SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
                m_hiddenWindow = SDL_CreateWindowWithProperties(props);
                SDL_DestroyProperties(props);

                if (!m_hiddenWindow)
                {
                    LOG("[SDL] - failed to wrap existing HWND for input: %s", SDL_GetError());
                }
                else
                {
                    SDL_SetWindowResizable(m_hiddenWindow, true);
                }
            }
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
        if (m_hiddenWindow)
        {
            SDL_DestroyWindow(m_hiddenWindow);
            m_hiddenWindow = nullptr;
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
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                if (!m_adapter || event.key.repeat)
                    break;
                {
                    i32 translatedKey = TranslateSDLKey(event.key.key);
                    if (translatedKey >= 0 && translatedKey < KEY_COUNT)
                    {
                        m_adapter->onKey(translatedKey,
                                         (event.type == SDL_EVENT_KEY_DOWN)
                                             ? InputAdapter::Pressed
                                             : InputAdapter::Released);
                        handleHotKeyEvent(translatedKey);
                    }
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (!m_adapter)
                    break;
                {
                    InputAdapter::MouseButton button = TranslateSDLMouseButton(event.button.button);
                    if (button != static_cast<InputAdapter::MouseButton>(-1))
                    {
                        InputAdapter::PressStatus status = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                                                               ? (event.button.clicks > 1
                                                                      ? InputAdapter::Double_Press
                                                                      : InputAdapter::Pressed)
                                                               : InputAdapter::Released;
                        m_adapter->onMouseButton(button, status);
                    }
                }
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                if (m_adapter)
                {
                    m_adapter->onMouseWheel(static_cast<i32>(event.wheel.y));
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if (m_adapter)
                {
                    m_adapter->onMousePos(static_cast<i32>(event.motion.x), static_cast<i32>(event.motion.y));
                }
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                if (m_adapter)
                {
                    m_adapter->setFocus();
                }
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                if (m_adapter)
                {
                    m_adapter->unsetFocus();
                }
                break;
            default:
                break;
            }
        }
        for (int k = 0; k < JOY_MAX_COUNT; k++)
        {
            auto& gamepad = m_gamepads[k];
            gamepad.updateInputState();
            updateSonyControllerDeviceIds(k, gamepad);
        }
        if (m_adapter)
        {
            m_adapter->dispatchEventsToListeners();
        }
    }
    void SDLInput::handleHotKeyEvent(i32 translatedKey)
    {
        InputAdapter_SDL3* sdlAdapter = static_cast<InputAdapter_SDL3*>(m_adapter);
        switch (translatedKey)
        {
        case KEY_ENTER:
        {
            bool altHeld = sdlAdapter->isKeyPressed(KEY_LALT) || sdlAdapter->isKeyPressed(KEY_RALT);
            if (altHeld && RAY_GAMEMANAGER)
            {
                bbool windowedNow = SYSTEM_ADAPTER->isFullScreenMode();
                const bbool windowedNext = (bbool)!windowedNow;

                RAY_GAMEMANAGER->setWindowed(windowedNext);
                RAY_GAMEMANAGER->applyWindowsMode(windowedNext);
            }
            break;
        }
        default:
            break;
        }
    }
    void SDLInput::setGamepadConnected(u32 index, bool connected, InputAdapter::PadType padType)
    {
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
                m_gamepads[i].m_padType = padType;
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

    void SDLInput::notifyDeviceConnectEvent(u32 padIndex, InputAdapter::PadType padType, bbool isConnected)
    {
        SDLGamepad& gamepad = m_gamepads[padIndex];
        gamepad.m_padType = padType;
        bool isSonyController = padType == InputAdapter::Pad_PS4 || gamepad.m_padType == InputAdapter::Pad_PS5;
        if (!isConnected)
        {
            m_adapter->OnControllerDisconnected(padIndex);
            if (isSonyController)
            {
                scePadClose(gamepad.m_deviceID);
            }
            gamepad.cleanup();
        }
        else
        {
            if (isSonyController)
            {
                updateSonyControllerDeviceIds(padIndex, gamepad);
            }
            else
            {
                gamepad.m_deviceID = padIndex;
                m_adapter->OnControllerConnected(padIndex, gamepad.m_deviceID, gamepad.m_deviceOutputID,
                                                 gamepad.m_padType);
            }
        }
        m_adapter->setPadConnected(padIndex, isConnected);
        m_adapter->setPadType(padIndex, padType);
    }

    bbool SDLInput::checkExistDeviceId(u32 deviceId)
    {
        for (auto& contr : m_gamepads)
        {
            if (contr.m_deviceID == deviceId)
            {
                return btrue;
            }
        }
        return bfalse;
    }

    void SDLInput::updateSonyControllerDeviceIds(u32 padIndex, SDLGamepad& gamepad)
    {
        if (!gamepad.isSonyController() || gamepad.m_deviceID != (u32)-1)
            return;

        for (int i = 0; i < JOY_MAX_COUNT; ++i)
        {
            //  m_scePadHandles[i] = scePadOpen(SCE_USER_SERVICE_STATIC_USER_ID_1 + i, SCE_PAD_PORT_TYPE_STANDARD, 0, nullptr);
            int scePadHandle = m_scePadHandles[i];
            if (scePadHandle >= 0)
            {
                bbool isExistingSonyDevice = checkExistDeviceId(scePadHandle);
                if (isExistingSonyDevice)
                {
                    continue;
                }
                uint32_t resolvedId = 0;
                getScePadDeviceId(scePadHandle, resolvedId);
                if (resolvedId == 0)
                {
                    continue;
                }
                gamepad.m_deviceID = scePadHandle;
                ScePadContainerIdInfo padContainerInfo;
                int ret = scePadGetContainerIdInformation(scePadHandle, &padContainerInfo);
                if (ret >= 0)
                {
                    IMMDevice* pMmDevice = nullptr;
                    if (getMMDeviceFromPadHandle(padContainerInfo.containerInfo, pMmDevice))
                    {
#ifdef USE_PAD_HAPTICS
                        gamepad.m_deviceOutputID = AUDIO_ADAPTER->getDeviceId(pMmDevice);
#endif
                        if (pMmDevice)
                            pMmDevice->Release();
                    }
                }
                m_adapter->OnControllerConnected(padIndex, gamepad.m_deviceID, gamepad.m_deviceOutputID,
                                                 gamepad.m_padType);
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
        outResolvedId = padHandle;
        // outResolvedId = (padType == SCE_PAD_CONTROLLER_TYPE_DUALSENSE)
        //                     ? (padHandle | DUALSENSE_ID_MARKER)
        //                     : padHandle;
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
        : m_lastWheelValue(0)
          , m_lastMouseX(0)
          , m_lastMouseY(0)
          , m_leftMBIsPressed(bfalse)
          , m_rightMBIsPressed(bfalse)
          , m_middleMBIsPressed(bfalse)
    {
        ITF_MemSet(m_keys, 0, sizeof(m_keys));
        ITF_MemSet(m_keysReleaseTime, 0, sizeof(m_keysReleaseTime));
        ITF_MemSet(m_keyStatus, 0, sizeof(m_keyStatus));
        ITF_MemSet(m_keyPressTime, 0, sizeof(m_keyPressTime));

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
        m_listeners.clear();
        m_eventPool.clear();
    }

    void InputAdapter_SDL3::addListener(Interface_InputListener* _listener, u32 _priority)
    {
        ITF_ASSERT(_listener);
        ListenerEntry entry;
        entry.m_listener = _listener;
        entry.m_priority = _priority;
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            if (m_listeners[i].m_priority > _priority)
            {
                m_listeners.insert(entry, i);
                return;
            }
        }
        m_listeners.push_back(entry);
    }

    void InputAdapter_SDL3::removeListener(Interface_InputListener* _listener)
    {
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            if (m_listeners[i].m_listener == _listener)
            {
                m_listeners.eraseKeepOrder(i);
                return;
            }
        }
    }

    void InputAdapter_SDL3::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
    {
        if (_but == MB_Left && _status == Pressed)
        {
            f64 thisTime = SYSTEM_ADAPTER->getTime();
            f64 deltaTime = thisTime - s_lastLeftMouseClick;
            if (deltaTime >= 0. && deltaTime < fDoublePressMaxDuration)
            {
                _status = Double_Press;
                s_lastLeftMouseClick = thisTime - fDoublePressMaxDuration - (f64)MTH_EPSILON;
            }
            else
            {
                s_lastLeftMouseClick = thisTime;
            }
        }

        switch (_but)
        {
        case MB_Left:
            m_leftMBIsPressed = (_status != Released);
            break;
        case MB_Right:
            m_rightMBIsPressed = (_status == Pressed);
            break;
        case MB_Middle:
            m_middleMBIsPressed = (_status == Pressed);
            break;
        default:
            break;
        }

        if (_status == Pressed || _status == Double_Press)
        {
            setLastUsedInputDevice(0, InputDevice_Keyboard);
        }

        pushMouseButtonEvent(_but, _status);
    }

    void InputAdapter_SDL3::onMouseWheel(i32 _wheelValue)
    {
        const i32 delta = _wheelValue;
        m_lastWheelValue += delta;
        pushMouseWheelEvent(m_lastWheelValue, delta);
    }

    void InputAdapter_SDL3::flushKeys()
    {
        ITF_MemSet(m_keys, 0, sizeof(m_keys));
    }

    void InputAdapter_SDL3::onKey(i32 _key, PressStatus _status)
    {
        ITF_ASSERT((_key >= 0) && (_key < KEY_COUNT));

        switch (_status)
        {
        case Pressed:
            m_keys[_key] = btrue;
            setLastUsedInputDevice(0, InputDevice_Keyboard);
            break;
        case Released:
            m_keys[_key] = bfalse;
            m_keysReleaseTime[_key] = ELAPSEDTIME;
            break;
        default:
            ITF_ASSERT(0);
        }
        pushKeyEvent(_key, _status);
    }

    bbool InputAdapter_SDL3::isKeyPressed(i32 _key) const
    {
        ITF_ASSERT((_key >= 0) && (_key < KEY_COUNT));
        return m_keys[_key];
    }

    void InputAdapter_SDL3::onMousePos(i32 _x, i32 _y)
    {
        m_lastMouseX = _x;
        m_lastMouseY = _y;
        pushMousePosEvent(_x, _y);
    }

    void InputAdapter_SDL3::getMousePos(i32& _x, i32& _y) const
    {
        _x = m_lastMouseX;
        _y = m_lastMouseY;
    }

    bbool InputAdapter_SDL3::isMousePressed(MouseButton _but) const
    {
        switch (_but)
        {
        case MB_Left: return m_leftMBIsPressed;
        case MB_Right: return m_rightMBIsPressed;
        case MB_Middle: return m_middleMBIsPressed;
        default: break;
        }
        return bfalse;
    }

    void InputAdapter_SDL3::dispatchEventsToListeners()
    {
        const u32 count = m_eventPool.size();
        for (u32 evtIt = 0; evtIt < count; ++evtIt)
        {
            EditorEvent& editorEvent = m_eventPool[evtIt];
            switch (editorEvent.m_eventType)
            {
            case IA_EventKey:
                {
                    const i32 key = editorEvent.m_key.m_key;
                    const PressStatus status = editorEvent.m_key.m_status;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onKey(key, status)))
                            break;
                    }
                }
                break;
            case IA_EventMouseButton:
                {
                    const MouseButton but = editorEvent.m_but.m_but;
                    const PressStatus status = editorEvent.m_but.m_status;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMouseButton(but, status)))
                            break;
                    }
                }
                break;
            case IA_EventMousePos:
                {
                    const i32 x = editorEvent.m_val.m_x;
                    const i32 y = editorEvent.m_val.m_y;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMousePos(x, y)))
                            break;
                    }
                }
                break;
            case IA_EventMouseWheel:
                {
                    const i32 wheelValue = editorEvent.m_val.m_x;
                    const i32 delta = editorEvent.m_val.m_y;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMouseWheel(wheelValue, delta)))
                            break;
                    }
                }
                break;
            }
        }
        m_eventPool.clear();
    }

    void InputAdapter_SDL3::platformUpdateKeyboardState()
    {
#if defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD
        bbool hasKeyboardInput = bfalse;

        for (u32 keyIndex = 0; keyIndex < KEY_COUNT; ++keyIndex)
        {
            const bbool pressed = m_keys[keyIndex];
            m_keyPressTime[keyIndex] += 1;

            if (pressed)
            {
                switch (m_keyStatus[keyIndex])
                {
                case Released:
                case JustReleased:
                    m_keyStatus[keyIndex] = JustPressed;
                    m_keyPressTime[keyIndex] = 0;
                    hasKeyboardInput = btrue;
                    break;
                case JustPressed:
                case Pressed:
                    m_keyStatus[keyIndex] = Pressed;
                    hasKeyboardInput = btrue;
                    break;
                default: ;
                }
            }
            else
            {
                switch (m_keyStatus[keyIndex])
                {
                case Released:
                case JustReleased:
                    m_keyStatus[keyIndex] = Released;
                    break;
                case JustPressed:
                case Pressed:
                    m_keyStatus[keyIndex] = JustReleased;
                    break;
                default: ;
                }
            }
        }

        if (hasKeyboardInput)
        {
            setLastUsedInputDevice(0, InputDevice_Keyboard);
        }

        updateKeyboardForPlayer0();
#endif
    }

    InputAdapter::PressStatus InputAdapter_SDL3::getKeyboardStatus(i32 key) const
    {
        if (key < 0 || key >= KEY_COUNT)
            return Released;
        return m_keyStatus[key];
    }

    u32 InputAdapter_SDL3::getKeyboardPressTime(i32 key) const
    {
        if (key < 0 || key >= KEY_COUNT)
            return std::numeric_limits<u32>::max();
        return m_keyPressTime[key];
    }

    i32 InputAdapter_SDL3::GetFirstPressedRawKey() const
    {
        for (i32 key = 0; key < KEY_COUNT; ++key)
        {
            if (m_keyStatus[key] == JustPressed || m_keyStatus[key] == Pressed)
            {
                return key;
            }
        }
        return -1;
    }

    void InputAdapter_SDL3::pushKeyEvent(i32 _key, PressStatus _status)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventKey;
        evt.m_key.m_key = _key;
        evt.m_key.m_status = _status;
        m_eventPool.push_back(evt);
    }

    void InputAdapter_SDL3::pushMouseButtonEvent(MouseButton _but, PressStatus _status)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMouseButton;
        evt.m_but.m_but = _but;
        evt.m_but.m_status = _status;
        m_eventPool.push_back(evt);
    }

    void InputAdapter_SDL3::pushMousePosEvent(i32 _x, i32 _y)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMousePos;
        evt.m_val.m_x = _x;
        evt.m_val.m_y = _y;
        m_eventPool.push_back(evt);
    }

    void InputAdapter_SDL3::pushMouseWheelEvent(i32 _wheel, i32 _delta)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMouseWheel;
        evt.m_val.m_x = _wheel;
        evt.m_val.m_y = _delta;
        m_eventPool.push_back(evt);
    }

    void InputAdapter_SDL3::updateKeyboardForPlayer0()
    {
        if (m_connectedPlayers[0] != eNotConnected)
        {
            bbool keyboardUsed = bfalse;

            auto updateKeyboardButton = [&](u32 buttonIndex, PressStatus status) {
                m_keyboardButtons[0][buttonIndex] = status;

                if (status == Pressed || status == JustPressed)
                {
                    keyboardUsed = btrue;
                }
            };

            updateKeyboardButton(m_joyButton_DPadU, getKeyboardStatus(KEY_UP));
            updateKeyboardButton(m_joyButton_DPadD, getKeyboardStatus(KEY_DOWN));
            updateKeyboardButton(m_joyButton_DPadL, getKeyboardStatus(KEY_LEFT));
            updateKeyboardButton(m_joyButton_DPadR, getKeyboardStatus(KEY_RIGHT));

            updateKeyboardButton(m_joyButton_A, getKeyboardStatus(KEY_SPACE));
            updateKeyboardButton(m_joyButton_X, getKeyboardStatus('s'));
            updateKeyboardButton(m_joyButton_B, getKeyboardStatus(KEY_BACKSPACE));
            updateKeyboardButton(m_joyButton_Start, getKeyboardStatus(KEY_ESC));

            const bbool sprintPressed = (getKeyboardStatus(KEY_LSHIFT) == Pressed || getKeyboardStatus(KEY_LSHIFT) == JustPressed);
            m_keyboardAxes[0][m_joyTrigger_Right] = sprintPressed ? 1.0f : 0.0f;
            if (sprintPressed)
            {
                keyboardUsed = btrue;
            }

            if (keyboardUsed)
            {
                setLastUsedInputDevice(0, InputDevice_Keyboard);
            }
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
            if (share)
            {
                setPadType(0, Pad_Keyboard);
                setLastUsedInputDevice(0, InputDevice_Keyboard);
            }
        }
        setPadConnected(0, btrue);
    }

    void InputAdapter_SDL3::OnPCControlModeChanged(PCControlMode previous, PCControlMode current)
    {
        ITF_UNUSED(previous)

        if (current != PCControlMode_Keyboard)
            return;

        m_slotGamepad[0] = -1;

        for (u32 axis = 0; axis < JOY_MAX_AXES; ++axis)
        {
            m_axes[0][axis] = 0.0f;
        }

        for (u32 button = 0; button < JOY_MAX_BUT; ++button)
        {
            m_buttons[0][button] = Released;
        }

        setPadType(0, Pad_Keyboard);
        m_connectedPlayers[0] = ePlaying;
        setPadConnected(0, btrue);
        setLastUsedInputDevice(0, InputDevice_Keyboard);
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

    void InputAdapter_SDL3::swapControllers(u32 index1, u32 index2)
    {
        LOG("[InputAdapter_SDL3] swapControllers(%u, %u)", index1, index2);
        if (index1 < JOY_MAX_COUNT && index2 < JOY_MAX_COUNT)
        {
            std::swap(m_slotGamepad[index1], m_slotGamepad[index2]);
        }
    }
}
