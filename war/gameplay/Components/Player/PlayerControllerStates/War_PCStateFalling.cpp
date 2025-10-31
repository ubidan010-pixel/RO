#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateFalling)

////////////////////
// Fall State
////////////////////
void War_PlayerControllerComponent::StateFalling::onEnter()
{
    Super::onEnter();

    if ( m_parent->m_prevState == &m_parent->m_stateIdle )
    {
        m_parent->m_jumpOnAirTimer = m_parent->m_jumpOnAirTime;
    }

    m_parent->m_jumpQueueTimer = 0.0f;
    m_parent->setStance(STANCE_STAND);
    m_parent->resetUTurn();
    m_parent->resetAllowEarthquakePunch();
    m_justEntered = btrue;
}

void War_PlayerControllerComponent::StateFalling::onExit()
{
    Super::onExit();

    // Reset the jump queue. For fall and jump we only use the jump queue timer
    m_parent->m_queueJump = bfalse;

    if ( m_parent->m_nextState == &m_parent->m_stateIdle )
    {
        m_parent->setCrouching(bfalse); //#142, no crouch when jumping
    }
}

bbool War_PlayerControllerComponent::StateFalling::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    if  ( m_parent->isHitQueued() )
    {
        m_parent->setAirPunch();
        return btrue;
    }

    if ( !m_justEntered &&
          m_parent->isJumpQueued() &&
          m_parent->m_jumpOnAirTimer )
    {
        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    // HACK: aiming here
    //if ( _axes[m_joyTrigger_Left] != 0.0f)
    //{
    //    m_parent->changeState(&m_parent->m_stateHelicopter);
    //    return btrue;
    //}

    HangingInfo info;

    if ( m_parent->tryHanging(m_characterPhys,info) )
    {
        m_parent->setHangState(info);
        return btrue;
    }

    if ( m_characterPhys->getStickedPolyline())
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

void War_PlayerControllerComponent::StateFalling::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    // If we pressed jump while falling we use the jump timer in case we land soon
    if ( !m_justEntered && m_parent->m_queueJump )
    {
        m_parent->m_jumpQueueTimer = m_parent->m_jumpQueueTime;
        m_parent->m_queueJump = bfalse; // Reset the queue since we only use the timer
    }

    m_parent->UpdatePhysicAir(m_characterPhys);
    m_justEntered = bfalse;

    if ( m_parent->m_freeFallCurveRange )
    {
        f32 speedY = m_parent->m_currentGravityDir.dot(m_characterPhys->getSpeed());
        f32 minSpeed = m_parent->m_freeFallCurveRange*0.5f;

        m_animCursor = ( speedY + minSpeed ) / m_parent->m_freeFallCurveRange;
        m_animCursor = Clamp(m_animCursor,0.f,1.f);
    }

    m_parent->updateAllowEarthquakePunch();
}

void War_PlayerControllerComponent::StateFalling::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setProceduralCursor(m_animCursor);
}

}