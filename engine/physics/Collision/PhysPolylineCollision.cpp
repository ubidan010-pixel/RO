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

namespace ITF
{

bbool PhysCollisionSolver::collidePolylinePolygon( const PhysSweepInfo& _sweepInfoA,
                                                   const PhysSweepInfo& _sweepInfoB,
                                                   PhysContactsContainer& _contacts )
{
    ITF_ASSERT(_sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535)));
    ITF_ASSERT(_sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)));

    const PhysShapePolyline* polyA = static_cast<const PhysShapePolyline*>(_sweepInfoA.m_shape);
    const PhysShapePolygon* polyB = static_cast<const PhysShapePolygon*>(_sweepInfoB.m_shape);

    const PhysShapePolygon::PointsList& polyVertex = polyB->getVertex();
    const PhysShapePolygon::PointsList& polyEdges = polyB->getEdges();
    const PolyLine* polyline = polyA->getPolyline();
    u32 numEdgesPolyline = polyline->getVectorsCount();
    SCollidableContact p;
    bbool collision = bfalse;

    transform2d xf;

    xf.m_pos = _sweepInfoB.m_pos;
    xf.m_rot.m_col1 = Vec2d::Right.Rotate(_sweepInfoB.m_angle);
    xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

    for ( u32 j = 0; j < polyEdges.size(); j++ )
    {
        Vec2d intersection;
        f32 t;
        Vec2d vertex0 = mulTransform2d(xf,polyVertex[j]);
        Vec2d vertex1;

        if ( j == polyEdges.size() - 1 )
        {
            vertex1 = mulTransform2d(xf,polyVertex[0]);
        }
        else
        {
            vertex1 = mulTransform2d(xf,polyVertex[j+1]);
        }

        for ( u32 i = 0; i < numEdgesPolyline; i++ )
        {
            const PolyLineEdge& edge = polyline->getEdgeAt(i);

            if ( Segment_Segment(edge.getPos(),edge.getPos()+edge.m_vector,vertex0,vertex1,intersection,&t) )
            {
                Vec2d edgeNormal = edge.m_normalizedVector.getPerpendicular();

                p.m_normal = -edgeNormal;
                p.m_testPosition = _sweepInfoB.m_pos;
                ITF_ASSERT(p.m_testPosition==p.m_testPosition);
                p.m_edgeIndex = i;
                p.m_contactPoint = intersection;
                p.m_t = 1.f;

                if ( !_contacts.full() )
                {
                    _contacts.push_back(p);
                }

                collision = btrue;
            }
        }
    }

    return collision;
}

bbool PhysCollisionSolver::rayCast( const Vec2d& _p0, const Vec2d& _p1,
                                    const PhysShapePolyline* _shape, const Vec2d& _pos, f32 _angle,
                                    PhysRayCastContactsContainer& _contacts )
{
    const PolyLine* poly = _shape->getPolyline();
    u32 numEdges = poly->getVectorsCount();

    if ( numEdges == 0 )
    {
        return bfalse;
    }

    bbool intersect = bfalse;
    Vec2d intersectP;
    Vec2d pA, pB;
    f32 t;
    f32 t0 = 0.f;
    f32 t1 = 0.f;
    u32 edge0 = U32_INVALID;
    u32 edge1 = U32_INVALID;

    for ( u32 i = 0; i < numEdges; i++ )
    {
        const PolyLineEdge& edge = poly->getEdgeAt(i);
        pA = edge.getPos();
        pB = pA + edge.m_vector;

        if ( Segment_Segment(_p0,_p1,pA,pB,intersectP,&t) )
        {
            if ( !intersect )
            {
                t0 = t1 = t;
                edge0 = i;
                edge1 = i;
                intersect = btrue;
            }
            else if ( t < t0 )
            {
                t0 = t;
                edge0 = i;
            }
            else if ( t > t1 )
            {
                t1 = t;
                edge1 = i;
            }
        }
    }

    if ( intersect )
    {
        SRayCastContact newContact;

        newContact.m_collidableUserData = ITF_INVALID_OBJREF;
        newContact.m_collidable = ITF_INVALID_OBJREF;
        newContact.m_edgeIndex0 = edge0;
        newContact.m_edgeIndex1 = edge1;
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

