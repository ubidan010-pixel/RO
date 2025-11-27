

#include "precompiled_gameplay.h"

#ifndef _ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_
#include "gameplay/components/misc/EventReceiveAnimPlayComponent.h"
#endif //_ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(EventReceivePlayAnimComponent)

BEGIN_SERIALIZATION_CHILD(EventReceivePlayAnimComponent)

    SERIALIZE_MEMBER("startValue",m_startValue );

    BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
#ifdef ITF_SUPPORT_DEBUGFEATURE
        SERIALIZE_MEMBER("displayPhantom",m_displayPhantom);
#endif // ITF_SUPPORT_DEBUGFEATURE
    END_CONDITION_BLOCK()

END_SERIALIZATION()

static StringID s_SndSpeedInput = "Speed";
static StringID s_StartInput = "Start";

EventReceivePlayAnimComponent::EventReceivePlayAnimComponent()
: m_animComponent(NULL)
, m_phantom(NULL)
, m_soundComponent( NULL )
, m_fxControllerComponent( NULL )
, m_resetTimeLength(0.f)
, m_resetTimeStartPos(0.f)
, m_resetTimeCounter(0.f)
, m_currentSpeed(0.f)
, m_currentPos(0.f)
, m_resetDelayCounter(0.f)
, m_eventsReceived(0)
, m_registered(bfalse)
, m_locked(bfalse)
#ifdef ITF_SUPPORT_DEBUGFEATURE
, m_displayPhantom(bfalse)
#endif // ITF_SUPPORT_DEBUGFEATURE
, m_fxHandle( U32_INVALID )
, m_startValue( 0.0f )
{
}

EventReceivePlayAnimComponent::~EventReceivePlayAnimComponent()
{
    clear();
}

void EventReceivePlayAnimComponent::clear()
{
    if ( m_phantom )
    {
        if ( m_registered )
        {
            PHYSWORLD->removePhantom(m_phantom);
            m_registered = bfalse;
        }

        PHYSWORLD->deallocPhantom(m_phantom);
    }

}

void EventReceivePlayAnimComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    if (  getTemplate()->getPhantomShape() )
    {
        if ( !m_phantom )
        {
            PhysPhantomInitCommonInfo initInfo;

            SET_OWNER(initInfo, m_actor->getRef());
            initInfo.m_shape =  getTemplate()->getPhantomShape();
            initInfo.m_pos = m_actor->get2DPos();
            initInfo.m_pos += getTemplate()->getPhantomOffset();
            initInfo.m_angle = m_actor->getAngle();
            initInfo.m_userData = m_actor->getRef();
            initInfo.m_collisionGroup = ECOLLISIONGROUP_CHARACTER;

            m_phantom = PHYSWORLD->allocPhantom(initInfo);
        }
    }

    if ( getTemplate()->getEventWatch() )
    {
        m_actor->registerEvent(getTemplate()->getEventWatch()->GetObjectClassCRC(),this);
    }
    if ( getTemplate()->getResetStartValueEvent() )
    {
        m_actor->registerEvent(getTemplate()->getResetStartValueEvent()->GetObjectClassCRC(),this);
    }

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setIsProcedural(btrue);
        m_animComponent->setUpdateAnimTime(bfalse);
    }

    m_soundComponent = m_actor->GetComponent<SoundComponent>();
    m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
}

void EventReceivePlayAnimComponent::onBecomeActive()
{
    Super::onBecomeActive();

    if ( m_phantom && !m_registered )
    {
        PHYSWORLD->insertPhantom(m_phantom,m_actor->getDepth());
        m_registered = btrue;
    }

    m_currentPos = m_startValue;
}

void EventReceivePlayAnimComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    if ( m_registered )
    {
        PHYSWORLD->removePhantom(m_phantom);
        m_registered = bfalse;
    }
}

