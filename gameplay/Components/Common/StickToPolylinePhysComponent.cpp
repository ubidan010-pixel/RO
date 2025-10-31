#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_INTERSECTION_H_
#include "engine/physics/Collision/Intersection.h"
#endif //_ITF_INTERSECTION_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(StickToPolylinePhysComponent)

BEGIN_SERIALIZATION_CHILD(StickToPolylinePhysComponent)
END_SERIALIZATION()

#define MEMORY_EDGE_TIME 0.1f

const f32 StickToPolylinePhysComponent::s_wallRunAngleThreshold = MTH_PIBY4;
const f32 StickToPolylinePhysComponent::s_physWallAngle = 63.43f * MTH_DEGTORAD;
const f32 StickToPolylinePhysComponent::s_physRoofAngle = 116.56f * MTH_DEGTORAD;

void StickToPolylinePhysComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    onForceMove();
}

StickToPolylinePhysComponent::StickToPolylinePhysComponent()
: Super()
, m_stickedLineRef(ITF_INVALID_OBJREF)
, m_stickedEdge(U32_INVALID)
, m_currentGroundDir(Vec2d::Zero)
, m_extraCollisionCheckPos(Vec2d::Zero)
, m_extraCollisionCheckRadius(0.f)
, m_unstickMinFrictionMultiplier(1.f)
, m_angularSpeedMultiplier(1.f)
, m_groundAngularSpeedMultiplierMaxAngle(bfalse,1.f)
, m_groundCurrentAngularSpeed(MTH_2PI)
, m_speedLossMultiplier(1.0f)
, m_speedMultiplier(Vec2d::One)
, m_windForcesThisFrame(Vec2d::Zero)
, m_windSpeedMultiplier(Vec2d::One)
, m_relDist(0.f)
, m_insideEdgeFactor(0.f)
, m_betweenEdgesFactor(0.f)
, m_relAngle(0.f)
, m_targetAngle(0.f)
, m_frictionMultiplier(1.f)
, m_airFrictionMultiplier(0.f)
, m_currentFrictionCoef(0.f)
, m_frictionProjection(0.f)
, m_averageXSpeed(0.f)
, m_averageYSpeed(0.f)
, m_prevPosition(Vec2d::Zero)
, m_worldSpeed(Vec2d::Zero)
, m_averageTorque(0.f)
, m_prevAngle(0.f)
, m_prevGroundAngle(0.f)
, m_torqueRotationSign(0.f)
, m_forceRotateWithGravity(bfalse)
, m_updateRotation(btrue)
, m_debugLog(bfalse)
, m_processContacts(btrue)
, m_rotateWithTorque(bfalse)
, m_forceEdgeAngleNext(bfalse)
, m_disableSticking(bfalse)
, m_hasExternalForces(bfalse)
, m_disableWindForces(bfalse)
, m_stickOnWalls(bfalse)
, m_stickOnWallsInputDir(Vec2d::Zero)
, m_stickOnWallsCheckConvex(btrue)
, m_stickOnWallsShiftGravity(btrue)
, m_maxSpeedEnabled(bfalse)
, m_maxSpeedX(0.f)
, m_maxSpeedY(0.f)
, m_maxSpeedTotal(0.f)
, m_cornerCaseDisableDelay(0.f)
{
    for ( u32 i = 0; i < s_speed_average_num_frames; i++ )
    {
        m_averageXSpeedMeasures[i] = 0.f;
        m_averageYSpeedMeasures[i] = 0.f;
    }

    for ( u32 i = 0; i < s_torque_average_num_frames; i++ )
    {
        m_averageTorqueMeasures[i] = 0.f;
    }
}

StickToPolylinePhysComponent::~StickToPolylinePhysComponent()
{
}

void StickToPolylinePhysComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    updateCurrentGravity(m_actor->get2DPos());

    m_prevPosition = m_actor->get2DPos();
    m_prevAngle = m_actor->getAngle();
    m_prevGroundAngle = m_prevAngle;

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTeleport,1075308886),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetSpeed,2924911315),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventUnstick,54024986),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventFluidCollisionPolylineChanged,3871666563),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventForceStickOnEdge,3946845675),this);
}

void StickToPolylinePhysComponent::enableMaxSpeedClamping( f32 _maxSpeedTotal, f32 _maxSpeedX, f32 _maxSpeedY )
{
    m_maxSpeedEnabled = btrue;
    m_maxSpeedX = _maxSpeedX;
    m_maxSpeedY = _maxSpeedY;
    m_maxSpeedTotal = _maxSpeedTotal;
}

void StickToPolylinePhysComponent::disableMaxSpeed()
{
    m_maxSpeedEnabled = bfalse;
}

f32 StickToPolylinePhysComponent::getWallAngle() const
{
    return s_physWallAngle;
}

f32 StickToPolylinePhysComponent::getRoofAngle() const
{
    return s_physRoofAngle;
}

void StickToPolylinePhysComponent::clampWithMaxSpeed( Vec2d& _speed ) const
{
    if ( !m_maxSpeedEnabled )
    {
        return;
    }

    if ( m_stickedEdge != U32_INVALID )
    {
        // Sticked mode, we use m_maxSpeedTotal
        if ( m_maxSpeedTotal )
        {
            f32 totalSpeedSq = _speed.sqrnorm();

            if ( totalSpeedSq > (m_maxSpeedTotal*m_maxSpeedTotal) )
            {
                f32 currentSpeed = f32_Sqrt(totalSpeedSq);
                f32 speedDif = currentSpeed - m_maxSpeedTotal;
                f32 newSpeed = m_maxSpeedTotal + ( speedDif * 0.5f );
                
                _speed.normalize();
                _speed *= newSpeed;
            }
        }
    }
    else
    {
        // non-sticked mode, we use m_maxSpeedX and m_maxSpeedY
        f32 xSpeed = _speed.dot(m_currentGravityDir.getPerpendicular());
        f32 ySpeed = _speed.dot(-m_currentGravityDir);

        if ( m_maxSpeedX )
        {
            if ( f32_Abs(xSpeed) > m_maxSpeedX )
            {
                if ( xSpeed > 0.f )
                {
                    f32 dif = xSpeed - m_maxSpeedX;

                    xSpeed = m_maxSpeedX + ( dif * 0.5f );
                }
                else
                {
                    f32 dif = xSpeed - (-m_maxSpeedX);

                    xSpeed = -m_maxSpeedX + ( dif * 0.5f );
                }
            }
        }

        if ( m_maxSpeedY )
        {
            if ( f32_Abs(ySpeed) > m_maxSpeedY )
            {
                if ( ySpeed > 0.f )
                {
                    f32 dif = ySpeed - m_maxSpeedY;

                    ySpeed = m_maxSpeedY + ( dif * 0.5f );
                }
                else
                {
                    f32 dif = ySpeed - (-m_maxSpeedY);

                    ySpeed = -m_maxSpeedY + ( dif * 0.5f );
                }
            }
        }

        _speed.m_x = xSpeed;
        _speed.m_y = ySpeed;
    }
}

void StickToPolylinePhysComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if(EventTeleport * teleportEvent = _event->DynamicCast<EventTeleport>(ITF_GET_STRINGID_CRC(EventTeleport,1075308886)))
    {
        if ( teleportEvent->getReset() )
        {
            setDisabled(bfalse, btrue, btrue);
            setSpeed(Vec2d::Zero);
            forceFindStickEdge();
        }
        else
        {
            if ( getStickedEdge() )
            {
                unstick();
                forceFindStickEdge();
            }

            m_prevPosition = m_actor->get2DPos();
            m_prevAngle = m_actor->getAngle();
            m_prevGroundAngle = calculateCurrentGroundAngle(m_prevAngle);
        }
    }
    else if (EventSetSpeed * onSetSpeed = _event->DynamicCast<EventSetSpeed>(ITF_GET_STRINGID_CRC(EventSetSpeed,2924911315)))
    {
        setSpeed(onSetSpeed->getSpeed());
    }
    else if (EventUnstick * onUnstick = _event->DynamicCast<EventUnstick>(ITF_GET_STRINGID_CRC(EventUnstick,54024986)))
    {
        unstick();
    }
    else if (EventSequenceActorActivate * sequenceActivate = _event->DynamicCast<EventSequenceActorActivate>(ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699)))
    {
        resetWorldSpeed();
    }
    else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventReset,2551104629)) )
    {
        setDisabled(bfalse, bfalse, btrue);
        setSpeed(Vec2d::Zero);
        unstick();
        setProcessContacts(btrue);
    }
    else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventFluidCollisionPolylineChanged,3871666563)) )
    {
        PolyLine *polyLine = getStickedPolyline();
        if (polyLine)
        {
            u32 edgeIndex = getStickedEdgeIndex();
            EventStickOnPolylineUpdate polyLineUpdate(polyLine->getRef(), edgeIndex, Vec2d::Up/*we approximate speed as Up vector for fluid change*/);
            polyLineUpdate.setSender(_event->getSender());
            m_actor->onEvent(&polyLineUpdate);
        }
    }
    else if (EventForceStickOnEdge * onForceStickEdge = _event->DynamicCast<EventForceStickOnEdge>(ITF_GET_STRINGID_CRC(EventForceStickOnEdge,3946845675)))
    {
        PolyLine* polyLine;
        const PolyLineEdge* currentEdge;

        AIUtils::getPolyLine(onForceStickEdge->getPolyline(),onForceStickEdge->getEdgeIndex(),polyLine,currentEdge);

        m_actor->set2DPos(onForceStickEdge->getPos());
        forceStickOnEdge(polyLine,currentEdge,onForceStickEdge->getEdgeIndex());
    }


}

void StickToPolylinePhysComponent::processQueryPhysics( class EventQueryPhysicsData* _query ) const
{
    Super::processQueryPhysics(_query);

    _query->setWorldSpeed(Vec2d(m_averageXSpeed,m_averageYSpeed));
    _query->setStickedPolyline(m_stickedLineRef);
    _query->setStickedEdgePolyline(m_stickedEdge);
    _query->setCurrentGroundDir(m_currentGroundDir);
}

void StickToPolylinePhysComponent::onUpdatePos()
{
    if ( !m_disabled && !m_disableSticking )
    {
	    forceFindStickEdge();
    }
}

void StickToPolylinePhysComponent::forceStickOnEdge( PolyLine* _polyLine,
                                                     const PolyLineEdge* _edge,
                                                     u32 _edgeIndex )
{
    Vec2d pos = m_actor->get2DPos();
    f32 angle = m_actor->getAngle();

    stick(_polyLine,_edgeIndex,pos,angle);

    applyPosAndAngle(pos,angle);
}

void StickToPolylinePhysComponent::forceFindStickEdge()
{
    Vec2d pos = m_actor->get2DPos();
    f32 angle = m_actor->getAngle();
    ObjectRef prevPoly = m_stickedLineRef;
    u32 prevEdge = m_stickedEdge;
    bbool blocked;

    m_traveledEdgesThisFrame.clear();
    processContacts(pos,pos,angle,0.1f,blocked,m_speed,LOGICDT);

    if ( prevPoly == m_stickedLineRef &&
         prevEdge == m_stickedEdge &&
         m_stickedEdge != U32_INVALID )
    {
        getRelDistFromPos(pos,1.f,m_relDist,m_relAngle);

        ITF_ASSERT(m_relDist==m_relDist);

        PolyLine* polyLine;
        const PolyLineEdge* currentEdge;

        AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,polyLine,currentEdge);

        if ( polyLine && currentEdge )
        {
            bbool checkBack = m_relDist < (currentEdge->m_length*0.5f);
            AIUtils::AdjacentEdgeData adjacentEdgeData;

            AIUtils::getAdjacentEdgeData(polyLine,*currentEdge,m_stickedEdge,m_currentGravityDir,checkBack,1.f,m_relDist,adjacentEdgeData);
            updateEdgeFactors(adjacentEdgeData,polyLine,currentEdge,m_stickedEdge,m_relDist,1.f,m_insideEdgeFactor,m_betweenEdgesFactor);
            ITF_ASSERT(m_insideEdgeFactor==m_insideEdgeFactor);
        }
    }

    applyPosAndAngle(pos,angle);
}

void StickToPolylinePhysComponent::onForceMove( )
{
    if (m_actor)
    {
        for ( u32 i = 0; i < s_speed_average_num_frames; i++ )
        {
            m_averageXSpeedMeasures[i] = 0.f;
            m_averageYSpeedMeasures[i] = 0.f;
        }

        m_averageXSpeed = 0.f;
        m_averageYSpeed = 0.f;
        m_prevPosition = m_actor->get2DPos();
        m_prevAngle = m_actor->getAngle();
        m_prevGroundAngle = m_actor->getAngle();
        m_speed = Vec2d::Zero;

        unstick();
    }
}

// This is used to find out what our relative distance is based on our current position
void StickToPolylinePhysComponent::getRelDistFromPos( const Vec2d& _pos, f32 _t, f32& _relDist, f32& _relAngle ) const
{
    PolyLine* polyLine = getStickedPolyline();
    const PolyLineEdge* edge = getStickedEdge();

    ITF_ASSERT(polyLine&&edge);

    if ( !polyLine || !edge )
    {
        return;
    }

    Vec2d edgeToPos = _pos - edge->getPos();
    f32 dist = edge->m_normalizedVector.dot(edgeToPos);

    if ( dist >= 0.f && dist <= edge->m_length )
    {
        // if it's within margins it's ok
        _relDist = dist;
        _relAngle = 0.f;
    }
    else
    {
        // If we are outside the bounds we need to calculate the relative distance depending on what
        // the angle is with the next edge
        AIUtils::AdjacentEdgeData adjacentEdgeData;

        AIUtils::getAdjacentEdgeData(polyLine,*edge,m_stickedEdge,m_currentGravityDir,dist < 0.f,_t,m_relDist,adjacentEdgeData);

        if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
        {
            f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

            if ( dotProd < 1.f )
            {
                f32 angleBetweenNormals = f32_ACos(dotProd);

                // Find out what's our angle between ourselves and the normal from the edge from the left
                if ( angleBetweenNormals )
                {
                    Vec2d myNorm = _pos - adjacentEdgeData.m_edgesCommonPoint;
                    myNorm.normalize();

                    f32 halfAngleBetweenNormals = angleBetweenNormals*0.5f;
                    f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                    // With the angle distance we can now set the proper relative distance
                    if ( dist < 0.f )
                    {
                        if ( angle < halfAngleBetweenNormals )
                        {
                            // We are on the other edge, clamp it
                            _relDist = -halfAngleBetweenNormals*getRadius();
                            _relAngle = halfAngleBetweenNormals;
                        }
                        else
                        {
                            _relDist = -(angleBetweenNormals-angle)*getRadius();
                            _relAngle = angleBetweenNormals-angle;
                        }
                    }
                    else
                    {
                        if ( angle > halfAngleBetweenNormals )
                        {
                            // We are on the other edge, clamp it
                            _relDist = edge->m_length + (halfAngleBetweenNormals*getRadius());
                            _relAngle = -halfAngleBetweenNormals;
                        }
                        else
                        {
                            _relDist = edge->m_length + (angle*getRadius());
                            _relAngle = -angle;
                        }
                    }
                }
                else
                {
                    if ( dist < 0.f )
                    {
                        _relDist = 0.f;
                    }
                    else
                    {
                        _relDist = edge->m_length;
                    }

                    _relAngle = 0.f;
                }
            }
            else
            {
                if ( dist < 0.f )
                {
                    _relDist = 0.f;
                }
                else
                {
                    _relDist = edge->m_length;
                }

                _relAngle = 0.f;
            }
        }
        else
        {
            // If the edge goes from down to up or parallel then we apply the regular rule
            _relDist = dist;
            _relAngle = 0.f;
        }
    }
}

