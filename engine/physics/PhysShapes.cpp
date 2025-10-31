#include "precompiled_engine.h"

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PhysShape)
IMPLEMENT_OBJECT_RTTI(PhysShapeCircle)
IMPLEMENT_OBJECT_RTTI(PhysShapePolygon)
IMPLEMENT_OBJECT_RTTI(PhysShapeBox)
IMPLEMENT_OBJECT_RTTI(PhysShapePolyline)
IMPLEMENT_OBJECT_RTTI(PhysShapeMovingPolyline)

void PhysShape::CopyShape( const PhysShape* _shapeSrc, PhysShape* _shapeDst )
{
    StringID::StringIdValueType classCRC = _shapeSrc->GetObjectClassCRC();

    ITF_ASSERT(classCRC == _shapeDst->GetObjectClassCRC());

    if ( classCRC == _shapeDst->GetObjectClassCRC() )
    {
        if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168) )
        {
            const PhysShapeCircle* circleSrc = static_cast<const PhysShapeCircle*>(_shapeSrc);
            PhysShapeCircle* circleDst = static_cast<PhysShapeCircle*>(_shapeDst);
            circleSrc->copyTo(circleDst);
        }
        else if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112) )
        {
            const PhysShapePolygon* polySrc = static_cast<const PhysShapePolygon*>(_shapeSrc);
            PhysShapePolygon* polyDst = static_cast<PhysShapePolygon*>(_shapeDst);
            polySrc->copyTo(polyDst);
        }
        else if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapeBox,3520962473) )
        {
            const PhysShapeBox* boxSrc = static_cast<const PhysShapeBox*>(_shapeSrc);
            PhysShapeBox* boxDst = static_cast<PhysShapeBox*>(_shapeDst);
            boxSrc->copyTo(boxDst);
        }
        else if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535) )
        {
            // Not supported
            ITF_ASSERT(0);
        }
    }
}

void PhysShape::CopyShapeScaled( const PhysShape* _shapeSrc, PhysShape* _shapeDst, const Vec2d& _scale )
{
    StringID::StringIdValueType classCRC = _shapeSrc->GetObjectClassCRC();

    ITF_ASSERT(classCRC == _shapeDst->GetObjectClassCRC());

    if ( classCRC == _shapeDst->GetObjectClassCRC() )
    {
        if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168) )
        {
            const PhysShapeCircle* circleSrc = static_cast<const PhysShapeCircle*>(_shapeSrc);
            PhysShapeCircle* circleDst = static_cast<PhysShapeCircle*>(_shapeDst);
            circleSrc->copyToScaled(circleDst,_scale);
        }
        else if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112) )
        {
            const PhysShapePolygon* polySrc = static_cast<const PhysShapePolygon*>(_shapeSrc);
            PhysShapePolygon* polyDst = static_cast<PhysShapePolygon*>(_shapeDst);
            polySrc->copyToScaled(polyDst,_scale);
        }
        else if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapeBox,3520962473) )
        {
            const PhysShapeBox* boxSrc = static_cast<const PhysShapeBox*>(_shapeSrc);
            PhysShapeBox* boxDst = static_cast<PhysShapeBox*>(_shapeDst);
            boxSrc->copyToScaled(boxDst,_scale);
        }
        else if ( classCRC == ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535) )
        {
            // Not supported
            ITF_ASSERT(0);
        }
    }
}

BEGIN_SERIALIZATION(PhysShapeCircle)

    SERIALIZE_MEMBER("Radius",m_radius);

END_SERIALIZATION()

void PhysShapeCircle::calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const
{
    _bbox.setMin(Vec2d(_pos.m_x-getRadius(),
                       _pos.m_y-getRadius()));
    _bbox.setMax(_bbox.getMin());

    _bbox.grow(Vec2d(_pos.m_x+getRadius(),
                     _pos.m_y+getRadius()));
    _bbox.grow(Vec2d(_prevPos.m_x-getRadius(),
                     _prevPos.m_y-getRadius()));
    _bbox.grow(Vec2d(_prevPos.m_x+getRadius(),
                     _prevPos.m_y+getRadius()));
}

void PhysShapeCircle::copyTo( PhysShapeCircle* _dst ) const
{
    _dst->m_radius = m_radius;
}

