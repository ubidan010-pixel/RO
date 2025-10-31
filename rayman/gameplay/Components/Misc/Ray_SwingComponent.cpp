#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SWINGCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_SwingComponent.h"
#endif //_ITF_RAY_SWINGCOMPONENT_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_SwingComponent)

BEGIN_SERIALIZATION_CHILD(Ray_SwingComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_SwingComponent)
    VALIDATE_COMPONENT_PARAM("", m_playerDetectorComponent, "Ray_SwingComponent requires a PlayerDetectorComponent");
END_VALIDATE_COMPONENT()

Ray_SwingComponent::Ray_SwingComponent()
: m_playerDetectorComponent(NULL)
, m_animatedComponent(NULL)
, m_asleep(bfalse)
, m_armAnimInputSleep(U32_INVALID)
{
}

Ray_SwingComponent::~Ray_SwingComponent()
{
}

void Ray_SwingComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_playerDetectorComponent = m_actor->GetComponent<PlayerDetectorComponent>();
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    u32 maxPlayers = GAMEMANAGER->getMaxPlayerCount();
    u32 numArms = getTemplate()->getNumArms();

    numArms = Min(numArms,maxPlayers);

    m_arms.resize(maxPlayers);

    for ( u32 i = 0; i < numArms; i++ )
    {
        ArmData& arm = m_arms[i];

        arm.m_animState = ARMSTATE_STANDING;
        arm.m_angularVel = 0.f;
        arm.m_lengthVel = 0.f;
        arm.m_target = ITF_INVALID_OBJREF;
        arm.m_attached = bfalse;
        arm.m_graspTargetCounter = 0.f;
        arm.m_graspPoint = Vec2d::Zero;
        arm.m_realLength = 0.f;
        arm.m_currentLength = 0.f;

        if ( m_animatedComponent )
        {
            arm.m_currentAngle = arm.m_realAngle = getTemplate()->getRestAngle(i);
        }
        else
        {
            arm.m_currentAngle = arm.m_realAngle = 0.f;
        }
    }

    for ( u32 i = numArms; i < maxPlayers; i++ )
    {
        ArmData& arm = m_arms[i];

        arm.m_animState = ARMSTATE_NONE;
        arm.m_target = ITF_INVALID_OBJREF;
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSwingDetach,3105940814), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);

    if ( m_animatedComponent )
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }
}

void Ray_SwingComponent::Update( f32 _delta )
{
    checkNewSwing();
    updateMemory(_delta);
    updateArms(_delta);
}

