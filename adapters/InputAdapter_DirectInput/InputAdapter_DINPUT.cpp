#include "precompiled_inputadapter_DINPUT.h"
#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_INPUTADAPTER_DINPUT_H_
#include "adapters/InputAdapter_DirectInput/InputAdapter_DINPUT.h"
#endif //_ITF_INPUTADAPTER_DINPUT_H_

#ifndef ITF_SYSTEMADAPTER_WIN_H_
#include "adapters/SystemAdapter_win/SystemAdapter_win.h"
#endif

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_
#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_



#include <algorithm>
#include <algorithm>
#include <array>
#include <cassert>
namespace ITF
{
#ifdef DX_KEYBOARD
    i32 DInputToITF(u32 _key);
    bbool isSystemKey(u32 _key);
#endif // DX_KEYBOARD
    const f32 Joy::INPUT_DEADZONE = (0.24f * FLOAT(0x7FFF));

    LPDIRECTINPUT8 pDevice = NULL;
    HWND hDlg = NULL;

    /// Constructor.
    Joy::Joy()
    {
        m_id = -1;
        m_joyDevice = 0;
        ZeroMemory(&m_axes, sizeof(float) * JOY_MAX_AXES);
        ZeroMemory(&m_buttons, sizeof(InputAdapter::PressStatus) * JOY_MAX_BUT);
        ZeroMemory(m_360_Controllers, sizeof( CONTROLER_360_STATE ) * Joy::MAX_PAD);
    }

    /// Destructor.
    Joy::~Joy()
    {
        /// Unacquire the device one last time just in case
        /// the app tried to exit while the device is still acquired.
        if (m_joyDevice)
            m_joyDevice->Unacquire();

        /// Release any DirectInput objects.
        SF_RELEASE(m_joyDevice);
    }

    // Update Direct Input devices
    HRESULT Joy::UpdateInputState()
    {
        HRESULT hr;

        if (NULL == m_joyDevice)
            return S_OK;

        /// Poll the device to read the current state
        hr = m_joyDevice->Poll();
        if (FAILED(hr))
        {
            hr = m_joyDevice->Acquire();
            while (hr == DIERR_INPUTLOST)
                hr = m_joyDevice->Acquire();

            return S_OK;
        }

        /// Get the input's device state
        DIJOYSTATE2 joyState;
        if (FAILED(hr = m_joyDevice->GetDeviceState( sizeof( DIJOYSTATE2 ), &joyState )))
            return hr; // The device should have been acquired during the Poll()

        /// get DirectX axes.
        m_axes[0] = ((f32)joyState.lX) / 1000.f;
        m_axes[1] = (((f32)joyState.lY) / 1000.f) * -1;
        m_axes[2] = ((f32)joyState.lZ) / 1000.f;
        m_axes[3] = ((f32)joyState.lRx) / 1000.f;
        m_axes[4] = ((f32)joyState.lRy) / 1000.f;
        m_axes[5] = ((f32)joyState.lRz) / 1000.f;

        InputAdapter::PressStatus temp_buttons[JOY_MAX_BUT];

        ITF_Memcpy(temp_buttons, m_buttons, sizeof(temp_buttons));

        /// get DirectX button.
        for (u32 i = 0; i < JOY_MAX_BUT; i++)
        {
            if (joyState.rgbButtons[i] & 0x80)
            {
                m_buttons[i] = InputAdapter::Pressed;
            }
            else
            {
                m_buttons[i] = InputAdapter::Released;
            }
        }

        //special case for dpad
        switch (joyState.rgdwPOV[0])
        {
        case 0:
            m_buttons[m_joyButton_DPadU] = InputAdapter::Pressed;
            break;
        case 4500: //up right direction, patch 2
            m_buttons[m_joyButton_DPadU] = InputAdapter::Pressed;
            m_buttons[m_joyButton_DPadR] = InputAdapter::Pressed;
            break;
        case 9000:
            m_buttons[m_joyButton_DPadR] = InputAdapter::Pressed;
            break;
        case 13500: //down right direction, patch 2
            m_buttons[m_joyButton_DPadD] = InputAdapter::Pressed;
            m_buttons[m_joyButton_DPadR] = InputAdapter::Pressed;
            break;
        case 18000:
            m_buttons[m_joyButton_DPadD] = InputAdapter::Pressed;
            break;
        case 22500: //down left direction, patch 2
            m_buttons[m_joyButton_DPadD] = InputAdapter::Pressed;
            m_buttons[m_joyButton_DPadL] = InputAdapter::Pressed;
            break;
        case 27000:
            m_buttons[m_joyButton_DPadL] = InputAdapter::Pressed;
            break;
        case 31500: //up left direction, patch 2
            m_buttons[m_joyButton_DPadU] = InputAdapter::Pressed;
            m_buttons[m_joyButton_DPadL] = InputAdapter::Pressed;
            break;
        default:
            m_buttons[m_joyButton_DPadL] = InputAdapter::Released;
            m_buttons[m_joyButton_DPadR] = InputAdapter::Released;
            m_buttons[m_joyButton_DPadU] = InputAdapter::Released;
            m_buttons[m_joyButton_DPadD] = InputAdapter::Released;
            break;
        }

        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            if (temp_buttons[i] != InputAdapter::JustPressed && temp_buttons[i] != InputAdapter::Pressed && m_buttons[i]
                == InputAdapter::Pressed)
                m_buttons[i] = InputAdapter::JustPressed;
            else if (temp_buttons[i] != InputAdapter::JustReleased && temp_buttons[i] != InputAdapter::Released &&
                m_buttons[i] == InputAdapter::Released)
                m_buttons[i] = InputAdapter::JustReleased;
        }

