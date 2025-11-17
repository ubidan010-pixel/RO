#ifndef _ITF_ZPAD_KEYBOARD_H_
#define _ITF_ZPAD_KEYBOARD_H_

#ifdef ITF_WINDOWS

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

namespace ITF
{
    class ZPad_keyboard : public IInputDevice
    {
    public:
        explicit ZPad_keyboard(u32 id);
        ~ZPad_keyboard() override = default;

        void InitDeviceInfo() override;
        void UpdateDeviceInfo(SDeviceInfo& deviceInfo) override;
        bbool IsDeviceValid() const override;

    private:
        enum Control
        {
            PAD_STICK_LX = 0,
            PAD_STICK_LY,
            PAD_TRIGGER_RIGHT,
            PAD_DPAD_UP,
            PAD_DPAD_DOWN,
            PAD_DPAD_LEFT,
            PAD_DPAD_RIGHT,
            PAD_BUTTON_A,
            PAD_BUTTON_B,
            PAD_BUTTON_X,
            PAD_BUTTON_START,
            CONTROL_MAX
        };

        static SInputInfo::EButtonStatus ConvertKeyStatus(InputAdapter::PressStatus status);
        static float ComputeAxis(InputAdapter::PressStatus negative, InputAdapter::PressStatus positive);
    };

    IInputDevice* CreateKeyboardDevice(u32 padIndex);
} // namespace ITF

#endif // ITF_WINDOWS

#endif //_ITF_ZPAD_KEYBOARD_H_
