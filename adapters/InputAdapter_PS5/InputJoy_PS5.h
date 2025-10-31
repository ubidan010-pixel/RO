#ifndef INPUT_ADAPTER_JOY_PS5_H_
#define INPUT_ADAPTER_JOY_PS5_H_

#pragma once

#include "engine/AdaptersInterfaces/InputAdapter.h"

namespace ITF
{
    struct CONTROL_DESCRIPTION
    {
        u32             ID;
        const char*     szName;
        u32             dwDataType;
        i32             mappingValue; // replaced u32 for -1 values on PS5
    };

    class InputJoy_PS5
    {
    public:
        enum JoyStickAxes
        {
            AXE_LEFT_X = 0,
            AXE_LEFT_Y,
            AXE_TRIGGER_LEFT,
            AXE_RIGHT_X,
            AXE_RIGHT_Y,
            AXE_TRIGGER_RIGHT,

            MAX_AXES
        };

        enum JoyType
        {
           JOY_TYPE_NONE = -1,
           JOY_TYPE_DUALSENSE = 0,
        };

                    InputJoy_PS5();
        virtual     ~InputJoy_PS5() = default;

        virtual     void        init(i32 _portHandle, i32 _id ) = 0;
        virtual     void        done(){}

        virtual     f32         getAxe(u32 _axe) const {return 0.f;};
        virtual     InputAdapter::PressStatus   getButton(u32 _button) const;

        virtual     bbool   isConnected() const {return bfalse;}
        virtual     InputAdapter::PadType getPadType() const {return InputAdapter::Pad_PS5;}

        virtual     void    updateInputState(){}

        virtual     void    getAcceleration(Vec3d& acc) const;
        virtual     Vec3d   getAngles() const;
        virtual     void    resetOrientation() {}
        virtual     void    getLowLevelAccels(ITF_VECTOR<Vec3d>& _accels, bbool _invertX, bbool _invertY, bbool _invertZ, f32 _maxLimit) const {}

        virtual     void    padVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time){}

        JoyType     getType() const { return m_type; }

    protected:
        void    init( i32 _portHandle, i32 _id, JoyType type);

        i32     m_id;
        i32     m_portHandle;
        InputAdapter::PressStatus   m_buttons[JOY_MAX_BUT];
        JoyType m_type;
    };

}

#endif //INPUT_ADAPTER_JOY_PS5_H_
