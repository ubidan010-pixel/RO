#ifndef _ITF_SPLINE_H_
#define _ITF_SPLINE_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/Vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

namespace ITF
{

    class Blob;

class Spline
{


    DECLARE_SERIALIZE();


public:
    enum    Interpolation : i32
    {
        interp_linear = 0,
        interp_spline = 1,
        interp_bezier = 2,
        interp_constant = 3,
        interp_bezier_standard = 4,
    };

    struct SplinePoint
    {
        DECLARE_SERIALIZE()

        SplinePoint() {m_time=0.f; m_interp=interp_linear;}
        SplinePoint( const Vec3d& _pos, f32 _time, Interpolation _interpolation );
        SplinePoint( const Vec3d& _pos, const Vec3d& _normalIn, const Vec3d& _normalInTime, const Vec3d& _normalOut, const Vec3d& _normalOutTime, f32 _time, Interpolation _interpolation );

        void            serializeToBlob (Blob* _blob);
        void            fillFromBlob (Blob* _blob);
        void            cleanNormals();

        Vec3d           m_vp;
        Vec3d           m_normalIn;
        Vec3d           m_normalInTime;
        Vec3d           m_normalOut;
        Vec3d           m_normalOutTime;

        f32             m_time;
        Interpolation   m_interp;

        u32             m_pointId;
    };

    // Constructors and destructor
    Spline();
    Spline(const Spline&);
    ~Spline();

    // Operations
    void                        AddSplinePoint(const Vec3d& v, const Vec3d& normalIn, const Vec3d& normalInTime, const Vec3d& normalOut, const Vec3d& normalOutTime,  Interpolation _interp, float _t);
    void                        AddSplinePoint(const Vec3d& v, Interpolation _interp, float _t);
    void                        AddSplinePoint(const SplinePoint& _splinePoint, bbool _computeId = btrue);
    void                        setSplinePointAt(const Vec3d& v, Interpolation _interp, float _t, int _index);
    bbool                       GetInterpolatedAtTime(float t, Vec3d& _dest) const;
    int                         GetNumPoints() const;
    const Vec3d&                GetNthPoint(int n) const;
    f32                         getTimeAt(u32 _index) const {return m_points[_index].m_time;}
    ITF_INLINE Interpolation    getInterpAt(u32 _index) const {return m_points[_index].m_interp;} 
	void						getPointsAtTime( f32 t, i32 &_res_FirstPt, i32 &_res_SecPt ) const; 
    void                        sortByTime();
    void                        setTimeAt(f32 _time, u32 _index);

    void                        postLoadForId();

    // Static method for computing the Catmull-Rom parametric equation
    // given a time (t) and a vector quadruple (p1,p2,p3,p4).
    static Interpolation m_defaultInterpolation;
    static Vec3d Eq(float t, const Vec3d& p1, const Vec3d& p2, const Vec3d& p3, const Vec3d& p4);

    void                        serializeToBlob (Blob* _blob);
    void                        fillFromBlob (Blob* _blob);
    
    void                        RemoveSplinePoint(const u32 _index);
    bbool                       ExtractSplinePoint(const u32 _indexBegin, const u32 _indexEnd, const f32 _timeDelta);
    void                        clear() {m_points.clear(); m_pointIdCounter = 0;}
    i32                         getSplinePointIndex(u32 _pointId);

    SplinePoint               & getSplinePointAt(const u32 _index) { return m_points[_index]; }
    void                        computeBezierStandardDefaultValues();

private:
    enum BoundsExceed
    {
        exceed_min,
        exceed_max,
        exceed_ok,
        ENUM_FORCE_SIZE_32(1)
    };

    u32 m_pointIdCounter;

    BoundsExceed   BOUNDS(int& _val, int _max) const;


    SafeArray<SplinePoint, 3> m_points;
    float delta_t;
};

} // namespace ITF

#endif //_ITF_SPLINE_H_
