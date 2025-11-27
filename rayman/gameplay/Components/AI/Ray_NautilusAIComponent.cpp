#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_NAUTILUSAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_NautilusAIComponent.h"
#endif //_ITF_RAY_NAUTILUSAICOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_


#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifdef DEVELOPER_BEN
#define DEBUG_NAUTILUS
#endif // DEVELOPER_BEN

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_NautilusAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_NautilusAIComponent_Template)
        SERIALIZE_MEMBER("angularSpeedMultiplier", m_angularSpeedMultiplier);
        SERIALIZE_MEMBER("angularAcceleration", m_angularAcceleration);
        SERIALIZE_MEMBER("angularDeceleration", m_angularDeceleration);
        SERIALIZE_MEMBER("stopDelay", m_stopDelay);
        SERIALIZE_MEMBER("rollbackSpeed", m_rollbackSpeed);

        SERIALIZE_MEMBER("moveRadius", m_moveRadius);
        SERIALIZE_MEMBER("minAngle", m_minAngle);
        SERIALIZE_MEMBER("maxAngle", m_maxAngle);
        SERIALIZE_MEMBER("lockOnMinReached", m_lockOnMinReached);
        SERIALIZE_MEMBER("lockOnMaxReached", m_lockOnMaxReached);
        SERIALIZE_MEMBER("endBrakeAngle", m_endBrakeAngle);
        SERIALIZE_MEMBER("input", m_input);

        SERIALIZE_MEMBER("airControlMinAngularSpeed", m_airControlMinAngularSpeed);
        SERIALIZE_MEMBER("airControlMaxAngularSpeed", m_airControlMaxAngularSpeed);
        SERIALIZE_MEMBER("airControlMinAmount", m_airControlMinAmount);
        SERIALIZE_MEMBER("airControlMaxAmount", m_airControlMaxAmount);
        SERIALIZE_MEMBER("airControlMinDuration", m_airControlMinDuration);
        SERIALIZE_MEMBER("airControlMaxDuration", m_airControlMaxDuration);

        SERIALIZE_MEMBER("fx", m_fxName);
        SERIALIZE_MEMBER("fxStartSpeed", m_fxStartSpeed);
        SERIALIZE_MEMBER("fxStopSpeed", m_fxStopSpeed);
        SERIALIZE_MEMBER("fxInput", m_fxInputName);
        SERIALIZE_MEMBER("fxEndReached", m_fxEndReachedName);
        SERIALIZE_MEMBER("fxEndReachedStartAngle", m_fxEndReachedStartAngle);
        SERIALIZE_MEMBER("fxEndReachedStopAngle", m_fxEndReachedStopAngle);
        SERIALIZE_MEMBER("moveOnTopSpeedMultiplier", m_moveOnTopSpeedMultiplier);
    END_SERIALIZATION()

    void Ray_NautilusAIComponent_Template::onTemplateLoaded()
    {
        Super::onTemplateLoaded();

        if (m_rollbackBrakeAngle.ToRadians())
        {
            m_rollbackBrakeRatio = f32_Inv(m_rollbackBrakeAngle.ToRadians());
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_NautilusAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_NautilusAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("minAngle", m_minAngle);
            SERIALIZE_MEMBER("maxAngle", m_maxAngle);
            SERIALIZE_MODE("mode", m_mode);
            SERIALIZE_MEMBER("lockOnMinReached", m_lockOnMinReached);
            SERIALIZE_MEMBER("lockOnMaxReached", m_lockOnMaxReached);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_NautilusAIComponent)
    END_VALIDATE_COMPONENT()

    Ray_NautilusAIComponent::Ray_NautilusAIComponent()
        : m_linkComponent(NULL)
        , m_fxController(NULL)
        , m_soundComponent(NULL)
        , m_minAngle(bfalse, UNINIT_F32)
        , m_maxAngle(bfalse, UNINIT_F32)
        , m_mode(Mode_Fixed)
        , m_lockOnMinReached(UNINIT_BBOOL)
        , m_lockOnMaxReached(UNINIT_BBOOL)
        , m_angularSpeed(0.f)
        , m_stopTimer(0.f)
        , m_angleTraveled(0.f)
        , m_fxHandle(U32_INVALID)
        , m_fxEndReachedPlayed(bfalse)
    {
    }

    Ray_NautilusAIComponent::~Ray_NautilusAIComponent()
    {
    }


    void Ray_NautilusAIComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_fxController = m_actor->GetComponent<FXControllerComponent>();
        m_soundComponent = m_actor->GetComponent<SoundComponent>();
        if ( m_soundComponent && getTemplate()->getFxInputName().isValid() )
        {
            m_soundComponent->setUpdateSoundInput(this);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryDisableAirControlInfo,1557318448), this);


        if (m_minAngle.ToRadians() == UNINIT_F32)
        {
            m_minAngle = getTemplate()->getMinAngle();
        }
        if (m_maxAngle.ToRadians() == UNINIT_F32)
        {
            m_maxAngle = getTemplate()->getMaxAngle();
        }
        if (m_lockOnMinReached == UNINIT_BBOOL)
        {
            m_lockOnMinReached = getTemplate()->getLockOnMinReached();
        }
        if (m_lockOnMaxReached == UNINIT_BBOOL)
        {
            m_lockOnMaxReached = getTemplate()->getLockOnMaxReached();
        }
    }

    void Ray_NautilusAIComponent::onSceneActive()
    {
        Super::onSceneActive();

        init();
    }

    void Ray_NautilusAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        init();
    }

    void Ray_NautilusAIComponent::init()
    {
        updateTweenInput();

        initFx();
    }

    void Ray_NautilusAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventStickOnPolyline* onStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            processStick(onStick);
        }
        else if ( Ray_EventQueryDisableAirControlInfo* airControlQuery = _event->DynamicCast<Ray_EventQueryDisableAirControlInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryDisableAirControlInfo,1557318448)) )
        {
            processQueryDisableAirControl(airControlQuery);
        }
    }

    Vec2d Ray_NautilusAIComponent::getUserPos( const Actor* _user ) const
    {
        const Vec2d& userPos = _user->get2DPos();
        //const Vec2d& userPos = _user->getBase().truncateTo2D();
        //StickToPolylinePhysComponent* userPhys = _user->GetComponent<StickToPolylinePhysComponent>();
        //const Vec2d& userPos = userPhys->getStickedPosition();
        return userPos - m_actor->get2DPos();
    }

    f32 Ray_NautilusAIComponent::getUserAngle( const Actor* _user ) const
    {
        return getUserPos(_user).normalize().getAngle();
    }

    void Ray_NautilusAIComponent::processStick( EventStickOnPolyline* _onStick )
    {
        ActorRef userRef = _onStick->getActor();

        if (_onStick->isSticked())
        {
            // new user
            if ( Actor* userActor = userRef.getActor() )
            {
                m_users.push_back(User());
                User& user = m_users.back();
                user.m_actor = userActor->getRef();
                user.m_pos = getUserPos(userActor);
                user.m_angle = getUserAngle(userActor);
            }
        }
        else
        {
            // remove user
            for (u32 i = 0; i < m_users.size(); i++)
            {
                User& user = m_users[i];
                if (user.m_actor == userRef)
                {
                    m_users.eraseNoOrder(i);
                    break;
                }
            }
        }
    }

    void Ray_NautilusAIComponent::processQueryDisableAirControl( Ray_EventQueryDisableAirControlInfo* _query )
    {
        const f32 minSpeed = getTemplate()->getAirControlMinAngularSpeed();
        const f32 maxSpeed = getTemplate()->getAirControlMaxAngularSpeed();

        f32 angSpeed = fabsf(m_angularSpeed);
        if ( angSpeed >= minSpeed )
        {
            angSpeed = Clamp(angSpeed, minSpeed, maxSpeed);
            const f32 t = InverseInterpolate(minSpeed, maxSpeed, angSpeed);

            f32 airControlAmount = Interpolate(getTemplate()->getAirControlMinAmount(), getTemplate()->getAirControlMaxAmount(), t);
            f32 disableDuration = Interpolate(getTemplate()->getAirControlMinDuration(), getTemplate()->getAirControlMaxDuration(), t);

            _query->setDisable(btrue);
            _query->setIsBlend(btrue);
            _query->setAmount(airControlAmount);
            _query->setDuration(disableDuration);
            _query->setAllowHelico(btrue);

            //String s;
            //s.setTextFormat("duration %.2f", disableDuration);
            //DebugInfo::get().addDebugInfoDisplay(s, 5.f);
            //s.setTextFormat("amount %.2f", airControlAmount);
            //DebugInfo::get().addDebugInfoDisplay(s, 5.f);
        }
    }

    void Ray_NautilusAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        const f32 oneOverDt = f32_Inv(_dt);
        const bbool hasLimits = (m_minAngle != m_maxAngle);
        const bbool lockedMin = hasLimits && m_lockOnMinReached && m_angleTraveled < (m_minAngle.ToRadians() + getTemplate()->getFxEndReachedStartAngle());
        const bbool lockedMax = hasLimits && m_lockOnMaxReached && m_angleTraveled > (m_maxAngle.ToRadians() - getTemplate()->getFxEndReachedStartAngle());


        // get angle delta from the fastest user
        bbool hasValidUser = bfalse;
        f32 refDeltaAngle = 0.f;
        f32 topAngle = m_actor->getWorldInitialRot() + MTH_PIBY2;
        EventQueryPhysicsData queryPhys;

        for (u32 i = 0; i < m_users.size(); i++)
        {
            User& user = m_users[i];
            Actor* userActor = user.m_actor.getActor();
            if (!userActor)
            {
                continue;
            }

            hasValidUser = btrue;

            f32 userAngle = getUserAngle(userActor);
            f32 deltaAngle = user.m_angle - userAngle;

            if (f32_Abs(deltaAngle) > f32_Abs(refDeltaAngle))
            {
                refDeltaAngle = deltaAngle;
            }

            if ( f32_Abs(m_angularSpeed) > 0.3f )
            {
                userActor->onEvent(&queryPhys);

                f32 difAngle = getShortestAngleDelta(topAngle,userAngle);
                f32 distance = queryPhys.getSpeed().norm()*getTemplate()->getMoveOnTopSpeedMultiplier()*_dt;

                if ( difAngle > 0.01f )
                {
                    Ray_MoveOnPolylineFromNautilus moveEvent(distance);
                    userActor->onEvent(&moveEvent);
                }
                else if ( difAngle < -0.01f )
                {
                    Ray_MoveOnPolylineFromNautilus moveEvent(-distance);
                    userActor->onEvent(&moveEvent);
                }
            }

            userAngle = getUserAngle(userActor);

            user.m_angle = userAngle;
        }

        if (fabs(refDeltaAngle) < MTH_EPSILON)
        {
            refDeltaAngle = 0;
        }


        // either try to match the fastest user, keep going, or stop
        f32 targetAngularSpeed;
        if (hasValidUser)
        {
            m_stopTimer = 0;
            if ( (!lockedMin || refDeltaAngle < 0) &&
                 (!lockedMax || refDeltaAngle > 0) )
            {
                targetAngularSpeed = refDeltaAngle * oneOverDt * getTemplate()->getAngularSpeedMultiplier();
            }
            else
            {
                // lock was reached and user is trying to go back: ignore it
                targetAngularSpeed = 0.f;
            }
        }
        else
        {
            m_stopTimer += _dt;
            if (m_stopTimer < getTemplate()->getStopDelay())
            {
                // keep going
                targetAngularSpeed = m_angularSpeed;
            }
            else if (!lockedMin && !lockedMax)
            {
                // stop / rollback
                const f32 ratio = getTemplate()->getRollbackBrakeRatio() ? Min(f32_Abs(m_angleTraveled) * getTemplate()->getRollbackBrakeRatio(), 1.f) : 1.f;
                targetAngularSpeed = getTemplate()->getRollbackSpeed() * ratio * -getSign(m_angleTraveled);

                //DebugDraw::text("rollback ratio : %.f%%", ratio * 100);
                //DebugDraw::text("rollback speed : %.f", targetAngularSpeed * MTH_RADTODEG);
            }
            else
            {
                // locked
                targetAngularSpeed = 0.f;
            }
        }

        if (hasLimits && targetAngularSpeed != 0)
        {
            // ramp down target speed as we approach the limits

            f32 rest;
            if (targetAngularSpeed > 0)
            {
                rest = m_maxAngle.ToRadians() - m_angleTraveled;
            }
            else
            {
                rest = m_angleTraveled - m_minAngle.ToRadians();
            }

            if (rest < getTemplate()->getEndBrakeAngle())
            {
                const f32 ratio = rest / getTemplate()->getEndBrakeAngle();
                targetAngularSpeed *= ratio;
            }

            //DebugDraw::text("rest %.2f° / %.f%%", rest * MTH_RADTODEG, rest / getTemplate()->getEndBrakeAngle() * 100);
        }


        // acc/decelerate to match the target speed
        if (targetAngularSpeed == 0.f)
        {
            // decelerate
            f32 speedDelta = getTemplate()->getAngularDeceleration() * _dt;
            if (m_angularSpeed > 0)
            {
                m_angularSpeed = Max(m_angularSpeed - speedDelta, 0.f);
            }
            else
            {
                m_angularSpeed = Min(m_angularSpeed + speedDelta, 0.f);
            }
        }
        else
        {
            // accelerate
            f32 speedDelta = getTemplate()->getAngularAcceleration() * _dt;
            if (targetAngularSpeed > 0)
            {
                if (targetAngularSpeed < m_angularSpeed)
                {
                    m_angularSpeed = Max(m_angularSpeed - speedDelta, targetAngularSpeed);
                }
                else
                {
                    m_angularSpeed = Min(m_angularSpeed + speedDelta, targetAngularSpeed);
                }
            }
            else
            {
                if (targetAngularSpeed < m_angularSpeed)
                {
                    m_angularSpeed = Max(m_angularSpeed - speedDelta, targetAngularSpeed);
                }
                else
                {
                    m_angularSpeed = Min(m_angularSpeed + speedDelta, targetAngularSpeed);
                }
            }
        }


        // this is how much we want to rotate this frame
        f32 deltaAngle = m_angularSpeed * _dt;
        //f32 deltaAngle = refDeltaAngle;   // compensate directly -forget all accelerations

        if (hasLimits)
        {
            // clamp angle if limits are reached
            f32 prevAngleTraveled = m_angleTraveled;
            m_angleTraveled = Clamp(m_angleTraveled + deltaAngle, m_minAngle.ToRadians(), m_maxAngle.ToRadians());
            deltaAngle = m_angleTraveled - prevAngleTraveled;
        }
        else
        {
            m_angleTraveled += deltaAngle;
        }


        if (deltaAngle)
        {
            // rotate
            m_actor->setBoundLocalAngle(m_actor->getBoundLocalAngle() + deltaAngle);
            m_angularSpeed = deltaAngle * oneOverDt;

            // offset users
            for (u32 i = 0; i < m_users.size(); i++)
            {
                User& user = m_users[i];
                user.m_angle += deltaAngle;
            }

            move(_dt, deltaAngle);

            updateTweenInput();

            updateFx();
        }
        else
        {
            m_angularSpeed = 0.f;
        }
    }

    void Ray_NautilusAIComponent::move( const f32 _dt, const f32 _deltaAngle )
    {
        if (m_mode == Mode_Fixed)
        {
            return;
        }

        const f32 radius = getTemplate()->getMoveRadius();
        if (!radius)
        {
            return;
        }

        const Vec2d groundDir = Vec2d::Right;
        const Vec2d deltaPos = groundDir * radius * -_deltaAngle;
        m_actor->setBoundLocal2DPos(m_actor->getBoundLocal2DPos() + deltaPos);
    }

    // angle input (to control tween/anim) - [0,1] cursor if we're limited, absolute m_angleTraveled otherwise
    void Ray_NautilusAIComponent::updateTweenInput()
    {
        f32 cursor = m_angleTraveled;
        if (m_minAngle.ToRadians() != m_maxAngle.ToRadians())
        {
            cursor = InverseInterpolate(m_minAngle.ToRadians(), m_maxAngle.ToRadians(), cursor);
            cursor = Clamp(cursor, 0.f, 1.f);
        }

        EventSetFloatInput eventInput;
        eventInput.setInputName(getTemplate()->getInput());
        eventInput.setInputValue(cursor);

        m_actor->onEvent(&eventInput);

        if (m_linkComponent)
        {
            m_linkComponent->sendEventToChildren(&eventInput);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_NautilusAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (_flags != ActorComponent::DrawEditFlag_All)
        {
            return;
        }

        //bbool stopping = (targetAngularSpeed == 0.f);
        //Color color = stopping ? Color::red() : Color::white();
        Color color = Color::white();

        f32 cursor = m_angleTraveled;
        if (m_minAngle.ToRadians() != m_maxAngle.ToRadians())
        {
            cursor = InverseInterpolate(m_minAngle.ToRadians(), m_maxAngle.ToRadians(), cursor);
            cursor = Clamp(cursor, 0.f, 1.f);
        }
        DebugDraw::text(color, "cursor : %.f%%", cursor * 100);
        DebugDraw::text(color, "total angle : %.f", m_angleTraveled * MTH_RADTODEG);
        DebugDraw::text(color, "angularSpeed : %.f", m_angularSpeed * MTH_RADTODEG);
        DebugDraw::text(color, "users : %d", m_users.size());
    }

#endif // ITF_SUPPORT_EDITOR


    void Ray_NautilusAIComponent::initFx()
    {
        // this avoids a clonk when the nautilus is parked at an end and the user steps on it

        const f32 restMin = m_maxAngle.ToRadians() - m_angleTraveled;
        const f32 restMax = m_angleTraveled - m_minAngle.ToRadians();
        const f32 rest = Min(restMin, restMax);

        if (rest < getTemplate()->getFxEndReachedStartAngle())
        {
            m_fxEndReachedPlayed = btrue;
        }
    }

    void Ray_NautilusAIComponent::updateFx()
    {
        if (!m_fxController)
        {
            return;
        }


        if (getTemplate()->getFxName().isValid())
        {
            // start/stop an FX depending on the speed

            if (m_fxHandle == U32_INVALID)
            {
                if (fabs(m_angularSpeed) > getTemplate()->getFxStartSpeed())
                {
                    m_fxHandle = m_fxController->playFX(getTemplate()->getFxName());
                }
            }
            else
            {
                if (fabs(m_angularSpeed) < getTemplate()->getFxStopSpeed())
                {
                    m_fxController->stopFXFromHandle(m_fxHandle);
                    m_fxHandle = U32_INVALID;
                }
            }
        }


        if (getTemplate()->getFxEndReachedName().isValid())
        {
            // play an fx when the end is reached

            const f32 restMin = m_maxAngle.ToRadians() - m_angleTraveled;
            const f32 restMax = m_angleTraveled - m_minAngle.ToRadians();
            const f32 rest = Min(restMin, restMax);

            if (!m_fxEndReachedPlayed)
            {
                if (rest < getTemplate()->getFxEndReachedStartAngle())
                {
                    m_fxEndReachedPlayed = btrue;
                    m_fxController->playFX(getTemplate()->getFxEndReachedName());
                }
            }
            else
            {
                if (rest > getTemplate()->getFxEndReachedStopAngle())
                {
                    m_fxEndReachedPlayed = bfalse;
                }
            }
        }
    }

    // angular speed controls the pitch via an input
    void Ray_NautilusAIComponent::updateSoundInput()
    {
        m_soundComponent->setInput(getTemplate()->getFxInputName(), m_angularSpeed);
    }

};
