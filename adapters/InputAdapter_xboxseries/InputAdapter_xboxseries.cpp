#include "precompiled_inputadapter_xboxseries.h"

#include "adapters/InputAdapter_xboxseries/InputAdapter_xboxseries.h"
#include "core/utility.h"

namespace ITF
{
    namespace
    {
        static IGameInput* createGameInput()
        {
            IGameInput* gameInput = nullptr;
            ITF_VERIFY_HR_CALL(::GameInputCreate(&gameInput));
            return gameInput;
        }
    }

    InputAdapter_XBoxSeries::InputAdapter_XBoxSeries()
        : m_gameInput(*createGameInput())
        , m_padsHandler(m_gameInput)
    {
    }

    InputAdapter_XBoxSeries::~InputAdapter_XBoxSeries()
    {
        m_gameInput.Release();
    }

    void InputAdapter_XBoxSeries::updateAllInputState()
    {
        m_padsHandler.update();
    }


    u32 InputAdapter_XBoxSeries::getGamePadCount()
    {
        return m_padsHandler.getGamePadCount();
    }

    void InputAdapter_XBoxSeries::getGamePadPos(u32 _environment, u32 _padIdx, float * _pos, u32 _nbAxis) const
    {
        ITF_ASSERT(_padIdx < m_padsHandler.getGamePadCount());
        if ((m_environmentInput & _environment) != 0)
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
        ITF_ASSERT(_padIdx < m_padsHandler.getGamePadCount());

        if ((m_environmentInput & _environment) != 0)
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
        ITF_ASSERT(_padIdx < m_padsHandler.getGamePadCount());
        m_padsHandler.startRumble(_padIdx, _duration, _leftMotorSpeed, _rightMotorSpeed);
    }

    void InputAdapter_XBoxSeries::stopRumble(u32 _padIdx)
    {
        ITF_ASSERT(_padIdx < m_padsHandler.getGamePadCount());
        m_padsHandler.stopRumble(_padIdx);
    }
} // namespace ITF
