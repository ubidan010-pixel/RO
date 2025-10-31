#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHECKPOINTCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CheckpointComponent.h"
#endif //_ITF_RAY_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_


#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_


namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_CheckpointComponent)

BEGIN_SERIALIZATION_CHILD(Ray_CheckpointComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_CONTAINER("SpawnPos",m_spawnPositions);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

Ray_CheckpointComponent::Ray_CheckpointComponent()
: m_animComponent(NULL)
, m_currentPlayerAnimCmp(NULL)
, m_currentStep(-1)
, m_waitTime(1.0f)
, m_timeBetweenRank(1.0f)
, m_displayScoreTime(1.0f)
, m_currentTime(0.f)
, m_state(CPSTATE_NONE)
{
}

Ray_CheckpointComponent::~Ray_CheckpointComponent()
{
    if (m_state != CPSTATE_NONE)
    {
        // Hide all
        displayAllScores(bfalse);

        // Allow to display join message
        RAY_GAMEMANAGER->setHideJoinMessage(bfalse);
    }
}

void Ray_CheckpointComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

    m_waitTime          = getTemplate()->getWaitTime();
    m_timeBetweenRank   = getTemplate()->getTimeBetweenRank();
    m_displayScoreTime  = getTemplate()->getDisplayScoreTime();

    for (u32 i = 0 ; i < getTemplate()->getFakePlayers().size() ; ++i)
    {
        SpawneeGenerator newSpawnee;
        SPAWNER->declareNeedsSpawnee(m_actor, &(newSpawnee), getTemplate()->getFakePlayers()[i]);
        m_spawneeGen.push_back(newSpawnee);
        m_fakeActors.push_back(ActorRef::InvalidRef);
    }

    ITF_ASSERT(m_fakeActors.size() == RewardManager::PlayersMax);
}

bbool sortPlayersByLums(std::pair<u32,u32> _p1, std::pair<u32,u32> _p2) { return _p1.second > _p2.second; }

void Ray_CheckpointComponent::checkpointReached( ObjectRef _player )
{
    // Don't do anything if we're not currently playing the game
    if ( CAMERA->getMode() != Camera::Camera_InGame )
        return;

    if ( m_state != CPSTATE_NONE )
        return;

    // Don't allow to display press join message
    RAY_GAMEMANAGER->setHideJoinMessage(btrue);

    if (getTemplate()->getSavesState())
    {
        if(RAY_GAMEMANAGER->getDebugSaves())
        {
            RAY_GAMEMANAGER->commitLevelStateToPersistentUniverse();

            // mark level as completed if not done already
            StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
            if ( tag != StringID::Invalid )
            {
                ESpot_State state = RAY_GAMEMANAGER->getWMSpotState(tag);
                if (state != SPOT_STATE_COMPLETED)
                {
                    RAY_GAMEMANAGER->setWMSpotState(tag, SPOT_STATE_COMPLETED);

                    // after a completed shooter map
                    // - mark the current world as completed too
                    // - go to the *world* selection (= invalidate current WM level)
                    if (s_MapConfig_Shooter == RAY_GAMEMANAGER->getMapConfigType(tag))
                    {
                        RAY_GAMEMANAGER->setWMSpotState(RAY_GAMEMANAGER->getWMCurrentWorldTag(), SPOT_STATE_COMPLETED);
                        RAY_GAMEMANAGER->setWMCurrentLevel(ObjectPath(), StringID::Invalid); 
                    }
                }
            }

            RAY_GAMEMANAGER->saveGameState(bfalse);
        }
        
        RAY_GAMEMANAGER->endOfLevelReached();
    }

    if ( getTemplate()->getSequence().isEmpty() )
    {
        RAY_GAMEMANAGER->onReachedCheckpoint(GetActor()->getRef());
    }
    else
    {
        //indirectly calls onReachedCheckpoint
        RAY_GAMEMANAGER->triggerCheckpointSequence(GetActor()->getRef(),_player,getTemplate()->getSequence());

        // Start ranking display
        REWARD_MANAGER->Session_Stop();

        // Sort players
        RewardManager::GameSession * gameSession = REWARD_MANAGER->Session_GetPrevious();
        m_sortedPlayers.clear();
        for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
        {
            if ( gameSession->m_actions[i].m_actif )
                m_sortedPlayers.push_back(std::pair<u32,u32>(i,RAY_GAMEMANAGER->getCurrentScore().getLums(i)));
        }

        // Sort players by lums score
        std::sort(m_sortedPlayers.begin(), m_sortedPlayers.end(), sortPlayersByLums);

        controlPlayers();

        // Display score page (will not display anything until we ask to
        RAY_GAMEMANAGER->goToCheckpointScore();

        m_state = CPSTATE_WAITING_CAMERA;
    }



}

