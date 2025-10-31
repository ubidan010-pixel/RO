///////////////////////////////////////////////////////////////////////////////
///////////////////////       Basic vector class
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_VEC2D_H_
#define _ITF_VEC2D_H_

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_MATHF32_H_
#include "core/math/Mathf32.h"
#endif _ITF_MATHF32_H_

#ifndef _ITF_ARCHIVEMEMORY_H_
#include "core/ArchiveMemory.h"
#endif //_ITF_ARCHIVEMEMORY_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#ifdef ITF_WII
#ifndef __FLOAT2_BROADWAY__
#include "core/math/WII/SIMD_Float2.h"
#endif
#endif

namespace ITF
{

class Vec2d
{
public:
    f32 m_x, m_y;

#if (defined (_DEBUG) || defined (COVERITY_CHECK)) && defined (ITF_WINDOWS)
    Vec2d();
#else
    ITF_INLINE Vec2d():m_x(0.0f), m_y(0.0f) {}
#endif

#if defined( ITF_WII )
    ITF_INLINE Vec2d(const Vec2d& src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&src.m_x;	
		asm	{		
			psq_l     f_p1,  0(pv), 0, 0; //load vector
			psq_st    f_p1,  0(me), 0, 0; //store result
		}
        #else

        m_x = src.m_x;
        m_y = src.m_y;

        #endif
    }
#endif

#if defined (_DEBUG) && defined (ITF_WINDOWS)
#define ENABLE_FLOAT_CHECK
#endif //_DEBUG) && defined (ITF_WINDOWS)

#ifdef ENABLE_FLOAT_CHECK
#define VECTOR_CHECK   \
    if (m_x != m_x || m_y != m_y)\
    {\
        ITF_ASSERT_CRASH(0,"Invalue value in vec2d");\
    }\

#define VECTOR_CHECK_DST   \
    if (_dst->m_x != _dst->m_x || _dst->m_y != _dst->m_y)\
    {\
    ITF_ASSERT_CRASH(0,"Invalue value in vec2d");\
    }\

#else

#define VECTOR_CHECK  
#define VECTOR_CHECK_DST
#endif //ENABLE_FLOAT_CHECK


    ITF_INLINE bbool isValid() const
    {
        return !(ITF_isnan(m_x) || ITF_isnan(m_y));
    }

    ITF_INLINE Vec3d to3d(const f32 _z = 0.0f)const
    {
        return Vec3d (m_x, m_y, _z);
    }

    static void  Vec3dTo2d(const Vec3d& _v3d, Vec2d& _v2d)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1;
		register float* p3 = (float*)&_v3d.m_x;
		register float* p2 = (float*)&_v2d.m_x;	
		asm	{		
			psq_l     f_p1,  0(p3), 0, 0; //load vector
			psq_st    f_p1,  0(p2), 0, 0; //store result
		}
        #else
        
        _v2d.m_x = _v3d.m_x;
        _v2d.m_y = _v3d.m_y;
        
        #endif
    }

    const Vec2d& operator = (const Vec2d& src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&src.m_x;	
		asm	{		
			psq_l     f_p1,  0(pv), 0, 0; //load vector
			psq_st    f_p1,  0(me), 0, 0; //store result
		}
        #else
        
        m_x = src.m_x;
        m_y = src.m_y;

        VECTOR_CHECK
        
        #endif        
        return *this;
    }

    void set(const f32 x, const f32 y)
    {
        m_x = x;
        m_y = y;

        VECTOR_CHECK
    }

    void  truncateVec3d(const Vec3d& _v3d)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1;
		register float* me = (float*)&this->m_x;
		register float* p3 = (float*)&_v3d.m_x;	
		asm	{		
			psq_l     f_p1,  0(p3), 0, 0; //load vector
			psq_st    f_p1,  0(me), 0, 0; //store result
		}
        #else
        
        m_x = _v3d.m_x;
        m_y = _v3d.m_y;

        VECTOR_CHECK
        
        #endif
    }
    // use explicitly Vec3dTo2d or truncateVec3d instead
/*    Vec2d(const Vec3d& v)
    {
        m_x = v.m_x;
        m_y = v.m_y;
    }*/

    Vec2d(const f32 x, const f32 y)
    {
        m_x = x;
        m_y = y;

        VECTOR_CHECK
    }


    void serialize(ArchiveMemory& _archive)
    {
        _archive.serialize(m_x);
        _archive.serialize(m_y);
    }


    ITF_INLINE Vec2d& operator+=(const Vec2d& v)
    {       
        Vec2d::Add(this, this, &v);
        return *this;
    }
/*
    ITF_INLINE Vec2d& operator+=(const Vec3d& v)
    {
        m_x+=v.m_x;
        m_y+=v.m_y;

        return *this;
    }*/

    ITF_INLINE Vec2d& operator+=(register f32 v)
    {
        Vec2d::Add(this, this, v);
        return *this;
    }

    ITF_INLINE Vec2d operator+(const Vec2d& v) const
    {
        Vec2d res;
        Vec2d::Add(&res, this, &v);
        return res;
    }

    ITF_INLINE Vec2d operator+(register f32 v) const
    {
        Vec2d res;
        Vec2d::Add(&res, this, v);
        return res;
    }
