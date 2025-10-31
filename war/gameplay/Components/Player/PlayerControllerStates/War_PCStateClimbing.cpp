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

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateClimbing)

////////////////////
// Climb State
////////////////////
const f32 War_PlayerControllerComponent::StateClimbing::s_swingStopTimer = 0.15f;

War_PlayerControllerComponent::StateClimbing::StateClimbing()
: m_moveInertia(0.f)
, m_prevMoveSign(0.f)
, m_animRate(1.f)
, m_stopSwingTimer(0.f)
, m_swingDir(SWINGDIR_NONE)
, m_prevClimbDir(MOVEDIR_NONE)
, m_inputVertical(bfalse)
, m_animVertical(bfalse)
, m_justEntered(bfalse)
, m_allowDropVertical(bfalse)
, m_allowExitHorizontal(bfalse)
{
}

void War_PlayerControllerComponent::StateClimbing::onEnter()
{
    Super::onEnter();

    if ( m_parent->m_stance != STANCE_CLIMB )
    {
        m_parent->resetUTurn();
        m_parent->climbFromPoint(m_hangInfo);

        PolyLine* poly;
        const PolyLineEdge* edge;

        m_parent->getClimbEdgePolyline(&poly,&edge);

        m_animVertical = m_parent->isVerticalClimb(edge);
        m_parent->m_climbingVerticalAnim = m_animVertical;

        if ( m_inputVertical && m_moveInertia )
        {
            // We cancel the inertia if it's a vertical rope and it's supposed to move us opposite to the gravity
            f32 dotProd = edge->m_normalizedVector.dot(m_parent->m_currentGravityDir);

            if ( ( dotProd > 0.f && m_moveInertia < 0.f ) ||
                 ( dotProd < 0.f && m_moveInertia > 0.f ) )
            {
                m_moveInertia = 0.f;
            }
        }

        m_parent->setStance(STANCE_CLIMB);
        m_justEntered = btrue;
        m_prevMoveSign = 0.f;
        m_allowDropVertical = bfalse;
        m_allowExitHorizontal = bfalse;
        m_swingDir = SWINGDIR_NONE;
        m_prevClimbDir = MOVEDIR_NONE;
        m_stopSwingTimer = 0.f;
    }
    else if ( m_parent->m_prevState == &m_parent->m_stateIdleToClimb )
    {
        m_prevMoveSign = 0.f;
        m_moveInertia = 0.f;
        m_allowDropVertical = bfalse;

        // For m_allowExitHorizontal we will handle it from the StateIdleToClimb
    }
}

void War_PlayerControllerComponent::StateClimbing::onExit()
{
    if ( m_parent->m_nextState != &m_parent->m_statePrepareHit &&
         m_parent->m_nextState != &m_parent->m_stateClimbingToIdle )
    {
        m_parent->clearClimbing(m_characterPhys,m_animComponent);
    }

    m_parent->resetUTurn();
    m_moveInertia = 0.f;
    m_justEntered = bfalse;
    m_stopSwingTimer = 0.f;
}

