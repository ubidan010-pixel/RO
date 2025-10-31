#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#define _ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
    class AIBehavior;
    class Ray_AIGroundRoamBehavior;
    class Ray_AIGroundRunAwayBehavior;
    class Ray_AIGroundAttackBehavior;
    class Ray_AIReceiveHitBehavior;
    class Ray_AISleepBehavior;
    class Ray_AIWaterBaseBehavior;

    class Ray_GroundAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GroundAIComponent, Ray_AIComponent,2230428483);
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

        Ray_GroundAIComponent();
        virtual ~Ray_GroundAIComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onCheckpointLoaded();
        virtual void                Update( f32 _dt );
        virtual void                onBehaviorFinished();
        virtual void                onEvent( Event* _event);

        virtual void                updateAnimInput();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR


        // public services for children behaviors

        // perception
        Actor*                      getClosestEnemy( const AABB& _range, u32* _enemyCount = NULL ) const;
        Actor*                      getClosestEnemy( f32 _range, bbool _detectBehind = btrue, u32* _enemyCount = NULL ) const;
        bbool                       hasEnemyInRange( f32 _range, bbool _detectBehind = btrue ) const;
        bbool                       hadEnemyInSight() const { return m_hadEnemyInSight; }

        // delay enemy detection randomly
        bbool                       queryAttack( f32 _dt );
        bbool               UpdatePhysicSwim( f32 _dt, bbool _static, Vec2d _direction);
        Stance              m_stance;
        ITF_INLINE Stance   getStance() const { return m_stance; }
        void                setStance(Stance _val);
        ObjectRef           getSwimingPolyline() const { return m_swimmingPolyline; }
        ITF_INLINE          u32 getSwimmingClosestEdge() const { return m_swimmingClosestEdge; }
        ITF_INLINE          bbool getSwimmingIsOnSurface() const { return m_swimmingIsOnSurface; }
        PhysShape *         getDominoHitShape() const;
    protected:
        ITF_INLINE const class Ray_GroundAIComponent_Template * getTemplate() const;
        // BEHAVIORS
        AIBehavior*                     m_roamBehavior;
        Ray_AIGroundRunAwayBehavior*    m_runAwayBehavior;
        class Ray_AIGroundBaseMovementAttackBehavior*     m_attackBehavior;
        Ray_AIReceiveHitBehavior*       m_receiveHitBehavior;
        AIBehavior*                     m_deathBehavior;
        AIBehavior*                     m_crushedBehavior;
        AIBehavior*                     m_darktoonedBehavior;
        AIBehavior*                     m_undarktoonifyBehavior;
        Ray_AISleepBehavior*            m_sleepBehavior;
        AIBehavior*                     m_spawnBehavior;
        AIBehavior *                    m_floatingBehavior;
        AIBehavior*                     m_closeRangeAttackBehavior;
        class Ray_AIHitWallBehavior *   m_hitWallBehavior;

        StickToPolylinePhysComponent *  m_physComponent;

        void                onActivate();

        virtual void        startRoam();
        void                startRunAway( ObjectRef _attacker );
        virtual void        startAttack( ObjectRef _victim );
        void                startReceiveHit( HitStim* _hit );
        virtual void        startDeath();
        void                startCrushed();
        void                startUndarktoonify();
        void                startSleep();
        void                startSpawn();
        void                startFloating(ObjectRef _ref);
        void                startCloseRangeAttack(Actor * _target);

        virtual void        updateBehaviors( f32 _dt );

        virtual void        updateRoam( f32 _dt );
        void                updateRunAway( f32 _dt );
        void                updateAttack( f32 _dt );
        void                updateDeath( f32 _dt );
        void                updateSleep( f32 _dt );
        void                updateFloating( f32 _dt);
        void                updateReceiveHit();
        bbool               tryAttackOrRunAway( f32 _dt, const AABB& _range );
        void                updateRehit();

        void                onFinishedAttack();
        void                onFinishedReceiveHit();
        void                onFinishedCrush();
        void                onFinishedSleep();
        void                onFinishedDeath();
        void                onFinishedUndarktoonify();
        void                onFinishedSpawn();
        void                onFinishedFloat();
        void                onFinishedCloseRangeAttack();

        // INTERRUPTIONS
        virtual void        processHit( PunchStim* _hit );
        void                processQueryCanBeAttacked( Ray_EventQueryCanBeAttacked* query );
        virtual void        processInteractionQuery( EventInteractionQuery* interactionQuery );
        void                processCrushed( EventCrushed* crushed );
        void                processQueryBlocksHits( EventQueryBlocksHits* _queryBlocksHit );
        void                processQueryCanDarktoonify( Ray_EventQueryCanDarktoonify* _queryCanDarktoonify );
        void                processQueryDarktoonificationPosition( Ray_EventQueryDarktoonificationPosition* _queryDarktoonifyPosition );
        void                processHitSuccessful(EventHitSuccessful * onHitSuccessful);
        void                processBlockedByPolyline( EventBlockedByPolyline * _onBlocked );
        void                processSoftCollision( Ray_EventAddSoftCollForce * onAddForce );
        void                darktoonify( ObjectRef _darktoon );
        Actor*              undarktoonify( const Vec2d& _darktoonForce );
        void                setDarktoonified( bbool _darktoonified, ObjectRef _darktoon );
        
        bbool               isBubbleDeathInProgress() const;

        f32                 m_timeCounter;



        void                trySwim();
        void                setSwimingPolyline( const PolyLine* _polyLine );
        void                swimingUpdatePolylines( const PolyLine* _polyLine, f32 _dt );
        void                swimUpdateDive( f32 _dt, const PolyLineEdge& _surfaceEdge, bbool _static );
        void                swimProcessPush( Vec2d& _moveDir );
        void                swimSetSwimForce( f32 _dt, const Vec2d& _moveDir, bbool _float );
        void                swimingUpdateAirFriction();
        void                swimApplyForces( f32 _dt, bbool _mustFloat );
        void                setSwimTargetRotation( f32 _target );
        void                resetSwimTargetRotation();
        void                UpdatePhysicSwimRotation( f32 _dt );
        void                UpdatePhysicSwimingCurve( f32 _dt, f32 targetDif );
        ObjectRef           m_swimmingPolyline;
        u32                 m_swimmingClosestEdge;
        u32                 m_swimmingSurfaceEdge;
        bbool               m_swimmingIsInside;
        bbool               m_swimmingIsOnSurface;
        //m_swimmingIsOnJumpRange = bfalse;
        f32                 m_swimmingWaterResistance;
        Vec2d               m_swimingFloatForce;

        f32                 m_swimingSurfaceT;
        f32                 m_swimingClosestEdgeT;
        Vec2d               m_swimDirection;
        f32                 m_swimingPushThreshold;
        f32                 m_swimingMoveTargetMultiplierBlendTime;
        f32                 m_moveTargetBlendTimer;
        bbool               m_swimingWaterFrictionSet;
        f32                 m_swimingPrevAirFrictionMultiplier;
        f32                 m_swimingWaterResistance;
        f32                 m_swimingWaterResistMaxMult;
        Vec2d               m_swimingInertia;
        f32                 m_swimingPrevUnstickMinFrictionMultiplier;
        bbool               m_swimingEnterDown;
        Vec2d               m_currentTargetForce;
        f32                 m_swimingTargetRotation;
        f32                 m_swimingStandPose;
        f32                 m_swimingTurnDistMult;
        f32                 m_swimingCurrentAngularSpeed;
        f32                 m_swimingTurnSpeedMult;
        enum EMoveDir
        {
            MOVEDIR_NONE,
            MOVEDIR_UP,
            MOVEDIR_DOWN,
            MOVEDIR_LEFT,
            MOVEDIR_RIGHT,
        };
        EMoveDir getMoveDirectionFromAngle( f32 _angle ) const;
        // public services for children behaviors
        f32                 m_attackDelay;
        f32                 m_attackTimer;

        // perception
        bbool               m_hadEnemyInSight;

        bbool               m_EnemyIsInFront;
        // darktoonification
        SpawneeGenerator            m_darktoonSpawner;
        bbool                       m_darktoonified;
        ActorRef                    m_darktoon;                 // ref to the fucker that darktoonified us
        bbool                       m_crushed;
        EventCrushed                m_crushEvent;

        //domino hits
        PhysShape *         m_dominoHitShape;
        bbool               m_reHit;
        bbool               m_crushSize; //0 = normal, 1 = reduced
        
        // prevent unwanted squashing in subclasses
        virtual bbool isSquashAuthorized() const { return btrue; }
    };


    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_GroundAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GroundAIComponent_Template, Ray_AIComponent_Template,981568444);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_GroundAIComponent);

    public:

        Ray_GroundAIComponent_Template();
        ~Ray_GroundAIComponent_Template();

        ITF_INLINE f32                 getRunAwayTime()                         const {return m_runAwayTime;}

        ITF_INLINE f32                 getRoamTime()                            const {return m_roamTime;}
        ITF_INLINE f32                 getSleepTime()                           const {return m_sleepTime;}
        ITF_INLINE bbool               getCanDetectEnemiesWhileSleeping()       const {return m_canDetectEnemiesWhileSleeping;}
        ITF_INLINE const AABB&         getCloseRangeAttackDetectionRange()      const {return m_closeRangeAttackDetectionRange;}

        ITF_INLINE f32                 getswimingWaterResistMinSpeed()          const {return m_swimingWaterResistMinSpeed;}
        ITF_INLINE f32                 getSwimingWaterResistMaxSpeed()          const {return m_swimingWaterResistMaxSpeed;}
        ITF_INLINE f32                 getSwimingTopSpeed()                     const {return m_swimingTopSpeed;}
        ITF_INLINE f32                 getSwimingSurfaceRange()                 const {return m_swimingSurfaceRange;}
        ITF_INLINE f32                 getSwimingTurnUpsideDownWait()           const {return m_swimingTurnUpsideDownWait;}
        ITF_INLINE f32                 getSwimingInertiaEnterDownMultiplier()   const {return m_swimingInertiaEnterDownMultiplier;}
        ITF_INLINE f32                 getSwimingInertiaEnterNormalMultiplier() const {return m_swimingInertiaEnterNormalMultiplier;}
        ITF_INLINE f32                 getSwimingInertiaEnterMaxSpeed()         const {return m_swimingInertiaEnterMaxSpeed;}
        ITF_INLINE f32                 getSwimingInertiaSpeed()                 const {return m_swimingInertiaSpeed;}

        ITF_INLINE f32                 getMaxAttackDelay()                      const {return m_maxAttackDelay;}

        ITF_INLINE const AABB&         getEnemyDetectionRange()                 const {return m_enemyDetectionRange;}
        ITF_INLINE const AABB&         getEnemyChangeRange()                    const {return m_enemyChangeRange;}
        ITF_INLINE f32                 getKeepRunningAwayRange()                const {return m_keepRunningAwayRange;}
        ITF_INLINE u32                 getRunAwayEnemyLimit()                   const {return m_runAwayEnemyLimit;}
        ITF_INLINE bbool               getWakeUpOnEnemyDetection()              const {return m_wakeUpOnEnemyDetection;}
        ITF_INLINE bbool               getWakeUpOnCloseEnemyDetection()         const {return m_wakeUpOnCloseEnemyDetection;}


        ITF_INLINE const Path &        getDarktoonPath()                        const {return m_darktoonPath;}
        ITF_INLINE bbool               getDarktoonified_default()               const {return m_darktoonified_default;}
        ITF_INLINE StringID            getDarktoonificationBone()               const {return m_darktoonificationBone;}
        ITF_INLINE u32                 getDarktoonifiedHealth()                 const {return m_darktoonifiedHealth;}


        ITF_INLINE const class TemplateAIBehavior* getRoamBehavior() const { return m_roamBehavior; }
        ITF_INLINE const class Ray_AIGroundRunAwayBehavior_Template* getRunAwayBehavior() const { return m_runAwayBehavior; }
        ITF_INLINE const class Ray_AIGroundBaseMovementAttackBehavior_Template* getAttackBehavior() const { return m_attackBehavior; }
        ITF_INLINE const class Ray_AIReceiveHitBehavior_Template* getReceiveHitBehavior() const { return m_receiveHitBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getDeathBehavior() const { return m_deathBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getCrushedBehavior() const { return m_crushedBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getDarktoonedBehavior() const { return m_darktoonedBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getUndarktoonifyBehavior() const { return m_undarktoonifyBehavior; }
        ITF_INLINE const class Ray_AISleepBehavior_Template* getSleepBehavior() const { return m_sleepBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getSpawnBehavior() const { return m_spawnBehavior; }
        ITF_INLINE const class TemplateAIBehavior * getFloatingBehavior() const { return m_floatingBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getCloseRangeAttackBehavior() const { return m_closeRangeAttackBehavior; }
        ITF_INLINE const class Ray_AIHitWallBehavior_Template *     getHitWallBehavior() const { return m_hitWallBehavior; }
        ITF_INLINE const class PhysShape* getDominoHitShape() const { return m_dominoHitShape; }
        ITF_INLINE f32 getDarktoonificationEjectionForceX() const { return m_darktoonificationEjectionForceX; }
        ITF_INLINE f32 getDarktoonificationEjectionForceY() const { return m_darktoonificationEjectionForceY; }
        ITF_INLINE bbool getCanRehit() const { return m_canRehit; }
        ITF_INLINE f32 getDominoHitSpeedMultiplier() const { return m_dominoHitSpeedMultiplier; }
        ITF_INLINE f32 getSquashPenetrationRadius() const { return m_squashPenetrationRadius; }
    private:
        class TemplateAIBehavior*                     m_roamBehavior;
        class Ray_AIGroundRunAwayBehavior_Template*    m_runAwayBehavior;
        class Ray_AIGroundBaseMovementAttackBehavior_Template*     m_attackBehavior;
        class Ray_AIReceiveHitBehavior_Template*       m_receiveHitBehavior;
        class TemplateAIBehavior*                     m_deathBehavior;
        class TemplateAIBehavior*                     m_crushedBehavior;
        class TemplateAIBehavior*                     m_darktoonedBehavior;
        class TemplateAIBehavior*                     m_undarktoonifyBehavior;
        class Ray_AISleepBehavior_Template*            m_sleepBehavior;
        class TemplateAIBehavior*                     m_spawnBehavior;
        class TemplateAIBehavior *                    m_floatingBehavior;
        class TemplateAIBehavior*                     m_closeRangeAttackBehavior;
        class Ray_AIHitWallBehavior_Template *          m_hitWallBehavior; 
        f32                 m_runAwayTime;

        f32                 m_roamTime;
        f32                 m_sleepTime;
        bbool               m_canDetectEnemiesWhileSleeping;
        AABB                m_closeRangeAttackDetectionRange; 

        f32                 m_swimingWaterResistMinSpeed;
        f32                 m_swimingWaterResistMaxSpeed;
        f32                 m_swimingTopSpeed;
        f32                 m_swimingSurfaceRange;
        f32                 m_swimingTurnUpsideDownWait;
        f32                 m_swimingInertiaEnterDownMultiplier;
        f32                 m_swimingInertiaEnterNormalMultiplier;
        f32                 m_swimingInertiaEnterMaxSpeed;
        f32                 m_swimingInertiaSpeed;

     
        // public services for children behaviors
        f32                 m_maxAttackDelay;
  
        // perception
        AABB                m_enemyDetectionRange;          // allows us to detect our first target and enter attack mode
        AABB                m_enemyChangeRange;             // allows us to change target if a better one comes along
        f32                 m_keepRunningAwayRange;         // when running away, keep running if enemies are nearby
        u32                 m_runAwayEnemyLimit;
        bbool               m_wakeUpOnEnemyDetection;
        bbool               m_wakeUpOnCloseEnemyDetection;

        // darktoonification
        
        Path                        m_darktoonPath;
        bbool                       m_darktoonified_default;
        StringID                    m_darktoonificationBone;
        u32                         m_darktoonifiedHealth;
        f32                         m_darktoonificationEjectionForceX;
        f32                         m_darktoonificationEjectionForceY;
        class PhysShape*            m_dominoHitShape;     
        bbool                       m_canRehit;
        f32                         m_dominoHitSpeedMultiplier;
        f32                         m_squashPenetrationRadius;
    };
    ITF_INLINE const Ray_GroundAIComponent_Template * Ray_GroundAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_GroundAIComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_
