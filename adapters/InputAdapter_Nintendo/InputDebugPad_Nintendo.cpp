#include "precompiled_InputAdapter_Nintendo.h"

#include "adapters/InputAdapter_Nintendo/InputDebugPad_Nintendo.h"

namespace ITF
{

    InputDebugPad_Nintendo::InputDebugPad_Nintendo(u32 _id)
        : InputPad_Nintendo(_id)
    {
    }


    InputDebugPad_Nintendo::~InputDebugPad_Nintendo() = default;


    bool InputDebugPad_Nintendo::IsAvailable()
    {
        nn::hid::DebugPadState padState = {};
        nn::hid::GetDebugPadState(&padState);
        return padState.attributes.Test<nn::hid::DebugPadAttribute::IsConnected>();
    }


    void InputDebugPad_Nintendo::Update()
    {
        nn::hid::DebugPadState padState = {};
        nn::hid::GetDebugPadState(&padState);

        // Update "connected" status
        setConnected(padState.attributes.Test<nn::hid::DebugPadAttribute::IsConnected>());

        // Update button status
        UpdateDebugButton<nn::hid::DebugPadButton::A     >(padState, m_joyButton_A           );
        UpdateDebugButton<nn::hid::DebugPadButton::B     >(padState, m_joyButton_B           );
        UpdateDebugButton<nn::hid::DebugPadButton::X     >(padState, m_joyButton_X           );
        UpdateDebugButton<nn::hid::DebugPadButton::Y     >(padState, m_joyButton_Y           );
        UpdateDebugButton<nn::hid::DebugPadButton::L     >(padState, m_joyButton_LB          );
        UpdateDebugButton<nn::hid::DebugPadButton::R     >(padState, m_joyButton_RB          );
        UpdateDebugButton<nn::hid::DebugPadButton::ZL    >(padState, m_joyButton_TriggerLeft );
        UpdateDebugButton<nn::hid::DebugPadButton::ZR    >(padState, m_joyButton_TriggerRight);
        UpdateDebugButton<nn::hid::DebugPadButton::Start >(padState, m_joyButton_Start       );
        UpdateDebugButton<nn::hid::DebugPadButton::Select>(padState, m_joyButton_Back        );
        UpdateDebugButton<nn::hid::DebugPadButton::Left  >(padState, m_joyButton_DPadL       );
        UpdateDebugButton<nn::hid::DebugPadButton::Up    >(padState, m_joyButton_DPadU       );
        UpdateDebugButton<nn::hid::DebugPadButton::Right >(padState, m_joyButton_DPadR       );
        UpdateDebugButton<nn::hid::DebugPadButton::Down  >(padState, m_joyButton_DPadD       );

        // Update stick status
        m_axesPos[m_joyStickLeft_X ] = static_cast<f32>(padState.analogStickL.x) / static_cast<f32>(nn::hid::AnalogStickMax);
        m_axesPos[m_joyStickLeft_Y ] = static_cast<f32>(padState.analogStickL.y) / static_cast<f32>(nn::hid::AnalogStickMax);
        m_axesPos[m_joyStickRight_X] = static_cast<f32>(padState.analogStickR.x) / static_cast<f32>(nn::hid::AnalogStickMax);
        m_axesPos[m_joyStickRight_Y] = static_cast<f32>(padState.analogStickR.y) / static_cast<f32>(nn::hid::AnalogStickMax);
    }

} // namespace ITF
