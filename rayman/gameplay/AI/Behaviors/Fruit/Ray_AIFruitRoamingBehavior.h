#ifndef _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_
#define _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_AIBOUNCEUTILITY_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIBounceUtility.h"
#endif

namespace ITF
{
    class StickToPolylinePhysComponent;
    class AIPlayAnimAction;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_AIFruitRoamingBehavior : public AIBehavior, public Ray_AIBounceUtility
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFruitRoamingBehavior, AIBehavior,1424968741);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIFruitRoamingBehavior();
        virtual ~Ray_AIFruitRoamingBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActivate();
        virtual void                        onDeactivate();
        virtual void                        onEvent(Event * _event);
        virtual void                        updateAnimInputs();

        void                                setFirstBounceDirection(f32 _direction) {m_firstBounceDirection = _direction;}

        ITF_INLINE u32                      getBouncesCount() const { return m_bouncesCount; }
        ITF_INLINE f32                      getTimeSinceLastBounce() const { return m_timeSinceLastBounce; }

    protected:
        enum State_t {State_NA, State_PreJump, State_JumpOrFall, State_FloatingBeforeFalling, State_WallBounce};

        struct PreJumpParameter
        {
            PreJumpParameter(const Vec2d &_normEdge, bbool _isFromActor) {m_normalizedEdge = _normEdge; m_isFromActor = _isFromActor;}
            Vec2d m_normalizedEdge;
            bbool m_isFromActor;
        };

