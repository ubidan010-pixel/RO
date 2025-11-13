#ifndef _ITF_IINPUT_SOURCE_H_
#define _ITF_IINPUT_SOURCE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_INPUT_TYPES_H_
#include "engine/input/InputTypes.h"
#endif //_ITF_INPUT_TYPES_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

namespace ITF
{
    class IInputSource
    {
    public:
        virtual ~IInputSource() = default;

        virtual void Update() = 0;

        virtual bool IsConnected() const = 0;

        virtual u32 GetDeviceId() const = 0;

        virtual InputAdapter::PressStatus GetButtonState(u32 buttonId) const
        {
            ITF_UNUSED(buttonId);
            return InputAdapter::Released;
        }

        virtual f32 GetAxisState(u32 axisId) const
        {
            ITF_UNUSED(axisId);
            return 0.0f;
        }

        virtual InputAdapter::PressStatus GetKeyState(u32 keyCode) const
        {
            ITF_UNUSED(keyCode);
            return InputAdapter::Released;
        }

        virtual PhysicalInput::Type GetInputType() const = 0;

        virtual void OnConnected() {}

        virtual void OnDisconnected() {}
    };

} // namespace ITF

#endif //_ITF_IINPUT_SOURCE_H_