void Ray_SwingComponent::checkNewSwing()
{
    const ActorRefList& actorsInsideThisFrame = m_playerDetectorComponent->getActorsInside();

    u32 numInsideThisFrame = actorsInsideThisFrame.size();
    u32 numArms = m_arms.size();

    for ( u32 itThisFrame = 0; itThisFrame < numInsideThisFrame; itThisFrame++ )
    {
        bbool found = bfalse;
        ActorRef actorRef = actorsInsideThisFrame[itThisFrame];

        for ( u32 itArms = 0; itArms < numArms; itArms++ )
        {
            ArmData& arm = m_arms[itArms];

            if ( !arm.isValid() )
            {
                continue;
            }

            if ( arm.m_attached && arm.m_target == actorRef )
            {
                found = btrue;
                break;
            }
        }

        if ( !found )
        {
            if ( !isInMem(actorRef) )
            {
                u32 targetArm = U32_INVALID;
                u32 freeArm = U32_INVALID;
                u32 armNotAttached = U32_INVALID;

                for ( u32 i = 0; i < numArms; i++ )
                {
                    ArmData& arm = m_arms[i];

                    if ( !arm.isValid() )
                    {
                        continue;
                    }

                    if ( arm.m_target == actorRef )
                    {
                        targetArm = i;
                        break;
                    }
                    else if ( arm.m_target == ITF_INVALID_OBJREF )
                    {
                        freeArm = i;
                    }
                    else if ( !arm.m_attached )
                    {
                        armNotAttached = i;
                    }
                }

                if ( targetArm == U32_INVALID )
                {
                    if ( freeArm == U32_INVALID )
                    {
                        // no free arms
                        if ( armNotAttached == U32_INVALID )
                        {
                            // no free arms
                            return;
                        }
                        else
                        {
                            targetArm = armNotAttached;
                        }
                    }
                    else
                    {
                        targetArm = freeArm;
                    }
                }

                Actor* actor = actorRef.getActor();

                if ( actor )
                {
                    Vec2d dirToActor = ( actor->get2DPos() - m_actor->get2DPos() );

                    if ( dirToActor == Vec2d::Zero )
                    {
                        dirToActor = Vec2d::Right;
                    }
                    else
                    {
                        dirToActor.normalize();
                    }

                    f32 actorAngle = getVec2Angle(dirToActor);
                    f32 clampedAngle = actorAngle;

                    clampAngleInRange(clampedAngle);

                    if ( clampedAngle == actorAngle )
                    {
                        Ray_EventSwingLatch swingEvent(m_actor->getRef(),getTemplate()->getLength(),getTemplate()->getAngle().ToRadians(),getTemplate()->getGravMultiplier());

                        actor->onEvent(&swingEvent);

                        ArmData& arm = m_arms[targetArm];

                        arm.m_attached = btrue;
                        arm.m_target = actorRef;
                    }
                }
            }
        }
    }
}

void Ray_SwingComponent::updateMemory( f32 _dt )
{
    u32 numMem = m_memPlayers.size();
    u32 itMem = 0;

    while ( itMem < numMem )
    {
        ActorMem& actorMem = m_memPlayers[itMem];

        actorMem.m_counter -= _dt;

        if ( actorMem.m_counter <= 0.f )
        {
            m_memPlayers.eraseNoOrder(itMem);
            numMem--;
        }
        else
        {
            itMem++;
        }
    }
}

void Ray_SwingComponent::updateArms( f32 _dt )
{
    u32 numArms = m_arms.size();

    for ( u32 itArms = 0; itArms < numArms; itArms++ )
    {
        ArmData& arm = m_arms[itArms];

        if ( !arm.isValid() )
        {
            continue;
        }

        if ( arm.m_attached )
        {
            updateAttachedArm(arm,itArms,_dt);
        }
        else if ( arm.m_target != ITF_INVALID_OBJREF )
        {
            updatePointingArm(arm,itArms,_dt,btrue);
        }
        else
        {
            updateIdleArm(arm,itArms,_dt);
        }

        updateArmAnimState(arm);
    }
}

void Ray_SwingComponent::updateAttachedArm( ArmData& _arm, u32 _index, f32 _dt )
{
    updatePointingArm(_arm,_index,_dt,bfalse);
}

