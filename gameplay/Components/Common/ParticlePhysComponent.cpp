#include "precompiled_gameplay.h"

#ifndef _ITF_PARTICLEPHYSCOMPONENT_H_
#include "gameplay/Components/Common/ParticlePhysComponent.h"
#endif //_ITF_PARTICLEPHYSCOMPONENT_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_


namespace ITF
{

IMPLEMENT_OBJECT_RTTI(ParticlePhysComponent)

BEGIN_SERIALIZATION_CHILD(ParticlePhysComponent)
END_SERIALIZATION()

ParticlePhysComponent::ParticlePhysComponent()
    : Super()
{
}

ParticlePhysComponent::~ParticlePhysComponent()
{
}

void ParticlePhysComponent::Update( f32 _deltaTime )
{
    if ( m_pause )
    {
        return;
    }

    Vec3d   pos = m_actor->getPos();
#ifdef ASSERT_ENABLED
    const f32 oldZ = pos.m_z;
#endif
    f32     angle = m_actor->getAngle();
    const Vec2d& pos2d = pos.truncateTo2D();

    updateCurrentGravity(pos2d);

    if ( m_disabled )
    {
        applyPosAndAngle(pos,angle);
        return;
    }

    // actor-bound + physics = unsafe
    ITF_WARNING(
        m_actor,
        !m_actor->getParentBind() || m_actor->getParentBind()->isRuntimeDisabled(),
        "This actor shouldn't be bound because its ParticlePhysComponent "
        "updates its position in world coordinates.");

    // Put all forces together and add friction to update the speed
    calculateSpeedFromForces(_deltaTime,pos.truncateTo2D());

    // Update our position
    move(_deltaTime,pos,angle);

    // Update our orientation
    rotate(_deltaTime,pos.truncateTo2D(),angle);

    // Apply the final data to the actor and physic body
    applyPosAndAngle(pos,angle);

    // Reset the forces
    m_force = Vec2d::Zero;
    ITF_ASSERT(oldZ == pos.m_z);
}

void ParticlePhysComponent::onForceMove( )
{
    m_speed = Vec2d::Zero;
}

void ParticlePhysComponent::updateCurrentGravity( const Vec2d& _pos )
{
    m_currentGravityDir = m_currentGravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
    m_currentGravityDir.normalize();
}

// Apply the forces and friction from the environment, then calculate our current speed
void ParticlePhysComponent::calculateSpeedFromForces( f32 _deltaTime, const Vec2d& _pos )
{
    PhysActorForceArray windForces;
    Vec2d totalWindForce = Vec2d::Zero;

    PHYSWORLD->getWindForces(m_actor->get2DPos(), m_speed, m_actor->getDepth(), windForces);

    for ( u32 i = 0; i < windForces.size(); i++ )
    {
        PhysActorForce& wind = windForces[i];

        if ( m_actor->getBinding()->isDescendant(wind.m_actor) )
        {
            continue;
        }

        totalWindForce += windForces[i].m_force;
    }

    Vec2d wind = totalWindForce * m_windMultiplier;

    if ( wind != Vec2d::Zero && m_applyWindSpeedLimiter )
    {
        PhysForceModifier::applySpeedLimitation(getTemplate()->getForce2Speed(),getTemplate()->getWindSpeedLimiter(),getTemplate()->getWindMaxScaleOpposite(),m_speed,wind);
    }


    m_force                 += wind;

    m_totalForcesThisFrame  = m_force;
    m_totalForcesThisFrame  += m_currentGravity*m_gravityMultiplier;

    m_speed                 += m_totalForcesThisFrame*_deltaTime;

    m_speed *= 1.f - getTemplate()->getFriction()*_deltaTime;
    //if ( m_friction )
    //{
    //    f32 speedSize = m_speed.norm();
    //    if ( speedSize > m_friction )
    //    {
    //        speedSize -= m_friction;
    //        m_speed.normalize();
    //        m_speed *= speedSize;
    //    }
    //    else
    //    {
    //        m_speed = Vec2d::Zero;
    //    }
    //}
}

// Get our current speed based on our linear speed
f32 ParticlePhysComponent::calculateAngularSpeedFromLinearSpeed() const
{
    f32 currentSpeed = m_speed.norm();

    currentSpeed = Clamp(currentSpeed,getTemplate()->getAngularSpeedMinLinear(),getTemplate()->getAngularSpeedMaxLinear());

    f32 t = ( currentSpeed - getTemplate()->getAngularSpeedMinLinear() ) / ( getTemplate()->getAngularSpeedMaxLinear() - getTemplate()->getAngularSpeedMinLinear() );

    f32     angularSpeed = Interpolate(getTemplate()->getAngularSpeedMinAngular().ToRadians(),getTemplate()->getAngularSpeedMaxAngular().ToRadians(),t);
    return  angularSpeed;
}

// Move the character in the air or on a polyline
void ParticlePhysComponent::move( f32 _delta, Vec3d& _pos, f32 _angle )
{
    _pos += (m_speed * _delta).to3d();
}

void ParticlePhysComponent::rotate( f32 _deltaTime, const Vec2d& _pos, f32& _angle )
{
    // If we are not sticked to a polyline we set our orientation back to zero over time with
    // our torque
    f32 zeroAngle = getVec2Angle(m_currentGravityDir) + MTH_PIBY2;

    if ( _angle != zeroAngle )
    {
        f32 angularSpeed = calculateAngularSpeedFromLinearSpeed();
        f32 dif = getShortestAngleDelta(_angle,zeroAngle);

        if ( f32_Abs(dif) < (angularSpeed*_deltaTime) )
        {
            _angle = zeroAngle;
        }
        else
        {
            if ( dif > 0.f )
            {
                _angle = _angle+(angularSpeed*_deltaTime);
            }
            else if ( dif < 0.f )
            {
                _angle = _angle-(angularSpeed*_deltaTime);
            }
        }
    }
    NormalizeAnglePI(_angle);
}

void ParticlePhysComponent::applyPosAndAngle( const Vec3d& _pos, f32 _angle )
{
    m_actor->setPos(_pos);
    m_actor->setAngle(_angle);

    m_actor->updateComponentPosAngle(_pos,_angle,this);
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(ParticlePhysComponent_Template)
BEGIN_SERIALIZATION_CHILD(ParticlePhysComponent_Template)

    SERIALIZE_MEMBER("physFriction",m_friction);
    SERIALIZE_MEMBER("physAngularSpeedMinLinear", m_angularSpeedMinLinear);
    SERIALIZE_MEMBER("physAngularSpeedMaxLinear", m_angularSpeedMaxLinear);
    SERIALIZE_MEMBER("physAngularSpeedMinAngular", m_angularSpeedMinAngular);
    SERIALIZE_MEMBER("physAngularSpeedMaxAngular", m_angularSpeedMaxAngular);

END_SERIALIZATION()

ParticlePhysComponent_Template::ParticlePhysComponent_Template()
: m_friction(0.f)
, m_angularSpeedMinLinear(1.f)
, m_angularSpeedMaxLinear(10.f)
, m_angularSpeedMinAngular(bfalse,150.f)
, m_angularSpeedMaxAngular(bfalse,250.f)
{
}

} // namespace