void EventReceivePlayAnimComponent::Update( f32 _dt )
{
    if ( m_phantom )
    {
        Vec2d posOffset = getTemplate()->getPhantomOffset();
        if ( m_actor->isFlipped() )
        {
            posOffset.m_x = -posOffset.m_x;
        }

        posOffset = posOffset.Rotate( m_actor->getAngle() );
        Vec3d phantomPos = ( m_actor->get2DPos() + posOffset ).to3d(m_actor->getPos().m_z );
        m_phantom->setPos3D(phantomPos);
        m_phantom->setAngle(m_actor->getAngle());
        m_phantom->updateAABB();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        if ( m_displayPhantom )
        {
            DebugDraw::shape(phantomPos.truncateTo2D(), m_actor->getPos().m_z,m_phantom->getAngle(),m_phantom->getShape(),Color::white(),1.f);
        }
#endif // ITF_SUPPORT_DEBUGFEATURE
    }

    if ( !m_locked )
    {
        updateSpeed(_dt);
        updatePos(_dt);
    }

    if ( m_animComponent && m_animComponent->isLoaded() )
    {
        m_animComponent->setProceduralCursor(m_currentPos);
        m_animComponent->updateAnimTime(_dt);
        m_animComponent->forceSynchronousFrame();
    }

    if ( m_soundComponent != NULL )
    {
        f32 speedAbs = m_currentSpeed;
        if ( ( m_currentSpeed >= 0.f ) && ( getTemplate()->getMaxSpeed() != 0.f ) )
        {
            speedAbs = Min( speedAbs / f32_Abs( getTemplate()->getMaxSpeed() ), 1.0f );
        }
        else if ( ( m_currentSpeed < 0.f ) && ( getTemplate()->getMinSpeed() != 0.f ) )
        {
            speedAbs = -Min( f32_Abs( speedAbs ) / f32_Abs( getTemplate()->getMinSpeed() ), 1.0f );
        }
        
        m_soundComponent->setInput( s_SndSpeedInput, speedAbs );
        //LOG( "%f", speedAbs );
    }
}

void EventReceivePlayAnimComponent::updateSpeed( f32 _dt )
{
    if ( m_eventsReceived )
    {
        m_currentSpeed = Min(m_currentSpeed+(getTemplate()->getAcceleration()*static_cast<f32>(m_eventsReceived)),getTemplate()->getMaxSpeed());

        if ( !getTemplate()->getUseSingleEvent() )
        {
            m_eventsReceived = 0;
        }

        m_resetTimeCounter = 0.f;
        m_resetTimeCounter = 0.f;
    }
    else
    {
        bbool wasGrowing = m_currentSpeed > 0.f;

        if ( !m_resetDelayCounter && !m_resetTimeCounter )
        {
            m_currentSpeed = Max(m_currentSpeed-(getTemplate()->getDeceleration()*_dt),getTemplate()->getMinSpeed());
        }

        bbool isGrowing = m_currentSpeed > 0.f;

        if ( wasGrowing && !isGrowing )
        {
            if ( getTemplate()->getResetDelay() )
            {
                m_resetDelayCounter = getTemplate()->getResetDelay();
                m_currentSpeed = 0.f;
            }
            else
            {
                m_resetDelayCounter = 0.f;
                setResetTime();
            }
        }
        else
        {
            if ( m_resetDelayCounter )
            {
                m_resetDelayCounter = Max(m_resetDelayCounter-_dt,0.f);

                if ( !m_resetDelayCounter )
                {
                    setResetTime();
                }
            }
        }
    }
}

void EventReceivePlayAnimComponent::setResetTime()
{
    if ( getTemplate()->getResetTimeMin() && getTemplate()->getResetTimeMax() > getTemplate()->getResetTimeMin() )
    {
        m_resetTimeLength = ( (getTemplate()->getResetTimeMax() - getTemplate()->getResetTimeMin() ) * m_currentPos ) +getTemplate()->getResetTimeMin();
        m_resetTimeCounter = 0.f;
        m_resetTimeStartPos = m_currentPos;
    }
    else
    {
        m_resetTimeLength = 0.f;
        m_resetTimeCounter = 0.f;
        m_resetTimeStartPos = 0.f;
    }
}

