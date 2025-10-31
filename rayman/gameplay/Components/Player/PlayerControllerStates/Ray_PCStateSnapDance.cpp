#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateSnapDance)

////////////////////
// Idle State
////////////////////
Ray_PlayerControllerComponent::StateSnapDance::StateSnapDance()
: m_exit(bfalse)
{
}

void Ray_PlayerControllerComponent::StateSnapDance::onEnter()
{
    Super::onEnter();

    m_exit = bfalse;
    m_characterPhys->setDisabled(btrue);
    m_characterPhys->setSpeed(Vec2d::Zero);
    m_parent->setSprinting(bfalse);
}

void Ray_PlayerControllerComponent::StateSnapDance::onExit()
{
    Super::onExit();

    m_characterPhys->setDisabled(bfalse);
}

bbool Ray_PlayerControllerComponent::StateSnapDance::checkStateChange( f32 _dt )
{
    if ( m_exit )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateSnapDance::update(f32 _dt)
{
    Super::update(_dt);

    m_parent->m_moveInput.m_x = 0.f;
    m_parent->setSprinting(bfalse);
}

void Ray_PlayerControllerComponent::StateSnapDance::onEvent(Event * _event)
{
    if (Ray_EventSnapDance* snapDance = _event->DynamicCast<Ray_EventSnapDance>(ITF_GET_STRINGID_CRC(Ray_EventSnapDance,806811309)))
    {
        m_exit = snapDance->getSnap() == bfalse;
    }
}

void Ray_PlayerControllerComponent::StateSnapDance::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_SUPPORTING],static_cast<u32>(m_parent->getMoveData().getMoveDirection() == MOVEDIR_UP));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ISCROUCHED],static_cast<u32>(m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN));
}

}
