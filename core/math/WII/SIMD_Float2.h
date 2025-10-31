
#ifndef __FLOAT2_BROADWAY__
#define __FLOAT2_BROADWAY__


#ifndef __OSFASTCAST_H__
#include <OSFastCast.h>
#endif


namespace ps
{

    #define OS_FASTCAST_S32  1  // Expect QPR 1 is reserved by Cw for int fats cast
    #define OS_FASTCAST_U16N 6  // Normalized unsigned short to float conversion.


    ///////////////////////////////////////////////////////////////////////////////
    // Float2 is an intrinsic data type holding 2 single precision floating point
    // numbers, X in low part and Y in high part.
    // !!! For data conversion functions, make sure OSInitFastCast() has been
    // called earlier.
    // Cast from float to Float2 is automatic and free, so you can use float arguments
    // with functions taking Float2 arguments. In fact, a float is a Float2 with the
    // same value in X and Y.
    // Actually, cast from Float2 to float is not possible.
    ///////////////////////////////////////////////////////////////////////////////

    #define PS_PARAM const&

    typedef __vec2x32float__ Float2;

    /*
        // Remember paired instructions

        ps_add   ps_sub      ps_mul      ps_muls0    psmuls1
        ps_madd	 ps_msub     ps_nmadd    ps_nmsub    ps_madds0  ps_madds1
        ps_sum0  ps_sum1
        ps_neg   ps_abs      ps_nabs     ps_sel
        ps_cmpo0 ps_cmpo1    ps_cmpu0    ps_cmpu1
        ps_mr    ps_merge00  ps_merge01  ps_merge10  ps_merge11
        ps_res   ps_rsqrte   ps_div
        psq_l    psq_lx      psq_lu      psq_lux
        psq_s    psq_sx      psq_su      psq_sux
    */


    void InitU16NConversion();

    Float2 Load( register float _a );
    Float2 Load( const float* _p );
    Float2 LoadX( const float* _p );
    Float2 LoadXX( const float* _p );
    Float2 LoadUChar2( const unsigned char* _p );
    Float2 LoadShort2( const short* _p );
    Float2 LoadUShort2( const unsigned short* _p );
    Float2 LoadXShort( const short* _p );
    Float2 LoadUShort2N( const unsigned short* _p );
    Float2 LoadXUShortN( const unsigned short* _p );

    void Store( Float2 PS_PARAM _a, float* _p );
    void StoreX( Float2 PS_PARAM _a, float* _p );
    void StoreUChar2( Float2 PS_PARAM _a, unsigned char* _p );
    void StoreShort2( Float2 PS_PARAM _a, short* _p );

