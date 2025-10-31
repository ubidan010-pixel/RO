#ifndef ITF_INPUTDEVICE_PS5_H_
#define ITF_INPUTDEVICE_PS5_H_

#pragma once

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{
#ifdef ITF_PS5
    class ZPad_ps5 : public IInputDevice
    {
    public:
        explicit ZPad_ps5( u32 id )
            : IInputDevice(id)
        {}
        virtual ~ZPad_ps5() = default;

    protected:
        virtual void   InitDeviceInfo() override;
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo ) override;

        enum EControl
        {
            PAD_STICK_LX = 0,
            PAD_STICK_LY,
            PAD_STICK_RX,
            PAD_STICK_RY,
            PAD_TRIGGER_LEFT,
            PAD_TRIGGER_RIGHT,
            PAD_DPAD_UP,
            PAD_DPAD_DOWN,
            PAD_DPAD_LEFT,
            PAD_DPAD_RIGHT,
            PAD_BUTTON_OPTIONS,
            PAD_BUTTON_L1,
            PAD_BUTTON_R1,
            PAD_BUTTON_LTHUMB,
            PAD_BUTTON_RTHUMB,
            PAD_BUTTON_CROSS,
            PAD_BUTTON_CIRCLE,
            PAD_BUTTON_SQUARE,
            PAD_BUTTON_TRIANGLE,
            PAD_BUTTON_TOUCHPAD,
            PAD_ACCELEROMETER,
            CONTROL_MAX
        };
    };
#endif // ITF_PS5
}//namespace ITF

#endif // ITF_INPUTDEVICE_PS5_H_

