#include "precompiled_engine.h"

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_INTERSECTION_H_
#include "engine/physics/Collision/Intersection.h"
#endif //_ITF_INTERSECTION_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{

bbool PhysCollisionSolver::collideCircleCircle( const PhysSweepInfo& _sweepInfoA,
                                                const PhysSweepInfo& _sweepInfoB,
                                                PhysContactsContainer& _contacts )
{
    ITF_ASSERT(_sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)));
    ITF_ASSERT(_sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)));

    const PhysShapeCircle* circleA = static_cast<const PhysShapeCircle*>(_sweepInfoA.m_shape);
    const PhysShapeCircle* circleB = static_cast<const PhysShapeCircle*>(_sweepInfoB.m_shape);

    if ( !_sweepInfoA.m_move )
    {
        f32 radiusSumSqr = ( circleA->getRadius() + circleB->getRadius() ) *
                           ( circleA->getRadius() + circleB->getRadius() );
        Vec2d aToB = ( _sweepInfoB.m_prevPos - _sweepInfoA.m_prevPos );
        f32 distSq = aToB.sqrnorm();

        if ( distSq <= radiusSumSqr )
        {
            SCollidableContact p;
            f32 colDist = f32_Sqrt(distSq);

            p.m_edgeIndex = U32_INVALID;
            p.m_normal = aToB;
            p.m_normal.normalize();
            p.m_contactPoint = _sweepInfoA.m_prevPos + ( p.m_normal * ( colDist - circleB->getRadius() ) );
            p.m_testPosition = _sweepInfoA.m_prevPos;
            ITF_ASSERT(p.m_testPosition==p.m_testPosition);
            p.m_t = 0.f;

            if ( !_contacts.full() )
            {
                _contacts.push_back( p );
            }

            return btrue;
        }
    }
    else
    {
        Vec2d c0, c1;

        f32 sqrDist = Segment_Segment_Dist(_sweepInfoA.m_prevPos, _sweepInfoA.m_pos,
            _sweepInfoB.m_prevPos, _sweepInfoB.m_pos,
            &c0,&c1);

        f32 radiusSum = circleA->getRadius() + circleB->getRadius();

        if ( sqrDist <= ( (radiusSum*radiusSum) + MTH_EPSILON ) )
        {
            SCollidableContact p;
            Vec2d normal = c1 - c0;
            f32 colDist = normal.norm();

            normal.normalize();

            p.m_edgeIndex = U32_INVALID;
            p.m_contactPoint = c0 + ( normal * ( colDist - circleB->getRadius() ) );
            p.m_normal = normal;
            p.m_testPosition = c0;
            ITF_ASSERT(p.m_testPosition==p.m_testPosition);
            p.m_t = ( c0 - _sweepInfoA.m_prevPos ).norm() / _sweepInfoA.m_moveSize;

            if ( !_contacts.full() )
            {
                _contacts.push_back( p );
            }

            return btrue;
        }
    }

    return bfalse;
}

