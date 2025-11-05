#include "precompiled_InputAdapter_xboxseries.h"

#include "adapters/InputAdapter_xboxseries/InputPadsHandler_xboxseries.h"
#include "core/utility.h"
#include <algorithm>

namespace ITF
{
    void CALLBACK InputPadsHandler_GameInput::onGameInputDevice(
        _In_ GameInputCallbackToken,
        _In_ void * context,
        _In_ IGameInputDevice * device,
        _In_ uint64_t,
        _In_ GameInputDeviceStatus currentStatus,
        _In_ GameInputDeviceStatus previousStatus) noexcept
    {
        InputPadsHandler_GameInput* m_padsHandler = reinterpret_cast<InputPadsHandler_GameInput*>(context);

        GameInputDeviceStatus currentConnectedStatus = currentStatus & GameInputDeviceConnected;
        GameInputDeviceStatus previousConnectedStatus = previousStatus & GameInputDeviceConnected;

        if (previousConnectedStatus > currentConnectedStatus)
        {
            ScopeLockMutex locker(m_padsHandler->m_mutexOnRemoveList);  
            m_padsHandler->m_removeDevices.emplace_back(device);
        }
    }

    InputPadsHandler_GameInput::InputPadsHandler_GameInput(ComPtr<IGameInput> _gameInput)
        : m_gameInput(std::move(_gameInput))
    {
        ITF_VERIFY_HR_CALL(m_gameInput->RegisterDeviceCallback(
            nullptr,
            GameInputKindGamepad,
            GameInputDeviceConnected,
            GameInputBlockingEnumeration,
            this,
            onGameInputDevice,
            &m_deviceToken));
    }

    InputPadsHandler_GameInput::~InputPadsHandler_GameInput()
    {
        ITF_ASSERT(m_deviceToken != 0);

        if (m_deviceToken!=0)
        {
            ITF_VERIFY(m_gameInput->UnregisterCallback(m_deviceToken, UINT64_MAX));
        }
        m_pads.clear();
        m_deviceToken = 0;
    }

    void InputPadsHandler_GameInput::updateRemovePads()
    {
        // Treat the pads notified as removed in onGameInputDevice
        ScopeLockMutex locker(m_mutexOnRemoveList);
        while (!m_removeDevices.empty())
        {
            removePad(std::move(m_removeDevices.back()));
            m_removeDevices.pop_back();
        }
    }

    void InputPadsHandler_GameInput::update()
    {
        updateRemovePads();

        ComPtr<IGameInputReading> reading{};

        HRESULT currentReadingRes = m_gameInput->GetCurrentReading(GameInputKindGamepad, nullptr, reading.ReleaseAndGetAddressOf());

        if (SUCCEEDED(currentReadingRes))
        {
            ComPtr<IGameInputDevice> device{};
            reading->GetDevice(device.ReleaseAndGetAddressOf());

            if (device != nullptr)
            {
                i32 deviceIndex = getIndex(device.Get());
                if (deviceIndex == -1)
                {
                    deviceIndex = addPad(std::move(device));
                }

                GameInputGamepadState state{};
                reading->GetGamepadState(&state);

                m_pads[deviceIndex].update(state);
            }
        }
    }

    u32 InputPadsHandler_GameInput::getIndex(IGameInputDevice * _device)
    {
        for (const InputPad_GameInput & pad : m_pads)
        {
            if (pad.getDevice() == _device)
            {
                return u32(distInRange(m_pads, pad));
            }
        }
        return U32_INVALID;
    }

    u32 InputPadsHandler_GameInput::addPad(ComPtr<IGameInputDevice> _device)
    {
        ITF_ASSERT_CRASH(getIndex(_device.Get()) == INVALID_PAD_INDEX, "Trying to add a pad already in the pad handler");
        m_pads.emplace_back(std::move(_device));
        return m_pads.size() - 1u;
    }

    void InputPadsHandler_GameInput::removePad(ComPtr<IGameInputDevice> _device)
    {
        auto removeIt = std::find_if(m_pads.begin(), m_pads.end(), [&](const InputPad_GameInput& pad) { return pad.getDevice() == _device.Get(); });

        if (removeIt != m_pads.end())
        {
            m_pads.erase(removeIt);
        }
    }

    u32 InputPadsHandler_GameInput::getMaxPadCount() const
    {
        return MAX_PAD_COUNT;
    }
    
    u32 InputPadsHandler_GameInput::getGamePadCount() const
    {
        return m_pads.size();
    }

    void InputPadsHandler_GameInput::getGamePadPos(u32 _pad, f32 * _pos, u32 _nbAxes) const
    {
        if (_pad < m_pads.size())
        {
            m_pads[_pad].getGamePadPos(_pos, _nbAxes);
        }
        else
        {
            std::fill(_pos, _pos + _nbAxes, 0.0f);
        }
    }

    void InputPadsHandler_GameInput::getGamePadButtons(u32 _pad, InputAdapter::PressStatus* _buttons, u32 _nbButtons) const
    {
        if (_pad < m_pads.size())
        {
            m_pads[_pad].getGamePadButtons(_buttons, _nbButtons);
        }
        else
        {
            std::fill(_buttons, _buttons + _nbButtons, InputAdapter::PressStatus::Released);
        }
    }

    void InputPadsHandler_GameInput::startRumble(u32 _pad, f64 _duration, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        if (_pad < m_pads.size())
        {
            m_pads[_pad].setVibration(_leftMotorSpeed, _rightMotorSpeed, _duration);
        }
    }

    void InputPadsHandler_GameInput::stopRumble(u32 _pad)
    {
        if (_pad < m_pads.size())
        {
            m_pads[_pad].stopVibration();
        }
    }

    InputAdapter::PadType InputPadsHandler_GameInput::getPadType(u32 _pad) const
    {
        if (_pad < m_pads.size())
        {
            return m_pads[_pad].getType();
        }
        return InputAdapter::PadType::Pad_Invalid;
    }

    bbool InputPadsHandler_GameInput::isPadConnected(u32 _pad) const
    {
        return _pad < m_pads.size();
    }

} // namespace ITF
