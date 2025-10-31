#ifndef _ITF_COLOR_H_
#define _ITF_COLOR_H_

#ifdef ITF_X360
//#define SIMD_COLOR_SUPPORTED
#endif //ITF_X360

#ifndef _ITF_COLOR_MACROS_H_
#include "core/Color_Macros.h"
#endif _ITF_COLOR_MACROS_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_


namespace ITF
{
    // Color[3] -> A, Color[2] -> R, Color[1] -> G, Color[0] -> B

    class Color 
    {
    public:
#ifndef ITF_CTR
        union 
        {
            struct 
            {
                f32 m_b;
                f32 m_g;
                f32 m_r;
                f32 m_a;
            };
#ifdef SIMD_COLOR_SUPPORTED
            __vector4 m_vector;
#endif //SIMD_COLOR_SUPPORTED
        };
#else
        f32 m_b;
        f32 m_g;
        f32 m_r;
        f32 m_a;
#endif

    public :
        const Color& operator = (const Color& src)
        {
#if defined( ITF_PSI )
		register __vec2x32float__ f_p1, f_p2;
		register float* me0 = (float*)this;
		register float* pv0 = (float*)&src;	
		register float* me1 = &me0[2];
		register float* pv1 = &pv0[2];
		asm	{		
			psq_l     f_p1,  0(pv0), 0, 0; //load vector
			psq_l     f_p2,  0(pv1), 0, 0; //load vector
			psq_st    f_p1,  0(me0), 0, 0; //store result
			psq_st    f_p2,  0(me1), 0, 0; //store result
		}
#else
            m_b = src.m_b;
            m_g = src.m_g;
            m_r = src.m_r;
            m_a = src.m_a;
#endif
            return *this;
        }

#ifdef SIMD_COLOR_SUPPORTED
        Color(const __vector4 &vectorfloat):m_vector(vectorfloat) {}
#endif //SIMD_COLOR_SUPPORTED

        Color()                 {    }

        Color(f32 _a, f32 _r, f32 _g, f32 _b)  {
            m_a = _a;
            m_r = _r; 
            m_g = _g; 
            m_b = _b; 
        }

        Color(u32 _color)       {
            m_a = getAlphaFromU32(_color);
            m_r = getRedFromU32(_color);
            m_g = getGreenFromU32(_color);
            m_b = getBlueFromU32(_color);
        }

        void serialize(class ArchiveMemory& _archive);

        ITF_INLINE f32 getBlue()const   {return m_b;}
        ITF_INLINE f32 getGreen()const  {return m_g;}
        ITF_INLINE f32 getRed()const    {return m_r;}
        ITF_INLINE f32 getAlpha()const  {return m_a;}

        ITF_INLINE void setBlue(f32 _b)       {m_b = _b;}
        ITF_INLINE void setGreen(f32 _g)      {m_g = _g;}
        ITF_INLINE void setRed(f32 _r)        {m_r = _r;}
        ITF_INLINE void setAlpha(f32 _a)      {m_a = _a;}

        ITF_INLINE void ClampColor()
        {
            Clamp(m_b, 0.f, 1.f);
            Clamp(m_g, 0.f, 1.f);
            Clamp(m_r, 0.f, 1.f);
            Clamp(m_a, 0.f, 1.f);
        }

        ITF_INLINE u32 getAsU32()const {
            return ITFCOLOR_TO_U32(ITFCOLOR_F32_TO_U8(m_a), ITFCOLOR_F32_TO_U8(m_r), ITFCOLOR_F32_TO_U8(m_g), ITFCOLOR_F32_TO_U8(m_b));
        }

