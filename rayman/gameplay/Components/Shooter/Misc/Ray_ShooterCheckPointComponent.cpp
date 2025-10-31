#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Misc/Ray_ShooterCheckPointComponent.h"
#endif //_ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SHAPECOMPONENT_H_
#include "gameplay/Components/Misc/ShapeComponent.h"
#endif //_ITF_SHAPECOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{   
    //--------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterCheckPointComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterCheckPointComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_CONTAINER("SpawnPos",m_spawnPositions);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()
    
    Ray_ShooterCheckPointComponent::Ray_ShooterCheckPointComponent()
    : Super()
    , m_linkComponent( NULL )
    {
        const u32 uCount = RAY_GAMEMANAGER->getMaxPlayerCount();
        for(u32 i = 0; i < uCount; ++i)
            m_spawnPositions.push_back(Vec2d::Zero);

        m_playersInChpZone.resize( RAY_GAMEMANAGER->getMaxPlayerCount() );
        m_playersInChpZone.fill( 0 );
    }

    Ray_ShooterCheckPointComponent::~Ray_ShooterCheckPointComponent()
    {
        if (m_actor)
        {
            RAY_GAMEMANAGER->getShooterGameModeController()->unregisterCheckPoint( m_actor->getRef() );
            RAY_GAMEMANAGER->getShooterGameModeController()->unregisterGameModeParametersOwner( m_actor->getRef() );
        }
    }


	//------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCheckPointComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        RAY_GAMEMANAGER->getShooterGameModeController()->registerCheckPoint( m_actor->getRef() );

        if ( getTemplate()->getShooterGameModeParameters() != NULL )
        {
            RAY_GAMEMANAGER->getShooterGameModeController()->registerGameModeParameters( getTemplate()->getShooterGameModeParameters(), m_actor->getRef() );
        }
        
        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        if ( AnimLightComponent * animComp = m_actor->GetComponent<AnimLightComponent>() )
        {
            animComp->setScaleMultiplier( getTemplate()->getVisualScaleMultiplier() );
        }
	}

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCheckPointComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        // if the current check point is me, disable linked flags
        //
        if ( ( m_linkComponent != NULL ) && ( RAY_GAMEMANAGER->getShooterGameModeController()->getActiveCheckPoint() == m_actor->getRef() ) )
        {
            const LinkComponent::ChildrenList& childrenlist = m_linkComponent->getChildren();
            for ( u32 i = 0; i < childrenlist.size(); ++i )
            {
                const ChildEntry & child = childrenlist[i];
                if ( child.hasTag( ITF_GET_STRINGID_CRC(flag,3727357253) ) )
                {
                    if ( BaseObject * base_flag =  m_linkComponent->getChildObject( child.getPath() ) )
                    {
                        if ( Actor * actor_flag = base_flag->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                        {
                            actor_flag->disable();
                        }
                    }
                }            
            }
        }

    }    
    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCheckPointComponent::checkpointReached( ObjectRef _player )
    {
        //specific case, do not save if enter / exit point
        //
        if ( getTemplate()->getEnterExitMode() )
            return;

        bbool isCameraActivator = btrue;
        if ( GAMEMANAGER->isPlayerActor(_player) )
        {
            isCameraActivator = bfalse;
            Player * activatorPlayer = RAY_GAMEMANAGER->getPlayerFromActor( _player, bfalse );
            if ( ( activatorPlayer == NULL ) || activatorPlayer->isDead() )
            {
                return;
            }
        }        

        if ( RAY_GAMEMANAGER->getShooterGameModeController()->isActive() )
        {
            for( u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); ++i )
            {
                Player * player = RAY_GAMEMANAGER->getPlayer( i );
                if ( ( player != NULL ) && player->getActive() )
                {
                    // No enter exit mode, save actor positions
                    if ( !( getTemplate()->getEnterExitMode() || getTemplate()->getExitOnlyMode() ) )
                    {
                        Actor * playerAct = player->getActor();
                        ITF_ASSERT( playerAct != NULL );
                        if ( playerAct != NULL )
                        {
                            m_spawnPositions[i] = playerAct->get2DPos();
                        }
                    }
                    else
                    {
                        m_spawnPositions[i] = Vec2d::Zero;
                    }
                }
            }
            RAY_GAMEMANAGER->onReachedCheckpoint( GetActor()->getRef() );

            RAY_GAMEMANAGER->getShooterGameModeController()->setActiveCheckPoint( m_actor->getRef(), isCameraActivator );

            // send event to children flag
            //
            if ( m_linkComponent != NULL )
            {
                EventActivate activate;
                activate.setSender( m_actor->getRef() );
                m_linkComponent->sendEventToChildren( &activate, ITF_GET_STRINGID_CRC(flag,3727357253) );
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    Vec2d Ray_ShooterCheckPointComponent::getPlayerSpawnPos( u32 _playerIndex ) const
    {
        Vec2d spawnPos = m_actor->get2DPos();

        Player * player = GAMEMANAGER->getPlayer( _playerIndex );
        if ( player == NULL )
            return spawnPos;

        if ( getTemplate()->useCameraBorderSpawn() && ( player->getCurrentMode() == RAY_GAMEMODE_SHOOTER ) )
        {
            u32 playersCount = GAMEMANAGER->getNumActivePlayers();
            // compute camera borders to be sure having the good size (the aabb may have not been updated)
            //
            ICameraController* camController = CAMERACONTROLLERMANAGER->getMainCameraController();
            if ( camController != NULL )
            {
                f32 camHeight = tanf( camController->getCameraFocale() * 0.5f ) * camController->getDepth();
                f32 camWidth = camHeight * CAMERACONTROLLERMANAGER->m_screenRatio;

                Vec2d sideInitPos = camController->getCameraPosition().truncateTo2D();
                sideInitPos -= Vec2d::Right * ( camWidth - getTemplate()->getCameraBorderSpawnOffset() );
                sideInitPos += Vec2d::Up * ( ( getTemplate()->getCameraBorderSpawnPlayersOffset() * 0.5f ) * f32( playersCount - 1 ) );

                spawnPos = sideInitPos - ( Vec2d::Up * f32(_playerIndex) * getTemplate()->getCameraBorderSpawnPlayersOffset() );
            }
        }
        else 
        {
            Vec2d targetPos;
            if ( m_linkComponent != NULL )
            {
                targetPos = Super::getPlayerSpawnPos( _playerIndex );
            }

             //  has children dummy positions ?
            if ( ( m_linkComponent != NULL ) && !targetPos.IsEqual( spawnPos ) )
            {
                return targetPos;
            }
            else if ( !m_spawnPositions[_playerIndex].IsEqual( Vec2d::Zero ) )
            {
                Vec2d vec = ( m_spawnPositions[_playerIndex] - spawnPos );
                vec.m_x = 0.f;
                spawnPos += vec;
            }
            else
            {
                const Vec2d & addPos = getTemplate()->getPlayerSpawnPos( RAY_GAMEMANAGER->getPlayer( _playerIndex )->getID() );
                if ( !addPos.IsEqual( Vec2d::Zero ) )
                {
                    spawnPos += addPos;
                }
            }
        }       
        
        return spawnPos;
    }
	
	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterCheckPointComponent::onEvent( Event * _event)
	{
        if (EventTrigger* onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if ( isActive() && onTrigger->getActivated() )
            {
                ObjectRef activator = onTrigger->getActivator();
                ICameraController* cameraController = CAMERACONTROLLERMANAGER->getMainCameraController();
                if ( ( cameraController != NULL ) && ( cameraController->getActorRef() == activator ) )
                {
                    checkpointReached(activator);
                    return;
                }
            }
        }

        Super::onEvent( _event );
	}

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCheckPointComponent::Update( f32 _deltaTime )
    {
        bbool curCheckPointIsExit = bfalse;

        Actor * activeCheckPoint = AIUtils::getActor( RAY_GAMEMANAGER->getShooterGameModeController()->getActiveCheckPoint() );
        
        if ( activeCheckPoint != NULL )
        {
            if ( Ray_ShooterCheckPointComponent * activeCheckPointComp = activeCheckPoint->GetComponent<Ray_ShooterCheckPointComponent>() )
            {
                curCheckPointIsExit = activeCheckPointComp->isExitMode();
            }
        }
        
        // Compute actor platformer detection with shooter actors 
        for( u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            Player * player = RAY_GAMEMANAGER->getPlayer( i );
            if ( ( player != NULL ) && player->getActive() )
            {
                // Check collisions between platformer / shooter actors
                //
                if (    ( player->getCurrentMode() != RAY_GAMEMODE_SHOOTER ) && 
                        (   (   !getTemplate()->getExitOnlyMode() && ( activeCheckPoint == m_actor ) ) ||
                            (   getTemplate()->getEnterExitMode() && !curCheckPointIsExit )
                        )
                    )
                {
                    if ( isPlayerColliding( player ) )
                    {
                        if ( !RAY_GAMEMANAGER->getShooterGameModeController()->isPlayerInMoskitoZone( i ) )
                        {
                            RAY_GAMEMANAGER->changePlayerGameMode( player, RAY_GAMEMODE_SHOOTER );
                        }
                        RAY_GAMEMANAGER->getShooterGameModeController()->setPlayerInMoskitoZone( i, btrue );
                    }
                    else
                    {
                        RAY_GAMEMANAGER->getShooterGameModeController()->setPlayerInMoskitoZone( i, bfalse );
                    }
                }
                else if ( ( player->getCurrentMode() == RAY_GAMEMODE_SHOOTER ) && ( getTemplate()->getEnterExitMode() || getTemplate()->getExitOnlyMode() ) )
                {
                    if ( isPlayerColliding( player ) )
                    {
                        if ( !m_playersInChpZone[i] )
                        {
                            EventTrigger evt;
                            evt.setSender( player->getCurrentActor() );
                            evt.setActivator( player->getCurrentActor() );
                            evt.setActivated( btrue );
                            m_actor->onEvent(&evt);
                            
                            if ( getTemplate()->getExitOnlyMode() )
                            {
                                startLanding( NULL, btrue );
                            }
                            else
                            {
                                startLanding( player, bfalse );
                            }
                        }
                        m_playersInChpZone[i] = btrue;
                    }
                    else
                    {
                        m_playersInChpZone[i] = bfalse;
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterCheckPointComponent::isPlayerColliding( Player * _player )
    {
        if (_player->isDead())
        {
            return bfalse;
        }

        Actor* playerAct = _player->getActor();
        if (!playerAct)
        {
            ITF_ASSERT( playerAct != NULL );
            return bfalse;
        }

        PlayerControllerComponent* playerController = playerAct->GetComponent<PlayerControllerComponent>();
        if (!playerController)
        {
            ITF_ASSERT( playerController != NULL );
            return bfalse;
        }

        if (!playerController->isDetectable(m_actor->getRef()))
        {
            return bfalse;
        }


        //Player * swappedPlayer = RAY_GAMEMANAGER->getPlayer( _player->getIndex( btrue ) );
        Player * swappedPlayer = _player; 
        BaseObject * bswappedPlayerAct = swappedPlayer->getActorForMode( RAY_GAMEMODE_SHOOTER ).getObject();
        if ( bswappedPlayerAct != NULL )
        {
            Actor * swappedPlayerAct = bswappedPlayerAct->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            ITF_ASSERT( swappedPlayerAct != NULL );
            PlayerControllerComponent* swappedPlayerController = swappedPlayerAct ? swappedPlayerAct->GetComponent<PlayerControllerComponent>() : NULL;
            ITF_ASSERT( swappedPlayerController != NULL );
            if ( swappedPlayerController != NULL )
            {
                if ( !swappedPlayerController->isDetectable(m_actor->getRef()) )
                {
                    return bfalse;
                }

                EventCollide evt;

                if ( _player->getCurrentMode() != RAY_GAMEMODE_SHOOTER )
                {
                    // get shooter actor shape
                    EventQueryPhysShape destShapeEvtQuery;
                    swappedPlayerAct->onEvent( &destShapeEvtQuery );

                    if ( destShapeEvtQuery.getPhysShape() != NULL )
                    {
                        evt.setShape( destShapeEvtQuery.getPhysShape() );
                        evt.setPos( destShapeEvtQuery.getPosition() );
                        evt.setAngle( destShapeEvtQuery.getAngle() );
                        playerAct->onEvent(&evt);
                    }
                }
                else if ( ( _player->getCurrentMode() == RAY_GAMEMODE_SHOOTER ) && ( getTemplate()->getEnterExitMode() || getTemplate()->getExitOnlyMode() ) )
                {
                    ShapeComponent * shapeComp = m_actor->GetComponent<ShapeComponent>();
                    ITF_ASSERT( shapeComp != NULL );
                    if ( ( shapeComp == NULL ) || ( shapeComp->getShape() == NULL ) )
                    {
                        return bfalse;
                    }
                    evt.setShape( shapeComp->getShape() );
                    evt.setPos( m_actor->get2DPos() );
                    evt.setAngle( m_actor->getAngle() );
                    swappedPlayerAct->onEvent(&evt);
                }
                return evt.isCollide();
            }
        }

        return bfalse;
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCheckPointComponent::startLanding( Player * _playerToLand, bbool _landAll )
    {
        if ( _landAll )
        {
            if ( !RAY_GAMEMANAGER->getShooterGameModeController()->isActive() )
                return;

            for( u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); ++i )
            {
                Player * player = RAY_GAMEMANAGER->getPlayer( i );
                if ( ( player != NULL ) && player->getActive() )
                {
                    Actor * playerActor = AIUtils::getActor( player->getActorForMode( RAY_GAMEMODE_SHOOTER ) );
                    if ( playerActor != NULL )
                    {
                        Vec2d targetPos = getPlayerSpawnPos( i );

                        Ray_EventShooterLandingRequested landRequested;
                        landRequested.setSender( m_actor->getRef() );
                        landRequested.setLandingPos( targetPos );
                        playerActor->onEvent( &landRequested );
                    }
                }
            }
        }
        else if ( ( _playerToLand != NULL ) && _playerToLand->getActive() )
        {
            Vec2d targetPos = Super::getPlayerSpawnPos( _playerToLand->getIndex() );
            bbool landSeq = btrue;
            // Has target ?
            if ( targetPos.IsEqual( m_actor->get2DPos() ) && getTemplate()->getEnterExitMode() )
            {
                landSeq = bfalse;
            }
            if ( landSeq )
            {
                Actor * playerActor = AIUtils::getActor( _playerToLand->getActorForMode( RAY_GAMEMODE_SHOOTER ) );
                if ( playerActor != NULL )
                {
                    Ray_EventShooterLandingRequested landRequested;
                    landRequested.setSender( m_actor->getRef() );
                    landRequested.setLandingPos( targetPos );
                    playerActor->onEvent( &landRequested );
                }
            }
            else
            {
                RAY_GAMEMANAGER->changePlayerGameMode( _playerToLand, RAY_GAMEMODE_PLATFORMER );
            }
        }
    }
	
    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterCheckPointComponent_Template)
    BEGIN_SERIALIZATION(Ray_ShooterCheckPointComponent_Template)
        SERIALIZE_MEMBER("enterExit", m_enterExit );
        SERIALIZE_MEMBER("exitOnly",  m_exitOnly );
        SERIALIZE_MEMBER("useCameraBorderSpawn",                    m_useCameraBorderSpawn );
        SERIALIZE_MEMBER("cameraBorderSpawnOffset",                 m_cameraBorderSpawnOffset );
        SERIALIZE_MEMBER("cameraBorderSpawnPlayersOffset",          m_cameraBorderSpawnPlayersOffset );
        SERIALIZE_CONTAINER_OBJECT("playersSpawnPosList",           m_playersSpawnPosList );
        SERIALIZE_OBJECT_WITH_FACTORY( "shooterGameModeParameters", m_shooterGameModeParameters, GAMEMANAGER->getGameModeParametersFactory() );
        SERIALIZE_MEMBER("visualScaleMultiplier",                   m_visualScaleMultiplier );
    END_SERIALIZATION()

    Ray_ShooterCheckPointComponent_Template::Ray_ShooterCheckPointComponent_Template()
    : m_shooterGameModeParameters( NULL )
    , m_enterExit( bfalse )
    , m_exitOnly( bfalse )
    , m_useCameraBorderSpawn( bfalse )
    , m_cameraBorderSpawnOffset( 1.0f )
    , m_cameraBorderSpawnPlayersOffset( 1.5f )
    , m_visualScaleMultiplier( 1.0f )
    {		
    }
    Ray_ShooterCheckPointComponent_Template::~Ray_ShooterCheckPointComponent_Template()
    {
        SF_DEL( m_shooterGameModeParameters );
    }

    const Vec2d & Ray_ShooterCheckPointComponent_Template::getPlayerSpawnPos( const StringID _playerID )const
    {
        for ( u32 i = 0; i < m_playersSpawnPosList.size(); i++ )
        {
            const PlayerSpawnPos & data = m_playersSpawnPosList[i];
            if ( data.m_playerID == _playerID )
            {
                return data.m_offset;
            }
        }
        return Vec2d::Zero;
    }

    BEGIN_SERIALIZATION_SUBCLASS( Ray_ShooterCheckPointComponent_Template, PlayerSpawnPos )
        SERIALIZE_MEMBER("playerID", m_playerID);
        SERIALIZE_MEMBER("offset", m_offset);
    END_SERIALIZATION()

    
};
