#ifndef _ITF_RAY_BUBBLEPRIZEHUBCOMPONENT_H_
#define _ITF_RAY_BUBBLEPRIZEHUBCOMPONENT_H_

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

namespace ITF {

class LinkComponent;
class Ray_BubblePrizeHubComponent_Template;
class Ray_EventNotifyBubblePrizeHub;

//------------------------------------------------------------------------------
class Ray_BubblePrizeHubComponent : public Ray_ChildLaunchComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizeHubComponent, Ray_ChildLaunchComponent,162474712)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:
    Ray_BubblePrizeHubComponent();
    virtual ~Ray_BubblePrizeHubComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }
    
    virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
    virtual void onSceneActive();

    virtual void onEvent(Event* _event);

private:
    ITF_INLINE const Ray_BubblePrizeHubComponent_Template* getTemplate() const;

    void registerToFrieze();
    void processNotify(const Ray_EventNotifyBubblePrizeHub* _notify);

    LinkComponent* m_linkComponent;
};

//------------------------------------------------------------------------------
class Ray_BubblePrizeHubComponent_Template : public Ray_ChildLaunchComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_BubblePrizeHubComponent_Template, Ray_ChildLaunchComponent_Template,3041818536);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BubblePrizeHubComponent);

public:
    Ray_BubblePrizeHubComponent_Template();
    virtual ~Ray_BubblePrizeHubComponent_Template();
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_BubblePrizeHubComponent_Template*
Ray_BubblePrizeHubComponent::getTemplate() const
{
    return static_cast<const Ray_BubblePrizeHubComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_BUBBLEPRIZEHUBCOMPONENT_H_
