#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AINOTEPIAFBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/NotePiaf/Ray_AINotePiafBehavior.h"
#endif //_ITF_RAY_AINOTEPIAFBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEPLAYSTIMS_H_
#include "gameplay/GameplayStims.h"
#endif //_ITF_GAMEPLAYSTIMS_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AINotePiafBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AINotePiafBehavior)
    VALIDATE_BEHAVIOR_PARAM("standAction", m_standAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("fallResistAction", m_fallResistAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("catchAction", m_catchAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("fallBreakAction", m_fallBreakAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("appear3dAction", m_appear3dAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("disappear3dAction", m_disappear3dAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("flyBackAction", m_flyBackAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("landAction", m_landAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("pseudoDeathAction", m_pseudoDeathAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_polylineComponent, "PolylineComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_childLaunchComponent, "Ray_ChildLaunchComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_fxController, "FXController mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AINotePiafBehavior::Ray_AINotePiafBehavior()
    : Super()
    , m_state(State_NA)
    , m_note(U32_INVALID)
    , m_openEvent()
    , m_standTimer(0.0f)
    , m_fallBreakTimer(0.0f)
    , m_carriedWeight(0.0f)
    , m_carriedActors()
    , m_triggerBounceReceived(bfalse)
    , m_standAction(NULL)
    , m_fallResistAction(NULL)
    , m_catchAction(NULL)
    , m_fallBreakAction(NULL)
    , m_appear3dAction(NULL)
    , m_disappear3dAction(NULL)
    , m_flyBackAction(NULL)
    , m_landAction(NULL)
    , m_pseudoDeathAction(NULL)
    , m_polylineComponent(NULL)
    , m_childLaunchComponent(NULL)
{        
}

//------------------------------------------------------------------------------
Ray_AINotePiafBehavior::~Ray_AINotePiafBehavior()
{
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    // create actions
    m_standAction = createAiAction(getTemplate()->getStandAction());
    m_fallResistAction = createAiAction(getTemplate()->getFallResistAction());
    m_catchAction = createAiAction(getTemplate()->getCatchAction());
    m_fallBreakAction = createAiAction(getTemplate()->getFallBreakAction());
    m_appear3dAction = createAiAction(getTemplate()->getAppear3dAction());
    m_disappear3dAction = createAiAction(getTemplate()->getDisappear3dAction());
    m_flyBackAction = createAiAction(getTemplate()->getFlyBackAction());
    m_landAction = createAiAction(getTemplate()->getLandAction());
    m_pseudoDeathAction = createAiAction(getTemplate()->getPseudoDeathAction());

    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_polylineComponent = m_actor->GetComponent<PolylineComponent>();
    m_childLaunchComponent = m_actor->GetComponent<Ray_ChildLaunchComponent>();
    
    // disable bubble prize hint fx
    if (m_childLaunchComponent)
    {
        m_childLaunchComponent->setHintFxRuntimeDisabled(btrue);
    }

    // disable collisions (RO-6676)
    AIUtils::disableCollision(m_actor, btrue);

    // listen to events
    if (!getTemplate()->getBumper())
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601), m_aiComponent);
    }
    else
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerBounce,1890727905), m_aiComponent);
    }
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_MusicScoreOpenEvent,2109898583), m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_MusicScoreCloseEvent,2878829646), m_aiComponent);
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::update(f32 _dt)
{
    Super::update(_dt);

    // start up
    if (m_state == State_NA)
    {
        changeState(State_Stand);
    }

    // update state
    updateState(_dt);
    
    // force vertical note piafs
    m_actor->setBoundWorldAngle(0.0f);
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::onEvent(Event* _event)
{
    Super::onEvent(_event);
    
    // process events
    if (EventStickOnPolyline* stick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
    {
        processStick(stick);
    }
    else if (EventHanging* hang = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)))
    {
        processHang(hang);
    }
    else if (Ray_EventTriggerBounce* triggerBounce = _event->DynamicCast<Ray_EventTriggerBounce>(ITF_GET_STRINGID_CRC(Ray_EventTriggerBounce,1890727905)))
    {
        processTriggerBounce(triggerBounce);
    }
    else if (Ray_MusicScoreOpenEvent* open = _event->DynamicCast<Ray_MusicScoreOpenEvent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreOpenEvent,2109898583)))
    {
        processMusicScoreOpen(open);
    }
    else if (Ray_MusicScoreCloseEvent* close = _event->DynamicCast<Ray_MusicScoreCloseEvent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreCloseEvent,2878829646)))
    {
        processMusicScoreClose(close);
    }
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::updateAnimInputs()
{
    Super::updateAnimInputs();
    
    if (m_animatedComponent && !getTemplate()->getBumper())
    {
        bbool isCarrying = m_carriedWeight >= getTemplate()->getWeightThreshold();
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(IsCarrying,3787999749), isCarrying);
    }
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::onActionFinished()
{
    Super::onActionFinished();
    
    // change state after actions finished "naturally"
    switch (m_state)
    {
    case State_NA: break;
    case State_Stand: break;
    case State_FallResist:
    {
        if (canCatch())
        {
            changeState(State_FallBreak);
        }
        else
        {
            changeState(State_PseudoDeath);
        }
    }
    break;
    case State_Catch:
    {
        changeState(State_Stand);
    }
    break;
    case State_FallBreak:
    {
        ITF_WARNING(m_actor, bfalse, "Something's blocking fall break action");
        changeState(State_PseudoDeath);
    }
    break;
    case State_Appear3d:
    {
        changeState(State_Land);
    }
    break;
    case State_Disappear3d:
    {
        m_actor->disable();
        changeState(State_NA);
    }
    break;
    case State_FlyBack:
    {
        changeState(State_Land);
    }
    break;
    case State_Land:
    {
        changeState(State_Stand);
    }
    break;
    case State_PseudoDeath:
    {
        changeState(State_Appear3d);
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::processStick(const EventStickOnPolyline* _stick)
{
    ITF_ASSERT(!getTemplate()->getBumper());
    if (getTemplate()->getBumper())
    {
        return;
    }

    // record/erase sticked actor
    carryActor(_stick->isSticked(), _stick->getActor(), _stick->getWeight());
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::processHang(const EventHanging* _hang)
{
    ITF_ASSERT(!getTemplate()->getBumper());
    if (getTemplate()->getBumper())
    {
        return;
    }

    // record/erase hanging actor
    carryActor(_hang->isHanging(), _hang->getActor(), _hang->getWeight());
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::processTriggerBounce(const Ray_EventTriggerBounce* _bounce)
{
    ITF_ASSERT(getTemplate()->getBumper());
    if (!getTemplate()->getBumper())
    {
        return;
    }

    // record trigger bounce
    m_triggerBounceReceived = btrue;
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::processMusicScoreOpen(const Ray_MusicScoreOpenEvent* _open)
{
    // copy snap params
    m_openEvent = *_open;
    m_note = _open->getSnapNote();

    // appear through 3D
    if (_open->isForced())
    {
        // snap back
        m_actor->setBoundWorldPos(m_actor->getWorldInitialPos());

        changeState(State_Stand);
    }
    else
    {
        changeState(State_Appear3d);
    }
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::processMusicScoreClose(const Ray_MusicScoreCloseEvent* _close)
{
    // erase snap params
    m_openEvent = Ray_MusicScoreOpenEvent();

    // disappear through 3D
    changeState(State_Disappear3d);
}

//------------------------------------------------------------------------------
bbool Ray_AINotePiafBehavior::detectsWeight()
{
    if (!getTemplate()->getBumper())
    {
        return m_carriedWeight >= getTemplate()->getWeightThreshold();
    }
    else
    {
        if (m_triggerBounceReceived)
        {
            m_triggerBounceReceived = bfalse;
            return btrue;
        }
        return bfalse;
    }
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::emitNote()
{
    // guard
    if (m_note >= getTemplate()->getNoteFxNames().size() ||
        !getTemplate()->getNoteFxNames()[m_note].isValid())
    {
        ITF_WARNING(m_actor, bfalse, "Missing note FX name %d", m_note);
    }
    // play note FX
    else
    {
        m_fxController->playFX(getTemplate()->getNoteFxNames()[m_note]);
    }
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::carryActor(bbool _onOff, const ActorRef& _actorRef, f32 _weight)
{
    if (_onOff)
    {
        // record carried actor
        m_carriedWeight += _weight;
        m_carriedActors.push_back(_actorRef);
    }
    else
    {
        // erase carried actor
        m_carriedWeight -= _weight;
        i32 index = m_carriedActors.find(_actorRef);
        if (index != -1)
        {
            m_carriedActors.eraseNoOrder(index);
        }
    }
}

//------------------------------------------------------------------------------
bbool Ray_AINotePiafBehavior::canCatch() const
{
    // true if snapped to a partition and not at the bottom-most note
    return (m_note != U32_INVALID) && (m_note < (m_openEvent.getMaxNote()-1));
}

//------------------------------------------------------------------------------
f32 Ray_AINotePiafBehavior::getNoteOffset(u32 _note) const
{
    ITF_ASSERT(_note != U32_INVALID);
    return m_openEvent.getYInterval() * (_note - m_openEvent.getSnapNote());
}

//------------------------------------------------------------------------------
bbool Ray_AINotePiafBehavior::noteCaught()
{
    if (m_note == U32_INVALID ||
        m_note >= (m_openEvent.getMaxNote()-1))
    {
        return bfalse;
    }
    
    f32 offset = (m_actor->getWorldInitialPos().m_y - m_actor->getPos().m_y);
    if (offset < getNoteOffset(m_note+1))
    {
        return bfalse;
    }
    
    ++m_note;
    return btrue;
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::changeState(State_t _newState)
{
    // clean previous state
    switch (m_state)
    {
    case State_Appear3d:
    {
        // enabled bubble prize hint fx
        if (m_childLaunchComponent)
        {
            m_childLaunchComponent->setHintFxRuntimeDisabled(bfalse);
        }

        // re-enable collisions (RO-6676)
        AIUtils::disableCollision(m_actor, bfalse);

        // offscreen update when appearing/disappearing through 3D (RO-11703)
        m_actor->setUpdateType(Pickable::UpdateType_OnlyIfVisible);
    }
    break;
    case State_Disappear3d:
    {
        // offscreen update when appearing/disappearing through 3D (RO-11703)
        m_actor->setUpdateType(Pickable::UpdateType_OnlyIfVisible);
    }
    break;
    case State_PseudoDeath:
    {
        // offscreen update on pseudo death (RO-12591)
        m_actor->setUpdateType(Pickable::UpdateType_OnlyIfVisible);
    }
    break;
    }

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
        setAction(m_standAction);
        m_standTimer = 0.0f;
    }
    break;
    case State_FallResist:
    {
        setAction(m_fallResistAction);
        
        // emit note
        emitNote();
    }
    break;
    case State_Catch:
    {
        setAction(m_catchAction);
        
        // setup catch action
        Vec3d catchPos = m_actor->getWorldInitialPos();
        if (m_note != U32_INVALID)
        {
            catchPos.m_y -= getNoteOffset(m_note);
        }
        m_actor->setBoundWorldPos(catchPos);
    }
    break;
    case State_FallBreak:
    {
        ITF_ASSERT(canCatch()); // can't catch? leads to infinite fall
        setAction(m_fallBreakAction);
        m_fallBreakTimer = 0.0f;
    }
    break;
    case State_Appear3d:
    {
        setAction(m_appear3dAction);

        // setup appear 3D action
        Vec3d initPos = m_actor->getWorldInitialPos();
        Vec3d appearPos = initPos + getTemplate()->getAppear3dOffset();
        Vec3d tangentStart = -Vec3d::Up;
        Vec3d tangentEnd = Vec3d::Front;
        EventSetupBezier setupBezier(
            appearPos,
            appearPos + tangentStart,
            initPos - tangentEnd,
            initPos
            );
        m_actor->onEvent(&setupBezier);

        // reset note        
        if (m_openEvent.getSnapNote() != U32_INVALID)
        {
            m_note = m_openEvent.getSnapNote();
        }

        // offscreen update when appearing/disappearing through 3D (RO-11703)
        m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    }
    break;
    case State_Disappear3d:
    {
        setAction(m_disappear3dAction);

        // setup disappear 3D action
        Vec3d initPos = m_actor->getWorldInitialPos();
        Vec3d disappearPos = initPos + getTemplate()->getAppear3dOffset();
        Vec3d tangentStart = -Vec3d::Front;
        Vec3d tangentEnd = Vec3d::Up;
        EventSetupBezier setupBezier(
            initPos,
            initPos + tangentStart,
            disappearPos - tangentEnd,
            disappearPos
            );
        m_actor->onEvent(&setupBezier);

        // disable bubble prize hint fx
        if (m_childLaunchComponent)
        {
            m_childLaunchComponent->setHintFxRuntimeDisabled(btrue);
        }
        // disable collisions (RO-6676)
        AIUtils::disableCollision(m_actor, btrue);

        // offscreen update when appearing/disappearing through 3D (RO-11703)
        m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    }
    break;
    case State_FlyBack:
    {
        setAction(m_flyBackAction);

        // setup fly back action
        Vec2d flyBackPos = m_actor->getWorldInitialPos().truncateTo2D();
        if (m_note != U32_INVALID)
        {
            flyBackPos.m_y -= getNoteOffset(m_note);
        }
        EventSetTargetPos setTargetPosEvt(flyBackPos);
        m_actor->onEvent(&setTargetPosEvt);
    }
    break;
    case State_Land:
    {
        setAction(m_landAction);
    }
    break;
    case State_PseudoDeath:
    {
        setAction(m_pseudoDeathAction);

        // offscreen update on pseudo death (RO-12591)
        m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_AINotePiafBehavior::updateState(f32 _dt)
{
    // update current state, change state on interrupts
    switch (m_state)
    {
    case State_Stand:
    {
        ITF_ASSERT(getCurrentAction() == m_standAction);
        if (detectsWeight())
        {
            changeState(State_FallResist);
        }
        else if ((m_note != U32_INVALID)
              && (m_note > m_openEvent.getSnapNote())
              && (m_standTimer += _dt) >= getTemplate()->getFlyBackDelay())
        {
            m_note = m_openEvent.getSnapNote();
            changeState(State_FlyBack);
        }
    }
    break;
    case State_FallResist:
    {
        ITF_ASSERT(getCurrentAction() == m_fallResistAction);
        if (noteCaught())
        {
            changeState(State_Catch);
        }
    }
    break;
    case State_Catch:
    {
        ITF_ASSERT(getCurrentAction() == m_catchAction);
        if (detectsWeight())
        {
            changeState(State_FallResist);
        }        
    }
    break;
    case State_FallBreak:
    {
        ITF_ASSERT(getCurrentAction() == m_fallBreakAction);
        if (noteCaught())
        {
            changeState(State_Catch);
        }
    }
    break;
    case State_Appear3d:
    {
        ITF_ASSERT(getCurrentAction() == m_appear3dAction);
    }
    break;
    case State_Disappear3d:
    {
        ITF_ASSERT(getCurrentAction() == m_disappear3dAction);
    }
    break;
    case State_FlyBack:
    {
        ITF_ASSERT(getCurrentAction() == m_flyBackAction);
        if (detectsWeight())
        {
            changeState(State_FallResist);
        }
    }
    break;
    case State_Land:
    {
        ITF_ASSERT(getCurrentAction() == m_landAction);
        if (detectsWeight())
        {
            changeState(State_FallResist);
        }
    }
    break;
    case State_PseudoDeath:
    {
        ITF_ASSERT(getCurrentAction() == m_pseudoDeathAction);
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AINotePiafBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AINotePiafBehavior_Template)
    SERIALIZE_MEMBER("weightThreshold", m_weightThreshold);
    SERIALIZE_MEMBER("flyBackDelay", m_flyBackDelay);
    SERIALIZE_MEMBER("fallBreakDelay", m_fallBreakDelay);
    SERIALIZE_MEMBER("appear3dOffset", m_appear3dOffset);
    SERIALIZE_MEMBER("isBumper", m_isBumper);
    SERIALIZE_CONTAINER("noteFxNames", m_noteFxNames);
    SERIALIZE_OBJECT_WITH_FACTORY("standAction", m_standAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("fallResistAction", m_fallResistAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("catchAction", m_catchAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("fallBreakAction", m_fallBreakAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("appear3dAction", m_appear3dAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("disappear3dAction", m_disappear3dAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("flyBackAction", m_flyBackAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("landAction", m_landAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("pseudoDeathAction", m_pseudoDeathAction, ACTORSMANAGER->getAIActionsFactory());
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AINotePiafBehavior_Template::Ray_AINotePiafBehavior_Template()
    : Super()
    , m_weightThreshold(1.0f)
    , m_flyBackDelay(1.0f)
    , m_fallBreakDelay(1.0f)
    , m_appear3dOffset(Vec3d::One)
    , m_isBumper(bfalse)
    , m_noteFxNames()
    , m_standAction(NULL)
    , m_fallResistAction(NULL)
    , m_catchAction(NULL)
    , m_fallBreakAction(NULL)
    , m_appear3dAction(NULL)
    , m_disappear3dAction(NULL)
    , m_flyBackAction(NULL)
    , m_landAction(NULL)
    , m_pseudoDeathAction(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AINotePiafBehavior_Template::~Ray_AINotePiafBehavior_Template()
{
    SF_DEL(m_standAction);
    SF_DEL(m_fallResistAction);
    SF_DEL(m_catchAction);
    SF_DEL(m_fallBreakAction);
    SF_DEL(m_appear3dAction);
    SF_DEL(m_disappear3dAction);
    SF_DEL(m_flyBackAction);
    SF_DEL(m_landAction);
    SF_DEL(m_pseudoDeathAction);
}

} // namespace ITF
