#ifndef _ITF_MATHF32_H_
#define _ITF_MATHF32_H_

#ifndef _ITF_CORE_MACROS_H_
#include "core/macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifdef ITF_WII
#include <dtrigf.h>
#endif

#ifdef ITF_CTR
#include <nn/math.h>
#endif

#ifdef ITF_PS3
#include "PS3/FastTrigo_PS3.h"
#endif

#ifdef ITF_WII
# define F32_WII
#endif

namespace ITF
{

#if defined(ITF_MICROSOFT)
#define ITF_isnan(a)   _isnan(a)
#define ITF_finite(a)  _finite(a)
#else
#define ITF_isnan(a)   isnan(a)
#define ITF_finite(a)  isfinite(a)
#endif // ITF_MICROSOFT

static const f32	f32_Inv255	= 1.f / 255.f;

extern ITF_INLINE f32 f32_Sel(register f32 _selector, register f32 _posSelected, register f32 _negSelected)
{
#if defined(ITF_PS3)
    return __fsels(_selector, _posSelected, _negSelected);
#elif defined(ITF_X360)
    return f32(__fsel(_selector, _posSelected, _negSelected));
#elif defined(F32_WII)
    register f32 ret;
    asm
    {
        fsel   ret, _selector, _posSelected, _negSelected
    }
    return ret;
#else
    return _selector>=0 ? _posSelected : _negSelected;
#endif
}

#ifdef F32_WII
ITF_INLINE f32 modulo2Pi(f32 val)
{
    f32 a   = val * MTH_BY2PI;
        a   = f32_Sel(a, a, a-1.0f);
    i32 i   = (i32)a;
    f32 res = val - (f32)i*MTH_2PI;
    return res;
}
#endif

extern ITF_INLINE  f32 f32_Modulo(f32 val, f32 mod)
{
#if defined(F32_WII)
    i32 ModNb = (i32) (val/mod);
    ModNb = (val>=0.f ? ModNb : ModNb-1);
    f32 res = val - mod * (f32)ModNb;

    // Ensure modulo returns a value in the correct interval
    return ((res<=0.f) || (res>=mod)) ? 0.f : res;
#elif defined(ITF_CTR)
    return nn::math::FModf(val, &mod);
#else
    return fmodf(val, mod);
#endif
}

extern ITF_INLINE f32 f32_Abs(register f32 _a)
{
#if defined(F32_WII)
    register f32 res;
    asm
    {
        fabs res, _a
    }
    return res;
#elif defined(ITF_CTR)
    return __fabsf(_a);
#elif defined(ITF_PS3)
    return __fabsf(_a);
#else
    return fabsf(_a);
#endif
}

extern ITF_INLINE f32 f32_InvSqrt(register f32 _a)
{
#if defined(F32_WII)
    register f32    res, rdist, nwork0, nwork1;
    register f32    c_half, c_three;

    c_half  = 0.5f;
    c_three = 3.0f;
    asm
    {
        // 1.0/sqrt : estimation[E]
        frsqrte     rdist, _a
        // Refinement x 1 : E' = (E/2)(3 - X*E*E)
        fmuls       nwork0, rdist, rdist
        fmuls       nwork1, rdist, c_half
        fnmsubs     nwork0, nwork0, _a, c_three
        fmuls       res, nwork0, nwork1
    }
    return res;
#elif defined(ITF_CTR)
    return 1.0f / __sqrtf(_a);
#else
    return 1.f/sqrtf(_a);
#endif
}

extern ITF_INLINE f32 f32_Sqrt(register f32 _a)
{
#if defined(F32_WII)
    return (_a>0)?(_a*f32_InvSqrt(_a)):0.f;
#elif defined (ITF_CTR)
    return __sqrtf(_a);
#else
    return sqrtf(_a);
#endif
}

ITF_INLINE f32 f32_ApproxSqrt(f32 _a)
{
    union
    {
        i32 i;
        f32 f;
    } raw;
    raw.f = _a;
    raw.i -= 0x800000;
    raw.i >>= 1;
    raw.i += 1<<29;
    //babylonian quadradic convergence
    f32 step = raw.f;
    if (step==0)
        return 0;
    return 0.5f*(step+_a/step);
}

extern ITF_INLINE f32 f32_Inv(register f32 _a)
{
#if 0 // defined F32_WII
    // $$$$ TODO actualy create visual artefacts.
    register f32 res;
    asm
    { 
        fres res, _a // 2 clock cycles instead of 17 for div.
    }
    return res;
#else
	return 1.f/_a;
#endif
}
extern ITF_INLINE f32 f32_MulAdd(register f32 _a, register f32 _b, register f32 _c)
{
#if defined(F32_WII)
	register f32 res;
	asm
	{ 
		fmadds res,_a,_b,_c 
	}
	return res;
#else
	return _a*_b+_c;
#endif 
}

extern ITF_INLINE f32 f32_Min(f32 _a, f32 _b)
{
    return f32_Sel((_a)-(_b), _b, _a);
}

extern ITF_INLINE f32 f32_Max(f32 _a, f32 _b)
{
    return f32_Sel((_a)-(_b), _a, _b);
}

extern ITF_INLINE f32 f32_Clamp(f32 _v, f32 _min, f32 _max)
{
    return f32_Min(_max, f32_Max(_v, _min));
}

extern ITF_INLINE f32 f32_Lerp(f32 _a0, f32 _b1, f32 _f)
{
    return _f*(_b1-_a0) + _a0;
}

extern ITF_INLINE f32 f32_LerpClamp(f32 _a, f32 _b, f32 _f)
{
    return f32_Lerp(_a, _b, f32_Clamp(_f, 0.f, 1.f));
}

extern ITF_INLINE f32 f32_CosPrecise(register f32 _float)
{
#if defined(ITF_CTR)
    return nn::math::CosRad(_float);
#else
	return cosf(_float);
#endif
}

extern ITF_INLINE f32 f32_SinPrecise(register f32 _float)
{
#if defined(ITF_CTR)
    return nn::math::SinRad(_float);
#else
	return sinf(_float);
#endif
}

#if defined(F32_WII)

f32 f32_Cos(register f32 _float);
f32 f32_Sin(register f32 _float);
f32 acosOpt(register float _x);

#else

extern ITF_INLINE f32 f32_Cos(register f32 _float)
{
#if defined(ITF_PS3)
    return FastTrigo::cos(_float);
#else
    return f32_CosPrecise(_float);
#endif
}
extern ITF_INLINE f32 f32_Sin(register f32 _float)
{
#if defined(ITF_PS3)
    return FastTrigo::sin(_float);
#else
    return f32_SinPrecise(_float);
#endif
}

#endif

extern ITF_INLINE f32 f32_CosOpt(register f32 _float)
{
#if defined(F32_WII)
    return cosf_f(f32_Modulo(_float,MTH_2PI));
#elif defined(ITF_PS3)
    return FastTrigo::cos(_float);
#else
    return f32_CosPrecise(_float);
#endif
}

extern ITF_INLINE f32 f32_SinOpt(register f32 _float)
{
#if defined(F32_WII)
    return sinf_f(f32_Modulo(_float,MTH_2PI));
#elif defined(ITF_PS3)
    return FastTrigo::sin(_float);
#else
    return f32_SinPrecise(_float);
#endif
}

extern ITF_INLINE f32 f32_ACos(register f32 _float)
{
    return (_float > 1.f) ? 0 : ((_float < -1.f) ? MTH_PI : acosf(_float));
}

extern ITF_INLINE f32 f32_ACosOpt(register f32 _float)
{
#if defined(F32_WII)
    return acosOpt(_float);
#else
    return (_float > 1.f) ? 0 : ((_float < -1.f) ? MTH_PI : acosf(_float));
#endif
}

extern ITF_INLINE f32 f32_ASin(register f32 _float)
{
    return (_float < -1.f) ? -MTH_PIBY2 : ((_float > 1.f) ? MTH_PIBY2 : asinf(_float));
}

#ifdef F32_WII
extern void CosSin_0_2pi_a(register f32 _float,register  f32 *_pf_Cos,register  f32 *_pf_Sin);
extern void CosSin_0_2pi_f(register f32 _float,register  f32 *_pf_Cos,register  f32 *_pf_Sin);
#endif // F32_WII

ITF_INLINE void f32_CosSin(register f32 _float,register  f32 *_pf_Cos,register  f32 *_pf_Sin)
{
#if defined(F32_WII)
    CosSin_0_2pi_a(_float, _pf_Cos,_pf_Sin);
#elif defined(ITF_CTR)
    nn::math::SinCosRad(_pf_Sin, _pf_Cos, _float);
#elif defined(ITF_PS3)
    return FastTrigo::sincos(_float, _pf_Sin, _pf_Cos);
#else
    *_pf_Cos = f32_Cos(_float);
    *_pf_Sin = f32_Sin(_float);
#endif
}

ITF_INLINE void f32_CosSinOpt(register f32 _float,register  f32 *_pf_Cos,register  f32 *_pf_Sin)
{
#if defined(F32_WII)
    CosSin_0_2pi_f(_float ,_pf_Cos,_pf_Sin);
#else
    f32_CosSin( _float, _pf_Cos, _pf_Sin);
#endif
}

ITF_INLINE f32 roundFloat( f32 _float )
{
    return floorf(_float + 0.5f);
}

ITF_INLINE f32 getSign( register f32 _float )
{
#ifdef F32_WII
    register f32 ret;
	register f32 cone = 1.0f;
	register f32 cminusone = -1.0f;
	asm
	{
		fsel   ret, _float, cone, cminusone
	}
	return ret;
#else
	return f32_Sel(_float, 1.f, -1.f);
#endif
}

} // namespace ITF
#endif // _ITF_MATHF32_H_