void Ray_CheckpointComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);


    switch( m_state )
    {
    case CPSTATE_NONE:
        break;
    case CPSTATE_WAITING_CAMERA:
        {
            m_currentTime += _deltaTime;
            if ( m_currentTime >= m_waitTime )
            {
                m_currentTime = 0.0f;

                if ( !playNextStep() )
                    ITF_ASSERT(0);
                m_state = CPSTATE_WAITING_ANIMATION;
            }
        }
        break;
    case CPSTATE_WAITING_ANIMATION:
        {
            ITF_ASSERT(m_currentPlayerAnimCmp != NULL);
            if ( m_currentPlayerAnimCmp!= NULL && m_currentPlayerAnimCmp->isSubAnimFinished() )
            {
                // Display Score
                displayScore(btrue);
                m_state = CPSTATE_DISPLAY_SCORE;
            }
        }
        break;
    case CPSTATE_DISPLAY_SCORE:
        {
            m_currentTime += _deltaTime;
            if ( m_currentTime >= m_timeBetweenRank )
            {
                m_currentTime = 0.0f;
                if ( playNextStep() )
                    m_state = CPSTATE_WAITING_ANIMATION;
                else
                {
                    // Hide all scores
                    displayAllScores(bfalse);

                    RAY_GAMEMANAGER->resumeCheckpointSequence();
                    m_state = CPSTATE_TAKE_PICTURE;
                }
            }
        }
        break;
    case CPSTATE_TAKE_PICTURE:
        {
            m_currentTime += _deltaTime;
            if ( m_currentTime >= 1.0f )
            {
                // Show continue
                //displayAllScores(btrue);
                displayContinue(btrue);

                m_currentTime = 0.0f;

                m_state = CPSTATE_WAITING_EXIT;
            }
        }
        break;
    case CPSTATE_WAITING_EXIT:
        {
            m_currentTime += _deltaTime;
            if ( m_currentTime >= 1.0f )
            {
                // Check for pads
                for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    // Buttons
                    InputAdapter::PressStatus buts[JOY_MAX_BUT];
                    INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buts, JOY_MAX_BUT);

                    if( buts[m_joyButton_A] == InputAdapter::JustPressed )
                    {
                        // Hide all
                        displayAllScores(bfalse);

                        // Allow to display join message
                        RAY_GAMEMANAGER->setHideJoinMessage(bfalse);

                        // Commented to avoid to seen characters disappearing right before loading next map
                        //releasePlayers();

                        RAY_GAMEMANAGER->resumeCheckpointSequence();

                        // Display level stats page
                        //RAY_GAMEMANAGER->goToLevelStats();

                        if(RAY_GAMEMANAGER->getDebugSaves())
                            RAY_GAMEMANAGER->resetToWorldMap();
                        else
                            RAY_GAMEMANAGER->goToTheMainMenu();                        

                        m_state = CPSTATE_NONE;
                    }
                }
            }
        }
        break;
    default:
        ITF_ASSERT(0);
    }
}

void Ray_CheckpointComponent::displayScore(bbool _show)
{
    if ( m_currentStep < 0 )
        return;

    if ( _show )
    {
        String8 fieldName, valueString;

        fieldName.setTextFormat("LumCounter%d",m_currentStep+1);
        valueString.setTextFormat("%d",m_sortedPlayers[m_currentStep].second);

        // Send event to set text
        Ray_EventHUDSetText setText(fieldName,valueString);
        EVENTMANAGER->broadcastEvent(&setText);
    }

    // Send event to display/hide score
    Ray_EventHUDDisplayElem displayElem(m_currentStep,_show);
    EVENTMANAGER->broadcastEvent(&displayElem);
}

