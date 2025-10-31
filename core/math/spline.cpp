#include "precompiled_core.h"

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

namespace ITF
{

BEGIN_SERIALIZATION_SUBCLASS(Spline,SplinePoint)

    SERIALIZE_MEMBER("Point",m_vp);
    SERIALIZE_MEMBER("Time", m_time);
    SERIALIZE_MEMBER("NormalIn", m_normalIn);
    SERIALIZE_MEMBER("NormalInTime", m_normalInTime);
    SERIALIZE_MEMBER("NormalOut", m_normalOut);
    SERIALIZE_MEMBER("NormalOutTime", m_normalOutTime);
    
    SERIALIZE_ENUM_BEGIN("Interpolation",m_interp);
        SERIALIZE_ENUM_VAR(interp_linear);
        SERIALIZE_ENUM_VAR(interp_spline);
    SERIALIZE_ENUM_END();

END_SERIALIZATION()


Spline::Interpolation Spline::m_defaultInterpolation = Spline::interp_linear;

Spline::SplinePoint::SplinePoint( const Vec3d& _pos, f32 _time, Interpolation _interpolation )
: m_vp(_pos)
, m_time(_time)
, m_interp(_interpolation)
, m_normalIn(Vec3d::Zero)
, m_normalInTime(Vec3d::One)
, m_normalOut(Vec3d::Zero)
, m_normalOutTime(Vec3d::One)
, m_pointId(U32_INVALID)
{
}

Spline::SplinePoint::SplinePoint( const Vec3d& _pos, const Vec3d& _normalIn, const Vec3d& _normalInTime, const Vec3d& _normalOut, const Vec3d& _normalOutTime, f32 _time, Interpolation _interpolation )
: m_vp(_pos)
, m_time(_time)
, m_interp(_interpolation)
, m_normalIn(_normalIn)
, m_normalInTime(_normalInTime)
, m_normalOut(_normalOut)
, m_normalOutTime(_normalOutTime)
, m_pointId(U32_INVALID)
{
}



void Spline::SplinePoint::serializeToBlob (Blob* _blob)
{
    cleanNormals();

    _blob->pushInt32(m_pointId);
    _blob->pushFloat32(m_vp.m_x);
    _blob->pushFloat32(m_vp.m_y);
    _blob->pushFloat32(m_vp.m_z);

    _blob->pushFloat32(m_normalIn.m_x);
    _blob->pushFloat32(m_normalIn.m_y);
    _blob->pushFloat32(m_normalIn.m_z);

    _blob->pushFloat32(m_normalInTime.m_x);
    _blob->pushFloat32(m_normalInTime.m_y);
    _blob->pushFloat32(m_normalInTime.m_z);

    _blob->pushFloat32(m_normalOut.m_x);
    _blob->pushFloat32(m_normalOut.m_y);
    _blob->pushFloat32(m_normalOut.m_z);

    _blob->pushFloat32(m_normalOutTime.m_x);
    _blob->pushFloat32(m_normalOutTime.m_y);
    _blob->pushFloat32(m_normalOutTime.m_z);

    _blob->pushFloat32(m_time);

    _blob->pushInt32((i32)m_interp);
}

void Spline::SplinePoint::fillFromBlob (Blob* _blob)
{
    m_pointId = _blob->extractInt32();
    m_vp.m_x = _blob->extractFloat32();
    m_vp.m_y = _blob->extractFloat32();
    m_vp.m_z = _blob->extractFloat32();

    m_normalIn.m_x = _blob->extractFloat32();
    m_normalIn.m_y = _blob->extractFloat32();
    m_normalIn.m_z = _blob->extractFloat32();

    m_normalInTime.m_x = _blob->extractFloat32();
    m_normalInTime.m_y = _blob->extractFloat32();
    m_normalInTime.m_z = _blob->extractFloat32();

    m_normalOut.m_x = _blob->extractFloat32();
    m_normalOut.m_y = _blob->extractFloat32();
    m_normalOut.m_z = _blob->extractFloat32();

    m_normalOutTime.m_x = _blob->extractFloat32();
    m_normalOutTime.m_y = _blob->extractFloat32();
    m_normalOutTime.m_z = _blob->extractFloat32();

    m_time = _blob->extractFloat32();

    i32 type = _blob->extractInt32();
    Interpolation typeInterpolation = (Interpolation)type;
    m_interp = typeInterpolation;

    switch (m_interp)
    {
    case interp_linear: 
    case interp_spline: 
    case interp_constant: 
        m_normalIn      = Vec3d::Zero;
        m_normalInTime  = Vec3d::One;
        m_normalOut     = Vec3d::Zero;
        m_normalOutTime = Vec3d::One;
        break;
    case interp_bezier: 
        break;
    case interp_bezier_standard: 
        m_normalInTime  = Vec3d::One;
        m_normalOutTime = Vec3d::One;
        break;
    }

    cleanNormals();
}

void Spline::SplinePoint::cleanNormals()
{
    if (m_normalInTime.m_x < 0.f)
        m_normalInTime.m_x = 0.f;
    if (m_normalInTime.m_y < 0.f)
        m_normalInTime.m_y = 0.f;
    if (m_normalInTime.m_z < 0.f)
        m_normalInTime.m_z = 0.f;

    if (m_normalOutTime.m_x < 0.f)
        m_normalOutTime.m_x = 0.f;
    if (m_normalOutTime.m_y < 0.f)
        m_normalOutTime.m_y = 0.f;
    if (m_normalOutTime.m_z < 0.f)
        m_normalOutTime.m_z = 0.f;

    if (m_normalIn.m_x == 0.f && m_normalInTime.m_x == 0.f )
        m_normalInTime.m_x = 1.f;
    if (m_normalIn.m_y == 0.f && m_normalInTime.m_y == 0.f )
        m_normalInTime.m_y = 1.f;
    if (m_normalIn.m_z == 0.f && m_normalInTime.m_z == 0.f )
        m_normalInTime.m_z = 1.f;

    if (m_normalIn.m_x == 0.f && m_normalOutTime.m_x == 0.f )
        m_normalOutTime.m_x = 1.f;
    if (m_normalIn.m_y == 0.f && m_normalOutTime.m_y == 0.f )
        m_normalOutTime.m_y = 1.f;
    if (m_normalIn.m_z == 0.f && m_normalOutTime.m_z == 0.f )
        m_normalOutTime.m_z = 1.f;

    //if (m_interp == interp_bezier_standard)
    //{
    //    if (m_normalIn.IsEqual(Vec3d::Zero, MTH_EPSILON))
    //    {
    //        m_normalIn = Vec3d::Right;
    //    }
    //    if (m_normalOut.IsEqual(Vec3d::Zero, MTH_EPSILON))
    //    {
    //        m_normalOut = Vec3d::Right;
    //    }
    //}
}

BEGIN_SERIALIZATION(Spline)

