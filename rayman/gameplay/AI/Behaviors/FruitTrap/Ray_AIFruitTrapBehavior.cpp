#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FRUITTRAPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/FruitTrap/Ray_AIFruitTrapBehavior.h"
#endif //_ITF_RAY_FRUITTRAPBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIFruitTrapBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AIFruitTrapBehavior)
    VALIDATE_BEHAVIOR_PARAM("standAction", m_standAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("fruitFallAction", m_fruitFallAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_polylineComponent, "PolylineComponent mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AIFruitTrapBehavior::Ray_AIFruitTrapBehavior()
    : Super()
    , m_state(State_NA)
    , m_timer(0.0f)
    , m_crushAttackReceived(bfalse)
    , m_standAction(NULL)
    , m_fruitFallAction(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIFruitTrapBehavior::~Ray_AIFruitTrapBehavior()
{
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    // create actions
    m_standAction = createAiAction(getTemplate()->getStandAction());
    m_fruitFallAction = createAiAction(getTemplate()->getFruitFallAction());

    Super::onActorLoaded(_hotReload);
    
    // mandatory, validated
    m_polylineComponent = m_actor->GetComponent<PolylineComponent>();

    // listen to punches, we want to detect crush attacks
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),m_aiComponent);
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::onActivate()
{
    // start up
    changeState(State_Stand);
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::update(f32 _dt)
{
    Super::update(_dt);

    // update state
    updateState(_dt);
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::onEvent(Event * _event)
{
    Super::onEvent(_event);

    // process events
    if (PunchStim* punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
    {
        processPunch(punch);
    }    
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::updateAnimInputs()
{
    Super::updateAnimInputs();

    // actors on platform polyline?
    static StringID s_IsCarrying = "IsCarrying";
    m_animatedComponent->setInput(s_IsCarrying, isCarrying());
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::onActionFinished()
{
    Super::onActionFinished();

    // fruit fall -> stand
    if (m_state == State_FruitFall)
    {
        changeState(State_Stand);
    }
}

//------------------------------------------------------------------------------
bbool Ray_AIFruitTrapBehavior::isCarrying() const
{
    return m_polylineComponent
        && m_polylineComponent->getMaxUserWeight() > getTemplate()->getWeightThreshold();
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::changeState(State_t _newState)
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
    case State_Stand:
    {
        // start action
        setAction(m_standAction);
        
        // reset stuff
        m_timer = 0.0f;
        m_crushAttackReceived = bfalse;
    }
    break;
    case State_FruitFall:
    {
        // start action
        setAction(m_fruitFallAction);
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::updateState(f32 _dt)
{
    // update state
    switch (m_state)
    {
    case State_Stand:
    {
        ITF_ASSERT(isCurrentAction(m_standAction));
        
        if (m_crushAttackReceived)
        {
            // fruit falls on crush attack
            changeState(State_FruitFall);
        }
        else
        {
            // update timer when carrying something (i.e. weight threshold passed)
            if (isCarrying())
            {
                m_timer += _dt;
            }
            else
            {
                m_timer = 0.0f;
            }

            // fruit falls when delay passed
            if (m_timer >= getTemplate()->getDelay())
            {
                changeState(State_FruitFall);
            }
        }
    }
    break;
    case State_FruitFall:
    {
        ITF_ASSERT(isCurrentAction(m_fruitFallAction));
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_AIFruitTrapBehavior::processPunch(const PunchStim* _punch)
{
    // launch all children (bubble prizes) on crush attack
    if (_punch->getReceivedHitType() == RECEIVEDHITTYPE_EARTHQUAKE)
    {
        m_crushAttackReceived = btrue;
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIFruitTrapBehavior_Template)
BEGIN_SERIALIZATION(Ray_AIFruitTrapBehavior_Template)
    SERIALIZE_MEMBER("delay", m_delay);
    SERIALIZE_MEMBER("weightThreshold", m_weightThreshold);
    SERIALIZE_OBJECT_WITH_FACTORY("standAction", m_standAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("fruitFallAction", m_fruitFallAction, ACTORSMANAGER->getAIActionsFactory());
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIFruitTrapBehavior_Template::Ray_AIFruitTrapBehavior_Template()
    : Super()
    , m_delay(0.0f)
    , m_weightThreshold(1.0f)
    , m_standAction(NULL)
    , m_fruitFallAction(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIFruitTrapBehavior_Template::~Ray_AIFruitTrapBehavior_Template()
{
    SF_DEL(m_standAction);
    SF_DEL(m_fruitFallAction);
}

} // namespace ITF
