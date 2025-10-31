#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_UTENSILTRAPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/UtensilTrap/Ray_AIUtensilTrapBehavior.h"
#endif //_ITF_RAY_UTENSILTRAPBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef ITF_SUPPORT_EDITOR
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIUtensilTrapBehavior)
BEGIN_VALIDATE_BEHAVIOR(Ray_AIUtensilTrapBehavior)
    VALIDATE_BEHAVIOR_PARAM("trapAction", m_trapAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("launchAction", m_launchAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("flyAction", m_flyAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("stickAction", m_stickAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("platformAction", m_platformAction, "Action mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_polylineComponent, "PolylineComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_subAnchorComponent, "Ray_SubAnchorComponent mandatory");
    VALIDATE_BEHAVIOR_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
END_VALIDATE_BEHAVIOR()

//------------------------------------------------------------------------------
Ray_AIUtensilTrapBehavior::Ray_AIUtensilTrapBehavior()
    : Super()
    , m_state(State_NA)
    , m_firstFrame(bfalse)
    , m_rayLength(0.0f)
    , m_noseColPos(Vec2d::Zero)
    , m_trapAction(NULL)
    , m_launchAction(NULL)
    , m_flyAction(NULL)
    , m_stickAction(NULL)
    , m_platformAction(NULL)
    , m_polylineComponent(NULL)
    , m_subAnchorComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIUtensilTrapBehavior::~Ray_AIUtensilTrapBehavior()
{
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    // create actions
    m_trapAction = createAiAction(getTemplate()->getTrapAction());
    m_launchAction = createAiAction(getTemplate()->getLaunchAction());
    m_flyAction = createAiAction(getTemplate()->getFlyAction());
    m_stickAction = createAiAction(getTemplate()->getStickAction());
    m_platformAction = createAiAction(getTemplate()->getPlatformAction());

    Super::onActorLoaded(_hotReload);
    
    // mandatory, validate
    m_polylineComponent = m_actor->GetComponent<PolylineComponent>();
    m_subAnchorComponent = m_actor->GetComponent<Ray_SubAnchorComponent>();
    if ( getTemplate()->getAssignRewardToActivator() )
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventStimNotify,1337973749), m_aiComponent );
    }
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::onResourceReady()
{
    Super::onResourceReady();

    // start up
    if (m_aiComponent->getHealth() != 0)
    {
        m_firstFrame = btrue;
        changeState(State_Trap);
    }
    // return to platform state after checkpoint (RO-11348)
    else
    {
        // will spawn liana if present (RO-11348 reopen)
        EventUndelaySpawn undelaySpawn;
        m_actor->onEvent(&undelaySpawn);

        changeState(State_Platform);
    }
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::onActivate()
{
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::update(f32 _dt)
{
    Super::update(_dt);

    // debug draw
#ifdef UTENSIL_TRAP_DEBUG_DRAW
    u32 boneIndex = m_animatedComponent->getBoneIndex(ITF_GET_STRINGID_CRC(Blade,4133787031));
    if (boneIndex != U32_INVALID)
    {
        Vec3d bonePos;
        m_animatedComponent->getBonePos(boneIndex, bonePos, btrue);
        DebugDraw::circle(bonePos, 0.1f, Color::red(), 10.0f);
    }
#endif

    updateState(_dt);
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::onEvent(Event* _event)
{
    Super::onEvent(_event);

    if ( EventStimNotify* stimNotify = _event->DynamicCast<EventStimNotify>(ITF_GET_STRINGID_CRC(EventStimNotify,1337973749)) )
    {
        if ( getTemplate()->getAssignRewardToActivator() )
        {
            Actor * sender = AIUtils::getActor( stimNotify->getSender() );
            if ( sender != NULL )
            {
                if ( Ray_AIComponent * senderAi = sender->GetComponent<Ray_AIComponent>() )
                {
                    senderAi->setRewardReceiver( m_activator );
                }
            }
        }
    }    
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::updateAnimInputs()
{
    Super::updateAnimInputs();

    // setup ray visual
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(RayLength,2314794861), m_rayLength / m_actor->getScale().m_x);

    // actors on platform polyline?
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(HasActorsOnPolyline,3510179870), m_polylineComponent && m_polylineComponent->hasActors());
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::onActionFinished()
{
    Super::onActionFinished();

    // launch -> fly
    if (m_state == State_Launch)
    {
        changeState(State_Fly);
    }
    // stick -> platform
    else if (m_state == State_Stick)
    {
        changeState(State_Platform);
    }
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::changeState(State_t _newState)
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
    case State_Trap:
    {
        // start action
        setAction(m_trapAction);
    }
    break;
    case State_Launch:
    {
        // start action
        setAction(m_launchAction);

        // offscreen update when flying (RO-11754)
        m_actor->setUpdateType(Pickable::UpdateType_OffscreenAllowed);
    }
    break;
    case State_Fly:
    {
        // start action
        setAction(m_flyAction);
    }
    break;
    case State_Stick:
    {
        // will spawn liana if present
        EventUndelaySpawn undelaySpawn;
        m_actor->onEvent(&undelaySpawn);

        // start action
        setAction(m_stickAction);

        // snap to stick pos
        m_actor->set2DPos(getRealStickPos());
    }
    break;
    case State_Platform:
    {
        // start action
        setAction(m_platformAction);

        // set health to zero
        // => to reset to this state after a checkpoint (RO-11347 & RO-11348)
        m_aiComponent->setHealth(0);

        // snap to stick pos
        m_actor->set2DPos(getRealStickPos());

        // offscreen update when flying (RO-11754)
        m_actor->setUpdateType(Pickable::UpdateType_OnlyIfVisible);
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_AIUtensilTrapBehavior::updateState(f32 _dt)
{
    // update state
    switch (m_state)
    {
    case State_Trap:
    {
        ITF_ASSERT(isCurrentAction(m_trapAction));

        // update ray length
        Vec3d stickPos = m_subAnchorComponent->getPos(ITF_GET_STRINGID_CRC(stickPos,3480208079));
        m_rayLength = (stickPos - m_actor->getPos()).norm();

#ifdef ITF_SUPPORT_EDITOR
        // editing only: allow flipping the actor by moving the stick pos
        if (stickPos.m_x < m_actor->getPos().m_x && !m_actor->isFlipped())
        {
            m_actor->setFlipped(btrue);
            m_actor->setStartFlipped(btrue);
        }
        else if (stickPos.m_x > m_actor->getPos().m_x && m_actor->isFlipped())
        {
            m_actor->setFlipped(bfalse);
            m_actor->setStartFlipped(bfalse);
        }
#endif
        
        // skip first frame, phantoms are not ready yet
        if (m_firstFrame)
        {
            m_firstFrame = bfalse;
        }
        // check for player phantoms in front of us
        else if (rayCastPhantoms())
        {
            changeState(State_Launch);
        }
    }
    break;
    case State_Launch:
    {
        ITF_ASSERT(isCurrentAction(m_launchAction));
    }
    break;
    case State_Fly:
    {
        ITF_ASSERT(isCurrentAction(m_flyAction));
        
        // move in straight line
        Vec2d travel = _dt * getTemplate()->getSpeed() * AIUtils::getLookDir(m_actor);
        Vec2d newPos = m_actor->get2DPos() + travel;
        m_actor->set2DPos(newPos);

        // stop when stick pos reached
        Vec3d stickPos = m_subAnchorComponent->getPos(ITF_GET_STRINGID_CRC(stickPos,3480208079));
        f32 initToStick = (stickPos - m_actor->getWorldInitialPos()).norm() - getTemplate()->getStickOffset();
        f32 initToHere = (m_actor->getPos() - m_actor->getWorldInitialPos()).norm();
        if (initToHere > initToStick)
        {
            changeState(State_Stick);
        }
    }
    break;
    case State_Stick:
    {
        ITF_ASSERT(isCurrentAction(m_stickAction));
        
#ifdef ITF_SUPPORT_EDITOR
        // editing only: allow editing stick offset at runtime
        m_actor->set2DPos(getRealStickPos());
#endif
    }
    break;
    case State_Platform:
    {
        ITF_ASSERT(isCurrentAction(m_platformAction));
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
bbool Ray_AIUtensilTrapBehavior::rayCastPhantoms()
{
    PhysRayCastContactsContainer contacts;
    if (!PHYSWORLD->rayCastPhantoms(
        m_actor->get2DPos(),
        m_actor->get2DPos() + m_rayLength * AIUtils::getLookDir(m_actor),
        ECOLLISIONFILTER_CHARACTERS, 
        m_actor->getDepth(),
        contacts))
    {
        return bfalse;
    }

    // find any player
    for (u32 i=0; i<contacts.size(); ++i)
    {
        if (Player* player = GAMEMANAGER->getPlayerFromActor(contacts[i].m_collidableUserData, bfalse))
        {
            if (!player->isDead())
            {
                m_activator = ( player->getActor() != NULL ) ? player->getActor()->getRef() : ITF_INVALID_OBJREF;
                return btrue;
            }
        }
    }
    return bfalse;
}

//------------------------------------------------------------------------------
Vec2d Ray_AIUtensilTrapBehavior::getRealStickPos() const
{
    Vec3d stickPos = m_subAnchorComponent->getPos(ITF_GET_STRINGID_CRC(stickPos,3480208079));
    Vec2d realStickPos = stickPos.truncateTo2D() - getTemplate()->getStickOffset() * AIUtils::getLookDir(m_actor);
    return realStickPos;
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIUtensilTrapBehavior_Template)
BEGIN_SERIALIZATION(Ray_AIUtensilTrapBehavior_Template)
    SERIALIZE_MEMBER("stickOffset", m_stickOffset);
    SERIALIZE_MEMBER("speed", m_speed);
    SERIALIZE_OBJECT_WITH_FACTORY("trapAction", m_trapAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("launchAction", m_launchAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("flyAction", m_flyAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("stickAction", m_stickAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("platformAction", m_platformAction, ACTORSMANAGER->getAIActionsFactory());
    SERIALIZE_MEMBER("assignRewardToActivator", m_assignRewardToActivator);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIUtensilTrapBehavior_Template::Ray_AIUtensilTrapBehavior_Template()
    : Super()
    , m_stickOffset(0.0f)
    , m_speed(1.0f)
    , m_trapAction(NULL)
    , m_launchAction(NULL)
    , m_flyAction(NULL)
    , m_stickAction(NULL)
    , m_platformAction(NULL)
    , m_assignRewardToActivator( bfalse )
{
}

//------------------------------------------------------------------------------
Ray_AIUtensilTrapBehavior_Template::~Ray_AIUtensilTrapBehavior_Template()
{
    SF_DEL(m_trapAction);
    SF_DEL(m_launchAction);
    SF_DEL(m_flyAction);
    SF_DEL(m_stickAction);
    SF_DEL(m_platformAction);
}

} // namespace ITF
