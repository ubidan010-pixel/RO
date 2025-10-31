#include "precompiled_gameplay_rayman.h"
#ifndef _ITF_STANDEECOMPONENT_H_
#include "engine/actors/components/StandeeComponent.h"
#endif
#ifndef _ITF_RAY_AIMEDALBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Medal/Ray_AIMedalBehavior.h"
#endif //_ITF_RAY_AIMEDALBEHAVIOR_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_


#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_ENTERDOORCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_EnterDoorComponent.h"
#endif //_ITF_RAY_ENTERDOORCOMPONENT_H_

#ifndef _ITF_AFTERFXCOMPONENT_H_
#include "engine/actors/components/AfterFxComponent.h"
#endif //_ITF_AFTERFXCOMPONENT_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_


namespace ITF
{
    #define MEDAL_IDLE_ANIM ITF_GET_STRINGID_CRC(Idle,1633200266)
    #define MEDAL_APPEARS_ANIM ITF_GET_STRINGID_CRC(Appear,1975175726)
    #define MEDAL_READD_ANIM ITF_GET_STRINGID_CRC(ReAdd,931967062)
    #define MEDAL_APPEARS_FINISH ITF_GET_STRINGID_CRC(Finish,2033741536)
    #define MEDAL_APPEARS_INPUT ITF_GET_STRINGID_CRC(medalAppears,2832006116)
    #define MEDAL_ELECTOON_PRESENT_FORMAT       "e%dPresence"
    #define MEDAL_ELECTOON_APPEARS_FORMAT       "e%dAppears"
    #define MEDAL_ELECTOON_BOUNCES_FORMAT       "e%dBounces"
    #define MEDAL_COMPLETED_SOUNDINPUT ITF_GET_STRINGID_CRC(medalCompleted,738589224)

    #define MEDAL_SCALE                         0.8f


    static u8 instanceCount = 0;
    bbool Ray_AIMedalBehavior::m_isSequencePlaying = bfalse;

	IMPLEMENT_OBJECT_RTTI(Ray_AIMedalBehavior_Template)
	BEGIN_SERIALIZATION_CHILD(Ray_AIMedalBehavior_Template)
    END_SERIALIZATION()

