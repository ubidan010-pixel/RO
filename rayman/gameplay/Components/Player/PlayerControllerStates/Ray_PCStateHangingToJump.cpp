#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHangToJump)

const f32 Ray_PlayerControllerComponent::StateHangToJump::s_blendTime = 0.1f;

////////////////////
// Hanging To Idle State
////////////////////
void Ray_PlayerControllerComponent::StateHangToJump::onEnter()
{
    Super::onEnter();

    m_timeCounter = 0.f;
}

bbool Ray_PlayerControllerComponent::StateHangToJump::checkStateChange( f32 _dt )
{
    if ( m_timeCounter >= s_blendTime )
    {
        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHangToJump::update(f32 _dt)
{
    Vec2d pos;
    f32 angle;

    m_parent->getWorldHangPos(pos,angle);
    m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));

    m_timeCounter = Min(m_timeCounter+_dt,s_blendTime);

    f32 t = m_timeCounter / s_blendTime;

    f32 targetAngle = getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular());
    f32 dif = getShortestAngleDelta(angle,targetAngle);

    m_actor->setAngle(angle + (dif*t));
}

void Ray_PlayerControllerComponent::StateHangToJump::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_HANGTOJUMPCURSOR],m_timeCounter/s_blendTime);
}

}