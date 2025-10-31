#ifndef _ITF_MATRIX44_X86_HEADER_H_
#define _ITF_MATRIX44_X86_HEADER_H_

//----------------------------------------------------------------------------//
// includes.
//----------------------------------------------------------------------------//
//#include <emmintrin.h>
#include <xmmintrin.h>

//----------------------------------------------------------------------------//
// defines.
//----------------------------------------------------------------------------//
#define _MUSTALIGN
#define ALIGN16(v)          __declspec(align(16)) v
#define FORCE_INLINE        __forceinline
#define SIMD_ARG
#define RESTRICT            __restrict
#define SIMD_Shuffle(_v,_eX,_eY,_eZ,_eW)		   _mm_shuffle_ps( (_v), (_v), _MM_SHUFFLE((_eW),(_eZ),(_eY),(_eX)) )

namespace ITF
{
    //----------------------------------------------------------------------------//
    // Float4 definition.
    //----------------------------------------------------------------------------//
    typedef __m128 Float4;

    //----------------------------------------------------------------------------//
    // Float4 functions.
    //----------------------------------------------------------------------------//
    enum Selector
    {
        s_X = 0,
        s_Y,
        s_Z,
        s_W,
        ENUM_FORCE_SIZE_32(Selector)
    };

    ITF_FORCE_INLINE void setFloat4(Float4 &_f4, f32 _x, f32 _y, f32 _z, f32 _w) 
    {
        _f4 = _mm_set_ps(_w, _z, _y, _x);
    }

    ITF_FORCE_INLINE Float4 makeFloat4(f32 _x, f32 _y, f32 _z, f32 _w)
    {
        Float4 v; v = _mm_set_ps(_w, _z, _y, _x); return v;
    }

    ITF_FORCE_INLINE Float4 splatX( Float4 SIMD_ARG _v )
    {
	    return SIMD_Shuffle( _v, s_X, s_X, s_X, s_X );
    }

    ITF_FORCE_INLINE Float4 splatY( Float4 SIMD_ARG _v )
    {
	    return SIMD_Shuffle( _v, s_Y, s_Y, s_Y, s_Y );
    }

    ITF_FORCE_INLINE Float4 splatZ( Float4 SIMD_ARG _v )
    {
	    return SIMD_Shuffle( _v, s_Z, s_Z, s_Z, s_Z );
    }

    ITF_FORCE_INLINE Float4 splatW( Float4 SIMD_ARG _v )
    {
	    return SIMD_Shuffle( _v, s_W, s_W, s_W, s_W );
    }

    ITF_FORCE_INLINE Float4 min4(Float4 SIMD_ARG _v1, Float4 SIMD_ARG _v2)
    {
        return _mm_min_ps(_v1, _v2);
    }

    ITF_FORCE_INLINE Float4 max4(Float4 SIMD_ARG _v1, Float4 SIMD_ARG _v2)
    {
        return _mm_max_ps(_v1, _v2);
    }
    
	ITF_FORCE_INLINE void getFloat4(Float4 &_f4In, f32* _out)
    {
        float vo[4];
        _mm_storeu_ps(vo, _f4In);
        _out[0] = vo[0];
        _out[1] = vo[1];
        _out[2] = vo[2];
        _out[3] = vo[3];
    }

} // namespace ITF


#endif // _ITF_MATRIX44_X86_HEADER_H_
