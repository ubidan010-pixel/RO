#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDDARKTOONEDBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundDarktoonedBehavior.h"
#endif //_ITF_RAY_AIGROUNDDARKTOONEDBEHAVIOR_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundDarktoonedBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIGroundDarktoonedBehavior_Template)
        SERIALIZE_MEMBER("stimEmissionSpeedLimit", m_stimEmissionSpeedLimit);
        SERIALIZE_MEMBER("stimEmissionOffset", m_stimEmissionOffset);
        SERIALIZE_MEMBER("stimEmissionOffsetEnd", m_stimEmissionOffsetEnd);
        SERIALIZE_MEMBER("stimLevel", m_stimLevel);
        SERIALIZE_RECEIVEDHITTYPE("stimType", m_stimType);
    END_SERIALIZATION()

    Ray_AIGroundDarktoonedBehavior_Template::Ray_AIGroundDarktoonedBehavior_Template()
    : m_stimEmissionSpeedLimit(1.f)
    , m_stimEmissionOffset(0.5f, -0.5f)
    , m_stimEmissionOffsetEnd(0.5f, 0.5f)
    , m_stimLevel(1)
    , m_stimType(RECEIVEDHITTYPE_EJECTY)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundDarktoonedBehavior)

    Ray_AIGroundDarktoonedBehavior::Ray_AIGroundDarktoonedBehavior()
    : Super()
    , m_stimPoints(4)
    {
        m_stimPoints.resize(4);
    }

    Ray_AIGroundDarktoonedBehavior::~Ray_AIGroundDarktoonedBehavior()
    {
    }

    void Ray_AIGroundDarktoonedBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013),m_aiComponent);
    }

    void Ray_AIGroundDarktoonedBehavior::onActivate()
    {
        Super::onActivate();

        m_prevPos = AIUtils::getOffsetPosition(m_actor, m_actor->isFlipped(), getTemplate()->getStimEmissionOffset());
        m_prevPosEnd = AIUtils::getOffsetPosition(m_actor, m_actor->isFlipped(), getTemplate()->getStimEmissionOffsetEnd());
    }

    bbool Ray_AIGroundDarktoonedBehavior::updateMove( f32 _dt )
    {
        if ( !Super::updateMove(_dt) )
        {
            return bfalse;
        }


        Vec2d pos = AIUtils::getOffsetPosition(m_actor, m_actor->isFlipped(), getTemplate()->getStimEmissionOffset());
        Vec2d posEnd = AIUtils::getOffsetPosition(m_actor, m_actor->isFlipped(), getTemplate()->getStimEmissionOffsetEnd());

        f32 sqrSpeed = m_physComponent->getSpeed().sqrnorm();
        if ( sqrSpeed > (getTemplate()->getStimEmissionSpeedLimit() * getTemplate()->getStimEmissionSpeedLimit()) )
        {
            sendStim(pos, posEnd);
        }

        m_prevPos = pos;
        m_prevPosEnd = posEnd;


        return btrue;
    }

    void Ray_AIGroundDarktoonedBehavior::sendStim( const Vec2d& _pos, const Vec2d& _posEnd )
    {
        Vec2d movement = _pos - m_prevPos;
        Vec2d normal = (_posEnd - _pos).getPerpendicular();
        bbool forward = (movement.dot(normal) > 0);

        if (forward)
        {
            m_stimPoints[0] = _pos;
            m_stimPoints[1] = _posEnd;
            m_stimPoints[2] = m_prevPosEnd;
            m_stimPoints[3] = m_prevPos;
        }
        else
        {
            m_stimPoints[0] = m_prevPos;
            m_stimPoints[1] = m_prevPosEnd;
            m_stimPoints[2] = _posEnd;
            m_stimPoints[3] = _pos;
        }

        m_stimShape.setPoints(m_stimPoints);
        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>(&m_stimShape);

#if defined(ITF_FINAL)
        if(stim)
#endif // ITF_FINAL
        {
            stim->setPrevPos(Vec2d::Zero);
            stim->setPos(Vec2d::Zero);
            stim->setAngle(0);
            stim->setDepth(m_actor->getDepth());
            stim->setSender(m_actor->getRef());
            stim->setDirection(movement.normalize());
            stim->setLevel(getTemplate()->getStimLevel());
            stim->setReceivedHitType(getTemplate()->getStimType());
            stim->setFaction(m_aiComponent->getFaction());

            STIMSMANAGER->sendStim(stim);
        }
    }

    void Ray_AIGroundDarktoonedBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);
        if (EventHitSuccessful * onHit = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)))
        {
            performUTurnImmediate();
        }
    }

}