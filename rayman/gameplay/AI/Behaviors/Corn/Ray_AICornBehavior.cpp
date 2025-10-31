#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AICORNBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Corn/Ray_AICornBehavior.h"
#endif //_ITF_RAY_AICORNBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_RAY_AICORNFLOATACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AICornFloatAction.h"
#endif //_ITF_RAY_AICORNFLOATACTION_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AICornBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AICornBehavior)
    VALIDATE_BEHAVIOR_PARAM("", m_physComponent, "PhysComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_subAnchorComponent, "Ray_SubAnchorComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("jumpAction", m_jumpAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("burnAction", m_burnAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("popAction", m_popAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("floatAction", m_floatAction, "Action mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AICornBehavior::Ray_AICornBehavior()
    : Super()
    , m_state(State_NA)
    , m_launchPos()
    , m_jumpAction(NULL)
    , m_burnAction(NULL)
    , m_popAction(NULL)
    , m_floatAction(NULL)
    , m_physComponent(NULL)
    , m_subAnchorComponent(NULL)
{        
}

//------------------------------------------------------------------------------
Ray_AICornBehavior::~Ray_AICornBehavior()
{
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_jumpAction = createAiAction(getTemplate()->getJumpAction());
    m_burnAction = createAiAction(getTemplate()->getBurnAction());
    m_popAction = createAiAction(getTemplate()->getPopAction());
    m_floatAction = createAiActionAndCast<Ray_AICornFloatAction>(getTemplate()->getFloatAction());

    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_physComponent = m_actor->GetComponent<PhysComponent>();
    m_subAnchorComponent = m_actor->GetComponent<Ray_SubAnchorComponent>();
    
    // listen to launch events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryChildLaunch,3130623924),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChildLaunch,2714500499),m_aiComponent);
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::onActivate()
{
    //Super::onActivate();

    // start up
    changeState(State_Float);
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::onDeactivate()
{
    Super::onDeactivate();

    // unregister from camera
    CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::update(f32 _dt)
{
    Super::update(_dt);
    
    updateState(_dt);
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::onEvent(Event* _event)
{
    Super::onEvent(_event);
    
    // process events
    if (Ray_EventQueryChildLaunch* query = _event->DynamicCast<Ray_EventQueryChildLaunch>(ITF_GET_STRINGID_CRC(Ray_EventQueryChildLaunch,3130623924)))
    {
        processLaunchQuery(query);
    }
    else if (Ray_EventChildLaunch* launch = _event->DynamicCast<Ray_EventChildLaunch>(ITF_GET_STRINGID_CRC(Ray_EventChildLaunch,2714500499)))
    {
        processLaunch(launch);
    }
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::onActionFinished()
{
    Super::onActionFinished();

    // jump -> burn -> pop -> float
    switch (m_state)
    {
    case State_Jump:
    {
        ITF_ASSERT(getCurrentAction() == m_jumpAction);
        changeState(State_Burn);
    }
    break;
    case State_Burn:
    {
        ITF_ASSERT(getCurrentAction() == m_burnAction);
        changeState(State_Pop);
    }
    break;
    case State_Pop:
    {
        ITF_ASSERT(getCurrentAction() == m_popAction);
        changeState(State_Float);
    }
    break;
    }
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::processLaunchQuery(Ray_EventQueryChildLaunch* _query)
{
    // yey! I'm a launch behavior, hello!
    _query->touch();
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::processLaunch(Ray_EventChildLaunch* _launch)
{
    m_launchPos = _launch->getLaunchPos();

    // re-apply forced Z (fix for RO-6190)
    const Actor_Template* actorTemplate = m_actor->getTemplate();
    if (actorTemplate->isZForced())
    {
        m_launchPos.m_z += actorTemplate->getForcedZ();
    }
    
    changeState(State_Jump);
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::changeState(State_t _newState)
{
    // skip if already in given state
    if (m_state == _newState)
    {
        return;
    }

    // start new state
    switch (m_state = _newState)
    {
    case State_NA: break;
    case State_Jump:
    {
        // set at source position, and reapply forced depth
        m_actor->setPos(m_launchPos);

        // start jump action
        setAction(m_jumpAction);
        EventSetTargetPos setTargetPosEvt(m_subAnchorComponent->getPos(ITF_GET_STRINGID_CRC(burnPos,1977685890)).truncateTo2D());
        m_actor->onEvent(&setTargetPosEvt);

        // disable physics
        m_physComponent->setDisabled(btrue);
    }
    break;
    case State_Burn:
    {
        // start burn action
        setAction(m_burnAction);

        // disable physics
        m_physComponent->setDisabled(btrue);
    }
    break;
    case State_Pop:
    {
        // start pop action
        setAction(m_popAction);
        EventSetTargetPos setTargetPosEvt(m_actor->getWorldInitialPos().truncateTo2D());
        m_actor->onEvent(&setTargetPosEvt);

        // disable physics
        m_physComponent->setDisabled(btrue);
    }
    break;
    case State_Float:
    {
        // start float action
        setAction(m_floatAction);

        // enable physics
        m_physComponent->setDisabled(bfalse);
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_AICornBehavior::updateState(f32 _dt)
{
    // update current state
    switch (m_state)
    {
    case State_Jump:
    {
        ITF_ASSERT(isCurrentAction(m_jumpAction));
    }
    break;
    case State_Burn:
    {
        ITF_ASSERT(isCurrentAction(m_burnAction));
    }
    break;
    case State_Pop:
    {
        ITF_ASSERT(isCurrentAction(m_popAction));
    }
    break;
    case State_Float:
    {
        ITF_ASSERT(isCurrentAction(m_floatAction));
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AICornBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AICornBehavior_Template)
    SERIALIZE_MEMBER("randomAngle", m_randomAngle);
    SERIALIZE_OBJECT_WITH_FACTORY("jumpAction", m_jumpAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("burnAction", m_burnAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("popAction", m_popAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("floatAction", m_floatAction, ACTORSMANAGER->getAIActionsFactory());
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AICornBehavior_Template::Ray_AICornBehavior_Template()
    : m_randomAngle(bfalse, 0.0f)
    , m_jumpAction(NULL)
    , m_burnAction(NULL)
    , m_popAction(NULL)
    , m_floatAction(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AICornBehavior_Template::~Ray_AICornBehavior_Template()
{
    SF_DEL(m_jumpAction);
    SF_DEL(m_burnAction);
    SF_DEL(m_popAction);
    SF_DEL(m_floatAction);
}

} // namespace ITF
