#ifndef _ITF_INPUTDEVICE_PS3_H_
#define _ITF_INPUTDEVICE_PS3_H_

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{
#ifdef ITF_PS3
    class ZPad_ps3 : public IInputDevice
    {
    public:
        ZPad_ps3( u32 id ) : IInputDevice(id) {} 
        virtual ~ZPad_ps3(){}

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
            PAD_BUTTON_SELECT,
            PAD_BUTTON_START,
            PAD_BUTTON_L1,
            PAD_BUTTON_R1,
            PAD_BUTTON_LTHUMB,
            PAD_BUTTON_RTHUMB,
            PAD_BUTTON_CROSS,
            PAD_BUTTON_CIRCLE,
            PAD_BUTTON_SQUARE,
            PAD_BUTTON_TRIANGLE,
            CONTROL_MAX
        };
    };
#endif // ITF_PS3
}//namespace ITF

#endif // _ITF_INPUTDEVICE_PS3_H_


