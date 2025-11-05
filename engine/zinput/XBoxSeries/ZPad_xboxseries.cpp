#include "precompiled_engine.h"

#include "engine/zinput/XBoxSeries/ZPad_XBoxSeries.h"
#include "engine/AdaptersInterfaces/InputAdapter.h"

namespace ITF
{
#if defined(ITF_XBOX_SERIES)
    
    void ZPad_XBoxSeries::InitDeviceInfo()
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
        m_inputInfo[PAD_BUTTON_BACK].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_START].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_LSHOULDER].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[PAD_BUTTON_RSHOULDER].m_type = SInputInfo::INPUTTYPE_BUTTON;
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
        REGISTER_CONTROL(PAD_TRIGGER_LEFT,      "TRIGGER_LEFT");
        REGISTER_CONTROL(PAD_TRIGGER_RIGHT,     "TRIGGER_RIGHT");
        REGISTER_CONTROL(PAD_DPAD_UP,           "DPAD_UP");
        REGISTER_CONTROL(PAD_DPAD_DOWN,         "DPAD_DOWN");
        REGISTER_CONTROL(PAD_DPAD_LEFT,         "DPAD_LEFT");
        REGISTER_CONTROL(PAD_DPAD_RIGHT,        "DPAD_RIGHT");
        REGISTER_CONTROL(PAD_BUTTON_BACK,       "BUTTON_BACK");
        REGISTER_CONTROL(PAD_BUTTON_START,      "BUTTON_START");
        REGISTER_CONTROL(PAD_BUTTON_LSHOULDER,  "LEFT_SHOULDER");
        REGISTER_CONTROL(PAD_BUTTON_RSHOULDER,  "RIGHT_SHOULDER");
        REGISTER_CONTROL(PAD_BUTTON_LTHUMB,     "LEFT_THUMB");
        REGISTER_CONTROL(PAD_BUTTON_RTHUMB,     "RIGHT_THUMB");
        REGISTER_CONTROL(PAD_BUTTON_A,          "BUTTON_A");
        REGISTER_CONTROL(PAD_BUTTON_B,          "BUTTON_B");
        REGISTER_CONTROL(PAD_BUTTON_X,          "BUTTON_X");
        REGISTER_CONTROL(PAD_BUTTON_Y,          "BUTTON_Y");
    }


    #define ISPRESSED(input) (buts[input] == InputAdapter::Pressed || buts[input] == InputAdapter::JustPressed) ?  SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;

    void ZPad_XBoxSeries::UpdateDeviceInfo( SDeviceInfo& deviceInfo )
    {
        // Get values from INPUT_ADAPTER instead of code again the same code .. 
        InputAdapter::PressStatus buts[JOY_MAX_BUT];
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

        deviceInfo.m_inputInfo[PAD_DPAD_UP].m_buttonInfo.m_status           = ISPRESSED(m_joyButton_DPadU);
        deviceInfo.m_inputInfo[PAD_DPAD_DOWN].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_DPadD);
        deviceInfo.m_inputInfo[PAD_DPAD_LEFT].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_DPadL);
        deviceInfo.m_inputInfo[PAD_DPAD_RIGHT].m_buttonInfo.m_status        = ISPRESSED(m_joyButton_DPadR);
        deviceInfo.m_inputInfo[PAD_BUTTON_START].m_buttonInfo.m_status      = ISPRESSED(m_joyButton_Start);
        deviceInfo.m_inputInfo[PAD_BUTTON_BACK].m_buttonInfo.m_status       = ISPRESSED(m_joyButton_Back);
        deviceInfo.m_inputInfo[PAD_BUTTON_LTHUMB].m_buttonInfo.m_status     = ISPRESSED(m_joyButton_ThumbLeft);
        deviceInfo.m_inputInfo[PAD_BUTTON_RTHUMB].m_buttonInfo.m_status     = ISPRESSED(m_joyButton_ThumbRight);
        deviceInfo.m_inputInfo[PAD_BUTTON_LSHOULDER].m_buttonInfo.m_status  = ISPRESSED(m_joyButton_LB);
        deviceInfo.m_inputInfo[PAD_BUTTON_RSHOULDER].m_buttonInfo.m_status  = ISPRESSED(m_joyButton_RB);
        deviceInfo.m_inputInfo[PAD_BUTTON_A].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_A);
        deviceInfo.m_inputInfo[PAD_BUTTON_B].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_B);
        deviceInfo.m_inputInfo[PAD_BUTTON_X].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_X);
        deviceInfo.m_inputInfo[PAD_BUTTON_Y].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_Y);
    }
#endif
} //namespace ITF
