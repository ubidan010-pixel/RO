#pragma once

#include "engine/zinput/ZInputDevice.h"
#include "engine/zinput/ZActionMap.h"

namespace ITF
{
#if defined(ITF_XBOX_SERIES)
    class ZPad_XBoxSeries : public IInputDevice
    {
    public:

        explicit ZPad_XBoxSeries( u32 id ) : IInputDevice(id) {}
        virtual ~ZPad_XBoxSeries() = default;

    protected:
        virtual void   InitDeviceInfo();
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo );

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
            PAD_BUTTON_BACK,
            PAD_BUTTON_START,
            PAD_BUTTON_LSHOULDER,
            PAD_BUTTON_RSHOULDER,
            PAD_BUTTON_LTHUMB,
            PAD_BUTTON_RTHUMB,
            PAD_BUTTON_A,
            PAD_BUTTON_B,
            PAD_BUTTON_X,
            PAD_BUTTON_Y,
            CONTROL_MAX
        };
    };
#endif // defined(ITF_XBOX_SERIES)
}//namespace ITF


