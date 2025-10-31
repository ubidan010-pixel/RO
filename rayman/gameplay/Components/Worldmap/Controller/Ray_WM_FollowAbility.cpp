#include "precompiled_gameplay_rayman.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_RAY_WM_FOLLOWABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_FollowAbility.h"
#endif //_ITF_RAY_WM_FOLLOWABILITY_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_WM_LEADABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_LeadAbility.h"
#endif //_ITF_RAY_WM_LEADABILITY_H_

#ifndef _ITF_RAY_WM_CONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_ControllerComponent.h"
#endif //_ITF_RAY_WM_CONTROLLERCOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_FollowAbility)
BEGIN_VALIDATE_OBJECT_CHILD(Ray_WM_FollowAbility)
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_WM_FollowAbility::Ray_WM_FollowAbility()
    : Super()
    , m_leadRef()
    , m_listenerRegistered(bfalse)
    , m_listenerLeave(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_WM_FollowAbility::~Ray_WM_FollowAbility()
{
    // reset input listener
    if(m_listenerRegistered)
    {
        m_listenerRegistered = bfalse;
        m_listenerLeave = bfalse;
        GAMEMANAGER->removeInputListener(this);
    }
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::onActorLoaded(
    Pickable::HotReloadType _hotReload,
    const Ray_WM_BaseAbility_Template* _template,
    Ray_WM_ControllerComponent* _controller,
    Actor* _actor)
{
    Super::onActorLoaded(_hotReload, _template, _controller, _actor);
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::onBecomeActive(u32 _playerIndex)
{
    Super::onBecomeActive(_playerIndex);

    // safety check
    ITF_ASSERT(_playerIndex != GAMEMANAGER->getMainIndexPlayer());

    // init input listener
    if(!m_listenerRegistered)
    {
        m_listenerRegistered = btrue;
        GAMEMANAGER->addGamePlayInputListener(this);
    }

    // get lead
    ITF_ASSERT(!m_leadRef.isValid());
    findLead();
    ITF_ASSERT(m_leadRef.isValid());
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::onPlayerIndexChange(u32 _newIndex)
{
    // get lead ability
    Ray_WM_LeadAbility* leadAbility = getLeadAbility(m_leadRef);
    ITF_WARNING(m_actor, leadAbility, "Can't get lead ability");

    // signal change to our lead
    if (leadAbility)
    {
        leadAbility->signalFollowing(m_actor, m_playerIndex, bfalse);
        leadAbility->signalFollowing(m_actor, _newIndex, btrue);
    }

    Super::onPlayerIndexChange(_newIndex);
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::onBecomeInactive()
{
    Super::onBecomeInactive();

    // reset input listener
    if(m_listenerRegistered)
    {
        m_listenerRegistered = bfalse;
        m_listenerLeave = bfalse;
        GAMEMANAGER->removeInputListener(this);
    }

    // reset on switch to world map (RO-14231)
    m_leadRef.invalidate();
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::update(f32 _dt)
{
    Super::update(_dt);

    applyInput();
    updateMove(_dt);

    // clear input listener
    m_listenerLeave = bfalse;
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::Receive(u32 _deviceID /* player */, f32 _axis, const StringID& _action)
{
    if (GAMEMANAGER->getMainIndexPlayer() == _deviceID || GAMEMANAGER->isInPause())
    {
        return; 
    }

    if ( _deviceID != m_playerIndex )
    {
        return; 
    }

    if (_action == ITF_GET_STRINGID_CRC(LEAVE,982417778))
        m_listenerLeave = _axis > 0.f;
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::applyInput()
{
    // leave
    if (m_listenerLeave)
    {
        Player* player = GAMEMANAGER->getPlayer(m_playerIndex);
        ITF_ASSERT(player && player->getActive());
        GAMEMANAGER->changePlayerActivation(m_playerIndex,bfalse);

        // signal no longer following
        Ray_WM_LeadAbility* leadAbility = getLeadAbility(m_leadRef);
        ITF_WARNING(m_actor, leadAbility, "Can't get lead ability");
        if (leadAbility)
        {
            leadAbility->signalFollowing(m_actor, m_playerIndex, bfalse);
        }
    }
}

//------------------------------------------------------------------------------
Ray_WM_LeadAbility* Ray_WM_FollowAbility::getLeadAbility(const ActorRef& _ref)
{
    // get lead actor
    Actor* lead = _ref.getActor();
    ITF_WARNING(m_actor, lead, "Can't find lead actor");
    if (!lead)
    {
        return NULL;
    }

    // get controller component
    Ray_WM_ControllerComponent* controller = lead->GetComponent<Ray_WM_ControllerComponent>();
    ITF_WARNING(m_actor, controller, "Can't get controller component");
    if (!controller)
    {
        return NULL;
    }

    // return its lead ability
    return &controller->getLeadAbility();
}

//------------------------------------------------------------------------------
bbool Ray_WM_FollowAbility::findLead()
{
    // find lead player
    u32 mainPlayerIndex = GAMEMANAGER->getMainIndexPlayer();
    Player* leadPlayer = GAMEMANAGER->getPlayer(mainPlayerIndex);
    ITF_WARNING(m_actor, leadPlayer, "Can't find lead player");
    if (!leadPlayer)
    {
        return bfalse;
    }

    // find lead ability
    ActorRef leadRef = leadPlayer->getActorForMode(RAY_GAMEMODE_WORLDMAP);
    Ray_WM_LeadAbility* leadAbility = getLeadAbility(leadRef);
    ITF_WARNING(m_actor, leadAbility, "Can't find lead ability");
    if (!leadAbility)
    {
        return bfalse;
    }

    // store ref + signal following
    m_leadRef = leadRef;
    leadAbility->signalFollowing(m_actor, m_playerIndex, btrue);
    return btrue;
}

//------------------------------------------------------------------------------
void Ray_WM_FollowAbility::updateMove(f32 _dt)
{
    // guard against lead deletion, hotreload...
    Ray_WM_LeadAbility* leadAbility = getLeadAbility(m_leadRef);
    ITF_WARNING(m_actor, leadAbility, "Can't get lead ability");
    if (!leadAbility)
    {
        m_leadRef.invalidate();
        return;
    }

    // update position and anim given by our lead
    const Ray_WM_MoveRecord& moveRecord = leadAbility->getMoveRecord(m_playerIndex);
    if (moveRecord.m_valid)
    {
        m_actor->setPos(moveRecord.m_pos);
        m_controller->changeState(moveRecord.m_state);
        m_controller->m_isSprinting = moveRecord.m_isSprinting;
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_FollowAbility_Template)
BEGIN_SERIALIZATION_CHILD(Ray_WM_FollowAbility_Template)
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_WM_FollowAbility_Template::Ray_WM_FollowAbility_Template()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_WM_FollowAbility_Template::~Ray_WM_FollowAbility_Template()
{
}

} // namespace ITF
