#ifndef _ITF_RAY_BLACKSWARMCOMPONENT_H_
#define _ITF_RAY_BLACKSWARMCOMPONENT_H_

#ifndef _ITF_SWARMCOMPONENT_H_
#include "gameplay/Components/Misc/SwarmComponent.h"
#endif //_ITF_SWARMCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF
{
	//*****************************************************************************
    //	Swarm Generator AI Component
	//		Inherit from swarm particle generator component
	//		Manage swarm container & repellers influence
	//		Manage swarm target influence
    //*****************************************************************************

	//#define _ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_ 1
	//#define _ITF_RAY_BLACKSWARMCOMPONENT_SIMPLE_RASTERS_ 1

	class Ray_BlackSwarmComponent : public SwarmComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_BlackSwarmComponent, SwarmComponent,3647179052)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

	public:
		Ray_BlackSwarmComponent();
        virtual ~Ray_BlackSwarmComponent();

		virtual void		onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void        onBecomeActive();
		virtual void		onBecomeInactive();
        virtual void        onDepthChanged( f32 _oldZ, f32 _newZ );
        virtual void        Draw();

		void				setContainerPolyline(const ObjectRef &_polyline);

		void				addRepeller(const Vec2d  & _position, f32 _radius);
		void				clearRepellers();

		void				addSpawner(const Vec2d & _position, const f32 _activationDistance);
		void				clearSpawners();

		ITF_INLINE void		clearTargets();
		ITF_INLINE void		addTarget(const Vec2d & _target);
		ITF_INLINE f32		getDetectionRadius() const;
		ITF_INLINE f32		getPlayerMaxTimeInZone() const;
		ITF_INLINE f32		getPlayerMovementAnticipationFactor() const;

        ActorRef            spawnPlayerProtection(const Vec2d &_position);

        ITF_INLINE bbool    getSwarmPresence(const Vec2d & _position);

	protected:
        //generator management
        virtual void initGenerator();
        virtual void updateGenerator(f32 _dt);
        //particles management
        virtual void updateParticles( f32 _deltaTime );
		virtual void initParticles();
        virtual void updateSwarmComponentAABB();


	private:
		ITF_INLINE const class Ray_BlackSwarmComponent_Template * getTemplate() const;

        //global init (called when a valid polyline is assigned to component)
        void init(const PolyLine * const _polyline);
        //init particles
        void initBlackSwarmParticles();
        //init particles placement
		void initParticlesPosition();

        //global destroy (called when component polyline is invalid)
        void destroy();
        //destroy all particles
        void destroyParticles();

		//grids commons
		void createGrids();
		void destroyGrids();
		void clearGrids();
		void updateGrids();
		ITF_INLINE i32 getGridIndexFromPosition(const Vec2d & _position) const;

		//particles grid
		ITF_INLINE void addParticleInGrid(const Vec2d & _particlePosition, const u32 _particleIndex);
		ITF_INLINE void getParticleNeighbourGridData(const i32 _particleGridIndex, i32 & _begin, i32 & _end) const;

		//repellers grid
		ITF_INLINE void getRepellersGridCoordsFromPosition(const Vec2d & _position, i32 & _x, i32 & _y) const;
		ITF_INLINE void addRepellerInGrid(const i32 _x, const i32 _y, const u32 _repellerIndex);
        ITF_INLINE void getParticleRepellersGridData(const i32 _particleGridIndex, i32 & _begin, i32 & _end) const;

		//container grid
		void computeContainerGrid();
        ITF_INLINE void getContainerGridCoordsFromPosition(const Vec2d &_position, i32 & _x, i32 & _y) const;
        ITF_INLINE void addContainerInGrid(const i32 _x, const i32 _y, const u32 _segmentId);
        ITF_INLINE void getParticleContainerGridData(const i32 _particleGridIndex, i32 & _begin, i32 & _end) const;

		//compute particles forces
        void computeParticleContainerRepulsion(const Vec2d &_particlePosition, const i32 _particleGridIndex, const f32 _repulsionFactor, Vec2d &_force) const;
		bbool computeParticleRepellersRepulsion(const Vec2d &_particlePosition, const i32 _particleGridIndex, const f32 _lightDamageRadiusSqrRatio, const f32 _repulsionFactor, Vec2d & _force) const;
		void computeParticleSeparation(const u32 _particleIndex, const Vec2d &_particlePosition, const i32 _particleGridIndex, const f32 _separationFactor, Vec2d & _force);
        void computeParticleAttraction(const u32 _particleIndex, const Vec2d & _particlePosition, const f32 _attractionFactor, Vec2d & _force) const;

		//compute spawner probabilities for each target
		void computeTargetsSpawners();
		//get the best spawn location for a given particle (if there is no spawners registered => generator position)
		void getBestSpawnPosition(const u32 _particleIndex, Vec2d &_position) const;

		//play swarm sound
		void startSound();
		void stopSound();
		void updateSound(const Vec2d &_barycenter, const f32 _averageSqrDistance);

        static const u32        GENERATORS_COUNT = 2;

		static const f32		GRID_CELL_SIZE;
		static const f32		GRID_CELL_SIZE_INV;
		static const f32		GRID_CELL_HALF_SIZE;

		static const u32		PARTICLES_GRID_MAX_ITEMS_PER_CELL = 4;
		static const u32		REPELLERS_GRID_MAX_ITEMS_PER_CELL = 4;
		static const u32		CONTAINER_GRID_MAX_ITEMS_PER_CELL = 4;
        
        static const u32        GRID_TOTAL_OFFSET = CONTAINER_GRID_MAX_ITEMS_PER_CELL + PARTICLES_GRID_MAX_ITEMS_PER_CELL + REPELLERS_GRID_MAX_ITEMS_PER_CELL;
        static const u32        CONTAINER_GRID_OFFSET = 0;
        static const u32        PARTICLE_GRID_OFFSET = CONTAINER_GRID_OFFSET + CONTAINER_GRID_MAX_ITEMS_PER_CELL;
        static const u32        REPELLER_GRID_OFFSET = PARTICLE_GRID_OFFSET + PARTICLES_GRID_MAX_ITEMS_PER_CELL;

		static const u32		PARTICLES_SEPARATION_UPDATES_PER_FRAME = 64;
        
        ITF_ParticleGenerator   m_secondParticleGenerator;
        SafeArray<ITF_ParticleGenerator *> m_particleGenerators;

		bbool m_registered;
		bbool m_initialized;
        u32 m_frameCounter; //frame counter used to optimize particles computations

		//polyline containing the swarm
		PolyLine m_containerPolyline;

		//repellers affecting the swarm
		struct CircleRepeller
		{
			ITF_INLINE CircleRepeller()
			:	m_position(Vec2d::Zero),
				m_radius(0.0f)
			{
			}
			ITF_INLINE CircleRepeller(const Vec2d & _position, const f32 _radius)
			:	m_position(_position),
				m_radius(_radius)
			{
			}
			Vec2d m_position;
			f32 m_radius;
		};
		SafeArray<CircleRepeller> m_repellers;	//list of repellers swarm is affected by

			/*	swarm spawners		*/
		struct SpawnerData
		{
			ITF_INLINE SpawnerData()
			: m_position(Vec2d::Zero)
			, m_sqrActivationRadius(0.0f)
            , m_active(btrue)
			{
			}
			ITF_INLINE SpawnerData(const Vec2d & _position, const f32 _activationRadius)
			:	m_position(_position)
            , m_sqrActivationRadius(_activationRadius*_activationRadius)
            , m_active(btrue)
			{
			}
			Vec2d m_position;
			f32 m_sqrActivationRadius;
            bbool m_active;
		};
		SafeArray<SpawnerData> m_spawnerData;
        SafeArray<SpawnerData> m_availableSpawnerData;
		ITF_VECTOR< SafeArray<u32> > m_targetSpawners;	//for each targets list of matching spawners

			/*	targets of the swarm */
		SafeArray<Vec2d> m_targets;

			/*	particles caught on light */
		struct BlackParticleData
		{
			BlackParticleData()
                : m_speed(Vec2d::Zero)
                , m_lifeTime(0.0f)
                , m_fullSize(1.0f)
                , m_generatorIndex(0)
                , m_generatorParticleIndex(0)
                , m_life(255)
                , m_updateType(0)
                , m_lightDamaged(0)
                , m_free(0)
			{
			}
            Vec2d m_speed;              //particle speed
            f32 m_lifeTime;             //alive time
            f32 m_fullSize;             //randomly generated full size
            u32 m_generatorIndex : 1;
            u32 m_generatorParticleIndex : 10;
            u32 m_life : 8;
            u32 m_updateType : 2;
            u32 m_lightDamaged : 1;
            u32 m_free : 10;
		};
		SafeArray<BlackParticleData> m_blackParticleData;
        SafeArray<Vec2d> m_particlePosition;

			/* Grid origin (common to all grids) */
		Vec2d m_gridOrigin;
        Vec2d m_gridEnd;
        i32 m_gridsWidth;
        i32 m_gridsHeight;

        //optim grid (container segments index + particles index + repellers index)
        SafeArray<u16> m_optimGrid;
        SafeArray<i8> m_optimGridData;

            /* Player protection repeller spawner */
        SpawneeGenerator m_playerProtectionSpawner;


			/* Sound management	*/
		//sound component
		SoundComponent * m_soundComponent;
		Vec2d m_particlesBarycenter;
		f32 m_averageDistanceToBarycenter;
		ITF::SoundHandle m_soundHandle;

	};

	//*****************************************************************************
    // Template
    //*****************************************************************************

	class Ray_BlackSwarmComponent_Template : public SwarmComponent_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_BlackSwarmComponent_Template, SwarmComponent_Template,344870632);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BlackSwarmComponent);

	public:
        Ray_BlackSwarmComponent_Template();
        ~Ray_BlackSwarmComponent_Template();

		ITF_INLINE f32 getDetectionRadius() const;
		ITF_INLINE f32 getParticleSize() const;
        ITF_INLINE f32 getParticleSizeRandomFactor() const;
        ITF_INLINE const Color & getParticleColor() const;
		ITF_INLINE f32 getParticleLifeTime() const;
        ITF_INLINE u32 getParticleDeathSpeed() const;
        ITF_INLINE u32 getParticleLifeSpeed() const;
		ITF_INLINE f32 getParticleMaxSpeed() const;
		ITF_INLINE f32 getParticleContainerRepulsionFactor() const;
		ITF_INLINE f32 getParticleRepellerRepulsionFactor() const;
		ITF_INLINE f32 getParticleSeparationFactor() const;
		ITF_INLINE f32 getParticleTargetAttractionFactor() const;
		ITF_INLINE f32 getPlayerMaxTimeInZone() const;
		ITF_INLINE f32 getPlayerMovementAnticipationFactor() const;
		ITF_INLINE f32 getSoundMaxCloseDensityDistance() const;
		ITF_INLINE f32 getSoundMinCloseDensityDistance() const;
        ITF_INLINE f32 getSoundMaxFarDensityDistance() const;
		ITF_INLINE f32 getSoundMinFarDensityDistance() const;
        ITF_INLINE const Path & getPlayerProtectionRepeller() const;
        ITF_INLINE u32 getSecondNumParticles() const;
        ITF_INLINE const ITF_ParticleGenerator_Template & getSecondParticleGenerator() const;

	private:
		f32 m_detectionRadius;
		f32 m_particleSize;
        f32 m_particleSizeRandomFactor;
        Color m_particleColor;
		f32 m_particleLifeTime;
        u32 m_particleDeathSpeed;
        u32 m_particleLifeSpeed;
		f32 m_particleMaxSpeed;
		f32 m_particleContainerRepulsionFactor;
		f32 m_particleRepellerRepulsionFactor;
		f32 m_particleSeparationFactor;
		f32 m_particleTargetAttractionFactor;
		f32 m_playerMaxTimeInZone;
		f32 m_playerMovementAnticipationFactor;
		f32 m_soundMaxCloseDensityDistance;
		f32 m_soundMinCloseDensityDistance;
        f32 m_soundMaxFarDensityDistance;
		f32 m_soundMinFarDensityDistance;
        Path m_playerProtectionRepeller;
        u32 m_secondNumParticles;
        ITF_ParticleGenerator_Template m_secondParticleGenerator;
	};



	//*****************************************************************************
    // inlined functions definition
    //*****************************************************************************

	ITF_INLINE const class Ray_BlackSwarmComponent_Template * Ray_BlackSwarmComponent::getTemplate() const
    {
        return static_cast<const class Ray_BlackSwarmComponent_Template *>(m_template);
    }

	ITF_INLINE void Ray_BlackSwarmComponent::clearTargets()
	{
		m_targets.clear();
	}
	
	ITF_INLINE void Ray_BlackSwarmComponent::addTarget(const Vec2d &_target)
	{
		m_targets.push_back(_target );
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent::getDetectionRadius() const
	{
		return getTemplate()->getDetectionRadius();
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent::getPlayerMaxTimeInZone() const
	{
		return getTemplate()->getPlayerMaxTimeInZone();
	}
	
	ITF_INLINE f32 Ray_BlackSwarmComponent::getPlayerMovementAnticipationFactor() const
	{
		return getTemplate()->getPlayerMovementAnticipationFactor();
	}

    ITF_INLINE bbool Ray_BlackSwarmComponent::getSwarmPresence(const Vec2d & _position)
    {
        const i32 gridIndex = getGridIndexFromPosition(_position);
        const i32 optimGridDataIndex = (gridIndex * 3) + 1;
        const i8 neighboursCount = m_optimGridData[optimGridDataIndex];
        return neighboursCount > 0;
    }
    
    ITF_INLINE i32 Ray_BlackSwarmComponent::getGridIndexFromPosition(const Vec2d &_position) const
	{
		i32 x = (i32)((_position.m_x - m_gridOrigin.m_x) * GRID_CELL_SIZE_INV);
        ITF_ASSERT( x >= 0 && x < m_gridsWidth );
		i32 y = (i32)((_position.m_y - m_gridOrigin.m_y) * GRID_CELL_SIZE_INV);
        ITF_ASSERT( y >= 0 && y < m_gridsHeight );
        return (x + (y * m_gridsWidth));
	}

	ITF_INLINE void Ray_BlackSwarmComponent::addParticleInGrid(const Vec2d &_particlePosition, const u32 _particleIndex)
	{
        const i32 gridIndex = getGridIndexFromPosition(_particlePosition);
        const i32 optimGridDataIndex = (gridIndex * 3) + 1;
        const i8 neighboursCount = m_optimGridData[optimGridDataIndex];
        if( neighboursCount < PARTICLES_GRID_MAX_ITEMS_PER_CELL )
        {
            const i32 neighboursGridIndex = (gridIndex * GRID_TOTAL_OFFSET) + PARTICLE_GRID_OFFSET + neighboursCount;
            m_optimGrid[neighboursGridIndex] = (u16)_particleIndex;
            ++m_optimGridData[optimGridDataIndex];
        }
	}

	ITF_INLINE void Ray_BlackSwarmComponent::getParticleNeighbourGridData(const i32 _particleGridIndex, i32 & _begin, i32 & _end) const
	{
        const i32 optimGridDataIndex = (_particleGridIndex * 3) + 1;
        const i8 itemsCount = m_optimGridData[optimGridDataIndex];
        ITF_ASSERT( itemsCount <= PARTICLES_GRID_MAX_ITEMS_PER_CELL );
        _begin = (_particleGridIndex * GRID_TOTAL_OFFSET) + PARTICLE_GRID_OFFSET;
        _end = _begin + itemsCount;
	}

	ITF_INLINE void Ray_BlackSwarmComponent::getContainerGridCoordsFromPosition(const Vec2d &_position, i32 & _x, i32 & _y) const
	{
        _x = (i32)((_position.m_x - m_gridOrigin.m_x) * GRID_CELL_SIZE_INV);
		_y = (i32)((_position.m_y - m_gridOrigin.m_y) * GRID_CELL_SIZE_INV);
        ITF_ASSERT(_x >= 0 && _x < m_gridsWidth);
        ITF_ASSERT(_y >= 0 && _y < m_gridsHeight);
	}

    void Ray_BlackSwarmComponent::addContainerInGrid(const i32 _x, const i32 _y, const u32 _segmentId)
	{
		ITF_ASSERT_MSG( _x >= 0 && _x < m_gridsWidth, "Invalid cell to add a segment id in container grid" ); 
		ITF_ASSERT_MSG( _y >= 0 && _y < m_gridsHeight, "Invalid cell to add a segment id in container grid" );

		const i32 index = _x + (_y * m_gridsWidth);
        const i32 optimGridDataIndex = (index * 3);
        const i8 itemsCount = m_optimGridData[optimGridDataIndex];
        if( itemsCount < CONTAINER_GRID_MAX_ITEMS_PER_CELL )
        {
            const i32 itemIndex = (index * GRID_TOTAL_OFFSET) + CONTAINER_GRID_OFFSET + itemsCount;
            m_optimGrid[itemIndex] = (u16)_segmentId;
            ++m_optimGridData[optimGridDataIndex];
        }
	}

    ITF_INLINE void Ray_BlackSwarmComponent::getParticleContainerGridData(const i32 _particleGridIndex, i32 & _begin, i32 & _end) const
	{
        const i32 optimGridDataIndex = _particleGridIndex * 3;
        const i8 itemsCount = m_optimGridData[optimGridDataIndex];
        ITF_ASSERT( itemsCount <= CONTAINER_GRID_MAX_ITEMS_PER_CELL );
        _begin = _particleGridIndex * GRID_TOTAL_OFFSET + CONTAINER_GRID_OFFSET;
        _end = _begin + itemsCount;
	}

	ITF_INLINE void Ray_BlackSwarmComponent::getRepellersGridCoordsFromPosition(const Vec2d & _position, i32 & _x, i32 & _y) const
	{
		//compute repellers cells coords from 2D position
        if( _position.m_x >= m_gridOrigin.m_x )
        {
            if( _position.m_x < m_gridEnd.m_x )
            {
		        _x = (i32)((_position.m_x - m_gridOrigin.m_x) * GRID_CELL_SIZE_INV);
            }
            else
            {
                _x = m_gridsWidth - 1;
            }
        }
        else
        {
            _x  = 0;
        }

        if( _position.m_y >= m_gridOrigin.m_y )
        {
            if( _position.m_y < m_gridEnd.m_y )
            {
		        _y = (i32)((_position.m_y - m_gridOrigin.m_y) * GRID_CELL_SIZE_INV);
            }
            else
            {
                _y = m_gridsHeight - 1;
            }
        }
        else
        {
            _y  = 0;
        }
	}

	ITF_INLINE void Ray_BlackSwarmComponent::addRepellerInGrid(const i32 _x, const i32 _y, const u32 _repellerIndex)
	{
        ITF_ASSERT_MSG( _x >= 0 && _x < m_gridsWidth, "Invalid cell to add repeller id in repellers grid." );
        ITF_ASSERT_MSG( _y >= 0 && _y < m_gridsHeight, "Invalid cell to add repeller id in repellers grid." );
        
        const i32 index = _x + (_y * m_gridsWidth);
        const i32 optimGridDataIndex = (index * 3) + 2;
        const i8 itemsCount = m_optimGridData[optimGridDataIndex];
        if( itemsCount < REPELLERS_GRID_MAX_ITEMS_PER_CELL )
        {
            const i32 optimGridIndex = (index * GRID_TOTAL_OFFSET) + REPELLER_GRID_OFFSET + itemsCount;
            m_optimGrid[optimGridIndex] = (u16)_repellerIndex;
            ++m_optimGridData[optimGridDataIndex];
        }
	}

    ITF_INLINE void Ray_BlackSwarmComponent::getParticleRepellersGridData(const i32 _particleGridIndex, i32 & _begin, i32 & _end) const
    {
        const i32 optimGridDataIndex = _particleGridIndex * 3 + 2;
        const i8 itemsCount = m_optimGridData[optimGridDataIndex];
        ITF_ASSERT(itemsCount <= REPELLERS_GRID_MAX_ITEMS_PER_CELL);
        _begin = (_particleGridIndex * GRID_TOTAL_OFFSET) + REPELLER_GRID_OFFSET;
        _end = _begin + itemsCount;
    }






	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleSize() const
	{
		return m_particleSize;
	}

    ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleSizeRandomFactor() const
	{
		return m_particleSizeRandomFactor;
	}

    ITF_INLINE const Color & Ray_BlackSwarmComponent_Template::getParticleColor() const
    {
        return m_particleColor;
    }

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleLifeTime() const
	{
		return m_particleLifeTime;
	}

    ITF_INLINE u32 Ray_BlackSwarmComponent_Template::getParticleDeathSpeed() const
    {
        return m_particleDeathSpeed;
    }

    ITF_INLINE u32 Ray_BlackSwarmComponent_Template::getParticleLifeSpeed() const
    {
        return m_particleLifeSpeed;
    }

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleMaxSpeed() const
	{
		return m_particleMaxSpeed;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getDetectionRadius() const
	{
		return m_detectionRadius;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleContainerRepulsionFactor() const
	{
		return m_particleContainerRepulsionFactor;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleRepellerRepulsionFactor() const
	{
		return m_particleRepellerRepulsionFactor;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleSeparationFactor() const
	{
		return m_particleSeparationFactor;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getParticleTargetAttractionFactor() const
	{
		return m_particleTargetAttractionFactor;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getPlayerMaxTimeInZone() const
	{
		return m_playerMaxTimeInZone;
	}
	
	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getPlayerMovementAnticipationFactor() const
	{
		return m_playerMovementAnticipationFactor;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getSoundMinCloseDensityDistance() const
	{
		return m_soundMinCloseDensityDistance;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getSoundMaxCloseDensityDistance() const
	{
		return m_soundMaxCloseDensityDistance;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getSoundMinFarDensityDistance() const
	{
		return m_soundMinFarDensityDistance;
	}

	ITF_INLINE f32 Ray_BlackSwarmComponent_Template::getSoundMaxFarDensityDistance() const
	{
		return m_soundMaxFarDensityDistance;
	}

    ITF_INLINE const Path & Ray_BlackSwarmComponent_Template::getPlayerProtectionRepeller() const
    {
        return m_playerProtectionRepeller;
    }

    ITF_INLINE u32 Ray_BlackSwarmComponent_Template::getSecondNumParticles() const
    {
        return m_secondNumParticles;
    }

    ITF_INLINE const ITF_ParticleGenerator_Template & Ray_BlackSwarmComponent_Template::getSecondParticleGenerator() const
    { 
        return m_secondParticleGenerator; 
    }
}

#endif //_ITF_RAY_BLACKSWARMCOMPONENT_H_
