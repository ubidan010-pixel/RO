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

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHitBase)

////////////////////
// Hit Base State
////////////////////
void War_PlayerControllerComponent::StateHitBase::onEnter()
{
    Super::onEnter();

    if ( m_parent->m_currentPunchHitType == WAR_PUNCHTYPE_EARTHQUAKE )
    {
        setupEarthquake();
    }
}

void War_PlayerControllerComponent::StateHitBase::update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes)
{
    switch(m_parent->m_stance)
    {
    case STANCE_STAND:
        updateStand(_dt,_buttons);
        break;
    case STANCE_HANG:
        updateHang(_dt,_buttons);
        break;
    case STANCE_CLIMB:
        updateClimb(_dt,_buttons);
        break;
    }
}

void War_PlayerControllerComponent::StateHitBase::updateLookDir()
{
    if ( !canUpdateLookDir() )
    {
        return;
    }

    m_parent->updateLookRightFromMoveDirection(m_parent->m_moveDirection);
}

void War_PlayerControllerComponent::StateHitBase::updateStand(f32 _dt,const InputAdapter::PressStatus* _buttons)
{
    updateLookDir();

    if ( !m_characterPhys->getStickedEdge() )
    {
        if ( m_parent->m_currentPunchHitType == WAR_PUNCHTYPE_EARTHQUAKE )
        {
            m_parent->UpdatePhysicEarthquake(_dt,m_characterPhys);
        }
        else
        {
            m_parent->UpdatePhysicAirHit(m_characterPhys);
        }
    }
}

void War_PlayerControllerComponent::StateHitBase::updateHang(f32 _dt,const InputAdapter::PressStatus* _buttons)
{
    m_parent->UpdatePhysicHanging(m_characterPhys);

    if ( !m_parent->isCurrentHangDirValid() || m_parent->isCurrentHangColliding() )
    {
        m_parent->clearHanging(m_characterPhys,m_animComponent);
    }
}

void War_PlayerControllerComponent::StateHitBase::updateClimb(f32 _dt,const InputAdapter::PressStatus* _buttons)
{
    m_parent->UpdatePhysicClimbingEdge(_dt,m_animComponent,m_characterPhys);
}

void War_PlayerControllerComponent::StateHitBase::onEvent(Event * _event)
{
    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == War_MRK_Perform_Hit)
        {
            performHit();
        }
    }
}

War_HitStim* War_PlayerControllerComponent::StateHitBase::setupHitStim( bbool _flipped ) const
{
    War_HitStim* stim = NULL;
    PhysShapePolygon* shape = m_parent->getCurrentPunchShape();

    ITF_ASSERT(shape);

    if ( shape )
    {
        Vec2d offset = m_parent->m_punchShapeOffset;
        bbool rightDir;

        if ( m_parent->isPerformingUTurn() )
        {
            rightDir = !m_parent->getIsLookingRight();
        }
        else
        {
            rightDir = m_parent->getIsLookingRight();
        }

        if ( _flipped )
        {
            rightDir = !rightDir;
        }

        shape->setFlipped(!rightDir);
        stim = STIMSMANAGER->requestStim<War_HitStim>(shape);

#if defined(ITF_FINAL)
        if(stim)
#endif // ITF_FINAL
        {
            stim->setDepth(m_actor->getDepth());
            stim->setSender(m_actor->getRef());
            stim->setLevel(m_parent->m_currentPunchHitLevel);
            stim->setAngle(m_actor->getAngle());

            setupHitStimReceivedHitType(stim);

            Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

            if ( !rightDir )
            {
                dir *= -1.f;
                offset.m_x *= -1.f;
            }

            offset = offset.Rotate(m_actor->getAngle());

            Vec2d shapePos = m_actor->get2DPos() + offset;
            stim->setPos(shapePos);
            stim->setPrevPos(shapePos);
            stim->setDirection(dir);

            Vec3d fxPos = m_parent->getHitFxPos(m_animComponent,shapePos.to3d(&m_actor->getPos().m_z));
            stim->setFxPos(fxPos);
        }
}

    return stim;
}

void War_PlayerControllerComponent::StateHitBase::setupHitStimReceivedHitType( War_HitStim* _stim ) const
{
    _stim->setReceivedHitType(WAR_RECEIVEDHITTYPE_NORMALHIT);
    // HACK
    //if ( m_parent->m_currentPunchHitType == WAR_PUNCHTYPE_ACCROBATIC_TORNADO ||
    //     m_parent->m_currentPunchHitType == WAR_PUNCHTYPE_TORNADO )
    //{
    //    _stim->setReceivedHitType(RECEIVEDHITTYPE_RANDOMEJECT);
    //}
    //else
    //{
    //    _stim->setReceivedHitType(RECEIVEDHITTYPE_NORMALPUNCH);
    //}
}

void War_PlayerControllerComponent::StateHitBase::sendClimbImpulse( const Vec2d& _dir ) const
{
    const BaseObject* pObj = GETOBJECT(m_parent->m_climbingPolyline);
    const PolyLine* polyLine;

    if ( pObj )
    {
        ITF_ASSERT(pObj->IsClass(PolyLine::GetClassNameStatic()));

        polyLine = static_cast<const PolyLine*>(pObj);

        if ( polyLine->getOwnerActor() )
        {
            Vec2d impulse = _dir*m_parent->m_climbEdgePunchImpulse*static_cast<f32>(m_parent->m_currentPunchHitLevel);

            EventPolylineImpulse impulseEvent(m_parent->m_climbingPolyline,
                m_parent->m_climbingEdge,
                m_parent->m_climbingT,
                m_characterPhys->getWeight(),
                impulse);

            polyLine->getOwnerActor()->onEvent(&impulseEvent);
        }
    }
}

void War_PlayerControllerComponent::StateHitBase::performHit()
{
    War_HitStim* stim = setupHitStim(bfalse);

    if ( stim )
    {
        STIMSMANAGER->sendStim(stim);

        if ( m_parent->m_currentPunchHitType == WAR_PUNCHTYPE_JCVD ||
             m_parent->m_currentPunchHitType == WAR_PUNCHTYPE_ACCROBATIC_TORNADO )
        {
            // Double sided stim
            stim = setupHitStim(btrue);
            STIMSMANAGER->sendStim(stim);
        }
        else if ( m_parent->m_stance == STANCE_CLIMB )
        {
            sendClimbImpulse(-stim->getDirection(m_actor->get2DPos()));
        }

            //  shake camera
        if ( m_parent->m_currentPunchHitType == WAR_PUNCHTYPE_EARTHQUAKE )
        {
            CAMERACONTROLLERMANAGER->addShake( stim->getLevel(m_actor->get2DPos()) );
        }

    }
}

void War_PlayerControllerComponent::StateHitBase::setupEarthquake()
{
    Vec2d gravityNormal = m_parent->m_currentGravityDir.getPerpendicular();
    f32 ySpeed = m_characterPhys->getWorldSpeedAverageY();

    ySpeed = Max(ySpeed,m_parent->m_earthquakeMinYSpeed);
    ySpeed *= m_parent->m_earthquakeYMultiplier;

    Vec2d speed = (-m_parent->m_currentGravityDir * ySpeed);// + (gravityNormal * xSpeed);

    m_characterPhys->setSpeed(speed);

    m_parent->m_earthquakeSuspensionCounter = m_parent->m_earthquakeSuspensionTime;
}

}