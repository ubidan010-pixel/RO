#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DARKTOONTRAPAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_DarktoonTrapAIComponent.h"
#endif //_ITF_RAY_DARKTOONTRAPAICOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEPLAYSTIMS_H_
#include "gameplay/GameplayStims.h"
#endif //_ITF_GAMEPLAYSTIMS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonTrapBehavior.h"
#endif //_ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DarktoonTrapHoleComponent.h"
#endif //_ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DarktoonTrapAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_DarktoonTrapAIComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_DarktoonTrapAIComponent)
    VALIDATE_COMPONENT_PARAM("", m_physComponent, "StickToPolylinePhysComponent mandatory");
    VALIDATE_COMPONENT_PARAM("trapBehavior", m_trapBehavior, "Ray_AIDarktoonTrapBehavior mandatory");
    VALIDATE_COMPONENT_PARAM("receiveHitBehavior", m_receiveHitBehavior, "Ray_AIReceiveHitBehavior mandatory");
    VALIDATE_COMPONENT_PARAM("deathBehavior", m_deathBehavior, "AIBehavior mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_DarktoonTrapAIComponent::Ray_DarktoonTrapAIComponent()
    : Super()
    , m_holeGenerator()
    , m_trapBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    , m_deathBehavior(NULL)
{
}

//------------------------------------------------------------------------------
Ray_DarktoonTrapAIComponent::~Ray_DarktoonTrapAIComponent()
{
}

//------------------------------------------------------------------------------
void Ray_DarktoonTrapAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    // all three behaviors mandatory and validated
    m_trapBehavior = createAiBehaviorAndCast<Ray_AIDarktoonTrapBehavior>(getTemplate()->getTrapBehavior());
    m_receiveHitBehavior = createAiBehaviorAndCast<Ray_AIReceiveHitBehavior>(getTemplate()->getReceiveHitBehavior());
    m_deathBehavior = createAiBehavior(getTemplate()->getDeathBehavior());

    // this *has* to be called *after* creating the behaviors!!!
    Super::onActorLoaded(_hotReload);

    // listen to events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);

    // hidden trap needs to spawn a hole
    if (m_trapBehavior && m_trapBehavior->getStartsHidden())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_holeGenerator, getTemplate()->getHolePath());
        Actor* hole = m_holeGenerator.getSpawnee(m_actor->getScene(), m_actor->getPos() + getTemplate()->getHoleOffset());
        if (!hole)
        {
            ITF_WARNING(m_actor, NULL, "Can't spawn hole");
        }
        else
        {
            m_hole = hole->getRef();
            ITF_WARNING(hole, hole->GetComponent<Ray_DarktoonTrapHoleComponent>(), "Hole has no Ray_DarktoonTrapHoleComponent");
        }
    }

    // starts without physics
    if (m_physComponent)
    {
        m_physComponent->setDisabled(true);
    }

    // set the trap
    if (m_trapBehavior)
    {
        setBehavior(m_trapBehavior);
    }
}

//------------------------------------------------------------------------------
void Ray_DarktoonTrapAIComponent::Update(f32 _dt)
{
    Super::Update(_dt);

    if (Actor* hole = m_hole.getActor())
    {
        if (Ray_DarktoonTrapHoleComponent* holeComponent = hole->GetComponent<Ray_DarktoonTrapHoleComponent>())
        {
            holeComponent->setOpenHole(m_trapBehavior->getOpenHole());
        }
    }
}

//------------------------------------------------------------------------------
void Ray_DarktoonTrapAIComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);

    // process hit stim
    if (HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)))
    {
        processHit(hit);
    }
}

//------------------------------------------------------------------------------
void Ray_DarktoonTrapAIComponent::onBehaviorFinished()
{
    if (m_currentBehavior == m_receiveHitBehavior)
    {
        setBehavior(m_deathBehavior);
    }
}

//------------------------------------------------------------------------------
void Ray_DarktoonTrapAIComponent::processHit(HitStim* _hit)
{
    ActorRef sender = _hit->getSender();
    if ( m_receiveHitBehavior->canReceiveHit(_hit))
    {
        // returns some hits
        if (m_trapBehavior->returnsHits(_hit->getLevel()))
        {
            // bounce stim in the opposite direction
            PunchStim bounce;
            bounce.setLevel(0);
            if (sender.getActor()->get2DPos().m_x > m_actor->get2DPos().m_x)
            {
                bounce.setDirection(Vec2d::XAxis);
            }
            else
            {
                bounce.setDirection(-Vec2d::XAxis);
            }
            bounce.setDepth(m_actor->getDepth());
            bounce.setSender(m_actor->getRef());
            bounce.setAngle(m_actor->getAngle());
            bounce.setPos(m_actor->get2DPos());
            bounce.setPrevPos(m_actor->get2DPos());
            bounce.setReceivedHitType(RECEIVEDHITTYPE_HURTBOUNCE);
            bounce.setFaction(getFaction());
            if (Actor* senderActor = sender.getActor())
            {
                senderActor->onEvent(&bounce);
            }
        }
        // otherwise fly to death
        else
        {
            // hacky: makes sure hurt mode is active
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(HurtMode,2985216329), true);
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Speed,4218325830), 1.0f);

            // logically dead
            m_health_current = 0;

            // receive hit, fly to death
            m_physComponent->setDisabled(false);
            setBehavior(m_receiveHitBehavior);
            m_receiveHitBehavior->setData(_hit);
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
//------------------------------------------------------------------------------
void Ray_DarktoonTrapAIComponent::onEditorMove(bbool _modifyInitialPos)
{
    if (Actor* hole = m_hole.getActor())
    {
        hole->set2DPos(m_actor->getWorldInitialPos().truncateTo2D());
    }
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DarktoonTrapAIComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_DarktoonTrapAIComponent_Template)
    SERIALIZE_MEMBER("holePath", m_holePath);
    SERIALIZE_MEMBER("holeOffset", m_holeOffset);
    SERIALIZE_OBJECT_WITH_FACTORY("trapBehavior", m_trapBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_DarktoonTrapAIComponent_Template::Ray_DarktoonTrapAIComponent_Template()
    : Super()
    , m_holePath()
    , m_holeOffset(Vec3d::Zero)
    , m_trapBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    , m_deathBehavior(NULL)
{
}

//------------------------------------------------------------------------------
Ray_DarktoonTrapAIComponent_Template::~Ray_DarktoonTrapAIComponent_Template()
{
    SF_DEL(m_trapBehavior);
    SF_DEL(m_receiveHitBehavior);
    SF_DEL(m_deathBehavior);
}

} // namespace ITF