bbool War_PlayerControllerComponent::StateClimbing::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    PolyLine* polyLine;
    const PolyLineEdge* edge;

    m_parent->getClimbEdgePolyline(&polyLine,&edge);

    if ( !polyLine || !edge )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( !m_justEntered && m_parent->isJumpQueued() )
    {
        processJump();
        return btrue;
    }

    if ( m_parent->isHitQueued() )
    {
        m_parent->changeState(&m_parent->m_statePrepareHit);
        return btrue;
    }

    if ( m_parent->checkClimbCollision(m_characterPhys,m_parent->m_climbingPolyline) )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_parent->m_moveDirection == MOVEDIR_DOWN && !m_parent->m_moveSameDir )
    {
        bbool drop = bfalse;

        if ( m_inputVertical )
        {
            if ( m_allowDropVertical )
            {
                f32 moveSign = getClimbSignFromClimbDir(MOVEDIR_DOWN,m_inputVertical,edge);

                if ( moveSign > 0.f )
                {
                    drop = btrue;
                }
            }
        }
        else
        {
            drop = m_allowExitHorizontal;
        }

        if ( drop )
        {
            m_parent->m_disableCrouchTimer = m_parent->m_disableCrouchTime;
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }
    else if ( m_parent->m_moveDirection == MOVEDIR_UP )
    {
        if ( !m_inputVertical && m_allowExitHorizontal )
        {
            if ( polyLine->m_isCollisionActive )
            {
                m_parent->changeState(&m_parent->m_stateClimbingToIdle);
                return btrue;
            }
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateClimbing::update(f32 _dt,const InputAdapter::PressStatus* _buttons,
                                                      f32 * _axes )
{
    PolyLine* polyLine;
    const PolyLineEdge* edge;
    Vec2d moveInput = Vec2d::Zero;

    m_parent->getClimbEdgePolyline(&polyLine,&edge);

    // Orient the character towards the gravity
    updateOrientation(_dt);

    if ( m_inputVertical )
    {
        // Swing if it's a vertical polyline
        updateSwing(_dt,polyLine,moveInput);
    }

    updateLookDir();

    // Move our character
    updateMove(_dt,polyLine,edge,moveInput);

    doSlideDamage();

    m_parent->UpdatePhysicClimbingEdge(_dt,m_animComponent,m_characterPhys);
    m_parent->m_orientationPose = m_animVertical ? ORIENTATION_VERTICAL : ORIENTATION_HORIZONTAL;
    m_justEntered = bfalse;
    m_parent->setMoveInput(moveInput);
}

void War_PlayerControllerComponent::StateClimbing::processJump()
{
    m_parent->m_queueJump = bfalse;
    Vec2d gravNorm = m_parent->m_currentGravityDir.getPerpendicular();
    f32 pushSize = gravNorm.dot(m_parent->getCurrentMove());
    m_parent->m_jumpForceMultiplier = m_parent->m_climbEdgeJumpMultiplier;
    m_characterPhys->addForce(gravNorm*pushSize*m_parent->m_climbVerticalJumpImpulse);
    m_parent->changeState(&m_parent->m_stateJumping);
}

void War_PlayerControllerComponent::StateClimbing::updateLookDir()
{
    if ( !m_parent->isPerformingUTurn() )
    {
        m_parent->updateLookRightFromMoveDirection(m_parent->m_moveDirection);
    }
}

// Sends an impulse event on the polyline so that we can swing on it
void War_PlayerControllerComponent::StateClimbing::updateSwing( f32 _dt, const PolyLine* _polyLine, Vec2d& _moveInput )
{
    f32 impulse = m_parent->getCurrentMove().norm() * m_parent->m_climbEdgeSwingImpulse;

    if ( !impulse ||
         !_polyLine->getOwnerActor() ||
       ( m_parent->m_moveDirection != MOVEDIR_RIGHT &&
         m_parent->m_moveDirection != MOVEDIR_LEFT ) )
    {
        m_stopSwingTimer = Max(m_stopSwingTimer-_dt,0.f);

        if ( !m_stopSwingTimer )
        {
            m_swingDir = SWINGDIR_NONE;

            if ( m_parent->isPerformingUTurn() )
            {
                m_parent->resetUTurn();
            }
        }

        return;
    }

    m_stopSwingTimer = s_swingStopTimer;

    if ( m_parent->isPerformingUTurn() && isCurrentAnimFinished() )
    {
        m_parent->resetUTurn();
    }

    ESwingDir newSwingDir;

    if ( m_parent->m_moveDirection == MOVEDIR_LEFT )
    {
        impulse *= -1.f;
        newSwingDir = SWINGDIR_LEFT;
    }
    else
    {
        newSwingDir = SWINGDIR_RIGHT;
    }

    if ( !m_parent->isPerformingUTurn() )
    {
        if ( newSwingDir != m_swingDir &&
             newSwingDir != SWINGDIR_NONE &&
             m_swingDir != SWINGDIR_NONE )
        {
            m_parent->startUTurn();
        }

        m_swingDir = newSwingDir;
    }

    Vec2d edgeDir = _polyLine->getEdgeAt(m_parent->m_climbingEdge).m_normalizedVector.getPerpendicular();

    EventPolylineImpulse eventImpulse(_polyLine->getRef(),m_parent->m_climbingEdge,
        m_parent->m_climbingT,m_characterPhys->getWeight(),
        edgeDir*impulse);

    _polyLine->getOwnerActor()->onEvent(&eventImpulse);
    _moveInput.m_x = impulse;
}

// Moves the character on the polyline. It will return true if it drops the polyline
void War_PlayerControllerComponent::StateClimbing::updateMove( f32 _dt, const PolyLine* _polyLine, const PolyLineEdge* _edge, Vec2d& _moveInput )
{
    // apply the movement based on the input
    f32 move, moveSign;
    EMoveDir climbDir;
    applyMove(_dt,_edge,move,moveSign,climbDir);

    // adjust the position on the rope. move will keep how much we moved this frame
    adjustClimbT(_polyLine,_edge,move);

    setAnimData(_dt,_edge,move,moveSign,_moveInput);

    //// Re-get the edge in case it changed
    //_edge = &_polyLine->getEdgeAt(m_parent->m_climbingEdge);

    if ( _polyLine->getOwnerActor() )
    {
        // Update the position of the actor on the rope
        EventHanging event(m_parent->GetActor()->getRef(),
            m_parent->m_climbingPolyline,
            m_parent->m_climbingEdge,
            m_characterPhys->getWeight(),
            m_characterPhys->getSpeed(),
            m_parent->m_climbingT,
            btrue);
        _polyLine->getOwnerActor()->onEvent(&event);
    }

    if ( m_inputVertical )
    {
        if ( m_allowDropVertical )
        {
            if ( m_parent->m_climbingT != 1.f ||
                 m_parent->m_climbingEdge != (_polyLine->getVectorsCount()-1) )
            {
                m_allowDropVertical = bfalse;
            }
        }
        else
        {
            if ( m_parent->m_climbingT == 1.f &&
                 m_parent->m_climbingEdge == (_polyLine->getVectorsCount()-1) &&
                 m_parent->m_moveDirection == MOVEDIR_NONE )
            {
                m_allowDropVertical = btrue;
            }
        }
    }
    else
    {
        if ( !m_allowExitHorizontal && m_parent->m_moveDirection != MOVEDIR_DOWN )
        {
            m_allowExitHorizontal = btrue;
        }
    }
}

War_PlayerControllerComponent::EMoveDir War_PlayerControllerComponent::StateClimbing::getClimbDirFromMoveSign( f32 _moveSign, bbool _vertical,
                                                                             const class PolyLineEdge* _edge ) const
{
    EMoveDir climbDir;

    if ( !_moveSign )
    {
        climbDir = MOVEDIR_NONE;
    }
    else if ( _vertical )
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_parent->m_currentGravityDir);

        if ( dotP > 0.f )
        {
            // The polyline is going down

            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_UP;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_DOWN;
            }
        }
        else
        {
            // The polyline is going up
            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_DOWN;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_UP;
            }
        }
    }
    else
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_parent->m_currentGravityDir.getPerpendicular());

        if ( dotP > 0.f )
        {
            // The polyline is going right

            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_LEFT;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_RIGHT;
            }
        }
        else
        {
            // The polyline is going left
            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_RIGHT;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_LEFT;
            }
        }
    }

    return climbDir;
}