void PhysShapeCircle::copyToScaled( PhysShapeCircle* _dst, const Vec2d& _scale ) const
{
    _dst->m_radius = m_radius * Max(_scale.m_x, _scale.m_y);
}

BEGIN_SERIALIZATION(PhysShapePolygon)

    SERIALIZE_CONTAINER("Points",m_points);
    SERIALIZE_FUNCTION(buildEdges,ESerialize_Data_Load|ESerialize_PropertyEdit_Load);

END_SERIALIZATION()

PhysShapePolygon::PhysShapePolygon( const PointsList& _points )
: m_center(Vec2d::Zero)
, m_flipped(bfalse)
{
    m_points = _points;
    buildEdges();
}

void PhysShapePolygon::setPoints( const Vec2d* _points, u32 _numPoints )
{
    m_points.resize(_numPoints);

    for ( u32 i = 0; i < _numPoints; i++ )
    {
        m_points[i] = _points[i];
    }

    buildEdges();
}

void PhysShapePolygon::Rotate(const f32 _angle)
{
    for(u32 index = 0; index < m_points.size(); ++index)
    {
        m_points[index] = m_points[index].Rotate(_angle);
    }
    buildEdges();
}

void PhysShapePolygon::Translate(const Vec2d &_translation)
{
    for(u32 index = 0; index < m_points.size(); ++index)
    {
        m_points[index] += _translation;
    }
    buildEdges();
}

void PhysShapePolygon::buildEdges()
{
    m_edges.resize(m_points.size());
    m_normals.resize(m_points.size());
    m_distances.resize(m_points.size());

    Vec2d p1, p2;
    
    m_center = Vec2d::Zero;

    for ( size_t i = 0; i < m_points.size(); i++ )
    {
        p1 = m_points[i];

        if ( i + 1 >= m_points.size() )
        {
            p2 = m_points[0];
        }
        else
        {
            p2 = m_points[i + 1];
        }

        m_edges[i] = p2 - p1;
        m_distances[i] = m_edges[i].norm();
        m_edges[i].normalize();
        m_normals[i] = m_edges[i].getPerpendicular();
        
        m_center += m_points[i];
    }

    m_center = m_center / static_cast<f32>(m_points.size());
}

void PhysShapePolygon::setFlipped( bbool _flipped )
{
    if ( m_flipped != _flipped )
    {
        ITF_ASSERT(m_points.size()>0);

        if ( m_points.size() > 0 )
        {
            u32 j = m_points.size() - 1;
            u32 count = m_points.size()/2;

            m_points[0].m_x *= -1.f;

            for ( u32 i = 1; i <= count; i++, j-- )
            {
                Vec2d pA = m_points[i];
                Vec2d pB = m_points[j];

                m_points[i] = pB;
                m_points[i].m_x *= -1.f;

                if ( i != j )
                {
                    m_points[j] = pA;
                    m_points[j].m_x *= -1.f;
                }
            }

            buildEdges();
        }

        m_flipped = _flipped;
    }
}

void PhysShapePolygon::calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const
{
    if ( _pos == _prevPos )
    {
        if ( m_points.size() )
        {
            Vec2d angleCS, pPpos;
            Vec2d::CosSin(&angleCS, _angle);
            Vec2d p = m_points[0].RotateCS(angleCS.m_x, angleCS.m_y);
            Vec2d::Add(&pPpos, &p, &_pos);

            _bbox.setMin(pPpos);
            _bbox.setMax(pPpos);

            for ( size_t i = 1; i < m_points.size(); i++ )
            {
                p = m_points[i].RotateCS(angleCS.m_x, angleCS.m_y);
                _bbox.grow(p + _pos);
            }
        }
        else
        { 
            _bbox.setMin(_pos);
            _bbox.setMax(_pos);
        }
    }
    else
    {
        if ( m_points.size() )
        {
            Vec2d angleCS, pPpos;
            Vec2d::CosSin(&angleCS, _angle);
            Vec2d p = m_points[0].RotateCS(angleCS.m_x, angleCS.m_y);
            Vec2d::Add(&pPpos, &p, &_pos);

            _bbox.setMin(pPpos);
            _bbox.setMax(pPpos);
            _bbox.grow(p+_prevPos);

            for ( size_t i = 1; i < m_points.size(); i++ )
            {
                p = m_points[i].RotateCS(angleCS.m_x, angleCS.m_y);
                _bbox.grow(p + _pos);
                _bbox.grow(p + _prevPos);
            }
        }
        else
        { 
            _bbox.setMin(_pos);
            _bbox.setMax(_pos);
            _bbox.grow(_prevPos);
        }
    }
}

