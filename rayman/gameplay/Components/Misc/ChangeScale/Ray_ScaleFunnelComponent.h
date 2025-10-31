#ifndef _ITF_RAY_SCALEFUNNELCOMPONENT_H_
#define _ITF_RAY_SCALEFUNNELCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class Ray_ScaleFunnelComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ScaleFunnelComponent, ActorComponent,3096730962)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_ScaleFunnelComponent();
    virtual ~Ray_ScaleFunnelComponent();

    virtual bbool               needsUpdate() const { return btrue; }
    virtual bbool               needsDraw() const { return bfalse; }
    virtual bbool               needsDraw2D() const { return bfalse; }

    virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                onActorClearComponents();
    virtual void                Update( f32 _dt );
    virtual void                onEvent( class Event* _event );
#ifdef ITF_SUPPORT_EDITOR
    virtual void                drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

private:

    enum EState
    {
        STATE_SUCKIN,
        STATE_BLOWOUT,
    };

    struct ActorData
    {
        ActorRef                m_ref;
        EState                  m_state;
        f32                     m_timer;
        f32                     m_timerDuration;
        Vec2d                   m_startPos;
    };

    ITF_INLINE const class Ray_ScaleFunnelComponent_Template * getTemplate() const;

    void                        updateShape();
    void                        checkTrigger();
    void                        updateTransforming( f32 _dt );

    void                        triggerActor( Actor* _actor );
    void                        initializeState( ActorData& _actorData );
    bbool                       updateTransformingActor( f32 _dt, ActorData& _actorData );

    bbool                       isTransformingActor( ObjectRef _ref ) const;

    void                        processQueryWindTunnelInfo( Ray_EventQueryWindTunnelInfo* _queryWindTunnel );

    AnimLightComponent*         m_animComponent;
    SafeArray <ActorData>       m_transformingActors;
    PhysShapePolygon            m_checkShape;
    Vec2d                       m_enterPoint;
    Vec2d                       m_exitPoint;
};

//-------------------------------------------------------------------------------------
class Ray_ScaleFunnelComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ScaleFunnelComponent_Template,TemplateActorComponent,1393552913);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ScaleFunnelComponent);

public:

    Ray_ScaleFunnelComponent_Template();
    ~Ray_ScaleFunnelComponent_Template() {}

    f32                                     getScale() const { return m_scale; }
    f32                                     getSuckMinTime() const { return m_suckMinTime; }
    f32                                     getSuckMaxTime() const { return m_suckMaxTime; }
    f32                                     getSuckMaxSpeed() const { return m_suckMaxSpeed; }
    f32                                     getExitDistance() const { return m_exitDistance; }
    const StringID&                         getEnterRegionName() const { return m_enterRegionName; }
    const StringID&                         getEnterPoint() const { return m_enterPoint; }
    const StringID&                         getExitPoint() const { return m_exitPoint; }

private:

    f32                                     m_scale;
    f32                                     m_suckMinTime;
    f32                                     m_suckMaxTime;
    f32                                     m_suckMaxSpeed;
    f32                                     m_exitDistance;
    StringID                                m_enterRegionName;
    StringID                                m_enterPoint;
    StringID                                m_exitPoint;
};

ITF_INLINE const class Ray_ScaleFunnelComponent_Template* Ray_ScaleFunnelComponent::getTemplate() const
{
    return static_cast<const class Ray_ScaleFunnelComponent_Template *>(m_template);
}

};

#endif //_ITF_RAY_SCALEFUNNELCOMPONENT_H_
