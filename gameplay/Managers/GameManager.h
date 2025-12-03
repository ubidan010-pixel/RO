#ifndef _ITF_GAMEMANAGER_H_
#define _ITF_GAMEMANAGER_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#include "engine/scene/worldEventInterface.h"
#endif //_ITF_WORLD_EVENT_INTERFACE_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_


#include "engine/AdaptersInterfaces/SoundAdapter.h"

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_ADAPTER_SAVEGAME_H__
#include "engine/AdaptersInterfaces/Adapter_Savegame.h"
#endif

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef _ITF_AFTERFXHANDLER_H_
#include "engine/gameplay/AfterFXHandler.h"
#endif // _ITF_AFTERFXHANDLER_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

#ifndef _ITF_GAMEMODECONTROLLER_H_
#include "gameplay/managers/GameModeController.h"
#endif //_ITF_GAMEMODECONTROLLER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

#define SAVEGAME_BASENAME "Savegame"


namespace ITF
{
#define GAMEMANAGER GameManager::get()
#define GAMEMANAGER_VALID (GameManager::get() && GameManager::get()->initDone())

    enum ControllerIconSlot
    {
        IconSlot_Default = 0,
        IconSlot_Wii,
        IconSlot_PS3,
        IconSlot_PS5,
        IconSlot_Vita,
        IconSlot_CTR,
        IconSlot_Switch,
        IconSlot_Ounce,
        IconSlot_XboxSeries,
        IconSlot_X360,
        IconSlot_Keyboard,
        IconSlot_Count
    };

    class GameScreenBase;
    class Event;
    class ActorComponent;
    class EventPlayerModeChanged;
    class IEventListener;
    class PersistentGameData_Universe;
    class ZInputManager;
    class IInputListener;

    typedef std::pair<ObjectRef,ObjectRef> WorldCheckpoint;
    typedef ITF_VECTOR<WorldCheckpoint> WorldCheckpointList;



    class PlayerIDInfo
    {
        DECLARE_SERIALIZE()

    public:

        class ActorInfo
        {
            DECLARE_SERIALIZE()

        public:
            ActorInfo() : m_isAlwaysActive(bfalse), m_isPlayable(bfalse) {}

            ITF_INLINE const Path&  getFile()           const { return m_file; }
            ITF_INLINE bbool        isAlwaysActive()    const {return m_isAlwaysActive;}
            ITF_INLINE bbool        isPlayable()        const {return m_isPlayable;}
            u32                     getNumGameModes()   const { return m_gameModes.size(); }
            GameMode                getGameMode( u32 _index ) const { return m_gameModes[_index]; }
            const SafeArray <GameMode>& getGameModes() const { return m_gameModes; }

        private:
            Path                    m_file;
            bbool                   m_isAlwaysActive;
            bbool                   m_isPlayable;
            SafeArray <GameMode>    m_gameModes;
        };

        class GameScreenInfo
        {
            DECLARE_SERIALIZE()

        public:

            const StringID&         getGameScreen() const { return m_gameScreen; }
            u32                     getNumActors() const { return m_actors.size(); }
            const ActorInfo&        getActorInfo( u32 _index ) const { return m_actors[_index]; }

        private:

            StringID                m_gameScreen;
            ITF_VECTOR <ActorInfo>  m_actors;
        };

        PlayerIDInfo();
        ~PlayerIDInfo() {}

        const String8&              getId() const { return m_id; }
        const String8&              getFamily() const { return m_family; }
        ITF_INLINE const Color &    getDeathBubbleColor() const { return m_deathBubbleColor; }
        u32                         getNumGameScreens() const { return m_gameScreens.size(); }
        const GameScreenInfo&       getGameScreenInfoAt( u32 _index ) const { return m_gameScreens[_index]; }
        const GameScreenInfo*       getGameScreenInfo( const StringID& _id ) const;

    private:

        String8                     m_id;
        String8                     m_family;
        Color                       m_deathBubbleColor;
        ITF_VECTOR <GameScreenInfo> m_gameScreens;
    };

    class Player
    {
    public:

        struct RegisteredActor
        {
            RegisteredActor() : m_alwaysActive( bfalse ), m_isPlayable( bfalse ) {}

            SafeArray <GameMode>    m_gameModes;
            ActorRef                m_actor;
            bbool                   m_alwaysActive;
            bbool                   m_isPlayable;
        };

        typedef ITF_VECTOR <RegisteredActor> RegisteredActorList;

        Player();
        virtual ~Player();

        // index is pad index
        ITF_INLINE u32        getIndex() const { return m_index; }
        ITF_INLINE void       setIndex(u32 _index) { m_index = _index; }

        // an ID that identifies the player regardless of its actor's name
        ITF_INLINE StringID         getID() const { return m_id; }
        ITF_INLINE void             setID( const char* _val) { m_id = _val; }

        ITF_INLINE bbool      getActive() const { return m_active; }
        ITF_INLINE void       setActive(bbool _val);

        // if the player should be re-activated in the world map or on a map init
        ITF_INLINE bbool      getActiveAndPersistent() const { return m_activeAndPersistent; }
        ITF_INLINE void       setActiveAndPersistent(bbool _val) { m_activeAndPersistent = _val; }

        // the order in which the player joined the game
        ITF_INLINE u32        getActiveIndex() const { return m_activeIndex; }
        ITF_INLINE void       setActiveIndex(u32 _index) { m_activeIndex = _index; }


        // the various actors associated to the game modes
        bbool      isRegisteredActor(ObjectRef _actorRef) const;
        GameMode	  getFirstAvailableGameMode();
        ObjectRef               getActorForMode(GameMode _mode, bbool _getPlayableActor = btrue );
        GameMode                getModeFromActor( ObjectRef _actorRef );
        void				    broadcastEventToModeActors( Event * _event, GameMode _exceptmode = GAMEMODE_UNKNOWN );

        // the current game mode (eg platformer or shooter)
        ITF_INLINE GameMode   getCurrentMode() const { return m_currentMode; }

        // the current actor for the current game mode
                   ActorRef   getCurrentActor() const;
                   void       setCurrentActor( ObjectRef _actorRef );
        ITF_INLINE Actor*     getActor() const { return getCurrentActor().getActor(); }