bbool PhysCollisionSolver::collideCirclePolyline( const PhysSweepInfo& _sweepInfoA,
                                                  const PhysSweepInfo& _sweepInfoB,
                                                  PhysContactsContainer& _contacts )
{
    ITF_ASSERT(_sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)));
    ITF_ASSERT(_sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535)));

    const PhysShapeCircle* circleA = static_cast<const PhysShapeCircle*>(_sweepInfoA.m_shape);
    const PhysShapePolyline* polylineB = static_cast<const PhysShapePolyline*>(_sweepInfoB.m_shape);

    u32 numEdges = polylineB->getPolyline()->getVectorsCount();
    f32 radius = circleA->getRadius()+MTH_EPSILON;
    SCollidableContact p;
    bbool collision = bfalse;
    Vec2d vecToPos;
    Vec2d contactOnEdge;

    if ( !_sweepInfoA.m_move )
    {
        for ( u32 i = 0; i < numEdges && !_contacts.full(); i++ )
        {
            const PolyLineEdge& edge = polylineB->getPolyline()->getEdgeAt(i);

            if ( IntersectionMath::intersectCircleWithSegment(_sweepInfoA.m_pos,radius,edge.getPos(),edge.m_normalizedVector,edge.m_length,p.m_contactPoint,p.m_normal) )
            {
                p.m_testPosition = _sweepInfoA.m_pos;
                ITF_ASSERT(p.m_testPosition==p.m_testPosition);
                p.m_t = 0.f;
                p.m_edgeIndex = i;
                _contacts.push_back(p);
                collision = btrue;
            }
        }

        return collision;
    }

    f32 retT;
    Vec2d retCirclePos;
    Vec2d retSegmentPos;
    Vec2d retNormal;

    for ( u32 i = 0; i < numEdges && !_contacts.full(); i++ )
    {
        const PolyLineEdge& edge = polylineB->getPolyline()->getEdgeAt(i);

        if ( IntersectionMath::intersectMovingCircleWithSegment(edge.getPos(),edge.getPos()+edge.m_vector,edge.m_normalizedVector,edge.m_length,
                                                                _sweepInfoA.m_prevPos,_sweepInfoA.m_pos,_sweepInfoA.m_dir,_sweepInfoA.m_moveSize,radius,
                                                                retT,retCirclePos,retSegmentPos,retNormal) )
        {
            p.m_testPosition = retCirclePos;
            ITF_ASSERT(p.m_testPosition==p.m_testPosition);
            p.m_contactPoint = retSegmentPos;
            p.m_t = retT;
            p.m_normal = retNormal;
            p.m_edgeIndex = i;
            _contacts.push_back(p);
            collision = btrue;
        }
    }

    return collision;
}

