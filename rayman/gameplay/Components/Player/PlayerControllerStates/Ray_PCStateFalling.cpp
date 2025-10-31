#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateFalling)

////////////////////
// Fall State
////////////////////
Ray_PlayerControllerComponent::StateFalling::StateFalling()
: m_airPose(AIRPOSE_MIDDLE)
, m_fallAngleCursor(0.f)
, m_initialDif(0.f)
, m_fallCounter(0.f)
, m_keepMiddlePose(btrue)
, m_justEntered(bfalse)
, m_skipResetUTurn(bfalse)
, m_jumpReleased(bfalse)
{
}

void Ray_PlayerControllerComponent::StateFalling::onEnter()
{
    Super::onEnter();

    m_parent->stopOnPedestal();
    m_parent->m_wallSlideAirControl = bfalse;
    m_parent->setStance(STANCE_STAND);
    m_characterPhys->setStickOnWalls(bfalse);

    if ( m_skipResetUTurn )
    {
        m_skipResetUTurn = bfalse;
    }
    else
    {
        m_parent->resetUTurn();
    }

    m_justEntered = btrue;
    m_fallAngleCursor = 0.f;
    m_fallCounter = 0.f;
    m_airPose = AIRPOSE_MIDDLE;

    f32 gravityAngle = getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular());
    m_initialDif = f32_Abs(getShortestAngleDelta(m_actor->getAngle(),gravityAngle));

    if ( m_parent->isSupporting() )
    {
        m_parent->setSupporting(bfalse);
    }

    m_keepMiddlePose = m_parent->m_prevStance != STANCE_SWIM &&
                       m_parent->m_prevState != &m_parent->m_stateJumping;

    m_jumpReleased = bfalse;
}

void Ray_PlayerControllerComponent::StateFalling::onExit()
{
    Super::onExit();

    if ( m_parent->m_nextState == &m_parent->m_stateIdle )
    {
        m_parent->m_unCrouchCounter = m_parent->getTemplate()->getCrouchUnhangTime(); //#749
    }

    m_parent->resetGravityMode(GRAVITYMODE_SWIMDOLPHIN);
    m_parent->setSlide(bfalse);
    m_parent->m_fallFromRoof = bfalse;
}

