#ifndef _ITF_QUATERNION_H_
#define _ITF_QUATERNION_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_


namespace ITF
{
    /// Quaternion class
    /// Models a quaternion of the form : m_w + i*m_axis.x + j*m_axis.y + k*m_axis.z     with (1,i,j,k)=hypercomplex basis
    class Quaternion
    {
    public:
        Vec3d m_axis;
        f32 m_w;

        /// Constructor
        Quaternion() {}

        ///Constructor from vector and w
        Quaternion(f32 _w, const Vec3d &_axis) {m_w = _w; m_axis = _axis;}

        ///Build a rotation from axis and angle
        void buildRotation(f32 _angle, const Vec3d &_axis)
        {
            f32 halfAngle = _angle*0.5f;
            f32 s,c;
            f32_CosSin(halfAngle, &c, &s);
            m_w = c;
            m_axis = _axis*s;
        }
        ///Set value from individial floats
        ITF_INLINE void set(f32 _w, f32 _x, f32 _y, f32 _z) {m_w = _w; m_axis.m_x = _x; m_axis.m_y = _y; m_axis.m_z = _z;}

        ///Set to identity quaternion
        ITF_INLINE void setToIdentity() {set(1,0,0,0);}

        ///Normalize so that it represents a rotation (does nothing to 0)
        ITF_INLINE void normalize()
        {
            f32 norm = m_w*m_w + m_axis.sqrnorm();
            if (norm==0)
                return;

            norm = f32_InvSqrt(norm);
            m_w *= norm;
            m_axis *= norm;
        }


        ///Multiply 2 quaternions. If q1 and q2 represent rotations R1 and R2, q1 * q2 represents R1(R2())
        ITF_INLINE Quaternion operator*(const Quaternion& _other) const
        {
            Quaternion result;
            result.m_w = m_w*_other.m_w - m_axis.dot(_other.m_axis);
            result.m_axis = (m_axis^_other.m_axis) + _other.m_axis*m_w + m_axis*_other.m_w;
            return result;
        }

        /// multiply/assign
        ITF_INLINE Quaternion& operator*=(const Quaternion& _other)
        {
            f32 newW = m_w*_other.m_w - m_axis.dot(_other.m_axis);
            m_axis = (m_axis^_other.m_axis) + _other.m_axis*m_w + m_axis*_other.m_w;
            m_w = newW;
            return *this;
        }
    };
}


#endif
