#ifndef _ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_
#define _ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_

#ifndef _ITF_GAMEMODECONTROLLER_H_
#include "gameplay/managers/GameModeController.h"
#endif //_ITF_GAMEMODECONTROLLER_H_

namespace ITF
{
    //------------------------------------------------------------------------------------------------------------
    //                                      SHOOTER GAME MODE PARAMS
    //------------------------------------------------------------------------------------------------------------
	class Ray_ShooterGameModeParameters : public GameModeParameters
	{
	public :
		DECLARE_SERIALIZE();
		DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterGameModeParameters, GameModeParameters,1675665911);

		Ray_ShooterGameModeParameters() {};
		virtual ~Ray_ShooterGameModeParameters(){};

        class PlayerData
        {
        public:
            DECLARE_SERIALIZE();
            ITF_INLINE const StringID &     getPlayerID() const         { return m_playerID; }
            ITF_INLINE const Path &         getPlayerLuaFile() const    { return m_luaFile; }
        private:
            StringID	m_playerID;
            Path		m_luaFile;
        };

        typedef ITF_VECTOR<PlayerData> PlayerDataList;

        ITF_INLINE const PlayerDataList &       getPlayerDataList()     { return m_playerDataList; }
        const PlayerData *                      getPlayerDataFromPlayerID( const StringID & _playerID ) const;

    private:
        ITF_VECTOR<PlayerData>      m_playerDataList;
    };

    //------------------------------------------------------------------------------------------------------------
    //                                          SHOOTER GAME MODE
    //------------------------------------------------------------------------------------------------------------
	
    class Ray_ShooterGameModeController : public GameModeController 
	{
	public :
		// Constructor / destructor
        Ray_ShooterGameModeController() { ITF_ASSERT(bfalse); m_gameMode = GAMEMODE_UNKNOWN; }
        Ray_ShooterGameModeController( GameMode _gamemode );
		virtual ~Ray_ShooterGameModeController();

        virtual void    update();
        virtual void    onPlayerGameModeHasChanged( Player * _player );
        virtual void    onCheckPointLoaded( ObjectRef _checkPointRef );
        virtual void    onPlayerActivationChanged( Player * _player, bbool _startDead );
        virtual void    onPlayerSwapped( u32 _from, u32 _to );
        virtual bbool   teleportToNextCheckpoint( bbool _next );

        virtual void    spawnPlayerActors( const StringID& _gameScreen );
        virtual void    onPlayerActorsRemoved();

        void            registerCheckPoint( const ObjectRef & _objref );
        void            unregisterCheckPoint( const ObjectRef & _objref );
        void            setActiveCheckPoint( const ObjectRef & _objref, bbool _cameraActivator = bfalse );
        
        ITF_INLINE const ObjectRef & getActiveCheckPoint() const { return m_activeCheckPoint; }
        ITF_INLINE Ray_ShooterGameModeParameters * getShooterGameModeParameters() const { return (( m_parameters != NULL ) ? m_parameters->DynamicCast<Ray_ShooterGameModeParameters>(ITF_GET_STRINGID_CRC(Ray_ShooterGameModeParameters,1675665911)) : NULL ); }
        ITF_INLINE bbool            activeCheckPointIsCameraActivator() const { return m_activeCheckPointIsCameraActivator; }

        // shooter specific
        void            setPlayerReadyToRestart( Player * _player );
        bbool           isAllPlayerDead();
        void            setPlayerInMoskitoZone( u32 _playerIdx, bool _inZone )      { if ( _playerIdx < m_playersData.size() ) m_playersData[ _playerIdx ].m_inMoskitoZone = _inZone; }
        bbool           isPlayerInMoskitoZone( u32 _playerIdx ) const               { return ( ( _playerIdx < m_playersData.size() ) ? m_playersData[ _playerIdx ].m_inMoskitoZone : bfalse ); }
        void            setPlayerLanding( u32 _playerIdx, bbool _landing )          { if ( _playerIdx < m_playersData.size() ) m_playersData[ _playerIdx ].m_isLanding = _landing; }

#ifdef SHOOTER_PROFILING
#define SP_ADD( _val, _ref )            RAY_GAMEMANAGER->getShooterGameModeController()->addProfilingValue( _val, _ref );
        void            addProfilingValue( const StringID & _value, ActorRef _owner );
        void            removeProfilingValue( const StringID & _value, ActorRef _owner );
        void            displayProfilingValues();
#endif //SHOOTER_PROFILING

    protected:

        virtual void    onActivated();
        virtual void    onDesactivated();
        virtual void    onGameModeParametersCreated();
        virtual void    onGameModeParametersBeforeDestroy();

        void            createPlayers();
        void            destroyPlayers();
        void            registerPlayersToGameManager();
        
        Actor *         findNearestCheckPointFromPos( const Vec3d & _pos, const Vec2d & _dir, bbool _avoidActive = bfalse );
        void            teleportPlayerOnActiveCheckPoint( Player * _player );
        void            disablePlayerForCurMode( Player * _player );
        void            activatePlayerForCurMode( Player * _player, const Vec3d & _pos );
        
        Actor *         getPlayerActorForCurMode( Player * _player );
        bbool           isExitCheckpoint( const ObjectRef & _objref );

        void            resetPlayerReadyToRestart();

    protected:
        ObjectRef               m_activeCheckPoint;
        bbool                   m_activeCheckPointIsCameraActivator;
        SafeArray<ObjectRef>    m_checkPointList;

        // class to store ingame shooter player data
        class PlayerInGameData
        {
        public:
            PlayerInGameData() { Reset(); }
            void Reset()       { m_inMoskitoZone = bfalse; m_readyToRestart = bfalse; m_active_Internal = bfalse; m_active_Visible = bfalse; m_isLanding = bfalse; }
            bbool   m_inMoskitoZone; 
            bbool   m_readyToRestart;
            bbool   m_active_Internal;
            bbool   m_active_Visible;
            bbool   m_isLanding;
        };

        SafeArray <PlayerInGameData, 4>    m_playersData;

        bbool           m_swapMode;
        bbool           m_swapMode_PlayerActive;
        
        bbool           m_playersRegisteredToGameManager;


#ifdef SHOOTER_PROFILING
        struct SP_Data
        {
            StringID    m_value;
            SafeArray<ActorRef> m_actorList;
        };
        ITF_VECTOR<SP_Data>     m_SP_DataList;
#endif //SHOOTER_PROFILING

	private :
		Ray_ShooterGameModeController(const GameModeController &_other) {ITF_ASSERT(bfalse);}			// Private copy constructor to avoid bad manip
	};


}// namespace ITF

#endif //_ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_