///////////////////////////////////////////////////////////////////////////////
///////////////////////       Basic vector class
///////////////////////////////////////////////////////////////////////////////

#ifndef _ITF_VEC3D_H_
#define _ITF_VEC3D_H_

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
    #ifdef ITF_PS3
        #include <fastmath.h>
    #else
        #include <math.h>
    #endif
#endif

#ifndef _ITF_MATHF32_H_
#include "core/math/Mathf32.h"
#endif _ITF_MATHF32_H_

#ifndef _ITF_ARCHIVEMEMORY_H_
#include "core/ArchiveMemory.h"
#endif //_ITF_ARCHIVEMEMORY_H_

namespace ITF
{

class Vec2d;

class Vec3d
{
public:
    f32 m_x, m_y, m_z;

    Vec3d() : m_x(0.0f), m_y(0.0f), m_z(0.0f)  {}

#if defined( ITF_WII )
    ITF_INLINE Vec3d(const Vec3d& src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1, f_p2;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&src.m_x;	
		asm	{		
			psq_l     f_p1,  0(pv), 0, 0; //load XY
			psq_l     f_p2,  8(pv), 1, 0; //load Z
			psq_st    f_p1,  0(me), 0, 0; //store XY
			psq_st    f_p2,  8(me), 1, 0; //store Z
		}

        #else

        m_x = src.m_x;
        m_y = src.m_y;
        m_z = src.m_z;
        #endif
    }
#endif

    ITF_INLINE bbool isValid() const
    {
        return !(ITF_isnan(m_x) || ITF_isnan(m_y) || ITF_isnan(m_z));
    }

    Vec3d(const f32 x, const f32 y, const f32 z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    const Vec3d& operator = (const Vec3d& src)
    {
        #if defined( ITF_PSI )
		register __vec2x32float__ f_p1, f_p2;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&src.m_x;	
		asm	{		
			psq_l     f_p1,  0(pv), 0, 0; //load XY
			psq_l     f_p2,  8(pv), 1, 0; //load Z
			psq_st    f_p1,  0(me), 0, 0; //store XY
			psq_st    f_p2,  8(me), 1, 0; //store Z
		}

        #else

        m_x = src.m_x;
        m_y = src.m_y;
        m_z = src.m_z;
        #endif
        return *this;
    }

    void set(const f32 x, const f32 y, const f32 z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    f32 getX() const { return m_x; }
    f32 getY() const { return m_y; }
    f32 getZ() const { return m_z; }

    void setX(f32 _x) { m_x = _x; }
    void setY(f32 _y) { m_y = _y; }
    void setZ(f32 _z) { m_z = _z; }

    void serialize(ArchiveMemory& _archive)
    {
        _archive.serialize(m_x);
        _archive.serialize(m_y);
        _archive.serialize(m_z);
    }

    void setFromVec2d(const Vec2d& _vec, const f32* _z = NULL);

    inline Vec3d& operator+=(const Vec3d& v)
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&v.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  0(pv), 0, 0; //load XY
			psq_l     f_p3,  8(me), 1, 0; //load Z			
			psq_l     f_p4,  8(pv), 1, 0; //load Z
			
			ps_add    f_p1,   f_p1, f_p2; //add XY
			ps_add    f_p3,   f_p3, f_p4; //add Z
			
			psq_st    f_p1,  0(me), 0, 0; //store XY
			psq_st    f_p3,  8(me), 1, 0; //store Z
		}
#else
        m_x+=v.m_x;
        m_y+=v.m_y;
        m_z+=v.m_z;
#endif
        return *this;
    }

    inline Vec3d& operator+=(const register f32 v)
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3;
		register float* me = (float*)&this->m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  8(me), 1, 0; //load Z
			ps_merge00 f_p3, v, v;
			fadds	  f_p2, f_p2, v;      // Add Z
			ps_add    f_p1, f_p1, f_p3;   // Add XY
								
