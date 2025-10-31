#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLEPRIZEPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/BubblePrize/Ray_BubblePrizePlatformComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZEPLATFORMCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_GAMEPLAYSTIMS_H_
#include "gameplay/GameplayStims.h"
#endif //_ITF_GAMEPLAYSTIMS_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizePlatformComponent)
BEGIN_SERIALIZATION_CHILD(Ray_BubblePrizePlatformComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_BubblePrizePlatformComponent)
    VALIDATE_COMPONENT_PARAM("hintFxPath", !getTemplate()->getHintFxPath().isEmpty(), "Path mandatory");
    VALIDATE_COMPONENT_PARAM("", m_polylineComponent, "PolylineComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_BubblePrizePlatformComponent::Ray_BubblePrizePlatformComponent()
    : Super()
    , m_polylineComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_BubblePrizePlatformComponent::~Ray_BubblePrizePlatformComponent()
{
}

//------------------------------------------------------------------------------
void Ray_BubblePrizePlatformComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    // mandatory validated, we want to detect weight changes
    m_polylineComponent = m_actor->GetComponent<PolylineComponent>();

    // listen to sticked and hanging actors, we want to detect players
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
}

//------------------------------------------------------------------------------
void Ray_BubblePrizePlatformComponent::Update(f32 _dt)
{
    Super::Update(_dt);
}

//------------------------------------------------------------------------------
void Ray_BubblePrizePlatformComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);
    
    // process events
    if (EventStickOnPolyline* stick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
    {
        processStick(stick);
    }
    else if (EventHanging* hang = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)))
    {
        processHang(hang);
    }
}

//------------------------------------------------------------------------------
void Ray_BubblePrizePlatformComponent::processStick(const EventStickOnPolyline* _stick)
{
    // launch (bubble prize) on new sticked player
    if (_stick->isSticked() &&
        canLaunch() &&
        GAMEMANAGER->isPlayerActor(_stick->getActor()))
    {
        // check player hit type: if in crush attack, launch all bubbles
        bbool isCrushAttack = bfalse;
        ActorRef actorRef = _stick->getActor();
        if (Actor* actor = actorRef.getActor())
        {
            Ray_EventQueryHitType query;
            actor->onEvent(&query);
            isCrushAttack = (query.getHitType()==PUNCHTYPE_CRUSH);
        }

        if (isCrushAttack)
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
void Ray_BubblePrizePlatformComponent::processHang(const EventHanging* _hang)
{
    // launch one child (bubble prize) on new hanging player
    if (_hang->isHanging() &&
        canLaunch() &&
        GAMEMANAGER->isPlayerActor(_hang->getActor()))
    {
        launch();
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizePlatformComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_BubblePrizePlatformComponent_Template)
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_BubblePrizePlatformComponent_Template::Ray_BubblePrizePlatformComponent_Template()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_BubblePrizePlatformComponent_Template::~Ray_BubblePrizePlatformComponent_Template()
{
}

} // namespace ITF
