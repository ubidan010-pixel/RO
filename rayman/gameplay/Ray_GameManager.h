#ifndef _ITF_RAY_GAMEMANAGER_H_
#define _ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_


#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_RAY_WORLDFILE_H_
#include "rayman/gameplay/Ray_WorldFile.h"
#endif //_ITF_RAY_WORLDFILE_H_

#ifndef _ITF_RAY_PERSISTENTGAMEDATA_H_
#include "rayman/gameplay/Ray_PersistentGameData.h"
#endif

#ifndef _ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_
#include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#endif //_ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_

#ifndef _ITF_RAY_WORLDMAPGAMEMODECONTROLLER_H_
#include "rayman/gameplay/Managers/Ray_WorldMapGameModeController.h"
#endif //_ITF_RAY_WORLDMAPGAMEMODECONTROLLER_H_

#ifndef _ITF_RAY_POWERUPMANAGER_H_
#include "rayman/gameplay/Ray_PowerUpManager.h"
#endif //_ITF_RAY_POWERUPMANAGER_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_RAY_GAMEOPTIONMANAGER_H_
#include "Managers/GameOptions/Ray_GameOptionManager.h"
#endif //_ITF_RAY_GAMEOPTIONMANAGER_H_

#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#include "Managers/GameOptions/Ray_GameOptionNames.h"
#endif //_ITF_RAY_GAMEOPTIONNAMES_H_

#ifndef _ITF_RAY_GAMEOPTIONPERSISTENCE_H_
#include "Managers/GameOptions/Ray_GameOptionPersistence.h"
#endif //_ITF_RAY_GAMEOPTIONPERSISTENCE_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif //_LOCALISATION_ID_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

#ifndef ITF_MOVIEPLAYER_H_
#include "engine/video/MoviePlayer.h"
#endif //ITF_MOVIEPLAYER_H_

#ifdef ITF_SUPPORT_BOT_AUTO
#ifndef _RAY_BOTCONTROLLER_H_
#include "gameplay/AI/PlayerBot/BotController.h"
#endif //_RAY_BOTCONTROLLER_H_
#ifndef _RAY_PERCEPTIONMODULE_H_
#include "gameplay/AI/PlayerBot/PerceptionModule.h"
#endif //_RAY_PERCEPTIONMODULE_H_
#endif // ITF_SUPPORT_BOT_AUTO

namespace ITF
{
    #define RAY_GAMEMANAGER Ray_GameManager::get()

    class ArchiveMemory;
    class Ray_EventSpawnReward;
    class Ray_TRCHelper;

    enum EPlayerHeartTier
    {
        HeartTier_None = 0,
        HeartTier_Red = 1,
        HeartTier_Golden = 2,
        HeartTier_Diamond = 3
    };

    class Ray_Player : public Player
    {
    public:
        Ray_Player();
        virtual void reset();

        ITF_INLINE u32        getHitPoints() const { return m_hitPoints; }
        ITF_INLINE void       setHitPoints(u32 _val) { m_hitPoints = _val; }
                   void       addHitPoints(i32 _numHP, bbool _evenIfInvincible = bfalse, bbool _broadcastEvent = bfalse);
                   void       die() { addHitPoints(-static_cast<i32>(m_hitPoints)); }
                   void       revive();
           virtual bbool      isDead() const {return getHitPoints()<=0;}

        ITF_INLINE u32        getMaxHitPoints() const { return m_maxHitPoints; }
        ITF_INLINE void       setMaxHitPoints(u32 _val) { m_maxHitPoints = _val; }
                   void       addMaxHitPoints(i32 _numHP);

       ITF_INLINE bbool       getIsLeaving() const { return m_isLeaving; }
       ITF_INLINE void        setIsLeaving(bbool _val) { m_isLeaving = _val; }

        ITF_INLINE bbool      getInvincible() const { return m_invincible; }
        ITF_INLINE void       setInvincible(bbool _val) { m_invincible = _val; }

                   bbool      heartNeeded() const;

        ITF_INLINE void       setWasHitInLevel( bbool _val ) { m_wasHitInThisLevel = _val; }
        ITF_INLINE bbool      getWasHitInLevel() const { return m_wasHitInThisLevel; }

        ITF_INLINE void       setSprintReleased( bbool _val ) { m_sprintReleased = _val; }
        ITF_INLINE bbool      getSprintWasReleased() const { return m_sprintReleased; }

        ITF_INLINE f32        getSprintDuration() const { return m_sprintDuration; }
        ITF_INLINE void       resetSprintDuration() { m_sprintDuration = 0.0f; }
        ITF_INLINE void       addSprintDuration(f32 _dt) { m_sprintDuration += _dt; }

        virtual void          toggleDeadMode();

        ITF_INLINE EPlayerHeartTier getHeartTier() const { return m_heartTier; }
        ITF_INLINE void             setHeartTier(EPlayerHeartTier _tier) { m_heartTier = _tier; }
                   void             onTakeDamage();
        ITF_INLINE u32              getConsecutiveHits() const { return m_consecutiveHits; }
        ITF_INLINE f32              getConsecutiveHitTimer() const { return m_consecutiveHitTimer; }
                   void             resetConsecutiveHits();
                   void             updateConsecutiveHitTimer(f32 _dt);

    private:
        bbool   m_isLeaving;
        bbool   m_invincible;
        u32     m_hitPoints;
        u32     m_maxHitPoints;
        bbool   m_wasHitInThisLevel;
        bbool   m_sprintReleased;
        f32     m_sprintDuration;
        EPlayerHeartTier m_heartTier;
        u32     m_consecutiveHits;
        f32     m_consecutiveHitTimer;
    };

    class Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceBase() : m_isFinished(bfalse) {}
        virtual ~Ray_GameManagerSequenceBase() {}

        virtual void            stop() {}
        virtual void            update( f32 _dt ) {}
        virtual void            onEvent( Event* _event ) {}
        virtual bbool           isSensitiveToPause() const { return btrue;}

        bbool                   isFinished() const { return m_isFinished; }
        virtual bbool           isPlayerInSequence( ObjectRef _player ) const { return bfalse; }

    protected:

