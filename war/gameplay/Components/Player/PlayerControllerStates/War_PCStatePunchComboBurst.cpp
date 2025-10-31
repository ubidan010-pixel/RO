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
    IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StatePunchComboBurst)

    bbool War_PlayerControllerComponent::StatePunchComboBurst::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
    {
        if ( m_hitFrequency )
        {
            // A button was pressed
            if ( m_parent->m_burstPunchInputFrequency && m_hitFrequency < m_parent->m_burstPunchInputFrequency )
            {
                m_hitFrequency = 0.f;
                return bfalse;
            }
            else if ( m_parent->m_accrobaticPunchInputFrequency && m_hitFrequency < m_parent->m_accrobaticPunchInputFrequency )
            {
                m_parent->changeState(&m_parent->m_statePunchAccrobatic);
            }
            else
            {
                m_parent->changeState(&m_parent->m_stateIdle);
            }

            return btrue;
        }
        else
        {
            // No button pressed
            if ( m_timer > m_parent->m_burstPunchInputFrequency &&
                 m_timer > m_parent->m_accrobaticPunchInputFrequency )
            {
                m_parent->changeState(&m_parent->m_stateIdle);
                return btrue;
            }
        }

        return bfalse;
    }

}