        return S_OK;
    }

    bool Joy::isConnected(u32 _numPad) const
    {
        return m_360_Controllers[_numPad].connected;
    }

    // Update Xinput devices
    HRESULT Joy::UpdateInputState360(u32 _numPad)
    {
        typedef struct Mapping
        {
            int m_XBoxButtonID, m_ITFButtonID;
        } Mapping;

        static Mapping mapping[] =
        {
            {XINPUT_GAMEPAD_DPAD_UP, m_joyButton_DPadU},
            {XINPUT_GAMEPAD_DPAD_DOWN, m_joyButton_DPadD},
            {XINPUT_GAMEPAD_DPAD_RIGHT, m_joyButton_DPadR},
            {XINPUT_GAMEPAD_DPAD_LEFT, m_joyButton_DPadL},
            {XINPUT_GAMEPAD_A, m_joyButton_A},
            {XINPUT_GAMEPAD_B, m_joyButton_B},
            {XINPUT_GAMEPAD_X, m_joyButton_X},
            {XINPUT_GAMEPAD_Y, m_joyButton_Y},
            {XINPUT_GAMEPAD_START, m_joyButton_Start},
            {XINPUT_GAMEPAD_BACK, m_joyButton_Back},
            {XINPUT_GAMEPAD_LEFT_SHOULDER, m_joyButton_LB},
            {XINPUT_GAMEPAD_RIGHT_SHOULDER, m_joyButton_RB},
            {XINPUT_GAMEPAD_LEFT_THUMB, m_joyButton_ThumbLeft},
            {XINPUT_GAMEPAD_RIGHT_THUMB, m_joyButton_ThumbRight}
        };
        u32 mappingElemsCount = sizeof(mapping) / sizeof(mapping[0]);

        InputAdapter::PressStatus temp_buttons[JOY_MAX_BUT];
        ITF_Memcpy(temp_buttons, m_buttons, sizeof(temp_buttons));

        DWORD dwResult = XInputGetState(_numPad, &m_360_Controllers[_numPad].state);
        if (dwResult == ERROR_SUCCESS)
        {
            WORD wButtons = m_360_Controllers[_numPad].state.Gamepad.wButtons;

            m_axes[0] = ((f32)m_360_Controllers[_numPad].state.Gamepad.sThumbLX) / 32767.f;
            m_axes[1] = (((f32)m_360_Controllers[_numPad].state.Gamepad.sThumbLY) / 32767.f);

            m_axes[2] = m_360_Controllers[_numPad].state.Gamepad.bLeftTrigger;

            m_axes[3] = ((f32)m_360_Controllers[_numPad].state.Gamepad.sThumbRX) / 32767.f;
            m_axes[4] = ((f32)m_360_Controllers[_numPad].state.Gamepad.sThumbRY) / 32767.f;

            m_axes[5] = m_360_Controllers[_numPad].state.Gamepad.bRightTrigger;

            if ((m_360_Controllers[_numPad].state.Gamepad.sThumbLX < INPUT_DEADZONE &&
                    m_360_Controllers[_numPad].state.Gamepad.sThumbLX > -INPUT_DEADZONE) &&
                (m_360_Controllers[_numPad].state.Gamepad.sThumbLY < INPUT_DEADZONE &&
                    m_360_Controllers[_numPad].state.Gamepad.sThumbLY > -INPUT_DEADZONE))
            {
                m_axes[0] = 0;
                m_axes[1] = 0;
            }

            if ((m_360_Controllers[_numPad].state.Gamepad.sThumbRX < INPUT_DEADZONE &&
                    m_360_Controllers[_numPad].state.Gamepad.sThumbRX > -INPUT_DEADZONE) &&
                (m_360_Controllers[_numPad].state.Gamepad.sThumbRY < INPUT_DEADZONE &&
                    m_360_Controllers[_numPad].state.Gamepad.sThumbRY > -INPUT_DEADZONE))
            {
                m_axes[3] = 0;
                m_axes[4] = 0;
            }

            for (u32 i = 0; i < mappingElemsCount; i++)
            {
                if (wButtons & mapping[i].m_XBoxButtonID)
                    m_buttons[mapping[i].m_ITFButtonID] = InputAdapter::Pressed;
                else
                    m_buttons[mapping[i].m_ITFButtonID] = InputAdapter::Released;
            }

            m_buttons[m_joyButton_TriggerLeft] = (m_axes[m_joyTrigger_Left] >= 128.0f)
                                                     ? InputAdapter::Pressed
                                                     : InputAdapter::Released;;
            m_buttons[m_joyButton_TriggerRight] = (m_axes[m_joyTrigger_Right] >= 128.0f)
                                                      ? InputAdapter::Pressed
                                                      : InputAdapter::Released;;


            if (!m_360_Controllers[_numPad].connected)
                m_360_Controllers[_numPad].connected = true;

            // rumble
            if (m_360_Controllers[_numPad].rumble.motorOn)
            {
                if (m_360_Controllers[_numPad].rumble.startTime + m_360_Controllers[_numPad].rumble.duringTime
                    < SYSTEM_ADAPTER->getTime())
                {
                    padVibration360(_numPad, 0.0f, 0.0f);
                }
            }
        }
        else // not connected
            m_360_Controllers[_numPad].connected = false;

        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            if (temp_buttons[i] != InputAdapter::JustPressed && temp_buttons[i] != InputAdapter::Pressed && m_buttons[i]
                == InputAdapter::Pressed)
                m_buttons[i] = InputAdapter::JustPressed;
            else if (temp_buttons[i] != InputAdapter::JustReleased && temp_buttons[i] != InputAdapter::Released &&
                m_buttons[i] == InputAdapter::Released)
                m_buttons[i] = InputAdapter::JustReleased;
        }

        return S_OK;
    }

    float Joy::getAxe(u32 _axe) const
    {
        ITF_ASSERT(_axe < JOY_MAX_AXES);
        return m_axes[_axe];
    }

    InputAdapter::PressStatus Joy::getButton(u32 _button) const
    {
        ITF_ASSERT(_button < JOY_MAX_BUT);
        return m_buttons[_button];
    }

    // The speed given is between 0-1
    // For 360 pads, the left motor handles low-frequency vibration and the right motor handles high-frequency vibration
    void Joy::padVibration360(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time)
    {
        DWORD dwResult = XInputGetState(_numPad, &m_360_Controllers[_numPad].state);
        if (dwResult == ERROR_SUCCESS)
        {
            bbool StartMotor = (_leftMotorSpeed + _rightMotorSpeed) > 0.0f;

            if (m_360_Controllers[_numPad].rumble.motorOn != StartMotor)
            {
                m_360_Controllers[_numPad].rumble.motorOn = StartMotor;

                // MotorSpeed: 0-65535
                WORD myLeftMotorSpeed = (WORD)(_leftMotorSpeed * 65535.0f);
                WORD myRightMotorSpeed = (WORD)(_rightMotorSpeed * 65535.0f);

                m_360_Controllers[_numPad].vibration.wLeftMotorSpeed = myLeftMotorSpeed;
                m_360_Controllers[_numPad].vibration.wRightMotorSpeed = myRightMotorSpeed;

                XInputSetState(_numPad, &m_360_Controllers[_numPad].vibration);
            }

            if (StartMotor)
            {
                m_360_Controllers[_numPad].rumble.duringTime = _time;
                m_360_Controllers[_numPad].rumble.startTime = SYSTEM_ADAPTER->getTime();
            }
        }
    }

    //----------------------------------------------------------------------------//



    int DINPUT_Key_ToITF(int _key)
    {
        switch (_key)
        {
        case VK_CLEAR: return KEY_DEL;
            break;
        case VK_RETURN: return KEY_ENTER;
            break;
        case VK_SHIFT: return KEY_SPECIAL;
            break;
        case VK_CONTROL: return KEY_SPECIAL;
            break;

        case VK_ESCAPE: return KEY_ESC;
            break;

        case VK_TAB: return KEY_TAB;
            break;
        case VK_SPACE: return KEY_SPACE;
            break;
        case VK_PRIOR: return KEY_PAGEUP;
            break;
        case VK_NEXT: return KEY_PAGEDOWN;
            break;
        case VK_END: return KEY_END;
            break;
        case VK_HOME: return KEY_HOME;
            break;
        case VK_LEFT: return KEY_LEFT;
            break;
        case VK_UP: return KEY_UP;
            break;
        case VK_RIGHT: return KEY_RIGHT;
            break;
        case VK_DOWN: return KEY_DOWN;
            break;
        case VK_BACK: return KEY_BACKSPACE;
            break;
        case VK_PRINT: return KEY_PRINT;
            break;
        /*
                    case VK_SELECT: return KEY_SPECIAL; break;
                    case VK_PRINT: return KEY_SPECIAL; break;
                    case VK_EXECUTE: return KEY_SPECIAL; break;
                    case VK_SNAPSHOT: return KEY_SPECIAL; break;
                    case VK_INSERT: return KEY_SPECIAL; break;
                    case VK_DELETE: return KEY_SPECIAL; break;
                    case VK_HELP: return KEY_SPECIAL; break;

                    case VK_LWIN: return KEY_SPECIAL; break;
                    case VK_RWIN: return KEY_SPECIAL; break;
                    case VK_APPS: return KEY_SPECIAL; break;

                    case VK_SLEEP: return KEY_SPECIAL; break;
        */
        case VK_INSERT: return KEY_INSERT;
            break;

        case VK_NUMPAD0: return KEY_KP_0;
            break;
        case VK_NUMPAD1: return KEY_KP_1;
            break;
        case VK_NUMPAD2: return KEY_KP_2;
            break;
        case VK_NUMPAD3: return KEY_KP_3;
            break;
        case VK_NUMPAD4: return KEY_KP_4;
            break;
        case VK_NUMPAD5: return KEY_KP_5;
            break;
        case VK_NUMPAD6: return KEY_KP_6;
            break;
        case VK_NUMPAD7: return KEY_KP_7;
            break;
        case VK_NUMPAD8: return KEY_KP_8;
            break;
        case VK_NUMPAD9: return KEY_KP_9;
            break;
        case VK_MULTIPLY: return KEY_KP_MULTIPLY;
            break;
        case VK_ADD: return KEY_KP_ADD;
            break;
        case VK_DELETE: return KEY_DEL;
            break;

        case VK_SEPARATOR: return KEY_KP_EQUAL;
            break;

        case VK_SUBTRACT: return KEY_KP_SUBTRACT;
            break;
        case VK_DECIMAL: return KEY_KP_DECIMAL;
            break;
        case VK_DIVIDE: return KEY_KP_DIVIDE;
            break;

        case VK_F1: return KEY_F1;
            break;
        case VK_F2: return KEY_F2;
            break;
        case VK_F3: return KEY_F3;
            break;
        case VK_F4: return KEY_F4;
            break;
        case VK_F5: return KEY_F5;
            break;
        case VK_F6: return KEY_F6;
            break;
        case VK_F7: return KEY_F7;
            break;
        case VK_F8: return KEY_F8;
            break;
        case VK_F9: return KEY_F9;
            break;
        case VK_F10: return KEY_F10;
            break;
        case VK_F11: return KEY_F11;
            break;
        case VK_F12: return KEY_F12;
            break;
        case VK_F13: return KEY_F13;
            break;
        case VK_F14: return KEY_F14;
            break;
        case VK_F15: return KEY_F15;
            break;
        case VK_F16: return KEY_F16;
            break;
        case VK_F17: return KEY_F17;
            break;
        case VK_F18: return KEY_F18;
            break;
        case VK_F19: return KEY_F19;
            break;
        case VK_F20: return KEY_F20;
            break;
        case VK_F21: return KEY_F21;
            break;
        case VK_F22: return KEY_F22;
            break;
        case VK_F23: return KEY_F23;
            break;
        case VK_F24: return KEY_F24;
            break;

        case VK_LSHIFT: return KEY_LSHIFT;
            break;
        case VK_RSHIFT: return KEY_RSHIFT;
            break;
        case VK_LCONTROL: return KEY_LCTRL;
            break;
        case VK_RCONTROL: return KEY_RCTRL;
            break;
        case VK_LMENU: return KEY_SPECIAL;
            break;
        case VK_RMENU: return KEY_SPECIAL;
            break;
        }

        return _key;
    }

    /// Constructor.
    InputAdapter_DINPUT::InputAdapter_DINPUT()
    {
#if 0
        glfwEnable(GLFW_MOUSE_CURSOR);
        glfwEnable(GLFW_KEY_REPEAT);
#endif
#ifdef DX_KEYBOARD
        DXinput.m_pDIKeyboardDevice = NULL;
#endif //DX_KEYBOARD

        auto adapter = static_cast<SystemAdapter_win*>(SYSTEM_ADAPTER);
        adapter->SetCallbackKeyboard(KeyCB);
        adapter->SetCallbackMousePos(MousePosCB);
        adapter->SetCallbackMouseWheel(MouseWheelCB);
        adapter->SetCallbackMouseButton(MouseButtonCB);

        DXinput.InitDirectInput(adapter->m_hwnd);
        for (u32 i = 0; i < DXinput.m_numberpad; i++)
        {
            LOG("[INPUT] Constructor: Device %d, Type=%d, HasDevice=%s", i, (int)DXinput.m_pad[i].m_typePad,
                DXinput.m_pad[i].m_joy.getDevice() ? "YES" : "NO");
            if (IsDirectInput(DXinput.m_pad[i].m_typePad))
            {
                if (DXinput.m_pad[i].m_joy.getDevice())
                {
                    HRESULT hr = DXinput.m_pad[i].m_joy.getDevice()->Acquire();
                    LOG("[INPUT] Acquire device %d result: 0x%08X (%s)",
                        i, hr, SUCCEEDED(hr) ? "SUCCESS" : "FAILED");
                }
                else
                {
                    LOG("[INPUT] ERROR: Cannot acquire device %d - no DirectInput interface!", i);
                }
            }
        }

        setPadConnected(0, btrue);
        InitializeActionStrings();
        memset(m_connectedPlayers, 0, JOY_MAX_COUNT * sizeof(PlayerState));
        m_connectedPlayers[0] = ePlaying;
        InputAdapter::LoadPlayerControlSettings();
#ifdef USE_WIIMOTE_LIB
        if(CONFIG->m_enableWiiRemoteonPC)
		    KPinput.InitKPadInput();
#endif //USE_WIIMOTE_LIB
    }

    /// Destructor.
    InputAdapter_DINPUT::~InputAdapter_DINPUT()
    {
#ifdef DX_KEYBOARD
        if(DXinput.m_pDIKeyboardDevice != NULL)
        {
            DXinput.m_pDIKeyboardDevice->Unacquire();
            DXinput.m_pDIKeyboardDevice->Release();
            DXinput.m_pDIKeyboardDevice = NULL;
        }
#endif //DX_KEYBOARD
    }

    // The speed given is between 0-1
    void InputAdapter_DINPUT::padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        Joy* joy = &DXinput.m_pad[_numPad].m_joy;
        if (DXinput.m_pad[_numPad].m_typePad == DXinput.CONTROLLER_TYPE::XINPUT)
            joy->padVibration360(_numPad, _leftMotorSpeed, _rightMotorSpeed);
    }

    u32 InputAdapter_DINPUT::getConnectedPadIndex(u32 _padIndex) const
    {
        u32 padIndex = 0;
        u32 activeCount = 0;
        for (; padIndex < Joy::MAX_PAD; padIndex++)
        {
            if (isPadConnected(padIndex))
            {
                if (activeCount == _padIndex)
                    break;
                activeCount++;
            }
        }
        return padIndex;
    }

    u32 InputAdapter_DINPUT::getGamePadCount()
    {
        return DXinput.m_numberpad;
    }

    void InputAdapter_DINPUT::UpdatePads()
    {
        DXinput.RefreshControllerList(this);
        DXinput.DetectControllerChanges(this);

        Joy* joy;
        for (u32 i = 0; i < DXinput.m_numberpad; i++)
        {
            joy = &DXinput.m_pad[i].m_joy;

            if (DXinput.m_pad[i].m_typePad == DXinput.CONTROLLER_TYPE::XINPUT)
            {
                joy->UpdateInputState360(i);
            }
            else if (IsDirectInput(DXinput.m_pad[i].m_typePad))
            {
                if (joy->getDevice())
                {
                    HRESULT hr = joy->UpdateInputState();
                    if (FAILED(hr))
                    {
                        // Device disconnected, handle gracefully
                        joy->getDevice()->Unacquire();
                        continue;
                    }
                }
            }
        }
    }





    InputAdapter::PressStatus InputAdapter_DINPUT::GetButtonStatus(InputValue inputValue)
    {
        return DXinput.m_pad[inputValue.inputIndex].m_joy.getButton(inputValue.inputValue);
    }

    float InputAdapter_DINPUT::GetAxe(InputValue inputValue)
    {
        return DXinput.m_pad[inputValue.inputIndex].m_joy.getAxe(inputValue.inputValue);
    }

    bbool InputAdapter_DINPUT::IsButtonPressed(InputValue inputValue)
    {
        auto state = DXinput.m_pad[inputValue.inputIndex].m_joy.getButton(inputValue.inputValue);
        return state == JustPressed || state == Pressed;
    }

    void InputAdapter_DINPUT::updateAllInputState()
    {
        InputAdapter::updateAllInputState();
#ifdef DX_KEYBOARD
        HRESULT res = DXinput.m_pDIKeyboardDevice->GetDeviceState(sizeof(DXinput.m_KeyBuffer), (LPVOID)&DXinput.m_KeyBuffer);
        ITF_ASSERT(!FAILED(res));
#endif // DX_KEYBOARD

#ifdef USE_WIIMOTE_LIB
        if(CONFIG->m_enableWiiRemoteonPC)
        {
		    KPinput.UpdateInputStateWiiMote();
            for (u32 i = 0; i < Joy::MAX_PAD; i++)
            {
                if (KPinput.m_pads[i].isConnected() && KPinput.m_pads[i].isActive())
                {
                    if (KPinput.m_pads[i].isNunchuckPlugged())
                        setPadType(i, Pad_WiiNunchuk);
                    else if (KPinput.m_pads[i].isClassicPlugged())
                        setPadType(i, Pad_WiiClassic);
                    else
                        setPadType(i, Pad_WiiSideWay);
                }
                else
                    setPadType(i, Pad_X360);
            }
        }
        else
        {
            for (u32 i = 0; i < Joy::MAX_PAD; i++)
                setPadType(i, Pad_X360);
        }
#endif //USE_WIIMOTE_LIB
    }

    void InputAdapter_DINPUT::getPadAcc(u32 _numPad, Vec3d* _acc)
    {
#ifdef USE_WIIMOTE_LIB
        if(CONFIG->m_enableWiiRemoteonPC && _numPad < Joy::MAX_PAD)
        {
            _acc->m_x = KPinput.m_pads[_numPad].remote.Acceleration.Orientation.X;
            _acc->m_y = KPinput.m_pads[_numPad].remote.Acceleration.Orientation.Y;
            _acc->m_z = KPinput.m_pads[_numPad].remote.Acceleration.Orientation.Z;
        }
#endif //USE_WIIMOTE_LIB
    }

    bbool InputAdapter_DINPUT::IsDirectInput(DirectInput::CONTROLLER_TYPE::Type controllerType)
    {
        return controllerType == DirectInput::CONTROLLER_TYPE::DINPUT ||
            controllerType == DirectInput::CONTROLLER_TYPE::PS5_CONTROLLER ||
            controllerType == DirectInput::CONTROLLER_TYPE::PS4_CONTROLLER ||
            controllerType == DirectInput::CONTROLLER_TYPE::GENERIC_CONTROLLER;
    }

    //----------------------------------------------------------------------------//

    HRESULT DirectInput::InitDirectInput(HWND _hDlg)
    {
        hDlg = _hDlg;
        HRESULT hr;
        m_directInput = NULL;

        if (!CONFIG->m_XInputDevicesOnly)
        {
            if (FAILED(hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION,
                IID_IDirectInput8, ( VOID** )&m_directInput, NULL )))
                return hr;
        }
        if (mb_FilterOutXinputDevices)
            SetupForIsXInputDevice();

        if (!CONFIG->m_XInputDevicesOnly)
        {
            if (FAILED(hr = m_directInput->EnumDevices( DI8DEVCLASS_GAMECTRL,
                EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY )))
                return hr;
        }
        else
        {
            for (u32 i = 0; i < Joy::MAX_PAD; i++)
            {
                m_pad[m_numberpad].m_typePad = CONTROLLER_TYPE::XINPUT;
                m_numberpad++;
            }
        }

        if (mb_FilterOutXinputDevices)
            CleanupForIsXInputDevice();

        if (!m_numberpad)
        {
            /*
            MessageBox( NULL, TEXT( "Joystick not found." ),
                        TEXT( "DirectInput" ),
                        MB_ICONERROR | MB_OK );*/
            EndDialog(hDlg, 0);
            return S_OK;
        }

        for (u32 i = 0; i < m_numberpad; i++)
        {
            LOG("[INPUT] Setting up device %d: Type=%d, HasDevice=%s",
                i, (int)m_pad[i].m_typePad,
                m_pad[i].m_joy.getDevice() ? "YES" : "NO");
            if (InputAdapter_DINPUT::IsDirectInput(m_pad[i].m_typePad))
            {
                if (!m_pad[i].m_joy.getDevice())
                {
                    LOG("[INPUT] ERROR: Device %d has no DirectInput interface!", i);
                    continue;
                }

                /// Set the data format to "simple joystick" - a predefined data format
                ///
                /// A data format specifies which controls on a device we are interested in,
                /// and how they should be reported. This tells DInput that we will be
                /// passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
                if (FAILED(hr = m_pad[i].m_joy.getDevice()->SetDataFormat( &c_dfDIJoystick2 )))
                {
                    LOG("[INPUT] ERROR: SetDataFormat failed for device %d: 0x%08X", i, hr);
                    continue;
                }

                /// Set the cooperative level to let DInput know how this device should
                /// interact with the system and with other DInput applications.
                if (FAILED(hr = m_pad[i].m_joy.getDevice()->SetCooperativeLevel( hDlg, DISCL_NONEXCLUSIVE |
                    DISCL_BACKGROUND )))
                {
                    LOG("[INPUT] ERROR: SetCooperativeLevel failed for device %d: 0x%08X", i, hr);
                    continue;
                }

                /// Enumerate the joystick objects. The callback function enabled user
                /// interface elements for objects that are found, and sets the min/max
                /// values property for discovered axes.
                if (FAILED(hr = m_pad[i].m_joy.getDevice()->EnumObjects( EnumObjectsCallback,
                    ( VOID* )&m_pad[i].m_joy, DIDFT_ALL )))
                {
                    LOG("[INPUT] ERROR: EnumObjects failed for device %d: 0x%08X", i, hr);
                    continue;
                }

                LOG("[INPUT] SUCCESS: Device %d setup completed", i);
            }
        }

