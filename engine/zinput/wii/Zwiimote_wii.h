#ifndef _ITF_INPUTDEVICE_WIIMOTE_WII_H_
#define _ITF_INPUTDEVICE_WIIMOTE_WII_H_

#ifdef ITF_WII

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{
    class ZWiimote_wii : public IInputDevice
    {
    public:

        ZWiimote_wii( u32 id ) : IInputDevice(id, PADTYPE_TO_FLAG(InputAdapter::Pad_WiiSideWay)|PADTYPE_TO_FLAG(InputAdapter::Pad_WiiNunchuk)) {} 
        virtual ~ZWiimote_wii(){}

        enum EControl
        {
            SW_DPAD_UP = 0,
            SW_DPAD_DOWN,
            SW_DPAD_LEFT,
            SW_DPAD_RIGHT,
            SW_BUTTON_MINUS,
            SW_BUTTON_PLUS,
            SW_BUTTON_1,
            SW_BUTTON_2,
            SW_BUTTON_A,
            SW_BUTTON_B,
            SW_ACCELEROMETER,
            CONTROL_MAX
        };

    protected:
        virtual void   InitDeviceInfo();
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo );

    };

}//namespace ITF

#endif // ITF_WII

#endif // _ITF_INPUTDEVICE_WIIMOTE_WII_H_