f32 War_PlayerControllerComponent::StateClimbing::getClimbSignFromClimbDir( EMoveDir _moveDir, bbool _vertical, const class PolyLineEdge* _edge ) const
{
    f32 moveSign;

    if ( _moveDir == MOVEDIR_NONE )
    {
        moveSign = 0.f;
    }
    else if ( _vertical )
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_parent->m_currentGravityDir);

        if ( _moveDir == MOVEDIR_DOWN )
        {
            // We want to go down. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going down, follow the direction
                moveSign = 1.f;
            }
            else
            {
                // If the polyline is going up, we go in the opposite direction
                moveSign = -1.f;
            }
        }
        else if ( _moveDir == MOVEDIR_UP )
        {
            // We want to go up. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going down, we go in the opposite direction
                moveSign = -1.f;
            }
            else
            {
                // If the polyline is going up, we go in the same direction
                moveSign = 1.f;
            }
        }
        else
        {
            moveSign = 0.f;
        }
    }
    else
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_parent->m_currentGravityDir.getPerpendicular());

        if ( _moveDir == MOVEDIR_RIGHT )
        {
            // We want to go right. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going right, follow the direction
                moveSign = 1.f;
            }
            else
            {
                // If the polyline is going left, we go in the opposite direction
                moveSign = -1.f;
            }
        }
        else if ( _moveDir == MOVEDIR_LEFT )
        {
            // We want to go left. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going right, we go in the opposite direction
                moveSign = -1.f;
            }
            else
            {
                // If the polyline is going left, follow the direction
                moveSign = 1.f;
            }
        }
        else
        {
            moveSign = 0.f;
        }
    }

    return moveSign;
}

