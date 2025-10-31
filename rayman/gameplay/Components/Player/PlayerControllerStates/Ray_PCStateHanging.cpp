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

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHanging)

////////////////////
// Hanging State
////////////////////
const f32 Ray_PlayerControllerComponent::StateHanging::s_blendTime = 0.1f;

Ray_PlayerControllerComponent::StateHanging::StateHanging()
: m_blendCounter(0.f)
, m_startRot(0.f)
, m_canMoveAgain(bfalse)
, m_justEntered(bfalse)
, m_climbUp(btrue)
, m_allowDrop(bfalse)
, m_coopJump(bfalse)
{
}

void Ray_PlayerControllerComponent::StateHanging::onEnter()
{
    Super::onEnter();

    bbool notHanged = m_parent->m_hangingEdge == U32_INVALID && !m_parent->m_hangingActor.isValid();

    if ( notHanged )
    {
        m_parent->resetUTurn();
        m_parent->setStance(STANCE_HANG);
        m_parent->hangFromPoint(m_parent->m_hangInfo);
        m_blendCounter = 0.f;
        m_startRot = m_actor->getAngle();
        m_canMoveAgain = bfalse;
        m_justEntered = btrue;
    }
    else
    {
        m_blendCounter = 1.f;
    }

    if ( notHanged || m_parent->m_prevState == &m_parent->m_stateIdleToHang )
    {
        m_allowDrop = bfalse;
    }

    m_coopJump = bfalse;
    m_climbUp = bfalse;
}

