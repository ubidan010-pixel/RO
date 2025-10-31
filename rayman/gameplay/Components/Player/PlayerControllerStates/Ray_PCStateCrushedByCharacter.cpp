#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateCrushedByCharacter)

////////////////////
// A character jumps on top of us
////////////////////
void Ray_PlayerControllerComponent::StateCrushedByCharacter::onEnter()
{
    Super::onEnter();

    m_canMove = bfalse;
}

bbool Ray_PlayerControllerComponent::StateCrushedByCharacter::checkStateChange( f32 _dt )
{
    if ( isCurrentAnimFinished() ||
       ( m_canMove && ( m_parent->getMoveData().getMoveDirection() != MOVEDIR_NONE || m_parent->isJumpQueued() || m_parent->isHitQueued() ) ) )
    {
        if ( m_characterPhys->getStickedEdge() )
        {
            m_parent->changeState(&m_parent->m_stateIdle);
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateFalling);
        }

        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateCrushedByCharacter::update(f32 _dt)
{
    if ( !m_canMove )
    {
        m_parent->setMoveInput(Vec2d::Zero);
        m_parent->getMoveData().resetMove();
    }

    if ( m_characterPhys->getStickedEdge() )
    {
        m_parent->UpdatePhysicWalk(_dt);
    }
    else
    {
        m_parent->UpdatePhysicAir();
    }
}

void Ray_PlayerControllerComponent::StateCrushedByCharacter::restart()
{
    m_animComponent->resetCurTime();
}

void Ray_PlayerControllerComponent::StateCrushedByCharacter::onEvent(Event * _event)
{
    if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
    {
        AnimGameplayEvent * animEvent = (AnimGameplayEvent *)_event;
        if ( animEvent->getName() == MRK_Allow_Move )
        {
            m_canMove = btrue;
        }
    }
}

}
