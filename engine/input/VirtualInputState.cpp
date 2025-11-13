#include "precompiled_engine.h"

#ifndef _ITF_VIRTUAL_INPUT_STATE_H_
#include "engine/input/VirtualInputState.h"
#endif //_ITF_VIRTUAL_INPUT_STATE_H_

#include <cmath>

namespace ITF
{
    VirtualInputState::VirtualInputState()
    {
        Reset();
    }

    void VirtualInputState::Reset()
    {
        for (u32 player = 0; player < JOY_MAX_COUNT; ++player)
        {
            for (u32 button = 0; button < VIRTUAL_BUTTON_COUNT; ++button)
            {
                m_virtualButtons[player][button] = InputAdapter::Released;
            }
        }

        for (u32 player = 0; player < JOY_MAX_COUNT; ++player)
        {
            for (u32 axis = 0; axis < VIRTUAL_AXIS_COUNT; ++axis)
            {
                m_virtualAxes[player][axis] = 0.0f;
            }
        }
    }

    void VirtualInputState::UpdateFromPhysicalInputs(const std::vector<IInputSource*>& sources, const InputMapping& mapping)
    {
        Reset();

        for (IInputSource* source : sources)
        {
            if (!source || !source->IsConnected())
                continue;

            u32 deviceId = source->GetDeviceId();

            if (source->GetInputType() == PhysicalInput::Keyboard)
            {
                for (u32 keyCode = 0; keyCode < KEY_COUNT; ++keyCode)
                {
                    InputAdapter::PressStatus keyStatus = source->GetKeyState(keyCode);
                    if (keyStatus == InputAdapter::Released)
                        continue;

                    bool foundAnyMapping = false;
                    
                    PhysicalInput physical = MakeKeyboardInput(keyCode);
                    VirtualInput virtualInput;
                    if (mapping.GetVirtualInput(physical, virtualInput))
                    {
                        foundAnyMapping = true;
                        if (virtualInput.type == VirtualInput::Button)
                        {
                            UpdateButtonFromPhysicalInput(physical, virtualInput, source);
                        }
                        else if (virtualInput.type == VirtualInput::Axis)
                        {
                            UpdateAxisFromPhysicalInput(physical, virtualInput, source);
                        }
                    }
                    
                    physical.axisValue = -1.0f;
                    if (mapping.GetVirtualInput(physical, virtualInput))
                    {
                        foundAnyMapping = true;
                        if (virtualInput.type == VirtualInput::Button)
                        {
                            UpdateButtonFromPhysicalInput(physical, virtualInput, source);
                        }
                        else if (virtualInput.type == VirtualInput::Axis)
                        {
                            UpdateAxisFromPhysicalInput(physical, virtualInput, source);
                        }
                    }
                    
                    physical.axisValue = 1.0f;
                    if (mapping.GetVirtualInput(physical, virtualInput))
                    {
                        foundAnyMapping = true;
                        if (virtualInput.type == VirtualInput::Button)
                        {
                            UpdateButtonFromPhysicalInput(physical, virtualInput, source);
                        }
                        else if (virtualInput.type == VirtualInput::Axis)
                        {
                            UpdateAxisFromPhysicalInput(physical, virtualInput, source);
                        }
                    }
                }
            }
            else if (source->GetInputType() == PhysicalInput::ControllerButton)
            {
                for (u32 buttonId = 0; buttonId < JOY_MAX_BUT; ++buttonId)
                {
                    InputAdapter::PressStatus buttonStatus = source->GetButtonState(buttonId);
                    if (buttonStatus == InputAdapter::Released || buttonStatus == InputAdapter::JustReleased)
                        continue;

                    PhysicalInput physical = MakeControllerButtonInput(deviceId, buttonId);
                    VirtualInput virtualInput;
                    if (mapping.GetVirtualInput(physical, virtualInput))
                    {
                        if (virtualInput.type == VirtualInput::Button)
                        {
                            UpdateButtonFromPhysicalInput(physical, virtualInput, source);
                        }
                    }
                }

                for (u32 axisId = 0; axisId < JOY_MAX_AXES; ++axisId)
                {
                    f32 axisValue = source->GetAxisState(axisId);
                    if (fabsf(axisValue) < 0.01f)
                        continue;

                    PhysicalInput physicalForLookup = MakeControllerAxisInput(deviceId, axisId, 0.0f);
                    VirtualInput virtualInput;
                    if (mapping.GetVirtualInput(physicalForLookup, virtualInput))
                    {
                        if (virtualInput.type == VirtualInput::Axis)
                        {
                            PhysicalInput physical = MakeControllerAxisInput(deviceId, axisId, axisValue);
                            UpdateAxisFromPhysicalInput(physical, virtualInput, source);
                        }
                    }
                }
            }
        }
    }

    InputAdapter::PressStatus VirtualInputState::GetButton(u32 player, VirtualButton button) const
    {
        if (player >= JOY_MAX_COUNT || button >= VIRTUAL_BUTTON_COUNT)
            return InputAdapter::Released;
        return m_virtualButtons[player][button];
    }