void StickToPolylinePhysComponent::getPosFromRelDist( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, f32 _relDist, f32 _t, Vec2d& _pos ) const
{
    ITF_ASSERT(_poly&&_edge);

    if ( !_poly || !_edge )
    {
        return;
    }

    AIUtils::AdjacentEdgeData adjacentEdgeData;

    // We check the adjacent edge on the left or on the right
    bbool checkBack = _relDist < (_edge->m_length*0.5f);

    AIUtils::getAdjacentEdgeData(_poly,*_edge,_edgeIndex,m_currentGravityDir,checkBack,_t,m_relDist,adjacentEdgeData);

    if ( ( _relDist < 0.f || _relDist > _edge->m_length ) && adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
    {
        f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

        if ( dotProd < 1.f )
        {
            f32 angleBetweenNormals = f32_ACos(dotProd);
            f32 arcLengthBetweenNormals = angleBetweenNormals * getRadius();

            // First we calculate in which part of the arc are we going to be based on our m_relDist. What we do is translate our linear length out of the bounds of
            // the edge into an angular length, converting from meters to radians. (L = angle * radius)
            // We will rotate our actor from normal A (left edge) to normal B (right edge)
            if ( arcLengthBetweenNormals )
            {
                f32 posRotation;

                // If we are exiting the edge from the left our rotation will be [totalAngle - currentAngle]
                if ( _relDist < 0.f )
                {
                    posRotation = ( (arcLengthBetweenNormals - adjacentEdgeData.m_offset) * angleBetweenNormals ) / arcLengthBetweenNormals;
                }
                // If we are exiting the edge from the right our rotation will be [currentAngle]
                else
                {
                    posRotation = ( adjacentEdgeData.m_offset * angleBetweenNormals ) / arcLengthBetweenNormals;
                }

                // Our position will be tangent to the common point of the two edges
                // Finally we update our position and angle
                Vec2d ourNormal = adjacentEdgeData.m_normalA.Rotate(-posRotation);

                ourNormal.normalize();

                _pos = adjacentEdgeData.m_edgesCommonPoint + ( ourNormal * getRadius() );
            }
            else
            {
                _pos = adjacentEdgeData.m_edgesCommonPoint + ( _edge->m_normalizedVector.getPerpendicular() * getRadius() );
            }
        }
        else
        {
            _pos = adjacentEdgeData.m_edgesCommonPoint + ( _edge->m_normalizedVector.getPerpendicular() * getRadius() );
        }
    }
    else
    {
        // We are inside the edge margins
        SetEdgePosition(_poly,*_edge,_edgeIndex,m_relDist,_pos,_t);
    }
}

// updates the percentage of our position between two edges
void StickToPolylinePhysComponent::updateEdgeFactors( const AIUtils::AdjacentEdgeData& _adjacentEdgeData,
                                                      PolyLine* _currentPolyline,
                                                      const PolyLineEdge* _currentEdge,
                                                      u32 _currentEdgeIndex,
                                                      f32 _relDist, f32 _t,
                                                      f32& _insideFactor, f32& _betweenFactor ) const
{
    Vec2d edgePosT, edgeVecT, edgeDirT;
    f32 edgeLengthT;

    AIUtils::getEdgeDataInTime(_t,_currentPolyline,*_currentEdge,_currentEdgeIndex,edgePosT,edgeVecT,edgeDirT,edgeLengthT);

    if ( m_relDist < 0.f || m_relDist > edgeLengthT )
    {
        if ( _adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
        {
            // In this case we pivot around the common point of the two edges
            // The distance we can move will be the arc length between the normal of both edges
            f32 dotProd = _adjacentEdgeData.m_normalA.dot(_adjacentEdgeData.m_normalB);

            if ( dotProd < 1.f )
            {
                f32 angleBetweenNormals = f32_ACos(dotProd);
                f32 arcLengthBetweenNormals = angleBetweenNormals * getRadius();
                f32 halfArcLength = arcLengthBetweenNormals*0.5f;
                bbool checkBack = m_relDist < 0.f;
                f32 distLeft;

                if ( checkBack )
                {
                    distLeft = f32_Abs(m_relDist);
                }
                else
                {
                    distLeft = m_relDist - edgeLengthT;
                }

                // If the distance left is less than the size of the arc it means we stop somewhere in
                // the middle
                distLeft = Clamp(distLeft,0.f,halfArcLength);
                _betweenFactor = distLeft / halfArcLength;
                _insideFactor = 0.f;
            }
            else
            {
                _betweenFactor = 0.f;

                if ( m_relDist < 0.f )
                {
                    _insideFactor = 0.f;
                }
                else
                {
                    _insideFactor = 1.f;
                }
            }
        }
        else
        {
            _betweenFactor = 0.f;

            if ( m_relDist < 0.f )
            {
                _insideFactor = 0.f;
            }
            else
            {
                _insideFactor = 1.f;
            }
        }
    }
    else
    {
        _betweenFactor = 0.f;

        if ( edgeLengthT )
        {
            _insideFactor = m_relDist / edgeLengthT;
        }
        else
        {
            _insideFactor = 0.f;
        }
    }
}

// This takes our current percentage when we are between two edges and resets the reldist according to that
f32 StickToPolylinePhysComponent::getRelDistFromEdgesFactor() const
{
    PolyLine* polyLine = getStickedPolyline();
    const PolyLineEdge* currentEdge = getStickedEdge();

    if ( currentEdge && polyLine )
    {
        if ( m_betweenEdgesFactor != 0.f )
        {
            bbool checkBack = m_relDist < (currentEdge->m_length*0.5f);
            AIUtils::AdjacentEdgeData adjacentEdgeData;

            AIUtils::getAdjacentEdgeData(polyLine,*currentEdge,m_stickedEdge,m_currentGravityDir,checkBack,1.f,m_relDist,adjacentEdgeData);

            if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
            {
                f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

                if ( dotProd < 1.f )
                {
                    f32 angleBetweenNormals = f32_ACos(dotProd);
                    f32 arcLengthBetweenNormals = angleBetweenNormals * getRadius();
                    f32 halfArcLength = arcLengthBetweenNormals*0.5f;

                    if ( checkBack )
                    {
                        return (-halfArcLength)*m_betweenEdgesFactor;
                    }
                    else
                    {
                        return currentEdge->m_length + ( halfArcLength * m_betweenEdgesFactor );
                    }
                }
                else
                {
                    return currentEdge->m_length * Clamp(m_insideEdgeFactor,0.f,1.f);
                }
            }
        }
        else
        {
            return currentEdge->m_length * m_insideEdgeFactor;
        }
    }

    return m_relDist;
}

Vec2d StickToPolylinePhysComponent::calculateGroundDir() const
{
    PolyLine* polyLine = getStickedPolyline();

    ITF_ASSERT(polyLine);

    if ( !polyLine )
    {
        return Vec2d::Zero;
    }

    const PolyLineEdge* currentEdge = getStickedEdge();

    ITF_ASSERT(currentEdge);

    if ( !currentEdge )
    {
        return Vec2d::Zero;
    }

    f32 relDist = getRelDistFromEdgesFactor();
    bbool checkBack = relDist < (currentEdge->m_length*0.5f);
    PolyLine* adjPoly;
    u32 adjEdge;

    AIUtils::getAdjacentEdge(polyLine,m_stickedEdge,!checkBack,adjPoly,adjEdge);
    f32 crossProdEdges;
    Vec2d adjEdgeDir, adjEdgeNormal, normalA, normalB;
    f32 distOffset;

    if ( checkBack )
    {
        if ( adjEdge == U32_INVALID )
        {
            AIUtils::getImaginaryAdjacentEdge(btrue,m_currentGravityDir,adjEdgeDir,adjEdgeNormal);
        }
        else
        {
            const PolyLineEdge& newEdge = adjPoly->getEdgeAt(adjEdge);

            adjEdgeDir = newEdge.m_normalizedVector;
            adjEdgeNormal = adjEdgeDir.getPerpendicular();
        }
        
        crossProdEdges = (-adjEdgeDir).cross(currentEdge->m_normalizedVector);
        normalA = adjEdgeNormal;
        normalB = currentEdge->m_normalizedVector.getPerpendicular();
        distOffset = f32_Abs(relDist);
    }
    else
    {
        if ( adjEdge == U32_INVALID )
        {
            AIUtils::getImaginaryAdjacentEdge(bfalse,m_currentGravityDir,adjEdgeDir,adjEdgeNormal);
        }
        else
        {
            const PolyLineEdge& newEdge = adjPoly->getEdgeAt(adjEdge);

            adjEdgeDir = newEdge.m_normalizedVector;
            adjEdgeNormal = adjEdgeDir.getPerpendicular();
        }

        crossProdEdges = (-currentEdge->m_normalizedVector).cross(adjEdgeDir);
        normalA = currentEdge->m_normalizedVector.getPerpendicular();
        normalB = adjEdgeNormal;
        distOffset = relDist - currentEdge->m_length;
    }

    if ( ( relDist < 0.f || relDist > currentEdge->m_length ) && crossProdEdges > MTH_EPSILON )
    {
        f32 dotProd = normalA.dot(normalB);

        if ( dotProd < 1.f )
        {
            f32 angleBetweenNormals = f32_ACos(dotProd);
            f32 arcLengthBetweenNormals = angleBetweenNormals * getRadius();

            if ( (distOffset-MTH_EPSILON) > (arcLengthBetweenNormals*0.5f) )
            {
                // This should not happen, it means we are out of the edge
                return currentEdge->m_normalizedVector;
            }
            else
            {
                f32 posRotation;

                // First we calculate in which part of the arc are we going to be based on our m_relDist.
                // What we do is translate our linear length out of the bounds of the edge into an angular
                // length, converting from meters to radians. (L = angle * radius)
                // We will rotate our actor from normal A (left edge) to normal B (right edge)
                if ( arcLengthBetweenNormals )
                {
                    if ( relDist < 0.f )
                    {
                        posRotation = ( (arcLengthBetweenNormals - distOffset) / arcLengthBetweenNormals) * angleBetweenNormals;
                    }
                    // If we are exiting the edge from the right our rotation will be [currentAngle]
                    else
                    {
                        posRotation = ( distOffset / arcLengthBetweenNormals ) * angleBetweenNormals;
                    }
                }
                else
                {
                    posRotation = 0.f;
                }

                // Our ground will be tangent to the common point of the two edges and our circle
                Vec2d ourNormal = normalA.Rotate(-posRotation);

                ourNormal.normalize();

                return -ourNormal.getPerpendicular();
            }
        }
        else
        {
            return currentEdge->m_normalizedVector;
        }
    }
    else
    {
        return currentEdge->m_normalizedVector;
    }
}

// This adjust our relative distance and angle in case we went out of bounds from the current edge
// If _newEdge is true it means we changed edge in this frame, so the applied force may not be valid any more
bbool StickToPolylinePhysComponent::adjustCurrentRelDist( f32 _t, Vec2d& _speed )
{
    PolyLine* polyLine = getStickedPolyline();

    ITF_ASSERT(polyLine);

    if ( !polyLine )
    {
        return btrue;
    }

    const PolyLineEdge* currentEdge = getStickedEdge();

    ITF_ASSERT(currentEdge);

    if ( !currentEdge )
    {
        return btrue;
    }

    AIUtils::AdjacentEdgeData adjacentEdgeData;
    u32 startEdge = m_stickedEdge;
    Vec2d prevEdgeDir = m_currentGroundDir;
    bbool edgeChanged = bfalse;
    bbool reuseAdjData = btrue;
    bbool shouldUnstick = bfalse;

    // We check the adjacent edge on the left or on the right
    bbool checkBack = m_relDist < (currentEdge->m_length*0.5f);
    AIUtils::getAdjacentEdgeData(polyLine,*currentEdge,m_stickedEdge,m_currentGravityDir,checkBack,_t,m_relDist,adjacentEdgeData);

    // First do a test force concave edges to see if we pass on to the next edge
    if ( adjacentEdgeData.m_crossProdEdges < -MTH_EPSILON && adjacentEdgeData.m_adjacentEdgeIndex != U32_INVALID )
    {
        Vec2d intersection;

        if ( findIntersectionBetweenEdges(currentEdge,adjacentEdgeData,checkBack,intersection) )
        {
            Vec2d vecToPos = intersection - currentEdge->getPos();
            f32 distOnEdge = currentEdge->m_normalizedVector.dot(vecToPos);

            if ( ( checkBack && m_relDist < distOnEdge ) || ( !checkBack && m_relDist > distOnEdge ) )
            {
                if ( adjacentEdgeData.m_polyline != NULL )
                {
                    EdgeOrientationType orientation = getEdgeOrientationType(adjacentEdgeData.m_adjacentEdgeDir);

                    const PolyLineEdge* newEdge = &adjacentEdgeData.m_polyline->getEdgeAt(adjacentEdgeData.m_adjacentEdgeIndex);
                    Vec2d vecToPos = intersection - newEdge->getPos();
                    f32 destRelDist = newEdge->m_normalizedVector.dot(vecToPos);

                    if ( orientation == EDGEORIENTATIONTYPE_GROUND || ( m_stickOnWalls && AIUtils::isWallRunEnabledOnPolyline( currentEdge->m_normalizedVector,adjacentEdgeData.m_adjacentEdgeDir) ) )
                    {
                        m_relAngle = ( m_relAngle + getVec2Angle(currentEdge->m_normalizedVector) ) - getVec2Angle(newEdge->m_normalizedVector);
                        m_relDist = destRelDist;

                        ITF_ASSERT(m_relDist==m_relDist);

                        if ( polyLine != adjacentEdgeData.m_polyline || currentEdge != newEdge )
                        {
                            edgeChanged = btrue;
                            setStickedEdge(adjacentEdgeData.m_polyline,adjacentEdgeData.m_adjacentEdgeIndex);
                            currentEdge = newEdge;
                            polyLine = adjacentEdgeData.m_polyline;
                            reuseAdjData = bfalse;
                        }

                        NormalizeAnglePI(m_relAngle);
                    }
                    else
                    {
                        // Can't pass to this wall, stay on the current edge limit
                        m_relDist = distOnEdge;
                        ITF_ASSERT(m_relDist==m_relDist);

                        Vec2d contactP = newEdge->getPos() + ( newEdge->m_normalizedVector * destRelDist );

                        notifyBlocked(adjacentEdgeData.m_polyline->getRef(),adjacentEdgeData.m_polyline,adjacentEdgeData.m_adjacentEdgeIndex,_speed,contactP);

                        if ( getEdgeOrientationType(currentEdge->m_normalizedVector) != EDGEORIENTATIONTYPE_GROUND &&
                             orientation != EDGEORIENTATIONTYPE_GROUND )
                        {
                            shouldUnstick = btrue;
                        }
                    }
                }
                else
                {
                    shouldUnstick = btrue;
                }
            }
        }
    }
    // If not concave then check for exiting the edge on concave edges
    else if ( m_relDist < 0.f || m_relDist > currentEdge->m_length )
    {
        f32 distanceToTravel;

        if ( m_relDist < 0.f )
        {
            distanceToTravel = m_relDist;
        }
        else
        {
            distanceToTravel = m_relDist - currentEdge->m_length;
        }

        u32 destEdge;
        f32 destRelDist;
        PolyLine* destPoly;

        travelToEdge(polyLine,m_stickedEdge,distanceToTravel,_t,destPoly,destEdge,destRelDist);

        // Update our data in case we moved into another edge
        const PolyLineEdge* newEdge = &destPoly->getEdgeAt(destEdge);

        m_relAngle = ( m_relAngle + getVec2Angle(currentEdge->m_normalizedVector) ) - getVec2Angle(newEdge->m_normalizedVector);
        m_relDist = destRelDist;

        ITF_ASSERT(m_relDist==m_relDist);

        if ( polyLine != destPoly || currentEdge != newEdge )
        {
            edgeChanged = btrue;
            setStickedEdge(destPoly,destEdge);
            currentEdge = newEdge;
            polyLine = destPoly;
            reuseAdjData = bfalse;
        }
        
        NormalizeAnglePI(m_relAngle);
    }

    if ( !reuseAdjData )
    {
        checkBack = m_relDist < (currentEdge->m_length*0.5f);
        AIUtils::getAdjacentEdgeData(polyLine,*currentEdge,m_stickedEdge,m_currentGravityDir,checkBack,_t,m_relDist,adjacentEdgeData);
    }

    updateEdgeFactors(adjacentEdgeData,polyLine,currentEdge,m_stickedEdge,m_relDist,_t,m_insideEdgeFactor,m_betweenEdgesFactor);
    ITF_ASSERT(m_insideEdgeFactor==m_insideEdgeFactor);

    // Now we are sure that our data is contained within our edge.
    // Set our target angle and our ground direction
    if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON && ( m_relDist < 0.f || m_relDist > currentEdge->m_length ) )
    {
        f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);
        f32 posRotation;

        if ( dotProd < 1.f )
        {
            f32 angleBetweenNormals = f32_ACos(dotProd);
            f32 arcLengthBetweenNormals = angleBetweenNormals * getRadius();

            // First we calculate in which part of the arc are we going to be based on our m_relDist.
            // What we do is translate our linear length out of the bounds of the edge into an angular
            // length, converting from meters to radians. (L = angle * radius)
            // We will rotate our actor from normal A (left edge) to normal B (right edge)
            if ( arcLengthBetweenNormals )
            {
                // If we are exiting the edge from the left our rotation will be [totalAngle - currentAngle]
                // NOTE: now we always fall in the middle of the angle, so we force it to be accurate and
                // avoid imprecission problems.
                if ( edgeChanged )
                {
                    posRotation = angleBetweenNormals*0.5f;
                }
                else
                {
                    if ( m_relDist < 0.f )
                    {
                        posRotation = ( (arcLengthBetweenNormals - adjacentEdgeData.m_offset) / arcLengthBetweenNormals) * angleBetweenNormals;
                    }
                    // If we are exiting the edge from the right our rotation will be [currentAngle]
                    else
                    {
                        posRotation = ( adjacentEdgeData.m_offset / arcLengthBetweenNormals ) * angleBetweenNormals;
                    }
                }
            }
            else
            {
                posRotation = 0.f;
            }

            // We are between normal A and normal B. We pivot around the common point, and our normal is
            // calculated from the relative distance
            // We update our angle so that we rotate our character as he passes from one edge to the other

            if ( m_relDist < 0.f )
            {
                m_targetAngle = angleBetweenNormals - posRotation;
            }
            else
            {
                m_targetAngle = -posRotation;
            }

            ITF_ASSERT(m_targetAngle==m_targetAngle);

            NormalizeAnglePI(m_targetAngle);
        }
        else
        {
            m_targetAngle = 0.f;
            posRotation = 0.f;
        }

        m_relAngle = m_targetAngle;

        // Our ground will be tangent to the common point of the two edges and our circle
        Vec2d ourNormal;
        
        if ( posRotation )
        {
            ourNormal = adjacentEdgeData.m_normalA.Rotate(-posRotation);
        }
        else
        {
            ourNormal = adjacentEdgeData.m_normalA;
        }

        ourNormal.normalize();

        m_currentGroundDir = -ourNormal.getPerpendicular();
    }
    else
    {
        m_targetAngle = 0.f;
        m_currentGroundDir = currentEdge->m_normalizedVector;
    }

    if ( m_stickedEdge != startEdge && getStickedEdge() )
    {
        Vec2d edgeNorm = prevEdgeDir.getPerpendicular();
        Vec2d speedDir = _speed;

        speedDir.normalize();

        f32 crossProd = speedDir.cross(edgeNorm);

        if ( f32_Abs(crossProd) < MTH_EPSILON )
        {
            _speed = Vec2d::Zero;
        }
        else
        {
            bbool back = crossProd > 0.f ? bfalse : btrue;

            transferSpeedToNewEdge(prevEdgeDir,m_currentGroundDir,back,_speed);
        }

        if ( adjacentEdgeData.m_crossProdEdges < 0.f )
        {
            setCurrentGroundAngularSpeedFromRotation(m_relAngle,m_targetAngle);
        }
    }
    
    calculateFrictionCoefficient();

    return shouldUnstick;
}

