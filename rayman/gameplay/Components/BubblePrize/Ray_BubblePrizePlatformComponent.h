#ifndef _ITF_RAY_BUBBLEPRIZEPLATFORMCOMPONENT_H_
#define _ITF_RAY_BUBBLEPRIZEPLATFORMCOMPONENT_H_

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

namespace ITF
{

class EventStickOnPolyline;
class EventHanging;
class PolylineComponent;
class Ray_BubblePrizePlatformComponent_Template;

//------------------------------------------------------------------------------
// Launch on child on weight change, all of them on a crush attack. Used only
// for bubble prizes at the moment, thus the naming. Could be used to launch any
// child though, just rename me if you do.
class Ray_BubblePrizePlatformComponent : public Ray_ChildLaunchComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizePlatformComponent, Ray_ChildLaunchComponent,1615332505)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_BubblePrizePlatformComponent();
    virtual ~Ray_BubblePrizePlatformComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void Update(f32 _dt);
    virtual void onEvent(Event* _event);

private:
    ITF_INLINE const Ray_BubblePrizePlatformComponent_Template* getTemplate() const;
    
    void processStick(const EventStickOnPolyline* _stick);
    void processHang(const EventHanging* _hang);
    
    PolylineComponent* m_polylineComponent;
};

//------------------------------------------------------------------------------
class Ray_BubblePrizePlatformComponent_Template : public Ray_ChildLaunchComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizePlatformComponent_Template,Ray_ChildLaunchComponent_Template,3959825116);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BubblePrizePlatformComponent);

public:
    Ray_BubblePrizePlatformComponent_Template();
    virtual ~Ray_BubblePrizePlatformComponent_Template();
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_BubblePrizePlatformComponent_Template*
Ray_BubblePrizePlatformComponent::getTemplate() const
{
    return static_cast<const Ray_BubblePrizePlatformComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_BUBBLEPRIZEPLATFORMCOMPONENT_H_
