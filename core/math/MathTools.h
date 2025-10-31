///////////////////////////////////////////////////////////////////////////////
///////////////////////       set of math utilities (to be cleaned)
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_MATHTOOLS_H_
#define _ITF_MATHTOOLS_H_

#ifndef _ITF_MATHF32_H_
#include "core/math/Mathf32.h"
#endif // _ITF_MATHF32_H_

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_CORETYPES_H_   
#include "core/types.h"
#endif //_ITF_CORETYPES_H_  

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

namespace ITF
{
extern ITF_INLINE void Vec3d_Rotate( Vec3d* _v, f32 _angle)
{
	f32 s;
	f32 c;
    f32_CosSin(_angle,&c,&s);
	f32 X = _v->m_x;
	_v->m_x =  _v->m_x*c - _v->m_y*s;
	_v->m_y =  X*s + _v->m_y*c;
}

ITF_INLINE f32 Vec3d_getAngle( const Vec3d& _vec3d )
{
    Vec2d vec2d = _vec3d.truncateTo2D();
    return vec2d.normalize().getAngle();
}


extern  float   Segment_Segment_Dist (const Vec2d& segment0p0, const Vec2d& segment0p1, const Vec2d& segment1p0, const Vec2d& segment1p1,
                                      Vec2d* closest0 = NULL, Vec2d* closest1 = NULL);

extern  int     SegmentCollision(const Vec2d& _s1p1, const Vec2d& _s1p2, const Vec2d& _s2p1, const Vec2d& _s2p2, float& _correction);

extern  bbool   Segment_Segment(const Vec2d& _pa, const Vec2d& _pb, const Vec2d& _pc, const Vec2d& _pd, Vec2d& _collision, f32* _t = NULL );

extern  bbool   Segment_Segment(Vec2d& _pa, Vec2d& _pb, Vec2d& _pc, Vec2d& _pd, Vec2d& _collision, f32 _epsilon );

extern  bbool   Line_Line( const Vec2d& _pA, const Vec2d& _dirA,
                           const Vec2d& _pB, const Vec2d& _dirB,
                           Vec2d& _intersection );

extern bbool Line_Plane( const Vec3d & _startPos, const Vec3d & _dir, const Vec3d & _planPos, const Vec3d & _planNormale, f32 & _res_dist, Vec3d & _res_collidePos );

extern  bbool   Point_In_Triangle( const Vec2d& _trA, const Vec2d& _trB, const Vec2d& _trC, const Vec2d& _p );

extern int lineCircleIntersection(Vec2d _lineP1, Vec2d _lineP2, Vec2d _cirlceCenter, float _circleRadius,  Vec2d* _col,  Vec2d* _norm);

extern  int     IntersectLineCircle(
    const Vec2d& O,  // Line origin
    const Vec2d& D,  // Line direction
    const Vec2d& C,  // Circle center
    float radius,      // Circle radius
    float *t,        // Parametric values at intersection points
    Vec2d *point,   // Intersection points
    Vec2d *normal); // Normals at intersection points

extern  int     findIntersection(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, int _checkSeg, Vec2d& _result);

extern  int     intersectionLineLine(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, Vec2d& _result);
extern bbool    intersectionLineLineCoeff(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, f32& _coeff);
// Compute the signed distance, result < 0.0f means that "_point" is on the left side of vector "pt1" -> "pt2"
extern f32   PointSignedDistToSegment (const Vec2d& _point, const Vec2d& _segPt1, const Vec2d& _segPt2);

extern bool        isPointInQuad(const Vec2d& _pa, Vec2d* _quad);

ITF_INLINE void truncFloat(float &_val)
{
    #define TRUNCFLOAT_THRESOLD 1000.0f

    _val = floorf(_val*TRUNCFLOAT_THRESOLD)/TRUNCFLOAT_THRESOLD;

    #undef TRUNCFLOAT_THRESOLD
}

extern float getArcLength (float _radius, float _angle);

extern float canonizeAngle(float _angle);
extern float getShortestAngleDelta(float _from, float _to);

// Distinction is done between base types and complex types 
// To return parameters by value or const ref 
template <class T>
ITF_INLINE typename ParamType<T, TypeInfo<T>::isBaseType>::Return
Min(const T & _a, const T & _b)
{
    return (_a<_b) ? _a : _b;
}

// thus allowing specialization for float to use f32_Min (using fsel)
template <>
ITF_INLINE float Min<float>(const float & _a, const float & _b)
{
    return f32_Min(_a, _b);
}

template <class T>
ITF_INLINE typename ParamType<T, TypeInfo<T>::isBaseType>::Return
Max( const T & _a, const T & _b )
{
    return (_a>_b) ? _a : _b;
}

template <>
ITF_INLINE float Max<float>(const float & _a, const float & _b)
{
    return f32_Max(_a, _b);
}

template <class T>
ITF_INLINE void Swap(T &v1, T &v2)
{
	T tmp;
	tmp=v1;
	v1=v2;
	v2=tmp;
}

template <class T>
ITF_INLINE T Clamp( const T& _val, const T& _min, const T& _max )
{
    return Min( Max(_val,_min), _max );
}

template <class T>
ITF_INLINE T Interpolate( const T& _a, const T& _b, f32 _t )
{
    return _a + ( ( _b - _a ) * _t );
}

template <class T>
ITF_INLINE T InverseInterpolate( const T& _a, const T& _b, const T& _value)
{
    return (_value - _a) / (_b - _a);
}

ITF_INLINE void NormalizeAngle2PI( f32& _angle )
{
    while ( _angle < 0.f )
    {
        _angle += MTH_2PI;
    }

    while ( _angle >= MTH_2PI )
    {
        _angle -= MTH_2PI;
    }
}

ITF_INLINE void NormalizeAnglePI( f32& _angle )
{
    while ( _angle < -MTH_PI )
    {
        _angle += MTH_2PI;
    }

    while ( _angle > MTH_PI )
    {
        _angle -= MTH_2PI;
    }
}

ITF_INLINE Vec2d transformPoint( const Vec2d& _pos, f32 _angle, const Vec2d& p )
{
    return p.Rotate(_angle) + _pos;
}

ITF_INLINE f32 getVec2Angle( const Vec2d& _vec )
{
    return f32_ACos(_vec.m_x) * ( Vec2d::Right.cross(_vec) > 0.f ? 1.f : -1.f );
}

ITF_INLINE Vec2d mulTranspose( f32 _angle, const Vec2d& _vec )
{
    Vec2d x = Vec2d::Right.Rotate(_angle);
    Vec2d y = x.getPerpendicular();

    return Vec2d( _vec.dot(x), _vec.dot(y) );
}

ITF_INLINE f32 timeToFrame( f32 _time, f32 _frameTime )
{
    f32 frameF = _time / _frameTime;
    return floorf(frameF + 0.5f);
}

ITF_INLINE f32 frameToTime( f32 _frame, f32 _frameTime )
{
    f32 timeF = _frame * _frameTime;
    return timeF;
}

ITF_INLINE u32 Roundup(u32 x, u32 base)
{
    return ((x) + ((base)-1)) & ~((base)-1);
}

template <class T>
ITF_INLINE T sqr(const T& x)
{
    return x * x;
}


ITF_INLINE Vec2d inverseTransformPos( const Vec2d& _refPos, const f32 _refAngle, const Vec2d& _worldPos )
{
    return (_worldPos - _refPos).Rotate(-_refAngle);
}

ITF_INLINE Vec2d transformPos( const Vec2d& _refPos, const f32 _refAngle, const Vec2d& _localPos )
{
    return _refPos + _localPos.Rotate(_refAngle);
}

ITF_INLINE Vec3d inverseTransformPos( const Vec3d& _refPos, const f32 _refAngle, const Vec3d& _worldPos )
{
    Vec3d localPos = _worldPos - _refPos;
    Vec3d_Rotate(&localPos, -_refAngle);
    return localPos;
}

ITF_INLINE Vec3d transformPos( const Vec3d& _refPos, const f32 _refAngle, const Vec3d& _localPos )
{
    Vec3d worldPos = _localPos;
    Vec3d_Rotate(&worldPos, _refAngle);
    worldPos += _refPos;
    return worldPos;
}



/************
 *  BEZIER  *
 ************/

// 4-points version (cubic)
template <class T>
inline T getBezierPosition( const T& p0, const T& p1, const T& p2, const T& p3, const f32 t )
{
    const f32 tInv = 1.f - t;

    return
          ( p0 * (tInv * tInv * tInv) ) 
        + ( p1 * (3.f * t * tInv * tInv) ) 
        + ( p2 * (3.f * t * t * tInv) ) 
        + ( p3 * (t * t * t) );
}

// 3-points version (quadratic)
template <class T>
inline T getBezierPosition( const T& p0, const T& p1, const T& p2, const f32 t )
{
    const f32 tInv = 1.f - t;

    return
          ( p0 * (tInv * tInv) ) 
        + ( p1 * (2.f * t * tInv) ) 
        + ( p2 * (t * t) );
}

// 4-points version (cubic)
template <class T>
inline T getBezierTangent( const T& p0, const T& p1, const T& p2, const T& p3, f32 t )
{
    f32 tInv = 1.f - t;

    return 3 * (  p0 * ( -tInv * tInv )
                + p1 * ( tInv * (tInv - 2 * t ))
                + p2 * ( t * (2 * tInv - t) )
                + p3 * ( t * t )
               );
}

// 3-points version (quadratic)
template <class T>
inline T getBezierTangent( const T& p0, const T& p1, const T& p2, f32 t )
{
    f32 tInv = 1.f - t;

    return 2 * (  p0 * ( -tInv )
                + p1 * ( 1 - 2 * t )
                + p2 * ( t )
               );
}

template <typename T>
class Bezier3
{
public:
    Bezier3()
    {}
    Bezier3(const T& _p0, const T& _p1, const T& _p2)
        : m_p0(_p0), m_p1(_p1), m_p2(_p2)
    {}

