#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FRIENDLYBTAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_FriendlyBTAIComponent.h"
#endif //_ITF_RAY_FRIENDLYBTAICOMPONENT_H_

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_AIBTFACTS_H_
#include "gameplay/ai/BTs/AIBTFacts.h"
#endif //_ITF_AIBTFACTS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

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

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkManager.h"
#endif //_ITF_RAY_AINETWORKMANAGER_H_

#ifndef _ITF_RAY_AIRESCUEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AIRescueManager.h"
#endif //_ITF_RAY_AIRESCUEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_FriendlyBTAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_FriendlyBTAIComponent)

    BEGIN_CONDITION_BLOCK(ESerialize_Checkpoint)

        SERIALIZE_MEMBER("targetWaypoint",m_targetWaypoint);
        SERIALIZE_MEMBER("respawnPoint",m_respawnPoint);
        SERIALIZE_MEMBER("rescued",m_rescued);
        SERIALIZE_MEMBER("rescueFinished",m_rescueFinished);

    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_FriendlyBTAIComponent)

    VALIDATE_COMPONENT_PARAM("Ray_GroundAIControllerComponent", m_aiController != NULL, "Ray_GroundAIControllerComponent is mandatory");

END_VALIDATE_COMPONENT()

Ray_FriendlyBTAIComponent::Ray_FriendlyBTAIComponent()
: m_prevOrder(ITF_INVALID_OBJREF)
, m_usedTeleporter(ITF_INVALID_OBJREF)
, m_aiController(NULL)
, m_retaliateTimer(0.f)
, m_rescued(bfalse)
, m_rescueFinished(bfalse)
, m_registered(bfalse)
, m_aiNetworkCurrentWaypoint(ITF_INVALID_OBJREF)
{
}

Ray_FriendlyBTAIComponent::~Ray_FriendlyBTAIComponent()
{
}

void Ray_FriendlyBTAIComponent::onActorClearComponents()
{
    unregisterObjects();

    AIUtils::setAlwaysActive(m_actor,bfalse);
    RAY_AIRESCUEMANAGER->unregisterActor(m_actor->getRef());
}

void Ray_FriendlyBTAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAIOrderBT,2497541985), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAIBTSetDetectionArea,208226879), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventDisableAIOrderBT,2752846623), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAINetworkWaypoint,3032038608), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerEnterDoor,849805494), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTeleportUsed,75030505), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventDance,83576490), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventActivateStone,2310010701), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventRestoreDeath,3768337963), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventInstantKill,2400145155), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventRescueFriendly,777320120), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryIsRescued,3718040811),this);

    if ( !getTemplate()->getDarktoonPath().isEmpty() )
    {
        m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_darktoonified,true);
        SPAWNER->declareNeedsSpawnee(m_actor, &m_darktoonSpawner, getTemplate()->getDarktoonPath());
    }
    else
    {
        m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_friendSOS,true);
    }

    RAY_AIRESCUEMANAGER->registerActor(m_actor->getRef());
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_faction,RAY_FACTION_FRIENDLY);

    m_aiController = m_actor->GetComponent<Ray_GroundAIControllerComponent>();
}

void Ray_FriendlyBTAIComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    if ( m_targetWaypoint.isValid() )
    {
        Pickable* targetWaypoint = SceneObjectPathUtils::getObjectFromAbsolutePath(m_targetWaypoint);

        if ( targetWaypoint )
        {
            m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_targetWaypoint,targetWaypoint->getRef());
            m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_waypointChanged,true);
        }
    }

    if ( m_rescued )
    {
        m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_darktoonified);
        m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_friendSOS);

        if ( getTemplate()->getDisappearOnRescue() )
        {
            m_actor->disable();
        }
    }
}

