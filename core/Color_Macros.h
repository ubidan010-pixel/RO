#ifndef _ITF_COLOR_MACROS_H_
#define _ITF_COLOR_MACROS_H_

#include "core/types.h"
#include "core/math/Mathf32.h"

#ifdef ITF_WII
#ifndef __FLOAT2_BROADWAY__
#include "core/math/WII/SIMD_Float2.h"
#endif
#endif


namespace ITF
{
    #ifdef ITFCOLOR_AS_ARGB
        // ARGB
        #define COLOR_SHIFT_ALPHA   24
        #define COLOR_SHIFT_RED     16
        #define COLOR_SHIFT_GREEN    8
        #define COLOR_SHIFT_BLUE     0
    #elif defined(ITFCOLOR_AS_ABGR)
        // ABGR in an integer -> will finish as RGBA when stored as bytes on little endian cpu
        #define COLOR_SHIFT_ALPHA   24
        #define COLOR_SHIFT_BLUE    16
        #define COLOR_SHIFT_GREEN    8
        #define COLOR_SHIFT_RED      0
    #else
        // RGBA
        #define COLOR_SHIFT_RED     24
        #define COLOR_SHIFT_GREEN   16
        #define COLOR_SHIFT_BLUE     8
        #define COLOR_SHIFT_ALPHA    0
    #endif // ITFCOLOR_AS_ARGB

    //----

    static const u32 COLOR_RGB_MASK = (0xFF<<COLOR_SHIFT_RED)|(0xFF<<COLOR_SHIFT_GREEN)|(0xFF<<COLOR_SHIFT_BLUE);

    static const u32 COLOR_ZERO     = 0x00000000;
    static const u32 COLOR_BLACK    = COLOR_ZERO|(0xFF<<COLOR_SHIFT_ALPHA);
    static const u32 COLOR_RED      = COLOR_BLACK|(0xFF<<COLOR_SHIFT_RED);
    static const u32 COLOR_GREEN    = COLOR_BLACK|(0xFF<<COLOR_SHIFT_GREEN);
    static const u32 COLOR_BLUE     = COLOR_BLACK|(0xFF<<COLOR_SHIFT_BLUE);
    static const u32 COLOR_WHITE    = COLOR_RED|COLOR_GREEN|COLOR_BLUE;

    //----

	#define ITFCOLOR_F32_TO_U8(val)	((u8)(f32_Clamp((val),0.f,1.f)*255.f))
	#define ITFCOLOR_U8_TO_F32(val)	((f32)(val)*f32_Inv255)
	
	#define ITFCOLOR_TO_U32(a,r,g,b) \
		((u32)((((a)&0xff)<<COLOR_SHIFT_ALPHA)|(((r)&0xff)<<COLOR_SHIFT_RED)|(((g)&0xff)<<COLOR_SHIFT_GREEN)|(((b)&0xff)<<COLOR_SHIFT_BLUE)))

	#define ITFCOLOR_U32_A(c)	(((c)>>COLOR_SHIFT_ALPHA) & 0xFF)
	#define ITFCOLOR_U32_R(c)	(((c)>>COLOR_SHIFT_RED)   & 0xFF)
	#define ITFCOLOR_U32_G(c)	(((c)>>COLOR_SHIFT_GREEN) & 0xFF)
	#define ITFCOLOR_U32_B(c)	(((c)>>COLOR_SHIFT_BLUE)  & 0xFF)

	#ifdef ITFCOLOR_AS_ARGB
		// ARGB
        #define ITFCOLOR_FROM_RGBA(c)	ITFCOLOR_TO_U32((c)>>0, (c)>>24, (c)>>16, (c)>>8)
		#define ITFCOLOR_FROM_ARGB(c)	(c)
		#define ITFCOLOR_TO_ARGB(c)     (c)
	#else
		// RGBA
        #define ITFCOLOR_FROM_RGBA(c)	(c)
		#define ITFCOLOR_FROM_ARGB(c)	ITFCOLOR_TO_U32((c)>>24, (c)>>16, (c)>>8, (c)>>0)
		#define ITFCOLOR_TO_ARGB(c)     ((u32)((ITFCOLOR_U32_A(c)<<24)|(ITFCOLOR_U32_R(c)<<16)|(ITFCOLOR_U32_G(c)<<8)|(ITFCOLOR_U32_B(c)<<0)))
	#endif // ITFCOLOR_AS_ARGB

    //----

	ITF_INLINE static u32 ITFCOLOR_SET_A(u32 _initialColor, f32 _alpha)
	{
		u32 color = _initialColor & COLOR_RGB_MASK;
		u32 alpha = ITFCOLOR_F32_TO_U8(_alpha) << COLOR_SHIFT_ALPHA;
		return color | alpha;
	}

