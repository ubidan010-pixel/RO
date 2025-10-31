

#include "precompiled_gameplay.h"

#ifndef _ITF_AIBOUNCEACTION_H_
#include "gameplay/ai/Actions/AIBounceAction.h"
#endif //_ITF_AIBOUNCEACTION_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_


namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(AIBounceAction_Template)
    BEGIN_SERIALIZATION(AIBounceAction_Template)

        SERIALIZE_MEMBER("maxXSpeed",m_maxXSpeed);
        SERIALIZE_MEMBER("bounceForce",m_bounceForce);
        SERIALIZE_MEMBER("maxXSpeedThresholdForce",m_maxXSpeedThresholdForce);
        SERIALIZE_MEMBER("minBounceAngle",m_minBounceAngle);

    END_SERIALIZATION()

        AIBounceAction_Template::AIBounceAction_Template()
        : m_maxXSpeed(1.f)
        , m_bounceForce(400.0f)
        , m_maxXSpeedThresholdForce(1000.0f)
        , m_minBounceAngle(btrue,35.0f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(BOUNCE,421809396);
    }

    AIBounceAction_Template::~AIBounceAction_Template()
    {
    }


    IMPLEMENT_OBJECT_RTTI(AIBounceAction)

        AIBounceAction::AIBounceAction()
        : m_physComponent(NULL)
        , m_justStarted(bfalse)
        , m_speed(Vec2d::Zero)
    {
    }

    AIBounceAction::~AIBounceAction()
    {

    }

    void AIBounceAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),m_aiComponent);

        ITF_ASSERT(m_physComponent);

    }

    void AIBounceAction::onEvent(Event * _event)
    {
        if ( EventBlockedByPolyline * stickOnPolyEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
        {
            BaseObject* obj = GETOBJECT(stickOnPolyEvent->getPolyline());
            if ( obj )
            {
                const PolyLine* myPoly = static_cast<PolyLine*>(obj);
                const PolyLineEdge* myEdge = &myPoly->getEdgeAt(stickOnPolyEvent->getEdge());

                Vec2d speed = m_physComponent->getSpeed();
                if( speed != Vec2d::Zero)
                {
                    Vec2d speedN = speed;
                    f32 speedNorm = speed.norm();
                    speedN.normalize();
                    f32 angle = speedN.getOrientedAngle(myEdge->m_normalizedVector);
                    speed = speedNorm * myEdge->m_normalizedVector.Rotate(-angle);

                    m_speed = speed;
                  
                }
            }
        }
    }

    void AIBounceAction::onActivate()
    {
        Super::onActivate();
    }

    void AIBounceAction::update( f32 _delta )
    {
        Vec2d speed = m_physComponent->getSpeed();

        //if we just sticked bounce
        const PolyLineEdge * edge = m_physComponent->getStickedEdge();
        Vec2d bounceForce = Vec2d::Zero;
        if ( edge )
        {
            bounceForce = bounce(speed,edge->m_normalizedVector);
        }
        m_physComponent->addForce(bounceForce);

        // speed taken from blocking bounce
        if (m_speed != Vec2d::Zero)
        {
            m_physComponent->setSpeed(m_speed);
            m_speed = Vec2d::Zero;
        }

        // air travel speed limitation X

        f32 absSpeedX =  f32_Abs(speed.m_x);
        f32 speedXSign = speed.m_x > 0.0f ? 1.0f : -1.0f;
        if (absSpeedX > getTemplate()->getMaxXSpeed())
        {
            Vec2d force = Vec2d::Right * -speedXSign * (absSpeedX - getTemplate()->getMaxXSpeed()) * getTemplate()->getMaxXSpeedThresholdForce();
            m_physComponent->addForce(force);
        }

        m_previousSpeed = speed;
    }

    Vec2d AIBounceAction::bounce( const Vec2d& speed, const Vec2d& _edgeDir )
    {
        Vec2d force = AIUtils::mirrorVector(_edgeDir,-m_previousSpeed,MTH_PI);
        
        force.normalize();
        force *= getTemplate()->getBounceForce();

        return force;
    }

}