// Travel along the polyline and return the destination data
void StickToPolylinePhysComponent::travelToEdge( PolyLine* _polyLine, u32 _edgeIndex, f32 _dist, f32 _t,
                                                 PolyLine*& _resPoly, u32& _resEdge, f32& _resRelDist ) const
{
    bbool checkBack = _dist < 0.f;
    f32 distLeft = f32_Abs(_dist);
    u32 currentEdgeIndex = _edgeIndex;
    PolyLine* currentPolyline = _polyLine;
    const PolyLineEdge* currentEdge = &currentPolyline->getEdgeAt(_edgeIndex);

    _resPoly = _polyLine;

    AIUtils::AdjacentEdgeData adjacentEdgeData;

    AIUtils::getAdjacentEdgeData(currentPolyline,*currentEdge,currentEdgeIndex,m_currentGravityDir,checkBack,_t,m_relDist,adjacentEdgeData);

    f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

    if ( dotProd < 1.f && ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON || adjacentEdgeData.m_crossProdEdges < -MTH_EPSILON ) )
    {
        // Check the junction between the two edges
        // If it's an edge going up and then one going down
        if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
        {
            // In this case we pivot around the common point of the two edges
            // The distance we can move will be the arc length between the normal of both edges
            f32 angleBetweenNormals = f32_ACos(dotProd);
            f32 arcLengthBetweenNormals = angleBetweenNormals * getRadius();
            f32 halfArcLength = arcLengthBetweenNormals*0.5f;
            static const f32 s_extraOffset = 0.005f;

            // If the distance left is less than the size of the arc it means we stop somewhere in
            // the middle
            if ( distLeft <= arcLengthBetweenNormals )
            {
                // If the distance left is less or equal than half the arc it means we stay on the same segment
                if ( distLeft <= halfArcLength)
                {
                    if ( checkBack )
                    {
                        _resRelDist = -distLeft;
                    }
                    else
                    {
                        _resRelDist = currentEdge->m_length + distLeft;
                    }

                    _resEdge = currentEdgeIndex;
                }
                // If the distance left is bigger than half the arc it means we pass onto the next segment
                else
                {
                    if ( adjacentEdgeData.m_adjacentEdgeIndex == U32_INVALID )
                    {
                        if ( checkBack )
                        {
                            _resRelDist = -halfArcLength;
                        }
                        else
                        {
                            _resRelDist = currentEdge->m_length + halfArcLength;
                        }

                        _resEdge = currentEdgeIndex;
                    }
                    else
                    {
                        if ( checkBack )
                        {
                            _resRelDist = adjacentEdgeData.m_adjacentEdgeLength + halfArcLength - s_extraOffset;
                        }
                        else
                        {
                            _resRelDist = -halfArcLength + s_extraOffset;
                        }

                        _resPoly = adjacentEdgeData.m_polyline;
                        _resEdge = adjacentEdgeData.m_adjacentEdgeIndex;
                    }
                }
            }
            // If it's bigger it means we pass the arc completely and we con continue on the next edge
            else
            {
                if ( adjacentEdgeData.m_adjacentEdgeIndex == U32_INVALID )
                {
                    if ( checkBack )
                    {
                        _resRelDist = -halfArcLength;
                    }
                    else
                    {
                        _resRelDist = currentEdge->m_length + halfArcLength;
                    }

                    _resEdge = currentEdgeIndex;
                }
                else
                {
                    if ( checkBack )
                    {
                        _resRelDist = adjacentEdgeData.m_adjacentEdgeLength + halfArcLength- s_extraOffset;
                    }
                    else
                    {
                        _resRelDist = -halfArcLength + s_extraOffset;
                    }

                    _resEdge = adjacentEdgeData.m_adjacentEdgeIndex;
                    _resPoly = adjacentEdgeData.m_polyline;
                }
            }
        }
        // If it's an edge going down and then going up
        else if ( adjacentEdgeData.m_crossProdEdges < -MTH_EPSILON )
        {
            // If the edge is not ground, and we cannot walk on walls then we don't want to continue...
            EdgeOrientationType orientation = getEdgeOrientationType(adjacentEdgeData.m_adjacentEdgeDir);

            if ( orientation != EDGEORIENTATIONTYPE_GROUND && !m_stickOnWalls )
            {
                if ( checkBack  )
                {
                    _resRelDist = -distLeft;
                }
                // If we are on the last segment and searching right we can't search anymore
                else
                {
                    _resRelDist = currentEdge->m_length + distLeft;
                }

                _resEdge = currentEdgeIndex;
            }
            else
            {
                if ( adjacentEdgeData.m_adjacentEdgeIndex == U32_INVALID )
                {
                    if ( checkBack )
                    {
                        _resRelDist = 0.f;
                    }
                    else
                    {
                        _resRelDist = currentEdge->m_length;
                    }

                    _resEdge = currentEdgeIndex;
                }
                else
                {
                    // We need to find the point where our circle would touch both edges
                    // The center of our circle will be at a distance from the both edges equal to the radius
                    // To find the center we just intersect the lines A and B (A parallel to edgeA, B parallel to
                    // edge B, at a distance of the radius of our circle)
                    // Once we find the center of our circle we can compute where it touches edgeA and edgeB
                    // by using the normals of the edges
                    Vec2d intersection;

                    findIntersectionBetweenEdges(currentEdge,adjacentEdgeData,checkBack,intersection);

                    Vec2d limit = intersection - ( adjacentEdgeData.m_adjacentEdgeNormal * getRadius() );
                    f32 length = ( limit - adjacentEdgeData.m_edgesCommonPoint ).norm();

                    if ( checkBack )
                    {
                        _resRelDist = adjacentEdgeData.m_adjacentEdgeLength - length - 0.001f;
                    }
                    else
                    {
                        _resRelDist = length + 0.001f;
                    }

                    _resEdge = adjacentEdgeData.m_adjacentEdgeIndex;
                    _resPoly = adjacentEdgeData.m_polyline;
                }
            }
        }
    }
    // The two edges are parallel we pass to the next one without doing anything
    else
    {
        if ( adjacentEdgeData.m_adjacentEdgeIndex == U32_INVALID )
        {
            if ( checkBack )
            {
                _resRelDist = 0.f;
            }
            else
            {
                _resRelDist = currentEdge->m_length;
            }

            _resEdge = currentEdgeIndex;
        }
        else
        {
            if ( checkBack )
            {
                _resRelDist = adjacentEdgeData.m_adjacentEdgeLength;
            }
            else
            {
                _resRelDist = 0.f;
            }

            _resEdge = adjacentEdgeData.m_adjacentEdgeIndex;
            _resPoly = adjacentEdgeData.m_polyline;
        }
    }
}

bbool StickToPolylinePhysComponent::findIntersectionBetweenEdges( const PolyLineEdge* _currentEdge, const AIUtils::AdjacentEdgeData& _adjEdgeData, bbool _checkBack, Vec2d& _intersection ) const
{
    Vec2d normalA = _currentEdge->m_normalizedVector.getPerpendicular();
    Vec2d normalB = _adjEdgeData.m_adjacentEdgeNormal;
    Vec2d pA0 = _currentEdge->getPos() + (normalA*getRadius());
    Vec2d pB0 = _adjEdgeData.m_adjacentEdgePos + (normalB*getRadius());
    Vec2d pA1 = pA0+_currentEdge->m_vector;
    Vec2d pB1 = pB0+(_adjEdgeData.m_adjacentEdgeDir*_adjEdgeData.m_adjacentEdgeLength);

    return intersectionLineLine(pA0,pA1,pB0,pB1,_intersection) == 1;
}

void StickToPolylinePhysComponent::prepareTravelledEdgesThisFrame()
{
    m_traveledEdgesThisFrame.clear();

    const PolyLineEdge* currentEdge = getStickedEdge();

    if ( currentEdge )
    {
        m_traveledEdgesThisFrame.push_back(TravelEdgeData(m_stickedLineRef,m_stickedEdge));
    }
}

void StickToPolylinePhysComponent::updateEdgeMemory( f32 _dt )
{
    u32 numMemory = m_memoryStickedEdges.size();
    u32 memIter = 0;

    while ( memIter < numMemory )
    {
        EdgeMemoryData& memData = m_memoryStickedEdges[memIter];

        memData.m_time -= _dt;

        if ( memData.m_time <= 0.f )
        {
            m_memoryStickedEdges.eraseNoOrder(memIter);
            numMemory--;
        }
        else
        {
            memIter++;
        }
    }
}

void StickToPolylinePhysComponent::applyForces( f32 _dt )
{
    // Calculate the wind forces
    setWindForces();

    // Check if we need to unstick from the current forces (in case we jumped, etc)
    checkUnstickFromForces();

    // Check material properties to modify our current force
    applyMaterialPreMove();

    // Calculate the final speed from all the forces
    calculateSpeedFromForces(_dt);
}

void StickToPolylinePhysComponent::setWindForces()
{
    m_hasExternalForces = bfalse;
    m_windForcesThisFrame = Vec2d::Zero;
    m_windSpeedMultiplier = Vec2d::One;

    if ( !m_disableWindForces )
    {
        Vec2d totalWindForce = Vec2d::Zero;
        PhysActorForceArray windForces;

        PHYSWORLD->getWindForces(m_actor->get2DPos(), m_speed, m_actor->getDepth(), windForces);

        for ( u32 i = 0; i < windForces.size(); i++ )
        {
            PhysActorForce& wind = windForces[i];

            if ( m_actor->getBinding()->isDescendant(wind.m_actor) )
            {
                continue;
            }

            totalWindForce += windForces[i].m_force;
            m_windSpeedMultiplier *= windForces[i].m_speedMultiplier;

            m_hasExternalForces = btrue;
        }

        m_windForcesThisFrame = totalWindForce * m_windMultiplier;

        if ( m_windForcesThisFrame != Vec2d::Zero && m_applyWindSpeedLimiter )
        {
            PhysForceModifier::applySpeedLimitation(getTemplate()->getForce2Speed(),getTemplate()->getWindSpeedLimiter(),getTemplate()->getWindMaxScaleOpposite(),m_speed,m_windForcesThisFrame);
        }
    }
}

void StickToPolylinePhysComponent::advanceDistance( f32 _dist )
{
    if ( m_stickedEdge == U32_INVALID )
    {
        return;
    }

    const PolyLineEdge* edge = getStickedEdge();

    if ( !edge )
    {
        return;
    }

    Vec2d speed = edge->m_normalizedVector*_dist;
    Vec2d pos = m_actor->get2DPos();
    f32 angle = m_actor->getAngle();

    moveSticked(1.f,speed,pos);
    rotate(1.f,pos,angle);
    applyPosAndAngle(pos,angle);
}

void StickToPolylinePhysComponent::Update( f32 _deltaTime )
{
    if ( m_pause || !_deltaTime )
    {
        return;
    }

    updateEdgeMemory(_deltaTime);

    if ( m_debugLog )
    {
        LOG("StickToPolylinePhysComponent: Start frame, Rel Dist %f",m_relDist);
    }

    Vec2d pos = m_actor->get2DPos();
    f32 angle = m_actor->getAngle();

    PolyLine* poly = getStickedPolyline();

    if ( poly )
    {
        if ( m_stickedEdge >= poly->getVectorsCount() || poly->m_isCollisionActive == bfalse )
        {
            unstick();
        }
        else
        {
            m_relDist = getRelDistFromEdgesFactor();

            ITF_ASSERT(m_relDist==m_relDist);

            if ( m_forceEdgeAngleNext )
            {
                m_relAngle = m_targetAngle;
            }

            if ( poly->getOwnerActor() )
            {
                // If it's a procedural polyline we recalculate the current relative distance because the edges
                // can move, so we need to reupdate our ground direction
                adjustCurrentRelDist(1.f,m_speed);
            }

            const PolyLineEdge* currentEdge;

            AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,poly,currentEdge);

            if ( poly && currentEdge )
            {
                getPosFromRelDist(poly,currentEdge,m_stickedEdge,m_relDist,1.f,pos);
            }
        }
    }
    else if ( m_stickedLineRef != ITF_INVALID_OBJREF )
    {
        unstick();
    }

    m_forceEdgeAngleNext = bfalse;

    updateCurrentGravity(pos);

    // If the component is disabled do some basic stuff and return
    if ( m_disabled )
    {
        applyPosAndAngle(pos,angle);
        measureSpeedAverage(_deltaTime);
        measureTorqueAverage(_deltaTime);

        m_prevPosition = pos;
        m_prevAngle = angle;
        m_prevGroundAngle = calculateCurrentGroundAngle(m_prevAngle);
        return;
    }
    
    // actor-bound + physics = unsafe
    ITF_WARNING(
        m_actor,
        !m_actor->getParentBind() || m_actor->getParentBind()->isRuntimeDisabled(),
        "This actor shouldn't be bound because its StickToPolylinePhysComponent "
        "updates its position in world coordinates.");

    // Count the current edges for this frame
    prepareTravelledEdgesThisFrame();

    applyForces(_deltaTime);

    // Update our position
    ITF_ASSERT(pos==pos);
    move(_deltaTime,pos,angle);
    // Update our orientation
    ITF_ASSERT(pos==pos);
    rotate(_deltaTime,pos,angle);

    // Apply the final data to the actor and physic body
    ITF_ASSERT(pos==pos);
    applyPosAndAngle(pos,angle);

    // Reset the forces
    m_force = Vec2d::Zero;
    m_windForcesThisFrame = Vec2d::Zero;
    m_windSpeedMultiplier = Vec2d::One;

    measureSpeedAverage(_deltaTime);
    measureTorqueAverage(_deltaTime);

    m_prevPosition = pos;
    m_prevAngle = angle;
    m_prevGroundAngle = calculateCurrentGroundAngle(angle);

    if ( m_debugLog )
    {
        LOG("StickToPolylinePhysComponent: End frame");
    }

    if ( getTemplate()->getTreatMovingPolylines() )
    {
        EventPhysicsUpdated done;
        m_actor->onEvent(&done);
    }

    if ( m_cornerCaseDisableDelay )
    {
        m_cornerCaseDisableDelay = Max(m_cornerCaseDisableDelay-_deltaTime,0.f);
    }
}

// Move the character in the air or on a polyline
void StickToPolylinePhysComponent::move( f32 _dt, Vec2d& _pos, f32 _angle )
{
    bbool blocked = bfalse;
    bbool sticked = m_stickedEdge != U32_INVALID;
    f32 distanceAdvanced = 0.f;
    Vec2d prevPos = _pos;

    if ( sticked )
    { 
        // If there are no forces and our speed is very very small we zero the speed
        // so that we don't have shaky issues on no friction floors
        f32 currentSpeed = m_speed.norm();

        if ( currentSpeed < 0.04f && (m_force+m_windForcesThisFrame) == Vec2d::Zero )
        {
            m_speed = Vec2d::Zero;
        }

        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,poly,edge);

        getPosFromRelDist(poly,edge,m_stickedEdge,m_relDist,1.f,_pos);
        ITF_ASSERT(_pos==_pos);
    }

    if ( m_speed.IsEqual(Vec2d::Zero,MTH_EPSILON) )
    {
        processContacts(prevPos,_pos,_angle,0.f,blocked,m_speed,_dt);

        sticked = getStickedEdge() != NULL;

        if ( !blocked && sticked )
        {
            // so that our current ground dir is updated because we have to take into account moving
            // platforms
            adjustCurrentRelDist(1.f,m_speed);

            // In case we unsticked from the previous call
            if ( m_stickedEdge != U32_INVALID )
            {
                PolyLine* poly;
                const PolyLineEdge* edge;

                AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,poly,edge);

                getPosFromRelDist(poly,edge,m_stickedEdge,m_relDist,1.f,_pos);
            }
        }
    }
    else
    {
        moveLoop(_dt,m_speed,blocked,prevPos,_pos,_angle,distanceAdvanced);
    }

    applyMaterialPostMove(_dt,blocked,_pos,_angle,distanceAdvanced);

    u32 numLoops = 0;

    while ( blocked && numLoops < m_solverMaxLoops )
    {
        processContacts(_pos,_pos,_angle,0.f,blocked,m_speed,_dt);
        numLoops++;
    }

    // If we didn't move we reset the speed
    if ( distanceAdvanced <= MTH_EPSILON )
    {
        m_speed = Vec2d::Zero;
    }
}

void StickToPolylinePhysComponent::applyMaterialPreMove()
{
    const PolyLineEdge* stickedEdge = getStickedEdge();

    if ( stickedEdge )
    {
        // If we are still sticking to a polyline, apply material physics
        const GameMaterial_Template* mat = World::getGameMaterial(stickedEdge->getGameMaterial());

        if ( mat )
        {
            f32 moveForce = mat->getMoveForce();

            if ( moveForce )
            {
                m_force += stickedEdge->m_normalizedVector*moveForce;
            }
        }
    }
}

void StickToPolylinePhysComponent::applyMaterialPostMove( f32 _dt, bbool& _blocked, Vec2d& _pos, f32 _angle, f32& _distancedAdvanced )
{
    const PolyLineEdge* stickedEdge = getStickedEdge();

    if ( stickedEdge )
    {
        // If we are still sticking to a polyline, apply material physics
        const GameMaterial_Template* mat = World::getGameMaterial(stickedEdge->getGameMaterial());

        if ( mat )
        {
            f32 moveSpeed = mat->getMoveSpeed();

            if ( moveSpeed )
            {
                Vec2d speed = stickedEdge->m_normalizedVector*moveSpeed;
                moveLoop(_dt,speed,_blocked,_pos,_pos,_angle,_distancedAdvanced);
            }
        }
    }
}

void StickToPolylinePhysComponent::moveLoop( f32 _dt, Vec2d& _speed, bbool& _blocked, const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32& _totalDistance )
{
    u32 numLoops = 0;
    Vec2d startPos = _prevPos;
    bbool extraCheck = bfalse;

    while ( !_speed.IsEqual(Vec2d::Zero,MTH_EPSILON) && _dt > MTH_EPSILON )
    {
        extraCheck = bfalse;

        f32 startSpeed = _speed.norm() * _dt;
        bbool isSticked = m_stickedEdge != U32_INVALID;

        if ( isSticked )
        {
            moveSticked(_dt,_speed,_pos);
        }
        else
        {
            moveFree(_dt,_speed,_pos);
        }

        processContacts(startPos,_pos,_angle,0.f,_blocked,_speed,_dt);

        f32 distAdvanced = (_pos - startPos).norm();
        f32 timeUsed = (distAdvanced/startSpeed)*_dt;

        _totalDistance += distAdvanced;
        _dt = Max(_dt - timeUsed,MTH_EPSILON);

        if ( !isSticked && m_stickedEdge != U32_INVALID )
        {
            extraCheck = btrue;
        }

        if ( distAdvanced < MTH_EPSILON && numLoops > 0 )
        {
            break;
        }

        numLoops++;
        startPos = _pos;
    }

    if ( extraCheck )
    {
        processContacts(startPos,_pos,_angle,0.f,_blocked,_speed,_dt);
    }
}

// Move in the air
void StickToPolylinePhysComponent::moveFree( f32 _dt, Vec2d& _speed, Vec2d& _pos )
{
    _pos += (_speed * _dt);
    ITF_ASSERT(_pos==_pos);
}

// Move on the polyline
void StickToPolylinePhysComponent::moveSticked( f32 _dt, Vec2d& _speed, Vec2d& _pos )
{
    f32 moveSide = _speed.cross(m_currentGroundDir.getPerpendicular()) > 0.f ? 1.f : -1.f;

    m_relDist += _speed.norm()*_dt*moveSide;

    ITF_ASSERT(m_relDist==m_relDist);

    PolyLine* prevPolyline;
    const PolyLineEdge* prevEdge;

    AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,prevPolyline,prevEdge);

    // Adjust the reldist in case we go out of the edge
    bbool shouldUnstick = adjustCurrentRelDist(1.f,_speed);

    if ( shouldUnstick )
    {
        unstick(bfalse);
    }
    else
    {
        PolyLine* currentPolyline;
        const PolyLineEdge* currentEdge;

        AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,currentPolyline,currentEdge);

        if ( currentPolyline && currentEdge )
        {
            if ( prevEdge && currentEdge && prevEdge != currentEdge && m_stickOnWalls )
            {
                EdgeOrientationType edgeOrientation = getEdgeOrientationType(currentEdge->m_normalizedVector);

                if ( edgeOrientation != EDGEORIENTATIONTYPE_GROUND )
                {
                    if ( !AIUtils::isWallRunEnabledOnPolyline( currentEdge, prevEdge ) )
                    {
                        unstick(bfalse);
                        currentPolyline = NULL;
                        currentEdge = NULL;
                    }
                }
            }
        }

        if ( currentPolyline && currentEdge )
        {
            getPosFromRelDist(currentPolyline,currentEdge,m_stickedEdge,m_relDist,1.f,_pos);

            // Check if we detach from the edge because we go too fast or there is no friction
            if ( checkIfDetachFromCurrentEdgeWithSpeed(moveSide,1.f) )
            {
                unstick(btrue);
            }
        }
    }

    if ( m_debugLog )
    {
        LOG("StickToPolylinePhysComponent: Move Sticked, RelDist %f",m_relDist);
    }
}

