#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CODECIMAGEADAPTER_H_
#include "engine/AdaptersInterfaces/CodecImageAdapter.h"
#endif //_ITF_CODECIMAGEADAPTER_H_
#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_
#ifndef _ITF_UDPADAPTER_H_
#include "engine/AdaptersInterfaces/UDPAdapter.h"
#endif //_ITF_UDPADAPTER_H_
#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_
#ifndef _ITF_LOGADAPTER_H_
#include "engine/AdaptersInterfaces/LOGAdapter.h"
#endif //_ITF_LOGADAPTER_H_
#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

#ifndef _ITF_RAKIINTERFACE_H_
#include "engine/sound/raki/RakiInterface.h"
#endif //_ITF_RAKIINTERFACE_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_
#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_
#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_
#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_
#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_
#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_
#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#if !defined(ITF_CTR)
#ifndef _ITF_NETPACKETHANDLER_H_
#include "engine/network/NETPacketHandler.h"
#endif //_ITF_NETPACKETHANDLER_H_
#endif

#if !defined(ITF_CTR) && !defined(ITF_WII)
#ifndef _ITF_TCPADAPTER_H_
#include "engine/AdaptersInterfaces/TCPAdapter.h"
#endif //_ITF_TCPADAPTER_H_
#ifndef _ITF_SCADAPTER_H_
#include "engine/AdaptersInterfaces/SCAdapter.h"
#endif //_ITF_SCADAPTER_H_
#endif


#ifndef _ITF_GUIADAPTER_H_
#include "engine/AdaptersInterfaces/GUIAdapter.h"
#endif //_ITF_GUIADAPTER_H_
#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_
#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_
#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_
#ifndef _ITF_FILEWATCHER_H_
#include "engine/file/FileWatcher.h"
#endif //_ITF_FILEWATCHER_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "engine/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "engine/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_ITF_THREADMANAGER_H_
#include "adapters/ITF_ThreadManager/ITF_ThreadManager.h"
#endif //_ITF_ITF_THREADMANAGER_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_GAME_EDITORINTERFACE_H_
#include "editor/GameEditorInterface.h"
#endif //_ITF_GAME_EDITORINTERFACE_H_

#ifndef _ITF_SINGLETONCONFIGLISTENER_H_
#include "engine/gameplay/SingletonConfigListener.h"
#endif //_ITF_SINGLETONCONFIGLISTENER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/ActorsManager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_WAYPOINTSMANAGER_H_
#include "gameplay/managers/WaypointsManager.h"
#endif //_ITF_WAYPOINTSMANAGER_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_TESTMANAGER_H_
#include "engine/test/testmanager.h"
#endif //_ITF_TESTMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_AUTOSAVEMANAGER_H_
#include "editor/AutoSaveManager.h"
#endif //_ITF_AUTOSAVEMANAGER_H_

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseobjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_BUNDLEMANAGER_H_
#include "engine/file/BundleManager.h"
#endif //_ITF_BUNDLEMANAGER_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_MENUMANAGER_H_
#include "engine/actors/managers/MenuManager.h"
#endif //_ITF_MENUMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_CONTEXTICONSMANAGER_H_
#include "engine/actors/managers/ContextIconsManager.h"
#endif //_ITF_CONTEXTICONSMANAGER_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_UPLAYSERVICE_
#include "engine/networkservices/UPlayService.h"
#endif //_ITF_UPLAYSERVICE_

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_H

#ifndef _ITF_ONLINEADAPTER_H_
#include "engine/AdaptersInterfaces/OnlineAdapter/OnlineAdapter.h"
#endif //_ITF_ONLINEADAPTER_H_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_FRIEZESTATS_H_
#include "engine/display/Frieze/FriezeStats.h"
#endif //_ITF_FRIEZESTATS_H_

#ifndef _ITF_ANIMATIONSTATS_H_
#include "engine/animation/AnimationStats.h"
#endif //_ITF_ANIMATIONSTATS_H_

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif // _ITF_ADAPTER_SAVEGAME_H__

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif //_ITF_SOURCECONTROLADAPTER_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif //_ITF_REWARDADAPTER_H_

