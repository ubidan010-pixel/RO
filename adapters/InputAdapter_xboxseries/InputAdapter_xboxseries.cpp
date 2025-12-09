#include "precompiled_inputadapter_xboxseries.h"
#include "core/error/ErrorHandler.h"

#include "adapters/InputAdapter_xboxseries/InputAdapter_xboxseries.h"
#include "core/utility.h"

namespace ITF
{
    static ComPtr<IGameInput> createGameInput()
    {
        ComPtr<IGameInput> gameInput{};
        ITF_VERIFY_HR_CALL(::GameInputCreate(gameInput.ReleaseAndGetAddressOf()));
        return gameInput;
    }

    InputAdapter_XBoxSeries::InputAdapter_XBoxSeries()
        : m_gameInput(createGameInput())
        , m_padsHandler(m_gameInput)
    {
    }

    InputAdapter_XBoxSeries::~InputAdapter_XBoxSeries() = default;

    void InputAdapter_XBoxSeries::updateAllInputState()
    {
        m_padsHandler.update();

        for (u32 padIdx = 0; padIdx < NbMaxPads; ++padIdx)
        {
            bbool wasConnected = isPadConnected(padIdx);
            bbool isConnected = m_padsHandler.isPadConnected(padIdx);
            
            if (wasConnected != isConnected)
            {
                if (isConnected)
                {
                    OnControllerConnected(padIdx);
                }
                else
                {
                    OnControllerDisconnected(padIdx);
                }
            }
            
            setPadConnected(padIdx, isConnected);
        }
    }

    u32 InputAdapter_XBoxSeries::getGamePadCount()
    {
        return m_padsHandler.getGamePadCount();
    }

    void InputAdapter_XBoxSeries::getGamePadPos(u32 _environment, u32 _padIdx, float * _pos, u32 _nbAxis) const
    {
        if ((m_environmentInput & _environment) != 0 && _padIdx < m_padsHandler.getMaxPadCount())
        {
            m_padsHandler.getGamePadPos(_padIdx, _pos, _nbAxis);
        }
        else
        {
            std::fill(_pos, _pos + _nbAxis, 0.0f);
        }
    }

    void InputAdapter_XBoxSeries::getGamePadButtons(u32 _environment, u32 _padIdx, PressStatus* _buttons, u32 _nbButtons) const
    {
        if ((m_environmentInput & _environment) != 0 && _padIdx < m_padsHandler.getMaxPadCount())
        {
            m_padsHandler.getGamePadButtons(_padIdx, _buttons, _nbButtons);
        }
        else
        {
            std::fill(_buttons, _buttons + _nbButtons, PressStatus::Released);
        }
    }

    void InputAdapter_XBoxSeries::startRumble(u32 _padIdx, f64 _duration, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
    {
        m_padsHandler.startRumble(_padIdx, _duration, _leftMotorSpeed, _rightMotorSpeed);
    }

    void InputAdapter_XBoxSeries::stopRumble(u32 _padIdx)
    {
        ITF_ASSERT(_padIdx < m_padsHandler.getGamePadCount());
        m_padsHandler.stopRumble(_padIdx);
    }

    void InputAdapter_XBoxSeries::swapControllers(u32 index1, u32 index2)
    {
        LOG("[InputAdapter_XBoxSeries] swapControllers(%u, %u)", index1, index2);
        m_padsHandler.swapPads(index1, index2);
    }
} // namespace ITF
