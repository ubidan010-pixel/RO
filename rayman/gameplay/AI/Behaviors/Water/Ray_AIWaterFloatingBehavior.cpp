#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIWATERLOATINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterFloatingBehavior.h"
#endif //_ITF_RAY_AIWATERLOATINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERFLOATACTION_H_
#include "rayman/gameplay/ai/Actions/Water/Ray_AIWaterFloatAction.h"
#endif //_ITF_RAY_AIWATERFLOATACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_AIWaterFloatingBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIWaterFloatingBehavior_Template)
    SERIALIZE_MEMBER("minSpeed", m_pushMinSpeed);
    SERIALIZE_MEMBER("maxSpeed", m_pushMaxSpeed);
    SERIALIZE_MEMBER("minForce", m_pushMinForce);
    SERIALIZE_MEMBER("maxForce", m_pushMaxForce);
    SERIALIZE_MEMBER("speedScaleMin", m_pushSpeedScaleMin);
    SERIALIZE_MEMBER("speedScaleMax", m_pushSpeedScaleMax);
    SERIALIZE_MEMBER("landForceMultiplier", m_landForceMultiplier);
    SERIALIZE_MEMBER("landXForceMultiplier", m_landXForceMultiplier);
    SERIALIZE_OBJECT_WITH_FACTORY("float", m_float, ACTORSMANAGER->getAIActionsFactory());
END_SERIALIZATION()

Ray_AIWaterFloatingBehavior_Template::Ray_AIWaterFloatingBehavior_Template()
: m_float(NULL)
, m_pushMinSpeed(0.f)
, m_pushMaxSpeed(3.f)
, m_pushMinForce(50.f)
, m_pushMaxForce(250.f)
, m_pushSpeedScaleMin(0.f)
, m_pushSpeedScaleMax(5.f)
, m_landForceMultiplier(25.f)
, m_landXForceMultiplier(5.f)
{

}

Ray_AIWaterFloatingBehavior_Template::~Ray_AIWaterFloatingBehavior_Template()
{
    SF_DEL(m_float);

}
IMPLEMENT_OBJECT_RTTI(Ray_AIWaterFloatingBehavior)

BEGIN_VALIDATE_BEHAVIOR(Ray_AIWaterFloatingBehavior)
    VALIDATE_BEHAVIOR_PARAM("float", m_float, "this action is mandatory");
END_VALIDATE_BEHAVIOR()

Ray_AIWaterFloatingBehavior::Ray_AIWaterFloatingBehavior()
: m_float(NULL)
{
}

Ray_AIWaterFloatingBehavior::~Ray_AIWaterFloatingBehavior()
{
}

void Ray_AIWaterFloatingBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_float = static_cast<Ray_AIWaterFloatAction*>(createAiAction(getTemplate()->getFloat()));

    Super::onActorLoaded(_hotReload);

    m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

    ITF_ASSERT(m_physComponent);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),m_aiComponent);
}

void Ray_AIWaterFloatingBehavior::update( f32 _delta  )
{
    Super::update(_delta);

}

void Ray_AIWaterFloatingBehavior::onActivate()
{
    Super::onActivate();

    setAction(m_float);
}

void Ray_AIWaterFloatingBehavior::onDeactivate()
{
    Super::onActivate();
    setNullAction();
}

void Ray_AIWaterFloatingBehavior::onEvent( Event * _event)
{
    if ( EventPolylineBlockingContact* blockedEvent = _event->DynamicCast<EventPolylineBlockingContact>(ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979)) )
    {
        processBlockingEvent(blockedEvent);
    }
    else if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        processStickOnPolyline(stickEvent);
    }
}

void Ray_AIWaterFloatingBehavior::processBlockingEvent( EventPolylineBlockingContact* _blockedEvent )
{
    Vec2d dirToCenter = m_actor->get2DPos() - _blockedEvent->getContact();

    dirToCenter.normalize();

    f32 dotProd = dirToCenter.dot(_blockedEvent->getSpeed());

    if ( dotProd <= 0.f )
    {
        return;
    }

    const PhysContactsContainer& contacts = m_physComponent->getContacts();
    u32 numContacts = contacts.size();

    for ( u32 i = 0; i < numContacts; i++ )
    {
        const SCollidableContact& c = contacts[i];

        if ( c.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        PolyLine* poly = AIUtils::getPolyLine(c.m_collidableUserData);

        if ( !poly || poly->getOwnerActor() == m_actor )
        {
            continue;
        }

        f32 contactDotProd = c.m_normal.dot(dirToCenter);

        if ( contactDotProd >= 0.6f )
        {
            return;
        }
    }

    if ( getTemplate()->getPushMinSpeed() != getTemplate()->getPushMaxSpeed() )
    {
        dotProd = Clamp(dotProd,getTemplate()->getPushMinSpeed(),getTemplate()->getPushMaxSpeed());
        f32 t = ( dotProd - getTemplate()->getPushMinSpeed() ) / ( getTemplate()->getPushMaxSpeed() - getTemplate()->getPushMinSpeed() );
        Vec2d force = dirToCenter;

        force *= Interpolate(getTemplate()->getPushMinForce(),getTemplate()->getPushMaxForce(),t);

        Vec2d forceDir = force;

        forceDir.normalize();

        f32 speedProj = forceDir.dot(m_physComponent->getSpeed());

        if ( speedProj > 0.f )
        {
            speedProj = Clamp(speedProj,getTemplate()->getPushSpeedScaleMin(),getTemplate()->getPushSpeedScaleMax());

            f32 t = (speedProj-getTemplate()->getPushSpeedScaleMin()) / (getTemplate()->getPushSpeedScaleMax()-getTemplate()->getPushSpeedScaleMin());

            force *= 1.f-t;
        }

        m_physComponent->addForce(force);
    }
    else if ( getTemplate()->getPushMaxForce() )
    {
        Vec2d force = _blockedEvent->getSpeed();

        force.normalize();
        force *= getTemplate()->getPushMaxForce();

        m_physComponent->addForce(force);
    }
}

void Ray_AIWaterFloatingBehavior::processStickOnPolyline( EventStickOnPolyline* _stickEvent )
{
    if ( _stickEvent->isSticked() )
    {
        Vec2d force = _stickEvent->getSpeed()*getTemplate()->getLandForceMultiplier();

        force.m_x *= getTemplate()->getLandXForceMultiplier();

        Vec2d forceDir = force;

        forceDir.normalize();

        f32 speedProj = forceDir.dot(m_physComponent->getSpeed());

        if ( speedProj > 0.f )
        {
            speedProj = Clamp(speedProj,getTemplate()->getPushSpeedScaleMin(),getTemplate()->getPushSpeedScaleMax());

            f32 t = (speedProj-getTemplate()->getPushSpeedScaleMin()) / (getTemplate()->getPushSpeedScaleMax()-getTemplate()->getPushSpeedScaleMin());

            force *= 1.f-t;
        }

        m_physComponent->addForce(force);
    }
}

}

