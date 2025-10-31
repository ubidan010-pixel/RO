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

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHangToStanding)

////////////////////
// Hanging To Idle State
////////////////////
void Ray_PlayerControllerComponent::StateHangToStanding::onEnter()
{
    Super::onEnter();

    m_canMove = bfalse;
}

bbool Ray_PlayerControllerComponent::StateHangToStanding::checkStateChange( f32 _dt )
{
    if ( ( m_canMove && ( m_parent->isHitQueued() || m_parent->isJumpQueued() || m_parent->getMoveData().getMoveDirection() != MOVEDIR_NONE ) ) || isCurrentAnimFinished() )
    {
        m_characterPhys->forceFindStickEdge();

        if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
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

void Ray_PlayerControllerComponent::StateHangToStanding::update(f32 _dt)
{
    Vec2d hangPos;
    f32 hangAngle;

    m_parent->getWorldHangPos(hangPos,hangAngle);
    m_animComponent->lockMagicBox(hangPos.to3d(m_actor->getPos().m_z));
    m_actor->setAngle(hangAngle);
}

void Ray_PlayerControllerComponent::StateHangToStanding::onEvent(Event * _event)
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