#include "precompiled_gameplay.h"

#ifndef _ITF_PENDULUMCOMPONENT_H_
#include "gameplay/Components/Environment/PendulumComponent.h"
#endif //_ITF_PENDULUMCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PendulumComponent)

BEGIN_SERIALIZATION_CHILD(PendulumComponent)
END_SERIALIZATION()


PendulumComponent::PendulumComponent()
: m_angle(0.f)
, m_angularVel(0.f)
{
}

PendulumComponent::~PendulumComponent()
{
}

void PendulumComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_angle = getTemplate()->getInitialAngle() * MTH_DEGTORAD;
    m_angularVel = getTemplate()->getInitialAngularVelocity();
}

void PendulumComponent::Update( f32 _deltaTime )
{
    Vec2d gravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
    m_angularVel = m_angularVel - (gravity.norm()*getTemplate()->getGravityMultiplier()*f32_Sin(m_angle)*_deltaTime);
    m_angle = m_angle + m_angularVel * _deltaTime / getTemplate()->getLength();
    m_actor->setAngle(m_actor->getWorldInitialRot()+m_angle);
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PendulumComponent_Template)
BEGIN_SERIALIZATION(PendulumComponent_Template)

    SERIALIZE_MEMBER("pendulumLength", m_length);
    SERIALIZE_MEMBER("pendulumInitialAngle", m_initialAngle);
    SERIALIZE_MEMBER("pendulumInitialVelocity", m_initialAngularVelocity);
    SERIALIZE_MEMBER("pendulumGravityMultiplier", m_gravityMultiplier);

END_SERIALIZATION()

PendulumComponent_Template::PendulumComponent_Template()
: m_length(2.f)
, m_initialAngle(45.f)
, m_initialAngularVelocity(0.f)
, m_gravityMultiplier(1.f)
{
}

}