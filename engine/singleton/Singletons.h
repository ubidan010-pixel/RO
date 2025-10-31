#ifndef _ITF_SINGLETONS_H_
#define _ITF_SINGLETONS_H_

namespace ITF
{
// object ref
class IdServer;
class ObjectIdServer;

// Adapters
class GFXAdapter;
class UDPAdapter;
class LOGAdapter;
class InputAdapter;
class TCPAdapter;
class SCAdapter;
class GUIAdapter;
class CodecImageAdapter;
class SystemAdapter;
class SoundAdapter;
class Adapter_AudioMiddleware; 
class FileManager;
class ThreadManager;
class SoundManager;
class EventManager;
class TRCManagerAdapter;
class OnlineTrackingAdapter;
class RewardAdapter;
class RewardManager;

// Handlers
class ErrorHandler;
class LUAHandler;
class NETPacketHandler;
class ResourceManager;
class TransactionHandler;
class FileServer;
class FileWatcher;
class PluginGateWay;
class SceneManager;
class AnimManager;
class LoadingScreen;

// Data Classes
class   PhysWorld;
class   Editor;
class   Camera;
struct  XMLConfig;
class   CommandHandler;
class   ProfilerManager;

class   GameInterface;
class   GameEditorInterface;
class   SingletonConfigListener;
class   ActorsManager;
class   TemplateDatabase;
class   StimsManager;
class   WaypointsManager;
class   RegionsManager;
class   CheatManager;
class   MusicManager;
class   RakiInterface;
class   TestManager;
class   UndoRedoManager;
class   AutoSaveManager;
class   BaseObjectFactory;
class   BundleManager;
class	FeedbackFXManager;
class	MenuManager;
class   UIMenuManager;
class   UITextManager;
class   ContextIconsManager;
class   NetworkServices;
class   UPlayService;
class   LogicDataBase;
class   WorldManager;
class   Adapter_Savegame;
class   DebugInfo;
class   RastersManager;
class	SourceControlAdapter;
class	StatsManager;
class   MetronomeManager;
class   AliasManager;
class   UVAtlasManager;
class   FileCacheManager;
class   LocalisationManager;

class Singletons
{
private:
    static  Singletons              m_this;
            // object ref
            IdServer*               m_idServer;
            // object id
            ObjectIdServer*         m_objectIdServer;

            // Adapters
            CodecImageAdapter*      m_CodecImageAdapter;
            GFXAdapter*             m_GFXAdapter;
            UDPAdapter*             m_UDPAdapter;
            LOGAdapter*             m_LOGAdapter;
            InputAdapter*           m_InputAdapter;

#ifdef ITF_SUPPORT_NET
            TCPAdapter*             m_TCPAdapter;
            SCAdapter*              m_SCAdapter;
#endif
            GUIAdapter*             m_GUIAdapter;
            SystemAdapter*          m_SystemAdapter;
#ifdef ITF_SUPPORT_RAKI
            SoundAdapter*            m_SoundAdapter;
#else
            Adapter_AudioMiddleware* m_Adapter_AudioMiddleware;
#endif
            FileManager*            m_FileManager;
            ThreadManager*          m_ThreadManager;
            Adapter_Savegame*        m_Adapter_Savegame;
			TRCManagerAdapter*		m_TRCManagerAdapter;
#ifdef ITF_SUPPORT_ONLINETRACKING
			OnlineTrackingAdapter*	m_OnlineTrackingAdapter;
#endif // ITF_SUPPORT_ONLINETRACKING
            RewardAdapter*          m_RewardAdapter;
            RewardManager*          m_RewardManager;

#ifdef ITF_SUPPORT_LUA
            // Handlers
            LUAHandler*             m_LUAHandler;
            LUAHandler*             m_LUAHandlerLoading;
#endif //ITF_SUPPORT_LUA


