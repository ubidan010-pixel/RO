#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIWATERFLOATACTION_H_
#include "rayman/gameplay/ai/Actions/Water/Ray_AIWaterFloatAction.h"
#endif //_ITF_RAY_AIWATERFLOATACTION_H_

#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterBaseBehavior.h"
#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_AIWaterFloatAction_Template)

BEGIN_SERIALIZATION_CHILD(Ray_AIWaterFloatAction_Template)

    SERIALIZE_MEMBER("floatForceDistMultiplier",m_floatForceDistMult);
    SERIALIZE_MEMBER("floatForceSpeedMultiplier",m_floatForceSpeedMult);
    SERIALIZE_MEMBER("orientForceDistMultiplier",m_orientForceDistMult);
    SERIALIZE_MEMBER("orientForceSpeedMultiplier",m_orientForceSpeedMult);
    SERIALIZE_MEMBER("weightMultiplier",m_weightMultiplier);
    SERIALIZE_MEMBER("speedMultiplier",m_speedMultiplier);

END_SERIALIZATION()

Ray_AIWaterFloatAction_Template::Ray_AIWaterFloatAction_Template()
: m_floatForceDistMult(10.f)
, m_floatForceSpeedMult(4.f)
, m_orientForceDistMult(50.f)
, m_orientForceSpeedMult(4.f)
, m_weightMultiplier(5.f)
, m_speedMultiplier(5.f)
{
    m_animDefaultAction = ITF_GET_STRINGID_CRC(WATERFLOAT,4128675539);
}

IMPLEMENT_OBJECT_RTTI(Ray_AIWaterFloatAction)

Ray_AIWaterFloatAction::Ray_AIWaterFloatAction()
: m_waterBehavior(NULL)
, m_physComponent(NULL)
, m_orientSpeed(0.f)
, m_forces(Vec2d::Zero)
{
}

Ray_AIWaterFloatAction::~Ray_AIWaterFloatAction()
{
}

void Ray_AIWaterFloatAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_waterBehavior = m_aiBehavior->DynamicCast<Ray_AIWaterBaseBehavior>(ITF_GET_STRINGID_CRC(Ray_AIWaterBaseBehavior,463503610));
    m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),m_aiComponent);
}

void Ray_AIWaterFloatAction::validate( bbool& _isComponentValidated )
{
    Super::validate(_isComponentValidated);

    VALIDATE_COMPONENT_PARAM("Ray_AIWaterFloatAction",m_waterBehavior != NULL,
        "Ray_AIWaterFloatAction:: %s - missing Ray_AIWaterBaseBehavior",
        m_actor->getUserFriendly().cStr());

    VALIDATE_COMPONENT_PARAM("Ray_AIWaterFloatAction",m_physComponent != NULL,
        "Ray_AIWaterFloatAction:: %s - missing StickToPolylinePhysComponent",
        m_actor->getUserFriendly().cStr());
}

void Ray_AIWaterFloatAction::onActivate()
{
    Super::onActivate();

    m_orientSpeed = m_physComponent->getTorqueAverage();
    m_forces = Vec2d::Zero;
}

void Ray_AIWaterFloatAction::onEvent( Event * _event )
{
    Super::onEvent(_event);
    if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        // someone landed on us or jumped from us
        processStick(stickEvent);
    }
}

void Ray_AIWaterFloatAction::processStick( EventStickOnPolyline* _stickEvent )
{
    if ( !_stickEvent->isSticked() )
    {
        return;
    }

    Vec2d landForce = _stickEvent->getSpeed() * getTemplate()->getSpeedMultiplier() * _stickEvent->getWeight() * getTemplate()->getWeightMultiplier();

    m_forces += landForce;
}

void Ray_AIWaterFloatAction::update( f32 _dt )
{
    Super::update(_dt);
    if ( !m_waterBehavior || !m_physComponent )
    {
        return;
    }

    u32 surfaceEdgeIndex = m_waterBehavior->getSurfaceEdge();

    if ( surfaceEdgeIndex == U32_INVALID )
    {
        return;
    }

    PolyLine* poly = AIUtils::getPolyLine(m_waterBehavior->getSwimingPolyline());

    if ( !poly )
    {
        return;
    }

    const PolyLineEdge& surfaceEdge = poly->getEdgeAt(surfaceEdgeIndex);

    // Rotate the object towards the surface orientation
    f32 surfaceAngle = getVec2Angle(surfaceEdge.m_normalizedVector);
    f32 dist = getShortestAngleDelta(m_actor->getAngle(),surfaceAngle);
    f32 torque = ( dist * getTemplate()->getOrientForceDistMult() ) - ( m_orientSpeed * getTemplate()->getOrientForceSpeedMult() );

    m_orientSpeed += torque*_dt;
    
    f32 newAngle = m_actor->getAngle();

    newAngle += m_orientSpeed * _dt;

    m_actor->setAngle(newAngle);

    // Add the force that pull us towards the surface
    Vec2d surfacePos = surfaceEdge.getPos() + (surfaceEdge.m_vector*m_waterBehavior->getSurfaceEdgeT());
    Vec2d vecToSurface = surfacePos - m_actor->get2DPos();
    f32 upSpeed = Vec2d::Up.dot(m_physComponent->getSpeed());
    Vec2d force = ( vecToSurface * getTemplate()->getFloatForceDistMult() ) - ( (Vec2d::Up*upSpeed) * getTemplate()->getFloatForceSpeedMult() );

    if ( m_waterBehavior )
    {
        if ( !m_waterBehavior->getIsInside() )
        {
            force += m_aiComponent->getCurrentGravity();
        }

        if ( force != Vec2d::Zero )
        {
            // Counter the water resistance
            f32 waterResistance = m_physComponent->getTotalAirFriction();

            if ( waterResistance )
            {
                Vec2d forceDir = force;

                forceDir.normalize();

                Vec2d counterResistance = forceDir * waterResistance;

                m_physComponent->addForce(counterResistance);
            }
        }
    }

    m_physComponent->addForce(force+m_forces);

    m_forces = Vec2d::Zero;
}

}