#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBUBBLEPRIZEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/BubblePrize/Ray_AIBubblePrizeBehavior.h"
#endif //_ITF_RAY_AIBUBBLEPRIZEBEHAVIOR_H_

#ifndef _ITF_AIBEZIERACTION_H_
#include "gameplay/ai/Actions/AIBezierAction.h"
#endif //_ITF_AIBEZIERACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_RAY_AIBLOWOFFACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIBlowOffAction.h"
#endif //_ITF_RAY_AIBLOWOFFACTION_H_

#ifndef _ITF_RAY_AIFLYIDLEACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIFlyIdleAction.h"
#endif //_ITF_RAY_AIFLYIDLEACTION_H_

#ifndef _ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIFollowBezierCurveAction.h"
#endif //_ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBubblePrizeBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AIBubblePrizeBehavior)
    m_bubblePrize.validate(_isBehaviorValidated);
    VALIDATE_BEHAVIOR_PARAM("bezierAction", getTemplate()->getBezierAction(), "AIBezierAction mandatory");
    if (!getTemplate()->isCustomCurve())
    {
        VALIDATE_BEHAVIOR_PARAM("blowOffAction",  getTemplate()->getBlowOffAction(), "Ray_AIBlowOffAction mandatory");
    }
    else
    {
        VALIDATE_BEHAVIOR_PARAM("followCurveAction", getTemplate()->getFollowCurveAction(), "Ray_AIFollowBezierCurveAction mandatory");
        VALIDATE_BEHAVIOR_PARAM("flyIdleAction", getTemplate()->getFlyIdleAction(), "Ray_AIFlyIdleAction mandatory");
    }
    VALIDATE_BEHAVIOR_PARAM("", m_animComponent, "AnimLightComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_playerDetectorComponent, "PlayerDetectorComponent mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AIBubblePrizeBehavior::Ray_AIBubblePrizeBehavior()
    : Super()
    , m_bubblePrize()
    , m_state(State_NA)
    , m_launchPos(Vec3d::Zero)
    , m_hitPlayerRef()
    , m_bezierAction(NULL)
    , m_blowOffAction(NULL)
    , m_followCurveAction(NULL)
    , m_flyIdleAction(NULL)
    , m_waterCheckOffset(0)
    , m_playerDetectorComponent(NULL)
{
    static u32 serialNumber = 0;
    m_waterCheckOffset = serialNumber++;
}

//------------------------------------------------------------------------------
Ray_AIBubblePrizeBehavior::~Ray_AIBubblePrizeBehavior()
{
#ifdef ITF_SUPPORT_EDITOR
    EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), m_aiComponent);
#endif
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
   
    // mandatory, validated
    m_playerDetectorComponent = m_actor->GetComponent<PlayerDetectorComponent>();

    // init bubble prize
    m_bubblePrize.init(m_actor, &getTemplate()->getBubblePrize());
   
    // create actions from template
    m_bezierAction = createAiActionAndCast<AIBezierAction>(getTemplate()->getBezierAction());
    if (!getTemplate()->isCustomCurve())
    {
        m_blowOffAction = createAiActionAndCast<Ray_AIBlowOffAction>(getTemplate()->getBlowOffAction());
    }
    else
    {
        m_followCurveAction = createAiActionAndCast<Ray_AIFollowBezierCurveAction>(getTemplate()->getFollowCurveAction());
        m_flyIdleAction = createAiActionAndCast<Ray_AIFlyIdleAction>(getTemplate()->getFlyIdleAction());
    }

    // listen to launch events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryChildLaunch,3130623924),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChildLaunch,2714500499),m_aiComponent);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),m_aiComponent);

#ifdef ITF_SUPPORT_EDITOR
    EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),m_aiComponent);