bbool PhysCollisionSolver::collideCircleMovingPolyline( const PhysSweepInfo& _sweepInfoA,
                                                        const PhysSweepInfo& _sweepInfoB,
                                                        PhysContactsContainer& _contacts )
{
    ITF_ASSERT(_sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)));
    ITF_ASSERT(_sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108)));

    const PhysShapeCircle* circleA = static_cast<const PhysShapeCircle*>(_sweepInfoA.m_shape);
    const PhysShapeMovingPolyline* movingPolyB = static_cast<const PhysShapeMovingPolyline*>(_sweepInfoB.m_shape);

    if ( !movingPolyB->hasMoved() )
    {
        return collideCirclePolyline(_sweepInfoA,_sweepInfoB,_contacts);
    }

    // The edges have moved. For each edge we check the collision with the volume it had
    const PolyLine* prevPoly = movingPolyB->getPreviousPolyline();
    const PolyLine* newPoly = movingPolyB->getPolyline();

    ITF_ASSERT(prevPoly->getPosCount()==newPoly->getPosCount());

    u32 numEdges = newPoly->getVectorsCount();
    Vec2d prev0, prev1, new0, new1;
    Vec2d tr1A, tr1B, tr1C, tr2A, tr2B, tr2C;
    f32 t = 0.f;
    Vec2d intersect;
    bbool isCrossing;
    SCollidableContact p;
    bbool collision = bfalse;
    f32 radius = circleA->getRadius();
    bbool useOneTriangle;

    for ( u32 i = 0; i < numEdges; i++ )
    {
        const PolyLineEdge& prevEdge = prevPoly->getEdgeAt(i);
        const PolyLineEdge& newEdge = newPoly->getEdgeAt(i);

        prev0 = prevEdge.getPos();
        prev1 = prevPoly->getEdgeAt(i+1).getPos();
        new0 = newEdge.getPos();
        new1 = newPoly->getEdgeAt(i+1).getPos();

        isCrossing = Segment_Segment(prev0,prev1,new0,new1,intersect,&t);

        if ( isCrossing )
        {
            // The edges are crossing, we create the triangles with the intersection
            if ( t == 0.f )
            {
                tr1A = prev0;
                tr1B = new1;
                tr1C = prev1;
                useOneTriangle = btrue;
            }
            else if ( t == 1.f )
            {
                tr1A = prev0;
                tr1B = new1;
                tr1C = new0;
                useOneTriangle = btrue;
            }
            else
            {
                tr1A = prev0;
                tr1B = intersect;
                tr1C = new0;
                tr2A = intersect;
                tr2B = prev1;
                tr2C = new1;
                useOneTriangle = bfalse;
            }
        }
        else
        {
            // The edges don't cross, we create the triangles with the quad
            tr1A = prev0;
            tr1B = prev1;
            tr1C = new0;
            tr2A = prev1;
            tr2B = new1;
            tr2C = new0;
            useOneTriangle = bfalse;
        }

        // Fix the normals to point outside
        {
            Vec2d ab = tr1B - tr1A;
            Vec2d bc = tr1C - tr1B;

            ab.normalize();
            bc.normalize();

            f32 crossP = ab.cross(bc);

            if ( crossP > 0.f )
            {
                // change the order to fix the normals
                Vec2d temp = tr1A;
                tr1A = tr1C;
                tr1C = temp;
            }
        }

        if ( !useOneTriangle )
        {
            Vec2d ab = tr2B - tr2A;
            Vec2d bc = tr2C - tr2B;

            ab.normalize();
            bc.normalize();

            f32 crossP = ab.cross(bc);

            if ( crossP > 0.f )
            {
                // change the order to fix the normals
                Vec2d temp = tr2A;
                tr2A = tr2C;
                tr2C = temp;
            }
        }

        // Intersection of triangle with capsule ( capsule = two circles and a rectangle )
        f32 intersectT;

        if ( IntersectionMath::intersectCapsuleWithTriangle(tr1A,tr1B,tr1C,_sweepInfoA.m_prevPos,_sweepInfoA.m_pos,radius,intersectT) ||
           ( !useOneTriangle && IntersectionMath::intersectCapsuleWithTriangle(tr2A,tr2B,tr2C,_sweepInfoA.m_prevPos,_sweepInfoA.m_pos,radius,intersectT) ) )
        {
            if ( !_contacts.full() )
            {
                Vec2d contact = _sweepInfoA.m_prevPos + ( (_sweepInfoA.m_pos - _sweepInfoA.m_prevPos ) * intersectT );
                Vec2d vecToPos = contact - new0;
                f32 dist = vecToPos.dot(newEdge.m_normalizedVector);

                dist = Clamp(dist,0.f,newEdge.m_length);

                p.m_testPosition = _sweepInfoA.m_prevPos + (_sweepInfoA.m_movement*intersectT);
                ITF_ASSERT(p.m_testPosition==p.m_testPosition);
                p.m_contactPoint = newEdge.getPos() + newEdge.m_normalizedVector * dist;
                p.m_t = intersectT;
                p.m_normal = p.m_contactPoint - p.m_testPosition;
                p.m_edgeIndex = i;
                p.m_normal.normalize();

                _contacts.push_back(p);
            }

            collision = btrue;
            continue;
        }
    }

    return collision;
}

