#include "precompiled_engine.h"

#if defined(ITF_WINDOWS)

#ifndef _ITF_ZPAD_PCKEYBOARD_H_
#include "engine/zinput/pc/ZPad_PCKeyboard.h"
#endif //_ITF_ZPAD_PCKEYBOARD_H_

namespace ITF
{
    namespace
    {
        struct KeyboardButtonMapping
        {
            u32 buttonIndex;
            u32 controlIndex;
        };

        struct KeyboardAxisMapping
        {
            u32 axisIndex;
            f32 threshold;
            bbool positive;
            u32 controlIndex;
        };

        const KeyboardButtonMapping kKeyboardButtonMappings[] = {
            { m_joyButton_DPadU,      ZPad_Base::DPAD_UP },
            { m_joyButton_DPadD,      ZPad_Base::DPAD_DOWN },
            { m_joyButton_DPadL,      ZPad_Base::DPAD_LEFT },
            { m_joyButton_DPadR,      ZPad_Base::DPAD_RIGHT },
            { m_joyButton_A,          ZPad_Base::BUTTON_FACE_SOUTH },
            { m_joyButton_B,          ZPad_Base::BUTTON_FACE_EAST },
            { m_joyButton_X,          ZPad_Base::BUTTON_FACE_WEST },
            { m_joyButton_Y,          ZPad_Base::BUTTON_FACE_NORTH },
            { m_joyButton_LB,         ZPad_Base::BUTTON_L_SHOULDER },
            { m_joyButton_RB,         ZPad_Base::BUTTON_R_SHOULDER },
            { m_joyButton_ThumbLeft,  ZPad_Base::BUTTON_L_THUMB },
            { m_joyButton_ThumbRight, ZPad_Base::BUTTON_R_THUMB },
            { m_joyButton_Start,      ZPad_Base::BUTTON_START },
            { m_joyButton_Back,       ZPad_Base::BUTTON_SELECT },
        };

        const KeyboardAxisMapping kKeyboardAxisMappings[] = {
            { m_joyTrigger_Left,   0.5f, btrue,  ZPad_Base::TRIGGER_LEFT },
            { m_joyTrigger_Right,  0.5f, btrue,  ZPad_Base::TRIGGER_RIGHT },
            { m_joyStickLeft_Y,    0.5f, btrue,  ZPad_Base::STICK_L_UP },
            { m_joyStickLeft_Y,    0.5f, bfalse, ZPad_Base::STICK_L_DOWN },
            { m_joyStickLeft_X,    0.5f, bfalse, ZPad_Base::STICK_L_LEFT },
            { m_joyStickLeft_X,    0.5f, btrue,  ZPad_Base::STICK_L_RIGHT },
            { m_joyStickRight_Y,   0.5f, btrue,  ZPad_Base::STICK_R_UP },
            { m_joyStickRight_Y,   0.5f, bfalse, ZPad_Base::STICK_R_DOWN },
            { m_joyStickRight_X,   0.5f, bfalse, ZPad_Base::STICK_R_LEFT },
            { m_joyStickRight_X,   0.5f, btrue,  ZPad_Base::STICK_R_RIGHT },
        };

        ITF_INLINE bbool IsKeyboardButtonActive(InputAdapter::PressStatus status)
        {
            return status == InputAdapter::Pressed || status == InputAdapter::JustPressed;
        }

        ITF_INLINE bbool IsKeyboardAxisActive(f32 value, f32 threshold, bbool positive)
        {
            return positive ? (value > threshold) : (value < -threshold);
        }
    }

    ZPad_Base::PhysicalButtonMapping ZPad_PCKeyboard::GetPhysicalButtonMapping() const
    {
        PhysicalButtonMapping mapping;
        mapping.faceButtonSouth = m_joyButton_A;
        mapping.faceButtonEast  = m_joyButton_B;
        mapping.faceButtonWest  = m_joyButton_X;
        mapping.faceButtonNorth = m_joyButton_Y;
        mapping.shoulderLeft    = m_joyButton_LB;
        mapping.shoulderRight   = m_joyButton_RB;
        mapping.selectButton    = m_joyButton_Back;
        mapping.startButton     = m_joyButton_Start;
        return mapping;
    }

    void ZPad_PCKeyboard::UpdateDeviceInfo(SDeviceInfo& deviceInfo)
    {
        u32 count = Min((u32)m_deviceInfo.m_inputInfo.size(), (u32)deviceInfo.m_inputInfo.size());
        for (u32 i = 0; i < count; ++i)
        {
            deviceInfo.m_inputInfo[i].m_type = m_deviceInfo.m_inputInfo[i].m_type;
        }

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        float axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getKeyboardPadButtons(m_id, buttons, JOY_MAX_BUT);
        INPUT_ADAPTER->getKeyboardPadPos(m_id, axes, JOY_MAX_AXES);

        UpdateAxisValues(deviceInfo, axes);
        UpdateButtonStates(deviceInfo, buttons);
        UpdatePlatformSpecificControls(deviceInfo, buttons, axes);
        ApplyRemapping(deviceInfo);
        ApplyDirectionRemapping(deviceInfo, axes, buttons);
    }

    bbool ZPad_PCKeyboard::IsSourceAllowed() const
    {
        return INPUT_ADAPTER && INPUT_ADAPTER->IsKeyboardMouseEnabled();
    }

    u32 ZPad_PCKeyboard::GetFirstActiveControl() const
    {
        if (!INPUT_ADAPTER || !IsSourceAllowed())
        {
            return U32_INVALID;
        }

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getKeyboardPadButtons(m_id, buttons, JOY_MAX_BUT);
        float axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getKeyboardPadPos(m_id, axes, JOY_MAX_AXES);

        for (const auto& mapping : kKeyboardButtonMappings)
        {
            if (IsKeyboardButtonActive(buttons[mapping.buttonIndex]))
            {
                return mapping.controlIndex;
            }
        }

        for (const auto& mapping : kKeyboardAxisMappings)
        {
            if (IsKeyboardAxisActive(axes[mapping.axisIndex], mapping.threshold, mapping.positive))
            {
                return mapping.controlIndex;
            }
        }

        return U32_INVALID;
    }

} // namespace ITF

#endif // defined(ITF_WINDOWS)
