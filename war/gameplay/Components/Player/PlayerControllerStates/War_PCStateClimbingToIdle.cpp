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

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateClimbingToIdle)

////////////////////
// Hanging To Idle State
////////////////////
const f32 War_PlayerControllerComponent::StateClimbingToIdle::s_blendTime = 0.2f;

War_PlayerControllerComponent::StateClimbingToIdle::StateClimbingToIdle()
: m_blendCounter(0.f)
, m_startRot(0.f)
, m_canMove(bfalse)
, m_moving(bfalse)
{

}

void War_PlayerControllerComponent::StateClimbingToIdle::onEnter()
{
    Super::onEnter();

    m_canMove = bfalse;
    m_moving = bfalse;

    m_blendCounter = s_blendTime;
    m_startRot = m_actor->getAngle();
}

void War_PlayerControllerComponent::StateClimbingToIdle::onExit()
{
    if ( !m_moving )
    {
        clearClimbing();
    }
}

bbool War_PlayerControllerComponent::StateClimbingToIdle::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( ( m_canMove && ( m_parent->isHitQueued() || m_parent->m_queueJump ) ) ||
         isCurrentAnimFinished() )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateClimbingToIdle::finishClimb()
{
    PolyLine* poly;
    const PolyLineEdge* edge;

    m_parent->getClimbEdgePolyline(&poly,&edge);

    if ( !edge )
    {
        return;
    }

    m_characterPhys->forceStickOnEdge(poly,edge,m_parent->m_climbingEdge);
}

void War_PlayerControllerComponent::StateClimbingToIdle::clearClimbing()
{
    finishClimb();
    m_parent->clearClimbing(m_characterPhys,m_animComponent);
}

void War_PlayerControllerComponent::StateClimbingToIdle::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    if ( m_canMove && !m_moving )
    {
        m_moving = btrue;
        clearClimbing();
    }

    if ( m_moving )
    {
        m_parent->UpdatePhysicWalk(m_characterPhys,_dt);
    }
    else
    {
        m_parent->UpdatePhysicClimbingEdge(_dt,m_animComponent,m_characterPhys);
    }

    if ( m_blendCounter )
    {
        m_blendCounter = Max(m_blendCounter-_dt,0.f);

        PolyLine* poly;
        const PolyLineEdge* edge;

        m_parent->getClimbEdgePolyline(&poly,&edge);

        if ( poly && edge )
        {
            f32 endRot = getVec2Angle(edge->m_normalizedVector);
            f32 rotDif = getShortestAngleDelta(m_startRot,endRot);

            f32 dif = rotDif * ( 1.f - ( m_blendCounter / s_blendTime ) );

            m_actor->setAngle(m_startRot+dif);
        }
    }
}

void War_PlayerControllerComponent::StateClimbingToIdle::onEvent(Event * _event)
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