void War_PlayerControllerComponent::StateClimbing::getMoveData( const PolyLineEdge* _edge,
                                                            f32& _moveSign, EMoveDir& _climbDir ) const
{
    if ( m_parent->m_moveSameDir )
    {
        // If we move in the same dir, we find out the data from the previous move sign
        _moveSign = m_prevMoveSign;
        _climbDir = getClimbDirFromMoveSign(_moveSign,m_inputVertical,_edge);
    }
    else
    {
        _climbDir = m_parent->m_moveDirection;
        _moveSign = getClimbSignFromClimbDir(_climbDir,m_inputVertical,_edge);
    }
}

f32 War_PlayerControllerComponent::StateClimbing::getMoveSize( f32 _deltaTime, f32 _moveDir, EMoveDir _climbDir ) const
{
    f32 move;

    if ( m_inputVertical )
    {
        if ( _climbDir != MOVEDIR_NONE )
        {
            if ( _climbDir == MOVEDIR_UP )
            {
                // Moving up
                move = m_parent->getCurrentMove().norm() * m_parent->m_climbVerticalSpeedUp;
            }
            else
            {
                // Descending, increase inertia
                move = m_moveInertia;
            }
        }
        else
        {
            move = m_moveInertia;
        }
    }
    else
    {
        if ( _climbDir != MOVEDIR_NONE )
        {
            // Moving left or right
            move = m_parent->getCurrentMove().norm() * m_parent->m_climbHorizontalSpeed;
        }
        else
        {
            move = m_moveInertia;
        }
    }

    return move*_moveDir;
}

void War_PlayerControllerComponent::StateClimbing::updateInertia( f32 _deltaTime, EMoveDir _climbDir )
{
    if ( m_inputVertical )
    {
        if ( _climbDir != MOVEDIR_NONE )
        {
            if ( _climbDir == MOVEDIR_UP )
            {
                m_moveInertia = 0.f;
            }
            else
            {
                // Descending, increase inertia
                f32 accel = m_parent->getCurrentMove().norm() * m_parent->m_climbVerticalDescendAccel;

                m_moveInertia += accel * _deltaTime;
                m_moveInertia = Min(m_moveInertia,m_parent->m_climbVerticalDescendMaxSpeed);
            }
        }
        else if ( m_moveInertia > 0.f )
        {
            m_moveInertia -= m_parent->m_climbEdgeInertiaBrake * _deltaTime;
            m_moveInertia = Max(m_moveInertia,0.f);
        }
        else if ( m_moveInertia < 0.f )
        {
            m_moveInertia += m_parent->m_climbEdgeInertiaBrake * _deltaTime;
            m_moveInertia = Min(m_moveInertia,0.f);
        }
    }
    else
    {
        if ( _climbDir != MOVEDIR_NONE )
        {
            m_moveInertia = 0.f;
        }
        else if ( m_moveInertia > 0.f )
        {
            // Idle, apply inertia
            m_moveInertia -= m_parent->m_climbEdgeInertiaBrake * _deltaTime;
            m_moveInertia = Max(m_moveInertia,0.f);
        }
        else if ( m_moveInertia < 0.f )
        {
            m_moveInertia += m_parent->m_climbEdgeInertiaBrake * _deltaTime;
            m_moveInertia = Min(m_moveInertia,0.f);
        }
    }
}

