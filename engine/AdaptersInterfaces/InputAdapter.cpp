#include "precompiled_engine.h"

#include <fstream>

#include "gameplay/managers/GameManager.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_
#ifdef ITF_WINDOWS
#include <windows.h>
#include <shlobj.h>
#endif

namespace ITF
{
    const f32 InputAdapter::fDoublePressMaxDuration = 0.2f;
    f64 InputAdapter::DINPUT_lastLeftMouseClick = 0.;


    bool CanUseInputValue(InputValue val)
    {
#ifdef ITF_WINDOWS
        if (val.inputType == Keyboard)
        {
            switch (val.inputValue)
            {
            case VK_UP:
            case VK_DOWN:
            case VK_LEFT:
            case VK_RIGHT:
            case VK_RETURN:
                return false;
            }
        }
#endif
        return true;
    }

    static InputAdapter::PadType getDefaultPadType()
    {
#if defined(ITF_X360)
    return InputAdapter::Pad_X360;
#elif defined(ITF_PS3)
    return InputAdapter::Pad_PS3;
#elif defined(ITF_VITA)
    return InputAdapter::Pad_Vita;
#elif defined(ITF_CTR)
    return InputAdapter::Pad_CTR;
#elif defined(ITF_PS5)
    return InputAdapter::Pad_PS5;
#elif defined(ITF_XBOX_SERIES)
        return InputAdapter::Pad_GenericXBox;
#else
        return InputAdapter::Pad_Other;
#endif
    }

    InputAdapter::InputAdapter() :
        m_inMenu(btrue),
        m_focused(true),
        m_useShakeAttack(bfalse),
        m_threshold(0.0f),
        m_delay(0.0f),
        m_swimMaxSpeed(0),
        m_swimSmooth(0),
        m_runUseB(0),
        m_runUseShake(0),
        m_runTimerStop(0)
    {
        m_lastWheelValue = 0;
        m_lastMouseX = 0;
        m_lastMouseY = 0;
        m_leftMBIsPressed = bfalse;
        m_rightMBIsPressed = bfalse;
        m_middleMBIsPressed = bfalse;
        m_environmentInput = EnvironmentEngine | EnvironmentLua;
        m_buttonMode = MixedMode;

        for (i32 i = 0; i < KEY_COUNT; i++)
        {
            m_keys[i] = bfalse;
            m_keysReleaseTime[i] = 0.0f;
        }

        for (u32 i = 0; i < JOY_MAX_BUT; i++)
        {
            switch (i)
            {
            case m_joyButton_A:
            case m_joyButton_B:
            case m_joyButton_X:
            case m_joyButton_Y:
            case m_joyButton_Start:
            case m_joyButton_Back:
                m_buttonClasses[i] = BUTTONCLASS_STANDARD;
                break;
            case m_joyButton_ThumbLeft:
            case m_joyButton_ThumbRight:
                m_buttonClasses[i] = BUTTONCLASS_ANALOGSTICK;
                break;
            case m_joyButton_LB:
            case m_joyButton_RB:
                m_buttonClasses[i] = BUTTONCLASS_SHOULDER_UPPER;
                break;
            case m_joyButton_DPadL:
            case m_joyButton_DPadR:
            case m_joyButton_DPadU:
            case m_joyButton_DPadD:
                m_buttonClasses[i] = BUTTONCLASS_DIGITALPAD;
                break;
            case m_joyButton_TriggerLeft:
            case m_joyButton_TriggerRight:
                m_buttonClasses[i] = BUTTONCLASS_TRIGGER;
                break;
            default:
                m_buttonClasses[i] = BUTTONCLASS_UNKNOWN;
                break;
            };
        }

        std::fill(m_PadType, m_PadType + ITF_ARRAY_SIZE(m_PadType), getDefaultPadType());
        m_configurationStorage = nullptr;
    }

