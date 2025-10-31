#include "precompiled_gameplay_rayman.h"

#ifdef ITF_SUPPORT_CHEAT

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateFlyMode)

//////////////////////////////////////////////////////////////////////////
// FLY MODE
//////////////////////////////////////////////////////////////////////////
void Ray_PlayerControllerComponent::StateFlyMode::onEnter()
{
    Super::onEnter();

    m_characterPhys->setDisabled(btrue);
    m_characterPhys->setSpeed(Vec2d::Zero);
}

void Ray_PlayerControllerComponent::StateFlyMode::onExit()
{
    m_characterPhys->setDisabled(bfalse);
}

bbool Ray_PlayerControllerComponent::StateFlyMode::checkStateChange( f32 _dt )
{
    if ( !m_parent->m_listenerFlyCheat )
    {
        if (m_parent->isDead())
        {
            m_parent->changeState(&m_parent->m_stateDeadSoul);
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateFalling);
        }

        return true;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateFlyMode::update(f32 _dt)
{
    m_parent->UpdatePhysicFlyMode(_dt);
}



}

#endif //ITF_SUPPORT_CHEAT