bbool Ray_PlayerControllerComponent::StateFalling::checkStateChange( f32 _dt )
{
    if ( m_parent->isOnPedestal() )
    {
        m_parent->changeState(&m_parent->m_stateOnPedestal);
        return btrue;
    }

    if ( checkBounce() )
    {
        return btrue;
    }

    if  ( m_parent->isHitQueued() )
    {
        if ( m_parent->setAirPunch() )
        {
            return btrue;
        }
    }

    if ( !m_justEntered && m_characterPhys->getStickedPolyline())
    { 
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    if ( !m_justEntered &&
          m_parent->isJumpQueued() )
    {
        if ( m_airPose == AIRPOSE_SURFING )
        {
            m_parent->setJumpData();
            m_parent->changeState(&m_parent->m_stateJumping);
            m_parent->setSurfingTimer();
            return btrue;
        }

        if ( m_parent->m_jumpOnAirTimer )
        {
            m_parent->setJumpData();

            if ( m_parent->m_prevStance == STANCE_WALLSLIDE )
            {
                m_parent->m_wallSlideDir = m_parent->m_jumpOnAirEdge;
                m_parent->m_performingUTurn = UTURNTYPE_GROUND;
                m_parent->m_currentJumpUseWallDir = btrue;
                m_parent->m_uTurnComboCounter = m_parent->getTemplate()->getTurnComboTime();
            }

            m_parent->changeState(&m_parent->m_stateJumping);
            return btrue;
        }
        else if ( m_parent->tryWallJump(bfalse) )
        {
            return btrue;
        }
    }

    if ( !m_justEntered &&
          m_characterPhys->getSpeed().dot(m_parent->m_currentGravityDir) >= 0.f &&
       ( m_parent->shouldTriggerHelicopterAction() ||
       ( m_jumpReleased && m_parent->shouldTriggerHelicoFromHold() ) ) )
    {
        m_parent->changeState(&m_parent->m_stateHelicopter);
        return btrue;
    }

    if ( !m_justEntered )
    {
        HangingInfo info;

        if ( m_parent->tryHanging(info) )
        {
            m_parent->setHangState(info);
            return btrue;
        }
    }

    if ( m_parent->tryWallSlide(_dt) )
    {
        m_parent->changeState(&m_parent->m_stateWallSlide);
        return btrue;
    }

    if ( m_parent->checkFallOnCharacter() )
    {
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateFalling::update( f32 _dt )
{
    if ( !m_parent->m_fallFromRoof )
    {
        m_parent->setDesiredLookDir();
    }

    m_parent->UpdatePhysicAir();

    if ( m_keepMiddlePose )
    {
        if ( !m_justEntered && !m_animComponent->isInTransition() )
        {
            m_keepMiddlePose = bfalse;
        }
        
        m_parent->setMoveInput(Vec2d::Zero);
    }

    m_fallCounter += _dt;

    if ( m_fallCounter >= m_parent->getTemplate()->getSlideFallDelay() )
    {
        m_parent->setSlide(bfalse);
    }

    updateSurfing();

    if ( m_airPose != AIRPOSE_SURFING && m_airPose != AIRPOSE_HOVER && ( !m_parent->isPerformingUTurn() || m_parent->m_uTurnFlipped ) && !m_keepMiddlePose )
    {
        if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_UP )
        {
            m_airPose = AIRPOSE_UP;
        }
        else if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN )
        {
            m_airPose = AIRPOSE_DOWN;
        }
        else
        {
            m_airPose = AIRPOSE_MIDDLE;
        }
    }

    if ( !m_jumpReleased )
    {
        m_jumpReleased = m_parent->m_listenerJump == bfalse &&
                         m_parent->m_listenerJumpHold == bfalse;
    }

    m_justEntered = bfalse;
}

void Ray_PlayerControllerComponent::StateFalling::updateSurfing()
{
    bbool checkHoover = (-m_parent->m_currentGravityDir).dot(m_characterPhys->getSpeed()) < m_parent->getTemplate()->getHoverMinYSpeed();
    bbool setHover = bfalse;

    Ray_EventQueryCanJumpFromCurrent queryJump(m_actor->get2DPos());
    queryJump.setSender(m_actor->getRef());

    u32 numForces = m_parent->m_currentWindForces.size();
    for ( u32 i = 0; i < numForces; i++ )
    {
        PhysActorForce& wind = m_parent->m_currentWindForces[i];

        if ( m_actor->getBinding()->isDescendant(wind.m_actor) )
        {
            continue;
        }

        Actor* actorCurrent = AIUtils::getActor(wind.m_actor);

        if ( !actorCurrent )
        {
            continue;
        }

        actorCurrent->onEvent(&queryJump);

        if ( queryJump.getCanJump() )
        {
            m_airPose = AIRPOSE_SURFING;
            return;
        }

        if ( checkHoover && m_parent->m_currentGravityDir.dot(wind.m_force) < 0.f )
        {
            setHover = btrue;
        }
    }

    if ( setHover )
    {
        m_airPose = AIRPOSE_HOVER;
    }
    else if ( m_airPose == AIRPOSE_SURFING || m_airPose == AIRPOSE_HOVER )
    {
        m_airPose = AIRPOSE_MIDDLE;
    }
}

void Ray_PlayerControllerComponent::StateFalling::updateAnimInputs()
{
    Super::updateAnimInputs();

    if ( m_initialDif )
    {
        f32 gravityAngle = getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular());
        f32 currentDif = f32_Abs(getShortestAngleDelta(m_actor->getAngle(),gravityAngle));
        m_fallAngleCursor = 1.f - ( currentDif / m_initialDif );
    }
    else
    {
        m_fallAngleCursor = 1.f;
    }

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_FALLANGLECURSOR],m_fallAngleCursor);
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_AIRPOSE],m_airPose);
}

}