void Ray_FriendlyBTAIComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_disabled )
    {
        return;
    }

    if ( m_retaliateTimer )
    {
        m_retaliateTimer = Max(m_retaliateTimer-_dt,0.f);

        if ( !m_retaliateTimer )
        {
            m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_attackTarget);
        }
    }

    if ( !m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_triggerDeath) &&
         !m_physComponent->getDisabled() )
    {
        checkSquash();
    }

    if ( !m_rescued &&
         !m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_darktoonified) &&
         !m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_friendSOS) )
    {
        setRescued();

        RAY_GAMEMANAGER->addFriendlyRescued(m_actor->getRef(),!getTemplate()->getDisappearOnRescue());
    }

    cleanFacts();
}

void Ray_FriendlyBTAIComponent::setRescued()
{
    m_rescued = btrue;

    if ( !getTemplate()->getDisappearOnRescue() )
    {
        AIUtils::setAlwaysActive(m_actor,btrue);
    }
}

void Ray_FriendlyBTAIComponent::cleanFacts()
{
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_hitReceived);

    if ( !m_rescueFinished && m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_waypointReached) )
    {
        RAY_GAMEMANAGER->resetRescuedFriendlyTeleport(m_actor->getRef());
        m_rescueFinished = btrue;
    }
}

void Ray_FriendlyBTAIComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( Ray_EventAIOrderBT* btOrder = _event->DynamicCast<Ray_EventAIOrderBT>(ITF_GET_STRINGID_CRC(Ray_EventAIOrderBT,2497541985)) )
    {
        receiveOrder(btOrder);
    }
    else if ( Ray_EventAIBTSetDetectionArea* btDetectionArea = _event->DynamicCast<Ray_EventAIBTSetDetectionArea>(ITF_GET_STRINGID_CRC(Ray_EventAIBTSetDetectionArea,208226879)) )
    {
        receiveOrderSetDetectionArea(btDetectionArea);
    }
    else if ( Ray_EventDisableAIOrderBT* btDisableOrder = _event->DynamicCast<Ray_EventDisableAIOrderBT>(ITF_GET_STRINGID_CRC(Ray_EventDisableAIOrderBT,2752846623)) )
    {
        receiveDisableOrder(btDisableOrder);
    }
    else if ( Ray_EventAINetworkWaypoint* networkWaypoint = _event->DynamicCast<Ray_EventAINetworkWaypoint>(ITF_GET_STRINGID_CRC(Ray_EventAINetworkWaypoint,3032038608)) )
    {
        receiveWaypoint(networkWaypoint);
    }
    else if ( Ray_EventTriggerChangePage* changePageEvent = _event->DynamicCast<Ray_EventTriggerChangePage>(ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481)) )
    {
        if ( m_usedTeleporter != changePageEvent->getSender() &&
            !m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_usingTeleport) )
        {
            EventPtr eventPtr(changePageEvent);
            m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_teleportEvent,eventPtr);
        }
    }
    else if ( Ray_EventTriggerEnterDoor* enterDoorEvent = _event->DynamicCast<Ray_EventTriggerEnterDoor>(ITF_GET_STRINGID_CRC(Ray_EventTriggerEnterDoor,849805494)) )
    {
        if ( m_usedTeleporter != enterDoorEvent->getSender() &&
            !m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_usingTeleport) )
        {
            EventPtr eventPtr(enterDoorEvent);
            m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_teleportEvent,eventPtr);
        }
    }
    else if ( Ray_EventTeleportUsed* teleportUsed = _event->DynamicCast<Ray_EventTeleportUsed>(ITF_GET_STRINGID_CRC(Ray_EventTeleportUsed,75030505)) )
    {
        receiveTeleportUsed(teleportUsed);
    }
    else if ( Ray_EventDance* danceEvent = _event->DynamicCast<Ray_EventDance>(ITF_GET_STRINGID_CRC(Ray_EventDance,83576490)) )
    {
        receiveDance(danceEvent);
    }
    else if ( Ray_EventActivateStone* activateStoneEvent = _event->DynamicCast<Ray_EventActivateStone>(ITF_GET_STRINGID_CRC(Ray_EventActivateStone,2310010701)) )
    {
        receiveActivateStone(activateStoneEvent);
    }
    else if ( PunchStim* punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
    {
        receivePunch(punch);
    }
    else if ( EventCrushed* crushedEvent = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
    {
        receiveCrush(crushedEvent);
    }
    else if ( Ray_EventRestoreDeath* restoreDeathEvent = _event->DynamicCast<Ray_EventRestoreDeath>(ITF_GET_STRINGID_CRC(Ray_EventRestoreDeath,3768337963)) )
    {
        receiveRestoreDeath(restoreDeathEvent);
    }
    else if ( Ray_EventInstantKill* instantKill = _event->DynamicCast<Ray_EventInstantKill>(ITF_GET_STRINGID_CRC(Ray_EventInstantKill,2400145155)) )
    {
        triggerDeath();
    }
    else if ( Ray_EventRescueFriendly* rescuedEvent = _event->DynamicCast<Ray_EventRescueFriendly>(ITF_GET_STRINGID_CRC(Ray_EventRescueFriendly,777320120)) )
    {
        receiveRescued(rescuedEvent);
    }
    else if ( EventTrigger* _trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
    {
        if ( m_behaviorTree.getBlackBoard().getFact(Ray_AIFact_darktoonified) )
        {
            m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_RunAway,true);
        }
    }
    else if (EventQueryCanStickOrCollide * onQueryCanStick = _event->DynamicCast<EventQueryCanStickOrCollide>(ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404)))
    {
        receiveQueryCanStickOrCollide(onQueryCanStick);
    }
    else if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
    {
        receiveInteractionQuery(query);
    }
    else if ( Ray_EventQueryIsRescued* queryRescued = _event->DynamicCast<Ray_EventQueryIsRescued>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsRescued,3718040811)) )
    {
        queryRescued->setIsRescued(m_rescued);
    }

}

