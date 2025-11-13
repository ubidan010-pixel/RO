#include "precompiled_engine.h"

#include "gameplay/managers/GameManager.h"
#ifdef USE_PAD_HAPTICS
#include "gameplay/Managers/PadHapticsManager.h"
#endif
#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_INPUT_MANAGER_H_
#include "engine/input/InputManager.h"
#endif //_ITF_INPUT_MANAGER_H_

#ifndef _ITF_KEYBOARD_INPUT_SOURCE_H_
#include "engine/input/KeyboardInputSource.h"
#endif //_ITF_KEYBOARD_INPUT_SOURCE_H_

#ifndef _ITF_INPUT_TYPES_H_
#include "engine/input/InputTypes.h"
#endif //_ITF_INPUT_TYPES_H_

#ifndef _ITF_INPUT_MAPPING_DEFAULTS_H_
#include "engine/input/InputMappingDefaults.h"
#endif //_ITF_INPUT_MAPPING_DEFAULTS_H_

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

    static bool ConvertInputValueToPhysicalInput(const InputValue& val, PhysicalInput& physical)
    {
        if (val.inputType == Keyboard)
        {
            physical = MakeKeyboardInput(val.inputValue);
            return true;
        }
        else if (val.inputType == ControllerButton)
        {
            physical = MakeControllerButtonInput(val.inputIndex, val.inputValue);
            return true;
        }
        else if (val.inputType == ControllerAxis)
        {
            f32 axisValue = (val.axisPosition == 1) ? 1.0f : -1.0f;
            physical = MakeControllerAxisInput(val.inputIndex, val.inputValue, axisValue);
            return true;
        }
        return false;
    }

    /**
     * Convert ActionType to VirtualInput
     * This maps game actions to virtual inputs based on default mappings
     * @param action Action type
     * @param virtualInput Output VirtualInput
     * @return true if mapping found, false otherwise
     */
    static bool ConvertActionToVirtualInput(InputAdapter::ActionType action, VirtualInput& virtualInput)
    {
        switch (action)
        {
        case InputAdapter::ActionJump:
            virtualInput = MakeVirtualButton(VIRTUAL_BUTTON_A);
            return true;
        case InputAdapter::ActionHit:
            virtualInput = MakeVirtualButton(VIRTUAL_BUTTON_X);
            return true;
        case InputAdapter::ActionBack:
            virtualInput = MakeVirtualButton(VIRTUAL_BUTTON_B);
            return true;
        case InputAdapter::ActionShowMenu:
            virtualInput = MakeVirtualButton(VIRTUAL_BUTTON_START);
            return true;
        case InputAdapter::ActionLeft:
            virtualInput = MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X);
            return true;
        case InputAdapter::ActionRight:
            virtualInput = MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X);
            return true;
        case InputAdapter::ActionUp:
            virtualInput = MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y);
            return true;
        case InputAdapter::ActionDown:
            virtualInput = MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y);
            return true;
        case InputAdapter::ActionSprint:
            virtualInput = MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_TRIGGER);
            return true;
        default:
            return false;
        }
    }

    /**
     * Convert VirtualInput to ActionType (reverse of ConvertActionToVirtualInput)
     * @param virtualInput Virtual input
     * @param action Output ActionType
     * @return true if mapping found, false otherwise
     */
    static bool ConvertVirtualInputToAction(const VirtualInput& virtualInput, InputAdapter::ActionType& action)
    {
        if (virtualInput.type == VirtualInput::Button)
        {
            VirtualButton button = static_cast<VirtualButton>(virtualInput.virtualId);
            switch (button)
            {
            case VIRTUAL_BUTTON_A:
                action = InputAdapter::ActionJump;
                return true;
            case VIRTUAL_BUTTON_X:
                action = InputAdapter::ActionHit;
                return true;
            case VIRTUAL_BUTTON_B:
                action = InputAdapter::ActionBack;
                return true;
            case VIRTUAL_BUTTON_START:
                action = InputAdapter::ActionShowMenu;
                return true;
            default:
                return false;
            }
        }
        else if (virtualInput.type == VirtualInput::Axis)
        {
            VirtualAxis axis = static_cast<VirtualAxis>(virtualInput.virtualId);
            switch (axis)
            {
            case VIRTUAL_AXIS_LEFT_STICK_X:
                action = InputAdapter::ActionLeft;
                return true;
            case VIRTUAL_AXIS_LEFT_STICK_Y:
                action = InputAdapter::ActionUp;
                return true;
            case VIRTUAL_AXIS_RIGHT_TRIGGER:
                action = InputAdapter::ActionSprint;
                return true;
            default:
                return false;
            }
        }
        return false;
    }

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
        m_inputManager(nullptr),
        m_inputManagerInitialized(bfalse),
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
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        LOG("[InputAdapter] onKey: key=%d, status=%d", _key, _status);

        KeyboardInputSource* keyboardSource =
            static_cast<KeyboardInputSource*>(m_inputManager->GetInputSource(KEYBOARD_DEVICE_ID));
        if (!keyboardSource)
        {
            LOG("[InputAdapter] onKey: ERROR - KeyboardInputSource is null!");
            ITF_ASSERT(keyboardSource != nullptr);
            return;
        }

        keyboardSource->OnKeyEvent(_key, _status);
        LOG("[InputAdapter] onKey: Forwarded to KeyboardInputSource");

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
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());
        m_inputManager->Update();

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
        ITF_ASSERT(_numAxes <= JOY_MAX_AXES);
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        if ((m_environmentInput & _environment) != 0)
        {
            m_inputManager->getGamePadPos(_pad, _pos, _numAxes);
        }
        else
        {
            for (u32 i = 0; i < _numAxes; i++)
                _pos[i] = 0.f;
        }
    }

    void InputAdapter::getGamePadButtonClasses(u32 /*_pad*/, ButtonClassMask* _buttons, u32 _numButtons) const
    {
        ITF_ASSERT_CRASH(_numButtons<=JOY_MAX_BUT, "button count is greater than limit");

        ITF_MEMCOPY(_buttons, m_buttonClasses, sizeof(_buttons[0]) * _numButtons);
    }

    void InputAdapter::getGamePadButtons(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const
    {
        ITF_ASSERT(_numButtons <= JOY_MAX_BUT);
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        if ((m_environmentInput & _environment) != 0)
        {
            m_inputManager->getGamePadButtons(_pad, _buttons, _numButtons);
        }
        else
        {
            for (u32 i = 0; i < _numButtons; i++)
                _buttons[i] = Released;
        }
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
            case VK_ESCAPE: return KEY_ESC;
            case VK_RETURN: return KEY_ENTER;
            case VK_CLEAR: return KEY_DEL;
            case VK_BACK: return KEY_BACKSPACE;
            case VK_TAB: return KEY_TAB;
            case VK_SPACE: return KEY_SPACE;
            case VK_PRIOR: return KEY_PAGEUP;
            case VK_NEXT: return KEY_PAGEDOWN;
            case VK_END: return KEY_END;
            case VK_HOME: return KEY_HOME;
            case VK_LEFT: return KEY_LEFT;
            case VK_UP: return KEY_UP;
            case VK_RIGHT: return KEY_RIGHT;
            case VK_DOWN: return KEY_DOWN;
            case VK_INSERT: return KEY_INSERT;
            case VK_DELETE: return KEY_DEL;
            case VK_NUMPAD0: return KEY_KP_0;
            case VK_NUMPAD1: return KEY_KP_1;
            case VK_NUMPAD2: return KEY_KP_2;
            case VK_NUMPAD3: return KEY_KP_3;
            case VK_NUMPAD4: return KEY_KP_4;
            case VK_NUMPAD5: return KEY_KP_5;
            case VK_NUMPAD6: return KEY_KP_6;
            case VK_NUMPAD7: return KEY_KP_7;
            case VK_NUMPAD8: return KEY_KP_8;
            case VK_NUMPAD9: return KEY_KP_9;
            case VK_MULTIPLY: return KEY_KP_MULTIPLY;
            case VK_ADD: return KEY_KP_ADD;
            case VK_SUBTRACT: return KEY_KP_SUBTRACT;
            case VK_DECIMAL: return KEY_KP_DECIMAL;
            case VK_DIVIDE: return KEY_KP_DIVIDE;
            case VK_F1: return KEY_F1;
            case VK_F2: return KEY_F2;
            case VK_F3: return KEY_F3;
            case VK_F4: return KEY_F4;
            case VK_F5: return KEY_F5;
            case VK_F6: return KEY_F6;
            case VK_F7: return KEY_F7;
            case VK_F8: return KEY_F8;
            case VK_F9: return KEY_F9;
            case VK_F10: return KEY_F10;
            case VK_F11: return KEY_F11;
            case VK_F12: return KEY_F12;
            case VK_F13: return KEY_F13;
            case VK_F14: return KEY_F14;
            case VK_F15: return KEY_F15;
            case VK_F16: return KEY_F16;
            case VK_F17: return KEY_F17;
            case VK_F18: return KEY_F18;
            case VK_F19: return KEY_F19;
            case VK_F20: return KEY_F20;
            case VK_F21: return KEY_F21;
            case VK_F22: return KEY_F22;
            case VK_F23: return KEY_F23;
            case VK_F24: return KEY_F24;
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
            INPUT_ADAPTER->onKey(KEY_RALT, (PressStatus)bRAlt);
        }
        /// Set Keys.
        i32 translatedKey = TranslateVirtualKey(nChar);
        LOG("[InputAdapter] KeyCB: nChar=%u, translatedKey=%d, status=%d", nChar, translatedKey, status);
        INPUT_ADAPTER->onKey(translatedKey, status);
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
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        ResetToDefaultControls();
    }

    void InputAdapter::SavePlayerControlSettings()
    {
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());
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

    void InputAdapter::CopyInputMapping()
    {
    }

    void InputAdapter::SaveInputMapping()
    {
    }

    void InputAdapter::ResetToDefaultControls()
    {
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        m_inputManager->ResetAllRemaps();
        InitializeDefaultKeyboardMappings(m_inputManager->GetInputMapping());

        uint32_t controllerCount = getGamePadCount();
        for (u32 i = 0; i < controllerCount; ++i)
        {
            InitializeDefaultControllerMappings(m_inputManager->GetInputMapping(), i);
        }
    }

