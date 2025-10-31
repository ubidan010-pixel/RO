#ifndef _ITF_GAMEMODECONTROLLER_H_
#define _ITF_GAMEMODECONTROLLER_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_  

namespace ITF
{
	class GameModeParameters : public IRTTIObject
	{
	public :
		DECLARE_SERIALIZE_VIRTUAL();
		DECLARE_OBJECT_CHILD_RTTI(GameModeParameters, IRTTIObject,3896257179);

		GameModeParameters() {};
		virtual ~GameModeParameters(){};
	};

	
	class GameModeController
	{
	public :

		// Creation
		template <class T>
		static T* CreateController( GameMode _gamemode ){ return newAlloc(mId_Gameplay, T(_gamemode) ); }
		
		// Constructor / destructor
        GameModeController() : m_active( bfalse ) , m_parameters( NULL ) , m_checkGameModeParamsOwnersCoherency( bfalse ) { ITF_ASSERT( m_gameMode != GAMEMODE_UNKNOWN ); }
		GameModeController( GameMode _gamemode );
		virtual ~GameModeController();

		// Main update function
		virtual void        update();

        // register parameters for given owner
        void                registerGameModeParameters( GameModeParameters * _gamemodeParams, const ObjectRef & _owner );
        void                unregisterGameModeParametersOwner( const ObjectRef & _owner );

		// Returns true if the player game mode can be changed, else store it to do it has soon all is ready 
		bbool	            requestPlayerModeChange( Player * _player, bbool _ifnotRegisterReq = btrue );
        void	            removePlayerModeChangeRequest( Player * _player );
        
        // player game mode changed
        virtual void        onPlayerGameModeHasChanged( Player * _player );
        // checkpoint loaded
        virtual void        onCheckPointLoaded( ObjectRef _checkPointRef );
        // player activation
        virtual void        onPlayerActivationChanged( Player * _player, bbool _startDead );
        // swap
        virtual void        onPlayerSwapped( u32 _from, u32 _to )   {}
        // player spawn
        virtual void        spawnPlayerActors( const StringID& _gameScreen ) {}
        // called when the player actor list has been destroyed
        virtual void        onPlayerActorsRemoved() {}

        virtual bbool       teleportToNextCheckpoint( bbool _next  )    { return bfalse; }

        
        ITF_INLINE bbool    isActive()const                         { return m_active; }
        ITF_INLINE void     setActive( bbool _active )              { if ( _active != m_active ){ if ( m_active == _active ) return; m_active = _active; if ( m_active ) { onActivated(); } else { onDesactivated(); } } }

	protected :		

        virtual void        onActivated()                   {}
        virtual void        onDesactivated()                {}
        virtual void        onGameModeParametersCreated()   {}
        virtual void        onGameModeParametersBeforeDestroy() {}

//        void                registerPlayerActor( Player * _player, ObjectRef _actor );

        void                gameRegisterPlayerActorForMode( const StringID & _playerID, GameMode _gameMode, bbool _reset );
        void                gameRegisterPlayerActorForModeAndIdx( i32 _idx, GameMode _gameMode, bbool _reset );
        void                gameUnregisterPlayerActorForMode( const StringID & _playerID );
        ObjectRef           getRegisteredPlayerActorForMode( const StringID & _playerID );
        i32                 getRegisteredPlayerIdx( const StringID & _playerID );

        void	            checkGameModeActivation();
		bbool	            isPlayerCanChangeToCurMode( Player * _player );
		void	            processPlayerModeChangeRequests();
        void	            checkGameModeParamsOwnersCoherency();


	protected :
		GameMode				            m_gameMode;
		bbool					            m_active;
        GameModeParameters  *               m_parameters;
        SafeArray< ObjectRef >              m_parametersOwnerList;      
        bbool                               m_checkGameModeParamsOwnersCoherency;
		
		SafeArray< Player *>	            m_PlayerModeChangeRequests;
        
        struct RegisteredPlayer
        {
            StringID    m_playerID;
            ObjectRef   m_actor;
        };
        SafeArray< RegisteredPlayer >       m_registeredPlayerActors;


	private :
        GameModeController(const GameModeController &_other) {ITF_ASSERT(bfalse);}			// Private copy constructor to avoid bad manip
	};


}// namespace ITF

#endif //_ITF_GAMEMODECONTROLLER_H_