// This updates our actor orientation based on the target angle and our angular speed
void StickToPolylinePhysComponent::rotate( f32 _deltaTime, const Vec2d& _pos, f32& _angle )
{
    if ( !m_updateRotation )
    {
        return;
    }

    const PolyLineEdge* edge = getStickedEdge();

    if ( edge )
    {
        // If we are trying to reach a target angle, do so by using our current speed value
        if ( m_targetAngle != m_relAngle )
        {
            f32 angleDif = getShortestAngleDelta(m_relAngle,m_targetAngle);

            if ( angleDif )
            {
                f32 absDif = f32_Abs(angleDif);
                f32 angleDelta = m_groundCurrentAngularSpeed*_deltaTime;

                if ( absDif < angleDelta )
                {
                    angleDelta = absDif;
                }

                if ( angleDif > 0.f )
                {
                    m_relAngle += angleDelta;
                }
                else
                {
                    m_relAngle -= angleDelta;
                }
            }
            else
            {
                m_relAngle = m_targetAngle;
            }

            ITF_ASSERT(m_relAngle==m_relAngle);

            if ( m_relAngle != m_relAngle )
            {
                m_relAngle = 0.f;
            }
            else
            {
                NormalizeAnglePI(m_relAngle);
            }
        }

        _angle = m_relAngle + getVec2Angle(edge->m_normalizedVector);

        ITF_ASSERT(_angle==_angle);

        if ( _angle != _angle )
        {
            _angle = 0.f;
        }
        else
        {
            NormalizeAnglePI(_angle);
        }
    }
    else
    {
        // If we are not sticked to a polyline we set our orientation back to zero over time with
        // our torque
        f32 zeroAngle = getVec2Angle(m_currentGravityDir) + MTH_PIBY2;

        if ( _angle != zeroAngle )
        {
            f32 angularSpeed = calculateAngularSpeedFromLinearSpeed(m_rotateWithTorque);

            angularSpeed *= m_angularSpeedMultiplier;

            f32 dif = getShortestAngleDelta(_angle,zeroAngle);
            
            if ( m_rotateWithTorque )
            {
                if ( m_torqueRotationSign > 0.f )
                {
                    if ( dif < 0.f )
                    {
                        dif *= -1.f;
                    }
                }
                else
                {
                    if ( dif > 0.f )
                    {
                        dif *= -1.f;
                    }
                }
            }

            if ( f32_Abs(dif) < (angularSpeed*_deltaTime) )
            {
                _angle = zeroAngle;
            }
            else
            {
                if ( dif > 0.f )
                {
                    _angle = _angle+(angularSpeed*_deltaTime);
                }
                else if ( dif < 0.f )
                {
                    _angle = _angle-(angularSpeed*_deltaTime);
                }
            }

            ITF_ASSERT(_angle==_angle);

            if ( _angle != _angle )
            {
                _angle = 0.f;
            }
            else
            {
                NormalizeAnglePI(_angle);
            }
        }
    }
}

// We can detach from our current edge if our speed is high and at a certain angle from the edge direction
bbool StickToPolylinePhysComponent::checkIfDetachFromCurrentEdgeWithSpeed( f32 _moveSide, f32 _t )
{
    PolyLine* currentPolyline;
    const PolyLineEdge* currentEdge;

    AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,currentPolyline,currentEdge);

    Vec2d edgePosT, edgeVecT, edgeDirT;
    f32 edgeLengthT;

    AIUtils::getEdgeDataInTime(_t,currentPolyline,*currentEdge,m_stickedEdge,edgePosT,edgeVecT,edgeDirT,edgeLengthT);

    // If we are not on ground, and we are not able to run on walls, then unstick directly
    EdgeOrientationType orientationType = getEdgeOrientationType(edgeDirT);
    bbool stopped = m_speed == Vec2d::Zero;

    if ( orientationType != EDGEORIENTATIONTYPE_GROUND  )
    {
        if ( !m_stickOnWalls || stopped )
        {
            return btrue;
        }

        f32 currentSpeed = m_speed.sqrnorm();

        if ( currentSpeed < (getTemplate()->getMinSpeedStickToWall()*getTemplate()->getMinSpeedStickToWall()) )
        {
            return btrue;
        }

        const GameMaterial_Template* mat = (const GameMaterial_Template*)(World::getGameMaterial(currentEdge->getGameMaterial()));

        if ( mat && mat->getNoWalkOnWall() )
        {
            return btrue;
        }
    }

    if ( stopped )
    {
        return bfalse;
    }

    // If we are inside an edge, we can stay sticked
    if ( m_relDist > 0.f && m_relDist < edgeLengthT )
    {
        return bfalse;
    }

    // We are on the corner of an edge and runing towards the next one
    AIUtils::AdjacentEdgeData adjData;

    if ( m_relDist >= (edgeLengthT*0.5f) )
    {
        AIUtils::getAdjacentEdgeData(currentPolyline,*currentEdge,m_stickedEdge,m_currentGravityDir,bfalse,_t,m_relDist,adjData);
    }
    else
    {
        AIUtils::getAdjacentEdgeData(currentPolyline,*currentEdge,m_stickedEdge,m_currentGravityDir,btrue,_t,m_relDist,adjData);
    }

    if ( m_stickOnWalls && m_stickOnWallsCheckConvex )
    {
        // If we are right at the end of an edge, and we are walking on a wall or a roof,
        // and the next edge angle is increasing, then we unstick directly
        if ( orientationType != EDGEORIENTATIONTYPE_GROUND  )
        {
            if ( adjData.m_crossProdEdges > MTH_EPSILON && m_currentGroundDir.dot(m_currentGravityDir) > 0.f )
            {
                return btrue;
            }
        }
    }

    // If we are outside an edge, but runing towards its center then we can stay sticked too
    if ( ( m_relDist >= edgeLengthT && _moveSide == -1.f ) ||
         ( m_relDist <= 0.f && _moveSide == 1.f ) )
    {
        return bfalse;
    }

    if ( adjData.m_adjacentEdgeIndex == U32_INVALID )
    {
        if ( m_stickOnWalls )
        {
            return btrue;
        }
        else if ( adjData.m_crossProdEdges <= MTH_EPSILON )
        {
            // When the angle increases we don't unstick
            return bfalse;
        }
        else
        {
            // If there is no adjacent edge and the angle decreases, we will unstick when we reach the middle point between the two edges
            f32 dotProd = adjData.m_normalA.dot(adjData.m_normalB);

            if ( dotProd < 1.f )
            {
                f32 angleBetweenNormals = f32_ACos(dotProd);
                f32 arcLengthBetweenNormals = angleBetweenNormals * getRadius();
                f32 halfArcLength = arcLengthBetweenNormals*0.5f;

                if ( m_relDist == ( edgeLengthT + halfArcLength ) ||
                     m_relDist == (-halfArcLength) && _moveSide == -1.f )
                {
                    return btrue;
                }
            }
            else
            {
                return btrue;
            }
        }
    }

    f32 speedProjection = f32_Abs(m_speed.dot(edgeDirT));
    f32 dotProd = adjData.m_normalA.dot(adjData.m_normalB);

    if ( dotProd >= 1.f )
    {
        return bfalse;
    }

    Angle angleBetweenNormals = Angle(bfalse,f32_ACos(dotProd));

    if ( angleBetweenNormals < getTemplate()->getUnstickMinAngle() )
    {
        return bfalse;
    }

    angleBetweenNormals = Min(angleBetweenNormals,getTemplate()->getUnstickMaxAngle());

    f32 t = ( ( angleBetweenNormals - getTemplate()->getUnstickMinAngle() ) / ( getTemplate()->getUnstickMaxAngle() - getTemplate()->getUnstickMinAngle() ) ).ToRadians();
    f32 unstickSpeed = Interpolate(getTemplate()->getUnstickMinAngleSpeed(),getTemplate()->getUnstickMaxAngleSpeed(),t);

    return speedProjection >= unstickSpeed;
}