#ifdef ITF_WINDOWS
    void InputAdapter::UpdateKeyboard()
    {
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        KeyboardInputSource* keyboardSource =
            static_cast<KeyboardInputSource*>(m_inputManager->GetInputSource(KEYBOARD_DEVICE_ID));
        if (!keyboardSource)
        {
            LOG("[InputAdapter] UpdateKeyboard: ERROR - KeyboardInputSource is null!");
            ITF_ASSERT(keyboardSource != nullptr);
            return;
        }

        keyboardSource->Update();

        for (u32 i = 0; i < KEY_COUNT; ++i)
        {
            if (m_keyStatus[i] == Pressed || m_keyStatus[i] == JustPressed)
            {
                m_keyPressTime[i] += 1;
            }
        }
    }
#endif
    bbool InputAdapter::UpdateActionForButton(u32 player, ActionType action, JoyButton_Common button)
    {
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        VirtualInput virtualInput;
        if (!ConvertActionToVirtualInput(action, virtualInput))
        {
            m_buttons[player][button] = Released;
            return bfalse;
        }

        PressStatus buttonState = Released;
        if (virtualInput.type == VirtualInput::Button)
        {
            buttonState = m_inputManager->GetVirtualInputState().GetButton(
                player, static_cast<VirtualButton>(virtualInput.virtualId));
        }
        else if (virtualInput.type == VirtualInput::Axis)
        {
            f32 axisValue = m_inputManager->GetVirtualInputState().GetAxis(
                player, static_cast<VirtualAxis>(virtualInput.virtualId));

            if (fabsf(axisValue) >= 0.65f)
            {
                buttonState = Pressed;
            }
        }
        m_buttons[player][button] = buttonState;
        return buttonState == JustPressed;
    }

    bbool InputAdapter::UpdateActionForAxis(u32 player, ActionType action, JoyAxis_t axis, f32 axisValue)
    {
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        VirtualInput virtualInput;
        if (!ConvertActionToVirtualInput(action, virtualInput))
        {
            m_axes[player][axis] = 0.0f;
            return bfalse;
        }

        if (virtualInput.type == VirtualInput::Axis)
        {
            f32 virtualAxisValue = m_inputManager->GetVirtualInputState().GetAxis(
                player, static_cast<VirtualAxis>(virtualInput.virtualId));
            m_axes[player][axis] = virtualAxisValue;
            return (fabsf(virtualAxisValue) >= 0.65f);
        }
        else if (virtualInput.type == VirtualInput::Button)
        {
            PressStatus buttonState = m_inputManager->GetVirtualInputState().GetButton(
                player, static_cast<VirtualButton>(virtualInput.virtualId));
            if (buttonState == Pressed || buttonState == JustPressed)
            {
                m_axes[player][axis] = axisValue;
                return btrue;
            }
            else
            {
                m_axes[player][axis] = 0.0f;
                return bfalse;
            }
        }

        m_axes[player][axis] = 0.0f;
        return bfalse;
    }

    void InputAdapter::SetInputValue(u32 player, u32 action, InputValue& value)
    {
        value.inputType = GetControllerType(value);

        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        PhysicalInput physicalInput;
        if (ConvertInputValueToPhysicalInput(value, physicalInput))
        {
            VirtualInput virtualInput;
            if (ConvertActionToVirtualInput(static_cast<ActionType>(action), virtualInput))
            {
                m_inputManager->SetRemap(player, physicalInput, virtualInput);

                m_inputManager->ClearRemap(player, virtualInput);
                m_inputManager->SetRemap(player, physicalInput, virtualInput);
            }
        }
    }

    void InputAdapter::ResetInputState()
    {
        memset(m_axes, 0, JOY_MAX_COUNT * JOY_MAX_AXES * sizeof(float));
        memset(m_axesPressTime, 0xFF, JOY_MAX_COUNT * JOY_MAX_AXES * sizeof(u32));
        memset(m_buttons, 0, JOY_MAX_COUNT * JOY_MAX_BUT * sizeof(PressStatus));
    }

    void InputAdapter::UpdateInputForMenu()
    {
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        for (u32 playerIndex = 0; playerIndex < JOY_MAX_COUNT; ++playerIndex)
        {
            UpdateActionForButton(playerIndex, ActionJump, m_joyButton_A);
            UpdateActionForButton(playerIndex, ActionBack, m_joyButton_B);
            UpdateActionForAxis(playerIndex, ActionLeft, m_joyStickLeft_X, -1);
            UpdateActionForAxis(playerIndex, ActionRight, m_joyStickLeft_X, 1);
            UpdateActionForAxis(playerIndex, ActionUp, m_joyStickLeft_Y, 1);
            UpdateActionForAxis(playerIndex, ActionDown, m_joyStickLeft_Y, -1);

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
        ITF_ASSERT(m_inputManagerInitialized && m_inputManager && m_inputManager->IsInitialized());

        for (u32 playerIndex = 0; playerIndex < JOY_MAX_COUNT; ++playerIndex)
        {
            if (m_connectedPlayers[playerIndex] == ePlaying || m_connectedPlayers[playerIndex] == eBubble)
            {
                bbool pressed = bfalse;
                pressed |= UpdateActionForButton(playerIndex, ActionJump, m_joyButton_A);
                pressed |= UpdateActionForButton(playerIndex, ActionHit, m_joyButton_X);
                pressed |= UpdateActionForButton(playerIndex, ActionBack, m_joyButton_B);
                pressed |= UpdateActionForButton(playerIndex, ActionShowMenu, m_joyButton_Start);
                UpdateActionForAxis(playerIndex, ActionLeft, m_joyStickLeft_X, -1);
                UpdateActionForAxis(playerIndex, ActionRight, m_joyStickLeft_X, 1);
                UpdateActionForAxis(playerIndex, ActionDown, m_joyStickLeft_Y, -1);
                UpdateActionForAxis(playerIndex, ActionUp, m_joyStickLeft_Y, 1);
                UpdateActionForAxis(playerIndex, ActionSprint, m_joyTrigger_Right, 1);

                if (pressed && m_connectedPlayers[playerIndex] == eBubble)
                {
                    m_connectedPlayers[playerIndex] = ePlaying;
                }
            }
            else //player should join on any pressed key/button/axis
            {
                bbool pressed = bfalse;

                pressed |= UpdateActionForButton(playerIndex, ActionJump, m_joyButton_A);
                pressed |= UpdateActionForButton(playerIndex, ActionHit, m_joyButton_X);
                pressed |= UpdateActionForButton(playerIndex, ActionBack, m_joyButton_B);
                pressed |= UpdateActionForButton(playerIndex, ActionShowMenu, m_joyButton_Start);
                pressed |= UpdateActionForAxis(playerIndex, ActionLeft, m_joyStickLeft_X, -1);
                pressed |= UpdateActionForAxis(playerIndex, ActionRight, m_joyStickLeft_X, 1);
                pressed |= UpdateActionForAxis(playerIndex, ActionDown, m_joyStickLeft_Y, -1);
                pressed |= UpdateActionForAxis(playerIndex, ActionUp, m_joyStickLeft_Y, 1);
                pressed |= UpdateActionForAxis(playerIndex, ActionSprint, m_joyTrigger_Right, 1);

                if (pressed)
                {
                    m_connectedPlayers[playerIndex] = eBubble;
                    setPadConnected(playerIndex, btrue);

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

    void InputAdapter::OnControllerConnected(u32 _padIndex, i32 _deviceID, i32 _deviceOutputID, bool isSony)
    {
#ifdef USE_PAD_HAPTICS
        HAPTICS_MANAGER->onControllerConnected(_padIndex, _deviceID, _deviceOutputID, isSony);
#endif
    }

    void InputAdapter::OnControllerDisconnected(u32 _padIndex)
    {
#ifdef USE_PAD_HAPTICS
        HAPTICS_MANAGER->onControllerDisconnected(_padIndex);
#endif
    }

    const InputValue& InputAdapter::GetInputValue(u32 player, u32 action) const
    {
        return m_inputMapping[player][action];
    }

    void InputAdapter::InitializeInputManager()
    {
        if (m_inputManagerInitialized)
        {
            LOG("[InputAdapter] InitializeInputManager: Already initialized");
            return;
        }

        LOG("[InputAdapter] InitializeInputManager: Starting initialization");

        static InputManager* s_inputManagerInstance = nullptr;
        if (!s_inputManagerInstance)
        {
            LOG("[InputAdapter] InitializeInputManager: Creating InputManager instance");
            s_inputManagerInstance = new InputManager();
        }

        m_inputManager = InputManager::getptr();
        ITF_ASSERT(m_inputManager != nullptr);

        if (m_inputManager)
        {
            LOG("[InputAdapter] InitializeInputManager: InputManager instance obtained, calling Initialize()");
            m_inputManager->Initialize();
            m_inputManagerInitialized = btrue;
            LOG("[InputAdapter] InitializeInputManager: Initialization complete");
        }
        else
        {
            LOG("[InputAdapter] InitializeInputManager: ERROR - InputManager is null!");
        }
    }
} // namespace ITF