    inline T getPos( const f32 _t ) const { return getBezierPosition(m_p0, m_p1, m_p2, _t); }
    inline T getTangent( const f32 _t ) const { return getBezierTangent(m_p0, m_p1, m_p2, _t); }
    f32 getLength(u32 _numSamples) const;
    void split(const f32 _t, Bezier3<T> & _left, Bezier3<T> & _right) const;

//private:
    T m_p0, m_p1, m_p2;
};

template <typename T> f32 Bezier3<T>::getLength(u32 _numSamples) const
{
    ITF_ASSERT(_numSamples > 0);
    f32 length = 0.0f;
    f32 numSamples = (f32)_numSamples;
    f32 sampleInc = 1 / numSamples;
    f32 t = 0.0f;
    T prev = getPos(0.0f);
    t+= sampleInc;
    for (u32 i = 0; i < _numSamples; ++i)
    {
        T pos = getPos(t);
        length += (pos - prev).norm();
        prev = pos;
        t+= sampleInc;
    }
    return length;
}

template <typename T> void Bezier3<T>::split(const f32 _t, Bezier3<T> & _left, Bezier3<T> & _right) const
{
        const f32 oneMinusT = 1.0f - _t;
        // interpolate from 3 to 2 points
        const T p4 = oneMinusT * m_p0 + _t * m_p1;
        const T p5 = oneMinusT * m_p1 + _t * m_p2;
        // interpolate from 2 points to 1 point
        const T p6 = oneMinusT * p4 + _t * p5;
        // we now have all the values we need to build the subcurves
        _left.m_p0 = m_p0;
        _left.m_p1 = p4;
        _left.m_p2 = p6;

        _right.m_p0 = p6;
        _right.m_p1 = p5;
        _right.m_p2 = m_p2;
}

template <typename T>
class Bezier4
{
public:
    Bezier4()
    {}
    Bezier4(const T& _p0, const T& _p1, const T& _p2, const T& _p3)
        : m_p0(_p0), m_p1(_p1), m_p2(_p2), m_p3(_p3)
    {}