        ITF_INLINE const StringID&  getCurrentPlayerIDInfo() const { return m_currentPlayerIDInfo; }
        void                        setCurrentPlayerIDInfo( const PlayerIDInfo* _id );

        ITF_INLINE InputAdapter::PressStatus * getButtons() { return m_buts; }

        ITF_INLINE const RegisteredActorList& getRegisteredActors() const { return m_registeredActors; }
        void setRegisteredActors( const RegisteredActorList& _list, bbool _resetActors = bfalse );
        ITF_INLINE void clearRegisteredActors() { m_registeredActors.clear(); resetCurrentRegisteredActors(); }

        ITF_INLINE bbool getHasJoinedBefore() const { return m_hasJoinedBefore; }
        ITF_INLINE void setHasJoinedBefore( const bbool _value ) { m_hasJoinedBefore = _value; }

        virtual void       reset(){}  // called when the player joins the game
        virtual void       toggleDeadMode(){}
        virtual bbool      isDead() const { return bfalse; }

        void                setOriginalPlayerIDInfoIndex( u32 _index ) { m_originalPlayerIDInfoIndex = _index; }
        u32                 getOriginalPlayerIDInfoIndex() const { return m_originalPlayerIDInfoIndex; }

        ITF_INLINE void setIsDisconnected(bbool _value) { m_isDisconnected = _value; }
        ITF_INLINE bbool getIsDisconnected() { return m_isDisconnected; }

        ITF_INLINE const TemplateClientHandler& getTemplateClientHandler() const { return m_templateClient; }

    protected:
        TemplateClientHandler m_templateClient;
        u32 m_index;
        u32 m_originalPlayerIDInfoIndex;
        StringID m_id;
        StringID m_currentPlayerIDInfo;
        bbool m_active;
        bbool m_activeAndPersistent;
        u32 m_activeIndex;
        bbool m_hasJoinedBefore;
        bbool m_isDisconnected;// player has to be disactived ASAP

        RegisteredActorList m_registeredActors;
        GameMode m_currentMode;
        u32 m_currentActorIndex;

        InputAdapter::PressStatus m_buts[JOY_MAX_BUT];

	/////////// RESERVED for game manager //////////////
	private :
		friend class GameManager;
        friend class GameModeController;
		void        changeGameMode( GameMode _mode );
		void        registerActor( ObjectRef _actorRef, GameMode _mode, bbool _alwaysActive, bbool _isPlayable );
        void        unregisterActor( ObjectRef _actorRef );
        void        resetCurrentRegisteredActors();

        ////////////////////////////////////////////////////
        class View* m_cellsView;
        void updateCellsView();

    };


    class GameManagerConfig_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(GameManagerConfig_Template, TemplateObj,2069144052);
        DECLARE_SERIALIZE()

    public:
        GameManagerConfig_Template()
            : Super()
            , m_cutSceneDefaultUnskippableDurationFirstTime(2.f)
            , m_tempThreshold(2.0f)
            , m_useShakeForAttack(bfalse)
            , m_tempDelay(0.2f)
            , m_tempswimMaxSpeed(1.0f)
            , m_tempswimSmooth(0.90f)
            , m_temprunUseB(bfalse)
            , m_temprunUseShake(bfalse)
            , m_temprunTimerStop(0.0f)
            , m_usePressConfMenu(bfalse)
        {
            for (u32 i = 0; i < IconSlot_Count; ++i)
            {
                m_controllerIconPaths[i] = Path::EmptyPath;
            }
            m_gpeIconsPath = Path::EmptyPath;
            m_gpeExtraIconsPath = Path::EmptyPath;
            m_skipIconsPath = Path::EmptyPath;
            m_menuLogosPath = Path::EmptyPath;
        }
        GameManagerConfig_Template(const Path& _path)
            : Super(_path)
            , m_cutSceneDefaultUnskippableDurationFirstTime(2.f)
            , m_tempThreshold(2.0f)
            , m_useShakeForAttack(bfalse)
            , m_tempDelay(0.2f)
            , m_tempswimMaxSpeed(1.0f)
            , m_tempswimSmooth(0.90f)
            , m_temprunUseB(bfalse)
            , m_temprunUseShake(bfalse)
            , m_temprunTimerStop(0.0f)
            , m_usePressConfMenu(bfalse)
        {
            for (u32 i = 0; i < IconSlot_Count; ++i)
            {
                m_controllerIconPaths[i] = Path::EmptyPath;
            }
            m_gpeIconsPath = Path::EmptyPath;
            m_gpeExtraIconsPath = Path::EmptyPath;
            m_skipIconsPath = Path::EmptyPath;
            m_menuLogosPath = Path::EmptyPath;
        }
        ~GameManagerConfig_Template();
        ITF_INLINE const ITF_VECTOR<String> &getMaps() const { return m_maps; }
        ITF_INLINE const ITF_VECTOR<String> &getPressConfMaps() const { return m_pressConfMaps; }
        ITF_INLINE const ITF_VECTOR<String> &getMenuPath() const { return m_menuPath; }
        ITF_INLINE const ITF_VECTOR<Path> &getLuaIncludes() const { return m_luaIncludes; }
        ITF_INLINE const ITF::Path &getBaseMapPath() const { return m_baseMapPath; }
        ITF_INLINE const ITF::Path &getGame2DScenePath() const { return m_game2DPath; }
        ITF_INLINE const ITF::String &getGameTextFilePath() const { return m_gameTextFilePath; }
        ITF_INLINE const ITF::Path &getMainMenuBackMap() const { return m_mainMenuBackMap; }
        ITF_INLINE const ITF::Path &getMainMenuBackMapForDebugSaves() const { return m_mainMenuBackMapForDebugSaves; }
        ITF_INLINE const ITF::Path &getWorldMap() const { return m_worldMap; }
        ITF_INLINE const ITF::Path &getSplash1Map() const { return m_splash1Map; }
        ITF_INLINE const ITF::Path &getEndLevelMap() const { return m_levelEndedMap; }
        ITF_INLINE const Path & getMenuSoundMap() const { return m_menuSoundMap; }
        ITF_INLINE const Path& getLoadingScreenPath() const { return m_loadingScreenAct; }
        ITF_INLINE const ITF_VECTOR<PlayerIDInfo*> &getPlayersIDInfo() const { return m_playersIDInfo; }
        ITF_INLINE const ITF_VECTOR<String8> &getFamilyList() const { return m_familyList; }
        ITF_INLINE const ITF::Path &getCameraShakeConfig() const { return m_cameraShakeConfig; }
        ITF_INLINE const ITF_MAP<StringID,u32>& getMusicThemes() const { return m_MusicThemesMap; }
        ITF_INLINE const String& getMusicPath(u32 _index) const { return _index < m_musicThemes.size() ? m_musicThemes[_index].m_path : String::emptyString; }
        ITF_INLINE const f32 getCutSceneDefaultUnskippableDurationFirstTime() const {return m_cutSceneDefaultUnskippableDurationFirstTime;}
        ITF_INLINE const ITF_VECTOR<Path> &getInputConfigFilePath() const { return m_inputConfigFiles; }
        ITF_INLINE const bbool getUsePressConfMenu() const { return m_usePressConfMenu; }

