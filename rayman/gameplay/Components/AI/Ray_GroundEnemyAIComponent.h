#ifndef _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
#define _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    class AIBehavior;
    class Ray_AIGroundRoamBehavior;
    class Ray_AIReceiveHitBehavior;

    class Ray_GroundEnemyAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GroundEnemyAIComponent, Ray_AIComponent,3660733364)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        enum Stance
        {
            Stance_Normal,
            Stance_Fight,
            Stance_Scared,
            Stance_Swim,
            ENUM_FORCE_SIZE_32(0)
        };

        Ray_GroundEnemyAIComponent();
        virtual ~Ray_GroundEnemyAIComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onCheckpointLoaded();
        virtual void                Update( f32 _dt );
        virtual void                onBehaviorFinished();
        virtual void                onEvent( Event* _event);

        virtual void                updateAnimInput();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

        Stance              m_stance;
        ITF_INLINE Stance   getStance() const { return m_stance; }
        void                setStance(Stance _val);

        PhysShape*          getCloseRangeDetectionShape() const ;
    
    protected:
        ITF_INLINE const class Ray_GroundEnemyAIComponent_Template * getTemplate() const;
        // BEHAVIORS
        AIBehavior*                     m_roamBehavior;
        Ray_AIReceiveHitBehavior*       m_receiveHitBehavior;
        AIBehavior*                     m_deathBehavior;
        AIBehavior*                     m_crushedBehavior;
        AIBehavior*                     m_spawnBehavior;
        AIBehavior*                     m_closeRangeAttackBehavior;
        class Ray_AIHitWallBehavior *   m_hitWallBehavior;
        class Ray_AISleepBehavior*      m_sleepBehavior;

        StickToPolylinePhysComponent *  m_physComponent;

        void                onActivate();

        virtual void        startRoam(const bbool _restart = bfalse);
        virtual void        startReceiveHit( HitStim* _hit );
        virtual void        startDeath();
        virtual void        startCrushed();
        virtual void        startSpawn();
        virtual void        startCloseRangeAttack(const bbool _flip);
        void                startSleep();

        virtual void        updateBehaviors( f32 _dt );

        virtual void        updateRoam( f32 _dt );
        void                checkReactionAttack();
        virtual bbool       checkHealthDeath();
        void                updateDeath( f32 _dt );
        void                updateSleep( f32 _dt );
        void                updateReceiveHit();

        virtual void        onFinishedReceiveHit();
        void                onFinishedCrush();
        virtual void        onFinishedDeath();
        void                onFinishedSpawn();
        virtual void        onFinishedCloseRangeAttack();
        void                onFinishedSleep();

        // INTERRUPTIONS
        virtual void        processHit( PunchStim* _hit );
        void                processBounceToLayer(Ray_EventBounceToLayer* _bounce);
        void                processQueryCanBeAttacked( Ray_EventQueryCanBeAttacked* query );
        virtual void        processInteractionQuery( EventInteractionQuery* interactionQuery );
        void                processCrushed( EventCrushed* crushed );
        void                processQueryBlocksHits( EventQueryBlocksHits* _queryBlocksHit );
        void                processHitSuccessful( EventHitSuccessful* _onHitSuccessful );
        void                processBlockedByPolyline( EventBlockedByPolyline * _onBlocked );
        void                processSoftCollision( Ray_EventAddSoftCollForce * onAddForce );
        f32                 m_timeCounter;

        void setChildrenBindRuntimeDisabled(const bbool _disabled);

        enum EMoveDir
        {
            MOVEDIR_NONE,
            MOVEDIR_UP,
            MOVEDIR_DOWN,
            MOVEDIR_LEFT,
            MOVEDIR_RIGHT,
        };
        EMoveDir getMoveDirectionFromAngle( f32 _angle ) const;

        bbool               m_EnemyIsInFront;

        PhysShape*          m_closeRangeDetectionShape;


        ActorRefList        m_closeRangeAttackVictims;

        bbool               m_crushSize; //0 = normal, 1 = reduced

        bbool               m_posSet;
        bbool               m_rayCastInitPos;
        bbool               m_useRayCast;
        bbool               m_bindSkippedFrame;

        ObjectRef           m_previousHitSender;
    };


    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_GroundEnemyAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GroundEnemyAIComponent_Template, Ray_AIComponent_Template,1079235816);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_GroundEnemyAIComponent);

    public:

        Ray_GroundEnemyAIComponent_Template();
        ~Ray_GroundEnemyAIComponent_Template();

        ITF_INLINE const class TemplateAIBehavior*                  getRoamBehavior() const { return m_roamBehavior; }
        ITF_INLINE const class TemplateAIBehavior*                  getDeathBehavior() const { return m_deathBehavior; }
        ITF_INLINE const class TemplateAIBehavior*                  getCrushedBehavior() const { return m_crushedBehavior; }
        ITF_INLINE const class Ray_AIReceiveHitBehavior_Template*   getReceiveHitBehavior() const { return m_receiveHitBehavior; }

        ITF_INLINE const class TemplateAIBehavior*                  getSpawnBehavior() const { return m_spawnBehavior; }
        ITF_INLINE const class TemplateAIBehavior*                  getCloseRangeAttackBehavior() const { return m_closeRangeAttackBehavior; }
        ITF_INLINE const class Ray_AIHitWallBehavior_Template *     getHitWallBehavior() const { return m_hitWallBehavior; }
        ITF_INLINE const class Ray_AISleepBehavior_Template*        getSleepBehavior() const { return m_sleepBehavior; }
        ITF_INLINE const class PhysShape*                           getCloseRangeDetectionShape() const { return m_closeRangeDetectionShape; }
        ITF_INLINE f32                                              getCloseRangeAttackPushBackForce() const { return m_closeRangeAttackPushBackForce; }
        ITF_INLINE f32                                              getSquashPenetrationRadius() const { return m_squashPenetrationRadius; }
        ITF_INLINE bbool                                            getDieInWater() const { return m_dieInWater; }
        ITF_INLINE f32                                              getRayCastDist() const { return m_rayCastDist; }
        ITF_INLINE bbool                                            getCheckSquash() const { return m_checkSquash; }
        ITF_INLINE bbool                                            unbindChildrenOnHit() const { return m_unbindChildrenOnHit; }
        ITF_INLINE bbool                                            getCheckStickForCloseRangeAttack() const { return m_checkStickForCloseRangeAttack; }
        ITF_INLINE bbool                                            getDetectHunterOwnBullet() const { return m_detectHunterOwnBullet; }

    private:
        class TemplateAIBehavior*                       m_roamBehavior;
        class Ray_AIReceiveHitBehavior_Template*        m_receiveHitBehavior;
        class TemplateAIBehavior*                       m_deathBehavior;
        class TemplateAIBehavior*                       m_crushedBehavior;
        class TemplateAIBehavior*                       m_spawnBehavior;
        class TemplateAIBehavior*                       m_closeRangeAttackBehavior;
        class Ray_AIHitWallBehavior_Template *          m_hitWallBehavior; 
        class Ray_AISleepBehavior_Template*             m_sleepBehavior;
        class PhysShape*                                m_closeRangeDetectionShape;
        f32                                             m_closeRangeAttackPushBackForce;
        f32                                             m_squashPenetrationRadius;
        bbool                                           m_dieInWater;
        f32                                             m_rayCastDist;
        bbool                                           m_checkSquash;
        bbool                                           m_unbindChildrenOnHit;
        bbool                                           m_checkStickForCloseRangeAttack;
        bbool                                           m_detectHunterOwnBullet;
    };
    ITF_INLINE const Ray_GroundEnemyAIComponent_Template * Ray_GroundEnemyAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_GroundEnemyAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
