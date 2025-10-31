#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BLACKSWARMCOMPONENT_H_
#include "Rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmComponent.h"
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_H_

#ifndef _ITF_RAY_BLACKSWARMZONEMANAGER_H_
#include "Rayman/gameplay/Managers/Ray_BlackSwarmZoneManager.h"
#endif //_ITF_RAY_BLACKSWARMZONEMANAGER_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

//#ifndef _ITF_DEBUGDRAW_H_
//#include "engine/debug/DebugDraw.h"
//#endif //_ITF_DEBUGDRAW_H_
#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

namespace ITF
{

#if defined(_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_)
	DECLARE_RASTER(blackSwarmContainer, RasterGroup_ComponentUpdate, Color::yellow());
	DECLARE_RASTER(blackSwarmRepellers, RasterGroup_ComponentUpdate, Color::green());
	DECLARE_RASTER(blackSwarmSeparation, RasterGroup_ComponentUpdate, Color::red());
	DECLARE_RASTER(blackSwarmGrid, RasterGroup_ComponentUpdate, Color::black());
	DECLARE_RASTER(blackSwarmLife, RasterGroup_ComponentUpdate, Color::pink());
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_

#if defined(_ITF_RAY_BLACKSWARMCOMPONENT_SIMPLE_RASTERS_)
	DECLARE_RASTER(blackSwarm, RasterGroup_ComponentUpdate, Color::black());
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_SIMPLE_RASTERS_

	IMPLEMENT_OBJECT_RTTI(Ray_BlackSwarmComponent)

	BEGIN_SERIALIZATION_CHILD(Ray_BlackSwarmComponent)
	END_SERIALIZATION()

	BEGIN_VALIDATE_COMPONENT(Ray_BlackSwarmComponent)
	END_VALIDATE_COMPONENT()

	const f32 Ray_BlackSwarmComponent::GRID_CELL_SIZE = 1.0f;
	const f32 Ray_BlackSwarmComponent::GRID_CELL_SIZE_INV = 1.0f / Ray_BlackSwarmComponent::GRID_CELL_SIZE;
	const f32 Ray_BlackSwarmComponent::GRID_CELL_HALF_SIZE = Ray_BlackSwarmComponent::GRID_CELL_SIZE * 0.5f;

	//------------------------------------------------------------------
	Ray_BlackSwarmComponent::Ray_BlackSwarmComponent()
    :   m_registered(bfalse),
		m_initialized(bfalse),
        m_frameCounter(0),
		m_gridOrigin(Vec2d::Zero),
        m_gridEnd(Vec2d::Zero),
        m_gridsWidth(0),
        m_gridsHeight(0),
        m_optimGrid(),
        m_playerProtectionSpawner(),
		m_soundComponent(NULL),
		m_particlesBarycenter(Vec2d::Zero),
		m_averageDistanceToBarycenter(0.0f),
		m_soundHandle(ITF::SoundHandle::getInvalidHandle())
	{
	}

