#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SCALEFUNNELCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/ChangeScale/Ray_ScaleFunnelComponent.h"
#endif //_ITF_RAY_SCALEFUNNELCOMPONENT_H_

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_ScaleFunnelComponent)

BEGIN_VALIDATE_COMPONENT(Ray_ScaleFunnelComponent)

    VALIDATE_COMPONENT_PARAM("enterRegion",getTemplate()->getEnterRegionName().isValid(), "Ray_ScaleFunnelComponent needs the name of the animation polyline to calculate the enter area");
    VALIDATE_COMPONENT_PARAM("enterPoint",getTemplate()->getEnterPoint().isValid(), "Ray_ScaleFunnelComponent needs the name of the animation polyline point to calculate the sucked-in position");
    VALIDATE_COMPONENT_PARAM("exitPoint",getTemplate()->getExitPoint().isValid(), "Ray_ScaleFunnelComponent needs the name of the animation polyline point to calculate the expulsion point");
    VALIDATE_COMPONENT_PARAM("AnimComponent",m_animComponent, "Ray_ScaleFunnelComponent needs an animation component");

END_VALIDATE_COMPONENT()

BEGIN_SERIALIZATION_CHILD(Ray_ScaleFunnelComponent)
END_SERIALIZATION()


Ray_ScaleFunnelComponent::Ray_ScaleFunnelComponent()
: m_animComponent(NULL)
, m_enterPoint(Vec2d::Zero)
, m_exitPoint(Vec2d::Zero)
{
}

Ray_ScaleFunnelComponent::~Ray_ScaleFunnelComponent()
{
}

void Ray_ScaleFunnelComponent::onActorClearComponents()
{
    u32 numActors = m_transformingActors.size();
    EventTrail showTrail(bfalse);
    EventShow alphaChange( 1.f, 0.f );
    EventStopPlayer stopPlayer(bfalse);

    for ( u32 i = 0; i < numActors; i++ )
    {
        Actor* target = m_transformingActors[i].m_ref.getActor();

        target->onEvent(&showTrail);
        target->onEvent(&alphaChange);
        target->onEvent(&stopPlayer);
    }

    m_transformingActors.clear();
}

void Ray_ScaleFunnelComponent::onActorLoaded(Pickable::HotReloadType _loadType)
{
    Super::onActorLoaded(_loadType);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryWindTunnelInfo,3603812346), this);
}

void Ray_ScaleFunnelComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    updateShape();
    checkTrigger();
    updateTransforming(_dt);
}

void Ray_ScaleFunnelComponent::updateShape()
{
    if (!m_animComponent->isLoaded())
    {
        return;
    }

    const Vec2d* points;
    u32 pointCount;

    points = m_animComponent->getCurrentPolylinePointBuffer(getTemplate()->getEnterRegionName(), &pointCount);

    FixedArray <Vec2d,32> localPoints;
    transform2d xf;

    xf.m_pos = m_actor->get2DPos();
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

    if ( !m_actor->isFlipped() )
    {
        for (u32 i = 0; i < pointCount; i++)
        {
            localPoints.push_back( mulTransform2dT(xf,points[i]) );
        }
    }
    else
    {
        for (i32 i = pointCount - 1; i >= 0; i--)
        {
            localPoints.push_back( mulTransform2dT(xf,points[i]) );
        }
    }

    m_checkShape.setPoints(localPoints);

    u32 pointIndex;

    points = m_animComponent->getCurrentFirstPolylinePoint(getTemplate()->getEnterPoint(),NULL,&pointIndex);

    if ( points )
    {
        m_enterPoint = *points;
    }

    points = m_animComponent->getCurrentFirstPolylinePoint(getTemplate()->getExitPoint(),NULL,&pointIndex);

    if ( points )
    {
        m_exitPoint = *points;
    }
}

