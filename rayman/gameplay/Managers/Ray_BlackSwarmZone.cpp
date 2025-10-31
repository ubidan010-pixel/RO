#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BLACKSWARMZONE_H_
#include "Rayman/gameplay/Managers/Ray_BlackSwarmZone.h"
#endif //_ITF_RAY_BLACKSWARMZONE_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_
#include "Rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmRepellerComponent.h"
#endif //_ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_

#ifndef _ITF_RAY_BLACKSWARMCOMPONENT_H_
#include "Rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmComponent.h"
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_H_

#ifndef _ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_
#include "Rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmSpawnerComponent.h"
#endif //_ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_POWERUPMANAGER_H_
#include "rayman/gameplay/Ray_PowerUpManager.h"
#endif //_ITF_RAY_POWERUPMANAGER_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{
	//----------------------------------------------------------
	Ray_BlackSwarmZone::Ray_BlackSwarmZone(const ActorRef & _generator, const ObjectRef & _polyline)
	:	m_generator(_generator),
		m_generatorAIComponent(NULL),
		m_polyline(_polyline)
	{
		//get ptr to generator AI component
		const Actor * const generator = m_generator.getActor();
		ITF_ASSERT_MSG( generator != NULL, "Swarm zone with a null generator actor." );
		m_generatorAIComponent = generator->GetComponent<Ray_BlackSwarmComponent>();
		//init component polyline
		m_generatorAIComponent->setContainerPolyline( _polyline );
	}

	//----------------------------------------------------------
	Ray_BlackSwarmZone::~Ray_BlackSwarmZone()
	{
        for(u32 index  = 0; index < m_playerZoneData.size(); ++index)
        {
            //destroy player protection
            destroyPlayerProtection( &m_playerZoneData[index] );
        }
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::setPolyLine(const ObjectRef &_polyline)
	{
		if( _polyline != m_polyline )
		{
			//init component polyline
			m_generatorAIComponent->setContainerPolyline( _polyline );
			//store polyline
			m_polyline = _polyline;
		}
	}

	//----------------------------------------------------------
	bbool Ray_BlackSwarmZone::zoneIsValid() const
	{
		//check generator validity
		if( m_generator.isValid() )
		{
			if( m_generator.getActor() != NULL )
			{
				if( m_polyline.isValid() )
				{
					if( m_polyline.getObject() != NULL )
					{
						return btrue;
					}
				}
			}
		}
		return bfalse;
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::registerRepeller(const ActorRef & _repeller)
	{
		m_repellers.push_back(_repeller);
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::clearRepellers()
	{
		m_repellers.clear();
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::addSpawner(const ActorRef &_spawner)
	{
		//store spawner in list
		m_spawners.push_back( _spawner );
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::removeSpawner(const ActorRef &_spawner)
	{
		//remove spanwer from list
		u32 index = m_spawners.find( _spawner );
		if(index != - 1)
		{
			m_spawners.eraseNoOrder( index );
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::update(const f32 _dt)
	{
		//detect PCs
		detectPCs();

		//check PC pafs
		checkPCPaf(_dt);

		//assign zone actors to swarm component
		updateSwarmComponentTargets();
		updateSwarmComponentRepellers();
		updateSwarmComponentSpawners();
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::updateSwarmComponentTargets()
	{
		//manage swarm target
        const f32 anticipationFactor = m_generatorAIComponent->getPlayerMovementAnticipationFactor();
	    m_generatorAIComponent->clearTargets();
	    for(u32 index = 0; index < m_detectedPCs.size(); ++index)
	    {
		    const Actor * const actor = m_detectedPCs[index].getActor();
		    ITF_ASSERT( actor != NULL );
		    //get player position
		    const Vec2d & playerPosition = actor->get2DPos();
		    //get player speed
		    PhysComponent * playerPhysComponent = actor->GetComponent<PhysComponent>();
		    //if player is protected
		    if( !isPlayerInDanger(m_detectedPCs[index]) )
		    {
			    //anticipate player position (add speed offset)
			    Vec2d targetSpeedOffset =  playerPhysComponent->getSpeed() * anticipationFactor;
			    m_generatorAIComponent->addTarget( playerPosition + targetSpeedOffset );
		    }
		    else
		    {
			    m_generatorAIComponent->addTarget( playerPosition );
		    }
	    }
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::updateSwarmComponentRepellers()
	{
		//manage swarm generator repellers
		m_generatorAIComponent->clearRepellers();
		for(u32 index = 0; index < m_repellers.size(); ++index)
		{
			const Actor * const repeller = m_repellers[index].getActor();
			ITF_ASSERT_MSG( repeller != NULL, "Swarm zone : a null repeller actor is registered" );
			if( repeller != NULL )
			{
				Ray_BlackSwarmRepellerAIComponent * const component = repeller->GetComponent<Ray_BlackSwarmRepellerAIComponent>();
				ITF_ASSERT_MSG( component != NULL, "Swarm zone : a repeller actor with a null Ray_BlackSwarmComponent is registered" );
				if( component != NULL )
				{
					const f32 radius = component->getRepelLogicalRadius();
					const Vec2d &position = component->getRepelCenter();
					m_generatorAIComponent->addRepeller( position, radius );
				}
			}
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::updateSwarmComponentSpawners()
	{
		//manage swarm generator spawners
		m_generatorAIComponent->clearSpawners();
		for(u32 index = 0; index < m_spawners.size(); ++index)
		{
			const Actor * const spawner = m_spawners[index].getActor();
			ITF_ASSERT_MSG( spawner != NULL, "Swarm zone : a null repeller actor is registered" );
			if( spawner != NULL )
			{
				Ray_BlackSwarmSpawnerComponent * const component = spawner->GetComponent<Ray_BlackSwarmSpawnerComponent>();
				ITF_ASSERT_MSG( component != NULL, "Swarm zone : a repeller actor with a null Ray_BlackSwarmComponent is registered" );
				if( component != NULL )
				{
					m_generatorAIComponent->addSpawner( spawner->get2DPos(), component->getActivationDistance() );
				}
			}
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::detectPCs()
	{
		//clear previous detected PCs list
		m_detectedPCs.clear();

		//get PC list
		ITF_ASSERT_MSG( m_polyline.isValid(), "Registered black swarm zone with an invalid polyline ref" );
		const PolyLine * const polyline = static_cast<PolyLine *>( m_polyline.getObject() );
		ITF_ASSERT_MSG( polyline != NULL, "Registered black swarm zone with a null polyline" );
		const DepthRange & zoneDepth = polyline->getDepth();
		ActorList playerCharacters;
		AIUtils::getPlayers(zoneDepth, playerCharacters);

		//get polyline AABB
		const AABB & polylineAABB = polyline->getAABB();
		//get detectionDistance
		const f32 detectionDistance = m_generatorAIComponent->getDetectionRadius();

		//for each character
		for(u32 index = 0; index < playerCharacters.size(); ++index)
		{
			//get actor from actor ref
			const Actor * const player = playerCharacters[index];
			ITF_ASSERT_MSG( player != NULL, "A referenced player's actor is null." );

			//get character's AABB
			const AABB & playerAABB = player->getAABB();

			//check AABBs collision using detection distance as tolerance
			if( polylineAABB.checkOverlap( playerAABB, detectionDistance ) )
			{
				m_detectedPCs.push_back( player->getRef() );
			}
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZone::checkPCPaf(const f32 _dt)
	{
		ITF_ASSERT_MSG( m_polyline.isValid(), "Registered black swarm zone with an invalid polyline ref" );
		const PolyLine * const polyline = static_cast<PolyLine *>( m_polyline.getObject() );
		ITF_ASSERT_MSG( polyline != NULL, "Registered black swarm zone with a null polyline" );

		//clear player zone data danger flag
		clearPlayerZoneDataValidFlag();

		const f32 maxTimeInZone = m_generatorAIComponent->getPlayerMaxTimeInZone();

		//check PCs in detected PCs list
		for(u32 index = 0; index < m_detectedPCs.size(); ++index)
		{
			Actor * const player = m_detectedPCs[index].getActor();
			ITF_ASSERT_MSG( player != NULL, "A referenced player's actor is null." );

			//check if player is in zone
			const Vec2d & playerPosition = player->get2DPos();
			if( polyline->isPointInside( playerPosition ) )
			{
                PlayerZoneData * playerData = getPlayerZoneData( m_detectedPCs[index] );
                if( playerData == NULL )
                {
                    m_playerZoneData.push_back( PlayerZoneData(m_detectedPCs[index], 0.0f, bfalse) );
                    playerData = &(m_playerZoneData.back());
                }
                //player data is valid
                playerData->m_valid = btrue;

				//check if player is not protected by a swarm repeller
                SafeArray<const Actor *> repellers;
				if( !isPositionProtected( playerPosition, repellers ) )
				{
                    if( playerData->m_time < maxTimeInZone )
				    {
					    playerData->m_time += _dt;
				    }
				    else
				    {
                        Player * typedPlayer = RAY_GAMEMANAGER->getPlayerFromActor(m_detectedPCs[index]);
                        ITF_ASSERT_MSG( typedPlayer != NULL, "A referenced player is null." );

                        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
                        bbool heartPowerUp = powerUpManager.isEnabled( Ray_PowerUp_HeartShield, typedPlayer->getIndex() );
                        bbool validLastRepeller = playerData->m_lastRepeller.isValid();
                        if( heartPowerUp )
                        {
                            if( validLastRepeller )
                            {
                                sendBounce(player, playerData->m_lastRepeller);
                            }
                            else
                            {
                                sendStim(playerPosition, RECEIVEDHITTYPE_HURTBOUNCE, Vec2d::Up);
                            }
                        }
                        else
                        {
                            sendStim(playerPosition, RECEIVEDHITTYPE_HURTBOUNCE);
                        }

                        //reset time
                        playerData->m_time = -1.0f;
                        createPlayerProtection(playerData, playerPosition);
				    }
				}
                else
                {
                    //in order to keep the "hit before" time bonus do not reset time to 0.0f instantly
                    //playerData->m_time = Min( playerData->m_time + _dt, 0.0f );
                    //reset timer...
                    playerData->m_time = 0.0f;

                    //clear last repeller
                     playerData->m_lastRepeller.invalidate();
                    
                    //look for a trustable repeller in protecting repellers
                    for(u32 index = 0; index < repellers.size(); ++index)
                    {
                        if( repellers[index] != NULL )
                        {
                            //only if repeller is not periodic or interactive or power up or...
                            Ray_BlackSwarmRepellerAIComponent * repellerComponent = repellers[index]->GetComponent<Ray_BlackSwarmRepellerAIComponent>();
                            if( repellerComponent->isTrustable() )
                            {
                                playerData->m_lastRepeller = repellers[index]->getRef();
                            }

                            //if PC is protecting by another repeller than the spawned one => destroy it
                            if( repellers[index]->getRef() != playerData->m_playerProtection )
                            {
                                destroyPlayerProtection( playerData );
                            }
                        }
                    }
                }
            
                //update player protection
                updatePlayerProtection(playerData, playerPosition);
			}
		}

        //clean obsolete player zone data
        cleanPlayerZoneData();
	}

    //----------------------------------------------------------
    void Ray_BlackSwarmZone::sendBounce(Actor * const _player, const ActorRef _bounceTarget) const
    {
        Ray_EventBounceToLayer bounceEvent(
            0.f,
            0.f,
            2.f,
            _bounceTarget,
            Vec2d::Zero,
            1);
        _player->onEvent(&bounceEvent);
    }

    //----------------------------------------------------------
    void Ray_BlackSwarmZone::sendStim(const Vec2d & _position, const EReceivedHitType _hitType, const Vec2d _direction/* = Vec2d::Zero*/) const
    {
        ITF_ASSERT( m_generator.isValid() );
        const Actor * const generator = m_generator.getActor();
        ITF_ASSERT( generator != NULL );

        PhysShapeCircle pafShape(0.25f);
        PunchStim * stim = STIMSMANAGER->requestStim<PunchStim>(&pafShape);

        Faction swarmFaction = AIUtils::getFaction(generator);

        stim->setSender(m_generator);
        stim->setDepth(generator->getDepth());
        stim->setPos(_position);
        stim->setPrevPos(_position);
        stim->setDirection(_direction);
        stim->setLevel(1);
        stim->setReceivedHitType(_hitType);
        stim->setAngle(0.0f);
        stim->setFaction(swarmFaction);        
        STIMSMANAGER->sendStim(stim);
    }

	//----------------------------------------------------------
	const Ray_BlackSwarmZone::PlayerZoneData * Ray_BlackSwarmZone::getPlayerZoneDataConst(const ActorRef &_actor) const
	{
		for(u32 index = 0; index < m_playerZoneData.size(); ++index)
		{
			if( m_playerZoneData[index].m_player == _actor )
			{
				return &m_playerZoneData[index];
			}
		}
		return NULL;
	}

	//----------------------------------------------------------
	Ray_BlackSwarmZone::PlayerZoneData * Ray_BlackSwarmZone::getPlayerZoneData(const ActorRef &_actor)
	{
		const PlayerZoneData * const data = getPlayerZoneDataConst(_actor);
		return const_cast<PlayerZoneData *>(data);
	}

	//----------------------------------------------------------
	bbool Ray_BlackSwarmZone::isPlayerInDanger(const ActorRef &_player) const
	{
		const PlayerZoneData * const data = getPlayerZoneDataConst(_player);
		return( data != NULL );
	}

	//----------------------------------------------------------
	bbool Ray_BlackSwarmZone::isPositionProtected(const Vec2d & _position, SafeArray<const Actor *> &_repellers) const
	{
        _repellers.clear();
		for(u32 index = 0; index < m_repellers.size(); ++index)
		{
			const Actor * const repeller = m_repellers[index].getActor();
			ITF_ASSERT_MSG( repeller != NULL, "A null repeller actor is registered." );
			if( repeller != NULL )
			{
				const Ray_BlackSwarmRepellerAIComponent * const component = repeller->GetComponent<Ray_BlackSwarmRepellerAIComponent>();
				ITF_ASSERT_MSG( component != NULL, "A repeller actor with a null Ray_BlackSwarmRepellerAIComponent is registered." );
				if( component != NULL )
				{
					//get repel actor position
					const Vec2d &repelPosition = component->getRepelCenter();
					//compute distance from _position to repel actor
					f32 sqrDistance = (repelPosition - _position).sqrnorm();
					//get repel radius
					f32 repelRadius = component->getRepelLogicalRadius();
					//sqr radius
					repelRadius *= repelRadius;
					//check if repel actor is protecting position
					if( sqrDistance < repelRadius )
					{
                        _repellers.push_back(repeller);
					}
				}
			}
		}
		if (_repellers.size() > 0)
        {
            return btrue;
        }
        else
        {
            return !m_generatorAIComponent->getSwarmPresence(_position);
        }
	}

    //----------------------------------------------------------
    void Ray_BlackSwarmZone::clearPlayerZoneDataValidFlag()
    {
        for(u32 index = 0; index < m_playerZoneData.size(); ++index)
        {
            m_playerZoneData[index].m_valid = bfalse;
        }
    }

    //----------------------------------------------------------
    void Ray_BlackSwarmZone::cleanPlayerZoneData()
    {
        u32 index = 0;
        while(index < m_playerZoneData.size())
        {
            //if data is valid
            if( m_playerZoneData[index].m_valid )
            {
                //iterate
                ++index;
            }
            else
            {
                //destroy player protection
                destroyPlayerProtection( &m_playerZoneData[index] );
                //remove data
                m_playerZoneData.eraseKeepOrder( index );
            }
        }
    }

    //----------------------------------------------------------
    void Ray_BlackSwarmZone::createPlayerProtection(PlayerZoneData *const _playerData, const Vec2d & _position)
    {
        //destroy previous protection
        //destroyPlayerProtection( _playerData );

        if( m_generatorAIComponent != NULL )
        {
            if( !_playerData->m_playerProtection.isValid() )
            {
                _playerData->m_playerProtection = m_generatorAIComponent->spawnPlayerProtection(_position);
            }
        }
    }

    //----------------------------------------------------------
    void Ray_BlackSwarmZone::updatePlayerProtection(PlayerZoneData *const _playerData, const Vec2d & _position)
    {
        if( _playerData->m_playerProtection.isValid() )
        {
            Actor * const repeller = _playerData->m_playerProtection.getActor();
            if( repeller != NULL )
            {
                const Ray_BlackSwarmRepellerAIComponent * const component = repeller->GetComponent<Ray_BlackSwarmRepellerAIComponent>();
                ITF_WARNING( repeller, component != NULL, "Repeller with a null swarm repeller component" );
                if( component != NULL )
                {
                    //if repeller is finished
                    if( component->isFinished() )
                    {
                        //destroy it
                        destroyPlayerProtection( _playerData );
                    }
                    else if( m_generator.isValid() )
                    {
                        //move repeller
                        Actor * const generator = m_generator.getActor();
                        if( generator != NULL )
                        {
                            const Vec3d repellerPosition(_position.m_x, _position.m_y, generator->getDepth() );
                            repeller->setPos( repellerPosition );
                        }
                    }
                }
            }
        }
    }

    //----------------------------------------------------------
    void Ray_BlackSwarmZone::destroyPlayerProtection(PlayerZoneData *const _playerData)
    {
        if( _playerData->m_playerProtection.isValid() )
        {
            Actor * const repeller = _playerData->m_playerProtection.getActor();
            if( repeller != NULL )
            {
                repeller->requestDestruction();
            }
        }
        _playerData->m_playerProtection.invalidate();
    }
}