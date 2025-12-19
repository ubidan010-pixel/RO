#pragma once

#include "engine/AdaptersInterfaces/InputAdapter.h"

#include "adapters/InputAdapter_xboxseries/InputPad_xboxseries.h"

#include "core/container/extendedVector.h"
#include <wrl/client.h>

namespace ITF
{
    using Microsoft::WRL::ComPtr;

    class InputPadsHandler_GameInput final
    {
    public:
        constexpr static u32 MAX_PAD_COUNT = 4; // Only 4 pads supported by the game.

        explicit InputPadsHandler_GameInput(ComPtr<IGameInput> _gameInput);
        ~InputPadsHandler_GameInput();

        InputPadsHandler_GameInput(const InputPadsHandler_GameInput&) = delete;
        InputPadsHandler_GameInput& operator = (const InputPadsHandler_GameInput&) = delete;

        void update();

        u32 getGamePadCount() const;
        u32 getMaxPadCount() const;

        void getGamePadPos(u32 _pad, f32 * _pos, u32 _nbAxes) const;
        void getGamePadButtons(u32 _pad, InputAdapter::PressStatus* _buttons, u32 _nbButtons) const;

        void swapPads(u32 index1, u32 index2);

        void startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed);
        void stopRumble(u32 _numPad);

        u32 addPad(ComPtr<IGameInputDevice> _device);
        void removePad(ComPtr<IGameInputDevice> _device);

        InputAdapter::PadType getPadType(u32 _numPad) const;
        bbool isPadConnected(u32 _numPad) const;
        IGameInputDevice* getDevice(u32 _index) { return m_pads[_index].getDevice();}
        String InputPadsHandler_GameInput::getDeviceName(u32 _pad);
    private:
        void updateRemovePads();
        enum : u32 { INVALID_PAD_INDEX = U32_INVALID };
        u32 getIndex(IGameInputDevice* _device); // return INVALID_PAD_INDEX if no assigned with this not found

        static void CALLBACK onGameInputDevice(
            _In_ GameInputCallbackToken,
            _In_ void * context,
            _In_ IGameInputDevice * device,
            _In_ uint64_t,
            _In_ GameInputDeviceStatus currentStatus,
            _In_ GameInputDeviceStatus) noexcept;

        ComPtr<IGameInput> m_gameInput{};
        GameInputCallbackToken m_deviceToken = 0;
        Vector<InputPad_GameInput> m_pads{};
        mutable Mutex m_mutexOnRemoveList{};
        Vector<ComPtr<IGameInputDevice>> m_removeDevices{};
    };

} // namespace ITF

