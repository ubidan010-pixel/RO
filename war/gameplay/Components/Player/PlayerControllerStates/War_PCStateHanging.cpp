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

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHanging)

////////////////////
// Hanging State
////////////////////
void War_PlayerControllerComponent::StateHanging::onEnter()
{
    Super::onEnter();

    if ( m_parent->m_hangingEdge == -1 )
    {
        m_parent->resetUTurn();
        m_parent->setStance(STANCE_HANG);
        m_parent->hangFromPoint(m_hangInfo);
        m_climbTimer = m_parent->m_climbTimerThreshold;
        m_canMoveAgain = bfalse;
        m_justEntered = btrue;
    }
}

void War_PlayerControllerComponent::StateHanging::onExit()
{
    if ( m_parent->m_nextState != &m_parent->m_statePrepareHit &&
         m_parent->m_nextState != &m_parent->m_stateHangToIdle &&
         m_parent->m_nextState != &m_parent->m_stateHangToJump )
    {
        m_parent->clearHanging(m_characterPhys,m_animComponent);
    }
}

bbool War_PlayerControllerComponent::StateHanging::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( m_canMoveAgain )
    {
        if ( m_parent->isHitQueued() )
        {
            m_parent->changeState(&m_parent->m_statePrepareHit);
            return btrue;
        }

        if ( m_parent->m_moveDirection == MOVEDIR_DOWN )
        {
            m_parent->m_disableCrouchTimer = m_parent->m_disableCrouchTime;
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }

        //JUMP if JUMP button pressed, we didn't just hang and we are within the timer
        if ( m_parent->isJumpQueued() )
        {
            if ( m_parent->m_hangingSolid )
            {
                m_parent->changeState(&m_parent->m_stateHangToJump);
            }
            else
            {
                m_parent->m_jumpForceMultiplier = m_parent->m_jumpForceFromHangMultiplier;
                m_parent->changeState(&m_parent->m_stateJumping);
                m_parent->m_queueJump = bfalse;
            }
            return btrue;
        }

        //CLIMB if we are pressing in the direction of the wall and we exceeded the timer,
        //OR we released the stick after having pressed in the proper direction
        if ( m_parent->m_moveDirection == MOVEDIR_UP ||
             ( m_parent->m_moveDirection == MOVEDIR_RIGHT && m_parent->getIsLookingRight() ) ||
             ( m_parent->m_moveDirection == MOVEDIR_LEFT && !m_parent->getIsLookingRight() ) )
        {
            if (m_climbTimer == 0.0f)
            {
                m_parent->changeState(&m_parent->m_stateHangToIdle);
                return btrue;
            }
        }
    }

    if ( m_parent->isCurrentHangColliding() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( !m_parent->isCurrentHangDirValid() )
    {
        m_parent->changeState(&m_parent->m_stateHangToStand);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHanging::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    Vec2d moveInput = m_parent->m_moveInput;

    if ( !m_justEntered && !m_canMoveAgain && !m_animComponent->isInTransition() )
    {
        m_canMoveAgain = btrue;
    }

    if (fabsf(m_parent->getCurrentMove().m_x) > 0.5f)
    {
        bbool goingRight =  m_parent->m_moveDirection == MOVEDIR_RIGHT;

        if ( goingRight == m_parent->getIsLookingRight() )
        {
            //always face obstacle unless specifically pressing stick against it
            moveInput.m_x = fabs(m_parent->getCurrentMove().m_x);
            m_climbTimer = Max(m_climbTimer-_dt,0.f);        
        }
        else
        {
            //always face obstacle unless specifically pressing stick against it
            moveInput.m_x = -fabs(m_parent->getCurrentMove().m_x);
            m_climbTimer = m_parent->m_climbTimerThreshold;
        }
    }
    else
    {
        //always face obstacle unless specifically pressing stick against it
        moveInput.m_x = fabs(m_parent->getCurrentMove().m_x);
        //override facing
        m_climbTimer = 0.0f;
    }

    m_parent->setMoveInput(moveInput);
    m_parent->UpdatePhysicHanging(m_characterPhys);
    m_justEntered = bfalse;
}

void War_PlayerControllerComponent::StateHanging::onEvent(Event * _event)
{
    if (_event->IsClass(AnimGameplayEvent::GetClassNameStatic()))
    {
        AnimGameplayEvent * animEvent = (AnimGameplayEvent *)_event;

        if (animEvent->getName() == War_MRK_Allow_Move)
        {
            m_canMoveAgain = btrue;
        }
    }
}

void War_PlayerControllerComponent::StateHanging::updateAnimInputs()
{
    Super::updateAnimInputs();

    static const StringID s_SolidHangin = "SolidHanging";

    m_animComponent->setInput(s_SolidHangin,static_cast<u32>(m_parent->m_hangingSolid));
}

}