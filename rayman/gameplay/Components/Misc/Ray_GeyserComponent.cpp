#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GEYSERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_GeyserComponent.h"
#endif //_ITF_RAY_GEYSERCOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_GeyserComponent)
BEGIN_SERIALIZATION_CHILD(Ray_GeyserComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_GeyserComponent)
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_GeyserComponent::Ray_GeyserComponent()
    : Super()
    , m_state(State_NA)
    , m_warnTimer(0.0f)
    , m_launchTimer(0.0f)
    , m_nextLaunchTime(0.0f)
    , m_warnFxGenerator()
    , m_launchFxGenerator()
    , m_animatedComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_GeyserComponent::~Ray_GeyserComponent()
{
}

//------------------------------------------------------------------------------
void Ray_GeyserComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // optional + we update anim inputs
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }

    // listen to trigger event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);

    // needs spawnees
    if (!getTemplate()->getWarnFx().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(
            m_actor, &m_warnFxGenerator, getTemplate()->getWarnFx());
    }
    if (!getTemplate()->getLaunchFx().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(
            m_actor, &m_launchFxGenerator, getTemplate()->getLaunchFx());
    }
    
    // start idle
    setState(State_Idle);
}

//------------------------------------------------------------------------------
void Ray_GeyserComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    // backtracking fix, disable if triggered before checkpoint save (RO-12379)
    if (!canLaunch())
    {
        setState(State_Done);
    }
}

//------------------------------------------------------------------------------
void Ray_GeyserComponent::Update(f32 _dt)
{
    Super::Update(_dt);

    switch (m_state)
    {
    case State_Warn:
    {
        // update timer, start launch when duration passed
        if ((m_warnTimer += _dt) > getTemplate()->getWarnDuration())
        {
            if (getLaunchCount() == 0)
            {
                // warn if no children found
                ITF_WARNING(
                    m_actor, bfalse,
                    "Ray_GeyserComponent needs at least one launchable child."
                    );
                setState(State_Done);
            }
            else
            {
                setState(State_Launch);
            }
        }
    }
    break;
    case State_Launch:
    {
        // child launch at regular times
        if (m_launchTimer >= m_nextLaunchTime)
        {
            launch();
            
            ITF_ASSERT(getLaunchCount() > 0);
            f32 launchPeriod = getTemplate()->getLaunchDuration() / getLaunchCount();
            m_nextLaunchTime = launchPeriod * getNextLaunchIndex();
        }

        // update timer, done when duration passed
        if ((m_launchTimer += _dt) > getTemplate()->getLaunchDuration())
        {
            setState(State_Done);
        }
    }
    break;
    }
}

//------------------------------------------------------------------------------
void Ray_GeyserComponent::updateAnimInput()
{
    if (!m_animatedComponent)
    {
        return;
    }

    m_animatedComponent->setInput(
        ITF_GET_STRINGID_CRC(WarnCursor,2610543747),
        m_warnTimer / getTemplate()->getWarnDuration());

    m_animatedComponent->setInput(
        ITF_GET_STRINGID_CRC(LaunchCursor,874938823),
        m_launchTimer / getTemplate()->getLaunchDuration());
}

//------------------------------------------------------------------------------
void Ray_GeyserComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);

    // process events
    if (EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        processTrigger(trigger);
    }
}

//------------------------------------------------------------------------------
void Ray_GeyserComponent::processTrigger(EventTrigger* trigger)
{
    // ignore retrigger, handled internally
    if (trigger->getFromRetriggerOnCheckpoint())
    {
        return;
    }

    // idle -> warn
    if (m_state == State_Idle)
    {
        setState(State_Warn);
    }
}

//------------------------------------------------------------------------------
void Ray_GeyserComponent::setState(State_t _state)
{
    if (_state ==  m_state)
    {
        return;
    }

    // set new anims, reset timers
    switch (_state)
    {
    case State_Idle:
    {
        if (m_animatedComponent && getTemplate()->getIdleAnim().isValid())
        {
            m_animatedComponent->setAnim(getTemplate()->getIdleAnim());
        }
    }
    break;
    case State_Warn:
    {
        m_warnTimer = 0.0f;

        if (m_animatedComponent && getTemplate()->getWarnAnim().isValid())
        {
            m_animatedComponent->setAnim(getTemplate()->getWarnAnim());
        }

        // shake camera
        if (getTemplate()->getLaunchCameraShake().isValid())
        {
            CAMERACONTROLLERMANAGER->addShake(getTemplate()->getLaunchCameraShake());
        }

        // warn FX
        // + trigger it so it plays only once (RO-14499)
        if (m_warnFxGenerator.isInitialized())
        {
            if (Actor* fx = m_warnFxGenerator.getSpawnee(
                m_actor->getScene(),
                m_actor->getPos() + getTemplate()->getWarnFxOffset()))
            {
                EventTrigger trigger;
                trigger.setSender(m_actor->getRef());
                trigger.setActivated(btrue);
                fx->onEvent(&trigger);
            }
        }
    }
    break;
    case State_Launch:
    {
        m_launchTimer = 0.0f;

        if (m_animatedComponent && getTemplate()->getLaunchAnim().isValid())
        {
            m_animatedComponent->setAnim(getTemplate()->getLaunchAnim());
        }

        // spawn FX
        // + trigger it so it plays only once (RO-14499)
        if (m_launchFxGenerator.isInitialized())
        {
            if (Actor* fx = m_launchFxGenerator.getSpawnee(
                m_actor->getScene(),
                m_actor->getPos() + getTemplate()->getLaunchFxOffset()))
            {
                EventTrigger trigger;
                trigger.setSender(m_actor->getRef());
                trigger.setActivated(btrue);
                fx->onEvent(&trigger);
            }
        }
    }
    break;
    case State_Done:
    {
        if (m_animatedComponent && getTemplate()->getDoneAnim().isValid())
        {
            m_animatedComponent->setAnim(getTemplate()->getDoneAnim());
        }

        AIUtils::disableCollision(m_actor);
    }
    break;
    }
    m_state = _state;
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_GeyserComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_GeyserComponent_Template)
    SERIALIZE_MEMBER("warnDuration", m_warnDuration);
    SERIALIZE_MEMBER("launchDuration", m_launchDuration);
    SERIALIZE_MEMBER("launchCameraShake", m_launchCameraShake);
    SERIALIZE_MEMBER("warnFx", m_warnFx);
    SERIALIZE_MEMBER("warnFxOffset", m_warnFxOffset);
    SERIALIZE_MEMBER("launchFx", m_launchFx);
    SERIALIZE_MEMBER("launchFxOffset", m_launchFxOffset);
    SERIALIZE_MEMBER("idleAnim", m_idleAnim);
    SERIALIZE_MEMBER("warnAnim", m_warnAnim);
    SERIALIZE_MEMBER("launchAnim", m_launchAnim);
    SERIALIZE_MEMBER("doneAnim", m_doneAnim);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_GeyserComponent_Template::Ray_GeyserComponent_Template()
    : Super()
    , m_warnDuration(0.0f)
    , m_launchDuration(0.0f)
    , m_launchCameraShake()
    , m_warnFx()
    , m_warnFxOffset(Vec3d::Zero)
    , m_launchFx()
    , m_launchFxOffset(Vec3d::Zero)
    , m_idleAnim()
    , m_warnAnim()
    , m_launchAnim()
    , m_doneAnim()
{
}

//------------------------------------------------------------------------------
Ray_GeyserComponent_Template::~Ray_GeyserComponent_Template()
{
}

} // namespace ITF