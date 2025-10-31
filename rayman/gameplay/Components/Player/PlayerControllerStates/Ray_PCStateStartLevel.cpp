#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateStartLevel)

////////////////////
// Hanging To Idle State
////////////////////
void Ray_PlayerControllerComponent::StateStartLevel::onEnter()
{
    Super::onEnter();

    m_finished = bfalse;
    m_allowMove = bfalse;
}

void Ray_PlayerControllerComponent::StateStartLevel::onExit()
{
    Super::onExit();

}

bbool Ray_PlayerControllerComponent::StateStartLevel::checkStateChange( f32 _dt )
{
    if ( m_finished || ( m_allowMove && !m_parent->playerIsIdle() ) )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateStartLevel::update(f32 _dt)
{
}

void Ray_PlayerControllerComponent::StateStartLevel::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == MRK_Finish_Anim)
        {
            m_finished = btrue;
        }
        else if ( animEvent->getName() == MRK_Allow_Move )
        {
            m_allowMove = btrue;
        }
    }
}

}