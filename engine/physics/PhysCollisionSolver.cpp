#include "precompiled_engine.h"

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

namespace ITF
{

PhysCollisionSolver::PhysCollisionSolver()
{

}

PhysCollisionSolver::~PhysCollisionSolver()
{

}

bbool PhysCollisionSolver::collide( const PhysSweepInfo& _sweepInfoA,
                                    const PhysSweepInfo& _sweepInfoB,
                                    PhysContactsContainer& _contacts )
{
    if ( _sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)) )
    {
        if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)) )
        {
            return collideCircleCircle(_sweepInfoA,_sweepInfoB,_contacts);
        }
        else if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeMovingPolyline,309826108)) )
        {
            return collideCircleMovingPolyline(_sweepInfoA,_sweepInfoB,_contacts);
        }
        else if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535)) )
        {
            return collideCirclePolyline(_sweepInfoA,_sweepInfoB,_contacts);
        }
        else if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            return collideCirclePolygon(_sweepInfoA,_sweepInfoB,_contacts);
        }
    }
    else if ( _sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535)) )
    {
        if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)) )
        {
            return collideCirclePolyline(_sweepInfoB,_sweepInfoA,_contacts);
        }
        else if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            return collidePolylinePolygon(_sweepInfoA,_sweepInfoB,_contacts);
        }
    }
    else if ( _sweepInfoA.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
    {
        if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
        {
            return collidePolygonPolygon(_sweepInfoA,_sweepInfoB,_contacts);
        }
        else if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)) )
        {
            return collideCirclePolygon(_sweepInfoB,_sweepInfoA,_contacts);
        }
        else if ( _sweepInfoB.m_shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535)) )
        {
            return collidePolylinePolygon(_sweepInfoB,_sweepInfoA,_contacts);
        }
    }

    ITF_ASSERT_MSG(0,"Collision check not supported");

    return bfalse;
}

bbool PhysCollisionSolver::rayCast( const Vec2d& _p0, const Vec2d& _p1,
                                    const PhysShape* _shape, const Vec2d& _pos, f32 _angle,
                                    PhysRayCastContactsContainer& _contacts )
{
    if ( _shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)) )
    {
        return rayCast(_p0,_p1,static_cast<const PhysShapeCircle*>(_shape),_pos,_angle,_contacts);
    }
    else if ( _shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535)) )
    {
        return rayCast(_p0,_p1,static_cast<const PhysShapePolyline*>(_shape),_pos,_angle,_contacts);
    }
    else if ( _shape->IsClassCRC(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
    {
        return rayCast(_p0,_p1,static_cast<const PhysShapePolygon*>(_shape),_pos,_angle,_contacts);
    }

    return bfalse;
}

void PhysCollisionSolver::calculateSweepInfo( const Vec2d& _prevPos, const Vec2d& _pos, f32 _angle,
                                              const PhysShape* _shape, PhysSweepInfo& _ret )
{
    _ret.m_pos = _pos;
    _ret.m_angle = _angle;
    _ret.m_shape = _shape;

    if ( _shape->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168) )
    {
        _ret.m_prevPos = _prevPos;
        _ret.m_movement = _pos - _prevPos;
        _ret.m_moveSize = _ret.m_movement.norm();

        if ( _ret.m_movement != Vec2d::Zero )
        {
            _ret.m_dir = _ret.m_movement;
            _ret.m_dir.normalize();
            _ret.m_move = btrue;
        }
        else
        {
            _ret.m_dir = Vec2d::Zero;
            _ret.m_move = bfalse;
        }
    }
    else
    {
        _ret.m_move = bfalse;
    }
}

}
