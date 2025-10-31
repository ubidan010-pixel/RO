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

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHitRelease)

////////////////////
// Release Hit State
////////////////////
Ray_PlayerControllerComponent::StateHitRelease::StateHitRelease()
: m_switchHitTimer(0.f)
, m_crushTravelTime(0.f)
, m_crushPrevSpeedY(0.f)
, m_crushPrevPos(Vec2d::Zero)
, m_crushStartPos(Vec2d::Zero)
, m_crushStuckCounter(0.f)
, m_crushDistanceLimit(0.f)
, m_crushWasBlockedCounter(0)
, m_crushWasBlocked(bfalse)
, m_dashCounter(0.f)
, m_dashTotal(0.f)
, m_dashAttackMoveDir(MOVEDIR_NONE)
, m_dashSpeedLastFrame(Vec2d::Zero)
, m_dashHadHitLastFrame(bfalse)
, m_preparingHit(bfalse)
, m_performingHit(bfalse)
, m_skipFirstReleaseHit(bfalse)
, m_justEntered(bfalse)
, m_canMove(bfalse)
, m_exitSoon(bfalse)
, m_permanentHit(bfalse)
, m_shakeCamReleased(bfalse)
, m_hitsReleased(bfalse)
, m_hitSomethingThisRound(bfalse)
, m_hitSomethingThisFrame(bfalse)
, m_crushReleased(bfalse)
, m_crushWasOnGround(bfalse)
, m_resetFrictionMultiplier(bfalse)
, m_inverseHitDir(bfalse)
, m_reducedHitDir(MOVEDIR_NONE)
, m_forceReducedHitUp(bfalse)
{
}

void Ray_PlayerControllerComponent::StateHitRelease::onEnter()
{
    Super::onEnter();

    m_canMove = bfalse;
    m_justEntered = btrue;
    m_exitSoon = bfalse;
    m_crushTravelTime = 0.f;
    m_preparingHit = bfalse;
    m_performingHit = bfalse;
    m_permanentHit = bfalse;
    m_shakeCamReleased = bfalse;
    m_hitsReleased = bfalse;
    m_hitSomethingThisRound = bfalse;
    m_crushReleased = bfalse;
    m_crushWasOnGround = bfalse;
    m_crushPrevSpeedY = 0.f;
    m_resetFrictionMultiplier = bfalse;
    m_hitSomethingThisFrame = bfalse;
    m_inverseHitDir = bfalse;
    m_parent->m_walkOnWallsDisableTimer = 0.f;

    if ( !m_parent->isClimbing() &&
         !m_parent->isHanging() &&
         m_parent->m_currentPunchHitType != PUNCHTYPE_CRUSH )
    {
        m_switchHitTimer = m_parent->getTemplate()->getSwitchHitBufferTime();
    }
    else
    {
        m_switchHitTimer = 0.f;
    }

    if ( m_parent->isPerformingUTurn() )
    {
        m_parent->resetUTurn();
    }

    if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH )
    {
        setupCrushHit();
    }
    else if ( m_parent->m_currentPunchHitType == PUNCHTYPE_REDUCEDSIZE )
    {
        if ( m_parent->isOnPedestal() )
        {
            m_parent->stopOnPedestal();
        }

        setupReducedSizeHit();
    }
    if ( m_parent->m_currentPunchHitType == PUNCHTYPE_TORNADO )
    {
        setupTornadoAttack();
    }
    else if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH )
    {
        m_parent->setFrictionMode(FRICTIONMODE_CRUSH,m_parent->getTemplate()->getCrushHitFriction());
    }
    
    if ( m_parent->m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
    {
        if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID ||
           ( m_parent->m_currentPunchHitType != PUNCHTYPE_UPPERKICK &&
             m_parent->m_currentPunchHitType != PUNCHTYPE_REPEATING ) )
        {
            m_parent->m_kungFuComboTarget.clear();
        }
    }

    if ( m_parent->m_stance == STANCE_HELICO )
    {
        m_parent->setStance(STANCE_STAND);
    }
    else if ( m_parent->m_stance == STANCE_HANG )
    {
        m_inverseHitDir = m_parent->m_moveInput.m_x < 0.f;
    }

    m_parent->m_currentPunchResult = PUNCHRELEASERESULT_NOIMPACT;
    m_parent->m_currentPunchDistance = 1.f;
    m_animComponent->resetCurTime();
    m_hitQueue.clear();
    m_memorizedHits.clear();
    m_shapePoints.clear();
    
    if ( m_parent->m_prevState != &m_parent->m_statePrepareHit )
    {
        bbool wallSlide = m_parent->m_stance == STANCE_WALLSLIDE;
        bbool reducedHitUp = m_parent->m_currentPunchHitType == PUNCHTYPE_REDUCEDSIZE && m_reducedHitDir == MOVEDIR_UP;

        if ( ( !m_characterPhys->getStickedEdge() || wallSlide ) &&
             m_parent->m_currentPunchHitType != PUNCHTYPE_CRUSH &&
            !reducedHitUp &&
           ( m_parent->m_stance == STANCE_STAND || wallSlide ) )
        {
            m_parent->preparePhysicAirHit();
        }
        // Reset the queue since we press hit to get here
        m_parent->m_hitQueueTimer = 0.f;
    }

    m_parent->m_bubblizerIdentifier = CURRENTFRAME;

#ifdef ITF_SUPPORT_ONLINETRACKING
	RAY_GAMEMANAGER->getOnlineTrackingManager()->onAttack();
#endif
}

void Ray_PlayerControllerComponent::StateHitRelease::onExit()
{
    Super::onExit();

    if ( m_resetFrictionMultiplier )
    {
        m_characterPhys->setUnstickMinFrictionMultiplier(1.f);
    }

    m_parent->resetFrictionMode(FRICTIONMODE_TORNADO);
    m_parent->resetFrictionMode(FRICTIONMODE_CRUSH);
    m_parent->resetFrictionMode(FRICTIONMODE_REDUCEDSIZEHIT);
    m_parent->resetGravityMode(GRAVITYMODE_REDUCEDSIZEHIT);
    m_parent->resetUTurn();
    m_characterPhys->setAirFrictionMultiplier(0.f);
    m_switchHitTimer = 0.f;
    m_forceReducedHitUp = bfalse;
    m_crushDistanceLimit = 0.f;
}

bbool Ray_PlayerControllerComponent::StateHitRelease::checkChangeStateWantsMove() const
{
    EMoveDir moveDirection = m_parent->getMoveData().getMoveDirection();
    bbool wantsToMove =
        moveDirection != MOVEDIR_NONE &&
        (
            m_parent->m_stance != STANCE_STAND ||
            (
                // If we are in stand stance don't take as an input down if we are not standing up
                !( ( moveDirection == MOVEDIR_DOWN && m_parent->isCrouch() ) ||
                ( moveDirection == MOVEDIR_UP && m_parent->m_currentPunchHitType == PUNCHTYPE_CHARGE && m_parent->m_currentPunchOrientation > 0.f ) )
            )
        );

    if ( !wantsToMove && m_parent->isCrouch() )
    {
        EMoveDir lookDir = m_parent->getLookDirection();

        if ( ( lookDir == MOVEDIR_RIGHT && m_actor->isFlipped() ) ||
             ( lookDir == MOVEDIR_LEFT && !m_actor->isFlipped() ) )
        {
            wantsToMove = btrue;
        }
    }

    bbool wallSlide = m_parent->m_stance == STANCE_WALLSLIDE;
    bbool exitToMove = wantsToMove &&
                      !wallSlide &&
                     ( m_parent->m_stance != STANCE_STAND || m_characterPhys->getStickedEdge() );

    return exitToMove;
}