bbool StickToPolylinePhysComponent::isEdgeInMemory( ObjectRef _ref, u32 _edgeIndex ) const
{
    u32 memSize = m_memoryStickedEdges.size();
    PolyLine* poly;
    PolyLine* adjPoly;
    u32 adjEdgeR, adjEdgeL;

    for ( u32 i = 0; i < memSize; i++ )
    {
        const EdgeMemoryData& data = m_memoryStickedEdges[i];

        if ( data.m_polyRef == _ref )
        {
            if ( data.m_edgeIndex == _edgeIndex )
            {
                return btrue;
            }

            poly = AIUtils::getPolyLine(data.m_polyRef);

            if ( !poly )
            {
                continue;
            }

            AIUtils::getAdjacentEdge(poly,_edgeIndex,btrue,adjPoly,adjEdgeR);
            AIUtils::getAdjacentEdge(poly,_edgeIndex,bfalse,adjPoly,adjEdgeL);

            if ( adjEdgeR == data.m_edgeIndex || adjEdgeL == data.m_edgeIndex )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

void StickToPolylinePhysComponent::calculateFrictionCoefficient()
{
    const PolyLineEdge* edge = getStickedEdge();

    if ( edge )
    {
        Vec2d edgeNormInv = -m_currentGroundDir.getPerpendicular();

        m_currentFrictionCoef = m_frictionProjection = edgeNormInv.dot(m_totalForcesThisFrame);

        if ( m_currentFrictionCoef > 0.f )
        {
            f32 edgeFriction = AIUtils::getEdgeFriction(edge);

            applyGroundFrictionRegion(edgeFriction);

            m_currentFrictionCoef *= getTemplate()->getFriction() * m_frictionMultiplier * edgeFriction;
        }
        else
        {
            m_currentFrictionCoef = 0.f;
        }
    }
    else
    {
        m_frictionProjection = 0.f;
    }
}

void StickToPolylinePhysComponent::applyFrictionRegion( bbool& _sticked )
{
    PolyLine* poly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Friction,m_actor->get2DPos());

    if ( poly )
    {
        const GameMaterial_Template* mat = (const GameMaterial_Template*)(World::getGameMaterial(poly->getEdgeAt(0).getGameMaterial()));

        if ( mat )
        {
            if ( _sticked )
            {
                // Check if the force is perpendicular to the edge, in which case we will most probably unstick
                // and in that case we want to apply the air friction
                f32 forceProj = m_totalForcesThisFrame.dot(m_currentGroundDir.getPerpendicular());

                if ( forceProj >= 50.f )
                {
                    _sticked = bfalse;
                }
            }

            f32 regionFrictionMultiplier = _sticked ? mat->getRegionFriction() : mat->getRegionFrictionAir();

            if ( regionFrictionMultiplier )
            {
                Vec2d oppositeForce = -m_totalForcesThisFrame*regionFrictionMultiplier;

                m_totalForcesThisFrame += oppositeForce;
            }
        }
    }
}

void StickToPolylinePhysComponent::applyGroundFrictionRegion( f32& _friction )
{
    PolyLine* poly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_GroundFriction,m_actor->get2DPos());

    if ( poly )
    {
        const GameMaterial_Template* mat = (const GameMaterial_Template*)(World::getGameMaterial(poly->getEdgeAt(0).getGameMaterial()));

        if ( mat )
        {
            _friction *= mat->getFriction();
        }
    }
}

// Apply the forces and friction from the environment, then calculate our current speed
void StickToPolylinePhysComponent::calculateSpeedFromForces( f32 _deltaTime )
{
    m_totalForcesThisFrame = m_force + m_windForcesThisFrame;

    const PolyLineEdge* edge = getStickedEdge();
    bbool sticked = edge != NULL;
    Vec2d currentGravity = m_currentGravity*m_gravityMultiplier;

    if ( m_stickOnWalls && m_stickOnWallsShiftGravity && sticked )
    {
        EdgeOrientationType edgeOrientation = getEdgeOrientationType(edge->m_normalizedVector);

        if ( edgeOrientation == EDGEORIENTATIONTYPE_WALL || edgeOrientation == EDGEORIENTATIONTYPE_ROOF )
        {
            currentGravity = -edge->m_normalizedVector.getPerpendicular() * currentGravity.norm();
        }
    }

    if ( !sticked )
    {
        m_totalForcesThisFrame += currentGravity;
    }

    if ( m_totalForcesThisFrame != Vec2d::Zero )
    {
        applyFrictionRegion(sticked);
    }

    if ( sticked )
    {
        m_totalForcesThisFrame += currentGravity;
    }

    if ( edge )
    {
        Vec2d edgeNormInv = -m_currentGroundDir.getPerpendicular();
        const GameMaterial_Template* mat = (const GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

        calculateFrictionCoefficient();

        if ( m_currentFrictionCoef > 0.f )
        {
            Vec2d moveForce = m_currentGroundDir * m_currentGroundDir.dot(m_totalForcesThisFrame);

            m_speed += moveForce * _deltaTime;

            f32 friction = m_currentFrictionCoef * _deltaTime;

            if ( friction )
            {
                f32 spdOnEdge = m_speed.norm();

                if (friction > f32_Abs(spdOnEdge))
                {
                    m_speed = Vec2d::Zero;
                }
                else
                {
                    // Reduce our speed with the friction from the edge
                    Vec2d applyFriction = -m_speed;

                    applyFriction.normalize();
                    applyFriction *= friction;
                    m_speed += applyFriction;
                }
            }
        }
        else
        {
            m_speed += m_totalForcesThisFrame*_deltaTime;
        }

        if ( mat )
        {
            m_speed *= mat->getSpeedMultiplier();
        }
    }
    else
    {
        m_frictionProjection = m_currentFrictionCoef = 0.f;
        m_speed += m_totalForcesThisFrame*_deltaTime;

        if ( getTemplate()->getAirFriction() && m_airFrictionMultiplier )
        {
            f32 friction = getTemplate()->getAirFriction() * m_airFrictionMultiplier * _deltaTime;
            f32 speedSize = m_speed.norm();

            if ( speedSize > friction )
            {
                speedSize -= friction;
                m_speed.normalize();
                m_speed *= speedSize;
            }
            else
            {
                m_speed = Vec2d::Zero;
            }
        }
    }

    m_speed *= m_windSpeedMultiplier;
    m_speed *= m_speedMultiplier;

    clampWithMaxSpeed(m_speed);
}

// Get our current speed based on our linear speed
f32 StickToPolylinePhysComponent::calculateAngularSpeedFromLinearSpeed( bbool _torqueRotation ) const
{
    f32 currentSpeed = m_speed.norm();

    currentSpeed = Clamp(currentSpeed,getTemplate()->getAngularSpeedMinLinear(),getTemplate()->getAngularSpeedMaxLinear());

    f32 t = ( currentSpeed - getTemplate()->getAngularSpeedMinLinear() ) / ( getTemplate()->getAngularSpeedMaxLinear() - getTemplate()->getAngularSpeedMinLinear() );

    f32 angularSpeed = Interpolate(getTemplate()->getAngularSpeedMinAngular().ToRadians(),getTemplate()->getAngularSpeedMaxAngular().ToRadians(),t);

    if ( !getStickedEdge() )
    {
        angularSpeed *= getTemplate()->getAngularSpeedAirMultiplier();
    }

    if ( _torqueRotation )
    {
        angularSpeed *= getTemplate()->getTorqueRotationMultiplier();
    }

    return angularSpeed;
}

void StickToPolylinePhysComponent::setCurrentGroundAngularSpeedFromRotation( f32 _currentRotation, f32 _targetRotation )
{
    m_groundCurrentAngularSpeed = calculateAngularSpeedFromLinearSpeed(bfalse);

    f32 dif = f32_Abs(getShortestAngleDelta(_currentRotation,_targetRotation));

    NormalizeAngle2PI(dif);

    dif = Clamp(dif,getTemplate()->getGroundAngularSpeedMultiplierMinAngle().ToRadians(),getTemplate()->getGroundAngularSpeedMultiplierMaxAngle().ToRadians());

    f32 t = ( dif - getTemplate()->getGroundAngularSpeedMultiplierMinAngle().ToRadians() ) /
            ( m_groundAngularSpeedMultiplierMaxAngle.ToRadians() - getTemplate()->getGroundAngularSpeedMultiplierMinAngle().ToRadians() );

    f32 angleDifMultiplier = Interpolate(getTemplate()->getGroundAngularSpeedMultiplierMinMultiplier(),getTemplate()->getGroundAngularSpeedMultiplierMaxMultiplier(),t);

    m_groundCurrentAngularSpeed *= angleDifMultiplier;
    m_groundCurrentAngularSpeed *= m_angularSpeedMultiplier;
}

// Apply the final data to the actor and the physic body
void StickToPolylinePhysComponent::applyPosAndAngle( const Vec2d& _pos, f32 _angle )
{
    if ( !m_disabled  )
    {
        m_actor->updateComponentPosAngle(_pos.to3d(m_actor->getPos().m_z),_angle,this);
    }
}

// This sets the position and angle normally using the relative distance and relative angle. No interpolation between edges is done
void StickToPolylinePhysComponent::SetEdgePosition( PolyLine* _polyLine, const class PolyLineEdge& _edge, u32 _edgeIndex, f32 _relDist, Vec2d& _pos, f32 _t ) const
{
    Vec2d edgePosT, edgeVecT, edgeDirT;
    f32 edgeLengthT;

    AIUtils::getEdgeDataInTime(_t,_polyLine,_edge,_edgeIndex,edgePosT,edgeVecT,edgeDirT,edgeLengthT);

    Vec2d newPos = edgePosT + ( edgeDirT * _relDist );
    newPos += edgeDirT.getPerpendicular() * getRadius();

    _pos = newPos;
    ITF_ASSERT(_pos==_pos);
}

void StickToPolylinePhysComponent::stick( class PolyLine* _polyLine, u32 _edgeIndex, Vec2d& _pos, f32& _angle, f32 _t )
{
    const PolyLineEdge* prevEdge = getStickedEdge();
    bbool wasAttached = prevEdge != NULL;

    m_memoryNoStickPlatforms.clear();

    setStickedEdge(_polyLine,_edgeIndex);

    const PolyLineEdge& edge = _polyLine->getEdgeAt(_edgeIndex);

    // Adjust our relative angle to whatever our world is right now. We will move towards m_targetAngle
    // in the update
    m_relAngle = _angle - getVec2Angle(edge.m_normalizedVector);

    NormalizeAnglePI(m_relAngle);

    // Find out what our relative distance is and the correct relative angle
    getRelDistFromPos(_pos,_t,m_relDist,m_targetAngle);

    ITF_ASSERT(m_relDist==m_relDist);

    NormalizeAnglePI(m_targetAngle);

    // This adjust our edge and relative distance in case we are outside the boundaries
    // We don't want to modify our speed with this, it will be done later in this function
    Vec2d tempSpeed;
    adjustCurrentRelDist(1.f,tempSpeed);

    // It's possible that after adjusting our relative distance we realize we fell out of the edge
    // To cover this case, return here
    PolyLine* stickedPoly;
    const PolyLineEdge* stickedEdge;

    AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,stickedPoly,stickedEdge);

    if ( !stickedEdge || !stickedPoly )
    {
        return;
    }
    
    u32 cornerCase = U32_INVALID;
    bbool resetWorldSpeed;

    if ( !wasAttached &&
       ( m_relDist < 0.f || m_relDist > stickedEdge->m_length ) &&
         isCornerCase(_polyLine,m_stickedEdge,m_relDist,m_speed,1.f,cornerCase) &&
         cornerCase == 0 )
    {
        m_relDist = Clamp(m_relDist,0.f,stickedEdge->m_length);
        ITF_ASSERT(m_relDist==m_relDist);
        m_betweenEdgesFactor = 0.f;
        m_insideEdgeFactor = m_relDist / stickedEdge->m_length;
        ITF_ASSERT(m_insideEdgeFactor==m_insideEdgeFactor);
        m_speed = stickedEdge->m_normalizedVector * stickedEdge->m_normalizedVector.dot(m_speed);
        m_targetAngle = 0.f;
        resetWorldSpeed = btrue;
    }
    else
    {
        // Project our speed on the new edge
        resetWorldSpeed = btrue;

        if ( wasAttached )
        {
            // If we were attached to another edge we transfer the speed we carried to the new one
            Vec2d edgeNorm = prevEdge->m_normalizedVector.getPerpendicular();
            Vec2d speedDir = m_speed;

            speedDir.normalize();

            bbool back = speedDir.cross(edgeNorm) > 0.f ? bfalse : btrue;

            transferSpeedToNewEdge(prevEdge->m_normalizedVector,edge.m_normalizedVector,back,m_speed);
        }
        else
        {
            // Otherwise we do a projection and calculate the speed loss from our current speed angle and
            // the new edge angle
            transferSpeedFromLanding(m_currentGroundDir,m_speed,stickedPoly,stickedEdge,m_stickedEdge,m_relDist);
        }
    }

    // Get the final proper values for _pos and _angle
    getPosFromRelDist(stickedPoly,stickedEdge,m_stickedEdge,m_relDist,_t,_pos);

    if ( resetWorldSpeed )
    {
        m_prevPosition = _pos;
    }

    if ( !wasAttached )
    {
        m_forceEdgeAngleNext = btrue;
    }

    calculateFrictionCoefficient();

    if ( m_debugLog )
    {
        LOG("StickToPolylinePhysComponent: Sticked to new Edge, RelDist %f",m_relDist);
    }

    setCurrentGroundAngularSpeedFromRotation(m_relAngle,m_targetAngle);
}

// Refer to U:\DOC\GAMEPLAY\Corner.ppsx
bbool StickToPolylinePhysComponent::isCornerCase( PolyLine* _poly, u32 _edgeIndex,
                                                  f32 _relDist, const Vec2d& _speed, f32 _t,
                                                  u32& _case ) const
{
    if ( _speed == Vec2d::Zero || !getTemplate()->getTreatCornerCase() || m_cornerCaseDisableDelay )
    {
        return bfalse;
    }

    const PolyLineEdge& edge = _poly->getEdgeAt(_edgeIndex);
    AIUtils::AdjacentEdgeData adjacentEdgeData;

    AIUtils::getAdjacentEdgeData(_poly,edge,_edgeIndex,m_currentGravityDir,_relDist < 0.f,_t,m_relDist,adjacentEdgeData);

    // See if it's a corner where we could stand
    f32 dotProdGravEdgeA = f32_Abs(edge.m_normalizedVector.dot(m_currentGravityDir));
    f32 dotProdGravEdgeB = f32_Abs(adjacentEdgeData.m_adjacentEdgeDir.dot(m_currentGravityDir));

    f32 angleA = f32_ACos(dotProdGravEdgeA);
    f32 angleB = f32_ACos(dotProdGravEdgeB);

    bbool edgeAcanStick = angleA > (MTH_DEGTORAD*45.f) &&
                          angleA < (MTH_DEGTORAD*135.f);
    bbool edgeBcanStick = angleB > (MTH_DEGTORAD*45.f) &&
                          angleB < (MTH_DEGTORAD*135.f);

    if ( ( edgeAcanStick && edgeBcanStick ) ||
         ( !edgeAcanStick && !edgeBcanStick ) )
    {
        return bfalse;
    }

    f32 angle = f32_ACos(adjacentEdgeData.m_dotProdEdges);

    if ( angle > (MTH_DEGTORAD*135.f) )
    {
        // If the angle between the edges is bigger than 135 do nothing
        return bfalse;
    }

    Vec2d speedDir = _speed;

    speedDir.normalize();

    if ( speedDir.dot(m_currentGravityDir) < 0.f )
    {
        // If the speed is going opposite to the gravity do nothing
        return bfalse;
    }

    f32 dotProdGravityEdge = f32_Abs(edge.m_normalizedVector.dot(m_currentGravityDir));
    f32 angleGravityEdge = f32_ACos(dotProdGravityEdge);

    if ( angleGravityEdge >= 0.f &&
         angleGravityEdge <= (MTH_DEGTORAD*20.f) )
    {
        _case = 1;
        return btrue;
    }
    else
    {
        f32 dotProdSpeedEdge = f32_Abs(edge.m_normalizedVector.dot(speedDir));
        f32 angleSpeedEdge = f32_ACos(dotProdSpeedEdge);

        if ( angleGravityEdge >= (MTH_DEGTORAD*20.f) &&
             angleGravityEdge <= (MTH_DEGTORAD*160.f) &&
             angleSpeedEdge > (MTH_DEGTORAD*25.f) )
        {
            // In this corner-case, we teleport the character inside the range of the edge. To make sure we can do this,
            // we need to make sure there is no collision there that could kill us.
            Vec2d newPos;
            f32 myRadius = getRadius();

            if ( _relDist < 0.f )
            {
                newPos = edge.getPos();
            }
            else
            {
                newPos = edge.getPos() + edge.m_vector;
            }

            newPos += edge.m_normalizedVector.getPerpendicular() * myRadius;

            PhysContactsContainer contacts;
            PhysShapeCircle circle(myRadius);
            PolyLine* polyTest;
            const PolyLineEdge* edgeTest;

            PHYSWORLD->checkEncroachment(newPos,newPos,m_actor->getAngle(),&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

            u32 numContacts = contacts.size();
            f32 radiusCheck = (myRadius*myRadius)-0.001f;

            for ( u32 i = 0; i < numContacts; i++ )
            {
                const SCollidableContact& c = contacts[i];

                if ( c.m_edgeIndex == U32_INVALID )
                {
                    continue;
                }

                if ( c.m_collidableUserData == _poly->getRef() &&
                     c.m_edgeIndex == _edgeIndex )
                {
                    continue;
                }

                AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,polyTest,edgeTest);

                const GameMaterial_Template* mat = World::getGameMaterial(edgeTest->getGameMaterial());

                if ( mat && !mat->isSolid() )
                {
                    continue;
                }

                f32 dist = ( c.m_contactPoint - c.m_testPosition ).sqrnorm();

                if ( dist < radiusCheck )
                {
                    return bfalse;
                }
            }

            _case = 0;
            return btrue;
        }
    }

    return bfalse;
}

// This recalculates our speed vector to go along our current edge
void StickToPolylinePhysComponent::transferSpeedToNewEdge( const Vec2d& _prevDir, const Vec2d& _newDir, bbool _back, Vec2d& _speed ) const
{
    if ( _prevDir != _newDir )
    {
        f32 totalSpeed = _speed.norm();
        f32 crossProd = _back ? -_newDir.cross(_prevDir) : -_prevDir.cross(_newDir);
        f32 loss;

        if ( crossProd < -MTH_EPSILON )
        {
            f32 prevAngle = getVec2Angle(_prevDir);
            f32 newAngle = getVec2Angle(_newDir);
            f32 angleDif = f32_Abs(getShortestAngleDelta(prevAngle,newAngle));
            angleDif = Clamp(angleDif,getTemplate()->getTransferSpeedLossMinAngle().ToRadians(),getTemplate()->getTransferSpeedLossMaxAngle().ToRadians());
            f32 t = ( angleDif - getTemplate()->getTransferSpeedLossMinAngle().ToRadians() ) / ( getTemplate()->getTransferSpeedLossMaxAngle().ToRadians() - getTemplate()->getTransferSpeedLossMinAngle().ToRadians() );
            loss = Interpolate(getTemplate()->getTransferSpeedLossMinLoss(),getTemplate()->getTransferSpeedLossMaxLoss(),t);
            loss = Clamp(loss,0.f,1.f);
        }
        else
        {
            loss = 0.f;
        }

        _speed = _newDir * ( totalSpeed * ( 1.f - loss ) );

        if ( _back )
        {
            _speed *= -1.f;
        }
    }
    else
    {
        _speed = _newDir * _speed.norm();

        if ( _back )
        {
            _speed *= -1.f;
        }
    }
    _speed *= m_speedLossMultiplier;
}

void StickToPolylinePhysComponent::transferSpeedFromLanding( const Vec2d& _newDir, Vec2d& _speed, 
                                                             const PolyLine* _poly, 
                                                             const PolyLineEdge* _edge,
                                                             u32 _edgeIndex,
                                                             f32 _relDist ) const
{
    // Project the speed
    f32 finalSpeed = _newDir.dot(_speed);

    finalSpeed *= 1.f - _poly->m_speedLoss;

    _speed = _newDir * finalSpeed;

    if ( getEdgeOrientationType(_newDir) == EDGEORIENTATIONTYPE_GROUND )
    {
        if ( m_currentGravityDir.dot(_speed) > 0.f )
        {
            // If we are going down towards gravity we apply a speed loss based on the angle of the edge with the gravity
            f32 gravDotProd = f32_Abs(m_currentGravityDir.getPerpendicular().dot(_newDir));
            f32 angleWithGravity = f32_ACos(gravDotProd);

            angleWithGravity = Min(angleWithGravity,getTemplate()->getSpeedLossLandEdgeMaxAngle().ToRadians());

            if ( angleWithGravity >= getTemplate()->getSpeedLossLandEdgeMinAngle().ToRadians() )
            {
                f32 t = ( angleWithGravity - getTemplate()->getSpeedLossLandEdgeMinAngle().ToRadians() ) /
                        ( getTemplate()->getSpeedLossLandEdgeMaxAngle().ToRadians() - getTemplate()->getSpeedLossLandEdgeMinAngle().ToRadians() );

                f32 loss = Interpolate(getTemplate()->getSpeedLossLandMin(),getTemplate()->getSpeedLossLandMax(),t);

                _speed *= 1.f - loss;
            }
        }
    }

    _speed *= m_speedLossMultiplier;
}

void StickToPolylinePhysComponent::transferSpeedFromBlockingObstacle( const SCollidableContact& _contact,
                                                                      f32 _t,
                                                                      Vec2d& _speed ) const
{
    Vec2d currentSpeedDir = _speed;

    currentSpeedDir.normalize();

    // In the case that for whatever we are inside a collision, and our speed was actually going out of the collision, then we don't 
    // need to project anything
    f32 dotProd = _contact.m_normal.dot(currentSpeedDir);

    if ( dotProd < 0.f )
    {
        return;
    }

    Vec2d newDir = _contact.m_normal.getPerpendicular();

    _speed = projectSpeedOnDir(newDir,_speed);

    const PolyLineEdge* stickedEdge = getStickedEdge();

    if ( stickedEdge )
    {
        _speed = projectSpeedOnDir(stickedEdge->m_normalizedVector,_speed);
    }

    if ( _speed.IsEqual(Vec2d::Zero,MTH_EPSILON) )
    {
        _speed = Vec2d::Zero;
    }
}

// This will translate the speed as it is into the new edge
Vec2d StickToPolylinePhysComponent::projectSpeedOnDir( const Vec2d& _newDir, const Vec2d& _currentSpeed ) const
{
    return _newDir * _newDir.dot(_currentSpeed);
}

void StickToPolylinePhysComponent::unstick( bbool _addToPlatformMemory )
{
    const PolyLineEdge* prevEdge = getStickedEdge();

    if ( prevEdge )
    {
        m_memoryStickedEdges.push_back(EdgeMemoryData(m_stickedLineRef,m_stickedEdge,MEMORY_EDGE_TIME));

        bbool addToMemory;

        if ( _addToPlatformMemory )
        {
            addToMemory = btrue;
        }
        else
        {
            if ( !m_speed.IsEqual(Vec2d::Zero,MTH_EPSILON) )
            {
                Vec2d speedDir;
                
                Vec2d::Normalize(&speedDir,&m_speed);

                addToMemory = speedDir.dot(m_currentGroundDir) < 0.f;
            }
            else
            {
                addToMemory = bfalse;
            }
        }

        if ( addToMemory )
        {
            const GameMaterial_Template* mat = World::getGameMaterial(prevEdge->getGameMaterial());

            if ( mat && !mat->isSolid() )
            {
                m_memoryNoStickPlatforms.push_back(EdgeMemoryData(m_stickedLineRef,m_stickedEdge,MEMORY_EDGE_TIME));
            }
        }
    }

    setStickedEdge(NULL,U32_INVALID);

    if ( m_debugLog )
    {
        LOG("StickToPolylinePhysComponent: Unsticked");
    }

    m_rotateWithTorque = bfalse;
    
    if ( prevEdge &&
         getEdgeOrientationType(prevEdge->m_normalizedVector) == EDGEORIENTATIONTYPE_ROOF )
    {
        f32 xSpeed = m_speed.dot(m_currentGravityDir.getPerpendicular());
        f32 ySpeed = m_speed.dot(m_currentGravityDir);

        if ( f32_Abs(xSpeed) > 1.f && ySpeed < 0.f )
        {
            m_rotateWithTorque = btrue;
            m_torqueRotationSign = getSign(-xSpeed);
        }
    }

    m_cornerCaseDisableDelay = getTemplate()->getCornerCaseDisableDelay();
}

void StickToPolylinePhysComponent::setStickedEdge( PolyLine* _polyLine, u32 _edgeIndex )
{
    if ( ( !_polyLine && m_stickedLineRef == ITF_INVALID_OBJREF ) ||
         ( _polyLine && _polyLine->getRef() == m_stickedLineRef && _edgeIndex == m_stickedEdge ) )
    {
        return;
    }

    if ( m_debugLog && _polyLine )
    {
        LOG("StickToPolylinePhysComponent: Change to Edge %d Polyline %u, Rel Dist %f",_edgeIndex,_polyLine->getRef().getValue(),m_relDist);
    }

    PolyLine* prevPoly = getStickedPolyline();

    if ( prevPoly )
    {
        if ( prevPoly != _polyLine )
        {
            EventStickOnPolyline unstickEvent(prevPoly->getRef(),m_actor->getRef(),m_stickedEdge,getWeight(),
                m_actor->get2DPos(),m_speed,bfalse);
            if ( prevPoly->getOwnerActor())
            {
                prevPoly->getOwnerActor()->onEvent(&unstickEvent);
            }
            if ( prevPoly->m_ownerFrise)
            {
                prevPoly->m_ownerFrise->onEvent(&unstickEvent);
            }
        }
    }

    if ( _polyLine )
    {
        if ( prevPoly != _polyLine )
        {
            EventStickOnPolyline stickEvent(_polyLine->getRef(),m_actor->getRef(),_edgeIndex,getWeight(),
                                            m_actor->get2DPos(),m_speed,btrue);

            if ( _polyLine->getOwnerActor() )
            {
                _polyLine->getOwnerActor()->onEvent(&stickEvent);
            }
            if ( _polyLine->m_ownerFrise )
            {
                _polyLine->m_ownerFrise->onEvent(&stickEvent);
            }
        }
        else
        {
            if ( _polyLine->getOwnerActor() )
            {
                EventStickOnPolylineChangeEdge stickEvent(_polyLine->getRef(),m_actor->getRef(),_edgeIndex,getWeight(),
                                                          m_actor->get2DPos(),m_speed);
                _polyLine->getOwnerActor()->onEvent(&stickEvent);
            }
        }

        m_stickedLineRef = _polyLine->getRef();
        m_stickedEdge = _edgeIndex;
    }
    else
    {
        m_stickedLineRef = ITF_INVALID_OBJREF;
        m_stickedEdge = U32_INVALID;
    }

    EventStickOnPolylineUpdate polyUpdate(m_stickedLineRef,m_stickedEdge,m_speed);
    m_actor->onEvent(&polyUpdate);

    if ( m_stickedEdge != U32_INVALID )
    {
        if ( !m_traveledEdgesThisFrame.full() )
        {
            m_traveledEdgesThisFrame.push_back(TravelEdgeData(m_stickedLineRef,m_stickedEdge));
        }
    }
}

// The cheat to move the character freely around the map
void StickToPolylinePhysComponent::setDisabled( bbool _val, bbool _unstick, bbool _clearInfo )
{
    if ( m_disabled != _val )
    {
        m_disabled = _val;

        if ( m_disabled )
        {
            if ( _unstick)
            {
                unstick();
            }

            if ( _clearInfo )
            {
                m_contacts.clear();
                m_memoryStickedEdges.clear();
                m_memoryNoStickPlatforms.clear();
            }
        }

        // Reset this so that the world speed don't get crazy
        m_prevPosition = m_actor->get2DPos();
    }
}

PolyLine* StickToPolylinePhysComponent::getStickedPolyline() const
{
    if ( m_stickedLineRef != ITF_INVALID_OBJREF )
    {
        PolyLine* poly = static_cast<PolyLine*>(GETOBJECT(m_stickedLineRef));

        if ( poly && m_stickedEdge != U32_INVALID && m_stickedEdge < poly->getVectorsCount() )
        {
            return poly;
        }
    }

    return NULL;
}

const PolyLineEdge* StickToPolylinePhysComponent::getStickedEdge() const
{
    const PolyLine* poly = getStickedPolyline();

    if ( poly )
    {
        return &poly->getEdgeAt(m_stickedEdge);
    }
    else
    {
        return NULL;
    }
}

void StickToPolylinePhysComponent::processContacts( const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, bbool& _blocked, Vec2d& _speed, f32 _dt )
{
    if ( getTemplate()->getTreatMovingPolylines() )
    {
        processContactsDynamicEnv(_prevPos,_pos,_angle,_extraRadius,_blocked,_speed,_dt);
    }
    else
    {
        processContactsStaticEnv(_prevPos,_pos,_angle,_extraRadius,bfalse,_blocked,_speed);
    }
}

void StickToPolylinePhysComponent::processContactsStaticEnv( const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, bbool _reuseContacts, bbool& _blocked, Vec2d& _speed )
{
    ObjectRef foundStickingEdgePoly = ITF_INVALID_OBJREF;
    u32 foundStickingEdgeEdgeIndex = U32_INVALID;
    Vec2d foundStickingEdgePos = Vec2d::Zero;
    bbool foundStickingEdge = bfalse;
    f32 radius = getRadius();

    if ( !_reuseContacts )
    {
        m_contacts.clear();

        if (m_processContacts)
        {
            PhysShapeCircle circle(radius+_extraRadius);

            PHYSWORLD->checkEncroachment(_prevPos,_pos,_angle,&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),m_contacts);

            processContactsUpdateStickPlatforms();
        }
    }

    // search for sticking to polylines
    _blocked = bfalse;

    // First process the blocking contacts
    u32 blockingContact = U32_INVALID;
    f32 blockingContactT = -1.f;

    Vec2d moveDir =  _pos - _prevPos;
    f32 lastT = 0.f;
    f32 minDist = 0.f;

    moveDir.normalize();

    for ( u32 i = 0; i < m_contacts.size(); i++ )
    {
        const SCollidableContact& c = m_contacts[i];

        if ( c.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        if ( lastT != c.m_t && ( blockingContact != U32_INVALID || foundStickingEdge ) )
        {
            break;
        }

        PolyLine* polyLine = static_cast<PolyLine*>(GETOBJECT(c.m_collidableUserData));

        if ( !polyLine || polyLine->getOwnerActor() == m_actor )
        {
            continue;
        }

        if ( m_stickedLineRef != ITF_INVALID_OBJREF &&
             m_stickedLineRef == c.m_collidableUserData &&
             m_stickedEdge == c.m_edgeIndex )
        {
            continue;
        }

        if ( isContactValidToStick(c,1.f,c.m_testPosition,_angle,bfalse,_speed) )
        {
            f32 dist = ( c.m_testPosition - c.m_contactPoint ).sqrnorm();

            if ( !foundStickingEdge || dist < minDist )
            {
                foundStickingEdge = btrue;
                foundStickingEdgePoly = c.m_collidableUserData;
                foundStickingEdgeEdgeIndex = c.m_edgeIndex;
                foundStickingEdgePos = c.m_testPosition;
                minDist = dist;
            }
            continue;
        }

        if ( isContactBlockingObstacle(c,_angle,moveDir) )
        {
            f32 dist = ( c.m_contactPoint - _prevPos ).sqrnorm();

            if ( blockingContact == U32_INVALID ||
                 blockingContactT > dist )
            {
                blockingContact = i;
                blockingContactT = dist;
            }
        }

        lastT = c.m_t;
    }

    if ( blockingContact != U32_INVALID )
    {
        // If we found an obstacle we will want to readjust our position
        processContactObstacle(m_contacts[blockingContact],1.f,_pos,_speed,_blocked);
    }
    else if ( foundStickingEdgeEdgeIndex != U32_INVALID &&
            ( foundStickingEdgeEdgeIndex != m_stickedEdge || foundStickingEdgePoly != m_stickedLineRef ) )
    {
        ITF_ASSERT(foundStickingEdgePos==foundStickingEdgePos);
        _pos = foundStickingEdgePos;
        ITF_ASSERT(_pos==_pos);
        PolyLine* stickPoly = AIUtils::getPolyLine(foundStickingEdgePoly);

        if ( stickPoly )
        {
            stick(stickPoly,foundStickingEdgeEdgeIndex,_pos,_angle);
        }
    }
}

// Process collision with environment that could be dynamic (moving polylines)
void StickToPolylinePhysComponent::processContactsDynamicEnv( const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, bbool& _blocked, Vec2d& _speed, f32 _dt )
{
    m_contacts.clear();

    if (m_processContacts)
    {
        PhysShapeCircle circle(getRadius()+_extraRadius);

        PHYSWORLD->checkEncroachment(_prevPos,_pos,_angle,&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),m_contacts);

        processContactsUpdateStickPlatforms();
    }

    PhysContactsContainer dynContacts;

    processContactsDynamicEnv_findDynContacts(m_contacts,dynContacts);

    u32 numDynContacts = dynContacts.size();

    if ( !numDynContacts )
    {
        // No dynamic contacts, treat collisions as an static environment
        processContactsStaticEnv(_prevPos,_pos,_angle,_extraRadius,btrue,_blocked,_speed);
        return;
    }

    // We have dynamic contacts. Put all of them on a list and resolve the collisions with multiple iterations over time
    processContactsDynamicEnv_resolveContacts(m_contacts,_prevPos,_pos,_angle,_extraRadius,_speed,_blocked,_dt);
}

