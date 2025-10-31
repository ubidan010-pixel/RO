#include "precompiled_engine.h"

#ifndef _ITF_INPUTDEVICE_CLASSIC_WII_H_
#include "engine/zinput/wii/ZClassic_wii.h"
#endif //_ITF_INPUTDEVICE_CLASSIC_WII_H_

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_

namespace ITF
{
    void ZClassic_wii::InitDeviceInfo() 
    {
        ITF_VECTOR<SInputInfo> & m_inputInfo = m_deviceInfo.m_inputInfo; 
        m_inputInfo.resize(CONTROL_MAX);

        m_inputInfo[ZClassic_wii::CL_STICK_LX].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[ZClassic_wii::CL_STICK_LY].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[ZClassic_wii::CL_STICK_RX].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[ZClassic_wii::CL_STICK_RY].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[ZClassic_wii::CL_DPAD_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_DPAD_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_DPAD_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_DPAD_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_SELECT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_START].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_ZR].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_ZL].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_R].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_L].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_A].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_B].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_X].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZClassic_wii::CL_BUTTON_Y].m_type = SInputInfo::INPUTTYPE_BUTTON;

        REGISTER_CONTROL(CL_STICK_LX,          "STICK_LX");
        REGISTER_CONTROL(CL_STICK_LY,          "STICK_LY");
        REGISTER_CONTROL(CL_STICK_RX,          "STICK_RX");
        REGISTER_CONTROL(CL_STICK_RY,          "STICK_RY");
        REGISTER_CONTROL(CL_DPAD_UP,           "DPAD_UP");
        REGISTER_CONTROL(CL_DPAD_DOWN,         "DPAD_DOWN");
        REGISTER_CONTROL(CL_DPAD_LEFT,         "DPAD_LEFT");
        REGISTER_CONTROL(CL_DPAD_RIGHT,        "DPAD_RIGHT");
        REGISTER_CONTROL(CL_BUTTON_SELECT,     "BUTTON_SELECT");
        REGISTER_CONTROL(CL_BUTTON_START,      "BUTTON_START");
        REGISTER_CONTROL(CL_BUTTON_L,          "BUTTON_R");
        REGISTER_CONTROL(CL_BUTTON_R,          "BUTTON_L");
        REGISTER_CONTROL(CL_BUTTON_ZR,         "BUTTON_ZR");
        REGISTER_CONTROL(CL_BUTTON_ZL,         "BUTTON_ZL");
        REGISTER_CONTROL(CL_BUTTON_A,          "BUTTON_A");
        REGISTER_CONTROL(CL_BUTTON_B,          "BUTTON_B");
        REGISTER_CONTROL(CL_BUTTON_X,          "BUTTON_X");
        REGISTER_CONTROL(CL_BUTTON_Y,          "BUTTON_Y");
        
        m_specificConfig = "CLASSIC";
    }


#define ISPRESSED(input) (buts[input] == InputAdapter::Pressed || buts[input] == InputAdapter::JustPressed) ?  SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;

    void ZClassic_wii::UpdateDeviceInfo( SDeviceInfo& deviceInfo )
    {
        // Get values from INPUT_ADAPTER instead of code again the same code .. 
        InputAdapter::PressStatus buts[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_id, buts, JOY_MAX_BUT);

        f32 axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_id, axes, JOY_MAX_AXES);

        // And fill infos for control map
        deviceInfo.m_inputInfo[CL_STICK_LX].m_axisInfo.m_axis              = axes[m_joyStickLeft_X];
        deviceInfo.m_inputInfo[CL_STICK_LY].m_axisInfo.m_axis              = axes[m_joyStickLeft_Y];
        deviceInfo.m_inputInfo[CL_STICK_RX].m_axisInfo.m_axis              = axes[m_joyStickRight_X];
        deviceInfo.m_inputInfo[CL_STICK_RY].m_axisInfo.m_axis              = axes[m_joyStickRight_Y];

        deviceInfo.m_inputInfo[CL_DPAD_UP].m_buttonInfo.m_status           = ISPRESSED(m_joyButton_DPadU);
        deviceInfo.m_inputInfo[CL_DPAD_DOWN].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_DPadD);
        deviceInfo.m_inputInfo[CL_DPAD_LEFT].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_DPadL);
        deviceInfo.m_inputInfo[CL_DPAD_RIGHT].m_buttonInfo.m_status        = ISPRESSED(m_joyButton_DPadR);
        deviceInfo.m_inputInfo[CL_BUTTON_START].m_buttonInfo.m_status      = ISPRESSED(m_joyButton_Start);
        deviceInfo.m_inputInfo[CL_BUTTON_SELECT].m_buttonInfo.m_status     = ISPRESSED(m_joyButton_Select);
        deviceInfo.m_inputInfo[CL_BUTTON_L].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_L);
        deviceInfo.m_inputInfo[CL_BUTTON_R].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_R);
        deviceInfo.m_inputInfo[CL_BUTTON_ZL].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_ZL);
        deviceInfo.m_inputInfo[CL_BUTTON_ZR].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_ZR);
        deviceInfo.m_inputInfo[CL_BUTTON_A].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_A);
        deviceInfo.m_inputInfo[CL_BUTTON_B].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_B);
        deviceInfo.m_inputInfo[CL_BUTTON_X].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_X);
        deviceInfo.m_inputInfo[CL_BUTTON_Y].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_Y);
    }
} //namespace ITF
