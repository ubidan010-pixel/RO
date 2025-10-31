#ifndef _ITF_RAY_CAMERALIMITERCOMPONENT_H_
#define _ITF_RAY_CAMERALIMITERCOMPONENT_H_

namespace ITF
{
    class Ray_EventQueryCameraLimiter;

    class Ray_CameraLimiterComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CameraLimiterComponent, ActorComponent,1301298598)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_CameraLimiterComponent();
        virtual ~Ray_CameraLimiterComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );

    private:

        ITF_INLINE const class Ray_CameraLimiterComponent_Template* getTemplate() const;

        void                processNodeReached( Ray_EventNodeReached* _nodeReached );
        void                processQueryCameraLimiter( Ray_EventQueryCameraLimiter* _query );

        Margin  m_ejectMargin;
        Margin  m_deathMargin;
    };

    //-------------------------------------------------------------------------------------
    class Ray_CameraLimiterComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CameraLimiterComponent_Template,TemplateActorComponent,4158279952);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CameraLimiterComponent);

    public:

        Ray_CameraLimiterComponent_Template()
            : m_useEjectMargin(btrue)
            , m_ejectMargin(-1.f, -1.f, -1.f, -1.f)
            , m_ejectMarginDetachesPlayer(btrue)
            , m_ejectForce(100)
            , m_useDeathMargin(btrue)
            , m_deathMargin(1.f, 1.f, 1.f, 1.f)
            , m_ignoreConstraint(bfalse)
            , m_timeOut(2.f)
            , m_lastOnscreenPlayerKillsEveryone(btrue)
        {}
        ~Ray_CameraLimiterComponent_Template() {}

        bbool               getUseEjectMargin() const { return m_useEjectMargin; }
        ITF_INLINE const Margin& getEjectMargin() const { return m_ejectMargin; }
        ITF_INLINE bbool    getEjectMarginDetachesPlayer() const { return m_ejectMarginDetachesPlayer; }
        f32                 getEjectForce() const { return m_ejectForce; }
        bbool               getUseDeathMargin() const { return m_useDeathMargin; }
        ITF_INLINE const Margin& getDeathMargin() const { return m_deathMargin; }
        ITF_INLINE bbool    getIgnoreConstraint() const { return m_ignoreConstraint; }
        f32                 getTimeOut() const { return m_timeOut; }
        ITF_INLINE bbool    getLastOnscreenPlayerKillsEveryone() const { return m_lastOnscreenPlayerKillsEveryone; }

    private:

        bbool   m_useEjectMargin;
        Margin  m_ejectMargin;  // default: can be overridden by nodes
        bbool   m_ejectMarginDetachesPlayer;
        f32     m_ejectForce;
        bbool   m_useDeathMargin;
        Margin  m_deathMargin;  // default: can be overridden by nodes
        bbool   m_ignoreConstraint;
        f32     m_timeOut;
        bbool   m_lastOnscreenPlayerKillsEveryone;
    };

    ITF_INLINE const Ray_CameraLimiterComponent_Template* Ray_CameraLimiterComponent::getTemplate() const
    {
        return static_cast<const Ray_CameraLimiterComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_CAMERALIMITERCOMPONENT_H_