#ifndef _ITF_INPUTDEVICE_NUNCHUCK_WII_H_
#define _ITF_INPUTDEVICE_NUNCHUCK_WII_H_

#ifdef ITF_WII

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_ACTIONMAP_H_
#include "engine/zinput/ZActionMap.h"
#endif //_ITF_ACTIONMAP_H_

namespace ITF
{
    class ZNunchuck_wii : public IInputDevice
    {
    public:
        ZNunchuck_wii( u32 id ) : IInputDevice(id, PADTYPE_TO_FLAG(InputAdapter::Pad_WiiNunchuk)) {} 
        virtual ~ZNunchuck_wii(){}

    protected:
        virtual void   InitDeviceInfo();
        virtual void   UpdateDeviceInfo( SDeviceInfo& deviceInfo );

        enum EControl
        {
            NC_STICK_X = 0,
            NC_STICK_Y,
            NC_BUTTON_C,
            NC_BUTTON_Z,
            CONTROL_MAX
        };
    };

}//namespace ITF

#endif // ITF_WII

#endif // _ITF_INPUTDEVICE_NUNCHUCK_WII_H_