#ifndef ITF_FINAL

#ifndef _ITF_COMMANDHANDLER_H_
#include "engine/debug/CommandHandler.h"
#endif //_ITF_COMMANDHANDLER_H_

#ifndef _ITF_PROFILERMANAGER_H_
#include "engine/profiler/ProfilerManager.h"
#endif //_ITF_PROFILERMANAGER_H_

#endif

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_FRIEZEPLUGINREGISTRY_H_
#include "engine/display/Frieze/FriezePluginRegistry.h"
#endif //_ITF_FRIEZEPLUGINREGISTRY_H_


#ifndef _ITF_UVATLAS_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_UVATLAS_H_

#ifdef ITF_SUPPORT_LOGICDATABASE
#ifndef _ITF_FILECACHEMANAGER_H_
#include "core/AdaptersInterfaces/FileCacheManager.h"
#endif //_ITF_FILECACHEMANAGER_H_
#endif //ITF_SUPPORT_LOGICDATABASE

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

namespace ITF
{

Singletons Singletons::m_this;

Singletons::Singletons()
{
    m_bInit             =   bfalse;
    m_CodecImageAdapter = nullptr;
    m_GFXAdapter        = nullptr;
    m_UDPAdapter        = nullptr;
    m_LOGAdapter        = nullptr;
    m_InputAdapter      = nullptr;

#ifdef ITF_SUPPORT_NET
    m_NETPacketHandler  = nullptr;
    m_TCPAdapter        = nullptr;
    m_SCAdapter         = nullptr;
#endif

    m_GUIAdapter        = nullptr;
    m_SystemAdapter     = nullptr;
#ifdef ITF_SUPPORT_RAKI
    m_SoundAdapter      = nullptr;
#endif
    m_FileManager       = nullptr;

#ifdef ITF_SUPPORT_LUA
    m_LUAHandler        = nullptr;
    m_LUAHandlerLoading = nullptr;
#endif //ITF_SUPPORT_LUA

    m_elapsedTime       =   0.f;
    m_loopFPS           =   60.f;
    m_logicFPS          =   60.f;
    m_loopDT            =   (1.f/60.f);
    m_logicDT           =   (1.f/60.f);

    m_currentFrame      =   0;

#ifndef ITF_CONSOLE_FINAL
    m_PluginGateWay     = nullptr;
#endif // ITF_CONSOLE_FINAL
    
    m_ResourceManager   = nullptr;
    m_Editor            = nullptr;

    m_GameInterface     = nullptr;
#ifdef ITF_SUPPORT_EDITOR
    m_GameEditorInterface = nullptr;

    m_SingletonConfigListener = nullptr;
#endif // ITF_SUPPORT_EDITOR

    m_ActorsManager     = nullptr;
    m_TemplateDatabase  = nullptr;

    setAppLoopFPS(60.f);

#ifdef ITF_SUPPORT_NETWORKSERVICES
    m_NetworkServices   = nullptr;
#endif // ITF_SUPPORT_NETWORKSERVICES
#ifdef ITF_UPLAYPC
    m_UPlayService      = nullptr;
#endif // ITF_UPLAYPC

#ifdef ITF_SUPPORT_ONLINETRACKING
	m_OnlineTrackingAdapter	= nullptr;
#endif // ITF_SUPPORT_ONLINETRACKING

#ifdef ITF_SUPPORT_UBISERVICES
    m_OnlineAdapter = nullptr;
#endif // ITF_SUPPORT_UBISERVICES

    m_Adapter_Savegame   = nullptr;
    m_MusicManager      = nullptr;

#ifdef ITF_SUPPORT_LOGICDATABASE
    m_LogicDatabase     = nullptr;
    m_FileCacheManager  = nullptr;
#endif //ITF_SUPPORT_LOGICDATABASE
#ifdef ITF_SUPPORT_RAKI
    m_RakiInterface     = nullptr;
#endif

#ifdef  SUPPORT_STATS_MANAGER
    m_StatsManager			= nullptr;
#endif //SUPPORT_STATS_MANAGER

#ifndef ITF_FINAL
	m_SourceControlAdapter	= nullptr;
    m_commandHandler        = nullptr;
    m_profilerManager       = nullptr;
#endif  //ITF_FINAL

	m_TRCManagerAdapter= nullptr;
    m_RewardAdapter= nullptr;
    m_RewardManager= nullptr;
    m_FeedbackFXManager = nullptr;

    m_LocalisationManager = nullptr;
}

void Singletons::init()
{
    Singletons::m_this.initInternal();
}

void Singletons::initInternal()
{
    if (m_bInit)
        return;

    m_bInit = btrue;

#ifndef ITF_FINAL
    m_ErrorHandler      =   newAlloc(mId_Singleton,ErrorHandler);
#endif //ITF_FINAL

    m_ThreadManager     =   newAlloc(mId_Singleton,ITF_ThreadManager);

    m_idServer              =   newAlloc(mId_Singleton, IdServer);
    m_objectIdServer        =   newAlloc(mId_Singleton, ObjectIdServer);

#if defined(ITF_SUPPORT_NET)
    m_NETPacketHandler      =   newAlloc(mId_Singleton, NETPacketHandler);
#endif

    m_EventManager          =   newAlloc(mId_Singleton, EventManager);
    m_PhysWorld             =   newAlloc(mId_Phys, PhysWorld);
    View* pMainView         =   View::createView("main", btrue, bfalse);
    pMainView->createCamera();
    pMainView->setMainView();
    

    m_BaseObjectFactory     =   newAlloc(mId_Singleton, BaseObjectFactory);

#ifdef ITF_WINDOWS
    m_FileWatcher           =   newAlloc(mId_Singleton, FileWatcher);
#else
    m_FileWatcher           =   NULL;
#endif //ITF_WINDOWS

    m_SceneManager          =   newAlloc(mId_Singleton, SceneManager);

    m_Config                =   newAlloc(mId_Singleton, XMLConfig);

    m_FileServer            =   newAlloc(mId_Singleton, FileServer);

    m_AnimManager           =   newAlloc(mId_Singleton, AnimManager);
#ifdef ITF_SUPPORT_RAKI
    m_RakiInterface         =   newAlloc(mId_Singleton, RakiInterface);
#endif
    m_SoundManager = newAlloc(mId_Singleton, SoundManager);

    m_LoadingScreen         =   newAlloc(mId_Singleton, LoadingScreen);

    m_WaypointsManager      = newAlloc(mId_Singleton, WaypointsManager);
    m_RegionsManager        = newAlloc(mId_Singleton, RegionsManager);
#ifdef ITF_SUPPORT_EDITOR
    m_SingletonConfigListener = newAlloc(mId_Singleton, SingletonConfigListener);
#endif // ITF_SUPPORT_EDITOR
    m_ActorsManager         = newAlloc(mId_Singleton, ActorsManager);
    m_TemplateDatabase      = newAlloc(mId_Singleton, TemplateDatabase);
    m_StimsManager          = newAlloc(mId_Singleton, StimsManager);
    //m_CheatManager          = newAlloc(mId_Singleton, CheatManager);
    //m_MusicManager          = newAlloc(mId_Singleton, MusicManager);
#ifndef ITF_FINAL
    m_TestManager           = newAlloc(mId_Singleton, TestManager);
#endif // ITF_FINAL
#ifdef ITF_SUPPORT_EDITOR
    m_AutoSaveManager       = newAlloc(mId_Singleton, AutoSaveManager);
#endif // ITF_SUPPORT_EDITOR
    m_BundleManager         = newAlloc(mId_Singleton, BundleManager);
    m_UIMenuManager         = newAlloc(mId_Singleton, UIMenuManager);
	m_MenuManager			= newAlloc(mId_Singleton, MenuManager);
	m_UITextManager         = newAlloc(mId_Singleton, UITextManager);
    m_ContextIconsManager   = newAlloc(mId_Singleton, ContextIconsManager);

#ifdef ITF_SUPPORT_LOGICDATABASE
	m_LogicDatabase         = newAlloc(mId_Singleton, LogicDataBase);
    m_FileCacheManager      = newAlloc(mId_Singleton,FileCacheManager);
#endif //ITF_SUPPORT_LOGICDATABASE


#ifdef ITF_SUPPORT_LUA
    setLUAHandler (newAlloc(mId_Singleton,LUAHandler));
    m_LUAHandlerLoading    = newAlloc(mId_Singleton, LUAHandler);
#endif //ITF_SUPPORT_LUA

    m_WorldManager          = newAlloc(mId_Singleton, WorldManager);

#ifdef ITF_SUPPORT_DEBUGFEATURE
    m_DebugInfo             = newAlloc(mId_Singleton, DebugInfo);
#endif // ITF_SUPPORT_DEBUGFEATURE

#if defined(RASTER_ENABLED)
    m_RasterManager         = newAlloc(mId_Singleton, RastersManager);
#endif //RASTER_ENABLED

#ifdef SUPPORT_STATS_MANAGER
	m_StatsManager			= newAlloc(mId_Singleton, StatsManager);
#endif //SUPPORT_STATS_MANAGER

#ifndef ITF_FINAL
    m_commandHandler        = newAlloc(mId_Singleton, CommandHandler);
    m_profilerManager       = newAlloc(mId_Singleton, ProfilerManager);
#endif //ITF_FINAL

    m_RewardManager         = newAlloc(mId_Singleton, RewardManager);
    //CameraControllerManager::create();    // --> created in ApplicationFramework::createDefaultManagers()

    m_AliasManager          = newAlloc(mId_Singleton, AliasManager);
    m_AtlasManager          = newAlloc(mId_Singleton, UVAtlasManager);
    
    m_LocalisationManager   = newAlloc(mId_Singleton, LocalisationManager);

#ifdef ITF_SUPPORT_UPLAY
    m_UPlayService          = newAlloc(mId_Singleton, UPlayService);
#endif

    FRIEZESTAT_INIT;
    ANIMATIONSTAT_INIT;
}

void Singletons::setGameInterface( GameInterface* _interface )
{
    m_GameInterface = _interface;
}

#ifdef ITF_SUPPORT_EDITOR
void Singletons::setGameEditorInterface( GameEditorInterface* _interface )
{
    m_GameEditorInterface = _interface;
}
#endif // ITF_SUPPORT_EDITOR

Singletons::~Singletons()
{
#ifdef ITF_SUPPORT_LOGICDATABASE
    ITF_ASSERT(!m_LogicDatabase);
#endif

#ifdef ITF_SUPPORT_ONLINETRACKING
	ITF_ASSERT(!m_OnlineTrackingAdapter);
#endif // ITF_SUPPORT_ONLINETRACKING

#ifdef ITF_SUPPORT_NETWORKSERVICES
    ITF_ASSERT(!m_NetworkServices);
#endif // ITF_SUPPORT_NETWORKSERVICES

#ifdef ITF_UPLAYPC
    ITF_ASSERT(!m_UPlayService);
#endif // ITF_UPLAYPC

#ifdef ITF_SUPPORT_UBISERVICES
    ITF_ASSERT(!m_OnlineAdapter);
#endif // ITF_SUPPORT_UBISERVICES

    ITF_ASSERT(!m_BundleManager);

#ifdef ITF_SUPPORT_EDITOR
    ITF_ASSERT(!m_AutoSaveManager);
    ITF_ASSERT(!m_UndoRedoManager);
#endif // ITF_SUPPORT_EDITOR

    ITF_ASSERT(!m_FileWatcher);
    ITF_ASSERT(!m_idServer);
    ITF_ASSERT(!m_objectIdServer);
    ITF_ASSERT(!m_BaseObjectFactory);
    ITF_ASSERT(!m_CodecImageAdapter);
    ITF_ASSERT(!m_GUIAdapter);
    ITF_ASSERT(!m_FileServer);

#ifdef ITF_SUPPORT_LUA
    ITF_ASSERT(!m_LUAHandler);
    ITF_ASSERT(!m_LUAHandlerLoading);
#endif

    ITF_ASSERT(!m_GFXAdapter);
    ITF_ASSERT(!m_LOGAdapter);
    ITF_ASSERT(!m_UDPAdapter);

#ifdef ITF_SUPPORT_NET
    ITF_ASSERT(!m_TCPAdapter);
    ITF_ASSERT(!m_SCAdapter);
    ITF_ASSERT(!m_NETPacketHandler);
#endif

    ITF_ASSERT(!m_InputAdapter);
    ITF_ASSERT(!m_ErrorHandler);
    ITF_ASSERT(!m_ResourceManager);
    ITF_ASSERT(!m_Config);
    ITF_ASSERT(!m_SystemAdapter);
#ifdef ITF_SUPPORT_RAKI
    ITF_ASSERT(!m_SoundAdapter);
#else
    ITF_ASSERT(!m_Adapter_AudioMiddleware);
#endif    
    ITF_ASSERT(!m_SceneManager);
    ITF_ASSERT(!m_PhysWorld);
    ITF_ASSERT(!m_Editor);
    ITF_ASSERT(!m_FileManager);
    ITF_ASSERT(!m_LoadingScreen);
    ITF_ASSERT(!m_ThreadManager);
    ITF_ASSERT(!m_TestManager);
    ITF_ASSERT(!m_GameInterface);
#ifdef ITF_SUPPORT_EDITOR
    ITF_ASSERT(!m_GameEditorInterface);
    ITF_ASSERT(!m_SingletonConfigListener);
#endif // ITF_SUPPORT_EDITOR
    ITF_ASSERT(!m_ActorsManager);
    ITF_ASSERT(!m_TemplateDatabase);
    ITF_ASSERT(!m_StimsManager);
    ITF_ASSERT(!m_WaypointsManager);
    ITF_ASSERT(!m_RegionsManager);
    ITF_ASSERT(!m_CheatManager);
    ITF_ASSERT(!m_MusicManager);
#ifdef ITF_SUPPORT_RAKI
    ITF_ASSERT(!m_RakiInterface);
#endif
    ITF_ASSERT(!m_UIMenuManager);
    ITF_ASSERT(!m_UITextManager);
    ITF_ASSERT(!m_ContextIconsManager);
	ITF_ASSERT(!m_MenuManager);
    ITF_ASSERT(!m_WorldManager);
    ITF_ASSERT(!m_Adapter_Savegame);
#ifdef ITF_SUPPORT_DEBUGFEATURE
    ITF_ASSERT(!m_DebugInfo);
#endif // ITF_SUPPORT_DEBUGFEATURE
	ITF_ASSERT(!m_TRCManagerAdapter);
    ITF_ASSERT(!m_RewardAdapter);
    ITF_ASSERT(!m_RewardManager);
    ITF_ASSERT(!m_commandHandler);
    ITF_ASSERT(!m_profilerManager);
    ITF_ASSERT(!m_LocalisationManager);
}

void Singletons::preDestroy()
{

}

void Singletons::destroy(bbool _fastExit)
{
    if (!m_bInit)
        return;

    m_bInit             =   bfalse;

    // we unload maps. We need to do that early enough, as it may use low level functions.
    m_GameInterface->unloadGameResources();


#ifdef ITF_SUPPORT_LOGICDATABASE
    CSerializerObjectLogicDB::clear();
#endif //ITF_SUPPORT_LOGICDATABASE

#ifdef ITF_WINDOWS
#ifndef ITF_FINAL
    if (ITF::PLUGINGATEWAY)
        ITF::PLUGINGATEWAY->onCloseApp();
#endif
#endif


#ifdef ITF_WINDOWS
    SF_DEL(m_FileWatcher);
#endif //ITF_WINDOWS
#ifdef ITF_SUPPORT_ONLINETRACKING
	SF_DEL(m_OnlineTrackingAdapter);
#endif // ITF_SUPPORT_ONLINETRACKING
#ifdef ITF_SUPPORT_UBISERVICES
    SF_DEL(m_OnlineAdapter);
#endif // ITF_SUPPORT_UBISERVICES
#ifdef ITF_SUPPORT_NETWORKSERVICES
    SF_DEL(m_NetworkServices);
#endif // ITF_SUPPORT_NETWORKSERVICES
#ifdef ITF_UPLAYPC
    SF_DEL(m_UPlayService);
#endif // ITF_UPLAYPC
#ifdef ITF_SUPPORT_EDITOR
    SF_DEL(m_AutoSaveManager);
#endif // ITF_SUPPORT_EDITOR
    SF_DEL(m_BaseObjectFactory);

    SF_DEL(m_GUIAdapter);
    SF_DEL(m_LOGAdapter);
    SF_DEL(m_UDPAdapter);
#ifdef ITF_SUPPORT_NET
    SF_DEL(m_TCPAdapter);
    SF_DEL(m_SCAdapter);
#endif
    SF_DEL(m_InputAdapter);
    SF_DEL(m_CodecImageAdapter);

#ifdef ITF_SUPPORT_NET
    SF_DEL(m_NETPacketHandler);
#endif

    SF_DEL(m_Config);
    // SF_DEL(m_Editor);
    SF_DEL(m_MusicManager);
    SF_DEL(m_MetronomeManager);

#ifndef ITF_FINAL
    SF_DEL(m_TestManager);
#endif // ITF_FINAL
    SF_DEL(m_CheatManager);
    SF_DEL(m_GameInterface);
#ifdef ITF_SUPPORT_EDITOR
    SF_DEL(m_GameEditorInterface);
    SF_DEL(m_SingletonConfigListener);
#endif // ITF_SUPPORT_EDITOR
  
    // Deletion of spawner must happen BEFORE template deletion
    // spawned object may use it's template data before dying
    Spawner::terminate();

    SF_DEL(m_ActorsManager);
    SF_DEL(m_RewardAdapter);
    SF_DEL(m_RewardManager);
    SF_DEL(m_SoundManager);

    AnimDependenciesMapResource::terminate();

#ifndef ITF_SUPPORT_RAKI
    if (m_Adapter_AudioMiddleware)
        m_Adapter_AudioMiddleware->destroy();
#endif
    //SF_DEL(m_TemplateDatabase);
    SF_DEL(m_RegionsManager);
    SF_DEL(m_WaypointsManager);
    SF_DEL(m_StimsManager);
    SF_DEL(m_AnimManager);
    SF_DEL(m_UIMenuManager);
    SF_DEL(m_UITextManager);
    SF_DEL(m_ContextIconsManager);
	SF_DEL(m_MenuManager);
    SF_DEL(m_LoadingScreen);
    SF_DEL(m_PhysWorld);
    CameraControllerManager::destroy();
    SF_DEL(m_EventManager);
    SF_DEL(m_FileServer);

/*    static SceneManager * pSceneManagerDebug = SCENE_MANAGER;
    pSceneManagerDebug = pSceneManagerDebug;

    static Scene * pScene = pSceneManagerDebug->getLoadedSceneAt(0);
    pScene = pScene;*/
    // We need to skip some destroy to avoid a waste of time 
    // (during debug time and quit time)
    if(_fastExit)
    {
        m_WorldManager = NULL;
        m_SceneManager = NULL;
    }
    else
    {
        #ifdef ITF_WINDOWS
        if(SCENE_MANAGER)
        {
            const ObjectRefList& scenes = SCENE_MANAGER->getLoadedScenes();
            String8 arg("not properly deleted scenes:\n");
            PathString8_t pathName;
            for (u32 isc = 0; isc < scenes.size(); isc++)
            {
                const Scene* pScene = static_cast<const Scene*>(scenes[isc].getObject());
                if (pScene)
                {
                    pScene->getPath().getString(pathName);
                    arg += pathName;
                    arg += "\n";
                }
            }
            ITF_ASSERT_CRASH(SCENE_MANAGER->getLoadedSceneCount() == 0, arg.cStr());
        }
        #endif

        SF_DEL(m_WorldManager);
        SF_DEL(m_SceneManager);
    }

	SF_DEL(m_FeedbackFXManager);

    SF_DEL(m_TemplateDatabase);

    View::deleteAll();  // must be after all objects destroy

    //Must be deleted in last
	SF_DEL(m_Adapter_Savegame);

    ITF_ParticleGenerator::freePool();

    if (m_GFXAdapter)
        m_GFXAdapter->destroyResources();
      
    // ResourceManager can be waiting for a sleeping resource thread, do not try to delete it 
    // --> random crash during loading
    if(_fastExit)
    {
        m_ResourceManager = NULL;
    }
    else
    {
	    SF_DEL(m_ResourceManager);
    }

    if (m_GFXAdapter)
        m_GFXAdapter->destroy();

#ifdef SUPPORT_STATS_MANAGER
	SF_DEL(m_StatsManager);
#endif //SUPPORT_STATS_MANAGER

#ifndef ITF_FINAL
	SF_DEL(m_SourceControlAdapter);
    SF_DEL(m_commandHandler);
    SF_DEL(m_profilerManager);
#endif //ITF_FINAL

    SF_DEL(m_GFXAdapter);    // Always delete GFX adapter AFTER ResourceManager
#ifdef ITF_SUPPORT_RAKI
    SF_DEL(m_SoundAdapter);  // Always delete Sound adapter AFTER ResourceManager
    SF_DEL(m_RakiInterface);
#else

    SF_DEL(m_Adapter_AudioMiddleware);

#endif 
    SF_DEL(m_FileManager);

#ifdef  ITF_SUPPORT_LOGICDATABASE
    SF_DEL(m_LogicDatabase);
    SF_DEL(m_FileCacheManager);
#endif //ITF_SUPPORT_LOGICDATABASE


#ifdef ITF_SUPPORT_LUA
    SF_DEL(m_LUAHandlerLoading);
    SF_DEL(m_LUAHandler); // Always delete Lua adapter AFTER ResourceManager
#endif //ITF_SUPPORT_LUA

    SF_DEL(m_idServer);
    SF_DEL(m_objectIdServer);
    SF_DEL(m_BundleManager);
    SF_DEL(m_SystemAdapter);
#ifdef ITF_SUPPORT_DEBUGFEATURE
    SF_DEL(m_DebugInfo);
#endif // ITF_SUPPORT_DEBUGFEATURE

#if defined(RASTER_ENABLED)
    SF_DEL(m_RasterManager);
#endif //(RASTER_ENABLED)
    
	//Int last
	SF_DEL(m_TRCManagerAdapter);
    SF_DEL(m_ThreadManager);
    SF_DEL(m_AliasManager);
    SF_DEL(m_AtlasManager);

    SF_DEL(m_LocalisationManager);

    FRIEZE_PLUGIN_REGISTRY->terminate();

#ifndef ITF_FINAL
    SF_DEL(m_ErrorHandler); // Must be destroyed at the end
#endif //ITF_FINAL

#ifdef ITF_DEBUG_STRINGID
    StringID::dbDestroy();
#endif

    FRIEZESTAT_STOP;
    ANIMATIONSTAT_STOP;

    String::deleteThreadsData();
    String8::deleteThreadsData();
}

void Singletons::setAppLoopFPS( f32 _framesPerSecond )
{
    Singletons& pSing = get();

    pSing.m_loopFPS = _framesPerSecond;
    pSing.m_loopDT = 1.f / _framesPerSecond;
}

void Singletons::setLogicFPS( f32 _framesPerSecond )
{
    Singletons& pSing = get();

    pSing.m_logicFPS = _framesPerSecond;
    pSing.m_logicDT = 1.f / _framesPerSecond;
}

void Singletons::setPhysWorld(PhysWorld* _world)
{ 
    if (m_PhysWorld)
    {
        delete m_PhysWorld;
    }
    m_PhysWorld = _world;
}

} // namespace ITF

