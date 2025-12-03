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
            i32 defaultKey;  // Default keyboard key for this button
        };

        struct KeyboardAxisMapping
        {
            u32 axisIndex;
            f32 threshold;
            bbool positive;
            u32 controlIndex;
        };

        const KeyboardButtonMapping kKeyboardButtonMappings[] = {
            // DPad controls
            { m_joyButton_DPadU,      ZPad_Base::DPAD_UP,           KEY_UP },
            { m_joyButton_DPadD,      ZPad_Base::DPAD_DOWN,         KEY_DOWN },
            { m_joyButton_DPadL,      ZPad_Base::DPAD_LEFT,         KEY_LEFT },
            { m_joyButton_DPadR,      ZPad_Base::DPAD_RIGHT,        KEY_RIGHT },
            // Left stick controls (used by action remapping)
            { m_joyButton_DPadU,      ZPad_Base::STICK_L_UP,        KEY_UP },
            { m_joyButton_DPadD,      ZPad_Base::STICK_L_DOWN,      KEY_DOWN },
            { m_joyButton_DPadL,      ZPad_Base::STICK_L_LEFT,      KEY_LEFT },
            { m_joyButton_DPadR,      ZPad_Base::STICK_L_RIGHT,     KEY_RIGHT },
            // Face buttons
            { m_joyButton_A,          ZPad_Base::BUTTON_FACE_SOUTH, KEY_SPACE },
            { m_joyButton_B,          ZPad_Base::BUTTON_FACE_EAST,  KEY_BACKSPACE },
            { m_joyButton_X,          ZPad_Base::BUTTON_FACE_WEST,  's' },
            { m_joyButton_Y,          ZPad_Base::BUTTON_FACE_NORTH, 'w' },
            // Shoulder buttons
            { m_joyButton_LB,         ZPad_Base::BUTTON_L_SHOULDER, 'q' },
            { m_joyButton_RB,         ZPad_Base::BUTTON_R_SHOULDER, 'e' },
            // Thumbstick buttons
            { m_joyButton_ThumbLeft,  ZPad_Base::BUTTON_L_THUMB,    -1 },
            { m_joyButton_ThumbRight, ZPad_Base::BUTTON_R_THUMB,    -1 },
            // Menu buttons
            { m_joyButton_Start,      ZPad_Base::BUTTON_START,      KEY_ESC },
            { m_joyButton_Back,       ZPad_Base::BUTTON_SELECT,     KEY_TAB },
        };

        // Axis controls that can be triggered by keyboard keys
        struct KeyboardAxisButtonMapping
        {
            u32 axisIndex;
            u32 controlIndex;
            i32 defaultKey;
            f32 axisValue;  // Value to set when key is pressed
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
    }

    ZPad_PCKeyboard::ZPad_PCKeyboard(u32 id) : ZPad_Base(id)
    {
        ResetKeyMappings();
    }

    void ZPad_PCKeyboard::ResetKeyMappings()
    {
        for (u32 i = 0; i < MAX_KEY_MAPPINGS; ++i)
        {
            m_keyMappings[i] = -1;  // -1 means use default
        }
    }

    void ZPad_PCKeyboard::SetKeyMapping(u32 logicalControl, i32 keyCode)
    {
        if (logicalControl < MAX_KEY_MAPPINGS)
        {
            m_keyMappings[logicalControl] = keyCode;
        }
    }

    i32 ZPad_PCKeyboard::GetKeyMapping(u32 logicalControl) const
    {
        if (logicalControl < MAX_KEY_MAPPINGS)
        {
            // If custom mapping exists, return it
            if (m_keyMappings[logicalControl] >= 0)
            {
                return m_keyMappings[logicalControl];
            }
            // Otherwise find default key for this control in button mappings
            for (const auto& mapping : kKeyboardButtonMappings)
            {
                if (mapping.controlIndex == logicalControl)
                {
                    return mapping.defaultKey;
                }
            }
            // Also check axis button mappings (triggers)
            for (const auto& mapping : kKeyboardAxisButtonMappings)
            {
                if (mapping.controlIndex == logicalControl)
                {
                    return mapping.defaultKey;
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
                return mapping.defaultKey;
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

        // Read raw keyboard state for remapped keys
        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        float axes[JOY_MAX_AXES];
        
        // Initialize to released
        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            buttons[i] = InputAdapter::Released;
        }
        for (u32 i = 0; i < JOY_MAX_AXES; ++i)
        {
            axes[i] = 0.0f;
        }

        // Apply keyboard mappings - check both custom remaps and defaults
        // Track which buttons have been set by custom mappings to avoid conflicts
        bbool buttonSetByCustomMapping[JOY_MAX_BUT] = { bfalse };
        
        // First pass: apply custom remapped keys only
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
                // Mark this button as handled by custom mapping (even if not pressed)
                buttonSetByCustomMapping[mapping.buttonIndex] = btrue;
            }
        }
        
        // Second pass: apply default keys only for buttons not handled by custom mappings
        for (const auto& mapping : kKeyboardButtonMappings)
        {
            // Skip if this button was already handled by a custom mapping
            if (buttonSetByCustomMapping[mapping.buttonIndex])
                continue;
                
            // Skip if this control has a custom mapping (handled in first pass)
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

        // Apply axis button mappings (triggers mapped to keyboard keys)
        bbool axisSetByCustomMapping[JOY_MAX_AXES] = { bfalse };
        
        // First pass: custom mappings
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
        
        // Second pass: default keys
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

        // Check mapped buttons using current key mappings
        for (const auto& mapping : kKeyboardButtonMappings)
        {
            i32 keyToCheck = -1;
            
            // Check if there's a custom remap for this control
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

        // Check axis mappings
        InputAdapter::PressStatus shiftStatus = INPUT_ADAPTER->getKeyboardStatus(KEY_LSHIFT);
        if (IsKeyboardButtonActive(shiftStatus))
        {
            return ZPad_Base::TRIGGER_RIGHT;
        }

        return U32_INVALID;
    }

} // namespace ITF

#endif // defined(ITF_WINDOWS)