	//------------------------------------------------------------------
	Ray_BlackSwarmComponent::~Ray_BlackSwarmComponent()
	{
		if( m_registered )
		{
			//unregister repeller to swarm zone manager
			Ray_BlackSwarmZoneManager * const manager = Ray_BlackSwarmZoneManager::getSingleton();
            if( manager != NULL )
            {
                manager->unregisterSwarmGenerator( m_actor->getRef() );
            }
            m_registered = bfalse;
		}
		destroyGrids();
		//stop sound if handle is valid
		stopSound();
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
        Super::onActorLoaded(_hotReload);

		//get sound component
		m_soundComponent = m_actor->GetComponent<SoundComponent>();
		ITF_ASSERT_MSG(m_soundComponent != NULL, "Cannot get sound component from swarm generator actor.");

		//register swarm generator to swarm zones manager
        Ray_BlackSwarmZoneManager * const manager = Ray_BlackSwarmZoneManager::getSingleton();
        ITF_ASSERT_MSG( manager != NULL, "Swarm zones manager is not yet created." );
        if( manager != NULL )
        {
            manager->registerSwarmGenerator( m_actor->getRef() );
            m_registered = btrue;
        }

        //init spawner
        const Path & playerProtectionActor = getTemplate()->getPlayerProtectionRepeller();
        ITF_WARNING( m_actor, !playerProtectionActor.isEmpty(), "Black Swarm component : undefined player protection repeller actor" );
        if (!playerProtectionActor.isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_playerProtectionSpawner, playerProtectionActor);
        }

	}

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::initGenerator()
    {
        Super::initGenerator();

        m_secondParticleGenerator.init(&getTemplate()->getSecondParticleGenerator());
        m_secondParticleGenerator.setMaxParticles(getTemplate()->getSecondNumParticles());
        m_secondParticleGenerator.getParameters()->setNumToEmit(U32_INVALID);
        m_secondParticleGenerator.getParameters()->setTexture(m_resource);
        m_secondParticleGenerator.getParameters()->setGeneratorMode(PARGEN_MODE_MANUAL);
        m_secondParticleGenerator.getParameters()->setPosition(m_actor->getPos());
        m_secondParticleGenerator.spawnNParticle(getTemplate()->getSecondNumParticles(), 0.f);

        m_particleGenerators.clear();
        m_particleGenerators.push_back(&m_particleGenerator);
        m_particleGenerators.push_back(&m_secondParticleGenerator);
    }

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::onBecomeActive()
	{
		Super::onBecomeActive();
        
        //play sound
		startSound();

        //reinit frame counter
        m_frameCounter = 0;

        AIData * data = AI_MANAGER->addActor(m_actor->getRef(),m_actor->getDepth());
        if (data)
        {
            data->m_faction = RAY_FACTION_ENEMY;
        }
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::onBecomeInactive()
	{
		Super::onBecomeInactive();

	    //stop sound
	    stopSound();

        AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::setContainerPolyline(const ObjectRef & _polyline)
	{
		bbool validPolyline = _polyline.isValid() && (_polyline.getObject() != NULL);
		if( validPolyline )
		{
			const PolyLine * const polyline = static_cast<PolyLine *>( _polyline.getObject() );
			//init component and particles
            init(polyline);
		}
		else if( m_initialized )
		{
			//destroy
            destroy();
		}
	}

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::init(const PolyLine * const _polyline)
    {
        ITF_ASSERT( !m_initialized );

        //clear target spawners
        m_targets.clear();
        m_targetSpawners.clear();
        
        //copy polyline
		m_containerPolyline.copyFrom( _polyline );
        m_containerPolyline.forceRecomputeData();

        //adapt generator AABB to polyline AABB
        GetActor()->growRelativeAABBFromAbsolute( m_containerPolyline.getAABB() );
        GetActor()->registerInWorldCells();
                    
        //init particles
        initBlackSwarmParticles();

        //init particles position
        initParticlesPosition();

        //init particles color
        m_particleGenerator.changeManualAllParticlesColor( getTemplate()->getParticleColor() );
        m_secondParticleGenerator.changeManualAllParticlesColor( getTemplate()->getParticleColor() );

        //create grids
        createGrids();

        //update initialized flag
        m_initialized = btrue;
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::destroy()
    {
        //clear polyline
		m_containerPolyline.clear();

        //destroy particles
        destroyParticles();

        //destroy grids
	    destroyGrids();

		//component is not initialized
		m_initialized = bfalse;
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::initParticles()
    {
        //do not call super to avoid creating several particles data arrays
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::initBlackSwarmParticles()
    {

        //create and init separation forces buffer
        const u32 firstGeneratorNumParticles = getTemplate()->getNumParticles();
        const u32 numParticles = firstGeneratorNumParticles + getTemplate()->getSecondNumParticles();

        //get size rdm factor
        const f32 randomSizeFactor = f32_Clamp( getTemplate()->getParticleSizeRandomFactor(), 0.0f, 0.99f);
        const f32 minSizeFactor = 1.0f - randomSizeFactor;
        const f32 maxSizeFactor = 1.0f + randomSizeFactor;

		//create and init particles data array
		const f32 lifeTimeIncrement = getTemplate()->getParticleLifeTime() / numParticles;
        m_blackParticleData.resize( numParticles );
        m_particlePosition.resize( numParticles );

        //init each particle data
		for(u32 index = 0; index < numParticles; ++index)
		{
            BlackParticleData & data = m_blackParticleData[index];

            //compute life time
            data.m_lifeTime = lifeTimeIncrement * index;

            //compute particle position
            f32 radius = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStartRadius());
            f32 rot = Seeder::getSharedSeeder().GetFloat(MTH_2PI);
            Vec2d startPos = (Vec2d::Right*radius).Rotate(rot);
            //data.m_position = m_actor->get2DPos() + startPos;
            m_particlePosition[index] = m_actor->get2DPos() + startPos;

            //compute speed
            f32 speed = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStartSpeed());
            data.m_speed = (Vec2d::Right*speed).Rotate(rot);

            //init size
            data.m_fullSize = getTemplate()->getParticleSize() * Seeder::getSharedSeeder().GetFloat(minSizeFactor, maxSizeFactor);

            //get generator data from particle index
            if( index < firstGeneratorNumParticles )
            {
                data.m_generatorIndex = 0;
                data.m_generatorParticleIndex = index;
            }
            else
            {
                data.m_generatorIndex = 1;
                data.m_generatorParticleIndex = index - firstGeneratorNumParticles;
            }
            data.m_updateType = index % 3;

            //make particle alive
            m_particleGenerators[data.m_generatorIndex]->changeManualParticleState(data.m_generatorParticleIndex, btrue);
            m_particleGenerators[data.m_generatorIndex]->changeManualParticleSize(data.m_generatorParticleIndex, data.m_fullSize);
        }
    }

    //------------------------------------------------------------------
    void  Ray_BlackSwarmComponent::updateSwarmComponentAABB()
    {
        if( m_initialized )
        {
            //adapt generator AABB to polyline AABB
            GetActor()->growRelativeAABBFromAbsolute( m_containerPolyline.getAABB() );
        }
    }

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::destroyParticles()
	{
        for(u32 index = 0; index < m_blackParticleData.size(); ++index)
		{
            const BlackParticleData & data = m_blackParticleData[index];
            //kill particle
            m_particleGenerators[data.m_generatorIndex]->changeManualParticleState(data.m_generatorParticleIndex, bfalse);
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::clearRepellers()
	{
		m_repellers.clear();
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::addRepeller(const Vec2d & _position, f32 _radius)
	{
		m_repellers.push_back( CircleRepeller(_position, _radius) );
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::addSpawner(const Vec2d &_position, const f32 _activationDistance)
	{
		m_spawnerData.push_back( SpawnerData(_position, _activationDistance) );
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::clearSpawners()
	{
		m_spawnerData.clear();
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::createGrids()
	{
		//destroy grid (if grid already exists)
		destroyGrids();
		
		//get polyline container AABB
		const AABB & boundingBox = m_containerPolyline.getAABB();

		//store grid origin
        m_gridOrigin = boundingBox.getMin() - Vec2d(GRID_CELL_SIZE, GRID_CELL_SIZE);

        //compute grids size
        m_gridsWidth = (u32)(ceil(boundingBox.getWidth() * GRID_CELL_SIZE_INV)) + 2;
        m_gridsHeight = (u32)(ceil(boundingBox.getHeight() * GRID_CELL_SIZE_INV)) + 2;
        const u32 gridSize = m_gridsWidth * m_gridsHeight;

        //compute grid end
        m_gridEnd = m_gridOrigin + Vec2d( m_gridsWidth * GRID_CELL_SIZE, m_gridsHeight * GRID_CELL_SIZE );

        //allocate optim grid
        m_optimGrid.resize(gridSize * GRID_TOTAL_OFFSET);
        m_optimGridData.resize(gridSize * 3);
        for(u32 index = 0; index < gridSize; ++ index)
        {
            const u32 startIndex = index * 3;
            m_optimGridData[startIndex] = 0;
            m_optimGridData[startIndex + 1] = 0;
            m_optimGridData[startIndex + 2] = 0;
        }

		//compute container grid only once
		computeContainerGrid();
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::destroyGrids()
	{
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::clearGrids()
	{
        //m_repellersGridData.fillZero();
        //m_repellersGridData.fillZero();
        const u32 gridSize = m_gridsWidth * m_gridsHeight;
        for(u32 index = 0; index < gridSize; ++ index)
        {
            const u32 startIndex = index * 3;
            m_optimGridData[startIndex + 1] = 0;
            m_optimGridData[startIndex + 2] = 0;
        }
		//here do not clear container grid because this grid is computed one time on init
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::computeContainerGrid()
	{
		//for each polyline container line
		for(u32 polyPointIndex = 1; polyPointIndex < m_containerPolyline.getPosCount(); ++polyPointIndex)
		{
			//get line start & end
			const Vec2d & segmentStart = m_containerPolyline.getPosAt( polyPointIndex - 1 );
			const Vec2d & segmentEnd = m_containerPolyline.getPosAt( polyPointIndex );

			//compute bounding vectors
			Vec2d minPosition( Min(segmentStart.m_x, segmentEnd.m_x), Min(segmentStart.m_y, segmentEnd.m_y) );
			Vec2d maxPosition( Max(segmentStart.m_x, segmentEnd.m_x), Max(segmentStart.m_y, segmentEnd.m_y) );
			//add influence to bounding positions
			Vec2d influence( 1.0f, 1.0f);
			minPosition -= influence;
			maxPosition += influence;

			//compute grid coords for each bounding position
			i32 minX, minY;
            getContainerGridCoordsFromPosition( minPosition, minX, minY );
			i32 maxX, maxY;
            getContainerGridCoordsFromPosition( maxPosition, maxX, maxY );

			//add container segment in grid for each matching cell
			for(i32 i = minX; i <= maxX; ++i)
			{
				for(i32 j = minY; j <= maxY; ++j)
				{
                    addContainerInGrid(i, j, polyPointIndex);
				}
			}
		}
	}

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::updateGenerator(f32 _dt)
    {
        Super::updateGenerator(_dt);

        //update generator
        m_secondParticleGenerator.startManualUpdate(m_actor->getPos());
        m_secondParticleGenerator.update(_dt, bfalse);
    }

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::updateParticles(f32 _deltaTime)
	{
#if defined(_ITF_RAY_BLACKSWARMCOMPONENT_SIMPLE_RASTERS_)
		TIMERASTER_SCOPE(blackSwarm);
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_SIMPLE_RASTERS_

		if( m_initialized )
		{
			//update particles placement in grid
			updateGrids();

			//compute data for spawners
			computeTargetsSpawners();

			const f32 z = m_actor->getPos().m_z;
			const f32 particlesMaxSpeed = getTemplate()->getParticleMaxSpeed();
			const f32 particleMaxLifeTime = getTemplate()->getParticleLifeTime();
			const f32 particleContainerRepulsionFactor = getTemplate()->getParticleContainerRepulsionFactor();
			const f32 particleRepellersRepulsionFactor = getTemplate()->getParticleRepellerRepulsionFactor();
			const f32 particleSeparationFactor = getTemplate()->getParticleSeparationFactor();
			const f32 particleTargetAttractionFactor = getTemplate()->getParticleTargetAttractionFactor();
            const u8 particleDeathSpeed = (u8)(getTemplate()->getParticleDeathSpeed() * _deltaTime);
            ITF_ASSERT(particleDeathSpeed > 0);
            const u8 particleLifeSpeed = (u8)(getTemplate()->getParticleLifeSpeed() * _deltaTime);
            ITF_ASSERT(particleLifeSpeed > 0);
            const u32 particleDeathMinBound = particleDeathSpeed - 1;
            const u32 particleDeathMaxBound = 255 - particleLifeSpeed + 1;
            const f32 particleLightDamageSqrRadiusRatio = 0.9f * 0.9f;

			//local var used to compute barycenter
			Vec2d positionSum(0.0f, 0.0f);
			//local var used to compute average sqr distance from barycenter
			f32 sqrDistanceSum = 0.0f;
            f32 particleRotation = 0.0f;
            f32 particleLifeTime = 0.0f;
            u8 particleLife = 0;
            f32 particleSize = 0.0f;
            f32 particleNewLifeTime = 0.0f;
            ITF_ParticleGenerator * generator = NULL;
            u32 generatorParticleIndex = U32_INVALID;
            u32 gridIndex = U32_INVALID;
            u32 updateType = U32_INVALID;

			//update each particle
			const u32 particlesCount = m_blackParticleData.size();
			for(u32 particleIndex = 0; particleIndex < particlesCount; ++particleIndex)
			{
				//get particle data
				BlackParticleData & particle = m_blackParticleData[particleIndex];

                //particlePosition = particle.m_position;
                Vec2d particlePosition = m_particlePosition[particleIndex];
                Vec2d particleSpeed = particle.m_speed;
                //particleRotation = particle.m_rotation;
                particleLifeTime = particle.m_lifeTime;
                particleLife = particle.m_life;
                particleSize = particle.m_fullSize;
                bbool lightDamaged = particle.m_lightDamaged;
                updateType = particle.m_updateType;

                //get generator data from particle index
                generator = m_particleGenerators[particle.m_generatorIndex];
                generatorParticleIndex = particle.m_generatorParticleIndex;

                //compute particle grid index
                gridIndex = getGridIndexFromPosition(particlePosition);
			    
                Vec2d particleAcceleration(0.0f, 0.0f);
                //compute container repulsion
                if( updateType == 0 )
                {
                    computeParticleContainerRepulsion( particlePosition, gridIndex, particleContainerRepulsionFactor, particleAcceleration );
                }
                else if (updateType == 1 )
                {
                    lightDamaged = computeParticleRepellersRepulsion( particlePosition, gridIndex, particleLightDamageSqrRadiusRatio, particleRepellersRepulsionFactor, particleAcceleration );
                }
                else
                {
                    computeParticleSeparation( particleIndex, particlePosition, gridIndex, particleSeparationFactor, particleAcceleration );
                }
                
                //compute particle target attraction
                computeParticleAttraction( particleIndex, particlePosition, particleTargetAttractionFactor, particleAcceleration );

				//compute new particle speed and rotation
                particleSpeed += (particleAcceleration * _deltaTime);                
                particleRotation = 0.0f;
                f32 norm = particleSpeed.sqrnorm();
                if( norm > 0.0f )
                {
                    f32 normInv = f32_InvSqrt(norm);
                        norm    = normInv*norm;
                    Vec2d particleNormalizedSpeed = particleSpeed * normInv;
                    particleSpeed = particleNormalizedSpeed * f32_Min(norm, particlesMaxSpeed);
                    particleRotation = particleNormalizedSpeed.getAngleOpt() + MTH_PI;
                    particlePosition += particleSpeed * _deltaTime;
                }

                //add position to compute this frame barycenter
				positionSum += particlePosition;
				//compute distance to previous frame barycenter
				sqrDistanceSum += (m_particlesBarycenter - particlePosition).sqrnorm();

                //update particle life(time spent in light, alpha, scale, respawn, ...)
                particleNewLifeTime = particleLifeTime + _deltaTime;
                if( lightDamaged || (particleNewLifeTime > particleMaxLifeTime) )
                {
                    if( particleLife > particleDeathMinBound )
                    {
                        particleLife -= particleDeathSpeed;
                    }
                    else
                    {
                        //respawn particle
                        getBestSpawnPosition(particleIndex, particlePosition);
                        //reset particle life time only if particle is old
                        if( particleLifeTime > particleMaxLifeTime )
                        {
                            particleNewLifeTime = 0.0f;
                        }
                        particleLife = 255;
                        lightDamaged = bfalse;
                    }
                    generator->changeManualParticleAlpha(generatorParticleIndex, particleLife);
                }
                else if( particleLife < particleDeathMaxBound )
                {
                    particleLife += particleLifeSpeed;
                    generator->changeManualParticleAlpha(generatorParticleIndex, particleLife);
                }

                //translate speed and pos from vec2d to vec3d
                Vec3d particle3DPosition( particlePosition.m_x, particlePosition.m_y, z );
                Vec3d particle3DSpeed( particleSpeed.m_x, particleSpeed.m_y, 0.0f );
                generator->updateManualParticle( generatorParticleIndex, particle3DPosition, particle3DSpeed, Angle(bfalse, particleRotation) );

                //store particle pos, speed and rotation
                m_particlePosition[particleIndex] = particlePosition;
                particle.m_speed = particleSpeed;
                particle.m_life = particleLife;
                particle.m_lightDamaged = lightDamaged;
                ++particle.m_updateType;

                //store particle life time
                particle.m_lifeTime = particleNewLifeTime;
			}

			const f32 invParticlesCount = 1.0f / particlesCount;
			//compute average distance from barycenter
			m_averageDistanceToBarycenter = sqrDistanceSum * invParticlesCount;
			//compute new barycenter
			m_particlesBarycenter = positionSum * invParticlesCount;

			//update sound
			updateSound(m_particlesBarycenter, m_averageDistanceToBarycenter);

            //update frame counter
            ++m_frameCounter;
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::computeParticleContainerRepulsion(const Vec2d &_particlePosition, const i32 _particleGridIndex, const f32 _repulsionFactor, Vec2d &_force) const
	{
#if defined(_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_)
		TIMERASTER_SCOPE(blackSwarmContainer);
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_

        //local computation repulsion
        Vec2d repulsion = Vec2d::Zero;

        i32 containerGridBegin, containerGridEnd;
        getParticleContainerGridData(_particleGridIndex, containerGridBegin, containerGridEnd);

	    for(i32 registeredSegment = containerGridBegin; registeredSegment < containerGridEnd; ++registeredSegment)
	    {
            //get registered segment index
            const u32 segmentIndex = m_optimGrid[registeredSegment];

            //get polyline segment normal
            const Vec2d segment = m_containerPolyline.getPosAt(segmentIndex - 1) - m_containerPolyline.getPosAt(segmentIndex);
            const Vec2d normal = segment.getPerpendicular();
            
            //get segment center and check if particle is over the segment
            const Vec2d segmentCenter = m_containerPolyline.getPosAt(segmentIndex) + (segment * 0.5f);
            const Vec2d diffToSegmentCenter = segmentCenter - _particlePosition;
            if( normal.dot(diffToSegmentCenter) > 0.0f )
            {
                //add force
                repulsion += (normal * _repulsionFactor);
            }
	    }

        //affect out parameter
        _force += repulsion;
	}

	//------------------------------------------------------------------
	bbool Ray_BlackSwarmComponent::computeParticleRepellersRepulsion(const Vec2d &_particlePosition, const i32 _particleGridIndex, const f32 _lightDamageRadiusSqrRatio, const f32 _repulsionFactor, Vec2d &_force) const
	{
#if defined(_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_)
		TIMERASTER_SCOPE(blackSwarmRepellers);
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_

        //init result boolean
		bbool lightDamaged = bfalse;

        //local computation repulsion
        Vec2d repulsion = Vec2d::Zero;

	    //foreach repeller
	    f32 repellerInfluencesCount = 0;
        Vec2d repellersBarycenter = Vec2d::Zero;

	    //get particle coords in repellers grid
	    i32 repellerGridBegin, repellerGridEnd;
	    getParticleRepellersGridData(_particleGridIndex, repellerGridBegin, repellerGridEnd);

	    for(i32 index = repellerGridBegin; index < repellerGridEnd; ++index)
	    {
		    //get repeller data
		    u32 repellerIndex = m_optimGrid[index];
		    const CircleRepeller & repeller = m_repellers[repellerIndex];
		    //compute distance to repeller
		    Vec2d difference = _particlePosition - repeller.m_position;
		    const f32 distanceToRepeller = difference.sqrnorm();
            const f32 repellerSqrRadius = repeller.m_radius * repeller.m_radius;
		    //compute barycenter
		    if( distanceToRepeller < repellerSqrRadius )
		    {
			    repellersBarycenter += difference;
                repellerInfluencesCount += 1.0f;

                lightDamaged = lightDamaged || (distanceToRepeller < repellerSqrRadius * _lightDamageRadiusSqrRatio);
		    }
	    }

        //if some force has been computed
	    if( repellerInfluencesCount > 0 )
	    {
            //average position
            repellersBarycenter /= repellerInfluencesCount;
            //force
            const f32 distance = repellersBarycenter.sqrnorm();
            if( distance > 0.0f )
            {
                repulsion = repellersBarycenter * (_repulsionFactor * f32_InvSqrt(distance));
            }
	    }

        //affect out parameter
        _force += repulsion;

		return lightDamaged;
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::computeParticleSeparation(const u32 _particleIndex, const Vec2d &_particlePosition, const i32 _particleGridIndex, const f32 _separationFactor, Vec2d & _force)
	{
#if defined(_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_)
		TIMERASTER_SCOPE(blackSwarmSeparation);
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_

        i32 particleGridBegin, particleGridEnd;
        getParticleNeighbourGridData( _particleGridIndex, particleGridBegin, particleGridEnd );

        //separation factors
        f32 separationInfluencesCount = 0.0f;

        Vec2d averagePosition = Vec2d::Zero;

        //for each particle neighbour
        for(i32 neighbour = particleGridBegin; neighbour < particleGridEnd; ++neighbour)
        {
            const u32 neighbourIndex = m_optimGrid[neighbour];
	        //if neighbour != me
	        if( _particleIndex != neighbourIndex )
	        {
		        //get neighbour pos
                averagePosition += m_particlePosition[neighbourIndex];
			    //increment separation influence
			    separationInfluencesCount += 1.0f;
	        }
        }

        //if influence
        if( separationInfluencesCount > 0.0f )
        {
            //average position
            averagePosition /= separationInfluencesCount;
            //get diff and diff sqr norm
            const Vec2d diff = _particlePosition - averagePosition;
            const f32 diffSqrNorm = diff.sqrnorm();
            if( diffSqrNorm > 0.0f )
            {
	            //average force and apply factor
	            _force += diff * (_separationFactor / diffSqrNorm);
            }
        }
	}

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::computeParticleAttraction(const u32 _particleIndex, const Vec2d &_particlePosition, const f32 _attractionFactor, Vec2d &_force) const
    {
        //compute target influence
		if( m_targets.size() > 0 )
		{
			//choose target to follow
			const u32 targetIndex = _particleIndex % m_targets.size();
			//compute difference to target
			const Vec2d difference = (m_targets[targetIndex] - _particlePosition).normalize();
			//apply attraction factor
			_force += (difference * _attractionFactor);
		}
    }

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::updateGrids()
	{
#if defined(_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_)
		TIMERASTER_SCOPE(blackSwarmGrid);
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_RASTERS_

		//clear previous grid
		clearGrids();

		//parse particles and place them
		for(u32 particleIndex = 0; particleIndex < m_particlePosition.size(); ++particleIndex)
		{
            addParticleInGrid( m_particlePosition[particleIndex], particleIndex );
		}

		//parse repellers and place them in grid
		for(u32 repellerIndex = 0; repellerIndex < m_repellers.size(); ++repellerIndex)
		{
			const CircleRepeller & repeller = m_repellers[repellerIndex];
			//compute repeller bounding position
			Vec2d radius( repeller.m_radius, repeller.m_radius );
			Vec2d minPosition = repeller.m_position - radius;
			Vec2d maxPosition = repeller.m_position + radius;
			//get matching grid cell coords
			i32 minCellX, minCellY;
			getRepellersGridCoordsFromPosition( minPosition, minCellX, minCellY );
			i32 maxCellX, maxCellY;
			getRepellersGridCoordsFromPosition( maxPosition, maxCellX, maxCellY );
			//parse each cell between min and max cell and add the repeller
			for(i32 i = minCellX; i <= maxCellX; ++i)
			{
				for(i32 j = minCellY; j <= maxCellY; ++j)
				{
					addRepellerInGrid(i,j, repellerIndex);
				}
			}
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::computeTargetsSpawners()
	{
        //build valid spawners list
        m_availableSpawnerData.clear();
        const u32 numSpawners = m_spawnerData.size();
        for(u32 spawnerIndex = 0; spawnerIndex < numSpawners; ++spawnerIndex)
        {
            const SpawnerData & spawnerData = m_spawnerData[spawnerIndex];
            const Vec2d & spawnerPosition = spawnerData.m_position;
            //check if spawner is not on repeller influence
            const u32 gridIndex = getGridIndexFromPosition(spawnerPosition);
            i32 repellerBegin, repellerEnd;
            getParticleRepellersGridData(gridIndex, repellerBegin, repellerEnd);

            bbool spawnerActive = btrue;
            for(i32 repellerArrayIndex = repellerBegin; repellerArrayIndex < repellerEnd; ++repellerArrayIndex)
            {
                //get repeller data
                u32 repellerIndex = m_optimGrid[repellerArrayIndex];
                const CircleRepeller & repeller = m_repellers[repellerIndex];
                //compute distance to repeller
                const f32 sqrDistanceToRepeller = (spawnerPosition - repeller.m_position).sqrnorm();
                const f32 sqrRadius = repeller.m_radius * repeller.m_radius * 0.9f; //reduce a little to allow spawner to be active sooner in case the repeller is moving
                //compute repulsion factor
                if( sqrDistanceToRepeller < sqrRadius )
                {
                    spawnerActive = bfalse;
                    break;
                }
            }
            if(spawnerActive)
            {
                m_availableSpawnerData.push_back(spawnerData);
            }
        }

		const u32 numTargets = m_targets.size();
		if( numTargets > 0 )
		{
            if( numTargets != m_targetSpawners.size() )
            {
                m_targetSpawners.resize( numTargets );
            }
			
			for(u32 targetIndex = 0; targetIndex < numTargets; ++targetIndex)
			{
                SafeArray<u32> & probabilityArray = m_targetSpawners[targetIndex];
				//clear previously computed array
				probabilityArray.clear();
				//reserve spawner array size
				probabilityArray.reserve(numSpawners);
				//float array
				SafeArray<f32> distances;
				distances.reserve(numSpawners);
				//get reachable spawners
                const u32 numAvailableSpawners = m_availableSpawnerData.size();
				for(u32 spawnerIndex = 0; spawnerIndex < numAvailableSpawners; ++spawnerIndex)
				{
                    const Vec2d & spawnerPosition = m_availableSpawnerData[spawnerIndex].m_position;
					const f32 sqrDistance = (m_targets[targetIndex] - spawnerPosition).sqrnorm();
					if( sqrDistance <= m_availableSpawnerData[spawnerIndex].m_sqrActivationRadius )
					{
                        //check if spawner is not on repeller influence
                        const u32 gridIndex = getGridIndexFromPosition(spawnerPosition);
                        i32 repellerBegin, repellerEnd;
                        getParticleRepellersGridData(gridIndex, repellerBegin, repellerEnd);

                        bbool spawnerActive = btrue;
                        for(i32 repellerArrayIndex = repellerBegin; repellerArrayIndex < repellerEnd; ++repellerArrayIndex)
                        {
                            //get repeller data
			                u32 repellerIndex = m_optimGrid[repellerArrayIndex];
			                const CircleRepeller & repeller = m_repellers[repellerIndex];
			                //compute distance to repeller
			                const f32 sqrDistanceToRepeller = (spawnerPosition - repeller.m_position).sqrnorm();
                            const f32 sqrRadius = repeller.m_radius * repeller.m_radius * 0.9f; //reduce a little to allow spawner to be active sooner in case the repeller is moving
			                //compute repulsion factor
			                if( sqrDistanceToRepeller < sqrRadius )
			                {
                                spawnerActive = bfalse;
                                break;
                            }
                        }
                        if( spawnerActive )
                        {
					        probabilityArray.push_back( spawnerIndex );
                        }
					}
				}
			}
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::getBestSpawnPosition(const u32 _particleIndex, Vec2d &_position) const
	{
		//if there is some spawners registered
		if( m_availableSpawnerData.size() > 0 )
		{
			const u32 numTargets = m_targets.size();
			//if there is some targets
            if( numTargets > 0 )
			{
			    //get particle target index
			    const u32 targetIndex = _particleIndex % numTargets;
                ITF_ASSERT_MSG( targetIndex < m_targetSpawners.size(), "Target spawner entry is not valid for this target index, (array size : %d, index : %d)", m_targetSpawners.size(), targetIndex );
			    //get spawner from spawners per target array
			    const SafeArray<u32> &proba = m_targetSpawners[targetIndex];
			    if( proba.size() > 0 )
			    {
				    const u32 spawnerArrayIndex = _particleIndex % proba.size();
				    const u32 spawnerIndex = proba[spawnerArrayIndex];
                    ITF_ASSERT_MSG( spawnerIndex < m_availableSpawnerData.size(), "Spawner data entry is not valid for this spawner index, (array size : %d, index : %d)", m_spawnerData.size(), spawnerIndex );
				    _position = m_availableSpawnerData[spawnerIndex].m_position;
				    return;
			    }
			}

			//choose a spawner according to particle index
			const u32 spawnerIndex = _particleIndex % m_availableSpawnerData.size();
			_position = m_availableSpawnerData[spawnerIndex].m_position;
		}
		else
		{
            if( m_spawnerData.size() > 0)
            {
                //choose a spawner according to particle index
                const u32 spawnerIndex = _particleIndex % m_spawnerData.size();
                _position = m_spawnerData[spawnerIndex].m_position;
            }
            else
            {
			    _position = m_actor->get2DPos();
            }
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::initParticlesPosition()
	{
		const f32 z = m_actor->getDepth();

		for(u32 index = 0; index < m_blackParticleData.size(); ++index)
		{
            const BlackParticleData & data = m_blackParticleData[index];

            Vec2d position = Vec2d::Zero;
			getBestSpawnPosition(index, position);

			//m_blackParticleData[index].m_position = position;
            m_particlePosition[index] = position;
			const Vec3d particle3DPosition( position.m_x, position.m_y, z );
			const Vec3d particle3DSpeed( data.m_speed.m_x, data.m_speed.m_y, 0.0f );

            //update generator
            m_particleGenerators[data.m_generatorIndex]->updateManualParticle( data.m_generatorParticleIndex, particle3DPosition, particle3DSpeed, Angle(bfalse,0.0f));//m_blackParticleData[index].m_rotation) );
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::startSound()
	{
		if( m_soundHandle == ITF::SoundHandle::getInvalidHandle())
		{
			m_soundHandle = m_soundComponent->playSound("Swarm");
			ITF_ASSERT( m_soundHandle != ITF::SoundHandle::getInvalidHandle());

			m_soundComponent->setSoundPos(m_soundHandle, m_actor->getPos());
			m_soundComponent->setInput("Density", 0.0f);
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::stopSound()
	{
		if( m_soundHandle != ITF::SoundHandle::getInvalidHandle())
		{
			m_soundComponent->stopSound(m_soundHandle);
			m_soundHandle = ITF::SoundHandle::getInvalidHandle();
		}
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmComponent::updateSound(const Vec2d &_barycenter, const f32 _averageSqrDistance)
	{
		if( m_soundHandle != ITF::SoundHandle::getInvalidHandle())
		{
            //LOG("Distance = %f", _averageSqrDistance);

            //compute close swarm density
			const f32 sqrMinCloseDensityDistance = sqr(getTemplate()->getSoundMinCloseDensityDistance());
			const f32 sqrMaxCloseDensityDistance = sqr(getTemplate()->getSoundMaxCloseDensityDistance());
			const f32 clampedCloseDistance = Clamp( _averageSqrDistance, sqrMaxCloseDensityDistance, sqrMinCloseDensityDistance);
			const f32 closeDensity = (sqrMinCloseDensityDistance - clampedCloseDistance) / (sqrMinCloseDensityDistance - sqrMaxCloseDensityDistance);
			m_soundComponent->setInput("SwarmDensity_close", closeDensity);
			//LOG("Close Density = %f", density);

            //compute far swarm density
			const f32 sqrMinFarDensityDistance = sqr(getTemplate()->getSoundMinFarDensityDistance());
			const f32 sqrMaxFarDensityDistance = sqr(getTemplate()->getSoundMaxFarDensityDistance());
			const f32 clampedFarDistance = Clamp( _averageSqrDistance, sqrMaxFarDensityDistance, sqrMinFarDensityDistance);
			const f32 farDensity = (sqrMinFarDensityDistance - clampedFarDistance) / (sqrMinFarDensityDistance - sqrMaxFarDensityDistance);
			m_soundComponent->setInput("SwarmDensity_far", farDensity);

			f32 depth = m_actor->getDepth();
			Vec3d soundPos = _barycenter.to3d(depth);
            m_soundComponent->setSoundPos(m_soundHandle, soundPos);
#ifndef ITF_FINAL
			//DebugDraw::line2D(m_particlesBarycenter, m_particlesBarycenter + Vec2d::Up, m_actor->getDepth());
#endif // ITF_FINAL
		}
	}

    //------------------------------------------------------------------
    ActorRef Ray_BlackSwarmComponent::spawnPlayerProtection(const Vec2d &_position)
    {
        const Vec3d position(_position.m_x, _position.m_y, m_actor->getDepth());
        Actor * protection = m_playerProtectionSpawner.getSpawnee( m_actor->getScene(), position );
        if( protection )
        {
            return protection->getRef();
        }
        else
        {
            return ActorRef::InvalidRef;
        }
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        Super::onDepthChanged(_oldZ, _newZ  );
        
        if (m_registered)
        {
            AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);
        }
    }

    //------------------------------------------------------------------
    void Ray_BlackSwarmComponent::Draw()
    {
        Super::Draw();

        //render second particle generator
        m_secondParticleGenerator.render();
    }


	//*****************************************************************************
    // Template
    //*****************************************************************************

	IMPLEMENT_OBJECT_RTTI(Ray_BlackSwarmComponent_Template)

	BEGIN_SERIALIZATION_CHILD(Ray_BlackSwarmComponent_Template)
		SERIALIZE_MEMBER("detectionRadius", m_detectionRadius);
		SERIALIZE_MEMBER("particleMaxSpeed", m_particleMaxSpeed);
		SERIALIZE_MEMBER("particleSize", m_particleSize);
        SERIALIZE_MEMBER("particleSizeRandomFactor", m_particleSizeRandomFactor);
        SERIALIZE_MEMBER("particleColor", m_particleColor);
		SERIALIZE_MEMBER("particleLifeTime", m_particleLifeTime);
        SERIALIZE_MEMBER("particleDeathSpeed", m_particleDeathSpeed);
        SERIALIZE_MEMBER("particleLifeSpeed", m_particleLifeSpeed);
		SERIALIZE_MEMBER("particleContainerRepulsionFactor", m_particleContainerRepulsionFactor);
		SERIALIZE_MEMBER("particleRepellerRepulsionFactor", m_particleRepellerRepulsionFactor);
		SERIALIZE_MEMBER("particleSeparationFactor", m_particleSeparationFactor);
		SERIALIZE_MEMBER("particleTargetAttractionFactor", m_particleTargetAttractionFactor);
		SERIALIZE_MEMBER("playerMaxTimeInZone", m_playerMaxTimeInZone);
		SERIALIZE_MEMBER("soundMinCloseDensityDistance", m_soundMinCloseDensityDistance);
		SERIALIZE_MEMBER("soundMaxCloseDensityDistance", m_soundMaxCloseDensityDistance);
        SERIALIZE_MEMBER("soundMinFarDensityDistance", m_soundMinFarDensityDistance);
		SERIALIZE_MEMBER("soundMaxFarDensityDistance", m_soundMaxFarDensityDistance);
        SERIALIZE_MEMBER("playerProtectionRepeller", m_playerProtectionRepeller);
        SERIALIZE_MEMBER("secondNumParticles", m_secondNumParticles);
        SERIALIZE_OBJECT("secondParticleGenerator", m_secondParticleGenerator);
    END_SERIALIZATION()

	//------------------------------------------------------------------
	Ray_BlackSwarmComponent_Template::Ray_BlackSwarmComponent_Template()
	:	m_detectionRadius(30.0f),
		m_particleSize(0.35f),
        m_particleSizeRandomFactor(0.0f),
        m_particleColor(Color::white()),
		m_particleLifeTime(5.0f),
        m_particleDeathSpeed(500),
        m_particleLifeSpeed(100),
		m_particleMaxSpeed(4.0f),
		m_particleContainerRepulsionFactor(200.0f),
		m_particleRepellerRepulsionFactor(150.0f),
		m_particleSeparationFactor(50.0f),
		m_particleTargetAttractionFactor(10.0f),
		m_playerMaxTimeInZone(0.2f),
		m_playerMovementAnticipationFactor(0.25f),
		m_soundMinCloseDensityDistance(8.5f),
		m_soundMaxCloseDensityDistance(6.5f),
        m_soundMinFarDensityDistance(20.0f),
		m_soundMaxFarDensityDistance(8.0f),
        m_playerProtectionRepeller(Path::EmptyPath),
        m_secondNumParticles(200)
	{
	}

	//------------------------------------------------------------------
	Ray_BlackSwarmComponent_Template::~Ray_BlackSwarmComponent_Template()
	{
	}
}