            ErrorHandler*           m_ErrorHandler;
#ifdef ITF_SUPPORT_NET
            NETPacketHandler*       m_NETPacketHandler;
#endif
            ResourceManager*        m_ResourceManager;
            FileWatcher*            m_FileWatcher;
#ifndef ITF_CONSOLE_FINAL
            PluginGateWay*          m_PluginGateWay;
#endif // ITF_CONSOLE_FINAL
            SceneManager*           m_SceneManager;
            AnimManager*            m_AnimManager;
            LoadingScreen*          m_LoadingScreen;
            SoundManager*           m_SoundManager;
            EventManager*           m_EventManager;

            // Classes
            PhysWorld*              m_PhysWorld;
            FileServer*             m_FileServer;
            Editor*                 m_Editor;
            CommandHandler*         m_commandHandler;
            ProfilerManager*        m_profilerManager;

            XMLConfig*              m_Config;

            GameInterface*          m_GameInterface;
#ifdef ITF_SUPPORT_EDITOR
            GameEditorInterface*    m_GameEditorInterface;
            SingletonConfigListener* m_SingletonConfigListener;
#endif // ITF_SUPPORT_EDITOR
            ActorsManager*          m_ActorsManager;
            TemplateDatabase*       m_TemplateDatabase;
            StimsManager*           m_StimsManager;
            WaypointsManager *      m_WaypointsManager;
            RegionsManager *        m_RegionsManager;
            CheatManager*           m_CheatManager;
            MusicManager*           m_MusicManager;
            MetronomeManager*       m_MetronomeManager;
            RakiInterface*          m_RakiInterface;
#ifndef ITF_FINAL
            TestManager*            m_TestManager;
#endif // ITF_FINAL
#ifdef ITF_SUPPORT_EDITOR
            UndoRedoManager*        m_UndoRedoManager;
            AutoSaveManager*        m_AutoSaveManager;
#endif // ITF_SUPPORT_EDITOR
            BaseObjectFactory*      m_BaseObjectFactory;
			FeedbackFXManager*		m_FeedbackFXManager;
            MenuManager*			m_MenuManager;
			UIMenuManager*          m_UIMenuManager;
			UITextManager*          m_UITextManager;
            ContextIconsManager*    m_ContextIconsManager;
            BundleManager*          m_BundleManager;
#ifdef ITF_SUPPORT_NETWORKSERVICES
            NetworkServices*        m_NetworkServices;
#endif // ITF_SUPPORT_NETWORKSERVICES
#ifdef ITF_UPLAYPC
            UPlayService*           m_UPlayService;
#endif // ITF_UPLAYPC

#ifdef ITF_SUPPORT_LOGICDATABASE
            LogicDataBase*          m_LogicDatabase;
            FileCacheManager*       m_FileCacheManager;
#endif //ITF_SUPPORT_LOGICDATABASE
            
            WorldManager*           m_WorldManager;
#ifdef ITF_SUPPORT_DEBUGFEATURE
            DebugInfo*              m_DebugInfo;
#endif // ITF_SUPPORT_DEBUGFEATURE

#if defined(RASTER_ENABLED)
            RastersManager*         m_RasterManager;
#endif //(RASTER_ENABLED)
			SourceControlAdapter*	m_SourceControlAdapter;
            AliasManager*           m_AliasManager;
            UVAtlasManager*         m_AtlasManager;
#ifdef  SUPPORT_STATS_MANAGER
			StatsManager*			m_StatsManager;
#endif //SUPPORT_STATS_MANAGER

            LocalisationManager*        m_LocalisationManager;

                                    Singletons      (   );
                                    ~Singletons     (   );

                    void initInternal();

public:

                    f32             m_elapsedTime;
                    f32             m_loopFPS;                  // Frames per second of the application loop
                    f32             m_logicFPS;                 // Frames per second used by the logic of the game
                    f32             m_loopDT;                   // Frame time of the application loop
                    f32             m_logicDT;                  // Frame time of the game logic

                    bbool           m_bInit;
                    u32             m_currentFrame;
                    Time            m_currentTimeInFrame; //Store the current time from a start frame

    static              void                    setAppLoopFPS( f32 _framesPerSecond );
    static              void                    setLogicFPS( f32 _framesPerSecond );

