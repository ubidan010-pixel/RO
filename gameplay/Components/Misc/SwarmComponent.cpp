#include "precompiled_gameplay.h"

#ifndef _ITF_SWARMCOMPONENT_H_
#include "gameplay/components/misc/SwarmComponent.h"
#endif //_ITF_SWARMCOMPONENT_H_

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

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(SwarmComponent)

BEGIN_SERIALIZATION_CHILD(SwarmComponent)
END_SERIALIZATION()

SwarmComponent::SwarmComponent()
: m_best(Vec2d::Zero)
, m_bestDistSq(0.f)
, m_target(Vec2d::Zero)
, m_targetCounter(0.f)
{

}

SwarmComponent::~SwarmComponent()
{
    clear();
}

void SwarmComponent::clear()
{
    if ( m_actor && m_resource.isValidResourceId() )
    {
        m_actor->removeResource(m_resource);
        m_resource.invalidateResourceId();
    }

    m_resource.invalidateResourceId();
}

void SwarmComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    initGenerator();
    initParticles();
}

void SwarmComponent::initGenerator()
{
    m_resource = m_actor->addResource(Resource::ResourceType_Texture,getTemplate()->getTextureFile());
    m_particleGenerator.init(&getTemplate()->getParticleGenerator());
    m_particleGenerator.setMaxParticles(getTemplate()->getNumParticles());
    m_particleGenerator.getParameters()->setNumToEmit(U32_INVALID);
    m_particleGenerator.getParameters()->setTexture(m_resource);
    m_particleGenerator.getParameters()->setGeneratorMode(PARGEN_MODE_MANUAL);
    m_particleGenerator.getParameters()->setPosition(m_actor->getPos());
    m_particleGenerator.spawnNParticle(getTemplate()->getNumParticles(), 0.f);
}

void SwarmComponent::initParticles()
{
    m_particles.resize(getTemplate()->getNumParticles());

    for ( u32 i = 0; i < m_particles.size(); i++ )
    {
        ParticleData& data = m_particles[i];

        f32 radius = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStartRadius());
        f32 rot = Seeder::getSharedSeeder().GetFloat(MTH_2PI);
        Vec2d startPos = (Vec2d::Right*radius).Rotate(rot);
        data.m_pos = m_actor->get2DPos() + startPos;
        f32 speed = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStartSpeed());
        rot = Seeder::getSharedSeeder().GetFloat(MTH_2PI);
        data.m_speed = (Vec2d::Right*speed).Rotate(rot);
        data.m_best = Vec2d::Zero;
        data.m_bestDistSq = 0.f;
        data.m_rot = Seeder::getSharedSeeder().GetFloat(MTH_2PI);
        m_particleGenerator.changeManualParticleState(i, btrue);
    }

    m_best = Vec2d::Zero;
}

void SwarmComponent::onForceMove( )
{
    if(m_actor)
    {
        for ( u32 i = 0; i < m_particles.size(); i++ )
        {
            ParticleData& data = m_particles[i];

            f32 radius = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStartRadius());
            f32 rot = Seeder::getSharedSeeder().GetFloat(MTH_2PI);
            Vec2d startPos = (Vec2d::Right*radius).Rotate(rot);
            data.m_pos = m_actor->get2DPos() + startPos;
            data.m_bestDistSq = 0.f;
        }
        m_bestDistSq = 0.f;
    }
}

void SwarmComponent::Update( f32 _deltaTime )
{
    m_targetCounter -= _deltaTime;

    if ( m_targetCounter <= 0.f )
    {
        f32 radius = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStartRadius());
        f32 rot = Seeder::getSharedSeeder().GetFloat(MTH_2PI);
        m_target = m_actor->get2DPos() + (Vec2d::Right*radius).Rotate(rot);
        m_targetCounter = getTemplate()->getTargetTimer();
        m_bestDistSq = 0.f;

        for ( u32 i = 0; i < m_particles.size(); i++ )
        {
            ParticleData& data = m_particles[i];

            data.m_bestDistSq = 0.f;
        }
    }

    updateGenerator(_deltaTime);
    updateParticles(_deltaTime);

    updateSwarmComponentAABB();
}

void SwarmComponent::updateSwarmComponentAABB()
{
    GetActor()->growRelativeAABBFromAbsolute(m_particleGenerator.getParameters()->getBoundingBox());
}

void SwarmComponent::updateGenerator( f32 _deltaTime )
{
    m_particleGenerator.startManualUpdate(m_actor->getPos());
    m_particleGenerator.update(_deltaTime, bfalse);
}

