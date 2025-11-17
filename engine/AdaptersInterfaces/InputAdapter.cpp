#include "precompiled_engine.h"

#include "gameplay/managers/GameManager.h"
#ifdef USE_PAD_HAPTICS
#include "gameplay/Managers/PadHapticsManager.h"
#endif
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
        m_environmentInput = EnvironmentEngine | EnvironmentLua;
        m_buttonMode = MixedMode;

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
        ITF_UNUSED(_but);
        ITF_UNUSED(_status);
    }

    void InputAdapter::onMouseWheel(i32 _wheelValue)
    {
        ITF_UNUSED(_wheelValue);
    }

    void InputAdapter::flushKeys()
    {
    }


    void InputAdapter::onKey(i32 _key, InputAdapter::PressStatus _status)
    {
        ITF_UNUSED(_key);
        ITF_UNUSED(_status);
    }

    bbool InputAdapter::isKeyPressed(i32 _key) const
    {
        ITF_UNUSED(_key);
        return bfalse;
    }

    void InputAdapter::onMousePos(i32 _x, i32 _y)
    {
        ITF_UNUSED(_x);
        ITF_UNUSED(_y);
    }

    void InputAdapter::getMousePos(i32& _x, i32& _y) const
    {
        _x = 0;
        _y = 0;
    }

    void InputAdapter::updateAllInputState()
    {
        ResetInputState();
        UpdatePads();
        UpdateAdditionalInputs();
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
        if (CONFIG->m_enableWiiRemoteonPC)
        {
            if ((m_environmentInput & _environment) != 0 &&
                KPinput.m_pads[_pad].isConnected() && KPinput.m_pads[_pad].isActive())
            {
                for (u32 i = 0; i < _numAxes; i++)
                    _pos[i] = KPinput.m_pads[_pad].getAxe(i);
            }
        }
#endif //USE_WIIMOTE_LIB
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
        if (CONFIG->m_enableWiiRemoteonPC)
        {
            if ((m_environmentInput & _environment) != 0)
            {
                if (KPinput.m_pads[_pad].isConnected() && KPinput.m_pads[_pad].isActive())
                    for (u32 i = 0; i < _numButtons; i++)
                        _buttons[i] = KPinput.m_pads[_pad].getButton(i);
            }
        }
#endif //USE_WIIMOTE_LIB
    }

    // Speed between 0-1
    void InputAdapter::padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        // default implementation: ?
    }

    bbool InputAdapter::isMousePressed(MouseButton _but) const
    {
        ITF_UNUSED(_but);
        return bfalse;
    }

    void InputAdapter::dispatchEventsToListeners()
    {
    }


    void InputAdapter::LoadPlayerControlSettings()
    {
        ResetToDefaultControls();
        SaveInputMapping();
        //TODO: will be implemented later
        CopyInputMapping();
    }

    void InputAdapter::SavePlayerControlSettings()
    {
        //TODO: will be implemented later
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

    bbool InputAdapter::UpdateActionForButton(u32 player, ActionType action, JoyButton_Common button)
    {
        InputValue val = m_inputMapping[player][action];

        if (val.inputType == Keyboard)
        {
            m_buttons[player][button] = GetKeyboardStatusInternal(val.inputValue);
        }
        else if (val.inputType == X360Button || val.inputType == GenericButton)
        {
            m_buttons[player][button] = GetButtonStatus(val);
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

        if (m_buttons[player][button] == Released)
        {
            PressStatus overrideState = GetMenuButtonOverride(player, action, button);
            if (overrideState != Released)
            {
                m_buttons[player][button] = overrideState;
            }
        }

        return m_buttons[player][button] == JustPressed;
    }

    bbool InputAdapter::UpdateActionForAxis(u32 player, ActionType action, JoyAxis_t axis, f32 axisValue)
    {
        InputValue val = m_inputMapping[player][action];

        if (val.inputType == Keyboard)
        {
            PressStatus keyStatus = GetKeyboardStatusInternal(val.inputValue);
            u32 keyPressTime = GetKeyboardPressTimeInternal(val.inputValue);
            if ((keyStatus == JustPressed || keyStatus == Pressed) &&
                m_axesPressTime[player][axis] > keyPressTime)
            {
                m_axesPressTime[player][axis] = keyPressTime;
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

    void InputAdapter::UpdateAdditionalInputs()
    {
    }

    InputAdapter::PressStatus InputAdapter::GetKeyboardStatusInternal(u32 key) const
    {
        ITF_UNUSED(key);
        return Released;
    }

    u32 InputAdapter::GetKeyboardPressTimeInternal(u32 key) const
    {
        ITF_UNUSED(key);
        return 0xFFFFFFFFu;
    }

    void InputAdapter::ApplyMenuKeyboardOverrides(u32 playerIndex)
    {
        ITF_UNUSED(playerIndex);
    }

    bbool InputAdapter::ShouldProcessBubbleQuit(u32 playerIndex) const
    {
        ITF_UNUSED(playerIndex);
        return btrue;
    }

    InputAdapter::PressStatus InputAdapter::GetMenuButtonOverride(
        u32 player, ActionType action, JoyButton_Common button) const
    {
        ITF_UNUSED(player);
        ITF_UNUSED(action);
        ITF_UNUSED(button);
        return Released;
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

            ApplyMenuKeyboardOverrides(playerIndex);
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
                if (ShouldProcessBubbleQuit(playerIndex))
                {
                    if (UpdateActionForButton(playerIndex, ActionBubbleQuit, m_joyButton_Back))
                    {
                        if (m_connectedPlayers[playerIndex] == ePlaying)
                        {
                            m_connectedPlayers[playerIndex] = eBubble;
                        }
                        else if (m_connectedPlayers[playerIndex] == eBubble)
                        {
                            m_connectedPlayers[playerIndex] = eNotConnected;
                            setPadConnected(playerIndex, bfalse);
                        }
                    }
                }
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

    void InputAdapter:: OnControllerConnected(u32 _padIndex,i32 _deviceID,i32 _deviceOutputID,bool isSony)
    {
#ifdef USE_PAD_HAPTICS
        HAPTICS_MANAGER->onControllerConnected(_padIndex,_deviceID,_deviceOutputID,isSony);
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
} // namespace ITF
