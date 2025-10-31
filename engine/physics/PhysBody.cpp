#include "precompiled_engine.h"

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSISLAND_H_
#include "engine/physics/PhysIsland.h"
#endif //_ITF_PHYSISLAND_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(PhysBody)

PhysBody::PhysBody()
: m_force(Vec2d::Zero)
, m_speed(Vec2d::Zero)
, m_weight(0.f)
, m_gravityMultiplier(0.f)
, m_windMultiplier(0.f)
, m_collisionFilter(ECOLLISIONFILTER_NONE)
, m_static(bfalse)
, m_nextUpdate(NULL)
, m_prevUpdate(NULL)
{
}

void PhysBody::init( PhysShape* _shape, const PhysBodyInitCommonInfo& _initInfo )
{
    Super::init(_shape,
                _initInfo.m_pos,
                _initInfo.m_angle,
                _initInfo.m_userData,
                _initInfo.m_collisionGroup
#ifdef ITF_SUPPORT_DEBUGFEATURE
                ,_initInfo.m_owner
#endif //ITF_SUPPORT_DEBUGFEATURE
                );

    m_force = Vec2d::Zero;
    m_speed = Vec2d::Zero;
    m_weight = _initInfo.m_weight;
    m_gravityMultiplier = 1.f;
    m_windMultiplier = 1.f;
    m_collisionFilter = _initInfo.m_collisionFilter;
    m_static = _initInfo.m_static;
}

PhysBody::~PhysBody()
{
}

void PhysBody::setPos3D( const Vec3d& _pos )
{
    if(PhysIsland* pIsland = getIsland())
    {
        if(pIsland->getId() != DepthRange(_pos.m_z))
        {
            PHYSWORLD->removeBody(this);
            PHYSWORLD->insertBody(this, _pos.m_z);
        }
    }
    
    PhysCollidable::setPos3D(_pos);
}

void PhysBody::startFrame()
{
    ITF_ASSERT(getRef().getObject() == this);
#ifdef ITF_SUPPORT_DEBUGFEATURE
    ITF_ASSERT(m_owner.getObject());
#endif // ITF_SUPPORT_DEBUGFEATURE
    updateAABB();
}

void PhysBody::update( f32 _deltaTime )
{
    Vec2d forceDir = m_force;
    Vec2d gravityForce, speedMultiplier;

    forceDir.normalize();

    if ( m_island )
    {
        if ( !m_island->getGravityModifier(getPos(),gravityForce,speedMultiplier) )
        {
            gravityForce = PHYSWORLD->getGravity(getPos(),m_island->getZ());
        }
    }
    else
    {
        gravityForce = PHYSWORLD->getGravity(getPos(),0.f);
    }

    m_force += gravityForce * m_gravityMultiplier;

    if ( m_island )
    {
        PhysActorForceArray windForces;
        Vec2d totalWindForce = Vec2d::Zero;

        m_island->getWindForces(getPos(),m_speed,windForces);

        for ( u32 i = 0; i < windForces.size(); i++ )
        {
            totalWindForce += windForces[i].m_force;
        }

        m_force += totalWindForce * m_windMultiplier;
    }

    
    m_speed += m_force * LOGICDT;
    setPos(m_sweep.m_pos + (m_speed * _deltaTime));
}

void PhysBody::postUpdate( f32 _deltaTime )
{
    if ( !m_static )
    {
        if ( m_sweep.m_move )
        {
            m_speed = ( m_sweep.m_pos - m_sweep.m_prevPos ) / _deltaTime;
        }
        else
        {
            m_speed = Vec2d::Zero;
        }
    }

    synchronizePos();
    resetForce();
}

}
