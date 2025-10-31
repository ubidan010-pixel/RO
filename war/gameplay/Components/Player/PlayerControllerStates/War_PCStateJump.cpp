#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateJump)

////////////////////
// Jumping State
////////////////////
void War_PlayerControllerComponent::StateJump::onEnter()
{
    Super::onEnter();

    setupJump();

    // Ways to start/continue an accrobatic chain:
    // 1) Uturn jump
    // 2) Crouch jump
    // 3) Be in the chain and jump within a certain time
    bbool accrobatic = bfalse;

    if ( m_parent->isPerformingUTurn() )
    {
        accrobatic = btrue;
    }
    else if ( m_parent->m_currentAccrobaticStage && m_parent->m_lastJumpTimer < 0.7f )
    {
        // To be in accrobatic mode we must keep a certain speed
        f32 speedX = m_parent->m_currentGravityDir.getPerpendicular().dot(m_parent->m_playWorldSpeed);
        if ( fabs(speedX) >= m_parent->m_minSpeedForAccrobaticCombo )
        {
            accrobatic = btrue;
        }
    }

    if ( accrobatic )
    {
        m_parent->m_currentAccrobaticStage++;
    }
    else
    {
        m_parent->m_currentAccrobaticStage = 0;
    }

    m_parent->setDoubleJumping(bfalse);
    m_parent->resetAllowEarthquakePunch();

    static const StringID s_fxJump = "Jump";

    m_fxController->playFX(s_fxJump);
}

void War_PlayerControllerComponent::StateJump::onExit()
{
    m_parent->m_airSuspensionCounter = 0.0f;
    m_parent->m_jumpForceMultiplier = 1.0f;
    m_parent->m_lastJumpTimer = 0.f;
    m_parent->setDoubleJumping(bfalse);

    if ( m_parent->m_nextState == &m_parent->m_stateIdle )
    {
        m_parent->setCrouching(bfalse); //#142, no crouch when jumping
    }
    
    // Only reset it if we are not doing a uturn kick
    if ( m_parent->m_nextState != &m_parent->m_stateReleaseHit )
    {
        m_parent->resetUTurn();
    }

    m_justJumped = false;
}

void War_PlayerControllerComponent::StateJump::setupJump()
{
    m_setVerticalSpeed = bfalse;
    m_justJumped = btrue;
    m_verticalSpeed = 0.f;
    m_parent->PerformJump(m_characterPhys);
    m_parent->m_jumpQueueTimer = 0.0f;
    m_parent->m_queueJump = bfalse;
    m_parent->setStance(STANCE_STAND);
}

bbool War_PlayerControllerComponent::StateJump::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
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

    // Try hanging once the force was applied and we got jumping speed
    HangingInfo info;
    if ( m_verticalSpeed && m_parent->tryHanging(m_characterPhys,info) )
    {
        m_parent->setHangState(info);
        return btrue;
    }

    if ( !m_justJumped && m_characterPhys->getStickedPolyline() )
    {  
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    // HACK: aiming here
    //if (_axes[m_joyTrigger_Left] != 0.0f)
    //{
    //    m_parent->changeState(&m_parent->m_stateHelicopter);
    //    return btrue;
    //}

    return bfalse;
}

void War_PlayerControllerComponent::StateJump::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    if (m_parent->m_queueJump && !m_justJumped)
    {
        if ( m_parent->canDoubleJump() && !m_parent->isDoubleJumping() )
        {
            if ( m_parent->isPerformingUTurn() )
            {
                m_parent->resetUTurn();
            }

            m_parent->m_jumpForceMultiplier = m_parent->m_doubleJumpForceMultiplier;
            m_parent->setDoubleJumping(btrue);
            setupJump();
            return;
        }
        else
        {
            m_parent->m_jumpQueueTimer = m_parent->m_jumpQueueTime;
            m_parent->m_queueJump = bfalse;
        }
    }

    m_parent->UpdatePhysicAir(m_characterPhys);
    m_parent->updateAllowEarthquakePunch();

    //if jump button pressed during flight add some suspension
    if ( _buttons[m_joyButton_A]==InputAdapter::Pressed && m_parent->m_airSuspensionCounter )
    {
        // This adds suspension in the air
        f32 ySpeed = -m_parent->m_currentGravityDir.dot(m_characterPhys->getSpeed());

        if ( ySpeed > 0.f && !m_characterPhys->getStickedPolyline() )
        {
            Vec2d gravity = -m_parent->m_currentGravity;

            ySpeed = Clamp(ySpeed,m_parent->m_airSuspensionMinYSpeed,m_parent->m_airSuspensionMaxYSpeed);
            f32 t = ( ySpeed - m_parent->m_airSuspensionMinYSpeed ) / ( m_parent->m_airSuspensionMaxYSpeed - m_parent->m_airSuspensionMinYSpeed );
            f32 multiplier = Interpolate(m_parent->m_airSuspensionMinMultiplier,m_parent->m_airSuspensionMaxMultiplier,t);
            m_characterPhys->addForce(gravity*multiplier);
        }
    }

    if ( m_setVerticalSpeed )
    {
        // After the first update, we take the speed to put the proper position in the animation
        m_verticalSpeed = -m_characterPhys->getSpeedWRTGravity().m_x;
        m_setVerticalSpeed = bfalse;
    }

    if (m_justJumped)
    {
        m_setVerticalSpeed = btrue;
        m_justJumped = bfalse;
    }
    else
    {
        Actor* victim;

        if ( m_parent->checkFallOnCharacter(m_characterPhys,&victim) )
        {
            m_parent->m_jumpForceMultiplier = m_parent->m_fallOnVictimJumpMultiplier;
            m_parent->m_currentAccrobaticStage++;
            m_parent->setDoubleJumping(bfalse);
            setupJump();
            m_animComponent->resetCurTime();

            EventCrushed crushEvent(m_actor->getRef());

            victim->onEvent(&crushEvent);
        }
    }
}

void War_PlayerControllerComponent::StateJump::updateAnimInputs()
{
    Super::updateAnimInputs();

    static const StringID s_AccrobaticStage = "AccrobaticStage";
    static const StringID s_DoubleJump = "IsDoubleJump";

    m_animComponent->setInput(s_AccrobaticStage,m_parent->m_currentAccrobaticStage);
    m_animComponent->setInput(s_DoubleJump,static_cast<u32>(m_parent->isDoubleJumping()));

    if ( !m_characterPhys->getStickedEdge() )
    {
        f32 currentValue = 0.0f;

        if ( !m_setVerticalSpeed && !m_justJumped )
        {
            f32 range = fabs(m_verticalSpeed);

            if ( range )
            {
                f32 currentVerticalSpeed = -m_characterPhys->getSpeedWRTGravity().m_x;
                f32 t = fabs(currentVerticalSpeed)/range;

                if ( currentVerticalSpeed > 0.0f)
                {
                    // Going up we play from 0 to %50
                    currentValue = 0.f + ( 0.5f * ( 1.f - t ) );
                }
                else
                {
                    // Going down we play from %50 to 100%
                    currentValue = 0.5f + ( 0.5f * t );
                }
            }
        }

        currentValue = Clamp(currentValue, 0.f, 1.f);
        m_animComponent->setProceduralCursor(currentValue);
    }
}

}