        ITF_INLINE static Color Interpolate( const Color& _colA, const Color& _colB, f32 _t )
        {
            //u32 ColorOut, Interpoler, col1, col2;
            f32 inv_t = 1.f - _t;

            if(_t >= 0.98f) return _colB;
            if(_t <= 0.02f) return _colA;
            
            return _colA*inv_t + _colB*_t;

            //col1 = _colA.getAsU32();
            //col2 = _colB.getAsU32();

            //*((f32 *) &Interpoler) = _t + 32768.0f + 16384.0f;
            //ColorOut = (Interpoler & 128) ? (col2 & 0xfefefefe) >> 1 : (col1 & 0xfefefefe) >> 1;
            //ColorOut += (Interpoler & 64) ? (col2 & 0xfcfcfcfc) >> 2 : (col1 & 0xfcfcfcfc) >> 2;
            //ColorOut += (Interpoler & 32) ? (col2 & 0xf8f8f8f8) >> 3 : (col1 & 0xf8f8f8f8) >> 3;
            //ColorOut += (Interpoler & 16) ? (col2 & 0xf0f0f0f0) >> 4 : (col1 & 0xf0f0f0f0) >> 4;
            //ColorOut += (Interpoler & 8) ? (col2 & 0xe0e0e0e0) >> 5 : (col1 & 0xe0e0e0e0) >> 5;
            //ColorOut += (Interpoler & 4) ? (col2 & 0xc0c0c0c0) >> 6 : (col1 & 0xc0c0c0c0) >> 6;
            //ColorOut += (Interpoler & 2) ? (col2 & 0x80808080) >> 7 : (col1 & 0x80808080) >> 7;

            //return Color(ColorOut);
        }

		ITF_INLINE  void MulAdd(const Color &_colorMulA, const Color &_colorMulB,const Color &_colorAdd)
		{
			Vec2d::MulAdd((Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(&_colorMulA))[0], (Vec2d*)&((f32*)(&_colorMulB))[0], (Vec2d*)&((f32*)(&_colorAdd))[0]);
			Vec2d::MulAdd((Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(&_colorMulA))[2], (Vec2d*)&((f32*)(&_colorMulB))[2], (Vec2d*)&((f32*)(&_colorAdd))[2]);
		}

		ITF_INLINE  void Min(f32 _min)
		{
			Vec2d::MinScalar((Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(this))[0], _min);
			Vec2d::MinScalar((Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(this))[2], _min);
		}

        //ITF_INLINE        f32 & operator[](u32 _index)       {return ((f32*)this)[_index];}
        ITF_INLINE  const f32   operator[](u32 _index) const {return ((f32*)this)[_index];}

        ITF_INLINE  bool operator==(const Color &_color) const 
        {return m_r == _color.m_r &&
            m_g == _color.m_g &&
            m_b == _color.m_b &&
            m_a == _color.m_a;}

        ITF_INLINE  bool operator!=(const Color &_color) const 
        {return m_r != _color.m_r ||
            m_g != _color.m_g ||
            m_b != _color.m_b ||
            m_a != _color.m_a;}

        ITF_INLINE  const Color& operator+=(const Color &_color) 
        {
            Vec2d::Add((Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(&_color))[0]);
            Vec2d::Add((Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(&_color))[2]);
            return *this;
        }

		ITF_INLINE  Color operator*(const Color &_color) const 
        {
            Color res;
            Vec2d::Mul((Vec2d*)&((f32*)(&res))[0], (Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(&_color))[0]);
            Vec2d::Mul((Vec2d*)&((f32*)(&res))[2], (Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(&_color))[2]);
            return res;
        }

        ITF_INLINE  Color operator*(const f32 _factor) const 
        {
            Color res;
            Vec2d::Scale((Vec2d*)&((f32*)(&res))[0], (Vec2d*)&((f32*)(this))[0], _factor);
            Vec2d::Scale((Vec2d*)&((f32*)(&res))[2], (Vec2d*)&((f32*)(this))[2], _factor);
            return res;
        }

        ITF_INLINE  Color operator+(const Color &_color) const 
        {
            Color res;
            Vec2d::Add((Vec2d*)&((f32*)(&res))[0], (Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(&_color))[0]);
            Vec2d::Add((Vec2d*)&((f32*)(&res))[2], (Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(&_color))[2]);
            return res;
        }