void Ray_FriendlyBTAIComponent::receiveOrder( class Ray_EventAIOrderBT* _order )
{
    ObjectRef currentOrderSender = ITF_INVALID_OBJREF;

    m_behaviorTree.getBlackBoard().getFact(Ray_AIFact_order_sender,currentOrderSender);

    if ( _order->getType() == BTAIORDER_REMOVECURRENTORDER )
    {
        if ( currentOrderSender == _order->getSender() )
        {
            m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_order);
            m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_order_sender);
            m_prevOrder = ITF_INVALID_OBJREF;
        }
    }
    else
    {
        if ( m_prevOrder != _order->getSender() &&
             currentOrderSender != _order->getSender() )
        {
            switch(_order->getType())
            {
                case BTAIORDER_SETTARGETWAYPOINT:
                    receiveOrderSetTargetWaypoint(_order);
                    break;
                case BTAIORDER_SETRESPAWNPOINT:
                    receiveOrderSetRespawnPoint(_order);
                    break;
                default:
                    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_order,static_cast<i32>(_order->getType()));
                    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_order_sender,_order->getSender());
                    break;
            }

            m_prevOrder = _order->getSender();
        }
    }
}

void Ray_FriendlyBTAIComponent::receiveWaypoint( Ray_EventAINetworkWaypoint* _waypoint )
{
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_currentWaypoint,_waypoint->getSender());
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_waypointChanged,true);
    m_aiNetworkCurrentWaypoint = _waypoint->getSender();

    if ( m_registered )
    {
        RAY_AINETWORKMANAGER->updateWaypoint(m_actor->getRef(),m_aiNetworkCurrentWaypoint);
    }
}