bbool PhysShapePolygon::isPointInside( const Vec2d& _pos ) const
{
    u32 numEdges = m_edges.size();

    for ( u32 i = 0; i < numEdges; i++ )
    {
        const Vec2d& edgeNormal = m_normals[i];
        const Vec2d& p0 = m_points[i];

        Vec2d dirToPos = _pos - p0;

        dirToPos.normalize();

        f32 dotP = edgeNormal.dot(dirToPos);

        if ( dotP > 0.f )
        {
            return bfalse;
        }
    }

    return btrue;
}

void PhysShapePolygon::copyTo( PhysShapePolygon* _dst ) const
{
    _dst->m_points = m_points;
    _dst->m_normals = m_normals;
    _dst->m_edges = m_edges;
    _dst->m_distances = m_distances;
    _dst->m_center = m_center;
    _dst->m_flipped = m_flipped;
}

void PhysShapePolygon::copyToScaled( PhysShapePolygon* _dst, const Vec2d& _scale ) const
{
    u32 numPoints = m_points.size();

    _dst->m_points.resize(numPoints);

    for ( u32 i = 0; i < numPoints; i++ )
    {
        _dst->m_points[i] = m_points[i] * _scale;
    }

    _dst->buildEdges();
}

BEGIN_SERIALIZATION(PhysShapeBox)

    SERIALIZE_MEMBER("Extent",m_extent);
    SERIALIZE_FUNCTION(postLoad,ESerialize_Data_Load|ESerialize_PropertyEdit_Load);

END_SERIALIZATION()

void PhysShapeBox::postLoad()
{
    setExtent(m_extent);
}

PhysShapeBox::PhysShapeBox( const Vec2d& _extent )
{
    setExtent(_extent);
}

void PhysShapeBox::setExtent( const Vec2d& _extent )
{
    if ( m_extent != _extent )
    {
        m_extent = _extent;

        m_points.resize(4);

        m_points[0] = -_extent;
        m_points[1] = Vec2d(-_extent.m_x,_extent.m_y);
        m_points[2] = Vec2d(_extent.m_x,_extent.m_y);
        m_points[3] = Vec2d(_extent.m_x,-_extent.m_y);

        buildEdges();
    }
}

void PhysShapeBox::copyTo( PhysShapeBox* _dst ) const
{
    _dst->m_extent = m_extent;

    Super::copyTo(_dst);
}

void PhysShapeBox::copyToScaled( PhysShapeBox* _dst, const Vec2d& _scale ) const
{
    _dst->setExtent(m_extent*_scale);
}

BEGIN_SERIALIZATION(PhysShapePolyline)
END_SERIALIZATION()

void PhysShapePolyline::calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const
{
    _bbox = getPolyline()->getAABB();
}

BEGIN_SERIALIZATION(PhysShapeMovingPolyline)
END_SERIALIZATION()

PhysShapeMovingPolyline::PhysShapeMovingPolyline( const PolyLine* _polyLine )
{
    setPolyline(_polyLine);
}

void PhysShapeMovingPolyline::setPolyline( const PolyLine* _poly )
{
    Super::setPolyline(_poly);
    m_prevPoly.copyFrom(m_poly);
    m_moved = bfalse;
}

void PhysShapeMovingPolyline::updateMoved()
{
    u32 numP = m_poly->getPosCount();
    u32 numPrev = m_prevPoly.getPosCount();

    m_moved = numP != numPrev;

    if ( !m_moved )
    {
        for ( u32 i = 0; i < numP; i++ )
        {
            if ( m_poly->getPosAt(i) != m_prevPoly.getPosAt(i) )
            {
                m_moved = btrue;
                break;
            }
        }
    }
}

void PhysShapeMovingPolyline::sync()
{
    if ( m_moved )
    {
        m_prevPoly.copyFrom(m_poly);
        m_moved = bfalse;
    }
}

void PhysShapeMovingPolyline::calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const
{
    _bbox = m_poly->getAABB();
    _bbox.grow(m_prevPoly.getAABB());
}

}

