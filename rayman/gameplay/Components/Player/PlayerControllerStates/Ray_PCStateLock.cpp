#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateLock)

////////////////////
// Idle State
////////////////////
Ray_PlayerControllerComponent::StateLock::StateLock()
: m_prevState(NULL)
, m_clearLockOnExit(bfalse)
, m_exit(bfalse)
{
}

void Ray_PlayerControllerComponent::StateLock::onEnter()
{
    Super::onEnter();

    if (!m_parent->isActorLocked())
    {
        m_parent->actorLock();
        m_clearLockOnExit = btrue;
    }
    else
    {
        m_clearLockOnExit = bfalse;
    }

    m_exit = bfalse;
    m_prevState = m_parent->m_prevState;
    ITF_ASSERT(m_prevState!=this);
}

void Ray_PlayerControllerComponent::StateLock::onExit()
{
    if (m_clearLockOnExit)
    {
        m_parent->clearActorLock();
    }

    m_parent->clearHanging();
    m_characterPhys->forceFindStickEdge();
}

bbool Ray_PlayerControllerComponent::StateLock::checkStateChange( f32 _dt )
{
    if ( m_exit )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateLock::update(f32 _dt)
{
    m_parent->updateActorLock();
}

void Ray_PlayerControllerComponent::StateLock::onEvent(Event * _event)
{
    if (Ray_EventPlayLockAnim * lock = _event->DynamicCast<Ray_EventPlayLockAnim>(ITF_GET_STRINGID_CRC(Ray_EventPlayLockAnim,3679647184)))
    {
        if ( lock->getActivate() )
        {
            m_animComponent->resetCurTime();
            m_exit = bfalse;
        }
        else
        {
            m_exit = btrue;
        }
    }
}

void Ray_PlayerControllerComponent::StateLock::setData( bbool _lookRight )
{
    m_parent->setLookRight(_lookRight);
    m_parent->m_desiredLookDir = _lookRight?MOVEDIR_RIGHT:MOVEDIR_LEFT;
}

}
