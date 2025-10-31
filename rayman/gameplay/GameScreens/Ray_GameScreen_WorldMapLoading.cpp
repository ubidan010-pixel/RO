#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMapLoading.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages.h"
#endif //_ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_Gameplay.h"
#endif //_ITF_RAY_GAMESCREEN_GAMEPLAY_H_

#ifndef _ITF_INGAMETEXTCOMPONENT_H_
#include "gameplay/Components/UI/InGameTextComponent.h"
#endif //_ITF_INGAMETEXTCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF {

    IMPLEMENT_OBJECT_RTTI(Ray_GameScreen_WorldMapLoading)
        ObjectRef Ray_GameScreen_WorldMapLoading::m_worldRef;

    //------------------------------------------------------------------------------
    Ray_GameScreen_WorldMapLoading::Ray_GameScreen_WorldMapLoading() 
        : Super()
        , m_timer(0)
        ,  m_validLoadingScreen(bfalse)
        , m_state(State_PrefetchedLoading)
        , m_driveInputs(btrue)
    {
    }

    //------------------------------------------------------------------------------
    Ray_GameScreen_WorldMapLoading::~Ray_GameScreen_WorldMapLoading()
    {
        GAMEMANAGER->setInputModeForGamePlay(bfalse);
    }

    //------------------------------------------------------------------------------
    void Ray_GameScreen_WorldMapLoading::init()
    {
        GAMEMANAGER->addGamePlayInputListener(this);

        m_prefetchStarted = bfalse;
        InitializationParameters *params = (InitializationParameters*)getInitParams();

        ObjectRef ref;
        if (m_worldRef.isValid())
            ref = m_worldRef;
        else if (params && params->m_worldRef.isValid())
            ref = params->m_worldRef;

        //start the fade, it will be stopped by the next game screen
        if ( params && params->m_startFade )
            RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeLoadMap());

        if (!ref.isValid())
        {
            const Path & path = RAY_GAMEMANAGER->getLoadingScreen(RAY_GAMEMANAGER->getWMCurrentWorldTag());
            if (!path.isEmpty())
                preload(path);
            if (m_worldRef.isValid())
                ref = m_worldRef;
        }

        //if ref is valid, the loading screen was prefetched
        if ( ref.isValid())
        {
            m_world = static_cast<World*>(ref.getObject());
            //RAY_GAMEMANAGER->loadMap(params->m_worldRef, RAY_GAMEMANAGER->getWorldMap(), btrue, bfalse, btrue);
            m_validLoadingScreen = btrue;
            m_prefetchStarted = btrue;
            m_state = State_PrefetchedLoading;
        }
        else
        {
            ObjectRef rWorld = RAY_GAMEMANAGER->createMap();
            ITF_ASSERT(rWorld.isValid());

            m_world = (World*)GETOBJECT(rWorld);
            ITF_ASSERT(m_world!=NULL);
            //load loading screen
            if (RAY_GAMEMANAGER->getLoadingScreenPath().isEmpty())
            {
                m_validLoadingScreen = bfalse;
                LOADINGSCREEN->show();
            }
            else
            {
                m_validLoadingScreen = btrue;
                RAY_GAMEMANAGER->loadMap(rWorld, RAY_GAMEMANAGER->getLoadingScreenPath(), btrue, bfalse, bfalse, GameManager::LEVEL_NAME_INITIALISATION_TYPE_KEEP);
            }
            m_prefetchStarted = bfalse;
            m_state = State_FadeToLoading;
        }

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
            if (player)
            {
                player->setHasJoinedBefore(bfalse);
            }
        }

        GAMEMANAGER->setInputModeForGamePlay(btrue);
        m_newWorld.invalidate();

        m_timer = RAY_GAMEMANAGER->getTemplate()->getLoadingMinFrames();

        //invalidate checkpoint path to make sure it takes the first checkpoint of loading screen
        RAY_GAMEMANAGER->setFirstCheckpointPath(ObjectPath());


        //if there is no loading map, consider it loaded
        if (!m_validLoadingScreen)
            onWorldLoaded();

        Ray_PlayerHudScoreComponent::setIsLumCaptionEnabled(bfalse);
        Ray_PlayerHudScoreComponent::setVisibility(bfalse);


    }   


    //------------------------------------------------------------------------------
    void Ray_GameScreen_WorldMapLoading::onWorldLoaded()
    {
        //this function is called on switchtoworld

        //switch to loading level
        RAY_GAMEMANAGER->spawnPlayerActors(GetClassCRCStatic());
        RAY_GAMEMANAGER->setPlayersCurrentScale(1.f);
        RAY_GAMEMANAGER->enableBaseScene(btrue);

        for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player* player = GAMEMANAGER->getPlayer(i);
            if (player)
            {
                GAMEMANAGER->changePlayerGameMode(player, RAY_GAMEMODE_PLATFORMER);
            }
        }

        InitializationParameters *params = (InitializationParameters*)getInitParams();
        Vector<ObjectPath> prefetchList;
        ObjectPath path;
        //preload level
        if (params->m_mapName == RAY_GAMEMANAGER->getWorldMap())
        {
            //if world map, set prefetch target to last spot we were on
            path = RAY_GAMEMANAGER->getWMSpotObjectPath(RAY_GAMEMANAGER->getWMCurrentLevelTag());
            RAY_GAMEMANAGER->setCurrentLevelName(Path::EmptyPath);
            RAY_GAMEMANAGER->makeLevelStateFromPersistentUniverse();
            RAY_GAMEMANAGER->onBeginToLoadMap();
        }
        else
        {
            //otherwise use checkpoint path used
            path = params->m_checkPointPath;
            RAY_GAMEMANAGER->setCurrentLevelName(params->m_mapName);
            RAY_GAMEMANAGER->makeLevelStateFromPersistentUniverse();
            RAY_GAMEMANAGER->onBeginToLoadMap();
        }

        if (path.isValid())
            prefetchList.push_back(path);

        m_newWorld = RAY_GAMEMANAGER->preLoadMap(params->m_mapName,prefetchList );

        RAY_GAMEMANAGER->stopFade(bfalse);

        m_levelTitle = RAY_GAMEMANAGER->getLevelTitleActor();

        if (RAY_GAMEMANAGER->isInPause())
            RAY_GAMEMANAGER->leavePause(bfalse);
    }

    //------------------------------------------------------------------------------
    void Ray_GameScreen_WorldMapLoading::update()
    {

        InitializationParameters *params = (InitializationParameters*)getInitParams();

        //if loading map was preloaded
        if (m_state == State_PrefetchedLoading)
        {
            //wait for fade then switch to loading map
            if (RAY_GAMEMANAGER->isFadeOutFinished() && RAY_GAMEMANAGER->isPreloadedMapLoaded(m_worldRef))
            {
                World* world = reinterpret_cast< World* >( GETOBJECT( m_worldRef ) );

                if ( world != NULL )
                {
                    Scene* scene = world->getRootScene();

                    if ( scene != NULL )
                    {
                        PathString_t pathName;

                        scene->getPath().getString( pathName );
                        FILEMANAGER->unregisterBundle( FilePath::getFilenameWithoutExtension( pathName ) );
                    }
                }
                RAY_GAMEMANAGER->switchToPreloadedMap(m_world->getRef());
                m_prefetchStarted = bfalse;
                m_state = State_FadeToLoading;
                m_worldRef.invalidate();

            }
        }
        else if (m_state == State_FadeToLoading)
        {
            updatePlayers();
            if (RAY_GAMEMANAGER->isFadeInFinished())
            {
                if (Actor * actor = m_levelTitle.getActor())
                {
                    actor->enable();

                    InGameTextComponent * inGameTextComponent = actor->GetComponent<InGameTextComponent>();
                    if (inGameTextComponent)
                    {
                        if (params->m_mapName == RAY_GAMEMANAGER->getWorldMap())
                        {
                            inGameTextComponent->setText(RAY_GAMEMANAGER->getTitleId(ITF_GET_STRINGID_CRC(WorldMap, 1734041925)));
                        }
                        else
                        {
                            inGameTextComponent->setText(RAY_GAMEMANAGER->getTitleId(RAY_GAMEMANAGER->getWMCurrentLevelTag()));
                        }
                        inGameTextComponent->playAppear();
                    }

                }
                m_state = State_WaitForLevelLoad;
                Ray_PlayerHudScoreComponent::setVisibility(btrue);

            }
        }
        else if (m_state == State_WaitForLevelLoad)
        {
            updatePlayers();

            //wait for new level to finish prefetching
            if(m_newWorld.isValid() && RAY_GAMEMANAGER->isPreloadedMapLoaded(m_newWorld))
            {
                m_state = State_WaitForMusicLoad;
                //World * world = (World*)GETOBJECT(m_newWorld);

                //if (world)
                //{
                //    u32 sceneCount = world->getSceneCount();
                //    for (u32 i = 0 ; i < sceneCount ; ++i)
                //    {
                //        Scene * pScene = world->getSceneAt(i);
                //        if ( pScene && pScene->getMusicTheme() != StringID::Invalid )
                //            RAY_GAMEMANAGER->setMusicTheme(pScene->getMusicTheme());
                //    }
                //}

            }

            m_timer--;

            m_joinLeaveGameHandler.update();
        }
        else if (m_state == State_WaitForMusicLoad)
        {
            updatePlayers();

            if (m_timer <= 0) //this timer used for forcing a minimum loading time
            {
                if(m_newWorld.isValid() && RAY_GAMEMANAGER->isPreloadedMapLoaded(m_newWorld) && !RESOURCE_MANAGER->hasQueryOps() && !RESOURCE_MANAGER->hasResourcesToLoad())
                {
                    m_state = State_TitleDisappearing;
                    Ray_PlayerHudScoreComponent::setVisibility(bfalse);

                    if (Actor * actor = m_levelTitle.getActor())
                    {
                        InGameTextComponent * inGameTextComponent = actor->GetComponent<InGameTextComponent>();
                        if (inGameTextComponent)
                        {
                            inGameTextComponent->playDisappear();
                        }
                    }
                }
            }
            else
            {
                m_timer--;
            }

            m_joinLeaveGameHandler.update();
        }

        else if (m_state == State_TitleDisappearing)
        {
            bbool exit = bfalse;
            if (Actor * actor = m_levelTitle.getActor())
            {
                InGameTextComponent * inGameTextComponent = actor->GetComponent<InGameTextComponent>();
                if (inGameTextComponent)
                {
                    if (inGameTextComponent->isDisappearFinished() || !actor->isEnabled())
                        exit = btrue;
                }
            }
            else
                exit = btrue;

            if (exit)
            {

                //once ready if we were not using the loading map hide the classic loading screen
                if (!m_validLoadingScreen)
                {
                    LOADINGSCREEN->hide();
                }

                if(checkSignInOutChanges())
                    return; 

                //post the proper game screen change according to destination
                if (params->m_mapName == RAY_GAMEMANAGER->getWorldMap())
                {
                    //go to world map
                    Ray_GameScreen_WorldMap::InitializationParameters params;
                    params.m_worldRef = m_newWorld;
                    RAY_GAMEMANAGER->postGameScreenChange<Ray_GameScreen_WorldMap>(bfalse,&params);
                }
                else
                {
                    Ray_GameScreen_Gameplay::InitializationParameters initData;
                    initData.m_mapName = params->m_mapName;
                    initData.m_forceUnloadBaseMap = params->m_forceUnloadBaseMap;
                    initData.m_worldRef = m_newWorld;
                    if (!GAMEMANAGER->isLoadingVisible() )
                        LOADINGSCREEN->show();
                    RAY_GAMEMANAGER->postGameScreenChange<Ray_GameScreen_Gameplay>(bfalse, &initData );
                    //RAY_GAMEMANAGER->loadGameplayMap(params->m_mapName, params->m_forceUnloadBaseMap, bfalse, m_newWorld);
                }
            }
        }

    }

    //------------------------------------------------------------------------------
    void Ray_GameScreen_WorldMapLoading::close()
    {
        Ray_PlayerHudScoreComponent::setIsLumCaptionEnabled(btrue);
        Ray_PlayerHudScoreComponent::setVisibility(btrue);

        RAY_GAMEMANAGER->enableBaseScene(bfalse);
        GAMEMANAGER->removeInputListener(this);

        for (u32 i=0; i<RAY_GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player* player = RAY_GAMEMANAGER->getPlayer(i);
            if (player)
            {
                RAY_GAMEMANAGER->changePlayerGameMode(player, GAMEMODE_UNKNOWN);
            }
        }


        //set the first checkpoint for the new level
        InitializationParameters *params = (InitializationParameters*)getInitParams();

        ObjectPath path;

        if (params->m_mapName == RAY_GAMEMANAGER->getWorldMap())
        {
            path = RAY_GAMEMANAGER->getWMSpotObjectPath(RAY_GAMEMANAGER->getWMCurrentLevelTag());
        }
        else
        {
            path = params->m_checkPointPath;

        }

        if ( path.isValid() )
        {
            RAY_GAMEMANAGER->setFirstCheckpointPath(path);
        }

    }

    void Ray_GameScreen_WorldMapLoading::preload(const Path & _path)
    {
        if (!RAY_GAMEMANAGER->getLoadingScreenPath().isEmpty() && m_worldRef.isValid())
        {
            if (RAY_GAMEMANAGER->getLoadingScreenPath() != _path)
            {
                RAY_GAMEMANAGER->deletePreloadedMap(m_worldRef);
            }
        }
        RAY_GAMEMANAGER->setLoadingScreenPath(_path);
        Vector<ObjectPath> prefetchList;
        m_worldRef = RAY_GAMEMANAGER->preLoadMap(_path,prefetchList);
        World* world = (World*)GETOBJECT(m_worldRef);
        ITF_ASSERT(world);

    }

    void Ray_GameScreen_WorldMapLoading::updatePlayers()
    {
        if (m_driveInputs)
        {
            const StringID moveX = ITF_GET_STRINGID_CRC(MOVEX, 965019289);
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Player* player = GAMEMANAGER->getPlayer(i);
                if ( player && player->getActive() )
                {
                    if ( Actor* playerActor = player->getActor() )
                    {

                        Ray_PlayerControllerComponent * controller = playerActor->GetComponent<Ray_PlayerControllerComponent>();
                        if (controller)
                        {
                            controller->Receive(player->getIndex(), 1.0f, moveX);
                        }
                    }
                }
            }
        }
    }

    void Ray_GameScreen_WorldMapLoading::Receive( u32 deviceID, f32 axis, const StringID& action )
    {
        if (Actor * actor = GAMEMANAGER->getActivePlayer(deviceID))
        {
            m_driveInputs = bfalse;
        }
    }

    ITF::bbool Ray_GameScreen_WorldMapLoading::checkSignInOutChanges()
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





} // namespace ITF