void Ray_FriendlyBTAIComponent::receiveOrderSetTargetWaypoint( Ray_EventAIOrderBT* _order )
{
    Actor* sender = AIUtils::getActor(_order->getSender());

    if ( sender )
    {
        LinkComponent* linkComponent = sender->GetComponent<LinkComponent>();

        if ( linkComponent && !linkComponent->getChildren().empty() )
        {
            Pickable* child;
            const ObjectPath& childPath = linkComponent->getChildren()[0].getPath();

            if ( childPath.getIsAbsolute() )
            {
                child = SceneObjectPathUtils::getObjectFromAbsolutePath(childPath);
            }
            else
            {
                child = SceneObjectPathUtils::getObjectFromRelativePath(sender,childPath);
            }

            if ( child )
            {
                m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_targetWaypoint,child->getRef());
                m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_waypointChanged,true);
                m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_waypointReached);

                SceneObjectPathUtils::getAbsolutePathFromObject(child,m_targetWaypoint);
            }
        }
    }
}

void Ray_FriendlyBTAIComponent::receiveOrderSetRespawnPoint( Ray_EventAIOrderBT* _order )
{
    Actor* sender = AIUtils::getActor(_order->getSender());

    if ( sender )
    {
        SceneObjectPathUtils::getAbsolutePathFromObject(sender,m_respawnPoint);
    }
}

void Ray_FriendlyBTAIComponent::receiveOrderSetDetectionArea( Ray_EventAIBTSetDetectionArea* _detectionArea )
{
    if ( _detectionArea->getIsSet() )
    {
        m_behaviorTree.getBlackBoard().setFact(_detectionArea->getId(),static_cast<ObjectRef>(_detectionArea->getArea()));
    }
    else
    {
        m_behaviorTree.getBlackBoard().removeFact(_detectionArea->getId());
    }
}

void Ray_FriendlyBTAIComponent::receiveDisableOrder( Ray_EventDisableAIOrderBT* _order )
{
    ObjectRef currentOrderSender = ITF_INVALID_OBJREF;

    m_behaviorTree.getBlackBoard().getFact(Ray_AIFact_order_sender,currentOrderSender);

    if ( currentOrderSender == _order->getSender() )
    {
        m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_order);
        m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_order_sender);
    }
}

void Ray_FriendlyBTAIComponent::receiveTeleportUsed( Ray_EventTeleportUsed* _teleportUsed )
{
    // If we are in the middle of using a teleport, abort it
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_usingTeleport);

    // Abort any current order since we are being teleported
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_order);
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_order_sender);
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_currentWaypoint);
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_waypointLinkType);
    m_behaviorTree.getBlackBoard().removeFact(AIFact_navTargetActor);
    m_behaviorTree.getBlackBoard().removeFact(AIFact_navTargetPos);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_waypointChanged,true);
    m_prevOrder = ITF_INVALID_OBJREF;

    // Stop dancing if we were
    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_danceOnStone);

    m_aiController->resetJumpToTarget();
    m_usedTeleporter = _teleportUsed->getTeleportTarget();

    Vec3d targetPos = _teleportUsed->getPosition().to3d(m_actor->getPos().m_z);
    Actor* endActor = AIUtils::getActor(_teleportUsed->getTeleportTarget());

    if ( endActor )
    {
        targetPos.m_z = AIUtils::getSceneZFromPickable(endActor);

        if ( m_actor->getTemplate()->isZForced() )
        {
            targetPos.m_z += m_actor->getTemplate()->getForcedZ();
        }
    }

    m_aiNetworkCurrentWaypoint = ITF_INVALID_OBJREF;

    if ( m_registered )
    {
        RAY_AINETWORKMANAGER->updateWaypoint(m_actor->getRef(),m_aiNetworkCurrentWaypoint);
    }

    EventTeleport teleport(targetPos,0.f,btrue);

    m_actor->onEvent(&teleport);
}

void Ray_FriendlyBTAIComponent::receiveDance( Ray_EventDance* _dance )
{
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_danceOnStone,_dance->getSender());
}

