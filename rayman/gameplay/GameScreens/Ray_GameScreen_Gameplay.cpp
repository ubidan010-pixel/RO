#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_Gameplay.h"
#endif //_ITF_RAY_GAMESCREEN_GAMEPLAY_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#define PAUSEMENU_FRIENDLY "menuPause"
#define PAUSEMENU_CONFMAP_FRIENDLY "menuPauseConfMap"
#define SCOREBAORD_FRIENDLY "ScoreRecap"
#define WORLDMAPMENU_FRIENDLY "menuWorldMap"
#define CREDITSMAPMENU_FRIENDLY "menuCreditsMap"

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages.h"
#endif //_ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LumMusicManagerAIComponent.h"
#endif //_ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_MUSICMANAGER_H_
#include "rayman/gameplay/Ray_MusicManager.h"
#endif //_ITF_RAY_MUSICMANAGER_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif //ITF_RAYONLINETRACKING_MANAGER_H

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_GameScreen_Gameplay)
        //
        //
        ///////////////////////////////////////////////////////////////////////////////////////////
        Ray_GameScreen_Gameplay::Ray_GameScreen_Gameplay()
        : Super()
        , m_joinLeaveGameHandler()
        , m_isLoaded( bfalse )
        , m_confirmRestart(bfalse)
        , m_confirmExit(bfalse)
        , m_confirmMainMenu(bfalse)
        , m_backToWMOnConfirm(btrue)
        , m_wasPreloaded(bfalse)
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameScreen_Gameplay::~Ray_GameScreen_Gameplay()
    {
        GAMEMANAGER->setInputModeForGamePlay(bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::init()
    {
        World* playerWorld = (World*)GAMEMANAGER->getPlayerWorld().getObject();
        playerWorld->setActive(btrue);

        RAY_GAMEMANAGER->mapLoadResetInfo();
        RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeLoadMap());

        InitializationParameters *params = (InitializationParameters*)getInitParams();

        //BeginLoading();
        if ( params->m_worldRef.isValid() )
        {
            m_world = static_cast<World*>(params->m_worldRef.getObject());

            m_prefetchStarted = btrue;
            m_wasPreloaded = btrue;
        }
        else
        {
            m_wasPreloaded = bfalse;

            ObjectRef rWorld = RAY_GAMEMANAGER->createMap();
            ITF_ASSERT(rWorld.isValid());

            m_world = (World*)GETOBJECT(rWorld);
            ITF_ASSERT(m_world!=NULL);
            RAY_GAMEMANAGER->loadMap(rWorld, params->m_mapName, btrue, bfalse, params->m_forceUnloadBaseMap, GameManager::LEVEL_NAME_INITIALISATION_TYPE_SET);
            m_prefetchStarted = bfalse;
        }

#ifndef ITF_FINAL
        WarningManager::clear();
#endif

        //load checkpoint data

        //check coherency


        ITF_ASSERT(m_world!=NULL);


        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
            if (player)
            {
                player->setHasJoinedBefore(bfalse);
            }
        }

        // !!!!!! spawn player MUST NOT BE DONE HERE (because of resource loading order) !!
        // is done in on world loaded
        //RAY_GAMEMANAGER->spawnPlayerActors(GetClassNameStatic());
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


        GAMEMANAGER->setInputModeForGamePlay(btrue);


    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::onWorldLoaded()
    {
        if ( m_isLoaded )
            return;

        InitializationParameters *params = (InitializationParameters*)getInitParams();

        //spawnPlayerActors/spawnMedal/maySpawnElectoonToothScore must be done in the onworldLoaded ,don't move it
        RAY_GAMEMANAGER->setPlayersCurrentScale(1.f);
        RAY_GAMEMANAGER->spawnPlayerActors(GetClassCRCStatic());
        RAY_GAMEMANAGER->resetPlayerGlobalPowers();
        RAY_GAMEMANAGER->applyHealthModifierForAllPlayers();

        // Skip
        RAY_GAMEMANAGER->resetSkipData();

        if (params->m_mapName == RAY_GAMEMANAGER->getPrologueMap())
        {
            RAY_GAMEMANAGER->setMusicTheme(RAY_GAMEMANAGER->getMapConfigMusic(ITF_GET_STRINGID_CRC(prologue,291121491)));
        }

        createLumMusicManager();
        if (Actor * skipLevel = RAY_GAMEMANAGER->getSkipLevelActor().getActor())
        {
            skipLevel->enable();
        }

        RAY_GAMEMANAGER->enableBaseScene(btrue);
        RAY_GAMEMANAGER->enableMedalWorld(btrue);

        for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player* player = GAMEMANAGER->getPlayer(i);
            if (player)
            {
                GAMEMANAGER->changePlayerGameMode(player, RAY_GAMEMODE_PLATFORMER);
            }
        }

        const ObjectPath& chkPt = params->m_checkPointPath;

        RAY_GAMEMANAGER->setFirstCheckpointPath(chkPt);

        if ( chkPt.isValid() )
        {
            RAY_GAMEMANAGER->setFirstCheckpointPath(chkPt);
        }

        // Start Reward manager
        REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Event_StartNewLevel);

        //set rich presence data
        StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
        if ( tag != StringID::Invalid )
        {
            m_backToWMOnConfirm = RAY_GAMEMANAGER->isMenuToWMEnable(tag); // return false if in prologue
            u32 PresenceState = RAY_GAMEMANAGER->getRichPresenceIndex(RAY_GAMEMANAGER->getWMCurrentWorldTag());
            GAMEMANAGER->updateRichPresenceForActivePlayers(PresenceState);
        } else
            m_backToWMOnConfirm = btrue;

        m_isLoaded = btrue;
        m_currentlyExiting = bfalse;
        RAY_GAMEMANAGER->stopFade(bfalse);

