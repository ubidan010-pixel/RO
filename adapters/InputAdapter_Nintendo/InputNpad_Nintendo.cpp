#include "precompiled_InputAdapter_Nintendo.h"

#include "adapters/InputAdapter_Nintendo/InputAdapter_Nintendo.h"
#include "adapters/InputAdapter_Nintendo/InputNPad_Nintendo.h"

#include <nn/hid.h>

#include <nn/hid/hid_NpadJoyCommon.h>
#include <nn/hid/hid_NpadJoyDual.h>
#include <nn/hid/hid_NpadJoyLeft.h>
#include <nn/hid/hid_NpadJoyRight.h>

#include "core/math/MathTools.h"

namespace ITF
{
    /*
     * The right pad is the reference pad, except for:
     *    - the stick: it's considered as a left stick on both pad, for navigating into menus
     *    - the shoulder button: it's used as a "Left shoulder" button (= "LB" or "L") on both pad
     *    - the trigger button: it's used as a "Right shoulder" button (= "RB" or "R") on both pad
     */

    static const u32 m_joyButton_None = U32_INVALID;

    static const u32 ms_horizontalSingleButtonMappings[InputNpad_Nintendo::ESide_Count][m_joyButton_NxCount] = {
        // Left pad
        {
            //  A,                  B,                          X,                      Y
            m_joyButton_DPadD,
            m_joyButton_DPadL,
            m_joyButton_DPadR,
            m_joyButton_DPadU,  // mapping

            //  LB,                 RB,                         Back,                   Start
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_Minus,

            //  DPadL,              DPadR,                      DPadU,                  DPadD
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_None,

            //  Stick_Left,         Stick_Right,                ThumbLeft,              ThumbRight
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_ThumbLeft,
            m_joyButton_None,

            //  TriggerLeft,        TriggerRight,               SL,                     SR
            m_joyButton_LeftSL,
            m_joyButton_LeftSR,
            m_joyButton_None,
            m_joyButton_None,
        },

        // Right pad
        {
            //  A,                  B,                          X,                      Y
            m_joyButton_X,
            m_joyButton_A,
            m_joyButton_Y,
            m_joyButton_B,

            //  LB,                 RB,                         Back,                   Start
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_Plus,

            //  DPadL,              DPadR,                      DPadU,                  DPadD
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_None,

            //  Stick_Left,         Stick_Right,                ThumbLeft,              ThumbRight
            m_joyButton_None,
            m_joyButton_None,
            m_joyButton_ThumbRight,
            m_joyButton_None,

            //  TriggerLeft,        TriggerRight,               SL,                     SR
            m_joyButton_RightSL,
            m_joyButton_RightSR,
            m_joyButton_None,
            m_joyButton_None,
        },
    };

    InputNpad_Nintendo::InputNpad_Nintendo(u32 _id, nn::hid::NpadIdType _npadId)
        : InputPad_Nintendo(_id),
          m_npadId(_npadId),
          m_currentSide(ESide_None),
          m_vibrationEndTime(0.0),
          m_mostRecentlyReadSamplingNumber(INT64_MIN),
          m_lastUsedTime(0.0)
    {
        struct SideConfiguration
        {
            ESide m_side;
            nn::hid::NpadStyleSet m_style;
        };

        const SideConfiguration sideConfigurations[5] = {{ESide_Left, nn::hid::NpadStyleJoyLeft::Mask},
                                                         {ESide_Right, nn::hid::NpadStyleJoyRight::Mask},
                                                         {ESide_Dual, nn::hid::NpadStyleJoyDual::Mask},
                                                         {ESide_FullKey, nn::hid::NpadStyleFullKey::Mask},
                                                         {ESide_Handheld, nn::hid::NpadStyleHandheld::Mask}};

        for (const SideConfiguration& conf : sideConfigurations)
        {
            int nbHandles = nn::hid::GetVibrationDeviceHandles(m_vibrationHandles[conf.m_side], VibrationHandlesMax, m_npadId, conf.m_style);
            ITF_ASSERT(nbHandles <= VibrationHandlesMax);

            for (int i = 0; i < nbHandles; i++)
                nn::hid::InitializeVibrationDevice(m_vibrationHandles[conf.m_side][i]);
        }

        if (m_npadId == nn::hid::NpadId::Handheld)
        {
            m_currentSide = ESide_Handheld;
        }

        m_first = btrue;
    }

    InputNpad_Nintendo::~InputNpad_Nintendo() {}

    const char* InputNpad_Nintendo::GetTypeName() const
    {
        switch (m_currentSide)
        {
        case ESide_Left:
            return "Left Joycon";
        case ESide_Right:
            return "Right Joycon";
        case ESide_Dual:
            return "Dual Joycon";
        case ESide_FullKey:
            return "Pro Controler";
        case ESide_Handheld:
            return "Handheld";
        default:
            return "Unknown";
        }
    }

