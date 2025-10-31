#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TEENSYROBOTAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TeensyRobotAIComponent.h"
#endif //_ITF_RAY_TEENSYROBOTAICOMPONENT_H_

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_AIBTFACTS_H_
#include "gameplay/ai/BTs/AIBTFacts.h"
#endif //_ITF_AIBTFACTS_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_GroundAIControllerComponent.h"
#endif //_ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkManager.h"
#endif //_ITF_RAY_AINETWORKMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_TeensyRobotAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TeensyRobotAIComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_TeensyRobotAIComponent)

    VALIDATE_COMPONENT_PARAM("Ray_GroundAIControllerComponent", m_aiController != NULL, "Ray_GroundAIControllerComponent is mandatory");

END_VALIDATE_COMPONENT()

Ray_TeensyRobotAIComponent::Ray_TeensyRobotAIComponent()
: m_aiController(NULL)
, m_registered(bfalse)
, m_aiNetworkCurrentWaypoint(ITF_INVALID_OBJREF)
{
}

Ray_TeensyRobotAIComponent::~Ray_TeensyRobotAIComponent()
{
    unregisterObjects();
}

void Ray_TeensyRobotAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAINetworkWaypoint,3032038608), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),this);

    m_aiController = m_actor->GetComponent<Ray_GroundAIControllerComponent>();

    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_faction,RAY_FACTION_ENEMY);
}

void Ray_TeensyRobotAIComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_disabled )
    {
        return;
    }

    cleanFacts();
}

void Ray_TeensyRobotAIComponent::cleanFacts()
{
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_hitReceived);
}

void Ray_TeensyRobotAIComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( Ray_EventAINetworkWaypoint* networkWaypoint = _event->DynamicCast<Ray_EventAINetworkWaypoint>(ITF_GET_STRINGID_CRC(Ray_EventAINetworkWaypoint,3032038608)) )
    {
        receiveWaypoint(networkWaypoint);
    }
    else if ( PunchStim* punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
    {
        receivePunch(punch);
    }
    else if ( EventCrushed* crushedEvent = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
    {
        receiveCrush(crushedEvent);
    }
    else if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
    {
        receiveInteractionQuery(query);
    }

}

void Ray_TeensyRobotAIComponent::receiveWaypoint( Ray_EventAINetworkWaypoint* _waypoint )
{
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_currentWaypoint,_waypoint->getSender());
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_waypointChanged,true);
    m_aiNetworkCurrentWaypoint = _waypoint->getSender();

    if ( m_registered )
    {
        RAY_AINETWORKMANAGER->updateWaypoint(m_actor->getRef(),m_aiNetworkCurrentWaypoint);
    }
}

void Ray_TeensyRobotAIComponent::receivePunch( PunchStim* _punch )
{
    if ( _punch->getSender() == m_actor->getRef() )
    {
        return;
    }

    ObjectRef instigator = _punch->getSender();
    u32 hitLevel = _punch->getLevel();
    Vec2d hitDir = _punch->getDirection();
    EReceivedHitType reactionType = _punch->getReceivedHitType();
    Vec3d fxPos = Vec3d::Zero;

    const PhysContactsContainer& contacts = _punch->getContacts();

    u32 numContacts = contacts.size();

    if (numContacts > 0)
    {
        f32 fnumContacts = 0;
        Vec2d averagePos = Vec2d::Zero;

        for (u32 i = 0; i < numContacts; ++i)
        {
            averagePos += contacts[i].m_contactPoint;
            fnumContacts +=1.0f;
        }

        averagePos = averagePos / fnumContacts;

        f32 depth = m_actor->getDepth();

        fxPos = averagePos.to3d(depth);
    }
    else
    {
        fxPos = _punch->getFxPos();
    }

    if ( reactionType == RECEIVEDHITTYPE_HURTBOUNCE )
    {
        Vec2d bounceDir;

        if ( _punch->getIsRadial() )
        {
            bounceDir = m_actor->get2DPos() - _punch->getPos();
            bounceDir.normalize();
        }
        else
        {
            bounceDir = _punch->getDirection();
        }

        hitDir = bounceDir;
    }

    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_hitReceived,true);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_hitReceivedChanged,true);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_receivedHitInstigator,instigator);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_receivedHitLevel,hitLevel);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_receivedHitFxPos,fxPos);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_receivedHitType,static_cast<u32>(reactionType));
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_receivedHitDir,hitDir);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_receivedHitFeedbackAction,_punch->getActionFromHit());
}

