#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateJump)

    ////////////////////
    // Jumping State
    ////////////////////
    Ray_PlayerControllerComponent::StateJump::StateJump()
    : m_airPose(AIRPOSE_MIDDLE)
    , m_suspensionDelay(0.f)
    , m_suspensionPushCounter(0.f)
    , m_queueCrushCounter(0.f)
    , m_wallRunJumpCounter(0.f)
    , m_justJumped(btrue)
    , m_skipForce(bfalse)
    , m_allowSuspension(bfalse)
    , m_jumpReleased(bfalse)
    , m_bouncing(bfalse)
{
}

void Ray_PlayerControllerComponent::StateJump::onEnter()
{
    Super::onEnter();

    m_parent->stopOnPedestal();
    m_parent->m_wallSlideAirControl = bfalse;
    m_bouncing = bfalse;
    m_allowSuspension = btrue;
    m_wallRunJumpCounter = 0.f;

    if ( m_parent->m_currentJumpQueueCrushAttack )
    {
        m_queueCrushCounter = m_parent->getTemplate()->getReducedSizeHitCrushAttackQueueLength();
    }
    else
    {
        m_queueCrushCounter = 0.f;
    }

    // For wall slide uturn we do the flip at the end of the jump
    // For regular uturn jump we flip the character right at the first frame without cancelling the
    // uturn
    if ( m_parent->isPerformingUTurn() )
    {
        if ( !m_parent->m_uTurnFlipped &&
              m_parent->m_stance != STANCE_WALLSLIDE &&
              m_parent->m_stance != STANCE_HANG &&
             !m_parent->m_currentJumpUseWallDir )
        {
            m_parent->m_flipActor = btrue;
        }
    }
    else
    {
        m_parent->resetDesiredLookDir();
    }

    m_jumpReleased = bfalse;
    setupJump();

    m_parent->m_nextJumpIsAccrobatic = bfalse;

    m_parent->resetPhysicAirHit();
    m_parent->setStance(STANCE_STAND);
    m_justJumped = btrue;
    m_airPose = AIRPOSE_MIDDLE;
    m_suspensionDelay = m_parent->getTemplate()->getAirSuspensionDelay();
    m_suspensionPushCounter = 0.f;

#ifdef ITF_SUPPORT_ONLINETRACKING
	RAY_GAMEMANAGER->getOnlineTrackingManager()->m_jumpCount++;
#endif // ITF_SUPPORT_ONLINETRACKING
}

void Ray_PlayerControllerComponent::StateJump::onExit()
{
    m_parent->setJumpData();
    m_parent->m_lastJumpTimer = 0.f;
    m_parent->stopJumpFx();

    if ( m_parent->m_nextState == &m_parent->m_stateIdle )
    {
        m_parent->setCrouch(bfalse); //#142, no crouch when jumping
    }

    // Only reset it if we are not doing a uturn kick
    if ( m_parent->m_nextState != &m_parent->m_stateReleaseHit &&
        m_parent->m_nextState != &m_parent->m_statePrepareHit )
    {
        m_parent->resetUTurn();
    }

    if ( m_parent->isSupporting() )
    {
        if ( m_parent->m_nextState != &m_parent->m_stateIdle )
        {
            m_parent->setSupporting(bfalse);
        }
    }

    if ( m_parent->m_nextState == &m_parent->m_stateIdle )
    {
        m_parent->m_unCrouchCounter = m_parent->getTemplate()->getCrouchUnhangTime(); //#1121
    }

    if ( m_parent->m_bounceRepositionCounter )
    {
        m_characterPhys->setGravityMultiplier(1.f);
        m_parent->setMaxSpeedLimit(btrue);
        m_parent->m_bounceRepositionTotal = Vec2d::Zero;
        m_parent->m_bounceRepositionCounter = 0.f;
    }

    m_justJumped = bfalse;
    m_skipForce = bfalse;

    if ( m_wallRunJumpCounter )
    {
        m_parent->m_currentJumpWallRun = bfalse;
        m_characterPhys->setUpdateRotation(btrue);
        m_wallRunJumpCounter = 0.f;
        m_parent->resetGravityMode(GRAVITYMODE_WALLRUNJUMP);
    }
}

void Ray_PlayerControllerComponent::StateJump::restartJump()
{
    m_allowSuspension = btrue;
    m_suspensionDelay = m_parent->getTemplate()->getAirSuspensionDelay();
    setupJump();
}

