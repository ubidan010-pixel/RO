#ifndef _ITF_RAY_AVOIDANCECOMPONENT_H_
#define _ITF_RAY_AVOIDANCECOMPONENT_H_

namespace ITF
{
    class Ray_SquadMemberData;

    class Ray_AvoidanceComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AvoidanceComponent, ActorComponent,2384145511);

    public:
        DECLARE_SERIALIZE()

        Ray_AvoidanceComponent();
        virtual ~Ray_AvoidanceComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                Update( f32 _dt );
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);


        ITF_INLINE Ray_SquadMemberData* getSquadData() const { return m_squadData; }
        ITF_INLINE bbool            getEnabled() const { return m_enabled; }
        ITF_INLINE void             setEnabled( bbool _value ) { m_enabled = _value; }

        ITF_INLINE f32              getAnticipationCoeff() const;

    private:
        ITF_INLINE const class Ray_AvoidanceComponent_Template * getTemplate() const;
        void                        registerAvoidance();
        void                        unregisterAvoidance();

        Ray_SquadMemberData*        m_squadData;
        bbool                       m_enabled;
    };

    //-------------------------------------------------------------------------------------
    class Ray_AvoidanceComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AvoidanceComponent_Template,TemplateActorComponent,2626459762);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AvoidanceComponent);

    public:

        Ray_AvoidanceComponent_Template();
        ~Ray_AvoidanceComponent_Template() {}

        bbool                       getEnabled() const { return m_enabled; }
        f32                         getAvoidanceRadius() const { return m_avoidanceRadius; }
        f32                         getAvoidanceMinDelta() const { return m_avoidanceMinDelta; }
        f32                         getAvoidanceAnticipationCoeff() const { return m_avoidanceAnticipationCoeff; }

    private:

        bbool                       m_enabled;
        f32                         m_avoidanceRadius;
        f32                         m_avoidanceMinDelta;
        f32                         m_avoidanceAnticipationCoeff;
    };

    ITF_INLINE const class Ray_AvoidanceComponent_Template * Ray_AvoidanceComponent::getTemplate() const
    {
        return static_cast<const class Ray_AvoidanceComponent_Template *>(m_template);
    }

    f32 Ray_AvoidanceComponent::getAnticipationCoeff() const 
    {
        return getTemplate()->getAvoidanceAnticipationCoeff();
    }
}

#endif //_ITF_RAY_AVOIDANCECOMPONENT_H_