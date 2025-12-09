#ifndef ITF_INPUTADAPTER_NINTENDO_H_
#define ITF_INPUTADAPTER_NINTENDO_H_

#pragma once

#include "engine/AdaptersInterfaces/InputAdapter.h"

#include <nn/hid/hid_Npad.h>
#include <nn/hid/hid_NpadSixAxisSensor.h>
#include <nn/nn_Log.h>

namespace ITF
{
    class InputPad_Nintendo;

    #ifdef INPUT_USE_VKEYBOARD
        class Task_VKeyboard_Nintendo;
    #endif

    class InputAdapter_Nintendo : public InputAdapter
    {
    private:
        bool m_init = false;
        Vec3d m_gyroResetAnglesHandheld{};
        Vec3d m_gyroResetAnglesJoyLeft{};
        Vec3d m_gyroResetAnglesJoyRight{};
        bool m_showControllerAppletRequested = false;
        bool m_enableControllerApplet = false;
        void showControllerApplet();
        InputPad_Nintendo* m_activePad = nullptr;

    public:
        enum : u32
        {
            MAX_PAD_COUNT = JOY_MAX_COUNT,
            MaxStations = 2,
            MaxPadsPerStation = MAX_PAD_COUNT,
            MAX_ONLINE_PAD_COUNT = MaxStations * MaxPadsPerStation,
            MAX_VIRTUAL_JOYCON_COUNT = MAX_ONLINE_PAD_COUNT
        };

        InputAdapter_Nintendo();
        ~InputAdapter_Nintendo() override;

        u32 getGamePadCount() override;

        void getGamePadPos(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const override;
        void getGamePadButtons(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const override;
        void startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed) override;
        void stopRumble(u32 _numPad) override;
        void swapControllers(u32 index1, u32 index2) override;

        // not virtual methods

        void initialize();

        bbool isHandheld();

        void requestShowControllerApplet();
        void disableControllerAppletThisFrame() { m_enableControllerApplet = false; }

        void updateAllInputState() override;
    #ifdef INPUT_USE_VKEYBOARD
        void VKeyboardOpen(const VKeyboardOpenParams& _params) override;
        // ITF_PASTA override removed
        // void            VKeyboardUpdate             () override;
        void VKeyboardUpdate();
    #endif

    private:

        // Array of NPads. The pad 'handheld' is the first one and never null after init. Thus it can be in disconnected state.
        // All non handheld pads follow (joycons, pro controller).
        // So the max size of this vector is MAX_PAD_COUNT + 1:
        // nn::hid::NpadId::Handheld, nn::hid::NpadId::No1, nn::hid::NpadId::No2, nn::hid::NpadId::No3, nn::hid::NpadId::No4.
        Vector<InputPad_Nintendo*> m_pads;

    #ifdef INPUT_USE_VKEYBOARD
        RefCountingPointer<Task_VKeyboard_NX> m_VKeyboardTask;
    #endif

        InputPad_Nintendo* getPad(u32 _padIndex) const;

        void getGamePadPos(u32 _environment, InputPad_Nintendo* _padPtr, float* _pos, u32 _numAxes) const;
        void getGamePadButtons(u32 _environment, InputPad_Nintendo* _padPtr, PressStatus* _buttons, u32 _numButtons) const;

        void updateControllers();
    };

    #define INPUT_ADAPTER_NINTENDO (static_cast<InputAdapter_Nintendo*>(INPUT_ADAPTER))

}  // namespace ITF

#endif  // ITF_INPUTADAPTER_NINTENDO_H_