#ifdef DX_KEYBOARD
        InitKeyboard();
#endif //DX_KEYBOARD

        return S_OK;
    }

    // Constructor.
    DirectInput::DirectInput()
    {
        /// Set to true for skip XBOX 360 Pad.
        mb_FilterOutXinputDevices = bfalse; // Changed to support all controllers
        mp_XInputDeviceList = NULL;
        m_numberpad = 0;
        m_directInput = NULL;
        m_Instance = GUID_NULL;
        m_lastScanTime = 0;
        m_allowHotSwap = btrue;
#ifdef DX_KEYBOARD
        memset(m_KeyBuffer, 0, m_keyCount);
#endif // DX_KEYBOARD

#ifndef USE_WIIMOTE_LIB
    // STB: commented as not compiling, deactivated by not defining USE_WIIMOTE_LIB
#endif //USE_WIIMOTE_LIB
        // Initialize controller tracking
        for (u32 i = 0; i < Joy::MAX_PAD; i++)
        {
            m_pad[i].m_wasConnected = bfalse;
            m_pad[i].m_lastActiveTime = 0;
            m_pad[i].m_deviceGUID = GUID_NULL;
        }
    }

    DirectInput::~DirectInput()
    {
        FreeDirectInput();
    }

    void DirectInput::FreeDirectInput()
    {
        SF_RELEASE(m_directInput);
    }

    void DirectInput::CleanupForIsXInputDevice()
    {
        // Cleanup linked list
        XINPUT_DEVICE_NODE* pNode = mp_XInputDeviceList;
        while (pNode)
        {
            XINPUT_DEVICE_NODE* pDelete = pNode;
            pNode = pNode->pNext;
            SF_DEL(pDelete);
        }
    }

    ///-----------------------------------------------------------------------------
    /// Enum each PNP device using WMI and check each device ID to see if it contains
    /// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it’s an XInput device
    /// Unfortunately this information can not be found by just using DirectInput.
    /// Checking against a VID/PID of 0x028E/0x045E won't find 3rd party or future
    /// XInput devices.
    ///
    /// This function stores the list of xinput devices in a linked list
    /// at g_pXInputDeviceList, and IsXInputDevice() searchs that linked list
    ///-----------------------------------------------------------------------------
    HRESULT DirectInput::SetupForIsXInputDevice()
    {
        IWbemServices* pIWbemServices = NULL;
        IEnumWbemClassObject* pEnumDevices = NULL;
        IWbemLocator* pIWbemLocator = NULL;
        IWbemClassObject* pDevices[20] = {0};
        BSTR bstrDeviceID = NULL;
        BSTR bstrClassName = NULL;
        BSTR bstrNamespace = NULL;
        DWORD uReturned = 0;
        bool bCleanupCOM = false;
        UINT iDevice = 0;
        VARIANT var;
        HRESULT hr;

        // CoInit if needed
        hr = CoInitialize(NULL);
        bCleanupCOM = SUCCEEDED(hr);

        // Create WMI
        hr = CoCreateInstance(__uuidof( WbemLocator),
                              NULL,
                              CLSCTX_INPROC_SERVER,
                              __uuidof( IWbemLocator),
                              (LPVOID*)&pIWbemLocator);
        if (FAILED(hr) || pIWbemLocator == NULL)
            goto LCleanup;

        // Create BSTRs for WMI
        bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");
        if (bstrNamespace == NULL) goto LCleanup;
        bstrDeviceID = SysAllocString(L"DeviceID");
        if (bstrDeviceID == NULL) goto LCleanup;
        bstrClassName = SysAllocString(L"Win32_PNPEntity");
        if (bstrClassName == NULL) goto LCleanup;

        // Connect to WMI
        hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
                                          0L, NULL, NULL, &pIWbemServices);
        if (FAILED(hr) || pIWbemServices == NULL)
            goto LCleanup;

        // Switch security level to IMPERSONATE
        CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0);

        // Get list of Win32_PNPEntity devices
        hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
        if (FAILED(hr) || pEnumDevices == NULL)
            goto LCleanup;

        // Loop over all devices
        for (; ;)
        {
            // Get 20 at a time
            hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
            if (FAILED(hr))
                goto LCleanup;
            if (uReturned == 0)
                break;

            for (iDevice = 0; iDevice < uReturned; iDevice++)
            {
                // For each device, get its device ID
                hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
                if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
                {
                    // Check if the device ID contains "IG_".  If it does, then it’s an XInput device
                    // Unfortunately this information can not be found by just using DirectInput
                    if (wcsstr(var.bstrVal, L"IG_"))
                    {
                        // If it does, then get the VID/PID from var.bstrVal
                        DWORD dwPid = 0, dwVid = 0;
                        WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
                        if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
                            dwVid = 0;
                        WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
                        if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
                            dwPid = 0;

                        DWORD dwVidPid = MAKELONG(dwVid, dwPid);

                        // Add the VID/PID to a linked list
                        XINPUT_DEVICE_NODE* pNewNode = newAlloc(mId_System, XINPUT_DEVICE_NODE);
                        if (pNewNode)
                        {
                            pNewNode->dwVidPid = dwVidPid;
                            pNewNode->pNext = mp_XInputDeviceList;
                            mp_XInputDeviceList = pNewNode;
                        }
                    }
                }
                SF_RELEASE(pDevices[iDevice]);
            }
        }

    LCleanup:
        if (bstrNamespace)
            SysFreeString(bstrNamespace);
        if (bstrDeviceID)
            SysFreeString(bstrDeviceID);
        if (bstrClassName)
            SysFreeString(bstrClassName);
        for (iDevice = 0; iDevice < 20; iDevice++)
            SF_RELEASE(pDevices[iDevice]);
        SF_RELEASE(pEnumDevices);
        SF_RELEASE(pIWbemLocator);
        SF_RELEASE(pIWbemServices);

        return hr;
    }

    bool DirectInput::IsXInputDevice(const GUID* pGuidProductFromDirectInput)
    {
        // Check each xinput device to see if this device's vid/pid matches
        XINPUT_DEVICE_NODE* pNode = mp_XInputDeviceList;
        while (pNode)
        {
            if (pNode->dwVidPid == pGuidProductFromDirectInput->Data1)
                return true;
            pNode = pNode->pNext;
        }
        return false;
    }

    BOOL CALLBACK DirectInput::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, /*VOID* pContext ,*/
                                                     LPVOID pvRef)
    {
        HRESULT hr;
        DirectInput* input = (DirectInput*)pvRef;
        pDevice = input->m_directInput;

        for (u32 i = 0; i < input->m_numberpad; i++)
        {
            if (memcmp(&input->m_pad[i].m_deviceGUID, &pdidInstance->guidInstance, sizeof(GUID)) == 0)
            {
                return DIENUM_CONTINUE;
            }
        }
        if (input->m_numberpad >= Joy::MAX_PAD)
            return DIENUM_STOP;

        /// Set pad/joy id.
        Joy* joy = &input->m_pad[input->m_numberpad].m_joy;
        joy->setId(input->m_numberpad);
        input->m_pad[input->m_numberpad].m_deviceGUID = pdidInstance->guidInstance;

        if (CONFIG->m_XInputDevicesOnly)
        {
            //simplified path for xinput devices

            //XInput devices are not enumerated here
            if (input->IsXInputDevice(&pdidInstance->guidProduct))
            {
                return DIENUM_CONTINUE;
            }
            if (input->mb_FilterOutXinputDevices)
            {
                return DIENUM_CONTINUE;
            }
        }
        else
        {
            //Old code for Casa

            if (input->mb_FilterOutXinputDevices && input->IsXInputDevice(&pdidInstance->guidProduct))
            {
                input->m_pad[input->m_numberpad].m_typePad = CONTROLLER_TYPE::XINPUT;
                input->m_numberpad++;

                if (input->m_numberpad >= Joy::MAX_PAD)
                    return DIENUM_STOP;
                else
                    return DIENUM_CONTINUE;
            }
        }
        input->ClassifyController(input->m_numberpad, pdidInstance);

        // DEBUG: Log device info before CreateDevice
        LOG("[INPUT] Attempting CreateDevice for: %ls, VID/PID: %04X/%04X, Type: %d",
            pdidInstance->tszProductName,
            LOWORD(pdidInstance->guidProduct.Data1),
            HIWORD(pdidInstance->guidProduct.Data1),
            (int)input->m_pad[input->m_numberpad].m_typePad);

        // Obtain an interface to the enumerated joystick.
        hr = input->m_directInput->CreateDevice(pdidInstance->guidInstance, &joy->getRefDevice(), NULL);

        // DEBUG: Log CreateDevice result
        LOG("[INPUT] CreateDevice result: 0x%08X (%s) for device: %ls",
            hr,
            SUCCEEDED(hr) ? "SUCCESS" : "FAILED",
            pdidInstance->tszProductName);

        /// If it failed, then we can't use this joystick. (Maybe the user unplugged
        /// it while we were in the middle of enumerating it.)
        if (FAILED(hr))
        {
            LOG("[INPUT] ERROR: CreateDevice failed with HRESULT 0x%08X for %ls", hr, pdidInstance->tszProductName);
            delete joy;
            return DIENUM_CONTINUE;
        }

        input->m_numberpad++;

        if (input->m_numberpad >= Joy::MAX_PAD)
            return DIENUM_STOP;
        else
            return DIENUM_CONTINUE;
    }

    BOOL CALLBACK DirectInput::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
                                                   VOID* pContext)
    {
        Joy* joy = (Joy*)pContext;

        static int nSliderCount = 0; /// Number of returned slider controls
        static int nPOVCount = 0; /// Number of returned POV controls

        /// For axes that are returned, set the DIPROP_RANGE property for the
        /// enumerated axis in order to scale min/max values.
        if (pdidoi->dwType & DIDFT_AXIS)
        {
            DIPROPRANGE diprg;
            diprg.diph.dwSize = sizeof(DIPROPRANGE);
            diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
            diprg.diph.dwHow = DIPH_BYID;
            diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
            diprg.lMin = -1000;
            diprg.lMax = +1000;

            /// Set the range for the axis
            if (FAILED(joy->getDevice()->SetProperty( DIPROP_RANGE, &diprg.diph )))
                return DIENUM_STOP;
        }

        return DIENUM_CONTINUE;
    }

    void InputAdapter_DINPUT::setFocus()
    {
        InputAdapter::setFocus();
#ifdef DX_KEYBOARD
        // Update special keys status
        for (u32 keyID = 0; keyID < DirectInput::m_keyCount; keyID++)
        {
            //return;
        //    if (DXinput.m_prevKeyBuffer[keyID] != DXinput.m_KeyBuffer[keyID])
            {
                if (isSystemKey(keyID))
                {
                    if(DXinput.m_KeyBuffer[keyID] & 0x80)
                        INPUT_ADAPTER->onKey(DInputToITF(keyID), InputAdapter::Pressed);
          /*          else
                        INPUT_ADAPTER->onKey(DInputToITF(keyID), InputAdapter::Released);*/
                }
            }
        }
#endif //DX_KEYBOARD
    }

    void InputAdapter_DINPUT::unsetFocus()
    {
        InputAdapter::unsetFocus();
#ifdef DX_KEYBOARD
        // Reset keyboard keys
        for (i32 i = 0; i < KEY_COUNT; i++)
        {
            if(isKeyPressed(i))
            {
                LOG_OLIV("[InputAdapter] Unset focus : Force onKey %d '%c' Released",  DINPUT_Key_ToITF(i),  DINPUT_Key_ToITF(i));
                onKey(i, Released);
            }
        }

        // Reset mouse button
        {
            if(isMousePressed(MB_Left))
            {
                LOG_OLIV("[InputAdapter] Unset focus : Force onMouseButton MB_Left Released");
                onMouseButton(MB_Left, Released);
            }

            if(isMousePressed(MB_Right))
            {
                LOG_OLIV("[InputAdapter] Unset focus : Force onMouseButton MB_Right Released");
                onMouseButton(MB_Right, Released);
            }

            if(isMousePressed(MB_Middle))
            {
                LOG_OLIV("[InputAdapter] Unset focus : Force onMouseButton MB_Right Released");
                onMouseButton(MB_Middle, Released);
            }
        }
#ifdef DX_KEYBOARD
        memcpy(DXinput.m_prevKeyBuffer, DXinput.m_KeyBuffer, DirectInput::m_keyCount);
#endif //DX_KEYBOARD
#endif
    }
    //============================================================================
    // Enhanced Controller Detection and Hot-Swap Implementation
    //============================================================================
    void DirectInput::RefreshControllerList(InputAdapter* adapter)
    {
        if (!m_allowHotSwap) return;

        u32 currentTime = static_cast<u32>(GetTickCount());
        if (currentTime - m_lastScanTime < 500) return; // Scan every 500ms

        m_lastScanTime = currentTime;

        // Mark all pads as potentially disconnected
        for (u32 i = 0; i < m_numberpad; i++)
        {
            m_pad[i].m_wasConnected = m_pad[i].m_joy.isConnected(i);
        }

        // Re-enumerate devices
        if (m_directInput)
        {
            m_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
        }

        // Check for XInput controllers
        for (u32 i = 0; i < XUSER_MAX_COUNT; i++)
        {
            XINPUT_STATE state;
            bool connected = (XInputGetState(i, &state) == ERROR_SUCCESS);

            // Find corresponding pad slot
            u32 padIndex = i;
            if (padIndex < Joy::MAX_PAD)
            {
                if (connected && m_pad[padIndex].m_typePad != CONTROLLER_TYPE::XINPUT)
                {
                    HandleControllerConnection(padIndex, adapter);
                    m_pad[padIndex].m_typePad = CONTROLLER_TYPE::XINPUT;
                }
                else if (!connected && m_pad[padIndex].m_wasConnected)
                {
                    HandleControllerDisconnection(padIndex, adapter);
                }
            }
        }
    }

    bool DirectInput::DetectControllerChanges(InputAdapter* adapter)
    {
        bool changesDetected = false;

        for (u32 i = 0; i < Joy::MAX_PAD; i++)
        {
            bool currentlyConnected = false;

            if (m_pad[i].m_typePad == CONTROLLER_TYPE::XINPUT)
            {
                XINPUT_STATE state;
                currentlyConnected = (XInputGetState(i, &state) == ERROR_SUCCESS);
            }
            else if (m_pad[i].m_joy.getDevice())
            {
                HRESULT hr = m_pad[i].m_joy.getDevice()->Poll();
                currentlyConnected = SUCCEEDED(hr);
            }

            if (currentlyConnected != m_pad[i].m_wasConnected)
            {
                changesDetected = true;
                if (currentlyConnected)
                {
                    HandleControllerConnection(i, adapter);
                }
                else
                {
                    HandleControllerDisconnection(i, adapter);
                }
                m_pad[i].m_wasConnected = currentlyConnected;
            }
        }

        return changesDetected;
    }

    bool DirectInput::IsPS5Controller(const DIDEVICEINSTANCE* pdidInstance)
    {
        // PS5 DualSense Controller VID/PID
        const DWORD PS5_VID = 0x054C; // Sony
        const DWORD PS5_PID = 0x0CE6; // DualSense

        DWORD vid = LOWORD(pdidInstance->guidProduct.Data1);
        DWORD pid = HIWORD(pdidInstance->guidProduct.Data1);

        return (vid == PS5_VID && pid == PS5_PID);
    }

    bool DirectInput::IsPS4Controller(const DIDEVICEINSTANCE* pdidInstance)
    {
        // PS4 DualShock Controller VID/PIDs
        const DWORD PS4_VID = 0x054C; // Sony
        const DWORD PS4_PID_V1 = 0x05C4; // DualShock 4 v1
        const DWORD PS4_PID_V2 = 0x09CC; // DualShock 4 v2

        DWORD vid = LOWORD(pdidInstance->guidProduct.Data1);
        DWORD pid = HIWORD(pdidInstance->guidProduct.Data1);

        return (vid == PS4_VID && (pid == PS4_PID_V1 || pid == PS4_PID_V2));
    }

    void DirectInput::ClassifyController(u32 padIndex, const DIDEVICEINSTANCE* pdidInstance)
    {
        if (padIndex >= Joy::MAX_PAD) return;

        // Store device GUID for tracking
        m_pad[padIndex].m_deviceGUID = pdidInstance->guidInstance;

        if (IsXInputDevice(&pdidInstance->guidProduct))
        {
            m_pad[padIndex].m_typePad = CONTROLLER_TYPE::XINPUT;
        }
        else if (IsPS5Controller(pdidInstance))
        {
            m_pad[padIndex].m_typePad = CONTROLLER_TYPE::PS5_CONTROLLER;
        }
        else if (IsPS4Controller(pdidInstance))
        {
            m_pad[padIndex].m_typePad = CONTROLLER_TYPE::PS4_CONTROLLER;
        }
        else
        {
            m_pad[padIndex].m_typePad = CONTROLLER_TYPE::GENERIC_CONTROLLER;
        }
    }

    void DirectInput::HandleControllerDisconnection(u32 padIndex, InputAdapter* adapter)
    {
        if (padIndex >= Joy::MAX_PAD) return;

        // Clean up device
        if (m_pad[padIndex].m_joy.getDevice())
        {
            m_pad[padIndex].m_joy.getDevice()->Unacquire();
        }

        m_pad[padIndex].m_wasConnected = bfalse;
        m_pad[padIndex].m_lastActiveTime = GetTickCount();
        m_pad[padIndex].m_deviceGUID = GUID_NULL;

        // Call the virtual callback if adapter is provided
        if (adapter)
        {
            adapter->OnControllerDisconnected(padIndex);
        }
    }

    void DirectInput::HandleControllerConnection(u32 padIndex, InputAdapter* adapter)
    {
        if (padIndex >= Joy::MAX_PAD) return;

        m_pad[padIndex].m_wasConnected = btrue;
        m_pad[padIndex].m_lastActiveTime = GetTickCount();

        // Try to acquire the device if it's DirectInput
        if (InputAdapter_DINPUT::IsDirectInput(m_pad[padIndex].m_typePad))
        {
            if (m_pad[padIndex].m_joy.getDevice())
            {
                m_pad[padIndex].m_joy.getDevice()->Acquire();
            }
        }

        // Call the virtual callback if adapter is provided
        if (adapter)
        {
            adapter->OnControllerConnected(padIndex);
        }
    }

    void InputAdapter_DINPUT::HandleControllerStateChange()
    {
        // This method can be called by the TRC system when controller state needs to be checked
        u32 connectedCount = getGamePadCount();

        if (connectedCount == 0)
        {
            // No controllers connected - but keep player 0 available for keyboard input
            for (u32 i = 1; i < JOY_MAX_COUNT; ++i)
            {
                m_connectedPlayers[i] = eNotConnected;
                setPadConnected(i, bfalse);
            }
            // Always keep player 0 available for keyboard on PC
            m_connectedPlayers[0] = ePlaying;
            setPadConnected(0, bfalse); // No physical pad, but keyboard is available
        }
        else
        {
            // At least one controller connected - ensure player 0 is set to playing
            if (m_connectedPlayers[0] == eNotConnected && isPadConnected(0))
            {
                m_connectedPlayers[0] = ePlaying;
            }
        }
    }

    ControllerType InputAdapter_DINPUT::GetControllerType(InputValue& value)
    {
        if (value.inputType != Keyboard)
        {
            if (IsDirectInput(DXinput.m_pad[value.inputIndex].m_typePad))
            {
                return value.inputType == X360Button ? GenericButton : GenericAxis;
            }
        }
        return value.inputType;
    }

    const char* InputAdapter_DINPUT::GetControllerTypeName(u32 padIndex) const
    {
        if (padIndex >= Joy::MAX_PAD) return "Unknown";

        switch (DXinput.m_pad[padIndex].m_typePad)
        {
        case DXinput.CONTROLLER_TYPE::XINPUT:
            return "Xbox Controller";
        case DXinput.CONTROLLER_TYPE::PS5_CONTROLLER:
            return "PS5 DualSense";
        case DXinput.CONTROLLER_TYPE::PS4_CONTROLLER:
            return "PS4 DualShock";
        case DXinput.CONTROLLER_TYPE::GENERIC_CONTROLLER:
            return "Generic Controller";
        case DXinput.CONTROLLER_TYPE::DINPUT:
            return "DirectInput Controller";
        default:
            return "Unknown";
        }
    }

    void InputAdapter_DINPUT::startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        ITF_ASSERT(_numPad < Joy::MAX_PAD);

        if (_numPad < Joy::MAX_PAD)
        {
            Joy* pJoy = &DXinput.m_pad[_numPad].m_joy;
            if (pJoy) pJoy->padVibration360(_numPad, _leftMotorSpeed, _rightMotorSpeed, _time);
        }
    }

    void InputAdapter_DINPUT::stopRumble(u32 _numPad)
    {
        ITF_ASSERT(_numPad < Joy::MAX_PAD);
        if (_numPad < Joy::MAX_PAD)
        {
            Joy* pJoy = &DXinput.m_pad[_numPad].m_joy;
            if (pJoy) pJoy->padVibration360(_numPad, 0.0f, 0.0f);
        }
    }


