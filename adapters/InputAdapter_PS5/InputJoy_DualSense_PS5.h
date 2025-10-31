#ifndef INPUT_JOY_DUALSENSE_H_
#define INPUT_JOY_DUALSENSE_H_

#pragma once

#include <pad.h>

#include "InputJoy_PS5.h"
#include "core/container/extendedVector.h"

namespace ITF
{
    class InputJoy_DualSense : public InputJoy_PS5
    {
    public:
        InputJoy_DualSense();
        ~InputJoy_DualSense(){}

        virtual void    init(i32 _portHandle, i32 _id );
        virtual f32     getAxe(u32 _axe) const;

        virtual bbool   isConnected() const;
        virtual     InputAdapter::PadType getPadType() const;

        virtual void    updateInputState();

        virtual void    getAcceleration(Vec3d& acc) const;
        virtual Vec3d   getAngles() const;
        virtual void    resetOrientation();
        virtual void    getLowLevelAccels(Vector<Vec3d>& _accels, bbool _invertX, bbool _invertY, bbool _invertZ, f32 _maxLimit) const;

        virtual void    padVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time);

    protected:
        ScePadData  m_padData[SCE_PAD_MAX_DATA_NUM];
        ScePadControllerInformation m_padInfo;
        i32         m_statesNo;
        ScePadData  m_latestMoveData;

        bbool       m_isVibrating;
        f64         m_vibrationStartTime;
        f64         m_vibrationTime;
        f32         m_leftMotorSpeed;
        f32         m_rightMotorSpeed;

        struct TouchPadData
        {
            Vec2d   pos;
            bbool   valid;
        };
        TouchPadData    m_lastTouchData[SCE_PAD_MAX_TOUCH_NUM];
        TouchPadData    m_currentTouchData[SCE_PAD_MAX_TOUCH_NUM];
    };
}

#endif //INPUT_JOY_DUALSENSE_H_
