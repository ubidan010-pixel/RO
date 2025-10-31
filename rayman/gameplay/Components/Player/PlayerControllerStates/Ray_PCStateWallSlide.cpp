#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateWallSlide)

////////////////////
// Wall Slide State
////////////////////
Ray_PlayerControllerComponent::StateWallSlide::StateWallSlide()
: m_targetAngle(0.f)
, m_startSpeed(0.f)
, m_brakeTimer(0.f)
, m_unstickFromPushCounter(0.f)
, m_wallSlideDelay(0.f)
, m_justEntered(bfalse)
, m_keepImpulse(bfalse)
, m_fromHang(bfalse)
, m_delayApplied(bfalse)
{
}

void Ray_PlayerControllerComponent::StateWallSlide::onEnter()
{
    Super::onEnter();

    f32 speedProj = m_characterPhys->getSpeed().dot(m_parent->m_currentGravityDir);

    if ( m_parent->m_stance != STANCE_WALLSLIDE )
    {
        if ( m_parent->m_stance != STANCE_HANG )
        {
            if ( speedProj >= 0.f && speedProj <= m_parent->getTemplate()->getWallSlideStartDelaySpeedThreshold() )
            {
                m_wallSlideDelay = m_parent->getTemplate()->getWallSlideStartDelay();

                if ( m_wallSlideDelay )
                {
                    m_characterPhys->setDisabled(btrue,bfalse,bfalse);
                }

                m_delayApplied = btrue;
            }
            else
            {
                m_delayApplied = bfalse;
            }
        }
        else
        {
            m_delayApplied = btrue;
        }

        m_parent->setStance(STANCE_WALLSLIDE);
    }
    else
    {
        m_delayApplied = btrue;
    }

    m_justEntered = btrue;
    m_unstickFromPushCounter = m_parent->getTemplate()->getWallSlideUnstickDelay();

    f32 currentSpeed = m_characterPhys->getSpeed().norm();

    m_startSpeed = currentSpeed;

    if ( currentSpeed < m_parent->getTemplate()->getWallSlideStartSpeed() || speedProj < 0.f )
    {
        // we don't need to brake
        m_brakeTimer = 0.f;
    }
    else
    {
        m_brakeTimer = m_parent->getTemplate()->getWallSlideBrakeTime();
    }

    PolyLine* poly;
    u32 edgeIndex;

    m_fromHang = m_parent->m_prevStance == STANCE_HANG;
    m_parent->findWallSlidingPolyline(m_polyLine,m_edge,&poly,&edgeIndex,m_fromHang,bfalse);

    if ( !poly )
    {
        return;
    }

    m_parent->setFrictionMode(FRICTIONMODE_WALLSLIDE,0.f);

    m_characterPhys->forceStickOnEdge(poly,&poly->getEdgeAt(edgeIndex),edgeIndex);
    m_characterPhys->setStickOnWalls(btrue,bfalse,bfalse);
    m_characterPhys->setUpdateRotation(bfalse);
    m_characterPhys->setWindMultiplier(0.f);

    const PolyLineEdge & stickedEdge= poly->getEdgeAt(edgeIndex);
    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(stickedEdge.getGameMaterial()));

    m_parent->startWallLandFx(mat);
    m_parent->startWallSlideFx(poly,edgeIndex);
    
    if ( m_parent->m_actionPose == ACTIONPOSE_FIGHT )
    {
        m_parent->setActionPose(ACTIONPOSE_STAND);
    }

    if ( m_parent->m_prevState == &m_parent->m_stateJumping &&
         m_parent->m_stateJumping.allowSuspension() )
    {
        m_keepImpulse = btrue;
    }
    else
    {
        m_keepImpulse = bfalse;
    }
}

