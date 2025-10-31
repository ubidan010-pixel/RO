#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_WAR_ANIMATIONMARKERS_H_
#include "war/gameplay/War_AnimationMarkers.h"
#endif //_ITF_WAR_ANIMATIONMARKERS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StatePunchComboBase)

War_PlayerControllerComponent::StatePunchComboBase::StatePunchComboBase()
    : Super()
    , m_hitFrequency(0.f)
    , m_timer(0.f)
    , m_justEntered(bfalse)
{

}

void War_PlayerControllerComponent::StatePunchComboBase::onEnter()
{
    Super::onEnter();

    m_hitFrequency = 0.f;
    m_timer = 0.f;
    m_justEntered = btrue;
}

void War_PlayerControllerComponent::StatePunchComboBase::onExit()
{
    Super::onExit();

    // On combos we don't queue hits
    m_parent->m_queueHit = bfalse;
}

void War_PlayerControllerComponent::StatePunchComboBase::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    Super::update(_dt,_buttons,_axes);

    if ( !m_justEntered )
    {
        if  ( _buttons[m_joyButton_X]==InputAdapter::JustPressed )
        {
            m_hitFrequency = m_timer;
            m_timer = 0.f;
        }
        else
        {
            m_timer += _dt;
        }
    }

    m_justEntered = bfalse;
}

}