void EventReceivePlayAnimComponent::updatePos( f32 _dt )
{
    f32 previousPos = m_currentPos;
    f32 previousSpeed = m_currentSpeed;

    if ( m_resetTimeLength )
    {
        m_resetTimeCounter = Min(m_resetTimeCounter+_dt,m_resetTimeLength);

        if ( m_resetTimeCounter == m_resetTimeLength )
        {
            m_resetTimeCounter = 0.f;
            m_resetTimeLength = 0.f;
            m_currentPos = m_startValue;
        }
        else
        {
            f32 t = 1.f - ( m_resetTimeCounter / m_resetTimeLength );

            m_currentPos = m_resetTimeStartPos * t;
        }
    }
    else
    {
        m_currentPos = Clamp(m_currentPos+(m_currentSpeed*_dt),m_startValue,1.f);

        if ( m_currentPos == m_startValue )
        {
            m_currentSpeed = Max(m_currentSpeed,0.f);
        }
        else if ( m_currentPos == 1.f )
        {
            m_currentSpeed = Min(m_currentSpeed,0.f);
        }
    }

    if ( getTemplate()->getLockOnEnd() )
    {
        m_locked = m_currentPos == 1.f;
    }

    if ( m_currentPos && m_eventsReceived && getTemplate()->getUseSingleEvent() )
    {
        m_eventsReceived = 0;
    }    

    // bounce
    //
    if ( previousPos != m_currentPos )
    {
        if ( ( m_currentPos == m_startValue ) || ( m_currentPos == 1.0f ) )
        {
            m_currentSpeed = -previousSpeed * getTemplate()->getBounceFactor();
        }
    }

    // play FX inputs
    //
    if ( m_fxControllerComponent != NULL )
    {
        if ( ( m_currentPos == m_startValue ) && ( m_currentSpeed <= 0.f ) && ( m_fxHandle != U32_INVALID ) )
        {
            m_fxControllerComponent->stopFXFromHandle( m_fxHandle );
            m_fxHandle = U32_INVALID;
        }
        else if ( ( previousPos == 0.0f ) && ( m_currentPos > m_startValue ) && ( f32_Abs( m_currentSpeed ) > 0.0f ) )
        {
            if ( m_fxHandle == U32_INVALID )
            {
                m_fxHandle = m_fxControllerComponent->playFX( s_StartInput );
                m_fxControllerComponent->setFXPosFromHandle( m_fxHandle, m_actor->getPos() );
            }
        }
    }   
   
}

void EventReceivePlayAnimComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( getTemplate()->getEventWatch() &&
         _event->IsClassCRC( getTemplate()->getEventWatch()->GetObjectClassCRC() ) )
    {
        EventStim* stim = _event->DynamicCast<EventStim>(ITF_GET_STRINGID_CRC(EventStim,316939190));

        if ( !stim || ( m_phantom && stim->getPhantomRef() == m_phantom->getRef() ) )
        {
            if ( getTemplate()->getUseSingleEvent() )
            {
                m_eventsReceived = 1;
            }
            else
            {
                m_eventsReceived++;
            }
        }
    }
    
    if ( getTemplate()->getResetStartValueEvent() &&
        _event->IsClassCRC( getTemplate()->getResetStartValueEvent()->GetObjectClassCRC() ) )
    {
        m_startValue = 0.0f;        
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(EventReceivePlayAnimComponent_Template)
BEGIN_SERIALIZATION(EventReceivePlayAnimComponent_Template)

    SERIALIZE_OBJECT_WITH_FACTORY("phantomShape",m_phantomShape,PHYSWORLD->getShapesFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("watchEvent",m_eventWatch,ACTORSMANAGER->getEventFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("resetStartValueEvent",m_resetStartValueEvent,ACTORSMANAGER->getEventFactory());
    SERIALIZE_MEMBER("phantomOffset",m_phantomOffset);
    SERIALIZE_MEMBER("speedIncrease",m_acceleration);
    SERIALIZE_MEMBER("decrease",m_deceleration);
    SERIALIZE_MEMBER("maxSpeed",m_maxSpeed);
    SERIALIZE_MEMBER("minSpeed",m_minSpeed);
    SERIALIZE_MEMBER("resetTimeMin",m_resetTimeMin);
    SERIALIZE_MEMBER("resetTimeMax",m_resetTimeMax);
    SERIALIZE_MEMBER("resetDelay",m_resetDelay);
    SERIALIZE_MEMBER("lockOnEnd",m_lockOnEnd);
    SERIALIZE_MEMBER("useSingleEvent",m_useSingleEvent);
    SERIALIZE_MEMBER("bounceFactor",m_bounceFactor);

END_SERIALIZATION()

EventReceivePlayAnimComponent_Template::EventReceivePlayAnimComponent_Template()
: m_phantomShape(NULL)
, m_eventWatch(NULL)
, m_resetStartValueEvent(NULL)
, m_acceleration(0.1f)
, m_deceleration(0.2f)
, m_maxSpeed(1.f)
, m_minSpeed(-3.f)
, m_resetTimeMin(0.f)
, m_resetTimeMax(0.f)
, m_resetDelay(0.f)
, m_lockOnEnd(bfalse)
, m_useSingleEvent(bfalse)
, m_bounceFactor( 0.0f )
{
}


EventReceivePlayAnimComponent_Template::~EventReceivePlayAnimComponent_Template() 
{
    SF_DEL(m_phantomShape);
    SF_DEL(m_eventWatch);
    SF_DEL(m_resetStartValueEvent);
}

}


