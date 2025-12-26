#include "precompiled_engine.h"

#if defined(ITF_WINDOWS)

#ifndef _ITF_ZPAD_PCKEYBOARD_H_
#include "engine/zinput/pc/ZPad_PCKeyboard.h"
#endif //_ITF_ZPAD_PCKEYBOARD_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif

namespace ITF
{
    namespace
    {
        struct KeyboardButtonMapping
        {
            u32 buttonIndex;
            u32 controlIndex;
            i32 defaultKey;
        };

        struct KeyboardAxisMapping
        {
            u32 axisIndex;
            f32 threshold;
            bbool positive;
            u32 controlIndex;
        };

        const KeyboardButtonMapping kKeyboardButtonMappings[] = {

            { m_joyButton_DPadU,      ZPad_Base::DPAD_UP,           KEY_UP },
            { m_joyButton_DPadD,      ZPad_Base::DPAD_DOWN,         KEY_DOWN },
            { m_joyButton_DPadL,      ZPad_Base::DPAD_LEFT,         KEY_LEFT },
            { m_joyButton_DPadR,      ZPad_Base::DPAD_RIGHT,        KEY_RIGHT },

            { m_joyButton_DPadU,      ZPad_Base::STICK_L_UP,        KEY_UP },
            { m_joyButton_DPadD,      ZPad_Base::STICK_L_DOWN,      KEY_DOWN },
            { m_joyButton_DPadL,      ZPad_Base::STICK_L_LEFT,      KEY_LEFT },
            { m_joyButton_DPadR,      ZPad_Base::STICK_L_RIGHT,     KEY_RIGHT },

            { m_joyButton_A,          ZPad_Base::BUTTON_FACE_SOUTH, KEY_SPACE },
            { m_joyButton_B,          ZPad_Base::BUTTON_FACE_EAST,  KEY_BACKSPACE },
            { m_joyButton_X,          ZPad_Base::BUTTON_FACE_WEST,  'S' },
            { m_joyButton_Y,          ZPad_Base::BUTTON_FACE_NORTH, 'F' },

            { m_joyButton_LB,         ZPad_Base::BUTTON_L_SHOULDER, 'Q' },
            { m_joyButton_RB,         ZPad_Base::BUTTON_R_SHOULDER, 'E' },

            { m_joyButton_ThumbLeft,  ZPad_Base::BUTTON_L_THUMB,    -1 },
            { m_joyButton_ThumbRight, ZPad_Base::BUTTON_R_THUMB,    -1 },

            { m_joyButton_Start,      ZPad_Base::BUTTON_START,      KEY_ESC },
            { m_joyButton_Back,       ZPad_Base::BUTTON_SELECT,     -1 },
        };

        struct KeyboardAxisButtonMapping
        {
            u32 axisIndex;
            u32 controlIndex;
            i32 defaultKey;
            f32 axisValue;
        };

        const KeyboardAxisButtonMapping kKeyboardAxisButtonMappings[] = {
            { m_joyTrigger_Right,  ZPad_Base::TRIGGER_RIGHT, KEY_LSHIFT, 1.0f },
            { m_joyTrigger_Left,   ZPad_Base::TRIGGER_LEFT,  KEY_LCTRL,  1.0f },
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

        ITF_INLINE i32 NormalizeKeyboardKeyCode(i32 keyCode)
        {
            if (keyCode >= 'a' && keyCode <= 'z')
            {
                return keyCode - ('a' - 'A');
            }
            return keyCode;
        }
    }

    ZPad_PCKeyboard::ZPad_PCKeyboard(u32 id) : ZPad_Base(id)
    {
        ResetKeyMappings();
    }

    void ZPad_PCKeyboard::ResetKeyMappings()
    {
        for (u32 i = 0; i < MAX_KEY_MAPPINGS; ++i)
        {
            m_keyMappings[i] = -1;
        }
    }

