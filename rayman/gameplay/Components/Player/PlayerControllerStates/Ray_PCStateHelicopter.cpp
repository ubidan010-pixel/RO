#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHelicopter)

////////////////////
// Helicopter State
////////////////////
void Ray_PlayerControllerComponent::StateHelicopter::onEnter()
{
    Super::onEnter();

    m_justEntered = btrue;
    m_exit = bfalse;
    m_exitCounter = 0.f;
    m_parent->cancelPhysicAirHit();
    m_parent->m_airControlDisableDelay = 0.f;

    if ( m_parent->m_stance != STANCE_HELICO )
    {
        m_parent->setStance(STANCE_HELICO);
    }

    REWARD_MANAGER->Action_TimerResume(Ray_RewardManager::Timer_DoingHelico, m_parent->m_playerIndex, btrue);   
}

void Ray_PlayerControllerComponent::StateHelicopter::onExit()
{
    Super::onExit();

    REWARD_MANAGER->Action_TimerPause(Ray_RewardManager::Timer_DoingHelico, m_parent->m_playerIndex, btrue);
}

bbool Ray_PlayerControllerComponent::StateHelicopter::checkStateChange( f32 _dt )
{
    if (checkBounce())
    {
        return btrue;
    }

    if ( m_parent->isOnPedestal() )
    {
        m_parent->changeState(&m_parent->m_stateOnPedestal);
        return btrue;
    }

    if ( !m_justEntered )
    {
        if ( m_parent->isJumpQueued() )
        {
            if ( m_parent->tryWallJump(bfalse) )
            {
                return btrue;
            }
        }

        if ( m_exit && !m_exitCounter )
        {
            m_parent->m_helicopterCooldown = m_parent->getTemplate()->getHelicopterCooldown();
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }

    if ( m_parent->isHitQueued() )
    {
        if ( m_parent->setAirPunch() )
        {
            return btrue;
        }
    }

    if (m_characterPhys->getStickedPolyline())
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    HangingInfo info;
    if ( m_parent->tryHanging(info) )
    {
        m_parent->setHangState(info);
        return btrue;
    }

    if ( m_parent->checkFallOnCharacter() )
    {
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHelicopter::update(f32 _dt)
{
    m_parent->UpdatePhysicHairlicopter(_dt);

    if ( !m_parent->isHoldingHelicopterAction() )
    {
        if ( m_exit )
        {
            m_exitCounter = Max(0.f,m_exitCounter-_dt);
        }
        else
        {
            m_exit = btrue;
            m_exitCounter = m_parent->getTemplate()->getHelicopterReleaseDelay();
        }
    }
    else
    {
        m_exit = bfalse;
    }

    m_justEntered = bfalse;
}

}