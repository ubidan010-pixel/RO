#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHangToJump)

const f32 War_PlayerControllerComponent::StateHangToJump::s_blendTime = 0.02f;

////////////////////
// Hanging To Idle State
////////////////////
void War_PlayerControllerComponent::StateHangToJump::onEnter()
{
    Super::onEnter();

    m_timeCounter = 0.f;
    m_startAngle = m_actor->getAngle();
    m_targetAngle = getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular());
    m_angleDif = getShortestAngleDelta(m_startAngle,m_targetAngle);
}

void War_PlayerControllerComponent::StateHangToJump::onExit()
{
    Super::onExit();

    m_parent->m_queueJump = bfalse;
    m_parent->m_jumpForceMultiplier = m_parent->m_jumpForceFromHangMultiplier;
    m_parent->clearHanging(m_characterPhys,m_animComponent);
}

bbool War_PlayerControllerComponent::StateHangToJump::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( m_timeCounter >= s_blendTime )
    {
        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHangToJump::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    m_parent->UpdatePhysicHanging(m_characterPhys);

    m_timeCounter = Min(m_timeCounter+_dt,s_blendTime);

    f32 t = m_timeCounter / s_blendTime;

    f32 newAngle = m_startAngle + (m_angleDif*t);

    m_actor->setAngle(newAngle);
}

}