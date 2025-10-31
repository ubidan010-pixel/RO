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

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#include <float.h>

namespace ITF
{

struct ClipVertex
{
    Vec2d m_v;
    u32 m_referenceEdge;
    u32 m_incidentEdge;
    u32 m_incidentVertex;
};

static int ClipSegmentToLine( const ClipVertex* vIn, const Vec2d& normal, float offset, ClipVertex* vOut )
{
    // Start with no output points
    int numOut = 0;

    // Calculate the distance of end points to the line
    float distance0 = normal.dot( vIn[0].m_v) - offset;
    float distance1 = normal.dot( vIn[1].m_v) - offset;

    // If the points are behind the plane
    if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
    if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

    // If the points are on different sides of the plane
    if (distance0 * distance1 < 0.0f)
    {
        // Find intersection point of edge and plane
        float interp = distance0 / (distance0 - distance1);
        vOut[numOut].m_v = vIn[0].m_v + ( (vIn[1].m_v - vIn[0].m_v) * interp );
        if (distance0 > 0.0f)
        {
            vOut[numOut].m_referenceEdge = vIn[0].m_referenceEdge;
            vOut[numOut].m_incidentEdge = vIn[0].m_incidentEdge;
            vOut[numOut].m_incidentVertex = vIn[0].m_incidentVertex;
        }
        else
        {
            vOut[numOut].m_referenceEdge = vIn[1].m_referenceEdge;
            vOut[numOut].m_incidentEdge = vIn[1].m_incidentEdge;
            vOut[numOut].m_incidentVertex = vIn[1].m_incidentVertex;
        }
        ++numOut;
    }

    return numOut;
}

// Find the separation between poly1 and poly2 for a give edge normal on poly1.
static float EdgeSeparation(const PhysShapePolygon* poly1, const transform2d& _xf1, size_t edge1,
                            const PhysShapePolygon* poly2, const transform2d& _xf2 )
{
    const PhysShapePolygon::PointsList& polyVertex1 = poly1->getVertex();
    const PhysShapePolygon::PointsList& polyNormals = poly1->getNormals();
    const PhysShapePolygon::PointsList& polyVertex2 = poly2->getVertex();

    ITF_ASSERT(edge1 < polyVertex1.size());

    // Convert normal from poly1's frame into poly2's frame.
    Vec2d normal1World = mulMatrix2d(_xf1.m_rot,polyNormals[edge1]);
    Vec2d normal1 = mulMatrix2dT(_xf2.m_rot,normal1World);

    // Find support vertex on poly2 for -normal.
    size_t index = 0;
    f32 minDot = FLT_MAX;

    for (size_t i = 0; i < polyVertex2.size(); ++i)
    {
        f32 dot = polyVertex2[i].dot( normal1 );

        if (dot < minDot)
        {
            minDot = dot;
            index = i;
        }
    }

    Vec2d v1 = mulTransform2d(_xf1,polyVertex1[edge1]);
    Vec2d v2 = mulTransform2d(_xf2,polyVertex2[index]);

    f32 separation = (v2 - v1).dot( normal1World );

    return separation;
}

// Find the max separation between poly1 and poly2 using edge normals from poly1.
static float FindMaxSeparation( const PhysShapePolygon* poly1, const transform2d& _xf1,
                                const PhysShapePolygon* poly2, const transform2d& _xf2,
                                size_t* edgeIndex )
{
    const PhysShapePolygon::PointsList& polyVertex = poly1->getVertex();
    const PhysShapePolygon::PointsList& polyNormals = poly1->getNormals();

    // Vector pointing from the centroid of poly1 to the centroid of poly2.
    Vec2d d = mulTransform2d(_xf2,poly2->getCenter()) - mulTransform2d(_xf1,poly1->getCenter());
    Vec2d dLocal1 = mulMatrix2dT(_xf1.m_rot, d);

    // Find edge normal on poly1 that has the largest projection onto d.
    size_t edge = 0;
    f32 maxDot = -FLT_MAX;

    for (size_t i = 0; i < polyNormals.size(); ++i)
    {
        float dot = polyNormals[i].dot(dLocal1);

        if (dot > maxDot)
        {
            maxDot = dot;
            edge = i;
        }
    }

    // Get the separation for the edge normal.
    float s = EdgeSeparation(poly1, _xf1, edge, poly2, _xf2);
    if (s > 0.0f)
    {
        return s;
    }

    // Check the separation for the previous edge normal.
    size_t prevEdge = static_cast<int>(edge) - 1 >= 0 ? edge - 1 : polyVertex.size() - 1;
    float sPrev = EdgeSeparation(poly1, _xf1, prevEdge, poly2, _xf2);

    if (sPrev > 0.0f)
    {
        return sPrev;
    }

    // Check the separation for the next edge normal.
    size_t nextEdge = edge + 1 < polyVertex.size() ? edge + 1 : 0;
    float sNext = EdgeSeparation(poly1, _xf1, nextEdge, poly2, _xf2);

    if (sNext > 0.0f)
    {
        return sNext;
    }

    // Find the best edge and the search direction.
    size_t bestEdge;
    float bestSeparation;
    int increment;

    if (sPrev > s && sPrev > sNext)
    {
        increment = -1;
        bestEdge = prevEdge;
        bestSeparation = sPrev;
    }
    else if (sNext > s)
    {
        increment = 1;
        bestEdge = nextEdge;
        bestSeparation = sNext;
    }
    else
    {
        *edgeIndex = edge;
        return s;
    }

    // Perform a local search for the best edge normal.
    for ( ; ; )
    {
        if (increment == -1)
            edge = static_cast<int>(bestEdge) - 1 >= 0 ? bestEdge - 1 : polyVertex.size() - 1;
        else
            edge = bestEdge + 1 < polyVertex.size() ? bestEdge + 1 : 0;

        s = EdgeSeparation(poly1, _xf1, edge, poly2, _xf2);
        if (s > 0.0f)
        {
            return s;
        }

        if (s > bestSeparation)
        {
            bestEdge = edge;
            bestSeparation = s;
        }
        else
        {
            break;
        }
    }

    *edgeIndex = bestEdge;
    return bestSeparation;
}

static void FindIncidentEdge(ClipVertex* c,
                             const PhysShapePolygon* poly1, const transform2d& _xf1, size_t edge1,
                             const PhysShapePolygon* poly2, const transform2d& _xf2 )
{
    const PhysShapePolygon::PointsList& polyNormals1 = poly1->getNormals();
    const PhysShapePolygon::PointsList& polyVertex2 = poly2->getVertex();
    const PhysShapePolygon::PointsList& polyNormals2 = poly2->getNormals();

    ITF_ASSERT(edge1 < poly1->getVertex().size());

    // Get the normal of the reference edge in poly2's frame.
    Vec2d normal1 = mulMatrix2dT(_xf2.m_rot,mulMatrix2d(_xf1.m_rot,polyNormals1[edge1]) );

    // Find the incident edge on poly2.
    size_t index = 0;
    float minDot = FLT_MAX;

    for (size_t i = 0; i < polyNormals2.size(); ++i)
    {
        float dot = normal1.dot(polyNormals2[i]);

        if (dot < minDot)
        {
            minDot = dot;
            index = i;
        }
    }

    // Build the clip vertices for the incident edge.
    size_t i1 = index;
    size_t i2 = i1 + 1 < polyVertex2.size() ? i1 + 1 : 0;

    c[0].m_v = mulTransform2d(_xf2, polyVertex2[i1] );
    c[0].m_referenceEdge = u32(edge1);
    c[0].m_incidentEdge = u32(i1);
    c[0].m_incidentVertex = 0;

    c[1].m_v = mulTransform2d( _xf2, polyVertex2[i2] );
    c[1].m_referenceEdge = u32(edge1);
    c[1].m_incidentEdge = u32(i2);
    c[1].m_incidentVertex = 1;
}

// Polygon - Polygon
// Theorem of the separating axis
// from Box2d
bbool PhysCollisionSolver::collidePolygonPolygon( const PhysSweepInfo& _sweepInfoA,
                                                  const PhysSweepInfo& _sweepInfoB,
                                                  PhysContactsContainer& _contacts )
{
    ITF_ASSERT(_sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)));
    ITF_ASSERT(_sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)));

    const PhysShapePolygon* polyA = static_cast<const PhysShapePolygon*>(_sweepInfoA.m_shape);
    const PhysShapePolygon* polyB = static_cast<const PhysShapePolygon*>(_sweepInfoB.m_shape);

    size_t edgeA = 0;
    transform2d xfA, xfB;

    xfA.m_pos = _sweepInfoA.m_pos;
    xfA.m_rot.m_col1 = Vec2d::Right.Rotate(_sweepInfoA.m_angle);
    xfA.m_rot.m_col2 = xfA.m_rot.m_col1.getPerpendicular();

    xfB.m_pos = _sweepInfoB.m_pos;
    xfB.m_rot.m_col1 = Vec2d::Right.Rotate(_sweepInfoB.m_angle);
    xfB.m_rot.m_col2 = xfB.m_rot.m_col1.getPerpendicular();

    float separationA = FindMaxSeparation(polyA, xfA, polyB, xfB, &edgeA);

    if (separationA > 0.0f)
        return bfalse;

    size_t edgeB = 0;
    float separationB = FindMaxSeparation(polyB, xfB, polyA, xfA, &edgeB);

    if (separationB > 0.0f)
        return bfalse;

    const PhysShapePolygon* poly1;	// reference poly
    const PhysShapePolygon* poly2;	// incident poly
    size_t edge1;		// reference edge
    unsigned char flip;
    static const float k_relativeTol = 0.98f;
    static const float k_absoluteTol = 0.001f;

    transform2d xf1, xf2;

    if (separationB > k_relativeTol * separationA + k_absoluteTol)
    {
        poly1 = polyB;
        poly2 = polyA;

        xf1 = xfB;
        xf2 = xfA;

        edge1 = edgeB;
        flip = 1;
    }
    else
    {
        poly1 = polyA;
        poly2 = polyB;

        xf1 = xfA;
        xf2 = xfB;

        edge1 = edgeA;
        flip = 0;
    }

    ClipVertex incidentEdge[2];

    FindIncidentEdge(incidentEdge, poly1, xf1, edge1, poly2, xf2);

    const PhysShapePolygon::PointsList& polyVertex1 = poly1->getVertex();

    Vec2d v11 = polyVertex1[edge1];
    Vec2d v12 = edge1 + 1 < polyVertex1.size() ? polyVertex1[edge1+1] : polyVertex1[0];

    Vec2d dv = v12 - v11;
    Vec2d sideNormal = mulMatrix2d(xf1.m_rot, dv);
    sideNormal.normalize();
    Vec2d frontNormal = sideNormal.getPerpendicular();

    v11 = mulTransform2d( xf1, v11 );
    v12 = mulTransform2d( xf1, v12 );

    float frontOffset = frontNormal.dot(v11);
    float sideOffset1 = -(sideNormal.dot(v11));
    float sideOffset2 = sideNormal.dot(v12);

    // Clip incident edge against extruded edge1 side edges.
    ClipVertex clipPoints1[2];
    ClipVertex clipPoints2[2];
    int np;

    // Clip to box side 1
    np = ClipSegmentToLine(incidentEdge, -sideNormal, sideOffset1, clipPoints1);

    if (np < 2)
        return bfalse;

    // Clip to negative box side 1
    np = ClipSegmentToLine(clipPoints1,  sideNormal, sideOffset2, clipPoints2);

    if (np < 2)
        return bfalse;

    SCollidableContact contact;
    bbool collision = bfalse;

    contact.m_normal = flip ? -frontNormal : frontNormal;

    for (int i = 0; i < 2; ++i)
    {
        float separation = frontNormal.dot(clipPoints2[i].m_v) - frontOffset;

        if (separation <= 0.0f)
        {
            contact.m_contactPoint = clipPoints2[i].m_v;
            contact.m_t = 1.f;
            contact.m_testPosition = _sweepInfoA.m_pos;
            ITF_ASSERT(contact.m_testPosition==contact.m_testPosition);
            contact.m_edgeIndex = U32_INVALID;

            if ( !_contacts.full() )
            {
                _contacts.push_back(contact);
            }

            collision = btrue;
        }
    }

    return collision;
}

