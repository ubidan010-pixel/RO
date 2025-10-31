#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BABYPIRANHACOMPONENT_H_
#include "rayman/gameplay/Components/Piranha/Ray_BabyPiranhaComponent.h"
#endif //_ITF_RAY_BABYPIRANHACOMPONENT_H_

#ifndef _ITF_RAY_AIFRUITCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FRUitAIComponent.h"
#endif //_ITF_RAY_AIFRUITCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_


#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_


#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BabyPiranhaComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BabyPiranhaComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_OBJECT(Ray_BabyPiranhaComponent)
    VALIDATE_PARAM("fruitInfluenceDistMin",getTemplate()->getFruitInfluenceDistMin()>0,"fruit influence distance must be greater than 0");
    VALIDATE_PARAM("fruitInfluenceDistMax",getTemplate()->getFruitInfluenceDistMin()<getTemplate()->getFruitInfluenceDistMax() ,"fruit influence max distance must be strictly greater than its max influence");
    VALIDATE_PARAM("speedFractionWhenNearFruit", getTemplate()->getSpeedFractionWhenNearFruit()>0 && getTemplate()->getSpeedFractionWhenNearFruit()<1.f, "speed fraction must be in 0..1");
    VALIDATE_PARAM("pirahnaRadius", getTemplate()->getPirahnaRadius()>0, "pirahnaAABB must represent bounding sphere of 1 pyranha , centered on 0,0");
    END_VALIDATE_OBJECT()

    static PhysShapeCircle particleShape;


    Ray_BabyPiranhaComponent::Ray_BabyPiranhaComponent()
    : m_numParticles(0)
    , m_swimingPolyline(ITF_INVALID_OBJREF)
    , m_physComponent(NULL)
    , m_maxPerturbation(0.f)
    , m_escaping(bfalse)
    , m_rotationBlendFactor(0.95f)
    , m_pertubationRemaing(0.f)
    , m_pertubationDec(0.f)
    , m_canLimitSpeed(bfalse)
    , m_isScaredByFruit(btrue)
    {        
        memset(m_nbAttacking, 0, sizeof(m_nbAttacking));
        memset(m_maxAttacking , 0, sizeof(m_maxAttacking ));
        m_particleGeneratorList.push_back(&m_particleGeneratorStand);
        m_particleGeneratorList.push_back(&m_particleGeneratorAttack);
        ITF_ASSERT(m_particleGeneratorList.size()==State_Count);
    }

    Ray_BabyPiranhaComponent::~Ray_BabyPiranhaComponent()
    {
        clear();
    }

    void Ray_BabyPiranhaComponent::clear()
    {
        if ( m_actor && m_resource.isValidResourceId() )
        {
            m_actor->removeResource(m_resource);
            m_resource.invalidateResourceId();
        }

        m_isScaredByFruit = btrue;
        m_resource.invalidateResourceId();
    }

    void Ray_BabyPiranhaComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<PhysComponent>();
        m_resource = m_actor->addResource(Resource::ResourceType_Texture,getTemplate()->getTextureFile());
        m_particleGeneratorStand.init(&getTemplate()->getParticleGeneratorStand());
        m_particleGeneratorAttack.init(&getTemplate()->getParticleGeneratorAttack());
        
        //setup particles for each particle generator (stand/attack). Each is initialized from ACT parameters
        u32 count = m_particleGeneratorList.size();
        m_numParticles = 0; //Particle in each particle manager. Each should have same count
        for (u32 i=0; i<count; i++)
        {
            ITF_ParticleGenerator       *gen = m_particleGeneratorList[i];
            ParticleGeneratorParameters *params = gen->getParameters();

            //
            params->setFrequency(0); // manual process !!
            gen->setMaxParticles(params->getMaxParticles());
            params->setNumToEmit(U32_INVALID);
            params->setTexture(m_resource);
            params->setGeneratorMode(PARGEN_MODE_MANUAL);
            params->setPosition(m_actor->getPos());
            gen->spawnNParticle(params->getMaxParticles(), 0.2f);
            if (m_numParticles == 0 || m_numParticles > params->getMaxParticles())
                m_numParticles = params->getMaxParticles();

            //set position range
            if (i == 0)
            {
                m_maxXFromCenter = params->getBoxMax().m_x;
                m_minXFromCenter = params->getBoxMin().m_x;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaComponent::initParticles()
    {
        m_isScaredByFruit = btrue;
        if (m_numParticles <= 0)
            return;

        m_canLimitSpeed = bfalse;

        //Make an array of particles, compute a position in the available range, on the current surface edge of the polyline
        m_particles.resize(m_numParticles);

        const Vec2d actorPos      = m_actor->get2DPos();
        Seeder& seeder = Seeder::getSharedSeeder();
        for ( u32 i = 0; i < m_particles.size(); i++ )
        {
            ParticleData& data = m_particles[i];
            //const PolyLineEdge *pEdge = &m_inPolyline->getEdgeAt(m_surfaceEdge);
            f32 beginIndex = seeder.GetFloat(m_minXFromCenter, m_maxXFromCenter);

            data.m_standEdgeIndex   = m_surfaceEdge; //center is on edge # m_surfaceEdge
            data.m_standInEdgePos   = m_surfaceEdgeT; //and at m_surfaceEdgeT % of the edge
            data.m_standPosFromActor= beginIndex/* /pEdge->m_length*/; //pos relative to center
            data.m_lastPos          = m_actor->get2DPos();
            data.m_lastPos.m_x      += beginIndex;

            PolyLine* newPoly;
            //move on the polyline
            AIUtils::moveOnEdge( m_inPolyline, data.m_standPosFromActor, newPoly, data.m_standEdgeIndex, data.m_standInEdgePos, PolyLineEdge::Hole_Visual);
            ITF_ASSERT(m_inPolyline==newPoly);

            data.m_idleRadius       = seeder.GetFloat(getTemplate()->getIdleRadiusMin(), getTemplate()->getIdleRadiusMax())*0.5f; //movement amplitude when idle
            data.m_idleSpeed        = seeder.GetFloat(getTemplate()->getIdleSpeedMin(), getTemplate()->getIdleSpeedMax());
            data.m_xLimitMultiplier = seeder.GetFloat(getTemplate()->getLimitMultiplier(), 1.f);

            data.m_relativSpeed  = seeder.GetFloat(getTemplate()->getMinSpeed(), getTemplate()->getMaxSpeed());
            bool invert = seeder.GetFloat(1.f) > 0.5f;
            if (invert)
                data.m_speed      = -data.m_relativSpeed;
            else
                data.m_speed      =  data.m_relativSpeed;

            data.m_phase            = seeder.GetFloat(1.f/data.m_idleSpeed);
            data.m_currentGenerator = State_Stand; // begin stand
            data.m_attackIndex      = Attack_Air;

            data.m_randomizedFruitInfluenceDist = seeder.GetFloat(getTemplate()->getFruitInfluenceDistMin(),getTemplate()->getFruitInfluenceDistMax());
            data.m_isInfluencedByFruit = bfalse;
        }

        u32 count = m_particleGeneratorList.size();
        for (u32 i=0; i<count; i++)
        {
            ITF_ParticleGenerator *gen = m_particleGeneratorList[i];
            bbool alive = i==(u32)State_Stand;
            for (u32 j=0; j<m_numParticles; j++)
            {
                gen->changeManualParticleState(j, alive);
                gen->changeManualParticleFlip(j, m_particles[j].m_speed > 0.f);
            }
        }
        memset(m_nbAttacking, 0, sizeof(m_nbAttacking));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Set count of piranha to attack  with given attack index
    void Ray_BabyPiranhaComponent::startAttack(f32 percentAttacking, AttackType type)
    {
        u32 index = (u32)type;
        ITF_ASSERT((int)Attack_Count==2);
        m_maxAttacking[index] = (u32)(percentAttacking*m_numParticles);
        m_maxAttacking[index] = Clamp(m_maxAttacking[index], (u32)0, m_numParticles);
        if ((percentAttacking!=0) && (m_maxAttacking[index]==0))
        {
            m_maxAttacking[index] = 1;
        }


        m_maxAttacking[1-index] = Clamp(m_maxAttacking[1-index], (u32)0, m_numParticles - m_maxAttacking[index]);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaComponent::stopAllAttacks()
    {
        m_escaping = bfalse;
        for (u32 attackIdx=0; attackIdx<(u32)Attack_Count; attackIdx++)
            m_maxAttacking[attackIdx]    = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaComponent::startEscape()
    {
        m_escaping = btrue;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaComponent::processAttack()
    {
        ITF_ASSERT((int)Attack_Count==2);
        //for each attacked position (air/water)
        for (u32 attackType=0; attackType<(u32)Attack_Count; attackType++)
        {
            if (attackType==Attack_Air && !canAttackUp())
                continue;

            //make at most m_maxAttacking[attackType] (the *decided* count of attacker for this attack) piranha attack
            u32 localAttacking = m_nbAttacking[attackType];
            while (localAttacking < m_maxAttacking[attackType])
            {
                //get index of attacker, among particles that are not already attacking
                u32 remainingNonAttackingParticleCount = m_numParticles - m_nbAttacking[attackType] - m_nbAttacking[1-attackType];
                u32 attackerIdx = Seeder::getSharedSeeder().GetU32(1, remainingNonAttackingParticleCount) - 1;
                for (u32 pIdx=0; pIdx<m_numParticles; pIdx++)
                {
                    //Pick attackerIdx-th  particle among those who are in Stand mode
                    ParticleData& data = m_particles[pIdx];
                    if (data.m_currentGenerator == State_Stand) // stand mode
                    {
                        if (attackerIdx == 0)
                        {
                            //For this particle, make it attack if it is near the actor (otherwise, pass to another)
                            //we do so by increasing m_nbAttacking[attack type]
                            if (data.m_standPosFromActor < m_maxXFromCenter*data.m_xLimitMultiplier &&
                                data.m_standPosFromActor > m_minXFromCenter*data.m_xLimitMultiplier &&
                                !data.m_isInfluencedByFruit)
                            {                        
                                m_nbAttacking[attackType]++;
                                changeParticleMode(pIdx, State_Attack, (AttackType)attackType);
                            }
                            break;
                        } else
                        {
                            attackerIdx--;
                        }
                    }
                }
                ITF_ASSERT(attackerIdx == 0);
                localAttacking++;
            }
        }
    }


    void Ray_BabyPiranhaComponent::Update( f32 _deltaTime )
    {
        bbool isPrevOut = !m_swimingPolyline.isValid();
        if (isPrevOut)
        {
            //if is out of water (m_swimingPolyline is not valid), try to pick new one. If cant, exit
            m_inPolyline = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());
            if (!m_inPolyline)
                return;

            //init parameters to this polyline
            m_surfaceEdge = U32_INVALID;
            m_closestEdge = U32_INVALID;

            AIUtils::getSwimingPolylineData(m_inPolyline,m_actor->getPos(),-Vec2d::Up,0.1f,
                m_surfaceEdge,m_surfaceEdgeT,m_onSurface,
                m_closestEdge,m_closestEdgeT,m_inside);
            
            if (m_surfaceEdge != U32_INVALID && !(m_inPolyline->getHoleAt(m_surfaceEdge) & PolyLineEdge::Hole_Visual))
            {
                setSwimingPolyline(*m_inPolyline);
                if (!canGoInsideWater())
                {
                    moveOnPolyline(0);
                }
            }
        }

        m_inPolyline = (PolyLine*)m_swimingPolyline.getObject();
        if (!m_inPolyline)
        {
            clearSwimingPolyline();
            return;
        }
        else if (canGoInsideWater())
        {
            bbool wasOnSurface = m_onSurface;

            // we need to update the surface coordinates
            AIUtils::getSwimingPolylineData(m_inPolyline, m_actor->getPos(), -Vec2d::Up, 0.1f,
                m_surfaceEdge,m_surfaceEdgeT,m_onSurface,
                m_closestEdge,m_closestEdgeT,m_inside);

            if (wasOnSurface && !m_onSurface)
            {
                // we just got inside the water
                for ( u32 i = 0; i < m_particles.size(); i++ )
                {
                    ParticleData& data = m_particles[i];
                    data.m_standEdgeIndex = U32_INVALID;
                    data.m_standInEdgePos = 0.f;
                }
            }
            if (m_onSurface && !wasOnSurface)
            {
                // we just got back to the surface
                for ( u32 i = 0; i < m_particles.size(); i++ )
                {
                    ParticleData& data = m_particles[i];
                    data.m_standEdgeIndex = m_surfaceEdge;
                    data.m_standInEdgePos = m_surfaceEdgeT;
                }
            }
        }

#ifndef ITF_FINAL
        //DebugDraw::text("on surface: %d - %d - %.2f", m_onSurface, m_surfaceEdge, m_surfaceEdgeT);
        //DebugDraw::text("inside    : %d - %d - %.2f", m_inside, m_closestEdge, m_closestEdgeT);
#endif // ITF_FINAL

        //Setup perturbation in water
        if (m_inPolyline->m_ownerFrise && m_maxPerturbation > MTH_EPSILON)
        {
            // set perturbation
            m_pertubationRemaing -= _deltaTime;
            
            if (m_pertubationRemaing < 0.f)
            {
                m_pertubationDec        = m_actor->getRadius()*Seeder::getSharedSeeder().GetFloat(-1.f, 1.f)*0.5f;
                m_pertubationRemaing    = getTemplate()->getPertubationFrequence();
            }

            f32 mid = m_pertubationRemaing - getTemplate()->getPertubationFrequence()/2;
            if (mid > 0.f)
            {
                Vec2d   pos         = m_actor->get2DPos() + Vec2d::Right*m_pertubationDec;
                f32     radius      = m_physComponent ? m_physComponent->getRadius() * getTemplate()->getPerturbationRadiusRatio() : 0.f;
                f32     perturbation= m_maxPerturbation*(getTemplate()->getPertubationFrequence()/4 - f32_Abs(getTemplate()->getPertubationFrequence()/4 - mid));

                if (!canGoInsideWater() || m_onSurface)
                {
                    EventSwimingPlouf plouf(pos, perturbation, radius, bfalse);
                    m_inPolyline->m_ownerFrise->onEvent(&plouf);
                }
            }
        }

        if (isPrevOut)
        {
            //reinit particle if just put into water
            initParticles();
        }

        //normal behavior
        processAttack();
        m_isScaredByFruit = updateParticles(_deltaTime);
        updateGenerator(_deltaTime);

        //grow AABB by each item bounding box. NB : AABB was previously set by updateParticles.
        u32 count = m_particleGeneratorList.size();

        for (u32 i=0; i<count; i++)
        {
            ITF_ParticleGenerator *gen = m_particleGeneratorList[i];
            GetActor()->growRelativeAABBFromAbsolute(gen->getParameters()->getBoundingBox());
   
            // record gen for lightning
            WORLD_MANAGER->addParticleGenToLight(gen);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaComponent::findFruits(ActorList &_fruits)
    {
        _fruits.clear();
        _fruits.reserve(20);
        AABB searchZone(m_actor->getRelativeAABB());
        searchZone.grow(Vec2d(-getTemplate()->getFruitInfluenceDistMax(), -getTemplate()->getFruitInfluenceDistMax()));
        searchZone.grow(Vec2d(getTemplate()->getFruitInfluenceDistMax(), getTemplate()->getFruitInfluenceDistMax()));

        searchZone.Translate(m_actor->get2DPos());

        AI_MANAGER->getActorsFromLayerWithAIComponent<Ray_FruitAIComponent>(m_actor->getDepth(), searchZone, _fruits);

    }


    void Ray_BabyPiranhaComponent::moveOnPolyline(f32 _delta)
    {
        PolyLine *poly = ( PolyLine *)GETOBJECT(getSwimPolyline());
        if (!poly)
            return;

        //Move
        f32 usedDelta = _delta;
        PolyLine *newPolyline;

        //if edge is inverted, invert movement too because moveOnEdge is in edge direction
        const PolyLineEdge *pEdge = &poly->getEdgeAt(m_surfaceEdge);
        if (pEdge && pEdge->m_vector.m_x<0)
        {
            _delta = - _delta;
        }

        AIUtils::moveOnEdge(poly, usedDelta, newPolyline, m_surfaceEdge, m_surfaceEdgeT, PolyLineEdge::Hole_Visual);
        ITF_ASSERT(poly==newPolyline);

        //The following code is to stay away from borders
        f32 moveLeft    = m_minXFromCenter;
        AIUtils::moveOnEdge(poly, moveLeft, newPolyline, m_surfaceEdge, m_surfaceEdgeT, PolyLineEdge::Hole_Visual);
        ITF_ASSERT(poly==newPolyline);
            moveLeft    = -m_minXFromCenter;
        AIUtils::moveOnEdge(poly, moveLeft, newPolyline, m_surfaceEdge, m_surfaceEdgeT, PolyLineEdge::Hole_Visual);
        ITF_ASSERT(poly==newPolyline);

        f32 moveRight   = m_maxXFromCenter;
        AIUtils::moveOnEdge(poly, moveRight, newPolyline, m_surfaceEdge, m_surfaceEdgeT, PolyLineEdge::Hole_Visual);
        ITF_ASSERT(poly==newPolyline);
            moveRight   = -m_maxXFromCenter;
        AIUtils::moveOnEdge(poly, moveRight, newPolyline, m_surfaceEdge, m_surfaceEdgeT, PolyLineEdge::Hole_Visual);
        ITF_ASSERT(poly==newPolyline);

        //Compute position in 2D from edge and parametric position
        const PolyLineEdge & edge = poly->getEdgeAt(m_surfaceEdge);
        Vec2d                pos  = edge.getPos() + edge.m_vector*m_surfaceEdgeT + Vec2d::Up*getTemplate()->getDstFromSurface();

        //set position
        m_lastPos = pos;
        m_actor->setPos(pos.to3d(m_actor->getPos().m_z));
    }

    void Ray_BabyPiranhaComponent::updateGenerator( f32 _deltaTime )
    {
        //set position for each generator
        u32 count = m_particleGeneratorList.size();
        for (u32 i=0; i<count; i++)
        {
            ITF_ParticleGenerator *gen = m_particleGeneratorList[i];
            gen->startManualUpdate(m_actor->getPos());
            gen->update(_deltaTime, bfalse);
        }
    }

    bbool Ray_BabyPiranhaComponent::applyFruitInfluence(Vec2d &_pos, const ParticleData &_data, ActorList &_fruits)
    {
        f32 fruitInfluenceDist = _data.m_randomizedFruitInfluenceDist;
        f32 invFruitDistance = 1.f/fruitInfluenceDist;
        u32 fruitCount = _fruits.size();
        bbool isInfluencedByFruit = bfalse;

        for (u32 i=0; i<fruitCount; i++)
        {
            Vec2d fruitPos = _fruits[i]->get2DPos();

            Vec2d fruitToPiranha=_pos-fruitPos;
            //Implementation note : we could filter by y
            {
                f32 cosValue = (_pos.m_x-fruitPos.m_x)*invFruitDistance;
                if (f32_Abs(cosValue)<=1.f)
                {
                    isInfluencedByFruit = btrue;
                    f32 angle = f32_ACos(cosValue);
                    ITF_ASSERT(angle>=0 && angle<=MTH_PI);
                    f32 yValueOnInfluenceCircle = fruitPos.m_y-f32_Sin(angle)*fruitInfluenceDist;

                    f32 currentOffset = yValueOnInfluenceCircle-_pos.m_y;
                    if (currentOffset<0)
                    {
                        _pos.m_y += currentOffset;
                    }
                }
            }
        }

        return isInfluencedByFruit;
    }

    bbool Ray_BabyPiranhaComponent::updateParticles( f32 _deltaTime )
    {
        // Now update the velocities and positions of each particle
        Vec3d actorPos      = m_actor->getPos();
        Angle actorAngle(bfalse, m_actor->getAngle());
        Vec2d actorSpeed    = m_physComponent ? m_physComponent->getSpeed() : Vec2d::Zero;
        AABB aabb = m_actor->getAABB();
        bbool shouldNotAttack = bfalse;


        //static ActorList fruits;
        //fruits.clear();
        //findFruits(fruits);


        const f32 z = m_actor->getDepth();
        //for each particle, compute its position from 
        for ( u32 pIdx = 0; pIdx < m_numParticles; pIdx++ )
        {
            Vec2d           pos;
            Vec3d           speed;

            ParticleData& data = m_particles[pIdx];
            ITF_ParticleGenerator *gen = m_particleGeneratorList[(u32)data.m_currentGenerator];

            data.m_isInfluencedByFruit = bfalse;
            switch (data.m_currentGenerator)
            {
            case State_Attack:  // attacking
                if (updateParticlesAttack(gen, pIdx, _deltaTime, pos))
                {
                    f32   angle   =  data.m_phase*data.m_idleSpeed*MTH_2PI;
                    //We place a circle below the actor (top of the circle is the actor position)
                    //Let Angle be the vertical angle of point P on the circle (Angle=0 is for bottom)
                    //the Y position of pyranha is the Y of P
                    pos.m_y      -= (f32_Cos(angle) + 1.f)*data.m_idleRadius;
                    break;
                } else
                {
                    // recompute gen
                    gen = m_particleGeneratorList[(u32)data.m_currentGenerator];
                }
            case State_Stand: // standing
                {
                    updateParticlesStand(gen, pIdx, _deltaTime, pos, btrue);

                    // add phase
                    data.m_phase    += _deltaTime;
                    f32   angle   =  data.m_phase*data.m_idleSpeed*MTH_2PI;
                    pos.m_y      -= (f32_Cos(angle) + 1.f)*data.m_idleRadius;

                    data.m_isInfluencedByFruit = bfalse;//applyFruitInfluence(pos, data, fruits);

                    if (m_canLimitSpeed)
                    {
                        useSpeedLimit(pos, data.m_lastPos, data.m_speed, _deltaTime);
                    }
                } break;
            }


            shouldNotAttack = data.m_isInfluencedByFruit || shouldNotAttack;

            Vec2d dir = (pos - data.m_lastPos)*data.m_speed;
            data.m_lastAngle = Angle::Atan2(dir.m_y, dir.m_x)*(1.f-m_rotationBlendFactor) + data.m_lastAngle*m_rotationBlendFactor;
            data.m_lastPos = pos;
            aabb.grow(pos);
            gen->updateManualParticle(pIdx,pos.to3d(z), Vec2d::Right.Rotate(data.m_lastAngle.Radians()).to3d(),actorAngle);
        }
        m_actor->growRelativeAABBFromAbsolute(aabb);
        m_canLimitSpeed = btrue;

        return shouldNotAttack;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_BabyPiranhaComponent::useSpeedLimit(Vec2d &_pos, const Vec2d &_lastPos, f32 _speed, f32 _deltaTime)
    {
        Vec2d newSpeed(_pos-_lastPos);
        f32 len = newSpeed.norm();
        if (len!=0.f)
        {
            newSpeed *= f32_Abs(_speed)/len;
        }
        _pos = _lastPos + newSpeed * _deltaTime;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_BabyPiranhaComponent::isCollidingWithPiranha(Actor *_player, PhysShapeCircle *&_collidingPiranha, Vec2d &_collidingPos)
    {

        //get Piranha shape
        AABB relAABB(Vec2d(-getTemplate()->getPirahnaRadius(),-getTemplate()->getPirahnaRadius()),Vec2d(getTemplate()->getPirahnaRadius(),getTemplate()->getPirahnaRadius()));
        f32 shapeRadius = getTemplate()->getPirahnaRadius();
        particleShape.setRadius(shapeRadius);

        //get player shape and aabb
        EventQueryPhysShape queryShape;
        _player->onEvent(&queryShape);
        const PhysShape *playerShape = queryShape.getPhysShape();
        if (!playerShape)
        {
            return bfalse;
        }
        AABB playerAABB = _player->getAABB();

        //get sweep infos
        PhysSweepInfo playerSweep, particleSweep;
        PhysCollisionSolver::calculateSweepInfo(queryShape.getPosition(),
            queryShape.getPosition(),
            queryShape.getAngle(),
            queryShape.getPhysShape(),
            playerSweep);
        //
        static PhysContactsContainer contacts;
        //
        u32 particleCount = m_particles.size();
        for (u32 particleIndex=0; particleIndex<particleCount; particleIndex++)
        {
            const ParticleData &particle = m_particles[particleIndex];

            const Vec2d &pos = particle.m_lastPos;
            if (particle.m_currentGenerator != State_Attack)
                continue;
            AABB particleAABB(relAABB);
            particleAABB.Translate(pos);

            if (particleAABB.checkOverlap(playerAABB))
            {
                PhysCollisionSolver::calculateSweepInfo(pos,pos,0.f,&particleShape,particleSweep);

                if (PhysCollisionSolver::collide(playerSweep, particleSweep, contacts))
                {
                    contacts.clear();
                    _collidingPiranha = &particleShape;
                    _collidingPos = pos;
                    return btrue;
                }
                ITF_ASSERT(contacts.size()==0);
            }
        }
           
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_BabyPiranhaComponent::updateParticlesStand( ITF_ParticleGenerator* _gen, const i32 _index, const f32 _dt, Vec2d& _pos, const bbool _updateSpeed )
    {
        //update position of particle on edge, using the particle speed
        ParticleData& data = m_particles[_index];
        f32 previousSpeed = data.m_speed;


        bbool stayOnSurface = btrue;
        if (canGoInsideWater())
        {
            stayOnSurface = m_onSurface;
        }

        if (stayOnSurface)
        {
            // ON SURFACE

            f32 delta = data.m_speed * _dt;
            PolyLine* newPoly;
            if (AIUtils::moveOnEdge(m_inPolyline, delta, newPoly, data.m_standEdgeIndex, data.m_standInEdgePos, PolyLineEdge::Hole_Visual))
            {
                data.m_speed *= -1;
            }

            // Update position 
            data.m_standPosFromActor = AIUtils::getDistanceBetweenEdgePoints(m_inPolyline, m_surfaceEdge, m_surfaceEdgeT, data.m_standEdgeIndex, data.m_standInEdgePos);
            const PolyLineEdge *pEdge = &m_inPolyline->getEdgeAt(data.m_standEdgeIndex);
            _pos = pEdge->getPos() + pEdge->m_vector * data.m_standInEdgePos;
            _pos.m_y += getTemplate()->getYOffset();
        }
        else
        {
            // INSIDE WATER

            const Vec2d& actorPos = m_actor->get2DPos();

            _pos.m_x = data.m_lastPos.m_x + data.m_speed * _dt;
            _pos.m_y = actorPos.m_y + getTemplate()->getYOffset();

            data.m_standPosFromActor = _pos.m_x - actorPos.m_x;
        }


        f32 relativSpeed = data.m_relativSpeed;
        if (_updateSpeed)
        {
            f32 unsignedAcceleration = getTemplate()->getInertia() * 10 * _dt;
            f32 oldSpeed = data.m_speed;
            f32 newSpeed = oldSpeed;
            f32 signedAcceleration = (newSpeed < 0) ? -unsignedAcceleration : unsignedAcceleration;
            bbool noInertia = bfalse;

            if (!m_escaping)
            {
                //if not escaping and going beyond the limits, brake using a spring equation
                if (data.m_standPosFromActor > m_maxXFromCenter * data.m_xLimitMultiplier)
                {
                    newSpeed -= unsignedAcceleration;
                }
                else if (data.m_standPosFromActor < m_minXFromCenter * data.m_xLimitMultiplier)
                {
                    newSpeed += unsignedAcceleration;
                }
                else
                {
                    //if not leaving the zone, increase speed
                    newSpeed += signedAcceleration;
                }

                if (m_isScaredByFruit)
                {
                    relativSpeed *= getTemplate()->getSpeedFractionWhenNearFruit();
                }
            }
            else
            {
                //if escaping, increase speed
                newSpeed += signedAcceleration;
            }

            //clamp speed
            if (newSpeed < -relativSpeed)
            {
                newSpeed = -relativSpeed;
            }

            if (newSpeed > relativSpeed)
            {
                newSpeed = relativSpeed;
            }

            if (noInertia)
            {
                data.m_speed = newSpeed;
            }
            else if (oldSpeed < newSpeed)
            {
                data.m_speed = oldSpeed + unsignedAcceleration;
            } 
            else if (oldSpeed > newSpeed)
            {
                data.m_speed = oldSpeed - unsignedAcceleration;
            }
            else
            {
                data.m_speed = oldSpeed;
            }
        }

        if (previousSpeed * data.m_speed < 0)
        {
            //may change particle orientation
            _gen->changeManualParticleFlip(_index, data.m_speed > 0.f);
        }

        return btrue;
    }

    bbool Ray_BabyPiranhaComponent::updateParticlesAttack( ITF_ParticleGenerator* _gen, const i32 _index, const f32 _dt, Vec2d& _pos )
    {
        updateParticlesStand(_gen, _index, _dt, _pos, bfalse);

        //equation of attack : y is     sinf([0..PI])*data.m_attackMoveDst
        ParticleData& data = m_particles[_index];
        data.m_attackTime += _dt;
        f32 angle = data.m_attackTime/data.m_attackMoveTime*MTH_PI;
        _pos.m_y += data.m_attackMoveDst * f32_Sin(angle);

        if (data.m_attackTime < data.m_attackMoveTime)
        {
            return btrue;
        }

        // remove particle from nbAttack
        ITF_ASSERT(m_nbAttacking[data.m_attackIndex] > 0);
        m_nbAttacking[data.m_attackIndex]--;
        changeParticleMode(_index, State_Stand, data.m_attackIndex);
        return bfalse;
    }


    void Ray_BabyPiranhaComponent::Draw()
    {
        //render particles
        u32 count = m_particleGeneratorList.size();
        for (u32 i=0; i<count; i++)
        {
            ITF_ParticleGenerator *gen = m_particleGeneratorList[i];
            gen->render();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///change state of a given particle (remove from old generator, enable in #mode generator)
    ///Also set its attackIndex
    void Ray_BabyPiranhaComponent::changeParticleMode( u32 index, State mode, AttackType _attackIndex )
    {
        ParticleData& data = m_particles[index];
        if (data.m_currentGenerator == mode)
            return;

        //set state of particle in its previous generator to disable, and enable it in new generator
        ITF_ParticleGenerator *prevGen = m_particleGeneratorList[data.m_currentGenerator];
        data.m_currentGenerator = mode;
        data.m_attackIndex      = _attackIndex;
        ITF_ParticleGenerator *newGen = m_particleGeneratorList[data.m_currentGenerator];
        prevGen->changeManualParticleState(index, bfalse);
        newGen->changeManualParticleState(index, btrue);
        newGen->changeManualParticleFlip(index,data.m_speed > 0.f);
 
        //If we change to generator devoted to attacks, decide its attack time and distance (for jump/dive) attacks
        if (data.m_currentGenerator == State_Attack)
        {
            data.m_attackTime = 0;
            ITF_ASSERT((int)Attack_Count==2);
            if (data.m_attackIndex == Attack_Air)
            {
                data.m_attackMoveTime   = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinDiveTime(), getTemplate()->getMaxDiveTime());
                data.m_attackMoveDst    = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinJumpDst(), getTemplate()->getMaxJumpDst());
            } else
            {
                data.m_attackMoveTime   = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinDiveTime(), getTemplate()->getMaxDiveTime());
                data.m_attackMoveDst    = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMinDiveDst(),getTemplate()->getMaxDiveDst());
            }

            data.m_speed           *= getTemplate()->getAttackSlowDown();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///If a component updated the position, compute position on polyline
    void Ray_BabyPiranhaComponent::onUpdatePos()
    {
        if (canGoInsideWater())
        {
            return;
        }

        // move on X only
        if (m_lastPos.IsEqual(m_actor->get2DPos(), MTH_EPSILON))
        {
            return;
        }

        //set X position
        f32 xDest = m_actor->getPos().m_x - m_lastPos.m_x;
        moveOnPolyline(xDest);
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_BabyPiranhaComponent::onEditorMove(bbool _modifyInitialPos)
    {
        clearSwimingPolyline();
    }
#endif // ITF_SUPPORT_EDITOR

    Ray_BabyPiranhaComponent::PiranhaMap Ray_BabyPiranhaComponent::s_registeredPiranhas;

    void Ray_BabyPiranhaComponent::setSwimingPolyline( PolyLine& _polyline )
    {
        m_swimingPolyline = _polyline.getRef();

        s_registeredPiranhas.insert(std::pair<ObjectRef, ActorRef>( m_swimingPolyline, m_actor->getRef() ));
    }

    void Ray_BabyPiranhaComponent::clearSwimingPolyline()
    {
        if (m_swimingPolyline.isValid())
        {
            std::pair<PiranhaMap::iterator, PiranhaMap::iterator> range = s_registeredPiranhas.equal_range(m_swimingPolyline);
            for (PiranhaMap::iterator it = range.first; it != range.second; ++it)
            {
                if (it->second == m_actor->getRef())
                {
                    s_registeredPiranhas.erase(it);
                    break;
                }
            }
        }

        m_swimingPolyline.invalidate();
    }

    bbool Ray_BabyPiranhaComponent::hasPiranha( ObjectRef _waterPolyline )
    {
        return s_registeredPiranhas.count(_waterPolyline) > 0;
    }

    void Ray_BabyPiranhaComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        clearSwimingPolyline();
    }


    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BabyPiranhaComponent_Template)
    BEGIN_SERIALIZATION(Ray_BabyPiranhaComponent_Template)
        SERIALIZE_OBJECT("particleGeneratorStand",m_particleGeneratorStand);
        SERIALIZE_OBJECT("particleGeneratorAttack",m_particleGeneratorAttack);
        SERIALIZE_MEMBER("texture",m_textureFile);
        SERIALIZE_MEMBER("idleRadiusMin",m_idleRadiusMin); //NB : it's not a radius, it's a diameter
        SERIALIZE_MEMBER("idleRadiusMax",m_idleRadiusMax);
        SERIALIZE_MEMBER("idleSpeedMin",m_idleSpeedMin);
        SERIALIZE_MEMBER("idleSpeedMax",m_idleSpeedMax);
        SERIALIZE_MEMBER("inertia",m_inertia);
        SERIALIZE_MEMBER("xLimitMultiplier",m_xLimitMultiplier);
        SERIALIZE_MEMBER("desyncRatio",m_desyncRation);
        SERIALIZE_MEMBER("minJumpTime", m_minJumpTime);
        SERIALIZE_MEMBER("maxJumpTime", m_maxJumpTime);
        SERIALIZE_MEMBER("minJumpDst", m_minJumpDst);
        SERIALIZE_MEMBER("maxJumpDst", m_maxJumpDst);
        SERIALIZE_MEMBER("minDiveTime", m_minDiveTime);
        SERIALIZE_MEMBER("maxDiveTime", m_maxDiveTime);
        SERIALIZE_MEMBER("minDiveDst", m_minDiveDst);
        SERIALIZE_MEMBER("maxDiveDst", m_maxDiveDst);
        SERIALIZE_MEMBER("minSpeed", m_minSpeed);
        SERIALIZE_MEMBER("maxSpeed", m_maxSpeed);
        SERIALIZE_MEMBER("insideLen", m_insideLen);
        SERIALIZE_MEMBER("attackSlowDown", m_attackSlowDown);
        SERIALIZE_MEMBER("dstFromSurface", m_dstFromSurface);
        SERIALIZE_MEMBER("pertubationFrequence", m_pertubationFrequence);
        SERIALIZE_MEMBER("perturbationRadiusRatio", m_perturbationRadiusRatio);
        SERIALIZE_MEMBER("fruitInfluenceDistMin", m_fruitInfluenceDistMin);
        SERIALIZE_MEMBER("fruitInfluenceDistMax", m_fruitInfluenceDistMax);
        SERIALIZE_MEMBER("speedFractionWhenNearFruit", m_speedFractionWhenNearFruit);
        SERIALIZE_MEMBER("pirahnaRadius", m_pirahnaRadius);
        SERIALIZE_MEMBER("yOffset", m_yOffset);
        SERIALIZE_MEMBER("canGoInsideWater", m_canGoInsideWater);
    END_SERIALIZATION()

    Ray_BabyPiranhaComponent_Template::Ray_BabyPiranhaComponent_Template()
    : m_idleRadiusMin(0.f)
    , m_idleRadiusMax(0.f)
    , m_idleSpeedMin(1.f)
    , m_idleSpeedMax(1.f)
    , m_inertia(0.1f)
    , m_xLimitMultiplier(1.f)
    , m_desyncRation(1.f)
    , m_minJumpTime(2.f)
    , m_maxJumpTime(4.f)
    , m_minJumpDst(2.f)
    , m_maxJumpDst(4.f)
    , m_minDiveTime(2.f)
    , m_maxDiveTime(4.f)
    , m_minDiveDst(-1.f)
    , m_maxDiveDst(-2.f)
    , m_minSpeed(1.f)
    , m_maxSpeed(2.f)
    , m_insideLen(0.5f)
    , m_attackSlowDown(0.25f)
    , m_dstFromSurface(0.f)
    , m_pertubationFrequence(2.f)
    , m_perturbationRadiusRatio(0.1f)
    , m_fruitInfluenceDistMin(1.f)
    , m_fruitInfluenceDistMax(3.f)
    , m_speedFractionWhenNearFruit(0.5f)
    , m_pirahnaRadius(0)
    , m_yOffset(0)
    , m_canGoInsideWater(bfalse)
    {
    }
}