        bbool                   m_isFinished;
    };

    class Ray_GameManagerSequenceTeleportCheat : public Ray_GameManagerSequenceBase
    {
    public:
        Ray_GameManagerSequenceTeleportCheat();
        virtual ~Ray_GameManagerSequenceTeleportCheat() {}

        void            start( GameManager::teleportTarget _target );
        virtual void    update( f32 _dt );
        virtual void    stop();

    private:
        GameManager::teleportTarget m_target;
        class View*                 m_prefetchView;

        enum SequenceState
        {
            State_Idle,
            State_FadeIn,
            State_WaitFadeAndPrefetchEnd,
            State_FadeOut,
            State_WaitFadeOutEnd,
            ENUM_FORCE_SIZE_32(0)
        };

        SequenceState m_state;
    };

    class Ray_GameManagerSequenceVictory : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceVictory() : m_goToWorldMap(bfalse) {}
        ~Ray_GameManagerSequenceVictory() {}

        void            start( const ObjectPath& _path, bbool _goToWorldMap );
        virtual void    onEvent( Event* _event );
        virtual bbool   isPlayerInSequence( ObjectRef _player ) const { return btrue; }

    private:

        ObjectPath      m_path;
        bbool           m_goToWorldMap;
    };

    class Ray_GameManagerSequenceHiddenArea : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceHiddenArea();
        ~Ray_GameManagerSequenceHiddenArea() {}

        void                        start( ObjectRef _start, ObjectRef _end, f32 _waitPlayersDuration, f32 _waitLoadDuration );
        virtual void                stop();
        void                        addPlayerEntering( ObjectRef _ref );
        virtual bbool               isPlayerInSequence( ObjectRef _player ) const { return m_playersMoving.find(_player) != -1; }
        virtual void                update( f32 _dt );

        ObjectRef                   getStart() const { return m_start; }
        ObjectRef                   getEnd() const { return m_end; }

    private:

        enum SeqState
        {
            STATE_WAITFORPLAYERSENTRY,
            STATE_FADEOUT,
            STATE_WAIT,
            STATE_FADEIN,
            ENUM_FORCE_SIZE_32(4)
        };

        void                        updateWaitForPlayersEntry( f32 _dt );
        void                        startFadeOut();
        void                        updateFadeOut( f32 _dt );
        void                        updateWait( f32 _dt );
        void                        updateFadeIn( f32 _dt );

        void                        updateMoving();

        ObjectRef                           m_start;
        ObjectRef                           m_end;
        FixedArray <ObjectRef,s_maxPlayers> m_playersMoving;
        SeqState                            m_state;
        Vec2d                               m_entry;
        Vec2d                               m_exit;
        f32                                 m_timeCounter;
        f32                                 m_waitPlayersDuration;
        f32                                 m_waitLoadDuration;
        u32                                 m_playersProcessed;
        bbool                               m_noEntry;
    };

    class Ray_GameManagerRestoreDeath : public Ray_GameManagerSequenceBase,public UIMenuManager::MenuItemActionListener
    {
    public:

        Ray_GameManagerRestoreDeath();
        ~Ray_GameManagerRestoreDeath();

        void                        start();
        virtual void                stop();
        virtual bbool               isPlayerInSequence( ObjectRef _player ) const { return btrue; }
        virtual void                update( f32 _dt );
        //virtual bbool               isSensitiveToPause() const { return bfalse;}
        bbool                       isSkipActorDone();

        void                        cancelSkipLevel();
        void                       showSkipActor(bbool _show, bbool _bforce);

        void                        skipLevel();
        virtual void                onMenuItemAction (UIComponent* _UIComponent);
        virtual StringID            onMenuPageAction(UIMenu * , const StringID & , const StringID &_defaultAction);

        bbool                       canPause();
    private:

        enum RestoreState
        {
            STATE_FADE_BLACK,
            STATE_ASK_TO_SKIP,
            STATE_WAIT_FOR_ACTOR,
            STATE_FADE_GAME,
            ENUM_FORCE_SIZE_32(1)
        };

        void                                updateFadeBlack( f32 _dt );
        void                                updateFadeToGame( f32 _dt );

        bbool                               showSkipLevelActor();

        void                                freePrefetch();

        void                                startSoundFadeOut();
        void                                startSoundFadeIn();

        RestoreState                        m_currentState;
        f32                                 m_prefetchFailSafe;
        class View*                         m_prefetchView;

        bbool                               m_askToSkip;
        bbool                               m_skipLevel;
        bbool                               m_soundFaded;
        bbool                               m_hasChangedInput;
    };

    class Ray_GameManagerPlaySeqActor : public Ray_GameManagerSequenceBase
    {
    public:

        void                                start();
        virtual void                        stop();
        void                                addSequence( const Path& _path, const Vec3d& _pos, Scene* _scene );
        virtual void                        update( f32 _dt );

    private:

        struct SeqInfo
        {
            ObjectRef       m_sceneRef;
            ActorRef        m_actorRef;
        };

        bbool                               updateSeq( const SeqInfo& _ref );

        SafeArray<SeqInfo>                  m_playingSeqs;
    };

    class Ray_GameManagerSequenceCheckpoint : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceCheckpoint();
        ~Ray_GameManagerSequenceCheckpoint() {}

        void                                start( ObjectRef _checkpoint, const Path& _sequencePath, f32 _waitLength );
        virtual void                        stop();
        void                                resume();
        void                                addPlayerSaving( ObjectRef _ref );
        virtual bbool                       isPlayerInSequence( ObjectRef _player ) const { return m_playersInside.find(_player) != -1; }
        virtual void                        update( f32 _dt );

    private:

        enum SeqState
        {
            STATE_WAITFORPLAYERSENTRY,
            STATE_PLAYSEQUENCE,
            ENUM_FORCE_SIZE_32(2)
        };

        void                                updateWaitForPlayersEntry( f32 _dt );
        void                                updatePlaySequence( f32 _dt );

        Vec2d                               getTargetPointForPlayer( u32 _playerIndex ) const;

        FixedArray <ActorRef,s_maxPlayers>  m_playersInside;
        FixedArray <bbool,s_maxPlayers>     m_playersInPosition;
        Path                                m_sequenceActorPath;
        ActorRef                            m_checkpoint;
        ActorRef                            m_spawnedSeq;
        ObjectRef                           m_spawnedSeqScene;
        f32                                 m_waitCounter;
        SeqState                            m_state;
    };


	class Ray_GameManagerSequenceChangePage : public Ray_GameManagerSequenceBase
	{
	public:
		Ray_GameManagerSequenceChangePage();
		virtual ~Ray_GameManagerSequenceChangePage();

		void					start(  Vec3d const & _enterPoint,
                                        Vec3d const & _exitPoint,
                                        Vec3d const & _finalPoint,
                                        bbool       _verticalEjectToFinalPoint,
                                        ActorRef    _triggerPlayer,
                                        ActorRef    _teleportPage,
                                        ActorRef    _destinationPage,
                                        ActorRef    _checkpoint,
                                        f32         _waitPlayersDuration,
                                        bbool       _useFade,
                                        bbool       _isCageMap);
		virtual void            stop();
		virtual void            update( f32 _dt );

        void                    addPlayer( ActorRef _playerRef );

        ITF_INLINE bbool        canPauseGame() {return m_state == STATE_WAIT_BEFORE_TRAVEL;}

        ActorRef                getStartPage() const { return m_teleportPage; }
        ActorRef                getTriggerer() const { return m_triggerPlayer; }

        virtual bbool           isPlayerInSequence( ObjectRef _player ) const { return isActorOnList(_player); }

        const Vec3d&            getTravelFinalPoint() const { return m_travelFinalPoint; }

	private:
        enum ChangePageState
        {
            STATE_WAIT_BEFORE_TRAVEL,
            STATE_TRAVELLING,
            STATE_TRAVELLING_FINISHED,
            STATE_FADE_OUT,
            STATE_FADE_OUT_WAIT,
            STATE_TELEPORT,
            STATE_FADE_IN,
            STATE_ENTER,
            STATE_POST_ENTER,
            STATE_NONE,
            ENUM_FORCE_SIZE_32( state_SequenceChangePage )
        };

		struct PlayerTravelInfo
		{
			ActorRef	m_player;
			Spline		m_spline;
			f32			m_speed;
			Vec3d		m_moveDir;
			bbool		m_travelfinished;
			Vec2d		m_enterEjectForce;
			bbool		m_enterMovefinished;
            bbool       m_dead;
		};

        bbool                   isActorOnList( ActorRef _ref ) const;

		ActorRef 	            m_triggerPlayer;
        ActorRef                m_teleportPage;
        ActorRef                m_destinationPage;
        ActorRef                m_checkpoint;
        ActorRef                m_prefetchRef;
        class View*             m_prefetchView;
		Vec3d		            m_travelEnterPoint;
		Vec3d		            m_travelExitPoint;
		Vec3d		            m_travelFinalPoint;
		bbool		            m_verticalEjectToFinalPoint;

		bbool		            m_travelFinished;
        ChangePageState         m_state;

        f32                     m_waitPlayerTimer;
        bbool                   m_useFade;
        u32                     m_waitTeleport;

        PackedObjectPath        m_cageMapEntry; //This is so that we can set the "visited" state in persistent game data, after the checkpoint

        void                    changeState( ChangePageState _state );
        void                    updateTravellingState( f32 _dt );
        void                    updateEnterState( f32 _dt );
        void                    updatePostEnterState( f32 _dt );

        bbool                   needWaitForPlayers() const;

		void					initPlayerTravel( Ray_Player* _player, PlayerTravelInfo & _travelInfo );
		void					updatePlayerTravel( f32 _dt, PlayerTravelInfo & _travelInfo );
		void					getTravelExitPointWithActorFeet( Vec3d & _res_Pos, Actor * _actor );

        void                    setPrefetch();
        bbool                   checkPrefetchFinished() const;
        void                    clearPrefetch();

		FixedArray <PlayerTravelInfo,s_maxPlayers> m_playersTravelInfo;

	};

    class Ray_GameManagerSequenceStargate : public Ray_GameManagerSequenceBase
    {
    public:
        Ray_GameManagerSequenceStargate();
        virtual ~Ray_GameManagerSequenceStargate();

        void					start( ActorRef _stargate, const Vec3d& _start, f32 _speed, f32 _waitForPlayersTime,
                                       f32 _waitDistFromDoor, f32 _waitAngleOffset, f32 _waitPointZOffset, f32 _startToDoorTimeMultiplier,
                                       const Vec3d& _cameraOffset );
        virtual void            stop();
        virtual void            update( f32 _dt );
        virtual void            onEvent( Event* _event );
        virtual bbool           isPlayerInSequence( ObjectRef _player ) const { return btrue; }

    private:

        enum ActorState
        {
            ACTORSTATE_SUCKIN,
            ACTORSTATE_WAIT,
            ACTORSTATE_TRAVEL,           // Traveling from one door to the other, changing z
            ACTORSTATE_LANDING,          // Last part of the trip, landing on the target
        };

        enum SeqState
        {
            SEQSTATE_TAKEOFF,
            SEQSTATE_TRAVEL,
        };

        struct ActorInfo
        {
            ActorRef	m_actor;
            ActorState  m_currentState;
            Vec2d       m_startDif;
            bbool       m_finished;
        };

        void                    setupPlayer( u32 _index, Actor* _actor, const Vec3d& _start, f32 _speed );
        void                    calculateStartPosForPlayer( u32 _index, Actor* _actor, const Vec3d& _start, Vec3d& _ret );
        void                    preparePlayersTrip();
        void                    setPlayerSpline( Actor* _actor, ActorInfo& _actorInfo, bbool _updateCurrentNode, Spline& _spline );

        void                    updateTakeOff( f32 _dt );
        void                    updateTravel( f32 _dt );
        void                    updateFade( f32 _dt );
        bbool                   updateTeleport();
        bbool                   waitPrefetch() const;

        class Actor*            getNextNode( Actor* _currentNode ) const;
        void                    findCurrentPrefetch();
        void                    clearPrefetch();

        ITF_VECTOR <ActorInfo>          m_playerData;
        Vec3d                           m_start;
        f32                             m_speed;
        f32                             m_timer;
        SeqState                        m_state;
        ActorRef                        m_stargate;
        ActorRef                        m_currentNode;
        ActorRef                        m_currentPrefetch;
        bbool                           m_lastTrip;

        // Fading parameters
        bbool                           m_fadeToBlack;
        bbool                           m_fadeToGame;
        f32                             m_fadeDuration;
        f32                             m_fadeTimer;

        // Parameters from template
        f32                             m_waitForPlayersTime;
        f32                             m_waitDistFromDoor;
        f32                             m_waitAngleOffset;
		f32								m_waitPointZOffset;
        f32                             m_startToDoorTimeMultiplier;
        Vec3d                           m_cameraLookAtOffset;
    };

    class Ray_GameManagerSequenceScaleDoor : public Ray_GameManagerSequenceBase
    {
    public:
        Ray_GameManagerSequenceScaleDoor();
        virtual ~Ray_GameManagerSequenceScaleDoor();

        void					start( ActorRef _door, ActorRef _targetActor, f32 _scale, bbool _allPlayers, const Vec2d& _targetPos, f32 _waitDelay,
                                       f32 _moveK, f32 _moveD );
        virtual void            stop();
        virtual void            update( f32 _dt );

    private:

        enum SeqState
        {
            SEQSTATE_WAIT_ALLIN,
            SEQSTATE_WAIT_ALLOUT,
            SEQSTATE_DONE,
        };

        enum PlayerState
        {
            PLAYERSTATE_START_DELAY,
            PLAYERSTATE_GOING_IN,
            PLAYERSTATE_WAIT,
            PLAYERSTATE_GOING_OUT,
        };

        struct PlayerData
        {
            ActorRef                                m_actor;
            PlayerState                             m_state;
        };

        struct DoorSeq
        {
            SeqState                                m_state;
            ActorRef                                m_door;
            FixedArray <PlayerData,s_maxPlayers>    m_playerData;
            f32                                     m_timeCounter;
            Vec2d                                   m_endPoint;
            f32                                     m_scale;
            bbool                                   m_allPlayers;
        };

        void                    setPlayerGoIn( DoorSeq& _seq, ActorRef _player );
        void                    performScaleAndExit( DoorSeq& _seq );

        void                    updateDoorSeq( DoorSeq& _seq, f32 _dt );

        ITF_VECTOR <DoorSeq>                        m_doors;
        f32                                         m_moveK;
        f32                                         m_moveD;
    };

    class Ray_GameManagerSequenceBounceToLayer : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceBounceToLayer();
        virtual ~Ray_GameManagerSequenceBounceToLayer();

        void                                start( Actor* _player, ObjectRef _target, const Vec2d& _offset, f32 _targetOffsetDist, f32 _totalTime, f32 _waitAngleOffset );
        virtual void                        stop();
        virtual void                        update( f32 _dt );

    private:

        struct PlayerInfo
        {
            ActorRef        m_ref;
            Spline          m_spline;
            Vec3d           m_prevPos;
        };

        typedef FixedArray<PlayerInfo,s_maxPlayers> PlayerInfoList;

        void                                setupPlayer( PlayerInfo& _info, u32 _playerIndex, Actor* _playerActor, Actor* _playerTrigger );
        Vec3d                               getTargetPoint( u32 _playerIndex, Actor* _playerActor ) const;
        void                                restorePlayers();

        ObjectRef                           m_playerTrigger;
        ObjectRef                           m_target;
        Vec2d                               m_offset;
        f32                                 m_targetOffsetDist;
        f32                                 m_totalTime;
        f32                                 m_timeCounter;
        f32                                 m_waitAngleOffset;
        PlayerInfoList                      m_playerInfo;
    };

    class Ray_GameManagerSequenceStartTimeAttack : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceStartTimeAttack();
        virtual ~Ray_GameManagerSequenceStartTimeAttack();

        void                                start( f32 _totalTime );
        virtual void                        stop();
        virtual void                        update( f32 _dt );

    private:

        struct PlayerInfo
        {
            ActorRef        m_ref;
            Spline          m_spline;
            Vec3d           m_prevPos;
        };

        typedef FixedArray<PlayerInfo,s_maxPlayers> PlayerInfoList;

        void                                setupPlayer( PlayerInfo& _info, u32 _playerIndex, Actor* _playerActor );
        void                                restorePlayers();

        ObjectRef                           m_playerTrigger;
        f32                                 m_totalTime;
        f32                                 m_timeCounter;
        PlayerInfoList                      m_playerInfo;
        Vec3d                               m_destination;
    };

    class Ray_GameManagerSequenceChangeRoom : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceChangeRoom();
        ~Ray_GameManagerSequenceChangeRoom();

        void                        start( bbool _goIn, ObjectRef _doorRef );
        virtual void                stop();
        virtual bbool               isPlayerInSequence( ObjectRef _player ) const { return btrue; }
        virtual void                update( f32 _dt );

    private:

        enum State
        {
            STATE_FADE_BLACK,
            STATE_FADE_GAME,
            ENUM_FORCE_SIZE_32(State)
        };

        void                                updateFadeBlack( f32 _dt );
        void                                updateFadeToGame( f32 _dt );

        void                                freePrefetch();

        ObjectRef                           m_doorRef;
        State                               m_currentState;
        f32                                 m_prefetchFailSafe;
        class View*                         m_prefetchView;
        bbool                               m_goIn;
    };

    class Ray_GameManagerSequenceReturnTeeth : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceReturnTeeth();
        ~Ray_GameManagerSequenceReturnTeeth();

        void                        start( Player* _triggerer, Actor* _deadMan, const Vec3d& _cameraOffsetSurprise, const Vec3d& _cameraOffsetThrow, const Vec2d& _playerOffset );
        virtual void                stop();
        virtual bbool               isPlayerInSequence( ObjectRef _player ) const { return btrue; }
        virtual void                update( f32 _dt );

        void                        startThrow();
        void                        throwFinished();

    private:

        Vec3d                       getCameraPos() const;

        ActorRef                    m_deadMan;
        u32                         m_playerTrigger;
        u32                         m_teethReturning;
        Vec3d                       m_cameraOffsetSurprise;
        Vec3d                       m_cameraOffsetThrow;
        Vec2d                       m_playerOffset;
        bbool                       m_throwing;
    };

    class Ray_GameManagerSequenceFadeAndLoadMap : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceFadeAndLoadMap();
        ~Ray_GameManagerSequenceFadeAndLoadMap();

        void                        start( const StringID& _map );
        virtual void                stop();
        virtual bbool               isPlayerInSequence( ObjectRef _player ) const { return btrue; }
        virtual void                update( f32 _dt );

    private:

        StringID                    m_mapTagToLoad;
    };

    class Ray_GameManagerSequenceChangePageWithAnim : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceChangePageWithAnim();
        ~Ray_GameManagerSequenceChangePageWithAnim();

        void                        start( ActorRef _startPage, ActorRef _endPage, ActorRef _checkpoint, const StringID& _anim,
                                           const Vec3d& _targetPos );
        virtual void                stop();
        virtual bbool               isPlayerInSequence( ObjectRef _player ) const { return btrue; }
        virtual void                update( f32 _dt );

    private:

        enum Stage
        {
            STAGE_ENTER,
            STAGE_FADEBLACK,
            STAGE_FADEGAME,
        };

        struct PlayerTravelInfo
        {
            ActorRef	m_player;
            Spline		m_spline;
            f32			m_speed;
            Vec3d		m_moveDir;
            bbool		m_travelfinished;
            Vec2d		m_enterEjectForce;
            bbool		m_enterMovefinished;
            bbool       m_dead;
        };

        void					    initPlayerTravel( Player* _player, Actor* _actor, const Vec3d& _targetPoint, PlayerTravelInfo & _travelInfo );
        void					    updatePlayerTravel( f32 _dt, PlayerTravelInfo & _travelInfo );

        void                        setupPlayer( Player* _player );
        void                        setFinalMove();

        void                        updateEnter( f32 _dt );
        void                        updateFadeBlack( f32 _dt );
        void                        updateFadeGame( f32 _dt );

        void                        setPrefetch();
        bbool                       checkPrefetchFinished() const;
        void                        clearPrefetch();

        ActorRef                    m_startPage;
        ActorRef                    m_endPage;
        ActorRef                    m_checkpoint;
        Vec3d                       m_targetPos;
        StringID                    m_anim;
        Stage                       m_stage;

        ActorRef                    m_prefetchRef;
        class View*                 m_prefetchView;

        FixedArray <PlayerTravelInfo,s_maxPlayers> m_playersTravelInfo;
    };


    class Ray_GameManagerSequenceEndGame : public Ray_GameManagerSequenceBase, public IMoviePlayerListener
    {
    public:

        Ray_GameManagerSequenceEndGame()
            : m_stage(Stage_None)
            , m_playMovie(bfalse)
            , m_goToEndLevel(bfalse)
            , m_waitSpawnMedalFrame(10)
            , m_waitMedal(0.0f)
            , m_waitBeforeSkipVideo(1.0f)
            , m_playersTeleported(bfalse)
        {}
        ~Ray_GameManagerSequenceEndGame(){}

        void                        start( bbool _playMovie );
        virtual void                stop();
        virtual bbool               isPlayerInSequence( ObjectRef _player ) const { return btrue; }
        virtual void                update( f32 _dt );
        virtual void                onMovieStopped();
        virtual void                onMovieStarted();
        virtual void                onMoviePaused(){}
        virtual void                onFadeInEnded(){}
        void                        launchCredits();
        void                        skipMovie();
        bbool                       canPauseGame();

    private:

        void                        playMovie();
        void                        reinitPlayers();
        void                        preloadCredits();
        void                        switchToLevelEnding();
        void                        spawnMedal();


        ObjectRef m_creditsMap;
        ObjectRef m_worldMap;
        enum Stage
        {
            Stage_None,
            Stage_PlayMovie,
            Stage_WaitBeforeSpawnMedal,
            Stage_SpawnMedal,
            Stage_WaitingEndFade,
            Stage_WaitingLevelEnding,
            Stage_LevelEnding,
            Stage_WaitingCredits,
            Stage_Credits,
        };
        Stage m_stage;

        bbool       m_playMovie;
        bbool       m_goToEndLevel;

        MoviePlayer m_moviePlayer;
        ObjectRef	m_preloadedEndLevelMapRef;
        ObjectRef   m_preloadedCreditMapRef;

        u32         m_waitSpawnMedalFrame;
        f32         m_waitMedal;
        f32         m_waitBeforeSkipVideo;
        bbool       m_playersTeleported;
    };

    //--------------------------------------------------------------------------
    class Ray_GameManagerSequenceWorldMapUnlock : public Ray_GameManagerSequenceBase
    {
    public:
        Ray_GameManagerSequenceWorldMapUnlock();
        virtual ~Ray_GameManagerSequenceWorldMapUnlock();

        ITF_INLINE bbool isPending() const { return m_pending; }
        ITF_INLINE void setPending(bbool _val) { m_pending = _val; }

        ITF_INLINE bbool isDisplayingUnlocks() const { return m_displayingUnlocks; }

        virtual void start(const ActorRef& _nodeRef);
        virtual void update(f32 _dt);
        virtual void stop();

        void clearJustCompletedSpots();
        void setJustCompletedSpot(const StringID& _tag, bbool _bWorldOnly);
        bbool isJustCompletedSpot(const StringID& _tag) const;
        bbool isJustCompletedSpotBWorldOnly(const StringID& _tag) const;

    private:

        void startOpenPaths();

        enum Stage
        {
            Stage_NA = -1,
            Stage_Wait,
            Stage_ElectoonTooth,
            Stage_DisplayUnlocks,
            Stage_OpenPaths,
            Stage_Done,
        };

        struct CompletedSpot
        {
            CompletedSpot() : m_tag(), m_bWorldOnly(bfalse) {}
            StringID m_tag;
            bbool m_bWorldOnly;
        };

        bbool m_pending;
        Stage m_stage;
        ActorRef m_nodeRef;
        f32 m_waitCountdown;
        bbool m_displayingUnlocks;

        // just completed spots. start needs approx. 84
        static const u32 s_maxCompletedSpots = 128;
        typedef FixedArray<CompletedSpot, s_maxCompletedSpots> CompletedSpotList;
        CompletedSpotList m_justCompletedSpots;
    };

    //--------------------------------------------------------------------------
    class Ray_GameManagerSequenceWorldMapTeleport : public Ray_GameManagerSequenceBase
    {
    public:

        Ray_GameManagerSequenceWorldMapTeleport();
        ~Ray_GameManagerSequenceWorldMapTeleport();

        void start(const ActorRef& _spotFromRef, const ActorRef& _spotToRef);
        virtual void stop();
        virtual bbool isPlayerInSequence(ObjectRef _player) const { return btrue; }
        virtual void update(f32 _dt);

    private:

        enum State
        {
            STATE_FADE_BLACK,
            STATE_FADE_GAME,
            ENUM_FORCE_SIZE_32(State)
        };

        void updateFadeBlack(f32 _dt);
        void updateFadeToGame(f32 _dt);

        void freePrefetch();

        ActorRef m_spotFromRef;
        ActorRef m_spotToRef;
        State m_currentState;
        f32 m_prefetchFailSafe;
        class View* m_prefetchView;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_GameManagerConfig_Template : public GameManagerConfig_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GameManagerConfig_Template, GameManagerConfig_Template,2754889544);
        DECLARE_SERIALIZE()

    public:
        struct MedalSlotConfig
        {
            DECLARE_SERIALIZE()

            enum MedalSlotType {Cage=0,TimeAttack=1,LumAttack=2,Boss=3};
            MedalSlotConfig() : m_type(Cage), m_value(0), m_cupValue(0) {}

            MedalSlotType m_type;
            i32 m_value;
            i32 m_cupValue;
        };

        struct MapConfig
        {
            MapConfig()
                : m_tag(StringID::Invalid)
                , m_worldTag(StringID::Invalid)
                , m_type(StringID::Invalid)
                , m_music(StringID::Invalid)
                , m_powerUnlocked(StringID::Invalid)
                , m_path(Path::EmptyPath)
                , m_loadingScreen(Path::EmptyPath)
                , m_defaultDisplayName(String8::emptyString)
                , m_minTeeth(0)
                , m_minElectoons(0)
                , m_defaultState(SPOT_STATE_CLOSED)
                , m_lastLevel(bfalse)
                , m_isDefaultLevel(bfalse)
                , m_richPresenceIndex(0)
                , m_disableMenuToWM(bfalse)
                , m_lumAttack1(-1), m_lumAttack2(-1), m_lumAttack3(-1), m_timeAttack1(-1), m_timeAttack2(-1)
                , m_disableRewards(bfalse)
                , m_isSkippable(btrue)
                , m_maxRelics(0)
            {}

            DECLARE_SERIALIZE()
            StringID    m_tag;
            StringID    m_worldTag;
            StringID    m_type;
            StringID    m_music;
            StringID    m_powerUnlocked;
            ESpot_State m_defaultState;
            Path        m_path;
            Path        m_loadingScreen;
            String8     m_defaultDisplayName;
            LocalisationId         m_titleId;
            u32         m_minTeeth;
            u32         m_minElectoons;
            bbool       m_lastLevel;
            bbool       m_isDefaultLevel;
            Volume      m_musicVolume;
            i32         m_lumAttack1, m_lumAttack2, m_lumAttack3, m_timeAttack1, m_timeAttack2;
            u32         m_richPresenceIndex;
            bbool       m_disableMenuToWM;
            ITF_VECTOR<StringID>    m_unlock;
            ITF_VECTOR<StringID>    m_unlockedBy;
            bbool       m_disableRewards;
            bbool       m_isSkippable;
            u32         m_maxRelics;
        };

        struct MissionConfig
        {
            DECLARE_SERIALIZE()
            MissionConfig() :  m_lumAttack3(I32_MAX) {}

            ITF_VECTOR<MedalSlotConfig> m_medalSlots;
            StringID m_type;
            Path m_medalPath;
            i32 m_lumAttack3;
        };

        Ray_GameManagerConfig_Template();
        Ray_GameManagerConfig_Template(const Path& _path);

        ITF_INLINE ITF::u32 getMinHitPoints() const { return m_minHitPoints; }
        ITF_INLINE ITF::u32 getMaxHitPoints() const { return m_maxHitPoints; }
        ITF_INLINE ITF::u32 getStartHitPoints() const { return m_startHitPoints; }
        ITF_INLINE ITF::u32 getStartHitPointsAfterDeath() const { return m_startHitPointsAfterDeath; }
        ITF_INLINE ITF::u32 getStartMaxHitPoints() const { return m_startMaxHitPoints; }
        ITF_INLINE ITF::f32 getHiddenAreaWaitPlayersLength() const { return m_hiddenAreaWaitPlayersLength; }
        ITF_INLINE ITF::f32 getHiddenAreaWaitLoadLength() const { return m_hiddenAreaWaitLoadLength; }
        ITF_INLINE ITF::f32 getCheckpointSeqWaitPlayersLength() const { return m_checkpointSeqWaitPlayersLength; }
        ITF_INLINE const ITF::Path & getLumPath() const { return m_lumPath; }
        ITF_INLINE f32 getLumSpawnRadius() const { return m_lumSpawnRadius; }
        ITF_INLINE const ITF::Path & getHeartPath() const { return m_heartPath; }
        ITF_INLINE const ITF::Path & getDefaultLumMusicManagerPath() const {return m_defaultLumMusicManagerPath;}
        ITF_INLINE const Path & getElectoonToothScore() const {return m_electoonToothScore;}

        // Stargate parameters
        ITF_INLINE f32      getStargateWaitForPlayersTime() const { return m_stargateWaitForPlayersTime; }
        ITF_INLINE f32      getStargateWaitDistFromDoor() const { return m_stargateWaitDistFromDoor; }
        ITF_INLINE f32      getStargateWaitAngleOffset() const { return m_stargateWaitAngleOffset; }
        ITF_INLINE f32      getStargateStartToDoorTimeMultiplier() const { return m_stargateStartToDoorTimeMultiplier; }
		ITF_INLINE f32      getStargateWaitPointZOffset() const { return m_stargateWaitPointZOffset; }
        ITF_INLINE Vec3d    getStargateCameraLookAtOffset() const { return m_stargateCameraLookAtOffset; }

        // Scale door parameters
        ITF_INLINE f32      getScaleDoorWaitDelay() const { return m_scaleDoorWaitDelay;  }
        ITF_INLINE f32      getScaleDoorMoveK() const { return m_scaleDoorMoveK; }
        ITF_INLINE f32      getScaleDoorMoveD() const { return m_scaleDoorMoveD; }

        ITF_INLINE f32      getBounceToLayerOffsetDistFromTarget() const { return m_bounceToLayerOffsetDistFromTarget; }
        ITF_INLINE f32      getBounceToLayerAngleOffset() const { return m_bounceToLayerAngleOffset; }

        // Throw teeth parameters
        ITF_INLINE Vec3d    getThrowTeethSurpriseCameraOffset() const { return m_throwTeethSurpriseCameraOffset; }
        ITF_INLINE Vec3d    getThrowTeethThrowCameraOffset() const { return m_throwTeethThrowCameraOffset; }
        ITF_INLINE Vec2d    getThrowTeethPlayerOffset() const { return m_throwTeethPlayerOffset; }

        ITF_INLINE bbool    getDebugSaves() const { return m_debugSaves; }

        // Skip
        ITF_INLINE const u32    getNbDeathBeforeFirstSkip() const { return m_nbDeathBeforeFirstSkip; }
        ITF_INLINE const u32    getNbDeathBeforeSecondSkip() const { return m_nbDeathBeforeSecondSkip; }

        // Level info
        const Path&         getLevelPath(const StringID& _levelTag) const;
        const StringID&     getLevelTag(const Path& _path) const;
        const struct MapConfig* getMapConfig(const StringID& _levelTag) const;
        const StringID&     getMapConfigType(const StringID& _levelTag) const;
        const StringID&     getMapConfigMusic(const StringID& _levelTag) const;
        const String8&      getDefaultDisplayName(const StringID& _levelTag) const;
        const LocalisationId           getTitleId(const StringID& _levelTag) const;
        const u32           getMinTeeth(const StringID& _levelTag) const;
        const u32           getMinElectoons(const StringID& _levelTag) const;
        const u32           getMaxRelics(const StringID& _levelTag) const;
        ESpot_State         getDefaultState(const StringID& _levelTag) const;
        const bbool         isLastLevel(const StringID& _levelTag) const;
        const StringID&     getDefaultLevelTag() const;
        const f32           getMusicVolume(const StringID& _levelTag) const;
        const Path&         getLoadingScreen(const StringID& _tag) const;
        void                getLoadingScreens( ITF_VECTOR< String >& _screens ) const;
        const bbool         isOfficialMap(const Path& _map) const;
        const StringID&     getLevelWorldTag(const StringID& _levelTag) const;
        const bbool         isSkippable(const StringID& _levelTag) const;
        const ITF_MAP< StringID,ITF_VECTOR<StringID> >& getUnlockedByList() const { return m_unlockedByList; }

        const Ray_PowerUpManager_Template& getPowerUpManagerData() const { return m_powerUpManager; }

        const ITF_VECTOR<MissionConfig> &getMissionConfigs() const {return m_missionTypes;}

        // sprint tutorial
        ITF_INLINE u32 getSprintTutorialFailureCount() const { return m_sprintTutorialFailureCount; }
        ITF_INLINE f32 getSprintTutorialRequiredDuration() const { return m_sprintTutorialRequiredDuration; }

        void getMissionMaps( ITF_VECTOR< String >& maps ) const;

        const ITF_VECTOR<MapConfig>& getLevelsInfo() const { return m_levelsInfo; }

        const StringID& getFadeTypeDeath() const { return m_fadeDeath; }
        const StringID& getFadeTypeChangePage() const { return m_fadeChangePage; }
        const StringID& getFadeTypeHiddenArea() const { return m_fadeHiddenArea; }
        const StringID& getFadeTypeTeleport() const { return m_fadeTeleport; }
        const StringID& getFadeTypeCostume() const { return m_fadeCostume; }
        const StringID& getFadeTypeLoadMap() const { return m_fadeLoadMap; }
        const StringID& getFadeTypeChangePageWithAnim() const { return m_fadeChangePageWithAnim; }
        const StringID& getFadeTypeWorldMapTeleport() const { return m_fadeWorldMapTeleport; }
        const StringID& getFadeTypeFlash() const { return m_fadeFlash; }
        const StringID& getFadeTypePrologue() const { return m_fadePrologue; }

        const ObjectPath& getWmStartNode() const { return m_wmStartNode; }

        u32 getLoadingMinFrames() const { return m_loadingMinFrames; }

        const Vec3d    getEndSequencePlayerPosition() const { return m_endSequencePlayerPosition; }

        //Tarik::Fresco
        const ITF_VECTOR<Vec2d>& getFrescoGamePlayProgressionInfo() const { return m_frescoGamePlayProgressionInfo; }

    private:

        void fillLevelsInfo();