    SERIALIZE_CONTAINER_OBJECT("Points",m_points);
    SERIALIZE_FUNCTION(postLoadForId,ESerialize_Data_Load);

END_SERIALIZATION()

Spline::Spline()
: delta_t(0)
, m_pointIdCounter(0)
{
}

Spline::Spline(const Spline& s)
: m_pointIdCounter(0)
{
    m_points = s.m_points;
    delta_t = s.delta_t;
}

Spline::~Spline()
{}

void Spline::postLoadForId()
{
    const u32 count = GetNumPoints();
    for (u32 i = 0; i < count ; i++)
    {
        m_points[i].m_pointId = m_pointIdCounter;
        m_pointIdCounter++;
    }
}


// Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4)
Vec3d Spline::Eq(float t, const Vec3d& p1, const Vec3d& p2, const Vec3d& p3, const Vec3d& p4)
{
    float t2 = t * t;
    float t3 = t2 * t;

    float b1 = .5f * (  -t3 + 2*t2 - t);
    float b2 = .5f * ( 3*t3 - 5*t2 + 2);
    float b3 = .5f * (-3*t3 + 4*t2 + t);
    float b4 = .5f * (   t3 -   t2    );

    return (p1*b1 + p2*b2 + p3*b3 + p4*b4); 
}

void Spline::RemoveSplinePoint(const u32 _index)
{
    m_points.eraseKeepOrder(_index);
}

bbool Spline::ExtractSplinePoint(const u32 _indexBegin, const u32 _indexEnd, const f32 _timeDelta)
{
    SafeArray<SplinePoint, 3> newPoints;

    if (_indexEnd <= _indexBegin || _indexEnd > m_points.size())
        return bfalse;

    newPoints.reserve(_indexEnd - _indexBegin);
    for (u32 i=_indexBegin; i<_indexEnd; i++)
    {
        SplinePoint pt = m_points[i];
        pt.m_time += _timeDelta;
        newPoints.push_back(pt);
    }

    m_points = newPoints;
    return btrue;
}

void Spline::AddSplinePoint(const SplinePoint& _splinePoint, bbool _computeId /* = true*/)
{
    m_points.push_back(_splinePoint);
    if(_computeId)
    {
        m_points[m_points.size() - 1].m_pointId = m_pointIdCounter;
        m_pointIdCounter++;
    }
    else
    {
        if(m_pointIdCounter <= _splinePoint.m_pointId)
            m_pointIdCounter = _splinePoint.m_pointId + 1;
    }
    delta_t = (float)1 / (float)m_points.size();
}


void Spline::AddSplinePoint(const Vec3d& v, Interpolation _interp, float _t)
{
    SplinePoint sp = SplinePoint(v,_t,_interp);
    AddSplinePoint(sp, btrue);
}


void Spline::AddSplinePoint(const Vec3d& v, const Vec3d& normalIn, const Vec3d& normalInTime, const Vec3d& normalOut, const Vec3d& normalOutTime,  Interpolation _interp, float _t)
{
    SplinePoint sp = SplinePoint(v,normalIn, normalInTime, normalOut, normalOutTime, _t,_interp);
    AddSplinePoint(sp, btrue);
}


void Spline::setSplinePointAt(const Vec3d& v, Interpolation _interp , float _t, int _index)
{
    m_points[_index].m_vp = v;
    m_points[_index].m_time = _t;
    m_points[_index].m_interp = _interp;
}

i32 Spline::getSplinePointIndex(u32 _pointId)
{    
    const u32 count = GetNumPoints();
    for (u32 i = 0; i < count ; i++)
    {
    	if(m_points[i].m_pointId == _pointId)
            return i;
    }
    return -1;   
}


Spline::BoundsExceed Spline::BOUNDS(int& _val, int _max) const
{
    if (_val < 0) 
    {
        _val = 0; 
        return exceed_min;
    }
    if (_val > _max)
    {
        _val = _max;
        return exceed_max;
    }
    return exceed_ok;
}


f32 getBezierY(const Vec2d & p1, const Vec2d & p2, const Vec2d & p3, const Vec2d & p4, f32 _x)
{
    f32 tBegin  = 0.f;
    f32 tEnd    = 1.f;
    f32 tMid;
    u32 counter;
    Vec2d pos;

#define MAX_COUNTER 20


    for (counter = 0; counter < MAX_COUNTER ; counter++)
    {
        tMid    = (tEnd + tBegin)*0.5f;
        pos = getBezierPosition(p1,p2,p3,p4,tMid);
        if (f32_Abs(pos.m_x - _x) < 0.001f)
            break;

        if (pos.m_x < _x)
        {
            tBegin = tMid;
        } else
        {
            tEnd = tMid;
        }
    }
    return pos.m_y;
}

void Spline::getPointsAtTime( f32 t, i32 &_res_p1, i32 &_res_p2 ) const
{
	static const float time_eps = 0.00001f;
	int p = m_points.size()-1;
	for (i32 i = 0; i < (i32)m_points.size(); i++)
	{
		if (m_points[i].m_time > t + time_eps) // needed epsilon otherwise frame 0 is not selected at time 0 (reasonable amount when talking about seconds)
		{
			p = i-1;
			break;
		}
	}

	_res_p1 = p;     
	_res_p2 = p + 1;     
	BoundsExceed exceedBounds = BOUNDS(_res_p1, m_points.size() - 1);
	BOUNDS(_res_p2, m_points.size() - 1);

	if ((m_points[_res_p1].m_time > t + time_eps) || (exceedBounds == exceed_min))
	{
		_res_p2 = -1;
	}
	if (exceedBounds == exceed_max)
	{
		_res_p1 = -1;
	}
}

bbool Spline::GetInterpolatedAtTime(float t, Vec3d& _dest) const
{
	int p1, p2;
	getPointsAtTime( t ,p1, p2 );
	if ( ( p1 < 0 ) || ( p2 < 0 ) )
	{
		_dest = m_points[ ( ( p1 < 0 ) ? p2 : p1 ) ].m_vp;
		return bfalse;
	}

    const SplinePoint & sp1 = m_points[p1];
    const SplinePoint & sp2 = m_points[p2];
    float deltaT = t - sp1.m_time;
    if (deltaT < 0.f)
        deltaT = 0.f;   // handle epsilion

    float deltaTime = sp2.m_time-sp1.m_time;
    if (deltaTime <= 0.f)
    {
        _dest = sp2.m_vp;
        return btrue;
    }
    float lt = deltaT / deltaTime;

    switch (sp1.m_interp)
    {
    case interp_constant:
        if (lt >= 1.f)
            _dest = sp2.m_vp;
        else
            _dest =  sp1.m_vp;
        return btrue;
    case interp_linear:
        {
            if (lt >= 1.f)
            {
                _dest = sp2.m_vp;
                if (lt <= 1.f + MTH_EPSILON)
                    return btrue;
                else
                    return bfalse;
            }

            _dest =  sp1.m_vp + (sp2.m_vp-sp1.m_vp) * lt;
        }
        return btrue;
    case interp_spline:
        {
            int p0 = p1 - 1;     BOUNDS(p0, m_points.size() - 1);
            int p3 = p1 + 2;     BOUNDS(p3, m_points.size() - 1);
            _dest = Spline::Eq(lt, m_points[p0].m_vp, sp1.m_vp, sp2.m_vp, m_points[p3].m_vp);
        }
        return btrue;
    case interp_bezier_standard:
        {
            Vec3d   normalP1Next = sp1.m_vp + sp1.m_normalOut;
            Vec3d   normalP2Prev = sp2.m_vp - sp2.m_normalIn;

            _dest = getBezierPosition(sp1.m_vp,normalP1Next,normalP2Prev,sp2.m_vp,lt);
        }
        return btrue;
    case interp_bezier:
        {
            Vec3d   normalP1Next = sp1.m_vp + sp1.m_normalOut;
            Vec3d   normalP2Prev = sp2.m_vp - sp2.m_normalIn;

            _dest.m_x = getBezierY(Vec2d(sp1.m_time, sp1.m_vp.m_x),
                Vec2d(sp1.m_time + sp1.m_normalOutTime.m_x, normalP1Next.m_x),
                Vec2d(sp2.m_time - sp2.m_normalInTime.m_x, normalP2Prev.m_x),
                Vec2d(sp2.m_time, sp2.m_vp.m_x), t);

            _dest.m_y = getBezierY(Vec2d(sp1.m_time, sp1.m_vp.m_y),
                Vec2d(sp1.m_time + sp1.m_normalOutTime.m_y, normalP1Next.m_y),
                Vec2d(sp2.m_time - sp2.m_normalInTime.m_y, normalP2Prev.m_y),
                Vec2d(sp2.m_time, sp2.m_vp.m_y), t);

            _dest.m_z = getBezierY(Vec2d(sp1.m_time, sp1.m_vp.m_z),
                Vec2d(sp1.m_time + sp1.m_normalOutTime.m_z, normalP1Next.m_z),
                Vec2d(sp2.m_time - sp2.m_normalInTime.m_z, normalP2Prev.m_z),
                Vec2d(sp2.m_time, sp2.m_vp.m_z), t);
        }
        return btrue;
    }
    return btrue;
}


int Spline::GetNumPoints() const
{
    return m_points.size();
}

const Vec3d& Spline::GetNthPoint(int n) const
{
    return m_points[n].m_vp;
}

void Spline::serializeToBlob (Blob* _blob)
{
    sortByTime();
    i32 count = m_points.size();
    _blob->pushInt32(count);
    for (i32 i = 0; i < count; i++)
    {
        m_points[i].serializeToBlob(_blob);
    }
}

void Spline::fillFromBlob (Blob* _blob)
{
    clear();
    u32 count = (u32)_blob->extractInt32();    
    for (u32 i = 0; i < count; i++)
    {
        SplinePoint sp;
        sp.fillFromBlob(_blob);        
        AddSplinePoint(sp, false);        
    }

    sortByTime();
}

void Spline::sortByTime()
{
    u32 size = m_points.size();
    for (u32 i = 0; i < size; i++)
    {
        ITF_ASSERT(m_pointIdCounter > m_points[i].m_pointId);

        f32 t = m_points[i].m_time;
        u32 swapIndex = i;
        for (u32 j=i+1; j<size; j++)
        {
            if (t > m_points[j].m_time)
            {
                swapIndex = j;
                t = m_points[j].m_time;
            }
        }
        // do swap
        if (i != swapIndex)
        {
            SplinePoint p = m_points[i];

            m_points[i] = m_points[swapIndex];
            m_points[swapIndex] = p;
        }
    }
    computeBezierStandardDefaultValues();
    
}


void Spline::setTimeAt(f32 _time, u32 _index)
{
    if(_index < m_points.size())
        m_points[_index].m_time = _time; 
}

void Spline::computeBezierStandardDefaultValues()
{
    u32 count = m_points.size();
    for (u32 i=0; i<count; i++)
    {
        SplinePoint & pt = m_points[i];
        if (pt.m_interp == interp_bezier_standard && pt.m_normalOut.IsEqual(Vec3d::Zero, MTH_EPSILON))
        {
            pt.m_normalOut = Vec3d::Right;
        }
        if ( i > 0 && m_points[i - 1].m_interp == interp_bezier_standard && pt.m_normalIn.IsEqual(Vec3d::Zero, MTH_EPSILON))
        {
            pt.m_normalIn = Vec3d::Right;
        }
    }
}

} // namespace ITF