        ITF_INLINE const bbool getUseSakeForAttackTEMP() const { return m_useShakeForAttack; }
        ITF_INLINE const f32 getThresholdTEMP() const { return m_tempThreshold; }
        ITF_INLINE const f32 getDelayTEMP() const { return m_tempDelay; }
        ITF_INLINE const f32 getSwimMaxSpeedTEMP() const { return m_tempswimMaxSpeed; }
        ITF_INLINE const f32 getSwimSmoothTEMP() const { return m_tempswimSmooth; }
        ITF_INLINE const bbool getRunUseBTEMP() const { return m_temprunUseB; }
        ITF_INLINE const bbool getRunUseShakeTEMP() const { return m_temprunUseShake; }
        ITF_INLINE const f32 getRunTimerStopTEMP() const { return m_temprunTimerStop; }

        ITF_INLINE const ITF::Path& getControllerIconPath(ControllerIconSlot _slot) const
        {
            ITF_ASSERT(_slot >= 0 && _slot < IconSlot_Count);
            return m_controllerIconPaths[_slot];
        }
        ITF_INLINE const ITF::Path& getIconsBtnPath() const { return getControllerIconPath(IconSlot_Default); }
        ITF_INLINE const ITF::Path& getIconsBtnPathWii() const { return getControllerIconPath(IconSlot_Wii); }
        ITF_INLINE const ITF::Path& getIconsBtnPathPS3() const { return getControllerIconPath(IconSlot_PS3); }
        ITF_INLINE const ITF::Path& getIconsBtnPathPS5() const { return getControllerIconPath(IconSlot_PS5); }
        ITF_INLINE const ITF::Path& getIconsBtnPathVita() const { return getControllerIconPath(IconSlot_Vita); }
        ITF_INLINE const ITF::Path& getIconsBtnPathCTR() const { return getControllerIconPath(IconSlot_CTR); }
        ITF_INLINE const ITF::Path& getIconsBtnPathSwitch() const { return getControllerIconPath(IconSlot_Switch); }
        ITF_INLINE const ITF::Path& getIconsBtnPathOunce() const { return getControllerIconPath(IconSlot_Ounce); }
        ITF_INLINE const ITF::Path& getIconsBtnPathXboxSeries() const { return getControllerIconPath(IconSlot_XboxSeries); }
        ITF_INLINE const ITF::Path& getIconsBtnPathX360() const { return getControllerIconPath(IconSlot_X360); }
        ITF_INLINE const ITF::Path& getIconsBtnPathKeyboard() const { return getControllerIconPath(IconSlot_Keyboard); }
        ITF_INLINE const ITF::Path& getIconsGpePath() const { return m_gpeIconsPath; }
        ITF_INLINE const ITF::Path& getIconsGpeExtraPath() const { return m_gpeExtraIconsPath; }
        ITF_INLINE const ITF::Path& getIconsSkipPath() const { return m_skipIconsPath; }
        ITF_INLINE const ITF::Path& getMenuLogosPath() const { return m_menuLogosPath; }

    private:

        void fillMusicMap();

        struct MusicTheme
        {
            MusicTheme()
                : m_theme(StringID::Invalid)
                , m_path(String::emptyString)
            {}

            DECLARE_SERIALIZE()
            StringID    m_theme;
            String      m_path;
        };

        ITF_VECTOR<String>          m_maps;
        ITF_VECTOR<String>          m_menuPath;
        ITF_VECTOR<Path>            m_luaIncludes;
        ITF_VECTOR<Path>            m_inputConfigFiles;
        ITF_VECTOR<MusicTheme>      m_musicThemes;
        ITF_MAP<StringID,u32>       m_MusicThemesMap;
        Path                        m_baseMapPath;
        Path                        m_game2DPath;
        String                      m_gameTextFilePath;
        Path                        m_mainMenuBackMap;
        Path                        m_mainMenuBackMapForDebugSaves;
        Path                        m_worldMap;
        Path                        m_splash1Map;
        Path                        m_levelEndedMap;
        Path                        m_menuSoundMap;
        Path                        m_loadingScreenAct;
        ITF_VECTOR<PlayerIDInfo*>	m_playersIDInfo;
        ITF_VECTOR<String8>	        m_familyList;
        Path                        m_cameraShakeConfig;
        f32                         m_cutSceneDefaultUnskippableDurationFirstTime;
        bbool                       m_usePressConfMenu;
        ITF_VECTOR<String>          m_pressConfMaps;
        Path                        m_controllerIconPaths[IconSlot_Count];
        Path                        m_gpeIconsPath;
        Path                        m_gpeExtraIconsPath;
        Path                        m_skipIconsPath;
        Path                        m_menuLogosPath;

        // TEMP values  -- Will be removed soon --
        f32                         m_tempThreshold;
        f32                         m_tempDelay;
        bbool                       m_useShakeForAttack;
        f32                         m_tempswimMaxSpeed;
        f32                         m_tempswimSmooth;
        bbool                       m_temprunUseB;
        bbool                       m_temprunUseShake;
        f32                         m_temprunTimerStop;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Game manager : game-level behavior
    class GameManager : public Interface_WorldEventsListener, public Interface_SceneEventsListener, public IEventListener
    {
        DECLARE_SERIALIZE_VIRTUAL()

    public:

