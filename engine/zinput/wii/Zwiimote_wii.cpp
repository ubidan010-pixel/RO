#include "precompiled_engine.h"

#ifndef _ITF_INPUTDEVICE_WIIMOTE_WII_H_
#include "engine/zinput/wii/Zwiimote_wii.h"
#endif //_ITF_INPUTDEVICE_WIIMOTE_WII_H_

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_

namespace ITF
{

    void ZWiimote_wii::InitDeviceInfo()
    {
        ITF_VECTOR<SInputInfo> & m_inputInfo = m_deviceInfo.m_inputInfo; 
        m_inputInfo.resize(CONTROL_MAX);

        m_inputInfo[ZWiimote_wii::SW_DPAD_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_DPAD_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_DPAD_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_DPAD_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_BUTTON_PLUS].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_BUTTON_MINUS].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_BUTTON_A].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_BUTTON_B].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_BUTTON_1].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_BUTTON_2].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZWiimote_wii::SW_ACCELEROMETER].m_type = SInputInfo::INPUTTYPE_ACCELEROMETER;

        REGISTER_CONTROL(SW_DPAD_UP,           "DPAD_UP");
        REGISTER_CONTROL(SW_DPAD_DOWN,         "DPAD_DOWN");
        REGISTER_CONTROL(SW_DPAD_LEFT,         "DPAD_LEFT");
        REGISTER_CONTROL(SW_DPAD_RIGHT,        "DPAD_RIGHT");
        REGISTER_CONTROL(SW_BUTTON_MINUS,      "BUTTON_MINUS");
        REGISTER_CONTROL(SW_BUTTON_PLUS,       "BUTTON_PLUS");
        REGISTER_CONTROL(SW_BUTTON_A,          "BUTTON_A");
        REGISTER_CONTROL(SW_BUTTON_B,          "BUTTON_B");
        REGISTER_CONTROL(SW_BUTTON_1,          "BUTTON_1");
        REGISTER_CONTROL(SW_BUTTON_2,          "BUTTON_2");
        REGISTER_CONTROL(SW_ACCELEROMETER,     "ACCELEROMETER");

        m_specificConfig = "SIDEWAY";
    }


#define ISPRESSED(input) (buts[input] == InputAdapter::Pressed || buts[input] == InputAdapter::JustPressed) ?  SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;

    void ZWiimote_wii::UpdateDeviceInfo( SDeviceInfo& deviceInfo )
    {
        // Get values from INPUT_ADAPTER instead of code again the same code .. 
        InputAdapter::PressStatus buts[JOY_MAX_BUT];        
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_id, buts, JOY_MAX_BUT);

        // And fill infos for control map
        deviceInfo.m_inputInfo[SW_DPAD_UP].m_buttonInfo.m_status           = ISPRESSED(m_joyButton_DPadU);
        deviceInfo.m_inputInfo[SW_DPAD_DOWN].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_DPadD);
        deviceInfo.m_inputInfo[SW_DPAD_LEFT].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_DPadL);
        deviceInfo.m_inputInfo[SW_DPAD_RIGHT].m_buttonInfo.m_status        = ISPRESSED(m_joyButton_DPadR);
        deviceInfo.m_inputInfo[SW_BUTTON_PLUS].m_buttonInfo.m_status       = ISPRESSED(m_joyButton_Plus);
        deviceInfo.m_inputInfo[SW_BUTTON_MINUS].m_buttonInfo.m_status      = ISPRESSED(m_joyButton_Minus);
        deviceInfo.m_inputInfo[SW_BUTTON_A].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_A);
        deviceInfo.m_inputInfo[SW_BUTTON_B].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_B);
        deviceInfo.m_inputInfo[SW_BUTTON_1].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_1);
        deviceInfo.m_inputInfo[SW_BUTTON_2].m_buttonInfo.m_status          = ISPRESSED(m_joyButton_2);

        Vec3d acc ;
        INPUT_ADAPTER_WII->getWiimoteAcc(m_id, &acc);

        deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_prevAcc.x = deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_acc.x;
        deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_prevAcc.y = deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_acc.y;
        deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_prevAcc.z = deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_acc.z;

        deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_acc.x = acc.m_x; 
        deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_acc.y = acc.m_y; 
        deviceInfo.m_inputInfo[SW_ACCELEROMETER].m_accelerometerInfo.m_acc.z = acc.m_z; 
    }
} //namespace ITF
