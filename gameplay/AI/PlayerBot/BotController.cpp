#include "precompiled_gameplay.h"
#ifdef ITF_SUPPORT_BOT_AUTO
#include "BotController.h"
#include "ActionExecutor.h"
#include "DecisionModule.h"
#include "PerceptionModule.h"

namespace ITF
{
    BotController::BotController(GameManager* gameManager)
    : m_currentMode(BotMode_Disabled)
    , m_gameManager(gameManager)
    , m_decision(nullptr)
    , m_initialized(false)
    {
    }

    BotController::~BotController()
    {
        shutdown();
    }

    void BotController::initialize()
    {
        if (m_initialized)
            return;

        if (!m_gameManager)
            return;

        m_perception = new PerceptionModule(m_gameManager);
        if (!m_perception)
        {
            shutdown();
            return;
        }

        m_decision = new DecisionModule();
        if (!m_decision)
        {
            shutdown();
            return;
        }

        m_actionExecutor = new ActionExecutor(INPUT_ADAPTER);
        if (!m_actionExecutor)
        {
            shutdown();
            return;
        }
        m_initialized = true;
    }

    void BotController::shutdown()
    {
        if (!m_initialized)
            return;
        setMode(BotMode_Disabled);

        if (m_decision)
        {
            delete m_decision;
            m_decision = nullptr;
        }

        if (m_actionExecutor)
        {
            delete m_actionExecutor;
            m_actionExecutor = nullptr;
        }

        if (m_perception)
        {
            delete m_perception;
            m_perception = nullptr;
        }

        m_initialized = false;
    }

    void BotController::update(f32 deltaTime)
    {
        if (!m_initialized || m_currentMode == BotMode_Disabled)
            return;

        if (!m_perception || !m_decision || !m_actionExecutor)
        {
            setMode(BotMode_Disabled);
            return;
        }

        m_perception->update(deltaTime);
        const GameState& currentState = m_perception->getCurrentState();

        // BotAction selectedAction = m_decision->selectAction(currentState);
        // m_actionExecutor->executeAction(selectedAction, deltaTime);
        // m_actionExecutor->update(deltaTime);
    }

    void BotController::setMode(BotMode mode)
    {
        if (m_currentMode == mode)
            return;
        m_currentMode = mode;
    }

    void BotController::setStanceCallback(BotStanceCallback callback)
    {
        if (m_perception)
        {
            m_perception->setStanceCallback(callback);
        }
    }

    void BotController::setScanTargetsCallback(ScanTargetsCallback callback)
    {
        if (m_perception)
        {
            m_perception->setScanTargetsCallback(callback);
        }
    }
}
#endif