void Ray_CheckpointComponent::displayAllScores(bbool _show)
{

    if ( !_show )
    {
        for ( u32 i = 0 ; i < m_sortedPlayers.size() ; ++i )
        {
            // Send event to display/hide score
            Ray_EventHUDDisplayElem displayElem(i,_show);
            EVENTMANAGER->broadcastEvent(&displayElem);
        }
    }
}

void Ray_CheckpointComponent::displayContinue(bbool _show)
{
    // There is a fifth element: next
    Ray_EventHUDDisplayElem displayElem(4,_show);
    EVENTMANAGER->broadcastEvent(&displayElem);

#ifdef ITF_PS3
    // PS3 X button
    Ray_EventHUDDisplayElem buttonElem(5,_show);
    EVENTMANAGER->broadcastEvent(&buttonElem);
#else
    // XBOX A button
    Ray_EventHUDDisplayElem buttonElem(6,_show);
    EVENTMANAGER->broadcastEvent(&buttonElem);
#endif
}

void Ray_CheckpointComponent::controlPlayers()
{
    AnimLightComponent* animCmp = m_actor->GetComponent<AnimLightComponent>();
    ITF_ASSERT(animCmp != NULL);

    const ITF_VECTOR<StringID>& anims       = getTemplate()->getAnimationsByRank();
    const ITF_VECTOR<StringID>& boneNames   = getTemplate()->getBoneNamesByRank();

    EventStopPlayer stopPlayerEvt(btrue);
    f32     newZ = m_actor->getPos().m_z - 0.023f;

    for ( u32 i = 0 ; i < m_sortedPlayers.size() ; i++ )
    {
        u32 playerIndex = m_sortedPlayers[i].first;

        Actor * realPlayer = GAMEMANAGER->getActivePlayer(m_sortedPlayers[i].first);
        ITF_ASSERT(realPlayer != NULL);
        if ( realPlayer != NULL )
        {
            // Spawn fake player
            Actor * fakePlayer = NULL;
            if ( !m_fakeActors[playerIndex].isValid() )
            {
                fakePlayer = m_spawneeGen[playerIndex].getSpawnee( m_actor->getScene(),m_actor->getPos());
                ITF_ASSERT(fakePlayer != NULL);
                if ( fakePlayer != NULL )
                    m_fakeActors[playerIndex] = fakePlayer->getRef();
            }
            else
            {
                fakePlayer = m_fakeActors[playerIndex].getActor();
            }

            if ( fakePlayer == NULL )
                return;

            // Get its animation component
            AnimLightComponent* currentFakeAnimCmp      = fakePlayer->GetComponent<AnimLightComponent>();
            ITF_ASSERT(currentFakeAnimCmp != NULL);
            if ( currentFakeAnimCmp != NULL )
            {
                // Send event to stop players
                realPlayer->onEvent(&stopPlayerEvt);
                realPlayer->disable();

                if ( i == 3 )
                    fakePlayer->setFlipped(btrue);
                else
                    fakePlayer->setFlipped(bfalse);

                // Move player to its position
                if ( i < boneNames.size() )
                {
                    u32 boneIndex = animCmp->getBoneIndex(boneNames[i]);
                    ITF_ASSERT(boneIndex != U32_INVALID);
                    Vec3d destination = Vec3d::Zero;
                    if ( animCmp->getBonePos(boneIndex,destination) )
                    {
                        destination.m_z = newZ;
                        fakePlayer->setPos(destination);
                        realPlayer->setPos(destination);
                    }
                }

                if ( i < anims.size() )
                {
                    currentFakeAnimCmp->resetSubAnimsToPlay();
                    currentFakeAnimCmp->addSubAnimToPlay(anims[i], 1.0f, btrue);
                    currentFakeAnimCmp->commitSubAnimsToPlay();
                    currentFakeAnimCmp->setAnim(StringID::Invalid);
                    currentFakeAnimCmp->setPlayRate(0.0f);
                    currentFakeAnimCmp->setAnimLoop(bfalse);
                }
            }
        }
    }
}