void War_PlayerControllerComponent::StateClimbing::applyMove( f32 _dt, const PolyLineEdge* _edge, f32& _move, f32& _moveSign, EMoveDir& _climbDir )
{
    getMoveData(_edge,_moveSign,_climbDir);
    updateInertia(_dt,_climbDir);
    _move = getMoveSize(_dt,_moveSign,_climbDir);

    _move *= _dt;

    f32 currentDist = _edge->m_length * m_parent->m_climbingT;

    currentDist += _move;

    m_parent->m_climbingT = currentDist / _edge->m_length;
    m_prevMoveSign = _moveSign;
    m_prevClimbDir = _climbDir;
}

void War_PlayerControllerComponent::StateClimbing::setAnimData( f32 _dt, const class PolyLineEdge* _edge, f32 _move, f32 _moveSign, Vec2d& _moveInput )
{
    f32 currentMove = fabs(_move);

    if ( currentMove < MTH_EPSILON )
    {
        currentMove = 0.f;
        _moveSign = 0.f;
    }

    EMoveDir animMoveDir = getClimbDirFromMoveSign(_moveSign,m_animVertical,_edge);
    Vec2d speed;

    switch(animMoveDir)
    {
    case MOVEDIR_UP:
        speed = (-m_parent->m_currentGravityDir)*fabs(_move);
        _moveInput.m_y = currentMove;
        m_animRate = fabs(_move/_dt) / m_parent->m_climbVerticalUpAnimRate;
        break;
    case MOVEDIR_DOWN:
        speed = (m_parent->m_currentGravityDir)*fabs(_move);
        _moveInput.m_y = -currentMove;
        m_animRate = 1.f;
        break;
    case MOVEDIR_LEFT:
        speed = (-m_parent->m_currentGravityDir.getPerpendicular())*fabs(_move);
        _moveInput.m_x = -currentMove;
        m_animRate = fabs(_move/_dt) / m_parent->m_climbHorizontalMoveAnimRate;
        break;
    case MOVEDIR_RIGHT:
        speed = (m_parent->m_currentGravityDir.getPerpendicular())*fabs(_move);
        _moveInput.m_x = currentMove;
        m_animRate = fabs(_move/_dt) / m_parent->m_climbHorizontalMoveAnimRate;
        break;
    default:
        speed = Vec2d::Zero;
        m_animRate = 1.f;
        break;
    }

    m_characterPhys->setSpeed(speed/_dt);
}

void War_PlayerControllerComponent::StateClimbing::adjustClimbT( const PolyLine* _polyLine, const PolyLineEdge* _edge, f32& _move )
{
    if ( m_parent->m_climbingT < 0.f )
    {
        while ( m_parent->m_climbingT < 0.f )
        {
            if ( m_parent->m_climbingEdge == 0 )
            {
                // We are going out of the polyline, so stop
                // Remove the extra movement from the move counter
                f32 extra = _edge->m_length * -m_parent->m_climbingT;

                if ( _move > 0.f )
                {
                    _move -= extra;
                }
                else if ( _move < 0.f )
                {
                    _move += extra;
                }

                m_parent->m_climbingT = 0.f;
            }
            else
            {
                f32 dist = _edge->m_length * m_parent->m_climbingT;
                m_parent->m_climbingEdge = m_parent->m_climbingEdge - 1;
                _edge = &_polyLine->getEdgeAt(m_parent->m_climbingEdge);
                m_parent->m_climbingT = ( _edge->m_length + dist ) / _edge->m_length;
            }
        }
    }
    else if ( m_parent->m_climbingT > 1.f )
    {
        while ( m_parent->m_climbingT > 1.f )
        {
            if ( m_parent->m_climbingEdge == _polyLine->getVectorsCount()-1 )
            {
                // We are going out of the polyline, so stop
                // Remove the extra movement from the move counter
                f32 extra = _edge->m_length * (m_parent->m_climbingT-1.f);

                if ( _move > 0.f )
                {
                    _move -= extra;
                }
                else if ( _move < 0.f )
                {
                    _move += extra;
                }

                m_parent->m_climbingT = 1.f;
            }
            else
            {
                f32 dist = _edge->m_length * (m_parent->m_climbingT - 1.f);
                m_parent->m_climbingEdge = m_parent->m_climbingEdge + 1;
                _edge = &_polyLine->getEdgeAt(m_parent->m_climbingEdge);
                m_parent->m_climbingT = dist / _edge->m_length;
            }
        }
    }
}