			psq_st    f_p2,  8(me), 1, 0; //store Z
			psq_st    f_p1,  0(me), 0, 0; //store XY
		}
#else
        m_x+=v;
        m_y+=v;
        m_z+=v;
#endif
        return *this;
    }

    inline Vec3d operator+(const Vec3d& v) const
    {
#if defined( ITF_PSI__ )
        Vec3d res;
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4, f_p5, f_p6;
		register float* pr = (float*)&res.m_x;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&v.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  0(pv), 0, 0; //load XY
			psq_l     f_p3,  8(me), 1, 0; //load Z
			psq_l     f_p4,  8(pv), 1, 0; //load Z
			
			ps_add    f_p5,   f_p1, f_p2; //add XY
			ps_add    f_p6,   f_p3, f_p4; //add Z
			
			psq_st    f_p5,  0(pr), 0, 0; //store XY
			psq_st    f_p6,  8(pr), 1, 0; //store Z
		}
		return res;
#else
        return Vec3d(m_x+v.m_x, m_y+v.m_y, m_z+v.m_z);
#endif        
    }

    inline Vec3d& operator-=(const Vec3d& v)
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&v.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  0(pv), 0, 0; //load XY
			psq_l     f_p3,  8(me), 1, 0; //load Z			
			psq_l     f_p4,  8(pv), 1, 0; //load Z
			
			ps_sub    f_p1,   f_p1, f_p2; //sub XY
			ps_sub    f_p3,   f_p3, f_p4; //sub Z
			
			psq_st    f_p1,  0(me), 0, 0; //store XY
			psq_st    f_p3,  8(me), 1, 0; //store Z
		}
#else
        m_x-=v.m_x;
        m_y-=v.m_y;
        m_z-=v.m_z;
#endif
        return *this;
    }

    inline Vec3d& operator-=(register f32 v)
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3;
		register float* me = (float*)&this->m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  8(me), 1, 0; //load Z
			ps_merge00 f_p3, v, v;
			fsubs	  f_p2, f_p2, v;      // Sub Z
			ps_sub    f_p1, f_p1, f_p3;   // Sub XY
								
			psq_st    f_p2,  8(me), 1, 0; //store Z
			psq_st    f_p1,  0(me), 0, 0; //store XY
		}
#else
        m_x-=v;
        m_y-=v;
        m_z-=v;
#endif        
        return *this;
    }

    inline Vec3d getPerpendicular() const
    {
        return Vec3d(-m_y, m_x, m_z);
    }


    inline Vec3d operator-(const Vec3d& v) const
    {
#if defined( ITF_PSI__ )
        Vec3d res;
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4, f_p5, f_p6;
		register float* pr = (float*)&res.m_x;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&v.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  0(pv), 0, 0; //load XY
			psq_l     f_p3,  8(me), 1, 0; //load Z
			psq_l     f_p4,  8(pv), 1, 0; //load Z
			
			ps_sub    f_p5,   f_p1, f_p2; //sub XY
			ps_sub    f_p6,   f_p3, f_p4; //sub Z
			
			psq_st    f_p5,  0(pr), 0, 0; //store XY
			psq_st    f_p6,  8(pr), 1, 0; //store Z
		}
		return res;
#else    
        return Vec3d(m_x-v.m_x, m_y-v.m_y, m_z-v.m_z);
#endif        
    }

    inline Vec3d operator-() const
    {
        return Vec3d(-m_x, -m_y, -m_z);
    }

    inline Vec3d& operator*=(const register f32 s)
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3;
		register float* me = (float*)&this->m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  8(me), 1, 0; //load Z
			ps_merge00 f_p3, s, s;
			fmuls	  f_p2, f_p2, s;      // Mul Z
			ps_mul    f_p1, f_p1, f_p3;   // Mul XY
								
			psq_st    f_p2,  8(me), 1, 0; //store Z
			psq_st    f_p1,  0(me), 0, 0; //store XY
		}
#else
        m_x*=s;
        m_y*=s;
        m_z*=s;
#endif
        return *this;
    }

    inline Vec3d operator*=(const Vec3d& v)
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&v.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  0(pv), 0, 0; //load XY
			psq_l     f_p3,  8(me), 1, 0; //load Z			
			psq_l     f_p4,  8(pv), 1, 0; //load Z
			
			ps_mul    f_p1,   f_p1, f_p2; //mul XY
			ps_mul    f_p3,   f_p3, f_p4; //mul Z
			
			psq_st    f_p1,  0(me), 0, 0; //store XY
			psq_st    f_p3,  8(me), 1, 0; //store Z
		}