void StickToPolylinePhysComponent::processContactsDynamicEnv_findDynContacts( const PhysContactsContainer& _allContacts, PhysContactsContainer& _dynContacts ) const
{
    u32 numContacts = _allContacts.size();
    PolyLine* poly;
    const PolyLineEdge* edge;

    for ( u32 i = 0; i < numContacts; i++ )
    {
        const SCollidableContact& c = _allContacts[i];

        if ( c.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        if ( c.m_edgeIndex == m_stickedEdge && c.m_collidableUserData == m_stickedLineRef )
        {
            continue;
        }

        AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

        if ( !poly || !edge )
        {
            continue;
        }

        if ( !poly->m_physShape )
        {
            continue;
        }

        PhysShapeMovingPolyline* moving = poly->m_physShape->DynamicCast<PhysShapeMovingPolyline>(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108));

        if ( !moving || !moving->hasMoved() )
        {
            continue;
        }

        _dynContacts.push_back(c);
    }
}

f32 StickToPolylinePhysComponent::processContactsDynamicEnv_findTIncrement( const PhysContactsContainer& _dynContacts, f32 _dt, f32 _moveSize ) const
{
    DynamicContactData data;
    static const f32 s_maxMoveClamp = 0.1f;
    f32 maxMove = _moveSize;

    u32 numNumContacts = _dynContacts.size();

    for ( u32 i = 0; i < numNumContacts; i++ )
    {
        const SCollidableContact& c = _dynContacts[i];
        AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,data.m_polyline,data.m_edgeEnd);
        PhysShapeMovingPolyline* movingPoly = data.m_polyline->m_physShape->DynamicCast<PhysShapeMovingPolyline>(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108));

        if ( movingPoly )
        {
                data.m_edgeStart = &movingPoly->getPreviousPolyline()->getEdgeAt(c.m_edgeIndex);
                data.m_edgePos0Start = data.m_edgeStart->getPos();
                data.m_edgePos0End = data.m_edgeEnd->getPos();
                data.m_edgePos1Start = data.m_edgeStart->getPos() + data.m_edgeStart->m_vector;
                data.m_edgePos1End = data.m_edgeEnd->getPos() + data.m_edgeEnd->m_vector;

                f32 dist = ( data.m_edgePos0Start - data.m_edgePos0End ).norm();

                if ( maxMove == 0.f || dist > maxMove )
                {
                    maxMove = dist;
                }

                dist = ( data.m_edgePos1Start - data.m_edgePos1End ).norm();

                if ( maxMove == 0.f || dist > maxMove )
                {
                    maxMove = dist;
                }
        }
    }

    if ( maxMove <= s_maxMoveClamp )
    {
        return 1.f;
    }
    else
    {
        f32 dtIncr = 1.f/(maxMove/s_maxMoveClamp);
        return dtIncr;
    }
}

void StickToPolylinePhysComponent::processContactsDynamicEnv_resolveContacts( const PhysContactsContainer& _dynContacts, const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, Vec2d& _speed, bbool& _blocked, f32 _dt )
{
    f32 currentT = 0.f;
    f32 radius = getRadius();
    PhysShapeCircle circle(radius);
    Vec2d retCirclePos;
    Vec2d retSegmentPos;
    Vec2d retNormal;
    Vec2d moveDir = _pos - _prevPos;
    f32 moveSize = moveDir.norm();
    f32 tIncrement = processContactsDynamicEnv_findTIncrement(_dynContacts,_dt,moveSize);
    tIncrement = Clamp(tIncrement,0.1f,1.f);
    f32 timeDt = _dt*tIncrement;

    moveDir.normalize();

    Vec2d moveIncr = moveDir*moveSize*tIncrement;

    _pos = _prevPos;

    Vec2d prevPos = _pos;

    while ( currentT <= 1.f )
    {
        m_contacts.clear();

        PHYSWORLD->checkEncroachment(_pos,_pos,_angle,&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),m_contacts);

        ObjectRef foundStickingEdgePoly = ITF_INVALID_OBJREF;
        u32 foundStickingEdgeEdgeIndex = U32_INVALID;
        Vec2d foundStickingEdgePos = Vec2d::Zero;
        bbool foundStickingEdge = bfalse;
        u32 blockingContact = U32_INVALID;
        f32 blockingContactT = -1.f;

        Vec2d currentMoveDir = _pos - prevPos;
        f32 currentMoveSize = currentMoveDir.norm();
        currentMoveDir.normalize();

        for ( u32 i = 0; i < m_contacts.size(); i++ )
        {
            SCollidableContact& c = m_contacts[i];

            if ( c.m_edgeIndex == U32_INVALID )
            {
                continue;
            }

            PolyLine* polyLine = static_cast<PolyLine*>(GETOBJECT(c.m_collidableUserData));

            if ( !polyLine || polyLine->getOwnerActor() == m_actor )
            {
                continue;
            }

            if ( m_stickedLineRef != ITF_INVALID_OBJREF &&
                 m_stickedLineRef == c.m_collidableUserData &&
                 m_stickedEdge == c.m_edgeIndex )
            {
                continue;
            }

            PhysShapeMovingPolyline* movingPoly = polyLine->m_physShape->DynamicCast<PhysShapeMovingPolyline>(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108));

            if ( movingPoly )
            {
                const PolyLineEdge& newEdge = movingPoly->getPolyline()->getEdgeAt(c.m_edgeIndex);

                Vec2d edgePosT, edgeVecT, edgeDirT;
                f32 edgeLengthT;

                AIUtils::getEdgeDataInTime(currentT,polyLine,newEdge,c.m_edgeIndex,edgePosT,edgeVecT,edgeDirT,edgeLengthT);

                if ( prevPos == _pos )
                {
                    if ( IntersectionMath::intersectCircleWithSegment(_pos,radius,edgePosT,edgeDirT,edgeLengthT,c.m_contactPoint,c.m_normal) )
                    {
                        c.m_t = currentT;
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    f32 retT;
                    Vec2d retCirclePos;
                    Vec2d retSegmentPos;
                    Vec2d retNormal;

                    if ( IntersectionMath::intersectMovingCircleWithSegment(edgePosT,edgePosT+edgeVecT,edgeDirT,edgeLengthT,
                        prevPos,_pos,currentMoveDir,currentMoveSize,radius,
                        retT,retCirclePos,retSegmentPos,retNormal) )
                    {
                        c.m_testPosition = retCirclePos;
                        ITF_ASSERT(c.m_testPosition==c.m_testPosition);
                        c.m_contactPoint = retSegmentPos;
                        c.m_t = 0.f;
                        c.m_normal = retNormal;
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            if ( isContactValidToStick(c,1.f,c.m_testPosition,_angle,bfalse,_speed) )
            {
                foundStickingEdge = btrue;
                foundStickingEdgePoly = c.m_collidableUserData;
                foundStickingEdgeEdgeIndex = c.m_edgeIndex;
                foundStickingEdgePos = c.m_testPosition;
            }
            else if ( isContactBlockingObstacle(c,_angle,moveDir,currentT) )
            {
                f32 dist = ( c.m_contactPoint - _prevPos ).sqrnorm();

                if ( blockingContact == U32_INVALID ||
                    blockingContactT > dist )
                {
                    blockingContact = i;
                    blockingContactT = dist;
                }
            }
        }

        if ( blockingContact != U32_INVALID )
        {
            // If we found an obstacle we will want to readjust our position
            processContactObstacle(m_contacts[blockingContact],currentT,_pos,_speed,_blocked);

            moveDir = _speed;
            moveIncr = moveDir*timeDt;
            moveSize = moveDir.norm();
            moveDir.normalize();
        }
        else if ( foundStickingEdgeEdgeIndex != U32_INVALID &&
                ( foundStickingEdgeEdgeIndex != m_stickedEdge || foundStickingEdgePoly != m_stickedLineRef ) )
        {
            ITF_ASSERT(foundStickingEdgePos==foundStickingEdgePos);
            _pos = foundStickingEdgePos;
            ITF_ASSERT(_pos==_pos);
            PolyLine* stickPoly = AIUtils::getPolyLine(foundStickingEdgePoly);

            if ( stickPoly )
            {
                stick(stickPoly,foundStickingEdgeEdgeIndex,_pos,_angle,currentT);

                moveDir = _speed;
                moveIncr = moveDir*timeDt;
                moveSize = moveDir.norm();
                moveDir.normalize();
            }
        }

        if ( currentT == 1.f )
        {
            break;
        }
        else
        {
            currentT = Min(currentT+tIncrement,1.f);

            if ( m_stickedEdge != U32_INVALID )
            {
                PolyLine* currentPolyline;
                const PolyLineEdge* currentEdge;

                AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,currentPolyline,currentEdge);

                if ( currentPolyline && currentEdge )
                {
                    getPosFromRelDist(currentPolyline,currentEdge,m_stickedEdge,m_relDist,currentT,_pos);

                    prevPos = _pos;

                    f32 moveSide = _speed.cross(m_currentGroundDir.getPerpendicular()) > 0.f ? 1.f : -1.f;

                    m_relDist += _speed.norm()*timeDt*moveSide;
                    ITF_ASSERT(m_relDist==m_relDist);

                    // Adjust the reldist in case we go out of the edge
                    adjustCurrentRelDist(currentT,_speed);

                    PolyLine* currentPolyline;
                    const PolyLineEdge* currentEdge;

                    AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,currentPolyline,currentEdge);

                    if ( currentPolyline && currentEdge )
                    {
                        moveDir = _speed;
                        moveIncr = moveDir*timeDt;
                        moveSize = moveDir.norm();
                        moveDir.normalize();

                        getPosFromRelDist(currentPolyline,currentEdge,m_stickedEdge,m_relDist,currentT,_pos);

                        // Check if we detach from the edge because we go too fast or there is no friction
                        if ( checkIfDetachFromCurrentEdgeWithSpeed(moveSide,currentT) )
                        {
                            unstick(btrue);
                        }
                    }
                    else
                    {
                        _pos += moveIncr;
                    }
                }
                else
                {
                    prevPos = _pos;
                    _pos += moveIncr;
                }
            }
            else
            {
                prevPos = _pos;
                _pos += moveIncr;
            }
        }
    }
}

bbool StickToPolylinePhysComponent::processContactsDynamicEnv_resolveBlockingContact( const SCollidableContact& _contact, const Vec2d& _prevPos, Vec2d& _newPos, Vec2d& _newSpeed, f32 _angle, f32 _t, const PhysShapeCircle& _circle )
{
    PhysContactsContainer contacts;
    bbool blocked = bfalse;

    processContactObstacle(_contact,_t,_newPos,_newSpeed,blocked);

    // First check if we are sticked, and the moving edge is pushing us inside the sticked edge. In this case, we don't want to continue
    if ( m_stickedEdge != U32_INVALID )
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,poly,edge);

        if ( edge )
        {
            Vec2d edgeNormal = edge->m_normalizedVector.getPerpendicular();
            Vec2d moveDir = _newPos - _prevPos; moveDir.normalize();
            f32 dotProd = edgeNormal.dot(moveDir);

            if ( dotProd < -0.5f )
            {
                getPosFromRelDist(poly,edge,m_stickedEdge,m_relDist,1.f,_newPos);
                return bfalse;
            }
        }
    }

    contacts.clear();

    Vec2d moveDir = _newPos - _prevPos;

    moveDir.normalize();

    PHYSWORLD->checkEncroachment(_prevPos,_newPos,_angle,&_circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

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

        if ( !poly ||
           ( poly->m_physShape &&
             poly->m_physShape->DynamicCast<PhysShapeMovingPolyline>(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108)) ) )
        {
            continue;
        }

        if ( isContactValidToStick(c,1.f,c.m_testPosition,_angle,btrue,_newSpeed) )
        {
            stick(poly,c.m_edgeIndex,_newPos,_angle);
        }
        else if ( isContactBlockingObstacle(c,_angle,moveDir) )
        {
            processContactObstacle(c,_t,_newPos,_newSpeed,blocked);
        }
    }

    return btrue;
}

void StickToPolylinePhysComponent::processContactsUpdateStickPlatforms()
{
    bbool checkPlatforms = bfalse;

    if ( m_stickedEdge == U32_INVALID )
    {
        checkPlatforms = btrue;
    }
    else if ( m_stickOnWalls && !m_stickOnWallsShiftGravity )
    {
        const PolyLineEdge* currentEdge = getStickedEdge();

        if ( currentEdge )
        {
            EdgeOrientationType orientationType = getEdgeOrientationType(currentEdge->m_normalizedVector);

            if ( orientationType == EDGEORIENTATIONTYPE_WALL )
            {
                checkPlatforms = btrue;
            }
        }
    }

    if ( checkPlatforms )
    {
        bbool goingUp = m_speed.dot(m_currentGravityDir) < -MTH_EPSILON;

        // Remove the platforms we don't collide with anymore
        u32 numMemory = m_memoryNoStickPlatforms.size();
        u32 i = 0;

        while ( i < numMemory )
        {
            const EdgeMemoryData& edgeData = m_memoryNoStickPlatforms[i];

            u32 numContacts = m_contacts.size();
            bbool found = bfalse;

            for ( u32 j = 0; j < numContacts; j++ )
            {
                const SCollidableContact& c = m_contacts[j];

                if ( edgeData.m_polyRef == c.m_collidableUserData &&
                     edgeData.m_edgeIndex == c.m_edgeIndex )
                {
                    found = btrue;
                    break;
                }
            }

            if ( !found )
            {
                m_memoryNoStickPlatforms.eraseNoOrder(i);
                numMemory--;
            }
            else
            {
                i++;
            }
        }

        if ( goingUp )
        {
            // Add the platforms we are colliding with
            u32 numContacts = m_contacts.size();
            PolyLine* poly;
            const PolyLineEdge* edge;

            for ( u32 i = 0; i < numContacts; i++ )
            {
                const SCollidableContact& c = m_contacts[i];

                if ( c.m_edgeIndex == U32_INVALID )
                {
                    continue;
                }

                AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

                if ( !edge )
                {
                    continue;
                }

                const GameMaterial_Template* mat = World::getGameMaterial(edge->getGameMaterial());

                if ( !mat || mat->isSolid() )
                {
                    continue;
                }

                if ( c.m_t == 0.f && goingUp )
                {
                    // Let's see if the platform is already colliding at our feet. If it is, don't memorize it
                    if ( c.m_normal.dot(m_currentGravityDir) >= 0.f )
                    {
                        continue;
                    }
                }

                bbool found = bfalse;

                u32 numMemory = m_memoryNoStickPlatforms.size();

                for ( u32 j = 0; j < numMemory; j++ )
                {
                    const EdgeMemoryData& edgeData = m_memoryNoStickPlatforms[j];

                    if ( edgeData.m_polyRef == c.m_collidableUserData &&
                         edgeData.m_edgeIndex == c.m_edgeIndex )
                    {
                        found = btrue;
                        break;
                    }
                }

                if ( !found )
                {
                    m_memoryNoStickPlatforms.push_back(EdgeMemoryData(c.m_collidableUserData,c.m_edgeIndex,MEMORY_EDGE_TIME));
                }
            }
        }
    }
}