    bbool                                       isInit              ()                              { return m_bInit;}
    static    ITF_INLINE   Singletons&          get                 (   )   { return m_this; }
    static               void                   init();
                         void                   destroy(bbool _fastExit = bfalse);
                         void                   preDestroy();

                         void                   setGameInterface    ( GameInterface* _interface );
#ifdef ITF_SUPPORT_EDITOR
                         void                   setGameEditorInterface ( GameEditorInterface* _interface );
#endif // ITF_SUPPORT_EDITOR

            // object ref
            ITF_INLINE   void                   setIdServer         (   IdServer* _server       )   { m_idServer = _server; }
            ITF_INLINE   IdServer*              getIdServer         (                           )   { return m_idServer; }
            // object id
            ITF_INLINE   void                   setObjectIdServer   (   ObjectIdServer* _server )   { m_objectIdServer = _server; }
            ITF_INLINE   ObjectIdServer*        getObjectIdServer   (                           )   { return m_objectIdServer; }

            // Adapters
            ITF_INLINE   void                   setCodecImageAdapter(   CodecImageAdapter* _adapter    )   { m_CodecImageAdapter = _adapter; }
            ITF_INLINE   CodecImageAdapter*     getCodecImageAdapter(                           )   { return m_CodecImageAdapter; }
            ITF_INLINE   void                   setGFXAdapter       (   GFXAdapter* _adapter    )   { m_GFXAdapter = _adapter; }
            ITF_INLINE   GFXAdapter*            getGFXAdapter       (                           )   { return m_GFXAdapter; }
            ITF_INLINE   void                   setUDPAdapter       (   UDPAdapter* _adapter    )   { m_UDPAdapter = _adapter; }
            ITF_INLINE   UDPAdapter*            getUDPAdapter       (                           )   { return m_UDPAdapter; }
            ITF_INLINE   void                   setLOGAdapter       (   LOGAdapter* _adapter    )   { m_LOGAdapter = _adapter; }
            ITF_INLINE   LOGAdapter*            getLOGAdapter       (                           )   { return m_LOGAdapter; }
            ITF_INLINE   void                   setInputAdapter     (   InputAdapter* _adapter  )   { m_InputAdapter = _adapter; }
            ITF_INLINE   InputAdapter*          getInputAdapter     (                           )   { return m_InputAdapter; }
#ifndef ITF_SUPPORT_NET
            ITF_INLINE   TCPAdapter*            getTCPAdapter       (                           )   { return NULL; }
#else
            ITF_INLINE   void                   setTCPAdapter       (   TCPAdapter* _adapter    )   { m_TCPAdapter = _adapter; }
            ITF_INLINE   TCPAdapter*            getTCPAdapter       (                           )   { return m_TCPAdapter; }
            ITF_INLINE   void                   setSCAdapter        (   SCAdapter* _adapter     )   { m_SCAdapter = _adapter; }
            ITF_INLINE   SCAdapter*             getSCAdapter        (                           )   { return m_SCAdapter; }
#endif
            ITF_INLINE   void                   setGUIAdapter       (   GUIAdapter* _adapter    )   { m_GUIAdapter = _adapter; }
            ITF_INLINE   GUIAdapter*            getGUIAdapter       (                           )   { return m_GUIAdapter; }
            ITF_INLINE   void                   setSystemAdapter    (   SystemAdapter* _adapter )   { m_SystemAdapter = _adapter; }
            ITF_INLINE   SystemAdapter*         getSystemAdapter    (                           )   {return m_SystemAdapter; }
#ifdef ITF_SUPPORT_RAKI
            ITF_INLINE   void                   setSoundAdapter(SoundAdapter* _adapter) { m_SoundAdapter = _adapter; }
            ITF_INLINE   SoundAdapter* getSoundAdapter() { return m_SoundAdapter; }
#else
            ITF_INLINE   void                   setAdapter_AudioMiddleware(Adapter_AudioMiddleware* _adapter) { m_Adapter_AudioMiddleware = _adapter; }
            ITF_INLINE   Adapter_AudioMiddleware* getAdapter_AudioMiddleware() { return m_Adapter_AudioMiddleware; }
#endif // ITF_SUPPORT_RAKI           

