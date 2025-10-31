#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIORANGEFLOATINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIOrangeFloatingBehavior.h"
#endif //_ITF_RAY_AIORANGEFLOATINGBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIOrangeFloatingBehavior)
    BEGIN_VALIDATE_BEHAVIOR(Ray_AIOrangeFloatingBehavior)
    END_VALIDATE_BEHAVIOR()

    const f32 Ray_AIOrangeFloatingBehavior::TOGGLE_DIRECTION_DELAY = 0.25f;

    //------------------------------------------------------------------
    Ray_AIOrangeFloatingBehavior::Ray_AIOrangeFloatingBehavior()
    : m_initialX(0.0f)
    , m_signedWantedSpeed(0.0f)
    , m_previousDistance(0.0f)
    , m_toggleDirectionTimer(0.0f)
    , m_orangeState(OrangeState_UNDEFINED)
    {
    }

    //------------------------------------------------------------------
    Ray_AIOrangeFloatingBehavior::~Ray_AIOrangeFloatingBehavior()
    {
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventOrangeContact,4040257774), m_aiComponent);
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::onActivate()
    {
        Super::onActivate();
        changeState(OrangeState_IDLE);
        m_initialX = m_actor->get2DPos().m_x;
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::update(f32 _dt)
    {
        Super::update(_dt);

        updateState(m_orangeState, _dt);
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::changeState(const OrangeState _state, const Vec2d& _param)
    {
        const OrangeState previousState = m_orangeState;
        m_orangeState = _state;

        switch( m_orangeState )
        {
        case OrangeState_MOVING:
            startMoving(_param, previousState);
            break;
        default:
            break;
        }
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::updateState(const OrangeState _state, const f32 _dt)
    {
        switch( _state )
        {
        case OrangeState_IDLE:
            updateIdle();
            break;
        case OrangeState_MOVING:
            updateMoving(_dt);
            break;
        case OrangeState_ARRIVAL:
            updateArrival(_dt);
            break;
        default:
            break;
        }
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::updateIdle()
    {
        const Vec2d & orangePos = m_actor->get2DPos();
        const f32 diffX = m_initialX - orangePos.m_x;
        if( f32_Abs(diffX) > getTemplate()->getMoveThreshold() )
        {
            changeState(OrangeState_MOVING, Vec2d(diffX, 0.0f));
        }
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::updateMoving(const f32 _dt)
    {
        //check back to initial position
        const Vec2d & pos = m_actor->get2DPos();
        const f32 distance = m_initialX - pos.m_x;
        m_previousDistance = distance;
        if( distance * m_signedWantedSpeed > 0.0f )
        {
            if( f32_Abs(distance) < getTemplate()->getArrivalDistance() )
            {
                changeState(OrangeState_ARRIVAL);
                return;
            }
        }

        //get phys speed
        const Vec2d & speed = m_physComponent->getSpeed();
        if( speed.m_x < m_signedWantedSpeed )
        {
            m_physComponent->addForce( Vec2d::XAxis * 10.0f );
        }
        else
        {
            m_physComponent->addForce( Vec2d::XAxis * -10.0f );
        }

        m_toggleDirectionTimer += _dt;
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::startMoving(const Vec2d & _impulsion, const OrangeState _previousState)
    {
        computeWantedSpeedAndArrivalFactor(_impulsion.m_x);
        m_toggleDirectionTimer = 0.0f;
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::toggleMoveDirection()
    {
        m_signedWantedSpeed *= -1.0f;
        m_toggleDirectionTimer = 0.0f;
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::updateArrival(const f32 _dt)
    {
        const Vec2d & position = m_actor->get2DPos();
        const f32 distance = m_initialX - position.m_x;
        if( distance * m_previousDistance > 0.0f )
        {
            const f32 distanceAbs = f32_Abs(distance);
            const Vec2d & speed = m_physComponent->getSpeed();
            const f32 ratio = distanceAbs / getTemplate()->getArrivalDistance();
            const f32 wantedSpeed = ratio * m_signedWantedSpeed;
            if( speed.m_x < wantedSpeed )
            {
                m_physComponent->addForce( Vec2d::XAxis * 10.0f );
            }
            else
            {
                m_physComponent->addForce( Vec2d::XAxis * -10.0f );
            }
            m_previousDistance = distance;
        }
        else
        {
            changeState(OrangeState_IDLE);
        }
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if( EventBlockedByPolyline * blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
        {
            processBlockedEvent(blockedEvent);
        }
        else if( EventOrangeContact * orangeContactEvent = _event->DynamicCast<EventOrangeContact>(ITF_GET_STRINGID_CRC(EventOrangeContact,4040257774)) )
        {
            processOrangeContactEvent(orangeContactEvent);
        }
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::processStickOnPolyline(EventStickOnPolyline * _event)
    {
        Super::processStickOnPolyline(_event);

        if( (m_orangeState != OrangeState_MOVING) && _event->isSticked() )
        {
            changeState(OrangeState_MOVING, _event->getSpeed());
        }
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::processBlockedEvent(const EventBlockedByPolyline * const _event)
    {
        if( isAbleToToggleDirection() )
        {
            Vec2d contactDir = m_actor->get2DPos() - _event->getContact();
            if( contactDir.dot( Vec2d::XAxis * m_signedWantedSpeed ) < 0.0f )
            {
                toggleMoveDirection();
            }

            BaseObject* obj = GETOBJECT(_event->getPolyline());
            if ( obj )
            {
                const PolyLine* myPoly = static_cast<PolyLine*>(obj);
                Actor * owner = myPoly->getOwnerActor();
                if( owner && owner != m_actor )
                {
                    EventOrangeContact contactEvent;
                    contactEvent.setSpeed( _event->getSpeed() );
                    owner->onEvent( &contactEvent );
                }
            }
        }
    }
        
    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::processOrangeContactEvent(const EventOrangeContact * const _event)
    {
        if (m_orangeState != OrangeState_MOVING)
        {
            changeState(OrangeState_MOVING, _event->getSpeed());
        }
    }

    //------------------------------------------------------------------
    void Ray_AIOrangeFloatingBehavior::computeWantedSpeedAndArrivalFactor(const f32 _x)
    {
        //compute speed value
        const f32 absX = f32_Abs(_x);
        if( absX <= getTemplate()->getLandingSpeedXThreshold() )
        {
            m_signedWantedSpeed = getTemplate()->getOrangeMinSpeed();
        }
        else
        {
            m_signedWantedSpeed = getTemplate()->getOrangeMaxSpeed();
        }

        //compute speed sign
        const f32 sign = (_x < 0.0f) ? -1.0f : 1.0f;
        m_signedWantedSpeed *= sign;
    }




    //**************************************************************************************
    //  Template
    //**************************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_AIOrangeFloatingBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIOrangeFloatingBehavior_Template)
        SERIALIZE_MEMBER("orangeMinSpeed", m_orangeMinSpeed);
        SERIALIZE_MEMBER("orangeMaxSpeed", m_orangeMaxSpeed);
        SERIALIZE_MEMBER("landingSpeedXThreshold", m_landingSpeedXThreshold);
        SERIALIZE_MEMBER("arrivalDistance", m_arrivalDistance);
        SERIALIZE_MEMBER("moveThreshold", m_moveThreshold);
    END_SERIALIZATION()

    //------------------------------------------------------------------
    Ray_AIOrangeFloatingBehavior_Template::Ray_AIOrangeFloatingBehavior_Template()
    : m_orangeMinSpeed(6.0f)
    , m_orangeMaxSpeed(10.0f)
    , m_landingSpeedXThreshold(5.0f)
    , m_arrivalDistance(1.0f)
    , m_moveThreshold(0.75f)
    {
    }

    //------------------------------------------------------------------
    Ray_AIOrangeFloatingBehavior_Template::~Ray_AIOrangeFloatingBehavior_Template()
    {
    }

}