#ifndef _RAY_DECISIONMODULE_H_
#define _RAY_DECISIONMODULE_H_

#ifdef ITF_SUPPORT_BOT_AUTO

namespace ITF
{
    struct GameState;

    struct BotAction
    {
        bool moveLeft;
        bool moveRight;
        bool jump;
        bool attack;
        bool sprint;

        enum AttackType
        {
            AttackType_None = 0,
            AttackType_Quick,
            AttackType_Heavy,
            AttackType_Aerial,
            AttackType_Spin,
            AttackType_Count
        };
        AttackType attackType;

        f32 movementIntensity;
        f32 confidence;

        BotAction()
            : moveLeft(false)
            , moveRight(false)
            , jump(false)
            , attack(false)
            , sprint(false)
            , attackType(AttackType_None)
            , movementIntensity(1.0f)
            , confidence(0.0f)
        {}
    };

    class DecisionModule
    {
    public:
        DecisionModule();
        ~DecisionModule();

        BotAction selectAction(const GameState& state);
    };
}
#endif
#endif
