#include "precompiled_engine.h"

#ifndef _ITF_CONTROLLER_INPUT_SOURCE_H_
#include "engine/input/ControllerInputSource.h"
#endif //_ITF_CONTROLLER_INPUT_SOURCE_H_

#include <cmath>

namespace ITF
{
    ControllerInputSource::ControllerInputSource(u32 deviceId)
        : m_deviceId(deviceId)
        , m_connected(false)
        , m_initialized(false)
    {
        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            m_buttonStates[i] = InputAdapter::Released;
        }

        for (u32 i = 0; i < JOY_MAX_AXES; ++i)
        {
            m_axisStates[i] = 0.0f;
        }

        m_initialized = true;
    }

    void ControllerInputSource::Update()
    {
        if (!m_initialized || !m_connected)
            return;
    }

    bool ControllerInputSource::IsConnected() const
    {
        return m_connected && m_initialized;
    }

    InputAdapter::PressStatus ControllerInputSource::GetButtonState(u32 buttonId) const
    {
        if (buttonId >= JOY_MAX_BUT)
            return InputAdapter::Released;
        return m_buttonStates[buttonId];
    }

    f32 ControllerInputSource::GetAxisState(u32 axisId) const
    {
        if (axisId >= JOY_MAX_AXES)
            return 0.0f;
        return m_axisStates[axisId];
    }

    void ControllerInputSource::SetButtonState(u32 buttonId, bool pressed)
    {
        if (buttonId >= JOY_MAX_BUT)
            return;

        UpdateButtonPressStatus(buttonId, pressed);
    }

    void ControllerInputSource::SetAxisState(u32 axisId, f32 value)
    {
        if (axisId >= JOY_MAX_AXES)
            return;

        const f32 deadzone = GetDeadzone();
        if (fabsf(value) < deadzone)
        {
            m_axisStates[axisId] = 0.0f;
        }
        else
        {
            f32 sign = (value >= 0.0f) ? 1.0f : -1.0f;
            f32 normalized = (fabsf(value) - deadzone) / (1.0f - deadzone);
            m_axisStates[axisId] = sign * normalized;
        }
    }

    void ControllerInputSource::GetAllButtonStates(InputAdapter::PressStatus* buttons, u32 numButtons) const
    {
        if (!buttons)
            return;

        const u32 count = (numButtons < JOY_MAX_BUT) ? numButtons : JOY_MAX_BUT;
        for (u32 i = 0; i < count; ++i)
        {
            buttons[i] = m_buttonStates[i];
        }
    }

    void ControllerInputSource::GetAllAxisStates(f32* axes, u32 numAxes) const
    {
        if (!axes)
            return;

        const u32 count = (numAxes < JOY_MAX_AXES) ? numAxes : JOY_MAX_AXES;
        for (u32 i = 0; i < count; ++i)
        {
            axes[i] = m_axisStates[i];
        }
    }

    void ControllerInputSource::SetConnected(bool connected)
    {
        if (m_connected == connected)
            return;

        m_connected = connected;

        if (connected)
        {
            OnConnected();
        }
        else
        {
            OnDisconnected();
            for (u32 i = 0; i < JOY_MAX_BUT; ++i)
            {
                m_buttonStates[i] = InputAdapter::Released;
            }
            for (u32 i = 0; i < JOY_MAX_AXES; ++i)
            {
                m_axisStates[i] = 0.0f;
            }
        }
    }

    void ControllerInputSource::UpdateButtonPressStatus(u32 buttonId, bool pressed)
    {
        InputAdapter::PressStatus previousStatus = m_buttonStates[buttonId];

        if (pressed)
        {
            if (previousStatus == InputAdapter::Released ||
                previousStatus == InputAdapter::JustReleased)
            {
                m_buttonStates[buttonId] = InputAdapter::JustPressed;
            }
            else
            {
                m_buttonStates[buttonId] = InputAdapter::Pressed;
            }
        }
        else
        {
            if (previousStatus == InputAdapter::Pressed ||
                previousStatus == InputAdapter::JustPressed)
            {
                m_buttonStates[buttonId] = InputAdapter::JustReleased;
            }
            else
            {
                m_buttonStates[buttonId] = InputAdapter::Released;
            }
        }
    }

} // namespace ITF

