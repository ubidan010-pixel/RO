#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEJUMPPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPolylineJumpPunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPOLYLINEJUMPPUNCHACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitPolylineJumpPunchAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIPerformHitPolylineJumpPunchAction_Template)

    SERIALIZE_MEMBER("jumpForce", m_jumpYForce);
    SERIALIZE_MEMBER("jumpXForce", m_jumpXForce);
        SERIALIZE_MEMBER("antiGravTime", m_antiGravTime);

    END_SERIALIZATION()

    Ray_AIPerformHitPolylineJumpPunchAction_Template::Ray_AIPerformHitPolylineJumpPunchAction_Template()
    : m_jumpYForce(200.0f)
    , m_jumpXForce(0.0f)
    , m_antiGravTime(1.0f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitPolylineJumpPunchAction)

    Ray_AIPerformHitPolylineJumpPunchAction::Ray_AIPerformHitPolylineJumpPunchAction()
    : Super()
    , m_physComponent(NULL)
    , m_speedReset(bfalse)
    , m_previousGravityMultiplier(1.0f)
    , m_timer(0.0f)
    , m_jump(bfalse)
    , m_antiGravStart(bfalse)
    , m_antiGravStop(bfalse)
    , m_justJumped(bfalse)
    {
    }

    Ray_AIPerformHitPolylineJumpPunchAction::~Ray_AIPerformHitPolylineJumpPunchAction()
    {
    }

    void Ray_AIPerformHitPolylineJumpPunchAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

    }
    void Ray_AIPerformHitPolylineJumpPunchAction::onActivate()
    {
        Super::onActivate();

        m_physComponent->setSpeed(Vec2d::Zero);
        m_physComponent->resetForces();
        m_speedReset = bfalse;
        m_jump = bfalse;
        m_antiGravStart = bfalse;
        m_antiGravStop = bfalse;
        m_justJumped = bfalse;
    }

    void Ray_AIPerformHitPolylineJumpPunchAction::update( f32 _dt )
    {
        Super::update( _dt );
        if (m_justJumped && m_physComponent->getStickedPolyline())
        {
            m_justJumped = bfalse;
            m_aiBehavior->onActionFinished();
            return;
        }
        if (m_jump)
        {
            m_jump = bfalse;
            const Vec2d lookdir = AIUtils::getLookDir(m_actor->getAngle(),m_actor->isFlipped());
            const f32 angle = lookdir.dot(Vec2d::Right) > 0.0f ? m_actor->getAngle() : MTH_PI - m_actor->getAngle();
            m_physComponent->addForce(Vec2d::Up.Rotate(m_actor->getAngle()) * getTemplate()->getJumpYForce());
            m_physComponent->addForce(Vec2d::Right.Rotate(angle) * getTemplate()->getJumpXForce());
            m_justJumped = btrue;
        }
        if (m_antiGravStart)
        {
            m_antiGravStart = bfalse;
            m_previousGravityMultiplier = m_physComponent->getGravityMultiplier();
            m_physComponent->setGravityMultiplier(0.0f);
        }
        if (m_antiGravStop)
        {
            m_antiGravStop = bfalse;
            m_physComponent->setGravityMultiplier(m_previousGravityMultiplier);
        }

    }

    void Ray_AIPerformHitPolylineJumpPunchAction::onDeactivate()
    {
        Super::onDeactivate(  );
        m_physComponent->setGravityMultiplier(m_previousGravityMultiplier);

    }

    void Ray_AIPerformHitPolylineJumpPunchAction::onEvent( Event* _event )
    {
        Super::onEvent(_event);
        if (AnimGameplayEvent * gEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (gEvent->getName() == "MRK_Jump")
            {
                m_jump = btrue;
            }
            else if (gEvent->getName() == "MRK_Antigrav_Start")
            {
                m_antiGravStart = btrue;
            }
            else if (gEvent->getName() == "MRK_Antigrav_Stop")
            {
                m_antiGravStop = btrue;
            }
        }
    }

}