#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHitPrepare)

////////////////////
// Prepare Hit State
////////////////////
Ray_PlayerControllerComponent::StateHitPrepare::StateHitPrepare()
: m_punchLevel(0)
, m_timeCounter(0.f)
, m_justEntered(bfalse)
, m_fromUTurn(bfalse)
{
}

void Ray_PlayerControllerComponent::StateHitPrepare::onEnter()
{
    Super::onEnter();

    m_timeCounter = 0.f;
    m_punchLevel = 0;
    m_justEntered = btrue;
    
    if ( m_parent->isPerformingUTurn() )
    {
        m_fromUTurn = btrue;
        m_parent->resetUTurn();
    }
    else
    {
        m_fromUTurn = bfalse;
    }

    if ( !m_characterPhys->getStickedEdge() &&
         m_parent->m_stance == STANCE_STAND )
    {
        m_parent->preparePhysicAirHit();
    }
}

bbool Ray_PlayerControllerComponent::StateHitPrepare::checkStateChange( f32 _dt )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    if ( !m_justEntered && ( !m_parent->m_listenerAttack && !m_parent->m_listenerAttackHold ) && m_fromUTurn && !m_parent->isHanging() )
    {
        m_parent->setUTurnKick();
        return btrue;
    }

    if ( !m_justEntered && ( ( !m_parent->m_listenerAttack && !m_parent->m_listenerAttackHold ) || m_characterPhys->getStickedEdgeIndex() == U32_INVALID ) )
    {
        if ( m_punchLevel == 0 &&
           ( m_parent->m_stance == STANCE_STAND ||
             m_parent->m_stance == STANCE_SWIM ) )
        {
            m_parent->setCurrentPunch(PUNCHTYPE_REPEATING,m_parent->getTemplate()->getBurstHitLevel());
            m_parent->changeState(&m_parent->m_statePunchMitraille);
        }
        else
        {
            m_parent->setCurrentPunch(PUNCHTYPE_CHARGE,m_punchLevel);
            m_parent->changeState(&m_parent->m_stateReleaseHit);
        }
        return btrue;
    }

    return bfalse;
}

f32 Ray_PlayerControllerComponent::StateHitPrepare::getStageLength() const
{
    if ( m_punchLevel == 0 )
    {
        return m_parent->getTemplate()->getPrepareHitLengthWeak();
    }
    else if ( m_punchLevel == 1 )
    {
        return m_parent->getTemplate()->getPrepareHitLengthStrong();
    }
    else
    {
        return m_parent->getTemplate()->getPrepareHitLengthMega();
    }
}

void Ray_PlayerControllerComponent::StateHitPrepare::update(f32 _dt)
{
    Super::update(_dt);

    if ( !m_animComponent->isInTransition() )
    {
        f32 stageLength = getStageLength();
        
        m_timeCounter += _dt;

        if ( m_timeCounter >= stageLength )
        {
            m_timeCounter -= stageLength;

            if ( !m_animComponent->isAnimNodeLooped() )
            {
                m_punchLevel++;
                m_fromUTurn = bfalse;
            }
        }
    }

    ECornerPose cornerPose = m_parent->calculateCornerPose();

    if ( cornerPose != CORNERPOSE_NONE &&
         m_characterPhys->getForce() == Vec2d::Zero )
    {
        m_parent->exitCornerPose(cornerPose,_dt);
    }

    m_justEntered = bfalse;
    m_parent->setCurrentPunch(PUNCHTYPE_CHARGE,m_punchLevel);
    m_parent->updateCurrentPunchOrientation();

    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        m_parent->setActionPose(ACTIONPOSE_FIGHT);
    }
}

void Ray_PlayerControllerComponent::StateHitPrepare::updateAnimInputs()
{
    Super::updateAnimInputs();

    f32 cursor = m_timeCounter/getStageLength();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_PREPAREHITCURSOR],cursor);
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_ACTIONFROMUTURN],m_fromUTurn);
}

}