        ITF_INLINE  Color operator-(const Color &_color) const 
        {
            Color res;
            Vec2d::Sub((Vec2d*)&((f32*)(&res))[0], (Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(&_color))[0]);
            Vec2d::Sub((Vec2d*)&((f32*)(&res))[2], (Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(&_color))[2]);
            return res;
        }

		ITF_INLINE static f32 getAlphaFromU32 (u32 _color)
		{
			return ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_A(_color));
		}

		ITF_INLINE static f32 getRedFromU32 (u32 _color)
		{
			return ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_R(_color));
		}

		ITF_INLINE static f32 getGreenFromU32 (u32 _color)
		{
			return ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_G(_color));
		}

		ITF_INLINE static f32 getBlueFromU32 (u32 _color)
		{
			return ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_B(_color)); 
		}

        ITF_INLINE static Color colorBlend( const Color* ITF_RESTRICT  _colorA, const Color*  ITF_RESTRICT  _colorB, f32 _f )
        {
#ifdef SIMD_COLOR_SUPPORTED
            __vector4 vF ={{_f,_f,_f,_f}};
            __vector4 vAMinusB = __vsubfp(_colorA.m_vector,_colorB.m_vector);
            __vector4 _result = __vmulfp(vAMinusB,vF);
            return Color(__vaddfp(_result,_colorB.m_vector));
#else
            Color res;
            Vec2d * ITF_RESTRICT cOar, * ITF_RESTRICT cOgb;
            Vec2d * ITF_RESTRICT cAar, * ITF_RESTRICT cAgb;
            Vec2d * ITF_RESTRICT cBar, * ITF_RESTRICT cBgb;
            const f32 * ITF_RESTRICT pR = (const f32*)&res;
            const f32 * ITF_RESTRICT pA = (const f32*)_colorA;
            const f32 * ITF_RESTRICT pB = (const f32*)_colorB;
            cOar = (Vec2d*)(&pR[0]);
            cOgb = (Vec2d*)(&pR[2]);            
            cAar = (Vec2d*)(&pA[0]);
            cAgb = (Vec2d*)(&pA[2]);
            cBar = (Vec2d*)(&pB[0]);
            cBgb = (Vec2d*)(&pB[2]);

            //Lerp
            Vec2d::Sub(cOar, cAar, cBar);
            Vec2d::Sub(cOgb, cAgb, cBgb);
            Vec2d::ScaleAdd(cOar, cOar, _f, cBar);
            Vec2d::ScaleAdd(cOgb, cOgb, _f, cBgb);

            return res;
#endif //SIMD_COLOR_SUPPORTED
        }

        //Accessors to make standard colors
        #define ADD_COLOR(name,a,r,g,b) \
            static ITF_INLINE const Color& name() {static const Color name##_color(a,r,g,b); return name##_color;}

        ADD_COLOR(zero,     0.0f, 0.0f, 0.0f, 0.0f);
        ADD_COLOR(white,    1.0f, 1.0f, 1.0f, 1.0f);
        ADD_COLOR(black,    1.0f, 0.0f, 0.0f, 0.0f);
        ADD_COLOR(grey,     1.0f, 0.5f, 0.5f, 0.5f);

        ADD_COLOR(red,      1.0f, 1.0f, 0.0f, 0.0f);
        ADD_COLOR(green,    1.0f, 0.0f, 1.0f, 0.0f);
        ADD_COLOR(blue,     1.0f, 0.0f, 0.0f, 1.0f);

        ADD_COLOR(yellow,   1.0f, 1.0f, 1.0f, 0.0f);
        ADD_COLOR(cyan,     1.0f, 0.0f, 1.0f, 1.0f);
        ADD_COLOR(magenta,  1.0f, 1.0f, 0.0f, 1.0f);

        ADD_COLOR(orange,   1.0f, 1.0f, 0.5f, 0.0f);
        ADD_COLOR(pink,     1.0f, 1.0f, 0.0f, 0.5f);
        ADD_COLOR(violet,   1.0f, 0.5f, 0.0f, 1.0f);
    };
}

#endif // _ITF_COLOR_H_