#ifdef ITF_SUPPORT_ONLINETRACKING


        Ray_OnlineTrackingManager *pManager = RAY_GAMEMANAGER->getOnlineTrackingManager();

        if (pManager )
        {
            //postlaunch

            pManager->m_currentLevelTime			= 0.0f;
            pManager->m_hiddenBrokenCageIndex1		= -1;
            pManager->m_hiddenBrokenCageIndex2		= -1;
            pManager->m_finalBrokenCageIndex		= -1;
            pManager->m_runCount					= 0;
            pManager->m_lumStage1Count				= 0;
            pManager->m_lumStage2Count				= 0;




            if (RAY_GAMEMANAGER->getTimeAttackActorChrono() != ITF_INVALID_OBJREF && RAY_GAMEMANAGER->isLevelCompletedOnce(RAY_GAMEMANAGER->getCurrentLevelName())
                )
            {
                pManager->m_TimeAttackAvailableForThisMap = btrue;
            }
            else
            {
                pManager->m_TimeAttackAvailableForThisMap = bfalse;
            }


            if (RAY_GAMEMANAGER->getCurrentLevelData())
            {

                Ray_PersistentGameData_LevelTracking *pLevelData = RAY_GAMEMANAGER->getCurrentLevelData()->getTrackingData();

                if (pLevelData)
                {
                    pLevelData->updateRunCount(1);
                    pManager->m_runCount = pLevelData->getRunCount();
                    pManager->m_challengeTimeAttackCount = pLevelData->getChallengeTimeAttackCount();
                    pManager->m_lumStage1Count		     = pLevelData->getChallengeLumsStage1();
                    pManager->m_lumStage2Count		     = pLevelData->getChallengeLumsStage2();


                }

            }

            Ray_PersistentGameData_UniverseTracking *pUniverseData = RAY_GAMEMANAGER->getPersistentGameData()->getTrackingData();

            if (pUniverseData)
            {
                for (u32  i=0;i<4;i++)
                {
                    pManager->m_PlayTimeForPlayerIndex[i] = pUniverseData->getTimer(i);
                }

                pManager->m_Paf2Players = pUniverseData->getPafCounter(0);
                pManager->m_Paf3Players = pUniverseData->getPafCounter(1);
                pManager->m_Paf4Players = pUniverseData->getPafCounter(2);

            }


            // prelaunch
            if (CONFIG->m_enableonlineTracking==btrue)
            {

                pManager->m_checkPointOrderSinceLevelBegin = 1;
                pManager->ResetDeathCountTimers(bfalse);
                pManager->resetStats(bfalse);

            }

        }
