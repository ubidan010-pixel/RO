#ifndef _ITF_RAY_NAUTILUSAICOMPONENT_H_
#define _ITF_RAY_NAUTILUSAICOMPONENT_H_

namespace ITF
{
    class FXControllerComponent;
    class SoundComponent;

    class Ray_NautilusAIComponent : public ActorComponent, IUpdateSoundInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_NautilusAIComponent, ActorComponent,3430587283)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_NautilusAIComponent();
        virtual ~Ray_NautilusAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onSceneActive();
        virtual void        onCheckpointLoaded();
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface , u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_NautilusAIComponent_Template* getTemplate() const;

        class User
        {
        public:
            User()
                : m_angle(0.f)
            {}

            ActorRef m_actor;
            Vec2d m_pos;    // local
            f32 m_angle;
        };
        typedef SafeArray<User> UserList;


        enum Mode
        {
            Mode_Fixed,
            Mode_Roll,
            ENUM_FORCE_SIZE_32(Mode)
        };

        #define SERIALIZE_MODE(name,val) \
        SERIALIZE_ENUM_BEGIN(name,val); \
            SERIALIZE_ENUM_VAR(Mode_Fixed); \
            SERIALIZE_ENUM_VAR(Mode_Roll); \
        SERIALIZE_ENUM_END();


        LinkComponent* m_linkComponent;

        void init();

        void initFx();

        // instance-serialized
        AngleAmount m_minAngle;
        AngleAmount m_maxAngle;
        Mode m_mode;
        bbool m_lockOnMinReached;
        bbool m_lockOnMaxReached;

        UserList m_users;
        f32 m_angularSpeed;
        f32 m_stopTimer;        // when no more user are present, wait a bit then drift to 0
        f32 m_angleTraveled;    // cumulated angle

        Vec2d getUserPos( const Actor* _user ) const;
        f32 getUserAngle( const Actor* _user ) const;
        void processStick( EventStickOnPolyline* onStick );
        void processQueryDisableAirControl( Ray_EventQueryDisableAirControlInfo* _query );
        void move( const f32 _dt, const f32 _deltaAngle );
        void updateTweenInput();



        FXControllerComponent* m_fxController;
        SoundComponent* m_soundComponent;

        u32 m_fxHandle;
        bbool m_fxEndReachedPlayed;

        void updateFx();
        virtual void updateSoundInput();
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_NautilusAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_NautilusAIComponent_Template, TemplateActorComponent,101315663);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_NautilusAIComponent);

    public:

        Ray_NautilusAIComponent_Template()
            : m_angularSpeedMultiplier(1.f)
            , m_angularAcceleration(btrue, 30.f)
            , m_angularDeceleration(btrue, 30.f)
            , m_stopDelay(0.1f)
            , m_rollbackSpeed(bfalse, 0.f)
            , m_rollbackBrakeAngle(btrue, 15.f)
            , m_rollbackBrakeRatio(0.f)
            , m_moveRadius(10.f)
            , m_minAngle(bfalse, 0.f)
            , m_maxAngle(bfalse, 0.f)
            , m_lockOnMinReached(bfalse)
            , m_lockOnMaxReached(bfalse)
            , m_endBrakeAngle(btrue, 15.f)
            , m_input("Cursor")
            , m_airControlMinAngularSpeed(btrue, 5.f)
            , m_airControlMaxAngularSpeed(btrue, 90.f)
            , m_airControlMinAmount(0.5f)
            , m_airControlMaxAmount(0.f)
            , m_airControlMinDuration(0.1f)
            , m_airControlMaxDuration(0.5f)
            , m_fxStartSpeed(btrue, 5.f)
            , m_fxStopSpeed(btrue, 10.f)
            , m_fxEndReachedStartAngle(btrue, 5.f)
            , m_fxEndReachedStopAngle(btrue, 10.f)
            , m_moveOnTopSpeedMultiplier(0.5f)
        {
        }
        virtual ~Ray_NautilusAIComponent_Template() {}

        virtual void onTemplateLoaded();

        ITF_INLINE f32 getAngularSpeedMultiplier() const { return m_angularSpeedMultiplier; }
        ITF_INLINE f32 getAngularAcceleration() const { return m_angularAcceleration.ToRadians(); }
        ITF_INLINE f32 getAngularDeceleration() const { return m_angularDeceleration.ToRadians(); }
        ITF_INLINE f32 getStopDelay() const { return m_stopDelay; }
        ITF_INLINE f32 getRollbackSpeed() const { return m_rollbackSpeed.ToRadians(); }
        ITF_INLINE f32 getRollbackBrakeRatio() const { return m_rollbackBrakeRatio; }

        ITF_INLINE f32 getMoveRadius() const { return m_moveRadius; }
        const AngleAmount& getMinAngle() const { return m_minAngle; }
        const AngleAmount& getMaxAngle() const { return m_maxAngle; }
        ITF_INLINE bbool getLockOnMinReached() const { return m_lockOnMinReached; }
        ITF_INLINE bbool getLockOnMaxReached() const { return m_lockOnMaxReached; }
        ITF_INLINE f32 getEndBrakeAngle() const { return m_endBrakeAngle.ToRadians(); }
        const StringID& getInput() const { return m_input; }

        ITF_INLINE f32 getAirControlMinAngularSpeed() const { return m_airControlMinAngularSpeed.ToRadians(); }
        ITF_INLINE f32 getAirControlMaxAngularSpeed() const { return m_airControlMaxAngularSpeed.ToRadians(); }
        ITF_INLINE f32 getAirControlMinAmount() const { return m_airControlMinAmount; }
        ITF_INLINE f32 getAirControlMaxAmount() const { return m_airControlMaxAmount; }
        ITF_INLINE f32 getAirControlMinDuration() const { return m_airControlMinDuration; }
        ITF_INLINE f32 getAirControlMaxDuration() const { return m_airControlMaxDuration; }

        ITF_INLINE const StringID& getFxName() const { return m_fxName; }
        ITF_INLINE f32 getFxStartSpeed() const { return m_fxStartSpeed.ToRadians(); }
        ITF_INLINE f32 getFxStopSpeed() const { return m_fxStopSpeed.ToRadians(); }
        ITF_INLINE const StringID& getFxInputName() const { return m_fxInputName; }
        ITF_INLINE const StringID& getFxEndReachedName() const { return m_fxEndReachedName; }
        ITF_INLINE f32 getFxEndReachedStartAngle() const { return m_fxEndReachedStartAngle.ToRadians(); }
        ITF_INLINE f32 getFxEndReachedStopAngle() const { return m_fxEndReachedStopAngle.ToRadians(); }
        ITF_INLINE f32 getMoveOnTopSpeedMultiplier() const { return m_moveOnTopSpeedMultiplier; }

    private:

        f32 m_angularSpeedMultiplier;
        Angle m_angularAcceleration;    // rad/s²
        Angle m_angularDeceleration;    // rad/s²
        f32 m_stopDelay;
        Angle m_rollbackSpeed;
        Angle m_rollbackBrakeAngle;
        f32 m_rollbackBrakeRatio;

        f32 m_moveRadius;   // convert rotation to linear movement (0 = don't move)
        AngleAmount m_minAngle;
        AngleAmount m_maxAngle;
        bbool m_lockOnMinReached;
        bbool m_lockOnMaxReached;
        Angle m_endBrakeAngle;
        StringID m_input;

        Angle m_airControlMinAngularSpeed;
        Angle m_airControlMaxAngularSpeed;
        f32 m_airControlMinAmount;
        f32 m_airControlMaxAmount;
        f32 m_airControlMinDuration;
        f32 m_airControlMaxDuration;

        StringID m_fxName;
        Angle m_fxStartSpeed;
        Angle m_fxStopSpeed;
        StringID m_fxInputName;
        StringID m_fxEndReachedName;
        Angle m_fxEndReachedStartAngle;
        Angle m_fxEndReachedStopAngle;

        f32 m_moveOnTopSpeedMultiplier;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_NautilusAIComponent_Template* Ray_NautilusAIComponent::getTemplate() const
    {
        return static_cast<const Ray_NautilusAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_NAUTILUSAICOMPONENT_H_