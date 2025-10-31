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

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_


namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateClimbing)

////////////////////
// Climb State
////////////////////
Ray_PlayerControllerComponent::StateClimbing::StateClimbing()
: m_allowMove(bfalse)
{
}

void Ray_PlayerControllerComponent::StateClimbing::onEnter()
{
    Super::onEnter();

    if ( m_parent->m_stance != STANCE_CLIMB )
    {
        m_parent->resetUTurn();
        m_parent->climbFromPoint(m_parent->m_hangInfo);
        m_parent->getMoveData().resetSameDir();
        m_allowMove = bfalse;
    }
    else
    {
        m_allowMove = btrue;
    }
}

void Ray_PlayerControllerComponent::StateClimbing::onExit()
{
    m_parent->resetUTurn();
    m_parent->stopLianaSlideFx();
}

bbool Ray_PlayerControllerComponent::StateClimbing::checkStateChange( f32 _dt )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    PolyLine* polyLine;
    const PolyLineEdge* edge;

    m_parent->getClimbEdgePolyline(&polyLine,&edge);

    if ( !polyLine || !edge )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( !m_parent->m_climbingJustStarted && m_parent->isJumpQueued() && !m_animComponent->isLockMagicBoxInterpolating() )
    {
        m_parent->prepareClimbingJump();
        return btrue;
    }

    Vec2d climbPos = edge->getPos() + (edge->m_vector*m_parent->m_climbingT);

    if ( !m_parent->m_climbingJustStarted && m_parent->checkClimbCollision(m_parent->m_climbingPolyline,m_parent->m_climbingEdge,climbPos,getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular())) )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_parent->m_climbingSlide && m_parent->m_climbingOnBottomCorner )
    {
        m_parent->m_jumpOnAirTimer = m_parent->getTemplate()->getJumpOnAirTime();
        exitToFall();
    }

    if ( m_parent->m_coopActionMode != ACTIONCOOPMODE_MAIN )
    {
        if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN && !m_parent->getMoveData().getMoveSameDir() )
        {
            bbool drop = bfalse;

            if ( m_parent->m_climbingVertical )
            {
                if ( m_parent->m_climbingAllowDropVertical )
                {
                    f32 moveSign = m_parent->updateClimbEdgeGetClimbSignFromClimbDir(MOVEDIR_DOWN,m_parent->m_climbingVertical,edge);

                    if ( moveSign > 0.f )
                    {
                        drop = btrue;
                    }
                }
            }
            else
            {
                drop = m_parent->m_climbingAllowExitHorizontal;
            }

            if ( drop )
            {
                exitToFall();
                return btrue;
            }
        }
    }
    else
    {
        if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN &&
            !m_parent->m_climbingVertical &&
             m_parent->m_climbingAllowExitHorizontal )
        {
            m_parent->m_disableCrouchTimer = m_parent->getTemplate()->getDisableCrouchTime();
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }

    if ( m_parent->isHitQueued() )
    {
        if ( m_parent->m_coopActionMode == ACTIONCOOPMODE_MAIN &&
            m_parent->m_coopActionVictim != ITF_INVALID_OBJREF )
        {
            // coop punch removed
        }
        else
        {
            if ( m_parent->startNormalPunch() )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateClimbing::exitToFall()
{
    m_parent->m_disableCrouchTimer = m_parent->getTemplate()->getDisableCrouchTime();

    Vec2d averageSpeed = m_characterPhys->getWorldSpeedAverage();

    averageSpeed.m_x = Clamp(averageSpeed.m_x,-m_parent->getTemplate()->getClimbMaxJumpSpeedTransfer().m_x,m_parent->getTemplate()->getClimbMaxJumpSpeedTransfer().m_x);
    averageSpeed.m_y = Clamp(averageSpeed.m_y,-m_parent->getTemplate()->getClimbMaxJumpSpeedTransfer().m_y,m_parent->getTemplate()->getClimbMaxJumpSpeedTransfer().m_y);

    m_characterPhys->setSpeed(averageSpeed);

    m_parent->changeState(&m_parent->m_stateFalling);
}

void Ray_PlayerControllerComponent::StateClimbing::update( f32 _dt )
{
    m_parent->UpdatePhysicClimbingEdge(_dt);
    m_parent->m_climbingJustStarted = bfalse;
}

void Ray_PlayerControllerComponent::StateClimbing::onEvent(Event * _event)
{
    if ( AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == MRK_Force_Impulse )
        {
            sendMoveImpulseEvent();
        }
        else if ( animEvent->getName() == MRK_Perform_UTurn )
        {
            m_parent->startUTurn(UTURNTYPE_CLIMB);
        }
        else if ( animEvent->getName() == MRK_Allow_Move )
        {
            m_allowMove = btrue;
        }
    }
    else if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
    {
        if ( m_parent->m_climbingAllowHangCoop && 
           ( m_parent->m_coopActionVictim == ITF_INVALID_OBJREF ||
             m_parent->m_coopActionVictim == query->getSender() ) )
        {
            query->setInteraction(CharacterInteractionType_Grab);
        }
    }
}

void Ray_PlayerControllerComponent::StateClimbing::sendMoveImpulseEvent()
{
    const BaseObject* pObj = GETOBJECT(m_parent->m_climbingPolyline);
    const PolyLine* polyLine;

    if ( pObj )
    {
        ITF_ASSERT(pObj->IsClassCRC(ITF_GET_STRINGID_CRC(PolyLine,1932163747)));

        polyLine = static_cast<const PolyLine*>(pObj);

        if ( polyLine->getOwnerActor() && m_parent->m_climbingEdge < polyLine->getPosCount() )
        {
            Vec2d impulseDir = Vec2d::Right.Rotate(m_actor->getAngle()) * m_parent->getTemplate()->getClimbEdgeMoveImpulse();

            if ( m_actor->isFlipped() )
            {
                impulseDir *= -1.f;
            }

            EventRopeSwingImpulse impulseEvent(m_actor->getRef(),m_parent->m_climbingPolyline,
                m_parent->m_climbingEdge,
                m_parent->m_climbingT,
                m_characterPhys->getWeight(),
                impulseDir,
                m_parent->getCharacterSize());

            polyLine->getOwnerActor()->onEvent(&impulseEvent);
        }
    }
}

void Ray_PlayerControllerComponent::StateClimbing::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ONCORNER],static_cast<u32>(m_parent->m_climbingOnBottomCorner));
}

}