    void InputAdapter::addListener(Interface_InputListener* _listener, u32 _priority)
    {
        ITF_ASSERT(_listener);
        LOG("[InputAdapter] Add listener => 0x%x (prio: %u)", _listener, _priority);
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

    void InputAdapter::removeListener(Interface_InputListener* _listener)
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

    InputAdapter::~InputAdapter()
    {
        m_listeners.clear();
    }

    void InputAdapter::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
    {
        if (_but == MB_Left)
        {
            if (_status != Released)
                m_leftMBIsPressed = btrue;
            else
                m_leftMBIsPressed = bfalse;
        }
        else if (_but == MB_Right)
        {
            if (_status == Pressed)
                m_rightMBIsPressed = btrue;
            else
                m_rightMBIsPressed = bfalse;
        }
        else if (_but == MB_Middle)
        {
            if (_status == Pressed)
                m_middleMBIsPressed = btrue;
            else
                m_middleMBIsPressed = bfalse;
        }
        pushMouseButtonEvent(_but, _status);
    }

    void InputAdapter::onMouseWheel(i32 _wheelValue)
    {
        i32 delta = _wheelValue - m_lastWheelValue;
        pushMouseWheelEvent(_wheelValue, delta);
        m_lastWheelValue = _wheelValue;
    }

    void InputAdapter::flushKeys()
    {
        ITF_MemSet(&m_keys[0], 0, sizeof(m_keys[0]) * KEY_COUNT);
    }


    void InputAdapter::onKey(i32 _key, InputAdapter::PressStatus _status)
    {
        ITF_ASSERT((_key >= 0) && (_key < KEY_COUNT));

        switch (_status)
        {
        case Pressed:
            m_keys[_key] = btrue;

            // Disable dbl press detection on keyboard
            /*
            if(ELLASPEDTIME - m_keysReleaseTime[_key] < fDoublePressMaxDuration)
            {
                _status = Double_Press;
                m_keysReleaseTime[_key] = ELLASPEDTIME - fDoublePressMaxDuration - MTH_EPSILON;
            }
            */
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

    bbool InputAdapter::isKeyPressed(i32 _key) const
    {
        ITF_ASSERT((_key >= 0) && (_key < KEY_COUNT));

        return m_keys[_key];
    }

    void InputAdapter::onMousePos(i32 _x, i32 _y)
    {
        m_lastMouseX = _x;
        m_lastMouseY = _y;
        pushMousePosEvent(_x, _y);
    }

    void InputAdapter::getMousePos(i32& _x, i32& _y) const
    {
        _x = m_lastMouseX;
        _y = m_lastMouseY;
    }

    void InputAdapter::updateAllInputState()
    {
        ResetInputState();
        UpdatePads();
#if defined(ITF_WINDOWS) && (defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD)
        UpdateKeyboard();
#endif // ITF_WINDOWS
        if (m_focused)
        {
            if (m_inMenu)
            {
                UpdateInputForMenu();
            }
            else
            {
                UpdateInputForGame();
            }
        }
    }

    void InputAdapter::getGamePadPos(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const
    {
        if ((m_environmentInput & _environment) != 0)
        {
            for (u32 i = 0; i < _numAxes; i++)
            {
                _pos[i] = m_axes[_pad][i];
            }
        }
        else
        {
            for (u32 i = 0; i < _numAxes; i++)
                _pos[i] = 0.f;
        }

#ifdef USE_WIIMOTE_LIB
        if(CONFIG->m_enableWiiRemoteonPC)
        {
            if ((m_environmentInput & _environment) != 0 &&
                KPinput.m_pads[_pad].isConnected() && KPinput.m_pads[_pad].isActive())
            {
                for (u32 i = 0 ; i < _numAxes ; i++)
                    _pos[i] = KPinput.m_pads[_pad].getAxe(i);
            }
        }
#endif //USE_WIIMOTE_LIB
    }

    void InputAdapter::getGamePadPosStatus(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const
    {
        // default implementation fills destination array with zeroes
        for (u32 i = 0; i < _numAxes; i++)
            _pos[i] = 0.0f;
    }

    void InputAdapter::getGamePadButtonClasses(u32 /*_pad*/, ButtonClassMask* _buttons, u32 _numButtons) const
    {
        ITF_ASSERT_CRASH(_numButtons<=JOY_MAX_BUT, "button count is greater than limit");

        ITF_MEMCOPY(_buttons, m_buttonClasses, sizeof(_buttons[0]) * _numButtons);
    }

    void InputAdapter::getGamePadButtons(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const
    {
        ITF_ASSERT(_numButtons <= JOY_MAX_BUT);

        if ((m_environmentInput & _environment) != 0)
        {
            for (u32 i = 0; i < _numButtons; i++)
            {
                _buttons[i] = m_buttons[_pad][i];
            }
        }
        else
        {
            for (u32 i = 0; i < _numButtons; i++)
                _buttons[i] = Released;
        }
#ifdef USE_WIIMOTE_LIB
        if(CONFIG->m_enableWiiRemoteonPC)
        {
            if ((m_environmentInput & _environment) != 0)
            {
                if(KPinput.m_pads[_pad].isConnected() && KPinput.m_pads[_pad].isActive())
                    for (u32 i = 0; i < _numButtons; i++)
                        _buttons[i] = KPinput.m_pads[_pad].getButton(i);
            }
        }
#endif //USE_WIIMOTE_LIB
    }

    void InputAdapter::getGamePadButtonsStatus(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const
    {
        // default implementation fills destination array with "Released"
        for (u32 i = 0; i < _numButtons; i++)
            _buttons[i] = Released;
    }

    // Speed between 0-1
    void InputAdapter::padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        // default implementation: ?
    }

    bbool InputAdapter::isMousePressed(MouseButton _but) const
    {
        switch (_but)
        {
        case MB_Left:
            return m_leftMBIsPressed;
            break;
        case MB_Right:
            return m_rightMBIsPressed;
            break;
        case MB_Middle:
            return m_middleMBIsPressed;
            break;
        default:
            ITF_ASSERT(0);
            return bfalse;
        }
    }

    void InputAdapter::dispatchEventsToListeners()
    {
        u32 count = m_eventPool.size();
        for (u32 evtIt = 0; evtIt < count; ++evtIt)
        {
            EditorEvent& editorEvent = m_eventPool[evtIt];
            switch (editorEvent.m_eventType)
            {
            case IA_EventKey:
                {
                    i32 key = editorEvent.m_key.m_key;
                    PressStatus status = editorEvent.m_key.m_status;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onKey(key, status)))
                            break;
                    }
                }
                break;
            case IA_EventMouseButton:
                {
                    MouseButton but = editorEvent.m_but.m_but;
                    PressStatus status = editorEvent.m_but.m_status;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMouseButton(but, status)))
                            break;
                    }
                }
                break;
            case IA_EventMousePos:
                {
                    i32 x = editorEvent.m_val.m_x;
                    i32 y = editorEvent.m_val.m_y;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMousePos(x, y)))
                            break;
                    }
                }
                break;
            case IA_EventMouseWheel:
                {
                    i32 wheelValue = editorEvent.m_val.m_x;
                    i32 delta = editorEvent.m_val.m_y;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMouseWheel(wheelValue, delta)))
                            break;
                    }
                }
            }
        }
        m_eventPool.clear();
    }

    void InputAdapter::pushKeyEvent(i32 _key, PressStatus _status)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventKey;
        evt.m_key.m_key = _key;
        evt.m_key.m_status = _status;
        m_eventPool.push_back(evt);
    }

    void InputAdapter::pushMouseButtonEvent(MouseButton _but, PressStatus _status)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMouseButton;
        evt.m_key.m_key = _but;
        evt.m_key.m_status = _status;
        m_eventPool.push_back(evt);
    }

    void InputAdapter::pushMousePosEvent(i32 _x, i32 _y)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMousePos;
        evt.m_val.m_x = _x;
        evt.m_val.m_y = _y;
        m_eventPool.push_back(evt);
    }

    void InputAdapter::pushMouseWheelEvent(i32 _wheel, i32 _delta)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMouseWheel;
        evt.m_val.m_x = _wheel;
        evt.m_val.m_y = _delta;
        m_eventPool.push_back(evt);
    }
