#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_SWINGCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_SwingComponent.h"
#endif //_ITF_RAY_SWINGCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateSwing)

////////////////////
// Swing State
////////////////////
Ray_PlayerControllerComponent::StateSwing::StateSwing()
: m_characterSpeed(Vec2d::Zero)
, m_angularVel(0.f)
, m_adjustSpeedTimer(0.f)
, m_interpolateTimer(0.f)
, m_angle(0.f)
, m_disableJumpCounter(0.f)
, m_currentSwingQuadrant(0)
, m_prevPos(Vec2d::Zero)
, m_prevHitPosA(Vec2d::Zero)
, m_prevHitPosB(Vec2d::Zero)
, m_gotPrevHitPos(bfalse)
, m_justEntered(bfalse)
, m_canUnhang(bfalse)
{
    m_shapeHitPos.resize(4);
}

void Ray_PlayerControllerComponent::StateSwing::onEnter()
{
    Super::onEnter();

    m_justEntered = btrue;
    m_adjustSpeedTimer = m_parent->getTemplate()->getSwingSpeedAdjustTime();
    m_disableJumpCounter = m_parent->getTemplate()->getSwingJumpDelay();
    m_angle = 0.f;
    m_parent->m_swingJumpSign = 0.f;
    m_parent->setStance(STANCE_SWING);
    m_parent->playSnapFX();
    m_prevPos = m_actor->get2DPos();
    
    Actor* swingActor = AIUtils::getActor(m_parent->m_swingOnActor);

    if ( swingActor )
    {
        const Vec2d& swingPos = swingActor->get2DPos();

        startSwing(swingPos);
    }

    m_canUnhang = m_parent->getMoveData().getMoveDirection() != MOVEDIR_DOWN;
}

void Ray_PlayerControllerComponent::StateSwing::onExit()
{
    Super::onExit();

    m_parent->clearSwing();
}