        enum LevelNameInitializationType
        {
            LEVEL_NAME_INITIALISATION_TYPE_SET,
            LEVEL_NAME_INITIALISATION_TYPE_KEEP,
            LEVEL_NAME_INITIALISATION_TYPE_EMPTY,
        };

        typedef void (*startLoadingProfileCB)();
        typedef void (*stopLoadingProfileCB)();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Construction
        GameManager();
        virtual ~GameManager(){}
        virtual void destroyInternal();

        static void                 create()    { s_instance = NULL; }
        static void                 destroy()   { s_instance->destroyInternal(); SF_DEL(s_instance); }
        static GameManager*         get()       { return s_instance; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get Game Manager Type (eg "Ray_GameManager"...)
        const String &getGameManagerType() const {return m_gameManagerType;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        virtual void init();

        void initPlayers();
        virtual Player* allocPlayer() { return newAlloc(mId_Gameplay, Player); }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// unload game resources (maps, objects...)
        virtual void unloadGameResources();

        ///////////////////////////////////////////////////////////////////////////////////////////
        virtual bbool isConfigurationLoaded() const {return m_theGameScreen!=NULL;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        virtual void onEndOfPlayingVideo() { if(m_theGameScreen) m_theGameScreen->onEndOfPlayingVideo(); }
        virtual void onEndOfFadeIn() { if(m_theGameScreen) m_theGameScreen->onEndOfFadeIn(); }
        virtual void onStartOfPlayingVideo() { if(m_theGameScreen) m_theGameScreen->onStartOfPlayingVideo(); }
        virtual void onPauseOfPlayingVideo() { if(m_theGameScreen) m_theGameScreen->onPauseOfPlayingVideo(); }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// update
        /// @param _dt
        virtual void update(f32 _dt);
        virtual void draw();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// post a game screen change (template version). Will be achieved at next manageCurrentGameScreen, in next update()
        /// @param _GameScreen : required state change (give the class name)
        /// @param _evenIfOtherQueued : should be bfalse. ONLY set to btrue IF PRIORITARY, like TRC state change
        /// @param _data : give a pointer to parameter, or NULL. Will be copied, so copy constructors must be valid.
        /// @return btrue if ok, bfalse if a change was already queued. NB : YOU MUST CHECK THIS RETURN VALUE
        template<class _GameScreen> bbool postGameScreenChange(bbool _evenIfOtherQueued, typename _GameScreen::InitializationParameters* _data = NULL )
        {return postGameScreenChange_String(_GameScreen::GetClassCRCStatic(), _evenIfOtherQueued, _data);}
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// post a game screen change (string version). Will be achieved at next manageCurrentGameScreen, in next update()
        /// @param _screen : required state change (give the class name)
        /// @param _evenIfOtherQueued : should be bfalse. ONLY set to btrue IF PRIORITARY, like TRC state change
        /// @param _data : give a pointer to parameter, or NULL. Will be automatically deleted.
        /// @return btrue if ok, bfalse if a change was already queued. NB : YOU MUST CHECK THIS RETURN VALUE
        virtual bbool postGameScreenChange_String(const StringID &_screen, bbool _evenIfOtherQueued, GameScreenBase::InitializationParameters* _data = NULL );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Is current screen intended for gameplay (includes rayman,globox...)
        bbool isPlayableScreen();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get path of base map. Set by init()
        const Path& getBaseMapPath();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get music themes map
        const ITF_MAP<StringID,u32>&    getMusicThemes();
        virtual void                    setMusicTheme( const StringID& _theme );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Level name
        virtual void    setCurrentLevelName(const Path& _map){};
        const StringID& getCurrentLevelName() const { return m_currentLevelName; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get music path
        const String& getMusicPath(u32 _index);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// enable/disable BaseScene. Must be already loaded.
        /// @param _enable
        void enableBaseScene(bbool _enable);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// may load the base map asynchronously, if not already done
        void mayLoadBaseMap();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// unload base may, if already loaded
        void mayUnloadBaseMap();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// free the current world (if any)
        void freeCurrentWorld();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// load game menus
        void loadMenus();
        void loadMenuSound();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// unload game menus
        void unloadMenus();

        ITF_INLINE bbool isMenusLoaded() const { return m_areMenuLoaded && m_menuSoundLoaded; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Called when the system whants to empty the level game state. Eg when we hit F5
        virtual void emptyCurrentLevelGameState();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get Savegame Max Size. Virtual.
        virtual u32 getSavegameMaxSize() {return 65536;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// clear gamestate. Call when you create a new game.
        virtual void clearWholeGameState() {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// load game state
        /// @param _data pointer to data to load from
        /// @param _dataSize size of data in bytes
        virtual bbool loadGameState(const u8 * /*_data*/, u32 /*_dataSize*/) {return bfalse;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// save GameState
        /// @param _newGame btrue if for New Game. If brings confirmations in case we overwrite exiting game.
        virtual bbool saveGameState(bbool _newGame, u32 _slotIndex=0, bbool _resetSave = bfalse){return bfalse;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// load a gameplay map, and switch gamescreen
        /// @param _map : The map path
        /// @param _unloadBaseMap : set to btrue if you need to unload existing base map before loading new map
        virtual bbool loadGameplayMap(const Path &/*_map*/, bbool _unloadBaseMap, bbool _isReload, ObjectRef _worldRef = ObjectRef::InvalidRef) {ITF_ASSERT_MSG(bfalse, "Not implemented for this game");return bfalse;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// create a map, the ref can be used to load the map in a second step
        ObjectRef  createMap( );

        void create2DWorld();

        virtual void            createMainMenuWorld();
        ITF_INLINE ObjectRef    getMainMenuWorld() { return m_mainMenuWorld; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// load a map. If a gameplay map, you should load and enable the base map too.
        /// @param _bAsynchronous : asynchronous loading
        /// @param _disconnectFromCells : don't use cell map? (set to btrue for anything not depending on camera)
        /// @param _unloadBaseMap : unload base map if loaded
        bbool      loadMap( ObjectRef _rWorld, const Path& _map, bbool _asynchronous, bbool _disconnectCells, bbool _unloadBaseMap, LevelNameInitializationType _levelInitType );


        ///////////////////////////////////////////////////////////////////////////////////////////
        // Asynchronously load a map without switching to it.
        // @param _map : map path to load
        ObjectRef   preLoadMap( const Path& _map, Vector<ObjectPath> & _prefetch);

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Asynchronously load a map without switching to it.
        // @param _worldRef : world to switch
        // @param _autoWaitAndSwitch : if the world is not ready then wait for it and switch later
        bbool       switchToPreloadedMap(ObjectRef _worldRef, bbool _autoWaitAndSwitch = bfalse);

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Check if async load is over
        // @param _worldRef : world to check
        bbool       isPreloadedMapLoaded(ObjectRef _worldRef);

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Ask to delete a preloaded world. If it is currently loading, then it will be put inside a pending list,
        // and deleted when loaded is complete.
        // @param _worldRef : world to delete
        void deletePreloadedMap(ObjectRef _worldRef);

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Delete all preloaded worlds. If it is currently loading, then it will be put inside a pending list,
        // and deleted when loaded is complete.
        void deleteAllPreloadedMaps();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// calls createMap() + loadMap()
        ObjectRef  createAndLoadMap( const Path& _map, bbool _asynchronous, bbool _disconnectCells, bbool _unloadBaseMap, LevelNameInitializationType _levelInitType );

        void loadSubMap(const Path& _subMap);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Reload current map. Does the same thing as Editor::reloadCurrentMap, without editor stuff part
        void reloadCurrentMap();

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Change activated status of player
        virtual void changePlayerActivation( u32 _playerIndex, bbool _startLevel );

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///call this when player profile is signed in.out, or when its online status changes
        ///@param _userIndex the index of the user
        ///@param _signedIn btrue if user is signed in, bfalse if signed out
        ///@param _isOnlineChange btrue if its an online status change, bfalse if it's a profile change
        ///@return btrue if must show TRC message
        bbool userSignedInOut(u32 _userIndex, bbool _signedIn, bbool _isOnlineChange);

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Call this methode to trigger end of level
        virtual void    endOfLevelReached( bbool _saveGameState = btrue, bbool _levelCompleted = btrue );

        //LOADING/CHECKPOINTS
        void setInitialMap(const String & _initialMap){ m_initialMap = _initialMap;}
        const String& getInitialMap() const { return m_initialMap; }

        void    setCurrentCheckpoint( const ObjectPath& _path );
        void    setCurrentCheckpoint( Pickable* _pickable );
        void    setCurrentSceneReference( Scene* _scene );
        Scene*  getCurrentSceneReference() const;
        void    setFirstCheckpointPath(const ObjectPath &_path) {m_firstCheckpointPath = _path;}
        Actor*  getFirstCheckpoint(ObjectRef _world) const;
        Actor*  getCheckpointFromPath( const ObjectPath& _path ) const;
        Actor*  getCheckpointFromIndex( ObjectRef _world, u32 _index, bbool _forward ) const;
        Actor*  getCurrentCheckpoint();
        void    teleportToNextCheckpoint( bbool _next );
        void    teleportToFirstCheckpoint( bbool _save );
        void    teleportToCurrentCheckpoint( bbool _save );
        void    resetToLastCheckpoint();
        void    resetToLastCheckpointAndSave();
        Actor*  getCheckpointEnd();

        void    teleportPlayer(u32 _player, const Vec2d & _pos, f32 _sceneZ, bbool _teleportCamera );
#ifdef ITF_SUPPORT_CHEAT
        virtual void cheatUnlockAllLevel() {}
        virtual void cheatMaxCurrency() {}
#endif
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Pause/unpause currently loaded map, and eventually base map
        ///@param _pause btrue to pause, bfalse to unpause
        void pauseMap(bbool _pause);


        ObjectRef                   getBaseMapWorld() const { if(m_baseLoaded) { return m_rBaseWorld; } else { return ITF_INVALID_OBJREF; } }
        ObjectRef                   getCurrentWorld() const { return m_rCurrentWorld; }
        ObjectRef                   getPlayerWorld() const { return m_rPlayerWorld; }
        const ITF_VECTOR< String >& getMaps() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getMaps(); }
        const ITF_VECTOR< String >& getPressConfMaps() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getPressConfMaps(); }
        const Path&                 getLoadingScreenPath() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getLoadingScreenPath(); }
        const Path&                 getIconsBtnPath() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getControllerIconPath(IconSlot_Default); }
        const Path&                 getIconsBtnPath(ControllerIconSlot _slot) const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getControllerIconPath(_slot); }
        const Path&                 getIconsBtnPathWii() const { return getIconsBtnPath(IconSlot_Wii); }
        const Path&                 getIconsBtnPathPS3() const { return getIconsBtnPath(IconSlot_PS3); }
        const Path&                 getIconsBtnPathPS5() const { return getIconsBtnPath(IconSlot_PS5); }
        const Path&                 getIconsBtnPathVita() const { return getIconsBtnPath(IconSlot_Vita); }
        const Path&                 getIconsBtnPathCTR() const { return getIconsBtnPath(IconSlot_CTR); }
        const Path&                 getIconsBtnPathSwitch() const { return getIconsBtnPath(IconSlot_Switch); }
        const Path&                 getIconsBtnPathOunce() const { return getIconsBtnPath(IconSlot_Ounce); }
        const Path&                 getIconsBtnPathXboxSeries() const { return getIconsBtnPath(IconSlot_XboxSeries); }
        const Path&                 getIconsBtnPathX360() const { return getIconsBtnPath(IconSlot_X360); }
        const Path&                 getIconsBtnPathKeyboard() const { return getIconsBtnPath(IconSlot_Keyboard); }
        const Path&                 getIconsGpePath() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getIconsGpePath(); }
        const Path&                 getIconsGpeExtraPath() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getIconsGpeExtraPath(); }
        const Path&                 getIconsSkipPath() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getIconsSkipPath(); }
        const Path&                 getMenuLogosPath() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getMenuLogosPath(); }
        bbool getUsePressConfMenu() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getUsePressConfMenu(); }
        const ITF_VECTOR< String >& getMenus() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getMenuPath(); }

