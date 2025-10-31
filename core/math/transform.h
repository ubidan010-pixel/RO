

#ifndef _ITF_TRANSFORM_H_
#define _ITF_TRANSFORM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{

///////////////////////////////////////////////////////////////////////////////////////////
///2D vector (no translation) matrix
struct matrix2d
{
    Vec2d m_col1;
    Vec2d m_col2;

    void setIdentity() {m_col1.set(1.f,0.f); m_col2.set(0.f,1.f);}
    void setColumns(const Vec2d &_col1, const Vec2d &_col2) {m_col1 = _col1; m_col2 = _col2;}
    void set(f32 _c1x, f32 _c1y, f32 _c2x, f32 _c2y) {m_col1.set(_c1x,_c1y); m_col2.set(_c2x,_c2y);}

    const matrix2d operator*(const matrix2d &_other) const
    {
        matrix2d r;
#if defined( ITF_PSI )
        register float* pr = (float*)&r.m_col1;
        register float* pa = (float*)&this->m_col1;
        register float* pb = (float*)&_other.m_col1;
        register __vec2x32float__ f_p0, f_p1, f_p2;
        register __vec2x32float__ f_p3, f_p4, f_p5;
        asm	{
    		psq_l      f_p0,  0(pb), 0, 0;         //tmp load vector _other.m_col1
    		psq_l      f_p1,  8(pb), 0, 0;         //tmp load vector _other.m_col2
            psq_l      f_p2,  0(pa), 0, 0;      //load vector m_col1
            ps_merge11 f_p4, f_p0, f_p1;        // _other.m_col1.m_y | _other.m_col2.m_y
            ps_merge00 f_p5, f_p0, f_p1;        // _other.m_col1.m_x | _other.m_col2.m_x
    		psq_l      f_p3,  8(pa), 0, 0;      //load vector m_col2

            ps_muls0   f_p0, f_p5, f_p2;        // m_col1.m_x*_other.m_col1.m_x                                | m_col1.m_x*_other.m_col2.m_x
            ps_muls1   f_p1, f_p5, f_p2;        // m_col1.m_y*_other.m_col1.m_x                                | m_col1.m_y*_other.m_col2.m_x
            ps_madds0  f_p0, f_p4, f_p3, f_p0;  // m_col1.m_x*_other.m_col1.m_x + m_col2.m_x*_other.m_col1.m_y | m_col1.m_x*_other.m_col2.m_x + m_col2.m_x*_other.m_col2.m_y
            ps_madds1  f_p1, f_p4, f_p3, f_p1;  // m_col1.m_y*_other.m_col1.m_x + m_col2.m_y*_other.m_col1.m_y | m_col1.m_y*_other.m_col2.m_x + m_col2.m_y*_other.m_col2.m_y

            psq_st     f_p0,  0(pr), 0, 0;     //store result m_col1
            psq_st     f_p1,  8(pr), 0, 0;     //store result m_col2
        }
#else
        float r00 = m_col1.m_x*_other.m_col1.m_x + m_col2.m_x*_other.m_col1.m_y;
        float r01 = m_col1.m_x*_other.m_col2.m_x + m_col2.m_x*_other.m_col2.m_y;
        float r10 = m_col1.m_y*_other.m_col1.m_x + m_col2.m_y*_other.m_col1.m_y;
        float r11 = m_col1.m_y*_other.m_col2.m_x + m_col2.m_y*_other.m_col2.m_y;
        r.set(r00, r10, r01, r11);
#endif
        return r;
    }
};

ITF_INLINE Vec2d mulMatrix2d(const matrix2d& A, const Vec2d& v)
{
#if defined( ITF_PSI )
		Vec2d res;
		register float* pres = (float*)&res.m_x;
		register __vec2x32float__ f_p0, f_p1, f_p2, f_p3;
		register float* pv = (float*)&v.m_x;
		register float* p1 = (float*)&A.m_col1.m_x;
		register float* p2 = (float*)&A.m_col2.m_x;
		asm	{		
			psq_l      f_p1,  0(pv), 0, 0;       //load vector
			psq_l      f_p2,  0(p1), 0, 0;       //load vector
			psq_l      f_p3,  0(p2), 0, 0;       //load vector

            ps_muls0   f_p0, f_p2, f_p1;        // A.m_col1.m_x * v.m_x                        | A.m_col1.m_y * v.m_x
            ps_madds1  f_p0, f_p3, f_p1, f_p0;  // A.m_col1.m_x * v.m_x + A.m_col2.m_x * v.m_y | A.m_col1.m_y * v.m_x A.m_col2.m_y * v.m_y

			psq_st     f_p0,  0(pres), 0, 0;     //store result
		}
		return res;
#else
    return Vec2d(A.m_col1.m_x * v.m_x + A.m_col2.m_x * v.m_y,
                 A.m_col1.m_y * v.m_x + A.m_col2.m_y * v.m_y);
#endif
}

ITF_INLINE Vec2d mulMatrix2dT(const matrix2d& A, const Vec2d& v)
{
    return Vec2d(v.dot(A.m_col1),v.dot(A.m_col2));
}
///////////////////////////////////////////////////////////////////////////////////////////
/// 2D affine transform
struct transform2d
{
    matrix2d    m_rot;
    Vec2d       m_pos;

    void setIdentity() {m_rot.setIdentity(); m_pos.set(0.f,0.f);}

    const transform2d operator*(const transform2d &_other) const
    {
        transform2d r;
        r.m_rot = m_rot*_other.m_rot;
        r.m_pos = m_pos + mulMatrix2d(m_rot, _other.m_pos);
        return r;
    }
};


ITF_INLINE Vec2d mulTransform2d(const transform2d& T, const Vec2d& v)
{
    return T.m_pos + mulMatrix2d(T.m_rot, v);
}

ITF_INLINE Vec2d mulTransform2dT(const transform2d& T, const Vec2d& v)
{
    return mulMatrix2dT(T.m_rot, v - T.m_pos);
}

}

#endif // _ITF_TRANSFORM_H_

