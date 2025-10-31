#include "precompiled_gameplay_rayman.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_RAY_WM_CONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_ControllerComponent.h"
#endif //_ITF_RAY_WM_CONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_ControllerComponent)
BEGIN_SERIALIZATION_CHILD(Ray_WM_ControllerComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_WM_ControllerComponent)
    m_heartShield.validate(_isComponentValidated);
    VALIDATE_COMPONENT_PARAM("standAnim", getTemplate()->getStandAnim().isValid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("walkAnim", getTemplate()->getWalkAnim().isValid(), "Name mandatory");
    VALIDATE_COMPONENT_PARAM("jumpAnim", getTemplate()->getJumpAnim().isValid(), "Name mandatory");
    m_leadAbility.validate(_isComponentValidated);
    m_followAbility.validate(_isComponentValidated);
    VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_WM_ControllerComponent::Ray_WM_ControllerComponent()
    : Super()
    , m_playerIndex(U32_INVALID)
    , m_state(Ray_WM_ControllerState_NA)
    , m_previousPos(Vec2d::Zero)
    , m_speed2d(Vec2d::Zero)
    , m_moveCursor(0.0f)
    , m_performingUTurn(UTURNTYPE_NONE)
    , m_uTurnFlipped(bfalse)
    , m_uTurnFinished(bfalse)
    , m_flipActor(bfalse)
    , m_isSprinting(bfalse)
    , m_leadAbility()
    , m_followAbility()
    , m_animatedComponent(NULL)
    , m_heartShield()
{
}

//------------------------------------------------------------------------------
Ray_WM_ControllerComponent::~Ray_WM_ControllerComponent()
{
}


//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // init abilities, pass their templates
    m_leadAbility.onActorLoaded(_hotReload, &getTemplate()->getLeadAbility(), this, m_actor);
    m_followAbility.onActorLoaded(_hotReload, &getTemplate()->getFollowAbility(), this, m_actor);
    
    // mandatory, validated
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }

    // register events
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTeleportToActor,557779126), this);

    // heart shield
    m_heartShield.init(m_actor,m_playerIndex, &getTemplate()->getHeartShield());
    m_heartShield.onActorLoaded(_hotReload);
    m_heartShield.reset();
    m_heartShield.show( btrue );
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::onBecomeActive()
{
    Super::onBecomeActive();

    init();

    // restore power ups displays
    m_heartShield.onBecomeActive();
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    reset();

    // kill power ups displays
    m_heartShield.onBecomeInactive();
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::Update(f32 _dt)
{
    Super::Update(_dt);

    // update abilities
    if (m_leadAbility.isActive())
    {
        m_leadAbility.update(_dt);
    }
    else if (m_followAbility.isActive())
    {
        m_followAbility.update(_dt);
    }
    postUpdateAbility(_dt);
    m_heartShield.update(_dt);
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::updateAnimInput()
{
    // do nothing player index not set yet
    if (m_playerIndex == U32_INVALID)
    {
        return;
    }

    // guard against missing component
    ITF_ASSERT(m_animatedComponent);
    if (!m_animatedComponent)
    {
        return;
    }

    // start u-turns
    if (f32_Abs(m_speed2d.m_x) > 0.1f && !isPerformingUTurn())
    {
        if (m_speed2d.m_x > 0.0f)
        {
            if (m_actor->isFlipped())
            {
                startUTurn(UTURNTYPE_GROUND);
            }
        }
        else
        {
            if (!m_actor->isFlipped())
            {
                startUTurn(UTURNTYPE_GROUND);
            }
        }
    }

    // apply flip
    if ( m_flipActor )
    {
        m_actor->setFlipped(!m_actor->isFlipped());
        if ( isPerformingUTurn() )
        {
            m_uTurnFlipped = btrue;
        }
        m_flipActor = bfalse;
    }

    // done flipping
    if (m_uTurnFinished)
    {
        resetUTurn();
    }

    // speed anim inputs
    f32 speedX, speedY;
    if (m_state == Ray_WM_ControllerState_Jump ||
        m_state == Ray_WM_ControllerState_Swim ||
        m_state == Ray_WM_ControllerState_ToFly ||
        m_state == Ray_WM_ControllerState_Fly)
    {
        speedX = m_speed2d.m_x;
        speedY = m_speed2d.m_y;
    }
    else
    {
        speedX = m_speed2d.norm();
        speedY = 0.0f;
    }

    // NB[LaurentCou]: some bad anims with teensies with SpeedX>10.0
    speedX = Min(speedX, 10.0f);

    // input speed
    Vec2d inputSpeed(speedX, speedY);
    f32 inputSpeedNorm = inputSpeed.norm();

    // move anim inputs
    f32 moveX;
    if (!isMoving())
    {
        moveX = 0.0f;
    }
    else
    {
        moveX = m_speed2d.m_x >= 0.0f
            ? getTemplate()->getMoveX()
            : -getTemplate()->getMoveX();
    }

    // set anim inputs
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(MoveX,965019289), moveX);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Move,2544723026), isMoving());
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedX,3996234170), speedX);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SpeedY,461177981), speedY);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Speed,4218325830), inputSpeedNorm);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(MoveCursor,2402187973), m_moveCursor);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(AchievedSpeedFactor,3216667165), 1.0f);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(AirPose,2797818131), AIRPOSE_MIDDLE);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(UTurn,3271188893), m_performingUTurn);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SwimCurveCursor,3090384440), 0.5f);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(SwimEnterHead,288414836), 1u);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(IsSprinting,4259516180), m_isSprinting);
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);

    // set/change player index
    if (EventPlayerIndexChange* playerIndexChange = _event->DynamicCast<EventPlayerIndexChange>(ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152)))
    {
        processPlayerIndexChange(playerIndexChange);
    }
    else if (EventPlayerModeChanged* eventGameMode = _event->DynamicCast<EventPlayerModeChanged>(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275)))
    {
        processPlayerModeChanged(eventGameMode);
    }

    // stop here if player index not set yet
    if (m_playerIndex == U32_INVALID)
    {
        return;
    }

    // process events
    if (AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
    {
        processAnimGameplayEvent(animEvent);
    }
    else if (EventTeleportToActor* teleport = _event->DynamicCast<EventTeleportToActor>(ITF_GET_STRINGID_CRC(EventTeleportToActor,557779126)))
    {
        processTeleport(teleport);
    }

    m_heartShield.onEvent(_event);

}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_WM_ControllerComponent::drawEdit(class ActorDrawEditInterface* _drawInterface, u32 _flags) const
{
    Super::drawEdit(_drawInterface, _flags);

    m_leadAbility.drawEdit(_drawInterface, _flags);
    m_followAbility.drawEdit(_drawInterface, _flags);
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
bbool Ray_WM_ControllerComponent::isMoving() const
{
    return m_state!=Ray_WM_ControllerState_NA
        && m_state!=Ray_WM_ControllerState_Stand
        && m_state!=Ray_WM_ControllerState_StandSwim;
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::init()
{
    ITF_ASSERT(m_playerIndex != U32_INVALID);
    if (m_playerIndex == U32_INVALID)
    {
        return;
    }

    // activate/deactivate the right ability for the right index
    u32 mainPlayerIndex = GAMEMANAGER->getMainIndexPlayer();
    if (m_playerIndex == mainPlayerIndex)
    {
        if (!m_leadAbility.isActive())
        {
            m_leadAbility.onBecomeActive(m_playerIndex);
        }
        if (m_followAbility.isActive())
        {
            m_followAbility.onBecomeInactive();
        }
    }
    else
    {
        if (m_leadAbility.isActive())
        {
            m_leadAbility.onBecomeInactive();
        }
        if (!m_followAbility.isActive())
        {
            m_followAbility.onBecomeActive(m_playerIndex);
        }
    }

    // guard, can't lead and follow at the same time...
    ITF_ASSERT(!(m_leadAbility.isActive() && m_followAbility.isActive()));
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::reset()
{
    // invalidate state (fix for RO-14231)
    changeState(Ray_WM_ControllerState_NA);
    if (m_followAbility.isActive())
    {
        m_followAbility.onBecomeInactive();
    }
    if (m_leadAbility.isActive())
    {
        m_leadAbility.onBecomeInactive();
    }
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::changeState(Ray_WM_ControllerState _newState)
{
    // skip if already in given state
    if (m_state == _newState)
    {
        return;
    }

    // start new state
    switch (m_state = _newState)
    {
    case Ray_WM_ControllerState_NA: break;
    case Ray_WM_ControllerState_Stand:
    {
        m_animatedComponent->setAnim(getTemplate()->getStandAnim());
        m_animatedComponent->resetTree();
        resetUTurn();
    }
    break;
    case Ray_WM_ControllerState_StandSwim:
    {
        m_animatedComponent->setAnim(getTemplate()->getSwimAnim());
        m_animatedComponent->resetTree();
        resetUTurn();
    }
    break;
    case Ray_WM_ControllerState_Walk:
    {
        m_animatedComponent->setAnim(getTemplate()->getWalkAnim());
    }
    break;
    case Ray_WM_ControllerState_Jump:
    {
        m_animatedComponent->setAnim(getTemplate()->getJumpAnim());
        resetUTurn();
    }
    break;
    case Ray_WM_ControllerState_Swim:
    {
        m_animatedComponent->setAnim(getTemplate()->getSwimAnim());
    }
    break;
    case Ray_WM_ControllerState_ToFly:
    {
        m_animatedComponent->setAnim(getTemplate()->getJumpAnim());
        resetUTurn();
    }
    break;
    case Ray_WM_ControllerState_Fly:
    {
        m_animatedComponent->setAnim(getTemplate()->getFlyAnim());
        resetUTurn();
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//-----------------------------------------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::postUpdateAbility(f32 _dt)
{
    // compute speed
    Vec2d newPos = m_actor->get2DPos();
    m_speed2d = (_dt > 0.0f) ? (newPos - m_previousPos) / _dt : Vec2d::Zero;
    m_previousPos = newPos;

    // update move cursor
    if (!isMoving())
    {
        m_moveCursor = 0.0f;
    }
    else
    {
        m_moveCursor += _dt * getTemplate()->getMoveFactorMultiplier();
    }
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::processPlayerIndexChange(EventPlayerIndexChange* _playerIndexChanged)
{
    // skip if didn't really change
    if (_playerIndexChanged->getNewIndex() == m_playerIndex)
    {
        return;
    }

    // store player idnex, ensure it's valid
    m_playerIndex = _playerIndexChanged->getNewIndex();
    ITF_ASSERT(m_playerIndex != U32_INVALID);

    // signal change to follow ability
    if (m_followAbility.isActive())
    {
        m_followAbility.onPlayerIndexChange(_playerIndexChanged->getNewIndex());
    }

    // signal change to heart shield
    m_heartShield.setPlayerIndex(m_playerIndex);
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::processPlayerModeChanged(EventPlayerModeChanged* _eventGameMode)
{
    BaseObject* object = _eventGameMode->getActivator().getObject();
    Actor* activator = object
        ? object->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115))
        : NULL;
    if (activator &&
        activator->isEnabled() &&
        (_eventGameMode->getMode() == RAY_GAMEMODE_WORLDMAP))
    {
        init();
    }
    else
    {
        reset();
    }
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::processAnimGameplayEvent(AnimGameplayEvent* _animEvent)
{
    if (_animEvent->getName() == MRK_UTurn_Finish)
    {
        m_uTurnFinished = btrue;
        if ( m_flipActor )
        {
            // More than one frame was played together, so we need to flip the character right now or he will
            // appear wrong one frame
            m_actor->setFlipped(!m_actor->isFlipped());
            m_animatedComponent->forceUpdateFlip();
        }
    }
    else if ( _animEvent->getName() == MRK_Flip_Actor )
    {
        m_flipActor = btrue;
    }
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::processTeleport(EventTeleportToActor* _teleport)
{
    if (m_leadAbility.isActive())
    {
        m_leadAbility.teleport(_teleport->getActorRef().getActor());
    }

    // reset flip on teleport (RO-14438)
    m_actor->setFlipped(bfalse);
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::startUTurn( EUturnType _type )
{
    // workaround RO-11109
    if (m_state == Ray_WM_ControllerState_Jump ||
        m_state == Ray_WM_ControllerState_ToFly ||
        m_state == Ray_WM_ControllerState_Fly ||
        m_isSprinting)
    {
        m_actor->setFlipped(!m_actor->isFlipped());
    }
    else if (!isPerformingUTurn())
    {
        m_performingUTurn = _type;
        m_uTurnFlipped = bfalse;
        m_uTurnFinished = bfalse;
    }
}

//------------------------------------------------------------------------------
void Ray_WM_ControllerComponent::resetUTurn()
{
    if ( isPerformingUTurn() )
    {
        m_performingUTurn = UTURNTYPE_NONE;
        m_uTurnFinished = bfalse;

        if ( !m_uTurnFlipped )
        {
            m_actor->setFlipped(!m_actor->isFlipped());
            m_flipActor = bfalse;
        }
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_ControllerComponent_Template)
BEGIN_SERIALIZATION(Ray_WM_ControllerComponent_Template)
    SERIALIZE_MEMBER("moveX", m_moveX);
    SERIALIZE_MEMBER("moveFactorMultiplier", m_moveFactorMultiplier);
    SERIALIZE_MEMBER("standAnim", m_standAnim);
    SERIALIZE_MEMBER("walkAnim", m_walkAnim);
    SERIALIZE_MEMBER("jumpAnim", m_jumpAnim);
    SERIALIZE_MEMBER("swimAnim", m_swimAnim);
    SERIALIZE_MEMBER("flyAnim", m_flyAnim);
    SERIALIZE_OBJECT("leadAbility", m_leadAbility);
    SERIALIZE_OBJECT("followAbility", m_followAbility);
    SERIALIZE_OBJECT("heartShield",m_heartShield);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_WM_ControllerComponent_Template::Ray_WM_ControllerComponent_Template()
    : Super()
    , m_moveX(1.0f)
    , m_moveFactorMultiplier(1.0f)
    , m_standAnim()
    , m_walkAnim()
    , m_jumpAnim()
    , m_swimAnim()
    , m_flyAnim()
    , m_leadAbility()
    , m_followAbility()
    , m_heartShield()
{
}

//------------------------------------------------------------------------------
Ray_WM_ControllerComponent_Template::~Ray_WM_ControllerComponent_Template()
{
}

} // namespace ITF