    void InputNpad_Nintendo::getGamePadPos(u32 _environment, float* _pos, u32 _numAxes) const
    {
        // The stick on the right JOYCON is considered as a left stick, so we can navigate in the menus with it
        if (m_currentSide == ESide_Right)
        {
            float allAxes[JOY_MAX_AXES];
            InputPad_Nintendo::getGamePadPos(_environment, allAxes, ITF_ARRAY_SIZE(allAxes));

            ITF::Swap(allAxes[m_joyStickLeft_X], allAxes[m_joyStickRight_X]);
            ITF::Swap(allAxes[m_joyStickLeft_Y], allAxes[m_joyStickRight_Y]);

            ITF_Memcpy(_pos, allAxes, _numAxes * sizeof(*_pos));
        }
        else
        {
            InputPad_Nintendo::getGamePadPos(_environment, _pos, _numAxes);
        }
    }

    InputAdapter::PressStatus InputNpad_Nintendo::getGamePadButton(u32 _environment, InputAdapter::ButtonMode _buttonMode, u32 _buttonID) const
    {
        ITF_ASSERT(m_currentSide != ESide_None);
        if ((m_currentSide != ESide_None) && _buttonID < ITF_ARRAY_SIZE(ms_horizontalSingleButtonMappings[m_currentSide]))
        {
            u32 buttonID = _buttonID;
            if (m_currentSide == ESide_Left || m_currentSide == ESide_Right)
                buttonID = ms_horizontalSingleButtonMappings[m_currentSide][_buttonID];
            if (buttonID != m_joyButton_None)
            {
                return InputPad_Nintendo::getGamePadButton(_environment, _buttonMode, buttonID);
            }
        }

        return InputAdapter::Released;
    }

    void InputNpad_Nintendo::UpdateLastUsedTime()
    {
        for (int i = 0; i < m_joyButton_NxCount; i++)
        {
            if (m_buttonStatus[i] == InputAdapter::Pressed)
            {
                m_lastUsedTime = SYSTEM_ADAPTER->getTime();
            }
        }
    }

    void InputNpad_Nintendo::UpdateHandheld()
    {
        const nn::hid::NpadIdType npadId = nn::hid::NpadId::Handheld;
        nn::hid::NpadStyleSet npadStyleSet = nn::hid::GetNpadStyleSet(npadId);

        nn::hid::NpadHandheldState padState = {};

        bool connected = npadStyleSet.Test<nn::hid::NpadStyleHandheld>();
        setConnected(connected);
        if (isConnected())
        {
            nn::hid::GetNpadState(&padState, npadId);
        }

        UpdateNpad(padState.buttons, padState.analogStickL, padState.analogStickR);

        UpdateVibration(m_currentSide);
        UpdateLastUsedTime();
    }

    void InputNpad_Nintendo::UpdateWireless()
    {
        bool previousConnected = isConnected();
        bool isFirstUpdate = m_first;

        ESide oldSide = m_currentSide;

        nn::hid::NpadStyleSet npadStyleSet = nn::hid::GetNpadStyleSet(m_npadId);
        if (npadStyleSet.Test<nn::hid::NpadStyleJoyLeft>())
            m_currentSide = ESide_Left;
        else if (npadStyleSet.Test<nn::hid::NpadStyleJoyRight>())
            m_currentSide = ESide_Right;
        else if (npadStyleSet.Test<nn::hid::NpadStyleFullKey>())
            m_currentSide = ESide_FullKey;
        else if (npadStyleSet.Test<nn::hid::NpadStyleJoyDual>())
            m_currentSide = ESide_Dual;

        UpdateButtonsAndAxis(oldSide);
        UpdateVibration(oldSide);

        if (!previousConnected && isConnected() && !isFirstUpdate)
        {
            INPUT_ADAPTER_NINTENDO->requestShowControllerApplet();
        }
        UpdateLastUsedTime();
    }

    void InputNpad_Nintendo::Update()
    {
        if (m_npadId == nn::hid::NpadId::Handheld)
        {
            UpdateHandheld();
        }
        else
        {
            UpdateWireless();
        }
    }

