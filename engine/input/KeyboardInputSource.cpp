#include "precompiled_engine.h"

#ifndef _ITF_KEYBOARD_INPUT_SOURCE_H_
#include "engine/input/KeyboardInputSource.h"
#endif //_ITF_KEYBOARD_INPUT_SOURCE_H_

#ifdef ITF_WINDOWS
#include <windows.h>
#endif

namespace ITF
{
    KeyboardInputSource::KeyboardInputSource()
        : m_initialized(false)
    {
        for (u32 i = 0; i < KEY_COUNT; ++i)
        {
            m_keyStates[i] = InputAdapter::Released;
            m_keyPressTime[i] = 0;
        }
        m_initialized = true;
    }

    void KeyboardInputSource::Update()
    {
        if (!m_initialized)
            return;

        for (u32 keyIndex = 0; keyIndex < KEY_COUNT; ++keyIndex)
        {
            if (m_keyStates[keyIndex] == InputAdapter::Pressed || 
                m_keyStates[keyIndex] == InputAdapter::JustPressed)
            {
                m_keyPressTime[keyIndex] += 1;
            }

            switch (m_keyStates[keyIndex])
            {
            case InputAdapter::JustPressed:
                m_keyStates[keyIndex] = InputAdapter::Pressed;
                break;
            case InputAdapter::JustReleased:
                m_keyStates[keyIndex] = InputAdapter::Released;
                break;
            case InputAdapter::Pressed:
            case InputAdapter::Released:
                break;
            default:
                break;
            }
        }
    }

    InputAdapter::PressStatus KeyboardInputSource::GetKeyState(u32 keyCode) const
    {
        if (keyCode >= KEY_COUNT)
            return InputAdapter::Released;
        return m_keyStates[keyCode];
    }

    void KeyboardInputSource::OnKeyEvent(i32 keyCode, InputAdapter::PressStatus status)
    {
        if (keyCode < 0 || keyCode >= static_cast<i32>(KEY_COUNT))
            return;

        if (status == InputAdapter::Pressed)
        {
            if (m_keyStates[keyCode] == InputAdapter::Released ||
                m_keyStates[keyCode] == InputAdapter::JustReleased)
            {
                m_keyStates[keyCode] = InputAdapter::JustPressed;
                m_keyPressTime[keyCode] = 0;
            }
            else
            {
                m_keyStates[keyCode] = InputAdapter::Pressed;
            }
        }
        else if (status == InputAdapter::Released)
        {
            if (m_keyStates[keyCode] == InputAdapter::Pressed ||
                m_keyStates[keyCode] == InputAdapter::JustPressed)
            {
                m_keyStates[keyCode] = InputAdapter::JustReleased;
            }
            else
            {
                m_keyStates[keyCode] = InputAdapter::Released;
            }
        }
    }

    void KeyboardInputSource::GetAllKeyStates(InputAdapter::PressStatus* keyStates, u32 numKeys) const
    {
        if (!keyStates)
            return;

        const u32 count = (numKeys < KEY_COUNT) ? numKeys : KEY_COUNT;
        for (u32 i = 0; i < count; ++i)
        {
            keyStates[i] = m_keyStates[i];
        }
    }

} // namespace ITF