#ifdef DX_KEYBOARD
    bool DirectInput::InitKeyboard()
    {
        if(FAILED(pDevice->CreateDevice(GUID_SysKeyboard,
                                            &m_pDIKeyboardDevice,
                                            NULL)))
         {
             MessageBox(NULL,
                        L"CreateDevice() failed!",
                        L"InitKeyboard()",
                        MB_OK);
             return false;
        }

        if(FAILED(m_pDIKeyboardDevice->SetDataFormat(&c_dfDIKeyboard)))
         {
             MessageBox(hDlg,
                        L"SetDataFormat() failed!",
                        L"InitKeyboard()",
                        MB_OK);
             return false;
         }

        if(FAILED(m_pDIKeyboardDevice->SetCooperativeLevel(hDlg,
                                                           DISCL_BACKGROUND |
                                                           DISCL_NONEXCLUSIVE)))
         {
             MessageBox(NULL,
                        L"SetCooperativeLevel() failed!",
                        L"InitKeyboard()",
                        MB_OK);
             return false;
         }

        if(FAILED(m_pDIKeyboardDevice->Acquire()))
         {
             MessageBox(NULL,
                        L"Acquire() failed!",
                        L"InitKeyboard()",
                        MB_OK);
             return false;
         }

        return true;
    }

    bool DirectInput::KeyPressed(int Key)
    {
        if(m_KeyBuffer[Key] & 0x80)
        {
            return true;
        }
        return false;
    }


