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

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateAim)

////////////////////
// Aim State
////////////////////

War_PlayerControllerComponent::StateAim::StateAim()
: m_aimAngle(MTH_PIBY2)
, m_targetAimAngle(MTH_PIBY2)
, m_fxHandle(U32_INVALID)
{
}

void War_PlayerControllerComponent::StateAim::onEnter()
{
    Super::onEnter();

    m_targetAimAngle = 0.f;
    m_aimAngle = 0.f;
    m_parent->resetUTurn();
}

void War_PlayerControllerComponent::StateAim::onExit()
{
}

bbool War_PlayerControllerComponent::StateAim::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if (!m_characterPhys->getStickedPolyline())
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( _axes[m_joyTrigger_Left] == 0.0f)
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateAim::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    m_targetAimAngle = m_parent->getCurrentMove().m_x * MTH_PIBY2;

    updateAimAngle(_dt);

    m_parent->rotateAim(-m_aimAngle);
}

void War_PlayerControllerComponent::StateAim::updateAimAngle( f32 _dt )
{
    static const f32 speed = MTH_2PI * 2.f;
    f32 move = speed * _dt;

    if ( m_aimAngle < m_targetAimAngle )
    {
        m_aimAngle = Min(m_aimAngle+move,m_targetAimAngle);
    }
    else if ( m_aimAngle > m_targetAimAngle )
    {
        m_aimAngle = Max(m_aimAngle-move,m_targetAimAngle);
    }
}

void War_PlayerControllerComponent::StateAim::onBecomeActive()
{
    /*
    PHYSWORLD->moveWindForceToLayer(m_actor->getRef(),m_actor->getDepth());
    */
}

void War_PlayerControllerComponent::StateAim::onBecomeInactive()
{
    /*
    PHYSWORLD->moveWindForceToLayer(m_actor->getRef(),InvalidDepthRange);
    */
}

void War_PlayerControllerComponent::StateAim::updateAnimInputs()
{
    Super::updateAnimInputs();

    static const StringID s_AimAngle = "AimAngle";

    if ( m_aimAngle )
    {
        if ( m_aimAngle > 0.f )
        {
            m_parent->setLookRight(btrue);
        }
        else
        {
            m_parent->setLookRight(bfalse);
        }
    }

    m_animComponent->setInput(s_AimAngle,fabsf(m_aimAngle)*MTH_RADTODEG);
}

}