void Ray_PlayerControllerComponent::StateWallSlide::onExit()
{
    Super::onExit();

    if ( m_wallSlideDelay )
    {
        m_characterPhys->setDisabled(bfalse,bfalse,bfalse);
        m_wallSlideDelay = 0.f;
    }
    
    m_parent->resetFrictionMode(FRICTIONMODE_WALLSLIDE);
    m_parent->stopWallSlideFx();
    m_characterPhys->setUpdateRotation(btrue);
    m_parent->resetGravityMode(GRAVITYMODE_WALLSLIDE);
    m_characterPhys->setWindMultiplier(1.f);
    m_characterPhys->setStickOnWalls(bfalse);
    m_characterPhys->setUpdateRotation(btrue);
    m_polyLine = ITF_INVALID_OBJREF;
    m_edge = U32_INVALID;
}

bbool Ray_PlayerControllerComponent::StateWallSlide::checkStateChange( f32 _dt )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    if ( m_parent->isHitQueued() )
    {
        if ( m_parent->setAirPunch() )
        {
            m_parent->setPrevWallSlide(m_polyLine,m_edge);
            return btrue;
        }
    }

    if ( !m_justEntered &&
          m_parent->isJumpQueued() )
    {
        if ( m_parent->m_wallJumpDisableTimer )
        {
            m_parent->resetStance(STANCE_STAND);
        }
        else
        {
            m_parent->startUTurn(UTURNTYPE_GROUND);
        }

        m_parent->setJumpData();
        m_parent->changeState(&m_parent->m_stateJumping);

        if ( PolyLine* wallJumpPolyline = AIUtils::getPolyLine(m_characterPhys->getStickedPolylineRef()) )
        {
            if (wallJumpPolyline->m_ownerFrise)
            {
                EventWallJump eventWallJump;
                eventWallJump.setSender(m_actor->getRef());
                wallJumpPolyline->m_ownerFrise->onEvent(&eventWallJump);
            }
            else if (wallJumpPolyline->getOwnerActor())
            {
                EventWallJump eventWallJump;
                eventWallJump.setSender(m_actor->getRef());
                wallJumpPolyline->getOwnerActor()->onEvent(&eventWallJump);
            }
        }

        return btrue;
    }

    PolyLine* poly;
    const PolyLineEdge* stickedEdge;
    
    AIUtils::getPolyLine(m_characterPhys->getStickedPolylineRef(),m_characterPhys->getStickedEdgeIndex(),poly,stickedEdge);

    if ( poly && stickedEdge )
    {
        if ( !m_parent->isPolylineEdgeGoodForWallSlide(poly,stickedEdge,m_characterPhys->getStickedEdgeIndex(),m_fromHang) )
        {
            StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(stickedEdge->m_normalizedVector);

            if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
            { 
                m_parent->changeState(&m_parent->m_stateIdle);
                return btrue;
            }
            else
            {
                poly = NULL;
                stickedEdge = NULL;
            }
        }
        else
        {
            if ( !m_justEntered )
            {
                u32 numContacts = m_characterPhys->getContacts().size();

                for ( u32 i = 0; i < numContacts; i++ )
                {
                    const SCollidableContact& contact = m_characterPhys->getContacts()[i];
                    
                    if ( contact.m_collidableUserData == m_characterPhys->getStickedPolylineRef() &&
                         contact.m_edgeIndex == m_characterPhys->getStickedEdgeIndex() )
                    {
                        continue;
                    }

                    Vec2d contactDir = contact.m_normal.getPerpendicular();

                    StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(contactDir);

                    if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
                    {
                        poly = NULL;
                        stickedEdge = NULL;
                    }
                }
            }
        }
    }

    if ( !poly || !stickedEdge )
    {
        m_characterPhys->unstick();

        f32 ySpeed = -(m_characterPhys->getSpeed().dot(m_parent->m_currentGravityDir));

        if ( ySpeed > 0.f )
        {
            HangingInfo info;
            bbool hang = ySpeed <= m_parent->getTemplate()->getWallSlideYSpeedThresholdToHang();

            if ( hang )
            {
                hang = m_parent->tryHanging(info);
            }

            if ( hang )
            {
                m_parent->setHangState(info);
            }
            else
            {
                m_parent->m_moveInput.m_y = 0.f;
                m_parent->m_stateJumping.setSkipForce(btrue);
                m_parent->changeState(&m_parent->m_stateJumping);
            }

            return btrue;
        }
        else
        {
            // Reset this so that we have a nice transition from fall to fall up or down
            m_parent->m_moveInput.m_y = 0.f;
            m_parent->m_jumpOnAirTimer = m_parent->getTemplate()->getJumpOnAirTime();
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }

    m_parent->m_wallSlideDir = stickedEdge->m_normalizedVector;

    if ( m_unstickFromPushCounter == 0.f )
    {
        m_parent->m_wallSlideStickDelayCounter = m_parent->getTemplate()->getWallSlideRestickDelay();
        m_parent->m_moveInput.m_y = 0.f;
        m_parent->startUTurn(UTURNTYPE_AIR);
        m_parent->m_stateFalling.setSkitResetUTurn();
        m_parent->m_jumpOnAirTimer = m_parent->getTemplate()->getJumpOnAirTime();
        m_characterPhys->unstick();
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_parent->isOnGeyser() )
    {
        m_parent->m_moveInput.m_y = 0.f;
        m_parent->m_jumpOnAirTimer = m_parent->getTemplate()->getJumpOnAirTime();
        m_characterPhys->unstick();
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_parent->checkFallOnCharacter() )
    {
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateWallSlide::update(f32 _dt)
{
    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

    if ( !edge )
    {
        return;
    }

    if ( m_wallSlideDelay > 0.f )
    {
        f32 rot = calculateRotation(_dt,edge);

        m_actor->setAngle(rot);
        m_wallSlideDelay = Max(m_wallSlideDelay-_dt,0.f);

        if ( m_wallSlideDelay == 0.f )
        {
            m_characterPhys->setDisabled(bfalse,bfalse,bfalse);
        }
    }
    else
    {
        f32 rot = calculateRotation(_dt,edge);
        Vec2d speed = calculateSpeed(_dt,edge);

        f32 speedProj = speed.dot(m_parent->m_currentGravityDir);

        if ( !m_delayApplied )
        {
            if ( speedProj >= 0.f && speedProj <= m_parent->getTemplate()->getWallSlideStartDelaySpeedThreshold() )
            {
                m_wallSlideDelay = m_parent->getTemplate()->getWallSlideStartDelay();

                if ( m_wallSlideDelay )
                {
                    m_characterPhys->setDisabled(btrue,bfalse,bfalse);
                    speed = Vec2d::Zero;
                    speedProj = 0.f;
                }

                m_delayApplied = btrue;
            }
        }

        m_characterPhys->setSpeed(speed);
        m_actor->setAngle(rot);

        if ( speedProj < 0.f )
        {
            m_parent->setGravityMode(GRAVITYMODE_WALLSLIDE,1.f);
        }
        else
        {
            m_parent->setGravityMode(GRAVITYMODE_WALLSLIDE,m_parent->getTemplate()->getWallSlideGravityMultiplier());
        }

        if ( m_wallSlideDelay == 0.f )
        {
            if ( m_brakeTimer )
            {
                m_characterPhys->addForce(-m_parent->m_currentGravity*m_parent->getTemplate()->getWallSlideGravityMultiplier());
            }

            if ( m_keepImpulse )
            {
                if ( m_parent->m_listenerJumpHold )
                {
                    m_parent->addJumpAirImpulse();
                }
                else
                {
                    m_keepImpulse = bfalse;
                }
            }

            if ( ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_RIGHT && m_actor->isFlipped() ) ||
                 ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_LEFT && !m_actor->isFlipped() ) )
            {
                m_unstickFromPushCounter = Max(m_unstickFromPushCounter-_dt,0.f);
            }
            else
            {
                m_unstickFromPushCounter = m_parent->getTemplate()->getWallSlideUnstickDelay();
            }
        }
    }

    m_justEntered = bfalse;
    m_parent->m_jumpOnAirEdge = edge->m_normalizedVector;
}

