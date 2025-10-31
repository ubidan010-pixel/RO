#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateIdle)

////////////////////
// Idle State
////////////////////
War_PlayerControllerComponent::StateIdle::StateIdle()
: m_onCorner(CORNERPOS_NONE)
, m_justEntered(bfalse)
, m_allowUnhang(bfalse)
, m_allowCrouch(bfalse)
, m_isPathBlocked(bfalse)
{
}

void War_PlayerControllerComponent::StateIdle::onEnter()
{
    Super::onEnter();

    m_justEntered = btrue;
    m_allowUnhang = bfalse;
    m_isPathBlocked = bfalse;
    m_allowCrouch = m_parent->m_disableCrouchTimer == 0.f;
    // If we were crouched already we allow the crouch, otherwise we need to release the stick
    m_onCorner = CORNERPOS_NONE;
    m_parent->resetStance(STANCE_STAND);
    m_parent->setRolling(bfalse);
    m_parent->setDesiredLookDir();
}

void War_PlayerControllerComponent::StateIdle::onExit()
{
    m_justEntered = bfalse;
    m_parent->m_targetMoveEfficiency = 1.f;
}

bbool War_PlayerControllerComponent::StateIdle::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if (checkBounce())
    {
        return btrue;
    }

    if  ( m_parent->isHitQueued() )
    {
        m_parent->setIdlePunch(m_characterPhys);
        return btrue;
    }

    if ( !m_justEntered && m_parent->isJumpQueued() )
    {
        if ( m_parent->isRolling() )
        {
            m_parent->m_jumpForceMultiplier = m_parent->m_rollingJumpMultiplier;
        }
        else if ( m_parent->isCrouching() )
        {
            m_parent->m_jumpForceMultiplier = m_parent->m_crouchJumpMultiplier;
        }
        else if (m_parent->m_hasChains)
        {
            m_parent->m_jumpForceMultiplier = m_parent->m_jumpMultiplierWithChains;
        }
        else
        {
            m_parent->m_jumpForceMultiplier = 1.f;
        }

        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    if ( _axes[m_joyTrigger_Left] != 0.0f && m_characterPhys->getStickedPolyline())
    {
        m_parent->changeState(&m_parent->m_stateAim);
        return btrue;
    }

    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

    if ( !edge )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_parent->isCrouching() && m_allowUnhang && m_parent->m_moveDirection == MOVEDIR_DOWN )
    {
        PolyLine* poly = m_characterPhys->getStickedPolyline();

        if ( m_parent->tryHangFromPolyline(poly, edge, m_characterPhys->getStickedEdgeIndex()) )
        {
            return btrue;
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateIdle::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    f32 speedOnGround = m_characterPhys->getCurrentGroundDir().dot(m_characterPhys->getSpeed());
    
    bbool sprinting = _axes[m_joyTrigger_Right] != 0.f;
    m_parent->setSprinting(sprinting);

    m_parent->UpdatePhysicWalk(m_characterPhys,_dt);
    updateSpecialMoves(speedOnGround);
    m_justEntered = bfalse;
    updateOnCorner();
    m_parent->checkLowRoof(m_characterPhys);
    updateAllowFlags();
    updatePathBlocked();

}

void War_PlayerControllerComponent::StateIdle::updateSpecialMoves( f32 _speedOnGround )
{
    if ( m_parent->isRolling() &&
         isCurrentAnimFinished() )
    {
        m_parent->setRolling(bfalse);
    }

    if ( _speedOnGround )
    {
        if ( m_parent->isRolling() )
        {
            if ( m_parent->m_moveDirection != MOVEDIR_DOWN &&
                 m_parent->m_moveDirection != MOVEDIR_NONE )
            {
                m_parent->setRolling(bfalse);
            }
        }
        else
        {
            if ( m_parent->m_moveDirection == MOVEDIR_DOWN )
            {
                if ( fabs(_speedOnGround) >= m_parent->m_minSpeedForSlide )
                {
                    if ( m_justEntered )
                    {
                        m_parent->setRolling(btrue);
                    }
                }
                else if ( m_allowCrouch )
                {
                    m_parent->setCrouching(btrue);
                }
            }
            else if ( m_parent->m_moveDirection != MOVEDIR_NONE )
            {
                if ( m_parent->isCrouching() && !m_parent->isPerformingUTurn() )
                {
                    m_parent->setCrouching(bfalse);
                }
            }
        }
    }
    else
    {
        if ( m_parent->isRolling() )
        {
            m_parent->setRolling(bfalse);
        }

        if ( m_parent->isCrouching() )
        {
            if ( m_parent->m_moveDirection == MOVEDIR_UP )
            {
                m_parent->setCrouching(bfalse);
            }
        }
        else
        {
            if ( m_parent->m_moveDirection == MOVEDIR_DOWN && m_allowCrouch )
            {
                m_parent->setCrouching(btrue);
            }
        }
    }

    if ( m_parent->isRolling() )
    {
        // if we are still rolling we update our look direction
        f32 xSpeed = m_characterPhys->getCurrentGroundDir().dot(m_characterPhys->getSpeed());

        if ( xSpeed > 0.f )
        {
            m_parent->setLookRight(btrue);
        }
        else
        {
            m_parent->setLookRight(bfalse);
        }
    }
}

void War_PlayerControllerComponent::StateIdle::updateOnCorner()
{
    m_onCorner = CORNERPOS_NONE;

    const PolyLine* polyLine = m_characterPhys->getStickedPolyline();
    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

    if ( !edge || !polyLine )
    {
        return;
    }

    f32 relDist = m_characterPhys->getRelativeEdgeDistance();
    u32 edgeIndex = m_characterPhys->getStickedEdgeIndex();
    static const f32 edgeLimit = 0.8f;

    if ( relDist <= 0.f )
    {
        bbool onCorner = bfalse;

        if ( edgeIndex == 0 )
        {
            onCorner = btrue;
        }
        else
        {
            const PolyLineEdge& prevEdge = polyLine->getEdgeAt(edgeIndex-1);

            f32 dotProd = (-prevEdge.m_normalizedVector).dot(m_parent->m_currentGravityDir);

            if ( dotProd > edgeLimit )
            {
                onCorner = btrue;
            }
        }

        if ( onCorner )
        {
            if ( m_parent->getIsLookingRight() )
            {
                m_onCorner = CORNERPOS_BACK;
            }
            else
            {
                m_onCorner = CORNERPOS_FRONT;
            }
        }
    }
    else if ( relDist >= edge->m_length )
    {
        bbool onCorner = bfalse;

        if ( edgeIndex >= (polyLine->getVectorsCount()-1) )
        {
            onCorner = btrue;
        }
        else
        {
            const PolyLineEdge& prevEdge = polyLine->getEdgeAt(edgeIndex+1);

            f32 dotProd = prevEdge.m_normalizedVector.dot(m_parent->m_currentGravityDir);

            if ( dotProd > edgeLimit )
            {
                onCorner = btrue;
            }
        }

        if ( onCorner )
        {
            if ( m_parent->getIsLookingRight() )
            {
                m_onCorner = CORNERPOS_FRONT;
            }
            else
            {
                m_onCorner = CORNERPOS_BACK;
            }
        }
    }

    if ( m_onCorner != CORNERPOS_NONE &&
         m_parent->isPerformingUTurn() )
    {
        if ( m_onCorner == CORNERPOS_FRONT )
        {
            m_onCorner = CORNERPOS_BACK;
        }
        else
        {
            m_onCorner = CORNERPOS_FRONT;
        }
    }
}

void War_PlayerControllerComponent::StateIdle::updateAllowFlags()
{
    if ( m_parent->isCrouching() )
    {
        if ( !m_allowUnhang )
        {
            if ( m_parent->m_moveDirection == MOVEDIR_NONE )
            {
                m_allowUnhang = btrue;
            }
        }
    }
    else
    {
        m_allowUnhang = bfalse;
    }

    if ( !m_allowCrouch &&
          m_parent->m_moveDirection != MOVEDIR_DOWN )
    {
        m_allowCrouch = btrue;
    }
}

void War_PlayerControllerComponent::StateIdle::updatePathBlocked()
{
    m_isPathBlocked = bfalse;

    if ( m_parent->m_moveDirection == MOVEDIR_LEFT ||
         m_parent->m_moveDirection == MOVEDIR_RIGHT )
    {
        // If we are trying to move
        if ( m_characterPhys->getSpeed() == Vec2d::Zero )
        {
            // But we have no speed
            const PhysContactsContainer& contacts = m_characterPhys->getContacts();

            for ( u32 index = 0; index < contacts.size(); index++ )
            {
                const SCollidableContact& contact = contacts[index];

                if ( contact.m_edgeIndex == U32_INVALID )
                {
                    continue;
                }

                Vec2d contactLocal = contact.m_contactPoint - m_actor->get2DPos();

                contactLocal = contactLocal.Rotate(-m_actor->getAngle());

                if ( fabs(contactLocal.m_y) >= 0.1f )
                {
                    continue;
                }

                if ( m_parent->m_moveDirection == MOVEDIR_LEFT &&
                     contactLocal.m_x > 0.f )
                {
                    continue;
                }
                else if ( m_parent->m_moveDirection == MOVEDIR_RIGHT &&
                          contactLocal.m_x < 0.f )
                {
                    continue;
                }

                m_isPathBlocked = btrue;
                break;
            }
        }
    }
}

void War_PlayerControllerComponent::StateIdle::updateAnimInputs()
{
    Super::updateAnimInputs();

    static const StringID s_OnCorner = "OnCorner";
    static const StringID s_IsPathBlocked = "IsPathBlocked";
    static const StringID s_IsCrouched = "IsCrouched";
    static const StringID s_IsRolling = "IsRolling";
    static const StringID s_IsSprinting = "IsSprinting";
    static const StringID s_IsLowRoof = "IsLowRoof";

    m_animComponent->setInput(s_OnCorner,static_cast<u32>(m_onCorner));
    m_animComponent->setInput(s_IsPathBlocked,static_cast<u32>(m_isPathBlocked));
    m_animComponent->setInput(s_IsCrouched,static_cast<u32>(m_parent->isCrouching()));
    m_animComponent->setInput(s_IsRolling,static_cast<u32>(m_parent->isRolling()));
    m_animComponent->setInput(s_IsSprinting,static_cast<u32>(m_parent->isSprinting()));
    m_animComponent->setInput(s_IsLowRoof,static_cast<u32>(m_parent->isLowRoof()));
}

}
