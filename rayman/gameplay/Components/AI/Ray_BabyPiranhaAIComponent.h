#ifndef _ITF_RAY_BABYPIRANHAAICOMPONENT_H_
#define _ITF_RAY_BABYPIRANHAAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    class Ray_BabyPiranhaAIComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BabyPiranhaAIComponent, AIComponent,3844595021)

        enum State
        {
            BABYPIRANHA_IDLE = 0,
            BABYPIRANHA_FOLLOW,
            BABYPIRANHA_ATTACK_UP,
            BABYPIRANHA_ATTACK_DOWN,
            BABYPIRANHA_COOLDOWN,
            BABYPIRANHA_HIT,
            ENUM_FORCE_SIZE_32(0)
        };

    public:
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        Ray_BabyPiranhaAIComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();

        virtual void        onEvent( class Event * _event);
        virtual void        Update( f32 _deltaTime );

    private:
        ITF_INLINE const class Ray_BabyPiranhaAIComponent_Template * getTemplate() const;
        void                            setState(State _newState);
        bbool                           mayAttackTarget(); //Attacks either up or down. Returns btrue if could attack
        void                            mayStartHit(class PunchStim *hit);
        bbool                           isAttacking() const {return m_state == BABYPIRANHA_ATTACK_UP || m_state== BABYPIRANHA_ATTACK_DOWN;}

        bbool                           searchTargetByType(ActorRef *_pTarget, u32 *_pTargetEnemyCount, bbool _player/*if false, any other ennemy (but not player)*/);
        bbool                           searchTarget(ActorRef *_pTarget, u32 *_pTargetEnemyCount);
        bbool                           followTarget(f32 _deltaTime); //Changes position to follow a target. May invalidate/switch current target
        u32                             getLastFrameRequestedAttackersCount(ActorRef _victim);
        void                            requestAttack();

        //initiate* functions are initialization functions which are used in setState
        void                            initiateCurrentState_Normal();
        void                            initiateAttack(bbool _attackUp);
        void                            InitiateCurrentState_Escape();

        //processing function for each state
        void                            processStateIdle(f32 _deltaTime);
        void                            processStateFollow(f32 _deltaTime);
        void                            processStateAttack(f32 _deltaTime);
        void                            processStateCooldown(f32 _deltaTime);
        void                            processStateHit(f32 _deltaTime);

        void                            moveOnPolyline(f32 delta);

        void                            sendHit(ObjectRef _victim, PhysShapeCircle *_collidingPiranha, const Vec2d &_collidingPos);

        class Ray_BabyPiranhaComponent* m_babyPiranhaComp;


        f32                             m_timer;

        f32                             m_escapeDst;

        State                           m_state;
        ActorRef                        m_targetActor;

        Angle                           m_hitAngle;
    };

    //-------------------------------------------------------------------------------------
    class Ray_BabyPiranhaAIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BabyPiranhaAIComponent_Template,AIComponent_Template,2202515726);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BabyPiranhaAIComponent);

    public:

        Ray_BabyPiranhaAIComponent_Template();
        ~Ray_BabyPiranhaAIComponent_Template() {}

        f32                             getSpeed() const { return m_speed; }
        f32                             getDetectionRange() const { return m_detectionRange; }
        f32                             getAttackRange() const { return m_attackRange; }
        f32                             getAttackTime() const { return m_attackTime; }
        f32                             getAttackCooldown() const { return m_attackCooldown; }
        f32                             getIdlePercent() const { return m_idlePercent; }
        f32                             getAttackJumpPercent() const { return m_attackJumpPercent; }
        f32                             getAttackDivePercent() const { return m_attackDivePercent; }
        f32                             getIdlePerturbation() const { return m_idlePerturbation; }
        f32                             getAttackStartTime() const { return m_attackStartTime; }
        f32                             getAttackPerturbation() const { return m_attackPerturbation; }
        f32                             getEscapeTime() const { return m_escapeTime; }
        u32                             getHitLevel() const { return m_hitLevel; }
        ITF_INLINE f32                  getBounceMultiplier() const { return m_bounceMultiplier; }
        ITF_INLINE bbool                getOnlyAttackTargetInWater() const { return m_onlyAttackTargetInWater; }

    private:

        f32                             m_speed;
        f32                             m_detectionRange;
        f32                             m_attackRange;
        f32                             m_attackTime;
        f32                             m_attackCooldown;
        f32                             m_idlePercent;
        f32                             m_attackJumpPercent;
        f32                             m_attackDivePercent;
        f32                             m_idlePerturbation;
        f32                             m_attackStartTime;
        f32                             m_attackPerturbation;
        f32                             m_escapeDst;
        f32                             m_escapeTime;
        u32                             m_hitLevel;
        f32                             m_bounceMultiplier;
        bbool                           m_onlyAttackTargetInWater;    // only attack if target is in the same water region as us
    };

    ITF_INLINE const Ray_BabyPiranhaAIComponent_Template * Ray_BabyPiranhaAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BabyPiranhaAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_BABYPIRANHAAICOMPONENT_H_
