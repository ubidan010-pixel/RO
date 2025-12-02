#include "precompiled_gameplay.h"

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_


#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif // _ITF_OBJECTPATH_H_

#ifndef _ITF_GAMESCREEN_INITIAL_H_
#include "gameplay/Managers/GameScreens/GameScreen_Initial.h"
#endif //_ITF_GAMESCREEN_INITIAL_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_PERSISTENTGAMEDATA_H_
#include "gameplay/managers/PersistentGameData.h"
#endif

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_SAVENOTIFICATIONCOMPONENT_H_
#include "gameplay/Components/Misc/SaveNotificationComponent.h"
#endif //_ITF_SAVENOTIFICATIONCOMPONENT_H_

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_TUTORIALTEXTCOMPONENT_H_
#include "gameplay/Components/UI/TutorialTextComponent.h"
#endif //_ITF_TUTORIALTEXTCOMPONENT_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"


namespace ITF
{
    struct ControllerIconPathDescriptor
    {
        const char* key;
        ControllerIconSlot slot;
    };

    static const ControllerIconPathDescriptor g_controllerIconPathDescriptors[] =
    {
        { "iconsButtonPath", IconSlot_Default },
        { "iconsButtonPathWii", IconSlot_Wii },
        { "iconsButtonPathPS3", IconSlot_PS3 },
        { "iconsButtonPathPS5", IconSlot_PS5 },
        { "iconsButtonPathVita", IconSlot_Vita },
        { "iconsButtonPathCTR", IconSlot_CTR },
        { "iconsButtonPathSwitch", IconSlot_Switch },
        { "iconsButtonPathOunce", IconSlot_Ounce },
        { "iconsButtonPathXboxSeries", IconSlot_XboxSeries },
        { "iconsButtonPathX360", IconSlot_X360 },
        { "iconsButtonPathKeyboard", IconSlot_Keyboard },
    };

    /*
    GameManager::GameScreenOperationData::GameScreenOperationData() : m_loadCheckpointData(bfalse)
    {

    }

    void GameManager::GameScreenOperationData::clear()
    {
        m_mapName.clear();
        m_checkPointPath.clear();
        m_loadCheckpointData = bfalse;
        m_forceUnloadBaseMap = bfalse;
    }
    */

    BEGIN_SERIALIZATION(PlayerIDInfo)

        SERIALIZE_MEMBER("id", m_id);
        SERIALIZE_MEMBER("family", m_family);
        SERIALIZE_MEMBER("deathBubbleColor", m_deathBubbleColor);
        SERIALIZE_CONTAINER_OBJECT("gameScreens", m_gameScreens);

    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(PlayerIDInfo,GameScreenInfo)

        SERIALIZE_MEMBER("gameScreen", m_gameScreen);
        SERIALIZE_CONTAINER_OBJECT("actors", m_actors);

    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(PlayerIDInfo,ActorInfo)

        SERIALIZE_MEMBER("file", m_file);
        SERIALIZE_MEMBER("isAlwaysActive",  m_isAlwaysActive);
        SERIALIZE_MEMBER("isPlayable",      m_isPlayable);
        SERIALIZE_CONTAINER("gameModes", m_gameModes);

    END_SERIALIZATION()

    PlayerIDInfo::PlayerIDInfo()
    : m_deathBubbleColor(Color::black())
    {
    }

    const PlayerIDInfo::GameScreenInfo* PlayerIDInfo::getGameScreenInfo( const StringID& _id ) const
    {
        u32 numGameScreens = m_gameScreens.size();

        for ( u32 i = 0; i < numGameScreens; i++ )
        {
            const GameScreenInfo& gInfo = m_gameScreens[i];

            if ( gInfo.getGameScreen() == _id )
            {
                return &gInfo;
            }
        }

        return NULL;
    }

    IMPLEMENT_OBJECT_RTTI(GameManagerConfig_Template)
    BEGIN_SERIALIZATION(GameManagerConfig_Template)
        SERIALIZE_CONTAINER("debugMenuMapList",m_maps);
        SERIALIZE_CONTAINER("mapListPressConf",m_pressConfMaps);
        SERIALIZE_CONTAINER("menus",m_menuPath);
        SERIALIZE_CONTAINER("luaIncludes",m_luaIncludes);
        SERIALIZE_CONTAINER("inputs",m_inputConfigFiles);
        SERIALIZE_CONTAINER_OBJECT("musicThemes",m_musicThemes);
        SERIALIZE_MEMBER("baseMap",m_baseMapPath);
        SERIALIZE_MEMBER("game2dWorld",m_game2DPath);
        SERIALIZE_MEMBER("gameTextFilePath",m_gameTextFilePath);
        SERIALIZE_MEMBER("mainMenuBackMap",m_mainMenuBackMap);
        SERIALIZE_MEMBER("mainMenuBackMapForDebugSaves",m_mainMenuBackMapForDebugSaves);
        SERIALIZE_MEMBER("worldMap",m_worldMap);
        SERIALIZE_MEMBER("splash1Map",m_splash1Map);
        SERIALIZE_MEMBER("levelEndedMap",m_levelEndedMap);
        SERIALIZE_MEMBER("menuSoundMap", m_menuSoundMap);
        SERIALIZE_MEMBER("loading", m_loadingScreenAct);
        SERIALIZE_MEMBER("usePressConfMenu", m_usePressConfMenu);
		SERIALIZE_CONTAINER_OBJECT("playerIDInfo", m_playersIDInfo);
        SERIALIZE_CONTAINER("familyList",m_familyList);
        SERIALIZE_MEMBER("cameraShakeConfig",m_cameraShakeConfig);
        SERIALIZE_MEMBER("cutSceneDefaultUnskippableDurationFirstTime", m_cutSceneDefaultUnskippableDurationFirstTime);
        SERIALIZE_FUNCTION(fillMusicMap,ESerialize_Data_Load);
        SERIALIZE_MEMBER("TEMP_threshold", m_tempThreshold);
        SERIALIZE_MEMBER("TEMP_useshake", m_useShakeForAttack);
        SERIALIZE_MEMBER("TEMP_delay", m_tempDelay);
        SERIALIZE_MEMBER("TEMP_runUseB", m_temprunUseB);
        SERIALIZE_MEMBER("TEMP_runUseShake", m_temprunUseShake);
        SERIALIZE_MEMBER("TEMP_swimMaxSpeed", m_tempswimMaxSpeed);
        SERIALIZE_MEMBER("TEMP_swimSmooth", m_tempswimSmooth);
        SERIALIZE_MEMBER("TEMP_runTimerStop", m_temprunTimerStop);
        for (u32 iconIdx = 0; iconIdx < ITF_ARRAY_SIZE(g_controllerIconPathDescriptors); ++iconIdx)
        {
            const ControllerIconPathDescriptor& iconDesc = g_controllerIconPathDescriptors[iconIdx];
            serializer->Serialize(iconDesc.key, m_controllerIconPaths[iconDesc.slot]);
        }
        SERIALIZE_MEMBER("gpeIconsPath", m_gpeIconsPath);
        SERIALIZE_MEMBER("skipIconsPath", m_skipIconsPath);
        SERIALIZE_MEMBER("menuLogosPath", m_menuLogosPath);
    END_SERIALIZATION()

    void GameManagerConfig_Template::fillMusicMap()
    {
        for (u32 i = 0 ; i < m_musicThemes.size() ; ++i)
        {
            m_MusicThemesMap[m_musicThemes[i].m_theme] = i;
        }
    }

    BEGIN_SERIALIZATION_SUBCLASS(GameManagerConfig_Template,MusicTheme)
        SERIALIZE_MEMBER("theme",m_theme);
        SERIALIZE_MEMBER("path",m_path);
    END_SERIALIZATION()

    GameManagerConfig_Template::~GameManagerConfig_Template()
    {
        u32 numPlayerIds = m_playersIDInfo.size();

        for ( u32 i = 0; i < numPlayerIds; i++ )
        {
            SF_DEL(m_playersIDInfo[i]);
        }

        m_luaIncludes.clear();
    }

    BEGIN_SERIALIZATION(GameManager)
    END_SERIALIZATION()

    GameManager* GameManager::s_instance = NULL;

    GameManager::GameManager()
        : m_loadingMap(bfalse)
        , m_baseLoaded(bfalse)
        , m_mapSwitched(bfalse)
        , m_currentCheckpoint(ITF_INVALID_OBJREF)
        , m_currentSceneRef(ITF_INVALID_OBJREF)
        , m_nextOperation(GAMEOPERATION_NONE)
        , m_rBaseWorld(ITF_INVALID_OBJREF)
        , m_rCurrentWorld(ITF_INVALID_OBJREF)
        , m_rPlayerWorld(ITF_INVALID_OBJREF)
        , m_rGame2DWorld(ITF_INVALID_OBJREF)
        , m_rGame2DWorldLoaded(bfalse)
        , m_bIsPlaying(bfalse)
        , m_areMenuLoaded(bfalse)
        , m_menuSoundWorld(ITF_INVALID_OBJREF)
        , m_menuSoundLoaded(bfalse)
        , m_theGameScreen(NULL)
        , m_nextGameScreen(NULL)
        , m_isInPause(bfalse)
		, m_bInitDone(bfalse)
        , m_pauseMenuRequested(btrue)
        , m_cheatIgnoreNextPause(bfalse)
        , m_configTemplate(NULL)
        , m_persistentGameData(NULL)
        , m_backToTheMainMenuASAP(bfalse)
        , m_worldPhysicallyReady(bfalse)
        , m_loadingMainMenuStarted(bfalse)
        , m_checkpointData(NULL)
        , m_inputManager(NULL)
        , m_currentLevelName(StringID::Invalid)
        , m_mainIndexPlayer(U32_INVALID)
        , m_pauseOwnerIndex(U32_INVALID)
        , m_deconnectedPlayerIndex(U32_INVALID)
        , m_inputModeGamePlayCounter(0)
        , m_inputModeMenuCounter(0)
        , m_saveNotificationActor(ITF_INVALID_OBJREF)
        , m_inactiveRichPresenceIndex(0)
        , m_lastRichPresenceIndex(0)
        , m_tutorialMsgActor(ITF_INVALID_OBJREF)
    {
		// Create gamemode parameters factory
		//
		m_GameModeParametersFactory = newAlloc( mId_Gameplay, ObjectFactory );
		m_GameModeParametersFactory->RegisterObject<GameModeParameters>(ITF_GET_STRINGID_CRC(GameModeParameters,3896257179));
#ifndef ITF_FINAL
        m_funcStartLoadingProfile = NULL;
        m_funcStopLoadingProfile  = NULL;
#endif //ITF_FINAL
    }

    void GameManager::init()
    {
        static const Path gamemanagerIsg = GETPATH_ALIAS("gameconfig");
        m_configTemplate = loadConfig(gamemanagerIsg);
        if (!m_configTemplate)
        {
            ITF_FATAL_ERROR("Couldn't load game config: %s", gamemanagerIsg.getStringID().getDebugString());
        }

        u32 numPlayers = getMaxPlayerCount();

        m_players.resize(numPlayers);

        for ( u32 i = 0 ; i < numPlayers; i++ )
        {
            m_players[i] = NULL;
        }

        initPlayers();

        m_fxHandler.init();

        postLoadInit();

        initInputManager();

        // TEMP -- will be removed soon --
#ifdef ITF_WINDOWS
        if(INPUT_ADAPTER)
        {
            INPUT_ADAPTER->setThreshold(m_configTemplate->getThresholdTEMP());
            INPUT_ADAPTER->setUSEShakeAttack(m_configTemplate->getUseSakeForAttackTEMP());
            INPUT_ADAPTER->setDelay(m_configTemplate->getDelayTEMP());
            INPUT_ADAPTER->setRunUseB(m_configTemplate->getRunUseBTEMP());
            INPUT_ADAPTER->setRunUseShake(m_configTemplate->getRunUseShakeTEMP());
            INPUT_ADAPTER->setSwimMaxSpeed(m_configTemplate->getSwimMaxSpeedTEMP());
            INPUT_ADAPTER->setSwimSmooth(m_configTemplate->getSwimSmoothTEMP());
            INPUT_ADAPTER->setRunTimerStop(m_configTemplate->getRunTimerStopTEMP());
        }

#endif // ITF_WINDOWS
        m_bInitDone = btrue;
    }

    const GameManagerConfig_Template * GameManager::loadConfig(const Path & _path)
    {
        m_templateClientHandler.addUsedTemplate(_path.getStringID());
        return TEMPLATEDATABASE->getTemplate<GameManagerConfig_Template>(&m_templateClientHandler, _path);
    }

    void GameManager::initPlayers()
    {
        m_availablePlayersIndexes.clear();

        u32 numPlayerIds = m_configTemplate->getPlayersIDInfo().size();
        u32 numPlayers = getMaxPlayerCount();

        ITF_ASSERT_MSG( numPlayerIds >= numPlayers, "All player IDS not set !" );

        for (u32 i = 0; i < numPlayers; i++)
        {
            if ( m_players[i] )
            {
                SF_DEL(m_players[i]);
            }

            m_players[i] = allocPlayer();
            m_players[i]->setIndex(i);
            m_players[i]->setOriginalPlayerIDInfoIndex(i);

            if ( i < numPlayerIds )
            {
                m_players[i]->setID( m_configTemplate->getPlayersIDInfo()[i]->getId().cStr() );
                m_players[i]->setCurrentPlayerIDInfo( m_configTemplate->getPlayersIDInfo()[i] );
            }

            m_availablePlayersIndexes.push_back(i);
        }
    }

    void GameManager::resetPlayers()
    {
        for (u32 i = 0; i < getMaxPlayerCount(); i++)
        {
            Player *player = m_players[i];
            if (player)
            {
                player->reset();
            }
        }
    }

    void GameManager::destroyInternal()
    {
        if ( m_rPlayerWorld != ITF_INVALID_OBJREF )
        {
            WORLD_MANAGER->deleteWorld(static_cast<World*>(GETOBJECT(m_rPlayerWorld)));
        }

        if ( m_rGame2DWorld != ITF_INVALID_OBJREF )
        {
            WORLD_MANAGER->deleteWorld(static_cast<World*>(GETOBJECT(m_rGame2DWorld)));
        }

        WorldManager::removeListener(this);
        if(SCENE_MANAGER)
            SCENE_MANAGER->removeListener(this);

        SF_DEL(m_theGameScreen);
        SF_DEL(m_nextGameScreen);
        SF_DEL(m_persistentGameData);

        for (u32 i = 0; i < m_players.size(); i++)
        {
            SF_DEL(m_players[i]);
        }

        m_fxHandler.end();

		for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
		{
			SF_DEL( m_GameModeControllers[i] );
		}
		SF_DEL( m_GameModeParametersFactory );
        SF_DEL(m_checkpointData);
        SF_DEL(m_inputManager);
        m_templateClientHandler.freeUsedTemplates();
    }