#endif
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::update(f32 _dt)
{
    Super::update(_dt);
    
    // disabled until launched
    if (m_state == State_NA)
    {
        m_actor->disable();
    }
    
    // update bubble prize
    m_bubblePrize.update(_dt);

    // update content visual
    BubblePrizeBankState bankState = m_bubblePrize.getBankState();
    if (bankState != BubblePrizeBankState_Invalid)
    {
        m_animComponent->setBankState(bankState);
    }

    // actually start bezier action
    // CAVEAT[Laurent]: we can't start this right upon receiving the "launch
    // triggered" event, it would set the actor's position before it's actually
    // enabled, which just breaks it (actor disappears, no longer updated, no
    // longer in the scene...)
    if (m_state == State_Bezier && m_currentAction == NULL)
    {
        Vec2d startTangent = Vec2d::Up * getTemplate()->getBezierStretchStart();
        Vec2d endTangent = Vec2d::Up.Rotate(m_actor->getAngle()) * getTemplate()->getBezierStretchEnd();
        m_bezierAction->setupBezier(
            m_launchPos,
            m_launchPos - startTangent.to3d(),
            m_actor->getPos() - endTangent.to3d(),
            m_actor->getPos()
            );
        m_actor->setPos(m_launchPos);
        setAction(m_bezierAction);
    }

    // pop on first player detected when in a pickable state
    if (m_state == State_BlowOff ||
        m_state == State_FollowCurve ||
        m_state == State_FlyIdle)
    {
        // is there a receiver?
        ActorRef receiverRef;
        if (m_playerDetectorComponent->getActorsInside().size() > 0)
        {
            receiverRef = m_playerDetectorComponent->getActorsInside().back();
        }
        else if (m_hitPlayerRef.isValid())
        {   
            receiverRef = m_hitPlayerRef;
        }
        
        // if receiver found, pop
        if (receiverRef.isValid())
        {
            m_bubblePrize.pop(receiverRef);

            // done, to death behavior
            m_aiComponent->setHealth(0);
            m_state = State_Popped;
        }
    }
    
    // reset detected hits
    m_hitPlayerRef.invalidate();

    // change detector's radius if water detected (RO-16573)
    if (((CURRENTFRAME+m_waterCheckOffset) & 31)==0)
    {
        updatePlayerDetectorRadius();
    }
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::onEvent(Event * _event)
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
    else if (HitStim* hitStim = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
    {
        processHit(hitStim);
    }
    else if (Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
    {
        if (m_actor->getWorld() != SPAWNER->getSpawnerWorld())
        {
            m_bubblePrize.processQueryLumCount(queryLumCount);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::onActionFinished()
{
    Super::onActionFinished();

    // bezier -> blow off or follow curve
    if (isCurrentAction(m_bezierAction))
    {
        if (!getTemplate()->isCustomCurve())
        {
            setAction(m_blowOffAction);
            m_state = State_BlowOff;
        }
        else
        {
            setAction(m_followCurveAction);
            m_state = State_FollowCurve;
        }
    }
    // follow curve -> fly idle
    else if (isCurrentAction(m_followCurveAction))
    {
        ITF_ASSERT(getTemplate()->isCustomCurve());
        setAction(m_flyIdleAction);
        m_state = State_FlyIdle;
    }
    // blow off or fly idle -> to death behavior
    else if (isCurrentAction(m_blowOffAction)
          || isCurrentAction(m_flyIdleAction))
    {
        m_aiComponent->setHealth(0);
        m_state = State_Popped;
    }
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::processLaunchQuery(Ray_EventQueryChildLaunch* _query)
{
    // bubble prizes are launchable objects
    _query->touch();
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::processLaunch(Ray_EventChildLaunch* _launch)
{
    // start moving along a bezier curve
    m_state = State_Bezier;
    m_launchPos = _launch->getLaunchPos();
    setNullAction();
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::processHit(HitStim* _hitStim)
{
    if (GAMEMANAGER->isPlayerActor(_hitStim->getSender()))
    {
        m_hitPlayerRef = _hitStim->getSender();
    }
}

//------------------------------------------------------------------------------
void Ray_AIBubblePrizeBehavior::updatePlayerDetectorRadius()
{
    PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());
    m_playerDetectorComponent->setScaleMultiplier(waterPoly?getTemplate()->getPlayerDetectorMultiplierInWater():1.f);
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIBubblePrizeBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIBubblePrizeBehavior_Template)
    SERIALIZE_MEMBER("isCustomCurve",m_isCustomCurve);
    SERIALIZE_OBJECT("bubblePrize",m_bubblePrize);
    SERIALIZE_MEMBER("bezierStretchStart", m_bezierStretchStart);
    SERIALIZE_MEMBER("bezierStretchEnd", m_bezierStretchEnd);
    SERIALIZE_OBJECT_WITH_FACTORY("bezierAction", m_bezierAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("blowOffAction", m_blowOffAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("followCurveAction", m_followCurveAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("flyIdleAction", m_flyIdleAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_MEMBER("playerDetectorMultiplierInWater", m_playerDetectorMultiplierInWater);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIBubblePrizeBehavior_Template::Ray_AIBubblePrizeBehavior_Template()
    : Super()
    , m_isCustomCurve(bfalse)
    , m_bubblePrize()
    , m_bezierStretchStart(1.0f)
    , m_bezierStretchEnd(1.0f)
    , m_bezierAction(NULL)
    , m_blowOffAction(NULL)
    , m_followCurveAction(NULL)
    , m_flyIdleAction(NULL)
    , m_playerDetectorMultiplierInWater(1.5f)
{
}

//------------------------------------------------------------------------------
Ray_AIBubblePrizeBehavior_Template::~Ray_AIBubblePrizeBehavior_Template()
{
    SF_DEL(m_bezierAction);
    SF_DEL(m_blowOffAction);
    SF_DEL(m_followCurveAction);
    SF_DEL(m_flyIdleAction);
}

} // namespace ITF
