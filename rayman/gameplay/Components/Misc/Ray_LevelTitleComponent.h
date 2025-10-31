#ifndef _ITF_RAY_LEVELTITLECOMPONENT_H_
#define _ITF_RAY_LEVELTITLECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF {

//------------------------------------------------------------------------------
class Ray_LevelTitleComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_LevelTitleComponent, ActorComponent,347754987)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_LevelTitleComponent();
    virtual ~Ray_LevelTitleComponent();

    virtual bbool                               needsUpdate() const { return bfalse; }
    virtual bbool                               needsDraw() const { return bfalse; }
    virtual bbool                               needsDraw2D() const { return bfalse; }

    virtual void                                onActorLoaded(Pickable::HotReloadType _hotReload);

private:

    ITF_INLINE const class Ray_LevelTitleComponent_Template* getTemplate() const;
};

//------------------------------------------------------------------------------
class Ray_LevelTitleComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_LevelTitleComponent_Template, TemplateActorComponent,3531595838)
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LevelTitleComponent);
    DECLARE_SERIALIZE();

public:
    Ray_LevelTitleComponent_Template();
    virtual ~Ray_LevelTitleComponent_Template();
private:
};

ITF_INLINE const Ray_LevelTitleComponent_Template* Ray_LevelTitleComponent::getTemplate() const { return static_cast<const Ray_LevelTitleComponent_Template*>(m_template); }

} // namespace ITF

#endif //_ITF_RAY_LEVELTITLECOMPONENT_H_