#ifdef ITF_WINDOWS
namespace
{
#ifdef ITF_WINDOWS
    ITF_INLINE i32 TranslateVirtualKey(u32 vk)
    {
        switch (vk)
        {
        case VK_ESCAPE:     return KEY_ESC;
        case VK_RETURN:     return KEY_ENTER;
        case VK_CLEAR:      return KEY_DEL;
        case VK_BACK:       return KEY_BACKSPACE;
        case VK_TAB:        return KEY_TAB;
        case VK_SPACE:      return KEY_SPACE;
        case VK_PRIOR:      return KEY_PAGEUP;
        case VK_NEXT:       return KEY_PAGEDOWN;
        case VK_END:        return KEY_END;
        case VK_HOME:       return KEY_HOME;
        case VK_LEFT:       return KEY_LEFT;
        case VK_UP:         return KEY_UP;
        case VK_RIGHT:      return KEY_RIGHT;
        case VK_DOWN:       return KEY_DOWN;
        case VK_INSERT:     return KEY_INSERT;
        case VK_DELETE:     return KEY_DEL;
        case VK_NUMPAD0:    return KEY_KP_0;
        case VK_NUMPAD1:    return KEY_KP_1;
        case VK_NUMPAD2:    return KEY_KP_2;
        case VK_NUMPAD3:    return KEY_KP_3;
        case VK_NUMPAD4:    return KEY_KP_4;
        case VK_NUMPAD5:    return KEY_KP_5;
        case VK_NUMPAD6:    return KEY_KP_6;
        case VK_NUMPAD7:    return KEY_KP_7;
        case VK_NUMPAD8:    return KEY_KP_8;
        case VK_NUMPAD9:    return KEY_KP_9;
        case VK_MULTIPLY:   return KEY_KP_MULTIPLY;
        case VK_ADD:        return KEY_KP_ADD;
        case VK_SUBTRACT:   return KEY_KP_SUBTRACT;
        case VK_DECIMAL:    return KEY_KP_DECIMAL;
        case VK_DIVIDE:     return KEY_KP_DIVIDE;
        case VK_F1:         return KEY_F1;
        case VK_F2:         return KEY_F2;
        case VK_F3:         return KEY_F3;
        case VK_F4:         return KEY_F4;
        case VK_F5:         return KEY_F5;
        case VK_F6:         return KEY_F6;
        case VK_F7:         return KEY_F7;
        case VK_F8:         return KEY_F8;
        case VK_F9:         return KEY_F9;
        case VK_F10:        return KEY_F10;
        case VK_F11:        return KEY_F11;
        case VK_F12:        return KEY_F12;
        case VK_F13:        return KEY_F13;
        case VK_F14:        return KEY_F14;
        case VK_F15:        return KEY_F15;
        case VK_F16:        return KEY_F16;
        case VK_F17:        return KEY_F17;
        case VK_F18:        return KEY_F18;
        case VK_F19:        return KEY_F19;
        case VK_F20:        return KEY_F20;
        case VK_F21:        return KEY_F21;
        case VK_F22:        return KEY_F22;
        case VK_F23:        return KEY_F23;
        case VK_F24:        return KEY_F24;
        default:
            return (vk < 256) ? static_cast<i32>(vk) : KEY_SPECIAL;
        }
    }
#else
    ITF_INLINE i32 TranslateVirtualKey(u32 vk)
    {
        return static_cast<i32>(vk);
    }
#endif // ITF_WINDOWS
}

    void CALLBACK InputAdapter::KeyCB(u32 nChar, bool bKeyDown, bool bLAlt, bool bRAlt, bool bLCtrl, bool bRCtrl,
                                      bool bLShift,
                                      bool bRShift, void* pUserContext)
    {
        PressStatus status = Pressed;
        if (!bKeyDown)
            status = Released;

        if (!INPUT_ADAPTER) return;

        /// Virtual Keys.
        if (bLCtrl)
        {
            INPUT_ADAPTER->onKey(KEY_LCTRL, (PressStatus)bLCtrl);
            return;
        }

        if (bRCtrl)
        {
            INPUT_ADAPTER->onKey(KEY_RCTRL, (PressStatus)bRCtrl);
            return;
        }

        if (bLShift)
        {
            INPUT_ADAPTER->onKey(KEY_LSHIFT, (PressStatus)bLShift);
            return;
        }

        if (bRShift)
        {
            INPUT_ADAPTER->onKey(KEY_RSHIFT, (PressStatus)bRShift);
            return;
        }

        if (bLAlt) /// ALT.
        {
            INPUT_ADAPTER->onKey(KEY_LALT, (PressStatus)bLAlt);
            INPUT_ADAPTER->onKey(KEY_RALT, (PressStatus)bRAlt);
            return;
        }

        if (bRAlt) /// ALT.
        {
            INPUT_ADAPTER->onKey(KEY_RALT, (ITF::InputAdapter::PressStatus)bRAlt);
        }
        /// Set Keys.
        INPUT_ADAPTER->onKey(TranslateVirtualKey(nChar), status);
    }

    void CALLBACK InputAdapter::MousePosCB(i32 xPos, i32 yPos, void* pUserContext)
    {
        if (INPUT_ADAPTER)
            INPUT_ADAPTER->onMousePos(xPos, yPos);
    }

    void CALLBACK InputAdapter::MouseWheelCB(i32 nMouseWheelDelta, void* pUserContext)
    {
        if (INPUT_ADAPTER)
            INPUT_ADAPTER->onMouseWheel(nMouseWheelDelta);
    }

    void CALLBACK InputAdapter::MouseButtonCB(u32 _Button, u32 _action, void* pUserContext)
    {
        if (INPUT_ADAPTER)
        {
            MouseButton but = (MouseButton)_Button;
            PressStatus status = (PressStatus)_action;
            if (but == MB_Left && status == Pressed)
            {
                f64 thisTime = SYSTEM_ADAPTER->getTime();
                f64 deltaTime = thisTime - DINPUT_lastLeftMouseClick;
                if (deltaTime >= 0. && deltaTime < fDoublePressMaxDuration)
                {
                    status = Double_Press;
                    DINPUT_lastLeftMouseClick = thisTime - fDoublePressMaxDuration - (f64)MTH_EPSILON;
                }
                else
                    DINPUT_lastLeftMouseClick = thisTime;
            }
            INPUT_ADAPTER->onMouseButton(but, status);
        }
    }