void SwarmComponent::updateParticles( f32 _deltaTime )
{
    // First calculate the fitness value
    for ( u32 i = 0; i < getTemplate()->getNumParticles(); i++ )
    {
        ParticleData& data = m_particles[i];

        Vec2d distVec = m_target - data.m_pos;
        f32 distSq = distVec.sqrnorm();

        if ( !m_bestDistSq || distSq < m_bestDistSq )
        {
            m_best = data.m_pos;
            m_bestDistSq = distSq;
        }

        if ( !data.m_bestDistSq || distSq < data.m_bestDistSq )
        {
            data.m_best = data.m_pos;
            data.m_bestDistSq = distSq;
        }
    }

    static f32 dragDistSq = 1.5f*1.5f;
    f32 z = m_actor->getPos().m_z;
    
    // Now update the velocities and positions of each particle
    for ( u32 i = 0; i < getTemplate()->getNumParticles(); i++ )
    {
        ParticleData& data = m_particles[i];

        // Apply external forces
        PhysActorForceArray windForces;
        Vec2d wind = Vec2d::Zero;

        PHYSWORLD->getWindForces(data.m_pos, data.m_speed, m_actor->getDepth(), windForces);

        for ( u32 windIndex = 0; windIndex < windForces.size(); windIndex++ )
        {
            wind += windForces[windIndex].m_force;
        }

        data.m_speed += wind*getTemplate()->getWindMultiplier()*_deltaTime;

        f32 decision = Seeder::getSharedSeeder().GetFloat();
        Vec2d targetP = Vec2d::Zero;

        if ( decision < getTemplate()->getFollowBestChance() )
        {
            targetP = m_best;
        }
        else if ( decision < getTemplate()->getFollowTargetChance() )
        {
            targetP = m_target;
        }

        if ( targetP != Vec2d::Zero )
        {
            Vec2d vecToTarget = targetP - data.m_pos;

            if ( vecToTarget.sqrnorm() > dragDistSq )
            {
                f32 r1 = Seeder::getSharedSeeder().GetFloat();
                f32 r2 = Seeder::getSharedSeeder().GetFloat();

                Vec2d vecToGlobal = vecToTarget * ( getTemplate()->getGlobalInfluence() * r1 );
                Vec2d vecToLocal = ( data.m_best - data.m_pos ) * ( getTemplate()->getLocalInfluence() * r2 );
                Vec2d acceleration = vecToGlobal + vecToLocal;
                Vec2d result = ( acceleration * getTemplate()->getStiff() ) - ( data.m_speed * getTemplate()->getDamp() );

                data.m_speed += result * _deltaTime;
            }
        }
        
        data.m_pos += data.m_speed * _deltaTime;
        data.m_rot += ( data.m_speed.norm() * getTemplate()->getAngularRotationScale()  ) * _deltaTime;

        NormalizeAnglePI(data.m_rot);

        Vec3d pos = Vec3d(data.m_pos.m_x,data.m_pos.m_y,z);
        Vec3d speed = Vec3d(data.m_speed.m_x,data.m_speed.m_y,0.f);

        m_particleGenerator.updateManualParticle(i,pos,speed,Angle(btrue,data.m_rot));
    }
}

void SwarmComponent::Draw()
{
    m_particleGenerator.render();
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(SwarmComponent_Template)
BEGIN_SERIALIZATION(SwarmComponent_Template)

    SERIALIZE_MEMBER("swarmTexture",m_textureFile);
    SERIALIZE_MEMBER("numParticles",m_numParticles);
    SERIALIZE_MEMBER("startRadius",m_startRadius);
    SERIALIZE_MEMBER("startSpeed",m_startSpeed);
    SERIALIZE_MEMBER("followBestChance",m_followBestChance);
    SERIALIZE_MEMBER("followTargetChance",m_followTargetChance);
    SERIALIZE_MEMBER("targetTimer",m_targetTimer);
    SERIALIZE_MEMBER("windMultiplier",m_windMultiplier);
    SERIALIZE_MEMBER("angularRotationScale",m_angularRotationScale);
    SERIALIZE_MEMBER("globalInfluence",m_globalInfluence);
    SERIALIZE_MEMBER("localInfluence",m_localInfluence);
    SERIALIZE_MEMBER("damp",m_damp);
    SERIALIZE_MEMBER("stiff",m_stiff);
    SERIALIZE_OBJECT("particleGenerator",m_particleGenerator);

END_SERIALIZATION()

SwarmComponent_Template::SwarmComponent_Template()
: m_numParticles(300)
, m_startRadius(4.f)
, m_startSpeed(2.f)
, m_followBestChance(0.3f)
, m_followTargetChance(0.1f)
, m_targetTimer(1.f)
, m_windMultiplier(0.3f)
, m_angularRotationScale(0.1f)
, m_globalInfluence(3.f)
, m_localInfluence(1.f)
, m_damp(0.6f)
, m_stiff(1.1f)
{
}

}