    void GameManager::postLoadInit()
    {
//        UI_TEXTMANAGER->fillTextPaths(m_gameTextFilePath);//disable localisation file

        for (ITF_VECTOR<String>::const_iterator it = m_configTemplate->getMenuPath().begin(); it != m_configTemplate->getMenuPath().end(); ++it)
        {
            UI_MENUMANAGER->addMenu(UI_MENUMANAGER->getMenuBaseNameFromPath(it->cStr()));
        }

        WorldManager::addListener(this);
        SCENE_MANAGER->addListener(this);
        GAMEINTERFACE->setCutSceneDefaultUnskippableDurationFirstTime(m_configTemplate->getCutSceneDefaultUnskippableDurationFirstTime());
        SF_DEL(m_theGameScreen);
        SF_DEL(m_nextGameScreen);
        m_theGameScreen = GAMEINTERFACE->getGameScreenFactory()->CreateObject<GameScreenBase>("GameScreen_Initial");


        //loadMenus(); moved into ApplicationFramework::init()

        if ( m_rPlayerWorld == ITF_INVALID_OBJREF )
        {
            m_rPlayerWorld = WORLD_MANAGER->createAndLoadEmptyWorld(btrue);

            World* playerWorld = (World*)GETOBJECT(m_rPlayerWorld);

            playerWorld->setAlwaysActive(btrue);
        }
	}

    bbool GameManager::userSignedInOut(u32 _userIndex, bbool _signedIn, bbool _isOnlineChange)
    {
#ifdef ITF_SUPPORT_ONLINETRACKING
		if (ONLINETRACKING_ADAPTER && _isOnlineChange && _signedIn)
			ONLINETRACKING_ADAPTER->onSignIn(_userIndex);
#endif // ITF_SUPPORT_ONLINETRACKING

        u32 richPresenceIndex = m_inactiveRichPresenceIndex;
        if ( _isOnlineChange && _signedIn )
        {
            if(_userIndex < getMaxPlayerCount() && m_players[_userIndex]->getActiveAndPersistent())
                richPresenceIndex = m_lastRichPresenceIndex;
        }
        updateRichPresenceForPlayer(richPresenceIndex, _userIndex);
        return m_theGameScreen->onUserSignedInOut(_userIndex, _signedIn, _isOnlineChange);
    }

