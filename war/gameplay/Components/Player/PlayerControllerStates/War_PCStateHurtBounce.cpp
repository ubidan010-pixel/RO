#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHurtBounce)

////////////////////
// Bounce State
////////////////////
void War_PlayerControllerComponent::StateHurtBounce::onEnter()
{
    Super::onEnter();

#pragma message("FIX THIS")
//    GAMEMANAGER->getPlayer(m_parent->m_playerIndex)->addHitPoints(-m_parent->m_hurtBounceLevel);
}

bbool War_PlayerControllerComponent::StateHurtBounce::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    HangingInfo info;
    if ( m_parent->tryHanging(m_characterPhys,info) )
    {
        m_parent->setHangState(info);
        return btrue;
    }

    if ( !m_parent->m_hurtBounceLevel &&
          m_characterPhys->getStickedPolyline() )
    {  
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    Actor* victim;

    if ( m_parent->checkFallOnCharacter(m_characterPhys,&victim) )
    {
        EventCrushed crushEvent(m_actor->getRef());
        victim->onEvent(&crushEvent);

        m_parent->m_jumpForceMultiplier = m_parent->m_fallOnVictimJumpMultiplier;
        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHurtBounce::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    m_parent->UpdatePhysicAir(m_characterPhys);
}

}