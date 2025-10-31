#ifndef _ITF_RAY_BUBBLEPRIZEBUMPERCOMPONENT_H_
#define _ITF_RAY_BUBBLEPRIZEBUMPERCOMPONENT_H_

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

namespace ITF
{

class Ray_EventTriggerBounce;
class Ray_BubblePrizeBumperComponent_Template;

//------------------------------------------------------------------------------
// Launch bubble prize on "trigger bounce" (i.e. bumpers)
class Ray_BubblePrizeBumperComponent : public Ray_ChildLaunchComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizeBumperComponent, Ray_ChildLaunchComponent,542355520)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_BubblePrizeBumperComponent();
    virtual ~Ray_BubblePrizeBumperComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void Update(f32 _dt);
    virtual void onEvent(Event* _event);

private:
    ITF_INLINE const Ray_BubblePrizeBumperComponent_Template* getTemplate() const;
    
    void processTriggerBounce(const Ray_EventTriggerBounce* _triggerBounce);
};

//------------------------------------------------------------------------------
class Ray_BubblePrizeBumperComponent_Template : public Ray_ChildLaunchComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizeBumperComponent_Template,Ray_ChildLaunchComponent_Template,2167902645);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BubblePrizeBumperComponent);

public:
    Ray_BubblePrizeBumperComponent_Template();
    virtual ~Ray_BubblePrizeBumperComponent_Template();
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_BubblePrizeBumperComponent_Template*
Ray_BubblePrizeBumperComponent::getTemplate() const
{
    return static_cast<const Ray_BubblePrizeBumperComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_BUBBLEPRIZEBUMPERCOMPONENT_H_