    void ZPad_PCKeyboard::SetKeyMapping(u32 logicalControl, i32 keyCode)
    {
        if (logicalControl >= MAX_KEY_MAPPINGS)
        {
            return;
        }

        keyCode = NormalizeKeyboardKeyCode(keyCode);

        const i32 previousKey = GetKeyMapping(logicalControl);
        m_keyMappings[logicalControl] = keyCode;
        if (keyCode < 0)
        {
            return;
        }
        for (u32 i = 0; i < MAX_KEY_MAPPINGS; ++i)
        {
            if (i == logicalControl)
                continue;

            if (GetKeyMapping(i) == keyCode)
            {
                m_keyMappings[i] = previousKey;
                break;
            }
        }
    }

    i32 ZPad_PCKeyboard::GetKeyMapping(u32 logicalControl) const
    {
        if (logicalControl < MAX_KEY_MAPPINGS)
        {
            if (m_keyMappings[logicalControl] >= 0)
            {
                return NormalizeKeyboardKeyCode(m_keyMappings[logicalControl]);
            }
            for (const auto& mapping : kKeyboardButtonMappings)
            {
                if (mapping.controlIndex == logicalControl)
                {
                    return NormalizeKeyboardKeyCode(mapping.defaultKey);
                }
            }
            for (const auto& mapping : kKeyboardAxisButtonMappings)
            {
                if (mapping.controlIndex == logicalControl)
                {
                    return NormalizeKeyboardKeyCode(mapping.defaultKey);
                }
            }
        }
        return -1;
    }

    void ZPad_PCKeyboard::SetRemap(u32 logicalControl, u32 physicalControl)
    {
        SetKeyMapping(logicalControl, static_cast<i32>(physicalControl));
    }

    void ZPad_PCKeyboard::ResetRemapping()
    {
        ResetKeyMappings();
        ZPad_Base::ResetRemapping();
    }

    u32 ZPad_PCKeyboard::GetRemap(u32 logicalControl) const
    {
        const i32 keyCode = GetKeyMapping(logicalControl);
        if (keyCode >= 0)
        {
            return static_cast<u32>(keyCode);
        }
        return ZPad_Base::GetRemap(logicalControl);
    }

    i32 ZPad_PCKeyboard::GetDefaultKeyForButton(u32 buttonIndex)
    {
        for (const auto& mapping : kKeyboardButtonMappings)
        {
            if (mapping.buttonIndex == buttonIndex)
            {
                return NormalizeKeyboardKeyCode(mapping.defaultKey);
            }
        }
        return -1;
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

        if (!INPUT_ADAPTER)
            return;

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        float axes[JOY_MAX_AXES];
        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            buttons[i] = InputAdapter::Released;
        }
        for (u32 i = 0; i < JOY_MAX_AXES; ++i)
        {
            axes[i] = 0.0f;
        }

        bbool buttonSetByCustomMapping[JOY_MAX_BUT] = { bfalse };
        for (const auto& mapping : kKeyboardButtonMappings)
        {
            if (mapping.controlIndex < MAX_KEY_MAPPINGS && m_keyMappings[mapping.controlIndex] >= 0)
            {
                i32 keyToCheck = m_keyMappings[mapping.controlIndex];
                InputAdapter::PressStatus status = INPUT_ADAPTER->getKeyboardStatus(keyToCheck);
                if (IsKeyboardButtonActive(status))
                {
                    buttons[mapping.buttonIndex] = status;
                }
                buttonSetByCustomMapping[mapping.buttonIndex] = btrue;
            }
        }
        for (const auto& mapping : kKeyboardButtonMappings)
        {
            if (buttonSetByCustomMapping[mapping.buttonIndex])
                continue;
            if (mapping.controlIndex < MAX_KEY_MAPPINGS && m_keyMappings[mapping.controlIndex] >= 0)
                continue;

            if (mapping.defaultKey >= 0)
            {
                InputAdapter::PressStatus status = INPUT_ADAPTER->getKeyboardStatus(mapping.defaultKey);
                if (IsKeyboardButtonActive(status))
                {
                    buttons[mapping.buttonIndex] = status;
                }
            }
        }
        bbool axisSetByCustomMapping[JOY_MAX_AXES] = { bfalse };
        for (const auto& mapping : kKeyboardAxisButtonMappings)
        {
            if (mapping.controlIndex < MAX_KEY_MAPPINGS && m_keyMappings[mapping.controlIndex] >= 0)
            {
                i32 keyToCheck = m_keyMappings[mapping.controlIndex];
                InputAdapter::PressStatus status = INPUT_ADAPTER->getKeyboardStatus(keyToCheck);
                if (IsKeyboardButtonActive(status))
                {
                    axes[mapping.axisIndex] = mapping.axisValue;
                }
                axisSetByCustomMapping[mapping.axisIndex] = btrue;
            }
        }
        for (const auto& mapping : kKeyboardAxisButtonMappings)
        {
            if (axisSetByCustomMapping[mapping.axisIndex])
                continue;

            if (mapping.controlIndex < MAX_KEY_MAPPINGS && m_keyMappings[mapping.controlIndex] >= 0)
                continue;

            if (mapping.defaultKey >= 0)
            {
                InputAdapter::PressStatus status = INPUT_ADAPTER->getKeyboardStatus(mapping.defaultKey);
                if (IsKeyboardButtonActive(status))
                {
                    axes[mapping.axisIndex] = mapping.axisValue;
                }
            }
        }

