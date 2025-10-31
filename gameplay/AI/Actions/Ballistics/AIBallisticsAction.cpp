#include "precompiled_gameplay.h"

#ifndef _ITF_AIBALLISTICSACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsAction.h"
#endif //_ITF_AIBALLISTICSACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBallisticsAction)
BEGIN_VALIDATE_ACTION(AIBallisticsAction)
    VALIDATE_ACTION_PARAM("speed, duration", (getTemplate()->getSpeed()>0.0f) != (getTemplate()->getDuration() > 0.0f), "Positive speed xor duration required");
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
AIBallisticsAction::AIBallisticsAction()
    : Super()
    , m_gravity(0.0f)
    , m_start(Vec3d::Zero)
    , m_impulse(Vec3d::Zero)
    , m_origin(Vec3d::Zero)
    , m_target(Vec3d::Zero)
    , m_physWasDisabled(bfalse)
    , m_physComponent(NULL)
    , m_stickToPolylineComponent(NULL)
    , m_timer(0.0f)
{
}

//------------------------------------------------------------------------------
AIBallisticsAction::~AIBallisticsAction()
{
}

//------------------------------------------------------------------------------
void AIBallisticsAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // not mandatory
    m_physComponent = m_actor->GetComponent<PhysComponent>();
    m_stickToPolylineComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

    // listen to set target pos event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetTargetPos,2424671732),m_aiComponent);
}

//------------------------------------------------------------------------------
void AIBallisticsAction::onActivate()
{
    Super::onActivate();

    // reset timer
    m_timer = 0.0f;

    // remember start position
    m_start = m_actor->getPos();

    // subclasses need to determine the gravity and initial impulse
    setupTrajectory();

    // make sure the physics component is disabled if present
    if (m_physComponent)
    {
        m_physWasDisabled = m_physComponent->getDisabled();
        m_physComponent->setDisabled(true);
    }
}

//------------------------------------------------------------------------------
void AIBallisticsAction::onDeactivate()
{
    Super::onDeactivate();

    // re-enable physics if disabled previously
    if (m_physComponent)
    {
        m_physComponent->setDisabled(m_physWasDisabled);
    }
}

//------------------------------------------------------------------------------
void AIBallisticsAction::update(f32 _dt)
{
    Super::update(_dt);

    // update timer, check when done
    if ((m_timer += _dt) < getDuration())
    {
        // trajectory
        Vec3d pos = Vec3d::Zero;
        pos.m_x = m_start.m_x + m_impulse.m_x * m_timer;
        pos.m_y = m_start.m_y + m_impulse.m_y * m_timer + 0.5f * m_gravity * m_timer * m_timer;
        pos.m_z = m_start.m_z + m_impulse.m_z * m_timer;
        m_actor->setBoundWorldPos(pos);
    }
    else
    {
        // snap to final pos when done
        m_actor->setBoundWorldPos(m_target);
        m_aiBehavior->onActionFinished();
    }
}

//------------------------------------------------------------------------------
void AIBallisticsAction::onEvent(Event * _event)
{
    Super::onEvent(_event);
    
    // process events
    if (EventSetTargetPos* setTargetPosEvt = _event->DynamicCast<EventSetTargetPos>(ITF_GET_STRINGID_CRC(EventSetTargetPos,2424671732)))
    {
        setTarget(Vec3d(
            setTargetPosEvt->getTargetPos().m_x,
            setTargetPosEvt->getTargetPos().m_y,
            m_actor->getDepth()
            ));
        setupTrajectory();
    }
}

//------------------------------------------------------------------------------
void AIBallisticsAction::updateAnimInputs()
{
    // cursor for logical animations
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(BallisticsCursor,1818183743), m_timer/getDuration());
}

//------------------------------------------------------------------------------
f32 AIBallisticsAction::getDuration() const
{
    // fixed duration?
    if (getTemplate()->getDuration() > 0.0f)
    {
        return getTemplate()->getDuration();
    }
    
    // fixed speed?
    if (getTemplate()->getSpeed() > 0.0f)
    {
        f32 dist = (m_target - m_origin).norm();
        f32 duration = dist / getTemplate()->getSpeed();
        return duration;
    }
    
    ITF_ASSERT(bfalse);
    return 0.0f;
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBallisticsAction_Template)
BEGIN_SERIALIZATION_ABSTRACT_CHILD(AIBallisticsAction_Template)
    SERIALIZE_MEMBER("duration", m_duration);
    SERIALIZE_MEMBER("speed", m_speed);
END_SERIALIZATION()

//------------------------------------------------------------------------------
AIBallisticsAction_Template::AIBallisticsAction_Template()
    : Super()
    , m_duration(0.0f)
    , m_speed(0.0f)
{
    m_animDefaultAction = ITF_GET_STRINGID_CRC(BALLISTICS,710018913);
}

//------------------------------------------------------------------------------
AIBallisticsAction_Template::~AIBallisticsAction_Template()
{
}

} // namespace ITF
