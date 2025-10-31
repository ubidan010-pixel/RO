#ifndef _ITF_PLANE_H_
#define _ITF_PLANE_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

namespace ITF
{
    //Describes a plane, with equation : m_normal.X = m_constant
    class Plane 
    {
    public:        
        Vec3d m_normal;
        f32 m_constant;

        /// default constructor
        Plane() {}

        /// constructor by normal/constants
        Plane(const Vec3d &_normal, f32 _constant) : m_normal(_normal), m_constant(_constant) {}

        /// Test line intersection
        /// @param _pointOnLine [in] a point in the line
        /// @param _direction [in] direction of the line
        /// @param _intersection [out] intersection, if any
        /// @param _intersectionMultiplier [out] if the line intersects, _intersection=_pointOnLine+_intersectionMultiplier*_direction
        /// @return btrue if intersection (strictly), and _intersection/_intersectionMultiplier describe the intersection point
        bbool testLineIntersection(const Vec3d &_pointOnLine, const Vec3d &_direction, Vec3d &_intersection, f32 &_intersection_DirectionMultiplier)const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        bbool isPointInPositiveSide(const Vec3d &_p) const {return _p.dot(m_normal)>=m_constant;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// tests intersection of 2 planes. If any, must have valid intersection with z=0
        /// @param _otherPlane
        /// @param[out] _lineDirection
        /// @param[out] _PZ0 an intersection point with z=0
        /// @return btrue if there is an intersection
        bbool intersectWithPlane_hasZ0(const Plane &_otherPlane, Vec3d &_lineDirection, Vec3d &_PZ0);
    };
}

#endif