void Ray_PlayerControllerComponent::StateJump::setupJump()
{
    // Ways to start/continue an accrobatic chain:
    // 1) Uturn jump
    // 2) Crouch jump
    // 3) Be in the chain and jump within a certain time
    // 4) Bouncing on something
    bbool accrobatic = bfalse;

    if ( !m_parent->isSupportHolding() )
    {
        if ( m_parent->m_nextJumpIsAccrobatic )
        {
            accrobatic = btrue;
        }
        else if ( m_parent->isPerformingUTurn() )
        {
            accrobatic = btrue;
        }
        else if ( m_parent->isPerformingSlide() || m_parent->isRolling() )
        {
            accrobatic = btrue;
        }
        else if ( m_parent->m_bounceCurrentDir != Vec2d::Zero )
        {
            accrobatic = btrue;
        }
    }

    if ( accrobatic )
    {
        m_parent->addAccrobaticStage();
    }
    else
    {
        m_parent->resetAccrobaticStage();
    }

    if ( m_parent->isPerformingUTurn() &&
       ( !m_parent->m_uTurnComboCounter || m_parent->isSupportHolding() ) )
    {
        m_parent->resetUTurn();
    }

    m_jumpReleased = bfalse;
    m_justJumped = btrue;
    m_parent->startJumpFx();

    if ( m_parent->m_bounceCurrentDir != Vec2d::Zero )
    {
        m_allowSuspension = bfalse;
        m_jumpReleased = btrue;
        m_parent->performBounce();
        m_bouncing = btrue;
    }
    else
    {
        if ( !m_skipForce )
        {
            m_parent->PerformJump();

            if ( m_parent->m_currentJumpWallRun )
            {
                m_allowSuspension = bfalse;
                m_wallRunJumpCounter = m_parent->getTemplate()->getWallRunJumpDuration();

                if ( m_wallRunJumpCounter )
                {
                    m_characterPhys->setUpdateRotation(bfalse);
                    m_parent->setGravityMode(GRAVITYMODE_WALLRUNJUMP,0.f);
                }
                else
                {
                    m_parent->m_currentJumpWallRun = bfalse;
                }
            }

        }
        else
        {
            m_skipForce = bfalse;
        }

        m_bouncing = bfalse;
    }
}