void Ray_FriendlyBTAIComponent::receiveActivateStone( Ray_EventActivateStone* _activateStone )
{
    _activateStone->setReceived(btrue);
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_activateStone,_activateStone->getStone());
	m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_activateStoneTrigger,_activateStone->getSender());
}

void Ray_FriendlyBTAIComponent::receivePunch( class PunchStim* _punch )
{
    if ( _punch->getSender() == m_actor->getRef() ||
         m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_danceOnStone) )
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

    if ( m_behaviorTree.getBlackBoard().getFact(Ray_AIFact_darktoonified) )
    {
        releaseDarktoon(_punch);
    }
    else if ( GAMEMANAGER->isPlayerActor(_punch->getSender()) )
    {
        m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_attackTarget,_punch->getSender());
        m_retaliateTimer = getTemplate()->getRetaliationDuration();
    }
    else
    {
        m_retaliateTimer = 0.f;
    }
}

void Ray_FriendlyBTAIComponent::receiveCrush( EventCrushed* _crushed )
{
    if ( m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_darktoonified) )
    {
        releaseDarktoon(_crushed);
    }
}

void Ray_FriendlyBTAIComponent::receiveRestoreDeath( Ray_EventRestoreDeath* _restoreDeath )
{
    if ( m_respawnPoint.isValid() )
    {
        Pickable* respawnPoint = SceneObjectPathUtils::getObjectFromAbsolutePath(m_respawnPoint);

        if ( respawnPoint )
        {
            Vec3d targetPos = respawnPoint->get2DPos().to3d(m_actor->getPos().m_z);

            targetPos.m_z = AIUtils::getSceneZFromPickable(respawnPoint);

            if ( m_actor->getTemplate()->isZForced() )
            {
                targetPos.m_z += m_actor->getTemplate()->getForcedZ();
            }

            EventTeleport teleport(targetPos,0.f,bfalse);

            m_actor->onEvent(&teleport);
        }
    }
}

void Ray_FriendlyBTAIComponent::receiveRescued( Ray_EventRescueFriendly* _rescued )
{
    if ( m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_friendSOS) )
    {
        m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_friendSOS);
        m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_PlayGreet,true);
    }
}

void Ray_FriendlyBTAIComponent::receiveQueryCanStickOrCollide( EventQueryCanStickOrCollide* _queryCanStick )
{
    _queryCanStick->setCanStick(bfalse);
    _queryCanStick->setCanCollide(bfalse);

    ObjectRef actorThatWantsToStickRef = _queryCanStick->getSender();
    Actor* actor = AIUtils::getActor(actorThatWantsToStickRef);

    if ( actor )
    {
        // Using an event here could be a bit overkill, and anyway this code is going to disappear when the pedestal stops being a polyline
        StickToPolylinePhysComponent* physComponent = actor->GetComponent<StickToPolylinePhysComponent>();

        if ( physComponent && physComponent->getStickedEdgeIndex() == U32_INVALID )
        {
            InteractionMask interactions = FACTION_MANAGER->getInteractions(RAY_FACTION_FRIENDLY, AIUtils::getFaction(actor));

            if ( (interactions&INTERACTION_ALLY) != 0 )
            {
                _queryCanStick->setCanStick(btrue);
                _queryCanStick->setCanCollide(btrue);
            }
        }
    }
}

void Ray_FriendlyBTAIComponent::receiveInteractionQuery(EventInteractionQuery * _event)
{
    if ( m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_darktoonified) )
    {
        _event->setInteraction(CharacterInteractionType_Crush);
    }
}

void Ray_FriendlyBTAIComponent::releaseDarktoon( Event* _event )
{
    Actor* darktoon = m_darktoonSpawner.getSpawnee(m_actor->getScene(), m_actor->getPos());
    if (darktoon)
    {
        darktoon->onEvent(_event);
    }

    m_behaviorTree.getBlackBoard().removeFact(Ray_AIFact_darktoonified);
    m_retaliateTimer = 0.f;

    bbool willDisappear = getTemplate()->getDisappearOnRescue();

    if ( willDisappear )
    {
        m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_PlayDisappear,true);
    }

    setRescued();
    m_rescueFinished = btrue;

    RAY_GAMEMANAGER->addFriendlyRescued(m_actor->getRef(),!willDisappear);
}

