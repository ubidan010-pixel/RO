#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_
#include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#endif //_ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Misc/Ray_ShooterCheckPointComponent.h"
#endif //_ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_


#ifndef _ITF_RAY_SHOOTERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERACOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_VIRTUALLINKCOMPONENT_H_
#include "gameplay/components/misc/VirtualLinkComponent.h"
#endif //_ITF_VIRTUALLINKCOMPONENT_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMapLoading.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_ShooterGameModeParameters)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterGameModeParameters)
        SERIALIZE_CONTAINER_OBJECT( "playersDataList", m_playerDataList );
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS( Ray_ShooterGameModeParameters, PlayerData )
        SERIALIZE_MEMBER( "playerID",   m_playerID );
        SERIALIZE_MEMBER( "luaFile",    m_luaFile );
    END_SERIALIZATION()

    const static StringID s_shooterMusicHolder_channelID( "MusicShooter_Holder" );
    
    //---------------------------------------------------------------------------------------------------------
    const Ray_ShooterGameModeParameters::PlayerData * Ray_ShooterGameModeParameters::getPlayerDataFromPlayerID( const StringID & _playerID ) const
    {
        for ( u32 i = 0; i < m_playerDataList.size(); ++i )
        {
            const PlayerData & playerData = m_playerDataList[i];
            if ( playerData.getPlayerID() == _playerID )
            {
                return &playerData;
            }
        }        
        return NULL;    
    }


	//---------------------------------------------------------------------------------------------------------
    Ray_ShooterGameModeController::Ray_ShooterGameModeController( GameMode _gamemode ) 
    : m_swapMode( bfalse )
    , m_swapMode_PlayerActive( bfalse )
    , m_activeCheckPointIsCameraActivator( bfalse )
    , m_playersRegisteredToGameManager( bfalse )
	{
        m_gameMode = _gamemode;

        m_playersData.resize( GAMEMANAGER->getMaxPlayerCount() );
	}

	//---------------------------------------------------------------------------------------------------------
	Ray_ShooterGameModeController::~Ray_ShooterGameModeController()
	{

	}
    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::spawnPlayerActors( const StringID& _gameScreen )
    {
        GameModeController::spawnPlayerActors( _gameScreen );

        Ray_ShooterGameModeParameters * shooterParams = getShooterGameModeParameters();
        if ( shooterParams == NULL )
            return;

        if ( _gameScreen != ITF_GET_STRINGID_CRC(Ray_GameScreen_WorldMapLoading,1826584106) )
        {
            registerPlayersToGameManager();
             
            // remove unwanted prefetched actor templates
            //
            if ( CURRENTWORLD != NULL )
            {
                if ( Scene * rootScene = CURRENTWORLD->getRootScene() )
                {
                    if ( TemplateClientHandler* clientHandler = rootScene->getTemplateClientHandler() )
                    {
                        const Ray_ShooterGameModeParameters::PlayerDataList & dataList = shooterParams->getPlayerDataList();
                        for ( u32 i = 0; i < dataList.size(); ++i )
                        {
                            const Ray_ShooterGameModeParameters::PlayerData & playerData = dataList[i];
                            StringID luaFile = playerData.getPlayerLuaFile().getStringID();

                            if ( ( getRegisteredPlayerIdx( playerData.getPlayerID() ) < 0 ) && clientHandler->isUsedTemplate( luaFile ) )
                            {
                                clientHandler->removeUsedTemplate( luaFile );
                            }
                        }                       
                    }
                }
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onPlayerActorsRemoved()
    {
        GameModeController::onPlayerActorsRemoved();

        if ( getShooterGameModeParameters() == NULL )
            return;

        // clear the registered player actors  list
        if ( m_playersRegisteredToGameManager )
        {
            m_registeredPlayerActors.clear();
            m_playersRegisteredToGameManager = bfalse;
        }
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onGameModeParametersCreated()
    {
        GameModeController::onGameModeParametersCreated();

        // Player actors creation is done on paremeter creation
        createPlayers();
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onGameModeParametersBeforeDestroy()
    {
        GameModeController::onGameModeParametersBeforeDestroy();

        // reset data
        m_activeCheckPoint.invalidate();

        destroyPlayers();
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::createPlayers()
    {
        Ray_ShooterGameModeParameters * shooterParams = getShooterGameModeParameters();
        ITF_ASSERT( shooterParams != NULL );        // invalid params ?
        if ( shooterParams == NULL )
            return;

        // get player scene
        //
        World* playerWorld = (World*)GETOBJECT(GAMEMANAGER->getPlayerWorld());
        Scene * playerScene = NULL;
        ITF_ASSERT( playerWorld != NULL );
        if ( playerWorld == NULL )
        {
            return;
        }
        
        playerScene = playerWorld->getRootScene();
        ITF_ASSERT( playerScene != NULL );
        if ( playerScene == NULL )
        {
            return;
        }
        
        //**************************
        // CREATE PLAYERS FROM IDS
        //**************************
        if ( m_registeredPlayerActors.size() != 0 )
        {
            destroyPlayers();
        }
        m_registeredPlayerActors.clear();

        PathString_t luaFilename;

        for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer( i );
            if ( player != NULL )
            {
                const StringID & playerInfoID = player->getCurrentPlayerIDInfo();

                // find the given ID in the data list
                //
                const Ray_ShooterGameModeParameters::PlayerData * playerData = shooterParams->getPlayerDataFromPlayerID( playerInfoID );

                ITF_ASSERT( playerData != NULL );   // config not found ??

                if ( playerData != NULL )
                {
                    Actor * theActor = ACTORSMANAGER->spawnActor(   Vec3d(0,0,0), 
                                                                    playerScene, 
                                                                    playerData->getPlayerLuaFile(),
                                                                    bfalse,
                                                                    NULL);

                    ITF_ASSERT( theActor != NULL );

                    if ( theActor != NULL ) 
                    {
                        theActor->loadResources();

                        playerData->getPlayerLuaFile().getString(luaFilename);

                        String theActorName = FilePath::getFilenameWithoutExtension(luaFilename);
                        theActor->setUserFriendly(StringConverter(theActorName).getChar());
                        theActor->disable();

                        // register the actor in the list
                        RegisteredPlayer data;
                        data.m_actor = theActor->getRef();
                        data.m_playerID = player->getID();

                        m_registeredPlayerActors.push_back( data );

                        EventPlayerIndexChange playerIndexEvent(i);
                        theActor->onEvent(&playerIndexEvent);

                        m_playersData[i].Reset();

                        theActor->onLoaded();
                    }
                }
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------
    // player actor registration to game mode is done just before map start.
    // But actor creation is done during checkpoint loading
    //
    void Ray_ShooterGameModeController::registerPlayersToGameManager()
    {
        for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer( i );
            if ( player != NULL )
            {
                i32 idx = getRegisteredPlayerIdx( player->getID() );
                ITF_ASSERT( idx >= 0 );

                if ( idx >= 0 )
                {
                    RegisteredPlayer & registeredPlayer = m_registeredPlayerActors[idx];

                    gameRegisterPlayerActorForModeAndIdx( idx, m_gameMode, bfalse );
                    // register actor in platformer mode because the moskito must appear in platform maps 
                    gameRegisterPlayerActorForModeAndIdx( idx, RAY_GAMEMODE_PLATFORMER, btrue );

                    Actor * playerActor = AIUtils::getActor( registeredPlayer.m_actor );
                    ITF_ASSERT( playerActor != NULL );

                    if ( playerActor != NULL )
                    {
                        EventPlayerIndexChange playerIndexEvent(i);
                        playerActor->onEvent(&playerIndexEvent);

                        if ( player->getActive() )
                        {
                            teleportPlayerOnActiveCheckPoint( player );
                        }
                    }
                }
            }
        }
        m_playersRegisteredToGameManager = btrue;
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::destroyPlayers()
    {
        Ray_ShooterGameModeParameters * shooterParams = getShooterGameModeParameters();
        //ITF_ASSERT( shooterParams != NULL );        // invalid params ?
        if ( shooterParams == NULL )
            return ;

        // get player scene
        //
        World* playerWorld = (World*)GETOBJECT(GAMEMANAGER->getPlayerWorld());
        Scene * playerScene = NULL;
        ITF_ASSERT( playerWorld != NULL );
        if ( playerWorld == NULL )
            return;
            
        playerScene = playerWorld->getRootScene();
        ITF_ASSERT( playerScene != NULL );
        if ( playerScene == NULL )
        {
            return;
        }        

        for ( u32 i = 0; i < m_registeredPlayerActors.size(); ++i )
        {
            if ( Actor* actor = AIUtils::getActor( m_registeredPlayerActors[i].m_actor ) )
            {
                playerScene->unregisterPickable( actor );
            }

            gameUnregisterPlayerActorForMode( m_registeredPlayerActors[i].m_playerID );
            m_registeredPlayerActors.eraseKeepOrder( i );
            i--;
        }

        m_playersRegisteredToGameManager = bfalse;
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onActivated()
    {
        GameModeController::onActivated();

        //BaseObject * cpBaseObj = m_activeCheckPoint.getObject();
        //Actor * checkPoint = ( cpBaseObj != NULL ) ? cpBaseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) : NULL;
        //if ( checkPoint != NULL )
        //{
        //    //GAMEMANAGER->onReachedCheckpoint( m_activeCheckPoint );
        //}

        // start the music only on shooter mode activation on Check point IN
        //
        if ( m_parametersOwnerList.size() > 0 )
        {
            if ( m_parametersOwnerList[0] == m_activeCheckPoint )
            {
                EventPlayMusic startMusic;
                VirtualLinkComponent::broadcastEventToChannel( ObjectRef::InvalidRef, s_shooterMusicHolder_channelID, &startMusic );
            }        
        }

        resetPlayerReadyToRestart();
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onDesactivated()
    {
        GameModeController::onDesactivated();

        // Add player actor in current mode in camera
        //
        for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer( i );
            if ( ( player != NULL ) && player->getActive() )
            {
                if ( Actor * actor = getPlayerActorForCurMode( player ) )
                {
                    CAMERACONTROLLERMANAGER->unregisterSubject( actor->getRef() );
                }
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onPlayerGameModeHasChanged( Player * _player )
    {
        GameModeController::onPlayerGameModeHasChanged( _player );

        if ( ( m_parameters == NULL ) || !m_playersRegisteredToGameManager )
            return;
        
        if ( ( _player != NULL ) && _player->getActive() )
        {
            Actor * onePlayer = getPlayerActorForCurMode( _player );

            if ( ( onePlayer != NULL ) && !onePlayer->isEnabled() )
            {
                activatePlayerForCurMode( _player, onePlayer->getPos() );
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onCheckPointLoaded( ObjectRef _checkPointRef )
    {
        GameModeController::onCheckPointLoaded( _checkPointRef );

        if ( ( m_parameters == NULL ) || !m_playersRegisteredToGameManager )
            return;

        // check if the checkpoint is a shooter one
        //
        i32 checkPointShooterIdx = m_checkPointList.find(_checkPointRef);
        bbool isExitCheckPoint = ( checkPointShooterIdx >= 0 ) ? isExitCheckpoint( _checkPointRef ) : bfalse;
        
        // Enable /teleport all activated player for mode
        //
        for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer( i );
            Player * swappedPlayer = GAMEMANAGER->getPlayer( player->getIndex() );

            // check if the registered actor is in the player actors 
            // can arrives if costumes has changed
            if ( !player->isRegisteredActor( getRegisteredPlayerActorForMode( player->getID() ) ) )
            {
                destroyPlayers();
                createPlayers();
            }

            // Not a shooter checkpoint or exit checkpoint => reset to platform mode
            //
            if ( ( ( checkPointShooterIdx < 0 ) && ( player->getCurrentMode() == m_gameMode ) ) || isExitCheckPoint )
            {
                GAMEMANAGER->changePlayerGameMode( player, RAY_GAMEMODE_PLATFORMER );

                ITF_ASSERT_CRASH( i < m_playersData.size(),"player in game array not initialized ?" );
                m_playersData[i].m_inMoskitoZone = bfalse;
            }
            // is not in current gamemode and not on exit checkpoint
            //
            else if ( ( checkPointShooterIdx >= 0 ) && ( player->getCurrentMode() != m_gameMode ) && !isExitCheckPoint )
            {
                GAMEMANAGER->changePlayerGameMode( player, m_gameMode );

                ITF_ASSERT_CRASH( i < m_playersData.size(),"player in game array not initialized ?" );
                m_playersData[i].m_inMoskitoZone = btrue;
            }           

            // teleport current mode actor if active else disable it
            //
            if ( player->getActive() )
            {
                teleportPlayerOnActiveCheckPoint( swappedPlayer );
            }
            else
            {
                disablePlayerForCurMode( swappedPlayer );
            }
        }

        resetPlayerReadyToRestart();
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onPlayerSwapped( u32 _from, u32 _to )
    {
        GameModeController::onPlayerSwapped( _from, _to );

        if ( ( m_parameters == NULL ) || ( m_registeredPlayerActors.size() == 0 ) )
            return;
        
        Player * fromPlayer = GAMEMANAGER->getPlayer( _from );
        Player * toPlayer = GAMEMANAGER->getPlayer( _to );
       
        if ( m_active )
        {
            onPlayerActivationChanged( fromPlayer, bfalse ); // dest actor
            onPlayerActivationChanged( toPlayer, bfalse );
        }   
        else
        {
            onPlayerActivationChanged( fromPlayer, bfalse );
            onPlayerActivationChanged( toPlayer, bfalse );
        }

        // swap registered actors
        //
        i32 fromPlayerIdx = getRegisteredPlayerIdx( fromPlayer->getID() );
        i32 toPlayerIdx = getRegisteredPlayerIdx( toPlayer->getID() );
        ITF_ASSERT( fromPlayerIdx >= 0 );
        ITF_ASSERT( toPlayerIdx >= 0 );
        if ( ( fromPlayerIdx < 0 ) || ( toPlayerIdx < 0 ) )
        {
            return;
        }

        ObjectRef toPlayerActorRef = m_registeredPlayerActors[toPlayerIdx].m_actor;
        m_registeredPlayerActors[toPlayerIdx].m_actor = m_registeredPlayerActors[fromPlayerIdx].m_actor;
        m_registeredPlayerActors[fromPlayerIdx].m_actor = toPlayerActorRef;

    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::onPlayerActivationChanged( Player * _player, bbool _startDead )
    {
        ITF_ASSERT( _player != NULL );

        if ( ( m_parameters == NULL ) || !m_playersRegisteredToGameManager )
        {
            return;
        }

        bbool playerActive =  m_swapMode ? m_swapMode_PlayerActive : _player->getActive();

        if ( playerActive )
        {
            // Mode not active or one player in current mode
            //
            if ( !m_active || ( GAMEMANAGER->getNumActivePlayers() == 1 ) )
            {
                teleportPlayerOnActiveCheckPoint( _player );
            }
            // Activated during active game mode
            //
            else
            {
                // check landing
                bbool landing = bfalse;

                // drop checkpoints don't need landing check
                if ( isExitCheckpoint( m_activeCheckPoint ) )
                {
                    for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
                    {
                        if ( _player->getIndex() != i )
                        {
                            if ( m_playersData[i].m_isLanding )
                            {
                                landing = btrue;
                                break;
                            }
                        }
                    }
                }

                // landing, consider the new activation has platformer activation
                //
                if ( landing )
                {
                    teleportPlayerOnActiveCheckPoint( _player );
                    GAMEMANAGER->changePlayerGameMode( _player, RAY_GAMEMODE_PLATFORMER );

                    // re-send change game mode to force player position on moskito pos
                    if ( Actor * platformerActor = AIUtils::getActor( _player->getActorForMode(  RAY_GAMEMODE_PLATFORMER ) ) )
                    {
                        if ( Actor * shooterActor = getPlayerActorForCurMode( _player ) )
                        {
                            EventTeleport evtTeleport( shooterActor->get2DPos().to3d( platformerActor->getTemplate()->getForcedZ() ), 0.f, btrue );
                            platformerActor->onEvent( &evtTeleport );
                        }
                    }
                    // skip super call to avoid game mode re-activation
                    if ( m_active )
                    {
                        checkGameModeActivation();
                        return;
                    }
                }
                else
                {
                    Vec3d playerSpawnPos;
                    // get activated player to teleport new one one him
                    for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
                    {
                        if ( _player->getIndex() != i )
                        {
                            Actor * onePlayer = GAMEMANAGER->getActivePlayer( i );
                            if ( onePlayer != NULL ) 
                            {
                                ITF_ASSERT( GAMEMANAGER->getPlayer( i )->getCurrentMode() == m_gameMode );
                                playerSpawnPos = onePlayer->getPos();
                                break;
                            }
                        }
                    }
                    // Activate player for current mode and teleport him
                    activatePlayerForCurMode( _player, playerSpawnPos );
                }
            }
        }
        else
        {
            disablePlayerForCurMode(_player);
        }

        GameModeController::onPlayerActivationChanged( _player, _startDead );
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::registerCheckPoint( const ObjectRef & _objref )
    {
        // still registered ?
        if ( m_checkPointList.find(_objref) >= 0 )
            return;

        m_checkPointList.push_back( _objref );
    }
    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::unregisterCheckPoint( const ObjectRef & _objref )
    {
        i32 idx = m_checkPointList.find(_objref);
        if ( idx >= 0 )
        {
            m_checkPointList.eraseNoOrder( idx );
        }
    }
    
    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::setActiveCheckPoint( const ObjectRef & _objref, bbool _cameraActivator /* = bfalse*/)
    {
        m_activeCheckPoint = _objref; 
        m_activeCheckPointIsCameraActivator = _cameraActivator;
    }

    //---------------------------------------------------------------------------------------------------------
    //
    bbool Ray_ShooterGameModeController::teleportToNextCheckpoint( bbool _next )
    {
       /* if ( !isActive() )
        {
            return bfalse;
        }*/

        Actor * checkpoint = GAMEMANAGER->getCurrentCheckpoint();

        if ( checkpoint == NULL )
        {
            return bfalse;
        }
        CheckpointComponent* comp = checkpoint->GetComponent<CheckpointComponent>();

        if ( comp == NULL )
        {
            return bfalse;
        }
        
        u32 nextIndex = comp->getIndex();
        if ( _next )
        {
            nextIndex += 1;
        }
        else
        {
            nextIndex -= 1;
        }

        const ObjectRef& worldRef = GAMEMANAGER->getCurrentWorld();
        Actor* newCheckp = GAMEMANAGER->getCheckpointFromIndex(worldRef, nextIndex,_next);
        ITF_ASSERT(newCheckp);

        if ( newCheckp == NULL )
            return bfalse;

        // is not my check point ?
        //****************************
        if ( m_checkPointList.find( newCheckp->getRef() ) < 0 )
        {
            // change player mode
            for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
            {
                Player * player = GAMEMANAGER->getPlayer( i );
                if ( ( player != NULL ) && player->getActive() )
                {
                    GAMEMANAGER->changePlayerGameMode( player, RAY_GAMEMODE_PLATFORMER );
                }
            }
            return bfalse;
        }

        // teleport player on "shooter" check point
        //*******************************************
        Vec3d finalPos  = newCheckp->getPos();

        for( u32 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer( i );
            if ( ( player != NULL ) && player->getActive() )
            {
                GAMEMANAGER->changePlayerGameMode( player, m_gameMode );
                activatePlayerForCurMode( player, finalPos );
            }
        }
        if ( SHOOTERCAMERA )
        {
            // if the checkpoint actor is in the param owner list, it's the checkpoint IN
            SHOOTERCAMERA->shooterTeleportToCheckPoint( newCheckp, m_parametersOwnerList.find( newCheckp->getRef() ) < 0 ? btrue : bfalse );
        }
        // active check point
        GAMEMANAGER->onReachedCheckpoint( newCheckp->getRef() );
        setActiveCheckPoint( newCheckp->getRef() );
      
        return btrue;
    }

    //---------------------------------------------------------------------------------------------------------
    //
    Actor * Ray_ShooterGameModeController::findNearestCheckPointFromPos( const Vec3d & _pos, const Vec2d & _dir, bbool _avoidActive /*= bfalse*/ )
    {
        f32     nearestDist = F32_INFINITY;
        Actor * nearestCheckPoint = NULL;
        f32     dist;
        bbool   checkDir = !_dir.IsEqual( Vec2d::Zero );

        for ( u32 i = 0; i < m_checkPointList.size(); ++i )
        {
            if ( _avoidActive && ( m_activeCheckPoint == m_checkPointList[i] ) )
            {
                continue;
            }

            BaseObject * baseObj = m_checkPointList[i].getObject();
            if ( baseObj != NULL )
            {
                Actor * checkPoint = baseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                ITF_ASSERT( checkPoint != NULL );
                if ( checkPoint  != NULL )
                {
                    if ( checkDir && ( ( checkPoint->get2DPos() - _pos.truncateTo2D() ).dot( _dir ) < 0.f ) )
                        continue;

                    dist = ( checkPoint->get2DPos() - _pos.truncateTo2D() ).sqrnorm();
                    if ( dist <= nearestDist )
                    {
                        nearestDist = dist;
                        nearestCheckPoint = checkPoint;
                    }
                }
            }        
        }
        return nearestCheckPoint;
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::teleportPlayerOnActiveCheckPoint( Player * _player )
    {
        Actor * playerActorCurMode = getPlayerActorForCurMode( _player );
        ITF_ASSERT( playerActorCurMode != NULL );
        if ( playerActorCurMode == NULL )
            return;

        BaseObject * cpBaseObj = m_activeCheckPoint.getObject();
        Actor * checkPoint = ( cpBaseObj != NULL ) ? cpBaseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) : NULL;
        
        // Nearest check point get
        //
        if ( ( checkPoint == NULL ) && !m_active && ( _player->getCurrentMode() != m_gameMode ) )
        {
            // No check point, get the check point IN
            if ( m_parametersOwnerList.size() > 0 )
            {
                if ( BaseObject * baseObj = m_parametersOwnerList[0].getObject() )
                {
                    checkPoint = baseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                }
                // no check point found, get nearest
                //
                if ( checkPoint == NULL )
                {
                    Actor * playerActor = _player->getActor();
                    ITF_ASSERT( playerActor != NULL );
                    if ( playerActor != NULL )
                    {
                        checkPoint = findNearestCheckPointFromPos( playerActor->getPos(), Vec2d::Zero );
                    }
                }
                // valid "check point"
                if ( checkPoint != NULL )
                {
                    setActiveCheckPoint( checkPoint->getRef() );
                }
            }
        }
        
        if ( checkPoint != NULL )
        {
            Vec3d playerSpawnPos;
            CheckpointComponent * checkPointComp = checkPoint->GetComponent<CheckpointComponent>();
            ITF_ASSERT( checkPointComp != NULL );
            if ( checkPointComp != NULL )
            {
                playerSpawnPos = checkPointComp->getPlayerSpawnPos( _player->getIndex() ).to3d((checkPoint->getPos().m_z) );
            }

            // Activate player for current mode and teleport him
            //playerActorCurMode->enable();
            //EventTeleport evtTeleport( playerSpawnPos, 0.f, btrue );
            //playerActorCurMode->onEvent( &evtTeleport ); 
            activatePlayerForCurMode( _player, playerSpawnPos );
        }
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::disablePlayerForCurMode( Player * _player )
    {
        Actor * playerActorCurMode = getPlayerActorForCurMode( _player );
        //ITF_ASSERT( playerActorCurMode != NULL );
        if ( playerActorCurMode == NULL )
            return;

        playerActorCurMode->disable();
        m_playersData[ _player->getIndex() ].m_active_Internal = bfalse;
        m_playersData[ _player->getIndex() ].m_active_Visible = bfalse;
        m_playersData[ _player->getIndex() ].m_isLanding = bfalse;
      
        CAMERACONTROLLERMANAGER->unregisterSubject( playerActorCurMode->getRef() );
    }
    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::activatePlayerForCurMode( Player * _player, const Vec3d & _pos )
    {
        Actor * playerActorCurMode = getPlayerActorForCurMode( _player );
        ITF_ASSERT( playerActorCurMode != NULL );
        if ( playerActorCurMode == NULL )
            return;

        m_playersData[ _player->getIndex() ].m_active_Internal = btrue;
        m_playersData[ _player->getIndex() ].m_active_Visible = btrue;
        m_playersData[ _player->getIndex() ].m_isLanding = bfalse;

        playerActorCurMode->enable();
        EventTeleport evtTeleport( _pos, 0.f, btrue );
        playerActorCurMode->onEvent( &evtTeleport );

        CAMERACONTROLLERMANAGER->unregisterSubject( playerActorCurMode->getRef() );
    }

    //---------------------------------------------------------------------------------------------------------
    //
    Actor *Ray_ShooterGameModeController::getPlayerActorForCurMode( Player * _player )
    {
        ITF_ASSERT( _player != NULL );
        if ( _player == NULL ) 
            return NULL;

        BaseObject * bactiveplayerActor = getRegisteredPlayerActorForMode( _player->getID() ).getObject();
        return ( ( bactiveplayerActor != NULL ) ? bactiveplayerActor->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) : NULL );
    }

    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::setPlayerReadyToRestart( Player * _player )
    {
        ITF_ASSERT( _player != NULL );
        if ( ( _player == NULL ) || ( _player->getIndex() >= m_playersData.size() ) )
        {
            return;
        }
        m_playersData[ _player->getIndex() ].m_readyToRestart = btrue;
        
        // restart ?
        //
        bbool reload = btrue;

        for ( u8 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer(i);
            if ( player && player->getActive() )
            {
                PlayerInGameData & playerInGameData = m_playersData[ player->getIndex() ];

                if ( !player->isDead() || ( ( player->getCurrentMode() == m_gameMode ) && !playerInGameData.m_readyToRestart ) )
                {
                    reload = bfalse;
                }
            }
        }

        if ( reload )
        {
            RAY_GAMEMANAGER->triggerRestoreDeath( _player->getCurrentActor());
            resetPlayerReadyToRestart();
        }
    }

    //---------------------------------------------------------------------------------------------------------
    //
    bbool Ray_ShooterGameModeController::isAllPlayerDead()
    {
        for ( u8 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer(i);
            if ( player && player->getActive() && !player->isDead() )
            {
                return bfalse;
            }
        }
        return btrue;
    }

    //---------------------------------------------------------------------------------------------------------
    //
    bbool Ray_ShooterGameModeController::isExitCheckpoint( const ObjectRef & _objref )
    {
        if ( Actor * checkpoint = AIUtils::getActor( _objref ) )
        {
            Ray_ShooterCheckPointComponent * comp = checkpoint->GetComponent<Ray_ShooterCheckPointComponent>();
            ITF_ASSERT( comp != NULL ); // must have the shooter checkpoint component
            if ( comp != NULL )
            {
                return comp->isExitMode();
            }
        }
        return bfalse;
    }

    //---------------------------------------------------------------------------------------------------------
    // 
    void Ray_ShooterGameModeController::resetPlayerReadyToRestart()
    {
        for ( u8 i = 0 ; i < m_playersData.size(); ++i )
        {
            m_playersData[i].m_readyToRestart = bfalse;
        }
    }

    //---------------------------------------------------------------------------------------------------------
    // 
    void Ray_ShooterGameModeController::update()
    {
        GameModeController::update();

        if ( ( m_parameters == NULL ) || !m_playersRegisteredToGameManager )
            return;

        bbool shooterPlayerVisible = m_active;
        
        // mode not activated, check shooter actors visibility
        //
        if ( !m_active )
        {
            ICameraController* camController = CAMERACONTROLLERMANAGER->getMainCameraController();
            if ( camController != NULL )
            {
                AABB screenAABB = CAMERACONTROLLERMANAGER->getAABB();
                screenAABB.ScaleFromCenter( Vec2d( 2.0f, 2.0f ) );
                //GFX_ADAPTER->drawDBGAABB( screenAABB, 0, 1, 1, 0, 2, CAMERACONTROLLERMANAGER->getCamPos().m_z - CAMERACONTROLLERMANAGER->getDepth() );

                for ( u8 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
                {
                    Player * player = GAMEMANAGER->getPlayer(i);
                    
                    if ( ( player != NULL ) && player->getActive() )
                    {
                        if ( Actor * actor = getPlayerActorForCurMode( player ) )
                        {
                            if ( screenAABB.checkOverlap( actor->getAABB() ) )
                            {
                                shooterPlayerVisible = btrue;
                                break;
                            }
                        }
                    }
                }
                // no player visible, check the checkpoint IN
                //
                if ( !shooterPlayerVisible )
                {
                    if ( Actor * activeCheckpoint = AIUtils::getActor( m_activeCheckPoint ) )
                    {
                        if ( screenAABB.checkOverlap( activeCheckpoint->getAABB() ) )
                        {
                            shooterPlayerVisible = btrue;
                        }
                    }
                }                    
            }
        }

        for ( u8 i = 0 ; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            PlayerInGameData & ingamedata = m_playersData[i];

            if ( !ingamedata.m_active_Internal || ( ingamedata.m_active_Visible == shooterPlayerVisible ) )
            {
                continue;
            }

            if ( Player * player = GAMEMANAGER->getPlayer(i) )
            {
                if ( Actor * actor = getPlayerActorForCurMode( player ) )
                {
                    // check activation coherency
                    if ( ingamedata.m_active_Visible == actor->isEnabled() )
                    {
                        ingamedata.m_active_Visible = shooterPlayerVisible;

                        if ( shooterPlayerVisible )
                        {
                            actor->enable();
                        }
                        else
                        {
                            actor->disable();
                        }
                    }
                }
            }
        }

#ifdef SHOOTER_PROFILING
        displayProfilingValues();
#endif //SHOOTER_PROFILING
    }


#ifdef SHOOTER_PROFILING
    //---------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterGameModeController::addProfilingValue( const StringID & _value, ActorRef _owner )
    {
        for ( u32 i = 0; i < m_SP_DataList.size(); ++i )
        {
            SP_Data & data = m_SP_DataList[i];

            if ( data.m_value == _value )
            {
                if ( data.m_actorList.find( _owner ) < 0 )
                {
                    data.m_actorList.push_back( _owner );
                }
                return;
            }        
        }
        SP_Data data;
        data.m_value = _value;
        data.m_actorList.push_back( _owner );
        m_SP_DataList.push_back(data);
    }

    void Ray_ShooterGameModeController::removeProfilingValue( const StringID & _value, ActorRef _owner )
    {
        for ( u32 i = 0; i < m_SP_DataList.size(); ++i )
        {
            SP_Data & data = m_SP_DataList[i];

            if ( data.m_value == _value )
            {
                i32 idx = data.m_actorList.find( _owner );
                if ( idx >= 0  )
                {
                    data.m_actorList.eraseNoOrder( idx );
                }
                return;
            }        
        }
    }

    void Ray_ShooterGameModeController::displayProfilingValues()
    {
        String s_Totaltext;

        for ( u32 i = 0; i < m_SP_DataList.size(); ++i )
        {
            SP_Data & data = m_SP_DataList[i];

            u32 activeCount = 0;
            u32 enableCount = 0;

            for ( u32 u = 0; u < data.m_actorList.size(); ++u )
            {
                if ( Actor * actor = AIUtils::getActor( data.m_actorList[u] ) )
                {
                    enableCount += actor->isEnabled() ? 1 : 0;
                    activeCount += actor->isActive() ? 1 : 0;
                }
                else
                {
                    data.m_actorList.eraseNoOrder(u);
                    --u;
                }
            }
            
            String s_text;
            s_text.setTextFormat( "%s : %d, %d, %d\n", data.m_value.getDebugString(), data.m_actorList.size(), enableCount, activeCount );
            s_Totaltext += s_text;
        }

        
        Vec2d pos = Vec2d( 0.0f, GFX_ADAPTER->getScreenHeight() * 0.5f );
        GFX_ADAPTER->drawDBGText( s_Totaltext, pos.m_x, pos.m_y );
    }
#endif //SHOOTER_PROFILING
}
	