void Ray_TeensyRobotAIComponent::receiveCrush( EventCrushed* _crushed )
{
}

void Ray_TeensyRobotAIComponent::receiveInteractionQuery(EventInteractionQuery * _event)
{
    _event->setInteraction(CharacterInteractionType_Crush);
}

void Ray_TeensyRobotAIComponent::updateAnimInput()
{
    Super::updateAnimInput();

    const Vec2d& controllerMoveInput = m_aiController->getMoveDirThisFrame();
    Vec2d dirX = m_physComponent->getCurrentGravityDir().getPerpendicular();
    Vec2d dirY = -m_physComponent->getCurrentGravityDir();
    EStance stance = m_aiController->isSwiming() ? STANCE_SWIM : STANCE_STAND;

    if ( m_actor->isFlipped() )
    {
        dirX = -dirX;
    }

    Vec2d moveInput;
    
    moveInput.m_x = controllerMoveInput.dot(dirX);
    moveInput.m_y = controllerMoveInput.dot(dirY);

    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(move,2544723026),moveInput.norm());
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(moveX,965019289),moveInput.m_x);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(moveY,951160567),moveInput.m_y);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(isSprinting,4259516180),m_aiController->isSprinting());
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(stance,2021520747),static_cast<u32>(stance));

    StickToPolylinePhysComponent* stickedPhys = m_physComponent->DynamicCast<StickToPolylinePhysComponent>(ITF_GET_STRINGID_CRC(StickToPolylinePhysComponent,3089159513));

    if ( stickedPhys )
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(inAir,1518932154),stickedPhys->getStickedEdgeIndex()==U32_INVALID);
    }
}

void Ray_TeensyRobotAIComponent::onBecomeActive()
{
    Super::onBecomeActive(  );
    registerObjects();
}

void Ray_TeensyRobotAIComponent::onBecomeInactive()
{
    Super::onBecomeInactive(  );
    unregisterObjects();
}

void Ray_TeensyRobotAIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
{
    Super::onDepthChanged( _oldZ,_newZ );
    
    if (m_registered)
    {
        AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);
    }
}

void Ray_TeensyRobotAIComponent::registerObjects()
{
    if (!m_registered)
    {
        m_registered = btrue;
        AIData * data = AI_MANAGER->addActor(m_actor->getRef(),m_actor->getDepth());

        if (data)
        {
            data->m_faction = RAY_FACTION_ENEMY;
        }

        RAY_AINETWORKMANAGER->registerActor(m_actor->getRef());

        if ( m_aiNetworkCurrentWaypoint != ITF_INVALID_OBJREF )
        {
            RAY_AINETWORKMANAGER->updateWaypoint(m_actor->getRef(),m_aiNetworkCurrentWaypoint);
        }
    }
}

void Ray_TeensyRobotAIComponent::unregisterObjects()
{
    if (m_registered)
    {
        m_registered= bfalse;
        AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
        RAY_AINETWORKMANAGER->unregisterActor(m_actor->getRef());
    }
}
///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_TeensyRobotAIComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_TeensyRobotAIComponent_Template)

END_SERIALIZATION()

Ray_TeensyRobotAIComponent_Template::Ray_TeensyRobotAIComponent_Template()
{
}

Ray_TeensyRobotAIComponent_Template::~Ray_TeensyRobotAIComponent_Template()
{
}

}