#endif


        if (RAY_GAMEMANAGER->isInPause())
            RAY_GAMEMANAGER->leavePause(bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::update()
    {
        if (m_prefetchStarted && GAMEMANAGER->isPreloadedMapLoaded(m_world->getRef()) && GAMEMANAGER->getBaseMapWorld().isValid())
        {
            if(RAY_GAMEMANAGER->isFadeOutFinished())
            {
                RAY_GAMEMANAGER->switchToPreloadedMap(m_world->getRef());
                CONFIG->m_blackClearColorNeeded = bfalse;
                m_prefetchStarted = bfalse;
            }
        }
        else
        {
            if (!m_isWorldLoaded || !m_isLoaded)
                return;

            //EndLoading();

            checkSignInOutChanges();

            if (m_endOfLevelReached
                && !SAVEGAME_ADAPTER->hasPendingOperation()
                && !GAMEINTERFACE->isShowingSavingNotification()
                && !RAY_GAMEMANAGER->isSequencePlaying())
            {
                if (!m_currentlyExiting)
                {
#ifdef _CAN_EXIT_LEVEL
                    bbool bGotoWorldMap = btrue;
                    if(EDITOR && !EDITOR->checkAndAskForUnsavedWork())
                    {
                        m_endOfLevelReached = bfalse;
                        bGotoWorldMap = bfalse;
                    }

                    if(bGotoWorldMap)
                    {
                        m_currentlyExiting = btrue;
                        RAY_GAMEMANAGER->resetToWorldMap();
                    }
#else
                    //DEBUGINFO->addDebugInfoDisplay("TEMPORARY : should exit level here!", 20.f);
                    // I don't understand why there is this flag set to true here..
                    // Fix the onSignout behavior by removing this
                    //m_currentlyExiting = btrue;
#endif
                }
            }

            if (m_currentlyExiting)
                return;

            m_joinLeaveGameHandler.update();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::close()
    {
        showPauseMenu(bfalse);

        RAY_GAMEMANAGER->enableBaseScene(bfalse);
        RAY_GAMEMANAGER->enableMedalWorld(bfalse);
        RAY_GAMEMANAGER->unspawnMedal();
        Ray_LumMusicManagerAIComponent::destroyAllExisting();
        if (Actor * skipLevel = RAY_GAMEMANAGER->getSkipLevelActor().getActor())
        {
            skipLevel->disable();
        }

        for (u32 i=0; i<RAY_GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player* player = RAY_GAMEMANAGER->getPlayer(i);
            if (player)
            {
                RAY_GAMEMANAGER->changePlayerGameMode(player, GAMEMODE_UNKNOWN);
            }
        }

        // hide tutorial
        if (inTutorial())
        {
            hideTutorial();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_Gameplay::canPause()
    {
        return (m_isLoaded
            && RAY_GAMEMANAGER->isFxFinished()
            && !RAY_GAMEMANAGER->isInGameplayPause()
            && RAY_GAMEMANAGER->isRestoreDeathSequenceCanPause()
            && RAY_GAMEMANAGER->canPauseDuringEndSequence());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::showPauseMenu(bbool _isVisible)
    {
        if (_isVisible)
        {
            bbool inPrologue = bfalse;
            bbool inCredits = bfalse;

            //set rich presence data
            StringID tag = RAY_GAMEMANAGER->getCurrentLevelName();
            if ( tag != StringID::Invalid )
            {
                inPrologue = !RAY_GAMEMANAGER->isMenuToWMEnable(tag); // in prologue?
                inCredits = (tag == ITF_GET_STRINGID_CRC(credits, 2384050872));
            }

            if (inPrologue)
            {
                UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getPauseMenuPriority(), WORLDMAPMENU_FRIENDLY, btrue, this);
            }
            else if (inCredits)
            {
                UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getPauseMenuPriority(), CREDITSMAPMENU_FRIENDLY, btrue, this);
            }
            else
                UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getPauseMenuPriority(), PAUSEMENU_FRIENDLY, btrue, this);


        }
        else
        {
            if (UI_MENUMANAGER->isDisplayingPriority(GAMEINTERFACE->getPauseMenuPriority()))
                UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getPauseMenuPriority());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_Gameplay::inTutorial() const
    {
        return UI_MENUMANAGER->isDisplayingPriority(GAMEINTERFACE->getTutorialMenuPriority());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::showTutorial()
    {
        // show tutorial "menu"
        if (GAMEMANAGER->isMenusLoaded())
        {
            UI_MENUMANAGER->showMenuPage(
                GAMEINTERFACE->getTutorialMenuPriority(),
                GAMEMANAGER->getTutorialMenuId(),
                btrue);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::hideTutorial()
    {
        // hide tutorial "menu"
        if (GAMEMANAGER->isMenusLoaded())
        {
            UI_MENUMANAGER->showMenuPage(
                GAMEINTERFACE->getTutorialMenuPriority(),
                GAMEMANAGER->getTutorialMenuId(),
                bfalse);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::pause_showWorldMap()
    {
        RAY_GAMEMANAGER->resetToWorldMap();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::pause_addPlayer()
    {
        ITF_ASSERT(RAY_GAMEMANAGER->getNumActivePlayers() == 1);
        RAY_GAMEMANAGER->changePlayerActivation(1,bfalse);
    }

    void Ray_GameScreen_Gameplay::onMenuItemAction( UIComponent* _UIComponent )
    {
        if(!_UIComponent) return;

        const StringID id = _UIComponent->getID();

        if ( id == ITF_GET_STRINGID_CRC(RESUME, 2819602867) )
        {
            if (RAY_GAMEMANAGER->isInPause())
            {
                SYSTEM_ADAPTER->setSystemUnPauseFlag(btrue);
            }
        }
        else if ( id == ITF_GET_STRINGID_CRC(RESTART, 2941005503) )
        {
            m_confirmRestart = btrue;
            m_confirmExit   = bfalse;
            m_confirmMainMenu= bfalse;
        }
        else if ( id == ITF_GET_STRINGID_CRC(EXIT, 98510012) )
        {
            m_confirmExit = btrue;
            m_confirmRestart = bfalse;
            m_confirmMainMenu= bfalse;
        }
        else if ( id == ITF_GET_STRINGID_CRC(MAINMENU, 244070065))
        {
            m_confirmRestart = bfalse;
            m_confirmExit    = bfalse;
            m_confirmMainMenu= btrue;
        }

        // To skip the confirmation menu
        if ( id == ITF_GET_STRINGID_CRC(CONFIRM_YES, 3118550926) || GAMEMANAGER->getUsePressConfMenu() )
        {
            if(m_confirmRestart)
            {
                // if inside end sequence, then stop it now
                RAY_GAMEMANAGER->onRestartLevelDuringEndSequence();

                if (RAY_GAMEMANAGER->isInPause())
                {
                    SYSTEM_ADAPTER->setSystemUnPauseFlag(btrue);
                }

                REWARD_MANAGER->Session_Reset();
                RAY_GAMEMANAGER->clearNextOperation();
                RAY_GAMEMANAGER->stopAllSequences();
                RAY_GAMEMANAGER->loadGameplayMap(RAY_GAMEMANAGER->getLevelPath(RAY_GAMEMANAGER->getCurrentLevelName()),btrue,btrue);

#ifdef ITF_SUPPORT_ONLINETRACKING
                RAY_GAMEMANAGER->getOnlineTrackingManager()->onGameRestart(btrue);
#endif //ITF_SUPPORT_ONLINETRACKING
            }
            else if (m_confirmExit)
            {
                // Stop music
                MUSICMANAGER->stopAll(1.0f);

#ifdef ITF_SUPPORT_ONLINETRACKING
                RAY_GAMEMANAGER->getOnlineTrackingManager()->onReachedCheckpoint(Ray_OnlineTrackingManager::CP_ExitLevel);
#endif //ITF_SUPPORT_ONLINETRACKING

                if(m_backToWMOnConfirm)
                {
                    RAY_GAMEMANAGER->resetToWorldMap();

                    // reset heart shields on back to world map (RO-15484)
                    for (u32 i=0; i<s_maxPlayers; ++i)
                    {
                        RAY_GAMEMANAGER->getPowerUpManager().setEnabled(Ray_PowerUp_HeartShield, i, bfalse);
                    }
                }
                else
                {
                    RAY_GAMEMANAGER->goToTheMainMenu();
                }

                REWARD_MANAGER->Session_Stop();
            }
            else if(m_confirmMainMenu)
            {
                RAY_GAMEMANAGER->backToTheMainMenuASAP(btrue);
            }

        } else if ( id == ITF_GET_STRINGID_CRC(CONFIRM_NO, 893466327) )
        {
            UI_MENUMANAGER->showPreviousMenu();
        }

        if (id == ITF_GET_STRINGID_CRC(Options, 3527952213))
        {
            m_optionMenuHelper.activateForOptionMenu(this);
        }
        else if (id == ITF_GET_STRINGID_CRC(Controls, 1174371653))
        {
            m_controlsRemappingMenuHelper.activateForControlsRemappingMenu(this);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_Gameplay::createLumMusicManager()
    {
        World *world = (World*)RAY_GAMEMANAGER->getBaseMapWorld().getObject();
        if (!world)
            return;
        Scene *scene = world->getRootScene();
        ITF_ASSERT_CRASH(scene, "no scene!");

        Path managerPath;
        Ray_MusicManager *musicManager = IRTTIObject::SafeDynamicCast<Ray_MusicManager>(MUSICMANAGER,ITF_GET_STRINGID_CRC(Ray_MusicManager,2851409933));
        if (musicManager)
        {
            m_lumMusicManagerACT = musicManager->getLumMusicManagerPath();
        }
        if (m_lumMusicManagerACT.isEmpty())
        {
            managerPath = RAY_GAMEMANAGER->getDefaultLumMusicManagerPath();
        }
        else
        {
            managerPath = m_lumMusicManagerACT;
        }

        Actor *act = ACTORSMANAGER->spawnActor(Vec3d(0,0,0), scene, managerPath, btrue);

        if (act)
        {
            act->loadResources();
        }
    }

    void Ray_GameScreen_Gameplay::onEndOfLevelReached()
    {
        Super::onEndOfLevelReached();

#ifdef ITF_SUPPORT_ONLINETRACKING
        //count lums for tracking lum attacks
        i32 lumCount = 0;
        for (u32 i=0; i<s_maxPlayers; i++)
        {
            Player *player = RAY_GAMEMANAGER->getPlayer(i);
            if (player && player->getActive())
            {
                lumCount += (i32)RAY_GAMEMANAGER->getCurrentScore().getLums(i);
            }
        }
        //
        StringID levelName = RAY_GAMEMANAGER->getCurrentLevelName();
        i32 threshold1 = RAY_GAMEMANAGER->getLumAttackThreshold(levelName,0);
        i32 threshold2 = RAY_GAMEMANAGER->getLumAttackThreshold(levelName,1);
        if (lumCount>=threshold1 && !RAY_GAMEMANAGER->isChallengeWon(CHALLENGE_LUMATTACK1))
        {
            RAY_GAMEMANAGER->getOnlineTrackingManager()->onUnLockLumStage1();
        }
        if (lumCount>=threshold2 && !RAY_GAMEMANAGER->isChallengeWon(CHALLENGE_LUMATTACK2))
        {
            RAY_GAMEMANAGER->getOnlineTrackingManager()->onUnLockLumStage2();
        }
        //
        RAY_GAMEMANAGER->getOnlineTrackingManager()->onReachedCheckpoint(Ray_OnlineTrackingManager::CP_EndOfLevel);
#endif //ITF_SUPPORT_ONLINETRACKING
    }

    void Ray_GameScreen_Gameplay::onMapLoaded(ObjectRef _world)
    {
        Super::onMapLoaded(_world);

        bbool isEndLevel = bfalse;

        const Path endLevelPath = RAY_GAMEMANAGER->getEndLevelMap();
        if (World* checkWorld =  static_cast<World*>(_world.getObject()))
        {
            const Path currentWorldPath = checkWorld->getRootScene()->getPath();

            if (currentWorldPath == endLevelPath)
            {
                isEndLevel = btrue;
            }
        }


        //we went through a classic loading screen. do a fade.
        if(!m_wasPreloaded && !isEndLevel)
        {
            RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeLoadMap());
            RAY_GAMEMANAGER->stopFade(bfalse);
        }


    }

    ITF::bbool Ray_GameScreen_Gameplay::checkSignInOutChanges()
    {
        if (m_playerSignedInOut)
        {
            if (!m_currentlyExiting)
            {
                m_currentlyExiting = btrue;
                RAY_GAMEMANAGER->goToTheMainMenu();
            }
        }
        return m_currentlyExiting;
    }

}