#endif // ITF_WINDOWS
    void InputAdapter::LoadPlayerControlSettings()
    {
        ResetToDefaultControls();
        SaveInputMapping();
        // TODO: load control setting from game options
        CopyInputMapping();
    }

    void InputAdapter::SavePlayerControlSettings()
    {
        // TODO: save control setting to game options
    }

    void InputAdapter::InitializeActionStrings()
    {
        static constexpr std::array<const wchar_t*, MAX_ACTIONS> kActionStrings = {
            L"ActionJoinLeave",
            L"ActionSelect",
            L"ActionDelete",
            L"ActionShowMenu",
            L"ActionBack",
            L"ActionLeft",
            L"ActionRight",
            L"ActionUp",
            L"ActionDown",
            L"ActionJump",
            L"ActionHit",
            L"ActionSprint"
        };
        std::copy(kActionStrings.begin(), kActionStrings.end(), m_actionStrings);
    }

    void InputAdapter::InitializeKeyNames()
    {
        static const char* s_keyNames[KEY_COUNT] =
        {
            "[4685]" //00 - key not set
            ,
            "Mouse left" //01 - VK_LBUTTON
            ,
            "Mouse right" //02 - VK_RBUTTON
            ,
            "" //03 - VK_CANCEL
            ,
            "Mouse middle" //04 - VK_MBUTTON
            ,
            "" //05
            ,
            "" //06
            ,
            "" //07
            ,
            "[4666]" //08 - VK_BACK
            ,
            "[4657]" //09 - VK_TAB
            ,
            "" //0A
            ,
            "" //0B
            ,
            "" //0C - VK_CLEAR
            ,
            "[4665]" //0D - VK_RETURN
            ,
            "" //0E
            ,
            "" //0F
            ,
            "" //10 - VK_SHIFT
            ,
            "" //11 - VK_CONTROL
            ,
            "" //12 - VK_MENU
            ,
            "[4680]" //13 - VK_PAUSE
            ,
            "[4658]" //14 - VK_CAPITAL
            ,
            "" //15
            ,
            "" //16
            ,
            "" //17
            ,
            "" //18
            ,
            "" //19
            ,
            "" //1A
            ,
            "" //1B - VK_ESCAPE
            ,
            "" //1C
            ,
            "" //1D
            ,
            "" //1E
            ,
            "" //1F
            ,
            "[4683]" //20 - VK_SPACE
            ,
            "[4671]" //21 - VK_PRIOR
            ,
            "[4672]" //22 - VK_NEXT
            ,
            "[4670]" //23 - VK_END
            ,
            "[4668]" //24 - VK_HOME
            ,
            "[4675]" //25 - VK_LEFT
            ,
            "[4673]" //26 - VK_UP
            ,
            "[4676]" //27 - VK_RIGHT
            ,
            "[4674]" //28 - VK_DOWN
            ,
            "Select" //29 - VK_SELECT
            ,
            "" //2A
            ,
            "" //2B - VK_EXECUTE
            ,
            "[4678]" //2C - VK_SNAPSHOT
            ,
            "[4667]" //2D - VK_INSERT
            ,
            "[4669]" //2E - VK_DELETE
            ,
            "" //2F - VK_HELP
            ,
            "0" //30 - 0
            ,
            "1" //31 - 1
            ,
            "2" //32 - 2
            ,
            "3" //33 - 3
            ,
            "4" //34 - 4
            ,
            "5" //35 - 5
            ,
            "6" //36 - 6
            ,
            "7" //37 - 7
            ,
            "8" //38 - 8
            ,
            "9" //39 - 9
            ,
            "" //3A
            ,
            "" //3B
            ,
            "" //3C
            ,
            "" //3D
            ,
            "" //3E
            ,
            "" //3F
            ,
            "" //40
            ,
            "A" //41 - A
            ,
            "B" //42 - B
            ,
            "C" //43 - C
            ,
            "D" //44 - D
            ,
            "E" //45 - E
            ,
            "F" //46 - F
            ,
            "G" //47 - G
            ,
            "H" //48 - H
            ,
            "I" //49 - I
            ,
            "J" //4A - J
            ,
            "K" //4B - K
            ,
            "L" //4C - L
            ,
            "M" //4D - M
            ,
            "N" //4E - N
            ,
            "O" //4F - O
            ,
            "P" //50 - P
            ,
            "Q" //51 - Q
            ,
            "R" //52 - R
            ,
            "S" //53 - S
            ,
            "T" //54 - T
            ,
            "U" //55 - U
            ,
            "V" //56 - V
            ,
            "W" //57 - W
            ,
            "X" //58 - X
            ,
            "Y" //59 - Y
            ,
            "Z" //5A - Z
            ,
            "" //5B - VK_LWIN
            ,
            "" //5C - VK_RWIN
            ,
            "" //5D - VK_APPS
            ,
            "" //5E
            ,
            "" //5F - VK_SLEEP
            ,
            "[4684] 0" //60 - VK_NUMPAD0
            ,
            "[4684] 1" //61 - VK_NUMPAD1
            ,
            "[4684] 2" //62 - VK_NUMPAD2
            ,
            "[4684] 3" //63 - VK_NUMPAD3
            ,
            "[4684] 4" //64 - VK_NUMPAD4
            ,
            "[4684] 5" //65 - VK_NUMPAD5
            ,
            "[4684] 6" //66 - VK_NUMPAD6
            ,
            "[4684] 7" //67 - VK_NUMPAD7
            ,
            "[4684] 8" //68 - VK_NUMPAD8
            ,
            "[4684] 9" //69 - VK_NUMPAD9
            ,
            "*" //6A - VK_MULTIPLY
            ,
            "+" //6B - VK_ADD
            ,
            "\\" //6C - VK_SEPARATOR
            ,
            "-" //6D - VK_SUBTRACT
            ,
            "." //6E - VK_DECIMAL
            ,
            "/" //6F - VK_DIVIDE
            ,
            "F1" //70 - VK_F1
            ,
            "F2" //71 - VK_F2
            ,
            "F3" //72 - VK_F3
            ,
            "F4" //73 - VK_F4
            ,
            "F5" //74 - VK_F5
            ,
            "F6" //75 - VK_F6
            ,
            "F7" //76 - VK_F7
            ,
            "F8" //77 - VK_F8
            ,
            "F9" //78 - VK_F9
            ,
            "F10" //79 - VK_F10
            ,
            "F11" //7A - VK_F11
            ,
            "F12" //7B - VK_F12
            ,
            "F13" //7C - VK_F13
            ,
            "F14" //7D - VK_F14
            ,
            "F15" //7E - VK_F15
            ,
            "F16" //7F - VK_F16
            ,
            "F17" //80 - VK_F17
            ,
            "F18" //81 - VK_F18
            ,
            "F19" //82 - VK_F19
            ,
            "F20" //83 - VK_F20
            ,
            "F21" //84 - VK_F21
            ,
            "F22" //85 - VK_F22
            ,
            "F23" //86 - VK_F23
            ,
            "F24" //87 - VK_F24
            ,
            "" //88
            ,
            "" //89
            ,
            "" //8A
            ,
            "" //8B
            ,
            "" //8C
            ,
            "" //8D
            ,
            "" //8E
            ,
            "" //8F
            ,
            "[4677]" //90 - VK_NUMLOCK
            ,
            "[4679]" //91 - VK_SCROLL
            ,
            "" //92
            ,
            "" //93
            ,
            "" //94
            ,
            "" //95
            ,
            "" //96
            ,
            "" //97
            ,
            "" //98
            ,
            "" //99
            ,
            "" //9A
            ,
            "" //9B
            ,
            "" //9C
            ,
            "" //9D
            ,
            "" //9E
            ,
            "" //9F
            ,
            "[4659]" //A0 - VK_LSHIFT
            ,
            "[4664]" //A1 - VK_RSHIFT
            ,
            "[4660]" //A2 - VK_LCONTROL
            ,
            "[4663]" //A3 - VK_RCONTROL
            ,
            "[4661]" //A4 - VK_LMENU
            ,
            "[4662]" //A5 - VK_RMENU
            ,
            "" //A6
            ,
            "" //A7
            ,
            "" //A8
            ,
            "" //A9
            ,
            "" //AA
            ,
            "" //AB
            ,
            "" //AC
            ,
            "" //AD
            ,
            "" //AE
            ,
            "" //AF
            ,
            "" //B0
            ,
            "" //B1
            ,
            "" //B2
            ,
            "" //B3
            ,
            "" //B4
            ,
            "" //B5
            ,
            "" //B6
            ,
            "" //B7
            ,
            "" //B8
            ,
            "" //B9
            ,
            ";" //BA - ";"
            ,
            "+" //BB - "+"
            ,
            "," //BC - ","
            ,
            "-" //BD - "-"
            ,
            "." //BE - "."
            ,
            "/" //BF - "/"
            ,
            "~" //C0 - "~"
            ,
            "" //C1
            ,
            "" //C2
            ,
            "" //C3
            ,
            "" //C4
            ,
            "" //C5
            ,
            "" //C6
            ,
            "" //C7
            ,
            "" //C8
            ,
            "" //C9
            ,
            "" //CA
            ,
            "" //CB
            ,
            "" //CC
            ,
            "" //CD
            ,
            "" //CE
            ,
            "" //CF
            ,
            "" //D0
            ,
            "" //D1
            ,
            "" //D2
            ,
            "" //D3
            ,
            "" //D4
            ,
            "" //D5
            ,
            "" //D6
            ,
            "" //D7
            ,
            "" //D8
            ,
            "" //D9
            ,
            "" //DA
            ,
            "[" //DB - "["
            ,
            "\"" //DC - "\"
            ,
            "]" //DD - "]"
            ,
            "'" //DE - "'"
            ,
            "" //DF
            ,
            "" //E0
            ,
            "" //E1
            ,
            "\\" //E2 - "\"
            ,
            "" //E3
            ,
            "" //E4
            ,
            "" //E5
            ,
            "" //E6
            ,
            "" //E7
            ,
            "" //E8
            ,
            "" //E9
            ,
            "" //EA
            ,
            "" //EB
            ,
            "" //EC
            ,
            "" //ED
            ,
            "" //EE
            ,
            "" //EF
            ,
            "" //F0
            ,
            "" //F1
            ,
            "" //F2
            ,
            "" //F3
            ,
            "" //F4
            ,
            "" //F5
            ,
            "" //F6
            ,
            "" //F7
            ,
            "" //F8
            ,
            "" //F9
            ,
            "" //FA
            ,
            "" //FB
            ,
            "" //FC
            ,
            "" //FD
            ,
            "" //FE
        };

        memcpy(m_keyNames, s_keyNames, KEY_COUNT * sizeof(char*));
    }

    void InputAdapter::InitializeX360Names()
    {
        const char* s_buttonNames[JOY_MAX_BUT] =
        {
            "#-[4631]",
            "#-[4632]",
            "#-[4633]",
            "#-[4634]",
            "#-[4635]",
            "#-[4636]",
            "#-[4644]",
            "#-[4643]",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
        };

        memcpy(m_X360ButtonNames, s_buttonNames, JOY_MAX_BUT * sizeof(char*));

        const char* s_axisNames[JOY_MAX_AXES][2] =
        {
            {"#-[4623]", "#-[4624]"},
            {"#-[4626]", "#-[4625]"},
            {"#-[4637]", "#-[4637]"},
            {"#-[4627]", "#-[4628]"},
            {"#-[4630]", "#-[4629]"},
            {"#-[4638]", "#-[4638]"}
        };

        memcpy(m_X360AxisNames, s_axisNames, JOY_MAX_AXES * 2 * sizeof(char*));
    }

    void InputAdapter::InitializeGenericNames()
    {
        // Button names for generic controller
        static constexpr std::array<const char*, JOY_MAX_BUT> kButtonNames = {
            "#-[4646] 1", "#-[4646] 2", "#-[4646] 3", "#-[4646] 4",
            "#-[4646] 5", "#-[4646] 6", "#-[4646] 7", "#-[4646] 8",
            "#-[4646] 9", "#-[4646] 10", "#-[4646] 11", "#-[4646] 12",
            "#-[4646] 13", "#-[4646] 14", "#-[4646] 15", "#-[4646] 16",
            "#-[4646] 17", "#-[4646] 18", "#-[4646] 19", "#-[4646] 20",
            "#-[4646] 21", "#-[4646] 22", "#-[4646] 23", "#-[4646] 24",
            "#-[4646] 25", "#-[4646] 26", "#-[4646] 27", "#-[4646] 28",
            "#-[4646] 29", "#-[4646] 30", "#-[4646] 31", "#-[4646] 32"
        };

        // Axis names for generic controller (Negative/Positive)
        static const std::array<std::array<const char*, 2>, JOY_MAX_AXES> kAxisNames = {
            {
                {"#-[4645] 1(-)", "#-[4645] 1(+)"}, // Axis 1
                {"#-[4645] 2(-)", "#-[4645] 2(+)"}, // Axis 2
                {"#-[4645] 3(-)", "#-[4645] 3(+)"}, // Axis 3
                {"#-[4645] 4(-)", "#-[4645] 4(+)"}, // Axis 4
                {"#-[4645] 5(-)", "#-[4645] 5(+)"}, // Axis 5
                {"#-[4645] 6(-)", "#-[4645] 6(+)"}, // Axis 6
            }
        };
        std::copy(kButtonNames.begin(), kButtonNames.end(), m_GenericButtonNames);
        for (size_t i = 0; i < JOY_MAX_AXES; ++i)
        {
            std::copy(kAxisNames[i].begin(), kAxisNames[i].end(), m_GenericAxisNames[i]);
        }
    }

    void InputAdapter::CopyInputMapping()
    {
        memcpy(m_inputMappingTemporary, m_inputMapping, JOY_MAX_COUNT * MAX_ACTIONS * sizeof(InputValue));
    }

    void InputAdapter::SaveInputMapping()
    {
        memcpy(m_inputMapping, m_inputMappingTemporary, JOY_MAX_COUNT * MAX_ACTIONS * sizeof(InputValue));
    }

    void InputAdapter::ResetToDefaultControls()
    {
        memset(m_inputMappingTemporary, 0, MAX_ACTIONS * JOY_MAX_COUNT * sizeof(InputValue));

        m_inputMappingTemporary[0][ActionBubbleQuit].inputValue = 0x70;
        m_inputMappingTemporary[0][ActionBubbleQuit].inputType = Keyboard;
        m_inputMappingTemporary[1][ActionBubbleQuit].inputValue = 0x71;
        m_inputMappingTemporary[1][ActionBubbleQuit].inputType = Keyboard;
        m_inputMappingTemporary[2][ActionBubbleQuit].inputValue = 0x72;
        m_inputMappingTemporary[2][ActionBubbleQuit].inputType = Keyboard;
        m_inputMappingTemporary[3][ActionBubbleQuit].inputValue = 0x73;
        m_inputMappingTemporary[3][ActionBubbleQuit].inputType = Keyboard;


        uint32_t controllerCount = getGamePadCount();
        for (u32 i = 0; i < controllerCount; i++)
        {
            m_inputMappingTemporary[i][ActionUp].inputValue = 1; //left stick Y
            m_inputMappingTemporary[i][ActionUp].inputType = X360Axis;
            m_inputMappingTemporary[i][ActionUp].axisPosition = 1; //+
            m_inputMappingTemporary[i][ActionUp].inputIndex = i;

            m_inputMappingTemporary[i][ActionDown].inputValue = 1; //left stick Y
            m_inputMappingTemporary[i][ActionDown].inputType = X360Axis;
            m_inputMappingTemporary[i][ActionDown].axisPosition = 0; //-
            m_inputMappingTemporary[i][ActionDown].inputIndex = i;

            m_inputMappingTemporary[i][ActionLeft].inputValue = 0; //left stick X
            m_inputMappingTemporary[i][ActionLeft].inputType = X360Axis;
            m_inputMappingTemporary[i][ActionLeft].axisPosition = 0; //-
            m_inputMappingTemporary[i][ActionLeft].inputIndex = i;

            m_inputMappingTemporary[i][ActionRight].inputValue = 0; //left stick X
            m_inputMappingTemporary[i][ActionRight].inputType = X360Axis;
            m_inputMappingTemporary[i][ActionRight].axisPosition = 1; //+
            m_inputMappingTemporary[i][ActionRight].inputIndex = i;

            m_inputMappingTemporary[i][ActionJump].inputValue = 0; //A
            m_inputMappingTemporary[i][ActionJump].inputType = X360Button;
            m_inputMappingTemporary[i][ActionJump].inputIndex = i;

            m_inputMappingTemporary[i][ActionHit].inputValue = 2; //X
            m_inputMappingTemporary[i][ActionHit].inputType = X360Button;
            m_inputMappingTemporary[i][ActionHit].inputIndex = i;

            m_inputMappingTemporary[i][ActionSprint].inputValue = 5;
            m_inputMappingTemporary[i][ActionSprint].inputType = X360Axis;
            m_inputMappingTemporary[i][ActionSprint].axisPosition = 1; //+
            m_inputMappingTemporary[i][ActionSprint].inputIndex = i;

            m_inputMappingTemporary[i][ActionBack].inputValue = 1; //back
            m_inputMappingTemporary[i][ActionBack].inputType = X360Button;
            m_inputMappingTemporary[i][ActionBack].inputIndex = i;

            m_inputMappingTemporary[i][ActionBubbleQuit].inputValue = 6; //back
            m_inputMappingTemporary[i][ActionBubbleQuit].inputType = X360Button;
            m_inputMappingTemporary[i][ActionBubbleQuit].inputIndex = i;

            m_inputMappingTemporary[i][ActionShowMenu].inputValue = 7; //start
            m_inputMappingTemporary[i][ActionShowMenu].inputType = X360Button;
            m_inputMappingTemporary[i][ActionShowMenu].inputIndex = i;
        }
        if (controllerCount == 0)
        {
            //no X360 pads were found
            //use default keyboard controls for player0
#ifdef ITF_WINDOWS
            m_inputMappingTemporary[0][ActionUp].inputValue = VK_UP;
            m_inputMappingTemporary[0][ActionUp].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionDown].inputValue = VK_DOWN;
            m_inputMappingTemporary[0][ActionDown].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionLeft].inputValue = VK_LEFT;
            m_inputMappingTemporary[0][ActionLeft].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionRight].inputValue = VK_RIGHT;
            m_inputMappingTemporary[0][ActionRight].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionJump].inputValue = VK_SPACE;
            m_inputMappingTemporary[0][ActionJump].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionHit].inputValue = 'S';
            m_inputMappingTemporary[0][ActionHit].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionSprint].inputValue = VK_LSHIFT;
            m_inputMappingTemporary[0][ActionSprint].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionBack].inputValue = VK_BACK;
            m_inputMappingTemporary[0][ActionBack].inputType = Keyboard;

            m_inputMappingTemporary[0][ActionShowMenu].inputValue = VK_ESCAPE;
            m_inputMappingTemporary[0][ActionShowMenu].inputType = Keyboard;