void StickToPolylinePhysComponent::recalculateRelPos( f32 _t, Vec2d& _pos, Vec2d& _speed )
{
    PolyLine* stickedPoly;
    const PolyLineEdge* stickedEdge;

    AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,stickedPoly,stickedEdge);

    Vec2d edgePosT, edgeVecT, edgeDirT;
    f32 edgeLengthT;

    AIUtils::getEdgeDataInTime(_t,stickedPoly,*stickedEdge,m_stickedEdge,edgePosT,edgeVecT,edgeDirT,edgeLengthT);

    static const f32 s_epsilonCheck = 0.05f;
    f32 myRadiusSq = getRadius();

    myRadiusSq *= myRadiusSq;

    Vec2d edgeToPos = _pos - edgePosT;
    f32 distOnEdge = edgeDirT.dot(edgeToPos);

    if ( distOnEdge >= 0.f && distOnEdge <= edgeLengthT )
    {
        // We are still inside the range of the current edge.
        // We need to check if we were pulled outside of it
        Vec2d posOnEdge = edgePosT + (edgeDirT*distOnEdge);
        f32 distFromPosOnEdgeSq = ( _pos - posOnEdge ).sqrnorm();

        if ( (distFromPosOnEdgeSq-myRadiusSq) > s_epsilonCheck )
        {
            // The blocking contact is pulling us out of the edge where we are sticking
            unstick();
        }
        else
        {
            getRelDistFromPos(_pos,_t,m_relDist,m_relAngle);
            ITF_ASSERT(m_relDist==m_relDist);
        }
    }
    else
    {
        // We are out of the range of the current edge
        // This means we can be still on it (on the corner), or on another edge

        bbool outOfEdge = bfalse;
        AIUtils::AdjacentEdgeData adjacentEdgeData;

        AIUtils::getAdjacentEdgeData(getStickedPolyline(),*stickedEdge,m_stickedEdge,m_currentGravityDir,distOnEdge < 0.f,_t,distOnEdge,adjacentEdgeData);

        if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
        {
            // Its a convex edge. Check if our position is inside the normals range. If not, then find in which edge we are
            f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

            if ( dotProd < 1.f )
            {
                f32 angleBetweenNormals = f32_ACos(dotProd);

                // Find out what's our angle between ourselves and the normal from the edge from the left
                if ( angleBetweenNormals )
                {
                    Vec2d myNorm = _pos - adjacentEdgeData.m_edgesCommonPoint;
                    myNorm.normalize();

                    if ( adjacentEdgeData.m_normalA.cross(myNorm) > 0.f )
                    {
                        outOfEdge = btrue;
                    }
                    else
                    {
                        f32 halfAngleBetweenNormals = angleBetweenNormals*0.5f;
                        f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                        if ( distOnEdge < 0.f )
                        {
                            // The blocking contact is putting us out of our current edge
                            if ( angle < (halfAngleBetweenNormals-0.001f) || angle > angleBetweenNormals )
                            {
                                outOfEdge = btrue;
                            }
                        }
                        else
                        {
                            // The blocking contact is putting us out of our current edge
                            if ( angle > (halfAngleBetweenNormals+0.001f) )
                            {
                                outOfEdge = btrue;
                            }
                        }
                    }
                }
                else
                {
                    outOfEdge = btrue;
                }
            }
            else
            {
                outOfEdge = btrue;
            }
        }
        else
        {
            outOfEdge = btrue;
        }

        if ( outOfEdge )
        {
            // We went out of the current edge. We need to find in which edge we are now.
            ObjectRef startPoly = m_stickedLineRef;
            u32 startEdge = m_stickedEdge;
            PolyLine* adjPoly = NULL;
            u32 adjEdge;
            bbool checkLeft = distOnEdge<0.f;
            bbool found = bfalse;

            AIUtils::getAdjacentEdge(stickedPoly,m_stickedEdge,!checkLeft,adjPoly,adjEdge);

            while ( adjPoly && ( adjPoly->getRef() != startPoly || adjEdge != startEdge ) )
            {
                // Check if we are in this other edge
                const PolyLineEdge* edge = &adjPoly->getEdgeAt(adjEdge);

                Vec2d adjEdgePosT, adjEdgeVecT, adjEdgeDirT;
                f32 adjEdgeLengthT;

                AIUtils::getEdgeDataInTime(_t,adjPoly,*edge,adjEdge,adjEdgePosT,adjEdgeVecT,adjEdgeDirT,adjEdgeLengthT);

                Vec2d edgeToPos = _pos - adjEdgePosT;
                f32 distOnEdge = adjEdgeDirT.dot(edgeToPos);

                if ( distOnEdge >= 0.f && distOnEdge <= adjEdgeLengthT )
                {
                    // We are inside the range of this edge.
                    // We need to check if we were pulled outside of it
                    Vec2d posOnEdge = adjEdgePosT + (adjEdgeDirT*distOnEdge);
                    f32 distFromPosOnEdgeSq = ( _pos - posOnEdge ).sqrnorm();

                    if ( (distFromPosOnEdgeSq-myRadiusSq) > s_epsilonCheck )
                    {
                        // The blocking contact is pulling us out of the edge where we are sticking
                        unstick();
                        found = btrue;
                        break;
                    }
                    else
                    {
                        setStickedEdge(adjPoly,adjEdge);
                        getRelDistFromPos(_pos,_t,m_relDist,m_relAngle);
                        ITF_ASSERT(m_relDist==m_relDist);
                        found = btrue;
                        break;
                    }
                }
                else
                {
                    // We are out of the range of the edge, but we can still be inside
                    AIUtils::AdjacentEdgeData adjacentEdgeData;

                    AIUtils::getAdjacentEdgeData(adjPoly,*edge,adjEdge,m_currentGravityDir,distOnEdge < 0.f,_t,distOnEdge,adjacentEdgeData);

                    if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
                    {
                        // Its a convex edge. Check if our position is inside the normals range. If not, then find in which edge we are
                        bbool outOfEdge = bfalse;
                        f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

                        if ( dotProd < 1.f )
                        {
                            f32 angleBetweenNormals = f32_ACos(dotProd);

                            // Find out what's our angle between ourselves and the normal from the edge from the left
                            if ( angleBetweenNormals )
                            {
                                Vec2d myNorm = _pos - adjacentEdgeData.m_edgesCommonPoint;
                                myNorm.normalize();

                                if ( adjacentEdgeData.m_normalA.cross(myNorm) > 0.f )
                                {
                                    outOfEdge = btrue;
                                }
                                else
                                {
                                    f32 halfAngleBetweenNormals = angleBetweenNormals*0.5f;
                                    f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                                    if ( distOnEdge < 0.f )
                                    {
                                        // The blocking contact is putting us out of our current edge
                                        if ( angle < (halfAngleBetweenNormals-0.001f) )
                                        {
                                            outOfEdge = btrue;
                                        }
                                    }
                                    else
                                    {
                                        // The blocking contact is putting us out of our current edge
                                        if ( angle > (halfAngleBetweenNormals+0.001f) )
                                        {
                                            outOfEdge = btrue;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                outOfEdge = btrue;
                            }
                        }
                        else
                        {
                            outOfEdge = btrue;
                        }

                        if ( outOfEdge )
                        {
                            // This edge is no good.
                            // If we changed direction then we stop searching, just unstick
                            if ( checkLeft != (distOnEdge<0.f) )
                            {
                                unstick();
                                found = btrue;
                                break;
                            }

                            PolyLine* newPoly;
                            u32 newEdge;

                            AIUtils::getAdjacentEdge(adjPoly,adjEdge,!checkLeft,newPoly,newEdge);

                            adjPoly = newPoly;
                            adjEdge = newEdge;
                        }
                        else
                        {
                            // We found the right edge. We still have to check if we are being pulled out of it
                            f32 distClamped = Clamp(distOnEdge,0.f,adjEdgeLengthT);
                            Vec2d posOnEdge = adjEdgePosT + (adjEdgeDirT*distClamped);
                            f32 distFromPosOnEdgeSq = ( _pos - posOnEdge ).sqrnorm();

                            if ( (distFromPosOnEdgeSq-myRadiusSq) > s_epsilonCheck )
                            {
                                // The blocking contact is pulling us out of the edge where we are sticking
                                unstick();
                                found = btrue;
                                break;
                            }
                            else
                            {
                                setStickedEdge(adjPoly,adjEdge);
                                getRelDistFromPos(_pos,_t,m_relDist,m_relAngle);
                                ITF_ASSERT(m_relDist==m_relDist);
                                found = btrue;
                                break;
                            }
                        }
                    }
                    else
                    {
                        // It's concave, so we pass onto the next edge
                        PolyLine* newPoly;
                        u32 newEdge;

                        AIUtils::getAdjacentEdge(adjPoly,adjEdge,!checkLeft,newPoly,newEdge);

                        adjPoly = newPoly;
                        adjEdge = newEdge;
                    }
                }
            }

            if ( !found )
            {
                unstick();
            }
        }
        else
        {
            // We are still inside the edge. The last test is just to check if we are being pulled out of the edge or not
            f32 distClamped = Clamp(distOnEdge,0.f,edgeLengthT);
            Vec2d posOnEdge = edgePosT + (edgeDirT*distClamped);
            f32 distFromPosOnEdgeSq = ( _pos - posOnEdge ).sqrnorm();

            if ( (distFromPosOnEdgeSq-myRadiusSq) > s_epsilonCheck )
            {
                // The blocking contact is pulling us out of the edge where we are sticking
                unstick();
            }
            else
            {
                getRelDistFromPos(_pos,_t,m_relDist,m_relAngle);
                ITF_ASSERT(m_relDist==m_relDist);
            }
        }
    }

    if ( m_stickedEdge != U32_INVALID )
    {
        AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,stickedPoly,stickedEdge);

        if ( stickedPoly && stickedEdge )
        {
            AIUtils::AdjacentEdgeData adjacentEdgeData;

            AIUtils::getAdjacentEdgeData(stickedPoly,*stickedEdge,m_stickedEdge,m_currentGravityDir,m_relDist<(stickedEdge->m_length*0.5f),_t,m_relDist,adjacentEdgeData);

            updateEdgeFactors(adjacentEdgeData,stickedPoly,stickedEdge,m_stickedEdge,m_relDist,_t,m_insideEdgeFactor,m_betweenEdgesFactor);
            ITF_ASSERT(m_insideEdgeFactor==m_insideEdgeFactor);
            getPosFromRelDist(stickedPoly,stickedEdge,m_stickedEdge,m_relDist,_t,_pos);
        }
    }
}

void StickToPolylinePhysComponent::processContactObstacle( const SCollidableContact& _contact, f32 _t,
                                                           Vec2d& _pos, Vec2d& _speed, bbool& _blocked )
{
    Vec2d originalSpeed = _speed;
    PolyLine* polyLine = static_cast<PolyLine*>(GETOBJECT(_contact.m_collidableUserData));
    const PolyLineEdge& edge = polyLine->getEdgeAt(_contact.m_edgeIndex);

    if ( m_stickedEdge != U32_INVALID )
    {
        // If we are stick to an edge we must recalculate our relative distance from the result
        // of being pushed by the obstacle
        Vec2d adjustedPos;

        if ( _contact.m_t == 0.f )
        {
            // If t == 0 it means we may be already inside the collision, so we need to make sure we go
            // out of it
            adjustedPos = _contact.m_contactPoint - ( _contact.m_normal * getRadius() );
        }
        else
        {
            // If t != 0 it means we can advance at least t
            adjustedPos = _contact.m_testPosition;
        }

        if ( adjustedPos != _pos )
        {
            _blocked = true;
            _pos = adjustedPos;
            ITF_ASSERT(_pos==_pos);

            recalculateRelPos(_t,_pos,_speed);
            transferSpeedFromBlockingObstacle(_contact,_t,_speed);
        }
    }
    else
    {
        bbool speedSet = bfalse;
        bbool posSet = bfalse;

        _blocked = btrue;

        if ( getTemplate()->getTreatCornerCase() && m_cornerCaseDisableDelay == 0.f )
        {
            // If we were not sticked we check if it's a corner case
            Vec2d vecToPos = _contact.m_testPosition - edge.getPos();
            f32 dist = edge.m_normalizedVector.dot(vecToPos);

            if ( dist < 0.f || dist > edge.m_length )
            {
                AIUtils::AdjacentEdgeData adjacentEdgeData;

                AIUtils::getAdjacentEdgeData(polyLine,edge,_contact.m_edgeIndex,m_currentGravityDir,dist < 0.f,_t,m_relDist,adjacentEdgeData);

                if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
                {
                    f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

                    if ( dotProd < 1.f )
                    {
                        f32 angleBetweenNormals = f32_ACos(dotProd);

                        // Find out what's our angle between ourselves and the normal from the edge from the left
                        if ( angleBetweenNormals )
                        {
                            Vec2d myNorm = _contact.m_testPosition - adjacentEdgeData.m_edgesCommonPoint;
                            myNorm.normalize();

                            f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                            // With the angle we can check if we are within the boundaries of the edge
                            if ( ( dist < 0.f && angle >= (angleBetweenNormals*0.5f) ) ||
                                 ( dist > 0.f && angle <= (angleBetweenNormals*0.5f) ) )
                            {
                                u32 cornerCase = U32_INVALID;

                                if ( isCornerCase(polyLine,_contact.m_edgeIndex,dist,_speed,1.f,cornerCase) &&
                                     cornerCase == 1 )
                                {
                                    _speed = projectSpeedOnDir(edge.m_normalizedVector,_speed);
                                    speedSet = btrue;
                                    posSet = btrue;
                                    
                                    _pos = edge.getPos() + ( edge.m_normalizedVector * dist );
                                    ITF_ASSERT(_pos==_pos);
                                    _pos += edge.m_normalizedVector.getPerpendicular() * getRadius();
                                    ITF_ASSERT(_pos==_pos);
                                 }
                            }
                            else
                            {
                                // It looks like it's a corner case but this is not the edge we want to process
                                // Most probably it later in the contacts array so we wait for it to arrive
                                // By setting speedSet to true we avoid any process hoping that we will get the
                                // right edge later in the loop
                                if ( adjacentEdgeData.m_adjacentEdgeIndex != U32_INVALID )
                                {
                                    speedSet = btrue;
                                    posSet = btrue;
                                    _blocked = bfalse;
                                }
                            }
                        }
                    }
                }
            }
        }
       
        if ( !posSet )
        {
            _pos = _contact.m_contactPoint - ( _contact.m_normal * getRadius() );
            ITF_ASSERT(_pos==_pos);
        }

        if ( !speedSet )
        {
            transferSpeedFromBlockingObstacle(_contact,_t,_speed);
        }
    }

    if ( _blocked )
    {
        notifyBlocked(_contact.m_collidableUserData,polyLine,_contact.m_edgeIndex,originalSpeed,_contact.m_contactPoint);
    }
}

void StickToPolylinePhysComponent::notifyBlocked( ObjectRef _polyRef, PolyLine* _poly, u32 _edgeIndex, const Vec2d& _speed, const Vec2d& _contactP )
{
    EventBlockedByPolyline blockedEvent(_polyRef,_edgeIndex,_speed,_contactP);

    m_actor->onEvent(&blockedEvent);

    if (_poly->m_ownerFrise)
    {
        _poly->m_ownerFrise->onEvent(&blockedEvent);
    }

    if ( _poly->getOwnerActor() )
    {
        EventPolylineBlockingContact blockingContact(_polyRef,_edgeIndex,_speed,_contactP);

        _poly->getOwnerActor()->onEvent(&blockingContact);
    }
}

bbool StickToPolylinePhysComponent::isContactValidToStick( const SCollidableContact& _contact, f32 _t, const Vec2d& _pos, f32 _angle, bbool _dynCollisionTest, const Vec2d& _speed ) const
{
    // If it's not an edge skip it
    if ( _contact.m_edgeIndex == U32_INVALID || m_disableSticking )
    {
        return bfalse;
    }

    if ( !_dynCollisionTest && isEdgeInMemory(_contact.m_collidableUserData,_contact.m_edgeIndex) )
    {
        return bfalse;
    }

    PolyLine* polyLine = AIUtils::getPolyLine(_contact.m_collidableUserData);

    if ( !polyLine || polyLine->m_isCollisionActive == bfalse )
    {
        return bfalse;
    }

    const PolyLineEdge& edge = polyLine->getEdgeAt(_contact.m_edgeIndex);
    const GameMaterial_Template* mat = World::getGameMaterial(edge.getGameMaterial());

    if ( mat && mat->getNoStick() )
    {
        return bfalse;
    }

    if (polyLine->getOwnerActor())
    {
        EventQueryCanStickOrCollide queryCanStick;
        queryCanStick.setSender(m_actor->getRef());
        polyLine->getOwnerActor()->onEvent(&queryCanStick);
        if (!queryCanStick.canStick())
            return bfalse;
    }

    // In case the polyline changed and the edge is not valid anymore
    if ( _contact.m_edgeIndex >= polyLine->getPosCount() )
    {
        return bfalse;
    }

    // If we are not sticked now, and we were already sticked on this edge during this frame, then we skip it
    for ( u32 i = 0; i < m_traveledEdgesThisFrame.size(); i++ )
    {
        if ( m_traveledEdgesThisFrame[i].m_polyRef == _contact.m_collidableUserData &&
             m_traveledEdgesThisFrame[i].m_edgeIndex == _contact.m_edgeIndex &&
           ( m_stickedEdge != _contact.m_edgeIndex ||
             m_stickedLineRef != _contact.m_collidableUserData ) )
        {
            return bfalse;
        }
    }

    EdgeOrientationType orientationType = getEdgeOrientationType(edge.m_normalizedVector);

    if ( orientationType != EDGEORIENTATIONTYPE_GROUND  )
    {
        if ( !m_stickOnWalls )
        {
            return bfalse;
        }
            
        if ( mat && mat->getNoWalkOnWall() )
        {
            return bfalse;
        }

        if ( m_stickedEdge == U32_INVALID )
        {
            if ( orientationType == EDGEORIENTATIONTYPE_ROOF )
            {
                return bfalse;
            }
        }
        else
        {
            const PolyLineEdge* currentEdge = getStickedEdge();

            if ( currentEdge )
            {
                if ( !AIUtils::isWallRunEnabledOnPolyline( &edge, currentEdge ) )
                {
                    return bfalse;
                }

                EdgeOrientationType orientationType = getEdgeOrientationType(currentEdge->m_normalizedVector);

                if ( orientationType == EDGEORIENTATIONTYPE_GROUND )
                {
                    // If we are on the ground and we want to wallrun we have to make sure that we are pushing towards the wall
                    if ( m_stickOnWallsInputDir == Vec2d::Zero )
                    {
                        return bfalse;
                    }
                    else
                    {
                        f32 dotProd = edge.m_normalizedVector.getPerpendicular().dot(m_stickOnWallsInputDir);

                        if ( dotProd >= 0.f )
                        {
                            return bfalse;
                        }
                    }
                }
            }
        }

        f32 currentSpeed = _speed.norm();

        if ( currentSpeed < getTemplate()->getMinSpeedStickToWall() )
        {
            return bfalse;
        }
    }

    Vec2d edgePosT, edgeVecT, edgeDirT;
    f32 edgeLengthT;

    AIUtils::getEdgeDataInTime(_t,polyLine,edge,_contact.m_edgeIndex,edgePosT,edgeVecT,edgeDirT,edgeLengthT);

    Vec2d edgeDir;
    Vec2d pos;

    // If it's the same edge where we are we use our current data (where we are now)
    if ( m_stickedLineRef != _contact.m_collidableUserData ||
         m_stickedEdge != _contact.m_edgeIndex )
    {
        edgeDir = edgeDirT;
        pos = _contact.m_testPosition;
    }
    else
    {
        edgeDir = m_currentGroundDir;
        pos = _pos;
    }

    Vec2d edgeNormal = edgeDir.getPerpendicular();
    bbool wasSticked = m_stickedEdge != U32_INVALID;

    if ( _contact.m_t == 0.f )
    {
        if ( !wasSticked && !_speed.IsEqual(Vec2d::Zero,MTH_EPSILON) &&
           ( ( _contact.m_normal.dot(edgeNormal) > 0.f ||
             edgeNormal.dot(_speed) > MTH_EPSILON ) ) )
        {
            return bfalse;
        }
        else
        {
            f32 radius = getRadius();
            f32 distToContact = ( _contact.m_testPosition - _contact.m_contactPoint ).sqrnorm();

            if ( distToContact >= ((radius*radius)+MTH_EPSILON) )
            {
                return bfalse;
            }
        }
    }

    // Now we need to check if we are inside the range to collide with this edge or not
    Vec2d edgeToPos = pos - edge.getPos();
    f32 dist = edgeDirT.dot(edgeToPos);

    if ( dist < 0.f || dist > edge.m_length )
    {
        // If we are outside, check if we are within range with the next normal
        AIUtils::AdjacentEdgeData adjacentEdgeData;

        AIUtils::getAdjacentEdgeData(polyLine,edge,_contact.m_edgeIndex,m_currentGravityDir,dist < 0.f,_t,m_relDist,adjacentEdgeData);

        if ( adjacentEdgeData.m_crossProdEdges > MTH_EPSILON )
        {
            f32 dotProd = adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB);

            if ( dotProd < 1.f )
            {
                f32 angleBetweenNormals = f32_ACos(dotProd);

                // Find out what's our angle between ourselves and the normal from the edge from the left
                if ( angleBetweenNormals )
                {
                    Vec2d myNorm = pos - adjacentEdgeData.m_edgesCommonPoint;
                    myNorm.normalize();

                    f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                    // With the angle we can check if we are within the boundaries of the edge
                    if ( dist < 0.f )
                    {
                        if ( angle < (angleBetweenNormals*0.5f) )
                        {
                            return bfalse;
                        }
                    }
                    else
                    {
                        if ( angle > (angleBetweenNormals*0.5f) )
                        {
                            return bfalse;
                        }
                    }
                }
            }
            else
            {
                return bfalse;
            }
        }
    }

    u32 memNoStick = m_memoryNoStickPlatforms.size();

    for ( u32 i = 0; i < memNoStick; i++ )
    {
        const EdgeMemoryData& edgeData = m_memoryNoStickPlatforms[i];

        if ( edgeData.m_edgeIndex == _contact.m_edgeIndex &&
             edgeData.m_polyRef == _contact.m_collidableUserData )
        {
            return bfalse;
        }
    }

    if ( m_stickedLineRef != ITF_INVALID_OBJREF )
    {
        PolyLine* currentPoly;
        const PolyLineEdge* currentEdge;

        AIUtils::getPolyLine(m_stickedLineRef,m_stickedEdge,currentPoly,currentEdge);

        if ( currentPoly && currentEdge )
        {
            PolyLine* adjPoly;
            u32 adjEdge;

            AIUtils::getAdjacentEdge(currentPoly,m_stickedEdge,btrue,adjPoly,adjEdge);

            if ( adjEdge == _contact.m_edgeIndex &&
                 adjPoly &&
                 adjPoly->getRef() == _contact.m_collidableUserData )
            {
                // If we are already sticked do not treat the adjacent edge as a valid contact, let the physic
                // travel from one edge to another normally
                return bfalse;
            }

            AIUtils::getAdjacentEdge(currentPoly,m_stickedEdge,bfalse,adjPoly,adjEdge);

            if ( adjEdge == _contact.m_edgeIndex &&
                 adjPoly &&
                 adjPoly->getRef() == _contact.m_collidableUserData )
            {
                // If we are already sticked do not treat the adjacent edge as a valid contact, let the physic
                // travel from one edge to another normally
                return bfalse;
            }

            // Trying to fix two bugs:
                // 1) If the contact is at t = 0, and our speed is going out of this contact, and it's a different polyline, then we skip it
                //    There is a case where we have a moving frieze below and a wall we are using to run up, and we unstick from the wall and stick on the frieze
                //    because we detect that at the begining of the movement we touched it
                //    http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-9335
                // 2) For surfaces for very small edges that can collide with us at the same time. For this case
                //    we want to keep ourselves sticked to the same edge if the edges we collide with are at t = 0 and we are moving away from them
                //    http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-9398
            if ( _contact.m_t == 0.f &&
                 _speed.sqrnorm() > MTH_EPSILON )
            {
                f32 dotProd = edgeNormal.dot(_speed);
                
                if ( dotProd > 0.f )
                {
                    return bfalse;
                }
            }
        }
    }

    return btrue;
}

