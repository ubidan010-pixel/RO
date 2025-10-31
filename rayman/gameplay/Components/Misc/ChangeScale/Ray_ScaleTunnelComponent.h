#ifndef _ITF_RAY_SCALETUNNELCOMPONENT_H_
#define _ITF_RAY_SCALETUNNELCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class Ray_ScaleTunnelComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ScaleTunnelComponent, ActorComponent,1405566658)
    DECLARE_SERIALIZE()

public:

    Ray_ScaleTunnelComponent();
    virtual ~Ray_ScaleTunnelComponent();

    virtual bbool               needsUpdate() const { return btrue; }
    virtual bbool               needsDraw() const { return bfalse; }
    virtual bbool               needsDraw2D() const { return bfalse; }

    virtual void                onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void                Update( f32 _dt );
    virtual void                onEvent( Event* _event );
#ifdef ITF_SUPPORT_EDITOR
    virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

private:

    struct ActorData
    {
        ActorRef        m_ref;
        bbool           m_ignore;
    };

    typedef SafeArray <ActorData> ScaleActorList;

    ITF_INLINE const class Ray_ScaleTunnelComponent_Template * getTemplate() const;

    void                        updateShape();
    void                        processScaleChanged( EventScaleChanged* _event );
    f32                         calculateTFromPos( const Vec2d& _pos ) const;

    PhysShapeBox                m_scaledBox;
    ScaleActorList              m_actorsInside;
};

//-------------------------------------------------------------------------------------
class Ray_ScaleTunnelComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ScaleTunnelComponent_Template,TemplateActorComponent,436792055);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ScaleTunnelComponent);

public:

    Ray_ScaleTunnelComponent_Template();
    ~Ray_ScaleTunnelComponent_Template() {}

    const PhysShapeBox&             getShape() const { return m_shape; }
    f32                             getScale() const { return m_scale; }

private:

    PhysShapeBox                    m_shape;
    f32                             m_scale;
};

ITF_INLINE const class Ray_ScaleTunnelComponent_Template* Ray_ScaleTunnelComponent::getTemplate() const
{
    return static_cast<const class Ray_ScaleTunnelComponent_Template *>(m_template);
}

};

#endif //_ITF_RAY_SCALEDOORCOMPONENT_H_

