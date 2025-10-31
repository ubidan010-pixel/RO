#include "precompiled_gameplay.h"

#ifndef _ITF_GAMEMODECONTROLLER_H_
#include "gameplay/managers/GameModeController.h"
#endif //_ITF_GAMEMODECONTROLLER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

namespace ITF
{
	IMPLEMENT_OBJECT_RTTI(GameModeParameters)
	
    BEGIN_SERIALIZATION(GameModeParameters)
    END_SERIALIZATION()

    //---------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	GameModeController::GameModeController( GameMode _gamemode )
	 : m_gameMode( _gamemode )
	 , m_active( bfalse )
     , m_parameters( NULL )
     , m_checkGameModeParamsOwnersCoherency( bfalse )
	{		
	}

	//---------------------------------------------------------------------------------------------------------
	GameModeController::~GameModeController()
	{
        m_parametersOwnerList.clear();
        m_PlayerModeChangeRequests.clear();

        SF_DEL( m_parameters );
	}

    //---------------------------------------------------------------------------------------------------------
    void GameModeController::registerGameModeParameters( GameModeParameters * _gamemodeParams, const ObjectRef & _owner )   
    { 
        ITF_ASSERT_MSG( _gamemodeParams != NULL, "invalid game mode parameters !!" );  
        if ( _gamemodeParams == NULL )
        {
            return;
        }

        // Owner ref existing ?
        if ( m_parametersOwnerList.find( _owner ) >= 0 )
            return;

        // save the owner has ref
        m_parametersOwnerList.push_back(_owner);
        
        // Parameter not valid, duplicate the given one
        if ( m_parameters == NULL )
        {
            m_parameters = static_cast< GameModeParameters *>( _gamemodeParams->CreateNewObject() );
            BinaryClone( _gamemodeParams, m_parameters );

            onGameModeParametersCreated();
        }        
    }

    //---------------------------------------------------------------------------------------------------------
    void GameModeController::unregisterGameModeParametersOwner( const ObjectRef & _owner )
    {
        // unregister check next frame if the owner is destroyed 
        m_checkGameModeParamsOwnersCoherency = btrue;
    }

	//---------------------------------------------------------------------------------------------------------
	bbool GameModeController::isPlayerCanChangeToCurMode( Player * _player )
	{
		BaseObject * obj = GETOBJECT( _player->getActorForMode( m_gameMode ) );
		if ( obj == NULL )
		{
			return bfalse;
		}		
		return btrue;
	}

	//---------------------------------------------------------------------------------------------------------
	bbool GameModeController::requestPlayerModeChange( Player * _player, bbool _ifnotRegisterReq )
	{
		if ( isPlayerCanChangeToCurMode( _player ) )
			return btrue;

		// register player to mode change 
		if ( _ifnotRegisterReq )
		{
			i32 idx = m_PlayerModeChangeRequests.find( _player );
			if ( idx < 0 )
			{
				m_PlayerModeChangeRequests.push_back( _player );
			}
		}		
		return bfalse;
	}
    
    //---------------------------------------------------------------------------------------------------------
    void GameModeController::removePlayerModeChangeRequest( Player * _player )
    {
        i32 idx = m_PlayerModeChangeRequests.find( _player );
        if ( idx >= 0 )
        {
            m_PlayerModeChangeRequests.eraseNoOrder( idx );
        }    
}

	//---------------------------------------------------------------------------------------------------------
	void GameModeController::processPlayerModeChangeRequests()
	{
		// Check player mode change requests
		for( i32 i = m_PlayerModeChangeRequests.size() - 1; i >= 0 ; --i )
		{
			Player * player = m_PlayerModeChangeRequests[i];
			if ( isPlayerCanChangeToCurMode( player ) )
			{
				m_PlayerModeChangeRequests.eraseNoOrder( i );
				GAMEMANAGER->changePlayerGameMode( player, m_gameMode );
			}
		}
	}

    //---------------------------------------------------------------------------------------------------------
    void GameModeController::onPlayerGameModeHasChanged( Player * _player )
    {
        checkGameModeActivation();
    }

    //---------------------------------------------------------------------------------------------------------
    void GameModeController::onPlayerActivationChanged( Player * _player, bbool _startDead )
    {
        // just activated, put the player in the activated gamemode
        //
        if ( ( _player != NULL ) && _player->getActive() && m_active )
        {
            GAMEMANAGER->changePlayerGameMode( _player, m_gameMode );
        }

        checkGameModeActivation();
    }

    //---------------------------------------------------------------------------------------------------------
    void GameModeController::checkGameModeActivation()
    {
        Player * validPlayer = NULL;
        bbool modeValid = btrue; 

        for( u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = GAMEMANAGER->getPlayer( i );
            if ( ( player != NULL ) && player->getActive() )
            {
                validPlayer = player;
                if ( player->getCurrentMode() != m_gameMode )
                {
                    modeValid = bfalse;
                    break;
                }
            }				
        }

        if ( !isActive() && modeValid && ( validPlayer != NULL ) )
        {
            setActive( btrue );

            EventPlayerModeChanged eventGameMode;
            eventGameMode.setMode( m_gameMode );
            eventGameMode.setActivator( validPlayer->getCurrentActor() );
            EVENTMANAGER->broadcastEvent( &eventGameMode );
        }
        else if ( isActive() && ( !modeValid || ( validPlayer == NULL ) ) )
        {
            setActive( bfalse );
        }
    }
    
