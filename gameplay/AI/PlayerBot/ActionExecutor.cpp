#include "precompiled_gameplay.h"
#ifdef ITF_SUPPORT_BOT_AUTO
#include "ActionExecutor.h"

#include <algorithm>

namespace ITF
{
    ActionExecutor::ActionExecutor(InputAdapter* inputAdapter)
        : m_inputAdapter(inputAdapter)
          , m_inputQueue()
          , m_minButtonHoldTime(0.05f)
          , m_minButtonReleaseTime(0.05f)
    {
        for (u32 i = 0; i < MAX_BUTTONS; ++i)
        {
            m_buttonStates[i] = false;
            m_buttonHoldTimers[i] = 0.0f;
        }

        for (u32 i = 0; i < MAX_AXES; ++i)
        {
            m_axisStates[i] = 0.0f;
        }
    }

    ActionExecutor::~ActionExecutor()
    {
        clearInputQueue();
    }

    void ActionExecutor::executeAction(const BotAction& action, f32 deltaTime)
    {
        mapActionToInputs(action);
    }

    void ActionExecutor::pressButton(u32 buttonIndex)
    {
        if (buttonIndex >= MAX_BUTTONS || !m_inputAdapter)
            return;
        if (!m_buttonStates[buttonIndex])
        {
            m_inputAdapter->virtualPressButton(buttonIndex);
            m_buttonStates[buttonIndex] = true;
            m_buttonHoldTimers[buttonIndex] = 0.0f;
        }
    }

    void ActionExecutor::releaseButton(u32 buttonIndex)
    {
        if (buttonIndex >= MAX_BUTTONS || !m_inputAdapter)
            return;
        if (m_buttonStates[buttonIndex] && m_buttonHoldTimers[buttonIndex] >= m_minButtonHoldTime)
        {
            m_inputAdapter->virtualReleaseButton(buttonIndex);
            m_buttonStates[buttonIndex] = false;
            m_buttonHoldTimers[buttonIndex] = 0.0f;
        }
    }

    void ActionExecutor::setAxisValue(u32 axisIndex, f32 value)
    {
        if (axisIndex >= MAX_AXES || !m_inputAdapter)
            return;

        f32 clampedValue = value;
        clampedValue = std::max(clampedValue, -1.0f);
        clampedValue = std::min(clampedValue, 1.0f);

        m_inputAdapter->virtualSetAxis(axisIndex, clampedValue);
        m_axisStates[axisIndex] = clampedValue;
    }

    void ActionExecutor::queueInputSequence(const InputSequence& sequence)
    {
        for (u32 i = 0; i < sequence.events.size(); ++i)
        {
            InputEvent event = sequence.events[i];
            event.timestamp = 0.0f;
            m_inputQueue.push_back(event);
        }
    }

    void ActionExecutor::update(f32 deltaTime)
    {
        updateButtonTimers(deltaTime);
        processInputQueue(deltaTime);
    }

    void ActionExecutor::mapActionToInputs(const BotAction& action)
    {
    }

    void ActionExecutor::processInputQueue(f32 deltaTime)
    {
        if (m_inputQueue.size() == 0)
            return;

        SafeArray<InputEvent> remainingEvents;
        for (u32 i = 0; i < m_inputQueue.size(); ++i)
        {
            InputEvent& event = m_inputQueue[i];
            event.timestamp += deltaTime;

            bool processed = false;

            switch (event.type)
            {
            case InputEvent::Type_Press:
                pressButton(event.buttonIndex);
                processed = true;
                break;

            case InputEvent::Type_Release:
                releaseButton(event.buttonIndex);
                processed = true;
                break;

            case InputEvent::Type_Hold:
                if (event.timestamp == deltaTime)
                {
                    pressButton(event.buttonIndex);
                }

                if (event.timestamp >= event.duration)
                {
                    releaseButton(event.buttonIndex);
                    processed = true;
                }
                break;

            case InputEvent::Type_Wait:
                if (event.timestamp >= event.duration)
                {
                    processed = true;
                }
                break;
            }
            if (!processed)
            {
                remainingEvents.push_back(event);
            }
        }
        m_inputQueue = remainingEvents;
    }

    void ActionExecutor::updateButtonTimers(f32 deltaTime)
    {
        for (u32 i = 0; i < MAX_BUTTONS; ++i)
        {
            if (m_buttonStates[i])
            {
                m_buttonHoldTimers[i] += deltaTime;
            }
            else
            {
                m_buttonHoldTimers[i] = 0.0f;
            }
        }
    }

    void ActionExecutor::clearInputQueue()
    {
        m_inputQueue.clear();
    }
}
#endif
