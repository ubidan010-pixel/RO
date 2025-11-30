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
        m_deviceInfo.m_inputInfo.resize(BASE_CONTROL_COUNT);
        InitializeInputTypes();

        PhysicalButtonMapping mapping = GetPhysicalButtonMapping();

        RegisterControlNames();
        RegisterPlatformControlNames(mapping);
        InitPlatformSpecificControls();
        m_controlRemap.resize(m_deviceInfo.m_inputInfo.size());
        ResetRemapping();
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

        inputInfo[STICK_L_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[STICK_L_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[STICK_L_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[STICK_L_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;

        inputInfo[STICK_R_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[STICK_R_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[STICK_R_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        inputInfo[STICK_R_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;
    }

    void ZPad_Base::RegisterControlNames()
    {
        struct ControlName
        {
            u32 index;
            const char* name;
        };

        static const ControlName kCanonicalNames[] = {
            {STICK_LX, "STICK_LX"},
            {STICK_LY, "STICK_LY"},
            {STICK_RX, "STICK_RX"},
            {STICK_RY, "STICK_RY"},
            {TRIGGER_LEFT, "TRIGGER_LEFT"},
            {TRIGGER_RIGHT, "TRIGGER_RIGHT"},
            {DPAD_UP, "DPAD_UP"},
            {DPAD_DOWN, "DPAD_DOWN"},
            {DPAD_LEFT, "DPAD_LEFT"},
            {DPAD_RIGHT, "DPAD_RIGHT"},
            {BUTTON_SELECT, "BUTTON_SELECT"},
            {BUTTON_START, "BUTTON_START"},
            {BUTTON_L_SHOULDER, "LEFT_SHOULDER"},
            {BUTTON_R_SHOULDER, "RIGHT_SHOULDER"},
            {BUTTON_L_THUMB, "LEFT_THUMB"},
            {BUTTON_R_THUMB, "RIGHT_THUMB"},
            {BUTTON_FACE_SOUTH, "FACE_SOUTH"},
            {BUTTON_FACE_EAST, "FACE_EAST"},
            {BUTTON_FACE_WEST, "FACE_WEST"},
            {BUTTON_FACE_NORTH, "FACE_NORTH"},
            {STICK_L_UP, "STICK_L_UP"},
            {STICK_L_DOWN, "STICK_L_DOWN"},
            {STICK_L_LEFT, "STICK_L_LEFT"},
            {STICK_L_RIGHT, "STICK_L_RIGHT"},
            {STICK_R_UP, "STICK_R_UP"},
            {STICK_R_DOWN, "STICK_R_DOWN"},
            {STICK_R_LEFT, "STICK_R_LEFT"},
            {STICK_R_RIGHT, "STICK_R_RIGHT"},
        };

        for (const ControlName& control : kCanonicalNames)
        {
            REGISTER_CONTROL(control.index, control.name)
        }
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
        ApplyRemapping(deviceInfo);
        ApplyDirectionRemapping(deviceInfo, axes, buttons);
    }

    void ZPad_Base::UpdateAxisValues(SDeviceInfo& deviceInfo, const f32* axes)
    {
        deviceInfo.m_inputInfo[STICK_LX].m_axisInfo.m_axis = axes[m_joyStickLeft_X];
        deviceInfo.m_inputInfo[STICK_LY].m_axisInfo.m_axis = axes[m_joyStickLeft_Y];
        deviceInfo.m_inputInfo[STICK_RX].m_axisInfo.m_axis = axes[m_joyStickRight_X];
        deviceInfo.m_inputInfo[STICK_RY].m_axisInfo.m_axis = axes[m_joyStickRight_Y];
        deviceInfo.m_inputInfo[TRIGGER_LEFT].m_axisInfo.m_axis = axes[m_joyTrigger_Left];
        deviceInfo.m_inputInfo[TRIGGER_RIGHT].m_axisInfo.m_axis = axes[m_joyTrigger_Right];

        const f32 threshold = 0.5f;
        deviceInfo.m_inputInfo[STICK_L_UP].m_buttonInfo.m_status = (axes[m_joyStickLeft_Y] > threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        deviceInfo.m_inputInfo[STICK_L_DOWN].m_buttonInfo.m_status = (axes[m_joyStickLeft_Y] < -threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        deviceInfo.m_inputInfo[STICK_L_LEFT].m_buttonInfo.m_status = (axes[m_joyStickLeft_X] < -threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        deviceInfo.m_inputInfo[STICK_L_RIGHT].m_buttonInfo.m_status = (axes[m_joyStickLeft_X] > threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;

        deviceInfo.m_inputInfo[STICK_R_UP].m_buttonInfo.m_status = (axes[m_joyStickRight_Y] > threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        deviceInfo.m_inputInfo[STICK_R_DOWN].m_buttonInfo.m_status = (axes[m_joyStickRight_Y] < -threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        deviceInfo.m_inputInfo[STICK_R_LEFT].m_buttonInfo.m_status = (axes[m_joyStickRight_X] < -threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
        deviceInfo.m_inputInfo[STICK_R_RIGHT].m_buttonInfo.m_status = (axes[m_joyStickRight_X] > threshold) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
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

    f32 ZPad_Base::GetAxisValueFromControl(u32 physicalControl, const f32* axes, const InputAdapter::PressStatus* buttons) const
    {
        switch (physicalControl)
        {
        case STICK_L_UP:
            return f32_Max(0.0f, axes[m_joyStickLeft_Y]);
        case STICK_L_DOWN:
            return f32_Max(0.0f, -axes[m_joyStickLeft_Y]);
        case STICK_L_LEFT:
            return f32_Max(0.0f, -axes[m_joyStickLeft_X]);
        case STICK_L_RIGHT:
            return f32_Max(0.0f, axes[m_joyStickLeft_X]);
        case STICK_R_UP:
            return f32_Max(0.0f, axes[m_joyStickRight_Y]);
        case STICK_R_DOWN:
            return f32_Max(0.0f, -axes[m_joyStickRight_Y]);
        case STICK_R_LEFT:
            return f32_Max(0.0f, -axes[m_joyStickRight_X]);
        case STICK_R_RIGHT:
            return f32_Max(0.0f, axes[m_joyStickRight_X]);
        default:
            break;
        }

        if (physicalControl == TRIGGER_LEFT)
            return axes[m_joyTrigger_Left];
        if (physicalControl == TRIGGER_RIGHT)
            return axes[m_joyTrigger_Right];

        PhysicalButtonMapping mapping = GetPhysicalButtonMapping();
        bbool isPressed = bfalse;

        switch (physicalControl)
        {
        case DPAD_UP:
            isPressed = (buttons[m_joyButton_DPadU] == InputAdapter::Pressed || buttons[m_joyButton_DPadU] == InputAdapter::JustPressed);
            break;
        case DPAD_DOWN:
            isPressed = (buttons[m_joyButton_DPadD] == InputAdapter::Pressed || buttons[m_joyButton_DPadD] == InputAdapter::JustPressed);
            break;
        case DPAD_LEFT:
            isPressed = (buttons[m_joyButton_DPadL] == InputAdapter::Pressed || buttons[m_joyButton_DPadL] == InputAdapter::JustPressed);
            break;
        case DPAD_RIGHT:
            isPressed = (buttons[m_joyButton_DPadR] == InputAdapter::Pressed || buttons[m_joyButton_DPadR] == InputAdapter::JustPressed);
            break;
        case BUTTON_FACE_SOUTH:
            isPressed = (buttons[mapping.faceButtonSouth] == InputAdapter::Pressed || buttons[mapping.faceButtonSouth] == InputAdapter::JustPressed);
            break;
        case BUTTON_FACE_EAST:
            isPressed = (buttons[mapping.faceButtonEast] == InputAdapter::Pressed || buttons[mapping.faceButtonEast] == InputAdapter::JustPressed);
            break;
        case BUTTON_FACE_WEST:
            isPressed = (buttons[mapping.faceButtonWest] == InputAdapter::Pressed || buttons[mapping.faceButtonWest] == InputAdapter::JustPressed);
            break;
        case BUTTON_FACE_NORTH:
            isPressed = (buttons[mapping.faceButtonNorth] == InputAdapter::Pressed || buttons[mapping.faceButtonNorth] == InputAdapter::JustPressed);
            break;
        case BUTTON_L_SHOULDER:
            isPressed = (buttons[mapping.shoulderLeft] == InputAdapter::Pressed || buttons[mapping.shoulderLeft] == InputAdapter::JustPressed);
            break;
        case BUTTON_R_SHOULDER:
            isPressed = (buttons[mapping.shoulderRight] == InputAdapter::Pressed || buttons[mapping.shoulderRight] == InputAdapter::JustPressed);
            break;
        case BUTTON_L_THUMB:
            isPressed = (buttons[m_joyButton_ThumbLeft] == InputAdapter::Pressed || buttons[m_joyButton_ThumbLeft] == InputAdapter::JustPressed);
            break;
        case BUTTON_R_THUMB:
            isPressed = (buttons[m_joyButton_ThumbRight] == InputAdapter::Pressed || buttons[m_joyButton_ThumbRight] == InputAdapter::JustPressed);
            break;
        case BUTTON_START:
            isPressed = (buttons[mapping.startButton] == InputAdapter::Pressed || buttons[mapping.startButton] == InputAdapter::JustPressed);
            break;
        case BUTTON_SELECT:
            isPressed = (buttons[mapping.selectButton] == InputAdapter::Pressed || buttons[mapping.selectButton] == InputAdapter::JustPressed);
            break;
        default:
            break;
        }

        return isPressed ? 1.0f : 0.0f;
    }

    void ZPad_Base::ApplyDirectionRemapping(SDeviceInfo& deviceInfo, const f32* axes, const InputAdapter::PressStatus* buttons)
    {
        if (m_controlRemap.empty()) return;
        struct DirectionAxisMapping
        {
            u32 directionControl;   
            u32 axisControl;        
            bbool isPositive;     
        };

        static const DirectionAxisMapping kDirectionMappings[] = {
            { STICK_L_UP,    STICK_LY, btrue },
            { STICK_L_DOWN,  STICK_LY, bfalse },
            { STICK_L_LEFT,  STICK_LX, bfalse },
            { STICK_L_RIGHT, STICK_LX, btrue },
            { STICK_R_UP,    STICK_RY, btrue },
            { STICK_R_DOWN,  STICK_RY, bfalse },
            { STICK_R_LEFT,  STICK_RX, bfalse },
            { STICK_R_RIGHT, STICK_RX, btrue },
        };
        bbool lxHasRemap = (m_controlRemap[STICK_L_LEFT] != STICK_L_LEFT) || (m_controlRemap[STICK_L_RIGHT] != STICK_L_RIGHT);
        bbool lyHasRemap = (m_controlRemap[STICK_L_UP] != STICK_L_UP) || (m_controlRemap[STICK_L_DOWN] != STICK_L_DOWN);
        bbool rxHasRemap = (m_controlRemap[STICK_R_LEFT] != STICK_R_LEFT) || (m_controlRemap[STICK_R_RIGHT] != STICK_R_RIGHT);
        bbool ryHasRemap = (m_controlRemap[STICK_R_UP] != STICK_R_UP) || (m_controlRemap[STICK_R_DOWN] != STICK_R_DOWN);
        if (!lxHasRemap && !lyHasRemap && !rxHasRemap && !ryHasRemap) return;
        f32 remappedAxes[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; 

        for (const auto& mapping : kDirectionMappings)
        {
            u32 dirControl = mapping.directionControl;
            if (dirControl >= m_controlRemap.size()) continue;

            u32 axisIndex = mapping.axisControl;
            if (axisIndex == STICK_LX && !lxHasRemap) continue;
            if (axisIndex == STICK_LY && !lyHasRemap) continue;
            if (axisIndex == STICK_RX && !rxHasRemap) continue;
            if (axisIndex == STICK_RY && !ryHasRemap) continue;

            u32 physicalControl = m_controlRemap[dirControl];
            f32 axisValue = GetAxisValueFromControl(physicalControl, axes, buttons);
            if (mapping.isPositive)
            {
                remappedAxes[axisIndex] += axisValue;
            }
            else
            {
                remappedAxes[axisIndex] -= axisValue;
            }
        }
        if (lxHasRemap)
        {
            deviceInfo.m_inputInfo[STICK_LX].m_axisInfo.m_axis = f32_Clamp(remappedAxes[STICK_LX], -1.0f, 1.0f);
        }
        if (lyHasRemap)
        {
            deviceInfo.m_inputInfo[STICK_LY].m_axisInfo.m_axis = f32_Clamp(remappedAxes[STICK_LY], -1.0f, 1.0f);
        }
        if (rxHasRemap)
        {
            deviceInfo.m_inputInfo[STICK_RX].m_axisInfo.m_axis = f32_Clamp(remappedAxes[STICK_RX], -1.0f, 1.0f);
        }
        if (ryHasRemap)
        {
            deviceInfo.m_inputInfo[STICK_RY].m_axisInfo.m_axis = f32_Clamp(remappedAxes[STICK_RY], -1.0f, 1.0f);
        }
    }
} // namespace ITF