void Ray_SwingComponent::updatePointingArm( ArmData& _arm, u32 _index, f32 _dt, bbool _useClamp )
{
    Actor* targetActor = _arm.m_target.getActor();
    const DepthRange depthRange(m_actor->getDepth());

    if ( !targetActor || !targetActor->isEnabled() ||
         depthRange != DepthRange(targetActor->getDepth()) ||
         !isActorInDetectRange(targetActor) )
    {
        _arm.m_target = ITF_INVALID_OBJREF;
        _arm.resetGrasp();
        return;
    }

    Ray_PlayerControllerComponent* pcComponent = targetActor->GetComponent<Ray_PlayerControllerComponent>();

    if ( !pcComponent || pcComponent->isDead() )
    {
        _arm.m_target = ITF_INVALID_OBJREF;
        _arm.resetGrasp();
        return;
    }

    bbool useGrasp = _arm.m_graspPoint != Vec2d::Zero;
    Vec2d targetP;

    if ( useGrasp )
    {
        targetP = _arm.m_graspPoint;
    }
    else
    {
        targetP = targetActor->get2DPos();
    }

    Vec2d dirToActor = targetP - m_actor->get2DPos();
    f32 targetDist = dirToActor.norm();
    
    targetDist = Min(targetDist,getTemplate()->getArmsDistance());

    f32 f = ( (targetDist-_arm.m_currentLength) * getTemplate()->getArmsLengthStiff() ) - ( _arm.m_lengthVel * getTemplate()->getArmsLengthDamp() );

    _arm.m_lengthVel += f * _dt;
    _arm.m_currentLength += _arm.m_lengthVel * _dt;

    if ( dirToActor == Vec2d::Zero )
    {
        dirToActor = Vec2d::Right;
    }
    else
    {
        dirToActor.normalize();
    }
    
    f32 angleOffset = m_actor->getAngle();
    f32 targetAngle = getVec2Angle(dirToActor) - angleOffset;

    NormalizeAnglePI(targetAngle);

    if ( _useClamp )
    {
        clampAngleInRange(targetAngle);
    }

    f32 angleDif = getShortestAngleDelta(_arm.m_currentAngle,targetAngle);

    f = ( angleDif * getTemplate()->getArmsAngleStiff() ) - ( _arm.m_angularVel * getTemplate()->getArmsAngleDamp() );

    _arm.m_angularVel += f * _dt;
    _arm.m_currentAngle += _arm.m_angularVel * _dt;

    if ( useGrasp )
    {
        f32 interpolateTime = getTemplate()->getArmsGraspInterpolateTime();

        if ( _arm.m_graspTargetCounter == interpolateTime )
        {
            _arm.m_angularVel = getShortestAngleDelta(_arm.m_realAngle,targetAngle)/LOGICDT;
            _arm.m_lengthVel = (targetDist - _arm.m_realLength)/LOGICDT;
            _arm.m_currentLength = _arm.m_realLength = targetDist;
            _arm.m_currentAngle = _arm.m_realAngle = targetAngle;
        }
        else
        {
            f32 angleDif = getShortestAngleDelta(_arm.m_currentAngle,targetAngle);
            f32 t = _arm.m_graspTargetCounter/interpolateTime;
            _arm.m_realLength = Interpolate(_arm.m_currentLength,targetDist,t);
            _arm.m_realAngle = _arm.m_currentAngle + (angleDif*t);
            _arm.m_graspTargetCounter = Min(_arm.m_graspTargetCounter+_dt,interpolateTime);
        }
    }
    else
    {
        _arm.m_realLength = _arm.m_currentLength;
        _arm.m_realAngle = _arm.m_currentAngle;
    }
}

