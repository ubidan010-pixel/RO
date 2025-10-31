#include "precompiled_gameplay.h"
#ifdef ITF_SUPPORT_BOT_AUTO
#include "DecisionModule.h"
#include "ActionExecutor.h"

namespace ITF
{
    struct GameState;

    DecisionModule::DecisionModule()
    {
    }

    DecisionModule::~DecisionModule()
    {
    }

    BotAction DecisionModule::selectAction(const GameState& state)
    {
        BotAction action;
        return action;
    }
}
#endif