bbool Ray_PlayerControllerComponent::StateSwing::checkStateChange( f32 _dt )
{
    Actor* swingActor = AIUtils::getActor(m_parent->m_swingOnActor);

    if ( !swingActor )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( !m_justEntered )
    {
        if ( m_parent->isJumpQueued() && !m_justEntered )
        {
            if ( m_disableJumpCounter )
            {
                m_parent->m_jumpQueueTimer = 0.f;
            }
            else
            {
                setJumpForceDir();
                m_parent->setJumpData();
                m_parent->changeState(&m_parent->m_stateJumping);
                return btrue;
            }
        }

        if ( m_canUnhang && m_parent->getMoveData().getMoveDirection() == MOVEDIR_DOWN )
        {
            m_parent->changeState(&m_parent->m_stateFalling);
            return btrue;
        }
    }

    if ( checkCollision() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateSwing::setJumpForceDir()
{
    EMoveDir moveDir = m_parent->getMoveData().getMoveDirection();

    if ( moveDir == MOVEDIR_RIGHT )
    {
        m_parent->m_swingJumpSign = 1.f;
    }
    else if ( moveDir == MOVEDIR_LEFT )
    {
        m_parent->m_swingJumpSign = -1.f;
    }
    else
    {
        if ( f32_Abs(m_angularVel) <= m_parent->getTemplate()->getSwingJumpXForceThreshold() )
        {
            if ( m_angle > 0.f )
            {
                m_parent->m_swingJumpSign = -1.f;
            }
            else
            {
                m_parent->m_swingJumpSign = 1.f;
            }
        }
        else
        {
            f32 angleThreshold = m_parent->getTemplate()->getSwingJumpXAngleThreshold();

            if ( ( m_angle <= m_parent->m_swingMaxAngle && m_angle >= ( m_parent->m_swingMaxAngle - angleThreshold ) ) ||
                 ( m_angle >= -m_parent->m_swingMaxAngle && m_angle <= ( -m_parent->m_swingMaxAngle + angleThreshold ) ) )
            {
                if ( m_angle > 0.f )
                {
                    m_parent->m_swingJumpSign = -1.f;
                }
                else
                {
                    m_parent->m_swingJumpSign = 1.f;
                }
            }
            else
            {
                if ( m_angularVel > 0.f )
                {
                    m_parent->m_swingJumpSign = -1.f;
                }
                else
                {
                    m_parent->m_swingJumpSign = 1.f;
                }
            }
        }
    }
}

f32 Ray_PlayerControllerComponent::StateSwing::calculateInitialAngularVelocity( const Vec2d& _swingStart ) const
{
    Vec2d speed = m_characterPhys->getSpeed();
    f32 angularVelocity = speed.norm() / m_parent->m_swingLength;
    Vec2d dirToPos = m_actor->get2DPos() - _swingStart;

    dirToPos.normalize();

    Vec2d torque = dirToPos.getPerpendicular();
    f32 dotProd = speed.dot(torque);
    f32 dotProdOpp = speed.dot(-torque);

    if ( dotProd > dotProdOpp )
    {
        angularVelocity *= -1.f;
    }

    speed.normalize();
    dotProd = speed.dot(torque);

    return angularVelocity;
}

f32  Ray_PlayerControllerComponent::StateSwing::calculateAngle( const Vec2d& _swingPos, const Vec2d& _pos ) const
{
    Vec2d dirA = _pos - _swingPos;

    dirA.normalize();

    Vec2d dirB = m_parent->m_currentGravityDir;
    f32 dotP = dirA.dot(dirB);
    f32 currentAngle = f32_ACos(dotP);

    ITF_ASSERT(currentAngle==currentAngle);

    if ( dirA.cross(dirB) < 0.f )
    {
        currentAngle *= -1.f;
    }

    return currentAngle;
}

// The main pendulum equations are:
// angle at time = m_parent->m_swingMaxAngle*cosf(f32_Sqrt(gravity/m_parent->m_swingLength)*m_latchTime);
// angular speed at angle = f32_Sqrt((2.f*gravity*m_parent->m_swingLength)*(cosf(angle)-cosf(m_parent->m_swingMaxAngle)));
// angular acceleration at angle = ( -gravity / m_parent->m_swingLength ) * sinf(angle);
void Ray_PlayerControllerComponent::StateSwing::update(f32 _dt)
{
    Actor* swingActor = AIUtils::getActor(m_parent->m_swingOnActor);

    if ( !swingActor )
    {
        return;
    }

    if ( !m_canUnhang )
    {
        m_canUnhang = m_parent->getMoveData().getMoveDirection() != MOVEDIR_DOWN;
    }

    updateOrientation(swingActor->getAngle());

    m_disableJumpCounter = Max(m_disableJumpCounter-_dt,0.f);

    const Vec2d& swingPos = swingActor->get2DPos();

    Vec2d endPos = updateSwinging(_dt,swingPos,swingActor->getAngle());

    //updateHitting(swingPos);

    if ( m_interpolateTimer )
    {
        m_actor->setPos(m_actor->getPos() + (m_characterSpeed.to3d()*_dt));
        m_interpolateTimer = Max(m_interpolateTimer-_dt,0.f);
        m_animComponent->resetLockMagicBoxFrom();
    }

    m_animComponent->lockMagicBox(endPos.to3d(m_actor->getPos().m_z),m_interpolateTimer);

    Vec2d linearSpeed = Vec2d::Right.Rotate(-m_angle) * ( (-m_angularVel) * m_parent->m_swingLength );

    m_characterPhys->setSpeed(linearSpeed);

    Ray_SwingComponent* swingComponent = swingActor->GetComponent<Ray_SwingComponent>();

    swingComponent->setPlayerGraspPoint(endPos,m_actor->getRef());
    
    updateCurrentQuadrant(m_angle);

    m_justEntered = bfalse;
    m_prevPos = m_actor->get2DPos();
}

void Ray_PlayerControllerComponent::StateSwing::updateOrientation( f32 _targetAngle )
{
    f32 currentAngle = m_actor->getAngle();

    if ( currentAngle != _targetAngle )
    {
        f32 dif = getShortestAngleDelta(m_actor->getAngle(),_targetAngle);

        if ( fabs(dif) > MTH_EPSILON )
        {
            dif *= 0.5f;
        }

        f32 newAngle = currentAngle + dif;

        m_actor->setAngle(newAngle);
    }
}

void Ray_PlayerControllerComponent::StateSwing::updateCurrentQuadrant( f32 _angle )
{
    if ( m_angle <= -MTH_PIBY2 )
    {
        if ( m_actor->isFlipped() )
        {
            m_currentSwingQuadrant = 3;
        }
        else
        {
            m_currentSwingQuadrant = 0;
        }
    }
    else if ( m_angle <= 0.f )
    {
        if ( m_actor->isFlipped() )
        {
            m_currentSwingQuadrant = 2;
        }
        else
        {
            m_currentSwingQuadrant = 1;
        }
    }
    else if ( m_angle <= MTH_PIBY2 )
    {
        if ( m_actor->isFlipped() )
        {
            m_currentSwingQuadrant = 1;
        }
        else
        {
            m_currentSwingQuadrant = 2;
        }
    }
    else
    {
        if ( m_actor->isFlipped() )
        {
            m_currentSwingQuadrant = 0;
        }
        else
        {
            m_currentSwingQuadrant = 3;
        }
    }
}

void Ray_PlayerControllerComponent::StateSwing::startSwing( const Vec2d& _swingPos )
{
    m_angle = calculateAngle(_swingPos,m_actor->get2DPos());
    m_angularVel = calculateInitialAngularVelocity(_swingPos);
    m_angle += m_angularVel*LOGICDT*3.f;
    m_characterSpeed = m_characterPhys->getSpeed();
    m_characterPhys->setDisabled(btrue);

    NormalizeAnglePI(m_angle);

    Vec2d speed = m_characterSpeed;
    Vec2d dirToPos = m_actor->get2DPos() - _swingPos;

    dirToPos.normalize();
    speed.normalize();

    f32 dotProd = speed.dot(dirToPos);

    m_interpolateTimer = Max(0.1f,m_parent->getTemplate()->getSwingInterpolateTime() * f32_Abs(dotProd));

    if ( ( m_angle > 0.f && m_angle <= m_parent->m_swingMaxAngle ) ||
         ( m_angle < 0.f && m_angle >= -m_parent->m_swingMaxAngle ) )
    {
    }
    else
    {
        // Out of the range of our pendulum
        if ( (m_angle > 0.f) == (m_angularVel > 0.f) )
        {
            // we will be losing speed, so we need a boost so that we can pass and complete the circle
            f32 gravity = m_parent->m_currentGravity.norm()*m_parent->m_swingGravMultiplier;
            f32 accel = getAcceleration(gravity,m_angle);

            f32 expectedSpeed = getExpectedSpeed(gravity,m_angle,MTH_PI) + (accel*LOGICDT) + m_parent->getTemplate()->getSwingInitialBoost();
            
            if ( f32_Abs(m_angularVel) < expectedSpeed )
            {
                m_angularVel = expectedSpeed * getSign(m_angularVel);
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateSwing::updateHitting( const Vec2d& _swingPos )
{
    if ( f32_Abs(m_angularVel) >= m_parent->getTemplate()->getSwingHitMinAngularSpeed() )
    {
        if ( m_gotPrevHitPos )
        {
            processHitSwing(_swingPos);
        }
        else
        {
            calculateHitPos(_swingPos,m_prevHitPosA,m_prevHitPosB);
            m_gotPrevHitPos = btrue;
        }
    }
    else
    {
        m_gotPrevHitPos = bfalse;
    }
}

f32 Ray_PlayerControllerComponent::StateSwing::getAcceleration( f32 _gravity, f32 _angle ) const
{
    return ( -_gravity / m_parent->m_swingLength ) * f32_Sin(_angle);
}

f32 Ray_PlayerControllerComponent::StateSwing::getExpectedSpeed( f32 _gravity, f32 _angle, f32 _maxAngle ) const
{
    f32 diff = f32_Cos(_angle)-f32_Cos(_maxAngle);
    diff = f32_Sel(f32_Abs(diff)-MTH_EPSILON, diff, 0.f);
    return f32_Sqrt((2.f*(_gravity/m_parent->m_swingLength))*diff);
}

Vec2d Ray_PlayerControllerComponent::StateSwing::updateSwinging( f32 _dt, const Vec2d& _swingPos, f32 _swingActorAngle )
{
    f32 gravity = m_parent->m_currentGravity.norm()*m_parent->m_swingGravMultiplier;

    if ( m_adjustSpeedTimer )
    {
        // During this time we try to reach a specific angular velocity at specific angles so that we accomplish
        // the desired pendular motion
        if ( ( m_angle > 0.f && m_angle <= m_parent->m_swingMaxAngle ) ||
             ( m_angle < 0.f && m_angle >= -m_parent->m_swingMaxAngle ) )
        {
            // Only do the adjustments while we are inside the pendulum range
            f32 expectedVelocity = getExpectedSpeed(gravity,m_angle,m_parent->m_swingMaxAngle);
            f32 t = 1.f - ( m_adjustSpeedTimer / m_parent->getTemplate()->getSwingSpeedAdjustTime() );

            if ( m_angularVel < 0.f )
            {
                expectedVelocity *= -1.f;
            }

            m_angularVel = Interpolate(m_angularVel,expectedVelocity,t);
            ITF_ASSERT(m_angularVel==m_angularVel);

            m_adjustSpeedTimer = Max(m_adjustSpeedTimer-_dt,0.f);
        }

        f32 acc = getAcceleration(gravity,m_angle);

        m_angularVel = m_angularVel + (acc*_dt);
    }
    else
    {
        // We follow a fixed pendular motion
        m_angle = Clamp(m_angle,-m_parent->m_swingMaxAngle,m_parent->m_swingMaxAngle);

        f32 expectedSpeed = getExpectedSpeed(gravity,m_angle,m_parent->m_swingMaxAngle);

        if ( m_angularVel > 0.f )
        {
            m_angularVel = expectedSpeed;
        }
        else if ( m_angularVel < 0.f )
        {
            m_angularVel = -expectedSpeed;
        }
        else
        {
            f32 acc = getAcceleration(gravity,m_angle);
            m_angularVel = m_angularVel + (acc*_dt);
        }
    }

    m_angle += (m_angularVel * _dt);

    NormalizeAnglePI(m_angle);

    Vec2d dir = m_parent->m_currentGravityDir.Rotate(-m_angle+_swingActorAngle);
    Vec2d endPos = _swingPos + (dir*m_parent->m_swingLength);

    return endPos;
}

bbool Ray_PlayerControllerComponent::StateSwing::checkCollision() const
{
    PhysContactsContainer contacts;
    PhysShapeCircle circle(m_characterPhys->getRadius());

    PHYSWORLD->checkEncroachment(m_prevPos,m_actor->get2DPos(),0.f,&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

    u32 numContacts = contacts.size();
    PolyLine* poly;
    const PolyLineEdge* edge;

    for ( u32 i = 0; i < numContacts; i++ )
    {
        const SCollidableContact& c = contacts[i];

        if ( c.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

        if ( !poly || !edge )
        {
            continue;
        }

        const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

        if ( mat && !mat->isSolid() )
        {
            continue;
        }

        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateSwing::calculateHitPos( const Vec2d& _swingPos, Vec2d& _posA, Vec2d& _posB ) const
{
    Vec2d shapeDir = m_actor->get2DPos() - _swingPos;

    shapeDir.normalize();

    _posA = m_actor->get2DPos() + (shapeDir*m_parent->getTemplate()->getSwingHitAreaOffset());
    _posB = m_actor->get2DPos() + (shapeDir*-m_parent->getTemplate()->getSwingHitAreaHeight());
}

void Ray_PlayerControllerComponent::StateSwing::processHitSwing( const Vec2d& _swingPos )
{
    Vec2d newPosA, newPosB;

    calculateHitPos(_swingPos,newPosA,newPosB);

    Vec2d hitDir = newPosA - m_prevHitPosA;

    hitDir.normalize();

    if ( hitDir != Vec2d::Zero )
    {
        if ( hitDir.m_x != 0.f )
        {
            if ( hitDir.m_x > 0.f )
            {
                // Moving to the right
                m_shapeHitPos[0] = m_prevHitPosA;
                m_shapeHitPos[1] = newPosA;
                m_shapeHitPos[2] = newPosB;
                m_shapeHitPos[3] = m_prevHitPosB;
            }
            else
            {
                // Moving to the left
                m_shapeHitPos[0] = newPosA;
                m_shapeHitPos[1] = m_prevHitPosA;
                m_shapeHitPos[2] = m_prevHitPosB;
                m_shapeHitPos[3] = newPosB;
            }
        }
        else
        {
            if ( hitDir.m_y > 0.f )
            {
                // Moving to the right
                m_shapeHitPos[0] = m_prevHitPosA;
                m_shapeHitPos[1] = newPosA;
                m_shapeHitPos[2] = newPosB;
                m_shapeHitPos[3] = m_prevHitPosB;
            }
            else
            {
                // Moving to the left
                m_shapeHitPos[0] = newPosA;
                m_shapeHitPos[1] = m_prevHitPosA;
                m_shapeHitPos[2] = m_prevHitPosB;
                m_shapeHitPos[3] = newPosB;
            }
        }
    }

    m_parent->m_swingHitShape.setPoints(m_shapeHitPos);

    PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>(&m_parent->m_swingHitShape);

#if defined(ITF_FINAL)
    if(stim)
#endif // ITF_FINAL
    {
        stim->setDepth(m_actor->getDepth());
        stim->setSender(m_actor->getRef());
        stim->setLevel(m_parent->getTemplate()->getSwingHitLevel());
        stim->setAngle(0.f);
        stim->setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
        stim->setPos(Vec2d::Zero);
        stim->setPrevPos(Vec2d::Zero);
        stim->setDirection(hitDir);
        stim->setFxPos(newPosA.to3d(m_actor->getPos().m_z));
        STIMSMANAGER->sendStim(stim);
    }

    m_prevHitPosA = newPosA;
    m_prevHitPosB = newPosB;
}

void Ray_PlayerControllerComponent::StateSwing::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_SWINGQUADRANT],m_currentSwingQuadrant);
}

}