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
#endif
namespace ITF
{
    const f32 InputAdapter::fDoublePressMaxDuration = 0.2f;

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
        m_focused(true),
        m_keyboardShareEnabled(btrue),
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
        
        // Initialize last used input device tracking
        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            m_lastUsedInputDevice[i] = InputDevice_None;
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
        ITF_UNUSED(_listener);
        ITF_UNUSED(_priority);
    }

    void InputAdapter::removeListener(Interface_InputListener* _listener)
    {
        ITF_UNUSED(_listener);
    }

    InputAdapter::~InputAdapter() = default;

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
        platformUpdateKeyboardState();
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

    void InputAdapter::padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        ITF_UNUSED(_numPad);
        ITF_UNUSED(_leftMotorSpeed);
        ITF_UNUSED(_rightMotorSpeed);
    }

    bbool InputAdapter::isMousePressed(MouseButton _but) const
    {
        ITF_UNUSED(_but);
        return bfalse;
    }

    void InputAdapter::dispatchEventsToListeners()
    {
    }

    void InputAdapter::platformUpdateKeyboardState()
    {
    }

    void InputAdapter::ResetInputState()
    {
        memset(m_axes, 0, JOY_MAX_COUNT * JOY_MAX_AXES * sizeof(float));
        memset(m_buttons, 0, JOY_MAX_COUNT * JOY_MAX_BUT * sizeof(PressStatus));
    }

    void InputAdapter::OnControllerConnected(u32 _padIndex,i32 _deviceID,i32 _deviceOutputID,bool isSony)
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

    InputAdapter::PressStatus InputAdapter::getKeyboardStatus(i32 key) const
    {
        ITF_UNUSED(key);
        return Released;
    }

    u32 InputAdapter::getKeyboardPressTime(i32 key) const
    {
        ITF_UNUSED(key);
        return std::numeric_limits<u32>::max();
    }
} // namespace ITF
