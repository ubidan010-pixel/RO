#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StatePunchComboAccrobatic)

////////////////////
// Idle to Climb State
////////////////////
void War_PlayerControllerComponent::StatePunchComboAccrobatic::onEnter()
{
    Super::onEnter();

    m_stage = 0;
    m_nextHit = bfalse;
}

bbool War_PlayerControllerComponent::StatePunchComboAccrobatic::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( m_parent->isPerformingUTurn() )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    if ( isCurrentAnimFinished() )
    {
        if ( m_hitFrequency )
        {
            // A button was pressed
            if ( m_parent->m_burstPunchInputFrequency && m_hitFrequency < m_parent->m_burstPunchInputFrequency )
            {
                m_parent->changeState(&m_parent->m_statePunchBurst);
                return btrue;
            }
            else if ( m_parent->m_accrobaticPunchInputFrequency && m_hitFrequency < m_parent->m_accrobaticPunchInputFrequency )
            {
                m_nextHit = btrue;
            }
            else
            {
                m_parent->changeState(&m_parent->m_stateIdle);
                return btrue;
            }
        }
        else
        {
            // No button pressed
            m_parent->changeState(&m_parent->m_stateIdle);
            return btrue;
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::StatePunchComboAccrobatic::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    Super::update(_dt,_buttons,_axes);

    if ( m_nextHit )
    {
        m_hitFrequency = 0.f;
        m_stage++;

        if ( m_stage >= m_parent->m_numAccrobaticPunchCombo )
        {
            m_stage = 0;
        }

        m_nextHit = bfalse;
    }

    m_parent->UpdatePhysicWalk(m_characterPhys,_dt);
}

void War_PlayerControllerComponent::StatePunchComboAccrobatic::updateAnimInputs()
{
    Super::updateAnimInputs();

    static const StringID s_ComboState= "PunchComboStage";

    m_animComponent->setInput(s_ComboState,m_stage);
}

}