#ifndef ITF_INPUTPAD_NINTENDO_H_
#define ITF_INPUTPAD_NINTENDO_H_

#pragma once

#include "core/types.h"

#include "engine/AdaptersInterfaces/InputAdapter.h"

namespace ITF
{

    class InputPad_Nintendo
    {
    public:
        explicit InputPad_Nintendo(u32 _id);
        virtual ~InputPad_Nintendo();

        u32 GetID() const { return m_id; }
        bool isConnected() const { return m_connected; };

        virtual InputAdapter::PadType GetType() const = 0;
        virtual const char* GetTypeName() const = 0;

        virtual bool IsHandheld() const = 0;

        virtual void getGamePadPos(u32 _environment, float* _pos, u32 _numAxes) const;
        virtual InputAdapter::PressStatus getGamePadButton(u32 _environment, InputAdapter::ButtonMode _buttonMode, u32 _buttonID) const;

        virtual void Update() = 0;

        virtual void setVibration(f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time) = 0;
        void stopVibration() { setVibration(0.0f, 0.0f, 0.0); }


    protected:
        u32 m_id = 0;

        InputAdapter::PressStatus m_buttonStatus[m_joyButton_NxCount]{};
        f32 m_axesPos[JOY_MAX_AXES]{};


        void setConnected(bool _connected);

        void UpdateButton(u32 _buttonId, bool _isPressed);

        template<typename ButtonSet, typename ButtonAttribute>
        void UpdateButton(const ButtonSet& _buttonSet, u32 _buttonId)
        {
            bool buttonPressed = _buttonSet.template Test<ButtonAttribute>();
            UpdateButton(_buttonId, buttonPressed);
        }

    private:
        bool m_connected = false;
    };

} // namespace ITF


#endif // ITF_INPUTPAD_NINTENDO_H_