bbool isSystemKey(u32 _key)
{
    switch (_key)
    {
        case DIK_ESCAPE:
        case DIK_BACK:
        case DIK_LCONTROL:
        case DIK_LSHIFT:
        case DIK_RSHIFT:
        case DIK_SPACE:
        case DIK_RCONTROL:
            return btrue;

        default:
            return bfalse;
    }
}

i32 DInputToITF(u32 _key)
{
    switch (_key)
    {
        case DIK_ESCAPE:
             return KEY_ESC;

        case DIK_1:
            return '1';

        case DIK_2:
            return '2';

        case DIK_3:
            return '3';

        case DIK_4:
            return '7';

        case DIK_5:
            return '5';

        case DIK_6:
            return '6';

        case DIK_7:
            return '7';

        case DIK_8:
            return '8';

        case DIK_9:
            return '9';

        case DIK_0:
            return '0';

        case DIK_MINUS:
            return KEY_KP_SUBTRACT;

        case DIK_EQUALS:
            return KEY_KP_EQUAL;

        case DIK_BACK:
            return KEY_BACKSPACE;

        case DIK_TAB:
            return KEY_TAB;

        case DIK_Q:
            return 'Q';

        case DIK_W:
            return 'W';

        case DIK_E:
            return 'E';

        case DIK_R:
            return 'R';

        case DIK_T:
            return 'T';

        case DIK_Y:
            return 'Y';

        case DIK_U:
            return 'U';

        case DIK_I:
            return 'I';

        case DIK_O:
            return 'O';

        case DIK_P:
            return 'P';

        case DIK_LBRACKET:
            return '[';

        case DIK_RBRACKET:
            return ']';

        case DIK_RETURN:
        case DIK_NUMPADENTER:
            return KEY_KP_ENTER;

        case DIK_LCONTROL:
            return KEY_LCTRL;

        case DIK_A:
            return 'A';

        case DIK_S:
            return 'S';

        case DIK_D:
            return 'D';

        case DIK_F:
            return 'F';

        case DIK_G:
            return 'G';

        case DIK_H:
            return 'H';

        case DIK_J:
            return 'J';

        case DIK_K:
            return 'K';

        case DIK_L:
            return 'L';

        case DIK_SEMICOLON:
            return ';';

        case DIK_APOSTROPHE:
            return '\'';

        case DIK_LSHIFT:
            return KEY_LSHIFT;

        case DIK_Z:
            return 'Z';

        case DIK_X:
            return 'X';

        case DIK_C:
            return 'C';

        case DIK_V:
            return 'V';

        case DIK_B:
            return 'B';

        case DIK_N:
            return 'N';

        case DIK_M:
            return 'M';

        case DIK_COMMA:
            return ',';

        case DIK_RSHIFT:
            return KEY_RSHIFT;

        case DIK_MULTIPLY:
            return KEY_KP_MULTIPLY;

        case DIK_SPACE:
            return KEY_SPACE;

        case DIK_F1:
            return KEY_F1;

        case DIK_F2:
            return KEY_F2;

        case DIK_F3:
            return KEY_F3;

        case DIK_F4:
            return KEY_F4;

        case DIK_F5:
            return KEY_F5;

        case DIK_F6:
            return KEY_F6;

        case DIK_F7:
            return KEY_F7;

        case DIK_F8:
            return KEY_F8;

        case DIK_F9:
            return KEY_F9;

        case DIK_F10:
            return KEY_F10;

        case DIK_NUMPAD7:
            return KEY_KP_7;

        case DIK_NUMPAD8:
            return KEY_KP_8;

        case DIK_NUMPAD9:
            return KEY_KP_9;

        case DIK_SUBTRACT:
            return KEY_KP_SUBTRACT;

        case DIK_NUMPAD4:
            return KEY_KP_4;

        case DIK_NUMPAD5:
            return KEY_KP_5;

        case DIK_NUMPAD6:
            return KEY_KP_6;

        case DIK_ADD:
            return KEY_KP_ADD;

        case DIK_NUMPAD1:
            return KEY_KP_1;

        case DIK_NUMPAD2:
            return KEY_KP_2;

        case DIK_NUMPAD3:
            return KEY_KP_3;

        case DIK_NUMPAD0:
            return KEY_KP_0;

        case DIK_F11:
            return KEY_F11;

        case DIK_F12:
            return KEY_F12;

        case DIK_F13:
            return KEY_F13;

        case DIK_F14:
            return KEY_F14;

        case DIK_F15:
            return KEY_F15;

        case DIK_RCONTROL:
            return KEY_RCTRL;

        case DIK_UP:
            return KEY_UP;

        case DIK_PRIOR:
            return KEY_HOME;

        case DIK_LEFT:
            return KEY_LEFT;

        case DIK_RIGHT:
            return KEY_RIGHT;

        case DIK_END:
            return KEY_END;

        case DIK_DOWN:
            return KEY_DOWN;

        case DIK_NEXT:
            return KEY_PAGEDOWN;

        case DIK_DELETE:
            return KEY_DEL;

        default:
            return _key;
    }
}
#endif //DX_KEYBOARD


    //----------------------------------------------------------------------------//