            ITF_INLINE   void                   setFileManager      (   FileManager* _adapter  )    { m_FileManager = _adapter; }
            ITF_INLINE   FileManager*           getFileManager     (                           )    {return m_FileManager; }
            ITF_INLINE   void                   setThreadManager     (   ThreadManager* _adapter  )   {m_ThreadManager = _adapter; }
            ITF_INLINE   ThreadManager*         getThreadManager     (                           )    {return m_ThreadManager; }
            ITF_INLINE   void                   setBaseObjectFactory(BaseObjectFactory* _baseObjectFactory) {m_BaseObjectFactory = _baseObjectFactory;}
            ITF_INLINE   BaseObjectFactory*     getBaseObjectFactory()                                   {return m_BaseObjectFactory;}
#ifdef ITF_SUPPORT_NETWORKSERVICES
            ITF_INLINE   void                   setNetworkServices( NetworkServices* _NetworkServices)                      { m_NetworkServices = _NetworkServices;}
            ITF_INLINE   NetworkServices*       getNetworkServices(                          )      { return m_NetworkServices;}
#endif // ITF_SUPPORT_NETWORKSERVICES
#ifdef ITF_UPLAYPC
            ITF_INLINE   void                   setUPlayService( UPlayService* _UplayService) { m_UPlayService = _UplayService; }
            ITF_INLINE   UPlayService*          getUPlayService() { return m_UPlayService; }
#endif // ITF_UPLAYPC
            ITF_INLINE   void                   setAdapter_Savegame  (   Adapter_Savegame* _adapter  ){ m_Adapter_Savegame = _adapter; }
            ITF_INLINE   Adapter_Savegame*       getAdapter_Savegame  (                           )   {return m_Adapter_Savegame; }

			ITF_INLINE   void                   setTRCManagerAdapter (   TRCManagerAdapter* _adapter  ){ m_TRCManagerAdapter = _adapter; }
			ITF_INLINE   TRCManagerAdapter*     getTRCManagerAdapter (                           )   {return m_TRCManagerAdapter; }
#ifdef ITF_SUPPORT_ONLINETRACKING
			ITF_INLINE   void                   setOnlineTrackingAdapter( OnlineTrackingAdapter* _OnlineTrackingAdapter)                      { m_OnlineTrackingAdapter = _OnlineTrackingAdapter;}
			ITF_INLINE   OnlineTrackingAdapter*	getOnlineTrackingAdapter(                          )      { return m_OnlineTrackingAdapter;}
#endif // ITF_SUPPORT_ONLINETRACKING
            ITF_INLINE   void                   setRewardAdapter (   RewardAdapter* _adapter  ){ m_RewardAdapter = _adapter; }
            ITF_INLINE   RewardAdapter*         getRewardAdapter (                           )   {return m_RewardAdapter; }
            ITF_INLINE   void                   setRewardManager (   RewardManager* _adapter  ){ m_RewardManager = _adapter; }
            ITF_INLINE   RewardManager*         getRewardManager (                           )   {return m_RewardManager; }

#ifdef ITF_SUPPORT_LUA
            // Handlers
            ITF_INLINE   void                   setLUAHandler           (   LUAHandler* _handler        )   { m_LUAHandler = _handler; }
            ITF_INLINE   LUAHandler*            getLUAHandler           (    )   {return m_LUAHandler; }
            ITF_INLINE   LUAHandler*            getLUAHandlerLoading    (    )   {return m_LUAHandlerLoading; }
#endif //ITF_SUPPORT_LUA


