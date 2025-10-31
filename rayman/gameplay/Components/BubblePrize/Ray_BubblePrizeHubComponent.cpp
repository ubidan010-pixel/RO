#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLEPRIZEHUBCOMPONENT_H_
#include "rayman/gameplay/Components/BubblePrize/Ray_BubblePrizeHubComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZEHUBCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizeHubComponent)
BEGIN_SERIALIZATION_CHILD(Ray_BubblePrizeHubComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_BubblePrizeHubComponent)
    VALIDATE_COMPONENT_PARAM("", m_linkComponent, "LinkComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_BubblePrizeHubComponent::Ray_BubblePrizeHubComponent()
    : Super()
    , m_linkComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_BubblePrizeHubComponent::~Ray_BubblePrizeHubComponent()
{
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeHubComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_linkComponent = m_actor->GetComponent<LinkComponent>();

    // listen to frieze notifications
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventNotifyBubblePrizeHub,3264814460), this);
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeHubComponent::onSceneActive()
{
    Super::onSceneActive();

    // register
    registerToFrieze();
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeHubComponent::onEvent(Event* _event)
{
    // process events
    if (Ray_EventNotifyBubblePrizeHub* notify = _event->DynamicCast<Ray_EventNotifyBubblePrizeHub>(ITF_GET_STRINGID_CRC(Ray_EventNotifyBubblePrizeHub,3264814460)))
    {
        processNotify(notify);
    }
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeHubComponent::registerToFrieze()
{
    // find first frieze
    AIUtils::LinkIterator iterator(m_linkComponent, btrue);
    Frise* frieze = iterator.getNextRelativeFrise();
    ITF_WARNING(m_actor, frieze, "No child frieze, link to one");
    if (!frieze)
    {
        return;
    }

    // warn against several friezes
    Frise* another = iterator.getNextRelativeFrise();
    ITF_WARNING(m_actor, !another, "Several children friezes, not supported");
    if (another)
    {
        return;
    }

    // register to first frieze
    Ray_EventRegisterBubblePrizeHub registerEvent;
    registerEvent.setSender(m_actor->getRef());
    frieze->onEvent(&registerEvent);
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeHubComponent::processNotify(const Ray_EventNotifyBubblePrizeHub* _notify)
{
    // launch child (= bubble prize)
    if (canLaunch())
    {
        if (_notify->isCrushAttack())
        {
            launchAll();
        }
        else
        {
            launch();
        }
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizeHubComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_BubblePrizeHubComponent_Template)
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_BubblePrizeHubComponent_Template::Ray_BubblePrizeHubComponent_Template()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_BubblePrizeHubComponent_Template::~Ray_BubblePrizeHubComponent_Template()
{
}

} // namespace ITF
