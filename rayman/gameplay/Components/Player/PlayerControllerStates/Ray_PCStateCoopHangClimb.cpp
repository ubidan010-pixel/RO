#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateCoopHangClimb)

////////////////////
// Idle to Climb State
////////////////////
const f32 Ray_PlayerControllerComponent::StateCoopHangClimb::s_blendTime = 0.1f;

void Ray_PlayerControllerComponent::StateCoopHangClimb::onEnter()
{
    Super::onEnter();

    if ( m_parent->m_hangingActor != ITF_INVALID_OBJREF )
    {
        m_actor->getBinding()->unbindFromParent();
    }

    m_parent->m_hangingActor = ITF_INVALID_OBJREF;
    m_parent->m_coopActionMode = ACTIONCOOPMODE_NONE;
    m_parent->m_coopActionMain = ITF_INVALID_OBJREF;
    m_parent->m_coopActionVictim = ITF_INVALID_OBJREF;

    PolyLine* poly;
    const PolyLineEdge* edge;

    if ( m_parent->isHanging() )
    {
        m_parent->getHangEdgePolyline(&poly,&edge);
        m_blendCounter = s_blendTime;
        m_startRot = m_actor->getAngle();
    }
    else
    {
        m_parent->getClimbEdgePolyline(&poly,&edge);
        m_parent->setStance(STANCE_CLIMB);
        m_parent->m_climbingAllowExitHorizontal = bfalse;
        m_blendCounter = 0.f;
    }

    Vec2d climbPos = edge->getPos() + ( edge->m_vector *  m_parent->m_climbingT );

    m_animComponent->lockMagicBox(climbPos.to3d(m_actor->getPos().m_z));

    EventHanging event(m_actor->getRef(),m_parent->m_climbingPolyline,m_parent->m_climbingEdge,
        m_characterPhys->getWeight(),Vec2d::Zero,m_parent->m_climbingT,btrue,m_parent->getCharacterSize());

    if ( poly->getOwnerActor() )
    {
        poly->getOwnerActor()->onEvent(&event);
    }
    else if (poly->m_ownerFrise)
    {
        poly->m_ownerFrise->onEvent(&event);
    }
}

bbool Ray_PlayerControllerComponent::StateCoopHangClimb::checkStateChange( f32 _dt )
{
    if ( isCurrentAnimFinished() )
    {
        if ( m_parent->isHanging() )
        {
            m_parent->changeState(&m_parent->m_stateHanging);
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateClimbing);
        }
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateCoopHangClimb::update(f32 _dt)
{
    Vec2d pos;
    f32 angle;

    if ( m_parent->isHanging() )
    {
        m_parent->getWorldHangPos(pos,angle);
    }
    else
    {
        m_parent->getWorldClimbPos(pos,angle);
    }

    m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));

    if ( !m_parent->m_climbingAllowExitHorizontal && m_parent->getMoveData().getMoveDirection() != MOVEDIR_DOWN )
    {
        m_parent->m_climbingAllowExitHorizontal = btrue;
    }

    if ( m_blendCounter )
    {
        m_blendCounter = Max(m_blendCounter-_dt,0.f);
        f32 rotDif = getShortestAngleDelta(m_startRot,angle);
        f32 t = 1.f - ( m_blendCounter / s_blendTime );

        m_actor->setAngle( m_startRot + (rotDif*t) );
    }
}

}