bbool Ray_PlayerControllerComponent::StateHitRelease::checkStateChange( f32 _dt )
{
    if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH &&
        !m_parent->m_bounceTimer && m_parent->m_bounceCurrentDir != Vec2d::Zero )
    {
        processPerformCrush();
        return btrue;
    }

    if ( checkBounce() )
    {
        return btrue;
    }

    // We want to abort the release hit animation when the player tries to perform another action.
    // However there are special cases:
    // * If we are in the air there is no need to abort the animation since the character is floating and he can already move
    // * If we are on the ground but pressing a direction that doesn't move us there is no need to abort
    bbool changeState = bfalse;
    bbool exitToMove = checkChangeStateWantsMove();
    bbool pressedAction = m_parent->isJumpQueued() || m_parent->isHitQueued() || m_parent->shouldTriggerHelicopterAction();
    bbool exitSwim = bfalse;

    if ( m_parent->m_stance == STANCE_SWIM )
    {
        PolyLine* swimPolyline = AIUtils::getPolyLine(m_parent->m_swimingPolyline);

        if ( !swimPolyline )
        {
            exitSwim = btrue;
        }
        else
        {
            m_parent->swimingUpdatePolylines(swimPolyline,_dt);

            if ( m_parent->m_swimingSurfaceEdge == U32_INVALID )
            {
                exitSwim = btrue;
            }
        }
    }

    if ( m_canMove && ( exitToMove || pressedAction || m_exitSoon ) ) 
    {
        changeState = btrue;
    }
    else if ( isCurrentAnimFinished() )
    {
        changeState = btrue;
    }
    else if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH &&
            ( m_parent->isOnGeyser() ||
            ( m_shakeCamReleased &&
              m_characterPhys->getStickedEdge() == NULL ) ) )
    {
        changeState = btrue;
    }
    else if ( m_parent->m_stance == STANCE_HANG && !m_parent->isCurrentHangValid() )
    {
        changeState = btrue;
    }
    else if ( m_parent->m_stance == STANCE_SWIM && exitSwim )
    {
        changeState = btrue;
    }
    else if ( m_parent->m_stance == STANCE_CLIMB )
    {
        PolyLine* polyLine;
        const PolyLineEdge* edge;

        m_parent->getClimbEdgePolyline(&polyLine,&edge);

        Vec2d climbPos = edge->getPos() + (edge->m_vector*m_parent->m_climbingT);

        if ( m_parent->checkClimbCollision(m_parent->m_climbingPolyline,m_parent->m_climbingEdge,climbPos,getVec2Angle(m_parent->m_currentGravityDir.getPerpendicular())) )
        {
            changeState = btrue;
        }
    }
    else
    {
        switch(m_parent->m_currentPunchHitType)
        {
        case PUNCHTYPE_TORNADO:
            {
                if ( m_parent->isSwimming() )
                {
                    if ( checkShouldExitDashHit(m_parent->getTemplate()->getSwimAttackMinSpeed()) )
                    {
                        changeState = btrue;
                    }
                    else if ( m_parent->isPowerUpEnabled(Ray_PowerUp_Dive) && m_parent->trySwimSprintJump() )
                    {
                        changeState = btrue;
                        m_parent->setupSwimDolphinJump();
                        return btrue;
                    }
                }
                else
                {
                    if ( checkShouldExitDashHit(m_parent->getTemplate()->getTornadoMinSpeed()) )
                    {
                        changeState = btrue;
                    }
                    else if ( ( m_dashTotal - m_dashCounter ) > m_parent->getTemplate()->getTornadoAllowMoveTime() )
                    {
                        changeState = m_parent->isJumpQueued();
                    }
                }
            }
            break;
        case PUNCHTYPE_REDUCEDSIZE:
            {
                if ( checkShouldExitDashHit(m_parent->getTemplate()->getReducedSizeHitMinSpeed()) )
                {
                    changeState = btrue;
                }
            }
            break;
        default:
            changeState = bfalse;
            break;
        }
    }

    if ( changeState )
    {
        changeToNextState();
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHitRelease::changeToNextState()
{
    switch(m_parent->m_stance)
    {
    case STANCE_STAND:
        {
            if ( m_parent->isOnPedestal() )
            {
                m_parent->changeState(&m_parent->m_stateOnPedestal);
            }
            else if ( m_parent->isJumpQueued() &&
                    ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID ||
                      m_parent->m_jumpOnAirTimer ) )
            {
                m_parent->changeState(&m_parent->m_stateJumping);
            }
            else if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
            {
                m_parent->changeState(&m_parent->m_stateIdle);
            }
            else
            {
                if ( m_parent->shouldTriggerHelicoFromHold() ||
                     m_parent->shouldTriggerHelicopterAction() )
                {
                    m_parent->changeState(&m_parent->m_stateHelicopter);
                }
                else
                {
                    m_parent->changeState(&m_parent->m_stateFalling);
                }
            }
        }
        break;
    case STANCE_HANG:
        {
            if ( m_parent->isCurrentHangValid() )
            {
                m_parent->changeState(&m_parent->m_stateHanging);
            }
            else
            {
                m_parent->changeState(&m_parent->m_stateFalling);
            }
        }
        break;
    case STANCE_CLIMB:
        {
            m_parent->changeState(&m_parent->m_stateClimbing);
        }
        break;
    case STANCE_SWIM:
        {
            m_parent->changeState(&m_parent->m_stateSwimIdle);
        }
        break;
    case STANCE_WALLSLIDE:
        {
            PolyLine* poly;
            const PolyLineEdge* edge;

            AIUtils::getPolyLine(m_parent->m_wallSlidePrevPoly,m_parent->m_wallSlidePrevEdge,poly,edge);

            if ( poly && edge )
            {
                Vec2d vecToPos = m_actor->get2DPos() - edge->getPos();
                f32 dist = edge->m_normalizedVector.getPerpendicular().dot(vecToPos);

                if ( dist > (m_characterPhys->getRadius()*0.9f) &&
                    dist < (m_characterPhys->getRadius()*1.1f) )
                {
                    dist = edge->m_normalizedVector.dot(vecToPos);

                    if ( dist >= 0.f && dist <= edge->m_length )
                    {
                        m_parent->m_stateWallSlide.forceWallSlideEdge(m_parent->m_wallSlidePrevPoly,m_parent->m_wallSlidePrevEdge);
                        m_parent->changeState(&m_parent->m_stateWallSlide);
                    }
                    else
                    {
                        m_parent->changeState(&m_parent->m_stateFalling);
                    }
                }
                else
                {
                    m_parent->changeState(&m_parent->m_stateFalling);
                }
            }
            else
            {
                m_parent->changeState(&m_parent->m_stateFalling);
            }
        }
        break;
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::update(f32 _dt)
{
    Super::update(_dt);

#ifdef ITF_SUPPORT_DEBUGFEATURE
    m_dbgDraws.clear();
#endif //ITF_SUPPORT_DEBUGFEATURE

    m_hitSomethingThisFrame = bfalse;

    if ( m_parent->isOnPedestal() && !m_parent->m_onPedestalValid )
    {
        m_parent->stopOnPedestal();
    }

    if ( m_switchHitTimer && !m_parent->isSwimming() )
    {
        checkSwitchHit();
    }

    if ( m_preparingHit )
    {
        fillHitQueue(btrue,getBaseHitDir());
    }

    if ( m_performingHit )
    {
        m_hitQueue.clear();
        fillHitQueue(bfalse,getBaseHitDir());

        PunchStim* stim = setupPunchStim();
        releaseHitsFromQueue(stim);
        STIMSMANAGER->recycleStim(stim);
    }

    if ( m_permanentHit )
    {
        updatePermanentHit();
    }

    if ( m_parent->m_stance == STANCE_STAND )
    {
        updateCrouchStand();
    }

    updateMemorizedHits(_dt);

    ECornerPose cornerPose = m_parent->calculateCornerPose();

    if ( cornerPose != CORNERPOSE_NONE &&
         m_characterPhys->getForce() == Vec2d::Zero )
    {
        m_parent->exitCornerPose(cornerPose,_dt);
    }

    switch(m_parent->m_currentPunchHitType)
    {
    case PUNCHTYPE_CRUSH:
        updateCrushHit(_dt);
        break;
    case PUNCHTYPE_REDUCEDSIZE:
    case PUNCHTYPE_TORNADO:
        updateDashHit(_dt);
        break;
    default:
        break;
    }

    m_justEntered = bfalse;
    m_switchHitTimer = Max(m_switchHitTimer-_dt,0.f);
    
    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        m_parent->setActionPose(ACTIONPOSE_FIGHT);
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::checkSwitchHit()
{
    EMoveDir moveDir = m_parent->getMoveData().getMoveDirection();

    if ( moveDir == MOVEDIR_NONE )
    {
        return;
    }

    bbool switched = bfalse;

    if ( m_parent->isSizeReduced() )
    {
        if ( moveDir == MOVEDIR_DOWN &&
             m_parent->m_currentPunchHitType != PUNCHTYPE_CRUSH &&
            !m_parent->isOnPedestal() &&
             m_parent->m_crushRetriggerCounter == 0.f &&
            !m_parent->isOnGeyser() )
        {
            if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID )
            {
                switched = m_parent->setAirPunch();
            }
            else
            {
                m_parent->m_hitQueueTimer = 0.f;
                m_parent->m_currentJumpQueueCrushAttack = btrue;
                m_parent->setJumpData();
                m_parent->changeState(&m_parent->m_stateJumping);
            }

            switched = btrue;
        }
        else if ( moveDir != m_reducedHitDir )
        {
            if ( moveDir != MOVEDIR_UP ||
                 m_parent->isSwimming() ||
                !m_parent->m_reducedHitUpDisabledUntilLand )
            {
                m_parent->changeState(&m_parent->m_stateReleaseHit);
                switched = btrue;
            }
        }
    }
    else
    {
        if ( m_parent->m_currentPunchHitType != PUNCHTYPE_UTURNKICK &&
           ( moveDir == MOVEDIR_LEFT &&
            !m_actor->isFlipped() ) ||
           ( moveDir == MOVEDIR_RIGHT &&
             m_actor->isFlipped() ) )
        {
            m_parent->setUTurnKick();
            m_parent->setLookRight(m_actor->isFlipped());
            m_parent->resetDesiredLookDir();
            switched = btrue;
        }
        else if ( moveDir == MOVEDIR_UP &&
                  m_characterPhys->getStickedEdgeIndex() == U32_INVALID &&
                  m_parent->m_currentPunchHitType != PUNCHTYPE_UPPERKICK &&
                 !m_parent->m_reducedHitUpDisabledUntilLand &&
                 !m_parent->isSizeReduced() )
        {
            switched = m_parent->setAirPunch();
        }
        else if ( moveDir == MOVEDIR_DOWN &&
            m_characterPhys->getStickedEdgeIndex() == U32_INVALID &&
            m_parent->m_currentPunchHitType != PUNCHTYPE_CRUSH &&
            m_parent->m_crushRetriggerCounter == 0.f &&
           !m_parent->isOnGeyser() )
        {
            switched = m_parent->setAirPunch();
        }
    }

    if ( switched )
    {
        m_switchHitTimer = 0.f;
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::updateCrushHit( f32 _dt )
{
    bbool resetCrushBlockCount = bfalse;

    if ( m_crushWasBlocked )
    {
        m_crushWasBlockedCounter++;
        m_characterPhys->setSpeed(Vec2d::Zero);
    }

    if ( !m_crushReleased )
    {
        if ( m_crushWasOnGround )
        {
            if ( m_characterPhys->getStickedEdge() )
            {
                m_crushReleased = btrue;
            }
            else
            {
                m_crushWasOnGround = bfalse;
                m_crushStuckCounter = 0.f;

                if ( !m_crushWasBlocked )
                {
                    resetCrushBlockCount = btrue;
                }
            }
        }
        else
        {
            if ( m_characterPhys->getStickedEdge() )
            {
                m_crushWasOnGround = btrue;
                m_crushStuckCounter = 0.f;
            }
            else
            {
                if ( !m_crushWasBlocked )
                {
                    resetCrushBlockCount = btrue;
                }

                m_crushWasOnGround = bfalse;

                m_crushTravelTime += _dt;

                if ( m_crushTravelTime >= m_parent->getTemplate()->getCrushHitLevelIncrTimeSecond() )
                {
                    m_parent->m_currentPunchHitLevel = m_parent->getTemplate()->getCrushHitLevel() + 2;
                }
                else if ( m_crushTravelTime >= m_parent->getTemplate()->getCrushHitLevelIncrTimeFirst() )
                {
                    m_parent->m_currentPunchHitLevel = m_parent->getTemplate()->getCrushHitLevel() + 1;
                }
                else
                {
                    m_parent->m_currentPunchHitLevel = m_parent->getTemplate()->getCrushHitLevel();
                }

                if ( m_parent->m_crushHitSuspensionCounter == 0.f )
                {
                    f32 ySpeed = m_parent->m_currentGravityDir.dot(m_characterPhys->getSpeed());

                    if ( m_crushPrevSpeedY == 0.f )
                    {

                    }
                    else
                    {
                        f32 gravity = m_parent->m_currentGravityDir.dot(m_parent->m_currentGravity) * _dt;

                        if ( ySpeed <= ((m_crushPrevSpeedY+gravity)-MTH_EPSILON) )
                        {
                            ySpeed = m_crushPrevSpeedY + gravity;

                            f32 xSpeed = m_parent->m_currentGravityDir.getPerpendicular().dot(m_characterPhys->getSpeed());

                            Vec2d speed = (m_parent->m_currentGravityDir*ySpeed) +
                                (m_parent->m_currentGravityDir.getPerpendicular()*xSpeed);

                            m_characterPhys->setSpeed(speed);
                            m_crushWasBlockedCounter = 0;
                        }
                    }

                    m_crushPrevSpeedY = ySpeed;

                    f32 distPrevSq = ( m_crushPrevPos - m_actor->get2DPos() ).sqrnorm();

                    if ( distPrevSq < MTH_EPSILON )
                    {
                        m_crushStuckCounter += _dt;

                        if ( m_crushStuckCounter >= 0.05f )
                        {
                            m_crushReleased = btrue;
                        }
                    }
                    else
                    {
                        m_crushStuckCounter = 0.f;

                        if ( m_crushDistanceLimit )
                        {
                            Vec2d vec = m_actor->get2DPos() - m_crushStartPos;
                            f32 dist = m_parent->m_currentGravityDir.dot(vec);

                            if ( dist > m_crushDistanceLimit )
                            {
                                m_crushReleased = btrue;
                            }
                        }
                    }
                }
            }
        }
    }

    if ( m_crushStuckCounter == 0.f )
    {
        m_crushPrevPos = m_actor->get2DPos();
    }

    if ( !m_crushReleased )
    {
        if ( resetCrushBlockCount )
        {
            m_crushWasBlockedCounter = 0;
        }
        else
        {
            if ( m_crushWasBlockedCounter > 1 )
            {
                m_crushReleased = btrue;
            }
        }
    }

    m_crushWasBlocked = bfalse;
}

void Ray_PlayerControllerComponent::StateHitRelease::setupReducedSizeHit()
{
    EMoveDir moveDir = m_parent->getMoveData().getMoveDirection();
    f32 forceSize = 0.f;
    bbool useTornadoSetup = bfalse;

    if ( m_forceReducedHitUp )
    {
        moveDir = MOVEDIR_UP;
    }

    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        if ( moveDir == MOVEDIR_UP )
        {
            forceSize = m_parent->getTemplate()->getReducedSizeHitUpForce();
            m_parent->setGravityMode(GRAVITYMODE_REDUCEDSIZEHIT,m_parent->getTemplate()->getReducedSizeHitGravity());
        }
        else
        {
            useTornadoSetup = btrue;
        }
    }
    else
    {
        if ( moveDir == MOVEDIR_UP )
        {
            forceSize = m_parent->getTemplate()->getReducedSizeHitUpForce();
            m_parent->setGravityMode(GRAVITYMODE_REDUCEDSIZEHIT,m_parent->getTemplate()->getReducedSizeHitGravity());
        }
        else
        {
            forceSize = m_parent->getTemplate()->getReducedSizeHitSideForceAir();
        }
    }

    if ( useTornadoSetup )
    {
        setupTornadoAttack();
    }
    else
    {
        Vec2d force;

        m_reducedHitDir = moveDir;
        m_characterPhys->setSpeed(Vec2d::Zero);
        m_characterPhys->resetForces();

        if ( moveDir == MOVEDIR_UP )
        {
            force = Vec2d::Up * forceSize;
            m_parent->cancelPhysicAirHit();
            m_parent->m_currentPunchOrientation = 90.f;
            m_parent->m_reducedHitUpDisabledUntilLand = btrue;
            m_dashCounter = m_parent->getTemplate()->getReducedSizeHitUpDuration();
        }
        else
        {
            if ( moveDir == MOVEDIR_RIGHT )
            {
                m_parent->setLookRight(btrue);
                force = Vec2d::Right * forceSize;
                m_parent->m_currentPunchOrientation = 0.f;
            }
            else if ( moveDir == MOVEDIR_LEFT )
            {
                m_parent->setLookRight(bfalse);
                force = -Vec2d::Right * forceSize;
                m_parent->m_currentPunchOrientation = 0.f;
            }
            else
            {
                if ( m_actor->isFlipped() )
                {
                    m_reducedHitDir = MOVEDIR_LEFT;
                    force = -Vec2d::Right * forceSize;
                }
                else
                {
                    m_reducedHitDir = MOVEDIR_RIGHT;
                    force = Vec2d::Right * forceSize;
                }
                m_parent->m_currentPunchOrientation = 0.f;
            }

            if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID )
            {
                m_dashCounter = m_parent->getTemplate()->getReducedSizeHitDuration();
            }
            else
            {
                m_dashCounter = m_parent->getTemplate()->getReducedSizeHitAirDuration();
            }

            const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();

            if ( currentEdge )
            {
                StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector);

                if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL ||
                    edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF )
                {
                    m_switchHitTimer = 0.f;
                }
            }
        }

        if ( !m_characterPhys->getDisabled() )
        {
            m_characterPhys->addForce(force);
        }

        m_forceReducedHitUp = bfalse;
        m_dashSpeedLastFrame = force*LOGICDT;
        m_dashHadHitLastFrame = bfalse;
        m_permanentHit = btrue;
        m_performingHit = btrue;
        m_dashTotal = m_dashCounter;
        m_dashAttackMoveDir = m_parent->getMoveData().getMoveDirection();

        // launch one super punch if available (RO-11295)
        Ray_EventSuperPunchLaunch launch(getBaseHitDir());
        launch.setSender(m_actor->getRef());
        launch.setReducedHit(m_parent->m_currentPunchHitType==PUNCHTYPE_REDUCEDSIZE);
        m_actor->onEvent(&launch);
    }

    if ( !m_parent->isSwimming() )
    {
        m_animComponent->resetTree();
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::setupTornadoAttack()
{
    f32 duration;
    f32 speed;
    EMoveDir dashAttackDirection = m_parent->getMoveData().getMoveDirection();
    bbool isSpam = bfalse;

    if ( m_parent->isSwimming() )
    {
        isSpam = m_parent->m_timeSinceDashAttack <= m_parent->getTemplate()->getSwimAttackSpamTime();

        if ( m_parent->isSprinting() )
        {
            duration = m_parent->getTemplate()->getSwimAttackSprintDuration();
            speed = m_parent->getTemplate()->getSwimAttackSprintSpeed();

            if ( isSpam )
            {
                speed *= m_parent->getTemplate()->getSwimAttackSpamImpulseMultiplier();
            }
        }
        else
        {
            duration = m_parent->getTemplate()->getSwimAttackNormalDuration();
            speed = m_parent->getTemplate()->getSwimAttackNormalSpeed();

            if ( isSpam )
            {
                speed *= m_parent->getTemplate()->getSwimAttackSpamImpulseMultiplierSprint();
            }
        }
    }
    else
    {
        if ( m_parent->isSizeReduced() )
        {
            duration = m_parent->getTemplate()->getReducedSizeHitDuration();
            speed = m_parent->getTemplate()->getReducedSizeHitSideForceGround()*LOGICDT;

            f32 friction = m_parent->getTemplate()->getReducedSizeHitFriction();

            const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();

            if ( currentEdge )
            {
                f32 edgeFriction = AIUtils::getEdgeFriction(currentEdge);
                f32 t = Clamp(edgeFriction,0.f,1.f);

                friction *= Interpolate(m_parent->getTemplate()->getReducedSizeHitGroundFrictionMultiplierMin(),
                                        m_parent->getTemplate()->getReducedSizeHitGroundFrictionMultiplierMax(),
                                        t);
            }

            m_parent->setFrictionMode(FRICTIONMODE_REDUCEDSIZEHIT,friction);
        }
        else
        {
            duration = m_parent->getTemplate()->getTornadoDuration();
            speed = m_parent->getTemplate()->getTornadoSpeed();
            m_parent->setFrictionMode(FRICTIONMODE_TORNADO,m_parent->getTemplate()->getTornadoFriction());
        }
    }

    Vec2d direction;
    bbool keepSpeed = bfalse;

    if ( m_parent->isSwimming() )
    {
        if ( !isSpam && dashAttackDirection != MOVEDIR_NONE )
        {
            direction = m_parent->getMoveDataAnalog().getMoveNormalized();
        }
        else if ( m_characterPhys->getSpeed().sqrnorm() > 1.f )
        {
            direction = m_characterPhys->getSpeed();
            direction.normalize();
        }
        else
        {
            direction = AIUtils::getLookDir(m_actor);
        }
    }
    else
    {
        const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();

        if ( currentEdge &&
             m_characterPhys->getSpeed().sqrnorm() > 1.f &&
           ( m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND ||
             m_parent->m_walkOnWallKeepDirFromSpeed ) )
        {
            f32 dotProd = currentEdge->m_normalizedVector.dot(m_characterPhys->getSpeed());

            if ( dotProd > 0.f )
            {
                dashAttackDirection = MOVEDIR_RIGHT;
            }
            else if ( dotProd < 0.f )
            {
                dashAttackDirection = MOVEDIR_LEFT;
            }
            else
            {
                dashAttackDirection = MOVEDIR_NONE;
            }

            keepSpeed = btrue;
            direction = m_characterPhys->getSpeed();
            direction.normalize();
        }
        else
        {
            direction = AIUtils::getLookDir(m_actor);
        }
    }

    if ( direction.dot(m_characterPhys->getSpeed()) > 0 )
    {
        f32 currentSpeedSq = m_characterPhys->getSpeed().sqrnorm();

        if ( currentSpeedSq < (speed*speed) )
        {
            Vec2d newSpeed = direction * speed;
            m_characterPhys->setSpeed(newSpeed);
        }
        else
        {
            f32 currentSp = f32_Sqrt(currentSpeedSq);

            Vec2d newSpeed = direction * currentSp;
            m_characterPhys->setSpeed(newSpeed);
        }
    }
    else
    {
        Vec2d newSpeed = direction * speed;
        m_characterPhys->setSpeed(newSpeed);
    }

    m_dashSpeedLastFrame = m_characterPhys->getSpeed();

    m_dashTotal = m_dashCounter = duration;
    m_dashHadHitLastFrame = bfalse;
    m_permanentHit = btrue;
    m_performingHit = btrue;
    m_dashAttackMoveDir = dashAttackDirection;

    if ( keepSpeed )
    {
        m_switchHitTimer = 0.f;
    }

    // launch one super punch if available (RO-11291 + RO-11295)
    Vec2d launchDirection;

    if (m_parent->m_listenerMove != Vec2d::Zero)
    {
        launchDirection = m_parent->m_listenerMove;
    }
    else
    {
        launchDirection = direction;
    }

    Ray_EventSuperPunchLaunch launch(direction);
    launch.setSender(m_actor->getRef());
    launch.setReducedHit(m_parent->m_currentPunchHitType==PUNCHTYPE_REDUCEDSIZE);
    m_actor->onEvent(&launch);
}

void Ray_PlayerControllerComponent::StateHitRelease::updateDashHit( f32 _dt )
{
    if ( !m_dashHadHitLastFrame && m_hitSomethingThisFrame )
    {
        if ( m_characterPhys->getSpeed().sqrnorm() < 1.f )
        {
            m_characterPhys->setSpeed(m_dashSpeedLastFrame);
        }
    }

    if ( !m_hitSomethingThisRound )
    {
        if ( m_parent->isSwimming() )
        {
            m_parent->m_dashAttackCooldown = m_parent->getTemplate()->getSwimAttackCooldown();
        }
        else
        {
            if ( m_parent->isSizeReduced() )
            {
                if ( m_reducedHitDir == MOVEDIR_UP )
                {
                    m_parent->m_dashAttackCooldown = m_parent->getTemplate()->getReducedSizeHitUpCooldown();
                }
                else
                {
                    m_parent->m_dashAttackCooldown = m_parent->getTemplate()->getReducedSizeHitCooldown();
                }
            }
            else
            {
                m_parent->m_dashAttackCooldown = m_parent->getTemplate()->getTornadoCooldown();
            }
        }
    }
    else
    {
        m_parent->m_dashAttackCooldown = 0.f;
    }

    m_dashCounter = Max(m_dashCounter-_dt,0.f);
    m_dashHadHitLastFrame = m_hitSomethingThisFrame;

    if ( !m_justEntered )
    {
        m_dashSpeedLastFrame = m_characterPhys->getSpeed();
    }

    if ( m_switchHitTimer )
    {
        EMoveDir currentMove = m_parent->getMoveData().getMoveDirection();

        if ( m_dashAttackMoveDir != currentMove && currentMove != MOVEDIR_NONE )
        {
            // restart the hit in the new direction
            if ( m_parent->isSwimming() )
            {
                setupTornadoAttack();

                Vec2d moveDir = m_characterPhys->getSpeed();
                moveDir.normalize();
                f32 targetRotation = getVec2Angle(moveDir);

                if ( m_actor->isFlipped() )
                {
                    targetRotation += MTH_PI;
                }

                m_actor->setAngle(targetRotation);

                m_switchHitTimer = 0.f;
            }
            else
            {
                if ( m_parent->isSizeReduced() )
                {
                    if ( currentMove != MOVEDIR_UP ||
                        !m_parent->m_reducedHitUpDisabledUntilLand )
                    {
                        setupReducedSizeHit();
                        m_switchHitTimer = 0.f;
                    }
                }
                else
                {
                    if ( currentMove == MOVEDIR_LEFT || currentMove == MOVEDIR_RIGHT )
                    {
                        setupTornadoAttack();
                        m_switchHitTimer = 0.f;
                    }
                }
            }
        }
    }
}

bbool Ray_PlayerControllerComponent::StateHitRelease::checkShouldExitDashHit( f32 _minSpeed ) const
{
    if ( m_dashCounter == 0.f )
    {
        return btrue;
    }

    if ( m_hitSomethingThisRound &&
         m_parent->m_dashAttackCooldown == 0.f )
    {
        f32 speed = m_characterPhys->getSpeed().norm();

        bbool canExit = speed <= _minSpeed;

        if ( canExit && m_parent->isHitQueued() || m_parent->isJumpQueued() )
        {
            return btrue;
        }
        else
        {
            return bfalse;
        }
    }
    else
    {
        return bfalse;
    }
}

Vec2d Ray_PlayerControllerComponent::StateHitRelease::getBaseHitDir() const
{
    f32 angle = m_actor->getAngle();

    switch(m_parent->m_currentPunchHitType)
    {
        case PUNCHTYPE_CRUSH:
            angle += -MTH_PIBY2;
            break;
        case PUNCHTYPE_UPPERKICK:
            angle += MTH_PIBY2;
            break;
        case PUNCHTYPE_CHARGE:
        case PUNCHTYPE_REPEATING:
            if ( !m_actor->isFlipped() )
            {
                angle += MTH_DEGTORAD*m_parent->m_currentPunchOrientation;
            }
            else
            {
                angle += MTH_PI - (MTH_DEGTORAD*m_parent->m_currentPunchOrientation);
            }
            break;
        case PUNCHTYPE_REDUCEDSIZE:
            {
                if ( m_parent->m_currentPunchOrientation > 0 )
                {
                    angle += MTH_PIBY2;
                }
                else
                {
                    if ( m_actor->isFlipped() )
                    {
                        angle += MTH_PI;
                    }
                }
            }
            break;
        default:
            if ( m_actor->isFlipped() )
            {
                angle += MTH_PI;
            }
            break;
    }

    if ( m_inverseHitDir )
    {
        angle += MTH_PI;
    }

    return Vec2d::Right.Rotate(angle);
}

void Ray_PlayerControllerComponent::StateHitRelease::updatePermanentHit()
{
    u32 numPointsBegin, numPointsEnd;
    const Vec2d* pointsBegin = m_animComponent->getCurrentPolylinePointBuffer(ITF_GET_STRINGID_CRC(L_Col_Punch_Begin,1958330576), &numPointsBegin);
    const Vec2d* pointsEnd = m_animComponent->getCurrentPolylinePointBuffer(ITF_GET_STRINGID_CRC(L_Col_Punch_End,706093612), &numPointsEnd);

    if ( !pointsBegin || !pointsEnd )
    {
        return;
    }

    ITF_ASSERT(numPointsBegin == 2&&numPointsEnd == 2);

    if (numPointsBegin < 2 || numPointsEnd < 2)
    {
        return;
    }

    m_shapePoints.clear();

    transform2d xf;

    xf.m_pos = m_actor->get2DPos();
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

    if ( m_actor->isFlipped() )
    {
        m_shapePoints.push_back( mulTransform2dT(xf,pointsBegin[1]) );
        m_shapePoints.push_back( mulTransform2dT(xf,pointsBegin[0]) );
        m_shapePoints.push_back( mulTransform2dT(xf,pointsEnd[1]) );
        m_shapePoints.push_back( mulTransform2dT(xf,pointsEnd[0]) );
    }
    else
    {
        m_shapePoints.push_back( mulTransform2dT(xf,pointsBegin[0]) );
        m_shapePoints.push_back( mulTransform2dT(xf,pointsBegin[1]) );
        m_shapePoints.push_back( mulTransform2dT(xf,pointsEnd[0]) );
        m_shapePoints.push_back( mulTransform2dT(xf,pointsEnd[1]) );
    }
    
    m_shape.setPoints(m_shapePoints);
    m_hitQueue.clear();

    fillHitQueue(bfalse,getBaseHitDir());

    PunchStim* stim = setupPunchStim();
    releaseHitsFromQueue(stim);
    STIMSMANAGER->recycleStim(stim);
}

void Ray_PlayerControllerComponent::StateHitRelease::updateMemorizedHits( f32 _dt )
{
    i32 numHits = static_cast<i32>(m_memorizedHits.size());

    for ( i32 i = 0; i < numHits; i++ )
    {
        MemorizedHitInfo& hitInfo = m_memorizedHits[i];

        if ( hitInfo.m_timer > 0.f )
        {
            hitInfo.m_timer -= _dt;

            if ( hitInfo.m_timer < 0.f )
            {
                m_memorizedHits.eraseNoOrder(i);
                numHits--;
                i--;
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::updateCrouchStand()
{
    if ( m_characterPhys->getStickedEdge() )
    {
        if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN )
        {
            if ( !m_parent->isCrouch() )
            {
                m_parent->setCrouch(btrue);
                m_exitSoon = btrue;
            }
        }
        else
        {
            if ( m_parent->isCrouch() )
            {
                m_parent->setCrouch(bfalse);
                m_exitSoon = btrue;
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == MRK_Perform_Hit)
        {
            m_hitQueue.clear();
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            m_performingHit = btrue;
            processPerformHit();
            m_performingHit = bfalse;
            m_memorizedHits.clear();
        }
        else if ( animEvent->getName() == MRK_Perform_Hit_Start )
        {
            m_hitQueue.clear();
            m_performingHit = btrue;
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            processPerformHit();
        }
        else if ( animEvent->getName() == MRK_Perform_Hit_Stop )
        {
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            m_performingHit = bfalse;
            m_memorizedHits.clear();
        }
        else if ( animEvent->getName() == MRK_Allow_Move )
        {
            m_canMove = btrue;
        }
    }
    else if ( AnimPolylineEvent* animPolyEvent = _event->DynamicCast<AnimPolylineEvent>(ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773)) )
    {
        if (animPolyEvent->getName() == MRK_Prepare_Hit )
        {
            m_hitQueue.clear();
            m_preparingHit = btrue;
            m_permanentHit = bfalse;
            m_performingHit = bfalse;
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(btrue,getBaseHitDir());
            m_preparingHit = bfalse;
        }
        else if (animPolyEvent->getName() == MRK_Prepare_Hit_Start )
        {
            m_hitQueue.clear();
            m_preparingHit = btrue;
            m_permanentHit = bfalse;
            m_performingHit = bfalse;
            m_skipFirstReleaseHit = btrue;
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(btrue,getBaseHitDir());
        }
        else if (animPolyEvent->getName() == MRK_Prepare_Hit_Stop )
        {
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            m_performingHit = bfalse;
        }
        else if (animPolyEvent->getName() == MRK_Perform_Hit)
        {
            m_hitQueue.clear();
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            m_performingHit = btrue;
            m_memorizedHits.clear();
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(bfalse,getBaseHitDir());
            processPerformHit();
            m_performingHit = bfalse;
        }
        else if ( animPolyEvent->getName() == MRK_Perform_Hit_Start )
        {
            m_hitQueue.clear();
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            m_performingHit = btrue;
            calculateHitShapeFromEvent(animPolyEvent);
            fillHitQueue(bfalse,getBaseHitDir());
            processPerformHit();
        }
        else if ( animPolyEvent->getName() == MRK_Perform_Hit_Stop )
        {
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            m_performingHit = bfalse;
            m_memorizedHits.clear();
        }
        else if ( animPolyEvent->getName() == MRK_Permanent_Hit )
        {
            m_preparingHit = bfalse;
            m_permanentHit = btrue;
            m_performingHit = btrue;
        }
        else if ( animPolyEvent->getName() == MRK_Crush_Hit )
        {
            m_preparingHit = bfalse;
            m_permanentHit = bfalse;
            m_performingHit = btrue;
            processPerformCrush();
            m_performingHit = bfalse;
        }
    }
    else if (Ray_EventQueryHitType* hitTypeQuery = _event->DynamicCast<Ray_EventQueryHitType>(ITF_GET_STRINGID_CRC(Ray_EventQueryHitType,4146626226)) )
    {
        processQueryHitType(hitTypeQuery);
    }
    else if ( EventStickOnPolylineUpdate * stickOnPolyEvent = _event->DynamicCast<EventStickOnPolylineUpdate>(ITF_GET_STRINGID_CRC(EventStickOnPolylineUpdate,2702146949)) )
    {
        if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH &&
             stickOnPolyEvent->getPolyline() != ITF_INVALID_OBJREF &&
             stickOnPolyEvent->getEdge() != U32_INVALID )
        {
            processCrushHitGround(stickOnPolyEvent->getPolyline(),stickOnPolyEvent->getEdge());
        }
    }
    else if ( EventBlockedByPolyline * blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
    {
        if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH )
        {
            if ( blockedEvent->getPolyline() != ITF_INVALID_OBJREF &&
                 blockedEvent->getEdge() != U32_INVALID )
            {
                processCrushHitGround(blockedEvent->getPolyline(),blockedEvent->getEdge());
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::processPerformHit()
{
    PunchStim* stim = setupPunchStim();

    releaseHitsFromQueue(stim);

    STIMSMANAGER->recycleStim(stim);

    // launch one super punch if available
    Ray_EventSuperPunchLaunch launch(getBaseHitDir());
    launch.setSender(m_actor->getRef());
    m_actor->onEvent(&launch);
}

void Ray_PlayerControllerComponent::StateHitRelease::processPerformCrush()
{
    PunchStim* stim = setupPunchStim();

    if ( !stim )
    {
        return;
    }

    releaseHitsFromQueue(stim);

    u32 numPoints;
    Vec2d* points = m_animComponent->getCurrentPolylinePointBuffer(ITF_GET_STRINGID_CRC(L_Col_Punch,1934609272), &numPoints);
    Vec2d hitDir = getBaseHitDir();
    bbool bounceOnEnemy = bfalse;
    const PolyLineEdge* bounceEdge = NULL;
    ObjectRef bounceEdgePoly = ITF_INVALID_OBJREF;
    u32 bounceEdgeIndex = U32_INVALID;
    u32 bounceEdgeDangerLevel = 0;    

    if ( points )
    {
        calculateHitShapeFromPolyLine(points,numPoints);
        fillHitQueue(bfalse,hitDir);

        u32 numHits = m_hitQueue.size();
        PolyLine* poly;
        const PolyLineEdge* edge;

        for ( u32 i = 0; i < numHits; i++ )
        {
            const HitQueueInfo& hitInfo = m_hitQueue[i];

            u32 numContacts = hitInfo.m_contacts.size();

            if ( !bounceEdge )
            {
                for ( u32 j = 0; j < numContacts; j++ )
                {
                    const SCollidableContact& contact = hitInfo.m_contacts[j];

                    if ( contact.m_edgeIndex != U32_INVALID )
                    {
                        AIUtils::getPolyLine(contact.m_collidableUserData,contact.m_edgeIndex,poly,edge);

                        if ( poly && edge )
                        {
                            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

                            if ( mat && mat->getIsBounce() )
                            {
                                bounceEdge = edge;
                                bounceEdgeDangerLevel = mat->getDangerousLevel();
                                bounceEdgePoly = contact.m_collidableUserData;
                                bounceEdgeIndex = contact.m_edgeIndex;
                                break;
                            }
                        }
                    }
                }
            }

            if ( m_hitQueue[i].m_receiver != ITF_INVALID_OBJREF )
            {
                HitQueueInfo& hitInfo = m_hitQueue[i];
                Actor* otherActor = AIUtils::getActor(hitInfo.m_receiver);

                if ( otherActor )
                {
                    if ( AIUtils::isEnemy(m_parent->getFaction(),AIUtils::getFaction(otherActor)) )
                    {
                        if ( hitInfo.m_closestEdgeIndex != U32_INVALID )
                        {
                            AIUtils::getPolyLine(hitInfo.m_closestPoly, hitInfo.m_closestEdgeIndex, poly, edge);

                            if ( poly && edge )
                            {
                                const Ray_GameMaterial_Template* mat = static_cast<const Ray_GameMaterial_Template*>(World::getGameMaterial(edge->getGameMaterial()));

                                if ( mat && mat->bounceOnCrushAttack() )
                                {
                                    bounceOnEnemy = btrue;
                                }
                            }
                        }
                        else
                        {
                            bounceOnEnemy = btrue;
                        }
                    }
                }
            }
        }

        releaseHitsFromQueue(stim);
    }

    stim->setReceivedHitType(RECEIVEDHITTYPE_EJECTY);

    points = m_animComponent->getCurrentPolylinePointBuffer(ITF_GET_STRINGID_CRC(L_Col_Punch_Crush_L,2020340889), &numPoints);

    if ( points )
    {
        calculateHitShapeFromPolyLine(points,numPoints);
        fillHitQueue(bfalse,-hitDir);
    }

    points = m_animComponent->getCurrentPolylinePointBuffer(ITF_GET_STRINGID_CRC(L_Col_Punch_Crush_R,2153081474), &numPoints);

    if ( points )
    {
        calculateHitShapeFromPolyLine(points,numPoints);
        fillHitQueue(bfalse,-hitDir);
    }

    releaseHitsFromQueue(stim);

    STIMSMANAGER->recycleStim(stim);

    m_shakeCamReleased = btrue;

    bbool jumped = bfalse;
    bbool addShakeCamera = btrue;

    if ( bounceOnEnemy || bounceEdge )
    {
        if ( m_crushDistanceLimit == 0.f )
        {
            m_parent->resetBounce();

            if ( bounceOnEnemy )
            {
                m_parent->processBounce(0,-m_parent->m_currentGravityDir,BOUNCETYPE_ENEMY,m_characterPhys->getSpeed(),StringID::Invalid);
            }
            else
            {
                m_parent->m_bounceCheckEdgeDir = bounceEdge->m_normalizedVector;
                m_parent->m_bounceCheckEdgeGameMaterial = bounceEdge->getGameMaterial();
                m_parent->m_bounceCheckEdgeDangerLevel = bounceEdgeDangerLevel;
                m_parent->m_bounceCheckEdgeIndex = bounceEdgeIndex;
                m_parent->m_bounceCheckEdgePolyRef = bounceEdgePoly;
                m_parent->checkBounceEdge();
            }

            m_parent->changeState(&m_parent->m_stateJumping);
            jumped = btrue;
            addShakeCamera = bfalse;
        }        
    }

    if ( !jumped )
    {
        if ( !checkBounce() )
        {
            m_parent->changeState(&m_parent->m_stateIdle);
        }
        else
        {
            addShakeCamera = bfalse;
        }
    }

    m_parent->m_crushRetriggerCounter = m_parent->getTemplate()->getCrushHitRetriggerDelay();

    if ( addShakeCamera )
    {
        shakeCamera(m_parent->m_currentPunchHitLevel);
    }
    
    // also rumble player's pad a bit (RO-11860)
    PADRUMBLEMANAGER->startRumble(ITF_GET_STRINGID_CRC(LightShort,1228717030), m_parent->m_playerIndex);
}

void Ray_PlayerControllerComponent::StateHitRelease::processQueryHitType(Ray_EventQueryHitType* _hitTypeQuery)
{
    _hitTypeQuery->setHitType(m_parent->m_currentPunchHitType);
}

void Ray_PlayerControllerComponent::StateHitRelease::fillHitQueue( bbool _updateAnimValues, const Vec2d& _hitDir )
{
    if ( !m_shapePoints.size() )
    {
        return;
    }

    m_shape.setFlipped(bfalse);

    // We only calculate collision when we are setting the animation values
    calculateHitQueue(_hitDir,m_hitQueue);
    addDbgShape();

    if ( _updateAnimValues )
    {
        updateAnimValuesFromQueue();
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::updateAnimValuesFromQueue()
{
    // Use the information on the queue to setup the animation
    if ( m_hitQueue.size() )
    {
        HitQueueInfo& hitInfo = m_hitQueue[m_hitQueue.size()-1];

        if ( hitInfo.m_blocks )
        {
            m_parent->m_currentPunchDistance = hitInfo.m_animDist;
        }
        else
        {
            m_parent->m_currentPunchDistance = 1.f;
        }

        if ( hitInfo.m_fail )
        {
            m_parent->m_currentPunchResult = PUNCHRELEASERESULT_FAIL;
        }
        else
        {
            m_parent->m_currentPunchResult = PUNCHRELEASERESULT_IMPACT;
        }
    }
    else
    {
        m_parent->m_currentPunchDistance = 1.f;
        m_parent->m_currentPunchResult = PUNCHRELEASERESULT_NOIMPACT;
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::calculateHitShapeFromEvent( AnimPolylineEvent* _animPolyEvent )
{
    u32 numPoints = _animPolyEvent->getNumPoints();

    if ( numPoints > 1 )
    {
        const Vec2d* points = _animPolyEvent->getPoints();
        calculateHitShapeFromPolyLine(points,numPoints);
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::calculateHitShapeFromPolyLine( const Vec2d* _points, u32 _numPoints )
{
    Vec2d p;
    bbool flipped = m_actor->isFlipped();

    m_shapePoints.clear();

    transform2d xf;

    xf.m_pos = m_actor->get2DPos();
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

    if ( flipped )
    {
        for ( i32 i = static_cast<i32>(_numPoints)-1; i >= 0; i-- )
        {
            p = mulTransform2dT(xf,_points[i]);

            m_shapePoints.push_back(p);
        }
    }
    else
    {
        for ( u32 i = 0; i < _numPoints; i++ )
        {
            p = mulTransform2dT(xf,_points[i]);

            m_shapePoints.push_back(p);
        }
    }

    m_shape.setPoints(m_shapePoints);
}

void Ray_PlayerControllerComponent::StateHitRelease::hitQueueApplyFilter( const PhysContactsContainer& _allContacts, const Vec2d& _hitDir,
                                                                          PhysContactsContainer& _filteredContacts, u32& _collisionIndex )
{
    PolyLine* poly(NULL);
    const PolyLineEdge* edge(NULL);
    bbool hitFriendly = bfalse;
    bbool hitOtherActor = m_hitSomethingThisRound;
    GameManager* gm = GAMEMANAGER;
    bbool crushPermanent = m_permanentHit && m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH;
    u32 numContacts = _allContacts.size();
    u32 newCollisionIndex = _collisionIndex;
    bbool removeFriendlies = m_parent->isInNoFriendlyFireRegion();

    // discard non-hittable polylines
    for ( u32 i = 0; i < numContacts && !_filteredContacts.full(); i++ )
    {
        const SCollidableContact& contact = _allContacts[i];

        if ( contact.m_collidableUserData == m_actor->getRef() || contact.m_collidableUserData == m_parent->m_onPedestal )
        {
            if ( i < _collisionIndex )
            {
                newCollisionIndex--;
            }

            continue;
        }

        if ( contact.m_edgeIndex != U32_INVALID )
        {
            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

            if ( poly && edge )
            {
                if ( poly->m_isCollisionActive )
                {
                    const GameMaterial_Template* mat = World::getGameMaterial(edge->getGameMaterial());

                    if ( mat && mat->getNoStim() )
                    {
                        if ( i < _collisionIndex )
                        {
                            newCollisionIndex--;
                        }

                        continue;
                    }

                    f32 dotProd = _hitDir.dot(edge->m_normalizedVector);

                    if ( fabs(dotProd) >= 0.99f )
                    {
                        if ( i < _collisionIndex )
                        {
                            newCollisionIndex--;
                        }

                        continue;
                    }
                }

                if ( i >= _collisionIndex )
                {
                    Actor* ownerActor = poly->getOwnerActor();

                    if ( ownerActor )
                    {
                        if ( poly->getRef() == m_parent->m_climbingPolyline )
                        {
                            continue;
                        }

                        Player* player = gm->getPlayerFromActor(ownerActor->getRef(),bfalse);

                        if ( player )
                        {
                            if ( player->isDead() )
                            {
                            }
                            else if ( crushPermanent )
                            {
                                continue;
                            }
                            else
                            {
                                hitFriendly = btrue;
                            }
                        }
                        else
                        {
                            Faction hisFaction = AIUtils::getFaction(ownerActor);
                            InteractionMask interactions = FACTION_MANAGER->getInteractions(m_parent->getFaction(), hisFaction);

                            if ( (interactions & INTERACTION_RECEIVEDAMAGE) != 0 )
                            {
                                hitOtherActor = btrue;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if ( i >= _collisionIndex )
            {
                Player* player = gm->getPlayerFromActor(contact.m_collidableUserData,bfalse);

                if ( player )
                {
                    if ( contact.m_collidableUserData == m_parent->m_hangingActor )
                    {
                        continue;
                    }

                    if ( player->isDead() )
                    {
                    }
                    else if ( crushPermanent )
                    {
                        continue;
                    }
                    else
                    {
                        hitFriendly = btrue;
                    }
                }
                else
                {
                    Actor* actor = AIUtils::getActor(contact.m_collidableUserData);

                    if ( actor )
                    {
                        Faction hisFaction = AIUtils::getFaction(actor);
                        InteractionMask interactions = FACTION_MANAGER->getInteractions(m_parent->getFaction(), hisFaction);

                        if ( (interactions & INTERACTION_RECEIVEDAMAGE) != 0 )
                        {
                            hitOtherActor = btrue;
                        }
                    }
                }
            }
        }

        _filteredContacts.push_back(contact);
    }

    numContacts = _filteredContacts.size();

    if ( !numContacts )
    {
        return;
    }

    if ( !removeFriendlies )
    {
        if ( hitOtherActor && hitFriendly )
        {
            removeFriendlies = btrue;
        }
    }

    // If we hit a friendly and a non-friendly we remove the friendly
    if ( removeFriendlies )
    {
        u32 index = 0;

        while ( index < numContacts )
        {
            const SCollidableContact& contact = _filteredContacts[index];

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

                if ( poly && edge )
                {
                    Actor* ownerActor = poly->getOwnerActor();

                    if ( ownerActor )
                    {
                        Player* player = gm->getPlayerFromActor(ownerActor->getRef(),bfalse);

                        if ( player && !player->isDead() )
                        {
                            if ( index < newCollisionIndex )
                            {
                                newCollisionIndex--;
                            }

                            _filteredContacts.eraseNoOrder(index);
                            numContacts--;
                            continue;
                        }
                    }
                }
            }
            else
            {
                Player* player = gm->getPlayerFromActor(contact.m_collidableUserData,bfalse);

                if ( player && !player->isDead() )
                {
                    if ( index < newCollisionIndex )
                    {
                        newCollisionIndex--;
                    }

                    _filteredContacts.eraseNoOrder(index);
                    numContacts--;
                    continue;
                }
            }

            index++;
        }
    }

    m_hitSomethingThisRound = hitOtherActor;
    m_hitSomethingThisFrame = m_hitSomethingThisRound;
}

void Ray_PlayerControllerComponent::StateHitRelease::calculateHitQueue( const Vec2d& _hitDir, HitQueue& _queue )
{
    PhysContactsContainer allContacts, contacts;
    u32 collisionIndex = 0;

    PHYSWORLD->checkEncroachment(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),
                                 &m_shape,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),allContacts);
    collisionIndex = allContacts.size();
    PHYSWORLD->collidePhantoms(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),
                              &m_shape,m_actor->getDepth(),ECOLLISIONFILTER_ALL,allContacts);

    hitQueueApplyFilter(allContacts,_hitDir,contacts,collisionIndex);

    f32 rehitDelay;

    if ( m_permanentHit )
    {
        rehitDelay = m_parent->getTemplate()->getRehitDelayPermanentHit();
    }
    else
    {
        // Remember it while the hit last so that we don't hit it again
        rehitDelay = -1.f;
    }

    if ( contacts.size() > 0 )
    {
        AIUtils::SortedContactInfoArray sortedContacts;
        f32 shapeStart, shapeEnd;
        AIUtils::sortContactsOnShape(m_actor,_hitDir,m_shape,contacts,
                                     sortedContacts,shapeStart,shapeEnd);

        u32 numContacts = sortedContacts.size();
        bbool hitBlocked;
        HitQueueInfo tempHit;
        bbool needSort = _queue.size() > 0;
        u32 lastHitIndex = needSort ? _queue.size()-1 : U32_INVALID;
        f32 blockingHitDist = needSort ? _queue[lastHitIndex].m_dist : 0.f;
        f32 animStart = m_characterPhys->getRadius();
        f32 animEnd = Max(animStart,shapeEnd);
        f32 animRange = animEnd - animStart;
        PolyLine* poly;
        bbool checkCollision = m_preparingHit && !m_permanentHit;
        const PolyLineEdge* edge;
        Vec2d myLookDir = AIUtils::getLookDir(m_actor);
        bbool checkFacing = m_parent->m_currentPunchHitType == PUNCHTYPE_REPEATING &&
                            myLookDir.dot(m_characterPhys->getSpeed()) < -m_parent->getTemplate()->getHitQueueBlockingHitsMinDistance();

        for ( u32 i = 0; i < numContacts; i++ )
        {
            AIUtils::SortedContactInfo& contact = sortedContacts[i];
            SCollidableContact& closestContact = contact.m_contacts[0].m_contact;
            f32 contactDist = contact.m_contacts[0].m_dist;

            if ( checkCollision )
            {
                if ( blockingHitDist && contactDist > blockingHitDist )
                {
                    // a previous hit was blocking, if this hit is close we send it, otherwise we stop
                    f32 delta = contactDist - blockingHitDist;
                    if ( delta > m_parent->getTemplate()->getHitQueueBlockingHitsMinDistance() )
                    {
                        // Flush the hits after this
                        _queue.resize(lastHitIndex+1);
                        break;
                    }
                }
            }

            hitBlocked = bfalse;

            tempHit.m_contacts.clear();

            for (u32 i = 0; i < contact.m_contacts.size() && !tempHit.m_contacts.full(); i++)
            {
                tempHit.m_contacts.push_back(contact.m_contacts[i].m_contact);
            }

            if ( checkCollision )
            {
                if ( contact.m_actor != ITF_INVALID_OBJREF )
                {
                    Actor* receiver = AIUtils::getActor(contact.m_actor);

                    if ( receiver )
                    {
                        if ( checkFacing && contact.m_polyline == ITF_INVALID_OBJREF )
                        {
                            // we don't hit an enemy if we are moving in the opposite direction of our character, and our character is not facing the target
                            Vec2d vecToTarget = receiver->get2DPos() - m_actor->get2DPos();

                            if ( myLookDir.dot(vecToTarget) < 0.f )
                            {
                                continue;
                            }
                        }

                        EventQueryBlocksHits queryBlock;
                        queryBlock.setContacts(&tempHit.m_contacts);
                        queryBlock.setCurrentPunchHitLevel(m_parent->m_currentPunchHitLevel);

                        receiver->onEvent(&queryBlock);

                        if ( queryBlock.getCanBlockHits() )
                        {
                            hitBlocked = btrue;
                        }
                    }
                }
            }

            tempHit.m_fail = bfalse;
            tempHit.m_blocks = bfalse;
            tempHit.m_dist = contactDist;
            tempHit.m_pos = closestContact.m_contactPoint;
            tempHit.m_dir = _hitDir;
            tempHit.m_closestEdgeIndex = closestContact.m_edgeIndex;
            tempHit.m_closestPoly = closestContact.m_edgeIndex != U32_INVALID ? closestContact.m_collidableUserData : ITF_INVALID_OBJREF;
            tempHit.m_closestCollideable = closestContact.m_collidable;
            tempHit.m_receiver = contact.m_actor;

            f32 animDist = Clamp(contactDist,animStart,animEnd);

            if ( animRange )
            {
                tempHit.m_animDist = ( animDist - animStart ) / animRange;
            }
            else
            {
                tempHit.m_animDist = 0.f;
            }

            tempHit.m_timer = rehitDelay;

            if ( contact.m_polyline != ITF_INVALID_OBJREF )
            {
                AIUtils::getPolyLine(contact.m_polyline,closestContact.m_edgeIndex,poly,edge);

                if ( poly->m_isCollisionActive && edge )
                {
                    // If it's a polyline of the environment and it's solid we will stop hitting
                    // Also, if we are performing a crush attack we stop when we hit any environment polyline
                    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

                    if ( mat )
                    {
                        tempHit.m_contactMaterial = mat->getFile().getStringID();
                    }

                    if ( checkCollision )
                    {
                        bbool isFail = mat && mat->getDangerousLevel();
                        bbool isSolid = !mat || mat->isSolid();

                        if ( isFail )
                        {
                            tempHit.m_fail = btrue;
                        }

                        if ( isSolid && contact.m_actor == ITF_INVALID_OBJREF )
                        {
                            f32 dotProd = _hitDir.dot(edge->m_normalizedVector.getPerpendicular());
                            
                            if ( dotProd <= 0.f )
                            {
                                hitBlocked = btrue;
                            }
                        }
                    }
                }
            }

            bbool hitBlocks = tempHit.m_fail || hitBlocked;

            if ( hitBlocks )
            {
                blockingHitDist = tempHit.m_dist;
                tempHit.m_blocks = btrue;
            }

            u32 queueSize = _queue.size();

            for ( u32 queueIndex = 0; queueIndex < queueSize; queueIndex++ )
            {
                const HitQueueInfo& hitInfo = _queue[queueIndex];

                if ( hitInfo.m_receiver == tempHit.m_receiver &&
                     hitInfo.m_closestEdgeIndex == tempHit.m_closestEdgeIndex &&
                     hitInfo.m_closestCollideable == tempHit.m_closestCollideable )
                {
                    _queue.eraseKeepOrder(queueIndex);
                    break;
                }
            }

            if ( _queue.full() )
            {
                continue;
            }

            if ( !needSort )
            {
                lastHitIndex = _queue.size();
                _queue.push_back(tempHit);
            }
            else
            {
                bbool inserted = bfalse;

                for ( u32 queueIndex = 0; queueIndex < _queue.size(); queueIndex++ )
                {
                    HitQueueInfo hitInfo = _queue[queueIndex];

                    if ( hitInfo.m_dist > tempHit.m_dist)
                    {
                        inserted = btrue;
                        _queue.insert(tempHit,queueIndex);
                        
                        if ( hitBlocks )
                        {
                            lastHitIndex = queueIndex;
                        }

                        break;
                    }
                }

                if ( !inserted )
                {
                    lastHitIndex = _queue.size();
                    _queue.push_back(tempHit);
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::releaseHitsFromQueue( PunchStim* _stim )
{
    u32 numHits = m_hitQueue.size();

    if ( !numHits )
    {
        m_skipFirstReleaseHit = bfalse;
        m_hitsReleased = btrue;
        return;
    }

    if ( !m_skipFirstReleaseHit )
    {
        m_hitsReleased = btrue;

        bbool isMemorized;
        u32 numMemorized = m_memorizedHits.size();

        for ( u32 i = 0; i < numHits; i++ )
        {
            HitQueueInfo& info = m_hitQueue[i];

            isMemorized = bfalse;

            for ( u32 memorizedIndex = 0; memorizedIndex < numMemorized; memorizedIndex++ )
            {
                const MemorizedHitInfo& memInfo = m_memorizedHits[memorizedIndex];

                if ( memInfo.m_receiver == info.m_receiver &&
                     memInfo.m_edgeIndex == info.m_closestEdgeIndex &&
                     memInfo.m_collideable == info.m_closestCollideable )
                {
                    isMemorized = btrue;
                }
            }

            if ( isMemorized )
            {
                continue;
            }

            Actor* receiver = AIUtils::getActor(info.m_receiver);

            if ( receiver )
            {
                bbool hasPhantom = bfalse;
                
                _stim->resetContacts();

                for (u32 i = 0; i < info.m_contacts.size(); i++)
                {
                    const SCollidableContact& c = info.m_contacts[i];
                    BaseObject* obj = GETOBJECT(c.m_collidable);

                    if ( obj )
                    {
                        hasPhantom |= (obj->DynamicCast<PhysPhantom>(ITF_GET_STRINGID_CRC(PhysPhantom,126559815)) != NULL);
                    }

                    _stim->addContact(c);
                }

                if ( hasPhantom )
                {
                    if ( m_parent->m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
                    {
                        if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH )
                        {
                            Faction hisFaction = AIUtils::getFaction(receiver);
                            InteractionMask interactions = FACTION_MANAGER->getInteractions(m_parent->getFaction(), hisFaction);

                            if ( (interactions & INTERACTION_CRUSHENEMY) != 0 )
                            {
                                EventQueryIsDead isDead;

                                isDead.setIsDead(btrue);

                                receiver->onEvent(&isDead);

                                if ( !isDead.getIsDead() )
                                {
                                    REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetCrushedEnemy, m_parent->m_playerIndex);
                                    REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetCrushedEnemySameAttack, m_parent->m_playerIndex);
                                }
                            }
                        }
                        else if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CROUCHKICK )
                        {
                            Faction hisFaction = AIUtils::getFaction(receiver);
                            InteractionMask interactions = FACTION_MANAGER->getInteractions(m_parent->getFaction(), hisFaction);

                            if ( (interactions & INTERACTION_RECEIVEDAMAGE) != 0 )
                            {
                                if ( m_parent->m_kungFuComboTarget.find(info.m_receiver) == -1 )
                                {
                                    EventQueryIsDead isDead;

                                    receiver->onEvent(&isDead);

                                    if ( isDead.getIsDead() == bfalse )
                                    {
                                        m_parent->m_kungFuComboTarget.push_back(info.m_receiver);
                                    }
                                }
                            }
                        }
                        else if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID &&
                                ( m_parent->m_currentPunchHitType == PUNCHTYPE_UPPERKICK || m_parent->m_currentPunchHitType == PUNCHTYPE_REPEATING ) )
                        {
                            if ( m_parent->m_kungFuComboTarget.find(info.m_receiver) != -1 )
                            {
                                Ray_EventQueryIsReceivingHit receivingHit;

                                receiver->onEvent(&receivingHit);

                                if ( receivingHit.getValue() )
                                {
                                    REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetKungFuCombo, m_parent->m_playerIndex);
                                }
                            }
                        }
                    }

                    _stim->setDirection(info.m_dir);
                    receiver->onEvent(_stim);
                }
            }
            else
            {
                static StringID s_hitWall = "HitWallFx";
                const GameMaterial_Template* mat = World::getGameMaterial(info.m_contactMaterial);
                m_fxController->playMaterialFX(s_hitWall,mat);

                if( !m_shakeCamReleased &&
                     m_parent->m_currentPunchHitType == PUNCHTYPE_CHARGE &&
                     m_parent->m_currentPunchHitLevel >= m_parent->getTemplate()->getNormalPunchLevelForShakeCam() )
                {
                    shakeCamera(m_parent->m_currentPunchHitLevel);
                    m_shakeCamReleased = btrue;
                }
            }

            if ( !m_memorizedHits.full() )
            {
                MemorizedHitInfo memorized;

                memorized.m_edgeIndex = info.m_closestEdgeIndex;
                memorized.m_poly = info.m_closestPoly;
                memorized.m_collideable = info.m_closestCollideable;
                memorized.m_receiver = info.m_receiver;
                memorized.m_timer = info.m_timer;

                m_memorizedHits.push_back(memorized);
            }
        }
    }

    m_hitQueue.clear();
    m_skipFirstReleaseHit = bfalse;
}

PunchStim* Ray_PlayerControllerComponent::StateHitRelease::setupPunchStim() const
{
    PunchStim* stim = NULL;

    stim = STIMSMANAGER->requestStim<PunchStim>(NULL);

    if(stim)
    {
        stim->setDepth(m_actor->getDepth());
        stim->setSender(m_actor->getRef());
        stim->setLevel(m_parent->m_currentPunchHitLevel);
        stim->setAngle(m_actor->getAngle());
        stim->setPushBackDistance(m_parent->getTemplate()->getPunchPushbackRadius());

        setReceiveHitTypeOnStim(stim);

        stim->setPos(m_actor->get2DPos());
        stim->setPrevPos(m_actor->get2DPos());
        stim->setFaction(m_parent->getFaction());
        Vec3d fxPos = m_parent->getHitFxPos(m_actor->getPos());
        stim->setFxPos(fxPos);
        stim->setIdentifier(m_parent->m_bubblizerIdentifier);
    }

    return stim;
}

void Ray_PlayerControllerComponent::StateHitRelease::setReceiveHitTypeOnStim( PunchStim* _stim ) const
{
    if ( m_parent->m_currentPunchHitType == PUNCHTYPE_SPIN )
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_EJECTY);
    }
    else if (m_parent->m_currentPunchHitType == PUNCHTYPE_CROUCHKICK)
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_EJECTY);
    }
    else if (m_parent->m_currentPunchHitType == PUNCHTYPE_UPPERKICK || m_parent->m_currentPunchOrientation == 90)
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_UPPUNCH);
    }
    else if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH )
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_EARTHQUAKE);
    }
    else if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CHARGE )
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_CHARGE_PUNCH);
    }
    else
    {
        _stim->setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::sendClimbImpulse( const Vec2d& _dir ) const
{
    const BaseObject* pObj = GETOBJECT(m_parent->m_climbingPolyline);
    const PolyLine* polyLine;

    if ( pObj )
    {
        ITF_ASSERT(pObj->IsClassCRC(ITF_GET_STRINGID_CRC(PolyLine,1932163747)));

        polyLine = static_cast<const PolyLine*>(pObj);

        if ( polyLine->getOwnerActor() )
        {
            Vec2d impulse = _dir*m_parent->getTemplate()->getClimbEdgePunchImpulse()*static_cast<f32>(m_parent->m_currentPunchHitLevel);

            EventRopeSwingImpulse impulseEvent(m_actor->getRef(),m_parent->m_climbingPolyline,
                m_parent->m_climbingEdge,
                m_parent->m_climbingT,
                m_characterPhys->getWeight(),
                impulse,
                m_parent->getCharacterSize());

            polyLine->getOwnerActor()->onEvent(&impulseEvent);
        }
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::setupCrushHit()
{
    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        m_characterPhys->unstick();
    }

    f32 ySpeed = m_characterPhys->getWorldSpeedAverageY();

    ySpeed = ySpeed > 0.0f?0.0f:ySpeed;
    ySpeed = Max(ySpeed,m_parent->getTemplate()->getCrushHitMinYSpeed());
    ySpeed *= m_parent->getTemplate()->getCrushHitYMultiplier();

    Vec2d speed = (-m_parent->m_currentGravityDir * ySpeed);

    m_characterPhys->setSpeed(speed);
    m_characterPhys->setUnstickMinFrictionMultiplier(1000.f);   // With this we make sure the character will not unstick when he reaches the ground
    m_resetFrictionMultiplier = btrue;
    m_crushStuckCounter = 0.f;
    m_crushPrevPos = m_actor->get2DPos();
    m_crushStartPos = m_actor->get2DPos();
    m_crushWasBlocked = bfalse;
    m_crushWasBlockedCounter = 0;

    m_parent->m_crushHitSuspensionCounter = m_parent->getTemplate()->getCrushHitSuspensionTime();

    if ( m_parent->m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
    {
        REWARD_MANAGER->resetTotalOf(Ray_RewardManager::Action_GetCrushedEnemySameAttack, m_parent->m_playerIndex);
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_CRUSHTRAVELTIME],m_crushTravelTime);
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_CRUSHRELEASED],m_crushReleased);
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_PUNCHRESULT],static_cast<u32>(m_parent->m_currentPunchResult));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_PUNCHDISTANCE],m_parent->m_currentPunchDistance);
}