        virtual void                onReachedCheckpoint(ObjectRef _ref, bbool _force = bfalse);
        void                        registerCheckpoint( ObjectRef _world, ObjectRef _checkpoint);
        void                        unregisterCheckpoint( ObjectRef _world, ObjectRef _checkpoint);

        //PLAYER MANAGEMENT
        virtual u32                 getMaxPlayerCount() const { return 0; }
        Player*                     getPlayer(u32 _index);
        Player*                     getPlayerFromActiveIndex(u32 _activeIndex);
        Player*                     getPlayerFromID(StringID _playerID);
        Player*                     getPlayerFromActor(ObjectRef _actorRef, bbool _warnIfNotAPlayer = btrue);
        Player*                     getPlayerAndModeFromActor(ObjectRef _actorRef, GameMode & _res_Mode );
        bbool                       isPlayerActor(ObjectRef _actorRef) const;
        Actor*                      getActivePlayer(u32 _index) const;
        Actor*                      getActiveAndPersistentPlayer(u32 _index) const;
        u32                         getNumActivePlayers() const;
        u32                         getNumActiveAndPersistent() const;

        void                        setMainIndexPlayer(u32 _index);
        ITF_INLINE u32              getMainIndexPlayer() const { if (m_mainIndexPlayer==U32_INVALID) return 0; return m_mainIndexPlayer;}
        ITF_INLINE u32              getMainIndexPlayer_Internal() const { return m_mainIndexPlayer; }
        ITF_INLINE void             setIndexPauseOwner(u32 _index) { m_pauseOwnerIndex = _index; }
        ITF_INLINE u32              getIndexPauseOwner() const { return m_pauseOwnerIndex; }
        ITF_INLINE void             setDeconnectedPlayerIndex(u32 _index) { m_deconnectedPlayerIndex = _index; }
        u32                         getIndexNextAvailablePlayer() const { return (m_availablePlayersIndexes.size() > 0 ? m_availablePlayersIndexes[0] : U32_INVALID); }

