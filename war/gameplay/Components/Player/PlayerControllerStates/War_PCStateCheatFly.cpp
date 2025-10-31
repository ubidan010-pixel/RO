#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateCheatFlyMode)

//////////////////////////////////////////////////////////////////////////
// FLY MODE
//////////////////////////////////////////////////////////////////////////
void War_PlayerControllerComponent::StateCheatFlyMode::onEnter()
{
    Super::onEnter();

    m_characterPhys->setDisabled(btrue);
    m_characterPhys->setSpeed(Vec2d::Zero);
}

void War_PlayerControllerComponent::StateCheatFlyMode::onExit()
{
    m_characterPhys->setDisabled(bfalse);
}

bbool War_PlayerControllerComponent::StateCheatFlyMode::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if (_buttons[m_joyButton_LB] != InputAdapter::Pressed ||
        _buttons[m_joyButton_RB] != InputAdapter::Pressed ||
        _axes[m_joyTrigger_Left] == 0.0f ||
        _axes[m_joyTrigger_Right] == 0.0f)
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return true;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateCheatFlyMode::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    m_parent->UpdatePhysicFlyMode(_dt);
}

}