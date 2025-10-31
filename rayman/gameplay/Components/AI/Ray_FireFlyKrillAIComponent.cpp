#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FIREFLYKRILLAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FireFlyKrillAIComponent.h"
#endif //_ITF_RAY_FIREFLYKRILLAICOMPONENT_H_

#ifndef _ITF_RAY_ABYSSLIGHTCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AbyssLightComponent.h"
#endif //_ITF_RAY_ABYSSLIGHTCOMPONENT_H_

#ifndef _ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FireFlyKrillSpawnerComponent.h"
#endif //_ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_FireFlyKrillAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_FireFlyKrillAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_FireFlyKrillAIComponent)
    END_VALIDATE_COMPONENT()

    const f32 Ray_FireFlyKrillAIComponent::KRILL_MIN_INTRA_DISTANCE = 0.05f;

    //------------------------------------------------------------------
    //static members used to manage players <-> krill association
    //------------------------------------------------------------------
    ActorRefList Ray_FireFlyKrillAIComponent::ms_krills = ActorRefList();
    Ray_FireFlyKrillAIComponent::KrillPlayerMatching Ray_FireFlyKrillAIComponent::ms_playerMatching = Ray_FireFlyKrillAIComponent::KrillPlayerMatching();


    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::registerKrill(const ActorRef & _krillRef)
    {
        ITF_ASSERT(ms_krills.find(_krillRef) == U32_INVALID);
        ms_krills.push_back(_krillRef);
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::unregisterKrill(const ActorRef & _krillRef)
    {
        const u32 krillIndex = ms_krills.find(_krillRef);
        ITF_ASSERT(krillIndex != U32_INVALID);
        if( krillIndex != U32_INVALID )
        {
            removePlayerMatchingFromKrill(_krillRef);
            ms_krills.eraseNoOrder(krillIndex);
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::matchPlayerWithKrill(const ActorRef & _playerRef, const ActorRef & _krillRef)
    {
        ITF_ASSERT(ms_playerMatching.find(_playerRef) == ms_playerMatching.end());
        ms_playerMatching.insert(KrillPlayerMatching::value_type(_playerRef, _krillRef));
    }

    //------------------------------------------------------------------
    bbool Ray_FireFlyKrillAIComponent::isPlayerMatched(const ActorRef & _playerRef)
    {
        return (ms_playerMatching.find(_playerRef) != ms_playerMatching.end());
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::removePlayerMatchingFromKrill(const ActorRef & _krillRef)
    {
        KrillPlayerMatching::iterator it = ms_playerMatching.begin();
        while( it != ms_playerMatching.end() )
        {
            if( (*it).second != _krillRef )
            {
                ++it;
            }
            else
            {   
                ms_playerMatching.erase(it);
                return;
            }
        }
    }

    //------------------------------------------------------------------
    Ray_FireFlyKrillAIComponent::Ray_FireFlyKrillAIComponent()
    : m_instanceRegistered(bfalse)
    , m_nbParticles(0)
    , m_resource(ResourceID::Invalid)
    , m_targetsMode(KrillTargetMode_UNDEFINED)
    , m_playerRef(ActorRef::InvalidRef)
    , m_playerSmoothedPosition(Vec2d::Zero)
    , m_speed(Vec2d::Zero)
    , m_playerOffset(Vec2d::Zero)
    , m_state(KrillState_UNDEFINED)
    , m_abyssLightComponent(NULL)
    , m_targetsWantedRotationSpeed(0.0f)
    , m_targetsCurrentRotationSpeed(0.0f)
    , m_targetsAngle(0.0f)
    , m_particleSurface(0.0f)
    , m_protectionRadius(0.0f)
    , m_protectionSurface(0.0f)
    , m_nbActiveParticles(0)
    , m_particlesBarycenter(Vec2d::Zero)
    , m_lightOffset(Vec2d::Zero)
    , m_outOfSpawnerRange(bfalse)
    , m_spawnerPosition(Vec2d::Zero)
    , m_timeOutOfSpawnerRange(0.f)
    , m_mesh()
    , m_vertexBufferCache()
    {
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //compute particle surface
        const f32 wantedRadius = getTemplate()->getKrillRadius();
        const u32 nbParticles = getTemplate()->getNbParticles();
        m_particleSurface = (wantedRadius * wantedRadius * MTH_PI) / nbParticles;

        //init particle generator
        loadResource();
        createMesh();

        m_abyssLightComponent = m_actor->GetComponent<Ray_AbyssLightComponent>();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::loadResource()
    {
        m_resource = m_actor->addResource(Resource::ResourceType_Texture,getTemplate()->getTextureFile());
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::unloadResource()
    {
        if( m_resource.isValidResourceId() )
        {
            m_actor->removeResource(m_resource);
            m_resource.invalidateResourceId();
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        unregisterInstance();
        destroyMesh();
        unloadResource();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        registerInstance();

        const u32 nbGroups = getTemplate()->getNbGroups();
        const u32 nbParticles = getTemplate()->getNbParticles();
        initTargets(nbGroups);
        initParticles(nbParticles);
        changeState(KrillState_IDLE);
        computeProtectionRadius(0.0f);

        m_timeOutOfSpawnerRange = getTemplate()->getLifeDuration() + getTemplate()->getLifeDurationRandom();
        m_particlesBarycenter = m_actor->get2DPos();
        m_lightOffset = Vec2d::Zero;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::initTargets(const u32 _nbTargets)
    {
        m_targets.clear();
        if( _nbTargets > 0 )
        {
            m_targets.reserve(_nbTargets);
            
            const f32 angle = MTH_2PI / _nbTargets;
            for(u32 index = 0; index < _nbTargets; ++index)
            {
                Vec2d relativePosition = Vec2d::Right;
                KrillTarget target;
                target.m_position = relativePosition.Rotate(angle * index);
                target.m_rotationSpeedFactor = Seeder::getSharedSeeder().GetFloat(2.0f) - 1.0f;
                m_targets.push_back( target );
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::initParticles(const u32 _nbParticles)
    {
        m_krillParticleData.clear();
        m_nbParticles = _nbParticles;
        if( _nbParticles > 0 )
        {
            const f32 lifeDuration = getTemplate()->getLifeDuration();
            const f32 lifeDurationStep = getTemplate()->getLifeDurationRandom() / _nbParticles;
            const f32 particleFixedSize = getTemplate()->getParticleMinSize();
            const f32 particleRandomSize = getTemplate()->getParticleMaxSize() - particleFixedSize;

            m_krillParticleData.resize(_nbParticles);

            for(u32 index = 0; index < _nbParticles; ++index)
            {
                const f32 particleLifeDuration = lifeDuration + (lifeDurationStep * index);
                const Vec2d particlePosition = getParticleSpawningPosition();
                
                KrillParticleData data;
                data.m_position = particlePosition;
                data.m_speed = Vec2d::Zero;
                data.m_lifeDuration = particleLifeDuration;
                data.m_deathRatio = 1.0f;
                data.m_angle = 0.0f;
                data.m_active = bfalse;
                data.m_size = particleFixedSize + Seeder::getSharedSeeder().GetFloat(particleRandomSize);

                m_krillParticleData[index] = data;
            }

            m_vertexBufferCache.resize( _nbParticles * 4 );
        }
        m_nbActiveParticles = 0;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        unregisterInstance();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::Update(f32 _dt)
    {
        Super::Update(_dt);

        m_playerOffset = Vec2d::Zero;

        updateSpawnerDetection(_dt);
        updateState(m_state, _dt);
        updateTargets(_dt);
        updateParticles(_dt);
        computeProtectionRadius(_dt);
        updateLightOffset();

        buildMesh();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::Draw()
    {
        Super::Draw();
        drawMesh();
    }


    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::setTarget(const ActorRef & _playerRef)
    {
        m_playerRef = _playerRef;
        Ray_FireFlyKrillAIComponent::matchPlayerWithKrill(_playerRef, m_actor->getRef());
        const Actor * const player = _playerRef.getActor();
        ITF_ASSERT(player);
        if(player)
        {
            m_playerSmoothedPosition = player->get2DPos();
        }
    }

    //------------------------------------------------------------------
    //registration management
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::registerInstance()
    {
        if( !m_instanceRegistered )
        {
            Ray_FireFlyKrillAIComponent::registerKrill(m_actor->getRef());
            m_instanceRegistered = btrue;
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::unregisterInstance()
    {
        if( m_instanceRegistered )
        {
            Ray_FireFlyKrillAIComponent::unregisterKrill(m_actor->getRef());
            m_instanceRegistered = bfalse;
        }
    }

    //------------------------------------------------------------------
    //krill spawner detection
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::updateSpawnerDetection(const f32 _dt)
    {
        const f32 radius = m_protectionRadius + getTemplate()->getKrillRadius();
        if( Ray_FireFlyKrillSpawnerComponent::getSpawnerInRange(m_actor->get2DPos(), radius, m_spawnerPosition) )
        {
            m_outOfSpawnerRange = false;
             const f32 krillRespawnSpeedFactor = getTemplate()->getKrillRespawnSpeedFactor();
            m_timeOutOfSpawnerRange = f32_Max(0.0f, m_timeOutOfSpawnerRange - (_dt * krillRespawnSpeedFactor));
        }
        else
        {
            m_outOfSpawnerRange = btrue;
            m_timeOutOfSpawnerRange = f32_Min(m_timeOutOfSpawnerRange + _dt, getTemplate()->getLifeDuration() + getTemplate()->getLifeDurationRandom() );
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::updateTargets(const f32 _dt)
    {
        m_targetsCurrentRotationSpeed += (m_targetsWantedRotationSpeed - m_targetsCurrentRotationSpeed) * _dt;

        for(u32 index = 0; index < m_targets.size(); ++index)
        {
            KrillTarget & target = m_targets[index];
            f32 rotation = target.m_currentRotation + (m_targetsCurrentRotationSpeed * target.m_rotationSpeedFactor * _dt);
            if(rotation > MTH_2PI)
            {
                rotation -= MTH_2PI;
            }
            else if( rotation < 0.0f )
            {
                rotation += MTH_2PI;
            }
            target.m_currentRotation = rotation;
        }

        if( m_targetsMode == KrillTargetMode_ROTATION )
        {
            const f32 stepAngle = MTH_2PI / m_targets.size();
            const f32 distance = 1.0f * (m_nbActiveParticles * 1.0f / getTemplate()->getNbParticles());
            for(u32 index = 0; index < m_targets.size(); ++index)
            {
                const f32 targetAngle = index * stepAngle + m_targets[index].m_currentRotation;
                m_targets[index].m_position = Vec2d::Right.Rotate( targetAngle ) * distance;
            }
        }
        else if( m_targetsMode == KrillTargetMode_DOCKED )
        {
            const Vec2d centerOffset(m_protectionRadius * 0.23f, 0.0f);
            const f32 radiusX = m_protectionRadius * 0.2f;
            const f32 radiusY = m_protectionRadius * 0.4f;

            const f32 stepAngle = MTH_2PI / m_targets.size();
            for(u32 index = 0; index < m_targets.size(); ++index)
            {
                Vec2d targetPosition = Vec2d::Zero;
                if( index % 2 == 0 )
                {
                    f32 cos, sin;
                    f32_CosSinOpt(m_targets[index].m_currentRotation + index * stepAngle, &cos, &sin);
                    targetPosition = -centerOffset + Vec2d(radiusX * cos, radiusY * sin);

                }
                else
                {
                    f32 cos, sin;
                    f32_CosSinOpt(-m_targets[index].m_currentRotation + index * stepAngle, &cos, &sin);
                    targetPosition = centerOffset + Vec2d(radiusX * cos, radiusY * sin);
                }
                targetPosition = targetPosition.Rotate(m_targetsAngle);
                m_targets[index].m_position = targetPosition;
            }
        }
    }

    //------------------------------------------------------------------
    Vec2d Ray_FireFlyKrillAIComponent::getTargetAbsolutePosition(const u32 _particleIndex) const
    {
        const u32 targetIndex = _particleIndex % m_targets.size();
        return (m_actor->get2DPos() + m_playerOffset + m_targets[targetIndex].m_position);
    }

    //------------------------------------------------------------------
    Vec2d Ray_FireFlyKrillAIComponent::getTargetRelativePosition(const u32 _particleIndex) const
    {
        const u32 targetIndex = _particleIndex % m_targets.size();
        return m_targets[targetIndex].m_position;
    }

    //------------------------------------------------------------------
    bbool Ray_FireFlyKrillAIComponent::isPlayerDetected() const
    {
        return m_playerRef.isValid();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::changeState(const KrillState _state)
    {
        switch( m_state )
        {
        case KrillState_FOLLOW:
            stopFollow();
            break;
        default:
            break;
        }

        m_state = _state;

        switch( m_state )
        {
        case KrillState_IDLE:
            startIdle();
            break;
        case KrillState_FOLLOW:
            startFollow();
            break;
        default:
            break;
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::updateState(const KrillState _state, const f32 _dt)
    {
        switch(_state)
        {
        case KrillState_IDLE:
            updateIdle(_dt);
            break;
        case KrillState_FOLLOW:
            updateFollow(_dt);
            break;
        default:
            break;
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::startIdle()
    {
        m_targetsMode = KrillTargetMode_DOCKED;
        m_targetsWantedRotationSpeed = MTH_PIBY2;
        m_targetsAngle = 0.0f;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::updateIdle(const f32 _dt)
    {
        //to avoid spawning -> dying krill when players are respawning close to the krill spawner
        if( m_nbActiveParticles > getTemplate()->getNbParticles() / 2 )
        {
            if( isPlayerDetected() )
            {
                changeState(KrillState_FOLLOW);
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::startFollow()
    {
        m_targetsMode = KrillTargetMode_ROTATION;
        m_targetsWantedRotationSpeed = MTH_PIBY4;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::stopFollow()
    {
        m_speed = Vec2d::Zero;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::updateFollow(const f32 _dt)
    {
        Actor * player = m_playerRef.getActor();
        if( player != NULL )
        {
            if( !RAY_GAMEMANAGER->isPlayerInSequence( m_playerRef ) )
            {
                if( m_nbActiveParticles > 0 )
                {
                    Vec2d positionOffset = Vec2d::Zero;
                    const PhysComponent * const physComponent = player->GetComponent<PhysComponent>();
                    if( physComponent )
                    {
                        const Vec2d playerSpeed = physComponent->getSpeed();
                        m_playerOffset = playerSpeed * getTemplate()->getPositionOffsetFactor();
                        const f32 playerSpeedNorm = playerSpeed.norm();
                        const f32 ratio = 1.0f - f32_Min(playerSpeedNorm * 0.075f, 1.0f);
                        m_targetsWantedRotationSpeed = MTH_PIBY4 * ratio;
                    }
                    const Vec2d playerPosition = player->get2DPos();
                    m_playerSmoothedPosition = m_playerSmoothedPosition * 0.75f + playerPosition * 0.25f;
                    const Vec2d & krillPosition = m_actor->get2DPos();
                    const Vec2d difference = (m_playerSmoothedPosition - krillPosition);
                    const f32 distance = difference.norm();
                    const f32 maxSpeedDistance = 0.5f;
                    const f32 speedRatio = f32_Min(distance, maxSpeedDistance) / maxSpeedDistance;
                    if( speedRatio > 0.0f )
                    {
                        const f32 speed = speedRatio * getTemplate()->getSpeed();
                        m_speed = (difference * (speed / distance));
                        m_actor->set2DPos(krillPosition + (m_speed * _dt));
                    }
                    else
                    {
                        m_speed = Vec2d::Zero;
                    }
                }
                else
                {
                    m_actor->requestDestruction();
                }
            }
            else
            {
                m_actor->requestDestruction();
            }
        }
        else
        {
            m_actor->requestDestruction();
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::updateParticles(const f32 _dt)
    {
        const f32 separationDistance = getTemplate()->getSeparationDistance();
        const f32 separationFactor = getTemplate()->getSeparationFactor();
        const f32 attractionFactor = getTemplate()->getAttractionFactor();
        const f32 frictionFactor = getTemplate()->getFrictionFactor();
        const f32 deathDuration = getTemplate()->getDeathDuration();
        const f32 deathDurationInv = 1.0f / deathDuration;

        Vec2d particlesBarycenter = Vec2d::Zero;
        f32 protectionSurface = 0.0f;
        for(u32 index = 0; index < m_nbParticles; ++index)
        {
            KrillParticleData & data = m_krillParticleData[index];

            checkParticleLifeDuration(index, m_outOfSpawnerRange, m_timeOutOfSpawnerRange, deathDurationInv, _dt);

            if( data.m_active )
            {
                const Vec2d particlePosition = data.m_position;
                const Vec2d particleSpeed = data.m_speed;
                const f32 particleAngle = data.m_angle;
                const f32 deathRatio = data.m_deathRatio;

                Vec2d acceleration = Vec2d::Zero;                
                computeSeparation(index, particlePosition, separationFactor, separationDistance, acceleration);
                const f32 distanceFromTarget = computeAttraction(index, particlePosition, attractionFactor, acceleration);
                computeFriction(particleSpeed, acceleration, distanceFromTarget, frictionFactor, acceleration);

                const Vec2d speed = particleSpeed + (acceleration * _dt);
                const Vec2d position = particlePosition + (speed * _dt);
                //if speed is almost null keep previous angle
                const f32 speedNorm = speed.norm();
                const f32 angle = f32_Sel( speedNorm - MTH_EPSILON, (speed / speedNorm).getAngle(), particleAngle );
                
                particlesBarycenter += position;
                protectionSurface += (1.0f - deathRatio) * m_particleSurface;
                
                data.m_position = position;
                data.m_speed = speed;
                data.m_angle = angle;
            }
        }

        if( m_nbActiveParticles > 0)
        {
            m_particlesBarycenter = particlesBarycenter / (f32)m_nbActiveParticles;
            const Vec2d & actorPos = m_actor->get2DPos();
            const Vec2d offset = m_particlesBarycenter - actorPos;
            m_lightOffset = (m_lightOffset * 0.95f) + ( offset * 0.05f);
        }
        m_protectionSurface = protectionSurface;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::computeSeparation(const u32 _index, const Vec2d & _position, const f32 _factor, const f32 _maxDistance, Vec2d & _acceleration) const
    {
        Vec2d separation = Vec2d::Zero;
        for(u32 index = 0; index < m_nbParticles; ++index)
        {
            const KrillParticleData & data = m_krillParticleData[index];
            if( (index != _index) && data.m_active )
            {
                const Vec2d difference = _position - data.m_position;
                const f32 distance = difference.norm();
                const f32 clampedDistance = f32_Max(distance, KRILL_MIN_INTRA_DISTANCE);
                const f32 ratioDistance = f32_Min(clampedDistance, _maxDistance);
                const f32 separationRatio = (_maxDistance - ratioDistance) / (_maxDistance - KRILL_MIN_INTRA_DISTANCE);
                const Vec2d separationComp = difference * (_factor * separationRatio / clampedDistance);
                separation += separationComp;
            }
        }
        _acceleration += separation;
    }

    //------------------------------------------------------------------
    f32 Ray_FireFlyKrillAIComponent::computeAttraction(const u32 _index, const Vec2d & _particlePosition, const f32 _factor, Vec2d & _acceleration) const
    {
        const Vec2d targetPosition = getTargetAbsolutePosition(_index);
        const Vec2d difference = targetPosition - _particlePosition;
        const f32 distance = difference.norm();
        const f32 factor = f32_Sel(distance - MTH_EPSILON, f32_Min(distance, 1.0f) * _factor / distance, 0.0f);
        _acceleration += difference * factor;
        return distance;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::computeFriction(const Vec2d &_particleSpeed, const Vec2d _wantedAcceleration, const f32 _distanceFromTarget, const f32 _factor, Vec2d & _acceleration) const
    {
        const f32 speedNorm = _particleSpeed.norm();
        const f32 accelerationNorm = _wantedAcceleration.norm();
        if( speedNorm != 0.0f && accelerationNorm != 0.0f )
        {
            const f32 cos = Vec2d::Dot(&_particleSpeed, &_acceleration) / (speedNorm * accelerationNorm);
            const f32 orientationRatio = 1.0f - ((cos + 1.0f) * 0.5f);
            const f32 distanceRatio = 1.0f - f32_Min(_distanceFromTarget , 1.0f);
            const f32 finalRatio = f32_Max(orientationRatio, distanceRatio);
            _acceleration += _particleSpeed * (finalRatio * -_factor);
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::checkParticleLifeDuration(const u32 _index, const bbool _outside, const f32 _currentTime, const f32 _deathDurationInv, const f32 _dt)
    {
        KrillParticleData & data = m_krillParticleData[_index];
        f32 deathRatio = data.m_deathRatio;

        if( _outside )
        {
            if( data.m_active )
            {
                if( data.m_lifeDuration < _currentTime )
                {
                    if( deathRatio < 1.0f )
                    {
                        deathRatio = f32_Min(1.0f, deathRatio + (_dt * _deathDurationInv));
                    }
                    else
                    {
                        data.m_active = bfalse;
                        --m_nbActiveParticles;
                    }
                }
            }
        }
        else
        {
            if( data.m_lifeDuration > _currentTime )
            {
                if( data.m_active )
                {
                    if( deathRatio > 0.0f )
                    {
                        deathRatio = f32_Max(0.0f, deathRatio - (_dt * _deathDurationInv));
                    }
                }
                else
                {
                    data.m_active = btrue;
                    data.m_position = getParticleSpawningPosition();
                    ++m_nbActiveParticles;                        
                }
            }
        }

        data.m_deathRatio = deathRatio;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::computeProtectionRadius(const f32 _dt)
    {
        m_protectionRadius = f32_Sqrt( m_protectionSurface / MTH_PI );
        if( m_abyssLightComponent != NULL )
        {
            m_abyssLightComponent->setRadius( m_protectionRadius );
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::updateLightOffset()
    {
        if( m_abyssLightComponent )
        {
            const Vec2d offset = m_lightOffset * getTemplate()->getKrillBarycenterInfluence();
            m_abyssLightComponent->setOffset( offset );
        }
    }

    //------------------------------------------------------------------
    Vec2d Ray_FireFlyKrillAIComponent::getParticleSpawningPosition() const
    {
        return m_spawnerPosition;
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::createMesh()
    {
        //if a previous mesh is created destroy it
        destroyMesh();

        const u32 nbQuads = getTemplate()->getNbParticles();
        const u32 nbVertices = nbQuads * 4;
        const u32 nbIndices = nbQuads * 6;

#ifndef VBMANAGER_USE_DYNAMICRING_VB
        m_mesh.m_vtxBufferBuffered[0] = GFX_ADAPTER->createVertexBuffer(nbVertices, VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
        ITF_ASSERT(m_mesh.m_vtxBufferBuffered[0]);
        m_mesh.m_vtxBufferBuffered[1] = GFX_ADAPTER->createVertexBuffer(nbVertices, VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
        ITF_ASSERT(m_mesh.m_vtxBufferBuffered[1]);
#else
        m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(nbVertices, VertexFormat_PCT, sizeof(VertexPCT), 1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB

        //create mesh element
        if( m_mesh.m_ElementList.size() == 0 )
        {
            ITF_MeshElement elem;
            elem.m_startIndex = 0;
            elem.m_count = 0;
            m_mesh.m_ElementList.push_back(elem);
        }

        //create mesh element index buffer
        m_mesh.m_ElementList[0].m_indexBuffer = GFX_ADAPTER->createIndexBuffer(nbIndices, bfalse);

        //fill mesh element index buffer
        u16* indexBuffer;
        m_mesh.m_ElementList[0].m_indexBuffer->Lock((void**)&indexBuffer);
        for(u16 index = 0; index < nbQuads; ++index)
        {
            const u16 vertexStartIndex = index * 4;
            const u16 indiceStartIndex = index * 6;

            //first triangle
            indexBuffer[indiceStartIndex] = vertexStartIndex;
            indexBuffer[indiceStartIndex + 1] = vertexStartIndex + 1;
            indexBuffer[indiceStartIndex + 2] = vertexStartIndex + 2;
            //second triangle
            indexBuffer[indiceStartIndex + 3] = vertexStartIndex + 2;
            indexBuffer[indiceStartIndex + 4] = vertexStartIndex + 3;
            indexBuffer[indiceStartIndex + 5] = vertexStartIndex;
        }
        m_mesh.m_ElementList[0].m_indexBuffer->Unlock();

        //set up mesh element
        m_mesh.m_ElementList[0].m_count = nbIndices;
        m_mesh.m_ElementList[0].m_material.m_textureDiffuse = m_resource;
        m_mesh.m_ElementList[0].m_material.m_blendMode = GFX_BLEND_ADDALPHA;
        m_mesh.m_ElementList[0].m_material.m_TexAdressingModeU = GFXTADDRESS_MIRROR;
        m_mesh.m_ElementList[0].m_material.m_TexAdressingModeV = GFXTADDRESS_MIRROR;

        //set mesh color
        m_mesh.m_globalColor = Color::white();
        //set mesh matrix
        m_mesh.setMatrix((GMatrix44&)GMatrixIdentity);
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::destroyMesh()
    {
        //free vertex buffers
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        if( m_mesh.m_vtxBufferBuffered[0] != NULL )
        {
            GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBufferBuffered[0]);
            m_mesh.m_vtxBufferBuffered[0] = NULL;
        }
        if( m_mesh.m_vtxBufferBuffered[1] != NULL )
        {
            GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBufferBuffered[1]);
            m_mesh.m_vtxBufferBuffered[1] = NULL;
        }
#else
        if( m_mesh.m_vtxBuffer != NULL )
        {
            GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBuffer);
            m_mesh.m_vtxBuffer = NULL;	
        }		
#endif //VBMANAGER_USE_DYNAMICRING_VB

        //free index buffer
        if( m_mesh.m_ElementList.size() > 0 )
        {
            if(m_mesh.m_ElementList[0].m_indexBuffer != NULL)
            {
                GFX_ADAPTER->removeIndexBuffer(m_mesh.m_ElementList[0].m_indexBuffer);
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillAIComponent::buildMesh()
    {
        ITF_ASSERT( m_vertexBufferCache.size() == m_krillParticleData.size() * 4 );

        const Color defaultColor = Color::white();

        const Vec2d startVertexPosition = Vec2d(-1.0f, -1.0f).normalize();
        const f32 z = GetActor()->getDepth();

        for(u32 index = 0; index < m_krillParticleData.size(); ++index )
        {
            //get particle data
            const KrillParticleData & data = m_krillParticleData[index];
            
            //compute particle color
            Color particleColor = defaultColor;
            if( data.m_active )
            {
                particleColor.setAlpha(1.0f - data.m_deathRatio);
            }
            else
            {
                particleColor.setAlpha(0.0f);
            }
            const u32 u32Color = particleColor.getAsU32();

            //compute particle 3d position
            Vec3d particlePosition(data.m_position.m_x, data.m_position.m_y, z);

            //compute particle vertex buffer part
            const u32 vertexStartIndex = index * 4;

            Vec2d relativeVertexPosition = startVertexPosition.Rotate(data.m_angle) * data.m_size;
            m_vertexBufferCache[vertexStartIndex].m_pos = particlePosition + relativeVertexPosition.to3d();
            m_vertexBufferCache[vertexStartIndex].m_color = u32Color;
            m_vertexBufferCache[vertexStartIndex].m_uv = Vec2d::Zero;

            relativeVertexPosition = relativeVertexPosition.getPerpendicular();
            m_vertexBufferCache[vertexStartIndex + 1].m_pos = particlePosition + relativeVertexPosition.to3d();
            m_vertexBufferCache[vertexStartIndex + 1].m_color = u32Color;
            m_vertexBufferCache[vertexStartIndex + 1].m_uv = Vec2d::XAxis;

            relativeVertexPosition = relativeVertexPosition.getPerpendicular();
            m_vertexBufferCache[vertexStartIndex + 2].m_pos = particlePosition + relativeVertexPosition.to3d();
            m_vertexBufferCache[vertexStartIndex + 2].m_color = u32Color;
            m_vertexBufferCache[vertexStartIndex + 2].m_uv = Vec2d::One;

            relativeVertexPosition = relativeVertexPosition.getPerpendicular();
            m_vertexBufferCache[vertexStartIndex + 3].m_pos = particlePosition + relativeVertexPosition.to3d();
            m_vertexBufferCache[vertexStartIndex + 3].m_color = u32Color;
            m_vertexBufferCache[vertexStartIndex + 3].m_uv = Vec2d::YAxis;
        }
    }

    void Ray_FireFlyKrillAIComponent::drawMesh()
    {
        if( m_resource.isValidResourceId() )
        {
            Texture * tex = static_cast<Texture*>(m_resource.getResource());
            if( tex != NULL )
            {
                //get common information
                const f32 depth = m_actor->getDepth() + 10.0f;

#ifndef VBMANAGER_USE_DYNAMICRING_VB
                //switch mesh user buffer
                m_mesh.m_userBuffer = !m_mesh.m_userBuffer;
                u32 currentBuffer = m_mesh.m_userBuffer;
                //get vertex buffer ptr
                ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBufferBuffered[currentBuffer];
#else
                ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB

                //lock vertex buffer data
                VertexPCT* data = NULL;
                GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer, (void**)&data );
                ITF_ASSERT_MSG(data, "Unable to lock a vertex buffer");
                if(data != NULL)
                {
                    //write in vertex buffer data
                    ITF_MemcpyWriteCombined(data, m_vertexBufferCache.getDataBuffer(), sizeof(VertexPCT)*m_vertexBufferCache.size());
                    //unlock vertex buffer
                    GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);
                    //add primitive to be drawn
                    GFX_ADAPTER->addPrimitiveInZList((void*)&m_mesh, GFX_ITF_MESH, NULL, depth, btrue,m_actor->getRef());
                }
            }
        }
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

	IMPLEMENT_OBJECT_RTTI(Ray_FireFlyKrillAIComponent_Template)

	BEGIN_SERIALIZATION(Ray_FireFlyKrillAIComponent_Template)
        SERIALIZE_MEMBER("nbGroups", m_nbGroups);
        SERIALIZE_MEMBER("nbParticles", m_nbParticles);
        SERIALIZE_MEMBER("particleMinSize", m_particleMinSize);
        SERIALIZE_MEMBER("particleMaxSize", m_particleMaxSize);
        SERIALIZE_MEMBER("radius", m_krillRadius);
        SERIALIZE_MEMBER("generationRadius", m_krillGenerationRadius);
        SERIALIZE_MEMBER("lifeDuration", m_lifeDuration);
        SERIALIZE_MEMBER("deathDuration", m_deathDuration);
        SERIALIZE_MEMBER("lifeDurationRandom", m_lifeDurationRandom);
        SERIALIZE_MEMBER("separationDistance", m_separationDistance);
        SERIALIZE_MEMBER("separationFactor", m_separationFactor);
        SERIALIZE_MEMBER("attractionFactor", m_attractionFactor);
        SERIALIZE_MEMBER("frictionFactor", m_frictionFactor);
        SERIALIZE_MEMBER("positionOffsetFactor", m_positionOffsetFactor);
        SERIALIZE_MEMBER("speed", m_speed);
        SERIALIZE_MEMBER("krillRespawnSpeedFactor", m_krillRespawnSpeedFactor);
        SERIALIZE_MEMBER("krillBarycenterInfluence", m_krillBarycenterInfluence);
        SERIALIZE_MEMBER("textureFile", m_textureFile);
    END_SERIALIZATION()

	//------------------------------------------------------------------
	Ray_FireFlyKrillAIComponent_Template::Ray_FireFlyKrillAIComponent_Template()
    : m_nbGroups(8)
    , m_nbParticles(160)
    , m_particleMinSize(0.1f)
    , m_particleMaxSize(0.15f)
    , m_krillRadius(3.0f)
    , m_krillGenerationRadius(3.0f)
    , m_lifeDuration(5.0f)
    , m_lifeDurationRandom(5.0f)
    , m_deathDuration(1.0f)
    , m_separationDistance(0.25f)
    , m_separationFactor(1.0f)
    , m_attractionFactor(1.0f)
    , m_frictionFactor(1.0f)
    , m_positionOffsetFactor(0.5f)
    , m_speed(8.0f)
    , m_krillRespawnSpeedFactor(10.0f)
    , m_krillBarycenterInfluence(1.0f)
    , m_textureFile(Path::EmptyPath)
	{
	}

	//------------------------------------------------------------------
	Ray_FireFlyKrillAIComponent_Template::~Ray_FireFlyKrillAIComponent_Template()
	{
	}
}
