#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_WAR_ANIMATIONMARKERS_H_
#include "war/gameplay/War_AnimationMarkers.h"
#endif //_ITF_WAR_ANIMATIONMARKERS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateIdleToClimb)

////////////////////
// Idle to Climb State
////////////////////
const f32 War_PlayerControllerComponent::StateIdleToClimb::s_blendTime = 0.1f;

void War_PlayerControllerComponent::StateIdleToClimb::onEnter()
{
    Super::onEnter();

    m_parent->setStance(STANCE_CLIMB);
    m_parent->m_queueJump = bfalse; // Since we press jump to get here, we reset the queue
    m_parent->m_stateClimbing.setAllowExitHorizontal(bfalse);
    m_canMove = bfalse;
    m_blendCounter = s_blendTime;
    m_startRot = m_actor->getAngle();
}

bbool War_PlayerControllerComponent::StateIdleToClimb::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( ( m_canMove && !m_parent->playerIsIdle(_buttons,_axes) ) || isCurrentAnimFinished() )
    {
        // Special case: if he didn't release the DOWN direction we don't consider it as non-idle
        if ( m_parent->m_stateClimbing.getAllowExitHorizontal() ) // He is not pressing down anymore, so it's ok to exit
        {
            m_parent->changeState(&m_parent->m_stateClimbing);
            return btrue;
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateIdleToClimb::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    m_parent->UpdatePhysicClimbingEdge(_dt,m_animComponent,m_characterPhys);

    if ( !m_parent->m_stateClimbing.getAllowExitHorizontal() && m_parent->m_moveDirection != MOVEDIR_DOWN )
    {
        m_parent->m_stateClimbing.setAllowExitHorizontal(btrue);
    }

    if ( m_blendCounter )
    {
        m_blendCounter = Max(m_blendCounter-_dt,0.f);

        f32 endRot = getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular());
        f32 rotDif = getShortestAngleDelta(m_startRot,endRot);
        f32 dif = rotDif * ( 1.f - ( m_blendCounter / s_blendTime ) );

        m_actor->setAngle(m_startRot+dif);
    }
}

void War_PlayerControllerComponent::StateIdleToClimb::onEvent(Event * _event)
{
    if (_event->IsClass(AnimGameplayEvent::GetClassNameStatic()))
    {
        AnimGameplayEvent * animEvent = (AnimGameplayEvent *)_event;

        if (animEvent->getName() == War_MRK_Allow_Move)
        {
            m_canMove = btrue;
        }
    }
}

}