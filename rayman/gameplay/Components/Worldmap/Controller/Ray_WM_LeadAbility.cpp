#include "precompiled_gameplay_rayman.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_RAY_WM_LEADABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_LeadAbility.h"
#endif //_ITF_RAY_WM_LEADABILITY_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_RAYWMLEVELSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LevelSpotComponent.h"
#endif //_ITF_RAYWMLEVELSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMWORLDSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_WorldSpotComponent.h"
#endif //_ITF_RAYWMWORLDSPOTCOMPONENT_H_

#ifndef _ITF_RAY_WM_CONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_ControllerComponent.h"
#endif //_ITF_RAY_WM_CONTROLLERCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_LeadAbility)
BEGIN_VALIDATE_OBJECT_CHILD(Ray_WM_LeadAbility)
END_VALIDATE_OBJECT()

//------------------------------------------------------------------------------
Ray_WM_LeadAbility::Ray_WM_LeadAbility()
    : Super()
    , m_inLevelSelection(bfalse)
    , m_changedLayer(btrue)
    , m_wordlMapUnlockSequencePlayed(bfalse)
    , m_trajectoryData()
    , m_standNodeRef()
    , m_moveNodeRefs()
    , m_moveNodeIndex(U32_INVALID)
    , m_reverseForbidden(bfalse)
    , m_speed(0.0f)
    , m_targetSpeed(0.0f)
    , m_followers()
    , m_followersFrameOffset()
    , m_moveRecords()
    , m_frame(0)
    , m_listenerRegistered(bfalse)
{
    for (u32 i=0; i<s_maxFollowers; ++i)
    {
        m_followers.push_back(bfalse);
        m_followersFrameOffset.push_back(0);
    }
    for (u32 i=0; i<s_moveRecordsCount; ++i)
    {
        m_moveRecords.push_back(Ray_WM_MoveRecord());
    }

    clearListener();
}