    //MANAGE CHARACTER ACTIVATION/DEACTIVATION
    void GameManager::changePlayerActivation( u32 _playerIndex, bbool _startLevel )
    {
        Player* player = m_players[_playerIndex];

        if (player->getActive())
        {
            m_availablePlayersIndexes.push_back(player->getOriginalPlayerIDInfoIndex());

            deactivatePlayer(_playerIndex);
            player->setActiveAndPersistent(bfalse);
            REWARD_MANAGER->Action_RemovePlayer(_playerIndex);
        }
        else
        {
            // Do not change the available players queue if we are starting the level and players were already assigned
            if ( !_startLevel || m_availablePlayersIndexes.size() == m_players.size() )
            {
                ITF_ASSERT(m_availablePlayersIndexes.size()>0);

                if ( m_availablePlayersIndexes.size() == 0 )
                {
                    return;
                }

                u32 availableIndex = m_availablePlayersIndexes[0];
                u32 foundPlayer = U32_INVALID;

                for ( u32 i = 0; i < foundPlayer; i++ )
                {
                    Player* player = getPlayer(i);

                    if ( player && player->getOriginalPlayerIDInfoIndex() == availableIndex )
                    {
                        foundPlayer = i;
                        break;
                    }
                }

                ITF_ASSERT(foundPlayer!=U32_INVALID);

                if ( foundPlayer == U32_INVALID )
                {
                    return;
                }

                if ( foundPlayer != _playerIndex )
                {
                    Player* otherPlayer = getPlayer(foundPlayer);
                    u32 myIndex = player->getOriginalPlayerIDInfoIndex();
                    u32 otherIndex = otherPlayer->getOriginalPlayerIDInfoIndex();

                    swapPlayer(_playerIndex,foundPlayer);

                    otherPlayer->setOriginalPlayerIDInfoIndex(myIndex);
                    player->setOriginalPlayerIDInfoIndex(otherIndex);
                }

                m_availablePlayersIndexes.eraseKeepOrder(0);
            }

            activatePlayer(_playerIndex);
            player->setActiveAndPersistent(btrue);
            REWARD_MANAGER->Action_AddPlayer(_playerIndex);
            player->reset();
        }

        recomputeActiveIndices();

        bbool startDead = player->getHasJoinedBefore(); // first time = alive, next = dead
        player->setHasJoinedBefore(btrue);

        EventPlayerActivationChanged eventActivaton(player, startDead, _startLevel);
        EVENTMANAGER->broadcastEvent(&eventActivaton);

        for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
        {
            m_GameModeControllers[i]->onPlayerActivationChanged( player, startDead );
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bbool GameManager::postGameScreenChange_String(const StringID &_screen, bbool _evenIfOtherQueued, GameScreenBase::InitializationParameters* _data )
    {
        if (!_evenIfOtherQueued)
        {
            //We prevent multiple conflicting changes.
            //For example, if the system requires a TRC back to main menu, we don't want any change because of game event.
            ITF_ASSERT(!m_nextGameScreenID.isValid());
            if (m_nextGameScreenID.isValid())
                return bfalse;
        }

        if (!isInPause())
            enterPause(bfalse);

        m_nextGameScreenID = _screen;
        SF_DEL(m_nextGameScreen); //just in case we created a next game screen but did not switch to it.
        m_nextGameScreen = GAMEINTERFACE->getGameScreenFactory()->CreateObject<GameScreenBase>(m_nextGameScreenID);
        m_nextOperation = GAMEOPERATION_CHANGEGAMESCREEN;

        if ( _data )
            m_nextGameScreen->setInitParams(_data->clone());

        return btrue;
    }

    //////////////////////////////////////////////////////////////////////////
    bbool GameManager::isPlayableScreen()
    {
        ITF_ASSERT(m_theGameScreen);
        return !LOADINGSCREEN->isVisible() && m_theGameScreen->hasGameplay();
    }
    //////////////////////////////////////////////////////////////////////////
    void GameManager::manageCurrentGameScreen()
    {
        //Update
        ITF_ASSERT(m_theGameScreen);
        m_theGameScreen->update();
    }


    void GameManager::processNextOperation()
    {
        GameOperation currentOperation = m_nextOperation;

        m_nextOperation = GAMEOPERATION_NONE;

        switch(currentOperation)
        {
        case GAMEOPERATION_CHANGEGAMESCREEN:
            //save level state
            MUSICMANAGER->stopAll(1.0f);
            manageChangeGameScreen();
            break;

        case GAMEOPERATION_SAVECHECKPOINT:
            checkpointSave();
            break;

        case GAMEOPERATION_LOADCHECKPOINT:
            // Stop music
            MUSICMANAGER->stopAll(0.0f);
            checkpointLoad();
            break;

        case GAMEOPERATION_LOADCHECKPOINTANDSAVE:
            MUSICMANAGER->stopAll(0.0f);
            checkpointLoad();
            checkpointSave();
            break;

        case GAMEOPERATION_WAITPRELOADEDMAPREADY:
            checkNextPreloadedMapReady();
            break;

        case GAMEOPERATION_SWITCHTOPRELOADEDMAP:
            processSwitchToPreloadedMap();
            break;

        case GAMEOPERATION_LOADMAP:
            processLoadSubMap();
            break;

        default:
            break;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::update(f32 _dt)
    {
        processNextOperation();

        updatePauseMode();

        manageCurrentGameScreen();

        updateLoading();

        m_fxHandler.update(_dt);

		updateGameModes();

        updateInputManager();

        updateReturnToMainMenu();

        updatePlayersCellsView();
    }

    void GameManager::draw()
    {
        m_fxHandler.draw();
    }

    void GameManager::updatePlayersCellsView()
    {
		for (u32 i = 0; i < getMaxPlayerCount(); i++)
		{
			Player* player = getPlayer(i);
			if ( player->getActive() )
			{
                player->updateCellsView();
			}
		}
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::onPlayerDisconnected(u32 _padIndex)
    {
        if(getMainIndexPlayer()!=_padIndex && getPlayer(_padIndex))
        {
            //m_deconnectedPlayerIndex = _padIndex;
            if(!getPlayer(_padIndex)->getIsDisconnected())
            {
                getPlayer(_padIndex)->setIsDisconnected(btrue);

                //changePlayerActivation(_padIndex, bfalse);
                SYSTEM_ADAPTER->setSystemPauseFlag(btrue);
                setIndexPauseOwner(getMainIndexPlayer());
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void GameManager::onPlayerReconnected(u32 _padIndex)
    {
        if(canPause())
        SYSTEM_ADAPTER->setSystemPauseFlag(btrue);
        if(getIndexPauseOwner()==U32_INVALID)
            setIndexPauseOwner(_padIndex);
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::checkPlayerPadDisconnected()
    {
        if (SYSTEM_ADAPTER->isInSystemUnPause() && m_deconnectedPlayerIndex != U32_INVALID)
        {
            // pause will be called
            if(!INPUT_ADAPTER->isPadConnected(m_deconnectedPlayerIndex))
                deactivatePlayer(m_deconnectedPlayerIndex);

            m_deconnectedPlayerIndex = U32_INVALID;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::updatePauseMode()
    {
        if (TRC_ADAPTER->isDisplayingError() || LOADINGSCREEN->isVisible())
            return ;

        //sanity check
        if (canPause())
        {
            checkPlayerPadDisconnected();

            //Test system pause
            if (SYSTEM_ADAPTER->isInSystemPause())
            {
                if(!isInPause())
                    enterPause(m_pauseMenuRequested);
                SYSTEM_ADAPTER->setSystemPauseFlag(bfalse);
                SYSTEM_ADAPTER->setSystemUnPauseFlag(bfalse);
            } else if (SYSTEM_ADAPTER->isInSystemUnPause())
            {
                if(isInPause())
                    leavePause();
                SYSTEM_ADAPTER->setSystemUnPauseFlag(bfalse);
                SYSTEM_ADAPTER->setSystemPauseFlag(bfalse);
            }

            if (m_pauseMenuRequested)
            {
                //Pause toggle by any player
                bbool pauseButtonHit = bfalse;
                if (!m_cheatIgnoreNextPause)
                {
                    for (u32 i = 0; i < getMaxPlayerCount(); i++)
                    {
                        if (!SYSTEM_ADAPTER->isOSUIActive() && getPlayer(i)->getActiveAndPersistent())
                        {
                            InputAdapter::PressStatus buttons[JOY_MAX_BUT];
                            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua, i, buttons, JOY_MAX_BUT);

                            if (buttons[m_joyButton_Start] == InputAdapter::JustReleased)
                            {
                                if(!m_isInPause)
                                {
                                    setIndexPauseOwner(i);
                                    pauseButtonHit = btrue;
                                } else if(m_isInPause && getIndexPauseOwner() == i)
                                {
                                    pauseButtonHit = btrue;
                                }

                                if(pauseButtonHit)break;
                            }
                        }
                    }
                }
                else
                {
                    m_cheatIgnoreNextPause = bfalse;
                }
                //
                if (pauseButtonHit)
                {
                    // Request pause/unpause for next frame (to avoid some issue with input mapping order)
                    if (m_isInPause)
                    {
                        SYSTEM_ADAPTER->setSystemUnPauseFlag(btrue);
                    }
                    else
                    {
                        SYSTEM_ADAPTER->setSystemPauseFlag(btrue);
                    }
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    bbool	GameManager::canDisplayTRCMessages()
    {
    	return m_theGameScreen->canDisplayTRCMessages();
    }

    //////////////////////////////////////////////////////////////////////////
    bbool   GameManager::canOpenMovie()
    {
        if(m_theGameScreen)
            return m_theGameScreen->canOpenMovie();
        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    bbool   GameManager::canPause()
    {
        return m_theGameScreen->canPause();
    }

    //////////////////////////////////////////////////////////////////////////
    void    GameManager::enterPause(bbool _bShowMenu)
    {
        ITF_ASSERT_MSG(!m_isInPause, "entering pause, although game was already paused");
        if (!m_isInPause)
        {
            m_pauseMenuRequested = _bShowMenu;
            //if(m_pauseOwnerIndex == U32_INVALID || getPlayerFromActiveIndex(m_pauseOwnerIndex)==NULL)
            //    setIndexPauseOwner(getMainIndexPlayer());

            m_isInPause = btrue;
            pauseMap(btrue);

            if(_bShowMenu)
                m_theGameScreen->showPauseMenu(btrue);

            REWARD_MANAGER->Session_Pause();

            setInputModeForMenu(btrue);
            setInputModeForGamePlay(bfalse);
            CAMERACONTROLLERMANAGER->pauseMode(btrue);

            pauseSounds(btrue);
            stopDefaultBusMix();
            setPauseBusMix();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void    GameManager::leavePause(bbool _bHideMenu)
    {
        m_pauseMenuRequested = btrue;
        ITF_ASSERT_MSG(m_isInPause, "Attempt to leave pause, but game is not paused");
        if(_bHideMenu)
            m_theGameScreen->showPauseMenu(bfalse);
        pauseMap(bfalse);

        m_isInPause = bfalse;
        REWARD_MANAGER->Session_Resume();

        setInputModeForMenu(bfalse);
        setInputModeForGamePlay(btrue);
        CAMERACONTROLLERMANAGER->pauseMode(bfalse);

        m_pauseOwnerIndex = U32_INVALID;

        pauseSounds(bfalse);
        stopPauseBusMix();
        setDefaultBusMix();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Pause all sounds
    void GameManager::pauseSounds(bbool _pause)
    {
        if ( _pause )
        {
#if defined(ITF_SUPPORT_RAKI)
            SOUND_ADAPTER->getPauseSensitivePlayingVoices(m_playingVoicesWhenEnteringPause);
            for (u32 index=0; index<m_playingVoicesWhenEnteringPause.size(); index++)
                SOUND_ADAPTER->pause(m_playingVoicesWhenEnteringPause[index]);
#else
            AUDIO_ADAPTER->getPauseSensitivePlayingVoices(m_playingVoicesWhenEnteringPause);
            for (u32 index = 0; index < m_playingVoicesWhenEnteringPause.size(); index++)
                AUDIO_ADAPTER->pause(SoundHandle(m_playingVoicesWhenEnteringPause[index]));
#endif
            MUSICMANAGER->pause();
            METRONOME_MANAGER->pause(METRONOME_TYPE_DEFAULT);
        }
        else
        {
#if defined(ITF_SUPPORT_RAKI)
            for (u32 index=0; index<m_playingVoicesWhenEnteringPause.size(); index++)
                SOUND_ADAPTER->resume(m_playingVoicesWhenEnteringPause[index]);
#else
            for (u32 index = 0; index < m_playingVoicesWhenEnteringPause.size(); index++)
                AUDIO_ADAPTER->resume(SoundHandle(m_playingVoicesWhenEnteringPause[index]));
#endif
            m_playingVoicesWhenEnteringPause.clear();
            MUSICMANAGER->resume();
            METRONOME_MANAGER->resume(METRONOME_TYPE_DEFAULT);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    StringID GameManager::getTutorialMenuId() const
    {
        return ITF_GET_STRINGID_CRC(Tutorial_Menu,594804231);
    }

    //////////////////////////////////////////////////////////////////////////
    bbool GameManager::inTutorial() const
    {
        return m_theGameScreen->inTutorial();
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::showTutorial()
    {
        m_theGameScreen->showTutorial();
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::hideTutorial()
    {
        m_theGameScreen->hideTutorial();
    }

    void GameManager::showTutorialText( const LocalisationId& _id, InputAdapter::PadType _padType )
    {
        Actor* msg = m_tutorialMsgActor.getActor();

        if ( msg )
        {
            TutorialTextComponent* tutorialTxtComponent = msg->GetComponent<TutorialTextComponent>();

            if ( tutorialTxtComponent )
            {
                tutorialTxtComponent->show(_id,_padType);
                msg->enable();
            }
        }
    }

    void GameManager::hideTutorialText()
    {
        Actor* msg = m_tutorialMsgActor.getActor();

        if ( msg )
        {
            TutorialTextComponent* tutorialTxtComponent = msg->GetComponent<TutorialTextComponent>();

            if ( tutorialTxtComponent )
            {
                tutorialTxtComponent->hide();
            }
        }
    }

    void GameManager::forceHideTutorialText()
    {
        Actor* msg = m_tutorialMsgActor.getActor();

        if ( msg )
        {
            TutorialTextComponent* tutorialTxtComponent = msg->GetComponent<TutorialTextComponent>();

            if ( tutorialTxtComponent )
            {
                tutorialTxtComponent->forceHide();
            }
        }
    }

    void GameManager::disableTutorialActor()
    {
        Actor* msg = m_tutorialMsgActor.getActor();

        if ( msg )
        {
            msg->disable();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void    GameManager::showScoreboard(bbool _show)
    {
        m_theGameScreen->showScoreboard(_show);
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::updateMenuLoading()
    {
        u32 count = m_worldMenu.size();

        for ( u32 i = 0; i < m_worldMenu.size(); ++i )
        {
            World* pWorldMenu = static_cast< World* >( GETOBJECT( m_worldMenu[i].m_worldMenuRef ) );

            ITF_ASSERT( pWorldMenu != NULL );
            if ( pWorldMenu != NULL && pWorldMenu->isPhysicalReady() )
                count -= 1;
        }
        if ( !m_worldMenu.empty() && !m_areMenuLoaded && !count )
        {
            for ( u32 i = 0; i < m_worldMenu.size(); ++i )
            {
                World* pWorldMenu = static_cast< World* >( GETOBJECT( m_worldMenu[i].m_worldMenuRef ) );

                ITF_ASSERT( pWorldMenu != NULL );
                if ( pWorldMenu != NULL )
                {
                    // Hide it by default
                    pWorldMenu->setAllowUpdate(bfalse);
                    pWorldMenu->setAllowDraw(bfalse);
                }
            }
            m_areMenuLoaded = btrue;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    void GameManager::updateMenuSoundLoading()
    {
        //if menu sound map hasn't been loaded yet
        if( !m_menuSoundLoaded )
        {
            //check if map has been loaded
            World * const menuSoundWorld = static_cast<World*>(m_menuSoundWorld.getObject());

            if( menuSoundWorld != NULL && menuSoundWorld->isPhysicalReady() )
            {
                //init map
                menuSoundWorld->setAlwaysActive(btrue);
                //loading is done
                m_menuSoundLoaded = btrue;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    void GameManager::updateBaseMapLoading()
    {
        if (!m_baseLoaded)
        {
            World* pWorld = static_cast<World*>(GETOBJECT(m_rBaseWorld));
            if (pWorld && pWorld->isPhysicalReady())
            {
                World* playerWorld = (World*)GETOBJECT(m_rPlayerWorld);

                if ( playerWorld && playerWorld->isPhysicalReady() )
                {
                    LOG("[GameManager] Finished async loading of base scene at frame : %u", CURRENTFRAME);

                    m_baseLoaded = btrue;
                    onBaseMapLoaded();

                    pWorld->setAlwaysActive(btrue);

                    // Disable draw/update while we are not running a world
                    pWorld->setAllowDraw(bfalse);
                    pWorld->setAllowUpdate(bfalse);
                }
            }
        }
    }

	///////////////////////////////////////////////////////////////////////////
    void GameManager::updateLoading()
    {

        updateBaseMapLoading();
        updateMenuLoading();
        updateMenuSoundLoading();

        if ( !m_rGame2DWorldLoaded && m_rGame2DWorld.isValid() )
        {
            World* game2dWorld = static_cast< World* >( GETOBJECT( m_rGame2DWorld ) );

            if ( game2dWorld->isPhysicalReady() )
            {
                game2dWorld->setActive(btrue);
                game2dWorld->activeAll();
                game2dWorld->forceLoadResourcesAndDisableUnload();
                m_rGame2DWorldLoaded = true;
            }
        }

        for ( i32 i = 0 ; i < (i32)m_preLoadedAsyncWorld.size() ; ++i )
        {
            ObjectRef worldRef = m_preLoadedAsyncWorld[i];
            World * pWorld = static_cast<World *>(worldRef.getObject());
            if ( pWorld )
            {
                if ( !pWorld->isSceneAsyncLoadRunning() )
                {
                    onPreloadedAsyncFinished(pWorld);
                    m_preLoadedPrefetchWorld.push_back(pWorld->getRef());
                    m_preLoadedAsyncWorld.eraseNoOrder(i);

                    i--;
                }
            }
        }

        flushPreloadMapPendingDelete();

        if (!m_loadingMap)
            return;

        m_worldPhysicallyReady = bfalse;

        bbool isInGameplayScreen = m_theGameScreen->hasGameplay();

        bbool canFinalizeWorldLoading = (isInGameplayScreen && m_baseLoaded) || !isInGameplayScreen;

        if (canFinalizeWorldLoading)
        {
            ITF_ASSERT(m_rCurrentWorld.isValid());
            World* pWorld = static_cast<World*>(GETOBJECT(m_rCurrentWorld));
            ITF_ASSERT(pWorld);

            // BootRequirement needs to be done to continue here
            bbool canSwitchFromLoadingWorld = btrue;
            if(TRC_ADAPTER)
            {
                // To allow the SplashScreen menu to be loaded ...
                canSwitchFromLoadingWorld = canSwitchFromLoadingWorld &&
                    ( !IsMainMenuLoadingOrLoaded()  || TRC_ADAPTER->isBootRequirementsFinished() );
            }

            // Wait for the async load of the map to be done
            if(canSwitchFromLoadingWorld && !pWorld->isSceneAsyncLoadRunning())
            {
                // When it's over switch (and activate) the loaded map
                if(!m_mapSwitched)
                {
                    LOG("[GameManager] Finished async loading of requested map at frame : %u", CURRENTFRAME);

                    WORLD_MANAGER->switchToWorld(m_rCurrentWorld);

                    // Before switching manage destruction to prevent logical collide
                    do
                    {
                        RESOURCE_MANAGER->flushPendingOps();
                        RESOURCE_MANAGER->manageDestruction( btrue, btrue );
                    } while (RESOURCE_MANAGER->getResourceForDestroyCount() > 0);
                    RESOURCE_MANAGER->setDeleteLogicalAllowed(btrue);


                    pWorld->prefetchResource();

                    // Disable the map until it's ready
                    pWorld->setAllowUpdate(bfalse);
                    pWorld->setAllowDraw(bfalse);

                    m_mapSwitched = btrue;
                }
                else
                {
                    // When the map is active and prefetched check for it's status
                    World* playerWorld = (World*)m_rPlayerWorld.getObject();
                    if(pWorld->isPhysicalReady() && (!playerWorld || playerWorld->isPhysicalReady()))
                    {
                        pWorld->stopPrefetch();

                        LOG("[GameManager] Requested map is ready at frame : %u", CURRENTFRAME);

                        setPlaying(isInGameplayScreen); //Must be called before onMapLoaded
                        onMapLoaded(pWorld);

                        GAMEINTERFACE->onPostSceneActivation(pWorld->getRootScene(), btrue);

                        // The map is ready, enable it
                        pWorld->setAllowUpdate(btrue);
                        pWorld->setAllowDraw(btrue);


                        ITF_MAP<Path, ObjectRef>::iterator it = m_preloadMaps.find(pWorld->getRootScene()->getPath());
                        if(it != m_preloadMaps.end())
                            m_preloadMaps.erase(it);

                        m_worldPhysicallyReady = btrue;
                    }
                }
            }
        }
    }

    void GameManager::onPreloadedAsyncFinished(World * _world)
    {
        _world->prefetchResource();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const Path& GameManager::getBaseMapPath()
    {
        return m_configTemplate->getBaseMapPath();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const ITF_MAP<StringID,u32>& GameManager::getMusicThemes()
    {
        return m_configTemplate->getMusicThemes();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameManager::setMusicTheme(const StringID& _theme )
    {
        // Set music theme
        if ( _theme != StringID::Invalid )
        {
            const ITF_MAP<StringID,u32>& musicMap = m_configTemplate->getMusicThemes();
            ITF_MAP<StringID,u32>::const_iterator it = musicMap.find(_theme);

            if ( it != musicMap.end() )
            {
                const Path& musicPath = getMusicPath(it->second);

                MUSICMANAGER->addMusicFromConfigFile(musicPath);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    const String& GameManager::getMusicPath(u32 _index)
    {
        return m_configTemplate->getMusicPath(_index);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameManager::enableBaseScene(bbool _enable)
    {
        ObjectRef baseRef = getBaseMapWorld();
        World *base = (World*)GETOBJECT(baseRef);
        ITF_ASSERT(base!=NULL);
        if (base)
        {
            base->setAllowUpdate(_enable);
            base->setAllowDraw(_enable);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameManager::loadMenus()
    {
        u32 menuPathSize = m_configTemplate->getMenuPath().size();

        m_worldMenu.reserve( menuPathSize );
        for (u32 i = 0; i < menuPathSize; i++)
        {
            String   mapPath = m_configTemplate->getMenuPath()[i];
            LoadInfo loadInfoMenu;

            loadInfoMenu.m_mapPath              = mapPath;
            loadInfoMenu.m_forceLoadResources   = btrue;
            loadInfoMenu.m_disconnectFromCells  = btrue;
            loadInfoMenu.m_bAsyncLoad           = btrue;

            WorldMenu worldMenu;

            worldMenu.m_worldMenuRef = WORLD_MANAGER->createAndLoadWorld(loadInfoMenu);
            worldMenu.m_worldMenuBaseName = UI_MENUMANAGER->getMenuBaseNameFromPath(mapPath.cStr());

            UIMenu* pMenu = UI_MENUMANAGER->getMenu(worldMenu.m_worldMenuBaseName);

            ITF_ASSERT( pMenu != NULL );
            if ( pMenu != NULL )
                pMenu->setWorldMenuRef(worldMenu.m_worldMenuRef);
            m_worldMenu.push_back(worldMenu);
        }
        updateLoading();
    }

    void GameManager::loadMenuSound()
    {
        //if loading hasn't been requested yet
        if( !m_menuSoundWorld.isValid() )
        {
            //check map path validity
            if( !m_configTemplate->getMenuSoundMap().isEmpty() )
            {
                //request map loading
                LoadInfo loadInfoMenuSound;
                loadInfoMenuSound.m_mapPath              = m_configTemplate->getMenuSoundMap();
                loadInfoMenuSound.m_forceLoadResources   = btrue;
                loadInfoMenuSound.m_disconnectFromCells  = btrue;
                loadInfoMenuSound.m_bAsyncLoad           = btrue;
                m_menuSoundWorld = WORLD_MANAGER->createAndLoadWorld(loadInfoMenuSound);
                ITF_ASSERT(m_menuSoundWorld.isValid());
            }
            else
            {
                //nothing to load so skip the loading
                m_menuSoundLoaded = btrue;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameManager::unloadMenus()
    {
        //unload sound menu map
        World * const soundMenuWorld = static_cast<World *>(m_menuSoundWorld.getObject());
        if( soundMenuWorld )
        {
            WORLD_MANAGER->deleteWorld(soundMenuWorld);
        }

        for (u32 i=0; i<m_worldMenu.size(); i++)
        {
            World *world = (World*)m_worldMenu[i].m_worldMenuRef.getObject();
            ITF_ASSERT(world);
            if (world)
            {
                WORLD_MANAGER->deleteWorld(world);
            }
        }
        m_worldMenu.clear();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameManager::mayUnloadBaseMap()
    {
        if (m_rBaseWorld!=ITF_INVALID_OBJREF)
        {
            WORLD_MANAGER->flushPending();

            ///////////////////////
            //TEMP CODE : REMOVE OBJECTS THAT WERE TRANSFERED TO SLEEP WORLD
            ObjectRef   world2 = WORLD_MANAGER->getSleepWorld();
            World *pWorld = (World*)GETOBJECT(world2);
            if (pWorld)
            {
                u32 sceneCount = pWorld->getSceneCount();

                PickableList objToRemove;
                for (u32 sceneIndex=0; sceneIndex<sceneCount; sceneIndex++)
                {
                    Scene *pScene = pWorld->getSceneAt(sceneIndex);
                    if (pScene)
                    {
                        objToRemove.clear();
                        const PickableList&  actors = pScene->getActors();
                        for (u32 i=0; i<actors.size(); i++)
                        {
                            Pickable *pPickable = actors[i];
                            if (pPickable && pPickable->getScene() && pPickable->getScene()->getWorldID()==m_rBaseWorld)
                                objToRemove.push_back(pPickable);
                        }
                        for (u32 i=0; i<objToRemove.size(); i++)
                        {
                            ((Actor*)objToRemove[i])->enable();
                        }
                    }
                }
            }
            //////////////////////////

            ITF_VERIFY(WORLD_MANAGER->deleteWorld(static_cast<World*>(GETOBJECT(m_rBaseWorld))));

            m_rBaseWorld = ITF_INVALID_OBJREF;
            m_baseLoaded = bfalse;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameManager::mayLoadBaseMap()
    {
        if (m_rBaseWorld==ITF_INVALID_OBJREF)
        {
            // Start async load of base map
            ITF_ASSERT(!m_baseLoaded);
            LoadInfo loadInfo;
            loadInfo.m_mapPath          = getBaseMapPath();
            loadInfo.m_forceLoadResources   = btrue;
            loadInfo.m_disconnectFromCells  = btrue;
            loadInfo.m_bAsyncLoad           = btrue;

            m_rBaseWorld = WORLD_MANAGER->createAndLoadWorld(loadInfo);
        }
    }

    void GameManager::freeCurrentWorld()
    {
        if (m_rCurrentWorld!=ITF_INVALID_OBJREF)
        {
            ITF_ASSERT_MSG(m_rCurrentWorld.getObject(), "The current world is a deleted world !");
                WORLD_MANAGER->switchToWorld(ITF_INVALID_OBJREF);
                m_rCurrentWorld = ITF_INVALID_OBJREF; //will be deleted by the world manager
        }
    }

    void GameManager::freeCurrentGameScreen()
    {
        if (m_theGameScreen)
        {
            m_theGameScreen->close();
            SF_DEL(m_theGameScreen);
        }

        forceHideTutorialText();
    }

    void GameManager::unloadGameResources()
    {
        deleteAllPreloadedMaps();
        freeCurrentWorld();
        mayUnloadBaseMap();
        unloadMenus();
    }

    void GameManager::processSwitchToPreloadedMap()
    {
        if ( m_nextPreloadedMap.isValid() )
        {
            swapToWorld(m_nextPreloadedMap);
            WORLD_MANAGER->switchToWorld(m_nextPreloadedMap);

            teleportToFirstCheckpoint(bfalse);

            World * pWorld = (World*)m_nextPreloadedMap.getObject();
            pWorld->stopPrefetch();

            setPlaying(m_theGameScreen->hasGameplay()); //Must be called before onMapLoaded
            onMapLoaded(pWorld);

            m_nextPreloadedMap.invalidate();

            Scene* rootScene = pWorld->getRootScene();
            GAMEINTERFACE->onPostSceneActivation(rootScene, btrue);

            // The map is ready, enable it
            pWorld->setAllowUpdate(btrue);
            pWorld->setAllowDraw(btrue);

            ITF_MAP<Path, ObjectRef>::iterator it = m_preloadMaps.find(rootScene->getPath());
            ITF_ASSERT(it != m_preloadMaps.end());
            m_preloadMaps.erase(it);
        }
    }

    void GameManager::processLoadSubMap()
    {
        createAndLoadMap(m_subMapToLoad,btrue,bfalse,bfalse,LEVEL_NAME_INITIALISATION_TYPE_KEEP);
    }

    void GameManager::manageChangeGameScreen()
    {
        //Close former game screen
        freeCurrentGameScreen();
        //removePlayerActors();

        m_currentGameScreenID = m_nextGameScreenID;
        m_nextGameScreenID.invalidate();

#if defined(ITF_SUPPORT_RAKI)
        SOUND_ADAPTER->resetBusses();
#endif

        METRONOME_MANAGER->reset(METRONOME_TYPE_DEFAULT);
        METRONOME_MANAGER->setTimeSignature(METRONOME_DEFAULT_BPM,4,4,METRONOME_TYPE_DEFAULT);
        METRONOME_MANAGER->start(METRONOME_TYPE_DEFAULT);

        EVENTDELAY_HANDLER->reset();

        m_theGameScreen = m_nextGameScreen;
        m_nextGameScreen = NULL;
        ITF_ASSERT_MSG(m_theGameScreen, "this game screen is not registered");

        if(TRC_ADAPTER) TRC_ADAPTER->killCurrentMessage();
        m_theGameScreen->init();
    }

    //////////////////////////////////////////////////////////////////////////
    ObjectRef  GameManager::createAndLoadMap( const Path& _map, bbool _asynchronous, bbool _disconnectCells, bbool _unloadBaseMap, LevelNameInitializationType _levelInitType )
    {
        ObjectRef rWorld = createMap();
        if(loadMap(rWorld, _map, _asynchronous,_disconnectCells, _unloadBaseMap, _levelInitType))
            return rWorld;
        return ITF_INVALID_OBJREF;
    }

    //////////////////////////////////////////////////////////////////////////

    void GameManager::loadSubMap(const Path& _subMap)
    {
        m_subMapToLoad = _subMap;
        m_nextOperation = GAMEOPERATION_LOADMAP;
    }

    //////////////////////////////////////////////////////////////////////////
    ObjectRef  GameManager::createMap( )
    {
        return WORLD_MANAGER->newWorld();
    }

    void GameManager::create2DWorld()
    {
        if ( m_rGame2DWorld == ITF_INVALID_OBJREF )
        {
            LoadInfo loadInfo;
            loadInfo.m_mapPath = m_configTemplate->getGame2DScenePath();
            loadInfo.m_disconnectFromCells = btrue;
            loadInfo.m_forceLoadResources = btrue;
            loadInfo.m_disconnectFromCells = btrue;
            loadInfo.m_bAsyncLoad = btrue;

            World* game2dWorld = static_cast<World*>(GETOBJECT(WORLD_MANAGER->createAndLoadWorld(loadInfo)));

            m_rGame2DWorld = game2dWorld->getRef();
        }
    }

    void GameManager::createMainMenuWorld()
    {
        ITF_ASSERT_MSG( m_mainMenuWorld == ITF_INVALID_OBJREF, "Main menu world already created" );
        if ( m_mainMenuWorld == ITF_INVALID_OBJREF )
        {
            ITF_VECTOR< ObjectPath > prefetch;

            m_mainMenuWorld = preLoadMap( getMainMenuBackMapForDebugSaves(), prefetch );
            ITF_ASSERT( m_mainMenuWorld.isValid() );
        }
    }

    void GameManager::onBeginToLoadMap()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    bbool GameManager::loadMap( ObjectRef _rWorld, const Path& _map , bbool _asynchronous, bbool _disconnectCells, bbool _unloadBaseMap, LevelNameInitializationType _levelInitType )
    {
#ifndef ITF_FINAL
        if (m_funcStartLoadingProfile)
            m_funcStartLoadingProfile();
#endif //ITF_FINAL

        ITF_ASSERT(!m_loadingMap);

        bbool preloaded = bfalse;

        i32 index = m_preLoadedPrefetchWorld.find(_rWorld);

        if ( index >= 0 )
        {
            preloaded = btrue;
        }


        if (!preloaded && !isLoadingVisible() && LOADINGSCREEN->isEnabled() )
            LOADINGSCREEN->show();

        ITF_ASSERT(_rWorld.isValid());

        // Stop all music
        MUSICMANAGER->stopAll(0.0f);

        freeCurrentWorld();

        m_loadingMap = btrue;

        if (_unloadBaseMap)
        {
            // we force unloading the basemap (for example, it may reinit permanent characters)
            mayUnloadBaseMap();
        }

        if ( m_theGameScreen->hasGameplay() )
        {
            mayLoadBaseMap();
        }
        else
        {
            mayUnloadBaseMap();
        }

        // Start (async?) load of base map
        m_mapSwitched = bfalse;

        onBeginToLoadMap();
        m_rCurrentWorld = _rWorld;
        switch (_levelInitType)
        {
        case LEVEL_NAME_INITIALISATION_TYPE_SET:
            setCurrentLevelName(_map);
        	break;
        case LEVEL_NAME_INITIALISATION_TYPE_KEEP:
            break;
        case LEVEL_NAME_INITIALISATION_TYPE_EMPTY:
            setCurrentLevelName(Path::EmptyPath);
            break;
        default:
            ITF_ASSERT_CRASH(bfalse,"Unknown level initialization type");
            break;
        }

        makeLevelStateFromPersistentUniverse();

        if (!preloaded)
        {
        // Start loading world here
        LoadInfo loadInfo;
        loadInfo.m_mapPath              = _map;
        loadInfo.m_disconnectFromCells  = _disconnectCells;
        loadInfo.m_bAsyncLoad           = _asynchronous;
        WORLD_MANAGER->loadWorld(m_rCurrentWorld, loadInfo);
        }

        // Get object
        World *pWorld = (World*)GETOBJECT(m_rCurrentWorld);
        ITF_ASSERT(pWorld);

        if (pWorld && !pWorld->isAsyncLoadedWorld())
        {
            Scene *scene = pWorld->getRootScene();
            ITF_ASSERT(scene);
            scene->flushPending();

            updateLoading();
        }
        if (pWorld && pWorld->getIgnoreCells())
        {
            pWorld->forceLoadResourcesAndDisableUnload();
        }

        INPUT_ADAPTER->enableEnvironment(InputAdapter::EnvironmentEngine);

        return m_rCurrentWorld.isValid();
    }

    //////////////////////////////////////////////////////////////////////////
    ObjectRef GameManager::getPreloadedMap( const Path& _map )
    {
        ObjectRef worldRef = ObjectRef::InvalidRef;

        ITF_MAP<Path, ObjectRef>::iterator it = m_preloadMaps.find(_map);

        if(m_preloadMaps.end() != it)
        {
            worldRef = it->second;

            // check in pending maps to delete
            i32 index = m_preLoadedPendingWorldToDelete.find(worldRef);
            if(index != -1)
            {
                // Remove from the list as it seems we finally want to use this world
                m_preLoadedPendingWorldToDelete.eraseNoOrder(index);
            }
        }

        return worldRef;
    }

    ObjectRef GameManager::preLoadMap( const Path& _map, Vector<ObjectPath> & _prefetch)
    {
        // Map is already in preload process ? Then return its ref directly
        ObjectRef worldRef = getPreloadedMap(_map);
        if ( worldRef.isValid() )
            return worldRef;

        // create new map
        ObjectRef preloadedWorld = createMap();

        World * _world = static_cast<World *>(preloadedWorld.getObject());
        if ( _world )
        {
            _world->setPrefetchTargets(_prefetch);
        }


        // Start loading world here
        LoadInfo loadInfo;
        loadInfo.m_mapPath              = _map;
        loadInfo.m_disconnectFromCells  = bfalse;
        loadInfo.m_bAsyncLoad           = btrue;

        ITF_VERIFY(WORLD_MANAGER->loadWorld(preloadedWorld, loadInfo));
        m_preLoadedAsyncWorld.push_back(preloadedWorld);
        ITF_ASSERT(m_preloadMaps.find(_map) == m_preloadMaps.end());
        m_preloadMaps[_map] = preloadedWorld;

        return preloadedWorld;
    }

    bbool GameManager::switchToPreloadedMap(ObjectRef _worldRef, bbool _autoWaitAndSwitch /* = bfalse */)
    {
        if(!_autoWaitAndSwitch)
        {
            i32 index = m_preLoadedPrefetchWorld.find(_worldRef);
            ITF_WARNING_CATEGORY(GPP, NULL, index >= 0, "Trying to switch to a preloaded world but it's not ready yet. Try to preload it earlier");
            if ( index >= 0 )
            {
                // remove world ref form array
                m_preLoadedPrefetchWorld.eraseNoOrder(index);

                m_nextOperation = GAMEOPERATION_SWITCHTOPRELOADEDMAP;
                m_nextPreloadedMap = _worldRef;

                return btrue;
            }

            return bfalse;
        }
        else
        {
            const i32 indexAsync    = m_preLoadedAsyncWorld.find(_worldRef);
            const i32 indexPrefetch = m_preLoadedPrefetchWorld.find(_worldRef);

            if(indexAsync == -1 && indexPrefetch == -1)
            {
                ITF_ASSERT_MSG(0, "Trying to switch to an unknown preloaded map");
                return bfalse;
            }

            m_nextPreloadedMap = _worldRef;

            // Check if already ready to switch
            if(!checkNextPreloadedMapReady())
            {
                LOADINGSCREEN->show();

                m_nextOperation = GAMEOPERATION_WAITPRELOADEDMAPREADY;
            }

            return btrue;
        }
    }

    bbool GameManager::isPreloadedMapLoaded(ObjectRef _worldRef)
    {
        i32 index = m_preLoadedPrefetchWorld.find(_worldRef);
        if(index < 0)
            return bfalse;

        World* pNextWorld = static_cast<World*>(_worldRef.getObject());
        return pNextWorld->isPhysicalReady();
    }

    bbool GameManager::checkNextPreloadedMapReady()
    {
        bbool mapIsReadyToSwitch = bfalse;

        const bbool isWaiting = (m_nextOperation == GAMEOPERATION_WAITPRELOADEDMAPREADY);

        ITF_ASSERT_MSG(m_nextPreloadedMap.isValid(), "Game operation error");
        ITF_ASSERT_MSG(!isWaiting || LOADINGSCREEN->isVisible(), "The loading should be visible when waiting preload end");

        if(isPreloadedMapLoaded(m_nextPreloadedMap))
        {
            i32 index = m_preLoadedPrefetchWorld.find(m_nextPreloadedMap);
            m_preLoadedPrefetchWorld.eraseNoOrder(index);

            if(isWaiting)
                LOADINGSCREEN->hide();

            m_nextOperation = GAMEOPERATION_SWITCHTOPRELOADEDMAP;
            mapIsReadyToSwitch = btrue;
        }

        return mapIsReadyToSwitch;
    }

    void GameManager::deletePreloadedMap(ObjectRef _worldRef)
    {
        // World is currently being loaded in async thread
        i32 index = m_preLoadedAsyncWorld.find(_worldRef);
        if ( index >= 0 )
        {
            m_preLoadedPendingWorldToDelete.push_back(_worldRef);
            m_preLoadedAsyncWorld.eraseNoOrder(index);

            return;
        }

        // World is available in main thread
        index = m_preLoadedPrefetchWorld.find(_worldRef);
        if ( index >= 0 )
        {
            World * _world = static_cast<World *>(_worldRef.getObject());
            if ( _world )
            {
                bbool bFound = bfalse;
                ITF_MAP<Path, ObjectRef>::iterator it = m_preloadMaps.begin();
                while(it != m_preloadMaps.end())
                {
                    if(it->second == _worldRef)
                    {
                        m_preloadMaps.erase(it);
                        bFound = btrue;
                        break;
                    }
                    ++it;
                }

                ITF_ASSERT(bFound);

                _world->stopPrefetch();
                WORLD_MANAGER->deleteWorld(_world);
            }
            m_preLoadedPrefetchWorld.eraseNoOrder(index);

            return;
        }

        ITF_WARNING_CATEGORY(GPP,NULL,bfalse,"Trying to delete a preloaded world that has never been preloaded. Did you forget to preload it ?");
    }

    void GameManager::deleteAllPreloadedMaps()
    {
        m_preLoadedPendingWorldToDelete.mergeArray(m_preLoadedAsyncWorld);

        for(u32 i = 0; i < m_preLoadedPrefetchWorld.size(); ++i)
        {
            if(World* pWorld = static_cast<World*>(m_preLoadedPrefetchWorld[i].getObject()))
            {
                pWorld->stopPrefetch();
                WORLD_MANAGER->deleteWorld(pWorld);
            }
        }
        m_preLoadedPrefetchWorld.clear();
        m_preloadMaps.clear();
    }

    void GameManager::flushPreloadMapPendingDelete()
    {
        ObjectRefList remainingList;
        for(u32 i = 0; i < m_preLoadedPendingWorldToDelete.size(); ++i)
        {
            if(World* pWorld = static_cast<World*>(m_preLoadedPendingWorldToDelete[i].getObject()))
            {
                if (!pWorld->isSceneAsyncLoadRunning())
                {
                    WORLD_MANAGER->deleteWorld(pWorld);
                }
                else
                {
                    remainingList.push_back(m_preLoadedPendingWorldToDelete[i]);
                }
            }
        }
        m_preLoadedPendingWorldToDelete = remainingList;
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::reloadCurrentMap()
    {
        emptyCurrentLevelGameState();

        // Stop all music
        MUSICMANAGER->stopAll(0.0f);

        World* pCurrentWorld = CURRENTWORLD;
        ITF_VERIFY(GAMEMANAGER->loadGameplayMap(pCurrentWorld->getRootScene()->getPath(), btrue, btrue ));
    }

    void GameManager::emptyCurrentLevelGameState()
    {
        ActorsManager::ActorDataContainerMap::iterator actorMapCurrent = ACTORSMANAGER->getActorDataContainerMapStart();
        ActorsManager::ActorDataContainerMap::iterator actorMapEnd = ACTORSMANAGER->getActorDataContainerMapEnd();

        for (;actorMapEnd!=actorMapCurrent; actorMapCurrent++)
        {
            actorMapCurrent->second.clearCheckpointArchive();
        }

        disableTutorialActor();
    }

    //////////////////////////////////////////////////////////////////////////
    void GameManager::pauseMap(bbool _pause)
    {
        if (m_rCurrentWorld!=ITF_INVALID_OBJREF)
        {
            World *pWorld = (World*)GETOBJECT(m_rCurrentWorld);
            ITF_ASSERT(pWorld);
            if (pWorld)
                pWorld->setAllowUpdate(!_pause);
        }
        if (m_rBaseWorld!=ITF_INVALID_OBJREF)
        {
            World *pWorld = (World*)GETOBJECT(m_rBaseWorld);
            ITF_ASSERT(pWorld);
            if (pWorld)
                pWorld->setAllowUpdate(!_pause && m_theGameScreen->hasGameplay());
        }

        World* playerWorld = (World*)GETOBJECT(m_rPlayerWorld);

        if ( playerWorld )
        {
            playerWorld->setAllowUpdate(!_pause);
        }

        // stop / start uv scroll on mesh
        GFX_ADAPTER->setPause(_pause);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::onSwitchToWorld(World* _pWorld, bbool _bSwitchDone)
    {
        // Make sure the clear color is not forced to black after the switch
        CONFIG->m_blackClearColorNeeded = bfalse;

        if ( _pWorld && !_bSwitchDone )
        {
            m_currentCheckpoint.invalidate();
            for (u32 i = 0; i < getMaxPlayerCount(); ++i)
            {
                Player* player = getPlayer(i);

                if ( player && player->getActive() )
                {
                    deactivatePlayer(i);

                    EventPlayerActivationChanged eventActivaton(player, btrue,bfalse);
                    EVENTMANAGER->broadcastEvent(&eventActivaton);
                }
            }
        }

        if (_pWorld && _bSwitchDone)
        {
            //u32 sceneCount = _pWorld->getSceneCount();
            //for (u32 i = 0 ; i < sceneCount ; ++i)
            //{
            //    Scene * pScene = _pWorld->getSceneAt(i);
            //    if ( pScene && pScene->getMusicTheme() != StringID::Invalid )
            //        setMusicTheme(pScene->getMusicTheme());
            //}

#ifdef ITF_SUPPORT_ONLINETRACKING
		if (CONFIG->m_enableonlineTracking==btrue)
		{
            // It's completely forbidden to include rayman's code from gameplay!!!
		//  RAY_GAMEMANAGER->getOnlineTrackingManager()->m_checkPointOrderSinceLevelBegin = 1;
		//	RAY_GAMEMANAGER->getOnlineTrackingManager()->ResetDeathCountTimers(bfalse);
		//	RAY_GAMEMANAGER->getOnlineTrackingManager()->resetStats(bfalse);
		}
#endif

        }


    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::onMapLoaded( World* _pWorld )
    {
        m_loadingMap = bfalse;
        LOADINGSCREEN->hide();

        for (u32 i=0; i<getMaxPlayerCount(); ++i)
        {
            Player* player = getPlayer(i);

            if ( player )
            {
                player->reset();
            }
        }

        if ( isPlaying() )
        {
            ITF_ASSERT_MSG(m_baseLoaded, "Base must be loaded if you want to spawn some player");

            ITF_ASSERT(_pWorld->getRootScene());

            //_pWorld->applyWorldFileSettings();
            REWARD_MANAGER->Session_New();

            bbool spawnPlayers = bfalse;

            if (m_theGameScreen)
                m_theGameScreen->onMapLoaded(_pWorld->getRef());

            if ( m_theGameScreen && m_theGameScreen->getSpawnPlayersAlways() )
            {
                spawnPlayers = btrue;
            }
            else
            {
                spawnPlayers = getCurrentCheckpoint() != NULL;
            }

            if ( spawnPlayers )
            {

                // re-activate players that were active on the world map
                for (u32 i=0; i<getMaxPlayerCount(); ++i)
                {
                    Player* player = getPlayer(i);
                    if (player && !player->getActive() && player->getActiveAndPersistent())
                    {
                        changePlayerActivation(i,btrue);
                    }
                }

                // activate the first player if none is active
                const u32 activePlayerCount = getNumActivePlayers();
                if ( activePlayerCount == 0 )
                {
                    changePlayerActivation(getMainIndexPlayer(),btrue);
                }


                if ( getCurrentCheckpoint() )
                {
                    teleportToCurrentCheckpoint(btrue);
                    CAMERACONTROLLERMANAGER->teleport(getCurrentCheckpoint()->getPos()); // because register subject have been teleported to checkpoint, we need camera to be ready before first update!
                    // http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-15330
                    // vu avec Olivier
                    //CAMERACONTROLLERMANAGER->onEndUpdateProcess(0.000001f); // because register subject have been teleported to checkpoint, we need camera to be ready before first update!
                }
            }
            else
            {
                CAMERACONTROLLERMANAGER->teleport(Vec3d::Zero);
            }

            CAMERACONTROLLERMANAGER->onMapLoaded();

            /*EventMapLoaded eventMapLoaded;
            const Actor* checkpointIn = getCheckpointIN(pBaseScene->getRef());
            if (checkpointIn)
            {
                eventMapLoaded.setSender(checkpointIn->getRef());
            }
            broadcastEvent(&eventMapLoaded);*/
        }
#ifdef ITF_SUPPORT_PLUGIN
		if(PLUGINGATEWAY) PLUGINGATEWAY->OnMapLoaded(_pWorld->getRootScene());
#endif // ITF_SUPPORT_PLUGIN

#ifndef ITF_FINAL
        if (m_funcStopLoadingProfile)
            m_funcStopLoadingProfile();
#endif //ITF_FINAL

        // Because the metronome must be reset just before the first update
        METRONOME_MANAGER->reset(METRONOME_TYPE_DEFAULT);
        METRONOME_MANAGER->setTimeSignature(METRONOME_DEFAULT_BPM,4,4,METRONOME_TYPE_DEFAULT);
        METRONOME_MANAGER->start(METRONOME_TYPE_DEFAULT);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::activatePlayer(u32 _playerIndex)
    {
        Player* player = m_players[_playerIndex];
        player->setActive(btrue);

        Actor* playerActor = player->getActor();
        ITF_ASSERT(playerActor);
        if (playerActor)
        {
            GAMEINTERFACE->onActivatePlayer(_playerIndex, playerActor);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::deactivatePlayer(u32 _playerIndex)
    {
        Player* player = m_players[_playerIndex];
        player->setActive(bfalse);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::recomputeActiveIndices()
    {
        // find the max active index (and wipe inactive players)
        i32 maxActiveIndex = -1;
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            if (m_players[i]->getActive())
            {
                maxActiveIndex = Max((i32)m_players[i]->getActiveIndex(), maxActiveIndex);
            }
            else
            {
                m_players[i]->setActiveIndex(U32_INVALID);
            }
        }

        // give the max index to newly active players, and list active players
        ITF_VECTOR<Player*> activePlayers;
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            if (m_players[i]->getActive())
            {
                if (m_players[i]->getActiveIndex() == U32_INVALID)
                    m_players[i]->setActiveIndex(++maxActiveIndex);
                activePlayers.push_back(m_players[i]);
            }
        }

        // sort players by active index
        sort(activePlayers.begin(), activePlayers.end(), comparePlayerActiveIndices);

        // reassign indices
        for (u32 i = 0; i < activePlayers.size(); i++)
        {
            activePlayers[i]->setActiveIndex(i);
        }
    }
    bool GameManager::comparePlayerActiveIndices(const Player* a, const Player* b)
    {
        return a->getActiveIndex() < b->getActiveIndex();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::swapPlayer(u32 _playerIndex, u32 _otherPlayerIndex )
    {
        if ( _playerIndex == _otherPlayerIndex )
        {
            return;
        }

        Player* myPlayer = m_players[_playerIndex];

        if ( !myPlayer )
        {
            return;
        }

        ObjectRef myRef = myPlayer->getCurrentActor();
        Actor * myActor = AIUtils::getActor(myRef);
        Player* otherPlayer = m_players[_otherPlayerIndex];

        if ( !otherPlayer )
        {
            return;
        }

        // Get the actor from the current player mode
        ObjectRef otherRef = otherPlayer->getActorForMode( myPlayer->getCurrentMode() );
        Actor * otherActor = AIUtils::getActor(otherRef);

        if ( otherActor && otherActor == myActor )
            return;

        Player::RegisteredActorList myRegisteredActors = myPlayer->getRegisteredActors();
        StringID myPlayerIDInfo = myPlayer->getCurrentPlayerIDInfo();
        myPlayer->setRegisteredActors(otherPlayer->getRegisteredActors());

        if ( otherRef != ITF_INVALID_OBJREF )
        {
            myPlayer->setCurrentActor(otherRef);
        }

        TemplateClientHandler swap;

        swap.addUsedTemplate( myPlayer->getTemplateClientHandler() );
        changePlayerIdInfoForPlayer(myPlayer,getPlayerIDInfo(otherPlayer->getCurrentPlayerIDInfo()));

        otherPlayer->setRegisteredActors(myRegisteredActors);

        if ( myRef != ITF_INVALID_OBJREF )
        {
            otherPlayer->setCurrentActor(myRef);
        }

        changePlayerIdInfoForPlayer(otherPlayer,getPlayerIDInfo(myPlayerIDInfo));

        EventPlayerIndexChange myPlayerIndexEvent(myPlayer->getIndex());
        u32 numActors = myPlayer->getRegisteredActors().size();

        for ( u32 i = 0; i < numActors; i++ )
        {
            Actor* actor = myPlayer->getRegisteredActors()[i].m_actor.getActor();

            if ( actor )
            {
                actor->onEvent(&myPlayerIndexEvent);
            }
        }

        EventPlayerIndexChange otherPlayerIndexEvent(otherPlayer->getIndex());
        numActors = otherPlayer->getRegisteredActors().size();

        for ( u32 i = 0; i < numActors; i++ )
        {
            Actor* actor = otherPlayer->getRegisteredActors()[i].m_actor.getActor();

            if ( actor )
            {
                actor->onEvent(&otherPlayerIndexEvent);
            }
        }

        if (myPlayer->getActive() && !otherPlayer->getActive())
        {
            activatePlayer(_playerIndex);

            EventPlayerActivationChanged eventActivatonMy(myPlayer, bfalse, bfalse);
            EVENTMANAGER->broadcastEvent(&eventActivatonMy);

            deactivatePlayer(_otherPlayerIndex);

            EventPlayerActivationChanged eventActivatonOther(otherPlayer, bfalse, bfalse);
            EVENTMANAGER->broadcastEvent(&eventActivatonOther);
        }
        else if (!myPlayer->getActive() && otherPlayer->getActive())
        {
            activatePlayer(_otherPlayerIndex);

            EventPlayerActivationChanged eventActivatonOther(otherPlayer, bfalse, bfalse);
            EVENTMANAGER->broadcastEvent(&eventActivatonOther);

            deactivatePlayer(_playerIndex);

            EventPlayerActivationChanged eventActivatonMy(myPlayer, bfalse, bfalse);
            EVENTMANAGER->broadcastEvent(&eventActivatonMy);
        }

        // teleport
        if ( myActor && otherActor )
        {
        EventTeleport myPos(myActor->getPos(),myActor->getAngle(),btrue);
        EventTeleport otherPos(otherActor->getPos(),otherActor->getAngle(),btrue);

        otherActor->onEvent(&myPos);
        myActor->onEvent(&otherPos);
        }

        // send game mode change on swapped actors to be sure that the new actor is in current player mode
        EventPlayerModeChanged eventGameMode;
        eventGameMode.setMode( myPlayer->getCurrentMode() );
        eventGameMode.setActivator( otherRef );
        myPlayer->broadcastEventToModeActors( &eventGameMode );

        eventGameMode.setMode( otherPlayer->getCurrentMode() );
        eventGameMode.setActivator( myRef );
        otherPlayer->broadcastEventToModeActors( &eventGameMode );

        // Tell to game mode controllers
        for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
        {
            m_GameModeControllers[i]->onPlayerSwapped( _playerIndex, _otherPlayerIndex );
        }
    }

    void GameManager::changePlayerIdInfoForPlayer( Player* _player, const PlayerIDInfo* _newPlayerId )
    {
        _player->setCurrentPlayerIDInfo(_newPlayerId);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    Player* GameManager::getPlayer( u32 _index )
    {
        Player* result = NULL;

        if(_index < getMaxPlayerCount())
        {
            result = m_players[_index];
        }
        else
        {
            ITF_ASSERT_MSG(0, "Invalid player index");
        }

        return result;
    }

    //////////////////////////////////////////////////////////////////////////
    Player* GameManager::getPlayerFromActiveIndex(u32 _activeIndex)
    {
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            Player* player = m_players[i];
            if ( player->getActiveIndex() == _activeIndex )
            {
                return player;
            }
        }
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    Player* GameManager::getPlayerFromID( StringID _playerID )
    {
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            Player* player = m_players[i];
            if ( player->getID() == _playerID )
            {
                return player;
            }
        }

        ITF_WARNING(NULL, 0, "no player found with ID: %s", _playerID.getDebugString());
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    bbool GameManager::isPlayerActor( ObjectRef _actorRef ) const
    {
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            if ( m_players[i]->isRegisteredActor(_actorRef) )
            {
                return btrue;
            }
        }

        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    Player* GameManager::getPlayerFromActor( ObjectRef _playerActorRef, bbool _warnIfNotAPlayer )
    {
        ITF_ASSERT(_playerActorRef != ITF_INVALID_OBJREF);

        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            Player* player = m_players[i];
            if ( player->isRegisteredActor(_playerActorRef) )
            {
                return player;
            }
        }

        ITF_ASSERT_MSG(!_warnIfNotAPlayer, "this actor does not belong to a player!");
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    Player* GameManager::getPlayerAndModeFromActor(ObjectRef _actorRef, GameMode & _res_Mode )
    {
        ITF_ASSERT(_actorRef != ITF_INVALID_OBJREF);
        _res_Mode = GAMEMODE_UNKNOWN;

        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            Player* player = m_players[i];
            GameMode gamemode = player->getModeFromActor( _actorRef );
            if ( gamemode != GAMEMODE_UNKNOWN )
            {
                _res_Mode = gamemode;
                return player;
            }
        }
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    Actor* GameManager::getActivePlayer(u32 _index) const
    {
        ITF_ASSERT_CRASH(_index < getMaxPlayerCount(),"Accessing array out of bounds");
        return m_players[_index]->getActive() ? m_players[_index]->getActor() : NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    Actor* GameManager::getActiveAndPersistentPlayer(u32 _index) const
    {
        ITF_ASSERT_CRASH(_index < getMaxPlayerCount(),"Accessing array out of bounds");
        return m_players[_index]->getActiveAndPersistent() ? m_players[_index]->getActor() : NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    u32 GameManager::getNumActivePlayers() const
    {
        u32 count = 0;
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
            count += m_players[i]->getActive()?1:0;
        return count;
    }


    //////////////////////////////////////////////////////////////////////////
    u32 GameManager::getNumActiveAndPersistent() const
    {
        u32 count = 0;
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
            count += m_players[i]->getActiveAndPersistent()?1:0;
        return count;
    }

    Player::Player()
        : m_index(U32_INVALID)
        , m_originalPlayerIDInfoIndex(U32_INVALID)
        , m_active(bfalse)
        , m_activeAndPersistent(bfalse)
        , m_activeIndex(U32_INVALID)
        , m_currentMode(GAMEMODE_UNKNOWN)  // unknown
        , m_currentActorIndex(U32_INVALID)
        , m_hasJoinedBefore(bfalse)
        , m_isDisconnected(bfalse)
    {
        ITF_MemSet(m_buts,0,sizeof(m_buts));
        reset();
        m_cellsView = View::createView("player", bfalse, bfalse);
        m_cellsView->setActive(bfalse);
    }

    Player::~Player()
    {
        if (m_cellsView)
            View::deleteView(m_cellsView);
        m_templateClient.freeUsedTemplates();
    }

    bbool Player::isRegisteredActor( ObjectRef _actorRef ) const
    {
        u32 numRegisteredActors = m_registeredActors.size();

        for ( u32 i = 0; i < numRegisteredActors; i++ )
        {
            if ( m_registeredActors[i].m_actor == _actorRef )
            {
                return btrue;
            }
        }

        return bfalse;
    }

    void Player::setActive(bbool _val)
    {
        m_active = _val;
        m_cellsView->setActive(_val);

        u32 numRegisteredActors = getRegisteredActors().size();

        if ( _val )
        {
            for ( u32 i = 0; i < numRegisteredActors; i++ )
            {
                const Player::RegisteredActor& regActor = getRegisteredActors()[i];
                u32 numGameModes = regActor.m_gameModes.size();
                bbool found = bfalse;

                for ( u32 j = 0; j < numGameModes; j++ )
                {
                    if ( regActor.m_gameModes[j] == getCurrentMode() )
                    {
                        found = btrue;
                        break;
                    }
                }

                if ( found )
                {
                    Actor* actor = regActor.m_actor.getActor();

                    if ( actor )
                    {
                        actor->enable();
                    }
                }
            }
        }
        else
        {
            for ( u32 i = 0; i < numRegisteredActors; i++ )
            {
                const Player::RegisteredActor& regActor = getRegisteredActors()[i];
                Actor* actor = regActor.m_actor.getActor();

                if ( actor && !regActor.m_alwaysActive )
                {
                    actor->disable();
                }
            }
        }
    }

    void Player::updateCellsView()
    {
        Actor* pActor = getActor();
        if (pActor)
        {
            AABB projAABB;//  = pActor->getAABB(); WARNING!!! Don't use getAABB(), it can be huge when teleporting the player
            projAABB.setMin(pActor->getPos().truncateTo2D() - Vec2d(2.f, 2.f));
            projAABB.setMax(projAABB.getMin() + Vec2d(4.f, 4.f));
            projAABB.projectTo2D(CAMERA->getDeltaX(), CAMERA->getDeltaY(), pActor->getDepth());
            m_cellsView->setTargetAABB(projAABB);
        }
    }

	void Player::broadcastEventToModeActors( Event * _event, GameMode _exceptmode )
	{
        u32 numRegisteredActors = m_registeredActors.size();

		for ( u32 i = 0; i < numRegisteredActors; i++ )
		{
            RegisteredActor& regActor = m_registeredActors[i];

			if ( regActor.m_gameModes.size() == 1 && regActor.m_gameModes[0] == _exceptmode )
				continue;

			Actor* actor = regActor.m_actor.getActor();

			if ( actor != NULL )
			{
				actor->onEvent( _event );
			}
		}
	}

    void Player::registerActor( ObjectRef _actorRef, GameMode _mode, bbool _alwaysActive, bbool _isPlayable )
    {
		ITF_ASSERT( _mode != GAMEMODE_UNKNOWN );

        u32 numRegistered = m_registeredActors.size();

        for ( u32 i = 0; i < numRegistered; i++ )
        {
            RegisteredActor& regActor = m_registeredActors[i];

            if ( regActor.m_actor == _actorRef )
            {
                regActor.m_gameModes.push_back(_mode);
                return;
            }
        }

        RegisteredActor regActor;

        regActor.m_actor = _actorRef;
        regActor.m_gameModes.push_back(_mode);
        regActor.m_alwaysActive = _alwaysActive;
        regActor.m_isPlayable = _isPlayable;

        m_registeredActors.push_back(regActor);
    }

    void Player::unregisterActor( ObjectRef _actorRef )
    {
        u32 numRegistered = m_registeredActors.size();

        for ( u32 i = 0; i < numRegistered; i++ )
        {
            RegisteredActor& regActor = m_registeredActors[i];

            if ( regActor.m_actor == _actorRef )
            {
                m_registeredActors.removeAt( i );
                break;
            }
        }
    }

	GameMode Player::getFirstAvailableGameMode()
	{
		if ( m_registeredActors.size() > 0 )
		{
            return m_registeredActors[0].m_gameModes[0];
		}

		return GAMEMODE_UNKNOWN;
	}

    GameMode Player::getModeFromActor( ObjectRef _actorRef )
    {
        u32 numRegisteredActors = m_registeredActors.size();

        for ( u32 i = 0; i < numRegisteredActors; i++ )
        {
            RegisteredActor& regActor = m_registeredActors[i];

            if ( regActor.m_actor == _actorRef && regActor.m_gameModes.size() > 0 )
            {
                return regActor.m_gameModes[0];
            }
        }

        return GAMEMODE_UNKNOWN;
    }

    ObjectRef  Player::getActorForMode( GameMode _mode, bbool _getPlayableActor /*= btrue*/ )
    {
        u32 numRegisteredActors = m_registeredActors.size();

        for ( u32 i = 0; i < numRegisteredActors; i++ )
        {
            RegisteredActor& regActor = m_registeredActors[i];
            u32 numGameModes = regActor.m_gameModes.size();

            if ( ( regActor.m_isPlayable && _getPlayableActor ) || ( !regActor.m_isPlayable && !_getPlayableActor ) )
            {
                for ( u32 i = 0; i < numGameModes; i++ )
                {
                    if ( regActor.m_gameModes[i] == _mode )
                    {
                        return regActor.m_actor;
                    }
                }
            }
        }

        return ITF_INVALID_OBJREF;
    }

    void Player::changeGameMode( GameMode _mode )
    {
        if ( m_currentMode == _mode )
        {
            //ITF_ASSERT_MSG(0, "mode already active!");
            return;
        }

        m_currentMode = _mode;

        resetCurrentRegisteredActors();

        GAMEMANAGER->playerGameModeHasChanged( this );

	/*	// Create event
		EventPlayerModeChanged eventGameMode;
		eventGameMode.setMode(m_currentMode);
		eventGameMode.setActivator(m_currentActor);


		// Broadcast
        if (_broadcastChange)
        {
            EVENTMANAGER->broadcastEvent(&eventGameMode);
        }
		// Send event to all actors of player
		else
		{

		}*/
    }

    ActorRef Player::getCurrentActor() const
    {
        if ( m_currentActorIndex >= m_registeredActors.size() )
        {
            return ActorRef::InvalidRef;
        }

        return m_registeredActors[m_currentActorIndex].m_actor;
    }

    void Player::setCurrentActor( ObjectRef _actorRef )
    {
        m_currentActorIndex = U32_INVALID;

        u32 numRegisteredActors = m_registeredActors.size();

        bool ok  = false;
        for ( u32 i = 0; i < numRegisteredActors; i++ )
        {
            RegisteredActor& regActor = m_registeredActors[i];

            if ( regActor.m_actor == _actorRef )
            {
                m_currentActorIndex = i;
                ok = true;
                break;
            }
        }
        ITF_ASSERT(ok);
    }

    void Player::setCurrentPlayerIDInfo( const PlayerIDInfo* _id )
    {
        if ( _id == NULL )
        {
            m_templateClient.freeUsedTemplates();
            m_currentPlayerIDInfo.invalidate();
            return;
        }

        StringID id( _id->getId() );

        if ( m_currentPlayerIDInfo != id )
        {
            m_templateClient.freeUsedTemplates();
            for ( u32 gs = 0; gs < _id->getNumGameScreens(); ++gs )
            {
                const PlayerIDInfo::GameScreenInfo& gamescreen = _id->getGameScreenInfoAt( gs );

                for ( u32 act = 0; act < gamescreen.getNumActors(); ++act )
                {
                    const PlayerIDInfo::ActorInfo& actorInfo = gamescreen.getActorInfo( act );

                    for ( u32 mode = 0; mode < actorInfo.getNumGameModes(); ++mode )
                        // actorInfo.getGameMode( mode ) == 0 -> 0 stand for RAY_GAMEMODE_PLATFORMER from Ray_GameplayTypes.h...
                        if ( actorInfo.getGameMode( mode ) == 0 )
                        {
#ifdef ITF_WINDOWS
                            if ( TEMPLATEDATABASE->hasTemplate( actorInfo.getFile().getStringID() ) )
#elif !defined( ITF_FINAL )
                            if ( !FILEMANAGER->isRemote() || TEMPLATEDATABASE->hasTemplate( actorInfo.getFile().getStringID() ) )
#endif
                                m_templateClient.addUsedTemplate( actorInfo.getFile().getStringID() );
                        }
                }
            }
            m_currentPlayerIDInfo = id;
        }
    }

    void Player::setRegisteredActors( const RegisteredActorList& _list, bbool _resetActors )
    {
        m_registeredActors = _list;

        if ( _resetActors )
        {
            resetCurrentRegisteredActors();
        }
    }

    void Player::resetCurrentRegisteredActors()
    {
        Actor* prevActor = getCurrentActor().getActor();

        m_currentActorIndex = U32_INVALID;

        if ( ( m_registeredActors.size() > 0 ) && ( m_currentMode != GAMEMODE_UNKNOWN ) )
        {
            u32 numRegisteredActors = m_registeredActors.size();

            for ( u32 i = 0; i < numRegisteredActors; i++ )
            {
                RegisteredActor& regActor = m_registeredActors[i];
                u32 numGameModes = regActor.m_gameModes.size();
                bbool found = bfalse;

                for ( u32 j = 0; j < numGameModes; j++ )
                {
                    if ( regActor.m_gameModes[j] == m_currentMode )
                    {
                        if ( regActor.m_isPlayable && m_currentActorIndex == U32_INVALID )
                        {
                            m_currentActorIndex = i;
                        }

                        found = btrue;
                        break;
                    }
                }

                Actor* actor = regActor.m_actor.getActor();

                if ( actor )
                {
                    if ( found )
                    {
                        if ( getActive() )
                        {
                            actor->enable();
                        }
                    }
                    else
                    {
                        if ( !regActor.m_alwaysActive )
                        {
                            actor->disable();
                        }
                    }
                }
            }
        }

        Actor* currentActor = getCurrentActor().getActor();

        if ( prevActor && prevActor != currentActor )
        {
            CAMERACONTROLLERMANAGER->unregisterSubject(prevActor->getRef());
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::onReachedCheckpoint(ObjectRef _checkpoint, bbool _force)
    {
        if ( m_currentCheckpoint == _checkpoint && !_force)
        {
            return;
        }

        EventCheckpointReached eventCheckpoint;
        eventCheckpoint.setSender(_checkpoint);
        EVENTMANAGER->broadcastEvent(&eventCheckpoint);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::deactivate()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    void GameManager::endOfLevelReached(bbool _saveGameState /*= btrue*/, bbool _levelCompleted /*= btrue*/)
    {
        if (m_theGameScreen)
            m_theGameScreen->onEndOfLevelReached();
    }


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::checkpointSave()
    {
        World* pWorld = CURRENTWORLD;
        if(!pWorld)
            return;

        EventPreCheckpointSave preCheckpointSave;
        EVENTMANAGER->broadcastEvent(&preCheckpointSave);

        Scene* pScene = pWorld->getRootScene();
        pScene->forcedActorsSaveCheckpoint();

        u32 numPlayers = m_players.size();

        for ( u32 i = 0; i < numPlayers; i++ )
        {
            Player* p = m_players[i];

            if ( p->getActive() )
            {
                Actor* playerActor = p->getActor();

                if ( playerActor )
                {
                    playerActor->saveCheckpointData();
                }
            }
        }

        if ( m_checkpointData )
        {
            SF_DEL(m_checkpointData);
        }

        m_checkpointData = newAlloc(mId_Editor,ArchiveMemory);
        CSerializerObjectBinary serializeBin(m_checkpointData);
        Serialize(&serializeBin,ESerialize_Checkpoint_Save);

        saveCheckpointLevelState();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::checkpointLoad()
    {
        Actor * checkpoint = AIUtils::getActor(m_currentCheckpoint);

        if (checkpoint)
        {
            m_currentSceneRef = checkpoint->getSceneRef();

            loadCheckpointLevelState();

            if ( m_checkpointData )
            {
                m_checkpointData->rewindForReading();
                CSerializerObjectBinary serializeBin(m_checkpointData);
                Serialize(&serializeBin,ESerialize_Checkpoint_Load);
            }

            EVENTDELAY_HANDLER->reset();

            checkpointLoadScene();

            // Tell to game mode controllers
            for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
            {
                m_GameModeControllers[i]->onCheckPointLoaded( checkpoint->getRef() );
            }

            CAMERACONTROLLERMANAGER->unregisterNPCFromCamera();

            teleportToCurrentCheckpoint(bfalse);

            CAMERACONTROLLERMANAGER->teleport(checkpoint->getPos()); // because register subject have been teleported to checkpoint, we need camera to be ready before first update!
            // http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-15330
            // vu avec Olivier
            //CAMERACONTROLLERMANAGER->onEndUpdateProcess(0.000001f); // we need camera to be ready before next update

            // notify players
            EventRevertToLastCheckpoint eventRevert;
            eventRevert.setSender(checkpoint->getRef());
            EVENTMANAGER->broadcastEvent( &eventRevert );
#if defined(ITF_SUPPORT_RAKI)
            SOUND_ADAPTER->resetBusses();
#endif
            METRONOME_MANAGER->reset(METRONOME_TYPE_DEFAULT);
            METRONOME_MANAGER->setTimeSignature(METRONOME_DEFAULT_BPM,4,4,METRONOME_TYPE_DEFAULT);
            METRONOME_MANAGER->start(METRONOME_TYPE_DEFAULT);


            // hide tutorial
            if (inTutorial())
            {
                hideTutorial();
            }
        }
        else
        {
            ITF_ASSERT_MSG(0, "no active checkpoint found");
        }
    }

    void GameManager::teleportPlayer(u32 _player, const Vec2d & _pos, f32 _sceneZ, bbool _teleportCamera )
    {
        Actor* actor = GAMEMANAGER->getActivePlayer(_player);

        if ( !actor )
        {
            return;
        }

        Vec3d targetPos = _pos.to3d(_sceneZ);

        if ( actor->getTemplate()->isZForced() )
        {
            targetPos.m_z += actor->getTemplate()->getForcedZ();
        }

        EventTeleport teleport(targetPos,actor->getAngle(),btrue);
        actor->onEvent(&teleport);

        if (_teleportCamera)
        {
            CAMERACONTROLLERMANAGER->teleport( targetPos );
        }

        if (actor->getBinding())
        {
            BaseObject* obj = actor->getBinding()->getParent().getObject();

            if ( obj )
            {
                ObjBinding* bind = NULL;

                if ( PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)) )
                {
                    bind = poly->getBinding();
                }
                else if ( Actor* parentActor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                {
                    bind = parentActor->getBinding();
                }

                if ( bind )
                {
                    bind->unbindChild(actor->getRef());
                    bind->forceRemovePending();
                }
            }

            actor->getBinding()->clear();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::checkpointLoadScene()
    {
        World* pWorld = CURRENTWORLD;

        if( pWorld )
        {
            Scene* pScene = pWorld->getRootScene();

            if ( pScene )
            {
                pScene->forcedActorsLoadCheckpoint();
            }
        }

        u32 numPlayers = m_players.size();

        for ( u32 i = 0; i < numPlayers; i++ )
        {
            Player* p = m_players[i];

            if ( p->getActive() )
            {
                Actor* playerActor = p->getActor();

                if ( playerActor )
                {
                    playerActor->postLoadCheckpointData();
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::registerCheckpoint(ObjectRef _world,ObjectRef _checkpoint)
    {
        ITF_ASSERT(_checkpoint.isValid());
        //for (SceneCheckpointList::iterator it = m_checkpoints.begin(); it != m_checkpoints.end(); ++it)
        //{
        //    if (
        //}
        if (find(m_checkpoints.begin(),m_checkpoints.end(),std::pair<ObjectRef,ObjectRef>(_world,_checkpoint)) != m_checkpoints.end())
        {
            LOG("Checkpoint already registered");
            return;
        }

        m_checkpoints.push_back(std::pair<ObjectRef,ObjectRef>(_world,_checkpoint));
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::unregisterCheckpoint(ObjectRef _world,ObjectRef _checkpoint)
    {
        ITF_ASSERT(_checkpoint.isValid());
        WorldCheckpointList::iterator it = find(m_checkpoints.begin(),m_checkpoints.end(),std::pair<ObjectRef,ObjectRef>(_world,_checkpoint));
        if ( it == m_checkpoints.end())
        {
            LOG("Checkpoint already unregistered");
            return;
        }

        m_checkpoints.erase(it);
    }

    Actor* GameManager::getCheckpointFromPath( const ObjectPath& _path ) const
    {
        Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(_path);

        if ( pickable )
            {
            return pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        }

        return NULL;
    }

    Actor* GameManager::getCheckpointFromIndex( ObjectRef _world, u32 _index, bbool _forward ) const
    {
        Actor* checkpointFound = NULL;
        u32 closest = U32_INVALID;
        u32 loop = U32_INVALID;
        Actor* firstCheckpointFound = NULL;

        for (WorldCheckpointList::const_iterator it = m_checkpoints.begin(); it != m_checkpoints.end(); ++it)
        {
            if(it->first != _world)
                continue;

            Actor* checkpoint = AIUtils::getActor(it->second);

            if ( checkpoint )
            {
                CheckpointComponent * checkpointC = checkpoint->GetComponent<CheckpointComponent>();

                if ( checkpointC )
                {
                    if ( !firstCheckpointFound ||
                       ( ( _forward && checkpointC->getIndex() < loop ) ||
                         ( !_forward && checkpointC->getIndex() > loop ) ) )
                    {
                        firstCheckpointFound = checkpoint;
                        loop = checkpointC->getIndex();
                    }

                    if ( ( _forward && checkpointC->getIndex() >= _index ) ||
                         ( !_forward && checkpointC->getIndex() <= _index ) )
                    {
                        u32 tmp = _forward ? checkpointC->getIndex() - _index : _index - checkpointC->getIndex();

                        if ( !checkpointFound || tmp < closest )
                        {
                            checkpointFound = checkpoint;
                            closest = tmp;
                        }
                    }
                }
            }
        }

        if ( !checkpointFound )
        {
            checkpointFound = firstCheckpointFound;
        }

        return checkpointFound;
    }

    void GameManager::onEvent(Event* _event)
    {
        if ( EventPlayerModeChanged* eventGameMode = _event->DynamicCast<EventPlayerModeChanged>(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275)) )
        {
            processPlayerGameModeChanged(eventGameMode);
        }
    }

    void GameManager::setCurrentCheckpoint( const ObjectPath& _path )
    {
        Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(_path);

        ITF_ASSERT_MSG(pickable,"Checkpoint not found");

        if ( pickable )
        {
            setCurrentCheckpoint(pickable);
        }
    }

    void GameManager::setCurrentSceneReference( Scene* _scene )
    {
        m_currentSceneRef = _scene ? _scene->getRef() : ITF_INVALID_OBJREF;
    }

    Scene* GameManager::getCurrentSceneReference() const
    {
        BaseObject* obj = m_currentSceneRef.getObject();

        if ( obj )
        {
            return obj->DynamicCast<Scene>(ITF_GET_STRINGID_CRC(Scene,209039730));
        }
        else
        {
            return NULL;
        }
    }

    void GameManager::setCurrentCheckpoint( Pickable* _pickable )
    {
        m_currentCheckpoint = _pickable->getRef();
        m_currentSceneRef = _pickable->getSceneRef();
    }

    Actor* GameManager::getFirstCheckpoint(ObjectRef _world) const
    {
        Actor* newCheckp = NULL;

        if ( !m_firstCheckpointPath.isValid() )
        {
            newCheckp = getCheckpointFromIndex(_world, 0, btrue);
        }
        else
        {
            newCheckp = getCheckpointFromPath(m_firstCheckpointPath);
        }

        return newCheckp;
    }

    void GameManager::teleportToNextCheckpoint( bbool _next )
    {
        ObjectRef currentWorld = getCurrentWorld();

        // Gamemode check
        bbool result = bfalse;
        for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
        {
            result = result || m_GameModeControllers[i]->teleportToNextCheckpoint( _next );
        }

        if ( result )
            return;

        Actor* checkpoint = AIUtils::getActor(m_currentCheckpoint);

        if ( !checkpoint )
        {
            checkpoint = getFirstCheckpoint(currentWorld);
        }

        if ( checkpoint )
        {
            CheckpointComponent* comp = checkpoint->GetComponent<CheckpointComponent>();

            if ( comp )
            {
                u32 nextIndex = comp->getIndex();

                if ( _next )
                {
                    nextIndex += 1;
                }
                else
                {
                    nextIndex -= 1;
                }

                Actor* newCheckp = getCheckpointFromIndex(currentWorld, nextIndex,_next);

                ITF_ASSERT(newCheckp);

                if ( newCheckp )
                {
                    teleportToCheckpoint(newCheckp);
                }
            }
        }
    }

    Actor* GameManager::getCurrentCheckpoint()
    {
        Actor* checkpoint = AIUtils::getActor(m_currentCheckpoint);

        if ( !checkpoint )
        {
            checkpoint = getFirstCheckpoint(getCurrentWorld());
        }

        return checkpoint;
    }

    void GameManager::teleportToCurrentCheckpoint( bbool _save )
    {
        Actor* currentChk = getCurrentCheckpoint();

        ITF_ASSERT(currentChk);

        if ( currentChk )
        {
            teleportToCheckpoint(currentChk);

            if ( _save )
            {
                m_nextOperation = GAMEOPERATION_SAVECHECKPOINT;
            }
        }
    }

    void GameManager::teleportToFirstCheckpoint( bbool _save )
    {
        Actor* newCheckp = getFirstCheckpoint(getCurrentWorld());

        ITF_ASSERT(newCheckp);

        if ( newCheckp )
        {
            teleportToCheckpoint(newCheckp);

            if ( _save )
            {
                m_nextOperation = GAMEOPERATION_SAVECHECKPOINT;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void GameManager::resetToLastCheckpoint()
    {
        m_nextOperation = GAMEOPERATION_LOADCHECKPOINT;
    }
    void GameManager::resetToLastCheckpointAndSave()
    {
        m_nextOperation = GAMEOPERATION_LOADCHECKPOINTANDSAVE;
    }

    void GameManager::teleportToCheckpoint( Actor* _checkpoint )
    {
        CheckpointComponent * chkp = _checkpoint->GetComponent<CheckpointComponent>();
        if (chkp )
        {
            if (chkp->canSpawnPlayer())
            {
                f32 sceneZ = AIUtils::getSceneZFromPickable(_checkpoint);

                for (u32 i = 0; i < getMaxPlayerCount(); i++)
                {
                    Actor* p = getActivePlayer(i);

                    if(!p)
                    {
                        continue;
                    }

                    Vec2d pos = chkp->getPlayerSpawnPos(i);

                    p->setFlipped(chkp->getPlayerSpawnFlipped(i));

                    teleportPlayer(i,pos,sceneZ, btrue);
                }
            }

            setCurrentCheckpoint(_checkpoint);
        }
        else
        {
            f32 sceneZ = AIUtils::getSceneZFromPickable(_checkpoint);
            for (u32 i = 0; i < getMaxPlayerCount(); i++)
            {
                Actor* p = getActivePlayer(i);

                if(!p)
                {
                    continue;
                }

                teleportPlayer(i,_checkpoint->get2DPos(),sceneZ,btrue);

                p->setFlipped(_checkpoint->isFlipped());
            }
        }
    }

    void GameManager::onPostSceneActivation( const Scene* _pScene, bbool _bActive )
    {
// FRED TEST        GAMEINTERFACE->onPostSceneActivation(_pScene, _bActive);
		if (!_bActive)
			GAMEINTERFACE->onPostSceneActivation(_pScene, _bActive);
    }

    void GameManager::swapToWorld(ObjectRef _worldRef)
    {
        World * world = static_cast<World *>(_worldRef.getObject());
        ITF_ASSERT_MSG(world != NULL,"Trying to change current world with an invalid world, GameScreen might be broken if you continue");
        if ( world )
            m_theGameScreen->setInternalWorld(world);
        m_rCurrentWorld = _worldRef;
    }

	//-----------------------------------------------------------------------------------------------------------------
	void GameManager::changePlayerGameModeWithControllers( Player * _player, GameMode _mode, bbool _sendEvent )
	{
		ITF_ASSERT( _player != NULL );

        if ( _player != NULL )
        {
            if (  _player->getCurrentMode() != _mode )
            {
                if ( _mode == GAMEMODE_UNKNOWN )
                {
                    _player->changeGameMode( GAMEMODE_UNKNOWN );
                }
                else if ( getGameModeController(_mode)->requestPlayerModeChange( _player ) )
		        {
			        _player->changeGameMode( _mode );

                    // check player swap
                    Player * realPlayer = _player;

			        if (_sendEvent )
			        {
				        // Create event
				        EventPlayerModeChanged eventGameMode;
				        eventGameMode.setMode( realPlayer->getCurrentMode() );
				        eventGameMode.setActivator( realPlayer->getCurrentActor() );

				        realPlayer->broadcastEventToModeActors( &eventGameMode );
			        }
                }
            }

            // remove from before "changed requests"
            for ( u32 i = 0; i < m_GameModeControllers.size(); i++ )
            {
                if ( i != _mode )
                {
                    m_GameModeControllers[i]->removePlayerModeChangeRequest( _player );
                }
            }
		}
	}
	//-----------------------------------------------------------------------------------------------------------------
	void GameManager::processPlayerGameModeChanged( EventPlayerModeChanged* _eventGameMode )
	{
		Player* player = getPlayerFromActor(_eventGameMode->getActivator());
		if (player)
		{
			changePlayerGameMode( player, _eventGameMode->getMode() );
		}
	}
	//-----------------------------------------------------------------------------------------------------------------
	// Update all game manager game modes
	//
	void GameManager::updateGameModes()
	{
		for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
		{
			m_GameModeControllers[i]->update();
		}
	}
    //-----------------------------------------------------------------------------------------------------------------
    // tell game mode controllers that the game mode has changed
    //
    void GameManager::playerGameModeHasChanged( Player * _player )
    {
        for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
        {
            m_GameModeControllers[i]->onPlayerGameModeHasChanged( _player );
        }
    }

	//-----------------------------------------------------------------------------------------------------------------
	void GameManager::changePlayerGameMode( u32 _index, GameMode _mode )
	{
		ITF_ASSERT( _mode != GAMEMODE_UNKNOWN );

		Player* player = getPlayer( _index );
        changePlayerGameMode( player, _mode );
	}

    //-----------------------------------------------------------------------------------------------------------------
    void GameManager::changePlayerGameMode( Player * _player, GameMode _mode, bbool _sendEvent/* = btrue*/ )
    {
        if ( _player != NULL )
        {
            changePlayerGameModeWithControllers( _player, _mode, _sendEvent );
        }
    }

	//-----------------------------------------------------------------------------------------------------------------
	bbool GameManager::areAllActivePlayersInGameMode( u32 _mode )
	{
		for (u32 i = 0; i < getMaxPlayerCount(); i++)
		{
			Player* player = getPlayer(i);
			if ( player->getActive() )
			{if ( player->getCurrentMode() != _mode )
				{
					return bfalse;
				}
			}
		}

		return btrue;
	}

    //-----------------------------------------------------------------------------------------------------------------
    void GameManager::updateReturnToMainMenu()
    {
        if(m_backToTheMainMenuASAP && (!TRC_ADAPTER || !TRC_ADAPTER->isDisplayingError()))
        {
            m_backToTheMainMenuASAP = bfalse;
            goToTheMainMenu();
        }
    }

    // CASA::CTR
    Actor* GameManager::getCheckpointEnd()
    {
        WorldCheckpoint _Checkpoint = m_checkpoints.back();
        Actor* checkpoint = AIUtils::getActor(_Checkpoint.second);
        return checkpoint;
    }

    void GameManager::removePlayerActors()
    {
        World* playerWorld = (World*)GETOBJECT(m_rPlayerWorld);

        if ( playerWorld )
        {
            Scene* playerScene = playerWorld->getRootScene();

            if ( playerScene )
            {
                u32 numPlayers = m_players.size();

                for ( u32 i = 0; i < numPlayers; i++ )
                {
                    Player* p = getPlayer(i);
                    const Player::RegisteredActorList& registeredActors = p->getRegisteredActors();
                    u32 numRegistered = registeredActors.size();

                    for ( u32 i = 0; i < numRegistered; i++ )
                    {
                        Actor* actor = static_cast<Actor*>(registeredActors[i].m_actor.getActor());

                        if ( actor )
                        {
                            actor->requestDestruction();
                        }
                    }

                    p->clearRegisteredActors();
                }

                playerScene->flushPending();
                // tell the player actors list destruction to gamemode controllers
                //
                for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
                {
                    m_GameModeControllers[i]->onPlayerActorsRemoved();
                }
            }
        }
    }

    void GameManager::spawnPlayerActors( const StringID& _gameScreen )
    {
        removePlayerActors();

        World* playerWorld = (World*)GETOBJECT(m_rPlayerWorld);

        if ( playerWorld )
        {
            Scene* playerScene = playerWorld->getRootScene();

            if ( playerScene )
            {
                u32 numPlayers = m_players.size();

                for ( u32 i = 0; i < numPlayers; i++ )
                {
                    PlayerIDInfo* playerIdInfo = (PlayerIDInfo*)getPlayerIDInfoFromPlayerIndex(i);

                    if ( playerIdInfo )
                    {
                        const PlayerIDInfo::GameScreenInfo* gsInfo = playerIdInfo->getGameScreenInfo(_gameScreen);

                        if ( gsInfo )
                        {
                            Player* p = getPlayer(i);

                            if ( p )
                            {
                                EventPlayerIndexChange playerIndexEvent(i);

                                u32 numActors = gsInfo->getNumActors();

                                for ( u32 i = 0; i < numActors; i++ )
                                {
                                    const PlayerIDInfo::ActorInfo& actorInfo = gsInfo->getActorInfo(i);

                                    ActorsManager::SpawnInfo spawnInfo;

                                    spawnInfo.m_pScene = playerScene;
                                    spawnInfo.m_luaPath = actorInfo.getFile();
                                    spawnInfo.m_init = btrue;
                                    spawnInfo.m_serializable = btrue; // so that we can save on checkpoints

                                    Actor* newActor = ACTORSMANAGER->spawnActor(spawnInfo);

                                    if ( newActor )
                                    {
                                        newActor->loadResources();

                                        bbool matchGameMode = bfalse;
                                        u32 numGameModes = actorInfo.getNumGameModes();
                                        bbool isAlwaysActive = actorInfo.isAlwaysActive();

                                        for ( u32 i = 0; i < numGameModes; i++ )
                                        {
                                            GameMode gameMode = actorInfo.getGameMode(i);
                                            matchGameMode |= (p->getCurrentMode() == gameMode) || isAlwaysActive;
                                            p->registerActor(newActor->getRef(),gameMode,actorInfo.isAlwaysActive(), actorInfo.isPlayable() );
                                        }

                                        newActor->onEvent(&playerIndexEvent);

                                        if ( (p->getActive() && matchGameMode) || isAlwaysActive )
                                        {
                                            newActor->enable();
                                        }
                                        else
                                        {
                                            newActor->disable();
                                        }
                                    }
                                }

                                p->resetCurrentRegisteredActors();
                            }
                        }
                    }
                }
            }

            // tell the player actor spawn to gamemode controllers
            //
            for ( u32 i = 0; i < m_GameModeControllers.size(); ++i )
            {
                m_GameModeControllers[i]->spawnPlayerActors( _gameScreen );
            }
        }
    }

    const PlayerIDInfo* GameManager::getPlayerIDInfoFromPlayerIndex( u32 _playerIndex ) const
    {
        const Player* player = m_players[_playerIndex];

        if ( player->getCurrentPlayerIDInfo().isValid() )
        {
            const ITF_VECTOR<PlayerIDInfo*>& playerIDInfo = m_configTemplate->getPlayersIDInfo();
            u32 numIds = playerIDInfo.size();

            for ( u32 i = 0; i < numIds; i++ )
            {
                const PlayerIDInfo* idInfo = playerIDInfo[i];

                if ( StringID(idInfo->getId().cStr()) == player->getCurrentPlayerIDInfo() )
                {
                    return idInfo;
                }
            }
        }

        return NULL;
    }

    bbool GameManager::isPlayerIDInfoUsed( const StringID& _playerIDInfo ) const
    {
        u32 numPlayers = m_players.size();

        for ( u32 i = 0; i < numPlayers; i++ )
        {
            const Player* player = m_players[i];

            if ( player && player->getActive() && player->getCurrentPlayerIDInfo() == _playerIDInfo )
            {
                return btrue;
            }
        }

        return bfalse;
    }

    bbool GameManager::isCustomPlayerIDInfo( const PlayerIDInfo* _info ) const
    {
        const ITF_VECTOR<PlayerIDInfo*>& playerIDInfo = m_configTemplate->getPlayersIDInfo();
        u32 numIds = playerIDInfo.size();
        u32 maxPlayers = getMaxPlayerCount();

        for ( u32 i = 0; i < maxPlayers && i < numIds; i++ )
        {
            const PlayerIDInfo* idInfo = playerIDInfo[i];

            if ( idInfo == _info )
            {
                return bfalse;
            }
        }

        return btrue;
    }

    const PlayerIDInfo* GameManager::getPlayerIDInfo( const StringID& _id ) const
    {
        const ITF_VECTOR<PlayerIDInfo*>& playerIDInfo = m_configTemplate->getPlayersIDInfo();
        u32 numIds = playerIDInfo.size();

        for ( u32 i = 0; i < numIds; i++ )
        {
            const PlayerIDInfo* idInfo = playerIDInfo[i];

            if ( StringID(idInfo->getId().cStr()) == _id )
            {
                return idInfo;
            }
        }

        return NULL;
    }

    void GameManager::addGamePlayInputListener(IInputListener* _listener)
    {
        ITF_ASSERT(m_inputManager!=NULL);
        if(m_inputManager) m_inputManager->AddListener(_listener, ZInputManager::Cat_GamePlay);
    }
    void GameManager::addMenuInputListener(IInputListener* _listener)
    {
        ITF_ASSERT(m_inputManager!=NULL);
        if(m_inputManager) m_inputManager->AddListener(_listener, ZInputManager::Cat_Menu);
    }

    void GameManager::addCheatInputListener(IInputListener* _listener)
    {
        ITF_ASSERT(m_inputManager!=NULL);
        if(m_inputManager) m_inputManager->AddListener(_listener, ZInputManager::Cat_Cheat);
    }

    void GameManager::setInputModeForMenu(bbool _active)
    {
        if(m_inputManager)
        {
            if(_active) m_inputModeMenuCounter++;
            else        m_inputModeMenuCounter--;

            if( (m_inputModeMenuCounter==0 && !_active) || (m_inputModeMenuCounter==1 && _active) )
                m_inputManager->setActionMapsActive(ZInputManager::Cat_Menu, _active);
        }
    }

    void GameManager::setInputModeForGamePlay(bbool _active)
    {
        if(m_inputManager)
        {
            if(_active) m_inputModeGamePlayCounter++;
            else        m_inputModeGamePlayCounter--;

            if( (m_inputModeGamePlayCounter==0 && !_active) || (m_inputModeGamePlayCounter==1 && _active) )
                m_inputManager->setActionMapsActive(ZInputManager::Cat_GamePlay, _active);
        }
    }

    void GameManager::setInputModeNoFiltrer()
    {
        if(m_inputManager)
        {
            m_inputManager->setActionMapsActive(ZInputManager::Cat_Menu, btrue);
            m_inputManager->setActionMapsActive(ZInputManager::Cat_GamePlay, btrue);
        }
    }

    void GameManager::initInputManager()
    {
        if(m_inputManager!=NULL)
            return;

        m_inputManager = newAlloc(mId_Gameplay, ZInputManager);
        u32 inputFileCount = m_configTemplate->getInputConfigFilePath().size();
        for (u32 i = 0; i < inputFileCount; i++)
        {
#ifndef ITF_FINAL
                LOG("Input file loaded: \"%s\"", m_configTemplate->getInputConfigFilePath()[i].getString8().cStr());
#endif
                m_inputManager->load_configFile(m_configTemplate->getInputConfigFilePath()[i]);
            }
        }

    void GameManager::updateInputManager()
    {
        if(m_inputManager) m_inputManager->update();
    }

    void GameManager::removeInputListener( IInputListener* _listener )
    {
        ITF_ASSERT(m_inputManager!=NULL);
        if(m_inputManager) m_inputManager->RemoveListener(_listener);
    }

    void GameManager::displaySaveNotification( bbool _visible, bbool _force )
    {
        // Check first if we have to show the notification
        if(!TRC_ADAPTER->isSaveNotificationEnable())
            return;

        ITF_ASSERT_MSG(m_saveNotificationActor!=ITF_INVALID_OBJREF, "Save notification ref is NULL");

        if(m_saveNotificationActor==ITF_INVALID_OBJREF)
            return;

        Actor* pActor = static_cast<Actor*>(GETOBJECT(m_saveNotificationActor));
        if(pActor)
        {
            SaveNotificationComponent* pSaveNotification = pActor->GetComponent<SaveNotificationComponent>();
            if(pSaveNotification)
            {
                if(_visible && ( pSaveNotification->canShowNotification() || _force ) )
                    pSaveNotification->show();
                else if(!_visible && ( pSaveNotification->canShowNotification() || _force ) )
                    pSaveNotification->hide();
            }

        }
    }

    void GameManager::setMainIndexPlayer( u32 _index )
    {
        m_mainIndexPlayer = _index;

        if ( _index != U32_INVALID )
        {
            Player* myPlayer = getPlayer(_index);
            Player* otherPlayer = getPlayer(0);
            u32 myIndex = myPlayer->getOriginalPlayerIDInfoIndex();
            u32 otherIndex = otherPlayer->getOriginalPlayerIDInfoIndex();

            swapPlayer(_index,0);

            otherPlayer->setOriginalPlayerIDInfoIndex(myIndex);
            myPlayer->setOriginalPlayerIDInfoIndex(otherIndex);
        }
    }

    void GameManager::updateRichPresenceForActivePlayers( u32 _richPresenceIndex, bbool _forceAllPlayers )
    {
#ifdef ITF_SUPPORT_NETWORKSERVICES
        if (!NETWORKSERVICES || !CONFIG->m_enableNetwork)
        {
            LOG("[NetworkServices] Not init or disabled!");
            return;
        }

        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            if (m_players[i]->getIndex() == getMainIndexPlayer_Internal()
                || m_players[i]->getActiveAndPersistent() || _forceAllPlayers)
            {
                if (m_lastRichPresenceIndex != _richPresenceIndex)
                    NETWORKSERVICES->setPresenceInfo_State(_richPresenceIndex, m_players[i]->getIndex());
            }
            else
            {
                NETWORKSERVICES->setPresenceInfo_State(m_inactiveRichPresenceIndex, m_players[i]->getIndex());
            }
        }

        //LOG("PresenceIndex set for all players: %d", _richPresenceIndex);
        m_lastRichPresenceIndex = _richPresenceIndex;
#endif // ITF_SUPPORT_NETWORKSERVICES
    }

    void GameManager::updateRichPresenceForPlayer(u32 _richPresenceIndex, u32 _playerIndex)
    {
#ifdef ITF_SUPPORT_NETWORKSERVICES
        if(NETWORKSERVICES)
            NETWORKSERVICES->setPresenceInfo_State(_richPresenceIndex, _playerIndex);
        //LOG("PresenceIndex set for player %d: %d", _playerIndex, _richPresenceIndex);
#endif // ITF_SUPPORT_NETWORKSERVICES
    }

    ITF::bbool GameManager::isLoadingVisible()
    {
        return LOADINGSCREEN->isVisible();
    }

    ITF::bbool GameManager::isSaveNotificationDisplayed() const
    {
        // Check first if we have to show the notification
        if(!TRC_ADAPTER->isSaveNotificationEnable())
            return bfalse;

        ITF_ASSERT_MSG(m_saveNotificationActor!=ITF_INVALID_OBJREF, "Save notification ref is NULL");
        if(m_saveNotificationActor==ITF_INVALID_OBJREF)
            return bfalse;

        Actor* pActor = static_cast<Actor*>(GETOBJECT(m_saveNotificationActor));
        if(pActor)
        {
            SaveNotificationComponent* pSaveNotification = pActor->GetComponent<SaveNotificationComponent>();
            if(pSaveNotification)
            {
                return pSaveNotification->isDisplayed();
            }
        }
        return bfalse;
    }

} //namespace ITF
