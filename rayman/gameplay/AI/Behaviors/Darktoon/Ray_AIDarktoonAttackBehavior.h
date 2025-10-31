#ifndef _ITF_RAY_AIDARKTOONATTACKBEHAVIOR_H_
#define _ITF_RAY_AIDARKTOONATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_

namespace ITF
{
    class AIAction;
    class AIJumpAction;
    class AIFallAction;
    class AIPlayAnimAction;
    class AIPerformHitAction;
    class Ray_GroundAIComponent;
    class Ray_AvoidanceComponent;

    class Ray_AIDarktoonAttackBehavior : public Ray_AIGroundBaseMovementAttackBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonAttackBehavior, Ray_AIGroundBaseMovementAttackBehavior,2162341386);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIDarktoonAttackBehavior();
        virtual ~Ray_AIDarktoonAttackBehavior();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onActivate();
        virtual void                onDeactivate();
        virtual void                onActionFinished();

        virtual void                onEvent( Event* _event );
        virtual void                update( f32 _dt  );
        virtual void                updateAnimInputs();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

        bbool                       canBeInterrupted() const;

    private:
        ITF_INLINE const class Ray_AIDarktoonAttackBehavior_Template * getTemplate() const;
        bbool                       isAttacking() const;
        enum EBlockedSide
        {
            BLOCKED_NONE,
            BLOCKED_LEFT,
            BLOCKED_RIGHT,
        };



        Ray_GroundAIComponent*      m_groundAIComponent;
        Ray_AvoidanceComponent*     m_avoidanceComponent;

        // ACTIONS
        AIAction*                   m_detect;               // played as soon as we enter the behavior
        AIAction*                   m_backDetect;
        AIFallAction*               m_fall;
        AIPlayAnimAction*           m_giveUp;
        AIAction*                   m_outOfRange;
        AIPerformHitAction*         m_attack;
        AIPerformHitAction*         m_jumpAttack;

        void                        updateIdle( f32 _dt );
        void                        updateMove( f32 _dt );
        void                        updateAttack( f32 _dt );

        bbool                       checkStuck( f32 _dt );
        bbool                       checkGiveUpRange();
        bbool                       checkBlocked(PolyLine* _poly);

        bool isBlocked();
        bbool                       checkBlockingObstacle();
        bbool                       checkBlockingPolyline( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, const Vec2d& _contactPoint );
        bbool                       checkHole( PolyLine * _poly );
        bbool                       checkSlope( PolyLine * poly );


        void                        startAttack();
        void                        startDetect();
        void                        startIdle();
        void                        startWalk();
        void                        startFall();
        void                        startGiveUp();

        void                        onFinishedGiveUp();
        virtual void                 onActionFinishedUturn();
        void                        onFinishedJump();
        void                        onFinishedFall();
        void                        onFinishedDetect();
        void                        onFinishedAttack();
        void                        onFinishedJumpAttack();
        void                        onFinishedOutOfRange();
        void                        onFinishedBackDetect();
        virtual void                onActionFinishedBounceToLayer();
        virtual void                onActionFinishedBounce();
        bbool                       checkOutOfRange();

        void                        setBlocked();
        void                        avoidOtherActors();
        void                        setBackPedalingInput( bbool backPedaling );
        bbool                       isValidSlope( f32 _slopeAngle ) const;

        Actor*                      m_target;
        bbool                       m_targetIsLeft;
        f32                         m_targetDistance;

        EBlockedSide                m_blocked;

        f32                         m_timer;
        bbool                       m_nextAttackIsJumpAttack;

 
        // avoidance data

        f32                         m_avoidanceTimer;
        bbool                       m_neededToMove;


        u32                         m_currentMoveRangeIndex;

        // HACK for demo: we should probably use a custom tree node instead!
        bbool                       m_lastFlipped;
        bbool                       m_wasLookingLeft;  // was I looking left the last time I started an action?

        ActorRef                    m_obstacle;

    };
    class Ray_AIDarktoonAttackBehavior_Template : public Ray_AIGroundBaseMovementAttackBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonAttackBehavior_Template,Ray_AIGroundBaseMovementAttackBehavior_Template,4282596837);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIDarktoonAttackBehavior);

    public:

        Ray_AIDarktoonAttackBehavior_Template();
        ~Ray_AIDarktoonAttackBehavior_Template();

        struct MoveRange
        {
            DECLARE_SERIALIZE();

            MoveRange()
                : m_maxDistance(1000.f)
                , m_walkForce(5.f)
            {
            }

            f32 m_maxDistance;
            f32 m_walkForce;
        };

        ITF_INLINE const class AIAction_Template* getDetect() const { return m_detect; }
        ITF_INLINE const class AIAction_Template* getBackDetect() const { return m_backDetect; }
        ITF_INLINE const class AIFallAction_Template* getFall() const { return m_fall; }
        ITF_INLINE const class AIPlayAnimAction_Template* getGiveUp() const { return m_giveUp; }
        ITF_INLINE const class AIAction_Template* getOutOfRange() const { return m_outOfRange; }
        ITF_INLINE const class AIPerformHitAction_Template* getAttack() const { return m_attack; }
        ITF_INLINE const class AIPerformHitAction_Template* getJumpAttack() const { return m_jumpAttack; }
        ITF_INLINE const SafeArray<MoveRange> & getMoveRanges() const { return m_moveRanges; }
        ITF_INLINE f32 getGiveUpDistance() const { return m_giveUpDistance; }
        ITF_INLINE f32 getSlopeDetectionRange() const { return m_slopeDetectionRange; }
        ITF_INLINE Angle getMaxSlopeAngleUp() const { return m_maxSlopeAngleUp; }
        ITF_INLINE Angle getMaxSlopeAngleDown() const { return m_maxSlopeAngleDown; }
        ITF_INLINE f32 getOutOfRangeDistance() const { return m_outOfRangeDistance; }
        ITF_INLINE Angle getOutOfRangeAngle() const { return m_outOfRangeAngle; }
        ITF_INLINE ITF::f32 getMoveCycleTime() const { return m_moveCycleTime; }
        ITF_INLINE f32 getMinIdleTime() const { return m_minIdleTime; }
        ITF_INLINE f32 getMinWalkTime() const { return m_minWalkTime; }
        ITF_INLINE f32 getAttackDetectionRange() const { return m_attackDetectionRange; }
        ITF_INLINE bbool getSetVictim() const { return m_setVictim; }

    private:
        class AIAction_Template*                   m_detect;               // played as soon as we enter the behavior
        class AIAction_Template*                   m_backDetect;
        class AIFallAction_Template*               m_fall;
        class AIPlayAnimAction_Template*           m_giveUp;
        class AIAction_Template*                   m_outOfRange;
        class AIPerformHitAction_Template*         m_attack;               // played as soon as we enter the behavior
        class AIPerformHitAction_Template*         m_jumpAttack;               // played as soon as we enter the behavior
        SafeArray<MoveRange>        m_moveRanges;
        f32                         m_giveUpDistance;
        f32                         m_slopeDetectionRange;
        Angle                       m_maxSlopeAngleUp;
        Angle                       m_maxSlopeAngleDown;
        f32                         m_outOfRangeDistance;
        Angle                       m_outOfRangeAngle;
        f32                         m_moveCycleTime;
        f32                         m_minIdleTime;
        f32                         m_minWalkTime;
        f32                         m_attackDetectionRange;
        bbool                       m_setVictim;
    };

    ITF_INLINE const class Ray_AIDarktoonAttackBehavior_Template * Ray_AIDarktoonAttackBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIDarktoonAttackBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIDARKTOONATTACKBEHAVIOR_H_