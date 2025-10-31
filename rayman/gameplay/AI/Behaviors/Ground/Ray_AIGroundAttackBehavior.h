#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTATTACKBEHAVIOR_H_

#ifndef _ITF_AIPERFORMHITACTION_H_
#include "gameplay/AI/Actions/AIPerformHitAction.h"
#endif //_ITF_AIPERFORMHITACTION_H_


namespace ITF
{
    class AIAction;
    class AIJumpAction;
    class AIFallAction;
    class AIPlayAnimAction;
    class Ray_GroundAIComponent;
    class Ray_AvoidanceComponent;

    class Ray_AIGroundAttackBehavior : public Ray_AIGroundBaseMovementAttackBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundAttackBehavior, Ray_AIGroundBaseMovementAttackBehavior,1320632199);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIGroundAttackBehavior();
        virtual ~Ray_AIGroundAttackBehavior();

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

        virtual bbool               canBeInterrupted() const;

        virtual void                HACK_setDarktoonifyAction();

    private:
        ITF_INLINE const class Ray_AIGroundAttackBehavior_Template * getTemplate() const;

        enum EBlockedSide
        {
            BLOCKED_NONE,
            BLOCKED_LEFT,
            BLOCKED_RIGHT,
            ENUM_FORCE_SIZE_32(0)
        };



        Ray_GroundAIComponent*      m_groundAIComponent;
        Ray_AvoidanceComponent*     m_avoidanceComponent;

        // ACTIONS
        AIAction*                   m_detect;               // played as soon as we enter the behavior
        AIAction*                   m_backDetect;
        SafeArray<AIPerformHitAction*>        m_attacks;              // we need attacks to be sorted by distance
        AIJumpAction*               m_jump;
        AIFallAction*               m_fall;
        AIPlayAnimAction*           m_giveUp;
        AIAction*                   m_outOfRange;

        void                        updateIdle( f32 _dt );
        void                        updateMove( f32 _dt );
        void                        updateAttack( f32 _dt );

        bbool                       checkStuck( f32 _dt );
        bbool                       checkGiveUpRange();
        bbool                       checkBlocked();
        bbool                       checkBlockingObstacle();
        bbool                       checkBlockingPolyline( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, const Vec2d& _contactPoint );
        bbool                       checkHole( PolyLine * _poly );
        bbool                       getBestAttack();
        bbool                       checkSlope( PolyLine * poly );


        void                        startAttack();
        void                        startDetect();
        void                        startIdle();
        void                        startWalk();
        void                        startJump();
        void                        startFall();
        void                        startGiveUp();

        void                        onFinishedGiveUp();
        virtual void                 onActionFinishedUturn();
        void                        onFinishedJump();
        void                        onFinishedFall();
        void                        onFinishedDetect();
        void                        onFinishedAttack();
        void                        onFinishedOutOfRange();
        void                        onFinishedBackDetect();

        bbool                       checkOutOfRange();

        void                        setBlocked();
        void                        avoidOtherActors();
        void                        setBackPedalingInput( bbool backPedaling );
        bbool                       isValidSlope( f32 _slopeAngle ) const;



        Actor*                      m_target;
        bbool                       m_targetIsLeft;
        f32                         m_targetDistance;

        f32                         m_cooldownTimer;
        u32                         m_pendingAttack;
        u32                         m_currentAttack;

        EBlockedSide                m_blocked;

 
        // avoidance data

        f32                         m_avoidanceTimer;
        bbool                       m_neededToMove;


        u32                         m_currentMoveRangeIndex;

        // HACK for demo: we should probably use a custom tree node instead!
        bbool                       m_lastFlipped;
        bbool                       m_wasLookingLeft;  // was I looking left the last time I started an action?
    };
    class Ray_AIGroundAttackBehavior_Template : public Ray_AIGroundBaseMovementAttackBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundAttackBehavior_Template,Ray_AIGroundBaseMovementAttackBehavior_Template,609353726);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIGroundAttackBehavior);

    public:

        Ray_AIGroundAttackBehavior_Template();
        ~Ray_AIGroundAttackBehavior_Template();
        struct AttackData
        {
            DECLARE_SERIALIZE()

            AttackData()
                : m_range(Vec2d(0.0f,0.0f),Vec2d(1.0f,1.0f))
                , m_cooldownDuration(1)
                , m_action(NULL)
            {
            }
            ~AttackData();

            bbool isValid(f32 _coolDownTimer, bbool _lookRight, Actor * _actor, Actor * _target) const 
            {
                return  _coolDownTimer > m_cooldownDuration 
                    && isVictimInRange(_lookRight, _actor, _target);
            }
            bbool isVictimInRange(bbool _lookRight, Actor * _actor, Actor * _target) const
            {
                const Vec2d& myPos = _actor->get2DPos();
                const Vec2d victimPos = _target->get2DPos()- myPos;

                AABB aabb(m_range);
                if (!_lookRight)
                    aabb.FlipHorizontaly(bfalse);

                return aabb.contains(victimPos);
            }


            AABB                        m_range;
            f32                         m_cooldownDuration;
            AIPerformHitAction_Template* m_action;
        };
        typedef ITF_VECTOR<AttackData> AttackDataList;

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
        ITF_INLINE const Ray_AIGroundAttackBehavior_Template::AttackDataList & getAttacks() const { return m_attacks; }
        ITF_INLINE const class AIJumpAction_Template* getJump() const { return m_jump; }
        ITF_INLINE const class AIFallAction_Template* getFall() const { return m_fall; }
        ITF_INLINE const class AIPlayAnimAction_Template* getGiveUp() const { return m_giveUp; }
        ITF_INLINE const class AIAction_Template* getOutOfRange() const { return m_outOfRange; }
        ITF_INLINE const SafeArray<MoveRange> & getMoveRanges() const { return m_moveRanges; }
        ITF_INLINE f32 getGiveUpDistance() const { return m_giveUpDistance; }
        ITF_INLINE f32 getSlopeDetectionRange() const { return m_slopeDetectionRange; }
        ITF_INLINE Angle getMaxSlopeAngleUp() const { return m_maxSlopeAngleUp; }
        ITF_INLINE Angle getMaxSlopeAngleDown() const { return m_maxSlopeAngleDown; }
        ITF_INLINE f32 getOutOfRangeDistance() const { return m_outOfRangeDistance; }
        ITF_INLINE Angle getOutOfRangeAngle() const { return m_outOfRangeAngle; }
        ITF_INLINE f32 getMinIdleTime() const { return m_minIdleTime; }
        ITF_INLINE f32 getMinWalkTime() const { return m_minWalkTime; }
        ITF_INLINE bbool getHackFlip() const { return m_hackFlip; }

    private:
        class AIAction_Template*                   m_detect;               // played as soon as we enter the behavior
        class AIAction_Template*                   m_backDetect;
        AttackDataList                    m_attacks;              // we need attacks to be sorted by distance
        class AIJumpAction_Template*               m_jump;
        class AIFallAction_Template*               m_fall;
        class AIPlayAnimAction_Template*           m_giveUp;
        class AIAction_Template*                   m_outOfRange;
        SafeArray<MoveRange>        m_moveRanges;
        f32                         m_giveUpDistance;
        f32                         m_slopeDetectionRange;
        Angle                       m_maxSlopeAngleUp;
        Angle                       m_maxSlopeAngleDown;
        f32                         m_outOfRangeDistance;
        Angle                       m_outOfRangeAngle;
        f32                         m_minIdleTime;
        f32                         m_minWalkTime;
        bbool                       m_hackFlip;
    };

    ITF_INLINE const class Ray_AIGroundAttackBehavior_Template * Ray_AIGroundAttackBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIGroundAttackBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_