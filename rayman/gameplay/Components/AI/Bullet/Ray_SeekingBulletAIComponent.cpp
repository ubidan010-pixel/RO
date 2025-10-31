#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SEEKINGBULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_SeekingBulletAIComponent.h"
#endif //_ITF_RAY_SEEKINGBULLETAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_SeekingBulletAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SeekingBulletAIComponent)
    END_SERIALIZATION()

    Ray_SeekingBulletAIComponent::Ray_SeekingBulletAIComponent()
        : Super()
        , m_offset(0.0f)
        , m_autoSeekCountdown(0.0f)
    {
    }

    Ray_SeekingBulletAIComponent::~Ray_SeekingBulletAIComponent()
    {
    }


    void Ray_SeekingBulletAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetTarget,4067373371), this);
    }

    void Ray_SeekingBulletAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventSetTarget* onSetTarget = _event->DynamicCast<EventSetTarget>(ITF_GET_STRINGID_CRC(EventSetTarget,4067373371)) )
        {
            m_target = onSetTarget->getTarget();
            m_offset = onSetTarget->getOffset().norm();
        }
    }

    void Ray_SeekingBulletAIComponent::updateMovement( f32 _dt )
    {
        if (m_currentBehavior == m_launchBehavior)
        {
            return;    
        }
        
        if (_dt <= 0.0f)
        {
            return;
        }

        Vec2d currentDirection = m_physComponent->getSpeed();
        currentDirection.normalize();
        m_actor->setAngle(currentDirection.getAngle() + (m_actor->isFlipped()?MTH_PI:0.0f));
        
        if (getTemplate()->getAutoSeek())
        {
            updateAutoSeek(_dt);
        }

        Actor* target = m_target.getActor();
        Vec2d flyDirection = currentDirection;
        if (target)
        {
            if ((target->get2DPos() - m_actor->get2DPos()).sqrnorm() < getTemplate()->getPhaseChangeRadius() * getTemplate()->getPhaseChangeRadius())
            {
                m_offset = 0.0f;
            }
            flyDirection = seek(_dt, target, currentDirection);
        }

        Vec2d targetSpeed = flyDirection * getTemplate()->getSpeed();
        Vec2d flyForce = (targetSpeed - m_physComponent->getSpeed()) / _dt;

        m_physComponent->addForce(flyForce - getCurrentGravity());

        bbool flipped = m_direction.dot(Vec2d::Right) < 0.0f;
        m_actor->setFlipped(flipped);

    }

    void Ray_SeekingBulletAIComponent::updateAutoSeek( f32 _dt )
    {
        m_autoSeekCountdown = Max(0.0f, m_autoSeekCountdown-_dt);
        if (m_autoSeekCountdown == 0.0f)
        {
            Actor* closestEnemy = AIUtils::getClosestEnemy(
                m_actor,
                m_actor->isFlipped(),
                getTemplate()->getAutoSeekRange());
            if (closestEnemy)
            {
                m_target = closestEnemy->getRef();
            }
            else
            {
                m_target = ActorRef::InvalidRef;
            }
            m_autoSeekCountdown = getTemplate()->getAutoSeekDelay();
        }
    }

    Vec2d Ray_SeekingBulletAIComponent::seek( f32 _dt, Actor* _target, Vec2d& _currentDirection )
    {
        const Vec2d targetDirection = ((_target->get2DPos()  + (Vec2d::Up * m_offset)) - m_actor->get2DPos()).normalize();
        const f32 currentAngle = m_actor->getAngle() + (m_actor->isFlipped()?MTH_PI:0.0f);
        const f32 targetAngle =  targetDirection.getAngle();
        const f32 angleDiff = getShortestAngleDelta(currentAngle,targetAngle);
        const f32 turnAngle = getTemplate()->getMaxTurnAngle().Radians() * _dt;
        f32 angleToTurn = 0.0f;
        const f32 sign = angleDiff > 0.0f? 1.0f:-1.0f;
        if (f32_Abs(angleDiff) > turnAngle)
            angleToTurn = sign * turnAngle;
        else
            angleToTurn = angleDiff;

        const f32 finalAngle = currentAngle + angleToTurn;
        const Vec2d finalDirection = Vec2d::Right.Rotate(finalAngle);
        return finalDirection;
    }


    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_SeekingBulletAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_SeekingBulletAIComponent_Template)
        SERIALIZE_MEMBER("maxTurnAngle", m_maxTurnAngle);
        SERIALIZE_MEMBER("phaseChangeRadius", m_phaseChangeRadius);
        SERIALIZE_MEMBER("autoSeek", m_autoSeek);
        SERIALIZE_MEMBER("autoSeekDelay", m_autoSeekDelay);
        SERIALIZE_OBJECT("autoSeekRange", m_autoSeekRange);
        
    END_SERIALIZATION()

    Ray_SeekingBulletAIComponent_Template::Ray_SeekingBulletAIComponent_Template()
    : Super()
    , m_maxTurnAngle(btrue,10.0f)
    , m_phaseChangeRadius(0.0f)
    , m_autoSeek(bfalse)
    , m_autoSeekDelay(1.0f)
    , m_autoSeekRange(Vec2d::Zero, Vec2d::Zero)
    {
    }

};