	ITF_INLINE static u32 ITFCOLOR_SET_A(u32 _initialColor, u8 _alpha)
	{
		u32 color = _initialColor & COLOR_RGB_MASK;
		u32 alpha = _alpha << COLOR_SHIFT_ALPHA;
		return color | alpha;
	}

	ITF_INLINE static u32 ITFCOLOR_BLEND(u32 _colorA, u32 _colorB, f32 _factorClamped)
	{
//#ifdef ITF_WII // may be done not only on WII ... TODO : check after E3
        u32 ColorOut, Interpoler;

        if(_colorA == _colorB )
            return _colorA;
            
        if(_factorClamped >= 0.98f) 
            return _colorB;
        if(_factorClamped <= 0.02f)
            return _colorA;

        *((f32 *) &Interpoler) = _factorClamped + 32768.0f + 16384.0f;
        ColorOut = (Interpoler & 128) ? (_colorB & 0xfefefefe) >> 1 : (_colorA & 0xfefefefe) >> 1;
        ColorOut += (Interpoler & 64) ? (_colorB & 0xfcfcfcfc) >> 2 : (_colorA & 0xfcfcfcfc) >> 2;
        ColorOut += (Interpoler & 32) ? (_colorB & 0xf8f8f8f8) >> 3 : (_colorA & 0xf8f8f8f8) >> 3;
        ColorOut += (Interpoler & 16) ? (_colorB & 0xf0f0f0f0) >> 4 : (_colorA & 0xf0f0f0f0) >> 4;
        ColorOut += (Interpoler &  8) ? (_colorB & 0xe0e0e0e0) >> 5 : (_colorA & 0xe0e0e0e0) >> 5;
        ColorOut += (Interpoler &  4) ? (_colorB & 0xc0c0c0c0) >> 6 : (_colorA & 0xc0c0c0c0) >> 6;
        ColorOut += (Interpoler &  2) ? (_colorB & 0x80808080) >> 7 : (_colorA & 0x80808080) >> 7;

        return ColorOut;
//#else
//		//assume _factor is Clamped
//		u32 cOa, cOr, cOg, cOb;
//		u32 cAa, cAr, cAg, cAb;
//		u32 cBa, cBr, cBg, cBb;
//
//		cAa = ITFCOLOR_U32_A(_colorA);
//		cAr = ITFCOLOR_U32_R(_colorA);
//		cAg = ITFCOLOR_U32_G(_colorA);
//		cAb = ITFCOLOR_U32_B(_colorA);
//
//		cBa = ITFCOLOR_U32_A(_colorB);
//		cBr = ITFCOLOR_U32_R(_colorB);
//		cBg = ITFCOLOR_U32_G(_colorB);
//		cBb = ITFCOLOR_U32_B(_colorB);
//
//		cOa = (u32)f32_Lerp((f32)cAa, (f32)cBa, _factorClamped);
//		cOr = (u32)f32_Lerp((f32)cAr, (f32)cBr, _factorClamped);
//		cOg = (u32)f32_Lerp((f32)cAg, (f32)cBg, _factorClamped);
//		cOb = (u32)f32_Lerp((f32)cAb, (f32)cBb, _factorClamped);
//
//		return ITFCOLOR_TO_U32(cOa,cOr,cOg,cOb);
//#endif
	}

    // Much faster than previous function on Wii
    ITF_INLINE static void ITFCOLOR_BLEND(u32 *_destColor, u32 const* _colorA, u32 const* _colorB, f32 _factorClamped)
    {
#ifdef ITF_WII
        const unsigned char* a = (const unsigned char*)_colorA;
        const unsigned char* b = (const unsigned char*)_colorB;
        unsigned char*       d = (unsigned char*)_destColor;

        ps::Float2 Arg = ps::LoadUChar2(a);
        ps::Float2 Aba = ps::LoadUChar2(a+2);
        ps::Float2 Brg = ps::LoadUChar2(b);
        ps::Float2 Bba = ps::LoadUChar2(b+2);
        ps::Float2 Drg = ps::Madd((Brg-Arg), _factorClamped, Arg);
        ps::Float2 Dba = ps::Madd((Bba-Aba), _factorClamped, Aba);
        ps::StoreUChar2(Drg, d);
        ps::StoreUChar2(Dba, d+2);
#else
        *_destColor = ITFCOLOR_BLEND(*_colorA, *_colorB, _factorClamped);
#endif
    }
}

#endif // _ITF_COLOR_MACROS_H_