void War_PlayerControllerComponent::StateClimbing::updateOrientation( f32 _dt )
{
    f32 targetAngle = getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular());
    f32 currentAngle = m_actor->getAngle();
    f32 dif = getShortestAngleDelta(currentAngle,targetAngle);

    if ( dif )
    {
        f32 move = m_parent->m_climbEdgeAngularSpeed.ToRadians() * _dt;

        if ( fabs(dif) < move )
        {
            currentAngle = targetAngle;
        }
        else
        {
            if ( dif > 0.f )
            {
                currentAngle += move;
            }
            else
            {
                currentAngle -= move;
            }
        }

        m_actor->setAngle(currentAngle);
    }
}

void War_PlayerControllerComponent::StateClimbing::onEvent(Event * _event)
{
    if ( AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == War_MRK_Up_Start )
        {
            sendMoveImpulseEvent();
        }
    }
}

void War_PlayerControllerComponent::StateClimbing::sendMoveImpulseEvent()
{
    const BaseObject* pObj = GETOBJECT(m_parent->m_climbingPolyline);
    const PolyLine* polyLine;

    if ( pObj )
    {
        ITF_ASSERT(pObj->IsClass(PolyLine::GetClassNameStatic()));

        polyLine = static_cast<const PolyLine*>(pObj);

        if ( polyLine->getOwnerActor() && m_parent->m_climbingEdge < polyLine->getPosCount() )
        {
            Vec2d impulseDir = Vec2d::Right.Rotate(m_actor->getAngle()) * m_parent->m_climbEdgeMoveImpulse;

            if ( !m_parent->m_lookRight )
            {
                impulseDir *= -1.f;
            }

            EventPolylineImpulse impulseEvent(m_parent->m_climbingPolyline,
                m_parent->m_climbingEdge,
                m_parent->m_climbingT,
                m_characterPhys->getWeight(),
                impulseDir);

            polyLine->getOwnerActor()->onEvent(&impulseEvent);
        }
    }
}

void War_PlayerControllerComponent::StateClimbing::doSlideDamage()
{
    if ( m_animVertical && m_moveInertia >= m_parent->m_slideDownHitMinSpeed )
    {
        War_HitStim* stim = STIMSMANAGER->requestStim<War_HitStim>(&m_parent->m_slideDownHitShape);

#if defined(ITF_FINAL)
        if(stim)
#endif // ITF_FINAL
        {
            stim->setPos(m_actor->get2DPos());
            stim->setPrevPos(m_actor->get2DPos());
            stim->setDepth(m_actor->getDepth());
            stim->setSender(m_actor->getRef());
            stim->setLevel(m_parent->m_slideDownHitLevel);
            stim->setReceivedHitType(WAR_RECEIVEDHITTYPE_NORMALHIT);
            stim->setFxPos(m_actor->getPos());

            f32 angle = getVec2Angle(m_parent->m_currentGravityDir);
            stim->setAngle(angle);
            stim->setDirection(m_parent->m_currentGravityDir);

            STIMSMANAGER->sendStim(stim);
        }
    }
}

void War_PlayerControllerComponent::StateClimbing::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setPlayRate(m_animRate);
}

}