    inline T getPos( const f32 _t ) const { return getBezierPosition(m_p0, m_p1, m_p2, m_p3, _t); }
    inline T getTangent( const f32 _t ) const { return getBezierTangent(m_p0, m_p1, m_p2, m_p3, _t); }
    f32 getLength(u32 _numSamples) const;
    void split( const f32 _t, Bezier4<T> & _left, Bezier4<T> & _right) const;
    void getSegment(const f32 _t1, const f32 _t2, Bezier4<T> & _out) const;
//private:
    T m_p0, m_p1, m_p2, m_p3;
};

template <typename T> f32 Bezier4<T>::getLength(u32 _numSamples) const
{
    ITF_ASSERT(_numSamples > 0);
    f32 length = 0.0f;
    f32 numSamples = (f32)_numSamples;
    f32 sampleInc = 1 / numSamples;
    f32 t = 0.0f;
    T prev = getPos(0.0f);
    t+= sampleInc;
    for (u32 i = 0; i < _numSamples; ++i)
    {
        T pos = getPos(t);
        length += (pos - prev).norm();
        prev = pos;
        t+= sampleInc;
    }
    return length;
}

template <typename T> void Bezier4<T>::split(const f32 _t, Bezier4<T> & _left, Bezier4<T> & _right) const
{
    const f32 oneMinusT = 1.0f - _t;
    // interpolate from 4 to 3 points
    const T p5 = oneMinusT * m_p0 + _t * m_p1;
    const T p6 = oneMinusT * m_p1 + _t * m_p2;
    const T p7 = oneMinusT * m_p2 + _t * m_p3;
    // interpolate from 3 to 2 points
    const T p8 = oneMinusT * p5 + _t * p6;
    const T p9 = oneMinusT * p6 + _t * p7;
    // interpolate from 2 points to 1 point
    const T p10 = oneMinusT * p8 + _t * p9;
    // we now have all the values we need to build the subcurves

    _left.m_p0 = m_p0;
    _left.m_p1 = p5;
    _left.m_p2 = p8;
    _left.m_p3 = p10;

    _right.m_p0 = p10;
    _right.m_p1 = p9;
    _right.m_p2 = p7;
    _right.m_p3 = m_p3;
}

template <typename T> void Bezier4<T>::getSegment(const f32 _t1, const f32 _t2, Bezier4<T> & _out) const
{
    if(_t1==0.0f) 
    {
        Bezier4<T> tmp;
        split(_t2,_out,tmp);
    }
    else if(_t2==1.0f) 
    {
        Bezier4<T> tmp;
        split(_t1,tmp,_out);
    }
    else
    {
        Bezier4<T> tmp1,tmp2;
        split(_t1,tmp1,tmp2);
        const f32 t3 = (_t2-_t1) * 1.0f/(1.0f-_t1);
        tmp2.split(t3,_out,tmp1);
    }

}

} // namespace ITF

#endif // _ITF_MATHTOOLS_H_