Vec2d Ray_PlayerControllerComponent::StateWallSlide::calculateSpeed( f32 _dt, const PolyLineEdge* _edge )
{
    Vec2d speed = m_characterPhys->getSpeed();
    f32 totalSpeed = speed.dot(_edge->m_normalizedVector);

    speed = _edge->m_normalizedVector * totalSpeed;

    if ( m_brakeTimer )
    {
        // Braking
        m_brakeTimer = Max(m_brakeTimer-_dt,0.f);
        f32 t = m_brakeTimer / m_parent->getTemplate()->getWallSlideBrakeTime();
        f32 dif = m_startSpeed - m_parent->getTemplate()->getWallSlideStartSpeed();
        f32 speedValue = m_parent->getTemplate()->getWallSlideStartSpeed() + ( dif * t );
        Vec2d speedDir;

        if ( _edge->m_normalizedVector.dot(m_parent->m_currentGravityDir) > 0.f )
        {
            speedDir = _edge->m_normalizedVector;
        }
        else
        {
            speedDir = -_edge->m_normalizedVector;
        }

        speed = speedDir*speedValue;
    }

    if ( speed.dot(m_parent->m_currentGravityDir) > 0.f &&
         speed.sqrnorm() > ( m_parent->getTemplate()->getWallSlideMaxSpeed() * m_parent->getTemplate()->getWallSlideMaxSpeed() ) )
    {
        speed.normalize();
        speed *= m_parent->getTemplate()->getWallSlideMaxSpeed();
    }

    return speed;
}

