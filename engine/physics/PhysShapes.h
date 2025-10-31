
#ifndef _ITF_PHYSSHAPES_H_
#define _ITF_PHYSSHAPES_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_


namespace ITF
{

class PhysShape : public IRTTIObject
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(PhysShape, IRTTIObject,2593069837);
    DECLARE_SERIALIZE_INTERFACE()

public:

    PhysShape() {}
    virtual ~PhysShape() {}
    
    virtual void calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const = 0;
    
    static void CopyShape( const PhysShape* _shapeSrc, PhysShape* _shapeDst );
    static void CopyShapeScaled( const PhysShape* _shapeSrc, PhysShape* _shapeDst, const Vec2d& _scale );

    virtual bbool       isValid() const = 0;

private:

};

class PhysShapeCircle : public PhysShape
{
    DECLARE_OBJECT_CHILD_RTTI(PhysShapeCircle,PhysShape,3922519168);
    DECLARE_SERIALIZE()

public:

    PhysShapeCircle() : m_radius(0.f) {}
    PhysShapeCircle( f32 _radius ) : m_radius(_radius) {}
    virtual ~PhysShapeCircle() {}

    virtual void        calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const;

    ITF_INLINE void     setRadius( f32 _radius ) { m_radius = _radius; }
    ITF_INLINE f32      getRadius() const { return m_radius; }

    void                copyTo( PhysShapeCircle* _dst ) const;
    void                copyToScaled( PhysShapeCircle* _dst, const Vec2d& _scale ) const;

    virtual bbool       isValid() const { return m_radius > 0; }

private:

    f32                 m_radius;
};

class PhysShapePolygon : public PhysShape
{
    DECLARE_OBJECT_CHILD_RTTI(PhysShapePolygon,PhysShape,3261252112);
    DECLARE_SERIALIZE()

public:

    typedef SafeArray <Vec2d> PointsList;
    typedef SafeArray <f32> FloatList;

    PhysShapePolygon() : m_flipped(bfalse) {}
    PhysShapePolygon( const PointsList& _points );
    virtual ~PhysShapePolygon() {}

    virtual void        calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const;

    void                clear() { m_points.clear(); m_normals.clear(); m_edges.clear(); m_distances.clear(); }
    void                setPoints( const PointsList& _points ) { m_points = _points; m_flipped = bfalse; buildEdges(); }
    template <class T,u32 SIZE>
    void                setPoints( const FixedArray <T,SIZE>& _points );
    void                setPoints( const Vec2d* _points, u32 _numPoints );

    void                Rotate(const f32 _angle);
    void                Translate(const Vec2d & _translation);

    const PointsList&   getVertex() const { return m_points; }
    const PointsList&   getNormals() const { return m_normals; }
    const PointsList&   getEdges() const { return m_edges; }
    const FloatList&    getDistances() const { return m_distances; }
    const Vec2d&        getCenter() const { return m_center; }
    void                setFlipped( bbool _flipped );

    void                copyTo( PhysShapePolygon* _dst ) const;
    void                copyToScaled( PhysShapePolygon* _dst, const Vec2d& _scale ) const;

    bbool               isPointInside( const Vec2d& _pos ) const;

    virtual bbool       isValid() const { return m_points.size() > 1; }

protected:

    void                buildEdges();

    PointsList  m_points;
    PointsList  m_normals;
    PointsList  m_edges;
    FloatList   m_distances;
    Vec2d       m_center;
    bbool       m_flipped;
};

template <class T,u32 SIZE>
void PhysShapePolygon::setPoints( const FixedArray <T,SIZE>& _points )
{
    m_points.resize(_points.size());

    for ( u32 i = 0; i < _points.size(); i++ )
    {
        m_points[i] = _points[i];
    }

    m_flipped = bfalse;
    buildEdges();
}

class PhysShapeBox : public PhysShapePolygon
{
    DECLARE_OBJECT_CHILD_RTTI(PhysShapeBox,PhysShapePolygon,3520962473);
    DECLARE_SERIALIZE()

public:

    PhysShapeBox() : m_extent(Vec2d::Zero) {}
    PhysShapeBox( const Vec2d& _extent );
    virtual ~PhysShapeBox() {}

    void                setExtent( const Vec2d& _extent );
    const Vec2d&        getExtent() const { return m_extent; }

    void                copyTo( PhysShapeBox* _dst ) const;
    void                copyToScaled( PhysShapeBox* _dst, const Vec2d& _scale ) const;

private:

    void                postLoad();

    Vec2d               m_extent;
};

class PhysShapePolyline : public PhysShape
{
    DECLARE_OBJECT_CHILD_RTTI(PhysShapePolyline,PhysShape,4121355535);
    DECLARE_SERIALIZE()

public:

    PhysShapePolyline() : Super(), m_poly(NULL) {}
    PhysShapePolyline( const class PolyLine* _polyLine ) : m_poly(_polyLine) {}
    virtual ~PhysShapePolyline() {}

    virtual void            calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const;
    virtual void            setPolyline( const class PolyLine* _poly ) { m_poly = _poly; }
    const class PolyLine*   getPolyline() const { return m_poly; }
    virtual bbool           isValid() const { return m_poly != NULL; }

protected:

    const class PolyLine*   m_poly;
};

class PhysShapeMovingPolyline : public PhysShapePolyline
{
    DECLARE_OBJECT_CHILD_RTTI(PhysShapeMovingPolyline,PhysShapePolyline,309826108);
    DECLARE_SERIALIZE()

public:

    PhysShapeMovingPolyline() : m_moved(bfalse) {}
    PhysShapeMovingPolyline( const class PolyLine* _polyLine );
    virtual ~PhysShapeMovingPolyline() {}

    virtual void            calculateAABB( const Vec2d& _pos, const Vec2d& _prevPos, f32 _angle, AABB& _bbox ) const;
    virtual void            setPolyline( const class PolyLine* _poly );
    void                    updateMoved();
    void                    sync();
    bbool                   hasMoved() const { return m_moved; }
    const class PolyLine*   getPreviousPolyline() const { return &m_prevPoly; }

private:

    PolyLine                m_prevPoly;
    bbool                   m_moved;
};

}

#endif // _ITF_PHYSSHAPES_H_

