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

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateIdleToHang)

const f32 Ray_PlayerControllerComponent::StateIdleToHang::s_blendTime = 0.1f;

////////////////////
// Idle to Hang State
////////////////////
void Ray_PlayerControllerComponent::StateIdleToHang::onEnter()
{
    Super::onEnter();

    m_parent->setStance(STANCE_HANG);
    m_parent->resetUTurn();
    m_parent->hangFromPoint(m_parent->m_hangInfo);
    m_blendCounter = 0.f;
    m_startRot = m_actor->getAngle();
    m_canMove = bfalse;
}

void Ray_PlayerControllerComponent::StateIdleToHang::onExit()
{
    Super::onExit();

    if ( m_parent->m_nextState == &m_parent->m_stateHanging )
    {
        if ( m_parent->m_hangInfo.m_lookRight != !m_actor->isFlipped() )
        {
            m_parent->setLookRight(m_actor->isFlipped());
        }
    }
}

bbool Ray_PlayerControllerComponent::StateIdleToHang::checkStateChange( f32 _dt )
{
    if ( m_parent->m_hangingEdge != U32_INVALID )
    {
        PolyLine* polyLine;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_parent->m_hangingPolyline,m_parent->m_hangingEdge,polyLine,edge);

        if ( !polyLine || !edge )
        {
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }
    else if ( m_parent->m_hangingActor != ITF_INVALID_OBJREF )
    {
        BaseObject* obj = GETOBJECT(m_parent->m_hangingActor);

        if ( !obj || !obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
        {
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }
    else
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_canMove && m_parent->isJumpQueued() )
    {
        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }
    else if ( isCurrentAnimFinished() )
    {
        m_parent->changeState(&m_parent->m_stateHanging);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateIdleToHang::update(f32 _dt)
{
    Vec2d pos;
    f32 angle;

    m_parent->getWorldHangPos(pos,angle);
    m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));
    m_blendCounter = Min(m_blendCounter+_dt,s_blendTime);

    f32 t = m_blendCounter / s_blendTime;
    f32 dif = getShortestAngleDelta(m_startRot,angle);
    f32 newAngle = m_startRot + (dif*t);

    m_actor->setAngle(newAngle);
}

void Ray_PlayerControllerComponent::StateIdleToHang::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == MRK_Allow_Move )
        {
            m_canMove = btrue;
        }
    }
}

void Ray_PlayerControllerComponent::StateIdleToHang::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ONCORNER],static_cast<u32>(m_parent->m_onCorner));
}

}