        void                        updateRichPresenceForPlayer(u32 _richPresenceIndex, u32 _playerIndex);
        void                        updateRichPresenceForActivePlayers(u32 _richPresenceIndex, bbool _forceAllPlayers = bfalse);
        void                        setInactiveRichPresenceIndex(u32 _inactiveRichPresenceIndex) { m_inactiveRichPresenceIndex = _inactiveRichPresenceIndex; }

        void                        resetPlayers();
        void                        activatePlayer(u32 _index);
        void                        deactivatePlayer(u32 _index);
        void                        recomputeActiveIndices();
        static bool                 comparePlayerActiveIndices(const Player* a, const Player* b);
        void                        swapPlayer(u32 _playerIndex, u32 _otherIndex );
        virtual void                changePlayerIdInfoForPlayer( Player* _player, const PlayerIDInfo* _newPlayerId );

        void                        removePlayerActors();
        virtual void                spawnPlayerActors( const StringID& _gameScreen );
        const PlayerIDInfo*         getPlayerIDInfoFromPlayerIndex( u32 _playerIndex ) const;
        const PlayerIDInfo*         getPlayerIDInfo( const StringID& _id ) const;
        bbool                       isPlayerIDInfoUsed( const StringID& _playerIDInfo ) const;
        const ITF_VECTOR< PlayerIDInfo* >& getPlayerIDInfo() const { ITF_ASSERT_CRASH( m_configTemplate != NULL, "Template must be loaded first" ); return m_configTemplate->getPlayersIDInfo(); }
        bbool                       isCustomPlayerIDInfo( const PlayerIDInfo* _info ) const;
        const StringID&             getCurrentGameScreen() const { return m_currentGameScreenID; }
        const GameScreenBase*       getCurrentGameScreenInstance() const { return m_theGameScreen; }

        // From Interface_WorldEventsListener
        virtual void                onSwitchToWorld(World* _pWorld, bbool _bSwitchDone);
        virtual void                onDeleteWorld(World* /*_pWorld*/) {};
        virtual void                onStartDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void                onEndDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void                onStartUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
        virtual void                onEndUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
        virtual void                onUpdateProcess(f32 /*_dt*/){}; // inherited from Interface_WorldEventsListener

        ITF_INLINE const String&    getCurrentWorldMapNode() const { return m_currentWorldMapNode; }
        ITF_INLINE void             setCurrentWorldMapNode(const String & _val) { m_currentWorldMapNode = _val; }

        // Used when saving for ex ..
        void backToTheMainMenuASAP(bbool backValue) { m_backToTheMainMenuASAP = backValue; }
        virtual void updateReturnToMainMenu();
        virtual void onPlayerDisconnected(u32 _padIndex);
        virtual void onPlayerReconnected(u32 _padIndex);

