#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateIdle)

////////////////////
// Idle State
////////////////////
Ray_PlayerControllerComponent::StateIdle::StateIdle()
: m_pushedObj(ITF_INVALID_OBJREF)
, m_pushCounter(0.f)
, m_repushCounter(0.f)
, m_keepPushDelay(0.f)
, m_justEntered(bfalse)
, m_allowUnhang(bfalse)
, m_allowCrouch(bfalse)
, m_allowSupport(bfalse)
{
}

void Ray_PlayerControllerComponent::StateIdle::onEnter()
{
    Super::onEnter();

    if ( m_parent->isPerformingUTurn() )
    {
        m_parent->resetUTurn();
    }

    if ( !m_parent->m_wasOnGroundPrevFrame && m_parent->m_moveInput.m_x == 0.f && !m_characterPhys->getStickOnWalls() )
    {
        m_parent->m_sprintActivationDelay = m_parent->getTemplate()->getSprintActivationLandingDelay();
        m_parent->setSprinting(bfalse);
    }

    m_justEntered = btrue;
    m_allowUnhang = bfalse;
    m_pushedObj = ITF_INVALID_OBJREF;
    m_pushCounter = 0.f;
    m_keepPushDelay = 0.f;
    m_repushCounter = 0.f;
    m_allowCrouch = m_parent->m_disableCrouchTimer == 0.f;
    m_allowSupport = btrue;
    // If we were crouched already we allow the crouch, otherwise we need to release the stick
    m_parent->m_onCorner = CORNERPOSE_NONE;
    m_parent->resetStance(STANCE_STAND);
    m_parent->setRolling(bfalse);
    m_parent->setDesiredLookDir();
    m_parent->setLayingDown(bfalse);
    m_parent->setPathBlocked(bfalse);
    m_parent->resetPhysicAirHit();
    m_parent->m_jumpOnAirEdge = Vec2d::Zero;
}

void Ray_PlayerControllerComponent::StateIdle::onExit()
{
    m_justEntered = bfalse;
    m_parent->resetAchievedSpeedFactor();

    clearPush();

    if ( m_parent->isSupporting() )
    {
        if ( m_parent->m_nextState != &m_parent->m_stateJumping ||
             m_parent->m_objectsOnPedestal.size() == 0 )
        {
            m_parent->setSupporting(bfalse);
        }
    }

    if ( m_parent->m_nextState != &m_parent->m_stateFalling )
    {
        m_parent->setSlide(bfalse);
    }
}

