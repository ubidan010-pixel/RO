#pragma once
#include "engine/AdaptersInterfaces/InputAdapter.h"
#include "adapters/InputAdapter_xboxseries/InputPadsHandler_xboxseries.h"
#include <memory>
#include <wrl/client.h>

namespace ITF
{
    using Microsoft::WRL::ComPtr;

    class InputAdapter_XBoxSeries : public InputAdapter
    {
    private:
        enum : u32 { NbMaxPads = 4 }; // XUSER_MAX_COUNT is now 8 but we limit to 4 for the game
        
    public:
        InputAdapter_XBoxSeries();
        InputAdapter_XBoxSeries(const InputAdapter_XBoxSeries &) = delete;
        ~InputAdapter_XBoxSeries();

        InputAdapter_XBoxSeries & operator = (const InputAdapter_XBoxSeries&) = delete;

        virtual u32 getGamePadCount() override;
        virtual void getGamePadPos(u32 _environment, u32 _padIdx, float* _pos, u32 _nbAxis) const override;
        virtual void getGamePadButtons(u32 _environment, u32 _padIdx, PressStatus* _buttons, u32 _nbButtons) const override;
        virtual void startRumble(u32 _padIdx, f64 _duration, f32 _leftMotorSpeed, f32 _rightMotorSpeed) override;
        virtual void stopRumble(u32 _padIdx) override;

        void updateAllInputState();

    private:
        ComPtr<IGameInput> m_gameInput{};
        InputPadsHandler_GameInput m_padsHandler;

#if ITF_SUPPORT_IMGUI
    public:
        IGameInput* getGameInput() { return m_gameInput.Get(); }
#endif
    };

    #define INPUT_ADAPTER_XBOXSERIES (static_cast<InputAdapter_XBoxSeries*>(INPUT_ADAPTER))

} // namespace ITF
