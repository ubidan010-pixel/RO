#ifndef _ITF_INPUTDEVICE_CLASSIC_WII_H_
#define _ITF_INPUTDEVICE_CLASSIC_WII_H_

#ifdef ITF_WII

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{
    class ZClassic_wii : public IInputDevice
    {
    public:
        ZClassic_wii( u32 id ) : IInputDevice(id, PADTYPE_TO_FLAG(InputAdapter::Pad_WiiClassic)) {} 
        virtual ~ZClassic_wii(){}

        enum EControl
        {
            CL_STICK_LX = 0,
            CL_STICK_LY,
            CL_STICK_RX,
            CL_STICK_RY,
            CL_DPAD_UP,
            CL_DPAD_DOWN,
            CL_DPAD_LEFT,
            CL_DPAD_RIGHT,
            CL_BUTTON_ZL,
            CL_BUTTON_ZR,
            CL_BUTTON_SELECT,
            CL_BUTTON_START,
            CL_BUTTON_L,
            CL_BUTTON_R,
            CL_BUTTON_A,
            CL_BUTTON_B,
            CL_BUTTON_X,
            CL_BUTTON_Y,
            CONTROL_MAX
        };

    protected:
        virtual void   InitDeviceInfo();
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo );

    };

}//namespace ITF

#endif // ITF_WII

#endif // _ITF_INPUTDEVICE_CLASSIC_WII_H_