bbool Ray_PlayerControllerComponent::StateJump::checkStateChange( f32 _dt )
{
    if ( m_parent->isOnPedestal() )
    {
        m_parent->changeState(&m_parent->m_stateOnPedestal);
        return btrue;
    }

    if ( checkBounce() )
    {
        return btrue;
    }

    if ( m_queueCrushCounter )
    {
        m_queueCrushCounter = Max(m_queueCrushCounter-_dt,0.f);

        if ( m_queueCrushCounter == 0.f )
        {
            m_parent->setCurrentPunch(PUNCHTYPE_CRUSH,m_parent->getTemplate()->getCrushHitLevel());
            m_parent->changeState(&m_parent->m_stateReleaseHit);
        }
    }

    if  ( m_parent->isHitQueued() )
    {
        if ( m_parent->setAirPunch() )
        {
            return btrue;
        }
    }

    // Try hanging once the force was applied and we got jumping speed
    HangingInfo info;

    if ( m_parent->tryHanging(info) )
    {
        m_parent->setHangState(info);
        return btrue;
    }

    if ( !m_justJumped )
    {
        if ( m_parent->isJumpQueued() )
        {
            if ( m_parent->m_jumpOnAirTimer )
            {
                m_parent->setJumpData();
                restartJump();
                m_animComponent->resetCurTime();
                return bfalse;
            }
            else if ( m_parent->tryWallJump(bfalse) )
            {
                m_jumpReleased = bfalse;
                return bfalse;
            }
        }

        if ( m_characterPhys->getStickedPolyline() )
        {  
            m_parent->changeState(&m_parent->m_stateIdle);
            return btrue;
        }
    }

    if ( m_jumpReleased && ( m_parent->m_airControlDisableDelay == 0.f || m_parent->m_airControlDisableAllowHelico ) )
    {
        // In the case of the jump we only trigger the helico if we are going down
        if ( m_characterPhys->getSpeed().dot(m_parent->m_currentGravityDir) >= 0.f )
        {
            if ( m_parent->shouldTriggerHelicopterAction() ||
               ( m_jumpReleased && m_parent->shouldTriggerHelicoFromHold() ) )
            {
                m_parent->changeState(&m_parent->m_stateHelicopter);
                return btrue;
            }
        }
    }

    if ( !m_justJumped && m_parent->tryWallSlide(_dt) )
    {
        m_parent->changeState(&m_parent->m_stateWallSlide);
        return btrue;
    }

    if ( ( !m_parent->isPerformingUTurn() || m_parent->m_uTurnFinished ) &&
         !m_parent->isSupporting() &&
         isCurrentAnimFinished() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        m_parent->m_stateFalling.setJumpReleased(m_jumpReleased);
        return btrue;
    }

    if ( checkExitFromCurrent() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_wallRunJumpCounter )
    {
        m_wallRunJumpCounter -= _dt;

        if ( m_wallRunJumpCounter <= 0.f )
        {
            m_parent->changeState(&m_parent->m_stateFalling);
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateJump::update(f32 _dt)
{
    if (m_parent->isJumpQueued() && !m_justJumped)
    {
        if ( m_parent->tryWallJump(bfalse) )
        {
            return;
        }
    }

    m_parent->UpdatePhysicAir();

    if ( !m_jumpReleased )
    {
        m_jumpReleased = m_parent->m_listenerJump == bfalse &&
                         m_parent->m_listenerJumpHold == bfalse;
    }

    //if jump button pressed during flight add some suspension
    if ( m_allowSuspension )
    {
        if ( m_parent->m_currentGravityDir.dot(m_characterPhys->getSpeed()) > 0.f )
        {
            m_allowSuspension = bfalse;
        }
        else if ( m_jumpReleased && !m_suspensionDelay && m_suspensionPushCounter < m_parent->getTemplate()->getAirSuspensionPushThreshold() )
        {
            m_allowSuspension = bfalse;
        }
    }

    if ( m_allowSuspension )
    {
        m_suspensionDelay = Max(m_suspensionDelay-_dt,0.f);
        m_suspensionPushCounter += _dt;
        m_parent->addJumpAirImpulse();
    }


    if (!m_justJumped)
    {
        m_parent->checkFallOnCharacter();
    }

    if ( !m_parent->isPerformingUTurn() || m_parent->m_uTurnFlipped )
    {
        if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_UP )
        {
            m_airPose = AIRPOSE_UP;
        }
        else if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN )
        {
            m_airPose = AIRPOSE_DOWN;
        }
        else
        {
            m_airPose = AIRPOSE_MIDDLE;
        }
    }

    m_justJumped = bfalse;

    if ( m_bouncing )
    {
        if ( m_parent->m_bounceRepositionTotal != Vec2d::Zero )
        {
            f32 maxT = m_parent->m_bounceRepositionTotalTime;
            f32 time0 = m_parent->m_bounceRepositionCounter;
            f32 time1 = Min(time0 + _dt,maxT);

            // Use a curve x^2 so that we reposition ourselves more at the start but less at the end
            // The values change more at the end of the curve, so we invert it
            f32 t0 = 1.f - ( time0 / maxT );
            f32 t1 = 1.f - ( time1 / maxT );

            t0 = t0 * t0;
            t1 = t1 * t1;

            f32 t = t0 - t1;

            if ( t )
            {
                Vec2d d = m_parent->m_bounceRepositionTotal*t;
                Vec2d targetP = m_actor->get2DPos() + d;
                m_actor->setPos(targetP.to3d(m_actor->getPos().m_z));
                m_parent->m_bounceRepositionCounter = time1;
            }


            if ( time1 == maxT )
            {
                m_parent->m_bounceRepositionTotal = Vec2d::Zero;
                m_parent->m_bounceRepositionCounter = 0.f;

                if ( m_parent->m_bounceRepositionFinalSpeed != Vec2d::Zero )
                {
                    m_characterPhys->setSpeed(m_parent->m_bounceRepositionFinalSpeed);
                }

                m_parent->setMaxSpeedLimit(btrue);
                m_characterPhys->setGravityMultiplier(1.f);
            }
        }
        else
        {
            if ( m_parent->m_bounceRepositionCounter )
            {
                m_parent->m_bounceRepositionCounter = Max(m_parent->m_bounceRepositionCounter-_dt,0.f);

                if ( m_parent->m_bounceRepositionCounter == 0.f )
                {
                    m_characterPhys->setGravityMultiplier(1.f);
                    m_parent->setMaxSpeedLimit(btrue);
                }
            }
        }
    }
}

bbool Ray_PlayerControllerComponent::StateJump::checkExitFromCurrent() const
{
    if ( m_parent->m_surfingTimer )
    {
        return bfalse;
    }

    if ( m_parent->m_currentGravityDir.dot(m_characterPhys->getSpeed()) > 0.f )
    {
        return bfalse;
    }

    u32 numForces = m_parent->m_currentWindForces.size();
    Vec2d totalForces = Vec2d::Zero;

    for ( u32 i = 0; i < numForces; i++ )
    {
        PhysActorForce& wind = m_parent->m_currentWindForces[i];

        if ( m_actor->getBinding()->isDescendant(wind.m_actor) )
        {
            continue;
        }

        totalForces += wind.m_force;
    }


    f32 dotProd = m_parent->m_currentGravity.dot(totalForces);

    if ( dotProd < 0.f )
    {
        return btrue;
    }
    else
    {
        return bfalse;
    }
}

void Ray_PlayerControllerComponent::StateJump::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ACCROBATICSTAGE],m_parent->getAccrobaticStage());
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_AIRPOSE],m_airPose);
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_JUMPFROMWALLRUN],m_wallRunJumpCounter > 0.f);
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_INTERACTING],static_cast<u32>(m_parent->isSupportHolding()));
}

}
