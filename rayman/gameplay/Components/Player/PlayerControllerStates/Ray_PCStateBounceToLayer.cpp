#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateBounceToLayer)

////////////////////
// Bounce To Layer State
////////////////////
Ray_PlayerControllerComponent::StateBounceToLayer::StateBounceToLayer()
: m_fxRef(ITF_INVALID_OBJREF)
, m_target(ITF_INVALID_OBJREF)
, m_startPos(Vec3d::Zero)
, m_totalTime(0.f)
, m_timeCounter(0.f)
, m_targetOffset(Vec2d::Zero)
, m_gravity(0.f)
, m_currentSpeed(Vec2d::Zero)
, m_prevPos(Vec2d::Zero)
, m_targetPoint(Vec3d::Zero)
, m_hurt(0)
, m_triggerDeath(bfalse)
{
}

void Ray_PlayerControllerComponent::StateBounceToLayer::onEnter()
{
    Super::onEnter();

    m_startPos = m_actor->getPos();
    m_prevPos = m_actor->get2DPos();
    m_characterPhys->setDisabled(btrue);
    m_gravity = m_characterPhys->getCurrentGravity().norm();
    m_targetPoint = getTargetPosition();
    m_triggerDeath = bfalse;

    f32 dist = ( m_startPos - m_targetPoint ).norm();

    m_totalTime = Max(dist / m_parent->getTemplate()->getBounceToActorSpeed(),m_parent->getTemplate()->getBounceToActorMinTime());

    if ( ( m_startPos.m_x > m_targetPoint.m_x && !m_actor->isFlipped() ) ||
         ( m_startPos.m_x < m_targetPoint.m_x && m_actor->isFlipped() ) )
    {
        m_parent->startUTurn(UTURNTYPE_AIR);
    }

    if ( m_hurt )
    {
        m_parent->receiveDamage(m_parent->m_receivedHitDamage);
    }
    else
    {
        m_parent->m_receivedHitType = RECEIVEDHITTYPE_UNKNOWN;
        if ( Pickable* target = (Pickable*)m_target.getObject() )
        {
            if ( DepthRange(m_actor->getDepth()) != DepthRange(target->getDepth()) )
            {
                RAY_GAMEMANAGER->triggerBounceToLayer(m_actor,m_target,m_targetOffset,m_totalTime);
            }
        }

        if ( !m_parent->getTemplate()->getBounceToLayerFXFile().isEmpty() )
        {
            Actor* bounceTrail = m_parent->m_bounceToLayerSpawnGenerator.getSpawnee(m_actor->getScene(),m_actor->getPos(),0.0f);

            if ( bounceTrail )
            {
                m_fxRef = bounceTrail->getRef();
                bounceTrail->loadResources();
                m_actor->getBinding()->bindChild(m_fxRef);
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateBounceToLayer::onExit()
{
    Super::onExit();

    m_characterPhys->setDisabled(bfalse);

    if ( m_fxRef != ITF_INVALID_OBJREF )
    {
        Actor* fxActor = m_fxRef.getActor();

        if ( fxActor )
        {
            fxActor->requestDestruction();
        }

        m_fxRef = ITF_INVALID_OBJREF;
    }
}

void Ray_PlayerControllerComponent::StateBounceToLayer::setData( ObjectRef _target, const Vec2d& _offset, bbool _hurt )
{
    m_timeCounter = 0.f;
    m_target = _target;
    m_targetOffset = _offset;
    m_hurt = _hurt;
}

bbool Ray_PlayerControllerComponent::StateBounceToLayer::checkStateChange( f32 _dt )
{
    if ( m_triggerDeath && m_parent->isDead() ||
         m_timeCounter >= m_totalTime  ||
         GETOBJECT(m_target) == NULL )
    {
        if ( m_parent->isDead() )
        {
            m_parent->triggerDeath(DEATHTYPE_REGULARDEATH);
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateFalling);
        }

        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateBounceToLayer::update( f32 _dt )
{
    Super::update(_dt);

    m_timeCounter = Min(m_timeCounter+_dt,m_totalTime);

    m_targetPoint = getTargetPosition();

    updatePosition(_dt);
    updateSpeed(_dt);

    m_prevPos = m_actor->get2DPos();

    if ( m_fxRef != ITF_INVALID_OBJREF )
    {
        Actor* fxActor = m_fxRef.getActor();

        if ( fxActor )
        {
            fxActor->setPos(m_actor->getPos());
        }
    }
}

void Ray_PlayerControllerComponent::StateBounceToLayer::updatePosition( f32 _dt )
{
    f32 t = m_timeCounter / m_totalTime;

    Vec2d p0XZ(m_startPos.m_x,m_startPos.m_z);
    Vec2d p3XZ(m_targetPoint.m_x,m_targetPoint.m_z);
    Vec2d dirXZ = p3XZ - p0XZ;
    Vec2d p1XZ = p0XZ + (dirXZ*m_parent->getTemplate()->getBounceToActorXZFuncPoint0T());
    Vec2d p2XZ = p0XZ + (dirXZ*m_parent->getTemplate()->getBounceToActorXZFuncPoint1T());
    Vec2d pXZ = getBezierPosition(p0XZ,p1XZ,p2XZ,p3XZ,t);

    f32 p0Y = m_startPos.m_y;
    f32 p3Y = m_targetPoint.m_y;
    f32 p1Y = p0Y + m_parent->getTemplate()->getBounceToActorYFuncPoint0Dist();
    f32 p2Y = p3Y + m_parent->getTemplate()->getBounceToActorYFuncPoint1Dist();
    f32 pY = getBezierPosition(p0Y,p1Y,p2Y,p3Y,t);

    Vec3d currentP = Vec3d(pXZ.m_x,pY,pXZ.m_y);

    m_actor->setPos(currentP);
}

void Ray_PlayerControllerComponent::StateBounceToLayer::updateSpeed( f32 _dt )
{
    Vec2d dist = m_actor->get2DPos() - m_prevPos;
    Vec2d speed = dist / _dt;

    m_characterPhys->setSpeed(speed);
}

Vec3d Ray_PlayerControllerComponent::StateBounceToLayer::getTargetPosition() const
{
    Vec2d targetPos = m_targetPoint.truncateTo2D();
    f32 targetZ = m_targetPoint.m_z;
    Pickable* target = static_cast<Pickable*>(GETOBJECT(m_target));

    if ( target )
    {
        targetPos = target->get2DPos() + m_targetOffset;
        targetZ = AIUtils::getSceneZFromPickable(target);

        if ( m_actor->getTemplate()->isZForced() )
        {
            targetZ += m_actor->getTemplate()->getForcedZ();
        }
    }

    return Vec3d(targetPos.m_x,targetPos.m_y,targetZ);
}

void Ray_PlayerControllerComponent::StateBounceToLayer::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == MRK_Death )
        {
            m_triggerDeath = m_parent->isDead();
        }
    }
}

#ifndef ITF_FINAL
/*
void Ray_PlayerControllerComponent::StateBounceToLayer::drawDebug()
{
    Super::drawDebug();

    Vec3d bPoint0 = ( m_targetPoint + m_startPos ) * 0.5f;
    bPoint0 += Vec3d::Up * m_height;
    Vec3d bPoint1 = m_targetPoint + (Vec3d::Up*m_height2);

    GFX_ADAPTER->drawDBGCircle(m_targetPoint, 0.1f, 1.f, 0.f, 0.f);
    GFX_ADAPTER->drawDBGCircle(bPoint0, 0.1f, 1.f, 0.f, 0.f);
    GFX_ADAPTER->drawDBGCircle(bPoint1, 0.1f, 1.f, 0.f, 0.f);
}
*/
#endif // ITF_FINAL

}