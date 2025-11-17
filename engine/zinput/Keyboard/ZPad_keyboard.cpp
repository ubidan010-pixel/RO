#include "precompiled_engine.h"

#ifdef ITF_WINDOWS

#include <windows.h>

#include <algorithm>

#ifndef _ITF_ZPAD_KEYBOARD_H_
#include "engine/zinput/Keyboard/ZPad_keyboard.h"
#endif //_ITF_ZPAD_KEYBOARD_H_

namespace ITF
{
    namespace
    {
        inline bool IsKeyActive(InputAdapter::PressStatus status)
        {
            return status == InputAdapter::Pressed || status == InputAdapter::JustPressed;
        }
    }

    ZPad_keyboard::ZPad_keyboard(u32 id)
        : IInputDevice(id)
    {
    }

    bbool ZPad_keyboard::IsDeviceValid() const
    {
        return btrue;
    }

    void ZPad_keyboard::InitDeviceInfo()
    {
        ITF_VECTOR<SInputInfo>& info = m_deviceInfo.m_inputInfo;
        info.resize(CONTROL_MAX);

        info[PAD_STICK_LX].m_type = SInputInfo::INPUTTYPE_AXIS;
        info[PAD_STICK_LY].m_type = SInputInfo::INPUTTYPE_AXIS;
        info[PAD_TRIGGER_RIGHT].m_type = SInputInfo::INPUTTYPE_AXIS;

        info[PAD_DPAD_UP].m_type = SInputInfo::INPUTTYPE_BUTTON;
        info[PAD_DPAD_DOWN].m_type = SInputInfo::INPUTTYPE_BUTTON;
        info[PAD_DPAD_LEFT].m_type = SInputInfo::INPUTTYPE_BUTTON;
        info[PAD_DPAD_RIGHT].m_type = SInputInfo::INPUTTYPE_BUTTON;

        info[PAD_BUTTON_A].m_type = SInputInfo::INPUTTYPE_BUTTON;
        info[PAD_BUTTON_B].m_type = SInputInfo::INPUTTYPE_BUTTON;
        info[PAD_BUTTON_X].m_type = SInputInfo::INPUTTYPE_BUTTON;
        info[PAD_BUTTON_START].m_type = SInputInfo::INPUTTYPE_BUTTON;

        REGISTER_CONTROL(PAD_STICK_LX, "STICK_LX");
        REGISTER_CONTROL(PAD_STICK_LY, "STICK_LY");
        REGISTER_CONTROL(PAD_TRIGGER_RIGHT, "TRIGGER_RIGHT");

        REGISTER_CONTROL(PAD_DPAD_UP, "DPAD_UP");
        REGISTER_CONTROL(PAD_DPAD_DOWN, "DPAD_DOWN");
        REGISTER_CONTROL(PAD_DPAD_LEFT, "DPAD_LEFT");
        REGISTER_CONTROL(PAD_DPAD_RIGHT, "DPAD_RIGHT");

        REGISTER_CONTROL(PAD_BUTTON_A, "BUTTON_A");
        REGISTER_CONTROL(PAD_BUTTON_B, "BUTTON_B");
        REGISTER_CONTROL(PAD_BUTTON_X, "BUTTON_X");
        REGISTER_CONTROL(PAD_BUTTON_START, "BUTTON_START");
    }

    SInputInfo::EButtonStatus ZPad_keyboard::ConvertKeyStatus(InputAdapter::PressStatus status)
    {
        return IsKeyActive(status) ? SInputInfo::BUTTONSTATUS_PRESS : SInputInfo::BUTTONSTATUS_NONE;
    }

    float ZPad_keyboard::ComputeAxis(InputAdapter::PressStatus negative, InputAdapter::PressStatus positive)
    {
        float value = 0.0f;
        if (IsKeyActive(negative))
            value -= 1.0f;
        if (IsKeyActive(positive))
            value += 1.0f;
        value = std::min(value, 1.0f);
        value = std::max(value, -1.0f);
        return value;
    }

    void ZPad_keyboard::UpdateDeviceInfo(SDeviceInfo& deviceInfo)
    {
        const auto keyState = [](int vk) -> InputAdapter::PressStatus
        {
            return INPUT_ADAPTER->GetKeyState(static_cast<u32>(vk));
        };

        const InputAdapter::PressStatus stickLeft = keyState('A');
        const InputAdapter::PressStatus stickRight = keyState('D');
        const InputAdapter::PressStatus stickUp = keyState('W');
        const InputAdapter::PressStatus stickDown = keyState('S');

        const InputAdapter::PressStatus dpadLeft = keyState(VK_LEFT);
        const InputAdapter::PressStatus dpadRight = keyState(VK_RIGHT);
        const InputAdapter::PressStatus dpadUp = keyState(VK_UP);
        const InputAdapter::PressStatus dpadDown = keyState(VK_DOWN);
        const InputAdapter::PressStatus trigger = keyState(VK_LSHIFT);

        deviceInfo.m_inputInfo[PAD_STICK_LX].m_axisInfo.m_axis = ComputeAxis(stickLeft, stickRight);
        deviceInfo.m_inputInfo[PAD_STICK_LY].m_axisInfo.m_axis = ComputeAxis(stickDown, stickUp);
        deviceInfo.m_inputInfo[PAD_TRIGGER_RIGHT].m_axisInfo.m_axis = IsKeyActive(trigger) ? 1.0f : 0.0f;

        deviceInfo.m_inputInfo[PAD_DPAD_LEFT].m_buttonInfo.m_status = ConvertKeyStatus(dpadLeft);
        deviceInfo.m_inputInfo[PAD_DPAD_RIGHT].m_buttonInfo.m_status = ConvertKeyStatus(dpadRight);
        deviceInfo.m_inputInfo[PAD_DPAD_UP].m_buttonInfo.m_status = ConvertKeyStatus(dpadUp);
        deviceInfo.m_inputInfo[PAD_DPAD_DOWN].m_buttonInfo.m_status = ConvertKeyStatus(dpadDown);

        deviceInfo.m_inputInfo[PAD_BUTTON_A].m_buttonInfo.m_status = ConvertKeyStatus(keyState(VK_SPACE));
        deviceInfo.m_inputInfo[PAD_BUTTON_X].m_buttonInfo.m_status = ConvertKeyStatus(keyState('F'));
        deviceInfo.m_inputInfo[PAD_BUTTON_B].m_buttonInfo.m_status = ConvertKeyStatus(keyState('B'));
        deviceInfo.m_inputInfo[PAD_BUTTON_START].m_buttonInfo.m_status = ConvertKeyStatus(keyState(VK_ESCAPE));
    }

    IInputDevice* CreateKeyboardDevice(u32 padIndex)
    {
        return newAlloc(mId_System, ZPad_keyboard(padIndex));
    }
} // namespace ITF

#endif // ITF_WINDOWS