void Ray_SwingComponent::updateIdleArm( ArmData& _arm, u32 _index, f32 _dt )
{
    // Update the length if any
    if ( _arm.m_currentLength )
    {
        f32 f = ( -_arm.m_currentLength * getTemplate()->getArmsLengthStiff() ) - ( _arm.m_lengthVel * getTemplate()->getArmsLengthDamp() );

        _arm.m_lengthVel += f * _dt;
        _arm.m_currentLength += _arm.m_lengthVel * _dt;
        _arm.m_currentLength = Max(_arm.m_currentLength,0.f);
    }

    if ( m_animatedComponent )
    {
        f32 targetAngle = getTemplate()->getRestAngle(_index);
        f32 angleDif = getShortestAngleDelta(_arm.m_currentAngle,targetAngle);
        f32 f = ( angleDif * getTemplate()->getArmsAngleStiff() ) - ( _arm.m_angularVel * getTemplate()->getArmsAngleDamp() );

        _arm.m_angularVel += f * _dt;
        _arm.m_currentAngle += _arm.m_angularVel * _dt;
    }

    // Find a target
    const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
    const DepthRange depthRange(m_actor->getDepth());
    u32 numArms = m_arms.size();
    f32 angleOffset = m_actor->getAngle() - MTH_PIBY2;

    for (u32 i = 0; i < uPlayerCount; ++i)
    {
        Actor* pActor = GAMEMANAGER->getActivePlayer(i);

        if ( !pActor || depthRange != DepthRange(pActor->getDepth()) )
        {
            continue;
        }

        Ray_PlayerControllerComponent* pcComponent = pActor->GetComponent<Ray_PlayerControllerComponent>();

        if ( !pcComponent || pcComponent->isDead() )
        {
            continue;
        }

        if ( !isActorInDetectRange(pActor) )
        {
            continue;
        }

        bbool isTarget = bfalse;
        Vec2d dirToActor = ( pActor->get2DPos() - m_actor->get2DPos() );
        
        if ( dirToActor == Vec2d::Zero )
        {
            dirToActor = Vec2d::Right;
        }
        else
        {
            dirToActor.normalize();
        }

        f32 actorAngle = getVec2Angle(dirToActor) - angleOffset;
        f32 myDif = f32_Abs(getShortestAngleDelta(_arm.m_currentAngle,actorAngle));

        for ( u32 itArms = 0; itArms < numArms; itArms++ )
        {
            const ArmData& armData = m_arms[itArms];

            if ( itArms == _index )
            {
                continue;
            }

            if ( !armData.isValid() )
            {
                continue;
            }

            if ( armData.m_target == pActor->getRef() )
            {
                isTarget = btrue;
                break;
            }

            // If that arm is closer to this target then let her target him
            if ( armData.m_target == ITF_INVALID_OBJREF )
            {
                if ( m_animatedComponent || _arm.m_currentLength != 0.f )
                {
                    f32 hisDif = f32_Abs(getShortestAngleDelta(armData.m_currentAngle,actorAngle));

                    if ( hisDif < myDif )
                    {
                        isTarget = btrue;
                        break;
                    }
                }
            }
        }

        if ( isTarget )
        {
            continue;
        }

        _arm.m_target = pActor->getRef();

        if ( !m_animatedComponent && _arm.m_currentLength  == 0.f )
        {
            _arm.m_currentAngle = actorAngle;
            clampAngleInRange(_arm.m_currentAngle);
            _arm.m_lengthVel = 0.f;
            _arm.m_angularVel = 0.f;
            _arm.resetGrasp();
        }
    }

    _arm.m_realLength = _arm.m_currentLength;
    _arm.m_realAngle = _arm.m_currentAngle;
}

void Ray_SwingComponent::updateArmAnimState( ArmData& _arm )
{
    switch(_arm.m_animState)
    {
        case ARMSTATE_SLEEP:
            {
                if ( !m_asleep )
                {
                    _arm.m_animState = ARMSTATE_WAKING;
                }
            }
            break;
        case ARMSTATE_STANDING:
            {
                if ( _arm.m_target != ITF_INVALID_OBJREF )
                {
                    _arm.m_animState = ARMSTATE_STRETCHING;
                }
            }
            break;
        case ARMSTATE_STRETCH:
            {
                if ( _arm.m_target == ITF_INVALID_OBJREF )
                {
                    _arm.m_animState = ARMSTATE_UNSTRETCHING;
                }
            }
            break;
        default:
            break;
    }
}

void Ray_SwingComponent::clampAngleInRange( f32& _angle ) const
{
    f32 minAngle = m_actor->getAngle() - MTH_PIBY2 - getTemplate()->getAngle().ToRadians();
    f32 maxAngle = m_actor->getAngle() - MTH_PIBY2 + getTemplate()->getAngle().ToRadians();

    _angle = ClampAngle(Angle(bfalse,_angle),Angle(bfalse,minAngle),Angle(bfalse,maxAngle)).ToRadians();
}

bbool Ray_SwingComponent::isActorInDetectRange( Actor* _actor ) const
{
    f32 distSq = ( _actor->get2DPos() - m_actor->get2DPos() ).sqrnorm();
    f32 rangeSq = getTemplate()->getPlayerDetectRange();

    rangeSq *= rangeSq;

    return distSq <= rangeSq;
}

