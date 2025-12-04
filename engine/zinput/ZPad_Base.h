#ifndef _ITF_ZPAD_BASE_H_
#define _ITF_ZPAD_BASE_H_

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

namespace ITF
{
    class ZPad_Base : public IInputDevice
    {
    public:
        explicit ZPad_Base(u32 id) : IInputDevice(id)
        {
        }

        virtual ~ZPad_Base() = default;

        enum EStandardGamepadControl
        {
            STICK_LX = 0,
            STICK_LY,
            STICK_RX,
            STICK_RY,
            TRIGGER_LEFT,
            TRIGGER_RIGHT,
            DPAD_UP,
            DPAD_DOWN,
            DPAD_LEFT,
            DPAD_RIGHT,
            BUTTON_SELECT,
            BUTTON_START,
            BUTTON_L_SHOULDER,
            BUTTON_R_SHOULDER,
            BUTTON_L_THUMB,
            BUTTON_R_THUMB,
            BUTTON_FACE_SOUTH,
            BUTTON_FACE_EAST,
            BUTTON_FACE_WEST,
            BUTTON_FACE_NORTH,
            STICK_L_UP,
            STICK_L_DOWN,
            STICK_L_LEFT,
            STICK_L_RIGHT,
            STICK_R_UP,
            STICK_R_DOWN,
            STICK_R_LEFT,
            STICK_R_RIGHT,
            PLATFORM_SPECIFIC_START,
            BASE_CONTROL_COUNT = PLATFORM_SPECIFIC_START,
            CONTROL_MAX = BASE_CONTROL_COUNT
        };

    protected:
        struct PhysicalButtonMapping
        {
            u32 faceButtonSouth;
            u32 faceButtonEast;
            u32 faceButtonWest;
            u32 faceButtonNorth;
            u32 shoulderLeft;
            u32 shoulderRight;
            u32 selectButton;
            u32 startButton;
        };

        virtual void InitDeviceInfo() override;

        virtual void UpdateDeviceInfo(SDeviceInfo& deviceInfo) override;
        virtual void UpdateDeviceInfoRaw(SDeviceInfo& deviceInfo);

        virtual PhysicalButtonMapping GetPhysicalButtonMapping() const = 0;

        virtual void RegisterPlatformControlNames(const PhysicalButtonMapping&)
        {
        }

        virtual void InitPlatformSpecificControls()
        {
        }

        virtual void UpdatePlatformSpecificControls(SDeviceInfo&, const InputAdapter::PressStatus*, const f32*)
        {
        }

        void RegisterControlAlias(u32 controlIndex, const char* aliasName);

        u32 AddPlatformControl(SInputInfo::EInputType type, const char* controlName);

        void RegisterControlNames();

        void InitializeInputTypes();

        void UpdateAxisValues(SDeviceInfo& deviceInfo, const f32* axes);

        void UpdateButtonStates(SDeviceInfo& deviceInfo, const InputAdapter::PressStatus* buttons);

        void ApplyDirectionRemapping(SDeviceInfo& deviceInfo, const f32* axes, const InputAdapter::PressStatus* buttons);

        f32 GetAxisValueFromControl(u32 physicalControl, const f32* axes, const InputAdapter::PressStatus* buttons) const;

        static SInputInfo::EButtonStatus ConvertButtonStatus(InputAdapter::PressStatus status)
        {
            return (status == InputAdapter::Pressed || status == InputAdapter::JustPressed)
                       ? SInputInfo::BUTTONSTATUS_PRESS
                       : SInputInfo::BUTTONSTATUS_NONE;
        }
    };
} // namespace ITF

#endif // _ITF_ZPAD_BASE_H_