bbool PhysCollisionSolver::rayCast( const Vec2d& _p0, const Vec2d& _p1,
                                    const PhysShapePolygon* _shape, const Vec2d& _pos, f32 _angle,
                                    PhysRayCastContactsContainer& _contacts )
{
    const PhysShapePolygon::PointsList& polyNormals = _shape->getNormals();
    const PhysShapePolygon::PointsList& polyVertex = _shape->getVertex();

    if ( polyVertex.size() <= 1 )
    {
        return bfalse;
    }

    Vec2d p0Local = ( _p0 - _pos ).Rotate(-_angle);
    Vec2d p1Local = ( _p1 - _pos ).Rotate(-_angle);
    bbool intersect = bfalse;
    bbool pointIsInside = bfalse;
    Vec2d rayDir = p1Local - p0Local;
    Vec2d intersectP;
    Vec2d pA, pB;
    f32 t;
    f32 t0 = 0.f;
    f32 t1 = 0.f;

    rayDir.normalize();

    for ( u32 i = 0; i < polyVertex.size()-1; i++ )
    {
        pA = polyVertex[i];
        pB = polyVertex[i+1];

        if ( Segment_Segment(p0Local,p1Local,pA,pB,intersectP,&t) )
        {
            if ( !intersect || t < t0 || t > t1 )
            {
                bbool checkInside = bfalse;

                if ( !intersect )
                {
                    t0 = t1 = t;
                    checkInside = btrue;
                }
                
                if ( t < t0 )
                {
                    t0 = t;
                    checkInside = btrue;
                }
                else if ( t > t1 )
                {
                    t1 = t;
                }

                if ( checkInside )
                {
                    // entry or exit?
                    if ( rayDir.dot(polyNormals[i]) >= 0.f )
                    {
                        pointIsInside = btrue;
                    }
                    else
                    {
                        pointIsInside = bfalse;
                    }
                }
                
                intersect = btrue;
            }
        }
    }

    u32 lastIndex = polyVertex.size()-1;
    pA = polyVertex[lastIndex];
    pB = polyVertex[0];

    if ( Segment_Segment(p0Local,p1Local,pA,pB,intersectP,&t) )
    {
        // entry or exit?
        intersect = btrue;

        if ( !intersect || t < t0 || t > t1 )
        {
            bbool checkInside = bfalse;

            if ( !intersect )
            {
                t0 = t1 = t;
                checkInside = btrue;
            }
            else if ( t < t0 )
            {
                t0 = t;
                checkInside = btrue;
            }
            else if ( t > t1 )
            {
                t1 = t;
            }

            if ( checkInside )
            {
                // entry or exit?
                if ( rayDir.dot(polyNormals[lastIndex]) >= 0.f )
                {
                    pointIsInside = btrue;
                }
                else
                {
                    pointIsInside = bfalse;
                }
            }

            intersect = btrue;
        }
    }

    if ( intersect )
    {
        if ( pointIsInside )
        {
            t0 = 0.f;
        }
        else if ( t0 == t1 )
        {
            t1 = 1.f;
        }

        SRayCastContact newContact;

        newContact.m_collidableUserData = ITF_INVALID_OBJREF;
        newContact.m_collidable = ITF_INVALID_OBJREF;
        newContact.m_edgeIndex0 = U32_INVALID;
        newContact.m_edgeIndex1 = U32_INVALID;
        newContact.m_t0 = t0;
        newContact.m_t1 = t1;

        _contacts.push_back(newContact);

        return btrue;
    }
    else
    {
        return bfalse;
    }
}

}
