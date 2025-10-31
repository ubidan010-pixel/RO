#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PhysComponent)

BEGIN_SERIALIZATION_CHILD(PhysComponent)
    
END_SERIALIZATION()

PhysComponent::PhysComponent()
: Super()
, m_gravityMultiplier(1.f)
, m_windMultiplier(1.f)
, m_waterMultiplier(1.f)
, m_weightMultiplier(1.f)
, m_radiusMultiplier(1.f)
, m_speed(Vec2d::Zero)
, m_force(Vec2d::Zero)
, m_totalForcesThisFrame(Vec2d::Zero)
, m_currentGravity(Vec2d::Zero)
, m_currentGravityDir(Vec2d::Zero)
, m_applyWindSpeedLimiter(btrue)
, m_disabled(bfalse)
{
}

PhysComponent::~PhysComponent()
{
}

void PhysComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_gravityMultiplier = getTemplate()->getInitialGravityMultiplier();
    m_windMultiplier = getTemplate()->getInitialWindMultiplier();
    m_waterMultiplier = getTemplate()->getInitialWaterMultiplier();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryPhysicsData,2528864682),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventAddForce,1731368564),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699),this);
}

void PhysComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    setSpeed(Vec2d::Zero);
    resetForces();
}

void PhysComponent::onForceMove( )
{
    m_speed = Vec2d::Zero;
}

f32 PhysComponent::getRadius() const
{
    return getTemplate()->getRadius() * m_actor->getScale().m_y*m_radiusMultiplier;
}

f32 PhysComponent::getWeight() const
{
    return getTemplate()->getWeight() * m_actor->getScale().m_y * m_weightMultiplier;
}

void PhysComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventQueryPhysicsData* queryPhysics = _event->DynamicCast<EventQueryPhysicsData>(ITF_GET_STRINGID_CRC(EventQueryPhysicsData,2528864682)) )
    {
        processQueryPhysics(queryPhysics);
    }
    else if (EventAddForce * onAddForce = _event->DynamicCast<EventAddForce>(ITF_GET_STRINGID_CRC(EventAddForce,1731368564)))
    {
        f32 multiplier = 1.0f;
        if( onAddForce->getForceType() == EventAddForce::ForceType_FAN )
        {
            multiplier = getTemplate()->getFanForceMultiplier();
        }
        addForce(onAddForce->getForce() * multiplier);
    }
    else if (EventSequenceActorActivate * sequenceActivate = _event->DynamicCast<EventSequenceActorActivate>(ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699)))
    {
        setDisabled(sequenceActivate->getActivate());
        resetForces();
    }
}

void PhysComponent::processQueryPhysics( class EventQueryPhysicsData* _query ) const
{
    _query->setSpeed(m_speed);
    _query->setWeight(getWeight());
    _query->setRadius(getRadius());
}

#ifdef ITF_SUPPORT_EDITOR
void PhysComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    DebugDraw::circle(m_actor->getPos(), getRadius(),
        Color::red(), 0.f, "phys");
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PhysComponent_Template)
BEGIN_SERIALIZATION(PhysComponent_Template)

    SERIALIZE_MEMBER("physRadius", m_radius);
    SERIALIZE_MEMBER("physWeight", m_weight);
    SERIALIZE_MEMBER("physGravityMultiplier", m_initialGravityMultiplier);
    SERIALIZE_MEMBER("physWindMultiplier", m_initialWindMultiplier);
    SERIALIZE_MEMBER("physWaterMultiplier", m_initialWaterMultiplier);
    SERIALIZE_MEMBER("physForce2Speed", m_force2speed);
    SERIALIZE_MEMBER("physWindSpeedLimit", m_windSpeedLimiter);
    SERIALIZE_MEMBER("physWindScaleFactorWhenSpeedIsOpposite", m_windMaxScaleOpposite);
    SERIALIZE_MEMBER("physFanForceMultiplier", m_fanForceMultiplier);

END_SERIALIZATION()

PhysComponent_Template::PhysComponent_Template()
: m_radius(1.f)
, m_weight(1.f)
, m_initialGravityMultiplier(1.f)
, m_initialWindMultiplier(1.f)
, m_initialWaterMultiplier(1.f)
, m_force2speed(LOGICDT)
, m_windSpeedLimiter(20.f)
, m_windMaxScaleOpposite(1.5f)
, m_fanForceMultiplier(1.f)
{
}

}

