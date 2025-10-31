#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BLACKSWARMZONEMANAGER_H_
#include "Rayman/gameplay/Managers/Ray_BlackSwarmZoneManager.h"
#endif //_ITF_RAY_BLACKSWARMZONEMANAGER_H_

namespace ITF
{
	//**********************************************************
	//	singleton management
	//**********************************************************

	Ray_BlackSwarmZoneManager * Ray_BlackSwarmZoneManager::ms_singleton = NULL;

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::createSingleton()
	{
		ITF_ASSERT_MSG( ms_singleton == NULL, "Try to create an already existing Ray_BlackSwarmZoneManager singleton." );
		ms_singleton = newAlloc( mId_Gameplay, Ray_BlackSwarmZoneManager );
		ITF_ASSERT_MSG( ms_singleton != NULL, "Create of Ray_BlackSwarmZoneManager singleton has failed." );
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::destroySingleton()
	{
		SF_DEL( ms_singleton );
	}

	//----------------------------------------------------------
	Ray_BlackSwarmZoneManager * Ray_BlackSwarmZoneManager::getSingleton()
	{
		return ms_singleton;
	}

	

	//**********************************************************
	//	swarm zone management
	//**********************************************************

	//----------------------------------------------------------
	Ray_BlackSwarmZoneManager::Ray_BlackSwarmZoneManager()
	{
		m_zones.reserve( ZONES_MAX_COUNT );
		m_orphansGenerators.reserve( ZONES_MAX_COUNT );
		m_repellers.reserve( REPELLERS_MAX_COUNT );
	}

	//----------------------------------------------------------
	Ray_BlackSwarmZoneManager::~Ray_BlackSwarmZoneManager()
	{
		m_zones.clear();
		m_orphansGenerators.clear();
		m_repellers.clear();
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::update(f32 _dt)
	{
		//check zones polyline validity
		ITF_VECTOR<Ray_BlackSwarmZone>::iterator it = m_zones.begin();
		while(it != m_zones.end())
		{
			updateZonePolyline( &(*it) );
			//if polyline is still valid
			if( (*it).zoneIsValid() )
			{
				//next zone
				++it;
			}
			else
			{
				//destroy swarm zone
                destroyZone(*it);
				it = m_zones.erase(it);
			}
		}
		
		//update generator <-> zone
		updateOrphansGenerators();

		//update spawner <-> zone
		updateOrphansSpawners();

		//update repeller <-> zone
		updateRepellersBelonging();

		//update zones
		for(u32 index = 0; index < m_zones.size(); ++index)
		{
			m_zones[index].update(_dt);
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::registerSwarmGenerator(const ActorRef &_generator)
	{
		m_orphansGenerators.push_back( _generator );
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::unregisterSwarmGenerator(const ActorRef &_generator)
	{
		//look for generator in orphans generators list
		i32 generatorIndex = m_orphansGenerators.find( _generator );
		if( generatorIndex != -1 )
		{
			//remove generator from orphans list
			m_orphansGenerators.eraseNoOrder( generatorIndex );
			return;
		}

		//generator has not been found in orphans list, look for it in created swarm zones
		ITF_VECTOR<Ray_BlackSwarmZone>::iterator it = m_zones.begin();
		while(it != m_zones.end())
		{
			if( (*it).getGenerator() == _generator )
			{
				//remove swarm zone
				it = m_zones.erase(it);
				return;
			}
			else
			{
				++it;
			}
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::registerSwarmRepeller(const ActorRef &_repeller)
	{
		m_repellers.push_back( _repeller );
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::unregisterSwarmRepeller(const ActorRef &_repeller)
	{
		i32 index = m_repellers.find( _repeller );
		if( index != -1 )
		{
			m_repellers.eraseNoOrder( index );
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::registerSwarmSpawner(const ActorRef &_spawner)
	{
		m_orphansSpawners.push_back( _spawner );
        //const Actor * const actor = static_cast<Actor *>( _spawner.getObject() );
        //LOG("Spawner registered : %s", actor->getUserFriendly().cStr());
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::unregisterSwarmSpawner(const ActorRef &_spawner)
	{
		//look for generator in orphans generators list
		i32 spawnerIndex = m_orphansSpawners.find( _spawner );
		if( spawnerIndex != -1 )
		{
			//remove generator from orphans list
			m_orphansSpawners.eraseNoOrder( spawnerIndex );
            //const Actor * const actor = static_cast<Actor *>( _spawner.getObject() );
            //LOG("Spawner unregistered from orphans list : %s", actor->getUserFriendly().cStr());
			return;
		}

		//generator has not been found in orphans list, remove it from zones
		ITF_VECTOR<Ray_BlackSwarmZone>::iterator it = m_zones.begin();
		while(it != m_zones.end())
		{
			(*it).removeSpawner(_spawner);
            //const Actor * const actor = static_cast<Actor *>( _spawner.getObject() );
            //LOG("Spawner unregistered from zone : %s", actor->getUserFriendly().cStr());
			++it;
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::updateOrphansGenerators()
	{
		for(u32 index = 0; index < m_orphansGenerators.size(); ++index)
		{
			//get generator's actor
			const Actor * const generator = m_orphansGenerators[index].getActor();
			if( generator != NULL )
			{
				//look for a polyline zone including the generator
				const Vec2d &generatorPosition = generator->get2DPos();
				DepthRange generatorDepth = generator->getDepth();
				PolyLine * polyline = AIUtils::getPolylineRegion( generatorDepth, "swarm_region", generatorPosition );
				if( polyline != NULL )
				{
					//create swarm zone
					m_zones.push_back( Ray_BlackSwarmZone( m_orphansGenerators[index], polyline->getRef() ) );

					//remove generator from orphan list
					m_orphansGenerators.eraseKeepOrder( index );
					--index;
				}
			}
			else
			{
				//invalid actor => remove it from orphans list
				m_orphansGenerators.eraseKeepOrder( index );
				--index;
			}
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::updateOrphansSpawners()
	{
		//for each orphans
		for(u32 spawnerIndex = 0; spawnerIndex < m_orphansSpawners.size(); ++spawnerIndex)
		{
			if( m_orphansSpawners[spawnerIndex].isValid() )
			{
				//get spawner actor
				const Actor * const actor = m_orphansSpawners[spawnerIndex].getActor();
				if( actor != NULL )
				{
					//for each existing zone
					for(u32 zoneIndex = 0; zoneIndex < m_zones.size(); ++zoneIndex)
					{
						//get zone polyline
						ITF_ASSERT_MSG(  m_zones[zoneIndex].getPolyLine().isValid(), "Registered black swarm zone with an invalid polyline ref" );
						const PolyLine * const polyline = static_cast<PolyLine *>(  m_zones[zoneIndex].getPolyLine().getObject() );
						ITF_ASSERT_MSG( polyline != NULL, "Registered black swarm zone with a null polyline" );
						if( polyline != NULL )
						{
							if( polyline->isPointInside( actor->get2DPos() ) )
							{
								//add spawner to zone
								m_zones[zoneIndex].addSpawner( m_orphansSpawners[spawnerIndex] );
                                //LOG("Spawner added to zone : %s", actor->getUserFriendly().cStr());
								//remove spawner
								m_orphansSpawners.eraseKeepOrder(spawnerIndex);
								--spawnerIndex;
								//break to stop parsing zones
								break;
							}
						}
					}
				}
				else
				{
					//remove spawner
					m_orphansSpawners.eraseKeepOrder(spawnerIndex);
					--spawnerIndex;
				}
			}
			else
			{
				//remove spawner
				m_orphansSpawners.eraseKeepOrder(spawnerIndex);
				--spawnerIndex;
			}
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::updateRepellersBelonging()
	{
		//clear repellers previous belonging
		for(u32 index = 0; index < m_zones.size(); ++index)
		{
			m_zones[index].clearRepellers();
		}

		//for each reppeler try to find a zone to belong
		for(u32 index = 0; index < m_repellers.size(); ++index)
		{
			findRepellerSwarmZone( m_repellers[index] );
		}
	}


	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::findRepellerSwarmZone(const ActorRef & _repeller)
	{
		const Actor * const repeller = _repeller.getActor();
		ITF_ASSERT_MSG( repeller != NULL, "A null repeller actor is registered." );
		if( repeller != NULL )
		{
			//get repeller position
			const AABB & repellerAABB = repeller->getAABB();

			//iterate through zones until overlapping is detected
			for(u32 index = 0; index < m_zones.size(); ++index)
			{
				//get zone polyline
				ITF_ASSERT_MSG(  m_zones[index].getPolyLine().isValid(), "Registered black swarm zone with an invalid polyline ref" );
				const PolyLine * const polyline = static_cast<PolyLine *>(  m_zones[index].getPolyLine().getObject() );
				ITF_ASSERT_MSG( polyline != NULL, "Registered black swarm zone with a null polyline" );

				//check AABB intersection
				if( repellerAABB.checkOverlap( polyline->getAABB() ) )
				{
					m_zones[index].registerRepeller( _repeller );
				}
			}
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::updateZonePolyline(Ray_BlackSwarmZone *const _zone)
	{
		//check that zone polyline is still valid
		const Actor * const generator = _zone->getGenerator().getActor();
		ITF_ASSERT_MSG( generator != NULL, "Registered swarm zone with a null generator actor" );
		const Vec2d &generatorPosition = generator->get2DPos();
		const DepthRange &generatorDepth = generator->getDepth();
		PolyLine * polyline = AIUtils::getPolylineRegion( generatorDepth, "swarm_region", generatorPosition );
		if( polyline != NULL )
		{
			_zone->setPolyLine( polyline->getRef() );
		}
		else
		{
			_zone->setPolyLine( ObjectRef::InvalidRef );
		}
	}

	//----------------------------------------------------------
	void Ray_BlackSwarmZoneManager::destroyZone(const Ray_BlackSwarmZone &_zone)
	{
        //readd generator to orphans list
		m_orphansGenerators.push_back( _zone.getGenerator() );

		//readd zone's spawners to orphans spawners list
		const ActorRefList & spawners = _zone.getSpawners();
		for(u32 index = 0; index < spawners.size(); ++index)
		{
			registerSwarmSpawner( spawners[index] );
            //const Actor * const actor = static_cast<Actor *>( spawners[index].getObject() );
            //LOG("Spawner readded to orphans list : %s", actor->getUserFriendly().cStr());
		}
	}
}
