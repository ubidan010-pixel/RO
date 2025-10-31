#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHitRelease)

////////////////////
// Release Hit State
////////////////////
War_PlayerControllerComponent::StateHitRelease::StateHitRelease()
    : Super()
    , m_justEntered(bfalse)
    , m_hitPerformed(bfalse)
    , m_canMove(bfalse)
    , m_uturnTimer(0.f)
    , m_consecutivePunchCount(0)
    , m_comboWindowOpen(bfalse)
    , m_canPlaceCombo(bfalse)
    , m_comboQueued(bfalse)
    , m_comboStage(0)
{
}

void War_PlayerControllerComponent::StateHitRelease::onEnter()
{
    Super::onEnter();

    if ( m_parent->isPerformingUTurn() )
    {
        m_parent->m_punchLevel = 0;
    }

    m_justEntered = btrue;
    m_hitPerformed = bfalse;
    m_canMove = bfalse;

    // Reset the queue since we pressed hit to get here
    m_parent->m_queueHit = bfalse;


    if (m_comboQueued)
        m_comboStage++;
    else
        m_comboStage = 0;

    static const StringID s_consecutivePunchCount = "PunchCount";
    m_animComponent->setInput(s_consecutivePunchCount, m_consecutivePunchCount);

    if (m_comboQueued)
        LOG_BEN("hit combo %d (%d)", m_comboStage, m_comboStage % 2);
    else
        LOG_BEN("consecutive punch count %d", m_consecutivePunchCount);

    m_comboWindowOpen = bfalse;
    m_canPlaceCombo = bfalse;
    m_comboQueued = bfalse;
}

void War_PlayerControllerComponent::StateHitRelease::onExit()
{
    Super::onExit();

    m_parent->resetUTurn();
    m_uturnTimer = 0.f;
}

bbool War_PlayerControllerComponent::StateHitRelease::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if (m_justEntered)
        return bfalse;


    // combo interrupts current anim;
    // otherwise, we can hit again as soon as the anim is done;
    // transitions will take care of the rest

    bbool changeState(bfalse);

    if ( ( m_canMove && ( m_parent->m_moveInput != Vec2d::Zero || m_parent->isJumpQueued() ) )
       || m_comboQueued )
    {
        changeState = btrue;
    }
    else if ( isCurrentAnimFinished()  )
    {
        changeState = btrue;
    }

    if ( changeState )
    {
        switch(m_parent->m_stance)
        {
        case STANCE_STAND:
            {
                if ( m_parent->m_queueHit )
                {
                    m_consecutivePunchCount++;
                    m_parent->changeState(&m_parent->m_stateReleaseHit);
                }
                else
                {
                    m_consecutivePunchCount = 0;
                    if ( m_characterPhys->getStickedEdge() )
                    {
                        m_parent->changeState(&m_parent->m_stateIdle);
                    }
                    else
                    {
                        m_parent->changeState(&m_parent->m_stateFalling);
                    }
                }
            }
            break;

        case STANCE_HANG:
            {
                m_parent->changeState(&m_parent->m_stateHanging);
            }
            break;

        case STANCE_CLIMB:
            {
                m_parent->changeState(&m_parent->m_stateClimbing);
            }
            break;
        }
    }

    return changeState;
}

void War_PlayerControllerComponent::StateHitRelease::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    Super::update(_dt,_buttons,_axes);

    if ( m_uturnTimer &&
        !m_hitPerformed &&
        ( ( m_parent->m_moveDirection == MOVEDIR_LEFT && m_parent->getIsLookingRight() ) ||
        ( m_parent->m_moveDirection == MOVEDIR_RIGHT && !m_parent->getIsLookingRight() ) )
        )
    {
        m_parent->setCurrentPunch(WAR_PUNCHTYPE_UTURNKICK, m_parent->m_uTurnHitLevel);
        m_parent->startUTurn();
        m_uturnTimer = 0.f;
    }

    if (m_canPlaceCombo && _buttons[m_joyButton_X] == InputAdapter::JustPressed)
    {
        m_comboQueued = btrue;
    }

    m_justEntered = bfalse;
    m_uturnTimer = Max(m_uturnTimer-_dt,0.f);


#ifdef DEVELOPER_BEN
    if (m_comboWindowOpen)
    {
        Color color(m_comboQueued ? Color::Blue : m_parent->isHitQueued() ? Color::Red : Color::Green);
        GFX_ADAPTER->drawDBGCircle(m_actor->getPos(), 1, color.m_r, color.m_g, color.m_b);
    }
#endif
}

void War_PlayerControllerComponent::StateHitRelease::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == War_MRK_Perform_Hit)
        {
            m_hitPerformed = btrue;
            m_parent->m_punchLevel = -1;
        }
        else if ( animEvent->getName() == War_MRK_PunchComboWindow_Start )
        {
            m_comboWindowOpen = btrue;
            m_canPlaceCombo = !m_parent->isHitQueued();
        }
        else if ( animEvent->getName() == War_MRK_PunchComboWindow_Stop )
        {
            m_comboWindowOpen = bfalse;
            m_canPlaceCombo = bfalse;
        }
        else if ( animEvent->getName() == War_MRK_Allow_Move )
        {
            m_canMove = btrue;
        }
    }
}

}
