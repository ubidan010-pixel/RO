#ifndef _RAY_PERCEPTIONMODULE_H_
#define _RAY_PERCEPTIONMODULE_H_

#ifdef ITF_SUPPORT_BOT_AUTO

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif

namespace ITF
{
    class GameManager;
    struct GameState;
    typedef u32 (*BotStanceCallback)(GameManager*, u32);
    typedef void (*ScanTargetsCallback)(GameManager*, GameState*);

    struct TargetInfo
    {
        Vec3d position;
        f32 distance;
        ObjectRef actorRef;
        bbool isCheckpoint;

        TargetInfo()
            : position(0.0f, 0.0f, 0.0f)
            , distance(F32_INFINITY)
            , actorRef(ObjectRef::InvalidRef)
            , isCheckpoint(bfalse)
        {
        }
    };

    struct GameState
    {
        Vec3d playerPosition;
        Vec2d playerVelocity;
        bbool isGrounded;
        EStance stance;
        GameMode gameMode;

        TargetInfo nextTarget;

        GameState()
            : playerPosition(0.0f, 0.0f, 0.0f)
            , playerVelocity(0.0f, 0.0f)
            , isGrounded(bfalse)
            , stance(STANCE_STAND)
            , gameMode(0)
        {
        }
    };

    class PerceptionModule
    {
    private:
        void extractPlayerState();
        void scanNearbyTargets();
        void debugDraw() const;

    public:
        PerceptionModule(GameManager* gameManager);
        ~PerceptionModule();
        void update(f32 deltaTime);
        const GameState& getCurrentState() const { return m_currentState; }

        void setStanceCallback(BotStanceCallback callback) { m_getStanceCallback = callback; }
        void setScanTargetsCallback(ScanTargetsCallback callback) { m_scanTargetsCallback = callback; }

    private:
        GameManager* m_gameManager;
        GameState m_currentState;
        BotStanceCallback m_getStanceCallback;
        ScanTargetsCallback m_scanTargetsCallback;
    };
}
#endif
#endif
