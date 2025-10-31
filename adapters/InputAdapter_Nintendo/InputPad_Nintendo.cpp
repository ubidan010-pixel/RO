#include "precompiled_InputAdapter_Nintendo.h"

#include "core/error/ErrorHandler.h"

#include "adapters/InputAdapter_Nintendo/InputPad_Nintendo.h"

#include <nn/hid.h>

//#define ITF_LOG_INPUTS

namespace ITF
{

    InputPad_Nintendo::InputPad_Nintendo(u32 _id)
        : m_id(_id)
        , m_connected(false)
    {
        for (size_t buttonInd = 0; buttonInd < ITF_ARRAY_SIZE(m_buttonStatus); buttonInd++)
        {
            m_buttonStatus[buttonInd] = InputAdapter::Released;
        }

        ITF_MemSet(m_axesPos, 0, sizeof(m_axesPos));
    }

    InputPad_Nintendo::~InputPad_Nintendo() = default;

    void InputPad_Nintendo::getGamePadPos(u32 _environment, float* _pos, u32 _numAxes) const
    {
        if (_environment != 0)
        {
            ITF_Memcpy(_pos, m_axesPos, _numAxes * sizeof(*_pos));
        }
    }

    InputAdapter::PressStatus InputPad_Nintendo::getGamePadButton(u32 /*_environment*/, InputAdapter::ButtonMode /*_buttonMode*/, u32 _buttonID) const
    {
        if (_buttonID < ITF_ARRAY_SIZE(m_buttonStatus))
        {
            return m_buttonStatus[_buttonID];
        }

        return InputAdapter::Released;
    }

    void InputPad_Nintendo::setConnected(bool _connected)
    {
        if (m_connected != _connected)
        {
            m_connected = _connected;
            LOG("[InputPad_Nintendo] %s %u is now %s", GetTypeName(), m_id, m_connected ? "connected" : "disconnected");
        }
    }

    void InputPad_Nintendo::UpdateButton(u32 _buttonId, bool _isPressed)
    {
        InputAdapter::PressStatus& buttonStatus = m_buttonStatus[_buttonId];

        if (_isPressed)
        {
            if (buttonStatus == InputAdapter::JustPressed || buttonStatus == InputAdapter::Pressed)
            {
                buttonStatus = InputAdapter::Pressed;
            }
            else
            {
                buttonStatus = InputAdapter::JustPressed;
            #ifdef ITF_LOG_INPUTS
                LOG("[%s %u] Button %s has been pressed", GetTypeName(), m_id, InputAdapter::getJoyButtonNxName(_buttonId));
            #endif
            }
        }
        else
        {
            // Released
            if (buttonStatus == InputAdapter::JustReleased || buttonStatus == InputAdapter::Released)
            {
                buttonStatus = InputAdapter::Released;
            }
            else
            {
                buttonStatus = InputAdapter::JustReleased;
            #ifdef ITF_LOG_INPUTS
                LOG("[%s %u] Button %s has been released", GetTypeName(), m_id, InputAdapter::getJoyButtonNxName(_buttonId));
            #endif
            }
        }
    }

} // namespace ITF