            ITF_INLINE   void                   setErrorHandler         (   ErrorHandler* _handler      )   { m_ErrorHandler = _handler; }
            ITF_INLINE   ErrorHandler*          getErrorHandler         (    )   {return m_ErrorHandler; }
#ifdef ITF_SUPPORT_NET
            ITF_INLINE   void                   setNETPacketHandler     (   NETPacketHandler* _handler )    { m_NETPacketHandler = _handler; }
            ITF_INLINE   NETPacketHandler*      getNETPacketHandler     (    )   {return m_NETPacketHandler; }
#endif
            ITF_INLINE   void                   setResourceManager      (   ResourceManager* _mng       )   { m_ResourceManager = _mng; }
            ITF_INLINE   ResourceManager*       getResourceManager      (    )   {return m_ResourceManager; }
            ITF_INLINE   FileWatcher*           getFileWatcher          (    )   {return m_FileWatcher; }
#ifndef ITF_CONSOLE_FINAL
            ITF_INLINE   void                   setPluginGateWay        (   PluginGateWay* _adapter )   { m_PluginGateWay = _adapter; }
            ITF_INLINE   PluginGateWay*         getPluginGateWay        (                           )   {return m_PluginGateWay; }
#endif // ITF_CONSOLE_FINAL
            ITF_INLINE   void                   setSceneManager         (   SceneManager* _sm       )   { m_SceneManager = _sm; }
            ITF_INLINE   SceneManager*          getSceneManager         (                           )   {return m_SceneManager; }
            ITF_INLINE   void                   setAnimManager          (   AnimManager* _am        )   { m_AnimManager = _am; }
            ITF_INLINE   AnimManager*           getAnimManager          (                           )   {return m_AnimManager; }
#ifdef ITF_SUPPORT_EDITOR
            ITF_INLINE   void                   setUndoRedoManager      (  UndoRedoManager* _urm    )   { m_UndoRedoManager = _urm;}
            ITF_INLINE   UndoRedoManager*       getUndoRedoManager      (                           )   {return m_UndoRedoManager;}
            ITF_INLINE   AutoSaveManager*       getAutoSaveManager      (                           )   {return m_AutoSaveManager;}
#endif // ITF_SUPPORT_EDITOR
            ITF_INLINE   void                   setBundleManager        (   BundleManager* _bm      )   { m_BundleManager = _bm; }
            ITF_INLINE   BundleManager*         getBundleManager        (                           )   {return m_BundleManager; }
            ITF_INLINE   void                   setSoundManager      (   SoundManager* _adapter  )  {m_SoundManager = _adapter; }
            ITF_INLINE   SoundManager*          getSoundManager      (                           )  {return m_SoundManager; }
            ITF_INLINE   void                   setEventManager      (   EventManager* _adapter  )  {m_EventManager = _adapter; }
            ITF_INLINE   EventManager*          getEventManager      (                           )  {return m_EventManager; }

#ifndef ITF_FINAL
			ITF_INLINE   void                   setSourceControlAdapter    (  SourceControlAdapter* _sourcecontrol    )   { m_SourceControlAdapter = _sourcecontrol;}
#endif  //ITF_FINAL

            // Classes
                        void                       setPhysWorld        (   PhysWorld* _scene       );
            ITF_INLINE  PhysWorld*                 getPhysWorld        (                           )   { return m_PhysWorld; }
                        void                       setEditor           (   Editor* _ed             )   { m_Editor = _ed;   }
            ITF_INLINE  Editor*                    getEditor           (                           )   { return m_Editor; }
            ITF_INLINE  XMLConfig*                 getConfig           (                           )   { return m_Config; }
            ITF_INLINE  FileServer*                getFileServer       (                           )   { return m_FileServer;}
            
            ITF_INLINE  GameInterface*             getGameInterface    (                           )   { return m_GameInterface; }
#ifdef ITF_SUPPORT_EDITOR
            ITF_INLINE  GameEditorInterface*       getGameEditorInterface    (                      )   { return m_GameEditorInterface; }
            
            ITF_INLINE  SingletonConfigListener*   getSingletonConfigListener (                    )   { return m_SingletonConfigListener; }
#endif // ITF_SUPPORT_EDITOR
            
