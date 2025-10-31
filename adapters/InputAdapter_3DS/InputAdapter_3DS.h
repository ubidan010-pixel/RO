#ifndef _ITF_INPUTADAPTER_3DS_H_
#define _ITF_INPUTADAPTER_3DS_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#include <nn/hid.h>
//#include "device.h"

namespace ITF
{
    class InputAdapter_3DS : public InputAdapter
    {
    private:
        static const u32        maxPad    =    4;
        u32                     getConnectedPadIndex (u32 _padIndex) const;
		nn::hid::TouchPanelReader m_TouchPanelReader;
		nn::hid::AccelerometerReader m_accReader;
		nn::hid::GyroscopeReader m_GyroReader;

		//nn::hid::GyroscopeReader gyroReader(&accReader);
    public:
                                InputAdapter_3DS (    );
                                ~InputAdapter_3DS(    );

        void                    getGamePadPos       (  u32 _environment, u32 _pad, float *_pos, u32 _numAxes ) const;
        void                    getGamePadButtons   (  u32 _environment , u32 _pad, PressStatus* _buttons, u32 _numButtons ) const;
        u32                     getGamePadCount     (    );

        void                    updateAllInputState();

        void                    updateMouse();

///////////////////////////TP//////////////////////////////////////////
		nn::hid::TouchPanelStatus GetLatestTouchPanelStatus(void);
        bbool					IsTPPressed() const {return m_TpStatus == InputAdapter::Pressed;}
        bbool					IsTPJustPressed() const {return m_TpStatus == InputAdapter::JustPressed;}
        bbool					IsTPReleased() const {return m_TpStatus == InputAdapter::Released;}
        bbool					IsTPJustReleased() const {return m_TpStatus == InputAdapter::JustReleased;}
		u16						GetTPPosX() {return  GetLatestTouchPanelStatus().x ;}
		u16						GetTPPosY() {return  GetLatestTouchPanelStatus().y ;}
//////////////////////////////////////////////////////////////////////////

///////////////////////////Accelerometer//////////////////////////////////////////
		nn::hid::AccelerationFloat GetLatestAccelerometerStatus(void);
		f32						GetAccelerometerPosX(){return  GetLatestAccelerometerStatus().x ;}
		f32						GetAccelerometerPosY(){return  GetLatestAccelerometerStatus().y ;}
		f32						GetAccelerometerPosZ(){return  GetLatestAccelerometerStatus().z ;}

//////////////////////////////////////////////////////////////////////////

///////////////////////////Gyroscope//////////////////////////////////////////
		nn::hid::GyroscopeStatus	GetLatestGyroscopeStatus(void);
		nn::math::VEC3				GetGyroscopespeed(void){return GetLatestGyroscopeStatus().speed;}
		nn::math::VEC3				GetGyroscopeAngle(void){return GetLatestGyroscopeStatus().angle;}
		nn::hid::Direction			GetGyroscopeDirection(void){return GetLatestGyroscopeStatus().direction;}
//////////////////////////////////////////////////////////////////////////

		static void             mayInitAPI();

        PressStatus				m_virtualPadStatus[JOY_MAX_BUT];
        PressStatus				m_TpStatus;
        f32                     m_virtualPadAxes[JOY_MAX_AXES];

        bool                    m_Keys[256];        // array of key state
        bool                    m_LastKeys[256];    // array of last key state
        bool                    m_MouseButtons[5];  // array of mouse states

        bool                    bLAlt;
        bool                    bRAlt;
        bool                    bLCtrl;
        bool                    bRCtrl;
        bool                    bLShift;
        bool                    bRShift;
    };

} // namespace ITF

#endif //_ITF_INPUTADAPTER_3DS_H_