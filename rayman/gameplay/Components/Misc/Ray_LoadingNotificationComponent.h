#ifndef _ITF_RAY_LOADINGNOTIFICATIONCOMPONENT_H_
#define _ITF_RAY_LOADINGNOTIFICATIONCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

class Ray_LoadNotificationComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_LoadNotificationComponent, ActorComponent,1404551269)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_LoadNotificationComponent();
    virtual ~Ray_LoadNotificationComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onActorClearComponents();
    virtual void onBecomeActive();

    virtual void Update( f32 _dt );

#ifdef ITF_SUPPORT_EDITOR
    virtual void onEditorMove(bbool _modifyInitialPos);
#endif

private:

    ITF_INLINE const class Ray_LoadNotificationComponent_Template* getTemplate() const;

    class AnimLightComponent*       m_animComponent;
    Vec2d                           m_relPos;        
};

//------------------------------------------------------------------------------
class Ray_LoadNotificationComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_LoadNotificationComponent_Template, TemplateActorComponent,3491700814)
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LoadNotificationComponent);
    DECLARE_SERIALIZE();

public:
    Ray_LoadNotificationComponent_Template();
    virtual ~Ray_LoadNotificationComponent_Template();

    const Vec2d&                    getAnimSize() const { return m_animSize; }

private:

    Vec2d                           m_animSize;
};

const Ray_LoadNotificationComponent_Template* Ray_LoadNotificationComponent::getTemplate() const { return static_cast<const Ray_LoadNotificationComponent_Template*>(m_template); }

} // namespace ITF

#endif //_ITF_RAY_LOADINGNOTIFICATIONCOMPONENT_H_
