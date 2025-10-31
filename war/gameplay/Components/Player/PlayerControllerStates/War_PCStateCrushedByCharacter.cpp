#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateCrushedByCharacter)

////////////////////
// Idle to Climb State
////////////////////
bbool War_PlayerControllerComponent::StateCrushedByCharacter::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( isCurrentAnimFinished() )
    {
        if ( m_characterPhys->getStickedEdge() )
        {
            m_parent->changeState(&m_parent->m_stateIdle);
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateFalling);
        }

        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateCrushedByCharacter::restart()
{
    m_animComponent->resetCurTime();
}

}