#ifdef ITF_SUPPORT_EDITOR
        virtual void            drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR
        void setCurrentAnim(const StringID &_newAnim);
        void resetState();
        void setState(State_t _newState, const void *_params=NULL);
        Vec2d bounceUtility_getBounceForce( Vec2d _edgeDir);
        void bounceUtility_UpdateJumpOrFall_onIsOnGround(const Vec2d &_normalizedEdge, bbool isFromActor);
        void bounceUtility_UpdateJumpOrFall_onNearTopOfJump();
        void bounceOnPlayers(bbool _checkSpeed);
        bbool updateProceduralPreBounceAnim(f32 _delta);
        void setBounceAnimFromNormalAndPrevSpeed(const Vec2d &_normal, const Vec2d &_prevSpeed, bbool _squash=btrue);
        const StringID &getCurrentAnim() const {return m_currentAnim;}
        void bounceUtility_UpdateBounceOnWall_onBouncingOnWall(const Vec2d &_normal, const Vec2d &_newSpeed);
        void bounceUtility_bounceOnEdge_applyBouncePhysics(StickToPolylinePhysComponent *_physComponent, const Vec2d &_normalizedEdgeDir);
        void updateStickAndSnap();
        f32 computeBounceWave(f32 _time, f32 _duration);
        void updateGraphicBounce(f32 _deltaT,
            bbool &_isBouncingInDirection, f32 &_bouncingInDirection_Time, f32 _bouncingInDirection_Duration,
            const StringID &_bounceCursor_InputName, const StringID &_bounceWeight_InputName);
        void multiplySpeed(f32 _tangentialFactor, f32 _normalFactor, const Vec2d &_edgeDir);
        const ActorRef &getRider();
        void setRider(const ActorRef &_ref);
        void reactToOtherFruits();
        void updateYScale();
        void updateRotation(const f32 _dt);

        void launchAfterFirstBounceFX();
        void stopAfterFirstBounceFX();
        void launchOnBounceFx();


        ITF_INLINE const class Ray_AIFruitRoamingBehavior_Template * getTemplate() const;
        StickToPolylinePhysComponent* m_physComponent;
        StringID m_currentAnim;
        State_t m_state;
        f32 m_timeInCurrentState;
        Vec2d m_previousSpeed;
        Vec2d m_apexPoint;
        Vec2d m_edgeWhenEnteredPreJump;
        Vec2d m_speedWhenEnteredPreJump;
        Vec2d m_speedWhenEnteredWallBounce;
        f32 m_preBounceDuration;
        f32 m_timeInProceduralBounce;
        bbool m_dontChangeAnimThisFrame;
        Vec2d m_previousPos;
        f32 m_bounceInitialSpeed;
        f32 m_computedBounceForce;
        bbool m_reinitSpeed;
        f32 m_firstBounceDirection;
        bbool m_isInBounceFreeze, m_isBouncingTowardsLeft, m_isBouncingTowardsRight, m_isBouncingUpward;
        f32 m_bouncingTowardsLeft_Weight, m_bouncingTowardsRight_Weight, m_bouncingUpward_Weight;
        f32 m_bouncingTowardsLeft_Time, m_bouncingTowardsRight_Time, m_bouncingUpward_Time;
        f32 m_bouncingUpward_Duration, m_bouncingTowardsLeft_Duration, m_bouncingTowardsRight_Duration;

        ActorRef m_rider;
        u32 m_lastFrameLeftBounce;

        bbool m_useComplexAnimationSystem;
        u32 m_bouncesCount;
        f32 m_timeSinceLastBounce;

        u32 m_fxHandle;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////

    class Ray_AIFruitRoamingBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIFruitRoamingBehavior_Template,TemplateAIBehavior,2586393688);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIFruitRoamingBehavior);

    public:

        Ray_AIFruitRoamingBehavior_Template();
        ~Ray_AIFruitRoamingBehavior_Template();

        ITF_INLINE f32 getApexTypicalHeight() const { return m_apexTypicalHeight; }
        ITF_INLINE f32 getNormalGravityMultiplier() const { return m_normalGravityMultiplier; }
        ITF_INLINE f32 getStartToFloatTime() const { return m_startToFloatTime; }
        ITF_INLINE f32 getFloatTimeBeforeFalling() const { return m_floatTimeBeforeFalling; }
        ITF_INLINE f32 getFloatConstantForceDuration() const { return m_floatConstantForceDuration; }
        ITF_INLINE f32 getFloatMaxForce() const { return m_floatMaxForce; }
        ITF_INLINE f32 getSpeedThatTriggersFloating() const {return m_speedThatTriggersFloating;}
        ITF_INLINE const Angle &getBounceOnCharacterAreaAngle() const {return m_bounceOnCharacterAreaAngle;}
        ITF_INLINE const Angle &getAngleToBounceVerticallyOnCharacter() const {return m_angleToBounceVerticallyOnCharacter;}
        ITF_INLINE const Angle &getSlantingBounceOnCharacterAngle() const {return m_slantingBounceOnCharacterAngle;}
        ITF_INLINE f32 getBounceXSpeedFactor() const {return m_bounceXSpeedFactor;}
        ITF_INLINE f32 getBounceYSpeedFactor() const {return m_bounceYSpeedFactor;}
        ITF_INLINE f32 getSpeedForMaxSquash() const {return m_speedForMaxSquash;}
        ITF_INLINE f32 getMaxSquashDuration() const {return m_maxSquashDuration;}
        ITF_INLINE f32 getMaxSquashFactor() const {return m_maxSquashFactor;}
        ITF_INLINE f32 getInterFruitForce() const {return m_interFruitForce;}
        ITF_INLINE f32 getMaxBounceSpeed() const {return m_maxBounceSpeed;}
        ITF_INLINE bbool getCanAttach() const {return m_canAttach;}
        ITF_INLINE f32 getBounceSpeedAlongNormal() const {return m_bounceSpeedAlongNormal;}
        ITF_INLINE f32 getSpeedPerturbationWhenTooVertical() const {return m_speedPerturbationWhenTooVertical;}
        ITF_INLINE const Vec2d &getInitialSpeed() const {return m_initialSpeed;}
        ITF_INLINE bbool getCanBounceOnHead() const {return m_canBounceOnHead;}
		ITF_INLINE f32 getMinBounceDuration() const {return m_minBounceDuration;}
        ITF_INLINE f32 getGroundToVerticalBlendFactor() const {return m_groundToVerticalBlendFactor;}
        ITF_INLINE f32 getMinSpeedForPerturbation() const {return m_minSpeedForPerturbation;}
        ITF_INLINE bbool getIsStaticOnX() const {return m_isStaticOnX;}
        ITF_INLINE f32 getSpeedForMaxBounceDuration() const {return m_speedForMaxBounceDuration;}
        ITF_INLINE f32 getMaxBounceDuration() const {return m_maxBounceDuration;}
        ITF_INLINE f32 getInterreaction_maxBounceSpeedMultiplier() const {return m_interreaction_maxBounceSpeedMultiplier;}
        ITF_INLINE f32 getInterreaction_addVerticalSpeed() const {return m_interreaction_addVerticalSpeed;}
        ITF_INLINE f32 getMassInfluenceOnBounce() const {return m_massInfluenceOnBounce;}
        ITF_INLINE f32 getFallDeformationReactivity() const {return m_fallDeformationReactivity;}
        ITF_INLINE f32 getFallDeformation() const {return m_fallDeformation;}
        ITF_INLINE f32 getSpeedForFallDeformation() const {return m_speedForFallDeformation;}
        ITF_INLINE StringID getStandAnimationName() const { return m_standAnimationName; }
        ITF_INLINE const Angle & getRotationSpeed() const { return m_rotationSpeed; }
        ITF_INLINE const StringID & getFXOnBounce() const { return m_fxOnBounce; }
        ITF_INLINE const StringID & getFXAfterFirstBounce() const { return m_fxAfterFirstBounce; }

    private:
        f32 m_apexTypicalHeight;
        f32 m_normalGravityMultiplier;
        f32 m_startToFloatTime;
        f32 m_floatConstantForceDuration;
        f32 m_floatMaxForce;
        f32 m_speedThatTriggersFloating;
        f32 m_floatTimeBeforeFalling;
        Angle m_bounceOnCharacterAreaAngle;
        Angle m_angleToBounceVerticallyOnCharacter;
        Angle m_slantingBounceOnCharacterAngle;
        f32 m_bounceXSpeedFactor, m_bounceYSpeedFactor ;
        f32 m_speedForMaxSquash;
        f32 m_maxSquashDuration;
        f32 m_maxSquashFactor;
        f32 m_interFruitForce;
        f32 m_maxBounceSpeed;
        f32 m_bounceSpeedAlongNormal;
        f32 m_speedPerturbationWhenTooVertical;
        Vec2d m_initialSpeed;	
        bbool m_canAttach;
        bbool m_canBounceOnHead;
		f32 m_minBounceDuration;
        f32 m_groundToVerticalBlendFactor;
        f32 m_minSpeedForPerturbation;
        bbool m_isStaticOnX;
        f32 m_speedForMaxBounceDuration;
        f32 m_maxBounceDuration;
        f32 m_interreaction_maxBounceSpeedMultiplier;
        f32 m_interreaction_addVerticalSpeed;
        f32 m_massInfluenceOnBounce;
        f32 m_fallDeformationReactivity;
        f32 m_fallDeformation;
        f32 m_speedForFallDeformation;
        StringID m_standAnimationName;
        Angle m_rotationSpeed;
        StringID m_fxOnBounce;
        StringID m_fxAfterFirstBounce;

    };

    ITF_INLINE const class Ray_AIFruitRoamingBehavior_Template * Ray_AIFruitRoamingBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIFruitRoamingBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