        i32 upKey = GetKeyMapping(STICK_L_UP);
        i32 downKey = GetKeyMapping(STICK_L_DOWN);
        i32 leftKey = GetKeyMapping(STICK_L_LEFT);
        i32 rightKey = GetKeyMapping(STICK_L_RIGHT);

        if (upKey < 0) upKey = KEY_UP;
        if (downKey < 0) downKey = KEY_DOWN;
        if (leftKey < 0) leftKey = KEY_LEFT;
        if (rightKey < 0) rightKey = KEY_RIGHT;

        InputAdapter::PressStatus upStatus = INPUT_ADAPTER->getKeyboardStatus(upKey);
        InputAdapter::PressStatus downStatus = INPUT_ADAPTER->getKeyboardStatus(downKey);
        InputAdapter::PressStatus leftStatus = INPUT_ADAPTER->getKeyboardStatus(leftKey);
        InputAdapter::PressStatus rightStatus = INPUT_ADAPTER->getKeyboardStatus(rightKey);

        f32 stickX = 0.0f;
        f32 stickY = 0.0f;

        if (IsKeyboardButtonActive(leftStatus))
            stickX = -1.0f;
        else if (IsKeyboardButtonActive(rightStatus))
            stickX = 1.0f;

        if (IsKeyboardButtonActive(upStatus))
            stickY = 1.0f;
        else if (IsKeyboardButtonActive(downStatus))
            stickY = -1.0f;

        if (fabs(axes[m_joyStickLeft_X]) < 0.001f)
            axes[m_joyStickLeft_X] = stickX;
        if (fabs(axes[m_joyStickLeft_Y]) < 0.001f)
            axes[m_joyStickLeft_Y] = stickY;

        if (stickX != 0.0f || stickY != 0.0f)
        {
            buttons[m_joyButton_DPadU] = InputAdapter::Released;
            buttons[m_joyButton_DPadD] = InputAdapter::Released;
            buttons[m_joyButton_DPadL] = InputAdapter::Released;
            buttons[m_joyButton_DPadR] = InputAdapter::Released;
        }

        UpdateAxisValues(deviceInfo, axes);
        UpdateButtonStates(deviceInfo, buttons);
        UpdatePlatformSpecificControls(deviceInfo, buttons, axes);
        ApplyRemapping(deviceInfo);
        ApplyDirectionRemapping(deviceInfo, axes, buttons);
    }

    void ZPad_PCKeyboard::UpdateDeviceInfoRaw(SDeviceInfo& deviceInfo)
    {
        u32 count = Min((u32)m_deviceInfo.m_inputInfo.size(), (u32)deviceInfo.m_inputInfo.size());
        for (u32 i = 0; i < count; ++i)
        {
            deviceInfo.m_inputInfo[i].m_type = m_deviceInfo.m_inputInfo[i].m_type;
        }

        if (!INPUT_ADAPTER)
            return;

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        float axes[JOY_MAX_AXES];
        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            buttons[i] = InputAdapter::Released;
        }
        for (u32 i = 0; i < JOY_MAX_AXES; ++i)
        {
            axes[i] = 0.0f;
        }

