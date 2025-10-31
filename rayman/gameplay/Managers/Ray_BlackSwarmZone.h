#ifndef _ITF_RAY_BLACKSWARMZONE_H_
#define _ITF_RAY_BLACKSWARMZONE_H_

namespace ITF
{
	class Ray_BlackSwarmComponent;

	//**********************************************
	//	Black swarm zone
	//		Make the link between a black swarm region polyline, a black swarm generator and black swarm repellers
	//**********************************************
	class Ray_BlackSwarmZone
	{
	public:
		Ray_BlackSwarmZone(const ActorRef & _generator, const ObjectRef & _polyline);
		~Ray_BlackSwarmZone();

		void update(const f32 _dt);

		ITF_INLINE const ActorRef & getGenerator() const;
		ITF_INLINE const ObjectRef & getPolyLine() const;

		//clear registered repellers list
		void clearRepellers();
		//register a black swarm repeller to this zone
		void registerRepeller(const ActorRef &_repeller);
		//add a swarm spawner to zone
		void addSpawner(const ActorRef & _spawner);
		//remove a swarm spawner from zone
		void removeSpawner(const ActorRef & _spawner);
		//get swarm spawners list
		ITF_INLINE const ActorRefList & getSpawners() const;
		//set polyline matching with this zone
		void setPolyLine(const ObjectRef &_polyline);
		//return true if zone is valid(polyline object and generator are OK)
		bbool zoneIsValid() const;

	private:

		//struct describing player's presence in zone
		struct PlayerZoneData
		{
			PlayerZoneData()
			:	m_player(ActorRef::InvalidRef),
                m_lastRepeller(ActorRef::InvalidRef),
                m_playerProtection(ActorRef::InvalidRef),
				m_time(0.0f),
                m_valid(btrue)
			{
			}

			PlayerZoneData(const ActorRef & _actor, const f32 _time, const bbool _danger)
			:	m_player(_actor),
                m_lastRepeller(ActorRef::InvalidRef),
                m_playerProtection(ActorRef::InvalidRef),
				m_time(_time),
                m_valid(btrue)
			{
			}

			ActorRef m_player;
            ActorRef m_lastRepeller;
            ActorRef m_playerProtection;
			f32 m_time;            
            bbool m_valid;      //used to remove obsolete player zone data (player is not detected by zone anymore)
		};

		//assign each registered repeller to swarm component
		void updateSwarmComponentRepellers();
		//assign each detected target to swarm component
		void updateSwarmComponentTargets();
		//assign each registered spawner to swarm component
		void updateSwarmComponentSpawners();
		//detect PCs around the zone
		void detectPCs();
		//check if PCs had to be hit
		void checkPCPaf(const f32 _dt);
		//return true if a position inside the zone polyline is protected by a repeller
        //_repeller : position of the protecting repeller ref, 
        bbool isPositionProtected(const Vec2d & _position, SafeArray<const Actor *> &_repellers) const;
		//get player zone data matching with player
		const PlayerZoneData * getPlayerZoneDataConst(const ActorRef & _actor) const;
		PlayerZoneData * getPlayerZoneData(const ActorRef & _actor);
        //clear all existing player zone data valid flag
        void clearPlayerZoneDataValidFlag();
        //remove obsolete player zone data
        void cleanPlayerZoneData();
		//return true if given player is in danger
		bbool isPlayerInDanger(const ActorRef & _player) const;
        //create player protection
        void createPlayerProtection(PlayerZoneData * const _playerData, const Vec2d & _position);
        //update player protection
        void updatePlayerProtection(PlayerZoneData * const _playerData, const Vec2d & _position);
        //destroy player protection
        void destroyPlayerProtection(PlayerZoneData * const _playerData);

        //hit PC functions
        void sendBounce(Actor * const _player, const ActorRef _bounceTarget) const;
        void sendStim(const Vec2d &_position, const EReceivedHitType _hitType, const Vec2d _direction = Vec2d::Zero) const;



		/*	Data	*/

		//generator actor
		ActorRef m_generator;
		//generator AI component
		Ray_BlackSwarmComponent * m_generatorAIComponent;

		//Polyline
		ObjectRef m_polyline;

		//registered spawners
		ActorRefList m_spawners;
		//Registered repellers list
		ActorRefList m_repellers;
		//Detected PCs list
		ActorRefList m_detectedPCs;

		//Players time in zone
		SafeArray<PlayerZoneData> m_playerZoneData;
	};

	//inlined methods definition
	ITF_INLINE const ActorRef & Ray_BlackSwarmZone::getGenerator() const
	{
		return m_generator;
	}

	ITF_INLINE const ObjectRef & Ray_BlackSwarmZone::getPolyLine() const
	{
		return m_polyline;
	}

	const ActorRefList & Ray_BlackSwarmZone::getSpawners() const
	{
		return m_spawners;
	}
}

#endif //_ITF_RAY_BLACKSWARMZONE_H_