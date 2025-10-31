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

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHangToIdle)

const f32 War_PlayerControllerComponent::StateHangToIdle::s_blendTime = 0.2f;

////////////////////
// Hanging To Idle State
////////////////////
void War_PlayerControllerComponent::StateHangToIdle::onEnter()
{
    Super::onEnter();

    m_characterPhys->setDisabled(btrue);
    m_canMove = bfalse;
    m_moving = bfalse;
    m_timeCounter = 0.f;

    Vec2d pos;
    f32 angle;

    m_parent->getWorldHangPosFromLocal(0.f,pos,angle);
    m_animComponent->lockMagicBox(pos.to3d(&m_actor->getPos().m_z));
}

void War_PlayerControllerComponent::StateHangToIdle::onExit()
{
    if ( !m_moving )
    {
        clearHanging();
    }
}

bbool War_PlayerControllerComponent::StateHangToIdle::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if ( ( m_canMove && ( m_parent->isHitQueued() || m_parent->m_queueJump ) ) || isCurrentAnimFinished() )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHangToIdle::finishHang()
{
    if ( m_parent->m_hangingPolyline == ITF_INVALID_OBJREF )
    {
        return;
    }

    BaseObject* obj = GETOBJECT(m_parent->m_hangingPolyline);

    if ( obj )
    {
        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly && m_parent->m_hangingEdge != U32_INVALID && m_parent->m_hangingEdge < poly->getPosCount() )
        {
            const PolyLineEdge* edge = &poly->getEdgeAt(m_parent->m_hangingEdge);

            m_characterPhys->forceStickOnEdge(poly,edge,m_parent->m_hangingEdge);
        }
    }
}

void War_PlayerControllerComponent::StateHangToIdle::clearHanging()
{
    finishHang();
    m_parent->clearHanging(m_characterPhys,m_animComponent);
}

void War_PlayerControllerComponent::StateHangToIdle::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    if ( m_canMove && !m_moving )
    {
        m_moving = btrue;
        clearHanging();
    }

    if ( m_moving )
    {
        m_parent->UpdatePhysicWalk(m_characterPhys,_dt);
    }
    else
    {
        Vec2d pos;
        f32 angle;

        f32 blend = m_timeCounter / s_blendTime;

        blend = Clamp(blend,0.f,1.f);

        m_parent->getWorldHangPosFromLocal(blend,pos,angle);
        m_animComponent->lockMagicBox(pos.to3d(&m_actor->getPos().m_z));
        m_actor->setAngle(angle);
    }

    m_timeCounter += _dt;
}

void War_PlayerControllerComponent::StateHangToIdle::onEvent(Event * _event)
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