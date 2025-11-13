#ifndef _ITF_KEYBOARD_INPUT_SOURCE_H_
#define _ITF_KEYBOARD_INPUT_SOURCE_H_

#ifndef _ITF_IINPUT_SOURCE_H_
#include "engine/input/IInputSource.h"
#endif //_ITF_IINPUT_SOURCE_H_

namespace ITF
{
    class KeyboardInputSource : public IInputSource
    {
    public:
        KeyboardInputSource();
        virtual ~KeyboardInputSource() override = default;
        void Update() override;
        bool IsConnected() const override { return true; }
        u32 GetDeviceId() const override { return KEYBOARD_DEVICE_ID; }
        InputAdapter::PressStatus GetKeyState(u32 keyCode) const override;
        PhysicalInput::Type GetInputType() const override { return PhysicalInput::Keyboard; }
        void OnKeyEvent(i32 keyCode, InputAdapter::PressStatus status);
        void GetAllKeyStates(InputAdapter::PressStatus* keyStates, u32 numKeys) const;
    private:
        InputAdapter::PressStatus m_keyStates[KEY_COUNT];
        u32 m_keyPressTime[KEY_COUNT];
        bool m_initialized;
    };

} // namespace ITF

#endif //_ITF_KEYBOARD_INPUT_SOURCE_H_