void Ray_SwingComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( Ray_EventSwingDetach* detachEvent = _event->DynamicCast<Ray_EventSwingDetach>(ITF_GET_STRINGID_CRC(Ray_EventSwingDetach,3105940814)) )
    {
        processDetachEvent(detachEvent);
    }
    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        processAnimEvent(animEvent);
    }
}

void Ray_SwingComponent::processAnimEvent( class AnimGameplayEvent* _event )
{
    if ( _event->getName() == ITF_GET_STRINGID_CRC(mrk_arm0_transition_finish,2578160741) )
    {
        processArmTransitionFinished(0);
    }
    else if ( _event->getName() == ITF_GET_STRINGID_CRC(mrk_arm1_transition_finish,4078745164) )
    {
        processArmTransitionFinished(1);
    }
    else if ( _event->getName() == ITF_GET_STRINGID_CRC(mrk_arm2_transition_finish,2366572663) )
    {
        processArmTransitionFinished(2);
    }
    else if ( _event->getName() == ITF_GET_STRINGID_CRC(mrk_arm3_transition_finish,3216212104) )
    {
        processArmTransitionFinished(3);
    }
}

void Ray_SwingComponent::processArmTransitionFinished( u32 _armIndex )
{
    if ( _armIndex >= m_arms.size() )
    {
        return;
    }

    ArmData& arm = m_arms[_armIndex];

    if ( !arm.isValid() )
    {
        return;
    }

    switch(arm.m_animState)
    {
        case ARMSTATE_UNSTRETCHING:
        case ARMSTATE_WAKING:
            arm.m_animState = ARMSTATE_STANDING;
            break;
        case ARMSTATE_STRETCHING:
            arm.m_animState = ARMSTATE_STRETCH;
            break;
        default:
            break;
    }
}

void Ray_SwingComponent::setPlayerGraspPoint( const Vec2d& _point, ActorRef _actor )
{
    u32 numArms = m_arms.size();

    for ( u32 i = 0; i < numArms; i++ )
    {
        ArmData& arm = m_arms[i];

        if ( !arm.isValid() )
        {
            continue;
        }

        if ( arm.m_target == _actor )
        {
            arm.m_graspPoint = _point;
        }
    }
}

void Ray_SwingComponent::processDetachEvent( Ray_EventSwingDetach* _event )
{
    u32 numArms = m_arms.size();

    for ( u32 i = 0; i < numArms; i++ )
    {
        ArmData& arm = m_arms[i];

        if ( !arm.isValid() )
        {
            continue;
        }

        if ( arm.m_attached && arm.m_target == _event->getSender() )
        {
            arm.m_attached = bfalse;
            arm.resetGrasp();

            ActorMem actorMem;
            actorMem.m_ref = _event->getSender();
            actorMem.m_counter = getTemplate()->getMemoryDuration();

            m_memPlayers.push_back(actorMem);

            break;
        }
    }
}

bbool Ray_SwingComponent::isInMem( ActorRef _ref ) const
{
    u32 numMem = m_memPlayers.size();

    for ( u32 i = 0; i < numMem; i++ )
    {
        if ( m_memPlayers[i].m_ref == _ref )
        {
            return btrue;
        }
    }

    return bfalse;
}