bbool Ray_PlayerControllerComponent::StateHanging::checkStateChange( f32 _dt )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    if ( m_coopJump )
    {
        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    if ( !m_parent->isCurrentHangValid() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_canMoveAgain )
    {
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

        if ( m_allowDrop && m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN )
        {
            m_parent->m_disableCrouchTimer = m_parent->getTemplate()->getDisableCrouchTime();

            if ( m_parent->m_coopActionMode != ACTIONCOOPMODE_VICTIM &&
                 m_parent->m_coopActionMode != ACTIONCOOPMODE_MAIN_VICTIM &&
                 tryWallSlide() )
            {
                m_parent->changeState(&m_parent->m_stateWallSlide);
            }
            else
            {
                // Reset this so that we have a nice transition from fall to fall up or down
                m_parent->m_moveInput.m_y = 0.f;
                m_parent->changeState(&m_parent->m_stateFalling);
            }

            return btrue;
        }

        //JUMP if JUMP button pressed, we didn't just hang and we are within the timer
        if ( m_parent->isJumpQueued() )
        {
            m_parent->prepareClimbingJump();
            return btrue;
        }

        //CLIMB if we are pressing in the direction of the wall and we exceeded the timer,
        //OR we released the stick after having pressed in the proper direction
        if ( m_climbUp )
        {
            if ( m_parent->m_coopActionMode == ACTIONCOOPMODE_VICTIM || m_parent->m_coopActionMode == ACTIONCOOPMODE_MAIN_VICTIM )
            {
                Actor* coopMaster = AIUtils::getActor(m_parent->m_hangingActor);

                if ( coopMaster )
                {
                    Ray_EventQueryHangInfo queryEvent;

                    coopMaster->onEvent(&queryEvent);

                    if ( queryEvent.getIsClimb() || queryEvent.getIsHang() )
                    {
                        if ( queryEvent.getIsClimb() )
                        {
                            m_parent->m_climbingPolyline = queryEvent.getPoly();
                            m_parent->m_climbingEdge = queryEvent.getEdge();
                            m_parent->m_orientationPose = queryEvent.getOrientationPose();
                            m_parent->m_hangingPolyline = ITF_INVALID_OBJREF;
                            m_parent->m_hangingEdge = U32_INVALID;
                        }
                        else
                        {
                            m_parent->m_hangingPolyline = queryEvent.getPoly();
                            m_parent->m_hangingEdge = queryEvent.getEdge();

                            bbool lookRight = m_parent->m_climbingT < 0.5f;

                            if ( lookRight != !m_actor->isFlipped() )
                            {
                                m_parent->setLookRight(lookRight);
                            }
                        }

                        m_parent->m_climbingT = queryEvent.getT();

                        Ray_EventHangOnCharacter hangEvent(m_actor->getRef(),bfalse);
                        coopMaster->onEvent(&hangEvent);

                        m_parent->changeState(&m_parent->m_stateCoopHangClimb);
                        return btrue;
                    }
                }
            }
            else
            {
                if ( !m_parent->isHangTopColliding() )
                {
                    if ( m_parent->m_coopActionMode == ACTIONCOOPMODE_MAIN )
                    {
                        Actor* coopVictim = AIUtils::getActor(m_parent->m_coopActionVictim);
                        
                        if ( coopVictim )
                        {
                            Ray_EventPlayLockAnim eventLock(btrue,!m_actor->isFlipped(),ITF_INVALID_OBJREF);
                            coopVictim->onEvent(&eventLock);
                        }
                    }

                    m_parent->changeState(&m_parent->m_stateHangToIdle);
                    return btrue;
                }
            }
        }
    } 

    if ( ( m_parent->m_coopActionMode == ACTIONCOOPMODE_VICTIM || m_parent->m_coopActionMode == ACTIONCOOPMODE_MAIN_VICTIM ) &&
           m_parent->m_hangingActor != ITF_INVALID_OBJREF )
    {
        if ( Actor* actor = static_cast<Actor*>(GETOBJECT(m_parent->m_hangingActor)) )
        {
            EventInteractionQuery query;
            query.setSender(m_actor->getRef());
            actor->onEvent(&query);
            if (query.getInteraction() != CharacterInteractionType_Grab)
            {
                m_parent->changeState(&m_parent->m_stateFalling);
                return btrue;
            }
        }
    }

    if ( m_parent->m_hangingPolyline != ITF_INVALID_OBJREF )
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_parent->m_hangingPolyline,m_parent->m_hangingEdge,poly,edge);

        if ( !m_parent->isEdgeDirValidToHang(poly,m_parent->m_hangingEdge,*edge,m_parent->m_climbingT>0.f) )
        {
            PolyLine* adjPoly;
            u32 adjEdgeIndex;

            AIUtils::getAdjacentEdge(poly,m_parent->m_hangingEdge,m_parent->m_climbingT > 0.f,adjPoly,adjEdgeIndex);

            if ( adjEdgeIndex != U32_INVALID )
            {
                const PolyLineEdge& adjEdge = adjPoly->getEdgeAt(adjEdgeIndex);

                StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(adjEdge.m_normalizedVector);

                if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
                {
                    m_parent->changeState(&m_parent->m_stateHangToStand);
                    return btrue;
                }
            }

            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHanging::update(f32 _dt)
{
    if ( !m_justEntered && !m_canMoveAgain && !m_animComponent->isInTransition() )
    {
        m_canMoveAgain = btrue;
    }

    if ( !m_allowDrop )
    {
        m_allowDrop = m_parent->getMoveData().getMoveDirection() == MOVEDIR_NONE;
    }

    Vec2d hangPos;
    f32 hangAngle;

    m_parent->getWorldHangPos(hangPos,hangAngle);
    m_animComponent->lockMagicBox(hangPos.to3d(m_actor->getPos().m_z));

    if ( m_blendCounter < s_blendTime )
    {
        m_blendCounter = Min(m_blendCounter+_dt,s_blendTime);

        f32 t = m_blendCounter / s_blendTime;
        f32 dif = getShortestAngleDelta(m_startRot,hangAngle);

        hangAngle = m_startRot + (dif*t);
    }

    m_actor->setAngle(hangAngle);

    if ( m_justEntered )
    {
        m_justEntered = bfalse;
        m_parent->m_moveInput.m_x = 0.f;
        m_characterPhys->resetWorldSpeed();
    }
    else
    {
        updateInput(_dt);
    }
}

void Ray_PlayerControllerComponent::StateHanging::updateInput( f32 _dt )
{
    Vec2d moveInput = m_parent->m_moveInput;

    moveInput.m_x = 0.f;

    m_climbUp = bfalse;

    EMoveDir moveDirection = m_parent->getMoveData().getMoveDirection();

    if ( moveDirection == MOVEDIR_RIGHT ||
         moveDirection == MOVEDIR_LEFT ||
         moveDirection == MOVEDIR_UP )
    {
        if ( m_parent->m_coopActionMode == ACTIONCOOPMODE_NONE ||
            m_parent->m_coopActionMode == ACTIONCOOPMODE_MAIN )
        {
            if ( moveDirection == MOVEDIR_UP ||
               ( moveDirection == MOVEDIR_RIGHT ) == !m_actor->isFlipped() )
            {
                //always face obstacle unless specifically pressing stick against it
                moveInput.m_x = f32_Abs(m_parent->getMoveData().getMove().m_x);
                m_climbUp = btrue;
            }
            else
            {
                //always face obstacle unless specifically pressing stick against it
                moveInput.m_x = -f32_Abs(m_parent->getMoveData().getMove().m_x);
            }
        }
        else
        {
            moveInput.m_x = f32_Abs(m_parent->getMoveData().getMove().m_x);

            if ( !m_parent->isPerformingUTurn() )
            {
                if ( moveDirection == MOVEDIR_UP ||
                   ( moveDirection == MOVEDIR_RIGHT ) == !m_actor->isFlipped() )
                {
                    Actor* mainActor = AIUtils::getActor(m_parent->m_hangingActor);

                    if ( mainActor )
                    {
                        Ray_EventQueryHangInfo query;

                        mainActor->onEvent(&query);

                        if ( query.getIsHang() )
                        {
                            if ( ( query.getT() < 0.5f && !m_actor->isFlipped() ) ||
                                 ( query.getT() >= 0.5f && m_actor->isFlipped() ) )
                            {
                                m_climbUp = btrue;
                            }
                        }
                        else if ( query.getIsClimb() )
                        {
                            if ( moveDirection == MOVEDIR_UP )
                            {
                                m_climbUp = btrue;
                            }
                        }
                    }
                }
                else if ( moveDirection != MOVEDIR_UP )
                {
                    m_parent->startUTurn(UTURNTYPE_CLIMB);
                    moveInput.m_x *= -1.f;
                }
            }
        }
    }

    m_parent->setMoveInput(moveInput);
}

void Ray_PlayerControllerComponent::StateHanging::onEvent(Event * _event)
{
    if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
    {
        AnimGameplayEvent * animEvent = (AnimGameplayEvent *)_event;

        if (animEvent->getName() == MRK_Allow_Move)
        {
            m_canMoveAgain = btrue;
        }
    }
    else if ( Ray_EventHangCoopJump* jumpEvent = _event->DynamicCast<Ray_EventHangCoopJump>(ITF_GET_STRINGID_CRC(Ray_EventHangCoopJump,930438087)) )
    {
        m_coopJump = btrue;
    }
    else if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
    {
        if ( m_parent->m_coopActionVictim == ITF_INVALID_OBJREF ||
             m_parent->m_coopActionVictim == query->getSender() )
        {
            query->setInteraction(CharacterInteractionType_Grab);
        }
    }
}

bbool Ray_PlayerControllerComponent::StateHanging::tryWallSlide()
{
    PhysContactsContainer contacts;
    PhysShapeCircle shape(m_characterPhys->getRadius());
    Vec2d checkPos = m_actor->get2DPos();
    static const f32 offset_x = 0.2f;
    Vec2d offset = m_parent->m_currentGravityDir.getPerpendicular() * offset_x;

    if ( !m_actor->isFlipped() )
    {
        checkPos += offset;
    }
    else
    {
        checkPos -= offset;
    }

    PHYSWORLD->checkEncroachment(checkPos,checkPos,m_actor->getAngle(),&shape,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& c = contacts[i];

        if ( c.m_edgeIndex == U32_INVALID )
        {
            return bfalse;
        }

        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

        if ( !poly || !edge )
        {
            return bfalse;
        }

        if ( m_parent->isPolylineEdgeGoodForWallSlide(poly,edge,c.m_edgeIndex,btrue) )
        {
            m_parent->m_stateWallSlide.forceWallSlideEdge(c.m_collidableUserData,c.m_edgeIndex);
            return btrue;
        }
    }

    return bfalse;
}

}