            ITF_INLINE  ActorsManager*             getActorsManager    (                           )   { return m_ActorsManager; }
            ITF_INLINE  TemplateDatabase*          getTemplateDatabase (                           )   { return m_TemplateDatabase; }
            ITF_INLINE  StimsManager*              getStimsManager     (                           )   { return m_StimsManager; }
            ITF_INLINE  WaypointsManager*          getWaypointsManager (                           )   { return m_WaypointsManager; }
            ITF_INLINE  RegionsManager*            getRegionsManager   (                           )   { return m_RegionsManager; }
            ITF_INLINE  void                       setCheatManager     ( CheatManager* _manager    )   { m_CheatManager = _manager; }
            ITF_INLINE  CheatManager*              getCheatManager     (                           )   { return m_CheatManager; }
            ITF_INLINE  void                       setMusicManager     ( MusicManager* _pMM        )   { m_MusicManager = _pMM; }
            ITF_INLINE  void                       setMetronomeManager ( MetronomeManager* _pMM    )   { m_MetronomeManager = _pMM; }
            ITF_INLINE  MetronomeManager*          getMetronomeManager (                           )   { return m_MetronomeManager; }
            ITF_INLINE  MusicManager*              getMusicManager     (                           )   { return m_MusicManager; }
#ifndef ITF_FINAL
            ITF_INLINE  TestManager*               getTestManager      (                           )   { return m_TestManager; }
#endif // ITF_FINAL
			ITF_INLINE  FeedbackFXManager*         getFeedbackFXManager(                           )   { return m_FeedbackFXManager; }
            ITF_INLINE  void                       setFeedbackFXManager( FeedbackFXManager* _mng   )   { m_FeedbackFXManager = _mng; }
			ITF_INLINE  LoadingScreen*             getLoadingScreen    (                           )   { return m_LoadingScreen;} 
			ITF_INLINE  MenuManager*			   getMenuManager      (                           )   { return m_MenuManager; }
			ITF_INLINE  UIMenuManager*             getUIMenuManager    (                           )   { return m_UIMenuManager; }
            ITF_INLINE  UITextManager*             getUITextManager    (                           )   { return m_UITextManager; }
            ITF_INLINE  ContextIconsManager*       getContextIconsManager(                         )   { return m_ContextIconsManager; }

#ifdef  ITF_SUPPORT_LOGICDATABASE
            ITF_INLINE  void                       setLogicDatabase    ( LogicDataBase* _pLBD      )   { m_LogicDatabase = _pLBD;}
            ITF_INLINE  LogicDataBase*             getLogicDatabase    (                           )   { return m_LogicDatabase;}
#endif //ITF_SUPPORT_LOGICDATABASE

            ITF_INLINE  WorldManager*              getWorldManager(                           )        { return m_WorldManager;}
#ifdef ITF_SUPPORT_LOGICDATABASE
            ITF_INLINE  void                       setFileCacheManager ( FileCacheManager* _pFCM      )   { m_FileCacheManager = _pFCM;}
            ITF_INLINE  FileCacheManager*          getFileCacheManager    (                           )   { return m_FileCacheManager;}
#endif // ITF_SUPPORT_LOGICDATABASE

#ifdef ITF_SUPPORT_DEBUGFEATURE
            ITF_INLINE  DebugInfo*                 getDebugInfo()                                      { return m_DebugInfo;}
#endif // ITF_SUPPORT_DEBUGFEATURE

			
            ITF_INLINE  void                       setCommandHandler   ( CommandHandler* _ch       )   { m_commandHandler = _ch; }
            ITF_INLINE  CommandHandler*            getCommandHandler   (                           )   { return m_commandHandler; }

            ITF_INLINE  void                       setProfilerManager   ( ProfilerManager* _pm     )   { m_profilerManager = _pm; }
            ITF_INLINE  ProfilerManager*           getProfilerManager   (                          )   { return m_profilerManager; }