#ifdef USE_WIIMOTE_LIB

//FastPortage!
ITF_INLINE u32 u32_BSR(u32 _u32_Value)
{
    _asm
    {
        bsr eax, _u32_Value
    }
}
ITF_INLINE u32 u32_CountLeadingZero(u32 _u32_Value)
{
    return (_u32_Value == 0) ? 32 : 31 - u32_BSR(_u32_Value);
}
// Given an integer n, returns its power of two (ie the number p which respects 2^p <= n < 2^(p+1) )
// ex : 1 -> 0; 2 -> 1; 3 -> 1; 8 -> 3
ITF_INLINE i32 MTH_PowerOfTwo(u32 n)
{
    return 31 - (i32) u32_CountLeadingZero(n);
}

bbool on_prestart_trcmessage(TRCMessage_Base* pMessage, void* pParams)
{
    WiimotePC* pWiimote = (WiimotePC*)pParams;
    switch(pMessage->getContexte())
    {
    case TRCManagerAdapter::Pad_ExtensionNunchukDetected:
        return (pWiimote->isNunchuckPlugged());
    case TRCManagerAdapter::Pad_ExtensionClassicDetected:
        return (pWiimote->isClassicPlugged());
    case TRCManagerAdapter::Pad_NoExtensionDetected:
        return (!pWiimote->isNunchuckPlugged() && !pWiimote->isClassicPlugged());
    }
    ITF_ASSERT_MSG(0, "on_prestart_trcmessage-> extension unknown");
    return false; // something wrong?
}

