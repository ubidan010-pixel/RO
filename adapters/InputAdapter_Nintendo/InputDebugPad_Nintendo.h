#ifndef ITF_INPUTDEBUGPAD_NINTENDO_H_
#define ITF_INPUTDEBUGPAD_NINTENDO_H_

#pragma once

#include "adapters/InputAdapter_Nintendo/InputPad_Nintendo.h"

#include <nn/hid.h>


namespace ITF
{

    class InputDebugPad_Nintendo: public InputPad_Nintendo
    {
    public:
        InputDebugPad_Nintendo(u32 _id);
        ~InputDebugPad_Nintendo() override;

        static bool IsAvailable();

        InputAdapter::PadType GetType() const override { return InputAdapter::Pad_NX_Joycon_Dual;  }
        const char* GetTypeName() const override { return "DebugPad"; }

        virtual bool IsHandheld() const override { return false; }

        void Update() override;

        void setVibration(f32 /*_leftMotorSpeed*/, f32 /*_rightMotorSpeed*/, f64 /*_time*/) override { /* No rumble for the debug pad */ }

    protected:
        template<typename ButtonAttribute>
        void UpdateDebugButton(const nn::hid::DebugPadState& _padState, u32 _buttonId)
        {
            UpdateButton<nn::hid::DebugPadButtonSet, ButtonAttribute>(_padState.buttons, _buttonId);
        }
    };

} // namespace ITF


#endif // ITF_INPUTDEBUGPAD_NINTENDO_H_
