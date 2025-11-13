#ifndef _ITF_INPUT_TYPES_H_
#define _ITF_INPUT_TYPES_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#include <cstring>

namespace ITF
{
    enum VirtualButton : u32
    {
        VIRTUAL_BUTTON_A = 0,
        VIRTUAL_BUTTON_B,
        VIRTUAL_BUTTON_X,
        VIRTUAL_BUTTON_Y,
        VIRTUAL_BUTTON_START,
        VIRTUAL_BUTTON_BACK,
        VIRTUAL_BUTTON_LB,
        VIRTUAL_BUTTON_RB,
        VIRTUAL_BUTTON_DPAD_UP,
        VIRTUAL_BUTTON_DPAD_DOWN,
        VIRTUAL_BUTTON_DPAD_LEFT,
        VIRTUAL_BUTTON_DPAD_RIGHT,
        VIRTUAL_BUTTON_LTHUMB,
        VIRTUAL_BUTTON_RTHUMB,
        VIRTUAL_BUTTON_COUNT
    };

    enum VirtualAxis : u32
    {
        VIRTUAL_AXIS_LEFT_STICK_X = 0,
        VIRTUAL_AXIS_LEFT_STICK_Y,
        VIRTUAL_AXIS_RIGHT_STICK_X,
        VIRTUAL_AXIS_RIGHT_STICK_Y,
        VIRTUAL_AXIS_LEFT_TRIGGER,
        VIRTUAL_AXIS_RIGHT_TRIGGER,
        VIRTUAL_AXIS_COUNT
    };

    struct PhysicalInput
    {
        enum Type : u32
        {
            Keyboard = 0,
            ControllerButton,
            ControllerAxis,
            Mouse,
            TypeCount
        };

        Type type;
        u32 deviceId;      
        u32 inputId;      
        f32 axisValue;     

        PhysicalInput()
            : type(Keyboard)
            , deviceId(0)
            , inputId(0)
            , axisValue(0.0f)
        {
        }

        PhysicalInput(Type _type, u32 _deviceId, u32 _inputId, f32 _axisValue = 0.0f)
            : type(_type)
            , deviceId(_deviceId)
            , inputId(_inputId)
            , axisValue(_axisValue)
        {
        }

        bool operator<(const PhysicalInput& other) const
        {
            if (type != other.type)
                return type < other.type;
            if (deviceId != other.deviceId)
                return deviceId < other.deviceId;
            if (inputId != other.inputId)
                return inputId < other.inputId;
            const f32 epsilon = 0.001f;
            return (axisValue + epsilon) < other.axisValue;
        }

        bool operator==(const PhysicalInput& other) const
        {
            const f32 epsilon = 0.001f;
            return type == other.type &&
                   deviceId == other.deviceId &&
                   inputId == other.inputId &&
                   fabsf(axisValue - other.axisValue) < epsilon;
        }

        bool operator!=(const PhysicalInput& other) const
        {
            return !(*this == other);
        }
    };

    struct VirtualInput
    {
        enum Type : u32
        {
            Button = 0,
            Axis,
            TypeCount
        };

        Type type;
        u32 virtualId;    

        VirtualInput()
            : type(Button)
            , virtualId(0)
        {
        }

        VirtualInput(Type _type, u32 _virtualId)
            : type(_type)
            , virtualId(_virtualId)
        {
        }

        bool operator<(const VirtualInput& other) const
        {
            if (type != other.type)
                return type < other.type;
            return virtualId < other.virtualId;
        }

        bool operator==(const VirtualInput& other) const
        {
            return type == other.type && virtualId == other.virtualId;
        }

        bool operator!=(const VirtualInput& other) const
        {
            return !(*this == other);
        }
    };

    static const u32 KEYBOARD_DEVICE_ID = 0xFFFFFFFF;
    static const u32 MOUSE_DEVICE_ID = 0xFFFFFFFE;

    ITF_INLINE VirtualInput MakeVirtualButton(VirtualButton button)
    {
        return VirtualInput(VirtualInput::Button, static_cast<u32>(button));
    }

    ITF_INLINE VirtualInput MakeVirtualAxis(VirtualAxis axis)
    {
        return VirtualInput(VirtualInput::Axis, static_cast<u32>(axis));
    }

    ITF_INLINE PhysicalInput MakeKeyboardInput(u32 keyCode)
    {
        return PhysicalInput(PhysicalInput::Keyboard, KEYBOARD_DEVICE_ID, keyCode);
    }

    ITF_INLINE PhysicalInput MakeControllerButtonInput(u32 deviceId, u32 buttonId)
    {
        return PhysicalInput(PhysicalInput::ControllerButton, deviceId, buttonId);
    }

    ITF_INLINE PhysicalInput MakeControllerAxisInput(u32 deviceId, u32 axisId, f32 axisValue)
    {
        return PhysicalInput(PhysicalInput::ControllerAxis, deviceId, axisId, axisValue);
    }

} // namespace ITF

#endif //_ITF_INPUT_TYPES_H_

