#ifndef _ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_
#define _ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_

namespace ITF
{
    class Ray_SeekingJellyfishAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SeekingJellyfishAIComponent, ActorComponent,3961152351)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_SeekingJellyfishAIComponent();
        virtual ~Ray_SeekingJellyfishAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );

        ITF_INLINE ObjectRef getRegion() const { return m_region; }
        ITF_INLINE void     setRegion( const ObjectRef _value ) { m_region = _value; }
        ITF_INLINE void     setIgnoreRegions( const bbool _value ) { m_ignoreRegions = btrue; }

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        virtual void        drawDebug() const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_SeekingJellyfishAIComponent_Template* getTemplate() const;

        bbool m_disabled;

        enum State
        {
            State_TargetPlayer,
            State_Flee,
        };
        State m_state;

        Vec2d m_targetPos;
        f32 m_targetChangeTimer;

        Vec2d m_targetDir;
        f32 m_speed;
        f32 m_initialSpeedBoostTimer;
        f32 m_angularSpeed;

        bbool m_ignoreRegions;
        ObjectRef m_region; // the last region we were in
        Vec2d m_lastValidPos;
        f32 m_outOfRegionTimer;

        bbool avoidRegions( const Vec2d& _myPos, const Vec2d& _myDir );
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_SeekingJellyfishAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SeekingJellyfishAIComponent_Template, TemplateActorComponent,348013504);
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SeekingJellyfishAIComponent);
        DECLARE_SERIALIZE();

    public:

        Ray_SeekingJellyfishAIComponent_Template()
            : m_minSpeed(5.f)
            , m_maxSpeed(10.f)
            , m_acceleration(10.f)
            , m_minAngularSpeed(btrue, 60)
            , m_maxAngularSpeed(btrue, 90)
            , m_angularAcceleration(btrue, 90)
            , m_initialSpeedBoostMultiplier(0.f)
            , m_initialSpeedBoostDuration(0.f)
            , m_fleeSpeedMultiplier(2.f)
            , m_fleeZSpeed(0.f)

            , m_targetChangeTimeMin(1.f)
            , m_targetChangeTimeMax(3.f)
            , m_targetAnticipationMultiplier(1.f)

            , m_regionId("Jellyfish")
            , m_avoidRegionId("Jellyfish-avoid")
            , m_raycastDepth(3.f)
            , m_goBackToRegionTimeLimit(2.f)
        {
        }
        virtual ~Ray_SeekingJellyfishAIComponent_Template() {}

        ITF_INLINE f32 getMinSpeed() const { return m_minSpeed; }
        ITF_INLINE f32 getMaxSpeed() const { return m_maxSpeed; }
        ITF_INLINE f32 getAcceleration() const { return m_acceleration; }
        ITF_INLINE f32 getMinAngularSpeed() const { return m_minAngularSpeed.ToRadians(); }
        ITF_INLINE f32 getMaxAngularSpeed() const { return m_maxAngularSpeed.ToRadians(); }
        ITF_INLINE f32 getAngularAcceleration() const { return m_angularAcceleration.ToRadians(); }
        ITF_INLINE f32 getInitialSpeedBoostMultiplier() const { return m_initialSpeedBoostMultiplier; }
        ITF_INLINE f32 getInitialSpeedBoostDuration() const { return m_initialSpeedBoostDuration; }
        ITF_INLINE f32 getFleeSpeedMultiplier() const { return m_fleeSpeedMultiplier; }
        ITF_INLINE f32 getFleeZSpeed() const { return m_fleeZSpeed; }

        ITF_INLINE f32 getTargetChangeTimeMin() const { return m_targetChangeTimeMin; }
        ITF_INLINE f32 getTargetChangeTimeMax() const { return m_targetChangeTimeMax; }
        ITF_INLINE f32 getTargetAnticipationMultiplier() const { return m_targetAnticipationMultiplier; }

        ITF_INLINE const StringID& getRegionId() const { return m_regionId; }
        ITF_INLINE const StringID& getAvoidRegionId() const { return m_avoidRegionId; }
        ITF_INLINE f32 getRaycastDepth() const { return m_raycastDepth; }
        ITF_INLINE f32 getGoBackToRegionTimeLimit() const { return m_goBackToRegionTimeLimit; }

    private:

        f32 m_minSpeed;
        f32 m_maxSpeed;
        f32 m_acceleration;
        AngleAmount m_minAngularSpeed;
        AngleAmount m_maxAngularSpeed;
        AngleAmount m_angularAcceleration;
        f32 m_initialSpeedBoostMultiplier;
        f32 m_initialSpeedBoostDuration;
        f32 m_fleeSpeedMultiplier;
        f32 m_fleeZSpeed;

        f32 m_targetChangeTimeMin;
        f32 m_targetChangeTimeMax;
        f32 m_targetAnticipationMultiplier;

        StringID m_regionId;            // stay inside this type of region
        StringID m_avoidRegionId;       // avoid this type of region
        f32 m_raycastDepth;             // to avoid the region borders
        f32 m_goBackToRegionTimeLimit;  // if we were in a region and left it for too long, go back to it
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_SeekingJellyfishAIComponent_Template* Ray_SeekingJellyfishAIComponent::getTemplate() const
    {
        return static_cast<const Ray_SeekingJellyfishAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_