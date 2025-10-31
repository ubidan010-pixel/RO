#ifndef _ITF_RAY_BLACKSWARMZONEMANAGER_H_
#define _ITF_RAY_BLACKSWARMZONEMANAGER_H_

#ifndef _ITF_RAY_BLACKSWARMZONE_H_
#include "Rayman/gameplay/Managers/Ray_BlackSwarmZone.h"
#endif //_ITF_RAY_BLACKSWARMZONE_H_

namespace ITF
{
	//**********************************************
	//	Swarm zone manager
	//		Create/Destroy swarm zones on swarm generator register/unregister
	//**********************************************
	class Ray_BlackSwarmZoneManager
	{
		//singleton
	public:
		static void createSingleton();
		static void destroySingleton();
		static Ray_BlackSwarmZoneManager * getSingleton();

	private:
		static Ray_BlackSwarmZoneManager * ms_singleton;


	public:
		//update method
		void update(f32 _dt);

		//swarm generators
		void registerSwarmGenerator(const ActorRef &_generator);
		void unregisterSwarmGenerator(const ActorRef &_generator);

		//repellers
		void registerSwarmRepeller(const ActorRef &_repeller);
		void unregisterSwarmRepeller(const ActorRef &_repeller);

		//spawner
		void registerSwarmSpawner(const ActorRef & _spawner);
		void unregisterSwarmSpawner(const ActorRef & _spawner);

	private:
		//ctor and dtor are private because they're supposed to be called only by the singleton create/destroy functions
		Ray_BlackSwarmZoneManager();
		~Ray_BlackSwarmZoneManager();
		
		//try to find a matching frieze for orphans swarm generators
		void updateOrphansGenerators();
		//try to find a matching swarm zone for orphans swarm spawners
		void updateOrphansSpawners();

		//try to find a matching zone for repellers
		void updateRepellersBelonging();
		void findRepellerSwarmZone(const ActorRef & _repeller);

		//adapt polyline ptr for the given zone (polylines are reallocated on some events...)
		void updateZonePolyline(Ray_BlackSwarmZone * const _zone);
		//destroy zone
		void destroyZone(const Ray_BlackSwarmZone & _zone);


		static const u32 ZONES_MAX_COUNT = 4;
		static const u32 REPELLERS_MAX_COUNT = 16;

		ITF_VECTOR<Ray_BlackSwarmZone> m_zones;
		ActorRefList m_orphansGenerators;
		ActorRefList m_repellers;
		ActorRefList m_orphansSpawners;

	};
}

#endif //_ITF_RAY_BLACKSWARMZONEMANAGER_H_