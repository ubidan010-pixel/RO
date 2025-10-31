#include "precompiled_gameplay_rayman.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_RAY_AICHEST2BEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Chest2/Ray_AIChest2Behavior.h"
#endif //_ITF_RAY_AICHEST2BEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef ITF_CONSOLE_FINAL
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_DUMMYCOMPONENT_H_
#include "engine/actors/Components/DummyComponent.h"
#endif //_ITF_DUMMYCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_RAY_AIRELICBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Relic/Ray_AIRelicBehavior.h"
#endif //_ITF_RAY_AIRELICBEHAVIOR_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_TRAJECTORYFOLLOWERCOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryFollowerComponent.h"
#endif //_ITF_TRAJECTORYFOLLOWERCOMPONENT_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{

//------------------------------------------------------------------------------
Chest2LinkInfo::Chest2LinkInfo(const TrajectoryLinkInfo& _other)
    : TrajectoryLinkInfo(_other)
    , m_ignoreCamera(bfalse)
    , m_takeCamera(bfalse)
    , m_changeScale(bfalse)
    , m_breakable(bfalse)
    , m_invincible(bfalse)
    , m_safeDistance(-1.0f)
    , m_minSpeed(-1.0f)
    , m_maxSpeed(-1.0f)
{
    // NB[LaurentCou]: use *bool* (not *bbool*) to use the
    // proper specialization for getTagValue, ie. the tag has
    // to be 'true' or 'false'
    bool ignoreCamera; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(ignoreCamera,2302192342), ignoreCamera))
    {
        m_ignoreCamera = ignoreCamera;
    }

    // NB[LaurentCou]: ibid.
    bool takeCamera; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(takeCamera,4213974334), takeCamera))
    {
        m_takeCamera = takeCamera;
    }

    // NB[LaurentCou]: ibid.
    bool changeScale; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(changeScale,3165851357), changeScale))
    {
        m_changeScale = changeScale;
    }

    // NB[LaurentCou]: ibid.
    bool breakable; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(breakable,2364267816), breakable))
    {
        m_breakable = breakable;
    }

    // NB[LaurentCou]: ibid.
    bool invincible; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(invincible,3379187663), invincible))
    {
        m_invincible = invincible;
    }

    f32 safeDistance; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(safeDistance,409521617), safeDistance))
    {
        m_safeDistance = safeDistance;
    }

    f32 minSpeed; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(minSpeed,2209343178), minSpeed))
    {
        m_minSpeed = minSpeed;
    }

    f32 maxSpeed; 
    if (getChildEntry().getTagValue(ITF_GET_STRINGID_CRC(maxSpeed,1281740165), maxSpeed))
    {
        m_maxSpeed = maxSpeed;
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIChest2Behavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AIChest2Behavior)
    VALIDATE_BEHAVIOR_PARAM("minSpeed,maxSpeed", getTemplate()->getMinSpeed()<getTemplate()->getMaxSpeed(), "Valid interval mandatory");
    VALIDATE_BEHAVIOR_PARAM("swimMinSpeed,swimMaxSpeed", getTemplate()->getSwimMinSpeed()<getTemplate()->getSwimMaxSpeed(), "Valid interval mandatory");
    VALIDATE_BEHAVIOR_PARAM("safeDistance", getTemplate()->getSafeDistance()>0.0f, "Strictly positive distance mandatory");
    VALIDATE_BEHAVIOR_PARAM("eyeMinX", getTemplate()->getEyeMinX()>=0, "eyeMinX must be in 0..1");
    VALIDATE_BEHAVIOR_PARAM("eyeMaxX", getTemplate()->getEyeMaxX()<=1, "eyeMaxX must be in 0..1");
    VALIDATE_BEHAVIOR_PARAM("eyeMinY", getTemplate()->getEyeMinY()>=0, "eyeMinY must be in 0..1");
    VALIDATE_BEHAVIOR_PARAM("eyeMaxY", getTemplate()->getEyeMaxY()<=1, "eyeMaxY must be in 0..1");
    VALIDATE_BEHAVIOR_PARAM("standAction", m_standAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("runAction", m_runAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("jumpAction", m_jumpAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("toFlyAction", m_toFlyAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("flyAction", m_flyAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("landAction", m_landAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("skidAction", m_skidAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("waitAction", m_waitAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("tauntAction", m_tauntAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("diveAction", m_diveAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("swimAction", m_swimAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("thinkDetectAction", m_thinkDetectAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("thinkHideAction", m_thinkHideAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("breaksAction", m_breaksAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("brokenAction", m_brokenAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_trajectoryFollowerComponent, "TrajectoryFollowerComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_linkComponent, "LinkComponent mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AIChest2Behavior::Ray_AIChest2Behavior()
    : Super()
    , m_activated(bfalse)
    , m_breakable(bfalse)
    , m_invincible(bfalse)
    , m_thinking(bfalse)
    , m_upsideDownJump(bfalse)
    , m_state(State_NA)
    , m_minDot(0.0f)
    , m_targetSpeed(0.0f)
    , m_skidStartSpeed(0.0f)
    , m_tauntFlag(bfalse)
    , m_tauntStopCount(0)
    , m_safeDistance(0.0f)
    , m_changingScale(bfalse)
    , m_atReducedScale(bfalse)
    , m_closestPlayerRef()
    , m_breakFxGenerator()
    , m_relicGenerator()
    , m_breakEvents()
    , m_initialPreThinkNode()
    , m_preThinkNode()
    , m_thinkNode()
    , m_thinkNodeType(ThinkNode_Invalid)
    , m_hideCountdown(0)
    , m_detectCountdown(0.0f)
    , m_reverseCountdown(0.0f)
    , m_standAction(NULL)
    , m_runAction(NULL)
    , m_jumpAction(NULL)
    , m_toFlyAction(NULL)
    , m_flyAction(NULL)
    , m_landAction(NULL)
    , m_skidAction(NULL)
    , m_waitAction(NULL)
    , m_tauntAction(NULL)
    , m_diveAction(NULL)
    , m_swimAction(NULL)
    , m_thinkDetectAction(NULL)
    , m_thinkHideAction(NULL)
    , m_breaksAction(NULL)
    , m_brokenAction(NULL)
    , m_trajectoryFollowerComponent(NULL)
    , m_linkComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIChest2Behavior::~Ray_AIChest2Behavior()
{
    // unregister from camera if needed
    if (-1 != CAMERACONTROLLERMANAGER->getSubjectIndex(m_actor->getRef()))
    {
        CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());
    }
    
    // delete instance break events
    u32 breakEventsCount = m_breakEvents.size();
    for (u32 i=0; i<breakEventsCount; ++i)
    {
        SF_DEL(m_breakEvents[i]);
    }
    m_breakEvents.clear();
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    // create actions
    m_standAction = createAiAction(getTemplate()->getStandAction());
    m_runAction = createAiAction(getTemplate()->getRunAction());
    m_jumpAction = createAiAction(getTemplate()->getJumpAction());
    m_toFlyAction = createAiAction(getTemplate()->getToFlyAction());
    m_flyAction = createAiAction(getTemplate()->getFlyAction());
    m_landAction = createAiAction(getTemplate()->getLandAction());
    m_skidAction = createAiAction(getTemplate()->getSkidAction());
    m_waitAction = createAiAction(getTemplate()->getWaitAction());
    m_tauntAction = createAiAction(getTemplate()->getTauntAction());
    m_diveAction = createAiAction(getTemplate()->getDiveAction());
    m_swimAction = createAiAction(getTemplate()->getSwimAction());
    m_thinkDetectAction = createAiAction(getTemplate()->getThinkDetectAction());
    m_thinkHideAction = createAiAction(getTemplate()->getThinkHideAction());
    m_breaksAction = createAiAction(getTemplate()->getBreaksAction());
    m_brokenAction = createAiAction(getTemplate()->getBrokenAction());

    Super::onActorLoaded(_hotReload);

    m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    
    // declare spawnees
    if (!getTemplate()->getBreakFx().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_breakFxGenerator, getTemplate()->getBreakFx());
    }
    if (!getTemplate()->getRelicActor().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_relicGenerator, getTemplate()->getRelicActor());
    }

    // clone template break events
    const SafeArray<Event*>& templateBreakEvents = getTemplate()->getBreakEvents();
    u32 breakEventsCount = templateBreakEvents.size();
    m_breakEvents.reserve(breakEventsCount);
    for (u32 i=0; i<breakEventsCount; ++i)
    {
        Event* breakEvent = templateBreakEvents[i]->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
        BinaryClone(const_cast<Event*>(templateBreakEvents[i]), breakEvent);
        m_breakEvents.push_back(breakEvent);
    }

    // mandatory, validated
    m_trajectoryFollowerComponent = m_actor->GetComponent<TrajectoryFollowerComponent>();
    m_linkComponent= m_actor->GetComponent<LinkComponent>();
    
    // register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::onActivate()
{
    //Super::onActivate();

    // start up
    m_activated = bfalse;
    changeState(State_Stand);
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::onActorBecomeInactive()
{
    Super::onActorBecomeInactive();

    // unregister from camera if needed
    if (-1 != CAMERACONTROLLERMANAGER->getSubjectIndex(m_actor->getRef()))
    {
        CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::onDeactivate()
{
    Super::onDeactivate();

    // unregister from camera if needed
    if (-1 != CAMERACONTROLLERMANAGER->getSubjectIndex(m_actor->getRef()))
    {
        CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::update(f32 _dt)
{
    Super::update(_dt);

    // get info from trajectory follower component
    // + get our custom tags
    Chest2LinkInfo linkInfo(m_trajectoryFollowerComponent->getLinkInfo());

    // get link cursor
    f32 linkCursor = m_trajectoryFollowerComponent->getLinkCursor();

    updateClosestPlayerRef();
    updateCamera(linkInfo);
    updateEyePos(m_closestPlayerRef.getActor());
    updateSafeDistance(linkInfo);
    updateScale(linkInfo, linkCursor);
    updateState(linkInfo, linkCursor, _dt);
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::onEvent(Event* _event)
{
    Super::onEvent(_event);

    if (EventTrigger* triggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        processTrigger(triggerEvt);
    }
    else if (HitStim* hitStim = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
    {
        processHit(hitStim);
    }
    else if (EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)))
    {
        processInteractionQuery(query);
    }
    else if (EventCrushed* eventCrushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)))
    {
        processCrushed(eventCrushed);
    }
    else if (AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
    {
        processAnimEvent(animEvent);
    }            
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::updateAnimInputs()
{
    Super::updateAnimInputs();
    
    if (!m_animatedComponent)
    {
        return;
    }

    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(TargetSpeed,977456213), m_targetSpeed);

    // update anim inputs of the TrajectoryFollowerComponent
    // NB[LaurentCou]: only one component can update anim input at a time,
    // so as we also have a TrajectoryFollowerComponent, we have to call its
    // updateAnimInput from here
    m_trajectoryFollowerComponent->updateAnimInput();
}


//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::onActionFinished()
{
    Super::onActionFinished();

    // breaks -> broken
    if (m_state == State_Breaks)
    {
        changeState(State_Broken);
    }
}

//------------------------------------------------------------------------------
bbool Ray_AIChest2Behavior::isBreakable() const
{
    return m_breakable &&
        !m_invincible &&
        m_state != State_Breaks &&
        m_state != State_Broken;
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::processTrigger(const EventTrigger* _triggerEvt)
{
    if (!m_activated)
    {
        m_activated = btrue;
    }
    else
    {
        ++m_tauntStopCount;
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::processHit(const HitStim* _hitStim)
{
    if (isBreakable())
    {
        changeState(State_Breaks);
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::processInteractionQuery(EventInteractionQuery* _query)
{
    if (isBreakable())
    {
        _query->setInteraction(CharacterInteractionType_Crush);
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::processCrushed(const EventCrushed* _eventCrushed)
{
    if (isBreakable())
    {
        changeState(State_Breaks);
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::processAnimEvent(const AnimGameplayEvent* _animEvent)
{
    if (_animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_SpawnBreakFX,1425716077))
    {
        spawnBreakFx();
    }
    else if ( _animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Smash_opens,3159982031) )
    {
        // spawn relic and/or send break event
        if (m_state == State_Breaks)
        {
            spawnRelic();
            sendBreakEvents();
        }
    }
}
    
//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::spawnBreakFx()
{
    // nothing to spawn? skip.
    if (getTemplate()->getBreakFx().isEmpty())
    {
        return;
    }

    m_breakFxGenerator.getSpawnee(m_actor->getScene(), m_actor->getPos());
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::spawnRelic()
{
    // nothing to spawn? skip.
    if (getTemplate()->getRelicActor().isEmpty())
    {
        return;
    }
    
    // spawn relic
    Vec2d center(m_actor->getAABB().getCenter());
    Vec3d spawnPos(center.m_x, center.m_y, m_actor->getPos().m_z);
    Actor* actor = m_relicGenerator.getSpawnee(m_actor->getScene(), spawnPos);
    bbool isOk=bfalse;
    if (actor)
    {
        Ray_FixedAIComponent* fixedAI = actor->GetComponent<Ray_FixedAIComponent>();
        if (fixedAI)
        {
            Ray_AIRelicBehavior* behavior = fixedAI->getCurrentBehavior()->DynamicCast<Ray_AIRelicBehavior>(ITF_GET_STRINGID_CRC(Ray_AIRelicBehavior,573866982));
            if (behavior)
            {
                behavior->setRelicIndex(getTemplate()->getRelicIndex());
                actor->setWorldInitialPos(spawnPos);
                behavior->setIsSpawnedByChest(btrue);
                behavior->resetState();
                isOk = btrue;
            }
        }
    }
    ITF_WARNING(
        m_actor, isOk,
        "%s not declared as spawnable or is incorrect. Please use "
        "relic*Spawner actor",
        getTemplate()->getRelicActor().getString8().cStr()
        );
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::sendBreakEvents()
{
    // no break event? skip.
    if (m_breakEvents.size() == 0)
    {
        if (getTemplate()->getDisableAfterBreakEvents())
        {
            m_actor->disable();
        }
        return;
    }
    
    // send break events
    u32 breakEventsCount = m_breakEvents.size();
    for (u32 i=0; i<breakEventsCount; ++i)
    {
        // send sequence control event...
        if (EventSequenceControl* sequenceControl = m_breakEvents[i]->DynamicCast<EventSequenceControl>(ITF_GET_STRINGID_CRC(EventSequenceControl,1151213732)))
        {
            // ... to a child sequence actor, if there's one
            Actor* sequencePlayer = NULL;
            SequencePlayerComponent* sequencePlayerComponent = NULL;
            AIUtils::LinkIterator iterator(m_linkComponent, btrue);
            while (Actor* child = iterator.getNextRelativeActor())
            {
                if (SequencePlayerComponent* spc = child->GetComponent<SequencePlayerComponent>())
                {
                    sequencePlayer = child;
                    sequencePlayerComponent = spc;
                    break;
                }
            }

            // ... otherwise try the actor linked to the last node (legacy)
            if (!sequencePlayer)
            {
                ITF_ASSERT(!m_trajectoryFollowerComponent->hasNextNode());
                ActorRef lastNodeRef = m_trajectoryFollowerComponent->getCurrentNode();
                if (Actor* lastNode = lastNodeRef.getActor())
                {
                    LinkComponent* lastNodeLinkComponent = lastNode->GetComponent<LinkComponent>();
                    ITF_ASSERT(lastNodeLinkComponent);
                    if (lastNodeLinkComponent)
                    {
                        AIUtils::LinkIterator iterator(lastNodeLinkComponent, btrue);
                        while (Actor* lastNodeChild = iterator.getNextRelativeActor())
                        {
                            if (SequencePlayerComponent* spc = lastNodeChild->GetComponent<SequencePlayerComponent>())
                            {
                                sequencePlayer = lastNodeChild;
                                sequencePlayerComponent = spc;
                                break;
                            }
                        }
                    }
                }
            }

            // actually send event, if sequence player found
            ITF_WARNING(m_actor, sequencePlayer && sequencePlayerComponent, "Sequence player not found");
            if (sequencePlayer && sequencePlayerComponent)
            {
                // snap sequence player on self if required (some Darktoon Cages
                // can be killed while moving, so that's to make sure the
                // sequence doesn't glitch - RO-12814)
                if (getTemplate()->getSnapBreakSequencePlayer())
                {
                    Vec3d anchor = m_actor->getPos();
                    anchor.m_z = sequencePlayer->getDepth();
                    sequencePlayerComponent->setAnchorPos(anchor);
                }

                sequencePlayer->onEvent(sequenceControl);
            }
        }
        // send power up event to all players
        else if (Ray_EventPowerUp* powerUp = m_breakEvents[i]->DynamicCast<Ray_EventPowerUp>(ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764)))
        {
            ActorList players;
            AIUtils::getPlayers(players);
            u32 playersCount = players.size();
            for (u32 i=0; i<playersCount; ++i)
            {
                players[i]->onEvent(powerUp);
            }
            // achievements    
            if(powerUp->getId()==Ray_PowerUp_Reduction)
            {
                REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Action_PowerUp_Reduction);
            } 
            else if(powerUp->getId()==Ray_PowerUp_WalkOnWallsGlobal)
            {
                REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Action_PowerUp_WalkOnWallsGlobal);
            }
            else if(powerUp->getId()==Ray_PowerUp_Dive)
            {
                REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Action_Powerup_Dive);
            }
            else if(powerUp->getId()==Ray_PowerUp_Helicopter)
            {
                REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Action_PowerUp_Helicopter);
            }
            else if(powerUp->getId()==Ray_PowerUp_Fight)
            {
                REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Action_PowerUp_Fight);
            }
        }
        // send take tooth event to the game manager
        else if (Ray_EventTakeTooth* takeTooth = m_breakEvents[i]->DynamicCast<Ray_EventTakeTooth>(ITF_GET_STRINGID_CRC(Ray_EventTakeTooth,2760419748)))
        {
            takeTooth->setSender(m_actor->getRef());
            RAY_GAMEMANAGER->onEvent(takeTooth);
        }
        // not implemented: who should we send this event to?
        else
        {
            ITF_WARNING(m_actor, bfalse, "Unhandled event type");
        }
    }

    // if required, disable actor now
    if (getTemplate()->getDisableAfterBreakEvents())
    {
        m_actor->disable();
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::updateClosestPlayerRef()
{
    // store player ref for debug draw
    if (Actor* closestPlayer = AIUtils::getClosestPursuer(m_actor, m_minDot))
    {
        m_closestPlayerRef = closestPlayer->getRef();
    }
    else
    {
        m_closestPlayerRef.invalidate();
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::updateCamera(const Chest2LinkInfo& _linkInfo)
{
    // register to camera
    if (!_linkInfo.getIgnoreCamera() &&
        !m_breakable &&
        (m_state == State_Run   ||
        m_state == State_Jump   ||
        m_state == State_Dive   ||
        m_state == State_Swim   ||
        m_state == State_ToFly  ||
        m_state == State_Fly    ||
        m_state == State_Land   ||
        _linkInfo.getTakeCamera()))
    {
        if (-1 == CAMERACONTROLLERMANAGER->getSubjectIndex(m_actor->getRef()))
        {
            CAMERACONTROLLERMANAGER->registerSubject(
                m_actor->getRef(),  // ref to ourselves
                1.0f,               // delay
                bfalse,             // not a player
                bfalse,             // no teleport
                btrue               // leads camera
                );
        }
    }
    // unregister from camera
    else
    {
        if (-1 != CAMERACONTROLLERMANAGER->getSubjectIndex(m_actor->getRef()))
        {
            CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());
        }
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::computeEyeFromTargetPosition(const Vec2d &_pos, f32 _radiusScale, f32 &_animInterpX, f32 &_animInterpY)
{
    //radius of circle on which the eye goes (in animation space)
    f32 eyeRadiusX = (getTemplate()->getEyeMaxX()-getTemplate()->getEyeMinX())*0.5f;//*radiusScale;
    f32 eyeRadiusY = (getTemplate()->getEyeMaxY()-getTemplate()->getEyeMinY())*0.5f;//*radiusScale;

    Vec2d unrotatedEyeRestPos(getTemplate()->getRelativeNeutralEyePos().m_x*m_actor->getScale().m_x, getTemplate()->getRelativeNeutralEyePos().m_y*m_actor->getScale().m_y);
    Vec2d rotatedEyeRestPos = unrotatedEyeRestPos.Rotate(m_actor->getAngle())+m_actor->get2DPos();
#if defined(_DEBUG) && defined(_DEBUGRELICCHEST)
    DebugDraw::circle(Vec3d(rotatedEyeRestPos.m_x, rotatedEyeRestPos.m_y, m_actor->getDepth()), 0.1f, Color::red(), "eye");
    DebugDraw::circle(Vec3d(_pos.m_x, _pos.m_y, m_actor->getDepth()), 0.1f, Color::yellow(), "target");
#endif
    Vec2d eyeToTargetWorldSpace = _pos - rotatedEyeRestPos;
    Vec2d eyeToTargetModelSpace = eyeToTargetWorldSpace.Rotate(-m_actor->getAngle());

    Vec2d eyeToTargetAnimSpace = eyeToTargetModelSpace * getTemplate()->getDistanceToEyeFactor();
    eyeToTargetAnimSpace.m_y = -eyeToTargetAnimSpace.m_y;
    //normalize on ellipsis
    eyeToTargetAnimSpace.m_x /= eyeRadiusX;
    eyeToTargetAnimSpace.m_y /= eyeRadiusY;
    f32 eyeDist = eyeToTargetAnimSpace.norm();
    if (eyeDist>1)
    {
        eyeToTargetAnimSpace *= 1.0f/eyeDist;
    }
    eyeToTargetAnimSpace.m_x *= eyeRadiusX * _radiusScale;
    eyeToTargetAnimSpace.m_y *= eyeRadiusY * _radiusScale;

    eyeToTargetAnimSpace += Vec2d((getTemplate()->getEyeMaxX()+getTemplate()->getEyeMinX())*0.5f, (getTemplate()->getEyeMaxY()+getTemplate()->getEyeMinY())*0.5f);
    _animInterpX = eyeToTargetAnimSpace.m_x;
    _animInterpY = eyeToTargetAnimSpace.m_y;
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::updateEyePos(Actor *_player)
{
    if (_player)
    {
        f32 animInterpX, animInterpY;
        Vec2d headPos = _player->get2DPos() + Vec2d(0,getTemplate()->getTypicalPlayerHeadY());

        const StringID eyeLR_Id("EyeLeftToRight");
        const StringID eyeUD_Id("EyeUpToDown");
        u32 eyeLRIndex = m_animatedComponent->findInputIndex(eyeLR_Id);
        u32 eyeUDIndex = m_animatedComponent->findInputIndex(eyeUD_Id);
        if (eyeLRIndex != U32_INVALID && eyeUDIndex != U32_INVALID)
        {
            f32 oldAnimInterpX=0.5f, oldAnimInterpY=0.5f;
            const f32 interpFactor(0.5f);
            m_animatedComponent->getInputValue(eyeLRIndex, oldAnimInterpX);
            m_animatedComponent->getInputValue(eyeUDIndex, oldAnimInterpY);
            computeEyeFromTargetPosition(headPos, 1.0f, animInterpX, animInterpY);
            animInterpX = oldAnimInterpX+(animInterpX-oldAnimInterpX)*interpFactor;
            animInterpY = oldAnimInterpY+(animInterpY-oldAnimInterpY)*interpFactor;
            m_animatedComponent->setInput(eyeLR_Id, animInterpX);
            m_animatedComponent->setInput(eyeUD_Id, animInterpY);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::changeState(State_t _newState)
{
    // skip if already in given state
    if (m_state == _newState)
    {
        return;
    }

    // clean previous state
    switch (m_state)
    {
    case State_Skid:
    {
        m_skidStartSpeed = 0.0f;
    }
    break;
    case State_Jump:
    {
        // restart at min speed after upside down jump
        if (m_upsideDownJump)
        {
            m_targetSpeed = getTemplate()->getMinSpeed();
            m_upsideDownJump = bfalse;
        }
    }
    break;
    }

    // start new state
    switch (m_state = _newState)
    {
    case State_NA: break;
    case State_Stand:
    {
        setAction(m_standAction);
        m_trajectoryFollowerComponent->setIsWaiting(btrue);
    }
    break;
    case State_Run:
    {
        ITF_ASSERT(m_activated);
        setAction(m_runAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_Jump:
    {
        ITF_ASSERT(m_activated);
        setAction(m_jumpAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_ToFly:
    {
        ITF_ASSERT(m_activated);
        setAction(m_toFlyAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_Fly:
    {
        ITF_ASSERT(m_activated);
        setAction(m_flyAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_Land:
    {
        ITF_ASSERT(m_activated);
        setAction(m_landAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_Skid:
    {
        ITF_ASSERT(m_activated);
        m_skidStartSpeed = m_trajectoryFollowerComponent->getSpeed();
        setAction(m_skidAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_Wait:
    {
        ITF_ASSERT(m_activated);
        setAction(m_waitAction);
        m_trajectoryFollowerComponent->setIsWaiting(btrue);
    }
    break;
    case State_Taunt:
    {
        ITF_ASSERT(m_activated);
        setAction(m_tauntAction);
        m_trajectoryFollowerComponent->setIsWaiting(btrue);
    }
    break;
    case State_Dive:
    {
        ITF_ASSERT(m_activated);
        setAction(m_diveAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_Swim:
    {
        ITF_ASSERT(m_activated);
        setAction(m_swimAction);
        m_trajectoryFollowerComponent->setIsWaiting(bfalse);
    }
    break;
    case State_ThinkDetect:
    {
        ITF_ASSERT(m_activated);
        setAction(m_thinkDetectAction);
    }
    break;
    case State_ThinkHide:
    {
        ITF_ASSERT(m_activated);
        setAction(m_thinkHideAction);
    }
    break;
    case State_Breaks:
    {
        ITF_ASSERT(m_activated);
        setAction(m_breaksAction);
        m_trajectoryFollowerComponent->setIsWaiting(btrue);
        
        if (getTemplate()->getSendBreakEventsDirectly())
        {
            sendBreakEvents();
        }
    }
    break;
    case State_Broken:
    {
        ITF_ASSERT(m_activated);
        setAction(m_brokenAction);
        m_trajectoryFollowerComponent->setIsWaiting(btrue);
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::computeTargetSpeed(const Chest2LinkInfo& _linkInfo)
{
    // speed on curve depends on player distance
    f32 distanceFactor = m_minDot / m_safeDistance;
    distanceFactor = Clamp(1.0f - distanceFactor, 0.0f, 1.0f);
    
    // retrieve minimum speed
    f32 minSpeed;
    if (_linkInfo.getMinSpeed() >= 0.0f)
    {
        minSpeed = _linkInfo.getMinSpeed();
    }
    else if (m_breakable)
    {
        minSpeed = getTemplate()->getBreakableSpeed();
    }
    else if (_linkInfo.getType() == TrajectoryLinkType_Swim)
    {
        minSpeed = getTemplate()->getSwimMinSpeed();
    }
    else
    {
        minSpeed = getTemplate()->getMinSpeed();
    }
    
    // retrieve maximum speed
    f32 maxSpeed;
    if (_linkInfo.getMaxSpeed() >= 0.0f)
    {
        maxSpeed = _linkInfo.getMaxSpeed();
    }
    else if (m_breakable)
    {
        maxSpeed = getTemplate()->getBreakableSpeed();
    }
    else if (_linkInfo.getType() == TrajectoryLinkType_Swim)
    {
        maxSpeed = getTemplate()->getSwimMaxSpeed();
    }
    else
    {
        maxSpeed = getTemplate()->getMaxSpeed();
    }

    // target speed is lerped on player distance    
    m_targetSpeed = Interpolate(minSpeed, maxSpeed, distanceFactor);
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::updateSafeDistance(const Chest2LinkInfo& _linkInfo)
{
    if (_linkInfo.getSafeDistance() != -1)
    {
        m_safeDistance = _linkInfo.getSafeDistance();
    }
    else 
    {
        m_safeDistance = getTemplate()->getSafeDistance();
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::updateScale(const Chest2LinkInfo& _linkInfo, f32 _linkCursor)
{
    f32 scale = 1.0f;

    if (_linkInfo.getChangeScale())
    {
        // remember if we're changing scale for next frame
        m_changingScale = btrue;
        
        if (m_atReducedScale)
        {
            // was reduced, scale up
            scale = Interpolate(getTemplate()->getReducedScale(), 1.0f, _linkCursor);
        }
        else
        {
            // wasn't reduced, scale down
            scale = Interpolate(1.0f, getTemplate()->getReducedScale(), _linkCursor);
        }
    }
    else
    {
        // if was changing scale previously, we're done, flip the scale flag
        // NB[Laurent]: not safe e.g. for super-short links that the chest could
        // overtake in one frame, hopefully this won't happen; if it does, move
        // this logic up to the node component
        if (m_changingScale)
        {
            m_changingScale = bfalse;
            m_atReducedScale = !m_atReducedScale;
        }
        
        if (m_atReducedScale)
        {
            // reduced
            scale = getTemplate()->getReducedScale();
        }
        else
        {
            // fully scaled
            scale = 1.0f;
        }
    }
    
    // re-apply forced scale, and scale actor accordingly
    Vec2d scaleVec(scale, scale);
    if (m_actor->getTemplate()->getScaleForced() != Vec2d::Zero)
    {
        scaleVec *= m_actor->getTemplate()->getScaleForced(); 
    }
    m_actor->setScale(scaleVec);
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::updateState(const Chest2LinkInfo& _linkInfo, f32 _linkCursor, f32 _dt)
{
    // should the speed get smoothed?
    bbool smoothSpeed = btrue;
    
    // adapt animation to link type
    if (m_activated)
    {
        if (!m_trajectoryFollowerComponent->hasNextNode())
        {
            if (m_state != State_ThinkDetect &&
                m_state != State_ThinkHide &&
                m_state != State_Breaks &&
                m_state != State_Broken)
            {
                if (m_thinking)
                {
                    // already thought, this means we have to think again
                    startThink(); 
                }
                else
                {
                    // not breakable previously, just wait for an upcoming death
                    changeState(State_Wait);
                    m_breakable = btrue;
                }
            }
        }
        else if (_linkInfo.getBreakable() && !m_breakable)
        {
            // start being breakable on link tag
            m_breakable = btrue;

            ActorRef currentNode = m_trajectoryFollowerComponent->getCurrentNode();
            if (getThinkNodeType(currentNode) != ThinkNode_Invalid)
            {
                // remember previous node, we're not going back there
                m_initialPreThinkNode = m_trajectoryFollowerComponent->getPreviousNode();

                // think now
                startThink();
            }
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_Walk)
        {
            // to run action
            changeState(State_Run);
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_SkidAndWait)
        {
            // to skid action if player's far away and not already waiting
            if (m_minDot > m_safeDistance)
            {
                changeState(State_Skid);
            }
            else
            {
                changeState(State_Run);
            }
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_SkidAndTaunt)
        {
            // to skid action if trigger not passed
            if (m_tauntStopCount == 0)
            {
                changeState(State_Skid);
            }
            else
            {
                changeState(State_Run);
            }
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_Jump ||
            _linkInfo.getType() == TrajectoryLinkType_ToCeiling ||
            _linkInfo.getType() == TrajectoryLinkType_ToFloor)
        {
            // to jump action
            changeState(State_Jump);
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_ToFly)
        {
            // to to-fly action
            changeState(State_ToFly);
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_Fly)
        {
            // to fly action
            changeState(State_Fly);
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_Land)
        {
            // to land action
            changeState(State_Land);
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_Dive)
        {
            // to dive action
            changeState(State_Dive);
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_Swim)
        {
            // to swim action
            changeState(State_Swim);
        }
    }

    // target speed from current state
    switch (m_state)
    {
    case State_Stand:
    {
        ITF_ASSERT(isCurrentAction(m_standAction));
        m_targetSpeed = 0.0f;
    }
    break;
    case State_Run:
    {
        ITF_ASSERT(isCurrentAction(m_runAction));
        computeTargetSpeed(_linkInfo);

        // check if should reverse move, if so do it
        checkReverse(_dt);
    }
    break;
    case State_Jump:
    {
        ITF_ASSERT(isCurrentAction(m_jumpAction));
        if (_linkInfo.getType() == TrajectoryLinkType_Jump)
        {
            computeTargetSpeed(_linkInfo);
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_ToCeiling)
        {
            m_targetSpeed = Interpolate(
                getTemplate()->getUpsideDownFloorSpeed(),
                getTemplate()->getUpsideDownCeilingSpeed(),
                _linkCursor * _linkCursor);
            smoothSpeed = bfalse;
            m_upsideDownJump = btrue;
        }
        else if (_linkInfo.getType() == TrajectoryLinkType_ToFloor)
        {
            f32 invCursor = 1.0f - _linkCursor;
            m_targetSpeed = Interpolate(
                getTemplate()->getUpsideDownFloorSpeed(),
                getTemplate()->getUpsideDownCeilingSpeed(),
                invCursor * invCursor);
            smoothSpeed = bfalse;
            m_upsideDownJump = btrue;
        }
        else
        {
            ITF_ASSERT_MSG(bfalse, "Not a jump");
        }
    }
    break;
    case State_ToFly:
    {
        ITF_ASSERT(isCurrentAction(m_toFlyAction));
        computeTargetSpeed(_linkInfo);
    }
    break;
    case State_Fly:
    {
        ITF_ASSERT(isCurrentAction(m_flyAction));
        computeTargetSpeed(_linkInfo);
    }
    break;
    case State_Land:
    {
        ITF_ASSERT(isCurrentAction(m_landAction));
        computeTargetSpeed(_linkInfo);
    }
    break;
    case State_Skid:
    {
        // skid then wait / taunt
        ITF_ASSERT(isCurrentAction(m_skidAction));
        m_targetSpeed = Interpolate(
            m_skidStartSpeed, 0.0f,
            _linkCursor * _linkCursor * _linkCursor);
        smoothSpeed = bfalse;
        if (m_targetSpeed < getTemplate()->getWaitSpeed())
        {
            m_targetSpeed = getTemplate()->getWaitSpeed();
            if (_linkInfo.getType() == TrajectoryLinkType_SkidAndWait)
            {
                changeState(State_Wait);
            }
            else // TrajectoryLinkType_SkidAndTaunt
            {
                changeState(State_Taunt);
            }
        }
    }
    break;
    case State_Wait:
    {
        ITF_ASSERT(isCurrentAction(m_waitAction));
        if (!m_breakable &&
            m_minDot <= m_safeDistance)
        {
            // player detected, back to run
            changeState(State_Run);
        }
        else
        {
            // don't move
            // NB[LaurentCou]: maintain a low speed to make sure we reach the
            // end node; setIsWaiting prevents us from overtaking it
            m_targetSpeed = getTemplate()->getWaitSpeed();
        }
    }
    break;
    case State_Taunt:
    {
        ITF_ASSERT(isCurrentAction(m_tauntAction));
        if (m_tauntStopCount > 0)
        {
            // triggered, back to run
            changeState(State_Run);
        }
        else
        {
            // don't move
            // NB[LaurentCou]: maintain a low speed to make sure we reach the
            // end node; setIsWaiting prevents us from overtaking it
            m_targetSpeed = getTemplate()->getWaitSpeed();
        }
    }
    break;
    case State_Dive:
    {
        ITF_ASSERT(isCurrentAction(m_diveAction));
        m_targetSpeed = getTemplate()->getMinSpeed();
    }
    break;
    case State_Swim:
    {
        ITF_ASSERT(isCurrentAction(m_swimAction));
        computeTargetSpeed(_linkInfo);
    }
    break;
    case State_ThinkDetect:
    {
        ITF_ASSERT(isCurrentAction(m_thinkDetectAction));
        ITF_ASSERT(!m_trajectoryFollowerComponent->isActive());
        m_targetSpeed = 0.0f;

        // reaction delay
        PlayerDetectorComponent* playerDetector = getPlayerDetector(m_thinkNode, m_thinkNodeType);
        if (playerDetector && playerDetector->getActorsInside().size()>0)
        {
            m_detectCountdown = Max(m_detectCountdown-_dt, 0.0f);
        }
        else
        {
            m_detectCountdown = getTemplate()->getDetectDelay();
        }
        if (0.0f == m_detectCountdown)
        {
            // gosh! a player! time to stop thinking
            stopThink();
        }
    }
    break;
    case State_ThinkHide:
    {
        ITF_ASSERT(isCurrentAction(m_thinkHideAction));
        ITF_ASSERT(!m_trajectoryFollowerComponent->isActive());
        m_targetSpeed = 0.0f;

        // timeout
        m_hideCountdown = Max(m_hideCountdown-_dt, 0.0f);

        // always try to flee straight, or anywhere after timeout
        stopThink(m_hideCountdown>0.0f ? btrue : bfalse);
    }
    break;
    case State_Breaks:
    {
        ITF_ASSERT(isCurrentAction(m_breaksAction));
        m_targetSpeed = 0.0f;
    }
    break;
    case State_Broken:
    {
        ITF_ASSERT(isCurrentAction(m_brokenAction));
        m_targetSpeed = 0.0f;
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }

    // check if invincible
    if (m_breakable)
    {
        if (m_thinkNode.isValid())
        {
            m_invincible = isInvincibleThinkNode(m_thinkNode);
        }
        else
        {
            m_invincible = _linkInfo.getInvincible();
        }
    }
    else
    {
        m_invincible = bfalse;
    }
    
    // decrement taunt trigger count
    if (m_tauntFlag && _linkInfo.getType() != TrajectoryLinkType_SkidAndTaunt)
    {
        --m_tauntStopCount;
    }
    m_tauntFlag = (_linkInfo.getType() == TrajectoryLinkType_SkidAndTaunt);

    // input target speed in trajectory follower
    m_trajectoryFollowerComponent->setRuntimeSpeed(m_targetSpeed, smoothSpeed);
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::startThink()
{
    bbool wasThinking = m_thinking;
    m_thinking = btrue;

    // get think nodes, deactivate trajectory following
    m_preThinkNode = m_trajectoryFollowerComponent->getPreviousNode();
    m_thinkNode = m_trajectoryFollowerComponent->getCurrentNode();
    m_thinkNodeType = getThinkNodeType(m_thinkNode);
    m_trajectoryFollowerComponent->setIsWaiting(btrue);
    m_trajectoryFollowerComponent->deactivate();

    if (m_thinkNodeType == ThinkNode_Detect || m_thinkNodeType == ThinkNode_DetectExtern)
    {
        // skip first detect node if wasn't thinking before
        // that's to avoid an easy kill on the left-most node
        if (!wasThinking)
        {
            stopThink();
        }

        // for detect node, wait a bit
        if (!m_trajectoryFollowerComponent->isActive())
        {
            m_detectCountdown = getTemplate()->getDetectDelay();
            changeState(State_ThinkDetect);
        }
    }
    else
    {
        ITF_ASSERT_MSG(m_thinkNodeType == ThinkNode_Hide, "Neither detect nor hide node");

        // for hide node, try to flee straight directly
        stopThink(btrue);

        // if failed, stay hidden
        if (!m_trajectoryFollowerComponent->isActive())
        {
            m_hideCountdown = getTemplate()->getHideTimeout();
            changeState(State_ThinkHide);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::stopThink(bbool _straightOnly /*= bfalse*/)
{
    // warn if recorded nodes disappeared
    TrajectoryNodeComponent* thinkNode  = getNode(m_thinkNode);
    ITF_WARNING(m_actor, thinkNode, "Can't get think node");
    TrajectoryNodeComponent* preThinkNode  = getNode(m_preThinkNode);
    ITF_WARNING(m_actor, preThinkNode, "Can't get pre-think node");
    if (!(thinkNode && preThinkNode))
    {
        return;
    }

    // enough thinking, find a path on which to move again
    TrajectoryNodeComponent::NodeList path;
    TrajectoryNodeComponent::NodeList visited;
    visited.push_back(thinkNode);
    if (findPath(preThinkNode, thinkNode, path, visited, 0, _straightOnly))
    {
        // copy path to actor ref list
        ActorRefList moveNodeRefs;
        moveNodeRefs.push_back(m_thinkNode);
        u32 pathSize = path.size();
        for (u32 i=0; i<pathSize; ++i)
        {
            moveNodeRefs.push_back(path[i]->GetActor()->getRef());
        }

        // activate trajectory following again
        // NB: this will allow us to move again
        m_trajectoryFollowerComponent->activate(moveNodeRefs);

        // clear recorded nodes
        m_preThinkNode.invalidate();
        m_thinkNode.invalidate();
        m_thinkNodeType = ThinkNode_Invalid;

        // allow going reverse after a countdown
        if (getTemplate()->getCanReverse())
        {
            m_reverseCountdown = getTemplate()->getReverseInterval();
        }
    }
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* Ray_AIChest2Behavior::getNode(const ActorRef& _ref) const
{
    Actor* actor = _ref.getActor();
    return actor ? actor->GetComponent<TrajectoryNodeComponent>() : NULL;
}

//------------------------------------------------------------------------------
PlayerDetectorComponent* Ray_AIChest2Behavior::getPlayerDetector(const ActorRef& _ref, EThinkNode _thinkNodeType) const
{
    Actor* actor = _ref.getActor();
    if (!actor)
    {
        return NULL;
    }

    if (_thinkNodeType == ThinkNode_Detect)
    {
        // check if we have a player detector available ("regular" think node)
        if (PlayerDetectorComponent* playerDetectorComponent = actor->GetComponent<PlayerDetectorComponent>())
        {
            return playerDetectorComponent;
        }
    }
    else if (_thinkNodeType == ThinkNode_DetectExtern)
    {
        // check if we're linked to a player detector (extern box)
        LinkComponent* linkComponent = actor->GetComponent<LinkComponent>();
        if (!linkComponent)
        {
            return NULL;
        }
        AIUtils::LinkIterator iterator(linkComponent, btrue);
        while (Actor* child = iterator.getNextRelativeActor())
        {
            if (PlayerDetectorComponent* playerDetectorComponent = child->GetComponent<PlayerDetectorComponent>())
            {
                return playerDetectorComponent;
            }
        }
    }

    ITF_WARNING(m_actor, bfalse, "Can't find player detector");
    return NULL;
}

//------------------------------------------------------------------------------
Ray_AIChest2Behavior::EThinkNode Ray_AIChest2Behavior::getThinkNodeType(const ActorRef& _ref) const
{
    TrajectoryNodeComponent* node = getNode(_ref);
    if (!node)
    {
        return ThinkNode_Invalid;
    }

    if (node->getName() == ITF_GET_STRINGID_CRC(detect,3208010014) || node->getName() == ITF_GET_STRINGID_CRC(detect_invincible,3828396759))
    {
        return ThinkNode_Detect;
    }

    if (node->getName() == ITF_GET_STRINGID_CRC(extern,599418095) || node->getName() == ITF_GET_STRINGID_CRC(extern_invincible,3702358747))
    {
        return ThinkNode_DetectExtern;
    }

    if (node->getName() == ITF_GET_STRINGID_CRC(hide,2631523017) || node->getName() == ITF_GET_STRINGID_CRC(hide_invincible,2975414932))
    {
        return ThinkNode_Hide;
    }

    return ThinkNode_Invalid;
}

//------------------------------------------------------------------------------
bbool Ray_AIChest2Behavior::isInvincibleThinkNode(const ActorRef& _ref) const
{
    TrajectoryNodeComponent* node = getNode(_ref);
    if (!node)
    {
        return bfalse;
    }

    return node->getName() == ITF_GET_STRINGID_CRC(detect_invincible,3828396759)
        || node->getName() == ITF_GET_STRINGID_CRC(extern_invincible,3702358747)
        || node->getName() == ITF_GET_STRINGID_CRC(hide_invincible,2975414932);
}

//------------------------------------------------------------------------------
bbool Ray_AIChest2Behavior::findPath(
    TrajectoryNodeComponent* _previous,
    TrajectoryNodeComponent* _from,
    TrajectoryNodeComponent::NodeList& _path,
    TrajectoryNodeComponent::NodeList& _visited,
    u32 _recursionCount,
    bbool _straightOnly)
{
    // NB[LaurentCou]: this is a recursive search in a node components network.
    // - getting the first path to a node with no player detected
    // - always trying the "straightest" path first
    // - no guarantee to be the shortest one

    // guard
    ITF_ASSERT(_previous && _from);
    if (!(_previous && _from))
    {
        return bfalse;
    }

    // guard against too many recursions
    static const u32 s_maxRecursion = 16;
    if (_recursionCount >= s_maxRecursion)
    {
        ITF_WARNING(
            _from->GetActor(), bfalse,
            "Can't find path, too many recursions");
        return bfalse;
    }

    // retrieve candidate nodes
    TrajectoryNodeComponent::NodeList candidateNodes;
    SafeArray<SortedNode> sortedNodes;
    _from->getChildNodes(candidateNodes);
    _from->getParentNodes(candidateNodes);
    u32 count = candidateNodes.size();
    for (u32 i=0; i<count; ++i)
    {
        TrajectoryNodeComponent* node = candidateNodes[i];

        // guard
        ITF_ASSERT(node);
        if (!node)
        {
            continue;
        }

        // skip if is the initial pre-think node, we're not going back in the level
        if (m_initialPreThinkNode == node->GetActor()->getRef())
        {
            continue;
        }

        // skip if already visited
        u32 index = _visited.find(node);
        if (U32_INVALID != index)
        {
            continue;
        }

        // compute "straighness" = dot product
        Vec2d previousToFrom = _from->GetActor()->get2DPos() - _previous->GetActor()->get2DPos();
        previousToFrom.normalize();
        Vec2d fromToNode = node->GetActor()->get2DPos() - _from->GetActor()->get2DPos();
        fromToNode.normalize();
        f32 dot = previousToFrom.dot(fromToNode);
    
        // add to sorted nodes array        
        SortedNode sortedNode;
        sortedNode.m_node = node;
        sortedNode.m_dot = dot;
        sortedNodes.push_back(sortedNode);
    }

    // sort nodes by relative "straightness"
    sortedNodes.quickSort();

    // with flag, straightest path only for the first recursion
    count = sortedNodes.size();
    if (_recursionCount==0 && _straightOnly)
    {
        count = 1;
    }

    // check for player detectors or recurse
    for (u32 i=0; i<count; ++i)
    {
        TrajectoryNodeComponent* node = sortedNodes[i].m_node;

        // guard
        ITF_ASSERT(node);
        if (!node)
        {
            continue;
        }

        // add to visited nodes
        _visited.push_back(node);

        // stop condition: detect node
        EThinkNode thinkNodeType = getThinkNodeType(node->GetActor()->getRef());
        if (thinkNodeType == ThinkNode_Detect || thinkNodeType == ThinkNode_DetectExtern)
        {
            PlayerDetectorComponent* playerDetector = getPlayerDetector(node->GetActor()->getRef(), thinkNodeType);
            if (playerDetector)
            {
                if (playerDetector->getActorsInside().size() == 0)
                {
                    // a node with no player! sweet! let's move there
                    _path.insert(node, 0);
                    return btrue;
                }
                else
                {
                    // player blocking path
                    continue;
                }
            }
        }

        // stop condition: hide node
        if (thinkNodeType == ThinkNode_Hide)
        {
            _path.insert(node, 0);
            return btrue;
        }

        // recurse
        if (findPath(_from, node, _path, _visited, ++_recursionCount, _straightOnly))
        {
            _path.insert(node, 0);
            return btrue;
        }
    }

    return bfalse;    
}

//------------------------------------------------------------------------------
void Ray_AIChest2Behavior::checkReverse(f32 _dt)
{
    // not breakable? doesn't apply
    if (!m_breakable)
    {
        return;
    }

    // can't reverse? doesn't apply
    if (!getTemplate()->getCanReverse())
    {
        return;
    }

    // countdown not done? wait
    m_reverseCountdown = Max(m_reverseCountdown-_dt, 0.0f);
    if (m_reverseCountdown > 0.0f)
    {
        return;
    }

    // get current path
    const ActorRefList& nodeList = m_trajectoryFollowerComponent->getNodeList();
    ITF_ASSERT(nodeList.size() > 0);
    if (nodeList.size() == 0)
    {
        return;
    }

    // get last node's player detector
    ActorRef lastNodeRef = nodeList[nodeList.size()-1];
    PlayerDetectorComponent* playerDetector = getPlayerDetector(lastNodeRef, getThinkNodeType(lastNodeRef));
    ITF_ASSERT(playerDetector);
    if (!playerDetector)
    {
        return;
    }

    // player detected? reverse!
    if (playerDetector->getActorsInside().size() > 0)
    {
        m_trajectoryFollowerComponent->reverse();

        // update scale change
        if (m_changingScale)
        {
            m_atReducedScale = !m_atReducedScale;
        }

        // start countdown again
        m_reverseCountdown = getTemplate()->getReverseInterval();
    }
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_AIChest2Behavior::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (!m_thinking)
    {
        // debug draw safe distance stuff
        Actor* closestPlayer = m_closestPlayerRef.getActor();
        if (closestPlayer)
        {
            DebugDraw::circle(
                m_actor->getPos(), m_safeDistance,
                Color::magenta());

            DebugDraw::text(
                Interpolate(closestPlayer->getPos(), m_actor->getPos(), 0.5f),
                Color::black(), "dot=%f", m_minDot);

            DebugDraw::arrow3D(
                closestPlayer->getPos(), m_actor->getPos(),
                Color::magenta());
        }
    }
    else
    {
        // debug draw think info
        Vec3d anchorPos = m_actor->getPos();
        anchorPos.m_y += 0.2f;
        if (m_state == State_ThinkDetect)
        {
            DebugDraw::text(
                anchorPos, Color::green(),
                "detect %.3f", m_detectCountdown);
        }
        else if (m_state == State_ThinkHide)
        {
            DebugDraw::text(
                anchorPos, Color::red(),
                "hide %.3f", m_hideCountdown);
        }
        else
        {
            DebugDraw::text(
                m_actor->getPos(), Color::magenta(),
                "move");
        }
    }
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIChest2Behavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIChest2Behavior_Template)
    
    // movement
    SERIALIZE_MEMBER("safeDistance", m_safeDistance);
    SERIALIZE_MEMBER("minSpeed", m_minSpeed);
    SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
    SERIALIZE_MEMBER("swimMinSpeed", m_swimMinSpeed);
    SERIALIZE_MEMBER("swimMaxSpeed", m_swimMaxSpeed);
    SERIALIZE_MEMBER("waitSpeed", m_waitSpeed);
    SERIALIZE_MEMBER("upsideDownFloorSpeed", m_upsideDownFloorSpeed);
    SERIALIZE_MEMBER("upsideDownCeilingSpeed", m_upsideDownCeilingSpeed);
    SERIALIZE_MEMBER("breakableSpeed", m_breakableSpeed);
    SERIALIZE_MEMBER("reducedScale", m_reducedScale);
    
    // eye following
    SERIALIZE_MEMBER("eyeMinX", m_eyeMinX);
    SERIALIZE_MEMBER("eyeMaxX", m_eyeMaxX);
    SERIALIZE_MEMBER("eyeMinY", m_eyeMinY);
    SERIALIZE_MEMBER("eyeMaxY", m_eyeMaxY);
    SERIALIZE_MEMBER("relativeNeutralEyePos", m_relativeNeutralEyePos);
    SERIALIZE_MEMBER("distanceToEyeFactor", m_distanceToEyeFactor);
    SERIALIZE_MEMBER("typicalPlayerHeadY", m_typicalPlayerHeadY);

    // break fx
    SERIALIZE_MEMBER("breakFx", m_breakFx);

    // relic spawn
    SERIALIZE_MEMBER("relicIndex", m_relicIndex);
    SERIALIZE_MEMBER("relicActor", m_relicActor);

    // break events
    SERIALIZE_MEMBER("sendBreakEventsDirectly", m_sendBreakEventsDirectly);
    SERIALIZE_MEMBER("disableAfterBreakEvents", m_disableAfterBreakEvents);
    SERIALIZE_MEMBER("snapBreakSequencePlayer", m_snapBreakSequencePlayer);
    SERIALIZE_CONTAINER_WITH_FACTORY("breakEvents", m_breakEvents, ACTORSMANAGER->getEventFactory());

    // thinking
    SERIALIZE_MEMBER("detectDelay", m_detectDelay);
    SERIALIZE_MEMBER("hideTimeout", m_hideTimeout);
    SERIALIZE_MEMBER("canReverse", m_canReverse);
    SERIALIZE_MEMBER("reverseInterval", m_reverseInterval);

    // actions
    SERIALIZE_OBJECT_WITH_FACTORY("standAction", m_standAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("runAction", m_runAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("jumpAction", m_jumpAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("toFlyAction", m_toFlyAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("flyAction", m_flyAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("landAction", m_landAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("skidAction", m_skidAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("waitAction", m_waitAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("tauntAction", m_tauntAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("diveAction", m_diveAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("swimAction", m_swimAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("thinkDetectAction", m_thinkDetectAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("thinkHideAction", m_thinkHideAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("breaksAction", m_breaksAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("brokenAction", m_brokenAction, ACTORSMANAGER->getAIActionsFactory());

END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIChest2Behavior_Template::Ray_AIChest2Behavior_Template()
    : Super()
    , m_safeDistance(1.0f)
    , m_minSpeed(0.0f)
    , m_maxSpeed(1.0f)
    , m_swimMinSpeed(0.0f)
    , m_swimMaxSpeed(1.0f)
    , m_waitSpeed(0.0f)
    , m_upsideDownFloorSpeed(1.0f)
    , m_upsideDownCeilingSpeed(1.0f)
    , m_breakableSpeed(1.0f)
    , m_reducedScale(1.0f)
    , m_eyeMinX(0.f)
    , m_eyeMaxX(1.0f)
    , m_eyeMinY(0.2f)
    , m_eyeMaxY(0.8f)
    , m_relativeNeutralEyePos(0.0f, 0.4f)
    , m_distanceToEyeFactor(0.2f)
    , m_typicalPlayerHeadY(0.4f)
    , m_breakFx()
    , m_relicIndex(-1)
    , m_relicActor()
    , m_sendBreakEventsDirectly(bfalse)
    , m_disableAfterBreakEvents(bfalse)
    , m_snapBreakSequencePlayer(bfalse)
    , m_breakEvents()
    , m_detectDelay(1.0f)
    , m_hideTimeout(1.0f)
    , m_canReverse(bfalse)
    , m_reverseInterval(1.0f)
    , m_standAction(NULL)
    , m_runAction(NULL)
    , m_jumpAction(NULL)
    , m_toFlyAction(NULL)
    , m_flyAction(NULL)
    , m_landAction(NULL)
    , m_skidAction(NULL)
    , m_waitAction(NULL)
    , m_tauntAction(NULL)
    , m_diveAction(NULL)
    , m_swimAction(NULL)
    , m_thinkDetectAction(NULL)
    , m_thinkHideAction(NULL)
    , m_breaksAction(NULL)
    , m_brokenAction(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIChest2Behavior_Template::~Ray_AIChest2Behavior_Template()
{
    // delete template break events
    u32 breakEventsCount = m_breakEvents.size();
    for (u32 i=0; i<breakEventsCount; ++i)
    {
        SF_DEL(m_breakEvents[i]);
    }
    m_breakEvents.clear();

    SF_DEL(m_standAction);
    SF_DEL(m_runAction);
    SF_DEL(m_jumpAction);
    SF_DEL(m_toFlyAction);
    SF_DEL(m_flyAction);
    SF_DEL(m_landAction);
    SF_DEL(m_skidAction);
    SF_DEL(m_waitAction);
    SF_DEL(m_tauntAction);
    SF_DEL(m_diveAction);
    SF_DEL(m_swimAction);
    SF_DEL(m_thinkDetectAction);
    SF_DEL(m_thinkHideAction);
    SF_DEL(m_breaksAction);
    SF_DEL(m_brokenAction);
}

} // namespace ITF