        for (const auto& mapping : kKeyboardButtonMappings)
        {
            if (mapping.defaultKey >= 0)
            {
                InputAdapter::PressStatus status = INPUT_ADAPTER->getKeyboardStatus(mapping.defaultKey);
                if (IsKeyboardButtonActive(status))
                {
                    buttons[mapping.buttonIndex] = status;
                }
            }
        }

        for (const auto& mapping : kKeyboardAxisButtonMappings)
        {
            if (mapping.defaultKey >= 0)
            {
                InputAdapter::PressStatus status = INPUT_ADAPTER->getKeyboardStatus(mapping.defaultKey);
                if (IsKeyboardButtonActive(status))
                {
                    axes[mapping.axisIndex] = mapping.axisValue;
                }
            }
        }

        InputAdapter::PressStatus upStatus = INPUT_ADAPTER->getKeyboardStatus(KEY_UP);
        InputAdapter::PressStatus downStatus = INPUT_ADAPTER->getKeyboardStatus(KEY_DOWN);
        InputAdapter::PressStatus leftStatus = INPUT_ADAPTER->getKeyboardStatus(KEY_LEFT);
        InputAdapter::PressStatus rightStatus = INPUT_ADAPTER->getKeyboardStatus(KEY_RIGHT);

        f32 stickX = 0.0f;
        f32 stickY = 0.0f;

        if (IsKeyboardButtonActive(leftStatus))
            stickX = -1.0f;
        else if (IsKeyboardButtonActive(rightStatus))
            stickX = 1.0f;

        if (IsKeyboardButtonActive(upStatus))
            stickY = 1.0f;
        else if (IsKeyboardButtonActive(downStatus))
            stickY = -1.0f;

        if (fabs(axes[m_joyStickLeft_X]) < 0.001f)
            axes[m_joyStickLeft_X] = stickX;
        if (fabs(axes[m_joyStickLeft_Y]) < 0.001f)
            axes[m_joyStickLeft_Y] = stickY;

        if (stickX != 0.0f || stickY != 0.0f)
        {
            buttons[m_joyButton_DPadU] = InputAdapter::Released;
            buttons[m_joyButton_DPadD] = InputAdapter::Released;
            buttons[m_joyButton_DPadL] = InputAdapter::Released;
            buttons[m_joyButton_DPadR] = InputAdapter::Released;
        }

        UpdateAxisValues(deviceInfo, axes);
        UpdateButtonStates(deviceInfo, buttons);
        UpdatePlatformSpecificControls(deviceInfo, buttons, axes);
    }

    bbool ZPad_PCKeyboard::IsSourceAllowed() const
    {
        return INPUT_ADAPTER && INPUT_ADAPTER->IsKeyboardMouseEnabled();
    }

    i32 ZPad_PCKeyboard::GetFirstPressedRawKey() const
    {
        if (!INPUT_ADAPTER || !IsSourceAllowed())
            return -1;

        return INPUT_ADAPTER->GetFirstPressedRawKey();
    }

    u32 ZPad_PCKeyboard::GetFirstActiveControl() const
    {
        if (!INPUT_ADAPTER || !IsSourceAllowed())
        {
            return U32_INVALID;
        }
        for (const auto& mapping : kKeyboardButtonMappings)
        {
            i32 keyToCheck;
            if (mapping.controlIndex < MAX_KEY_MAPPINGS && m_keyMappings[mapping.controlIndex] >= 0)
            {
                keyToCheck = m_keyMappings[mapping.controlIndex];
            }
            else
            {
                keyToCheck = mapping.defaultKey;
            }

            if (keyToCheck >= 0)
            {
                InputAdapter::PressStatus status = INPUT_ADAPTER->getKeyboardStatus(keyToCheck);
                if (IsKeyboardButtonActive(status))
                {
                    return mapping.controlIndex;
                }
            }
        }
        InputAdapter::PressStatus shiftStatus = INPUT_ADAPTER->getKeyboardStatus(KEY_LSHIFT);
        if (IsKeyboardButtonActive(shiftStatus))
        {
            return ZPad_Base::TRIGGER_RIGHT;
        }

        return U32_INVALID;
    }

} // namespace ITF

#endif // defined(ITF_WINDOWS)
