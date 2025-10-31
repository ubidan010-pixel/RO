#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateOnPedestal)

//////////////////////////////////////////////////////////////////////////
// ON PEDESTAL
//////////////////////////////////////////////////////////////////////////
void Ray_PlayerControllerComponent::StateOnPedestal::onEnter()
{
    Super::onEnter();

    m_allowSupport = btrue;
    m_justEntered = btrue;
    m_parent->setCrouch(bfalse);
    m_parent->stopHelicoFx();
    m_parent->m_reducedHitUpDisabledUntilLand = bfalse;
    m_prevPos = m_actor->get2DPos();

    if ( m_parent->m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
    {
        REWARD_MANAGER->resetTotalOf(Ray_RewardManager::Action_GetBouncedEnemiesSameJump, m_parent->m_playerIndex);
    }

    m_parent->m_kungFuComboTarget.clear();
    m_parent->m_walkOnWallsDisableUntilLand = bfalse;
}

bbool Ray_PlayerControllerComponent::StateOnPedestal::checkStateChange( f32 _dt )
{
    if ( !m_parent->m_onPedestalValid )
    {
        m_parent->stopOnPedestal();
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    EMoveDir moveDirection = m_parent->getMoveData().getMoveDirection();

    if ( moveDirection == MOVEDIR_DOWN && m_parent->m_onPedestalAllowFall )
    {
        m_parent->stopOnPedestal();
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( !m_justEntered )
    {
        if ( m_parent->isJumpQueued() && !m_parent->isLowRoof() )
        {
            if ( m_parent->m_onPedestalLastPos != Vec2d::Zero )
            {
                Actor* pedestalActor = AIUtils::getActor(m_parent->m_onPedestal);

                if ( pedestalActor )
                {
                    Vec2d gravityNormal = m_parent->m_currentGravityDir.getPerpendicular();
                    Vec2d posDif = pedestalActor->get2DPos() - m_parent->m_onPedestalLastPos;
                    f32 moveX = gravityNormal.dot(posDif);
                    f32 speedX = moveX/LOGICDT;
                    m_parent->m_currentJumpSpeedX = gravityNormal * speedX;
                }
            }

            m_parent->stopOnPedestal();
            m_parent->m_jumpOnAirEdge = m_parent->m_currentGravityDir.getPerpendicular();
            m_parent->setJumpData(m_parent->getTemplate()->getJumpMultiplierOnPedestal(),m_parent->getTemplate()->getJumpMultiplierNoImpulseOnPedestal(),m_parent->getTemplate()->getJumpImpulseXOnPedestal());
            m_parent->changeState(&m_parent->m_stateJumping);
            return btrue;
        }
    }

    if  ( m_parent->isHitQueued() && m_parent->setIdlePunch() )
    {
        return btrue;
    }

    const Vec2d& newPos = m_actor->get2DPos();

    if ( m_prevPos != newPos )
    {
        PhysShapeCircle circle(m_characterPhys->getRadius());
        PhysContactsContainer contacts;

        PHYSWORLD->checkEncroachment(m_prevPos,newPos,m_actor->getAngle(),&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

        if ( contacts.size() > 0 )
        {
            m_parent->stopOnPedestal();
            m_actor->set2DPos(contacts[0].m_testPosition);
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }

    if ( m_parent->isLowRoof() )
    {
        m_parent->stopOnPedestal();
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateOnPedestal::update(f32 _dt)
{
    bbool exitCornerPose, allowCrouch;

    m_parent->setMoveInput(Vec2d::Zero);
    m_parent->checkLowRoof();
    m_parent->checkSetPedestal(exitCornerPose,m_allowSupport,allowCrouch);

    EMoveDir lookDir = m_parent->getLookDirection();

    if ( ( lookDir == MOVEDIR_LEFT && !m_actor->isFlipped() ) ||
         ( lookDir == MOVEDIR_RIGHT && m_actor->isFlipped() ) )
    {
        if ( !m_parent->isPerformingUTurn() )
        {
            m_parent->startUTurn(UTURNTYPE_GROUND);
        }
        else
        {
            // If we are trying to uturn while doing a uturn, then reset and continue doing it
            if ( m_parent->m_uTurnFlipped )
            {
                m_parent->resetUTurn();
                m_parent->startUTurn(UTURNTYPE_GROUND);
                m_animComponent->resetTree();
            }
        }
    }

    Actor* pedestalActor = AIUtils::getActor(m_parent->m_onPedestal);

    if ( pedestalActor )
    {
        m_parent->m_onPedestalLastPos = pedestalActor->get2DPos();
    }
    else
    {
        m_parent->m_onPedestalLastPos = Vec2d::Zero;
    }

    m_justEntered = bfalse;
    m_prevPos = m_actor->get2DPos();

    if ( m_parent->m_onPedestalInterpolateCounter )
    {
        m_animComponent->setAnim(ITF_GET_STRINGID_CRC(FALL,100613701));
    }
    else
    {
        m_animComponent->setAnim(ITF_GET_STRINGID_CRC(Idle,1633200266));
    }
}

}