void Ray_SwingComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( m_armAnimInputSleep == U32_INVALID )
    {
        m_armAnimInputIndex[ARM_STATE][0] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm0AnimState,3742302494));
        m_armAnimInputIndex[ARM_STATE][1] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm1AnimState,1326724194));
        m_armAnimInputIndex[ARM_STATE][2] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm2AnimState,38932636));
        m_armAnimInputIndex[ARM_STATE][3] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm3AnimState,1972228871));

        m_armAnimInputIndex[ARM_ANGLE][0] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm0Angle,4142951189));
        m_armAnimInputIndex[ARM_ANGLE][1] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm1Angle,2850962311));
        m_armAnimInputIndex[ARM_ANGLE][2] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm2Angle,2780405290));
        m_armAnimInputIndex[ARM_ANGLE][3] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm3Angle,2816053980));

        m_armAnimInputIndex[ARM_PRESENT][0] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm0Present,3316925951));
        m_armAnimInputIndex[ARM_PRESENT][1] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm1Present,1584357232));
        m_armAnimInputIndex[ARM_PRESENT][2] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm2Present,39991122));
        m_armAnimInputIndex[ARM_PRESENT][3] = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Arm3Present,1429838754));

        m_armAnimInputSleep = m_animatedComponent->findInputIndex(ITF_GET_STRINGID_CRC(Sleep,2301434061));
    }
}

void Ray_SwingComponent::updateAnimInput()
{
    u32 numArms = m_arms.size();

    for ( u32 i = 0; i < numArms; i++ )
    {
        ArmData& arm = m_arms[i];

        m_animatedComponent->setInputFromIndex(m_armAnimInputIndex[ARM_STATE][i],arm.m_animState);
        m_animatedComponent->setInputFromIndex(m_armAnimInputIndex[ARM_PRESENT][i],arm.m_animState!=ARMSTATE_NONE&&arm.m_animState!=ARMSTATE_STANDING);

        if ( !arm.isValid() )
        {
            continue;
        }

        f32 angle = arm.m_realAngle;

        if ( m_actor->isFlipped() )
        {
            angle += MTH_PI;
            angle *= -1.f;
        }

        NormalizeAngle2PI(angle);

        m_animatedComponent->setInputFromIndex(m_armAnimInputIndex[ARM_ANGLE][i],angle);
    }

    m_animatedComponent->setInputFromIndex(m_armAnimInputSleep,m_asleep);
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_SwingComponent_Template)
BEGIN_SERIALIZATION(Ray_SwingComponent_Template)

    SERIALIZE_MEMBER("length",m_length);
    SERIALIZE_MEMBER("angle",m_angle);
    SERIALIZE_MEMBER("gravityMultiplier",m_gravMultiplier);
    SERIALIZE_MEMBER("memoryTimer",m_memoryLength);
    SERIALIZE_MEMBER("playerDetectRange",m_playerDetectRange);
    SERIALIZE_MEMBER("armsDistance",m_armsDistance);
    SERIALIZE_MEMBER("armsLengthStiff",m_armsLengthStiff);
    SERIALIZE_MEMBER("armsLengthDamp",m_armsLengthDamp);
    SERIALIZE_MEMBER("armsAngleStiff",m_armsAngleStiff);
    SERIALIZE_MEMBER("armsAngleDamp",m_armsAngleDamp);
    SERIALIZE_MEMBER("armsGraspInterpolateTime",m_armsGraspInterpolateTime);
    SERIALIZE_MEMBER("numArms",m_numArms);
    SERIALIZE_CONTAINER("restAngles",m_restAngles);

END_SERIALIZATION()

Ray_SwingComponent_Template::Ray_SwingComponent_Template()
: m_length(1.f)
, m_angle(bfalse,MTH_PIBY2)
, m_gravMultiplier(1.f)
, m_memoryLength(2.f)
, m_playerDetectRange(5.f)
, m_armsDistance(3.f)
, m_armsLengthStiff(10.f)
, m_armsLengthDamp(1.f)
, m_armsAngleStiff(5.f)
, m_armsAngleDamp(1.f)
, m_armsGraspInterpolateTime(0.1f)
, m_numArms(1)
{
    m_restAngles.resize(4);
    m_restAngles[0].SetDegrees(45.f);
    m_restAngles[1].SetDegrees(135.f);
    m_restAngles[2].SetDegrees(225.f);
    m_restAngles[3].SetDegrees(315.f);
}

}

