#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLEPRIZEBUMPERCOMPONENT_H_
#include "rayman/gameplay/Components/BubblePrize/Ray_BubblePrizeBumperComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZEBUMPERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizeBumperComponent)
BEGIN_SERIALIZATION_CHILD(Ray_BubblePrizeBumperComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_BubblePrizeBumperComponent)
    VALIDATE_COMPONENT_PARAM("hintFxPath", !getTemplate()->getHintFxPath().isEmpty(), "Path mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_BubblePrizeBumperComponent::Ray_BubblePrizeBumperComponent()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_BubblePrizeBumperComponent::~Ray_BubblePrizeBumperComponent()
{
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeBumperComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    // listen to trigger bounce
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerBounce,1890727905),this);
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeBumperComponent::Update(f32 _dt)
{
    Super::Update(_dt);
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeBumperComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);
    
    // process events
    if (Ray_EventTriggerBounce* triggerBounce = _event->DynamicCast<Ray_EventTriggerBounce>(ITF_GET_STRINGID_CRC(Ray_EventTriggerBounce,1890727905)))
    {
        processTriggerBounce(triggerBounce);
    }
}

//------------------------------------------------------------------------------
void Ray_BubblePrizeBumperComponent::processTriggerBounce(const Ray_EventTriggerBounce* _triggerBounce)
{
    // launch (bubble prize) on trigger bounce
    if (canLaunch())
    {
        if (_triggerBounce->isCrushAttack())
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
IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizeBumperComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_BubblePrizeBumperComponent_Template)
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_BubblePrizeBumperComponent_Template::Ray_BubblePrizeBumperComponent_Template()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_BubblePrizeBumperComponent_Template::~Ray_BubblePrizeBumperComponent_Template()
{
}

} // namespace ITF
