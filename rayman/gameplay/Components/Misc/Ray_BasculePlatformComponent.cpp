#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYBASCULEPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_BasculePlatformComponent.h"
#endif //_ITF_RAYBASCULEPLATFORMCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_BasculePlatformComponent)

BEGIN_SERIALIZATION_CHILD(Ray_BasculePlatformComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_BasculePlatformComponent)
END_VALIDATE_COMPONENT()

Ray_BasculePlatformComponent::Ray_BasculePlatformComponent()
: m_forces(0.f)
, m_totalWeight(0.f)
, m_currentTorque(0.f)
, m_currentAngle(0.f)
{
}

void Ray_BasculePlatformComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
}

Ray_BasculePlatformComponent::~Ray_BasculePlatformComponent()
{
}

void Ray_BasculePlatformComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    m_totalWeight = 0.f;

    u32 numUsers = m_users.size();

    for ( u32 i = 0; i < numUsers; i++ )
    {
        const User& user = m_users[i];
        Actor* actor = AIUtils::getActor(user.m_user);

        if ( actor )
        {
            Vec2d pos2d = actor->get2DPos();

            applyWeightFromPoint(pos2d,user.m_weight);
        }
    }

    f32 targetAngle = m_totalWeight * getTemplate()->getWeightToAngle();
    targetAngle = ClampAngle(Angle(bfalse,targetAngle),-getTemplate()->getMaxAngle(),getTemplate()->getMaxAngle()).ToRadians();
    f32 delta = getShortestAngleDelta(m_currentAngle,targetAngle);

    m_forces += (delta * getTemplate()->getStiff()) - ( m_currentTorque * getTemplate()->getDamp());
    m_currentTorque += m_forces*_dt;
    m_currentAngle += m_currentTorque*_dt;
    m_currentAngle = ClampAngle(Angle(bfalse,m_currentAngle),-getTemplate()->getMaxAngle(),getTemplate()->getMaxAngle()).ToRadians();
    m_forces = 0.f;

    f32 newAngle = m_actor->getWorldInitialRot() - m_currentAngle;

    m_actor->setAngle(newAngle);
}

void Ray_BasculePlatformComponent::applyLand( ObjectRef _poly, const Vec2d& _pos, const Vec2d& _speed, f32 _weight, bbool _isCrushAttack )
{
    Vec2d gravityDir = PHYSWORLD->getGravity(_pos,m_actor->getDepth());

    gravityDir.normalize();

    Vec2d localPos;

    localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());

    if ( localPos != Vec2d::Zero )
    {
        f32 force = _speed.norm() * _weight * getTemplate()->getWeightMultiplier() * getTemplate()->getForceMultiplier();
        f32 radius = localPos.norm();

        localPos.normalize();

        f32 dotProd = localPos.dot(gravityDir.getPerpendicular());
        f32 angleSign = localPos.cross(gravityDir) > 0.f ? -1.f : 1.f;
        f32 angle = f32_ACos(dotProd)*angleSign;
        
        force *= radius * f32_Sin(angle);

        if ( _isCrushAttack )
        {
            force *= getTemplate()->getCrushMultiplier();
        }

        m_forces += force;
    }
}

void Ray_BasculePlatformComponent::applyWeightFromPoint( const Vec2d& _pos, f32 _weight )
{
    Vec2d gravityDir = PHYSWORLD->getGravity(_pos,m_actor->getDepth());

    gravityDir.normalize();

    Vec2d localPos = ( _pos - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());

    if ( localPos != Vec2d::Zero )
    {
        f32 dotProd = localPos.dot(gravityDir.getPerpendicular());
        f32 weight = _weight * getTemplate()->getWeightMultiplier() * dotProd;

        m_totalWeight += weight;
    }
}

void Ray_BasculePlatformComponent::onEvent( Event * _event )
{
    Super::onEvent(_event);

    if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        processStickEvent(stickEvent);
    }
    else if ( EventHanging* hangingEvent = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
    {
        processHangEvent(hangingEvent);
    }
}

void Ray_BasculePlatformComponent::processStickEvent( EventStickOnPolyline* _stickEvent )
{
    if (_stickEvent->isSticked())
    {
        User user;
        user.m_user = _stickEvent->getActor();
        user.m_weight = _stickEvent->getWeight();
        m_users.push_back(user);

        Actor* actor = user.m_user.getActor();
        bbool isCrushAttack = bfalse;

        if ( actor )
        {
            Ray_EventQueryHitType hitTypeQuery;
            actor->onEvent(&hitTypeQuery);
            isCrushAttack = (hitTypeQuery.getHitType() == PUNCHTYPE_CRUSH);
        }

        applyLand(_stickEvent->getPolyline(),_stickEvent->getPos(),_stickEvent->getSpeed(),_stickEvent->getWeight(),isCrushAttack);
    }
    else
    {
        for (u32 i = 0; i < m_users.size(); ++i)
        {
            if (m_users[i].m_user == _stickEvent->getActor())
            {
                m_users.eraseNoOrder(i);
                break;
            }
        }
    }
}

void Ray_BasculePlatformComponent::processHangEvent( EventHanging* _hangEvent )
{
    if (_hangEvent->isHanging())
    {
        User user;
        user.m_user = _hangEvent->getActor();
        user.m_weight = _hangEvent->getWeight();
        m_users.push_back(user);

        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(_hangEvent->getPolyline(),_hangEvent->getEdge(),poly,edge);

        if ( edge )
        {
            Vec2d pos = edge->getPos() + ( edge->m_vector * _hangEvent->getT() );

            applyLand(_hangEvent->getPolyline(),pos,_hangEvent->getSpeed(),_hangEvent->getWeight(),bfalse);
        }
    }
    else
    {
        for (u32 i = 0; i < m_users.size(); ++i)
        {
            if (m_users[i].m_user == _hangEvent->getActor())
            {
                m_users.eraseNoOrder(i);
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_BasculePlatformComponent_Template)
BEGIN_SERIALIZATION(Ray_BasculePlatformComponent_Template)
    SERIALIZE_MEMBER("stiff",m_stiff);
    SERIALIZE_MEMBER("damp",m_damp);
    SERIALIZE_MEMBER("weightToAngle",m_weightToAngle);
    SERIALIZE_MEMBER("maxAngle",m_maxAngle);
    SERIALIZE_MEMBER("weightMultiplier",m_weightMultiplier);
    SERIALIZE_MEMBER("forceMultiplier",m_forceMultiplier);
    SERIALIZE_MEMBER("crushMultiplier",m_crushMultiplier);
END_SERIALIZATION()

Ray_BasculePlatformComponent_Template::Ray_BasculePlatformComponent_Template()
: m_stiff(5.f)
, m_damp(2.5f)
, m_weightToAngle(0.05f)
, m_maxAngle(bfalse,0.785f)
, m_weightMultiplier(1.f)
, m_forceMultiplier(0.005f)
, m_crushMultiplier(100.f)
{
}

Ray_BasculePlatformComponent_Template::~Ray_BasculePlatformComponent_Template()
{
}

}
