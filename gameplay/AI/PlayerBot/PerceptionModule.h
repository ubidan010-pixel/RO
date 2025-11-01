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

    struct TerrainSensors
    {
        f32 groundAheadDistNear;
        f32 groundAheadDistMid;
        f32 groundAheadDistFar;
        bbool hasGroundAheadNear;
        bbool hasGroundAheadMid;
        bbool hasGroundAheadFar;

        f32 gapDepth;          // Estimated depth of the gap ahead (0 if ground detected)
        bbool frontWall;       // Blocking wall in front (horizontal ray)
        f32 frontWallHeight;   // Rough estimate of wall height (optional/approx)

        f32 slopeAhead;        // Approx slope from ground samples (dy/dx)
        bbool ledgeAbove;      // Ledge immediately above the front wall

        bbool movingPlatformBelow;
        Vec2d movingPlatformVel;

        bbool hazardAhead;
        f32 hazardDist;
        u32 hazardType;

        u32 groundedFrames;
        u32 airborneFrames;
        f32 coyoteTimeRemaining;

        TerrainSensors()
            : groundAheadDistNear(0.0f)
            , groundAheadDistMid(0.0f)
            , groundAheadDistFar(0.0f)
            , hasGroundAheadNear(bfalse)
            , hasGroundAheadMid(bfalse)
            , hasGroundAheadFar(bfalse)
            , gapDepth(0.0f)
            , frontWall(bfalse)
            , frontWallHeight(0.0f)
            , slopeAhead(0.0f)
            , ledgeAbove(bfalse)
            , movingPlatformBelow(bfalse)
            , movingPlatformVel(0.0f, 0.0f)
            , hazardAhead(bfalse)
            , hazardDist(0.0f)
            , hazardType(0)
            , groundedFrames(0)
            , airborneFrames(0)
            , coyoteTimeRemaining(0.0f)
        {
        }
    };

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

        TerrainSensors sensors;

        GameState()
            : playerPosition(0.0f, 0.0f, 0.0f)
            , playerVelocity(0.0f, 0.0f)
            , isGrounded(bfalse)
            , stance(STANCE_STAND)
            , gameMode(0)
            , sensors()
        {
        }
    };

    class PerceptionModule
    {
    private:
        void extractPlayerState();
        void extractTerrainSensors(f32 deltaTime);
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

        // Runtime counters for grounded/airborne/coyote
        u32 m_groundedFrames;
        u32 m_airborneFrames;
        f32 m_coyoteTimer;

        // Probe configuration
        f32 m_probeNearX;
        f32 m_probeMidX;
        f32 m_probeFarX;
        f32 m_probeUp;
        f32 m_probeDown;
        f32 m_wallProbeOffsetX;
        f32 m_wallProbeHeight;
        f32 m_wallScanMaxHeight;
        f32 m_coyoteTimeWindow;

        // Moving platform tracking
        ObjectRef m_lastPlatformRef;
        Vec2d m_lastPlatformPos;
    };
}
#endif
#endif
