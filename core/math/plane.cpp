#include "precompiled_core.h"

#ifndef _ITF_PLANE_H_
#include "core/math/plane.h"
#endif //_ITF_PLANE_H_


namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Plane::testLineIntersection(const Vec3d &_pointOnLine, const Vec3d &_direction, Vec3d &_intersection, f32 &_intersection_DirectionMultiplier) const
    {
        const f32 denominator = _direction.dot(m_normal);
        if (denominator==0.f)
            return bfalse;

        _intersection_DirectionMultiplier = (m_constant-_pointOnLine.dot(m_normal))/denominator;
        _intersection = _pointOnLine + _direction * _intersection_DirectionMultiplier;

        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Plane::intersectWithPlane_hasZ0(const Plane &_otherPlane, Vec3d &_lineDirection, Vec3d &_PZ0)
    {
        ITF_ASSERT_CRASH(bfalse, "not implemented");

        m_normal.crossProduct(_otherPlane.m_normal,_lineDirection);
        f32 sqnorm = _lineDirection.sqrnorm();
        if (sqnorm!=0)
        {
            _lineDirection *= f32_InvSqrt(sqnorm);
            f32 det = _otherPlane.m_normal.m_x*m_normal.m_y-m_normal.m_x*_otherPlane.m_normal.m_y;
            ITF_ASSERT(det!=0);
            if (det==0)
                return bfalse;
            f32 y0 =  (_otherPlane.m_normal.m_x*m_constant-m_normal.m_x*_otherPlane.m_constant)/det;
            if (m_normal.m_x!=0)
            {
                f32 x0 = (m_constant-m_normal.m_y*y0)/m_normal.m_x;
                _PZ0.set(x0,y0,0);
            }
            else if (_otherPlane.m_normal.m_x != 0)
            {
                f32 x0 = (_otherPlane.m_constant-_otherPlane.m_normal.m_y*y0)/_otherPlane.m_normal.m_x;
                _PZ0.set(x0,y0,0);
            }
            else
            {
                _PZ0.set(0,y0,0);
            }
            return btrue;
        }
        return bfalse;
    }
}