void Ray_PlayerControllerComponent::StateHitRelease::processCrushHitGround( ObjectRef _poly, u32 _edgeIndex )
{
    PolyLine* poly;
    const PolyLineEdge* edge;

    AIUtils::getPolyLine(_poly,_edgeIndex,poly,edge);

    if ( poly && edge )
    {
        const Ray_GameMaterial_Template* mat = static_cast<const Ray_GameMaterial_Template*>(World::getGameMaterial(edge->getGameMaterial()));

        if ( !mat || ( !mat->getIsBounce() && !mat->bounceOnCrushAttack() ) )
        {
            m_crushWasBlocked = btrue;
        }
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::shakeCamera(u32 _level)
{
    // shake camera
    switch (_level)
    {
    case 0:
    {
        CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake0,2087111994));
    }
    break;
    case 1:
    {
        CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake1,3041659572));
    }
    break;
    case 2:
    {
        CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake2,3890011725));
    }
    break;
    case 3:
    {
        CAMERACONTROLLERMANAGER->addShake(ITF_GET_STRINGID_CRC(Quake3,4080307277));
    }
    break;
    default: ITF_WARNING(m_actor, bfalse, "No camera shake for punch level %d", _level);
    }
}

#ifdef ITF_SUPPORT_DEBUGFEATURE
void Ray_PlayerControllerComponent::StateHitRelease::addDbgShape()
{
    if ( !m_dbgDraws.full() )
    {
        m_dbgDraws.push_back(m_shape);
    }
}

void Ray_PlayerControllerComponent::StateHitRelease::drawDebug()
{
    Super::drawDebug();

    u32 numShapes = m_dbgDraws.size();

    for ( u32 i = 0; i < numShapes; i++ )
    {
        DebugDraw::shape(m_actor->get2DPos(),m_actor->getPos().m_z,m_actor->getAngle(),&m_dbgDraws[i]);
    }
}
#endif //ITF_SUPPORT_DEBUGFEATURE

}
