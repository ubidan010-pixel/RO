#include "precompiled_engine.h"

#include "engine/zinput/ps5/ZPad_ps5.h"

#include "engine/AdaptersInterfaces/InputAdapter.h"
#include "adapters/InputAdapter_PS5/InputAdapter_PS5.h"

namespace ITF
{
    void ZPad_ps5::InitDeviceInfo()
    {
        ITF_VECTOR<SInputInfo> & m_inputInfo = m_deviceInfo.m_inputInfo; 
        m_inputInfo.resize(CONTROL_MAX);

        m_inputInfo[PAD_STICK_LX].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_STICK_LY].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_STICK_RX].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_STICK_RY].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_TRIGGER_LEFT].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_TRIGGER_RIGHT].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_DPAD_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_DPAD_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_DPAD_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_DPAD_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_OPTIONS].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_L1].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_R1].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_LTHUMB].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_RTHUMB].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_CROSS].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_CIRCLE].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_SQUARE].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_TRIANGLE].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_TOUCHPAD].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_ACCELEROMETER].m_type = SInputInfo::INPUTTYPE_ACCELEROMETER;

        REGISTER_CONTROL(PAD_STICK_LX,          "STICK_LX");
        REGISTER_CONTROL(PAD_STICK_LY,          "STICK_LY");
        REGISTER_CONTROL(PAD_STICK_RX,          "STICK_RX");
        REGISTER_CONTROL(PAD_STICK_RY,          "STICK_RY");
        REGISTER_CONTROL(PAD_TRIGGER_LEFT,      "TRIGGER_LEFT");
        REGISTER_CONTROL(PAD_TRIGGER_RIGHT,     "TRIGGER_RIGHT");
        REGISTER_CONTROL(PAD_DPAD_UP,           "DPAD_UP");
        REGISTER_CONTROL(PAD_DPAD_DOWN,         "DPAD_DOWN");
        REGISTER_CONTROL(PAD_DPAD_LEFT,         "DPAD_LEFT");
        REGISTER_CONTROL(PAD_DPAD_RIGHT,        "DPAD_RIGHT");
        REGISTER_CONTROL(PAD_BUTTON_OPTIONS,    "BUTTON_OPTIONS");
        REGISTER_CONTROL(PAD_BUTTON_L1,         "BUTTON_L1");
        REGISTER_CONTROL(PAD_BUTTON_R1,         "BUTTON_R1");
        REGISTER_CONTROL(PAD_BUTTON_LTHUMB,     "LEFT_THUMB");
        REGISTER_CONTROL(PAD_BUTTON_RTHUMB,     "RIGHT_THUMB");
        REGISTER_CONTROL(PAD_BUTTON_CROSS,      "BUTTON_CROSS");
        REGISTER_CONTROL(PAD_BUTTON_CIRCLE,     "BUTTON_CIRCLE");
        REGISTER_CONTROL(PAD_BUTTON_SQUARE,     "BUTTON_SQUARE");
        REGISTER_CONTROL(PAD_BUTTON_TRIANGLE,   "BUTTON_TRIANGLE");
        REGISTER_CONTROL(PAD_BUTTON_TOUCHPAD,   "BUTTON_TOUCHPAD");
        REGISTER_CONTROL(PAD_ACCELEROMETER,     "ACCELEROMETER");
    }


    void ZPad_ps5::UpdateDeviceInfo( SDeviceInfo& deviceInfo )
    {
        // Get values from INPUT_ADAPTER instead of code again the same code .. 
        InputAdapter::PressStatus buts[JOY_MAX_BUT];

        auto isPressed = [&](u32 input) -> SInputInfo::EButtonStatus
        {
            return (buts[input] == InputAdapter::Pressed || buts[input] == InputAdapter::JustPressed) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        };

        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_id, buts, JOY_MAX_BUT);

        f32 axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_id, axes, JOY_MAX_AXES);

        // And fill infos for control map
        deviceInfo.m_inputInfo[PAD_STICK_LX].m_axisInfo.m_axis              = axes[m_joyStickLeft_X];
        deviceInfo.m_inputInfo[PAD_STICK_LY].m_axisInfo.m_axis              = axes[m_joyStickLeft_Y];
        deviceInfo.m_inputInfo[PAD_STICK_RX].m_axisInfo.m_axis              = axes[m_joyStickRight_X];
        deviceInfo.m_inputInfo[PAD_STICK_RY].m_axisInfo.m_axis              = axes[m_joyStickRight_Y];
        deviceInfo.m_inputInfo[PAD_TRIGGER_LEFT].m_axisInfo.m_axis          = axes[m_joyTrigger_Left];
        deviceInfo.m_inputInfo[PAD_TRIGGER_RIGHT].m_axisInfo.m_axis         = axes[m_joyTrigger_Right];

        deviceInfo.m_inputInfo[PAD_DPAD_UP].m_buttonInfo.m_status           = isPressed(m_joyButton_DPadU);
        deviceInfo.m_inputInfo[PAD_DPAD_DOWN].m_buttonInfo.m_status         = isPressed(m_joyButton_DPadD);
        deviceInfo.m_inputInfo[PAD_DPAD_LEFT].m_buttonInfo.m_status         = isPressed(m_joyButton_DPadL);
        deviceInfo.m_inputInfo[PAD_DPAD_RIGHT].m_buttonInfo.m_status        = isPressed(m_joyButton_DPadR);
        deviceInfo.m_inputInfo[PAD_BUTTON_OPTIONS].m_buttonInfo.m_status    = isPressed(m_joyButton_Start);
        deviceInfo.m_inputInfo[PAD_BUTTON_LTHUMB].m_buttonInfo.m_status     = isPressed(m_joyButton_ThumbLeft);
        deviceInfo.m_inputInfo[PAD_BUTTON_RTHUMB].m_buttonInfo.m_status     = isPressed(m_joyButton_ThumbRight);
        deviceInfo.m_inputInfo[PAD_BUTTON_L1].m_buttonInfo.m_status         = isPressed(m_joyButton_L1);
        deviceInfo.m_inputInfo[PAD_BUTTON_R1].m_buttonInfo.m_status         = isPressed(m_joyButton_R1);
        deviceInfo.m_inputInfo[PAD_BUTTON_CROSS].m_buttonInfo.m_status      = isPressed(m_joyButton_Cross);
        deviceInfo.m_inputInfo[PAD_BUTTON_CIRCLE].m_buttonInfo.m_status     = isPressed(m_joyButton_Circle);
        deviceInfo.m_inputInfo[PAD_BUTTON_SQUARE].m_buttonInfo.m_status     = isPressed(m_joyButton_Square);
        deviceInfo.m_inputInfo[PAD_BUTTON_TRIANGLE].m_buttonInfo.m_status   = isPressed(m_joyButton_Triangle);
        deviceInfo.m_inputInfo[PAD_BUTTON_TOUCHPAD].m_buttonInfo.m_status   = isPressed(m_joyButton_TouchPad);

        Vec3d acc;
        InputAdapter_PS5* ps5InputAdapter = static_cast<InputAdapter_PS5 *>(InputAdapter::getptr());
        ps5InputAdapter->getPadAcc(m_id, acc);

        deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_prevAcc.x = deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_acc.x;
        deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_prevAcc.y = deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_acc.y;
        deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_prevAcc.z = deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_acc.z;

        deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_acc.x = acc.m_x; 
        deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_acc.y = acc.m_y;
        deviceInfo.m_inputInfo[PAD_ACCELEROMETER].m_accelerometerInfo.m_acc.z = acc.m_z;
    }
} //namespace ITF
