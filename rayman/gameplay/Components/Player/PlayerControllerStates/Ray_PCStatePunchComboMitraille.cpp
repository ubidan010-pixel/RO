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

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StatePunchComboMitraille)

void  Ray_PlayerControllerComponent::StatePunchComboMitraille::onEnter()
{
    m_parent->m_currentPunchHitType = PUNCHTYPE_REPEATING;

    Super::onEnter();

    m_burstStage = 0;
    m_numHits = 1;
    m_canContinue = btrue;
    m_comboHitNext = bfalse;
    m_parent->m_burstPunchToDeliver = 0;
}

bbool Ray_PlayerControllerComponent::StatePunchComboMitraille::checkStateChange( f32 _dt )
{
    if (checkBounce())
    {
        return btrue;
    }

    bbool animFinished = isCurrentAnimFinished();

    if ( m_parent->m_currentPunchResult == PUNCHRELEASERESULT_FAIL )
    {
        if ( animFinished )
        {
            changeToNextState();
            return btrue;
        }

        return bfalse;
    }

    bbool onGround = m_characterPhys->getStickedEdge() != NULL || m_parent->isOnPedestal();

    if ( m_numHits >= m_parent->getTemplate()->getMetrailleMaxHits() &&
         onGround && m_comboHitNext && m_parent->m_burstPunchToDeliver )
    {
        m_parent->setCurrentPunch(PUNCHTYPE_CHARGE,m_parent->getTemplate()->getMetrailleFinalHitLevel());
        m_parent->changeState(&m_parent->m_stateReleaseHit);
    }

    if ( m_parent->isHitQueued() )
    {
        if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN )
        {
            if ( onGround )
            {
                // Trying to change to crouch kick
                m_parent->setCurrentPunch(PUNCHTYPE_CROUCHKICK,m_parent->getTemplate()->getCrouchHitLevel());
                m_parent->changeState(&m_parent->m_stateReleaseHit);
                return btrue;
            }
            else
            {
                if ( !m_parent->isOnGeyser() && !m_parent->m_crushRetriggerCounter )
                {
                    m_parent->setCurrentPunch(PUNCHTYPE_CRUSH,m_parent->getTemplate()->getCrushHitLevel());
                    m_parent->cancelPhysicAirHit();
                    m_parent->changeState(&m_parent->m_stateReleaseHit);
                }
            }
        }
        else if ( m_parent->getMoveData().getMoveDirection() == MOVEDIR_UP )
        {
            if ( !onGround )
            {
                m_parent->setCurrentPunch(PUNCHTYPE_UPPERKICK,m_parent->getTemplate()->getUpperKickHitLevel());
                m_parent->changeState(&m_parent->m_stateReleaseHit);
            }
        }
    }

    if ( !onGround && m_canMove )
    {
        EMoveDir lookDir = m_parent->getLookDirection();

        if ( ( lookDir == MOVEDIR_RIGHT && m_actor->isFlipped() ) ||
             ( lookDir == MOVEDIR_LEFT && !m_actor->isFlipped() ) )
        {
            changeToNextState();
            return btrue;
        }
    }

    bbool exitToMove = checkChangeStateWantsMove() && m_characterPhys->getStickedEdge();
    bbool jump = m_characterPhys->getStickedEdge() && m_parent->isJumpQueued();
    bbool pressedAction = m_parent->m_burstPunchToDeliver == 0 && ( jump || m_parent->shouldTriggerHelicopterAction() );
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

    if ( m_parent->m_burstPunchToDeliver == 0 &&
       ( animFinished ||
       ( m_canMove && ( exitToMove || pressedAction || m_exitSoon ) ) ||
       ( m_parent->m_stance == STANCE_SWIM && exitSwim ) ) )
    {
        changeToNextState();
        return btrue;
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::StatePunchComboMitraille::canUpdateLookDir() const
{
    return bfalse;
}

Vec2d Ray_PlayerControllerComponent::StatePunchComboMitraille::getBaseHitDir() const
{
    if ( m_parent->isPerformingUTurn() )
    {
        f32 angle = m_actor->getAngle();

        if ( m_actor->isFlipped() )
        {
            angle += MTH_PI;
        }

        angle += MTH_PI;

        return Vec2d::Right.Rotate(angle);
    }
    else
    {
        return Super::getBaseHitDir();
    }
}

void Ray_PlayerControllerComponent::StatePunchComboMitraille::update(f32 _dt)
{
    bbool onGround = m_characterPhys->getStickedEdge() != NULL || m_parent->isOnPedestal();

    if ( onGround )
    {
        m_parent->setCurrentPunch(PUNCHTYPE_REPEATING,m_parent->getTemplate()->getBurstHitLevel());
    }
    else
    {
        m_parent->setCurrentPunch(PUNCHTYPE_REPEATING,m_parent->getTemplate()->getBurstAirHitLevel());
    }

    Super::update(_dt);

    m_parent->updateCurrentPunchOrientation();

    if ( m_parent->isHitQueued() )
    {
       if ( m_canContinue )
        {
            m_parent->m_burstPunchToDeliver = Min(m_parent->m_burstPunchToDeliver+1,m_parent->getTemplate()->getMetrailleMaxBufferedHits());
            m_parent->m_hitQueueTimer = 0.f;
        }
       else
       {
           m_parent->m_hitQueueTimer = 0.f;
           m_canMove = bfalse;
           m_comboHitNext = bfalse;
           m_hitsReleased = bfalse;
           m_performingHit = bfalse;
           m_preparingHit = bfalse;
           m_hitQueue.clear();
           m_memorizedHits.clear();
           m_numHits = 1;
           m_burstStage = 0;
           m_parent->m_burstPunchToDeliver = 0;
           m_parent->m_currentPunchDistance = 1.f;
           m_parent->m_currentPunchResult = PUNCHRELEASERESULT_NOIMPACT;
           m_canContinue = btrue;
           m_animComponent->resetTree();
           m_hitSomethingThisRound = bfalse;
           m_switchHitTimer = m_parent->getTemplate()->getSwitchHitBufferTime();
           return;
       }
    }

    if ( m_hitsReleased )
    {
        if ( !m_performingHit )
        {
            m_canContinue = bfalse;
        }

        if ( m_parent->m_burstPunchToDeliver &&
             m_comboHitNext &&
           ( m_numHits < m_parent->getTemplate()->getMetrailleMaxHits() || !onGround ) )
        {
            m_canMove = bfalse;
            m_comboHitNext = bfalse;
            m_hitsReleased = bfalse;
            m_performingHit = bfalse;
            m_preparingHit = bfalse;
            m_hitQueue.clear();
            m_memorizedHits.clear();
            m_numHits++;

            u32 numAnims;

            if ( m_characterPhys->getStickedEdge() )
            {
                numAnims = m_parent->getTemplate()->getMetrailleGroundVariation();
            }
            else
            {
                numAnims = m_parent->getTemplate()->getMetrailleAirVariation();
            }

            m_burstStage = (m_numHits-1)%numAnims;
            m_parent->m_burstPunchToDeliver--;
            m_parent->m_currentPunchDistance = 1.f;
            m_parent->m_currentPunchResult = PUNCHRELEASERESULT_NOIMPACT;
            m_canContinue = btrue;
            m_animComponent->resetTree();
            m_hitSomethingThisRound = bfalse;
        }
    }
}

void Ray_PlayerControllerComponent::StatePunchComboMitraille::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == MRK_ComboHit_Next)
        {
            m_comboHitNext = btrue;
        }
    }
}

void Ray_PlayerControllerComponent::StatePunchComboMitraille::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_PUNCHBURSTSTAGE],m_burstStage);
}

}