#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PUSHBUTTONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_PushButtonComponent.h"
#endif //_ITF_RAY_PUSHBUTTONCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PushButtonComponent)
BEGIN_SERIALIZATION_CHILD(Ray_PushButtonComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("activator", m_activator);
        SERIALIZE_ENUM_BEGIN("triggerCount", m_triggerCount);
        SERIALIZE_ENUM_END();
    END_CONDITION_BLOCK()
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_PushButtonComponent)
    VALIDATE_COMPONENT_PARAM("onOffDuration", getTemplate()->getOnOffDuration() > 0.0f, "Positive duration required");
    VALIDATE_COMPONENT_PARAM("triggerOnStick,triggerOnHit", getTemplate()->getTriggerOnStick() || getTemplate()->getTriggerOnHit(), "One of both mandatory");
    VALIDATE_COMPONENT_PARAM("", m_linkComponent, "LinkComponent mandatory");
    VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
    VALIDATE_COMPONENT_PARAM("", ( getTemplate()->getIsProgressive() && getTemplate()->getTriggerOnHit() ) || !getTemplate()->getIsProgressive(), "progressive mode must be used with on hit flag");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_PushButtonComponent::Ray_PushButtonComponent()
    : Super()
    , m_state(State_Off)
    , m_onOffCursor(0.0f)
    , m_countdown(0.0f)
    , m_stickedActorsCount(0)
    , m_wasHit(bfalse)
    , m_activator()
    , m_eventTrigger()
    , m_eventActivateHandler()
    , m_triggerCount(0)
    , m_linkComponent(NULL)
    , m_animatedComponent(NULL)
    , m_progressiveCursor(0)
{
}

