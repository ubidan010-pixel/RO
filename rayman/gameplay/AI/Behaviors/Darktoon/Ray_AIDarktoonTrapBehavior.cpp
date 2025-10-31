#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonTrapBehavior.h"
#endif //_ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonTrapBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AIDarktoonTrapBehavior)
    VALIDATE_BEHAVIOR_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("idle", m_idle, "Ray_AIPerformHitPolylinePunchAction mandatory");
    VALIDATE_BEHAVIOR_PARAM("attack", m_attack, "Ray_AIPerformHitPolylinePunchAction mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AIDarktoonTrapBehavior::Ray_AIDarktoonTrapBehavior()
    : Super()
    , m_state(State_NA)
    , m_coolDownTimer(0.0f)
    , m_lookOut(bfalse)
    , m_openHole(bfalse)
    , m_idle(NULL)
    , m_attack(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIDarktoonTrapBehavior::~Ray_AIDarktoonTrapBehavior()
{
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // create actions from template
    m_idle = createAiActionAndCast<Ray_AIPerformHitPolylinePunchAction>(getTemplate()->getIdle());
    m_attack = createAiActionAndCast<Ray_AIPerformHitPolylinePunchAction>(getTemplate()->getAttack());
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::update(f32 _dt)
{
    Super::update(_dt);

    switch (m_state)
    {
    case State_Idle:
    {
        updateIdleState(_dt);
    }
    break;
    case State_Attack:
    {
        updateAttackState(_dt);
    }
    break;
    }
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Lookout,4210123925), m_lookOut);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(AttackRate,4213680946), getTemplate()->getAttackRate());
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::onActivate()
{
    // starts idle
    setState(State_Idle);

    // no cool down at startup
    m_coolDownTimer = 0.0f; 
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::onDeactivate()
{
    Super::onDeactivate();
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::onActionFinished()
{
    Super::onActionFinished();

    if (m_state == State_Attack)
    {
        setState(State_Idle);
    }
}

//------------------------------------------------------------------------------
bbool Ray_AIDarktoonTrapBehavior::getStartsHidden() const
{
    return getTemplate()->getStartsHidden();
}

//------------------------------------------------------------------------------
bbool Ray_AIDarktoonTrapBehavior::getOpenHole() const
{
    return m_openHole;
}

//------------------------------------------------------------------------------
bbool Ray_AIDarktoonTrapBehavior::returnsHits(u32 hitLevel) const
{
    // can return hits only when attacking
    if (m_currentAction == m_idle && !m_idle->isPerformingHit())
    {
        return bfalse;
    }
    if (m_currentAction == m_attack && !m_attack->isPerformingHit())
    {
        return bfalse;
    }

    // check if sensible to this hit level
    if (hitLevel >= getTemplate()->getReturnHitMaxLevel())
    {
        return bfalse;
    }

    return btrue;
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::setState(State_t _state)
{
    switch (_state)
    {
    case State_Idle:
    {
        // root delta usage not really floating-point-precision-proof, make sure
        // to reset the start position each time we're back to idle
        m_actor->set2DPos(m_actor->getWorldInitialPos().truncateTo2D());
        m_coolDownTimer = getTemplate()->getCoolDownDuration();
        setAction(m_idle);
    }
    break;
    case State_Attack:
    {
        setAction(m_attack);
    }
    break;
    }

    m_state = _state;
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::updateIdleState(f32 _dt)
{
    // update cool down timer
    if (m_coolDownTimer > 0.0f)
    {
        m_coolDownTimer = Max(0.0f, m_coolDownTimer-_dt);
    }

    // check if there's players around
    bbool playerInOuterShape = bfalse;
    bbool playerInInnerShape = bfalse;
    const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
    for (u32 i = 0; i < uPlayerCount; ++i)
    {
        // ignore players on different layers
        Actor* pActor = GAMEMANAGER->getActivePlayer(i);
        if ( !pActor || DepthRange(pActor->getDepth()) != DepthRange(m_actor->getDepth()))
        {
            continue;
        }

        // ignore dead players
        EventQueryIsDead query;
        pActor->onEvent(&query);
        if (query.getIsDead())
        {
            continue;
        }

        // check collision with outer shape
        EventCollide evtOuter;
        evtOuter.setShape(getTemplate()->getOuterShape());
        evtOuter.setPos(m_actor->getWorldInitialPos().truncateTo2D());
        evtOuter.setAngle(m_actor->getAngle());
        pActor->onEvent(&evtOuter);
        if (evtOuter.isCollide())
        {
            playerInOuterShape = btrue;
        }
        else
        {
            continue; // won't collide with inner shape
        }

        // check collision with inner shape
        EventCollide evtInner;
        evtInner.setShape(getTemplate()->getInnerShape());
        evtInner.setPos(m_actor->getWorldInitialPos().truncateTo2D());
        evtInner.setAngle(m_actor->getAngle());
        pActor->onEvent(&evtInner);
        if (evtInner.isCollide())
        {
            playerInInnerShape = btrue;
        }
    }

    // look out and open hole if started hidden and a player is inside outer
    // shape 
    m_lookOut = getTemplate()->getStartsHidden() && playerInOuterShape;
    m_openHole = getTemplate()->getStartsHidden() && playerInOuterShape;

    // attack when cool down is finished and a player is inside inner shape 
    if (m_coolDownTimer == 0.0f && playerInInnerShape)
    {
        setState(State_Attack);
    }
}

//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::updateAttackState(f32 _dt)
{
    // doesn't apply to attack state
    m_lookOut = false;

    // hole stays open during attack
    m_openHole = true;

    // update position using anim offset
    if (m_animatedComponent->canGetRootDelta())
    {
        Vec2d rootDelta = m_animatedComponent->getRootDeltaCumulated(btrue);
        rootDelta.m_y *= getTemplate()->getHeightFactor();
        m_actor->set2DPos(m_actor->getWorldInitialPos().truncateTo2D() + rootDelta);
    }
}

#ifdef ITF_SUPPORT_EDITOR
//------------------------------------------------------------------------------
void Ray_AIDarktoonTrapBehavior::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All) return;

    DebugDraw::shape(
        m_actor->getWorldInitialPos().truncateTo2D(), m_actor->getPos().m_z, m_actor->getAngle(), getTemplate()->getOuterShape(),
        Color::green(), 0.f, "outerShape", 85 * MTH_DEGTORAD);
    DebugDraw::shape(
        m_actor->getWorldInitialPos().truncateTo2D(), m_actor->getPos().m_z, m_actor->getAngle(), getTemplate()->getInnerShape(),
        Color::red(), 0.f, "innerShape", 85 * MTH_DEGTORAD);
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonTrapBehavior_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIDarktoonTrapBehavior_Template)
    SERIALIZE_MEMBER("startsHidden", m_startsHidden);
    SERIALIZE_MEMBER("heightFactor", m_heightFactor);
    SERIALIZE_MEMBER("attackRate", m_attackRate);
    SERIALIZE_MEMBER("coolDownDuration", m_coolDownDuration);
    SERIALIZE_MEMBER("returnHitMaxLevel", m_returnHitMaxLevel);
    SERIALIZE_OBJECT_WITH_FACTORY("outerShape", m_outerShape, PHYSWORLD->getShapesFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("innerShape", m_innerShape, PHYSWORLD->getShapesFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIDarktoonTrapBehavior_Template::Ray_AIDarktoonTrapBehavior_Template()
    : m_startsHidden(bfalse)
    , m_heightFactor(1.0f)
    , m_attackRate(1.0f)
    , m_coolDownDuration(0.5f)
    , m_returnHitMaxLevel(0)
    , m_outerShape(NULL)
    , m_innerShape(NULL)
    , m_idle(NULL)
    , m_attack(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIDarktoonTrapBehavior_Template::~Ray_AIDarktoonTrapBehavior_Template()
{
    SF_DEL(m_outerShape);
    SF_DEL(m_innerShape);
    SF_DEL(m_idle);
    SF_DEL(m_attack);
}

} // namespace ITF