    void InputNpad_Nintendo::setVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time)
    {
        bool proceed = false;

        // PBA: Lowered default low frequency (from 160 to 110)
        const int lowFrequency = 110 /* nn::hid::VibrationFrequencyLowDefault */;

        if (m_currentSide == ESide_Left || m_currentSide == ESide_Right)
        {
            f32 amplitude = Clamp(m_currentSide == ESide_Left ? _leftMotorSpeed : _rightMotorSpeed, 0.0f, 1.0f);

            nn::hid::VibrationValue value = nn::hid::VibrationValue::Make(amplitude, lowFrequency, 0.0f, nn::hid::VibrationFrequencyHighDefault);
            nn::hid::SendVibrationValue(m_vibrationHandles[m_currentSide][0], value);  // Only one vibrator in a single-side joy-con

            proceed = (amplitude > 0.0f);
        }
        else if (m_currentSide == ESide_Handheld || m_currentSide == ESide_Dual || m_currentSide == ESide_FullKey)  // i.e. Pro Controller
        {
            for (int i = 0; i < 2; i++)  // Two vibrators in a Dual or Full 'joy-con', or in Handheld mode
            {
                f32 amplitude = Clamp(i == 0 ? _leftMotorSpeed : _rightMotorSpeed, 0.0f, 1.0f);

                nn::hid::VibrationValue value = nn::hid::VibrationValue::Make(amplitude, lowFrequency, 0.0f, nn::hid::VibrationFrequencyHighDefault);
                nn::hid::SendVibrationValue(m_vibrationHandles[m_currentSide][i], value);

                proceed |= (amplitude > 0.0f);
            }
        }

        if (proceed)
        {
            auto currentTimeInFrame = SYSTEM_ADAPTER->getTime();
            m_vibrationEndTime = currentTimeInFrame + Clamp(_time, 0.0, 10.0);
        }
        else
        {
            m_vibrationEndTime = 0.0;
        }
    }

    void InputNpad_Nintendo::UpdateButtonsAndAxis(ESide _oldSide)
    {
        nn::hid::NpadButtonSet buttons = {};
        nn::hid::AnalogStickState analogStickL = {};
        nn::hid::AnalogStickState analogStickR = {};
        nn::hid::NpadAttributesSet attributes = {};

        if (m_currentSide == ESide_Dual)
        {
            nn::hid::NpadJoyDualState padState = {};
            nn::hid::GetNpadState(&padState, m_npadId);

            buttons = padState.buttons;
            analogStickL = padState.analogStickL;
            analogStickR = padState.analogStickR;
            attributes = padState.attributes;
            if (!m_first && attributes != m_prevAttributes)
            {
                INPUT_ADAPTER_NINTENDO->requestShowControllerApplet();
            }
        }
        else if (m_currentSide == ESide_FullKey)
        {
            nn::hid::NpadFullKeyState padState = {};
            nn::hid::GetNpadState(&padState, m_npadId);

            buttons = padState.buttons;
            analogStickL = padState.analogStickL;
            analogStickR = padState.analogStickR;
            attributes = padState.attributes;
        }
        else if (m_currentSide == ESide_Left)
        {
            nn::hid::NpadJoyLeftState padState = {};
            nn::hid::GetNpadState(&padState, m_npadId);

            buttons = padState.buttons;
            analogStickL = padState.analogStickL;
            analogStickR = padState.analogStickR;
            attributes = padState.attributes;

            // horizontal !
            float lrealX = analogStickL.x;
            float lrealY = analogStickL.y;
            analogStickL.x = -lrealY;
            analogStickL.y = lrealX;

            float rrealX = analogStickR.x;
            float rrealY = analogStickR.y;
            analogStickR.x = -rrealY;
            analogStickR.y = rrealX;
        }
        else if (m_currentSide == ESide_Right)
        {
            nn::hid::NpadJoyRightState padState = {};
            nn::hid::GetNpadState(&padState, m_npadId);

            buttons = padState.buttons;
            analogStickL = padState.analogStickL;
            analogStickR = padState.analogStickR;
            attributes = padState.attributes;

            // horizontal !
            float lrealX = analogStickL.x;
            float lrealY = analogStickL.y;
            analogStickL.x = lrealY;
            analogStickL.y = -lrealX;

            float rrealX = analogStickR.x;
            float rrealY = analogStickR.y;
            analogStickR.x = rrealY;
            analogStickR.y = -rrealX;
        }

        m_prevAttributes = attributes;
        m_first = bfalse;

        bool connected = attributes.Test<nn::hid::NpadAttribute::IsConnected>();
        setConnected(connected);

        ITF_ASSERT_MSG(!connected || m_currentSide != ESide_None, "%s %u is connected with no side", GetTypeName(), m_id);
        ITF_ASSERT_MSG(connected || buttons.IsAllOff(), "%s %u is unconnected with pressed buttons", GetTypeName(), m_id);
        ITF_ASSERT_MSG(connected || analogStickL.x == 0, "%s %u is unconnected with a value on the X axis of its left stick", GetTypeName(), m_id);
        ITF_ASSERT_MSG(connected || analogStickL.y == 0, "%s %u is unconnected with a value on the Y axis of its left stick", GetTypeName(), m_id);
        ITF_ASSERT_MSG(connected || analogStickR.x == 0, "%s %u is unconnected with a value on the X axis of its right stick", GetTypeName(), m_id);
        ITF_ASSERT_MSG(connected || analogStickR.y == 0, "%s %u is unconnected with a value on the Y axis of its right stick", GetTypeName(), m_id);
        ITF_ASSERT_MSG(connected || attributes.IsAllOff(), "%s %u is unconnected with attribute flags set", GetTypeName(), m_id);

        UpdateNpad(buttons, analogStickL, analogStickR);
    }

    void InputNpad_Nintendo::UpdateVibration(ESide _oldSide)
    {
        if (isVibrating())
        {
            if (m_currentSide != _oldSide || SYSTEM_ADAPTER->getTime() >= m_vibrationEndTime)
            {
                stopVibration();
            }
        }
    }

    void InputNpad_Nintendo::UpdateNpad(const nn::hid::NpadButtonSet& _buttonSet,
                                  const nn::hid::AnalogStickState& _analogStickL,
                                  const nn::hid::AnalogStickState& _analogStickR)
    {
        UpdateNpadButtons(_buttonSet);
        UpdateNpadSticks(_analogStickL, _analogStickR);
    }

    void InputNpad_Nintendo::UpdateNpadButtons(const nn::hid::NpadButtonSet& _buttonSet)
    {
        UpdateNpadButton<nn::hid::NpadButton::A>(_buttonSet, m_joyButton_A);
        UpdateNpadButton<nn::hid::NpadButton::B>(_buttonSet, m_joyButton_B);
        UpdateNpadButton<nn::hid::NpadButton::X>(_buttonSet, m_joyButton_X);
        UpdateNpadButton<nn::hid::NpadButton::Y>(_buttonSet, m_joyButton_Y);
        UpdateNpadButton<nn::hid::NpadButton::StickL>(_buttonSet, m_joyButton_ThumbLeft);
        UpdateNpadButton<nn::hid::NpadButton::StickR>(_buttonSet, m_joyButton_ThumbRight);
        UpdateNpadButton<nn::hid::NpadButton::L>(_buttonSet, m_joyButton_LB);
        UpdateNpadButton<nn::hid::NpadButton::R>(_buttonSet, m_joyButton_RB);
        UpdateNpadButton<nn::hid::NpadButton::ZL>(_buttonSet, m_joyButton_TriggerLeft);
        UpdateNpadButton<nn::hid::NpadButton::ZR>(_buttonSet, m_joyButton_TriggerRight);
        UpdateNpadButton<nn::hid::NpadButton::Plus>(_buttonSet, m_joyButton_Plus);
        UpdateNpadButton<nn::hid::NpadButton::Minus>(_buttonSet, m_joyButton_Minus);
        UpdateNpadButton<nn::hid::NpadButton::Left>(_buttonSet, m_joyButton_DPadL);
        UpdateNpadButton<nn::hid::NpadButton::Up>(_buttonSet, m_joyButton_DPadU);
        UpdateNpadButton<nn::hid::NpadButton::Right>(_buttonSet, m_joyButton_DPadR);
        UpdateNpadButton<nn::hid::NpadButton::Down>(_buttonSet, m_joyButton_DPadD);
        UpdateNpadButton<nn::hid::NpadJoyButton::LeftSL>(_buttonSet, m_joyButton_LeftSL);
        UpdateNpadButton<nn::hid::NpadJoyButton::LeftSR>(_buttonSet, m_joyButton_LeftSR);
        UpdateNpadButton<nn::hid::NpadJoyButton::RightSL>(_buttonSet, m_joyButton_RightSL);
        UpdateNpadButton<nn::hid::NpadJoyButton::RightSR>(_buttonSet, m_joyButton_RightSR);
    }

    void InputNpad_Nintendo::UpdateNpadSticks(const nn::hid::AnalogStickState& _analogStickL, const nn::hid::AnalogStickState& _analogStickR)
    {
        m_axesPos[m_joyStickLeft_X] = static_cast<f32>(_analogStickL.x) / static_cast<f32>(nn::hid::AnalogStickMax);
        m_axesPos[m_joyStickLeft_Y] = static_cast<f32>(_analogStickL.y) / static_cast<f32>(nn::hid::AnalogStickMax);
        m_axesPos[m_joyStickRight_X] = static_cast<f32>(_analogStickR.x) / static_cast<f32>(nn::hid::AnalogStickMax);
        m_axesPos[m_joyStickRight_Y] = static_cast<f32>(_analogStickR.y) / static_cast<f32>(nn::hid::AnalogStickMax);
    }

}  // namespace ITF
