#ifndef _ITF_CONTROLLER_INPUT_SOURCE_H_
#define _ITF_CONTROLLER_INPUT_SOURCE_H_

#ifndef _ITF_IINPUT_SOURCE_H_
#include "engine/input/IInputSource.h"
#endif //_ITF_IINPUT_SOURCE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF
{
    class ControllerInputSource : public IInputSource
    {
    public:
        ControllerInputSource(u32 deviceId);
        virtual ~ControllerInputSource() override = default;
        void Update() override;
        bool IsConnected() const override;
        u32 GetDeviceId() const override { return m_deviceId; }
        InputAdapter::PressStatus GetButtonState(u32 buttonId) const override;
        f32 GetAxisState(u32 axisId) const override;
        PhysicalInput::Type GetInputType() const override { return PhysicalInput::ControllerButton; }

        void SetButtonState(u32 buttonId, bool pressed);
        void SetAxisState(u32 axisId, f32 value);
        void GetAllButtonStates(InputAdapter::PressStatus* buttons, u32 numButtons) const;
        void GetAllAxisStates(f32* axes, u32 numAxes) const;
        void SetConnected(bool connected);
        static f32 GetDeadzone() { return 0.2f; }

    private:
        u32 m_deviceId;
        bool m_connected;
        InputAdapter::PressStatus m_buttonStates[JOY_MAX_BUT];
        f32 m_axisStates[JOY_MAX_AXES];
        bool m_initialized;

        void UpdateButtonPressStatus(u32 buttonId, bool pressed);
    };

} // namespace ITF

#endif //_ITF_CONTROLLER_INPUT_SOURCE_H_

