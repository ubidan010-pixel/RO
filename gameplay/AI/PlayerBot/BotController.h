#ifndef _RAY_BOTCONTROLLER_H_
#define _RAY_BOTCONTROLLER_H_
#ifdef ITF_SUPPORT_BOT_AUTO

#ifndef _RAY_PERCEPTIONMODULE_H_
#include "PerceptionModule.h"
#endif

namespace ITF
{
    class DecisionModule;
    class ActionExecutor;
    
    enum BotMode
    {
        BotMode_Disabled = 0,
        BotMode_Training,
        BotMode_Inference
    };

    class BotController
    {
    public:
        BotController(GameManager* gameManager);
        ~BotController();

        // Lifecycle
        void initialize();
        void shutdown();
        void update(f32 deltaTime);

        // Mode control
        void setMode(BotMode mode);
        BotMode getMode() const { return m_currentMode; }

        // Callbacks
        void setStanceCallback(BotStanceCallback callback);
        void setScanTargetsCallback(ScanTargetsCallback callback);

        bool isActive() const { return m_initialized && m_currentMode != BotMode_Disabled; }
    private:
        GameManager* m_gameManager;
        PerceptionModule* m_perception;
        ActionExecutor* m_actionExecutor;
        DecisionModule* m_decision;
        bool m_initialized;
        BotMode m_currentMode;

    };
}
#endif
#endif //_RAY_BOTCONTROLLER_H_
