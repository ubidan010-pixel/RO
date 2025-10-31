#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHitPrepare)

////////////////////
// Prepare Hit State
////////////////////
void War_PlayerControllerComponent::StateHitPrepare::onEnter()
{
    Super::onEnter();

    m_justEntered = btrue;
    m_parent->m_punchLevel = -1;
    m_parent->setCurrentPunch(WAR_PUNCHTYPE_NORMAL,0);
    m_prevPunchLevel = 0;
    m_uturnTimer = m_parent->m_uTurnHitAllowTime;
    m_parent->m_queueHit = bfalse; // Reset the queue since we pressed hit to get here
}

void War_PlayerControllerComponent::StateHitPrepare::onExit()
{
    Super::onExit();

    m_uturnTimer = 0.f;
}

bbool War_PlayerControllerComponent::StateHitPrepare::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    if ( !m_justEntered && _buttons[m_joyButton_X]==InputAdapter::Released )
    {
        if ( !m_parent->isHanging() && !m_parent->isClimbing() )
        {
            m_parent->m_stateReleaseHit.setUTurnTimer(m_uturnTimer);
        }

        m_parent->changeState(&m_parent->m_stateReleaseHit);
        return btrue;
    }

    if ( !m_parent->isHanging() && !m_parent->isClimbing() )
    {
        if ( m_uturnTimer &&
           ( ( m_parent->m_moveDirection == MOVEDIR_LEFT &&
               m_parent->getIsLookingRight() ) ||
             ( m_parent->m_moveDirection == MOVEDIR_RIGHT &&
              !m_parent->getIsLookingRight() ) ) )
        {
            m_parent->setCurrentPunch(WAR_PUNCHTYPE_UTURNKICK,m_parent->m_uTurnHitLevel);
            m_parent->startUTurn();
            m_parent->changeState(&m_parent->m_stateReleaseHit);
            return btrue;
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHitPrepare::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    Super::update(_dt,_buttons,_axes);

    if ( isCurrentAnimFinished() &&
        !m_animComponent->isAnimLooped() &&
        !m_animComponent->isInTransition() )
    {
        m_parent->m_currentPunchHitLevel++;
    }

    if ( m_parent->m_currentPunchHitLevel != m_prevPunchLevel )
    {
        m_prevPunchLevel = m_parent->m_currentPunchHitLevel;
    }

    m_justEntered = bfalse;
    m_uturnTimer = Max(m_uturnTimer-_dt,0.f);
}

}