void on_state_change (wiimote			  &remote,
                      state_change_flags  changed,
                      const wiimote_state &new_state)
{
    // the wiimote just connected
    if(changed & CONNECTED)
    {
        // ask the wiimote to report everything (using the 'non-continous updates'
        //  default mode - updates will be frequent anyway due to the acceleration/IR
        //  values changing)
        if(new_state.bExtension)
            remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
        else
            remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
    }
    // a MotionPlus was detected
    if(changed & MOTIONPLUS_DETECTED)
    {
        // enable it if there isn't a normal extension plugged into it
        // (MotionPlus devices don't report like normal extensions until
        //  enabled - and then, other extensions attached to it will no longer be
        //  reported (so disable the M+ when you want to access them again).
        if(remote.ExtensionType == wiimote_state::NONE) {
            bool res = remote.EnableMotionPlus();
            ITF_ASSERT(res);
        }
    }
    // an extension is connected to the MotionPlus
    else if(changed & MOTIONPLUS_EXTENSION_CONNECTED)
    {
        // We can't read it if the MotionPlus is currently enabled, so disable it:
        if(remote.MotionPlusEnabled())
            remote.DisableMotionPlus();
    }
    // an extension disconnected from the MotionPlus
    else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
    {
        // enable the MotionPlus data again:
        if(remote.MotionPlusConnected())
            remote.EnableMotionPlus();
    }
    // another extension was just connected:
    else if(changed & EXTENSION_CONNECTED)
    {
        // switch to a report mode that includes the extension data (we will
        //  loose the IR dot sizes)
        // note: there is no need to set report types for a Balance Board.
        if(!remote.IsBalanceBoard())
            remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
    }
    // extension was just disconnected:
    else if(changed & EXTENSION_DISCONNECTED)
    {
        // use a non-extension report mode (this gives us back the IR dot sizes)
        remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
    }
}