    //---------------------------------------------------------------------------------------------------------
    void GameModeController::onCheckPointLoaded( ObjectRef _checkPointRef )
    {
        if ( isActive() )
        {
            EventPlayerModeChanged eventGameMode;
            eventGameMode.setMode( m_gameMode );
            EVENTMANAGER->broadcastEvent( &eventGameMode );
        }
    }

	//---------------------------------------------------------------------------------------------------------
	void GameModeController::update()
	{
		processPlayerModeChangeRequests();

        // Check params owners coherency
        if ( m_checkGameModeParamsOwnersCoherency )
        {
            checkGameModeParamsOwnersCoherency();
            m_checkGameModeParamsOwnersCoherency = bfalse;
        }
	}

    //---------------------------------------------------------------------------------------------------------
    // check if registered owners are still valid. if not, remove them.
    // if no more owners are valid, destroy the game mode parameters 
    //
    void GameModeController::checkGameModeParamsOwnersCoherency()
    {
        for( u32 i = 0; i < m_parametersOwnerList.size(); ++i )
        {
            BaseObject * obj = m_parametersOwnerList[i].getObject();
            if ( obj == NULL )
            {
                m_parametersOwnerList.eraseNoOrder(i);
                i--;
            }
        }
        // destroy parameters
        if ( m_parametersOwnerList.size() == 0 )
        {
            onGameModeParametersBeforeDestroy();
            SF_DEL( m_parameters );
        }
    }

    //---------------------------------------------------------------------------------------------------------
    // register the given actor in the given player for the current game mode 
    //
    void GameModeController::gameRegisterPlayerActorForMode( const StringID & _playerID, GameMode _gameMode, bbool _reset )
    {
        for ( u32 i = 0; i < m_registeredPlayerActors.size(); ++i )
        {
            RegisteredPlayer & registeredPlayer = m_registeredPlayerActors[i];

            if ( registeredPlayer.m_playerID == _playerID )
            {
                gameRegisterPlayerActorForModeAndIdx( i, _gameMode, _reset );
                break;
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------
    // register the given register actor (given by index) in the given player for the current game mode 
    //
    void GameModeController::gameRegisterPlayerActorForModeAndIdx( i32 _idx, GameMode _gameMode, bbool _reset )
    {
        if ( ( static_cast<u32>(_idx) >= m_registeredPlayerActors.size() ) || (_idx < 0 ) )
        {
            ITF_ASSERT( 0 );
            return;
        }
        
        RegisteredPlayer & registeredPlayer = m_registeredPlayerActors[_idx];

        Player * player = GAMEMANAGER->getPlayerFromID( registeredPlayer.m_playerID );

        if ( player != NULL )
        {
            player->registerActor( registeredPlayer.m_actor, _gameMode, bfalse, btrue );
            if ( _reset )
            {
                player->resetCurrentRegisteredActors();
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------
    // register the given actor in the given player for the current game mode 
    //
    void GameModeController::gameUnregisterPlayerActorForMode( const StringID & _playerID )
    {
       for ( u32 i = 0; i < m_registeredPlayerActors.size(); ++i )
        {
            RegisteredPlayer & registeredPlayer = m_registeredPlayerActors[i];

            if ( registeredPlayer.m_playerID == _playerID )
            {
                Player * player = GAMEMANAGER->getPlayerFromID( _playerID );

                if ( player != NULL )
                {
                     player->unregisterActor( registeredPlayer.m_actor );
                     player->resetCurrentRegisteredActors();
                }
                break;
            }
        }
    }


    //---------------------------------------------------------------------------------------------------------
    //
   ObjectRef GameModeController::getRegisteredPlayerActorForMode( const StringID & _playerID )
   {
       for ( u32 i = 0; i < m_registeredPlayerActors.size(); ++i )
       {
           RegisteredPlayer & registeredPlayer = m_registeredPlayerActors[i];

           if ( registeredPlayer.m_playerID == _playerID )
           {
               return registeredPlayer.m_actor;
           }
       }

       return ITF_INVALID_OBJREF;
   }

    //---------------------------------------------------------------------------------------------------------
    //
    i32 GameModeController::getRegisteredPlayerIdx( const StringID & _playerID )
    {
        for ( u32 i = 0; i < m_registeredPlayerActors.size(); ++i )
        {
            RegisteredPlayer & registeredPlayer = m_registeredPlayerActors[i];

            if ( registeredPlayer.m_playerID == _playerID )
            {
                return i;
            }
        }
        return -1;
    }
}