bbool StickToPolylinePhysComponent::isContactBlockingObstacle( const SCollidableContact& _contact, f32 _angle, const Vec2d& _moveDir, f32 _t ) const
{
    PolyLine* polyLine;
    const PolyLineEdge* edge;

    AIUtils::getPolyLine(_contact.m_collidableUserData,_contact.m_edgeIndex,polyLine,edge);

    if ( !polyLine || !edge )
    {
        return bfalse;
    }

    const GameMaterial_Template* mat = (World::getGameMaterial(edge->getGameMaterial()));

    Vec2d edgePosT, edgeVecT, edgeDirT;
    f32 edgeLengthT;

    AIUtils::getEdgeDataInTime(_t,polyLine,*edge,_contact.m_edgeIndex,edgePosT,edgeVecT,edgeDirT,edgeLengthT);

    return isContactBlockingObstacle(edgePosT,edgeDirT,edgeLengthT,mat,polyLine,edge,_t,_contact,_moveDir);
}

bbool StickToPolylinePhysComponent::isContactBlockingObstacle( const Vec2d& _edgePos0, const Vec2d& _edgeDir, f32 _edgeLength, const class GameMaterial_Template* _gameMaterial,
                                                               PolyLine* _poly, const PolyLineEdge* _edge, f32 _t,
                                                               const SCollidableContact& _contact, const Vec2d& _moveDir ) const
{
 
    if (_poly->getOwnerActor())
    {
        EventQueryCanStickOrCollide queryCanStick;
        queryCanStick.setSender(m_actor->getRef());
        _poly->getOwnerActor()->onEvent(&queryCanStick);
        if (!queryCanStick.canCollide())
            return bfalse;
    }

    // If we are going from one edge to another we cannot consider the next edge as an obstacle
    // This issue was found on RAYMAN_Colas_MULTI_glisse, the character was slowing down when sliding
    // from one edge to another
    PolyLine* currentPoly = getStickedPolyline();

    if ( currentPoly &&
       ( currentPoly == _poly ||
         currentPoly->getNext() == _poly ||
         currentPoly->getPrevious() == _poly ) )
    {
        for ( u32 i = 0; i < m_traveledEdgesThisFrame.size(); i++ )
        {
            if ( _contact.m_edgeIndex == m_traveledEdgesThisFrame[i].m_edgeIndex &&
                 _contact.m_collidableUserData == m_traveledEdgesThisFrame[i].m_polyRef )
            {
                return bfalse;
            }
        }
    }

    u32 memNoStick = m_memoryNoStickPlatforms.size();

    for ( u32 i = 0; i < memNoStick; i++ )
    {
        const EdgeMemoryData& edgeData = m_memoryNoStickPlatforms[i];

        if ( edgeData.m_edgeIndex == _contact.m_edgeIndex &&
            edgeData.m_polyRef == _contact.m_collidableUserData )
        {
            return bfalse;
        }
    }

    if ( !AIUtils::isContactBlockingObstacle(_edgePos0,_edgeDir,_edgeLength,_gameMaterial,_poly,_edge,_t,_contact,
                                             _moveDir,getRadius(),m_stickedLineRef,m_stickedEdge,m_relDist,m_currentGravityDir,
                                             getTemplate()->getTreatCornerCase()) )
    {
        return bfalse;
    }

    return btrue;
}

void StickToPolylinePhysComponent::updateCurrentGravity( const Vec2d& _pos )
{
    m_currentGravityDir = m_currentGravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
    m_currentGravityDir.normalize();
}

f32 StickToPolylinePhysComponent::getAngleWRTGravity()
{
    Vec2d gravity = PHYSWORLD->getGravity(m_actor->getPos().truncateTo2D(),m_actor->getDepth());
    gravity.normalize();
    Vec2d direction = Vec2d::XAxis.Rotate(m_actor->getAngle());
    return gravity.getShortestAngle(direction) - MTH_PIBY2 ;

}

Vec2d StickToPolylinePhysComponent::getSpeedWRTGravity()
{
    Vec2d gravity = PHYSWORLD->getGravity(m_actor->getPos().truncateTo2D(),m_actor->getDepth());
    gravity.normalize();
    f32 angle = getVec2Angle(gravity);
    Vec2d speed = getSpeed().Rotate(-angle);
    return speed;
}

void StickToPolylinePhysComponent::measureSpeedAverage( f32 _deltaTime )
{
    m_worldSpeed = ( m_actor->get2DPos() - m_prevPosition ) / _deltaTime;
    f32 xSpeed = m_currentGravityDir.getPerpendicular().dot(m_worldSpeed);
    f32 ySpeed = (-m_currentGravityDir).dot(m_worldSpeed);

    measureValueAverage(_deltaTime,xSpeed,btrue,m_averageXSpeed,m_averageXSpeedMeasures);
    measureValueAverage(_deltaTime,ySpeed,btrue,m_averageYSpeed,m_averageYSpeedMeasures);
}

void StickToPolylinePhysComponent::resetWorldSpeed()
{
    m_prevPosition = m_actor->get2DPos();
    m_worldSpeed = Vec2d::Zero;
    m_averageXSpeed = 0.f;
    m_averageYSpeed = 0.f;

    for ( u32 i = 0; i < s_speed_average_num_frames; i++ )
    {
        m_averageXSpeedMeasures[i] = 0.f;
        m_averageYSpeedMeasures[i] = 0.f;
    }
}

void StickToPolylinePhysComponent::measureTorqueAverage( f32 _deltaTime )
{
    f32 torque = 0.f;
    f32 currentGroundAngle = calculateCurrentGroundAngle(m_actor->getAngle());

    torque = getShortestAngleDelta(m_prevGroundAngle,currentGroundAngle) / _deltaTime;

    measureValueAverage(_deltaTime,torque,bfalse,m_averageTorque,m_averageTorqueMeasures);
}

void StickToPolylinePhysComponent::resetTorque()
{
    m_rotateWithTorque = bfalse;
    m_averageTorque = 0.f;
    m_prevAngle = m_actor->getAngle();
    m_prevGroundAngle = calculateCurrentGroundAngle(m_prevAngle);

    ITF_MemSet(m_averageTorqueMeasures,0,s_torque_average_num_frames*sizeof(f32));
}

void StickToPolylinePhysComponent::measureValueAverage( f32 _deltaTime, f32 _currentSpeed, bbool resetOnChange, f32& _averageSpeed, f32* _measures )
{
    if ( resetOnChange &&
       ( ( _currentSpeed < 0.f && _averageSpeed > 0.f ) ||
         ( _currentSpeed > 0.f && _averageSpeed < 0.f ) ) )
    {
        _measures[s_speed_average_num_frames-1] = _currentSpeed;

        for ( u32 i = 0; i < s_speed_average_num_frames-1; i++ )
        {
            _measures[i] = 0.f;
        }

        _averageSpeed = _currentSpeed / static_cast<f32>(s_speed_average_num_frames);
    }
    else
    {
        f32 total = 0;

        for ( u32 i = 0; i < s_speed_average_num_frames - 1; i++ )
        {
            _measures[i] = _measures[i+1];
            total += _measures[i];
        }

        _measures[s_speed_average_num_frames-1] = _currentSpeed;
        total += _currentSpeed;

        _averageSpeed = total / static_cast<f32>(s_speed_average_num_frames);
    }
}

void StickToPolylinePhysComponent::checkUnstickFromForces()
{
    // If our friction is going to be too low with this edge we bypass it
    const PolyLineEdge* edge = getStickedEdge();

    if ( edge )
    {
        Vec2d edgeNormInv = -m_currentGroundDir.getPerpendicular();
        Vec2d totalForce = m_force+m_windForcesThisFrame;
        f32 forceProjection = edgeNormInv.dot(totalForce);
        f32 minFriction = getUnstickMinFriction();

        if ( forceProjection < minFriction )
        {
            unstick();
        }
        else if ( m_hasExternalForces && (-forceProjection) > getUnstickMinForceExternal() )
        {
            unstick();
        }
    }
}

// when sticked, get the position on the edge
Vec2d StickToPolylinePhysComponent::getStickedPosition()
{
    if ( const PolyLineEdge* edge = getStickedEdge() )
    {
        f32 dist = Clamp(m_relDist,0.f,edge->m_length);
        return edge->getPos() + ( edge->m_normalizedVector * dist );
    }

    ITF_ASSERT_MSG(0, "can't get the sticked position because I'm not sticked!");
    return Vec2d::Zero;
}

f32 StickToPolylinePhysComponent::calculateCurrentGroundAngle( f32 _actorAngle ) const
{
    if ( m_stickedEdge != U32_INVALID && m_stickedLineRef.isValid() )
    {
        return getVec2Angle(m_currentGroundDir);
    }
    else
    {
        return _actorAngle;
    }
}

StickToPolylinePhysComponent::EdgeOrientationType StickToPolylinePhysComponent::getEdgeOrientationType( const Vec2d& _dir ) const
{
    return (StickToPolylinePhysComponent::EdgeOrientationType)AIUtils::getEdgeOrientationType( _dir, m_currentGravityDir );
}

void StickToPolylinePhysComponent::setSecondaryCollisionCheck( const Vec2d& _pos, f32 _radius )
{
    m_extraCollisionCheckPos = _pos;
    m_extraCollisionCheckRadius = _radius;
}

#ifdef ITF_SUPPORT_EDITOR
void StickToPolylinePhysComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    unstick();
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(StickToPolylinePhysComponent_Template)
BEGIN_SERIALIZATION_CHILD(StickToPolylinePhysComponent_Template)

    SERIALIZE_MEMBER("physFriction",m_friction);
    SERIALIZE_MEMBER("physAirFriction",m_airFriction);
    SERIALIZE_MEMBER("physUnstickMinAngle",m_unstickMinAngle);
    SERIALIZE_MEMBER("physUnstickMaxAngle",m_unstickMaxAngle);
    SERIALIZE_MEMBER("physUnstickMinAngleSpeed",m_unstickMinAngleSpeed);
    SERIALIZE_MEMBER("physUnstickMaxAngleSpeed",m_unstickMaxAngleSpeed);
    SERIALIZE_MEMBER("physUnstickMinFriction",m_unstickMinFriction);
    SERIALIZE_MEMBER("physUnstickMinForceExternal",m_unstickMinForceExternal);
    SERIALIZE_MEMBER("physAngularSpeedMinLinear",m_angularSpeedMinLinear);
    SERIALIZE_MEMBER("physAngularSpeedMaxLinear",m_angularSpeedMaxLinear);
    SERIALIZE_MEMBER("physAngularSpeedMinAngular",m_angularSpeedMinAngular);
    SERIALIZE_MEMBER("physAngularSpeedMaxAngular",m_angularSpeedMaxAngular);
    SERIALIZE_MEMBER("physAngularAirMultiplier",m_angularSpeedAirMultiplier);
    SERIALIZE_MEMBER("physGroundAngularSpeedMultiplierMinAngle",m_groundAngularSpeedMultiplierMinAngle);
    SERIALIZE_MEMBER("physGroundAngularSpeedMultiplierMaxAngle",m_groundAngularSpeedMultiplierMaxAngle);
    SERIALIZE_MEMBER("physGroundAngularSpeedMultiplierMinMult",m_groundAngularSpeedMultiplierMinMult);
    SERIALIZE_MEMBER("physGroundAngularSpeedMultiplierMaxMult",m_groundAngularSpeedMultiplierMaxMult);
    SERIALIZE_MEMBER("physTransferSpeedLossMinAngle",m_transferSpeedLossMinAngle);
    SERIALIZE_MEMBER("physTransferSpeedLossMaxAngle",m_transferSpeedLossMaxAngle);
    SERIALIZE_MEMBER("physTransferSpeedLossMinLoss",m_transferSpeedLossMinLoss);
    SERIALIZE_MEMBER("physTransferSpeedLossMaxLoss",m_transferSpeedLossMaxLoss);
    SERIALIZE_MEMBER("physSpeedLossLandEdgeMinAngle",m_speedLossLandEdgeMinAngle);
    SERIALIZE_MEMBER("physSpeedLossLandEdgeMaxAngle",m_speedLossLandEdgeMaxAngle);
    SERIALIZE_MEMBER("physSpeedLossLandMinimum",m_speedLossLandMin);
    SERIALIZE_MEMBER("physSpeedLossLandMaximum",m_speedLossLandMax);
    SERIALIZE_MEMBER("physTorqueRotationMultiplier",m_torqueRotationMultiplier);
    SERIALIZE_MEMBER("physMinSpeedStickToWall", m_minSpeedStickToWall);    
    SERIALIZE_MEMBER("physTreatCornerCase", m_treatCornerCase);
    SERIALIZE_MEMBER("physCornerCaseDisableDelay", m_cornerCaseDisableDelay);
    SERIALIZE_MEMBER("physTreatMovingPolylines", m_treatMovingPolylines);

END_SERIALIZATION()

StickToPolylinePhysComponent_Template::StickToPolylinePhysComponent_Template()
: m_unstickMinAngle(bfalse,0.f)
, m_unstickMaxAngle(bfalse,0.78f)
, m_unstickMinAngleSpeed(3.f)
, m_unstickMaxAngleSpeed(1.f)
, m_unstickMinFriction(-1.f)
, m_unstickMinForceExternal(50.f)
, m_angularSpeedMinLinear(1.f)
, m_angularSpeedMaxLinear(10.f)
, m_angularSpeedMinAngular(bfalse,2.61f)
, m_angularSpeedMaxAngular(bfalse,4.36f)
, m_angularSpeedAirMultiplier(2.f)
, m_groundAngularSpeedMultiplierMinAngle(bfalse,0.f)
, m_groundAngularSpeedMultiplierMaxAngle(bfalse,1.f)
, m_groundAngularSpeedMultiplierMinMult(1.f)
, m_groundAngularSpeedMultiplierMaxMult(5.f)
, m_transferSpeedLossMinAngle(bfalse,0.1f)
, m_transferSpeedLossMaxAngle(bfalse,0.8f)
, m_transferSpeedLossMinLoss(0.f)
, m_transferSpeedLossMaxLoss(0.1f)
, m_speedLossLandEdgeMinAngle(btrue,45.f)
, m_speedLossLandEdgeMaxAngle(btrue,72.f)
, m_speedLossLandMin(0.5f)
, m_speedLossLandMax(1.f)
, m_friction(2.f)
, m_airFriction(1.f)
, m_torqueRotationMultiplier(0.2f)
, m_minSpeedStickToWall(5.f)
, m_treatCornerCase(btrue)
, m_cornerCaseDisableDelay(0.3f)
, m_treatMovingPolylines(bfalse)
{
}

} // namespace ITF

