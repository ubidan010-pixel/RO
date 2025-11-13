#include "precompiled_engine.h"

#ifndef _ITF_INPUT_MAPPING_DEFAULTS_H_
#include "engine/input/InputMappingDefaults.h"
#endif //_ITF_INPUT_MAPPING_DEFAULTS_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifdef ITF_WINDOWS
#include <windows.h>
#endif

namespace ITF
{
#ifdef ITF_WINDOWS
    void InitializeDefaultKeyboardMappings(InputMapping& mapping)
    {
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, VK_UP, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, KEY_UP, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, VK_DOWN, -1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, KEY_DOWN, -1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, VK_LEFT, -1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, KEY_LEFT, -1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, VK_RIGHT, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, KEY_RIGHT, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X));

        mapping.AddMapping(MakeKeyboardInput(VK_SPACE), MakeVirtualButton(VIRTUAL_BUTTON_A));
        mapping.AddMapping(MakeKeyboardInput(KEY_SPACE), MakeVirtualButton(VIRTUAL_BUTTON_A));
        mapping.AddMapping(MakeKeyboardInput('S'), MakeVirtualButton(VIRTUAL_BUTTON_X));
        mapping.AddMapping(MakeKeyboardInput('s'), MakeVirtualButton(VIRTUAL_BUTTON_X));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, VK_LSHIFT, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_TRIGGER));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, KEY_LSHIFT, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_TRIGGER));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, VK_RSHIFT, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_TRIGGER));
        mapping.AddMapping(PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, KEY_RSHIFT, 1.0f), MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_TRIGGER));
        mapping.AddMapping(MakeKeyboardInput(VK_BACK), MakeVirtualButton(VIRTUAL_BUTTON_BACK));
        mapping.AddMapping(MakeKeyboardInput(KEY_BACKSPACE), MakeVirtualButton(VIRTUAL_BUTTON_BACK));
        mapping.AddMapping(MakeKeyboardInput(VK_ESCAPE), MakeVirtualButton(VIRTUAL_BUTTON_START));
        mapping.AddMapping(MakeKeyboardInput(KEY_ESC), MakeVirtualButton(VIRTUAL_BUTTON_START));
    }
#endif

    void InitializeDefaultControllerMappings(InputMapping& mapping, u32 deviceId)
    {
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_A), MakeVirtualButton(VIRTUAL_BUTTON_A));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_B), MakeVirtualButton(VIRTUAL_BUTTON_B));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_X), MakeVirtualButton(VIRTUAL_BUTTON_X));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_Y), MakeVirtualButton(VIRTUAL_BUTTON_Y));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_Start), MakeVirtualButton(VIRTUAL_BUTTON_START));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_Back), MakeVirtualButton(VIRTUAL_BUTTON_BACK));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_LB), MakeVirtualButton(VIRTUAL_BUTTON_LB));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_RB), MakeVirtualButton(VIRTUAL_BUTTON_RB));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_DPadU), MakeVirtualButton(VIRTUAL_BUTTON_DPAD_UP));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_DPadD), MakeVirtualButton(VIRTUAL_BUTTON_DPAD_DOWN));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_DPadL), MakeVirtualButton(VIRTUAL_BUTTON_DPAD_LEFT));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_DPadR), MakeVirtualButton(VIRTUAL_BUTTON_DPAD_RIGHT));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_ThumbLeft), MakeVirtualButton(VIRTUAL_BUTTON_LTHUMB));
        mapping.AddMapping(MakeControllerButtonInput(deviceId, m_joyButton_ThumbRight), MakeVirtualButton(VIRTUAL_BUTTON_RTHUMB));
        mapping.AddMapping(MakeControllerAxisInput(deviceId, m_joyStickLeft_X, 0.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X));
        mapping.AddMapping(MakeControllerAxisInput(deviceId, m_joyStickLeft_Y, 0.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y));
        mapping.AddMapping(MakeControllerAxisInput(deviceId, m_joyStickRight_X, 0.0f), MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_STICK_X));
        mapping.AddMapping(MakeControllerAxisInput(deviceId, m_joyStickRight_Y, 0.0f), MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_STICK_Y));
        mapping.AddMapping(MakeControllerAxisInput(deviceId, m_joyTrigger_Left, 0.0f), MakeVirtualAxis(VIRTUAL_AXIS_LEFT_TRIGGER));
        mapping.AddMapping(MakeControllerAxisInput(deviceId, m_joyTrigger_Right, 0.0f), MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_TRIGGER));
    }

    void InitializeDefaultActionMappings(ActionMapping& actionMapping)
    {
        for (u32 player = 0; player < JOY_MAX_COUNT; ++player)
        {
            actionMapping.SetActionMapping(player, MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X), InputAdapter::ActionLeft);
            actionMapping.SetActionMapping(player, MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_X), InputAdapter::ActionRight);
            actionMapping.SetActionMapping(player, MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y), InputAdapter::ActionUp);
            actionMapping.SetActionMapping(player, MakeVirtualAxis(VIRTUAL_AXIS_LEFT_STICK_Y), InputAdapter::ActionDown);
            actionMapping.SetActionMapping(player, MakeVirtualButton(VIRTUAL_BUTTON_A), InputAdapter::ActionJump);
            actionMapping.SetActionMapping(player, MakeVirtualButton(VIRTUAL_BUTTON_X), InputAdapter::ActionHit);
            actionMapping.SetActionMapping(player, MakeVirtualButton(VIRTUAL_BUTTON_B), InputAdapter::ActionBack);
            actionMapping.SetActionMapping(player, MakeVirtualButton(VIRTUAL_BUTTON_START), InputAdapter::ActionShowMenu);
            actionMapping.SetActionMapping(player, MakeVirtualAxis(VIRTUAL_AXIS_RIGHT_TRIGGER), InputAdapter::ActionSprint);
        }
    }

} // namespace ITF

