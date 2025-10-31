#pragma once

#include "engine/zinput/ZInputDevice.h"

#ifdef ITF_NINTENDO

namespace ITF
{
    class ZPad_Nintendo : public IInputDevice
    {
    public:
        ZPad_Nintendo(u32 id)
            : IInputDevice(id)
        {}

        virtual ~ZPad_Nintendo() override = default;

    protected:
        virtual void   InitDeviceInfo() override;
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo ) override;

        enum EControl
        {
            PAD_STICK_LX = 0,
            PAD_STICK_LY,
            PAD_STICK_RX,
            PAD_STICK_RY,
            PAD_DPAD_UP,
            PAD_DPAD_DOWN,
            PAD_DPAD_LEFT,
            PAD_DPAD_RIGHT,
            PAD_BUTTON_MINUS,
            PAD_BUTTON_PLUS,
            PAD_BUTTON_L,
            PAD_BUTTON_R,
            PAD_BUTTON_ZL,
            PAD_BUTTON_ZR,
            PAD_BUTTON_LTHUMB,
            PAD_BUTTON_RTHUMB,
            PAD_BUTTON_A,
            PAD_BUTTON_B,
            PAD_BUTTON_X,
            PAD_BUTTON_Y,
            CONTROL_MAX
        };
    };
}//namespace ITF

#endif // ITF_NINTENDO