//------------------------------------------------------------------------------
Ray_WM_LeadAbility::~Ray_WM_LeadAbility()
{
    // get stand node's spot, signal exit
    Ray_WM_SpotComponent* spotComponent = getSpot(m_standNodeRef);
    if (spotComponent)
    {
        spotComponent->onExit();
    }

    // reset input listener
    if(m_listenerRegistered)
    {
        m_listenerRegistered = bfalse;
        GAMEMANAGER->removeInputListener(this);
    }
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::onActorLoaded(
    Pickable::HotReloadType _hotReload,
    const Ray_WM_BaseAbility_Template* _template,
    Ray_WM_ControllerComponent* _controller,
    Actor* _actor)
{
    Super::onActorLoaded(_hotReload, _template, _controller, _actor);
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::onBecomeActive(u32 _playerIndex)
{
    Super::onBecomeActive(_playerIndex);

    // safety check
    ITF_ASSERT(_playerIndex == GAMEMANAGER->getMainIndexPlayer());

    // init input listener
    if(!m_listenerRegistered)
    {
        m_listenerRegistered = btrue;
        GAMEMANAGER->addGamePlayInputListener(this);
        GAMEMANAGER->addCheatInputListener(this);
    }

    // register to camera
    CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef());
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::onPlayerIndexChange(u32 _newIndex)
{
    Super::onPlayerIndexChange(_newIndex);
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::onBecomeInactive()
{
    Super::onBecomeInactive();

    // get stand node's spot, signal exit
    Ray_WM_SpotComponent* spotComponent = getSpot(m_standNodeRef);
    if (spotComponent)
    {
        spotComponent->onExit();
    }

    // reset input listener
    if(m_listenerRegistered)
    {
        m_listenerRegistered = bfalse;
        GAMEMANAGER->removeInputListener(this);
    }

    // reset on switch to world map (RO-14231)
    m_standNodeRef.invalidate();
    for (u32 i=0; i<s_maxFollowers; ++i)
    {
        m_followers[i] = bfalse;
        m_followersFrameOffset[i] = 0;
    }
    for (u32 i=0; i<s_moveRecordsCount; ++i)
    {
        m_moveRecords[i].clear();
    }
    m_frame = 0;

    // unbind all followers
    m_actor->getBinding()->clear();
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::update(f32 _dt)
{
    Super::update(_dt);
    
    if (m_controller->getState() == Ray_WM_ControllerState_NA )
    {
        m_changedLayer = btrue;

        if (!findStandNodeRef())
            return;
    }

    // start world map unlock sequence
    if (!m_wordlMapUnlockSequencePlayed)
    {
        RAY_GAMEMANAGER->triggerWorldMapUnlockSequence(m_standNodeRef);
        m_wordlMapUnlockSequencePlayed = btrue;
    }

    applyInput();
    updateMove(_dt);
    
    clearListener();
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_WM_LeadAbility::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    Vec2d pos = m_actor->get2DPos();
    static const f32 yOffset = 0.1f;

    DebugDraw::text(
        pos, 0.0f, Color::green(),
        "current node %s",
        getCurrentNodeRef().getActor()
        ? getCurrentNodeRef().getActor()->getUserFriendly().cStr()
        : "NULL");
    pos.m_y += yOffset;

    DebugDraw::text(
        pos, 0.0f, Color::magenta(),
        "speed %.3f (target %.3f)",
        m_speed, m_targetSpeed);
    pos.m_y += yOffset;
}
#endif

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::signalFollowing(Actor* _actor, u32 _playerIndex, bbool _following)
{
    // record follower
    u32 followerIndex = getFollowerIndex(_playerIndex);
    m_followers[followerIndex] = _following;
    m_followersFrameOffset[followerIndex] = computeFrameOffset(followerIndex);

    // make sure lead's updated before followers (RO-14377)
    if (_following)
    {
        m_actor->getBinding()->bindChild(_actor->getRef());
    }
    else
    {
        m_actor->getBinding()->unbindChild(_actor->getRef());
    }
}

//------------------------------------------------------------------------------
Ray_WM_MoveRecord Ray_WM_LeadAbility::getMoveRecord(u32 _playerIndex) const
{
    // get follower index for given player
    u32 followerIndex = getFollowerIndex(_playerIndex);

    // copy move record from circular buffer
    u32 offset = m_followersFrameOffset[followerIndex];
    u32 index = (m_frame > offset) ? m_frame-offset : 0;
    index %= s_moveRecordsCount;
    Ray_WM_MoveRecord moveRecord = m_moveRecords[index];

    // apply adjust offset
    Vec3d adjustOffset;
    ITF_WARNING(
        m_actor,
        (getTemplate()->getFollowersAdjustOffsets().size() <= s_maxFollowers),
        "Missing followers adjust offsets, needs %d", s_maxFollowers);
    if (getTemplate()->getFollowersAdjustOffsets().size() <= s_maxFollowers)
    {
        Vec3d adjustOffset = getTemplate()->getFollowersAdjustOffsets()[followerIndex];
        moveRecord.m_pos += moveRecord.m_adjustFactor * adjustOffset;
    }

    // return copied & adjusted record
    return moveRecord;
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::teleport(Actor* _to)
{
    ITF_ASSERT(_to);
    if (!_to)
    {
        return;
    }

    TrajectoryNodeComponent* node = _to->GetComponent<TrajectoryNodeComponent>();
    ITF_ASSERT(node);
    if (!node)
    {
        return;
    }

    Ray_WM_SpotComponent* spot = _to->GetComponent<Ray_WM_SpotComponent>();
    ITF_ASSERT(spot);
    if (!spot)
    {
        return;
    }

    // flags
    m_inLevelSelection = !spot->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_WM_WorldSpotComponent,3142377630));
    m_changedLayer = btrue;

    // restart
    changeStateInternal(spot->isStandSwim()
        ? Ray_WM_ControllerState_StandSwim
        : Ray_WM_ControllerState_Stand
        , _to->getRef());

    // remember first X
    m_controller->m_previousPos = m_actor->get2DPos();

    // teleport cam
    CAMERACONTROLLERMANAGER->teleport(_to->getPos());

    // invalidate move records
    for (u32 i=0; i<s_moveRecordsCount; ++i)
    {
        m_moveRecords[i].clear();
    }
    m_frame = 0;
}

//------------------------------------------------------------------------------
Ray_WM_SpotComponent* Ray_WM_LeadAbility::getStandSpot() const
{
    return getSpot(m_standNodeRef);
}

//------------------------------------------------------------------------------
u32 Ray_WM_LeadAbility::getFollowerIndex(u32 _playerIndex) const
{
    // safety checks
    ITF_ASSERT(_playerIndex!=m_playerIndex && _playerIndex<s_maxPlayers);
    if (_playerIndex==m_playerIndex || _playerIndex>=s_maxPlayers)
    {
        return 0;
    }

    // get follower index
    u32 followerIndex = (_playerIndex>m_playerIndex)
        ? _playerIndex-1
        : _playerIndex;

    // yet another safety check and return
    ITF_ASSERT(followerIndex<s_maxFollowers);
    if (followerIndex>=s_maxFollowers)
    {
        return 0;
    }
    return followerIndex;
}

//------------------------------------------------------------------------------
Ray_WM_SpotComponent* Ray_WM_LeadAbility::getSpot(const ActorRef& _ref) const
{
    // get actor from ref
    Actor* actor = _ref.getActor();
    if (!actor)
    {
        return NULL;
    }

    // return its spot component (or NULL if non-existent)
    return actor->GetComponent<Ray_WM_SpotComponent>();
}

//------------------------------------------------------------------------------
TrajectoryNodeComponent* Ray_WM_LeadAbility::getNode(const ActorRef& _ref) const
{
    // get actor from ref
    Actor* actor = _ref.getActor();
    if (!actor)
    {
        return NULL;
    }

    // return its node component (or NULL if non-existent)
    return actor->GetComponent<TrajectoryNodeComponent>();
}

//------------------------------------------------------------------------------
ActorRef Ray_WM_LeadAbility::getCurrentNodeRef() const
{
    if (!m_controller->isMoving())
    {
        return m_standNodeRef;
    }
    else
    {
        ITF_ASSERT(m_moveNodeIndex<m_moveNodeRefs.size());
        return m_moveNodeRefs[m_moveNodeIndex];
    }
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::applyInput()
{
    // ignore input in (world map unlock/teleport) sequence
    if (RAY_GAMEMANAGER->isSequencePlaying())
    {
        m_controller->m_isSprinting = bfalse;
        return;
    }

    // sprinting?
    m_controller->m_isSprinting = m_listenerSprint;

    // move with joystick
    if (m_listenerMove.norm() > 0.5f)
    {
        m_listenerMove.normalize();
        if (!m_controller->isMoving())
        {
            findMove(m_listenerMove, m_standNodeRef);
        }
        else if ((m_controller->getState() == Ray_WM_ControllerState_Walk || m_controller->getState() == Ray_WM_ControllerState_Swim) 
             && !m_controller->isPerformingUTurn()
             && !m_reverseForbidden) // workaround for RO-11109
        {
            reverseMove(m_listenerMove);
        }
    }
    // enter world/level node
    else if (!m_controller->isMoving() && m_listenerEnter)
    {
        // Get current spot we're on
        Ray_WM_SpotComponent* spotComponent = getSpot(m_standNodeRef);
        ITF_ASSERT(spotComponent);

        if (spotComponent)
        {
            spotComponent->onAction();
        }
        
        // world selection -> level selection
        if (spotComponent && spotComponent->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_WM_WorldSpotComponent,3142377630)))
        {
            // Special case for HomeMap -> don't load it as it's already in the worldmap,
            // just launch the teleportation process.
            if ( spotComponent->getTag() == s_HomeMap )
            {
                Player* player = RAY_GAMEMANAGER->getPlayer(GAMEMANAGER->getMainIndexPlayer());

                if ( player && player->getCurrentMode() == RAY_GAMEMODE_WORLDMAP )
                {
                    // We select the node
                    RAY_GAMEMANAGER->triggerChangeRoomSequence(btrue);

                    u32 PresenceState = RAY_GAMEMANAGER->getRichPresenceIndex(s_HomeMap);
                    GAMEMANAGER->updateRichPresenceForActivePlayers(PresenceState);
                    return;
                }
            }
            // Don't do anything if we're on land of the dead, as it's a world with level functionality
            // so onAction will start the loading of the level
            else if ( spotComponent->getTag() != s_DE_A1 )
            {
                // to level selection
                startTeleportToLevelSelection();
            }
        }
    }
    // back
    // ignore the input if we're on land of the dead level (we're already in the worldmap even if
    // current level node is valid (fixes http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-17122)
    else if (m_listenerBack)
    {
        // level selection -> world selection
        if (m_inLevelSelection)
        {
            startTeleportToWorldSelection();
        }
    }
    // home
    else if (m_listenerHome)
    {
        // From input
        u32 PresenceState = RAY_GAMEMANAGER->getRichPresenceIndex(s_HomeMap);
        GAMEMANAGER->updateRichPresenceForActivePlayers(PresenceState);

        RAY_GAMEMANAGER->triggerChangeRoomSequence(btrue);
    }

#ifdef ITF_SUPPORT_CHEAT
    // cheat to unlock the world/level we stand on
    Ray_WM_SpotComponent* spot = getSpot(m_standNodeRef);
    if (CHEATMANAGER->getCanListenActions() &&
        m_listenerCheatUnlock &&
        spot &&
        !RAY_GAMEMANAGER->isPlayingWorldMapUnlockSequence())
    {
        bbool wasCompleted = spot->getState() == SPOT_STATE_COMPLETED;

        // unlock everything if already completed
        if (wasCompleted)
        {
            const StringID& type = RAY_GAMEMANAGER->getMapConfigType(spot->getTag());
            if (type.isValid())
            {
                const Ray_GameManagerConfig_Template::MissionConfig* config = RAY_GAMEMANAGER->findMissionConfigByType(type, btrue);
                Ray_PersistentGameData_Level* levelData = RAY_GAMEMANAGER->getLevelData(spot->getTag());
                if (config && levelData)
                {
                    // unlock electoons
                    for (u32 i=0; i<config->m_medalSlots.size(); ++i)
                    {
                        const Ray_GameManagerConfig_Template::MedalSlotConfig& slot = config->m_medalSlots[i];
                        switch (slot.m_type)
                        {
                        case Ray_GameManagerConfig_Template::MedalSlotConfig::Cage:
                            {
                                const Path& path = RAY_GAMEMANAGER->getLevelPath(spot->getTag());
                                levelData->setBrokenCageFlag(path, slot.m_value, btrue);
                            } break;
                        case Ray_GameManagerConfig_Template::MedalSlotConfig::TimeAttack:
                            {
                                levelData->setBestTimeAttack(slot.m_value);
                                levelData->setChallengeWon(CHALLENGE_CUP);
                                levelData->setChallengeWon(CHALLENGE_TIMEATTACK);
                            } break;
                        case Ray_GameManagerConfig_Template::MedalSlotConfig::LumAttack:
                            {
                                levelData->setBestLumAttack(slot.m_value);
                                levelData->setChallengeWon(CHALLENGE_LUMATTACK1);
                                levelData->setChallengeWon(CHALLENGE_LUMATTACK2);
                                levelData->setChallengeWon(CHALLENGE_LUMATTACK3);
                            } break;
                        case Ray_GameManagerConfig_Template::MedalSlotConfig::Boss:
                            {
                                levelData->setChallengeWon(CHALLENGE_BEATENBOSS);
                            } break;
                        default: ITF_ASSERT_MSG(bfalse, "Unhandled medal slot type %d, please update WM cheat", slot.m_type);
                        }
                    }

                    // unlock skull tooth
                    if (type == s_MapConfig_Chest)
                    {
                        const Path& path = RAY_GAMEMANAGER->getLevelPath(spot->getTag());
                        levelData->setTakenTooth(path, PackedObjectPath(), btrue); // TODO[LaurentCou]: find a way to unlock tooth properly
                    }

                    levelData->setLevelIsSkipped(bfalse);

					u32 leveMaxRelics = RAY_GAMEMANAGER->getMaxRelics(spot->getTag());
					for (u32 i=0; i<leveMaxRelics; ++i)
					{
						RAY_GAMEMANAGER->setIsRelicTaken(spot->getTag(),i, btrue);
						u32 frescoId = RAY_GAMEMANAGER->getPersistentGameData()->GetDebugFrescoId();
						RAY_GAMEMANAGER->getPersistentGameData()->UpdateInGameRelicsInfo(frescoId);
					}

                    }
                }
            }

        // change current spot state
        // NB[LaurentCou]: this will update automatically all linked spots
        // => especially useful when the above code unlocks some other map
        spot->changeState(SPOT_STATE_COMPLETED);

        if (!wasCompleted && RAY_GAMEMANAGER->isLastLevel(spot->getTag()))
        {
            // teleport to world selection if last map newly completed
            // + play unlock sequence after this
            RAY_GAMEMANAGER->setWMSpotState(RAY_GAMEMANAGER->getWMCurrentWorldTag(), SPOT_STATE_COMPLETED);
            RAY_GAMEMANAGER->setPendingWorldMapUnlockSequence(btrue);
            startTeleportToWorldSelection();
        }
        else
        {
            // start unlock sequence
            RAY_GAMEMANAGER->triggerWorldMapUnlockSequence(m_standNodeRef);
        }
    }
#endif
}

//------------------------------------------------------------------------------
bbool Ray_WM_LeadAbility::findStandNodeRef()
{
    // get start node
    ObjectPath startNode = getTemplate()->getStartNode();
    if (RAY_GAMEMANAGER->getWMCurrentLevelObjectPath().isValid())
    {
        startNode = RAY_GAMEMANAGER->getWMCurrentLevelObjectPath();

        // in level selection?
        bbool onLevel = RAY_GAMEMANAGER->getWMCurrentLevelObjectPath().isValid();
        StringID tag = RAY_GAMEMANAGER->getWMCurrentLevelTag();
        if (onLevel && tag != s_DE_A1)
        {
            m_inLevelSelection = btrue;
        }
    }
    else if (RAY_GAMEMANAGER->getWMCurrentWorldObjectPath().isValid())
    {
        startNode = RAY_GAMEMANAGER->getWMCurrentWorldObjectPath();
    }
    else
    {
        ITF_ASSERT_MSG(bfalse, "No current world or level in world map");
    }
    ActorRef standNodeRef;
    if (Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(startNode))
    {
        if (Actor* actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
        {
            standNodeRef = actor->getRef();
        }
    }

    // stand on it
    ITF_WARNING(
        m_actor, standNodeRef.isValid(),
        "Can't find start node: %s", startNode.getStrId()
        );
    if (!standNodeRef.isValid())
    {
        return bfalse;
    }

    // guard for missing component
    ITF_WARNING(
        m_actor, getNode(standNodeRef),
        "Start node had no node component: %s", startNode.getStrId()
        );
    if (!getNode(standNodeRef))
    {
        return bfalse;
    }

    // guard for missing component
    Ray_WM_SpotComponent* spot = getSpot(standNodeRef);
    ITF_WARNING(
        m_actor, spot,
        "Start node had no spot component: %s", startNode.getStrId()
        );
    if (!getSpot(standNodeRef))
    {
        return bfalse;
    }

    // start up
    changeStateInternal(spot->isStandSwim()
        ? Ray_WM_ControllerState_StandSwim
        : Ray_WM_ControllerState_Stand
        , standNodeRef);

    // remember first X
    m_controller->m_previousPos = m_actor->get2DPos();

    return btrue;
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::updateMove(f32 _dt)
{
    if (m_controller->isMoving())
    {
        m_reverseForbidden = bfalse;

        TrajectoryNodeComponent* currentNode = m_trajectoryData.getCurrentNode();
        if (currentNode)
        {
            if (!m_listenerSprint)
            {
                m_speed = AIUtils::doubleMichelSmooth(
                    m_speed,
                    m_targetSpeed,
                    getTemplate()->getMaxSpeed(),
                    getTemplate()->getTargetSpeedBlendFactor(),
                    getTemplate()->getSpeedBlendFactor()
                    );
            }
            else
            {
                m_speed = AIUtils::doubleMichelSmooth(
                    m_speed,
                    m_targetSpeed,
                    getTemplate()->getSprintMaxSpeed(),
                    getTemplate()->getSprintTargetSpeedBlendFactor(),
                    getTemplate()->getSprintSpeedBlendFactor()
                    );
            }

            f32 travel = m_speed * _dt;
            m_trajectoryData.setCurrentNodeDist(m_trajectoryData.getCurrentNodeDist() + travel);

            bbool isWaiting = bfalse;
            Vec3d pos = m_actor->getPos();
            Vec3d tangent = Vec3d::Right;
            bbool flip = bfalse;

            while (currentNode->move(m_trajectoryData, isWaiting, pos, tangent, flip))
            {
                if (m_moveNodeIndex >= m_moveNodeRefs.size()-2)
                {
                    // get end node *then* reset move info
                    ActorRef endNode = m_moveNodeRefs[m_moveNodeRefs.size()-1];
                    m_trajectoryData.reset();
                    m_moveNodeRefs.clear();

                    // keep moving if possible (RO-11207)
                    bbool keepMoving = bfalse;
                    if (m_listenerMove.norm() > 0.5f)
                    {
                        m_listenerMove.normalize();
                        if (findMove(m_listenerMove, endNode))
                        {
                            keepMoving = btrue;
                        }
                    }

                    // stand if not moving anymore
                    if (!keepMoving)
                    {
                        Ray_WM_SpotComponent* spot = getSpot(endNode);
                        ITF_ASSERT(spot);
                        if (spot)
                        {
                            changeStateInternal(spot->isStandSwim()
                                ? Ray_WM_ControllerState_StandSwim
                                : Ray_WM_ControllerState_Stand
                                , endNode);
                        }
                    }

                    break;
                }
                else
                {
                    stepMove(m_moveNodeIndex+1);
                    currentNode = m_trajectoryData.getCurrentNode();
                }
            }

            // fix extra move once stopped
            if (m_controller->isMoving())
            {
                pos.m_z += getTemplate()->getZOffset();
                m_actor->setPos(pos);
            }
        }
    }
    else
    {
        m_speed = m_targetSpeed = 0.0f;
    }
    
    // fill move records circular buffer
    u32 index = (m_frame++) % s_moveRecordsCount;
    m_moveRecords[index].m_valid = btrue;
    m_moveRecords[index].m_pos = m_actor->getPos();
    m_moveRecords[index].m_adjustFactor = computeAdjustFactor();
    m_moveRecords[index].m_isSprinting = m_controller->m_isSprinting;
    m_moveRecords[index].m_state = m_controller->getState();

    // always try to reach target offset
    for (u32 i=1; i<s_maxFollowers; ++i)
    {
        if (m_followers[i])
        {
            u32 targetOffset = computeFrameOffset(i);
            u32 currentOffset = m_followersFrameOffset[i];
            if (targetOffset > currentOffset)
            {
                ++m_followersFrameOffset[i];
            }
            else if (targetOffset < currentOffset)
            {
                --m_followersFrameOffset[i];
            }

            ITF_ASSERT(
                m_followersFrameOffset[i]>=s_framesPerFollower &&
                m_followersFrameOffset[i]<=s_moveRecordsCount);
        }
    }
}

//------------------------------------------------------------------------------
f32 Ray_WM_LeadAbility::computeAdjustFactor() const
{
    f32 adjustFactor;
    if (!m_controller->isMoving())
    {
        adjustFactor = 1.0f;
    }
    else if (m_moveNodeIndex == 0)
    {
        u32 count = m_moveNodeRefs.size();
        if (count <= 2)
        {
            // fix adjust factor interpolation if no dummy node
            f32 linkCursor = m_trajectoryData.getLinkCursor();
            if (linkCursor < 0.5f)
            {
                adjustFactor = 2.0f * (0.5f - linkCursor);
            }
            else
            {
                adjustFactor = 2.0f * (linkCursor - 0.5f);
            }
        }
        else
        {
            adjustFactor = 1.0f - m_trajectoryData.getLinkCursor();
        }
    }
    else if (m_moveNodeIndex == m_moveNodeRefs.size()-2)
    {
        adjustFactor = m_trajectoryData.getLinkCursor();
    }
    else if (m_moveNodeIndex == m_moveNodeRefs.size()-1)
    {
        adjustFactor = 1.0f;
    }
    else
    {
        adjustFactor = 0.0f;
    }
    ITF_ASSERT(adjustFactor>=0.0f && adjustFactor<=1.0f);
    return adjustFactor;
}

//------------------------------------------------------------------------------
u32 Ray_WM_LeadAbility::computeFrameOffset(u32 _followerIndex) const
{
    // count players before
    u32 playersBefore = 1;
    for (u32 i=0; i<_followerIndex; ++i)
    {
        if (m_followers[i])
        {
            ++playersBefore;
        }
    }
    u32 frameOffset = s_framesPerFollower * playersBefore;
    ITF_ASSERT(frameOffset<=s_moveRecordsCount);
    return frameOffset;
}

//------------------------------------------------------------------------------
bbool Ray_WM_LeadAbility::findMove(const Vec2d& _dir, const ActorRef& _nodeRef)
{
    // get node component
    TrajectoryNodeComponent* startNode = getNode(_nodeRef);
    ITF_ASSERT(startNode);
    if (!startNode)
    {
        return bfalse;
    }

    // find node at shortest angle
    TrajectoryNodeComponent::NodeList candidateNodes;
    startNode->getChildNodes(candidateNodes);
    startNode->getParentNodes(candidateNodes);
    TrajectoryNodeComponent::NodeList path;
    TrajectoryNodeComponent* nextNode = NULL;
    f32 shortestAngleDiff = F32_INFINITY;
    u32 count = candidateNodes.size();
    for (u32 i=0; i<count; ++i)
    {
        // if already a spot, no need to search for a path
        TrajectoryNodeComponent::NodeList candidatePath;
        if (!candidateNodes[i]->GetActor()->GetComponent<Ray_WM_SpotComponent>())
        {
            // get first path to a spot, if failed don't use this node
            TrajectoryNodeComponent::NodeList visited;
            visited.push_back(startNode);
            visited.push_back(candidateNodes[i]);
            if (!Ray_GameScreen_WorldMap::findSpot(candidateNodes[i], candidatePath, visited, 0))
            {
                continue;
            }
        }

        // if spot isn't accessible, don't use
        u32 candidatePathSize = candidatePath.size();
        Ray_WM_SpotComponent* candidateSpot = NULL;
        if (candidatePathSize == 0)
        {
            candidateSpot = candidateNodes[i]->GetActor()->GetComponent<Ray_WM_SpotComponent>();
        }
        else
        {
            TrajectoryNodeComponent* lastNode = candidatePath[candidatePathSize-1];
            candidateSpot = lastNode->GetActor()->GetComponent<Ray_WM_SpotComponent>();
        }
        if (!candidateSpot || !candidateSpot->isAccessible())
        {
            continue;
        }

        // angle test
        Vec2d nodeDir = candidateNodes[i]->GetActor()->get2DPos() - startNode->GetActor()->get2DPos();
        nodeDir.normalize();
        f32 angleDiff = nodeDir.getShortestAngle(_dir);
        if (angleDiff < shortestAngleDiff)
        {
            path = candidatePath;
            nextNode = candidateNodes[i];
            shortestAngleDiff = angleDiff;
        }
    }
    
    // no next node? give up and keep standing
    if (!nextNode)
    {
        return bfalse;
    }

    // angle too large? give up and keep standing
    if (shortestAngleDiff > getTemplate()->getFindMoveAngle().Radians())
    {
        return bfalse;
    }
    
    // copy path to actor ref list
    ITF_ASSERT(m_moveNodeRefs.size() == 0);
    m_moveNodeRefs.push_back(_nodeRef);
    m_moveNodeRefs.push_back(nextNode->GetActor()->getRef());
    u32 pathSize = path.size();
    for (u32 i=0; i<pathSize; ++i)
    {
        m_moveNodeRefs.push_back(path[i]->GetActor()->getRef());
    }

    // start moving
    stepMove(0);

    return btrue;
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::reverseMove(const Vec2d& _dir)
{
    // get current and next node
    Actor* current = m_trajectoryData.getCurrentNodeRef().getActor();
    Actor* next = m_trajectoryData.getNextNodeRef().getActor();
    if (!current || !next)
    {
        return;
    }
    
    // if angle too large, give up
    Vec2d oppositeDir = current->get2DPos() - next->get2DPos();
    if (oppositeDir.getShortestAngle(_dir) > getTemplate()->getReverseMoveAngle().Radians())
    {
        return;
    }
    
    // otherwise reverse move
    u32 count = m_moveNodeRefs.size();
    u32 halfCount = count / 2;
    for (u32 i=0; i<halfCount; ++i)
    {
        std::swap(m_moveNodeRefs[i], m_moveNodeRefs[count - i - 1]);
    }
    ITF_ASSERT(count>=2);
    m_moveNodeIndex = count - m_moveNodeIndex - 2;
    m_trajectoryData.reverse();

    // reset speed
    m_speed = m_targetSpeed = 0.0f;
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::stepMove(u32 _index)
{
    ITF_ASSERT(_index < m_moveNodeRefs.size());
    if (_index >= m_moveNodeRefs.size())
    {
        return;
    }

    TrajectoryNodeComponent* nodeComponent0 = NULL;
    if (_index > 0)
    {
        if (Actor* node = m_moveNodeRefs[_index-1].getActor())
        {
            nodeComponent0 = node->GetComponent<TrajectoryNodeComponent>();
        }
    }

    TrajectoryNodeComponent* nodeComponent1 = NULL;
    if (Actor* node = m_moveNodeRefs[_index].getActor())
    {
        nodeComponent1 = node->GetComponent<TrajectoryNodeComponent>();
    }

    TrajectoryNodeComponent* nodeComponent2 = NULL;
    if (m_moveNodeRefs.size() > _index+1)
    {
        if (Actor* node = m_moveNodeRefs[_index+1].getActor())
        {
            nodeComponent2 = node->GetComponent<TrajectoryNodeComponent>();
        }
    }

    TrajectoryNodeComponent* nodeComponent3 = NULL;
    if (m_moveNodeRefs.size() > _index+2)
    {
        if (Actor* node = m_moveNodeRefs[_index+2].getActor())
        {
            nodeComponent3 = node->GetComponent<TrajectoryNodeComponent>();
        }
    }

    m_trajectoryData.init(
        nodeComponent0, // previous
        nodeComponent1, // current
        nodeComponent2, // next
        nodeComponent3  // next next
        );

    m_moveNodeIndex = _index;

    if (m_trajectoryData.getCurrentLink().getType() == TrajectoryLinkType_Jump)
    {
        changeStateInternal(Ray_WM_ControllerState_Jump);
    }
    else if (m_trajectoryData.getCurrentLink().getType() == TrajectoryLinkType_Swim)
    {
        m_reverseForbidden = btrue;
        changeStateInternal(Ray_WM_ControllerState_Swim);
    }
    else if (m_trajectoryData.getCurrentLink().getType() == TrajectoryLinkType_ToFly)
    {
        changeStateInternal(Ray_WM_ControllerState_ToFly);
    }
    else if (m_trajectoryData.getCurrentLink().getType() == TrajectoryLinkType_Fly)
    {
        changeStateInternal(Ray_WM_ControllerState_Fly);
    }
    else
    {
        m_reverseForbidden = btrue;
        changeStateInternal(Ray_WM_ControllerState_Walk);
    }
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::changeStateInternal(Ray_WM_ControllerState _newState, const ActorRef& _newStandNodeRef /*= ActorRef::InvalidRef*/)
{
    // skip if already in given state
    if ((m_controller->getState() == _newState) && (m_standNodeRef == _newStandNodeRef))
    {
        return;
    }
    
    // cleanup old state
    switch (m_controller->getState())
    {
    case Ray_WM_ControllerState_Stand:
    case Ray_WM_ControllerState_StandSwim:
    {
        // get stand node's spot, signal exit
        Ray_WM_SpotComponent* spotComponent = getSpot(m_standNodeRef);
        ITF_ASSERT(spotComponent);
        if (spotComponent)
        {
            spotComponent->onExit();
        }

        m_standNodeRef.invalidate();
    }
    break;
    }
    
    // start new state
    switch (_newState)
    {
    case Ray_WM_ControllerState_Stand:
    case Ray_WM_ControllerState_StandSwim:
    {
        ITF_ASSERT(_newStandNodeRef.isValid());

        // snap on stand node
        Actor* standNode = _newStandNodeRef.getActor();
        ITF_ASSERT(standNode);
        if (standNode)
        {
            Vec3d newPos = standNode->getPos();
            newPos.m_z += getTemplate()->getZOffset();
            m_actor->setPos(newPos);

            // signal entry
            Ray_WM_SpotComponent* spotComponent = standNode->GetComponent<Ray_WM_SpotComponent>();
            ITF_ASSERT(spotComponent);
            if (spotComponent)
            {
                spotComponent->onEnter(m_changedLayer);

                m_changedLayer = bfalse;
            }
        }

        m_standNodeRef = _newStandNodeRef;
        m_trajectoryData.reset();
        m_moveNodeRefs.clear();

        // reset speed
        m_speed = m_targetSpeed = 0.0f;
    }
    break;
    }

    // controller state change
    m_controller->changeState(_newState);
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::startTeleportToLevelSelection()
{
    Actor* standNode = m_standNodeRef.getActor();
    ITF_ASSERT(standNode);
    if (!standNode)
    {
        return;
    }
    LinkComponent* linkComponent = standNode->GetComponent<LinkComponent>();
    ITF_ASSERT(linkComponent);
    if (!linkComponent)
    {
        return;
    }

    // we're supposed to link to a level spot, find it
    bbool found = bfalse;
    AIUtils::LinkIterator it(linkComponent);
    while (Actor* child = it.getNextRelativeActor())
    {
        if (child->GetComponent<Ray_WM_LevelSpotComponent>())
        {
            found = btrue;
            RAY_GAMEMANAGER->triggerWorldMapTeleportSequence(m_standNodeRef,child->getRef());
        }
    }
    ITF_ASSERT_MSG(found, "Can't find level spot");
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::startTeleportToWorldSelection()
{
    // the game manager holds the path to the world spot for us
    ObjectPath path = RAY_GAMEMANAGER->getWMCurrentWorldObjectPath();
    ITF_ASSERT(path.isValid());
    if (Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(path))
    {
        if (Actor* actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
        {
            RAY_GAMEMANAGER->triggerWorldMapTeleportSequence(m_standNodeRef,actor->getRef());
        }
    }
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::Receive( u32 deviceID /* player */, f32 axis, const StringID& action )
{
    const u32 mainIndex = GAMEMANAGER->getMainIndexPlayer_Internal();
    if (mainIndex == U32_INVALID || GAMEMANAGER->isInPause())
    {
        return; 
    }

    if(action==ITF_GET_STRINGID_CRC(WM_ENTER,2267322818))
        m_listenerEnter = axis > 0.f;
    else if(action==ITF_GET_STRINGID_CRC(WM_BACK,4244387740))
        m_listenerBack = axis > 0.f;
    else if(action==ITF_GET_STRINGID_CRC(WM_HOME,1806382459))
        m_listenerHome = axis > 0.f;
    else if(action==ITF_GET_STRINGID_CRC(MOVEX,965019289))
        m_listenerMove.m_x = axis;
    else if(action==ITF_GET_STRINGID_CRC(MOVEY,951160567))
        m_listenerMove.m_y = axis;
    else if(action==ITF_GET_STRINGID_CRC(SPRINT,183769305))
        m_listenerSprint = axis > 0.f;
    else if(action==ITF_GET_STRINGID_CRC(CHEAT_WMUNLOCK,3491146268) && m_listenerCheatActive)
        m_listenerCheatUnlock = btrue;
    else if(action==ITF_GET_STRINGID_CRC(CHEAT_ACTIVE,3130787906))
        m_listenerCheatActive = btrue;
}

//------------------------------------------------------------------------------
void Ray_WM_LeadAbility::clearListener()
{
    m_listenerCheatUnlock = bfalse;
    m_listenerEnter = bfalse;
    m_listenerBack = bfalse;
    m_listenerHome = bfalse;
    m_listenerCheatActive = bfalse;
    m_listenerMove = Vec2d::Zero;
    m_listenerSprint = bfalse;
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_LeadAbility_Template)
BEGIN_SERIALIZATION_CHILD(Ray_WM_LeadAbility_Template)
    SERIALIZE_MEMBER("startNode", m_startNode);
    SERIALIZE_MEMBER("findMoveAngle", m_findMoveAngle);
    SERIALIZE_MEMBER("reverseMoveAngle", m_reverseMoveAngle);
    SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
    SERIALIZE_MEMBER("targetSpeedBlendFactor", m_targetSpeedBlendFactor);
    SERIALIZE_MEMBER("speedBlendFactor", m_speedBlendFactor);
    SERIALIZE_MEMBER("sprintMaxSpeed", m_sprintMaxSpeed);
    SERIALIZE_MEMBER("sprintTargetSpeedBlendFactor", m_sprintTargetSpeedBlendFactor);
    SERIALIZE_MEMBER("sprintSpeedBlendFactor", m_sprintSpeedBlendFactor);
    SERIALIZE_MEMBER("zOffset", m_zOffset);
    SERIALIZE_CONTAINER("followersAdjustOffsets", m_followersAdjustOffsets);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_WM_LeadAbility_Template::Ray_WM_LeadAbility_Template()
    : Super()
    , m_startNode()
    , m_findMoveAngle(btrue, 45.0f)
    , m_reverseMoveAngle(btrue, 45.0f)
    , m_maxSpeed(1.0f)
    , m_targetSpeedBlendFactor(1.0f)
    , m_speedBlendFactor(1.0f)
    , m_sprintMaxSpeed(2.0f)
    , m_sprintTargetSpeedBlendFactor(1.0f)
    , m_sprintSpeedBlendFactor(1.0f)
    , m_zOffset(0.0f)
    , m_followersAdjustOffsets()
{
}

//------------------------------------------------------------------------------
Ray_WM_LeadAbility_Template::~Ray_WM_LeadAbility_Template()
{
}

} // namespace ITF
