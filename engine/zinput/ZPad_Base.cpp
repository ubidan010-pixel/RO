#include "precompiled_engine.h"

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif

namespace ITF
{
    void ZPad_Base::InitDeviceInfo()
    {
        m_deviceInfo.m_inputInfo.clear();
        m_deviceInfo.m_inputInfo.resize(CONTROL_MAX);
        InitializeInputTypes();

        PhysicalButtonMapping mapping = GetPhysicalButtonMapping();

        RegisterControlNames(mapping);
        RegisterPlatformControlNames(mapping);
        InitPlatformSpecificControls();
    }

    void ZPad_Base::InitializeInputTypes()
    {
        ITF_VECTOR<SInputInfo>& inputInfo = m_deviceInfo.m_inputInfo;

        inputInfo[STICK_LX].m_type = SInputInfo::INPUTTYPE_AXIS;
        inputInfo[STICK_LY].m_type = SInputInfo::INPUTTYPE_AXIS;
        inputInfo[STICK_RX].m_type = SInputInfo::INPUTTYPE_AXIS;
        inputInfo[STICK_RY].m_type = SInputInfo::INPUTTYPE_AXIS;

        inputInfo[TRIGGER_LEFT].m_type = SInputInfo::INPUTTYPE_AXIS;
        inputInfo[TRIGGER_RIGHT].m_type = SInputInfo::INPUTTYPE_AXIS;

        inputInfo[DPAD_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[DPAD_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[DPAD_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[DPAD_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;

        inputInfo[BUTTON_SELECT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[BUTTON_START].m_type = SInputInfo::INPUTTYPE_BUTTON;

        inputInfo[BUTTON_L_SHOULDER].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[BUTTON_R_SHOULDER].m_type = SInputInfo::INPUTTYPE_BUTTON;

        inputInfo[BUTTON_L_THUMB].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[BUTTON_R_THUMB].m_type = SInputInfo::INPUTTYPE_BUTTON;

        inputInfo[BUTTON_FACE_SOUTH].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[BUTTON_FACE_EAST].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[BUTTON_FACE_WEST].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[BUTTON_FACE_NORTH].m_type = SInputInfo::INPUTTYPE_BUTTON;
    }

    void ZPad_Base::RegisterControlNames(const PhysicalButtonMapping& mapping)
    {
        REGISTER_CONTROL(STICK_LX, "STICK_LX")
        REGISTER_CONTROL(STICK_LY, "STICK_LY")
        REGISTER_CONTROL(STICK_RX, "STICK_RX")
        REGISTER_CONTROL(STICK_RY, "STICK_RY")
        REGISTER_CONTROL(TRIGGER_LEFT, "TRIGGER_LEFT")
        REGISTER_CONTROL(TRIGGER_RIGHT, "TRIGGER_RIGHT")
        REGISTER_CONTROL(DPAD_UP, "DPAD_UP")
        REGISTER_CONTROL(DPAD_DOWN, "DPAD_DOWN")
        REGISTER_CONTROL(DPAD_LEFT, "DPAD_LEFT")
        REGISTER_CONTROL(DPAD_RIGHT, "DPAD_RIGHT")
        REGISTER_CONTROL(BUTTON_SELECT, "BUTTON_SELECT")
        REGISTER_CONTROL(BUTTON_START, "BUTTON_START")
        REGISTER_CONTROL(BUTTON_L_SHOULDER, "LEFT_SHOULDER")
        REGISTER_CONTROL(BUTTON_R_SHOULDER, "RIGHT_SHOULDER")
        REGISTER_CONTROL(BUTTON_L_THUMB, "LEFT_THUMB")
        REGISTER_CONTROL(BUTTON_R_THUMB, "RIGHT_THUMB")

        REGISTER_CONTROL(BUTTON_FACE_SOUTH, "VB_FACE_SOUTH")
        REGISTER_CONTROL(BUTTON_FACE_EAST, "VB_FACE_EAST")
        REGISTER_CONTROL(BUTTON_FACE_WEST, "VB_FACE_WEST")
        REGISTER_CONTROL(BUTTON_FACE_NORTH, "VB_FACE_NORTH")
        REGISTER_CONTROL(BUTTON_L_SHOULDER, "VB_L_SHOULDER")
        REGISTER_CONTROL(BUTTON_R_SHOULDER, "VB_R_SHOULDER")
        REGISTER_CONTROL(TRIGGER_LEFT, "VB_L_TRIGGER")
        REGISTER_CONTROL(TRIGGER_RIGHT, "VB_R_TRIGGER")
        REGISTER_CONTROL(BUTTON_SELECT, "VB_SELECT")
        REGISTER_CONTROL(BUTTON_START, "VB_START")
        REGISTER_CONTROL(BUTTON_L_THUMB, "VB_L_THUMB")
        REGISTER_CONTROL(BUTTON_R_THUMB, "VB_R_THUMB")
        REGISTER_CONTROL(STICK_LX, "VB_STICK_LX")
        REGISTER_CONTROL(STICK_LY, "VB_STICK_LY")
        REGISTER_CONTROL(STICK_RX, "VB_STICK_RX")
        REGISTER_CONTROL(STICK_RY, "VB_STICK_RY")
        REGISTER_CONTROL(DPAD_UP, "VB_DPAD_UP")
        REGISTER_CONTROL(DPAD_DOWN, "VB_DPAD_DOWN")
        REGISTER_CONTROL(DPAD_LEFT, "VB_DPAD_LEFT")
        REGISTER_CONTROL(DPAD_RIGHT, "VB_DPAD_RIGHT")
    }

    void ZPad_Base::UpdateDeviceInfo(SDeviceInfo& deviceInfo)
    {
        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_id, buttons, JOY_MAX_BUT);
        f32 axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_id, axes, JOY_MAX_AXES);
        UpdateAxisValues(deviceInfo, axes);
        UpdateButtonStates(deviceInfo, buttons);
        UpdatePlatformSpecificControls(deviceInfo, buttons, axes);
    }

    void ZPad_Base::UpdateAxisValues(SDeviceInfo& deviceInfo, const f32* axes)
    {
        deviceInfo.m_inputInfo[STICK_LX].m_axisInfo.m_axis = axes[m_joyStickLeft_X];
        deviceInfo.m_inputInfo[STICK_LY].m_axisInfo.m_axis = axes[m_joyStickLeft_Y];
        deviceInfo.m_inputInfo[STICK_RX].m_axisInfo.m_axis = axes[m_joyStickRight_X];
        deviceInfo.m_inputInfo[STICK_RY].m_axisInfo.m_axis = axes[m_joyStickRight_Y];
        deviceInfo.m_inputInfo[TRIGGER_LEFT].m_axisInfo.m_axis = axes[m_joyTrigger_Left];
        deviceInfo.m_inputInfo[TRIGGER_RIGHT].m_axisInfo.m_axis = axes[m_joyTrigger_Right];
    }

    void ZPad_Base::UpdateButtonStates(SDeviceInfo& deviceInfo, const InputAdapter::PressStatus* buttons)
    {
        PhysicalButtonMapping mapping = GetPhysicalButtonMapping();

        deviceInfo.m_inputInfo[DPAD_UP].m_buttonInfo.m_status = ConvertButtonStatus(buttons[m_joyButton_DPadU]);
        deviceInfo.m_inputInfo[DPAD_DOWN].m_buttonInfo.m_status = ConvertButtonStatus(buttons[m_joyButton_DPadD]);
        deviceInfo.m_inputInfo[DPAD_LEFT].m_buttonInfo.m_status = ConvertButtonStatus(buttons[m_joyButton_DPadL]);
        deviceInfo.m_inputInfo[DPAD_RIGHT].m_buttonInfo.m_status = ConvertButtonStatus(buttons[m_joyButton_DPadR]);

        deviceInfo.m_inputInfo[BUTTON_START].m_buttonInfo.m_status = ConvertButtonStatus(buttons[mapping.startButton]);
        deviceInfo.m_inputInfo[BUTTON_SELECT].m_buttonInfo.m_status =
            ConvertButtonStatus(buttons[mapping.selectButton]);

        deviceInfo.m_inputInfo[BUTTON_L_THUMB].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[m_joyButton_ThumbLeft]);
        deviceInfo.m_inputInfo[BUTTON_R_THUMB].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[m_joyButton_ThumbRight]);

        deviceInfo.m_inputInfo[BUTTON_L_SHOULDER].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[mapping.shoulderLeft]);
        deviceInfo.m_inputInfo[BUTTON_R_SHOULDER].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[mapping.shoulderRight]);