    Ray_AIMedalBehavior_Template::Ray_AIMedalBehavior_Template()
    {
    }

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIMedalBehavior)
    END_VALIDATE_BEHAVIOR()

    IMPLEMENT_OBJECT_RTTI(Ray_AIMedalBehavior)

    Ray_AIMedalBehavior::Ray_AIMedalBehavior()
        : Super()
		, m_state(State_NA)
        , m_cineActorRef(ITF_INVALID_OBJREF)
        , m_lastCallingActor(ITF_INVALID_OBJREF)
        , m_timeInCurrentState(0.0f)
        , m_justBrokenCage(bfalse)
        , m_justBeatenBoss(bfalse)
        , m_playVictorySequence(bfalse)
        , m_playEndSequence(bfalse)
        , m_autoEndLevel(bfalse)
        , m_forTimeAndLumAttacks(bfalse)
        , m_dontSetTimeAttack(bfalse)
        , m_dontSetLumAttack1(bfalse)
        , m_dontSetLumAttack2(bfalse)
        , m_newElectoonMask(0)
        , m_oldElectoonMask(0)
        , m_electoonsToMakeAppear(0)
        , m_alreadyPresentElectoonsToEmphasize(0)
		, m_processEndLevel(bfalse)
		, m_processSwitchEndLevel(bfalse)
        , m_currentElectoonAlreadyExisted(bfalse)
    {
        instanceCount += 1;
    }

    Ray_AIMedalBehavior::~Ray_AIMedalBehavior()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonMedal,1324502185), m_aiComponent);
        freeUnusedCineActor(btrue);
        instanceCount -= 1;
    }

	void Ray_AIMedalBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);

        m_currentElectoonAlreadyExisted = bfalse;

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_aiComponent);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonMedal,1324502185), m_aiComponent);
        Actor *oldMedal = RAY_GAMEMANAGER->getMedalActor();
        if (oldMedal && oldMedal!=m_actor)
        {
            if (SYSTEM_ADAPTER->messageBox("CRITICAL ERROR",
                "The medal actor is present twice.\n"
                "Deleting the new actor.\n"
                "If you opened an incorrect scene, you should save it again.",
                ITF_MSG_ICONERROR | ITF_MSG_OK,
                ITF_IDNO
                ) == ITF_IDYES)
            {
                m_actor->requestDestruction();
            }

        }
        else
        {
            RAY_GAMEMANAGER->setMedalActor(m_actor);
        }

        m_isSequencePlaying = bfalse;
        m_justBrokenCage = bfalse;
        m_justBeatenBoss = bfalse;
        m_forTimeAndLumAttacks = bfalse;
        m_cineActorRef.invalidate();

        setState(State_Preappears);
        m_animComponent->setRank2D(UI_TEXTMANAGER->getScoreAnimDepthRank());

	}

	void Ray_AIMedalBehavior::onResourceReady()
	{
		Super::onResourceReady();

		setState(State_Preappears);
    }

	void Ray_AIMedalBehavior::update( f32 _deltaTime )
	{
        ITF_ASSERT_CRASH( instanceCount == 1, "Too many instance" );
        Super::update(_deltaTime);

		m_timeInCurrentState += _deltaTime;

        freeUnusedCineActor(bfalse);

		switch (m_state)
		{
            case State_Preappears:
            {
            }break;
			case State_Appears:
            case State_ReAdd:
			{
			} break;
			case State_AddingElectoon:
			{
			} break;

            case State_Finish:
            {
            } break;

			case State_JustDisappeared:
			{
            } break;

            default:
                ITF_ASSERT(bfalse);
                break;

		}

		if (m_processEndLevel && !m_processSwitchEndLevel)
		{
			processEndLevel();
		}
	}

    void Ray_AIMedalBehavior::processShowMedalEvent(Ray_EventShowElectoonMedal *_showMedalEvent)
    {
        m_lastCallingActor = _showMedalEvent->getSender();
        m_justBrokenCage = _showMedalEvent->m_forBrokenCage;
        m_justBeatenBoss =_showMedalEvent->m_forBeatenBoss;
        m_playVictorySequence   = _showMedalEvent->m_playVictorySequence;
        m_playEndSequence       = _showMedalEvent->m_playEndSequence;
        m_autoEndLevel          = _showMedalEvent->m_autoEndLevel;
        m_forTimeAndLumAttacks = _showMedalEvent->m_forLumAndTimeAttacks;
        m_dontSetTimeAttack = _showMedalEvent->m_dontSetTimeAttack;
        m_dontSetLumAttack1 = _showMedalEvent->m_dontSetLumAttack1;
        m_dontSetLumAttack2 = _showMedalEvent->m_dontSetLumAttack2;

        m_customCineActorRef = _showMedalEvent->m_customCineRef;

        // preload end level
        if ( m_autoEndLevel )
        {
            m_preloadedEndLevelMapRef = RAY_GAMEMANAGER->preloadEndLevelMap();
        }

        Actor *act = m_lastCallingActor.getActor();
        Actor *emphasizedCage = NULL;
        if (m_justBrokenCage)
        {
            if (act)
            {
                m_lastCallingCageId = act->getObjectId();
                emphasizedCage = act;
            }
            else
            {
                m_lastCallingCageId.invalidate();
            }
        }
        else
        {
            m_lastCallingCageId.invalidate();
        }

        bbool wasCompleted = bfalse;
        m_oldElectoonMask = computeElectoonMask(NULL,0,&wasCompleted);
        u32 emphasizedChallenge = _showMedalEvent->m_currentlyEmphasizedChallenge;
        if (m_justBeatenBoss)
        {
            emphasizedChallenge |= CHALLENGE_BEATENBOSS;
        }
        u32 electoonMaskWithoutEmphasized = computeElectoonMask(emphasizedCage,emphasizedChallenge);


        if (m_justBrokenCage)
        {
            if (act)
            {
                RAY_GAMEMANAGER->setIsCageBroken(act, btrue);
            }
        }
        if (m_forTimeAndLumAttacks)
        {
            updateChallengeElectoons();
        }
        if (m_justBeatenBoss)
        {
            updateBeatenBossELectoon();
        }

        bbool isCompleted = bfalse;
        m_newElectoonMask = computeElectoonMask(NULL, 0, &isCompleted);
        m_electoonsToMakeAppear = m_newElectoonMask & ~m_oldElectoonMask;
        m_alreadyPresentElectoonsToEmphasize = m_oldElectoonMask & ~electoonMaskWithoutEmphasized;
        m_electoonsToMakeAppear |= m_alreadyPresentElectoonsToEmphasize;

        if (m_electoonsToMakeAppear!=0)
        {
            setState(State_Appears);
        }
        bbool makeCompletedSound = bfalse;
        if (isCompleted)
        {
            if (!wasCompleted)
            {
                makeCompletedSound = btrue;
                u32 medalCount = RAY_GAMEMANAGER->computeCompleteMedalCount();
                REWARD_MANAGER->Action_Set(Ray_RewardManager::Action_MedalCompleted, GAMEMANAGER->getMainIndexPlayer(), f32(medalCount));
            }
        }
        m_soundComponent->setInput(MEDAL_COMPLETED_SOUNDINPUT, (makeCompletedSound?1.0f:0.0f));

    }

    void Ray_AIMedalBehavior::onEvent( Event* _event )
    {
        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( animEvent->getName() == "MRK_Electoon_Appears_Gameplay")
            {
                setState(State_AddingElectoon);
            }
            else if ( animEvent->getName() == "MRK_End_Gameplay")
            {
                setState(State_JustDisappeared);
            }

            //exiting of  adding electoon
            if (m_currentElectoonAlreadyExisted)
            {
                if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Turning_Stop_Gameplay,3865393881))
                {
                    if (m_state==State_AddingElectoon)
                    {
                        if (m_electoonsToMakeAppear!=0)
                        {
                            setState(State_ReAdd);
                        }
                        else
                        {
                            setState(State_Finish);
                        }
                    }
                }
            }
            else
            {
                if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Shaking_Stop_Gameplay,3810225474))
                {
                    if (m_state==State_AddingElectoon)
                    {
                        if (m_electoonsToMakeAppear!=0)
                        {
                            setState(State_ReAdd);
                        }
                        else
                        {
                            setState(State_Finish);
                        }
                    }
                }
            }
        }
        else if (Ray_EventShowElectoonMedal *showMedalEvent = _event->DynamicCast<Ray_EventShowElectoonMedal>(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonMedal,1324502185)))
        {
            processShowMedalEvent(showMedalEvent);
        }

        Super::onEvent(_event);
    }

    void Ray_AIMedalBehavior::pauseWorld(bbool _pause)
    {
        if (_pause)
            RAY_GAMEMANAGER->requestGameplayPause();
        else
            RAY_GAMEMANAGER->requestGameplayUnpause();
    }

    void Ray_AIMedalBehavior::setElectoonPresence(u32 _index, bbool _present)
    {
        char buffer[256];
        f32 presenceWeight = _present?1.f:0.f;
        sprintf(buffer, MEDAL_ELECTOON_PRESENT_FORMAT, _index+1);
        m_animatedComponent->setInput(buffer, presenceWeight);
    }

    void Ray_AIMedalBehavior::setElectoonAppears(u32 _index, bbool _appears)
    {
        char buffer[256];
        f32 appearsWeight = _appears?1.f:0.f;
        sprintf(buffer, MEDAL_ELECTOON_APPEARS_FORMAT, _index+1);
        m_animatedComponent->setInput(buffer, appearsWeight);
    }

    void Ray_AIMedalBehavior::setElectoonBounces(u32 _index, bbool _bounces)
    {
        char buffer[256];
        f32 appearsWeight = _bounces?1.f:0.f;
        sprintf(buffer, MEDAL_ELECTOON_BOUNCES_FORMAT, _index+1);
        m_animatedComponent->setInput(buffer, appearsWeight);
    }

    void Ray_AIMedalBehavior::freeUnusedCineActor(bbool _bforce)
    {
        // Reset values set for this sequence on CinematicManager
        CINEMATICMANAGER->resetPlayersChannels();

        Actor *actor = m_cineActorRef.getActor();
        if (actor)
        {
            SequencePlayerComponent *sequence = actor->GetComponent<SequencePlayerComponent>();
            ITF_WARNING(m_actor, sequence!=NULL, "invalid SequencePlayerComponent");
            if (!sequence)
            {
                return;
            }
            bbool sequenceFinished = sequence->getState()==SequencePlayerComponent::State_Stopped;
            if (_bforce || sequenceFinished)
            {
                if (actor && actor->getScene())
                {
                    actor->requestDestruction();
                    m_cineActorRef.invalidate();
                    m_isSequencePlaying = bfalse;
                    //when sequence finished, send a triggered event to actors pointed by linkcomponent
                    if (sequenceFinished)
                    {
                        Actor *caller = m_lastCallingActor.getActor();
                        if (caller)
                        {
                            Ray_EventMedalCinematicEnded cineEnded;
                            caller->onEvent(&cineEnded);
                        }

                        if ( m_autoEndLevel )
                        {
							m_processEndLevel = btrue;
                        }
                    }
                }
            }
        }
    }

    void Ray_AIMedalBehavior::processEndLevel()
    {
        if(GAMEMANAGER->isPreloadedMapLoaded(m_preloadedEndLevelMapRef))
        {
		    RAY_GAMEMANAGER->endOfLevelReached();

            // Switch to next map
            RAY_GAMEMANAGER->switchToPreloadedEndLevelMap();
            World* levelEnding = IRTTIObject::SafeDynamicCast<World>(m_preloadedEndLevelMapRef.getObject(),
                                                             ITF_GET_STRINGID_CRC(World, 3307309411));
            FindStandeeActor(levelEnding);
            m_processSwitchEndLevel = btrue;
        }
    }

    void Ray_AIMedalBehavior::FindStandeeActor(World* world)
    {
        if (world != nullptr)
        {
            for (u32 i = 0; i < world->getSceneCount(); i++)
            {
                Scene* scene = world->getSceneAt(i);
                if (scene)
                {
                    auto pickables = scene->getActors();
                    for (Pickable* pickable : pickables)
                    {
                        const Actor* actor = static_cast<const Actor*>(pickable);
                        if (actor != nullptr)
                        {
                            StandeeComponent* standee = actor->GetComponent<StandeeComponent>();
                            ChangeStandeePicture(standee);
                        }
                    }
                }
            }
        }
    }

    void Ray_AIMedalBehavior::ChangeStandeePicture(StandeeComponent* _standee)
    {
        if (_standee)
        {
            StringID levelName = RAY_GAMEMANAGER->getCurrentLevelName();
            _standee->ChangeStandee(levelName);
        }
    }

    void Ray_AIMedalBehavior::prepareVictorySequence()
    {
        // Sort players
        RewardManager::GameSession * gameSession = REWARD_MANAGER->Session_GetCurrent();

        ITF_VECTOR< std::pair<u32,i32> > sortedPlayers;
        for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
        {
            if ( gameSession->m_actions[i].m_actif )
                sortedPlayers.push_back(std::pair<u32,i32>(i,static_cast<i32>(RAY_GAMEMANAGER->getCurrentScore().getLums(i))));
            else
                sortedPlayers.push_back(std::pair<u32,i32>(i, -1));
        }

        // Sort players by lums score
        std::sort(sortedPlayers.begin(), sortedPlayers.end(), AIUtils::sortPlayersByScore);

        for ( u32 i = 0 ; i < sortedPlayers.size() ; ++i )
        {
            CINEMATICMANAGER->setPlayerChannel(sortedPlayers[i].first, i);
        }
    }

    void Ray_AIMedalBehavior::playVictorySequence()
    {
        Actor *callingActor = m_lastCallingActor.getActor();
        if (callingActor && (m_playVictorySequence || m_playEndSequence || m_customCineActorRef.isValid() ) )
        {
            freeUnusedCineActor(btrue);

            Actor *cineActor = NULL;
            if ( m_customCineActorRef.isValid() )
            {
                cineActor = m_customCineActorRef.getActor();
                m_customCineActorRef.invalidate();
            }
            else
                cineActor = NULL;

            if (cineActor)
            {
                // Compute players ranking depending on lums earned on each players
                prepareVictorySequence();

                m_cineActorRef = cineActor->getRef();
                m_isSequencePlaying = bfalse;
                SequencePlayerComponent *sequence = cineActor->GetComponent<SequencePlayerComponent>();
                if (sequence)
                {
                    ObjectRefList* actorList = sequence->getSpawnActors();
                   const u32 spawnActorCount = actorList->size();
                    for (u32 i = 0; i < spawnActorCount; i++)
                    {
                        auto objRef = (*actorList)[i];
                        if (objRef == NULL) continue;
                        Actor* actor = static_cast<Actor*>(objRef.getObject());
                        if (actor == nullptr) continue;
                        if (StandeeComponent* standee = actor->GetComponent<StandeeComponent>())
                        {
                            ChangeStandeePicture(standee);
                        }
                    }
                    EventSequenceControl playEvent;
                    playEvent.setSender(m_actor->getRef());
                    playEvent.setState(SequencePlayerComponent::State_Playing);
                    m_isSequencePlaying = btrue;
                    sequence->onEvent(&playEvent);
                }
            }
            else
            {
                m_cineActorRef.invalidate();
                m_isSequencePlaying = bfalse;
            }
        }
    }

    void Ray_AIMedalBehavior::updateScaleAndPosition()
    {
        Vec2d p(GFX_ADAPTER->getScreenWidth()/2.f, GFX_ADAPTER->getScreenHeight()/2.f);
        f32 scaleX = (f32)GFX_ADAPTER->getScreenWidth();
        f32 scaleY = (f32)GFX_ADAPTER->getScreenHeight();
        f32 scale = std::min(scaleX, scaleY)*MEDAL_SCALE;
        m_actor->set2DPos(p);
        m_actor->setScale(Vec2d(scale,scale));
    }

    void Ray_AIMedalBehavior::restoreDeadPlayers()
    {
        for (u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player *player = (Ray_Player*)RAY_GAMEMANAGER->getPlayer(i);
            if (player && player->getActive())
            {
                if (player->isDead())
                    player->revive();

                GAMEMANAGER->changePlayerGameMode(player,RAY_GAMEMODE_PLATFORMER);
            }
        }

    }

    u32 Ray_AIMedalBehavior::computeElectoonMask(Actor *_ignoredCage, u32 _ignoreChallenges, bbool* _isMedalComplete)
    {
        StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
        StringID type = RAY_GAMEMANAGER->getMapConfigType(tag);
#ifndef ITF_FINAL
        // To be able to test it with a test map
        if ( !type.isValid() )
            type = "platformer";
#endif ITF_FINAL

        bbool isComplete;
        u32 mask = Ray_AIUtils::computeElectoonMask(type, RAY_GAMEMANAGER->getCurrentLevelData(), _ignoredCage, _ignoreChallenges, isComplete);
        if(_isMedalComplete)
            *_isMedalComplete = isComplete;
        return mask;
    }


    void Ray_AIMedalBehavior::updateBeatenBossELectoon()
    {
        if (m_justBeatenBoss)
        {
            RAY_GAMEMANAGER->setChallengeWon(CHALLENGE_BEATENBOSS);
        }
    }

    void Ray_AIMedalBehavior::updateChallengeElectoons()
    {
        World *world = CURRENTWORLD;
        if (!world)
            return;

        Scene *scene = world->getRootScene();
        if (!scene)
            return;

        if (m_forTimeAndLumAttacks)
        {
            RewardManager::GameSession * gameSession = NULL;
            i32 lumCount=0;

            if (!m_dontSetLumAttack1 || !m_dontSetLumAttack2)
            {
                gameSession = REWARD_MANAGER->Session_GetPrevious();
            }
            i32 sessionTime = static_cast<i32>(RAY_GAMEMANAGER->getTimeAttackTimer());

            if (gameSession)
            {
                //compute lum count
                for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
                {
                    if ( REWARD_MANAGER->GetIsPlayerActif(gameSession, i) )
                    {
                        lumCount += RAY_GAMEMANAGER->getCurrentScore().getLums(i);
                    }
                }
            }

            StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
            i32 lumAttack1Threshold = RAY_GAMEMANAGER->getLumAttackThreshold(tag,0);
            i32 lumAttack2Threshold = RAY_GAMEMANAGER->getLumAttackThreshold(tag,1);
            i32 timeAttackThreshold = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, bfalse);
            i32 timeAttackThresholdForCup = RAY_GAMEMANAGER->getTimeAttackThreshold(tag, btrue);

            if ( timeAttackThreshold == -1 )
            {
                timeAttackThreshold = 90;
            }

            if ( timeAttackThresholdForCup == -1 )
            {
                timeAttackThresholdForCup = 60;
            }

            //time attack
            if (RAY_GAMEMANAGER->isTimeAttack() && (timeAttackThreshold>=sessionTime) && !m_dontSetTimeAttack)
            {
                RAY_GAMEMANAGER->setChallengeWon(CHALLENGE_TIMEATTACK);
                if (timeAttackThresholdForCup>=sessionTime)
                {
                    RAY_GAMEMANAGER->setChallengeWon(CHALLENGE_CUP);
                }
            }
            //lum attack
            if (!m_dontSetLumAttack1 || !m_dontSetLumAttack2)
            {
                if (lumAttack1Threshold<=lumCount)
                {
                    if (!m_dontSetLumAttack1)
                    {
                        RAY_GAMEMANAGER->setChallengeWon(CHALLENGE_LUMATTACK1);
                    }
                }

                if (lumAttack2Threshold<=lumCount)
                {
                    if (!m_dontSetLumAttack2)
                    {
                        RAY_GAMEMANAGER->setChallengeWon(CHALLENGE_LUMATTACK2);
                    }
                }
            }
        }
    }

    u32 Ray_AIMedalBehavior::computeLowestBit(u32 mask)
    {
        for (u32 bit=0; bit!=32; bit++)
        {
            u32 test = mask&(1<<bit);
            if (test)
            {
                return test;
            }
        }
        return 0;
    }

    void Ray_AIMedalBehavior::setupOldElectoonsAsAlreadyPresent()
    {
        for (u32 i=0; i<MEDAL_ELECTOON_COUNT; i++)
        {
            setElectoonPresence(i,(m_oldElectoonMask&(1<<i)));
            setElectoonAppears(i,bfalse);
            setElectoonBounces(i,bfalse);
        }
    }


	void Ray_AIMedalBehavior::setState(State_t _state)
	{
		if (_state!=m_state)
		{
			m_timeInCurrentState = 0;
			m_state = _state;
		}

        if (!m_actor)
            return;

        m_animatedComponent->enableDraw(_state!=State_Preappears);
        if (m_state!=State_NA && m_state!=State_Preappears)
        {
            updateScaleAndPosition();
        }

        bbool canShowMedal = (_state==State_Appears || _state==State_AddingElectoon || _state==State_Finish || _state==State_ReAdd);
        m_animatedComponent->setInput(MEDAL_APPEARS_INPUT, canShowMedal?1.f:0.f);

		switch(_state)
		{
            case State_Preappears:
            {
                m_animatedComponent->setAnim(MEDAL_IDLE_ANIM);
            }break;
		    case State_Appears:
		    {
                setupOldElectoonsAsAlreadyPresent();
                pauseWorld(btrue);
                m_animatedComponent->setAnim(MEDAL_APPEARS_ANIM);

		    } break;
		    case State_AddingElectoon:
		    {
                u32 justSetMask = computeLowestBit(m_electoonsToMakeAppear);
                m_oldElectoonMask = m_oldElectoonMask | justSetMask;
                m_electoonsToMakeAppear &= ~justSetMask;
                bbool isEmphasized = justSetMask & m_alreadyPresentElectoonsToEmphasize;

                m_alreadyPresentElectoonsToEmphasize &= ~justSetMask;
                m_currentElectoonAlreadyExisted = isEmphasized;

                for (u32 i=0; i<MEDAL_ELECTOON_COUNT; i++)
                {
                    if (justSetMask&(1<<i))
                    {
                        setElectoonPresence(i,bfalse);
                        setElectoonBounces(i,isEmphasized);
                        setElectoonAppears(i,!isEmphasized);
                        //setElectoonAppears(i,btrue);
                    }
                    else
                    {
                        setElectoonBounces(i,bfalse);
                        setElectoonAppears(i,bfalse);
                }

                }

		    } break;

            case State_ReAdd:
            {
                m_animatedComponent->setAnim(MEDAL_READD_ANIM);
                m_animatedComponent->resetCurTime();
                setupOldElectoonsAsAlreadyPresent();
            } break;
            case State_Finish:
            {
                m_animatedComponent->setAnim(MEDAL_APPEARS_FINISH);
                setupOldElectoonsAsAlreadyPresent();
            } break;

            case State_JustDisappeared: //transient state. Goes to preappears.
		    {
                for (u32 i=0; i<MEDAL_ELECTOON_COUNT; i++)
                {
                    setElectoonPresence(i,bfalse);
                    setElectoonAppears(i,bfalse);
                    setElectoonBounces(i,bfalse);
                }
                restoreDeadPlayers();
                playVictorySequence();
                pauseWorld(bfalse);

                Actor* callingActor = m_lastCallingActor.getActor();

                if ( callingActor )
                {
                    Ray_EventMedalNonCinematicEnded ended;
                    callingActor->onEvent(&ended);
                }

                setState(State_Preappears);
		    } break;

		}
	}
}
