#include "precompiled_engine.h"

#include "engine/zinput/Nintendo/ZPad_Nintendo.h"
#include "engine/AdaptersInterfaces/InputAdapter.h"
#include "adapters/InputAdapter_Nintendo/InputPad_Nintendo.h"

namespace ITF
{

    void ZPad_Nintendo::InitDeviceInfo()
    {
        ITF_VECTOR<SInputInfo> & m_inputInfo = m_deviceInfo.m_inputInfo; 
        m_inputInfo.resize(CONTROL_MAX);

        m_inputInfo[PAD_STICK_LX].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_STICK_LY].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_STICK_RX].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_STICK_RY].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[PAD_DPAD_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_DPAD_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_DPAD_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_DPAD_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_MINUS].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_PLUS].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_L].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_R].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_ZL].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_ZR].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_LTHUMB].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_RTHUMB].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_A].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_B].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_X].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_Y].m_type = SInputInfo::INPUTTYPE_BUTTON;

        REGISTER_CONTROL(PAD_STICK_LX,          "STICK_LX");
        REGISTER_CONTROL(PAD_STICK_LY,          "STICK_LY");
        REGISTER_CONTROL(PAD_STICK_RX,          "STICK_RX");
        REGISTER_CONTROL(PAD_STICK_RY,          "STICK_RY");
        REGISTER_CONTROL(PAD_DPAD_UP,           "DPAD_UP");
        REGISTER_CONTROL(PAD_DPAD_DOWN,         "DPAD_DOWN");
        REGISTER_CONTROL(PAD_DPAD_LEFT,         "DPAD_LEFT");
        REGISTER_CONTROL(PAD_DPAD_RIGHT,        "DPAD_RIGHT");
        REGISTER_CONTROL(PAD_BUTTON_MINUS,      "BUTTON_MINUS");
        REGISTER_CONTROL(PAD_BUTTON_PLUS,       "BUTTON_PLUS");
        REGISTER_CONTROL(PAD_BUTTON_L,          "BUTTON_L");
        REGISTER_CONTROL(PAD_BUTTON_R,          "BUTTON_R");
        REGISTER_CONTROL(PAD_BUTTON_ZL,         "BUTTON_ZL");
        REGISTER_CONTROL(PAD_BUTTON_ZR,         "BUTTON_ZR");
        REGISTER_CONTROL(PAD_BUTTON_LTHUMB,     "LEFT_THUMB");
        REGISTER_CONTROL(PAD_BUTTON_RTHUMB,     "RIGHT_THUMB");
        REGISTER_CONTROL(PAD_BUTTON_A,          "BUTTON_A");
        REGISTER_CONTROL(PAD_BUTTON_B,          "BUTTON_B");
        REGISTER_CONTROL(PAD_BUTTON_X,          "BUTTON_X");
        REGISTER_CONTROL(PAD_BUTTON_Y,          "BUTTON_Y");
    }

    void ZPad_Nintendo::UpdateDeviceInfo( SDeviceInfo& deviceInfo )
    {
        // Get values from INPUT_ADAPTER instead of code again the same code .. 
        InputAdapter::PressStatus buts[JOY_MAX_BUT];
        f32 axes[JOY_MAX_AXES];

        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_id, buts, JOY_MAX_BUT);
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_id, axes, JOY_MAX_AXES);

        auto isPressed = [&buts](u32 input) -> SInputInfo::EButtonStatus
        {
            return (buts[input] == InputAdapter::Pressed || buts[input] == InputAdapter::JustPressed) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        };

        // And fill infos for control map
        deviceInfo.m_inputInfo[PAD_STICK_LX].m_axisInfo.m_axis              = axes[m_joyStickLeft_X];
        deviceInfo.m_inputInfo[PAD_STICK_LY].m_axisInfo.m_axis              = axes[m_joyStickLeft_Y];
        deviceInfo.m_inputInfo[PAD_STICK_RX].m_axisInfo.m_axis              = axes[m_joyStickRight_X];
        deviceInfo.m_inputInfo[PAD_STICK_RY].m_axisInfo.m_axis              = axes[m_joyStickRight_Y];

        deviceInfo.m_inputInfo[PAD_DPAD_UP].m_buttonInfo.m_status           = isPressed(m_joyButton_DPadU);
        deviceInfo.m_inputInfo[PAD_DPAD_DOWN].m_buttonInfo.m_status         = isPressed(m_joyButton_DPadD);
        deviceInfo.m_inputInfo[PAD_DPAD_LEFT].m_buttonInfo.m_status         = isPressed(m_joyButton_DPadL);
        deviceInfo.m_inputInfo[PAD_DPAD_RIGHT].m_buttonInfo.m_status        = isPressed(m_joyButton_DPadR);
        deviceInfo.m_inputInfo[PAD_BUTTON_PLUS].m_buttonInfo.m_status       = isPressed(m_joyButton_Plus);
        deviceInfo.m_inputInfo[PAD_BUTTON_MINUS].m_buttonInfo.m_status      = isPressed(m_joyButton_Minus);
        deviceInfo.m_inputInfo[PAD_BUTTON_LTHUMB].m_buttonInfo.m_status     = isPressed(m_joyButton_ThumbLeft);
        deviceInfo.m_inputInfo[PAD_BUTTON_RTHUMB].m_buttonInfo.m_status     = isPressed(m_joyButton_ThumbRight);
        deviceInfo.m_inputInfo[PAD_BUTTON_L].m_buttonInfo.m_status          = isPressed(m_joyButton_LB);
        deviceInfo.m_inputInfo[PAD_BUTTON_R].m_buttonInfo.m_status          = isPressed(m_joyButton_RB);
        deviceInfo.m_inputInfo[PAD_BUTTON_ZL].m_buttonInfo.m_status         = isPressed(m_joyButton_TriggerLeft);
        deviceInfo.m_inputInfo[PAD_BUTTON_ZR].m_buttonInfo.m_status         = isPressed(m_joyButton_TriggerRight);
        deviceInfo.m_inputInfo[PAD_BUTTON_A].m_buttonInfo.m_status          = isPressed(m_joyButton_A);
        deviceInfo.m_inputInfo[PAD_BUTTON_B].m_buttonInfo.m_status          = isPressed(m_joyButton_B);
        deviceInfo.m_inputInfo[PAD_BUTTON_X].m_buttonInfo.m_status          = isPressed(m_joyButton_X);
        deviceInfo.m_inputInfo[PAD_BUTTON_Y].m_buttonInfo.m_status          = isPressed(m_joyButton_Y);
    }

} //namespace ITF