f32 Ray_PlayerControllerComponent::StateWallSlide::calculateRotation( f32 _dt, const PolyLineEdge* _edge )
{
    Vec2d edgeDir;

    if ( _edge->m_normalizedVector.dot(m_parent->m_currentGravityDir) > 0.f )
    {
        edgeDir = _edge->m_normalizedVector.getPerpendicular();
        m_parent->setLookRight(bfalse);
    }
    else
    {
        edgeDir = -_edge->m_normalizedVector.getPerpendicular();
        m_parent->setLookRight(btrue);
    }

    m_targetAngle = getVec2Angle(edgeDir);

    f32 angle = m_actor->getAngle();

    if ( angle != m_targetAngle )
    {
        if ( m_justEntered )
        {
            angle = m_targetAngle;
        }
        else
        {
            f32 angleDif = getShortestAngleDelta(m_actor->getAngle(),m_targetAngle);
            f32 angularSpeed = getAngularSpeed()*_dt;

            if ( f32_Abs(angleDif) < angularSpeed )
            {
                angle = m_targetAngle;
            }
            else
            {
                if ( angleDif > 0.f )
                {
                    angle = m_actor->getAngle() + angularSpeed;
                }
                else
                {
                    angle = m_actor->getAngle() - angularSpeed;
                }
            }
        }
    }

    return angle;
}

f32 Ray_PlayerControllerComponent::StateWallSlide::getAngularSpeed() const
{
    f32 currentSpeed = m_characterPhys->getSpeed().norm();

    currentSpeed = Clamp(currentSpeed,m_parent->getTemplate()->getWallSlideAngularSpeedMinLinear(),m_parent->getTemplate()->getWallSlideAngularSpeedMaxLinear());

    f32 t = ( currentSpeed - m_parent->getTemplate()->getWallSlideAngularSpeedMinLinear() ) / ( m_parent->getTemplate()->getWallSlideAngularSpeedMaxLinear() - m_parent->getTemplate()->getWallSlideAngularSpeedMinLinear() );
    
    return m_parent->getTemplate()->getWallSlideAngularSpeedMin().ToRadians() +
       ( ( m_parent->getTemplate()->getWallSlideAngularSpeedMax().ToRadians() - m_parent->getTemplate()->getWallSlideAngularSpeedMin().ToRadians() ) * t );
}

}