/*
    ITF_INLINE Vec2d operator+(const Vec3d& v) const
    {
        return Vec2d(m_x+v.m_x, m_y+v.m_y);
    }
*/
    ITF_INLINE Vec2d& operator-=(const Vec2d& v)
    {
        Vec2d::Sub(this, this, &v);
        return *this;
    }
/*
    ITF_INLINE Vec2d& operator-=(const Vec3d& v)
    {
        m_x-=v.m_x;
        m_y-=v.m_y;

        return *this;
    }*/

    ITF_INLINE Vec2d& operator-=(register f32 v)
    {
        Vec2d::Sub(this, this, v);
        return *this;
    }

    ITF_INLINE Vec2d operator-(const Vec2d& v) const
    {
        Vec2d res;
        Vec2d::Sub(&res, this, &v);
        return res;
    }

    ITF_INLINE Vec2d operator-(register f32 v) const
    {
        Vec2d res;
        Vec2d::Sub(&res, this, v);
        return res;
    }
/*
    ITF_INLINE Vec2d operator-(const Vec3d& v) const
    {
        return Vec2d(m_x-v.m_x, m_y-v.m_y);
    }
*/
    ITF_INLINE Vec2d operator-() const
    {
        #if defined( ITF_PSI )
		Vec2d res;
		register float* pres = (float*)&res.m_x;
		register __vec2x32float__ f_p0, f_p1;
		register float* me = (float*)&this->m_x;
		asm	{		
			psq_l      f_p1,  0(me), 0, 0;   //load vector
			ps_neg     f_p0,   f_p1;         //neg
			psq_st     f_p0,  0(pres), 0, 0; //store result
		}
		return res;
        #else
        
        VECTOR_CHECK
        
        return Vec2d(-m_x, -m_y);
        
        #endif
    }

    ITF_INLINE Vec2d& operator*=(const register f32 s)
    {
        Vec2d::Scale(this, this, s);
        return *this;
    }

    ITF_INLINE Vec2d& operator*=(const Vec2d& v)
    {
        Vec2d::Mul(this, this, &v);
        return *this;
    }

    /*ITF_INLINE Vec2d operator*=(const Vec3d& v)
    {
        m_x*=v.m_x;
        m_y*=v.m_y;
        return *this;
    }*/

    ITF_INLINE Vec2d operator*(const register f32 s) const
    {
        Vec2d res;
        Vec2d::Scale(&res, this, s);
        return res;
    }

    ITF_INLINE Vec2d operator*(const Vec2d& v) const
    {
        Vec2d res;
        Vec2d::Mul(&res, this, &v);
        return res;
    }

    ITF_INLINE Vec2d operator/=(const Vec2d& v)
    {
        Vec2d::Div(this, this, &v);
        return *this;
    }

    ITF_INLINE Vec2d& operator/=(const f32 s)
    {
        #if defined( ITF_PSI )
	    register __vec2x32float__ f_p1, f_p2;
	    register float* me = (float*)&this->m_x;
	    register f32 inv = f32_Inv(s);
	    asm	{		
		    psq_l     f_p2,  0(me), 0, 0; //load vector
		    ps_muls0  f_p1,   f_p2, inv;  //mul  vector
		    psq_st    f_p1,  0(me), 0, 0; //store result
	    }
        #else
        
        m_x/=s;
        m_y/=s;
        
        VECTOR_CHECK
        
        #endif
        return *this;
    }

    ITF_INLINE Vec2d operator/(const register f32 s) const
    {
        #if defined( ITF_PSI )
		Vec2d res;
		register float* pres = (float*)&res.m_x;
		register __vec2x32float__ f_p0, f_p1, f_p2;
		register float* me = (float*)&this->m_x;
		asm	{		
			psq_l      f_p1,  0(me), 0, 0;   //load vector
			ps_merge00 f_p2,  s, s;
			ps_div     f_p0,   f_p1, f_p2;   //div
			psq_st     f_p0,  0(pres), 0, 0; //store result
		}
		return res;
        #else
        

        VECTOR_CHECK

        return Vec2d(m_x/s, m_y/s);

        #endif
    }

    ITF_INLINE Vec2d operator/(const Vec2d& v) const
    {
        Vec2d res;
        Vec2d::Div(&res, this, &v);
        return res;
    }

    ITF_INLINE Vec2d& Lerp(const Vec2d& _a, const Vec2d& _b, f32 _st)
    {
        Vec2d::Lerp(this, &_a, &_b, _st);
        return *this;
    }

    ITF_INLINE bool operator==(const Vec2d& v) const
    {
        return (m_x == v.m_x) && (m_y == v.m_y);
    }

    ITF_INLINE bool operator!=(const Vec2d& v) const
    {
        return (m_x != v.m_x) || (m_y != v.m_y);
    }

    ITF_INLINE f32 sqrnorm() const
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ fXY;
		const register float *me = (float*)&this->m_x;
		register f32 fResult;
		asm {			
			psq_l	fXY, 0(me), 0, 0        // load XY  
			ps_mul  fXY, fXY, fXY           // sqr XY   
			ps_sum0 fResult, fXY, fXY, fXY  // add X² + Y² 
		}
		return fResult;
        #else

        VECTOR_CHECK
        return m_x*m_x+m_y*m_y;
        
        #endif
    }
    
    ITF_INLINE f32 norm() const
    {
        return f32_Sqrt(sqrnorm());
    }
    
    ITF_INLINE f32 norminv() const
    {
        const f32 n2 = sqrnorm();    
        return (n2>0.f) ? f32_InvSqrt(n2) : 0.f;
    }    

    ITF_INLINE Vec2d& normalize()
    {
        #if defined( F32_WII )
        const f32 n2 = sqrnorm();
        if ( n2 > MTH_EPSILON_2 )
            Vec2d::Scale(this, this, f32_InvSqrt(n2));
        else
        	clear();
        #else
        
        const f32 n = norm();
        if ( n > MTH_EPSILON )
            *this/=n;
        else
        	clear();
        	
        #endif
        return *this;
    }

    ITF_INLINE f32 dot(const Vec2d& _v) const
    {
        return Vec2d::Dot(this, &_v);
    }

    ITF_INLINE Vec2d getPerpendicular() const
    {
        return Vec2d(-m_y, m_x);
    }

    ITF_INLINE void setLength(f32 _len)
    {
        normalize();
        Vec2d::Scale(this, this, _len);
    }

    ITF_INLINE bbool isNormalized() const
    {
        const f32 sqrNorm = sqrnorm();
        return F32_ALMOST_EQUALS(sqrNorm, 1.0f, MTH_EPSILON);
    }

    /// Gets the angle of a normalized vector. Angle is oriented relative to Vec2d::Right
    ITF_INLINE f32 getAngle() const
    {
        return getOrientedAngle(Right);
    }

    ITF_INLINE f32 getAngleOpt() const
    {
        const f32 angle = f32_ACosOpt(m_x);
        return f32_Sel(m_y, angle, -angle);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets an oriented angle between two normalized vectors.
    /// @param  _v  The other vector.
    /// @return The oriented angle.
    f32 getOrientedAngle(const Vec2d& _v) const
    {
#ifdef _DEBUG
        ITF_ASSERT_MSG(isNormalized() && _v.isNormalized(), "Normalize your vectors first !");
#endif // _DEBUG
        const f32 angle = f32_ACos(dot(_v));
        return (cross(_v) < 0.0f ? angle : -angle);
    }

    f32 getShortestAngle (const Vec2d& _v) const
    {
        f32 angle = f32_ACos(dot(_v));
        ITF_ASSERT(angle >= 0.0f);
        ITF_ASSERT(angle < MTH_2PI);
        return angle;
    }

    
    ITF_INLINE f32 cross(const Vec2d& _vec) const
    {
    #if defined( ITF_PSI )
        Vec2d v;
        Vec2d::SwapXY(&v, &_vec);
        Vec2d::Mul(&v, this, &v);
        return v.m_x - v.m_y;
    #else
        return (m_x * _vec.m_y) - (m_y * _vec.m_x);
    #endif
    }

    ITF_INLINE void clear()
    {
        m_x = 0.f;
        m_y = 0.f;
    }

    ITF_INLINE bool IsEqual( const Vec2d& _vec, f32 _epsilon = 0.f ) const
    {
        return f32_Abs(m_x-_vec.m_x) <= _epsilon &&
               f32_Abs(m_y-_vec.m_y) <= _epsilon;
    }


    ITF_INLINE Vec2d RotateCS( register f32 _c, register f32 _s ) const
    {
        Vec2d retV;

        #if defined( ITF_PSI )
		register float* pres = (float*)&retV.m_x;
		register __vec2x32float__ f_cs, f_msc, f_p0, f_p1, f_p2;
		register float* me = (float*)&this->m_x;
		register float ms = -_s;
		asm	{		
			psq_l      f_p1,  0(me), 0, 0;       //load vector
			ps_merge00 f_cs,     _c, _s;
			ps_merge00 f_msc,    ms, _c;
			ps_muls0   f_p2,   f_cs, f_p1;       //           m_x*_c | m_x*_s
			ps_madds1  f_p0,  f_msc, f_p1, f_p2; // -m_y*_s + m_x*_c | m_x*_s + m_y*_c			
			psq_st     f_p0,  0(pres), 0, 0;     //store result
		}
        #else
        
        retV.m_x =  m_x*_c - m_y*_s;
        retV.m_y =  m_x*_s + m_y*_c;
        
        #endif
        return retV;
    }
    
    ITF_INLINE Vec2d RotateCS( const Vec2d & _cs ) const
    {
        Vec2d retV;

        #if defined( ITF_PSI )
		register float* pres = (float*)&retV.m_x;
		register __vec2x32float__ f_cs, f_msc, f_p0, f_p1, f_p2;
		register float* me = (float*)&this->m_x;
		register float* cs = (float*)&_cs.m_x;

		asm	{		
			psq_l      f_p1,  0(me), 0, 0;       //load vector
			psq_l      f_cs,  0(cs), 0, 0;       // _c | _s

            ps_neg     f_msc, f_cs;
			ps_merge10 f_msc,    f_msc, f_cs; // -_s | _c

			ps_muls0   f_p2,   f_cs, f_p1;       //           m_x*_c | m_x*_s
			ps_madds1  f_p0,  f_msc, f_p1, f_p2; // -m_y*_s + m_x*_c | m_x*_s + m_y*_c			
			psq_st     f_p0,  0(pres), 0, 0;     //store result
		}
        #else
        
        retV.m_x =  m_x*_cs.m_x - m_y*_cs.m_y;
        retV.m_y =  m_x*_cs.m_y + m_y*_cs.m_x;
        
        #endif
        return retV;
    }

    ITF_INLINE Vec2d Rotate( f32 _rad ) const
    {
        Vec2d cs;
        Vec2d::CosSin(&cs, _rad);
        return RotateCS( cs ) ;
    }


    ITF_INLINE Vec2d RotateAroundCS( const Vec2d & center, f32 _c, f32 _s ) const
    {
        Vec2d swapC = *this - center;
        Vec2d s(-_s, _s);
        Vec2d retV;

        //retV.m_x = center.m_x - swapC.m_y*_s;
        //retV.m_x = center.m_y + swapC.m_x*_s;
        Vec2d::SwapXY(&retV, &swapC);
        Vec2d::MulAdd(&retV, &retV, &s, &center);

        //retV.m_x +=  swapC.m_x*_c;
        //retV.m_y +=  swapC.m_y*_c;
        Vec2d::ScaleAdd(&retV, &swapC, _c, &retV);

        //retV.m_x =  center.m_x + swapC.m_x*_c - swapC.m_y*_s;
        //retV.m_y =  center.m_y + swapC.m_y*_c + swapC.m_x*_s;

        return retV;
    }

    ITF_INLINE Vec2d RotateAroundCS( const Vec2d & center, const Vec2d & cs ) const
    {
        Vec2d retV;
        Vec2d swapC = *this - center;

        Vec2d s = -cs;
        Vec2d::MergeYY(&s, &s, &cs); // s = -sinus | sinus

        //retV.m_x = center.m_x - swapC.m_y*_s;
        //retV.m_x = center.m_y + swapC.m_x*_s;
        Vec2d::SwapXY(&retV, &swapC);
        Vec2d::MulAdd(&retV, &retV, &s, &center);

        //retV.m_x +=  swapC.m_x*_c;
        //retV.m_y +=  swapC.m_y*_c;
        Vec2d::MulXAdd(&retV, &swapC, &cs, &retV);

        //retV.m_x =  center.m_x + swapC.m_x*_c - swapC.m_y*_s;
        //retV.m_y =  center.m_y + swapC.m_y*_c + swapC.m_x*_s;

        return retV;
    }

    ITF_INLINE Vec2d RotateAround( const Vec2d & center, f32 _rad ) const
    {
        Vec2d vCS;
        Vec2d::CosSin(&vCS, _rad);
        return RotateAroundCS(center, vCS) ;
    }

    ITF_INLINE Vec2d abs() const
    {
        Vec2d res;
        Vec2d::Abs(&res, this);
		return res;
    }

    static ITF_INLINE void Sel(register Vec2d* _dst, register f32 _selector, register Vec2d* _posSelected, register Vec2d* _negSelected)
    {
        #if defined( ITF_PSI )
        register __vec2x32float__ f_p0, f_p1, f_p2;
        asm {
            psq_l      f_p1,  0(_posSelected), 0, 0;       //load vector
            psq_l      f_p2,  0(_negSelected), 0, 0;       //load vector
            ps_sel     f_p0, _selector, f_p1, f_p2;
            psq_st     f_p0,  0(_dst), 0, 0;                //store result
        }

        #else

        _dst->m_x = f32_Sel(_selector, _posSelected->m_x, _negSelected->m_x);
        _dst->m_y = f32_Sel(_selector, _posSelected->m_y, _negSelected->m_y);

        VECTOR_CHECK_DST

        #endif
    }

    static ITF_INLINE void Sel(register Vec2d* _dst, register Vec2d* _selector, register Vec2d* _posSelected, register Vec2d* _negSelected)
    {
        #if defined( ITF_PSI )
        register __vec2x32float__ f_p0, f_p1, f_p2, f_p3;
        asm {
            psq_l      f_p1,  0(_posSelected), 0, 0;       //load vector
            psq_l      f_p2,  0(_negSelected), 0, 0;       //load vector
            psq_l      f_p3,  0(_selector), 0, 0;          //load vector
            ps_sel     f_p0, f_p3, f_p1, f_p2;
            psq_st     f_p0,  0(_dst), 0, 0;               //store result
        }

        #else

        _dst->m_x = f32_Sel(_selector->m_x, _posSelected->m_x, _negSelected->m_x);
        _dst->m_y = f32_Sel(_selector->m_y, _posSelected->m_y, _negSelected->m_y);

        VECTOR_CHECK_DST

        #endif
    }

    static ITF_INLINE void ClampMin0(register Vec2d* _dst, const register Vec2d* _v)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_zero, f_p0, f_p1;
		asm	{		
			psq_l      f_p1,  0(_v), 0, 0;       //load vector
			ps_sub     f_zero, f_zero, f_zero;   // zero
            ps_sel     f_p0,   f_p1, f_p1, f_zero;
            psq_st     f_p0,  0(_dst), 0, 0;     //store result
		}
        #else
        
        _dst->m_x = (_v->m_x >= 0.f) ? _v->m_x : 0.f;
        _dst->m_y = (_v->m_y >= 0.f) ? _v->m_y : 0.f;
        
        VECTOR_CHECK_DST
                     
        #endif
    }

    static ITF_INLINE void Max(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _b)
    {    
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2, f_p3;
		asm	{
			psq_l      f_p1,  0(_a), 0, 0;       //load vector
			psq_l      f_p2,  0(_b), 0, 0;       //load vector
			ps_sub     f_p3,   f_p1, f_p2;       // _a - _b
			ps_sel     f_p0,   f_p3, f_p1, f_p2; // max
			psq_st     f_p0,  0(_dst), 0, 0;     //store result
		}
        #else
        
        _dst->m_x = (_a->m_x >= _b->m_x) ? _a->m_x : _b->m_x;
        _dst->m_y = (_a->m_y >= _b->m_y) ? _a->m_y : _b->m_y;
        
        VECTOR_CHECK_DST

        #endif
    }
    
    static ITF_INLINE void Min(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _b)
    {    
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2, f_p3;
		asm	{		
			psq_l      f_p1,  0(_a), 0, 0;       //load vector
			psq_l      f_p2,  0(_b), 0, 0;       //load vector
			ps_sub     f_p3,   f_p1, f_p2;       // _a - _b
			ps_sel     f_p0,   f_p3, f_p2, f_p1; // min
			psq_st     f_p0,  0(_dst), 0, 0;     //store result
		}
        #else
        
        _dst->m_x = (_a->m_x < _b->m_x) ? _a->m_x : _b->m_x;
        _dst->m_y = (_a->m_y < _b->m_y) ? _a->m_y : _b->m_y;
        
        VECTOR_CHECK_DST

        #endif
    }    

	static ITF_INLINE void MaxScalar(register Vec2d* _dst, const register Vec2d* _v, f32 register _Max)
	{
#if defined( ITF_PSI )
		register __vec2x32float__ f_Max, f_p0, f_p1,f_Comp;
		asm	{		
			psq_l      f_p1,  0(_v), 0, 0;       //load vector
			ps_merge00 f_Max, _Max, _Max;		 // Store _Max's fp0 in f_Max's fp0 and fp1.
			ps_sub	   f_Comp, f_p1, f_Max;
			ps_sel     f_p0,   f_Comp, f_p1, f_Max; 
			psq_st     f_p0,  0(_dst), 0, 0;     //store result
		}
#else

		_dst->m_x = (_v->m_x > _Max) ? _v->m_x : _Max;
		_dst->m_y = (_v->m_y > _Max) ? _v->m_y : _Max;

		VECTOR_CHECK_DST

#endif
	}

	static ITF_INLINE void MinScalar(register Vec2d* _dst, const register Vec2d* _v, f32 register _Min)
	{
#if defined( ITF_PSI )
		register __vec2x32float__ f_Min, f_p0, f_p1,f_Comp;
		asm	{		
			psq_l      f_p1,  0(_v), 0, 0;       //load vector
			ps_merge00 f_Min, _Min, _Min;		 // Store _Min's fp0 in f_Min's fp0 and fp1.
			ps_sub	   f_Comp, f_p1, f_Min;
			ps_sel     f_p0,   f_Comp, f_Min, f_p1; 
			psq_st     f_p0,  0(_dst), 0, 0;     //store result
		}
#else

		_dst->m_x = (_v->m_x < _Min) ? _v->m_x : _Min;
		_dst->m_y = (_v->m_y < _Min) ? _v->m_y : _Min;

		VECTOR_CHECK_DST

#endif
	}

    static ITF_INLINE void Add(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_b), 0, 0;
			ps_add     f_p0, f_p1, f_p2
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x + _b->m_x;
        _dst->m_y = _a->m_y + _b->m_y;
        
        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void Add(register Vec2d* _dst, const register Vec2d* _a, const register f32 _f)
    {    
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1, f_p2;
		asm	{		
			psq_l      f_p1,  0(_a), 0, 0; //load vector
			ps_merge00 f_p2,  _f, _f;
			ps_add     f_p1,   f_p1, f_p2; //add
			psq_st     f_p1,  0(_dst), 0, 0; //store result
		}
		#else
		
        _dst->m_x = _a->m_x + _f;
        _dst->m_y = _a->m_y + _f;
        
        VECTOR_CHECK_DST
		
		#endif
	}

    static ITF_INLINE void Sub(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_b), 0, 0;
			ps_sub     f_p0, f_p1, f_p2
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x - _b->m_x;
        _dst->m_y = _a->m_y - _b->m_y;
        
        VECTOR_CHECK_DST
        #endif
    }

    static ITF_INLINE void Sub(register Vec2d* _dst, const register Vec2d* _a, const register f32 _f)
    {    
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1, f_p2;
		asm	{		
			psq_l      f_p1,  0(_a), 0, 0; //load vector
			ps_merge00 f_p2,  _f, _f;
			ps_sub     f_p1,   f_p1, f_p2; //sub
			psq_st     f_p1,  0(_dst), 0, 0; //store result
		}
		#else
		
        _dst->m_x = _a->m_x - _f;
        _dst->m_y = _a->m_y - _f;


        VECTOR_CHECK_DST
		
		#endif
	}

    static ITF_INLINE void Div(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_b), 0, 0;
			ps_div     f_p0, f_p1, f_p2
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x / _b->m_x;
        _dst->m_y = _a->m_y / _b->m_y;

        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void Mul(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_b), 0, 0;
			ps_mul     f_p0, f_p1, f_p2
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x * _b->m_x;
        _dst->m_y = _a->m_y * _b->m_y;

        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void MulAdd(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _b, const register Vec2d* _c)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2, f_p3;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_b), 0, 0;
			psq_l      f_p3,  0(_c), 0, 0;
			ps_madd    f_p0, f_p1, f_p2, f_p3
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x * _b->m_x + _c->m_x;
        _dst->m_y = _a->m_y * _b->m_y + _c->m_y;
    
        VECTOR_CHECK_DST
        #endif
    }
    
    static ITF_INLINE void MulXAdd(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _bX, const register Vec2d* _c)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2, f_p3;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_bX), 0, 0;
			psq_l      f_p3,  0(_c), 0, 0;
			ps_madds0  f_p0, f_p1, f_p2, f_p3
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x * _bX->m_x + _c->m_x;
        _dst->m_y = _a->m_y * _bX->m_x + _c->m_y;
    
        VECTOR_CHECK_DST
        #endif
    }

    static ITF_INLINE void MulYAdd(register Vec2d* _dst, const register Vec2d* _a, const register Vec2d* _bY, const register Vec2d* _c)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2, f_p3;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_bY), 0, 0;
			psq_l      f_p3,  0(_c), 0, 0;
			ps_madds1  f_p0, f_p1, f_p2, f_p3
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x * _bY->m_y + _c->m_x;
        _dst->m_y = _a->m_y * _bY->m_y + _c->m_y;
    
        VECTOR_CHECK_DST
        #endif
    }

    static ITF_INLINE void Scale(register Vec2d* _dst, const register Vec2d* _a, register f32 _s)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			ps_muls0   f_p0, f_p1, _s
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x * _s;
        _dst->m_y = _a->m_y * _s;
        
        VECTOR_CHECK_DST
        
        #endif
    }    

    static ITF_INLINE Vec2d ScaleX(const register Vec2d* _v1, const register Vec2d* _v2)
    {
        #if defined( ITF_PSI )

        Vec2d res;
        register float* pres = (float*)&res.m_x;
        register __vec2x32float__ f_p1, f_p2;
        asm {
            psq_l      f_p1,  0(_v1), 0, 0;
            psq_l      f_p2,  0(_v2), 0, 0;
            ps_muls0   f_p1, f_p1, f_p2
            psq_st     f_p1,  0(pres), 0, 0;
            }
        return res;

        #else

        return Vec2d(_v1->m_x*_v2->m_x, _v1->m_y*_v2->m_x);

        #endif
    }

    static ITF_INLINE Vec2d ScaleY(const register Vec2d* _v1, const register Vec2d* _v2)
    {
        #if defined( ITF_PSI )

        Vec2d res;
        register float* pres = (float*)&res.m_x;
        register __vec2x32float__ f_p1, f_p2;
        asm {
            psq_l      f_p1,  0(_v1), 0, 0;
            psq_l      f_p2,  0(_v2), 0, 0;
            ps_muls1   f_p1, f_p1, f_p2
            psq_st     f_p1,  0(pres), 0, 0;
        }
        return res;

        #else

        return Vec2d(_v1->m_x*_v2->m_y, _v1->m_y*_v2->m_y);

        #endif
    }

    static ITF_INLINE void ScaleAdd(register Vec2d* _dst, const register Vec2d* _a, register f32 _s, const register Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm {
			psq_l      f_p1,  0(_a), 0, 0;
			psq_l      f_p2,  0(_b), 0, 0;
			ps_madds0  f_p0, f_p1, _s, f_p2
			psq_st     f_p0,  0(_dst), 0, 0;
		}		
        #else
        
        _dst->m_x = _a->m_x * _s + _b->m_x;
        _dst->m_y = _a->m_y * _s + _b->m_y;

        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void Abs(register Vec2d* _dst, const register Vec2d* _a)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1;
		asm	{		
			psq_l      f_p1,  0(_a), 0, 0;   //load vector
			ps_abs     f_p0,   f_p1          //abs
			psq_st     f_p0,  0(_dst), 0, 0; //store result
		}
        #else
        
        _dst->m_x = f32_Abs(_a->m_x);
        _dst->m_y = f32_Abs(_a->m_y);
            
        VECTOR_CHECK_DST
        #endif
    }
    
    static ITF_INLINE f32 Dot(register const Vec2d* _a, register const Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		register f32 res;
		asm	{		
			psq_l      f_p1,  0(_a), 0, 0;       // load vector 
			psq_l      f_p2,  0(_b), 0, 0;       // load vector 
			ps_mul     f_p0,   f_p1, f_p2;       // mul _a->m_x*_b->m_x | _a->m_y*_b->m_y 
			ps_sum0     res,   f_p0, f_p0, f_p0; // _a->m_x*_b->m_x + _a->m_y*_b->m_y 
		}
		return res;
        #else
        
        return _a->m_x*_b->m_x + _a->m_y*_b->m_y;
        
        #endif    
    }
    
    static ITF_INLINE void SwapXY(register Vec2d* _dst, register const Vec2d* _src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1;
		asm	{		
			psq_l      f_p0,  0(_src), 0, 0;   // load vector 			        
            ps_merge10 f_p1,     f_p0, f_p0;
            psq_st     f_p1,  0(_dst), 0, 0;
        }
        #else
        
        f32 srcx = _src->m_x;        
        _dst->m_x = _src->m_y;
        _dst->m_y = srcx;
        
        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void SwapXmY(register Vec2d* _dst, register const Vec2d* _src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm	{		
			psq_l      f_p0,  0(_src), 0, 0;   // load vector
			ps_neg     f_p1,  f_p0;
            ps_merge10 f_p2,  f_p1, f_p0;
            psq_st     f_p2,  0(_dst), 0, 0;
        }
        #else
        
        f32 srcx = _src->m_x;        
        _dst->m_x = -_src->m_y;
        _dst->m_y = srcx;
        
        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void SwapmXY(register Vec2d* _dst, register const Vec2d* _src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm	{		
			psq_l      f_p0,  0(_src), 0, 0;   // load vector
			ps_neg     f_p1,  f_p0;
            ps_merge10 f_p2,  f_p0, f_p1;
            psq_st     f_p2,  0(_dst), 0, 0;
        }
        #else
        
        f32 srcx =  _src->m_x;        
        _dst->m_x = _src->m_y;
        _dst->m_y = -srcx;
        
        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void MergeXX(register Vec2d* _dst, register const Vec2d* _a, register const Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm	{		
			psq_l      f_p0,  0(_a), 0, 0;   // load vector
			psq_l      f_p1,  0(_b), 0, 0;   // load vector
            ps_merge00 f_p2,   f_p0, f_p1;
            psq_st     f_p2,  0(_dst), 0, 0;
        }
        #else
        
        _dst->m_y = _b->m_x;
        _dst->m_x = _a->m_x;
        
        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void MergeYY(register Vec2d* _dst, register const Vec2d* _a, register const Vec2d* _b)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p0, f_p1, f_p2;
		asm	{		
			psq_l      f_p0,  0(_a), 0, 0;   // load vector
			psq_l      f_p1,  0(_b), 0, 0;   // load vector
            ps_merge11 f_p2,   f_p0, f_p1;
            psq_st     f_p2,  0(_dst), 0, 0;
        }
        #else
        
        _dst->m_x = _a->m_y;
        _dst->m_y = _b->m_y;
        
        VECTOR_CHECK_DST
        
        #endif
    }

    static ITF_INLINE void NegY(register Vec2d* _dst, register const Vec2d* _src)
    {
#if defined( ITF_PSI )
        register __vec2x32float__ f_p0, f_p1, f_p2;
        asm	{		
            psq_l      f_p0,  0(_src), 0, 0;   // load vector
            ps_neg     f_p1,  f_p0;
            ps_merge01 f_p2,  f_p0, f_p1;
            psq_st     f_p2,  0(_dst), 0, 0;
        }
#else

        _dst->m_x =   _src->m_x;
        _dst->m_y = - _src->m_y;

        VECTOR_CHECK_DST

#endif
    }

    static ITF_INLINE Vec2d mergeXY(register const Vec2d* _v1, register const Vec2d* _v2)
    {
#if defined( ITF_PSI )
        Vec2d res;
        register float* pres = (float*)&res.m_x;
        register __vec2x32float__ f_p0, f_p1, f_p2;
        asm	{		
            psq_l      f_p1,  0(_v1), 0, 0;
            psq_l      f_p2,  0(_v2), 0, 0;
            ps_merge01 f_p0,   f_p1, f_p2;
            psq_st     f_p0,  0(pres), 0, 0;
        }
        return res;
#else
        return Vec2d(_v1->m_x, _v2->m_y);
#endif
    }

    static ITF_INLINE Vec2d mergeYX(register const Vec2d* _v1, register const Vec2d* _v2)
    {
#if defined( ITF_PSI )
        Vec2d res;
        register float* pres = (float*)&res.m_x;
        register __vec2x32float__ f_p0, f_p1, f_p2;
        asm	{		
            psq_l      f_p1,  0(_v1), 0, 0;
            psq_l      f_p2,  0(_v2), 0, 0;
            ps_merge10 f_p0,   f_p1, f_p2;
            psq_st     f_p0,  0(pres), 0, 0;
        }
        return res;
#else
        return Vec2d(_v1->m_y, _v2->m_x);
#endif
    }

    static ITF_INLINE f32 Normalize(register Vec2d* _dst, register const Vec2d* _src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ fXY, fXYres;
		register f32 fResult;
		asm {			
			psq_l	fXY, 0(_src), 0, 0                  // load XY
			ps_mul  fResult, fXY, fXY                   // sqr XY
			ps_sum0 fResult, fResult, fResult, fResult  // add X² + Y²
		}

        register f32 invsqrt = f32_InvSqrt(fResult);
        asm {
            ps_muls0   fXYres, fXY, invsqrt
            psq_st     fXYres,  0(_dst), 0, 0;
        }
        return f32_Inv(invsqrt);

        #else
        
        const f32 n = f32_Sqrt(_src->m_x*_src->m_x+_src->m_y*_src->m_y);
        *_dst = (*_src)/n;
        return n;
        #endif        
    }

    static ITF_INLINE void Lerp(register Vec2d* _dst, register const Vec2d* _A, register const Vec2d* _B, register f32 _st)
    {
        Vec2d tmp;
        Vec2d::Sub(&tmp, _B, _A);
        Vec2d::ScaleAdd(_dst, &tmp, _st, _A);
    }


    static ITF_INLINE void to3d(register Vec3d* _dst, register const Vec2d* _v, const f32 _z = 0.0f)
    {
#if defined( ITF_PSI )
        register __vec2x32float__ f_p1;
        asm	{
            psq_l     f_p1,  0(_v), 0, 0;
            psq_st    f_p1,  0(_dst), 0, 0;
        }
        _dst->m_z = _z;
#else
        _dst->m_x = _v->m_x;
        _dst->m_y = _v->m_y;
        _dst->m_z = _z;
#endif
    }

#if defined( ITF_PSI )
    static void SinCos(register Vec2d* _dst, register f32 _angle);
    static void CosSin(register Vec2d* _dst, register f32 _angle);   
    static void SinCosOpt(register Vec2d* _dst, register f32 _angle);
    static void CosSinOpt(register Vec2d* _dst, register f32 _angle);
#else
    static inline void SinCos(Vec2d* _dst, f32 _angle);
    static inline void CosSin(Vec2d* _dst, f32 _angle);   
    static inline void SinCosOpt(Vec2d* _dst, f32 _angle);
    static inline void CosSinOpt(Vec2d* _dst, f32 _angle);
#endif

    
    static const Vec2d XAxis;
    static const Vec2d YAxis;
    static const Vec2d Zero;
    static const Vec2d Up;
    static const Vec2d Right;
    static const Vec2d One;
    static const Vec2d Infinity;
};

ITF_INLINE const Vec2d operator*(const register f32 _s, const Vec2d& _vec)
{
    Vec2d res;
    Vec2d::Scale(&res, &_vec, _s);
	return res;
}

#if !defined( ITF_PSI )
inline void Vec2d::SinCos(Vec2d* _dst, f32 _angle)
{
    f32_CosSin(_angle, &_dst->m_y, &_dst->m_x);
    VECTOR_CHECK_DST
}

inline void Vec2d::CosSin(Vec2d* _dst, f32 _angle)
{
    f32_CosSin(_angle, &_dst->m_x, &_dst->m_y);
    VECTOR_CHECK_DST
}

inline void Vec2d::SinCosOpt(Vec2d* _dst, f32 _angle)
{
    f32_CosSinOpt(_angle, &_dst->m_y, &_dst->m_x);
    VECTOR_CHECK_DST
}

inline void Vec2d::CosSinOpt(Vec2d* _dst, f32 _angle)
{
    f32_CosSinOpt(_angle, &_dst->m_x, &_dst->m_y);
    VECTOR_CHECK_DST
}
#endif

ITF_INLINE void Vec3d::truncateTo2D(Vec2d& _to) const 
{
    _to = *(Vec2d*)&m_x;
}

ITF_INLINE void Vec3d::setFromVec2d( const Vec2d& _vec, const f32* _z /* = NULL*/ )
{
    *(Vec2d*)&m_x = _vec;
    if(_z)
        m_z = (*_z);
}

} // namespace ITF

#endif // _ITF_VEC2D_H_
