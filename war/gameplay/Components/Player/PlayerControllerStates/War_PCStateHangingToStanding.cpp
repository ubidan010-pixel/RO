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

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHangToStanding)

////////////////////
// Hanging To Idle State
////////////////////
void War_PlayerControllerComponent::StateHangToStanding::onEnter()
{
    Super::onEnter();

    m_canMove = bfalse;
}

bbool War_PlayerControllerComponent::StateHangToStanding::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( ( m_canMove && ( m_parent->isHitQueued() || m_parent->m_queueJump ) ) || isCurrentAnimFinished() )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHangToStanding::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    if ( m_canMove )
    {
        if ( m_characterPhys->getStickedEdge() )
        {
            m_parent->UpdatePhysicWalk(m_characterPhys,_dt);
        }
        else
        {
            m_parent->UpdatePhysicAir(m_characterPhys);
        }
    }
}

void War_PlayerControllerComponent::StateHangToStanding::onEvent(Event * _event)
{
    if (_event->IsClass(AnimGameplayEvent::GetClassNameStatic()))
    {
        AnimGameplayEvent * animEvent = (AnimGameplayEvent *)_event;
        if ( animEvent->getName() == War_MRK_Allow_Move )
        {
            m_canMove = btrue;
        }
    }
}

}