    f32 VirtualInputState::GetAxis(u32 player, VirtualAxis axis) const
    {
        if (player >= JOY_MAX_COUNT || axis >= VIRTUAL_AXIS_COUNT)
            return 0.0f;
        return m_virtualAxes[player][axis];
    }

    void VirtualInputState::GetButtons(u32 player, InputAdapter::PressStatus* buttons, u32 numButtons) const
    {
        if (!buttons || player >= JOY_MAX_COUNT)
            return;

        const u32 count = (numButtons < VIRTUAL_BUTTON_COUNT) ? numButtons : VIRTUAL_BUTTON_COUNT;
        for (u32 i = 0; i < count; ++i)
        {
            buttons[i] = m_virtualButtons[player][i];
        }
    }

    void VirtualInputState::GetAxes(u32 player, f32* axes, u32 numAxes) const
    {
        if (!axes || player >= JOY_MAX_COUNT)
            return;

        const u32 count = (numAxes < VIRTUAL_AXIS_COUNT) ? numAxes : VIRTUAL_AXIS_COUNT;
        for (u32 i = 0; i < count; ++i)
        {
            axes[i] = m_virtualAxes[player][i];
        }
    }

    void VirtualInputState::UpdateButtonFromPhysicalInput(const PhysicalInput& physical, const VirtualInput& virtualInput, IInputSource* source)
    {
        if (virtualInput.type != VirtualInput::Button || virtualInput.virtualId >= VIRTUAL_BUTTON_COUNT)
            return;

        u32 player = 0;
        if (physical.type == PhysicalInput::ControllerButton || physical.type == PhysicalInput::ControllerAxis)
        {
            player = physical.deviceId;
            if (player >= JOY_MAX_COUNT)
                return;
        }

        InputAdapter::PressStatus newStatus = InputAdapter::Released;

        if (physical.type == PhysicalInput::Keyboard)
        {
            newStatus = source->GetKeyState(physical.inputId);
            LOG("[VirtualInputState] UpdateButtonFromPhysicalInput: Keyboard key=%d -> VirtualButton=%d, player=%d, status=%d", 
                physical.inputId, virtualInput.virtualId, player, newStatus);
        }
        else if (physical.type == PhysicalInput::ControllerButton)
        {
            newStatus = source->GetButtonState(physical.inputId);
        }

        VirtualButton button = static_cast<VirtualButton>(virtualInput.virtualId);
        InputAdapter::PressStatus currentStatus = m_virtualButtons[player][button];
        m_virtualButtons[player][button] = CombinePressStatus(currentStatus, newStatus);
    }

    void VirtualInputState::UpdateAxisFromPhysicalInput(const PhysicalInput& physical, const VirtualInput& virtualInput, IInputSource* source)
    {
        if (virtualInput.type != VirtualInput::Axis || virtualInput.virtualId >= VIRTUAL_AXIS_COUNT)
            return;

        u32 player = 0;
        if (physical.type == PhysicalInput::ControllerButton || physical.type == PhysicalInput::ControllerAxis)
        {
            player = physical.deviceId;
            if (player >= JOY_MAX_COUNT)
                return;
        }

        f32 newValue = 0.0f;

        if (physical.type == PhysicalInput::Keyboard)
        {
            InputAdapter::PressStatus keyStatus = source->GetKeyState(physical.inputId);
            if (keyStatus == InputAdapter::Pressed || keyStatus == InputAdapter::JustPressed)
            {
                newValue = physical.axisValue;
            }
        }
        else if (physical.type == PhysicalInput::ControllerAxis)
        {
            newValue = source->GetAxisState(physical.inputId);
            if (physical.axisValue != 0.0f)
            {
                if (physical.axisValue < 0.0f && newValue > 0.0f)
                    newValue = 0.0f;
                else if (physical.axisValue > 0.0f && newValue < 0.0f)
                    newValue = 0.0f;
            }
        }

        VirtualAxis axis = static_cast<VirtualAxis>(virtualInput.virtualId);
        f32 currentValue = m_virtualAxes[player][axis];
        if (fabsf(newValue) > fabsf(currentValue))
        {
            m_virtualAxes[player][axis] = newValue;
        }
    }

    InputAdapter::PressStatus VirtualInputState::CombinePressStatus(InputAdapter::PressStatus current, InputAdapter::PressStatus newStatus) const
    {
        if (newStatus == InputAdapter::JustPressed || current == InputAdapter::JustPressed)
            return InputAdapter::JustPressed;

        if (newStatus == InputAdapter::Pressed || current == InputAdapter::Pressed)
            return InputAdapter::Pressed;

        if (newStatus == InputAdapter::JustReleased || current == InputAdapter::JustReleased)
            return InputAdapter::JustReleased;

        return InputAdapter::Released;
    }

} // namespace ITF