//------------------------------------------------------------------------------
Ray_PushButtonComponent::~Ray_PushButtonComponent()
{
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    // mandatory & validated, we want to send event to our children
    m_linkComponent = m_actor->GetComponent<LinkComponent>();

    // setup event trigger: will switch on an off actors, if supported,
    // otherwise will just activate them
    m_eventTrigger.setSender(m_actor->getRef());
    m_eventTrigger.setToggle(btrue);
    m_eventTrigger.setActivated(btrue);

    // mandatory & validated, we want to detect weight changes
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }
    
    // register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventOnLink,1558255516), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153),this);
    
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::onSceneActive()
{
    Super::onSceneActive();

    /* We need to deactivate children only once, but cannot do it onActorLoaded
       because the linked children may not be ready (EventActivate not registered).
       On normal scene load, we do it onSceneActive (everybody's loaded).
       On F5, we do it onCheckpointLoaded.
    */
    if (getTemplate()->getActivateChildren())
    {
        m_eventActivateHandler.deactivateChildren(m_linkComponent);
    }
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    if (getTemplate()->getActivateChildren())
    {
        m_eventActivateHandler.deactivateChildren(m_linkComponent);
    }

    // push buttons are use to toggle stuff, this is an attempt to always reset
    // this stuff to a proper state after a checkpoint reload (RO-11202)
    // NB: don't count this
    // NB2: reuse last activator for all triggers
    for (u32 i=0; i<m_triggerCount; ++i)
    {
        trigger(bfalse);
    }
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::Update(f32 _dt)
{
    Super::Update(_dt);
    
    // update state
    switch (m_state)
    {
    case State_Off:
    {
        ITF_ASSERT(m_onOffCursor == 0.0f);

        // progressive
        if ( getTemplate()->getIsProgressive() )
        {
            m_state = State_ProgressiveToOn;
        }
        // start switching on
        else if (getAndResetTriggered())
        {
            m_state = State_OffToOn;
            trigger();
        }
    }
    break;
    case State_OffToOn:
    {
        // stop when completely on
        m_onOffCursor += _dt / getTemplate()->getOnOffDuration();
        m_onOffCursor = Min(m_onOffCursor, 1.0f);
        if (m_onOffCursor == 1.0f)
        {
            m_state = State_On;
            m_countdown = getTemplate()->getDelay();
        }
    }
    break;
    case State_On:
    {
        ITF_ASSERT(m_onOffCursor == 1.0f);

        if ( !getTemplate()->getStayOn() )
        {
            // reset countdown if triggered again
            if (getAndResetTriggered())
            {
                m_countdown = getTemplate()->getDelay();
            }
            // start switching off if not triggered, and when delay has passed
            else
            {
                m_countdown = Max(m_countdown - _dt, 0.0f);
                if (m_countdown == 0.0f)
                {
                    m_state = State_OnToOff;
                }
            }
        }
    }
    break;
    case State_OnToOff:
    {
        // interrupt: start switching on (RO-5981)
        if (getAndResetTriggered())
        {
            m_state = State_OffToOn;
            trigger();
        }
        // stop when completely off
        else
        {
            m_onOffCursor -= _dt / getTemplate()->getOnOffDuration();
            m_onOffCursor = Max(m_onOffCursor, 0.0f);
            if (m_onOffCursor == 0.0f)
            {
                m_state = State_Off;
            }
        }
    }
    break;
    case State_ProgressiveToOn:
    {
        f32 curCursor = f32( m_progressiveCursor ) / 100.0f;
        m_onOffCursor += _dt * getTemplate()->getProgressiveSpeed();
        m_onOffCursor = Min(m_onOffCursor, curCursor);

        if (m_onOffCursor == 1.0f)
        {
            m_state = State_On;
        }
    }
    break;
    }    
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);
    
    // this an "editor" event, that's why it should remain processed
    // *before* any early return
    if (getTemplate()->getActivateChildren())
    {
        m_eventActivateHandler.onEvent(_event);
    }

    // process events
    if (EventStickOnPolyline* stick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
    {
        processStick(stick);
    }
    else if (HitStim* hitStim = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
    {
        processHit(hitStim);
    }
    else if ( Ray_EventQueryOpen * queryOpen = _event->DynamicCast<Ray_EventQueryOpen>(ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153)) )
    {
        queryOpen->setOpen( m_state == State_On ? btrue : bfalse );
    }
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::updateAnimInput()
{
    ITF_ASSERT(m_animatedComponent);
    if (m_animatedComponent)
    {
        m_animatedComponent->setInput(
            ITF_GET_STRINGID_CRC(OnOffCursor,3056046778),
            m_onOffCursor);
    }
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::processStick(const EventStickOnPolyline* _stick)
{
    if (!getTemplate()->getTriggerOnStick())
    {
        return;
    }

    // count sticked actors
    if (_stick->isSticked())
    {
        ++m_stickedActorsCount;
        m_activator = _stick->getActor();
    }
    else
    {
        ITF_ASSERT(m_stickedActorsCount>0);
        --m_stickedActorsCount;
    }
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::processHit(const HitStim* _hitStim)
{
    if (!getTemplate()->getTriggerOnHit())
    {
        return;
    }
 
    // ignore hits in a direction really different than ours (RO-7256)
    f32 angle = m_actor->getAngle();
    NormalizeAngle2PI(angle);
    Vec2d direction = Vec2d::Up.Rotate(angle);
    f32 angleDiff = _hitStim->getDirection().getShortestAngle(direction);
    if (angleDiff<2.0f*MTH_PIBY3 || angleDiff>4.0f*MTH_PIBY3)
    {
        return;
    }

    bbool hitSuccess = btrue;

    if ( getTemplate()->getIsProgressive() )
    {
        if ( ( m_progressiveCursor < 100 ) && ( _hitStim->getLevel() < getTemplate()->getProgressiveHitLevels().size() ) )
        {
            m_progressiveCursor += getTemplate()->getProgressiveHitLevels()[_hitStim->getLevel()];
            if ( m_progressiveCursor >= 100 )
            {
                m_progressiveCursor = 100;
                m_activator = _hitStim->getSender();
                trigger();
            }
        }
        else 
        {
            hitSuccess = bfalse;
        }
    }
    else
    {
        if ( ( m_state == State_On ) && getTemplate()->getStayOn() )
        {
            hitSuccess = bfalse;
        }
        // record hit if on or off, ignore otherwise
        else if (m_state == State_On || m_state == State_Off)
        {
            hitSuccess = !m_wasHit;
            m_activator = _hitStim->getSender();
            m_wasHit = btrue;
        }
        else
        {
            hitSuccess = bfalse;
        }
    }  

    if ( hitSuccess )
    {
        Actor * sender = AIUtils::getActor( _hitStim->getSender() );
        if ( sender != NULL )
        {
            EventHitSuccessful successfull;
            successfull.setSender( m_actor->getRef() );
            sender->onEvent( &successfull );
        }
    }
}

//------------------------------------------------------------------------------
bbool Ray_PushButtonComponent::getAndResetTriggered()
{
    bbool wasTriggered = bfalse;
    if (getTemplate()->getTriggerOnStick())
    {
        wasTriggered = wasTriggered || (m_stickedActorsCount > 0);
    }
    if (getTemplate()->getTriggerOnHit())
    {
        wasTriggered = wasTriggered || m_wasHit;
        m_wasHit = bfalse;
    }
    return wasTriggered;
}

//------------------------------------------------------------------------------
void Ray_PushButtonComponent::trigger(bbool _count /*= btrue */)
{
    // activate children if needed
    if (getTemplate()->getActivateChildren())
    {
        m_eventActivateHandler.activateChildrenIfNeeded(m_linkComponent);
    }

    // send (potentially delayed) event to children + switch event trigger
    ITF_ASSERT(m_activator.isValid() && m_activator.getActor());
    m_eventTrigger.setActivator(m_activator);
    EVENTDELAY_HANDLER->sendEventToChildren(m_linkComponent, &m_eventTrigger, btrue);

    // count triggers
    if (_count)
    {
        ++m_triggerCount;
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PushButtonComponent_Template)
BEGIN_SERIALIZATION(Ray_PushButtonComponent_Template)
    SERIALIZE_MEMBER("onOffDuration", m_onOffDuration);
    SERIALIZE_MEMBER("delay", m_delay);
    SERIALIZE_MEMBER("activateChildren", m_activateChildren);
    SERIALIZE_MEMBER("triggerOnStick", m_triggerOnStick);
    SERIALIZE_MEMBER("triggerOnHit", m_triggerOnHit);
    SERIALIZE_MEMBER("isProgressive", m_isProgressive );
    SERIALIZE_MEMBER("progressiveSpeed", m_progressiveSpeed );
    SERIALIZE_CONTAINER("progressiveHitLevels", m_progressiveHitLevels );
    SERIALIZE_MEMBER("stayOn", m_stayOn );
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_PushButtonComponent_Template::Ray_PushButtonComponent_Template()
    : Super()
    , m_onOffDuration(1.0f)
    , m_delay(0.0f)
    , m_activateChildren(bfalse)
    , m_triggerOnStick(btrue)
    , m_triggerOnHit(bfalse)
    , m_isProgressive( bfalse )
    , m_stayOn( bfalse )
    , m_progressiveSpeed( 3.f )
{

    m_progressiveHitLevels.resize(AIComponent::s_maxHitLevels);
    m_progressiveHitLevels[0] = 5;
    m_progressiveHitLevels[1] = 25;
    m_progressiveHitLevels[2] = 50;
    m_progressiveHitLevels[3] = 100;
}

//------------------------------------------------------------------------------
Ray_PushButtonComponent_Template::~Ray_PushButtonComponent_Template()
{
}

} // namespace ITF