void Ray_FriendlyBTAIComponent::checkSquash()
{
    f32 radius = m_physComponent->getRadius();
    const f32 penetratingRadius = radius*getTemplate()->getSquashDeathPenetration();
    const PolyLineEdge * crashingEdge = Ray_AIUtils::getSquashingPolyline(m_actor,getPhys()->getStickedPolylineRef(), getPhys()->getStickedEdge(), radius, penetratingRadius);

    if ( crashingEdge )
    {
        triggerDeath();
    }
}

void Ray_FriendlyBTAIComponent::triggerDeath()
{
    m_behaviorTree.getBlackBoard().setFact(Ray_AIFact_triggerDeath,true);
}

void Ray_FriendlyBTAIComponent::updateAnimInput()
{
    Super::updateAnimInput();

    bbool darktoonified = m_behaviorTree.getBlackBoard().factExists(Ray_AIFact_darktoonified);
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
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(darktoonified,2716098321),darktoonified);

    StickToPolylinePhysComponent* stickedPhys = m_physComponent->DynamicCast<StickToPolylinePhysComponent>(ITF_GET_STRINGID_CRC(StickToPolylinePhysComponent,3089159513));

    if ( stickedPhys )
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(inAir,1518932154),stickedPhys->getStickedEdgeIndex()==U32_INVALID);
    }
}

void Ray_FriendlyBTAIComponent::onBecomeActive()
{
    Super::onBecomeActive(  );
    registerObjects();
}

void Ray_FriendlyBTAIComponent::onBecomeInactive()
{
    Super::onBecomeInactive(  );
    unregisterObjects();
}

void Ray_FriendlyBTAIComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
{
    Super::onDepthChanged( _oldZ,_newZ );
    
    if (m_registered)
    {
        AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);
    }
}

void Ray_FriendlyBTAIComponent::registerObjects()
{
    if (!m_registered)
    {
        m_registered = btrue;
        AIData * data = AI_MANAGER->addActor(m_actor->getRef(),m_actor->getDepth());

        if (data)
        {
            data->m_faction = RAY_FACTION_FRIENDLY;
        }

        RAY_AINETWORKMANAGER->registerActor(m_actor->getRef());

        if ( m_aiNetworkCurrentWaypoint != ITF_INVALID_OBJREF )
        {
            RAY_AINETWORKMANAGER->updateWaypoint(m_actor->getRef(),m_aiNetworkCurrentWaypoint);
        }
    }
}

void Ray_FriendlyBTAIComponent::unregisterObjects()
{
    if (m_registered)
    {
        m_registered= bfalse;
        AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
        RAY_AINETWORKMANAGER->unregisterActor(m_actor->getRef());
    }
}
///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_FriendlyBTAIComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_FriendlyBTAIComponent_Template)

    SERIALIZE_MEMBER("retaliationDuration",m_retaliationDuration);
    SERIALIZE_MEMBER("squashDeathPenetration",m_squashDeathPenetration);
    SERIALIZE_MEMBER("darktoonSpawn", m_darktoonPath);
    SERIALIZE_MEMBER("disappearOnRescue", m_disappearOnRescue);

END_SERIALIZATION()

Ray_FriendlyBTAIComponent_Template::Ray_FriendlyBTAIComponent_Template()
: m_retaliationDuration(3.f)
, m_squashDeathPenetration(0.6f)
, m_disappearOnRescue(bfalse)
{
}

Ray_FriendlyBTAIComponent_Template::~Ray_FriendlyBTAIComponent_Template()
{
}

}