#ifndef ITF_FINAL
        void checkUnlockConsistency();
#endif //ITF_FINAL

        u32 m_minHitPoints;
        u32 m_maxHitPoints;
        u32 m_startHitPoints;
        u32 m_startHitPointsAfterDeath;
        u32 m_startMaxHitPoints;
        f32 m_hiddenAreaWaitPlayersLength;
        f32 m_hiddenAreaWaitLoadLength;
        f32 m_checkpointSeqWaitPlayersLength;
        Path m_lumPath;
        f32 m_lumSpawnRadius;
        Path m_heartPath;
        Path m_defaultLumMusicManagerPath;
        Path m_electoonToothScore;

        // Stargate parameters
        f32  m_stargateWaitForPlayersTime;
        f32  m_stargateWaitDistFromDoor;
        f32  m_stargateWaitAngleOffset;
        f32  m_stargateStartToDoorTimeMultiplier;
		f32	 m_stargateWaitPointZOffset;
        Vec3d m_stargateCameraLookAtOffset;

        // Scale door parameters
        f32  m_scaleDoorWaitDelay;
        f32  m_scaleDoorMoveK;
        f32  m_scaleDoorMoveD;

        // Bounce to layer parameters
        f32  m_bounceToLayerOffsetDistFromTarget;
        f32  m_bounceToLayerAngleOffset;

        // Throw teeth surprise camera offset
        Vec3d m_throwTeethSurpriseCameraOffset;
        Vec3d m_throwTeethThrowCameraOffset;
        Vec2d m_throwTeethPlayerOffset;

		// WorldMap
        ITF_VECTOR<MapConfig>                   m_levelsInfo;
        ITF_VECTOR<MissionConfig>               m_missionTypes;
        ITF_MAP<StringID,u32>                   m_levelsInfoMap;
        ITF_MAP< StringID,ITF_VECTOR<StringID> >  m_unlockedByList;

        Ray_PowerUpManager_Template     m_powerUpManager;

        // Saves
        bbool m_debugSaves;

        // sprint tutorial
        u32 m_sprintTutorialFailureCount;
        f32 m_sprintTutorialRequiredDuration;

        // Skip
        u32                     m_nbDeathBeforeFirstSkip;
        u32                     m_nbDeathBeforeSecondSkip;

        // Fade types
        StringID                m_fadeDeath;
        StringID                m_fadeChangePage;
        StringID                m_fadeHiddenArea;
        StringID                m_fadeTeleport;
        StringID                m_fadeCostume;
        StringID                m_fadeLoadMap;
        StringID                m_fadeChangePageWithAnim;
        StringID                m_fadeWorldMapTeleport;
        StringID                m_fadeFlash;
        StringID                m_fadePrologue;

        // initial node for new game
        ObjectPath              m_wmStartNode;

        //Loading screen
        u32                     m_loadingMinFrames;

        // End sequence : position to teleport player during end movie
        Vec3d                   m_endSequencePlayerPosition;

        //Tarik::Fresco
        ITF_VECTOR<Vec2d>       m_frescoGamePlayProgressionInfo;

    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_GameManager : public GameManager
    {
        DECLARE_SERIALIZE()

    public:
        typedef GameManager Super;
        Ray_GameManager();
        ~Ray_GameManager() {}
        virtual void destroyInternal();

        static void                 create()    { s_instance = newAlloc(mId_Singleton, Ray_GameManager()); }
        static Ray_GameManager*     get()       { return static_cast<Ray_GameManager*>(s_instance); }

        virtual void    init();
        virtual Player* allocPlayer() { return newAlloc(mId_Gameplay, Ray_Player); }
        virtual void    update( f32 _dt );

        void            updateDisconnectedPlayersStatus();
        void            updateBubblizerAchievement();

        virtual void    postLoadInit();
        bbool           isConfigurationLoaded() const {return m_wasPostLoadInitCalled && GameManager::isConfigurationLoaded();}
        void            unloadGameResources();

        virtual void    onReachedCheckpoint(ObjectRef _checkpoint, bbool _force = bfalse);
        virtual void    onEvent(Event* _event);
        virtual void    onMapLoaded(World* _pWorld);
        virtual void    onSceneAsyncLoadFinished(const Scene* _pScene);

        Vec2d			computeRewardEjectionForce(Ray_EventSpawnReward *_onSpawn);

        void            SpawnRewardLum( class Ray_EventSpawnRewardLum * onSpawnReward );
        void            SpawnRewardHeart( class Ray_EventSpawnRewardHeart * onSpawnReward );
        void            setSkipLevelActor(ObjectRef _ref);
        ActorRef        getSkipLevelActor() const { return m_skipLevelActor; }
        SpawneeGenerator &getLumGenerator() {return m_lumGenerator;}

        virtual void    emptyCurrentLevelGameState();
        virtual void    clearWholeGameState();
        virtual bbool   loadGameState(const u8 *_data, u32 _dataSize);
        virtual bbool   saveGameState(bbool _newGame, u32 _slotIndex=0, bbool _resetSave = bfalse);
        void            getSaveDisplayedGameName(String & _gameName );
        void            getSaveEmptyDisplayedName(String & _gameName );
        void            getSaveBaseName(String8 & _baseName, u32 _slot );
        bbool           getDebugSaves() const { return m_configTemplate->getDebugSaves(); }
        u32             computeSaveCodeCRC();
        u32             computeSaveDataCRC(const u8* _data, u32 _dataSize);
        virtual void    checkpointLoad();
        virtual void    checkpointLoadScene();
        virtual void    makeLevelStateFromPersistentUniverse();
        virtual void    commitLevelStateToPersistentUniverse();
        virtual void    commitFrescoStateToPersistentUniverse();
        virtual void    saveCheckpointLevelState();
        virtual void    loadCheckpointLevelState();
        ObjectRef       getMedalWorldRef() const {return m_medalWorldRef;}
        virtual void    onPlayerDisconnected(u32 _padIndex);
        virtual void    onPreloadedAsyncFinished(World * _world);
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Call this methode to trigger end of level
        void            endOfLevelReached( bbool _saveGameState = btrue, bbool _levelCompleted = btrue );

        void            commitAndSaveGameState();

        ///////////////////////////////////////////////////////////////////////////////////////////
        // End level map : lum attack sequences
        ObjectRef       preloadEndLevelMap();
        void            switchToPreloadedEndLevelMap();

        ObjectRef       preloadWorldMap();

        void            resetToWorldMap(bbool _fade = btrue);
        virtual bbool   loadGameplayMap(const Path &_map, bbool _unloadBaseMap, bbool _isReload, ObjectRef _worldRef = ObjectRef::InvalidRef);

        virtual void    createMainMenuWorld();

        void            mapLoadResetInfo();

        virtual bbool   switchToGameScreenGameplay(ObjectRef _mapRef);
        const Path&     getDefaultLumMusicManagerPath() const {return m_configTemplate->getDefaultLumMusicManagerPath();}

        virtual u32     getSavegameMaxSize();
        void            goToTheMainMenu();

        void            goToCheckpointScore();
        void            goToLevelStats();

        virtual u32     getMaxPlayerCount() const { return s_maxPlayers; }

        // SCORING

        // hitPoints are per-player
        void            addHitPoints(u32 _playerIndex, i32 _numHP, bbool _evenIfInvincible, bbool _broadcastEvent);
        ITF_INLINE u32  getMinHitPoints() const { return m_configTemplate->getMinHitPoints(); }
        ITF_INLINE u32  getMaxHitPoints() const { return m_configTemplate->getMaxHitPoints(); }
        ITF_INLINE u32  getStartHitPoints() const { return m_configTemplate->getStartHitPoints(); }
        ITF_INLINE u32  getStartHitPointsAfterDeath() const { return m_configTemplate->getStartHitPointsAfterDeath(); }
        ITF_INLINE u32  getStartMaxHitPoints() const { return m_configTemplate->getStartMaxHitPoints(); }

        //returns the number of active and dead players
        u32             getNumDeadPlayers() const;
        bbool           getIsAnyPlayerAlive() const;
        bbool           canOptInOptOut() const;

        // does any player needs HP?
        bbool           heartNeeded();


        // get sum of lums for all players
        u32             getTotalLums() const { return m_totalLums; }
        // get sum of tuns for all players
        u32             getTotalTuns() const { return m_totalTuns; }

        void            pickupLums(Actor *_obj, bbool _pickedUp);
        bbool           isLumPickedUp(Actor *_obj) const;


        void            addThuns(u32 _val) { m_totalTuns += _val;}

        //////////////////////////////////////////////////////////////////////////////////////////
        // Persistent information (cages, skull coins, ...)
        u32             getBrokenCageCount() const;
        u32             getReturnedTeethCount() const;
        void            returnTeeth();
        bbool           hasAllTeethBeenReturned() const;
        void            setCageMapPassedDoor(const PackedObjectPath &_packedObjectPath, bbool _passed);
        bbool           didPassDoorToCageMap(const PackedObjectPath &_packedObjectPath);
        void            setIsCutSceneAlreadySeen(Actor *_obj, bbool _alreadySeen);
        bbool           getIsCutSceneAlreadySeen(Actor *_obj) const;
        void            setIsCageBroken(Actor *_obj, bbool _state);
        bbool           getIsCageBroken(Actor *_obj) const;
        bbool           getIsCageBroken(u32 _cageIndex) const;
        void            setIsToothTaken(Actor *_obj, bbool _state);
        bbool           getIsToothTaken(Actor *_obj) const;
        Ray_PersistentGameData_Level* getCurrentLevelData()  {return &m_currentLevelData;}
        Ray_PersistentGameData_Level* getLevelData(const StringID& _levelName);

        u32             getCurrentRelicFoundMask() const;
        bbool           getIsRelicTaken(u32 _index) const { return getCurrentRelicFoundMask() & (1<<_index);}
        void            setCurrentLevelIsRelicTaken(u32 _index, bbool _taken);
        bbool           getIsRelicTaken(const StringID& _tag, u32 _index);
        void            setIsRelicTaken(const StringID& _tag, u32 _index, bbool _taken);
        u32				getTakenRelicCount(const StringID& _levelTag);


        void            setChallengeWon(EChallengeType _challenge);
        bbool           isChallengeWon(EChallengeType _challenge);
        bbool           isChallengeWon(const StringID& _levelName, EChallengeType _challenge);

        //////////////////////////////////////////////////////////////////////////////////////////
        /// Time attack accessors
        bbool           isTimeAttack() const {return m_isTimeAttack;}
        void            setIsTimeAttack(bbool _isTimeAttack);
        void            setTimeAttackActorTimer( ObjectRef _ref ) { m_timeAttackActorTimer = _ref; }
        ActorRef        getTimeAttackActorTimer() const { return m_timeAttackActorTimer; }
        void            setTimeAttackActorFlag( ObjectRef _ref ) { m_timeAttackActorFlag = _ref; }
        ActorRef        getTimeAttackActorFlag() const { return m_timeAttackActorFlag; }
        void            setTimeAttackActorResults( ObjectRef _ref ) { m_timeAttackActorResults = _ref; }
        ActorRef        getTimeAttackActorResults() const { return m_timeAttackActorResults; }
        void            setTimeAttackActorChrono( ObjectRef _ref ) { m_timeAttackActorChrono = _ref; }
        ActorRef        getTimeAttackActorChrono() const { return m_timeAttackActorChrono; }
        void            setTimeAttackFinished() { ITF_ASSERT(m_isTimeAttack); m_isTimeAttackFinished = btrue; }
        bbool           getTimeAttackFinished() const { return m_isTimeAttackFinished; }
        void            setTriggerTimeAttackSequence();
        f32             getTimeAttackTimer() const { return m_timeAttackTimer; }
        void            disableTimeAttackForLevel();
        void            disableTimeAttackHUD();

        // Unlock messages
        void            setWMUnlockMessageActor( ObjectRef _ref ) { m_worldMapUnlockMsgActor = _ref; }
        ActorRef        getWMUnlockMessageActor() const { return m_worldMapUnlockMsgActor; }

        // Load notification icon
        void            setLoadNotificationActor( ActorRef _ref ) { m_loadNotificationActor = _ref; }
        ActorRef        getLoadNotificationActor() const { return m_loadNotificationActor; }
        void            showLoadNotificationActor();
        void            hideLoadNotificationActor();

        // Fade
        void            setUIFadeActorRef( ActorRef _ref ) { m_uiFadeActorRef = _ref; }
        void            startFade( const StringID& _type , bbool _force = bfalse);
        bbool           isFadeOutFinished() const;
        bbool           isFadeInFinished() const;
        void            stopFade( bbool _force );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// enable/disable medal world
        /// @param _enable
        void            enableMedalWorld(bbool _enable);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get/set medal actors
        void            setMedalActor(Actor *_actor);
        Actor*          getMedalActor() const;


        bbool           isCurrentWorldPaused();

        ITF_INLINE u32  getCurrentSlotIndex() const { return m_currentSlotIndex; }
        ITF_INLINE void setCurrentSlotIndex(u32 _slotIndex) { m_currentSlotIndex = _slotIndex; }

        // WorldMap
        const ObjectPath    getWMCurrentWorldObjectPath();
        const StringID&     getWMCurrentWorldTag();
        void                setWMCurrentWorld(const ObjectPath& _path, const StringID& _tag);
        const ObjectPath    getWMCurrentLevelObjectPath();
        const StringID&     getWMCurrentLevelTag();
        void                setCurrentLevelName(const Path& _map);
        void                setWMCurrentLevel(const ObjectPath& _path, const StringID& _tag);
        ESpot_State         getWMSpotState(const StringID& _tag);
        void                setWMSpotState(const StringID& _tag, const ESpot_State _state);
        u32                 getWMLastElectoonCount() const; //NB : U32_INVALID IF NONE
        u32                 getWMLastToothCount() const;//NB : U32_INVALID IF NONE
        void                setWMLastElectoonCount(u32 _count);
        void                setWMLastToothCount(u32 _count);
        bbool               getWMUnlockRelicMessageIsDisplayed() const;
        void                setWMUnlockRelicMessageIsDisplayed(bbool _isUnlockRelicMessageDisplayed);
        void                setWMSpotHasWarning(const StringID& _tag, const bbool _hasWarning);
        bbool               getWMSpotHasWarning(const StringID& _tag);
        void                updateWMWorldHasWarning(const StringID& _worldTag);
        void                setCurrentLevelIsSkipped( const bbool _isSkipped);
        void                setWMLevelIsSkipped(const StringID& _tag, const bbool _isSkipped);
        bbool               getWMLevelIsSkipped(const StringID& _levelTag);
        void                skipCurrentLevel();
        bbool               isLevelCompletedOnce(const StringID& _tag);

        void                resetSkipData();
        void                resetSkipCounter(ObjectRef _newCheckpoint, bbool _forceReset = bfalse);
        void                increaseSkipCounter();
        const u32           getDeathCounterOnSameCheckpoint() const { return m_deathCounterOnSameCheckpoint; }
        const u32           getCurrentDeathMaxCount() const;

        void                setCurrentLevelState( const ESpot_State _state );
        ESpot_State         getCurrentLevelState();

        void                setCurrentLevelTimeAttackTime( u32 _time );
        u32                 getCurrentLevelTimeAttackTime() const;

        void                setCurrentLevelBestLumAttack(u32 _lumCount);
        u32                 getCurrentLevelBestLumAttack() const;

        const ObjectPath    getWMSpotObjectPath(const StringID& _tag);
        void                setWMSpotObjectPath(const StringID& _tag, const ObjectPath& _path);

        Actor*              getWMSpot(const StringID& _tag);

        bbool               isJustCompletedSpot(const StringID& _tag) const;
        bbool               isJustCompletedSpotBWorldOnly(const StringID& _tag) const;

        void                markWorldMapNodesAsCompleted( const ITF_VECTOR<StringID>& _unlocks );

        u32                 getLevelBrokenCageCount(const StringID& _tag);
        u32                 getLevelToothCount(const StringID& _tag);
        u32                 getLevelLumCount(const StringID& _tag);
        u32                 getLevelBestTime(const StringID& _tag); //returns U32_INVALID if not defined
        u32                 getLevelBestLumAttack(const StringID& _tag); //returns 0 if not defined

        void                setWMLastLevelTag(StringID _tag) { m_lastWMLevelTag = _tag;}
        StringID            getWMLastLevelTag() const {return m_lastWMLevelTag; }

        // RULES

        // can players punch each other?
        bbool   getAllowFriendlyFire() const { return m_allowFriendlyFire; }
        void    setAllowFriendlyFire(bbool _value) { m_allowFriendlyFire = _value; }

        // can players jump on top of each other?
        bbool   getAllowPlayerCrush() const { return m_allowPlayerCrush; }
        void    setAllowPlayerCrush(bbool _value) { m_allowPlayerCrush = _value; }

        // can players jump on top of AIs?
        bbool   getAllowNpcCrush() const { return m_allowNpcCrush; }
        void    setAllowNpcCrush(bbool _value) { m_allowNpcCrush = _value; }

        // Sequences
        void    stopAllSequences();
        virtual void onDisableSequencePlayers(){ stopAllSequences(); }
        void    stopTimeAttack();
        void    onLoadMapReached( const ObjectPath& _path, bbool _victory, bbool _goToWorldMap );
        void    onHiddenAreaReached( ObjectRef _start, ObjectRef _end, ObjectRef _player );
        void    triggerRestoreDeath( ObjectRef _player );
        void    playSeqActor( const Path& _act, const Vec3d& _pos, Scene* _scene );
        void    triggerCheckpointSequence( ObjectRef _checkpoint, ObjectRef _player, const Path& _sequence );
        void    resumeCheckpointSequence();
		void	triggerChangePageSequence( Vec3d const & _enterPoint, Vec3d const & _exitPoint, Vec3d const & _finalPoint, bbool _verticalEjectToFinalPoint,
                                           ObjectRef _player, ActorRef _teleportPage, ActorRef _destinationPage, ActorRef _checkpoint,
                                           f32 _waitPlayersDuration /*= 0.f*/, bbool _useFade /*= bfalse*/, bbool _isCageMap /*= bfalse*/ );
        void    triggerBounceToLayer( Actor* _player, ObjectRef _target, const Vec2d& _offset, f32 _totalTime );
        void    triggerStartTimeAttackSequence( f32 _duration );
        void    triggerStargateSequence( ObjectRef _stargate, const Vec3d& _start, f32 _speed );
        void    triggerScaleDoorSequence( ActorRef _door, ActorRef _targetActor, f32 _scale, bbool _allPlayers, const Vec2d& _targetPos );
        void    triggerTeleportCheatSequence( GameManager::teleportTarget _target );
        void    triggerChangeRoomSequence( bbool _on );
        void    triggerReturnTeeth( Player* _player, Actor* _deadMan );
        void    triggerFadeAndLoadMap( const StringID& _pathTag );
        void    returnTeethThrowStart();
        void    returnTeethThrowFinished();
        void    triggerChangePageWithAnim( ActorRef _startPage, ActorRef _endPage, ActorRef _checkpoint, const StringID& _anim, const Vec3d& _targetPos );
        void    triggerEndGameSequence(bbool _playMovie);
        void    onRestartLevelDuringEndSequence();
        bbool   launchCredits();
        bbool   skipEndMovie();
        bbool   isRestoreDeathPlaying() const { return isPlayingSeq(&m_restoreDeath) != U32_INVALID; }
        bbool   isEndSequencePlaying() const { return isPlayingSeq(&m_endGameSequence) != U32_INVALID; }
        bbool   isRestoreDeathSequenceCanPause()  { bbool res = btrue; if (isPlayingSeq(&m_restoreDeath) != U32_INVALID) res = m_restoreDeath.canPause();  return res;}
        bbool   canPauseDuringEndSequence();
        bbool   canPauseWorldMap();

        void    skipLevelRequested(bbool _skip);

        void    triggerFxScreen(class Ray_EventFxScreen* _onEventFxScreen);
        void    triggerWorldMapUnlockSequence(const ActorRef& _nodeRef);
        bbool   isPendingWorldMapUnlockSequence() { return m_worldMapUnlockSequence.isPending(); }
        void    setPendingWorldMapUnlockSequence(bbool _val) { return m_worldMapUnlockSequence.setPending(_val); }
        bbool   isPlayingWorldMapUnlockSequence() { return isPlayingSeq(&m_worldMapUnlockSequence) != U32_INVALID; }
        bool    isPlayingWorldMapUnlockSequenceAndDisplayingUnlocks() { return isPlayingWorldMapUnlockSequence() && m_worldMapUnlockSequence.isDisplayingUnlocks(); }
        void    triggerWorldMapTeleportSequence(const ActorRef& _spotFromRef, const ActorRef& _spotToRef);
        void    setChangeRoomDoor( ObjectRef _ref ) { m_changeRoomDoor = _ref; }
        void    resetChangeRoomDoor( ObjectRef _ref ) { if ( m_changeRoomDoor == _ref ) m_changeRoomDoor = ITF_INVALID_OBJREF; }
        void    setCostumeManager( ObjectRef _ref ) { m_costumeManager = _ref; }
        void    resetCostumeManager( ObjectRef _ref ) { if ( m_costumeManager == _ref ) m_costumeManager = ITF_INVALID_OBJREF; }
        ObjectRef getCostumeManager() const { return m_costumeManager; }
        void    changePlayerIdInfoForPlayer( Player* _player, const PlayerIDInfo* _newPlayerId );
        bbool   hasCostumeBeenUsed( const StringID& _id ) const;
        bbool   isPlayerInSequence( ObjectRef _player ) const;
        bbool   isSequencePlaying() const {return m_currentSequences.size()!=0;}

        const Vec3d&  getChangePageSequenceFinalPoint() const { return m_changePageSequence.getTravelFinalPoint(); }
        ObjectRef     getChangePageTriggerer() const { return m_changePageSequence.getTriggerer(); }

        ITF_INLINE  bbool   isPlayingPageSequence()         { return ( isPlayingSeq(&m_changePageSequence) != U32_INVALID ); }
        ITF_INLINE  bbool   isPlayingBounceToLayerSequence(){ return ( isPlayingSeq(&m_bounceToLayerSequence) != U32_INVALID ); }

        // FX
        void    fxFade( const Color& _from, const Color& _to, f32 _duration, bbool _removeOnFinished );
        bbool   isFxFinished() { return m_fxHandler.isFinished(); }
        virtual bbool postGameScreenChange_String(const StringID &_screen, bbool _evenIfOtherQueued, GameScreenBase::InitializationParameters* _data = NULL );
        // HUD
        void    setHideJoinMessage( bbool _hide);
        bbool   hideJoinMessage() { return m_hideJoinMessage > 0;}

        void    requestGameplayPause() {m_requestedGameplayPause = GameplayPause_Pause;}
        void    requestGameplayUnpause() {m_requestedGameplayPause = GameplayPause_Unpause;}

        virtual void    enterPause(bbool _bShowMenu = true);
        virtual void    leavePause(bbool _bHideMenu = true);

        bbool   isInGameplayPause() const { return (m_pauseType & PAUSEFLAG_GAMEPLAY);}
        bbool   isInInGamePause() const { return (m_pauseType & PAUSEFLAG_INGAME);}

        virtual void    changePlayerActivation( u32 _playerIndex, bbool _startLevel );

        void showRewards();

        void            forceInputConfigReset(u32 _padIndex);

        // Friendlies rescued
        void                                    addFriendlyRescued( ObjectRef _ref, bbool _teleport );
        void                                    teleportRescuedFriendlies( class Ray_EventTeleportUsed* _event );
        void                                    storeRescuedFriendlies();
        void                                    resetRescuedFriendlies();
        void                                    resetRescuedFriendlyTeleport( ObjectRef _ref );

        u32                                 getMaxPlayers() { return m_maxPlayers; }
        void                                setMaxPlayers( u32 _maxPlayers ) { m_maxPlayers = _maxPlayers; }

        ITF_INLINE Ray_ShooterGameModeController * getShooterGameModeController()   { return static_cast< Ray_ShooterGameModeController *>( getGameModeController(RAY_GAMEMODE_SHOOTER ) ); }
        ITF_INLINE Ray_WorldMapGameModeController * getWorldMapGameModeController() { return static_cast< Ray_WorldMapGameModeController *>( getGameModeController(RAY_GAMEMODE_WORLDMAP ) ); }

        ITF_INLINE Ray_PowerUpManager& getPowerUpManager() { return m_powerUpManager; }
        ITF_INLINE f32                 getPlayersCurrentScale() const { return m_playersCurrentScale; }
        ITF_INLINE void                setPlayersCurrentScale( f32 _scale ) { m_playersCurrentScale = _scale; }

        void                           resetPlayerGlobalPowers();

        ITF_INLINE Ray_RewardManager*  getRewardManager() { return &m_rewardManager; }
#ifdef ITF_SUPPORT_ONLINETRACKING
		ITF_INLINE Ray_OnlineTrackingManager* getOnlineTrackingManager() { return &m_onlineTrackingManager;};
#endif // ITF_SUPPORT_ONLINETRACKING
        ITF_INLINE Ray_PersistentGameData_Score& getCurrentScore() { return m_currentScore; }
        ITF_INLINE Ray_PersistentGameData_Universe* getPersistentGameData() { return static_cast<Ray_PersistentGameData_Universe*> (m_persistentGameData); }
        ITF_INLINE const Ray_PersistentGameData_Universe* getPersistentGameData() const { return static_cast<const Ray_PersistentGameData_Universe*> (m_persistentGameData); }

        // enumeration fonctions for broken cages, teeth, electoons, completed medals
        ITF_INLINE u32 getTotalBrokenCageCount() { return static_cast<Ray_PersistentGameData_Universe*>(m_persistentGameData)->getBrokenCageCount(); }
        u32 getTotalTakenToothCount() const;
        u32 getElectoonsCount() const;
        u32 computeCompleteMedalCount() const;

        //Accessors for level info
        ITF_INLINE const StringID&  getMapConfigTagByPath(const Path &_path) const {return m_configTemplate->getLevelTag(_path);}
        ITF_INLINE const Path&      getLevelPath(const StringID& _levelTag) const { return m_configTemplate->getLevelPath(_levelTag); }
        ITF_INLINE const Ray_GameManagerConfig_Template::MapConfig* getMapConfig(const StringID& _levelTag) const {return m_configTemplate->getMapConfig(_levelTag);}
        ITF_INLINE const StringID&  getMapConfigType(const StringID& _levelTag) const { return m_configTemplate->getMapConfigType(_levelTag); }
        ITF_INLINE const StringID&  getMapConfigMusic(const StringID& _levelTag) const { return m_configTemplate->getMapConfigMusic(_levelTag); }
        ITF_INLINE const String8&   getDefaultDisplayName(const StringID& _levelTag) const { return m_configTemplate->getDefaultDisplayName(_levelTag); }
        ITF_INLINE const LocalisationId        getTitleId(const StringID& _levelTag) const { return m_configTemplate->getTitleId(_levelTag); }
        ITF_INLINE const u32        getMinTeeth(const StringID& _levelTag) const { return m_configTemplate->getMinTeeth(_levelTag); }
        ITF_INLINE const u32        getMinElectoons(const StringID& _levelTag) const { return m_configTemplate->getMinElectoons(_levelTag); }
        ITF_INLINE const u32        getMaxRelics(const StringID& _levelTag) const { return m_configTemplate->getMaxRelics(_levelTag); }
        ITF_INLINE ESpot_State      getDefaultState(const StringID& _levelTag) const { return m_configTemplate->getDefaultState(_levelTag); }
        ITF_INLINE const bbool      isLastLevel(const StringID& _levelTag) const { return m_configTemplate->isLastLevel(_levelTag); }
        ITF_INLINE const StringID&  getDefaultLevelTag() const { return m_configTemplate->getDefaultLevelTag(); }
        const bbool                 isStillUnderLockCondition(const StringID& _tag);
        ITF_INLINE const f32        getMusicVolume(const StringID& _levelTag) const { return m_configTemplate->getMusicVolume(_levelTag); }
        ITF_INLINE void             getMissionMaps( ITF_VECTOR< String >& maps ) const { m_configTemplate->getMissionMaps( maps ); }
        ITF_INLINE void             getLoadingScreens( ITF_VECTOR< String >& screens ) const { m_configTemplate->getLoadingScreens( screens ); }
        ITF_INLINE const Path&      getLoadingScreen( const StringID& _tag ) const { return m_configTemplate->getLoadingScreen(_tag); }
        ITF_INLINE const StringID&  getLevelWorldOwner( const StringID& _LevelTag ) const { return m_configTemplate->getLevelWorldTag(_LevelTag); }
        ITF_INLINE const bbool      isLevelSkippable(const StringID& _levelTag) const { return m_configTemplate->isSkippable(_levelTag); }
        ITF_INLINE void             setHasRejectedSkip(bbool _rejected) { m_hasRejectedSkip = _rejected; }
        ITF_INLINE const bbool      getHasRejectedSkip() const { return m_hasRejectedSkip; }
        void                        fillWorldMapWorldsList();
        void                        getLevelTagsFromWorld( const StringID& _worldTag, ITF_VECTOR<StringID>& _levelTags );
        void                        getWorldsWithWarnings(ITF_VECTOR<StringID>& _worlds);
        bbool                       hasNewCostumeAvailable();
        void                        onWorldMapElectoonsEarned();

        // Gamescreen stuff
        ITF_INLINE ObjectRef        getGameScreenPreloadedMap() { return m_theGameScreen->getPreLoadedMap(); }
        ITF_INLINE void             setGameScreenPreloadedMap(ObjectRef _mapRef) { return m_theGameScreen->setPreLoadedMap(_mapRef); }

        ITF_INLINE const Path&      getWorldMap() const           { return getLevelPath(ITF_GET_STRINGID_CRC(WorldMap,1734041925)); }
        ITF_INLINE const Path&      getEndLevelMap() const        { return getLevelPath(ITF_GET_STRINGID_CRC(LevelEnding,3979194158)); }
        ITF_INLINE const Path&      getPrologueMap() const        { return getLevelPath(ITF_GET_STRINGID_CRC(Prologue,291121491)); }

        //Accessors for mission types
        const Ray_GameManagerConfig_Template::MissionConfig *findMissionConfigByType(const StringID &_type, bbool _useDefaultWhenNone) const;
        //Accessor for individual mission thresholds
        i32 getTimeAttackThreshold(const StringID &_levelTag, bbool _forCup); //returns -1 if none
        i32 getLumAttackThreshold(const StringID &_levelTag, u32 _lumAttackIndex/*0,1 or 2(lum medal)*/); // returns I32_MAX if none

        u32 getRichPresenceIndex(const StringID &_levelTag);
        bbool isMenuToWMEnable(const StringID &_levelTag);

        // register sprint tutorial
        void registerSprintTutorial() { m_sprintTutorialRegistered = btrue; };

        // record sequence switch triggerd
        void setSequenceSwitchTriggered(const ActorRef& _ref);
        bbool wasSequenceSwitchTriggered(const ActorRef& _ref) const;

        void bubblizerAchievement(ActorRef _victim, ActorRef _perpetrator, u32 _identifer);
        void loadLoadingScreen(StringID _tag);
        void loadMapThroughLoadingScreen(StringID _tag);

        virtual bbool isLoadingVisible();

        const Path & getCreditsMap() const { return getLevelPath(ITF_GET_STRINGID_CRC(Credits,2384050872)); }
        const Ray_GameManagerConfig_Template* getTemplate() const { return m_configTemplate; }

        //set default sound mix according to active players count
        virtual void setDefaultBusMix();
        virtual void stopDefaultBusMix();
        //set pause bus mix
        virtual void setPauseBusMix();
        virtual void stopPauseBusMix();

        void            createRewardResources();
        void            onBeginToLoadMap();

        void            setLevelTitleActor(ActorRef _ref)           { m_levelTitleActor = _ref; }
        ActorRef        getLevelTitleActor() const                  { return m_levelTitleActor; }

        bbool           isAllGameLevelsCompleted();
        void            setLoadingScreenPath(const Path & _path)    { m_loadingScreenPath = _path;}
        const Path &    getLoadingScreenPath() const                { return m_loadingScreenPath; }

        void            preloadPrologue();
        ObjectRef       getPreloadedPrologueWorld()const            { return m_preloadedPrologueWorld; }
        bbool           isPreloadedPrologueReady()const             { return m_preloadedPrologueReady; }

        virtual void    onDeleteWorld(World* _pWorld);

        const Vec3d     getEndSequencePlayerPosition() const { return m_configTemplate->getEndSequencePlayerPosition(); }

        void            spawnMedal( const StringID& _levelName );
        void            unspawnMedal();
#ifdef ITF_SUPPORT_CHEAT
        virtual void    cheatUnlockAllLevel();
        virtual void    cheatMaxCurrency();
#endif

        // Game Options / Accessibility
        Ray_GameOptionManager& getGameOptionManager() { return m_gameOptionManager; }
        const Ray_GameOptionManager& getGameOptionManager() const { return m_gameOptionManager; }
        void            applyDisplayOptions();

        EHealthModifier getHealthModifier() const;
        void            setHealthModifier(EHealthModifier _modifier);
        void            restoreHealthModifierForAllPlayers();
        void            clearHealthModifierForAllPlayers();

        //==========================================================================
        // Option Menu - Display Options
        //==========================================================================
        i32             getResolutionIndex() const;
        void            setResolutionIndex(i32 index);
        const char*     getResolutionDisplayName(i32 index) const;

        bbool           isWindowed() const;
        void            setWindowed(bbool windowed);

        i32             getLanguageIndex() const;
        void            setLanguageIndex(i32 index);
        const char*     getLanguageDisplayName(i32 index) const;
        //==========================================================================
        // Option Menu - Accessibility Options
        //==========================================================================
        i32             getStartWithHeartIndex() const;
        void            setStartWithHeartIndex(i32 index);
        const char*     getStartWithHeartDisplayName(i32 index) const;
        i32             getStartWithHeartLineId(i32 index) const;

        i32             getRunButtonMode() const;
        void            setRunButtonMode(i32 mode);
        const char*     getRunButtonDisplayName(i32 index) const;
        i32             getRunButtonLineId(i32 index) const;

        bbool           isMurfyAssistEnabled() const;
        void            setMurfyAssist(bbool enabled);

        i32             getVibrationMode() const;
        void            setVibrationMode(i32 mode);
        const char*     getVibrationDisplayName(i32 index) const;
        u32             getVibrationLineId(i32 index) const;
        bbool           areVibrationsEnabled() const;
        void            setVibrations(bbool enabled);

        //==========================================================================
        // Option Menu - Sound Options
        //==========================================================================
        f32             getMasterVolume() const;
        void            setMasterVolume(f32 volume);

        f32             getMusicVolume() const;
        void            setMusicVolume(f32 volume);

        f32             getSFXVolume() const;
        void            setSFXVolume(f32 volume);

        f32             getIntensity() const;
        void            setIntensity(f32 intensity);

        void            applyMasterVolumeOption();
        void            applyMusicVolumeOption();
        void            applySFXVolumeOption();
        void            applyIntensityOption();

        //==========================================================================
        // Save/Load Options
        //==========================================================================
        void            saveGameOptions();
        void            loadGameOptions();

        //CASA::VITA::TARIK::Fresco
        ITF_INLINE const ITF_VECTOR<Vec2d>& getFrescoGamePlayProgressionInfo() const { return m_configTemplate->getFrescoGamePlayProgressionInfo(); }
        void									PickUpAllRelics();
        ITF_INLINE void							setAllRelicsDuplicateCount(u32 _count) { getPersistentGameData()->setAllRelicsDuplicateCount(_count); }
        ITF_INLINE void							UnlockAllRelics(RelicType _relicType) { getPersistentGameData()->UnlockAllRelics(_relicType); }
        ITF_INLINE u32							GetUnlockedRelicCount() { return getPersistentGameData()->GetUnlockedRelicCount(); }
        ITF_INLINE u32							GetUnlockedRelicCount(RelicType _type) { return getPersistentGameData()->GetUnlockedRelicCount(_type); }
        ITF_INLINE void							UpdateFrescoRelicDuplicateCount(u32 _frescoId, u32 _relicId, u32 _count, RelicType _relicType = RELIC_TYPE_NONE) { getPersistentGameData()->getFrescoData().UpdateRelicDuplicateCount(_frescoId, _relicId, _count, _relicType); }

		ITF_INLINE								bbool getIsPlayingFrescoVideo() const { return m_isPlayingFrescoVideo; }
		ITF_INLINE								void setIsPlayingFrescoVideo(bbool _val) { m_isPlayingFrescoVideo = _val; }

#ifdef ITF_SUPPORT_BOT_AUTO
        ITF_INLINE BotController* getBotController() { return m_botController; }
        ITF_INLINE const BotController* getBotController() const { return m_botController; }
        void updateBotController(f32 dt);
        void setBotMode(BotMode mode);
        BotMode getBotMode() const;
        bbool isBotActive() const;
        virtual u32 getPlayerStance(u32 playerIndex) const;
        void scanTargetsForBot();
        void updateTargetDistancesForBot(struct GameState* state);
#endif // ITF_SUPPORT_BOT_AUTO
    private:
        // Save/Load callbacks
        static void     onSaveOptionsComplete(Ray_GameOptionPersistence::Result result);
        static void     onLoadOptionsComplete(Ray_GameOptionPersistence::Result result);
        Path m_loadingScreenPath;

        enum EPauseFlag
        {
            PAUSEFLAG_NONE          = 0x000,    // No pause
            PAUSEFLAG_INGAME        = 0x001,    // Ingame pause
            PAUSEFLAG_GAMEPLAY      = 0x002,    // Gameplay Pause
            ENUM_FORCE_SIZE_32(EPauseFlag)
        };

        enum GameplayPause_t {GameplayPause_NoChange,GameplayPause_Pause,GameplayPause_Unpause};

        u32             setSequence( Ray_GameManagerSequenceBase* _sequence );
        u32             isPlayingSeq( const Ray_GameManagerSequenceBase* _sequence ) const;

        void            createMedalMap();
        void            destroyMedalMap();
        void            updateLoading();

        void            pauseGameplay();
        void            unpauseGameplay();
        void            updatePauseMode();


        void            updateInputConfig();
        static bbool    on_ChangeExtensionPreStartTRCMessage(class TRCMessage_Base* pMessage, void* pParams);

        bbool           createRewardResources(const Path &_lumPath, const Path &_heartPath);
        void            releaseRewardResources();

        void            registerAllGameOptions();
        void            registerResolutionOption();
        void            registerWindowedOption();
        void            registerLanguageOption();
        void            registerStartWithHeartOption();
        void            registerRunButtonOption();
        void            registerMurfyAssistOption();
        void            registerVibrationOption();
        void            registerMasterVolumeOption();
        void            registerMusicVolumeOption();
        void            registerSFXVolumeOption();
        void            registerIntensityOption();
        void            registerLastPlayTime();
        void            applyLanguageOption();
        void            applyStartWithHeartOption();
        void            applyVibrationOption();
        void            applyRunButtonOption();
        void            applyMurfyAssistOption();

        Scene*          getSceneForPersistentActor(Actor *_obj) const;

        static void     cutSceneAlreadySeenCallback(Actor* _cutSceneActor);
        static bbool    canInterruptCutSceneCallback(Actor* _cutSceneActor);

        void            updateWMStates();
        ITF_INLINE bbool    isOfficialMap(const Path& _map) const {return m_configTemplate->isOfficialMap(_map); }

        void            onBaseMapLoaded();

        void            onMapLoadedUpdateTimeAttack();
        void            onCheckpointLoadUpdateTimeAttack();
        void            onCheckpointLoadUpdateRescued();
        void            onCheckpointLoadUpdateSprintTutorial();

        class Ray_UIFadeScreenComponent* getUIFade() const;

        virtual const GameManagerConfig_Template * loadConfig(const Path & _path);

        WorldFile       m_defaultWorldFile;

        const Ray_GameManagerConfig_Template * m_configTemplate;

        SafeArray<SourceHandle>         m_voicesPausedWhenEnteringGameplayPause;

        // Sequences
        Ray_GameManagerSequenceTeleportCheat                m_teleportCheatSequence;
        Ray_GameManagerSequenceVictory                      m_victorySequence;
        Ray_GameManagerSequenceHiddenArea                   m_hiddenAreaSequence;
        Ray_GameManagerRestoreDeath                         m_restoreDeath;
        Ray_GameManagerPlaySeqActor                         m_playSeqActor;
        Ray_GameManagerSequenceCheckpoint                   m_checkpointSequence;
		Ray_GameManagerSequenceChangePage                   m_changePageSequence;
        Ray_GameManagerSequenceStargate                     m_stargateSequence;
        Ray_GameManagerSequenceScaleDoor                    m_scaleDoorSequence;
        Ray_GameManagerSequenceBounceToLayer                m_bounceToLayerSequence;
        Ray_GameManagerSequenceStartTimeAttack              m_startTimeAttackSequence;
        Ray_GameManagerSequenceChangeRoom                   m_changeRoomSeq;
        Ray_GameManagerSequenceReturnTeeth                  m_returnTeethSeq;
        Ray_GameManagerSequenceFadeAndLoadMap               m_fadeAndLoadMapSeq;
        Ray_GameManagerSequenceChangePageWithAnim           m_changePageWithAnimSequence;
        Ray_GameManagerSequenceEndGame                      m_endGameSequence;
        Ray_GameManagerSequenceWorldMapUnlock               m_worldMapUnlockSequence;
        Ray_GameManagerSequenceWorldMapTeleport             m_worldMapTeleportSequence;
        FixedArray <Ray_GameManagerSequenceBase*,10>        m_currentSequences;

        // rules

        u32 m_totalLums;
        u32 m_totalTuns;

        Ray_PersistentGameData_Level m_currentLevelData;
        Ray_PersistentGameData_Level m_currentLevelData_Checkpoint;

        Ray_PersistentGameData_Score m_currentScore;
        Ray_PersistentGameData_Score m_currentScore_Checkpoint;


        bbool       m_allowFriendlyFire;
        bbool       m_allowPlayerCrush;
        bbool       m_allowNpcCrush;


        bbool       m_isTimeAttack;
        bbool       m_isTimeAttackFinished;
        bbool       m_triggerTimeAttackEndSequence;
        f32         m_timeAttackTimer;
        ActorRef    m_timeAttackActorTimer;
        ActorRef    m_timeAttackActorFlag;
        ActorRef    m_timeAttackActorResults;
        ActorRef    m_timeAttackActorChrono;

        ActorRef    m_worldMapUnlockMsgActor;

        ActorRef    m_skipLevelActor;

        ActorRef    m_loadNotificationActor;
        bbool       m_showingLoadNotificationActor;

        bbool m_wasPostLoadInitCalled;
        ActorRef m_medalActor;
        ObjectRef m_medalWorldRef;
        ActorRef m_skullActor;

        ResourceID m_spawnedRewardResourceGroup;
        SpawneeGenerator m_lumGenerator, m_heartGenerator;

        GameplayPause_t m_requestedGameplayPause;

        u32                         m_pauseType;

        // Friendlies
        struct FriendlyRescuedInfo
        {
            ObjectRef           m_ref;
            bbool               m_teleport;
        };
        typedef SafeArray <FriendlyRescuedInfo> FriendliesList;

        u32 findRescuedFriendly( ObjectRef _ref, const FriendliesList& _list ) const;
        FriendliesList          m_friendliesRescued;
        FriendliesList          m_storedFriendliesRescued;
        FriendliesList          m_totalRescuedFriendlies;

        Ray_PowerUpManager  m_powerUpManager;
        f32                 m_playersCurrentScale;

        Ray_RewardManager   m_rewardManager;
        Ray_GameOptionManager m_gameOptionManager;
        Ray_GameOptionPersistence* m_gameOptionPersistence;
        Ray_TRCHelper*      m_trcHelper;
        struct ResolutionEntry { i32 width; i32 height; };
        Vector<ResolutionEntry> m_availableResolutions;

#ifdef ITF_SUPPORT_ONLINETRACKING
		Ray_OnlineTrackingManager m_onlineTrackingManager;
#endif // ITF_SUPPORT_ONLINETRACKING

        ActorRef            m_lastPlayerAlive;
        u32                 m_maxPlayers;

        // Misc
        u32                 m_hideJoinMessage;

        ObjectRef           m_changeRoomDoor;
        ObjectRef           m_costumeManager;

        typedef ITF_MAP<StringID, ObjectPath> WMSpotList;
        WMSpotList          m_wmSpots;

        // sprint tutorial
        bbool m_sprintTutorialRegistered;
        u32 m_sprintTutorialFailureCount;

        // sequence switches (for RO-14848)
        static const u32 s_maxSequenceSwitchesPerMap = 8;
        FixedArray<ActorRef,s_maxSequenceSwitchesPerMap> m_sequenceSwitchesTriggered;

        typedef ITF_MAP<u32,ActorRefList> Bubblizer;
        Bubblizer                   m_bubblizer;

        // End level
        ObjectRef                   m_preloadedEndLevelMapRef;
        StringID m_lastWMLevelTag;

        ObjectRef                   m_preloadedWorldMap;

        // world structure
        struct WorldMapLevels
        {
            WorldMapLevels()
                : m_maskNew(0)
            {}
            ~WorldMapLevels(){}

            ITF_VECTOR<StringID>        m_levelTags;
            u32                         m_maskNew;
        };
        typedef ITF_VECTOR<StringID>        LevelTags;
        ITF_MAP<StringID,WorldMapLevels>     m_worldMapWorlds;
        ITF_VECTOR<StringID>                m_lockedLevelsWithElectoons;

        // Save info
        u32                         m_currentSlotIndex;

        // Skippable level
        u32                         m_deathCounterOnSameCheckpoint;
        bbool                       m_hasRejectedSkip;
        ObjectRef                   m_deathCounterCheckpointRef;

        ActorRef                    m_uiFadeActorRef;
        ActorRef                    m_levelTitleActor;

        u32                         m_lastPadType[s_maxPlayers];
        StringID                    m_currentMedal;

        ObjectRef                   m_preloadedPrologueWorld;
        bbool                       m_preloadedPrologueReady;
		bbool						m_isPlayingFrescoVideo;
#ifdef ITF_SUPPORT_BOT_AUTO
        BotController*              m_botController;
        ITF_VECTOR<TargetInfo>      m_allTargets;
#endif // ITF_SUPPORT_BOT_AUTO
    };

#define RAY_REWARD_MANAGER      RAY_GAMEMANAGER->getRewardManager()
}

#endif //_ITF_RAY_GAMEMANAGER_H_

