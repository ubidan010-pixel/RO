#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DARKBIRDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_DarkBirdAIComponent.h"
#endif //_ITF_RAY_DARKBIRDAICOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DarkBirdAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_DarkBirdAIComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_DarkBirdAIComponent)
    VALIDATE_COMPONENT_PARAM("", m_disappear3dBehavior, "AIBehavior mandatory");
    VALIDATE_COMPONENT_PARAM("", m_triggerBounceBehavior, "AIBehavior mandatory");
    VALIDATE_COMPONENT_PARAM("", m_physComponent, "StickToPolylinePhysComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_DarkBirdAIComponent::Ray_DarkBirdAIComponent() 
    : Super()
    , m_started(bfalse)
    , m_disappear3dBehavior(NULL)
    , m_triggerBounceBehavior(NULL)
{
}

//------------------------------------------------------------------------------
Ray_DarkBirdAIComponent::~Ray_DarkBirdAIComponent()
{
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    // create disappear 3D behavior
    m_disappear3dBehavior = createAiBehavior(getTemplate()->getDisappear3dBehavior());
    m_triggerBounceBehavior = createAiBehavior(getTemplate()->getTriggerBounceBehavior());

    Super::onActorLoaded(_hotReload);

    // register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_MusicScoreOpenEvent,2109898583), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_MusicScoreCloseEvent,2878829646), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHasBeenVacuumed,1042014523), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerBounce,1890727905), this);

    // no need to raycast init pos + breaks appear bezier action
    m_useRayCast = bfalse;
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::onBecomeActive()
{
    Super::onBecomeActive();

    // disable actor if not started through music score
    if (!m_started)
    {
        setNullBehavior();
        m_actor->disable();
    }
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    // disable actor if not started through music score
    if (!m_started)
    {
        setNullBehavior();
        m_actor->disable();
    }
    
    // force vertical dark birds
    m_actor->setBoundWorldAngle(0.0f);
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);
    
    if (Ray_MusicScoreOpenEvent* open = _event->DynamicCast<Ray_MusicScoreOpenEvent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreOpenEvent,2109898583)))
    {
        processMusicScoreOpen(open);
    }
    else if (Ray_MusicScoreCloseEvent* close = _event->DynamicCast<Ray_MusicScoreCloseEvent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreCloseEvent,2878829646)))
    {
        processMusicScoreClose(close);
    }
    else if ( Ray_EventHasBeenVacuumed * shooterSwallowed = _event->DynamicCast<Ray_EventHasBeenVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventHasBeenVacuumed,1042014523)) )
    {
        detach();
    }
    else if (Ray_EventTriggerBounce* triggerBounce = _event->DynamicCast<Ray_EventTriggerBounce>(ITF_GET_STRINGID_CRC(Ray_EventTriggerBounce,1890727905)))
    {
        if (m_currentBehavior != m_disappear3dBehavior)
            setBehavior(m_triggerBounceBehavior);
    }
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::onBehaviorFinished()
{
    Super::onBehaviorFinished();
    
    // disable after disappear 3D
    if (isCurrentBehavior(m_disappear3dBehavior))
    {
        setNullBehavior();

        m_actor->disable();
        m_started = bfalse;

        // offscreen update when appearing/disappearing through 3D (RO-11703)
        m_actor->setUpdateType(Pickable::UpdateType_OnlyIfVisible);
    }
    // back to roam after trigger bounce
    else if (isCurrentBehavior(m_triggerBounceBehavior))
    {
        startRoam();
    }
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::startSpawn()
{
    Super::startSpawn();

    // offscreen update when appearing/disappearing through 3D (RO-11703)
    m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::startRoam( const bbool _restart/* = bfalse*/ )
{
    Super::startRoam(_restart);

    // attach to musical score
    attach();

    // offscreen update when appearing/disappearing through 3D (RO-11703)
    m_actor->setUpdateType(Pickable::UpdateType_OnlyIfVisible);
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::startReceiveHit(HitStim* _hit)
{
    Super::startReceiveHit(_hit);

    // detach from musical score
    // for platformer: HP=1, needs to be detached right now (RO-9407)
    if (!getTemplate()->getDetachOnDeath())
    {
        detach();
    }
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::startDeath()
{
    // detach from musical score
    // for shooter: HP>1 so needs to stay attached until really dead
    if (getTemplate()->getDetachOnDeath())
    {
        detach();
    }

    Super::startDeath();
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::attach()
{
    // disable physics, enable bounce trigger
    m_physComponent->setDisabled(btrue);
    AIUtils::enableCollision(m_actor);

    // actor-bind
    ActorBind* parentBind = m_actor->getParentBind();
    ITF_ASSERT_MSG(parentBind, "Dark birds must be actor-bound");
    if (parentBind)
    {
        parentBind->setRuntimeDisabled(bfalse);
    }
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::detach()
{
    // re-enable physics, disable bounce trigger
    m_physComponent->setDisabled(bfalse);
    AIUtils::disableCollision(m_actor);

    // un-actor-bind
    ActorBind* parentBind = m_actor->getParentBind();
    ITF_ASSERT_MSG(parentBind, "Dark birds must be actor-bound");
    if (parentBind)
    {
        parentBind->setRuntimeDisabled(btrue);
    }
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::processMusicScoreOpen(const Ray_MusicScoreOpenEvent* _open)
{
    // do nothing if already dead
    if (getHealth() == 0)
    {
        return;
    }
    
    if (_open->isForced())
    {
        // snap back
        m_actor->setBoundWorldPos(m_actor->getWorldInitialPos());
    
        // start "roam" (= idle)
        startRoam();
    }
    else
    {
        // start spawn behavior
        startSpawn();

        // setup appear 3D action
        Vec3d initPos = m_actor->getBoundWorldInitialPos();
        initPos.m_z += m_actor->getParentBind() ? 0.02f : 0.0f;
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
    }
    
    // remember we were started by the music score
    m_started = btrue;
}

//------------------------------------------------------------------------------
void Ray_DarkBirdAIComponent::processMusicScoreClose(const Ray_MusicScoreCloseEvent* _close)
{
    // already dying, do *not* disappear
    if (isCurrentBehavior(m_receiveHitBehavior) ||
        isCurrentBehavior(m_deathBehavior))
    {
        return;
    }

    // disappear trhough 3D
    setBehavior(m_disappear3dBehavior);

    // offscreen update when appearing/disappearing through 3D (RO-11703)
    m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);

    // setup disappear 3D action
    Vec3d initPos = m_actor->getPos();
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
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DarkBirdAIComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_DarkBirdAIComponent_Template)
    SERIALIZE_MEMBER("appear3dOffset", m_appear3dOffset);
    SERIALIZE_MEMBER("detachOnDeath", m_detachOnDeath);
    SERIALIZE_OBJECT_WITH_FACTORY("disappear3dBehavior", m_disappear3dBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("triggerBounceBehavior", m_triggerBounceBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_DarkBirdAIComponent_Template::Ray_DarkBirdAIComponent_Template()
    : Super()
    , m_appear3dOffset(Vec3d::One)
    , m_detachOnDeath(bfalse)
    , m_disappear3dBehavior(NULL)
    , m_triggerBounceBehavior(NULL)
{
}

//------------------------------------------------------------------------------
Ray_DarkBirdAIComponent_Template::~Ray_DarkBirdAIComponent_Template()
{
    SF_DEL(m_disappear3dBehavior);
    SF_DEL(m_triggerBounceBehavior);
}

} // namespace ITF