void Ray_ScaleFunnelComponent::checkTrigger()
{
    if ( !m_checkShape.isValid() || m_enterPoint == Vec2d::Zero || m_exitPoint == Vec2d::Zero )
    {
        return;
    }

    const i32 playerCount = GAMEMANAGER->getMaxPlayerCount();
    AABB myAABB;

    m_checkShape.calculateAABB(m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(), myAABB);

    const DepthRange depthRange(m_actor->getDepth());

    for (i32 i = 0; i < playerCount; ++i)
    {
        Player* player = GAMEMANAGER->getPlayer(i);

        if ( !player->getActive() || player->isDead() )
        {
            continue;
        }

        Actor* playerActor = player->getActor();
        if ( !playerActor || depthRange != DepthRange(playerActor->getDepth()) )
        {
            continue;
        }

        if ( isTransformingActor(playerActor->getRef()) )
        {
            continue;
        }

        if ( RAY_GAMEMANAGER->isPlayerInSequence(playerActor->getRef()) )
        {
            continue;
        }

        Vec2d originalScale = playerActor->getWorldInitialScale();
        f32 currentScale = playerActor->getScale().m_x / originalScale.m_x;

        if ( currentScale == getTemplate()->getScale() && currentScale >= 1.f )
        {
            continue;
        }

        if ( !playerActor->getAABB().checkOverlap(myAABB) )
        {
            continue;
        }

        EventQueryPhysShape queryShape;

        playerActor->onEvent(&queryShape);

        if ( !queryShape.getPhysShape() )
        {
            continue;
        }

        AABB playerAABB;

        queryShape.getPhysShape()->calculateAABB(playerActor->get2DPos(), playerActor->get2DPos(), playerActor->getAngle(), playerAABB);

        if ( !myAABB.checkOverlap(playerAABB) )
        {
            continue;
        }

        PhysContactsContainer contacts;
        PhysSweepInfo sweepA, sweepB;

        PhysCollisionSolver::calculateSweepInfo(playerActor->get2DPos(), playerActor->get2DPos(), playerActor->getAngle(), queryShape.getPhysShape(), sweepA);
        PhysCollisionSolver::calculateSweepInfo(m_actor->get2DPos(), m_actor->get2DPos(), m_actor->getAngle(), &m_checkShape, sweepB);
        PhysCollisionSolver::collide(sweepA, sweepB, contacts);

        if ( contacts.size() == 0 )
        {
            continue;
        }

        triggerActor(playerActor);
    }
}

void Ray_ScaleFunnelComponent::updateTransforming( f32 _dt )
{
    u32 numTransforming = m_transformingActors.size();
    u32 i = 0;

    while ( i < numTransforming )
    {
        ActorData& actorData = m_transformingActors[i];

        if ( !updateTransformingActor(_dt,actorData) )
        {
            m_transformingActors.eraseNoOrder(i);
            numTransforming--;
        }
        else
        {
            i++;
        }
    }
}

void Ray_ScaleFunnelComponent::triggerActor( Actor* _actor )
{
    ActorData newTransform;

    newTransform.m_state = STATE_SUCKIN;
    newTransform.m_ref = _actor->getRef();

    initializeState(newTransform);

    m_transformingActors.push_back(newTransform);
}

void Ray_ScaleFunnelComponent::initializeState( ActorData& _actorData )
{
    Actor* target = _actorData.m_ref.getActor();

    if ( !target )
    {
        return;
    }

    switch(_actorData.m_state)
    {
        case STATE_SUCKIN:
            {
                EventQueryPhysicsData queryPhysics;

                target->onEvent(&queryPhysics);

                f32 t = queryPhysics.getSpeed().norm() / getTemplate()->getSuckMaxSpeed();

                t = 1.f - Clamp(t,0.f,1.f);

                _actorData.m_timer = _actorData.m_timerDuration = Interpolate(t,getTemplate()->getSuckMinTime(),getTemplate()->getSuckMaxTime());
                _actorData.m_startPos = target->get2DPos();

                Ray_EventScaleFunnelTrigger funnelEnter(m_actor->getRef(),btrue,getTemplate()->getScale(),m_exitPoint);

                target->onEvent(&funnelEnter);
            }
            break;
        case STATE_BLOWOUT:
            {
                Ray_EventScaleFunnelTrigger funnelEnter(m_actor->getRef(),bfalse,getTemplate()->getScale(),m_exitPoint);

                target->onEvent(&funnelEnter);

                BounceStim bounceStim;
                Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

                if ( m_actor->isFlipped() )
                {
                    dir *= -1.f;
                }

                bounceStim.setBounceType(BOUNCETYPE_WINDTUNEL);
                bounceStim.setBounceDir(dir);
                bounceStim.setIsRadial(bfalse);
                bounceStim.setMultiplier(1.f);
                bounceStim.setSender(m_actor->getRef());
                bounceStim.setAngle(m_actor->getAngle());
                bounceStim.setPos(m_exitPoint);

                target->onEvent(&bounceStim);
            }
            break;
    }
}

bbool Ray_ScaleFunnelComponent::updateTransformingActor( f32 _dt, ActorData& _actorData )
{
    Actor* actor = _actorData.m_ref.getActor();

    if ( !actor )
    {
        return bfalse;
    }

    if ( RAY_GAMEMANAGER->isPlayerInSequence(actor->getRef()) )
    {
        return bfalse;
    }

    bbool canContinue = btrue;

    switch(_actorData.m_state)
    {
        case STATE_SUCKIN:
            {
                _actorData.m_timer = Max(_actorData.m_timer-_dt,0.f);

                f32 t = _actorData.m_timer / _actorData.m_timerDuration;
                Vec2d pos = Interpolate(m_enterPoint,_actorData.m_startPos,t);

                EventTeleport evtTeleport( pos.to3d(actor->getPos().m_z), 0.0f, bfalse );

                actor->onEvent( &evtTeleport );

                if ( _actorData.m_timer == 0.f )
                {
                    _actorData.m_state = STATE_BLOWOUT;
                    initializeState(_actorData);
                }
            }
            break;
        case STATE_BLOWOUT:
            {
                canContinue = bfalse;
            }
            break;
    }

    return canContinue;
}