void Ray_CheckpointComponent::releasePlayers()
{
    EventStopPlayer stopPlayerEvt(bfalse);

    for ( u32 i = 0 ; i < m_sortedPlayers.size() ; i++ )
    {
        u32 playerIndex = m_sortedPlayers[i].first;

        Actor * fakeActor   = NULL;
        if ( i < m_fakeActors.size() )
            fakeActor = m_fakeActors[playerIndex].getActor();
        Actor * realActor   = GAMEMANAGER->getActivePlayer(m_sortedPlayers[i].first);
        ITF_ASSERT(realActor != NULL && fakeActor != NULL);
        if ( realActor != NULL && fakeActor != NULL )
        {
            realActor->enable();

            // Get its animation component
            AnimLightComponent* realAnimCmp = realActor->GetComponent<AnimLightComponent>();
            ITF_ASSERT(realAnimCmp != NULL);
            if ( realAnimCmp != NULL )
            {
                realActor->onEvent(&stopPlayerEvt);
            }

            fakeActor->disable();
        }
    }
}

bbool Ray_CheckpointComponent::playNextStep()
{
    if ( m_currentStep < 0 )
        m_currentStep = m_sortedPlayers.size() - 1;
    else
        m_currentStep--;

    if ( m_currentStep < 0 )
        return bfalse;
    // Get actor associated to this player
    if ( m_currentStep >= i32(m_fakeActors.size()) )
        return bfalse;
    Actor* actor = m_fakeActors[m_sortedPlayers[m_currentStep].first].getActor();
    ITF_ASSERT(actor != NULL);
    if ( actor != NULL )
    {
        // Get its animation component
        m_currentPlayerAnimCmp = actor->GetComponent<AnimLightComponent>();
        ITF_ASSERT(m_currentPlayerAnimCmp != NULL);
        if ( m_currentPlayerAnimCmp != NULL )
        {
            m_currentPlayerAnimCmp->setPlayRate(1.0f);
            return btrue;
        }
    }

    return bfalse;
}

Vec2d Ray_CheckpointComponent::getPlayerSpawnPos( u32 _playerIndex ) const
{
    if ( m_animComponent )
    {
        u32 numPoints = 0;
        Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(ITF_GET_STRINGID_CRC(Poly_PlayerPos,557395158), &numPoints);

        if ( points && numPoints )
        {
            // store the points in case the anim is unloaded later and we need to teleport
            m_spawnPositions.resize(numPoints);

            for ( u32 i = 0; i < numPoints; i++ )
            {
                m_spawnPositions[i] = points[i];
            }

            return points[Min(_playerIndex,numPoints-1)];
        }
    }
    else if ( m_spawnPositions.size() )
    {
        return m_spawnPositions[Min(_playerIndex,m_spawnPositions.size()-1)];
    }

    return Super::getPlayerSpawnPos(_playerIndex);
}

void Ray_CheckpointComponent::onEvent( Event * _event )
{
    bbool previousActivationState = getActive();
    Super::onEvent(_event);

    //For restoring the right animation if activation changed because of another checkpoint
    if (m_animComponent && !previousActivationState && getActive())
    {
        m_animComponent->setAnim(m_animComponent->getDefaultAnimation());
    }

    //
    if (EventTrigger *trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (trigger->getActivated())
        {
            m_actor->enable();
        }
        else
        {
            m_actor->disable();
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_CheckpointComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_CheckpointComponent_Template)

    SERIALIZE_MEMBER("Sequence", m_sequencePath);
    SERIALIZE_MEMBER("commitGameState", m_savesState);
    SERIALIZE_MEMBER("waitTime",m_waitTime);
    SERIALIZE_MEMBER("timeBetweenRank",m_timeBetweenRank);
    SERIALIZE_MEMBER("displayScoreTime",m_displayScoreTime);
    SERIALIZE_CONTAINER("startAnimations",m_startAnimations);
    SERIALIZE_CONTAINER("animationsByRank",m_animationsByRank);
    SERIALIZE_CONTAINER("boneNamesByRank",m_boneNamesByRank);
    SERIALIZE_CONTAINER("fakePlayers",m_fakePlayers);

END_SERIALIZATION()

}