#endif
        }
    }

#ifdef ITF_WINDOWS
    void InputAdapter::UpdateKeyboard()
    {
        for (u32 keyIndex = 0; keyIndex < KEY_COUNT; ++keyIndex)
        {
            const int pressed = GetKeyState(keyIndex) & 0x80;
            m_keyPressTime[keyIndex] += 1;

            if (pressed)
            {
                switch (m_keyStatus[keyIndex])
                {
                case Released:
                case JustReleased:
                    m_keyStatus[keyIndex] = JustPressed;
                    m_keyPressTime[keyIndex] = 0;
                    break;
                case JustPressed:
                case Pressed:
                    m_keyStatus[keyIndex] = Pressed;
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
    }
#endif
    bbool InputAdapter::UpdateActionForButton(u32 player, ActionType action, JoyButton_Common button)
    {
        InputValue val = m_inputMapping[player][action];

        if (val.inputType == Keyboard)
        {
            m_buttons[player][button] = m_keyStatus[val.inputValue];
        }
        else if (val.inputType == X360Button || val.inputType == GenericButton)
        {
            m_buttons[player][button] = GetButtonStatus(val);
            if (m_buttons[player][button] == Released && player == 0 && action == ActionShowMenu)
            {
#ifdef ITF_WINDOWS
                m_buttons[player][button] = m_keyStatus[VK_ESCAPE];
#endif
            }
        }
        else if (val.inputType == X360Axis || val.inputType == GenericAxis)
        {
            f32 axisValue = GetAxe(val);
            if (axisValue < -.65f && val.axisPosition == 0)
            {
                m_buttons[player][button] = Pressed;
            }
            if (axisValue > .65f && val.axisPosition == 1)
            {
                m_buttons[player][button] = Pressed;
            }
        }

        return m_buttons[player][button] == JustPressed;
    }

    bbool InputAdapter::UpdateActionForAxis(u32 player, ActionType action, JoyAxis_t axis, f32 axisValue)
    {
        InputValue val = m_inputMapping[player][action];

        if (val.inputType == Keyboard)
        {
            if ((m_keyStatus[val.inputValue] == JustPressed || m_keyStatus[val.inputValue] == Pressed) &&
                m_axesPressTime[player][axis] > m_keyPressTime[val.inputValue])
            {
                m_axesPressTime[player][axis] = m_keyPressTime[val.inputValue];
                m_axes[player][axis] = axisValue;
            }
        }
        else if (val.inputType == X360Button || val.inputType == GenericButton)
        {
            if (IsButtonPressed(val))
            {
                m_axes[player][axis] = axisValue;
            }
        }
        else if (val.inputType == X360Axis || val.inputType == GenericAxis)
        {
            m_axes[player][axis] = GetAxe(val);
        }

        return (fabsf(m_axes[player][axis]) >= .65f);
    }

    const String& InputAdapter::GetInputString(const String& x360Button)
    {
        static const String sprint = "X360_BUTTON_RT";
        static const String atack = "X360_BUTTON_X";
        static const String jump = "X360_BUTTON_A";
        static const String back = "X360_BUTTON_BACK";
        static const String stick = "X360_STICK";
        LocalisationId idd;
        idd.value = 4682;
        static const String stick_keys = LOCALISATIONMANAGER->getText(idd);

        if (x360Button == sprint)
        {
            return GetInputString(0, ActionSprint);
        }
        else if (x360Button == atack)
        {
            return GetInputString(0, ActionHit);
        }
        else if (x360Button == jump)
        {
            return GetInputString(0, ActionJump);
        }
        else if (x360Button == back)
        {
            return GetInputString(0, ActionBack);
        }
        else if (x360Button == stick)
        {
            return stick_keys;
        }

        return String::emptyString;
    }

    const String& InputAdapter::GetInputString(u32 player, u32 action)
    {
        m_inputString.setText("");

        switch (m_inputMappingTemporary[player][action].inputType)
        {
        case Keyboard:
            m_inputString.setText(m_keyNames[m_inputMappingTemporary[player][action].inputValue]);
            break;
        case X360Button:
            m_inputString.setText(m_X360ButtonNames[m_inputMappingTemporary[player][action].inputValue]);
            break;
        case X360Axis:
            m_inputString.setText(
                m_X360AxisNames[m_inputMappingTemporary[player][action].inputValue][m_inputMappingTemporary[player][
                    action].axisPosition]);
            break;
        case GenericButton:
            m_inputString.setText(m_GenericButtonNames[m_inputMappingTemporary[player][action].inputValue]);
            break;
        case GenericAxis:
            m_inputString.setText(
                m_GenericAxisNames[m_inputMappingTemporary[player][action].inputValue][m_inputMappingTemporary[player][
                    action].axisPosition]);
            break;
        }

        if (m_inputString.cStr()[0] == '#')
        {
            m_inputString.replace('#', char('1' + m_inputMappingTemporary[player][action].inputIndex));
        }

        return ValidateInputString();
    }

    void InputAdapter::SetInputValue(u32 player, u32 action, InputValue& value)
    {
        if (value.inputType != Keyboard)
        {
#ifdef ITF_USE_SDL
            if (value.inputType == X360Button)
                value.inputType = GenericButton;
            else if (value.inputType == X360Axis)
                value.inputType = GenericAxis;
#endif
        }
        value.inputType = GetControllerType(value);
        for (u32 playerIndex = 0; playerIndex < JOY_MAX_COUNT; ++playerIndex)
        {
            for (u32 actionIndex = ActionBubbleQuit; actionIndex < MAX_ACTIONS; ++actionIndex)
            {
                if (!memcmp(&value, &m_inputMappingTemporary[playerIndex][actionIndex], sizeof(InputValue)))
                {
                    memset(&m_inputMappingTemporary[playerIndex][actionIndex], 0, sizeof(InputValue));
                }
            }
        }

        m_inputMappingTemporary[player][action] = value;
    }

    const String& InputAdapter::GetInputString(u32 iconType)
    {
        m_inputString.setText("");
        if (!GAMEMANAGER->isPlayableScreen())
        {
#ifdef ITF_WINDOWS
            switch (iconType)
            {
            case ContextIconType_Select:
                m_inputString.setText(m_keyNames[VK_RETURN]);
                break;
            case ContextIconType_Back:
                m_inputString.setText(m_keyNames[VK_ESCAPE]);
                break;
            case ContextIconType_Delete:
                m_inputString.setText(m_keyNames[VK_DELETE]);
                break;
            case ContextIconType_RayHome:
                m_inputString.setText(m_keyNames[VK_ESCAPE]);
                break;
            }
#endif
        }
        else
        {
            u32 action = 0;
            switch (iconType)
            {
            case ContextIconType_Select:
                action = ActionJump;
                break;
            case ContextIconType_Back:
                action = ActionBack;
                break;
            case ContextIconType_Delete:
                action = ActionDelete;
                break;
            case ContextIconType_RayHome:
                action = ActionHit;
                break;
            }

            switch (m_inputMappingTemporary[0][action].inputType)
            {
            case Keyboard:
                m_inputString.setText(m_keyNames[m_inputMappingTemporary[0][action].inputValue]);
                break;
            case X360Button:
                m_inputString.setText(m_X360ButtonNames[m_inputMappingTemporary[0][action].inputValue]);
                break;
            case X360Axis:
                m_inputString.setText(
                    m_X360AxisNames[m_inputMappingTemporary[0][action].inputValue][m_inputMappingTemporary[0][action].
                        axisPosition]);
                break;
            case GenericButton:
                m_inputString.setText(m_GenericButtonNames[m_inputMappingTemporary[0][action].inputValue]);
                break;
            case GenericAxis:
                m_inputString.setText(
                    m_GenericAxisNames[m_inputMappingTemporary[0][action].inputValue][m_inputMappingTemporary[0][action]
                        .axisPosition]);
                break;
            }

            if (m_inputString.cStr()[0] == '#')
            {
                m_inputString.replace('#', char('1' + m_inputMappingTemporary[0][action].inputIndex));
            }
        }

        return ValidateInputString();
    }

    void InputAdapter::ResetInputState()
    {
        memset(m_axes, 0, JOY_MAX_COUNT * JOY_MAX_AXES * sizeof(float));
        memset(m_axesPressTime, 0xFF, JOY_MAX_COUNT * JOY_MAX_AXES * sizeof(u32));
        memset(m_buttons, 0, JOY_MAX_COUNT * JOY_MAX_BUT * sizeof(PressStatus));
    }

    void InputAdapter::UpdateInputForMenu()
    {
        for (u32 playerIndex = 0; playerIndex < JOY_MAX_COUNT; ++playerIndex)
        {
            if (CanUseInputValue(m_inputMapping[playerIndex][ActionJump]))
            {
                UpdateActionForButton(playerIndex, ActionJump, m_joyButton_A);
            }

            if (CanUseInputValue(m_inputMapping[playerIndex][ActionBack]))
            {
                UpdateActionForButton(playerIndex, ActionBack, m_joyButton_B);
            }

            if (CanUseInputValue(m_inputMapping[playerIndex][ActionLeft]))
            {
                UpdateActionForAxis(playerIndex, ActionLeft, m_joyStickLeft_X, -1);
            }

            if (CanUseInputValue(m_inputMapping[playerIndex][ActionRight]))
            {
                UpdateActionForAxis(playerIndex, ActionRight, m_joyStickLeft_X, 1);
            }

            if (CanUseInputValue(m_inputMapping[playerIndex][ActionUp]))
            {
                UpdateActionForAxis(playerIndex, ActionUp, m_joyStickLeft_Y, 1);
            }

            if (CanUseInputValue(m_inputMapping[playerIndex][ActionDown]))
            {
                UpdateActionForAxis(playerIndex, ActionDown, m_joyStickLeft_Y, -1);
            }

            //overwrite current status with default menu controls if they are released
#ifdef ITF_WINDOWS
            if (m_connectedPlayers[playerIndex] == ePlaying)
            {
                if (m_buttons[playerIndex][m_joyButton_Y] == Released)
                {
                    m_buttons[playerIndex][m_joyButton_Y] = m_keyStatus[VK_DELETE];
                }

                if (m_buttons[playerIndex][m_joyButton_A] == Released)
                {
                    m_buttons[playerIndex][m_joyButton_A] = m_keyStatus[VK_RETURN];
                }

                if (m_buttons[playerIndex][m_joyButton_B] == Released)
                {
                    m_buttons[playerIndex][m_joyButton_B] = m_keyStatus[VK_ESCAPE];
                }

                if (m_buttons[playerIndex][m_joyButton_DPadL] == Released)
                {
                    m_buttons[playerIndex][m_joyButton_DPadL] = m_keyStatus[VK_LEFT];
                }

                if (m_buttons[playerIndex][m_joyButton_DPadR] == Released)
                {
                    m_buttons[playerIndex][m_joyButton_DPadR] = m_keyStatus[VK_RIGHT];
                }

                if (m_buttons[playerIndex][m_joyButton_DPadU] == Released)
                {
                    m_buttons[playerIndex][m_joyButton_DPadU] = m_keyStatus[VK_UP];
                }

                if (m_buttons[playerIndex][m_joyButton_DPadD] == Released)
                {
                    m_buttons[playerIndex][m_joyButton_DPadD] = m_keyStatus[VK_DOWN];
                }
            }
#endif
        }
    }

    void InputAdapter::UpdateInputForGame()
    {
        for (u32 playerIndex = 0; playerIndex < JOY_MAX_COUNT; ++playerIndex)
        {
            if (m_connectedPlayers[playerIndex] == ePlaying || m_connectedPlayers[playerIndex] == eBubble)
            {
                bbool pressed = bfalse;
                pressed |= UpdateActionForButton(playerIndex, ActionJump, m_joyButton_A);
                pressed |= UpdateActionForButton(playerIndex, ActionHit, m_joyButton_X);
                pressed |= UpdateActionForButton(playerIndex, ActionBack, m_joyButton_B);
                pressed |= UpdateActionForButton(playerIndex, ActionShowMenu, m_joyButton_Start);
                (m_inputMapping[playerIndex][ActionLeft].inputType == X360Axis || m_inputMapping[playerIndex][
                        ActionLeft].inputType == GenericAxis || m_inputMapping[playerIndex][ActionLeft].inputType ==
                    Keyboard)
                    ? UpdateActionForAxis(playerIndex, ActionLeft, m_joyStickLeft_X, -1)
                    : UpdateActionForButton(playerIndex, ActionLeft, m_joyButton_DPadL);
                (m_inputMapping[playerIndex][ActionRight].inputType == X360Axis || m_inputMapping[playerIndex][
                        ActionRight].inputType == GenericAxis || m_inputMapping[playerIndex][ActionRight].inputType ==
                    Keyboard)
                    ? UpdateActionForAxis(playerIndex, ActionRight, m_joyStickLeft_X, 1)
                    : UpdateActionForButton(playerIndex, ActionRight, m_joyButton_DPadR);
                (m_inputMapping[playerIndex][ActionDown].inputType == X360Axis || m_inputMapping[playerIndex][
                        ActionDown].inputType == GenericAxis || m_inputMapping[playerIndex][ActionDown].inputType ==
                    Keyboard)
                    ? UpdateActionForAxis(playerIndex, ActionDown, m_joyStickLeft_Y, -1)
                    : UpdateActionForButton(playerIndex, ActionDown, m_joyButton_DPadD);
                (m_inputMapping[playerIndex][ActionUp].inputType == X360Axis || m_inputMapping[playerIndex][ActionUp].
                    inputType == GenericAxis || m_inputMapping[playerIndex][ActionUp].inputType == Keyboard)
                    ? UpdateActionForAxis(playerIndex, ActionUp, m_joyStickLeft_Y, 1)
                    : UpdateActionForButton(playerIndex, ActionUp, m_joyButton_DPadU);
                UpdateActionForAxis(playerIndex, ActionSprint, m_joyTrigger_Right, 1);

                if (pressed && m_connectedPlayers[playerIndex] == eBubble)
                {
                    m_connectedPlayers[playerIndex] = ePlaying;
                }
#ifdef ITF_WINDOWS
                if ((m_keyStatus[VK_LMENU] == Released && m_keyStatus[VK_RMENU] == Released &&
                        m_keyStatus[VK_LCONTROL] == Released && m_keyStatus[VK_RCONTROL] == Released &&
                        m_keyStatus[VK_LSHIFT] == Released && m_keyStatus[VK_RSHIFT] == Released) ||
                    m_inputMapping[playerIndex][ActionBubbleQuit].inputType != Keyboard)
                {
                    if (UpdateActionForButton(playerIndex, ActionBubbleQuit, m_joyButton_Back))
                    {
                        if (m_connectedPlayers[playerIndex] == ePlaying)
                        //if player press first time on Bubble/Quit button he will be in bubble state
                        {
                            m_connectedPlayers[playerIndex] = eBubble;
                        }
                        else if (m_connectedPlayers[playerIndex] == eBubble)
                        //if player press second time on Bubble/Quit button he will be disconnected
                        {
                            m_connectedPlayers[playerIndex] = eNotConnected;
                            setPadConnected(playerIndex, bfalse);
                        }
                    }
                }
#endif
            }
            else //player should join on any pressed key/button/axis
            {
                bbool pressed = bfalse;

                pressed |= UpdateActionForButton(playerIndex, ActionJump, m_joyButton_A);
                pressed |= UpdateActionForButton(playerIndex, ActionHit, m_joyButton_X);
                pressed |= UpdateActionForButton(playerIndex, ActionBack, m_joyButton_B);
                pressed |= UpdateActionForButton(playerIndex, ActionShowMenu, m_joyButton_Start);

                pressed |= (m_inputMapping[playerIndex][ActionLeft].inputType == X360Axis || m_inputMapping[playerIndex]
                               [ActionLeft].inputType == GenericAxis || m_inputMapping[playerIndex][ActionLeft].
                               inputType == Keyboard)
                               ? UpdateActionForAxis(playerIndex, ActionLeft, m_joyStickLeft_X, -1)
                               : UpdateActionForButton(playerIndex, ActionLeft, m_joyButton_DPadL);
                pressed |= (m_inputMapping[playerIndex][ActionRight].inputType == X360Axis || m_inputMapping[
                               playerIndex][ActionRight].inputType == GenericAxis || m_inputMapping[playerIndex][
                               ActionRight].inputType == Keyboard)
                               ? UpdateActionForAxis(playerIndex, ActionRight, m_joyStickLeft_X, 1)
                               : UpdateActionForButton(playerIndex, ActionRight, m_joyButton_DPadR);
                pressed |= (m_inputMapping[playerIndex][ActionDown].inputType == X360Axis || m_inputMapping[playerIndex]
                               [ActionDown].inputType == GenericAxis || m_inputMapping[playerIndex][ActionDown].
                               inputType == Keyboard)
                               ? UpdateActionForAxis(playerIndex, ActionDown, m_joyStickLeft_Y, -1)
                               : UpdateActionForButton(playerIndex, ActionDown, m_joyButton_DPadD);
                pressed |= (m_inputMapping[playerIndex][ActionUp].inputType == X360Axis || m_inputMapping[playerIndex][
                                   ActionUp].inputType == GenericAxis || m_inputMapping[playerIndex][ActionUp].inputType
                               ==
                               Keyboard)
                               ? UpdateActionForAxis(playerIndex, ActionUp, m_joyStickLeft_Y, 1)
                               : UpdateActionForButton(playerIndex, ActionUp, m_joyButton_DPadU);

                pressed |= UpdateActionForAxis(playerIndex, ActionSprint, m_joyTrigger_Right, 1);

                if (pressed)
                {
                    m_connectedPlayers[playerIndex] = eBubble;
                    setPadConnected(playerIndex, btrue);

                    //reset buttons states to ignore first command
                    m_buttons[playerIndex][m_joyButton_A] = Released;
                    m_buttons[playerIndex][m_joyButton_X] = Released;
                    m_buttons[playerIndex][m_joyButton_B] = Released;
                    m_buttons[playerIndex][m_joyButton_DPadL] = Released;
                    m_buttons[playerIndex][m_joyButton_DPadR] = Released;
                    m_buttons[playerIndex][m_joyButton_DPadU] = Released;
                    m_buttons[playerIndex][m_joyButton_DPadD] = Released;
                    m_buttons[playerIndex][m_joyButton_Start] = Released;
                }
            }
        }
    }

    const String& InputAdapter::ValidateInputString()
    {
        static String output;

        const u16* string = m_inputString.cStr();
        u32 index = 0;
        output.setText("");

        while (string[index] && string[index] != '[')
        {
            output += string[index++];
        }

        if (index != m_inputString.getLen())
        {
            u32 lineId = 0;
            ++index;

            while (string[index] && string[index] != ']')
            {
                lineId *= 10;
                lineId += string[index++] - '0';
            }

            LocalisationId locId;
            locId = lineId;
            output += LOCALISATIONMANAGER->getText(locId);
            output += String(string + index + 1);
        }

        return output;
    }

    const String& InputAdapter::GetKeyString(u32 key)
    {
        m_inputString.setText(m_keyNames[key]);

        return ValidateInputString();
    }

    const InputValue& InputAdapter::GetInputValue(u32 player, u32 action) const
    {
        return m_inputMapping[player][action];
    }
} // namespace ITF