bbool PhysCollisionSolver::collideCirclePolygon( const PhysSweepInfo& _sweepInfoA,
                                                 const PhysSweepInfo& _sweepInfoB,
                                                 PhysContactsContainer& _contacts )
{
    ITF_ASSERT(_sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)));
    ITF_ASSERT(_sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)));

    const PhysShapeCircle* circleA = static_cast<const PhysShapeCircle*>(_sweepInfoA.m_shape);
    const PhysShapePolygon* polygonB = static_cast<const PhysShapePolygon*>(_sweepInfoB.m_shape);

    const PhysShapePolygon::PointsList& polyVertex = polygonB->getVertex();
    const PhysShapePolygon::PointsList& polyEdges = polygonB->getEdges();
    const PhysShapePolygon::FloatList& polyDist = polygonB->getDistances();

    f32 radius = circleA->getRadius()+MTH_EPSILON;
    f32 radiusSqr = radius * radius;

    SCollidableContact p;
    bbool hasContact = bfalse;
    bbool insidePoly = btrue;
    f32 minDistToEdge = F32_INFINITY;

    transform2d xf;

    xf.m_pos = _sweepInfoB.m_pos;
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(_sweepInfoB.m_angle);
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

    for ( size_t i = 0; i < polyEdges.size(); i++ )
    {
        Vec2d vertex = mulTransform2d(xf,polyVertex[i]);
        Vec2d dir = mulMatrix2d(xf.m_rot,polyEdges[i]);
        Vec2d normal = dir.getPerpendicular();
        f32 norm = polyDist[i];

        Vec2d edgeToCenter = _sweepInfoA.m_pos - vertex;
        f32 distToEdge = -normal.dot(edgeToCenter);

        if (distToEdge < minDistToEdge)
        {
            minDistToEdge = distToEdge;

            f32 dist = dir.dot(edgeToCenter);   // distance of the circle center's projection on the edge
            dist = Clamp(dist, 0.f, norm);
            Vec2d proj = vertex + (dir * dist);

            if ( (proj - _sweepInfoA.m_pos).sqrnorm() <= radiusSqr )
            {
                p.m_edgeIndex = U32_INVALID;
                p.m_contactPoint = proj;
                p.m_normal = normal;

                hasContact = btrue;
                insidePoly = bfalse;
            }
            else if (insidePoly)
            {
                if (distToEdge > radius)
                {
                    p.m_edgeIndex = U32_INVALID;
                    p.m_contactPoint = proj;
                    p.m_normal = normal;
                }
                else
                {
                    insidePoly = bfalse;
                }
            }
        }
    }

    if (hasContact)
    {
        p.m_t = 1.f;
        p.m_testPosition = _sweepInfoA.m_pos;
        ITF_ASSERT(p.m_testPosition==p.m_testPosition);
        p.m_normal = p.m_contactPoint - _sweepInfoA.m_pos;
        p.m_normal.normalize();

        if ( !_contacts.full() )
        {
            _contacts.push_back(p);
        }

        return btrue;
    }
    else if (insidePoly)
    {
        p.m_t = 1.f;
        p.m_testPosition = _sweepInfoA.m_pos;
        ITF_ASSERT(p.m_testPosition==p.m_testPosition);
        p.m_normal = -p.m_normal;

        if ( !_contacts.full() )
        {
            _contacts.push_back(p);
        }

        return btrue;
    }
    else
    {
        return bfalse;
    }
}

bbool PhysCollisionSolver::rayCast( const Vec2d& _p0, const Vec2d& _p1,
                                    const PhysShapeCircle* _shape, const Vec2d& _pos, f32 _angle,
                                    PhysRayCastContactsContainer& _contacts )
{
    Vec2d rayVec = _p1 - _p0;
    f32 radius = _shape->getRadius();

    Vec2d rayDir = rayVec; rayDir.normalize();
    Vec2d normal = rayDir.getPerpendicular();
    Vec2d circleToRay = _p0 - _pos;
    f32 distToRay = normal.dot(circleToRay);

    if ( distToRay > radius )
    {
        // They don't intersect
        return bfalse;
    }

    f32 a = rayVec.dot( rayVec ) ;
    f32 b = 2.f*circleToRay.dot( rayVec ) ;
    f32 c = circleToRay.dot( circleToRay ) - (radius*radius) ;

    f32 discriminant = b*b - 4.f * a * c;
    if( discriminant < 0 )
    {
        // No intersection
        return bfalse;
    }

    discriminant = f32_Sqrt( discriminant );

    float t1 = (-b + discriminant)/(2*a);
    float t2 = (-b - discriminant)/(2*a);

    if ( t2 < t1 )
    {
        f32 temp = t1;
        t1 = t2;
        t2 = temp;
    }

    if ( ( t1 < 0.f && t2 < 0.f ) ||
         ( t1 > 1.f && t2 > 1.f ) )
    {
        // The intersection is out of the ray
        return bfalse;
    }

    SRayCastContact newContact;

    newContact.m_collidableUserData = ITF_INVALID_OBJREF;
    newContact.m_collidable = ITF_INVALID_OBJREF;
    newContact.m_edgeIndex0 = U32_INVALID;
    newContact.m_edgeIndex1 = U32_INVALID;
    newContact.m_t0 = Clamp(t1,0.f,1.f);
    newContact.m_t1 = Clamp(t2,0.f,1.f);

    _contacts.push_back(newContact);

    return btrue;
}

}
