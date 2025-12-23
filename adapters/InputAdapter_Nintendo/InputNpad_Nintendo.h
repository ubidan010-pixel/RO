#ifndef ITF_INPUTNPAD_NINTENDO_H_
#define ITF_INPUTNPAD_NINTENDO_H_

#pragma once

#include "adapters/InputAdapter_Nintendo/InputAdapter_Nintendo.h"

#include "adapters/InputAdapter_Nintendo/InputPad_Nintendo.h"

#include <nn/hid.h>
#include <nn/hid/hid_Npad.h>
#include <nn/hid/hid_Vibration.h>

namespace ITF
{

    class InputNpad_Nintendo : public InputPad_Nintendo
    {
    public:
        enum ESide
        {
            ESide_None = -1,  // Not connected / not available

            ESide_Left = 0,
            ESide_Right,
            ESide_Dual,
            ESide_FullKey,
            ESide_Handheld,

            ESide_Count
        };

        enum
        {
            VibrationHandlesMax = 2
        };

        InputNpad_Nintendo(u32 _id, nn::hid::NpadIdType _npadId);
        ~InputNpad_Nintendo() override;

        f64 getLastUsedTime() { return m_lastUsedTime; }

        InputAdapter::PadType GetType() const override
        {
            switch (m_currentSide)
            {
                case ESide::ESide_Left:
                case ESide::ESide_Right:
                    return InputAdapter::Pad_NX_Joycon;

                case ESide::ESide_Dual:
                    return InputAdapter::Pad_NX_Joycon_Dual;

                case ESide::ESide_FullKey:
                    return InputAdapter::Pad_NX_Pro;

                case ESide::ESide_Handheld:
                    return InputAdapter::Pad_NX_Joycon_Dual; // we assume handheld controls are identical to Dual. Use IsHandheld() to distinguish
                default:
                    return InputAdapter::Pad_Invalid;
            }

            return InputAdapter::Pad_Invalid;
        }

        virtual bool IsHandheld() const override
        {
            return m_currentSide == ESide_Handheld;
        }

        const char* GetTypeName() const override;

        void getGamePadPos(u32 _environment, float* _pos, u32 _numAxes) const override;
        InputAdapter::PressStatus getGamePadButton(u32 _environment, InputAdapter::ButtonMode _buttonMode, u32 _buttonID) const override;

        void Update() override;

        bool isVibrating() const { return (m_vibrationEndTime > 0.0); }
        void setVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time) override;

        ESide getPadSide() { return m_currentSide; }
        u32   getDeviceId() {return m_npadId;}
    protected:

        nn::hid::NpadIdType m_npadId{};
        nn::hid::NpadAttributesSet m_prevAttributes{};
        bbool m_first = bfalse;
        ESide m_currentSide = ESide_None;

        nn::hid::VibrationDeviceHandle m_vibrationHandles[ESide_Count][VibrationHandlesMax]{};

        f64 m_vibrationEndTime = 0.;
        f64 m_lastUsedTime = 0.;

        int64_t m_mostRecentlyReadSamplingNumber = 0;

        void UpdateButtonsAndAxis(ESide _oldSide);
        void UpdateVibration(ESide _oldSide);

        void UpdateNpad(const nn::hid::NpadButtonSet& _buttonSet, const nn::hid::AnalogStickState& _analogStickL, const nn::hid::AnalogStickState& _analogStickR);

        void UpdateNpadButtons(const nn::hid::NpadButtonSet& _buttonSet);

        template <typename ButtonAttribute>
        void UpdateNpadButton(const nn::hid::NpadButtonSet& _buttonSet, u32 _buttonId)
        {
            UpdateButton<nn::hid::NpadButtonSet, ButtonAttribute>(_buttonSet, _buttonId);
        }

        void UpdateNpadSticks(const nn::hid::AnalogStickState& _analogStickL, const nn::hid::AnalogStickState& _analogStickR);
        void UpdateHandheld();
        void UpdateWireless();
        void UpdateLastUsedTime();
    };

}  // namespace ITF

#endif  // ITF_INPUTNPAD_NINTENDO_H_
