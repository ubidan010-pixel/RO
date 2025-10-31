#ifndef _ITF_INPUTDEVICE_X360_H_
#define _ITF_INPUTDEVICE_X360_H_

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{
#if defined(ITF_WINDOWS) || defined(ITF_X360)
    class ZPad_x360 : public IInputDevice
    {
    public:

        ZPad_x360( u32 id ) : IInputDevice(id) {} 
        virtual ~ZPad_x360() {}

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
#endif // defined(ITF_WINDOWS) || defined(ITF_X360)
}//namespace ITF

#endif // _ITF_INPUTDEVICE_WIN32_H_


