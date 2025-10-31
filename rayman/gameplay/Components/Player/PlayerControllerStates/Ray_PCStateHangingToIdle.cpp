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

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHangToIdle)

const f32 Ray_PlayerControllerComponent::StateHangToIdle::s_blendTime = 0.2f;

////////////////////
// Hanging To Idle State
////////////////////
void Ray_PlayerControllerComponent::StateHangToIdle::onEnter()
{
    Super::onEnter();

    m_characterPhys->setDisabled(btrue);
    m_canMove = bfalse;
    m_moving = bfalse;
    m_timeCounter = 0.f;
    m_startRot = m_actor->getAngle();

    Vec2d pos;
    f32 angle;

    m_parent->getWorldHangPos(pos,angle);
    m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));
}

void Ray_PlayerControllerComponent::StateHangToIdle::onExit()
{
    if ( !m_moving )
    {
        clearHanging();
    }
}

bbool Ray_PlayerControllerComponent::StateHangToIdle::checkStateChange( f32 _dt )
{
    if ( m_parent->isJumpQueued() )
    {
        if ( !m_moving )
        {
            clearHanging();
        }

        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    if ( ( m_canMove && m_parent->isHitQueued() ) || isCurrentAnimFinished() )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    if ( !m_parent->isCurrentHangValid() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHangToIdle::finishHang()
{
    if ( m_parent->m_hangingPolyline == ITF_INVALID_OBJREF )
    {
        return;
    }

    BaseObject* obj = GETOBJECT(m_parent->m_hangingPolyline);

    if ( obj )
    {
        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly && m_parent->m_hangingEdge < poly->getPosCount() )
        {
            const PolyLineEdge* edge = &poly->getEdgeAt(m_parent->m_hangingEdge);

            m_characterPhys->forceStickOnEdge(poly,edge,m_parent->m_hangingEdge);
        }
    }
}

void Ray_PlayerControllerComponent::StateHangToIdle::clearHanging()
{
    if ( m_parent->m_coopActionMain != ITF_INVALID_OBJREF )
    {
        Actor* coopMaster = AIUtils::getActor(m_parent->m_coopActionMain);

        if ( coopMaster )
        {
            Ray_EventPlayLockAnim eventLock(bfalse);
            coopMaster->onEvent(&eventLock);
        }
    }
    
    if ( m_parent->m_coopActionVictim != ITF_INVALID_OBJREF )
    {
        Actor* coopVictim = AIUtils::getActor(m_parent->m_coopActionVictim);

        if ( coopVictim )
        {
            Ray_EventPlayLockAnim eventLock(bfalse);
            coopVictim->onEvent(&eventLock);
        }
    }

    finishHang();
    m_parent->clearHanging();
}

void Ray_PlayerControllerComponent::StateHangToIdle::update(f32 _dt)
{
    if ( m_canMove && !m_moving )
    {
        m_moving = btrue;
        clearHanging();
    }

    if ( m_moving )
    {
        m_parent->UpdatePhysicWalk(_dt);
    }
    else
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        Vec2d pos;
        f32 angle;

        m_parent->getWorldHangPos(pos,angle);

        m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));

        AIUtils::getPolyLine(m_parent->m_hangingPolyline,m_parent->m_hangingEdge,poly,edge);

        if ( poly && edge )
        {
            angle = getVec2Angle(edge->m_normalizedVector);
        }

        f32 t = m_timeCounter / s_blendTime;

        t = Clamp(t,0.f,1.f);
        
        f32 dif = getShortestAngleDelta(m_startRot,angle);

        m_actor->setAngle(m_startRot+(dif*t));
    }

    m_timeCounter += _dt;
}

void Ray_PlayerControllerComponent::StateHangToIdle::onEvent(Event * _event)
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