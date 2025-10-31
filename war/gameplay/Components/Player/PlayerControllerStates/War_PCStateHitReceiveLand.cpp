#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHitReceiveLand)

////////////////////
// Receive Hit Land State
////////////////////
void War_PlayerControllerComponent::StateHitReceiveLand::onEnter()
{
    Super::onEnter();

    m_prevFrictionMultiplier = m_characterPhys->getFrictionMultiplier();
    m_characterPhys->setFrictionMultiplier(m_parent->m_receiveHitFrictionMultiplier);
}

void War_PlayerControllerComponent::StateHitReceiveLand::onExit()
{
    m_characterPhys->setFrictionMultiplier(m_prevFrictionMultiplier);
}

bbool War_PlayerControllerComponent::StateHitReceiveLand::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( m_characterPhys->getSpeed().sqrnorm() < 5.f*5.f )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

}