    public: // Interface_SceneEventsListener
        virtual void onSceneAsyncLoadFinished(const Scene* /*_pScene*/) {}
        virtual void onScenePreDestroy(Scene* /*_pScene*/) {}
        virtual void onDeleteScene(const Scene* /*_pScene*/) {}
        virtual void onObjectAdded(const Scene* /*_pScene*/, BaseObject* /*_pObj*/) {}
        virtual void onObjectRemoved(const Scene* /*_pScene*/, BaseObject* /*_pObj*/, bbool /*_delete*/) {}
        virtual void onPostSceneActivation(const Scene* _pScene, bbool _bActive);

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Save notification
        ITF_INLINE  void registerSaveNotification(const ObjectRef & _ref) {  m_saveNotificationActor = _ref; }
        ITF_INLINE  void unregisterSaveNotification() { m_saveNotificationActor = ITF_INVALID_OBJREF; }
        ITF_INLINE  ObjectRef getSaveNotification() const { return m_saveNotificationActor; }
        virtual void displaySaveNotification(bbool _visible, bbool _force = bfalse);
        bbool isSaveNotificationDisplayed() const ;

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Check if the game can display a message now.
        bbool   canDisplayTRCMessages();

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Used for rolling-demo only
        bbool   canOpenMovie();

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Check if the game can pause now. To actually know if you can, you must check that !isInPause()
        bbool   canPause();
        ITF_INLINE void   setCanPause( bbool _canPause ) { m_theGameScreen->setCanPause(_canPause) ;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Enter pause now
        virtual void    enterPause(bbool _bShowMenu = true);

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Leave pause
        virtual void    leavePause(bbool _bHideMenu = true);

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Pause all sounds
        virtual void    pauseSounds(bbool _pause);

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Are we in pause?
        bbool   isInPause() const {return m_isInPause;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Are we loading the current map ?
        bbool   isLoadingMap()const { return m_loadingMap; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool   isPlaying()const { return m_bIsPlaying; }

		///////////////////////////////////////////////////////////////////////////////////////////
		bbool   initDone()const { return m_bInitDone; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Tutorial
        StringID getTutorialMenuId() const;
        bbool   inTutorial() const;
        void    showTutorial();
        void    hideTutorial();

        // Tutorial texts
        void            setTutorialMsgActor( ObjectRef _ref ) { m_tutorialMsgActor = _ref; }
        ActorRef        getTutorialMsgActor() const { return m_tutorialMsgActor; }
        void            showTutorialText( const LocalisationId& _id, InputAdapter::PadType _padType = InputAdapter::Pad_Invalid );
        void            hideTutorialText();
        void            forceHideTutorialText();
        void            disableTutorialActor();

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Enter scoreboard
        void    showScoreboard(bbool _show);

        ///////////////////////////////////////////////////////////////////////////////////////////
        virtual void showRewards(){}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Used by editor when the world is changed without changing the game screen (tabs system)
        void    swapToWorld(ObjectRef _worldRef);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// This is needed to know when we can really starting to load important things only during the game boot
        ITF_INLINE void setStartLoadingMainMenuFirstTime(bbool bStarted) {m_loadingMainMenuStarted = bStarted;}
        ITF_INLINE bbool IsMainMenuLoadingOrLoaded() { return m_loadingMainMenuStarted; }

        virtual void goToTheMainMenu(){}

        ITF_INLINE const Path&                getMainMenuBackMap() const    { return m_configTemplate->getMainMenuBackMap(); }
        ITF_INLINE const Path&                getMainMenuBackMapForDebugSaves() const    { return m_configTemplate->getMainMenuBackMapForDebugSaves(); }

        ITF_INLINE virtual const Path&        getWorldMap() const           { return Path::EmptyPath; }
        ITF_INLINE virtual const Path&        getEndLevelMap() const        { return Path::EmptyPath; }
        ITF_INLINE const Path&                getSplashMap() const          { return m_configTemplate->getSplash1Map(); }

        ITF_INLINE const Path&                getCameraShakeConfig() const  { return m_configTemplate->getCameraShakeConfig(); }
        ITF_INLINE const ITF_VECTOR<String8> &getFamilyList() const         { return m_configTemplate->getFamilyList(); }

        //EVENTS
        virtual void                onEvent( Event* _event );
        void                        cheatIgnoreNextPause() { m_cheatIgnoreNextPause = btrue;}

		///////////////////////////////////////////////////////////////////////////////////////////
		// GAME MODES
		///////////////////////////////////////////////////////////////////////////////////////////
		void						changePlayerGameMode( Player * _player, GameMode _mode, bbool _sendEvent = btrue );
		void						changePlayerGameMode( u32 _index, GameMode _mode );
		bbool                       areAllActivePlayersInGameMode( GameMode _mode );
        void                        playerGameModeHasChanged( Player * _player );

        ITF_INLINE GameModeController*	getGameModeController( GameMode _GameModeID )	{ return m_GameModeControllers[_GameModeID]; }
        ITF_INLINE ObjectFactory*		getGameModeParametersFactory()					{ return m_GameModeParametersFactory; }
        u32                             getNbGameModeControllers()                      { return m_GameModeControllers.size(); }

        virtual bbool isLoadingVisible();

        //
        static GameManager*         s_instance;

        const ITF_VECTOR<Path> & getLuaIncludes() const { return m_configTemplate->getLuaIncludes(); }

        enum teleportTarget
        {
            None,
            NextCheckPoint,
            PrevCheckPoint,
            FirstCheckPoint,
            LastCheckPoint,
            ENUM_FORCE_SIZE_32(0)
        };

        // INPUT Manager
        ITF_INLINE ZInputManager* getInputManager() const { return m_inputManager; }

        // There are 3 differnts method to avoid more dependencies
        void addGamePlayInputListener(IInputListener* _listener);
        void addMenuInputListener(IInputListener* _listener);
        void addCheatInputListener(IInputListener* _listener);

        void removeInputListener(IInputListener* _listener);

        // Active/Desactive mapping
        void setInputModeForMenu(bbool _active);
        void setInputModeForGamePlay(bbool _active);
        void setInputModeNoFiltrer();

#ifndef ITF_FINAL
        void setStartLoadingProfileCB(startLoadingProfileCB   _func)     { m_funcStartLoadingProfile = _func;}
        void setStopLoadingProfileCB(stopLoadingProfileCB     _func)     { m_funcStopLoadingProfile  = _func;}
#endif //ITF_FINAL

        virtual void    onDisableSequencePlayers() {}

        virtual void                onBeginToLoadMap();

        bbool                       isNextOperationValid() { return m_nextOperation != GAMEOPERATION_NONE; }
        void                        clearNextOperation() { m_nextOperation = GAMEOPERATION_NONE; }

        //play default sound bus mix
        virtual void setDefaultBusMix() {}
        virtual void stopDefaultBusMix() {}
        //play bus mix for pause
        virtual void setPauseBusMix() {}
        virtual void stopPauseBusMix() {}
    public :
        typedef void (*CompletionCallback_t)();
        CompletionCallback_t m_onGameSettingLoaded = nullptr;
        void setGameSettingLoadEvent(CompletionCallback_t callback) {m_onGameSettingLoaded = callback;};
    protected:

        enum GameOperation
        {
            GAMEOPERATION_NONE,
            GAMEOPERATION_CHANGEGAMESCREEN,
            GAMEOPERATION_LOADCHECKPOINT,
            GAMEOPERATION_SAVECHECKPOINT,
            GAMEOPERATION_LOADCHECKPOINTANDSAVE, //to reinitialize checkpoint. This is for F5
            GAMEOPERATION_WAITPRELOADEDMAPREADY,
            GAMEOPERATION_SWITCHTOPRELOADEDMAP,
            GAMEOPERATION_LOADMAP,
            ENUM_FORCE_SIZE_32(1)
        };

        virtual void                postLoadInit();
        void                        manageCurrentGameScreen();
        void                        manageChangeGameScreen();
        void                        freeCurrentGameScreen();

        void                        flushPreloadMapPendingDelete();

        virtual void                updateLoading();
        virtual void                onPreloadedAsyncFinished(World * _world);
        void                        updateBaseMapLoading(); //Update loading of base map. Called by updateLoading
        void                        updateMenuLoading(); //Update loading of menu pages. Called by updateLoading
        void                        updateMenuSoundLoading(); //Update loading of menu sound isc. Called by updateLoading
        void                        checkPlayerPadDisconnected();
        void                        updateInputManager();

        bbool                       checkNextPreloadedMapReady();
        void                        processSwitchToPreloadedMap();
        void                        processLoadSubMap();

        void                        setPlaying(bbool _bPlaying) { m_bIsPlaying = _bPlaying; }

        //pause management
        virtual void                updatePauseMode();

        void                        setMapToLoad( const String& _map, const String8* _checkpointName, bbool _asynchronous, bbool _disconnectFromCells );

        void                        processNextOperation();

        //checkpoint
        virtual void                checkpointSave();
        virtual void                checkpointLoad();
        virtual void                checkpointLoadScene();
        //get modifiable level state from persistent universe
        virtual void                makeLevelStateFromPersistentUniverse() {}
        //commit modified state to universe
        virtual void                commitLevelStateToPersistentUniverse() {}
        virtual void                saveCheckpointLevelState() {}
        virtual void                loadCheckpointLevelState() {}
        void                        checkCheckpointCycling(i32 _offset);
        void                        teleportToCheckpoint( Actor* _checkpoint );

        void                        deactivate();

        virtual void                onMapLoaded(World* _pWorld);
        virtual void                onBaseMapLoaded() {}


        virtual const GameManagerConfig_Template * loadConfig(const Path & _path);

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Check if this map is not currently loading . If so, it will return its objectRef and remove it
        // from the pending delete list if it was in it.
        // @param _map : map path to check
        ObjectRef                   getPreloadedMap( const Path& _map );


        ///////////////////////////////////////////////////////////////////////////////////////////
        // GAME MODES
        //////////////////////////////////////////////////////////////////////////////////////////
        // Register a game mode controller
        template <class T> void		registerGameModeController( GameMode _GameModeID )
        {
            ITF_ASSERT( m_GameModeControllers[_GameModeID] == NULL );			// Can't re-register same controller
            m_GameModeControllers[_GameModeID] = T::template CreateController<T>( _GameModeID );
        }
        void						updateGameModes();
        void                        processPlayerGameModeChanged( EventPlayerModeChanged* _eventGameMode );		// From event
        void						changePlayerGameModeWithControllers( Player * _player, GameMode _mode, bbool _sendEvent );

        struct WorldMenu
        {
            ObjectRef  m_worldMenuRef;
            String8  m_worldMenuBaseName;
        };

        //
        StringID                    m_nextGameScreenID;
        StringID                    m_currentGameScreenID;
        GameScreenBase*             m_theGameScreen;
        GameScreenBase*             m_nextGameScreen; //allocated but not initialized

        bbool                       m_isInPause;
        bbool                       m_pauseMenuRequested;
        String                      m_gameManagerType;
        SafeArray<SourceHandle>     m_playingVoicesWhenEnteringPause;
        //menu
        ITF_VECTOR<WorldMenu>       m_worldMenu;
        ObjectRef                   m_mainMenuWorld;
        bbool                       m_areMenuLoaded;
        ObjectRef                   m_menuSoundWorld;
        bbool                       m_menuSoundLoaded;

        //divers
        bbool                       m_backToTheMainMenuASAP;
        u32                         m_mainIndexPlayer, m_pauseOwnerIndex, m_deconnectedPlayerIndex;
        u32                         m_inactiveRichPresenceIndex, m_lastRichPresenceIndex;

        //loading
        bbool                       m_loadingMap;
        ObjectRef                   m_rBaseWorld;
        ObjectRef                   m_rCurrentWorld;
        ObjectRef                   m_rPlayerWorld;
        ObjectRef                   m_rGame2DWorld;
        bbool                       m_rGame2DWorldLoaded;
        bbool                       m_baseLoaded;
        bbool                       m_mapSwitched;
        String                      m_initialMap;
        bbool                       m_bIsPlaying;
		bbool                       m_bInitDone;
        SafeArray <Player*>         m_players;
        SafeArray <u32>             m_availablePlayersIndexes;
        ObjectRef                   m_nextPreloadedMap;

        Path                        m_subMapToLoad;

        ObjectRefList               m_preLoadedAsyncWorld;
        ObjectRefList               m_preLoadedPrefetchWorld;
        ObjectRefList               m_preLoadedPendingWorldToDelete;

        //LOADING
        PersistentGameData_Universe *m_persistentGameData;
        StringID m_currentLevelName;
        // TEMP E3 : done to know exactly when world can be paused right after the world
        // is physically ready (in order to pause it at the very beginning)
        // TODO : need to find a way to be able to pause world at the very beginning,
        // without any sounds played.
        bbool                       m_worldPhysicallyReady;
        // END TEMP

        WorldCheckpointList         m_checkpoints;
        ObjectRef                   m_currentCheckpoint;
        ObjectRef                   m_currentSceneRef;
        ObjectPath                  m_firstCheckpointPath;
        GameOperation               m_nextOperation;

        String                      m_currentWorldMapNode;

        // FX
        AfterFxHandler              m_fxHandler;

        bbool                       m_loadingMainMenuStarted;
        bbool                       m_cheatIgnoreNextPause;

		// GameModes
		ObjectFactory*                  m_GameModeParametersFactory;
		SafeArray<GameModeController*>	m_GameModeControllers;
        ArchiveMemory*                  m_checkpointData;

        // Input manager for action map
        ZInputManager*                  m_inputManager;
        i32                             m_inputModeMenuCounter, m_inputModeGamePlayCounter;
        TemplateClientHandler           m_templateClientHandler;

        // Save Notification
        ObjectRef                       m_saveNotificationActor;
    private:
        const GameManagerConfig_Template*   m_configTemplate;
        ITF_MAP<Path, ObjectRef>            m_preloadMaps;

        ActorRef                        m_tutorialMsgActor;

#ifndef ITF_FINAL
        startLoadingProfileCB           m_funcStartLoadingProfile;
        stopLoadingProfileCB            m_funcStopLoadingProfile;
#endif //ITF_FINAL

        void updatePlayersCellsView();
        void initInputManager();
    };
}

#endif // _ITF_GAMEMANAGER_H_