#else
        m_x*=v.m_x;
        m_y*=v.m_y;
        m_z*=v.m_z;
#endif        
        return *this;
    }

    inline Vec3d operator*(const register f32 s) const
    {
#if defined( ITF_PSI__ )
        Vec3d res;
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4, f_p5;
		register float* me = (float*)&this->m_x;
		register float* pr = (float*)&res.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  8(me), 1, 0; //load Z
			ps_merge00 f_p3, s, s;
			fmuls	  f_p4, f_p2, s;      // Mul Z
			ps_mul    f_p5, f_p1, f_p3;   // Mul XY
								
			psq_st    f_p4,  8(pr), 1, 0; //store Z
			psq_st    f_p5,  0(pr), 0, 0; //store XY
		}
		return res;
#else
        return Vec3d(m_x*s, m_y*s, m_z*s);
#endif        
    }

    inline Vec3d operator^(const Vec3d& v) const 
    { 
        return Vec3d((m_y * v.m_z) - (m_z * v.m_y),
                     (m_z * v.m_x) - (m_x * v.m_z),
                     (m_x * v.m_y) - (m_y * v.m_x));
    }
    
    inline Vec3d operator/=(const f32 s)
    {
        m_x/=s;
        m_y/=s;
        m_z/=s;
        return *this;
    }

    inline Vec3d operator/=(const Vec3d& v)
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&v.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  0(pv), 0, 0; //load XY
			psq_l     f_p3,  8(me), 1, 0; //load Z			
			psq_l     f_p4,  8(pv), 1, 0; //load Z
			
			ps_div    f_p1,   f_p1, f_p2; //div XY
			ps_div    f_p3,   f_p3, f_p4; //div Z
			
			psq_st    f_p1,  0(me), 0, 0; //store XY
			psq_st    f_p3,  8(me), 1, 0; //store Z
		}
#else    
        m_x/=v.m_x;
        m_y/=v.m_y;
        m_z/=v.m_z;