void WiimotePC::updateMapping()
{
    // Default button mapping.
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::LEFT)]	= m_joyButton_DPadL;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::RIGHT)]	= m_joyButton_DPadR;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::DOWN)]	= m_joyButton_DPadD;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::UP)]		= m_joyButton_DPadU;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::PLUS)]	= m_joyButton_Plus;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::TWO)]	= m_joyButton_2;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::ONE)]	= m_joyButton_1;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::_B)]		= m_joyButton_B;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::_A)]		= m_joyButton_A;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::MINUS)]	= m_joyButton_Minus;
    m_buttonMapping[MTH_PowerOfTwo(wiimote_state::buttons::HOME)]	= m_joyButton_Home;
}

void WiimotePC::init(i32 id)
{
    m_lastButtonPressed = JOY_MAX_BUT;
    m_lastTimePressed = 0.0;
    m_isDoublePressed = false;
    m_active = true;
    m_timeLastCheck = 0.0;
    setId(id);
    ITF_MemSet(m_buttonMapping, -1, sizeof(i32)*JOY_MAX_BUT);
    ITF_MemSet(m_buttons, 0, sizeof(InputAdapter::PressStatus)*JOY_MAX_BUT);
    ITF_MemSet(m_axes, 0, sizeof(float)*MAX_AXES);

    m_previousExtension = wiimote_state::NONE;
    updateMapping();

    remote.ChangedCallback		= on_state_change;
    remote.CallbackTriggerFlags = (state_change_flags)(CONNECTED | EXTENSION_CHANGED );
}

InputAdapter::PressStatus WiimotePC::getButton(u32 _button) const
{
    ITF_ASSERT(_button < JOY_MAX_BUT);
    return m_buttons[_button];
}


float WiimotePC::getAxe(u32 _axe) const
{
    ITF_ASSERT(_axe < JOY_MAX_AXES);
    return m_axes[_axe];
}

void WiimotePC::update()
{
    if(!remote.IsConnected())
    {
        f64 currentTime = SYSTEM_ADAPTER->getTime();
        if(m_timeLastCheck + 1.0 < currentTime)
        {
            m_timeLastCheck = currentTime;
            if(!remote.Connect(getId()+1))
                return;

            // connected - light all LEDs
            remote.SetLEDs(/*getId()<<1*/0x00f);
        } else
            return ;
    }

    // IMPORTANT: the wiimote state needs to be refreshed each pass
    if(remote.RefreshState() == NO_CHANGE)
        return;

    // did we loose the connection?
    if(remote.ConnectionLost())
    {
        return;
    }

    // Rumble
    if(remote.bRumble) {
    }

    if(m_previousExtension!=remote.ExtensionType)
    {
        m_previousExtension = remote.ExtensionType;

        //TRC Simulation
        if(TRC_ADAPTER)
        {
            switch(remote.ExtensionType)
            {
            case wiimote_state::NUNCHUK:
                TRC_ADAPTER->setCustomParam_u32(getId()+1);
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Pad_ExtensionNunchukDetected, on_prestart_trcmessage, this); break;
            case wiimote_state::CLASSIC:
                TRC_ADAPTER->setCustomParam_u32(getId()+1);
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Pad_ExtensionClassicDetected, on_prestart_trcmessage, this); break;
            case wiimote_state::NONE:
                TRC_ADAPTER->setCustomParam_u32(getId()+1);
                TRC_ADAPTER->addMessage(TRCManagerAdapter::Pad_NoExtensionDetected, on_prestart_trcmessage, this); break;
            default: break;
                // not supported
            }
        }
    }
    //remote.Disconnect();

    // Buttons
    u32 state, button;
    state = remote.Button.Bits & 0x0000ffff;
    button = 0;

    InputAdapter::PressStatus           m_oldbuttons[JOY_MAX_BUT];
    ITF_Memcpy(m_oldbuttons, m_buttons, sizeof(InputAdapter::PressStatus)*JOY_MAX_BUT);
    ITF_MemSet(m_buttons, 0, sizeof(InputAdapter::PressStatus)*JOY_MAX_BUT);

    while(button<JOY_MAX_BUT)
    {
        if(m_buttonMapping[button]>=0)
        {
            bool pressed = (state != 0) && (state & 1);
            bool waspressed = m_oldbuttons[m_buttonMapping[button]] == InputAdapter::Pressed ||
                m_oldbuttons[m_buttonMapping[button]] == InputAdapter::JustPressed;

            if(pressed && waspressed)
                m_buttons[m_buttonMapping[button]] = InputAdapter::Pressed;
            else if(pressed)
                m_buttons[m_buttonMapping[button]] = InputAdapter::JustPressed;
            else if(waspressed)
                m_buttons[m_buttonMapping[button]] = InputAdapter::JustReleased;
            else
                m_buttons[m_buttonMapping[button]] = InputAdapter::Released;
        }
        if(state)
            state >>= 1;
        button++;
    }

    if(getButton(m_joyButton_Home) == InputAdapter::JustPressed)
    {
        m_active = !m_active;
    }

    // Axes
    if(isNunchuckPlugged())
    {
        m_axes[0] = remote.Nunchuk.Joystick.X;
        m_axes[1] = remote.Nunchuk.Joystick.Y;
    }

    if(m_buttons[m_joyButton_X] == InputAdapter::Pressed)
    {
        //LOG (" %04f %04f %04f ", remote.Acceleration.X, remote.Acceleration.Y, remote.Acceleration.Z);
    }

    if(INPUT_ADAPTER_DINPUT->getUSEShakeAttack())
    {
        m_axes[m_joyTrigger_Right] = (  m_buttons[m_joyButton_1] == InputAdapter::JustPressed  ) ? 255.0f : 0.0f ;
        m_buttons[m_joyButton_1] = InputAdapter::Released;

    }



    static f32 threshold = INPUT_ADAPTER_DINPUT->getThreshold();
    static f32 prevZ = 0.0f;
    static f64 timeR = 0;
    static f64 delay = INPUT_ADAPTER_DINPUT->getDelay();

    f32 Z = remote.Acceleration.Z ;

    m_buttons[m_joyActionShaked] = InputAdapter::Released;

    if ( f32_Abs(Z) > threshold )
    {
        if (Z > 0 )
        {
            // punch
            if ( Z - prevZ  <  0  && timeR +delay < SYSTEM_ADAPTER->getTime() )
            {
                m_buttons[m_joyActionShaked] = InputAdapter::Pressed;
                timeR = SYSTEM_ADAPTER->getTime();
            }
        }
    }

    prevZ = remote.Acceleration.Z;

    if ( m_buttons[m_joyActionShaked] == InputAdapter::Pressed )
    {
        if(INPUT_ADAPTER_DINPUT->getUSEShakeAttack())
        {
            m_buttons[m_joyButton_1] = InputAdapter::JustPressed;
        }
    }

}
bool WiimotePC::isJustPressed   (u32 _button)   const
{
    return m_buttons[_button] == InputAdapter::JustPressed;
}

bool WiimotePC::isPressed   (u32 _button)   const
{
    return m_buttons[_button] == InputAdapter::Pressed || m_buttons[_button] == InputAdapter::Double_Press;
}

HRESULT KPadInput::InitKPadInput()
{
    for(u32 _pad = 0; _pad < WiimotePC::MAX_PAD; _pad++)
        m_pads[_pad].init(_pad);
    return S_OK;
}

void KPadInput::UpdateInputStateWiiMote()
{
    for(u32 _pad = 0; _pad < WiimotePC::MAX_PAD; _pad++)
        m_pads[_pad].update();
}
#endif	//USE_WIIMOTE_LIB
} // namespace ITF