bbool Ray_PlayerControllerComponent::StateIdle::checkStateChange( f32 _dt )
{
    if (checkBounce())
    {
        return btrue;
    }

    if ( m_parent->isHitQueued() && m_parent->setIdlePunch() )
    {
        return btrue;
    }

    if ( !m_justEntered && m_parent->isJumpQueued() && !m_parent->isLowRoof() )
    {
        if ( m_parent->isRolling() )
        {
            m_parent->setJumpData(m_parent->getTemplate()->getJumpMultiplierRolling(),m_parent->getTemplate()->getJumpMultiplierNoImpulseRolling(),m_parent->getTemplate()->getJumpImpulseXRolling());
        }
        else if ( m_parent->isCrouch() && !m_parent->isPerformingSlide() )
        {
            m_parent->setJumpData(m_parent->getTemplate()->getJumpMultiplierCrouch(),m_parent->getTemplate()->getJumpMultiplierNoImpulseCrouch(),m_parent->getTemplate()->getJumpImpulseXCrouch());
        }
        else if ( m_parent->isSupportHolding())
        {
            m_parent->setJumpData(m_parent->getTemplate()->getJumpMultiplierPedestal(),m_parent->getTemplate()->getJumpMultiplierNoImpulsePedestal(),m_parent->getTemplate()->getJumpImpulseXPedestal());
        }
        else
        {
            if ( m_parent->tryWallJump(btrue) )
            {
                return btrue;
            }
            else
            {
                m_parent->setJumpData();
            }
        }

        m_parent->changeState(&m_parent->m_stateJumping);
        return btrue;
    }

    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

    if ( !edge )
    {
        bbool addJumpOnAir = btrue;

        // We just unsticked from an edge, so we must fall
        if ( m_parent->isSprinting() )
        {
            StickToPolylinePhysComponent::EdgeOrientationType orientationType = m_characterPhys->getEdgeOrientationType(m_characterPhys->getCurrentGroundDir());

            if ( orientationType == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF )
            {
                m_parent->m_airControlDisableTotal = m_parent->m_airControlDisableDelay = m_parent->getTemplate()->getAirControlDelayFall();
                m_parent->m_airControlDisableBlend = bfalse;
                addJumpOnAir = bfalse;
            }
        }

        if ( addJumpOnAir )
        {
            m_parent->m_jumpOnAirTimer = m_parent->getTemplate()->getJumpOnAirTime();
        }

        f32 currentAngle = m_actor->getAngle();
        f32 angleDif = getShortestAngleDelta(currentAngle,0.f);

        if ( fabs(angleDif) >= MTH_PIBY2 )
        {
            Vec2d wallDir = Vec2d::Right.Rotate(currentAngle);

            if ( angleDif < 0.f )
            {
                m_actor->setAngle(currentAngle+MTH_PI);
            }
            else
            {
                m_actor->setAngle(currentAngle-MTH_PI);
            }

            m_actor->setFlipped(!m_actor->isFlipped());
            m_parent->resetDesiredLookDir();
            m_parent->m_fallFromRoof = btrue;
        }

        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }


    if ( edge && m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN )
    {
        if ( canTryUnhangCorner() && tryUnhangCorner() )
        {
            return btrue;
        }
    }

    if ( !m_justEntered &&
          m_parent->getMoveData().getMoveSameDir() &&
       (  m_parent->getMoveData().getMoveDirection() == MOVEDIR_LEFT ||
          m_parent->getMoveData().getMoveDirection() == MOVEDIR_RIGHT ) &&
          m_characterPhys->getSpeed() == Vec2d::Zero &&
          m_parent->tryStepMiniJump() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::StateIdle::canTryUnhangCorner() const
{
    return  m_parent->m_onCorner != CORNERPOSE_NONE &&
            m_characterPhys->getSpeed().sqrnorm() < (m_parent->getTemplate()->getHangCornerMaxSpeed()*m_parent->getTemplate()->getHangCornerMaxSpeed()) &&
           !m_parent->isPerformingSlide() &&
           !m_parent->isSprinting() &&
           !m_parent->isRolling();
}

bbool Ray_PlayerControllerComponent::StateIdle::tryUnhangCorner()
{
    PolyLine* poly = m_characterPhys->getStickedPolyline();

    if ( poly )
    {
        const PolyLineEdge* edge = m_characterPhys->getStickedEdge();
        const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

        if ( edge && ( !mat || mat->isSolid() ) )
        {
            u32 edgeIndex = m_characterPhys->getStickedEdgeIndex();
            
            f32 relDistance = m_characterPhys->getRelativeEdgeDistance();
            bbool right = relDistance > ( edge->m_length * 0.5f );
            HangingInfo hangInfo;
            bbool hang = bfalse;

            if ( m_parent->tryHangFromPolyline(poly,edge,edgeIndex,right,hangInfo) )
            {
                m_parent->m_hangInfo = hangInfo;
                hang = btrue;
            }
            else
            {
                // If the current polyline is no good, then maybe the adjacent one
                if ( relDistance < (edge->m_length*0.5f) )
                {
                    // Try on the left
                    if ( edgeIndex > 0 )
                    {
                        edgeIndex--;
                        hang = m_parent->tryHangFromPolyline(poly,
                            &poly->getEdgeAt(edgeIndex),
                            edgeIndex,
                            right,
                            hangInfo);
                    }
                }
                else
                {
                    // Try on the right
                    if ( edgeIndex < poly->getVectorsCount()-1 )
                    {
                        edgeIndex++;
                        hang = m_parent->tryHangFromPolyline(poly,
                            &poly->getEdgeAt(edgeIndex),
                            edgeIndex,
                            right,
                            hangInfo);
                    }
                }
            }

            if ( hang )
            {
                m_parent->resetUTurn();

                m_parent->m_onCorner = m_parent->calculateCornerPose(); 
                if (m_parent->isSoftCollisionEnabled() && m_parent->m_onCorner != CORNERPOSE_NONE)
                    m_parent->disableSoftCollision();
                else if (!m_parent->isSoftCollisionEnabled() && m_parent->m_onCorner == CORNERPOSE_NONE)
                    m_parent->restoreSoftCollision();

                
                m_parent->changeState(&m_parent->m_stateIdleToHang);
                return btrue;
            }
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateIdle::update(f32 _dt)
{
    f32 speedOnGround = m_characterPhys->getCurrentGroundDir().dot(m_characterPhys->getSpeed());

    bbool exitCornerPose = bfalse;

    m_parent->checkSetPedestal(exitCornerPose,m_allowSupport,m_allowCrouch);

    m_parent->UpdatePhysicWalk(_dt);
    updateSpecialMoves(speedOnGround,exitCornerPose);

    m_parent->m_onCorner = m_parent->calculateCornerPose(); 
    if (m_parent->isSoftCollisionEnabled() && m_parent->m_onCorner != CORNERPOSE_NONE)
        m_parent->disableSoftCollision();
    else if (!m_parent->isSoftCollisionEnabled() && m_parent->m_onCorner == CORNERPOSE_NONE)
        m_parent->restoreSoftCollision();

    m_parent->checkLowRoof();
    m_justEntered = bfalse;
    updateAllowFlags();
    updatePathBlocked(_dt);

    m_parent->updateSoftCollisions(_dt);

    if ( exitCornerPose &&
         m_parent->m_onCorner != CORNERPOSE_NONE &&
         m_characterPhys->getForce() == Vec2d::Zero )
    {
        m_parent->exitCornerPose(m_parent->m_onCorner,_dt);
    }

    m_parent->m_jumpOnAirEdge = m_characterPhys->getCurrentGroundDir();
}

void Ray_PlayerControllerComponent::StateIdle::updateSpecialMoves( f32 _speedOnGround, bbool& exitCornerPose )
{
    if ( m_parent->isRolling() && isCurrentAnimFinished() )
    {
        m_parent->setRolling(bfalse);
    }

    EMoveDir moveDirection = m_parent->getMoveData().getMoveDirection();

    if ( _speedOnGround )
    {
        if ( m_parent->isRolling() )
        {
            if ( moveDirection != MOVEDIR_DOWN &&
                 moveDirection != MOVEDIR_NONE )
            {
                if ( ( moveDirection == MOVEDIR_RIGHT && _speedOnGround > 0.f ) ||
                     ( moveDirection == MOVEDIR_LEFT && _speedOnGround < 0.f ) )
                {
                    ;
                }
                else
                {
                    m_parent->setRolling(bfalse);
                    m_parent->setCrouch(bfalse);
                }
            }
        }
        else if ( m_parent->isPerformingSlide() )
        {
            // See if we need to abort sliding or rolling
            if ( moveDirection != MOVEDIR_DOWN &&
                 moveDirection != MOVEDIR_NONE )
            {
                m_parent->setSlide(bfalse);
                m_parent->setCrouch(bfalse);
            }
        }
        else
        {
            if ( moveDirection == MOVEDIR_NONE )
            {
                // If we don't press anything...
                if ( m_parent->isCrouch() &&
                    !m_parent->isPerformingSlide() )
                {
                    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

                    if ( edge &&
                         f32_Abs(_speedOnGround) >= m_parent->getMinSlideSpeed(AIUtils::getEdgeFriction(edge)) )
                    {
                        // We can enter slide automatically if we move fast and we are crouch
                        m_parent->setSlide(btrue);
                    }
                }
            }
            else if ( moveDirection == MOVEDIR_DOWN )
            {
                const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

                if ( edge )
                {
                    StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(edge->m_normalizedVector);

                    if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
                    {
                        // If we are pressing down
                        if ( m_justEntered )
                        {
                            if ( f32_Abs(_speedOnGround) >= m_parent->getTemplate()->getRollingMinSpeed() )
                            {
                                // Start rolling if we just landed
                                m_parent->setRolling(btrue);
                            }
                            else if ( m_allowCrouch )
                            {
                                // Crouch if we allow it
                                m_parent->setCrouch(btrue);
                            }
                        }
                        else
                        {
                            if ( f32_Abs(_speedOnGround) >= m_parent->getMinSlideSpeed(AIUtils::getEdgeFriction(edge)) )
                            {
                                // Slide automatically if we move too fast
                                m_parent->setSlide(btrue);
                            }
                            else if ( m_allowCrouch )
                            {
                                // Crouch if we allow it
                                m_parent->setCrouch(btrue);
                            }
                        }
                    }
                }
            }
            else
            {
                // Any other case
                if ( m_parent->isCrouch() &&
                   ( !m_parent->isPerformingUTurn() || m_parent->m_uTurnCounter > 0.15f ) )
                {
                    // Uncrouch if we are sprinting
                    m_parent->setCrouch(bfalse);
                }
            }
        }
    }
    else
    {
        if ( m_parent->isRolling() )
        {
            m_parent->setRolling(bfalse);
        }

        if ( m_parent->isPerformingSlide() )
        {
            m_parent->setSlide(bfalse);
        }

        if ( m_parent->isCrouch() )
        {
            if ( moveDirection == MOVEDIR_UP )
            {
                m_parent->setCrouch(bfalse);
            }
            else if ( m_parent->isPathBlocked() &&
                    ( moveDirection == MOVEDIR_LEFT || moveDirection == MOVEDIR_RIGHT ) )
            {
                m_parent->setCrouch(bfalse);
            }
        }
        else
        {
            if ( moveDirection == MOVEDIR_DOWN && m_allowCrouch )
            {
                m_parent->setCrouch(btrue);
            }
        }
    }

    if ( m_parent->isRolling() )
    {
        // if we are still rolling we update our look direction
        f32 xSpeed = m_characterPhys->getCurrentGroundDir().dot(m_characterPhys->getSpeed());

        if ( f32_Abs(xSpeed) > 0.1f )
        {
            if ( xSpeed > 0.f )
            {
                m_parent->setLookRight(btrue);
            }
            else
            {
                m_parent->setLookRight(bfalse);
            }

            m_parent->resetDesiredLookDir();
        }
    }

    if ( m_parent->isCrouch() && !m_parent->isRolling() && !m_parent->isPerformingSlide() )
    {
        EMoveDir lookDir = m_parent->getLookDirection();

        if ( ( lookDir == MOVEDIR_RIGHT && m_actor->isFlipped() ) ||
             ( lookDir == MOVEDIR_LEFT && !m_actor->isFlipped() ) )
        {
            m_parent->startUTurn(UTURNTYPE_GROUND);
        }
    }

    if ( m_parent->isSprinting() )
    {
        exitCornerPose = btrue;
    }
}

void Ray_PlayerControllerComponent::StateIdle::updateAllowFlags()
{
    EMoveDir moveDirection = m_parent->getMoveData().getMoveDirection();

    if ( m_parent->isCrouch() )
    {
        if ( !m_allowUnhang )
        {
            if ( moveDirection == MOVEDIR_NONE )
            {
                m_allowUnhang = btrue;
            }
        }
    }
    else
    {
        m_allowUnhang = bfalse;
    }

    if ( !m_allowCrouch &&
          moveDirection != MOVEDIR_DOWN )
    {
        m_allowCrouch = btrue;
    }
}

void Ray_PlayerControllerComponent::StateIdle::clearPush()
{
    if ( m_pushedObj != ITF_INVALID_OBJREF )
    {
        m_keepPushDelay = 0.f;
        m_pushCounter = 0.f;
        m_repushCounter = 0.f;
        m_pushedObj = ITF_INVALID_OBJREF;
        m_characterPhys->setSpeed(Vec2d::Zero);
    }
}

void Ray_PlayerControllerComponent::StateIdle::updatePathBlocked( f32 _dt )
{
    m_keepPushDelay = Max(m_keepPushDelay-_dt,0.f);

    m_parent->setPathBlocked(bfalse);

    if ( m_parent->isPerformingUTurn() )
    {
        clearPush();
    }

    ObjectRef obj = ITF_INVALID_OBJREF;
    EMoveDir moveDirection = m_parent->getMoveData().getMoveDirection();

    if ( moveDirection == MOVEDIR_LEFT ||
         moveDirection == MOVEDIR_RIGHT )
    {
        // If we are trying to move
        if ( m_characterPhys->getSpeed().IsEqual(Vec2d::Zero,0.001f) || m_pushedObj != ITF_INVALID_OBJREF )
        {
            // But we have no speed
            const PhysContactsContainer& contacts = m_characterPhys->getContacts();
            const Vec2d& currentGroundDir = m_characterPhys->getCurrentGroundDir();

            for ( u32 index = 0; index < contacts.size(); index++ )
            {
                const SCollidableContact& contact = contacts[index];

                if ( contact.m_edgeIndex == U32_INVALID )
                {
                    continue;
                }

                PolyLine* poly = NULL;
                const PolyLineEdge* edge = NULL;

                AIUtils::getPolyLine(contact.m_collidableUserData,contact.m_edgeIndex,poly,edge);

                if ( !poly || !edge )
                {
                    continue;
                }

                Vec2d p0 = ( edge->getPos() - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());
                Vec2d p1 = ( (edge->getPos()+edge->m_vector) - m_actor->get2DPos() ).Rotate(-m_actor->getAngle());

                if ( ( p0.m_y > 0.f && p1.m_y > 0.f ) ||
                     ( p0.m_y < 0.f && p1.m_y < 0.f ) )
                {
                    continue;
                }

                Vec2d contactLocal = contact.m_contactPoint - m_actor->get2DPos();

                contactLocal = contactLocal.Rotate(-m_actor->getAngle());

                f32 groundWallAngle;

                if ( moveDirection == MOVEDIR_LEFT )
                {
                    if ( contactLocal.m_x > 0.f )
                    {
                        continue;
                    }

                    groundWallAngle = f32_ACos(currentGroundDir.dot(-edge->m_normalizedVector));
                }
                else
                {
                    if ( contactLocal.m_x < 0.f )
                    {
                        continue;
                    }

                    groundWallAngle = f32_ACos(-currentGroundDir.dot(edge->m_normalizedVector));
                }

                if ( groundWallAngle > m_parent->getTemplate()->getWallPushWallAngle().ToRadians() )
                {
                    continue;
                }

                const GameMaterial_Template* mat = World::getGameMaterial(edge->getGameMaterial());

                if ( mat && !mat->isSolid() )
                {
                    continue;
                }

                m_parent->setPathBlocked(btrue);
                obj = contact.m_collidableUserData;
                m_keepPushDelay = LOGICDT*3.f;
                break;
            }
        }
    }

    bbool touching = obj != ITF_INVALID_OBJREF;

    m_repushCounter = Max(m_repushCounter-_dt,0.f);

    if ( m_repushCounter && !touching )
    {
        obj = m_pushedObj;
        m_parent->setPathBlocked(btrue);
    }

    if ( obj != m_pushedObj || obj == ITF_INVALID_OBJREF )
    {
        if ( obj != ITF_INVALID_OBJREF || m_keepPushDelay == 0.f )
        {
            clearPush();
            m_pushedObj = obj;
        }
        else if ( m_pushedObj != ITF_INVALID_OBJREF )
        {
            m_parent->setPathBlocked(btrue);
        }
    }
    else
    {
        m_pushCounter = Min(m_pushCounter+_dt,m_parent->getTemplate()->getWallPushDelay());

        if ( m_pushCounter >= m_parent->getTemplate()->getWallPushDelay() && m_pushedObj != ITF_INVALID_OBJREF )
        {
            BaseObject* obj = GETOBJECT(m_pushedObj);

            if ( obj )
            {
                Actor* actor = NULL;
                PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

                if ( poly )
                {
                    actor = poly->getOwnerActor();
                }
                else
                {
                    actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                }

                if ( actor )
                {
                    Vec2d lookDir = AIUtils::getLookDir(actor,m_actor->isFlipped());
                    EventPushForce push(lookDir*m_parent->getTemplate()->getWallPushForce());

                    actor->onEvent(&push);

                    if ( touching )
                    {
                        m_repushCounter = m_parent->getTemplate()->getWallPushRepushDelay();
                    }
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateIdle::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ONCORNER],static_cast<u32>(m_parent->m_onCorner));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ROLLING],static_cast<u32>(m_parent->isRolling()));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_INTERACTING],static_cast<u32>(m_parent->isSupportHolding()));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_LAYINGDOWN],static_cast<u32>(m_parent->isLayingDown()));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ISLOWROOF],static_cast<u32>(m_parent->isLowRoof()));
}


}
