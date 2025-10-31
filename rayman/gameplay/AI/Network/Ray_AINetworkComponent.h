#ifndef _ITF_RAY_AINETWORKCOMPONENT_H_
#define _ITF_RAY_AINETWORKCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class Ray_AINetworkComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AINetworkComponent,ActorComponent,1696315433)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_AINetworkComponent();
    virtual ~Ray_AINetworkComponent();

    virtual bbool       needsUpdate() const { return btrue; }
    virtual bbool       needsDraw() const { return bfalse; }
    virtual bbool       needsDraw2D() const { return bfalse; }

    virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void        onActorClearComponents();
    virtual void        Update( f32 _dt );
    virtual void        onEvent( Event * _event);

#ifdef ITF_SUPPORT_EDITOR
    virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

protected:

    const class Ray_AINetworkComponent_Template* getTemplate() const;

    void                scaleShape();

    ActorRefList        m_actorsInsideLastFrame;
    PhysShape*          m_scaledShape;
};

class Ray_AINetworkComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AINetworkComponent_Template,TemplateActorComponent,1829036950);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AINetworkComponent);

public:

    Ray_AINetworkComponent_Template();
    ~Ray_AINetworkComponent_Template();

    const PhysShape*        getShape() const { return m_shape; }

private:

    PhysShape*              m_shape;
};

ITF_INLINE const class Ray_AINetworkComponent_Template* Ray_AINetworkComponent::getTemplate() const
{
    return static_cast<const class Ray_AINetworkComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_AINETWORKCOMPONENT_H_