        deviceInfo.m_inputInfo[BUTTON_FACE_SOUTH].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[mapping.faceButtonSouth]);
        deviceInfo.m_inputInfo[BUTTON_FACE_EAST].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[mapping.faceButtonEast]);
        deviceInfo.m_inputInfo[BUTTON_FACE_WEST].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[mapping.faceButtonWest]);
        deviceInfo.m_inputInfo[BUTTON_FACE_NORTH].m_buttonInfo.m_status = ConvertButtonStatus(
            buttons[mapping.faceButtonNorth]);
    }

    void ZPad_Base::RegisterControlAlias(u32 controlIndex, const char* aliasName)
    {
        if (!aliasName || !aliasName[0])
            return;

        ITF_ASSERT(controlIndex < m_deviceInfo.m_inputInfo.size());
        REGISTER_CONTROL(controlIndex, aliasName)
    }

    u32 ZPad_Base::AddPlatformControl(SInputInfo::EInputType type, const char* controlName)
    {
        ITF_ASSERT(controlName && controlName[0]);

        SInputInfo info;
        ITF_MemSet(&info, 0, sizeof(SInputInfo));
        info.m_type = type;

        m_deviceInfo.m_inputInfo.push_back(info);
        u32 index = m_deviceInfo.m_inputInfo.size() - 1;
        REGISTER_CONTROL(index, controlName)
        return index;
    }
} // namespace ITF