    Float2 Merge  ( register float _a, register float _b );
    Float2 MergeXX( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 MergeYY( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 MergeXY( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 MergeYX( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 SplatX( Float2 PS_PARAM _a );
    Float2 SplatY( Float2 PS_PARAM _a );

    float GetX( Float2 PS_PARAM _a );      // Coz cast from Float2 to float is not automatic nor free
    float GetY( Float2 PS_PARAM _a );      // Coz cast from Float2 to float is not automatic nor free

    Float2 Add   ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Add   ( Float2 PS_PARAM _a, register float  _b );
    Float2 Adds  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Sub   ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Sub   ( Float2 PS_PARAM _a, register float  _b );
    Float2 Subs  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 SumX  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 SumY  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 Mul   ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Mul   ( Float2 PS_PARAM _a, register float  _b );
    Float2 Muls  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 MulX  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 MulY  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Madd  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 Madd  ( Float2 PS_PARAM _a, register float  _b, Float2 PS_PARAM _c );
    Float2 Nmadd ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 Madds ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 MaddX ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 MaddY ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 Msub  ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 Msubs ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 Nmsub ( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    Float2 Nmsubs( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );
    float  Dot   ( Float2 PS_PARAM _a, Float2 PS_PARAM _b);

    Float2 RsqrtEstimate( Float2 PS_PARAM _a );
    Float2 RcpEstimate( Float2 PS_PARAM _a );
    Float2 SqrtEstimate(Float2 PS_PARAM _a);
    Float2 Rsqrt( Float2 PS_PARAM _a );
    Float2 Rcp( Float2 PS_PARAM _a );
    Float2 Sqrt( Float2 PS_PARAM _a );
    Float2 Div( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Div( Float2 PS_PARAM _a, register float _b );

    Float2 Min( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Max( Float2 PS_PARAM _a, Float2 PS_PARAM _b );
    Float2 Abs( Float2 PS_PARAM _a );
    Float2 Neg( Float2 PS_PARAM _a );
    Float2 Select( Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c );



    ///////////////////////////////////////////////////////////////////////////////
    // Init unsigned short to float conversion with normalization.
    // conversion result = ((float)i) / 65536.0f
    // Remember that OSInitFastCast() has to be called for int8/uint8 and
    // int16/uint16 conversions.
    ///////////////////////////////////////////////////////////////////////////////
    inline void InitU16NConversion()
    {
        OSSetGQR6(OS_GQR_U16, OS_GQR_SCALE_65536);
    }


    ///////////////////////////////////////////////////////////////////////////////
    // Copy _a in both X and Y the result.
    // Return Float2 ( X=_a, Y=_a )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Load(register float _a)
    {
        register Float2 a(_a), res;
	    asm
	    {
		    ps_merge00  res, a, a
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 2 float from address _p.
    // Return Float2 ( X=_p[0], Y=_p[1] )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Load(const register float* _p)
    {
        register Float2 res;
	    asm
	    {
		    psq_l  res, 0(_p), 0, 0
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 1 float from address _p in X and set Y to 1.0f.
    // Return Float2 ( X=_p[0], Y=1.0f )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadX(const register float* _p)
    {
	    register Float2 res;
	    asm
	    {
		    psq_l  res, 0(_p), 1, 0
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 1 float from address _p in X and duplicate it in Y
    // Return Float2 ( X=_p[0], Y=_p[0] )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadXX(const register float* _p)
    {
	    register Float2 res;
	    asm
	    {
		    lfs  res, 0(_p)
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 2 8 bits unsigned integer from address _p, and convert them to float.
    // Return Float2 ( X=(float)_p[0], Y=(float)_p[1] )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadUChar2(const register unsigned char* _p)
    {
        register Float2 res;
        asm
        {
            psq_l  res, 0(_p), 0, OS_FASTCAST_U8
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 2 16 bits signed integer from address _p, and convert them to float.
    // Return Float2 ( X=(float)_p[0], Y=(float)_p[1] )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadShort2(const register short* _p)
    {
	    register Float2 res;
	    asm
	    {
		    psq_l  res, 0(_p), 0, OS_FASTCAST_S16
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 2 16 bits unsigned integer from address _p, and convert them to float.
    // Return Float2 ( X=(float)_p[0], Y=(float)_p[1] )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadUShort2(const register unsigned short* _p)
    {
	    register Float2 res;
	    asm
	    {
		    psq_l  res, 0(_p), 0, OS_FASTCAST_U16
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 1 16 bits signed integer from address _p, and convert it to float, and
    // set Y component to 1.0f.
    // Return Float2 ( X=(float)_p[0], Y=1.0f )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadXShort(const register short* _p)
    {
	    register Float2 res;
	    asm
	    {
		    psq_l  res, 0(_p), 1, OS_FASTCAST_S16
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 2 16 bits unsigned integer from address _p, and convert them to float
    // with normalization.
    // Return Float2 ( X=((float)_p[0])/65536.0f, Y=((float)_p[1])/65536.0f )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadUShort2N(const register unsigned short* _p)
    {
	    register Float2 res;
	    asm
	    {
		    psq_l  res, 0(_p), 0, OS_FASTCAST_U16N
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Load 1 16 bits unsigned integer from address _p, and convert them to float
    // with normalization.
    // Return Float2 ( X=((float)_p[0])/65536.0f, Y=1.0f )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 LoadXUShortN(const register unsigned short* _p)
    {
	    register Float2 res;
	    asm
	    {
		    psq_l  res, 0(_p), 1, OS_FASTCAST_U16N
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Store the content of _a to memory address _p.
    // _p[0]=_a.X and _p[1]=_a.Y
    ///////////////////////////////////////////////////////////////////////////////
    inline void Store(Float2 PS_PARAM _a, register float* _p)
    {
        register Float2 a(_a);
	    asm
	    {
		    psq_st  a, 0(_p), 0, 0
	    }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Store the X component of _a to memory adress _p.
    // _p[0] = _a.X
    ///////////////////////////////////////////////////////////////////////////////
    inline void StoreX(Float2 PS_PARAM _a, register float* _p)
    {
        register Float2 a(_a);
	    asm
	    {
		    psq_st  a, 0(_p), 1, 0
	    }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Round the X and Y component of to integer toward zero, then store the two unsigned 8 bits
    // integer in memory address _p
    // Return Float2 ( X=(unsigned char)_p[0], Y=(unsigned char)_p[1] )
    ///////////////////////////////////////////////////////////////////////////////
    inline void StoreUChar2(Float2 PS_PARAM _a, register unsigned char* _p)
    {
        register Float2 a(_a);
        asm
        {
            psq_st  a, 0(_p), 0, OS_FASTCAST_U8
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Round the X and Y component of to integer toward zero, then store the two signed 16 bits
    // integer in memory address _p
    // Return Float2 ( X=(short)_p[0], Y=(short)_p[1] )
    ///////////////////////////////////////////////////////////////////////////////
    inline void StoreShort2(Float2 PS_PARAM _a, register short* _p)
    {
        register Float2 a(_a);
	    asm
	    {
		    psq_st  a, 0(_p), 0, OS_FASTCAST_S16
	    }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Merge pack two float in one Float2
    // Return Float2 ( X=_a, Y=_b )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Merge(register float _a, register float _b)
    {
        register Float2 a(_a), b(_b), res;
	    asm
	    {
		    ps_merge00  res, a, b
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Merge the X component of _a and _b into a Float2.
    // Return Float2 ( X=_a.X, Y=_b.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MergeXX(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
	    asm
	    {
		    ps_merge00  res, a, b
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Merge the Y component of _a and _b into a Float2.
    // Return Float2 ( X=_a.Y, Y=_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MergeYY(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
	    asm
	    {
		    ps_merge11  res, a, b
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Merge the X component of _a and the Y component of _b into a Float2.
    // Return Float2 ( X=_a.X, Y=_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MergeXY(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
	    asm
	    {
		    ps_merge01  res, a, b
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Merge the Y component of _a and the X component of _b into a Float2.
    // Return Float2 ( X=_a.Y, Y=_b.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MergeYX(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
	    asm
	    {
		    ps_merge10  res, a, b
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Duplicate the X component of _a.
    // Return Float2 ( X=_a.X, Y=_a.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 SplatX(Float2 PS_PARAM _a)
    {
        register Float2 a(_a), res;
        asm
        {
            ps_merge00  res, a, a
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Duplicate the Y component of _a.
    // Return Float2 ( X=_a.Y, Y=_a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 SplatY(Float2 PS_PARAM _a)
    {
        register Float2 a(_a), res;
        asm
        {
            ps_merge11  res, a, a
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Return the X component of _a.
    // Return _a.X
    ///////////////////////////////////////////////////////////////////////////////
    inline float GetX(Float2 PS_PARAM _a)
    {
        register float res;
        register Float2 a(_a);
	    asm
	    {
		    ps_mr  res, a
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Return the Y component of _a.
    // Return _a.Y
    ///////////////////////////////////////////////////////////////////////////////
    inline float GetY(Float2 PS_PARAM _a)
    {
        register float res;
        register Float2 a(_a);
	    asm
	    {
		    ps_merge11  res, a, a
	    }
	    return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component addition.
    // Return Float32 ( X=_a.X+_b.X, Y=_a.Y+_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Add(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        ps_add  res, a, b;
	    }
        return res;
    //	return _a + _b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Addition scalar
    // Return Float32 ( X=_a.X+_b, Y=_a.Y+_b )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Add(Float2 PS_PARAM _a, register float _b)
    {
        register Float2 a(_a), res;
        asm
        {
            ps_merge00  res, _b, _b
            ps_add      res, a, res;
        }
        return res;
        //	return _a + _b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Scalar addition, duplicate the result in both component.
    // Return Float32 ( X=_a.X+_b.X, Y=_a.X+_b.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Adds(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        fadds  res, a, b;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Return Float32 ( X=_a.X+_b.Y, Y=_c.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 SumX(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_sum0  res, a, c, b;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Return Float32 ( X=_c.X, Y=_a.X+_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 SumY(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_sum1  res, a, c, b;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component subtraction.
    // Return Float32 ( X=_a.X-_b.X, Y=_a.Y-_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Sub(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        ps_sub  res, a, b;
	    }
        return res;
    //	return _a - _b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Subtract scalar
    // Return Float32 ( X=_a.X-_b, Y=_a.Y-_b )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Sub(Float2 PS_PARAM _a, register float _b)
    {
        register Float2 a(_a), res;
        asm
        {
            ps_merge00  res, _b, _b
            ps_sub      res, a, res;
        }
        return res;
        //	return _a - _b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Scalar subtraction, duplicate the result in both component.
    // Return Float32 ( X=_a.X-_b.X, Y=_a.X-_b.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Subs(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        fsubs  res, a, b;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component multiply.
    // Return Float32 ( X=_a.X*_b.X, Y=_a.Y*_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Mul(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        ps_mul  res, a, b;
	    }
        return res;
    //	return _a * _b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // X broadcast multiply.
    // Return Float32 ( X=_a.X*_b, Y=_a.Y*_b )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Mul(Float2 PS_PARAM _a, register float _b)
    {
        register Float2 a(_a), res;
        asm
        {
            ps_muls0  res, a, _b;
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Sclar multiply, duplicate the result in both component.
    // Return Float32 ( X=_a.X*_b.X, Y=_a.X*_b.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Muls(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        fmuls  res, a, b;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // X broadcast multiply.
    // Return Float32 ( X=_a.X*_b.X, Y=_a.Y*_b.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MulX(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        ps_muls0  res, a, b;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Y broadcast multiply.
    // Return Float32 ( X=_a.X*_b.Y, Y=_a.Y*_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MulY(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        ps_muls1  res, a, b;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component multiply-add.
    // Return Float32 ( X=(_a.X*_b.X)+_c.X, Y=(_a.Y*_b.Y)+_c.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Madd(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_madd  res, a, b, c;
	    }
        return res;
    //	return _a*_b + _c;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component multiply-add.
    // Return Float32 ( X=(_a.X*_b)+_c.X, Y=(_a.Y*_b)+_c.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Madd(Float2 PS_PARAM _a, register float _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), c(_c), res;
        asm
        {
            ps_madds0  res, a, _b, c;
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component multiply-add and negate
    // Return Float32 ( X=-((_a.X*_b.X)+_c.X), Y=-((_a.Y*_b.Y)+_c.Y) )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Nmadd(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_nmadd  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Scalar multiply-add, duplicate the result in both component.
    // Return Float32 ( X=(_a.X*_b.X)+_c.X, Y=(_a.X*_b.X)+_c.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Madds(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        fmadds  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // X broadcast multiply-add.
    // Return Float32 ( X=(_a.X*_b.X)+_c.X, Y=(_a.Y*_b.X)+_c.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MaddX(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_madds0  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Y broadcast multiply-add.
    // Return Float32 ( X=(_a.X*_b.Y)+_c.X, Y=(_a.Y*_b.Y)+_c.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 MaddY(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_madds1  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component multiply-sub.
    // Return Float32 ( X=(_a.X*_b.X)-_c.X, Y=(_a.Y*_b.Y)-_c.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Msub(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_msub  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Scalar multiply-sub, duplicate the result in both component.
    // Return Float32 ( X=(_a.X*_b.X)-_c.X, Y=(_a.X*_b.X)-_c.X )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Msubs(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        fmsubs  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component multiply-sub and negate
    // Return Float32 ( X=(_c.X - _a.X*_b.X), Y=(_c.Y - _a.Y*_b.Y)
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Nmsub(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_nmsub  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Scalar multiply-sub and negate, duplicate the result in both component.
    // Return Float32 ( X=(_c.X - _a.X*_b.X), Y=(_c.X - _a.X*_b.X)
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Nmsubs(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        fnmsubs  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // 2D dot product
    // Return Float32 ( X=(_a.X*_b.X + _a.Y*_b.Y, Y=? )
    ///////////////////////////////////////////////////////////////////////////////
    inline float Dot(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b);
        register float res;
        asm
        {
            ps_mul   res, a, b;
            ps_sum0  res, res, res, res;
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Square root reciprocal estimation, with 12 bit precision (relative error is
    // smaller than 1/4096) for X and Y component.
    // Return Float32 ( X=rsqrte(_a.X), Y=rsqrte(_a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 RsqrtEstimate(Float2 PS_PARAM _a)
    {
        register Float2 a(_a), res;
        asm
	    {
	        ps_rsqrte  res, a;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Reciprocal estimation, with 12 bit precision (relative error is
    // smaller than 1/4096) for X and Y component.
    // Return Float32 ( X=rcpe(_a.X), Y=rcpe(_a.Y) )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 RcpEstimate(Float2 PS_PARAM _a)
    {
        register Float2 a(_a), res;
        asm
	    {
	        ps_res  res, a;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component square root estimation with 12 bits precision
    // (relative error is smaller than 1/4096)
    // Return Float32 ( X=sqrte(_a.X), Y=sqrte(_a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 SqrtEstimate(Float2 PS_PARAM _a)
    {
        register Float2 a(_a), res;
        register Float2 tmp;
        register Float2 zero = 0.0f;
        asm
        {
            ps_rsqrte  res, a;
            ps_nabs    tmp, a
            ps_mul	   res, res, a
            ps_sel     res, tmp, zero, res
        }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component reciprocal.
    // Return Float32 ( X=1.0f/_a.X, Y=1.0f/_a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Rcp(Float2 PS_PARAM _a)
    {
        // Use two Newton-Raphson iteration
        // y1 = y0 + y0 * (1.0 - x * y0)
        register Float2 a(_a), res;
	    register Float2 One = 1.0f;
	    register Float2 Temp;
        asm
	    {
	        ps_res     res, a;
		    ps_nmsub   Temp, a, res, One
		    ps_madd	   res, res, Temp, res
            ps_nmsub   Temp, a, res, One
            ps_madd    res, res, Temp, res
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component square root reciprocal.
    // Return Float32 ( X=rsqrt(_a.X), Y=rsqrt(_a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Rsqrt(Float2 PS_PARAM _a)
    {
        // Use one Newton-Raphson approximation iteration for 1.0f / x
        // y0 = reciprocal_estimate(x)
        // y1 = y0 + y0 * (1.0 - x * y0)
        //    = y0 - y0 * (x * y0 - 1.0)

        register Float2 a(_a), res;
	    register Float2 half = 0.5f;
	    register Float2 temp1, temp2;

        asm
	    {
	        ps_rsqrte  res, a;
		    ps_mul	   temp1, half, a
		    ps_mul	   temp2, res, res
            ps_nmsub   temp1, temp1, temp2, half
            ps_madd    res, temp1, res, res
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component square root.
    // Return Float32 ( X=sqrt(_a.X), Y=sqrt(_a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Sqrt(Float2 PS_PARAM _a)
    {
	    // y1 = y0 + y0 * (0.5 - 0.5 * x * y0 * y0)
        register Float2 a(_a), res;
	    register Float2 half = 0.5f;
        register Float2 zero = 0.0f;
	    register Float2 temp1, temp2, temp3;
        asm
	    {
	        ps_rsqrte  res, a;
            ps_nabs    temp3, a
		    ps_mul	   temp1, res, a
		    ps_mul	   temp2, res, half
            ps_nmsub   temp2, temp1, temp2, half
            ps_madd    res, temp2, temp1, temp1
            ps_sel     res, temp3, zero, res
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component divide.
    // Return Float32 ( X=_a.X/_b.X, Y=_a.Y/_b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Div(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
	        ps_div  res, a, b;
	    }
        return res;
    	//return _a / _b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Brodcast divide.
    // Return Float32 ( X=_a.X/_b, Y=_a.Y/_b )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Div(Float2 PS_PARAM _a, register float _b)
    {
        register Float2 a(_a), res;
        asm
	    {
            ps_merge00  res, _b, _b;
	        ps_div      res, a, res;
	    }
        return res;
    	//return _a / _b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component select.
    // Return Float32 ( X = _a.X>=0.0f ? _b.X : _c.X, Y = _a.Y>=0.0f ? _b.Y : _c.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Select(Float2 PS_PARAM _a, Float2 PS_PARAM _b, Float2 PS_PARAM _c)
    {
        register Float2 a(_a), b(_b), c(_c), res;
        asm
	    {
	        ps_sel  res, a, b, c;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component Min.
    // Return Float32 ( X = _a.X<_b.X ? _a.X : _b.X, Y = _a.Y<_b.Y ? _a.Y : _b.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Min(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
   	        ps_sub  res, a, b;
	        ps_sel  res, res, b, a;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component Max.
    // Return Float32 ( X = _a.X<_b.X ? _b.X : _a.X, Y = _a.Y<_b.Y ? _b.Y : _a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Max(Float2 PS_PARAM _a, Float2 PS_PARAM _b)
    {
        register Float2 a(_a), b(_b), res;
        asm
	    {
   	        ps_sub  res, b, a;
	        ps_sel  res, res, b, a;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component Neg.
    // Return Float32 ( X = -_a.X, Y = -_a.Y )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Neg(Float2 PS_PARAM _a)
    {
        register Float2 a(_a), res;
        asm
	    {
   	        ps_neg  res, a;
	    }
        return res;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Per component Absolute value.
    // Return Float32 ( X = fabsf(_a.X), Y = fbasf(_a.Y) )
    ///////////////////////////////////////////////////////////////////////////////
    inline Float2 Abs(Float2 PS_PARAM _a)
    {
        register Float2 a(_a), res;
        asm
	    {
   	        ps_abs  res, a;
	    }
        return res;
    }

}   // namespace ps


#endif //__FLOAT2_BROADWAY__