bbool Ray_ScaleFunnelComponent::isTransformingActor( ObjectRef _ref ) const
{
    u32 numActors = m_transformingActors.size();

    for ( u32 i = 0; i < numActors; i++ )
    {
        if ( m_transformingActors[i].m_ref == _ref )
        {
            return btrue;
        }
    }

    return bfalse;
}

void Ray_ScaleFunnelComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( Ray_EventQueryWindTunnelInfo* queryWindTunnel = _event->DynamicCast<Ray_EventQueryWindTunnelInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryWindTunnelInfo,3603812346)))
    {
        processQueryWindTunnelInfo(queryWindTunnel);
    }
}

void Ray_ScaleFunnelComponent::processQueryWindTunnelInfo( Ray_EventQueryWindTunnelInfo* _queryWindTunnel )
{
    _queryWindTunnel->setLevel(1);
    _queryWindTunnel->setStartPoint(m_exitPoint);

    Vec2d exitPoint = m_exitPoint;
    Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle())*getTemplate()->getExitDistance();

    if ( m_actor->isFlipped() )
    {
        dir *= -1.f;
    }

    _queryWindTunnel->setExitPoint(exitPoint + dir);
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_ScaleFunnelComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit(_drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    Vec3d screenPos;

    if ( m_checkShape.isValid() )
    {
        const PhysShapePolygon::PointsList& points = m_checkShape.getVertex();
        u32 numPoints = points.size();
        Vec2d average = Vec2d::Zero;

        for ( u32 i = 0; i < numPoints; i++ )
        {
            average += points[i];
        }

        average /= static_cast<f32>(numPoints);
        average = average.Rotate(m_actor->getAngle()) + m_actor->get2DPos();

        GFX_ADAPTER->compute3DTo2D(average.to3d(), screenPos);
        GFX_ADAPTER->drawDBGText("Interact Area", Color::white(), screenPos.m_x, screenPos.m_y);
        DebugDraw::shape(m_actor->get2DPos(), m_actor->getPos().m_z, m_actor->getAngle(), &m_checkShape);
    }

    if ( m_enterPoint != Vec2d::Zero )
    {
        GFX_ADAPTER->compute3DTo2D(m_enterPoint.to3d(), screenPos);
        GFX_ADAPTER->drawDBGText("Enter Point", Color::white(), screenPos.m_x, screenPos.m_y);
        GFX_ADAPTER->drawDBGCircle(m_enterPoint, 0.2f, 0.f, 1.f, 0.f);
    }

    if ( m_exitPoint != Vec2d::Zero )
    {
        GFX_ADAPTER->compute3DTo2D(m_exitPoint.to3d(), screenPos);
        GFX_ADAPTER->drawDBGText("Exit Point", Color::white(), screenPos.m_x, screenPos.m_y);
        GFX_ADAPTER->drawDBGCircle(m_exitPoint, 0.2f, 0.f, 1.f, 0.f);

        Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle())*getTemplate()->getExitDistance();

        if ( m_actor->isFlipped() )
        {
            dir *= -1.f;
        }

        GFX_ADAPTER->drawDBGLine(m_exitPoint,m_exitPoint+dir,Color::white());
    }
}
#endif // ITF_SUPPORT_EDITOR

IMPLEMENT_OBJECT_RTTI(Ray_ScaleFunnelComponent_Template)

BEGIN_SERIALIZATION(Ray_ScaleFunnelComponent_Template)

    SERIALIZE_MEMBER("scale",m_scale);
    SERIALIZE_MEMBER("suckMinTime",m_suckMinTime);
    SERIALIZE_MEMBER("suckMaxTime",m_suckMaxTime);
    SERIALIZE_MEMBER("suckMaxSpeed",m_suckMaxSpeed);
    SERIALIZE_MEMBER("exitDistance",m_exitDistance);
    SERIALIZE_MEMBER("enterRegion",m_enterRegionName);
    SERIALIZE_MEMBER("enterPoint",m_enterPoint);
    SERIALIZE_MEMBER("exitPoint",m_exitPoint);

END_SERIALIZATION()

Ray_ScaleFunnelComponent_Template::Ray_ScaleFunnelComponent_Template()
: m_scale(0.5f)
, m_suckMinTime(0.2f)
, m_suckMaxTime(0.5f)
, m_suckMaxSpeed(5.f)
, m_exitDistance(4.f)
{
}

};