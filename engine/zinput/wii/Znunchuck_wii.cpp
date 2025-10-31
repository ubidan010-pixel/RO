#include "precompiled_engine.h"

#ifndef _ITF_INPUTDEVICE_NUNCHUCK_WII_H_
#include "engine/zinput/wii/Znunchuck_wii.h"
#endif //_ITF_INPUTDEVICE_NUNCHUCK_WII_H_

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_

namespace ITF
{
    void ZNunchuck_wii::InitDeviceInfo()
    {
        ITF_VECTOR<SInputInfo> & m_inputInfo = m_deviceInfo.m_inputInfo; 
        m_inputInfo.resize(CONTROL_MAX);

        m_inputInfo[ZNunchuck_wii::NC_STICK_X].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[ZNunchuck_wii::NC_STICK_Y].m_type = SInputInfo::INPUTTYPE_AXIS;
        m_inputInfo[ZNunchuck_wii::NC_BUTTON_C].m_type = SInputInfo::INPUTTYPE_BUTTON;
        m_inputInfo[ZNunchuck_wii::NC_BUTTON_Z].m_type = SInputInfo::INPUTTYPE_BUTTON;

        REGISTER_CONTROL(NC_STICK_X,          "STICK_X");
        REGISTER_CONTROL(NC_STICK_Y,          "STICK_Y");
        REGISTER_CONTROL(NC_BUTTON_C,         "BUTTON_C");
        REGISTER_CONTROL(NC_BUTTON_Z,         "BUTTON_Z");

        m_specificConfig = "NUNCHUCK";
    }


#define ISPRESSED(input) (buts[input] == InputAdapter::Pressed || buts[input] == InputAdapter::JustPressed) ?  SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;

    void ZNunchuck_wii::UpdateDeviceInfo( SDeviceInfo& deviceInfo )
    {
        // Get values from INPUT_ADAPTER instead of code again the same code .. 
        InputAdapter::PressStatus buts[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_id, buts, JOY_MAX_BUT);

        f32 axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_id, axes, JOY_MAX_AXES);

        // And fill infos for control map
        deviceInfo.m_inputInfo[NC_STICK_X].m_axisInfo.m_axis              = axes[m_joyStick_X];
        deviceInfo.m_inputInfo[NC_STICK_Y].m_axisInfo.m_axis              = axes[m_joyStick_Y];
        deviceInfo.m_inputInfo[NC_BUTTON_C].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_C);
        deviceInfo.m_inputInfo[NC_BUTTON_Z].m_buttonInfo.m_status         = ISPRESSED(m_joyButton_Z);
    }
} //namespace ITF