            ITF_INLINE  LocalisationManager*       getLocalisationManager   (                      )   { return m_LocalisationManager; }
            ITF_INLINE  void                       setLocalisationManager   ( LocalisationManager* ptr )   { m_LocalisationManager = ptr; }

};

#define SINGLETONS          Singletons::get()
#define CODECIMAGE_ADAPTER  SINGLETONS.getCodecImageAdapter()
#define UDP_ADAPTER         SINGLETONS.getUDPAdapter()
#define TCP_ADAPTER         SINGLETONS.getTCPAdapter()
#define SC_ADAPTER          SINGLETONS.getSCAdapter()
#define GUI_ADAPTER         SINGLETONS.getGUIAdapter()
#define SAVEGAME_ADAPTER    SINGLETONS.getAdapter_Savegame()
#define LUA_HANDLER             SINGLETONS.getLUAHandler()
#define LUA_HANDLERLOADING      SINGLETONS.getLUAHandlerLoading()
#define NETPACKET_HANDLER       SINGLETONS.getNETPacketHandler()
#define TRANSACTION_HANDLER     SINGLETONS.getTransactionHandler()
#define FILEWATCHER             SINGLETONS.getFileWatcher()
#define EVENTMANAGER            SINGLETONS.getEventManager()
#define LOADINGSCREEN           SINGLETONS.getLoadingScreen()
#define CONFIG                  SINGLETONS.getConfig()

#define ELAPSEDTIME             SINGLETONS.m_elapsedTime
#define APPLOOPFPS              SINGLETONS.m_loopFPS                // Frames per second of the application loop
#define LOGICFPS                SINGLETONS.m_logicFPS               // Frames per second used by the logic of the game
#define APPLOOPDT               SINGLETONS.m_loopDT                 // Frame time of the application loop
#define LOGICDT                 SINGLETONS.m_logicDT                // Frame time of the game logic

#define CURRENTFRAME            SINGLETONS.m_currentFrame
#define CURRENTTIMEFRAME        SINGLETONS.m_currentTimeInFrame

#define GAMEINTERFACE           SINGLETONS.getGameInterface()
#define GAME_EDITOR_INTERFACE   SINGLETONS.getGameEditorInterface()
#define SINGLETONCONFIGLISTENER SINGLETONS.getSingletonConfigListener()
#define ACTORSMANAGER           SINGLETONS.getActorsManager()
#define TEMPLATEDATABASE        SINGLETONS.getTemplateDatabase()
#define STIMSMANAGER            SINGLETONS.getStimsManager()
#define WAYPOINTS_MANAGER       SINGLETONS.getWaypointsManager()
#define REGIONS_MANAGER         SINGLETONS.getRegionsManager()
#define CHEATMANAGER            SINGLETONS.getCheatManager()
#define TESTMANAGER             SINGLETONS.getTestManager()
#define AUTOSAVE_MANAGER        SINGLETONS.getAutoSaveManager()
#define BASEOBJECT_FACTORY      SINGLETONS.getBaseObjectFactory()
#define BUNDLEMANAGER           SINGLETONS.getBundleManager()
#define FEEDBACKFX_MANAGER      SINGLETONS.getFeedbackFXManager()
#define UI_MENUMANAGER          SINGLETONS.getUIMenuManager()
#define MENUMANAGER				SINGLETONS.getMenuManager()
#define UI_TEXTMANAGER          SINGLETONS.getUITextManager()
#define CONTEXTICONSMANAGER     SINGLETONS.getContextIconsManager()
#define NETWORKSERVICES         SINGLETONS.getNetworkServices()
#define UPLAYSERVICE            SINGLETONS.getUPlayService()
#define ONLINETRACKING_ADAPTER	SINGLETONS.getOnlineTrackingAdapter()
#define COMMANDHANDLER          SINGLETONS.getCommandHandler()
#define CURRENTWORLD            static_cast<World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()))
#define METRONOME_MANAGER       SINGLETONS.getMetronomeManager()
#define PROFILERMANAGER         SINGLETONS.getProfilerManager()
#define LOCALISATIONMANAGER     SINGLETONS.getLocalisationManager()


#define ADDWATCH(_name, _val) ((ITF::GlobalWatch*)(ITF::SINGLETONS.getPluginGateWay()->getPluginByName(ITF::GlobalWatch::getPluginName())))->addWatch(_name, _val);
#define REMOVEWATCH(_name) ((ITF::GlobalWatch*)(ITF::SINGLETONS.getPluginGateWay()->getPluginByName(ITF::GlobalWatch::getPluginName())))->removeWatch(_name);

#define DATA_FOLDER         SINGLETONS.getFileServer()->getDataDepot()

} // namespace ITF



#endif // _ITF_SINGLETONS_H_