#endif        
        return *this;
    }

    inline Vec3d operator/(const register f32 s) const
    {
#if defined( ITF_PSI__ )
        Vec3d res;
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4, f_p5;
		register float* me = (float*)&this->m_x;
		register float* pr = (float*)&res.m_x;
		asm	{		
			psq_l     f_p1,  0(me), 0, 0; //load XY
			psq_l     f_p2,  8(me), 1, 0; //load Z
			ps_merge00 f_p3, s, s;
			fdivs	  f_p4, f_p2, s;      // Mul Z
			ps_div    f_p5, f_p1, f_p3;   // Mul XY
								
			psq_st    f_p4,  8(pr), 1, 0; //store Z
			psq_st    f_p5,  0(pr), 0, 0; //store XY
		}
		return res;
#else    
        return Vec3d(m_x/s, m_y/s, m_z/s);
#endif        
    }

    inline bool operator==(const Vec3d& v) const
    {
        return (m_x == v.m_x) && (m_y == v.m_y) && (m_z == v.m_z);
    }

    inline bool operator!=(const Vec3d& v) const
    {
        return (m_x != v.m_x) || (m_y != v.m_y) || (m_z != v.m_z);
    }

    inline f32 norm() const
    {
        return f32_Sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
    }

    inline f32 sqrnorm() const
    {
#if defined( ITF_PSI__ )	    
		register __vec2x32float__ f_p0, f_p1, f_p2;
		register float* me = (float*)&this->m_x;
	    register f32 result;				    
    	asm
    	{
    		psq_l       f_p0, 0(me), 0, 0;          // X | Y
    		lfs         f_p1, 8(me);                // Z | Z
    		ps_mul      f_p2, f_p0, f_p0;           // XX | YY
    		ps_madd     result, f_p1, f_p1, f_p2;   // XX + ZZ | YY + ZZ
    		ps_sum0     result, f_p2, f_p0, result; // XX + YY + ZZ | ...
    	}
    	return result;
#else	
        return m_x*m_x+m_y*m_y+m_z*m_z;
#endif
    }

    inline Vec3d& normalize()
    {    
#ifdef F32_WII
        const f32 n2 = sqrnorm();
        if ( n2 != 0.f )
            *this*=f32_InvSqrt(n2);
#else
        const f32 n = norm();
        if ( n != 0 )
        {
            m_x/=n;
            m_y/=n;
            m_z/=n;
        }
#endif
        else
            clear();
        return *this;
    }

    inline f32 f32_normalize()
    {    
#ifdef F32_WII
        const f32 n2 = sqrnorm();
        f32 n = 0.f;
        if ( n2 != 0.f )
        {
            n = f32_InvSqrt(n2);
            *this *= n;
        }

#else
        const f32 n = norm();
        if ( n != 0 )
        {
            m_x/=n;
            m_y/=n;
            m_z/=n;
        }
#endif
        else
            clear();
        return n;
    }

    inline f32 dot(const Vec3d& _v) const
    {
#if defined( ITF_PSI__ )
		register __vec2x32float__ f_p1, f_p2, f_p3, f_p4;
		register float* me = (float*)&this->m_x;
		register float* pv = (float*)&_v.m_x;
		register float r;
		asm	{		
			psq_l     f_p1,  4(me), 0, 0; // AY AZ
			psq_l     f_p2,  4(pv), 0, 0; // BY BZ
			
			psq_l     f_p3,  0(me), 0, 0; // AX | AY
			psq_l     f_p4,  0(pv), 0, 0; // BX | BY
			
			ps_mul    f_p1,   f_p1, f_p2; // AY*BY | AZ*BZ			
			
			ps_madd   f_p2, f_p3, f_p4, f_p1; // AX*BX + AY*BY | AY*BY + AZ*BZ
			ps_sum0      r, f_p2, f_p1, f_p1; // AX*BX + AY*BY + AZ*BZ | ... 
		}
		return r;
#else    
        return m_x*_v.m_x + m_y*_v.m_y + m_z*_v.m_z;
#endif        
    }

    ITF_INLINE const Vec2d& truncateTo2D() const
    {
        return *reinterpret_cast<const Vec2d*>(this);
    }

    void  truncateTo2D(Vec2d& _to) const;

    inline void setLength(f32 _len)
    {
        normalize();
        *this *= _len;
    }

    inline void clear()
    {
        m_x = 0;
        m_y = 0;
        m_z = 0;
    }

    inline void crossProduct(const Vec3d& _in, Vec3d& _out) const
    {
	    _out.m_x = ((m_y * _in.m_z) - (m_z * _in.m_y));
	    _out.m_y = ((m_z * _in.m_x) - (m_x * _in.m_z));
	    _out.m_z = ((m_x * _in.m_y) - (m_y * _in.m_x));
    }

    inline bool IsEqual( const Vec3d& _vec, f32 _epsilon = 0.f ) const
    {
        return f32_Abs(m_x-_vec.m_x) <= _epsilon &&
               f32_Abs(m_y-_vec.m_y) <= _epsilon &&
               f32_Abs(m_z-_vec.m_z) <= _epsilon;
    }

    static const Vec3d XAxis;
    static const Vec3d YAxis;
    static const Vec3d ZAxis;
    static const Vec3d Zero;
    static const Vec3d One;
    static const Vec3d Up;
    static const Vec3d Right;
    static const Vec3d Front;
    static const Vec3d Infinity;
};

ITF_INLINE const Vec3d operator*(const register f32 _s, const Vec3d& _vec)
{
#if defined( ITF_PSI__ )
    Vec3d res;
	register __vec2x32float__ f_p1, f_p2, f_p3;
	register const float* pv = (const float*)&_vec.m_x;
	register float* pr = (float*)&res.m_x;
	asm	{		
		psq_l     f_p1,  0(pv), 0, 0; //load XY
		psq_l     f_p2,  8(pv), 1, 0; //load Z
		ps_merge00 f_p3, _s, _s;
		fmuls	  f_p2, f_p2, _s;     // Mul Z
		ps_mul    f_p1, f_p1, f_p3;   // Mul XY
							
		psq_st    f_p2,  8(pr), 1, 0; //store Z
		psq_st    f_p1,  0(pr), 0, 0; //store XY
	}
    return res;
#else
    return Vec3d(_vec.m_x * _s, _vec.m_y * _s, _vec.m_z * _s);
#endif
}

} // namespace ITF

#endif // _ITF_VEC3D_H_
