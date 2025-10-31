#ifndef _ITF_GAMESCREENBASE_H_
#define _ITF_GAMESCREENBASE_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#include "engine/scene/worldEventInterface.h"
#endif //_ITF_WORLD_EVENT_INTERFACE_H_

namespace ITF
{
    class ActorRef;
    class Player;

    //////////////////////////////////////////////////////////////////////////
    ///Base class for game screens (main menu screen, gameplay screen, end screen...)
    class GameScreenBase : public IRTTIObject, public Interface_WorldEventsListener
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(GameScreenBase, IRTTIObject,3085858710);

    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Base class for parameters
        class InitializationParameters
        {
        public:
            InitializationParameters() {}
            virtual ~InitializationParameters() {}
            
            virtual InitializationParameters *clone() const = 0;
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Constructor. Called when entering the screen
        GameScreenBase();
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Destructor. Called when quitting the screen
        virtual ~GameScreenBase();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Can the game pause now?
        virtual bbool canPause() {return bfalse;}
        virtual void setCanPause(bbool _canPause) { m_canPause = _canPause; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// initialization. Called right after the constructor, when entering the screen
        virtual void init() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// termination. Called right before the destructor, when quitting the screen
        virtual void close() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// update. Called each frame, just after pad read (...)
        virtual void update() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// return btrue if this screen has gameplay
        virtual bbool hasGameplay() {return bfalse;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// return btrue if we should spawn the players even if there is no checkpoint (someone else will handle the start position)
        virtual bbool getSpawnPlayersAlways() {return bfalse;}
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// return btrue if we can display a TRC message
		virtual bbool canDisplayTRCMessages() {return btrue;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// return btrue if a video can be opened
        virtual bbool canOpenMovie() {return btrue; }
		
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///This must be called at gamescreen creation, right after construction, to set initialization parameters r
        void setInitParams(InitializationParameters *_params) {ITF_ASSERT(!m_initializationParams);m_initializationParams = _params;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// this callback is fired when the world in m_world finished loading
        virtual void onWorldLoaded() {}
        virtual void onStartDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void onEndDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void onStartUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
        virtual void onEndUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
        virtual void onUpdateProcess(f32 /*_dt*/){}; // inherited from Interface_WorldEventsListener
        virtual void onMapLoaded(ObjectRef _world){}; 

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// this callback is fired when a player signs in/out (status changes)
        /// @param _playerIndex the player index
        /// @param _signedIn btrue if signed in, bfalse if signed out
        /// @param _isOnlineStatusChange btrue if its an online status change, bfalse if it's a profile change
        /// @return btrue if must display TRC message
        virtual bbool onUserSignedInOut(u32 /*_playerIndex*/, bbool /*_signedIn*/, bbool _isOnlineStatusChange);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// called by game manager when end of level is reached. 
        /// There may be saving operations pending, so you should wait for their completion before exiting
        virtual void onEndOfLevelReached() {m_endOfLevelReached=btrue;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// called when a movie is over
        virtual void onEndOfPlayingVideo(){}
        virtual void onStartOfPlayingVideo(){}
        virtual void onPauseOfPlayingVideo(){}
        virtual void onEndOfFadeIn(){}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Show or hide the pause menu
        /// @param _isVisible btrue if the menu is visible
        virtual void showPauseMenu (bbool /*_isVisible*/) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        // Tutorial
        virtual bbool inTutorial() const { return bfalse; }
        virtual void showTutorial () {}
        virtual void hideTutorial () {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Show scoreboard
        /// @param _isVisible btrue if the menu is visible
        virtual void showScoreboard (bbool /*_isVisible*/) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Show level stats
        /// @param _isVisible btrue if the menu is visible
        virtual void showLevelStats (bbool /*_isVisible*/) {}

        virtual bbool isLoaded() const { return bfalse; }

        // WORLDMAP
        virtual void        setPreLoadedMap( ObjectRef _mapRef ) {}
        virtual ObjectRef   getPreLoadedMap() const { return ObjectRef::InvalidRef; }

    protected:

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///(optional) Call from update to check signin/signout changes. may change m_currentlyExiting, returns its value
        virtual bbool checkSignInOutChanges() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        virtual void onSwitchToWorld(World* _pWorld, bbool _bSwitchDone);
        virtual void onDeleteWorld(World* _pWorld);

        void BeginLoading();
        void EndLoading();

        World *m_world;
        bbool m_isWorldLoaded;
        bbool m_currentlyExiting;
        bbool m_playerSignedInOut;
        bbool m_endOfLevelReached;

        bbool m_canPause;

        bbool m_prefetchStarted;

        const InitializationParameters * getInitParams() const {return m_initializationParams;}

    private:
        friend class GameManager;
        InitializationParameters *m_initializationParams;

        bbool m_resourceLoadingPaused;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Set the World, USE ONLY IF YOU CHANGE THE WORLD WITHOUT CHANGING THE GAMESCREEN
        /// @param _pWorld the world to set
        void        setInternalWorld(World* _pWorld) { m_world = _pWorld; }
    };
}

#endif
