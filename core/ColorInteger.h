#ifndef _ITF_COLORINTEGER_H_
#define _ITF_COLORINTEGER_H_

#include "core/Color_Macros.h"

namespace ITF
{
    class ColorInteger 
    {
    public :

        struct rgba
        {
			#ifdef ITFCOLOR_AS_ARGB
			    // ARGB
                DECL_BF(u8 m_b;,
                    DECL_BF(u8 m_g;,
                    DECL_BF(u8 m_r;,
                    DECL_BF(u8 m_a;,
                    ))))
            #elif defined(ITFCOLOR_AS_ABGR)
                // ABGR
                DECL_BF(u8 m_r; ,
                    DECL_BF(u8 m_g; ,
                        DECL_BF(u8 m_b; ,
                            DECL_BF(u8 m_a; ,
                                ))))
            #else
                // RGBA
                DECL_BF(u8 m_a;,
                    DECL_BF(u8 m_b;,
                    DECL_BF(u8 m_g;,
                    DECL_BF(u8 m_r;,
                    ))))
            #endif // ITFCOLOR_AS_ARGB
        };

        union 
        {
            rgba    m_rgba;
            u32     m_color;
        };

        ColorInteger(u8 _a, u8 _r,u8 _g, u8 _b)
        {
            m_rgba.m_a = _a;
            m_rgba.m_r = _r;
            m_rgba.m_g = _g;
            m_rgba.m_b = _b;
        }

        ITF_INLINE const ColorInteger& operator = (const ColorInteger& src)
        {
            m_color = src.m_color;
            return *this;
        }

        ITF_INLINE const ColorInteger& operator = (const u32& src)
        {
            m_color = src;
            return *this;
        }

        ColorInteger()                 {    }

        ColorInteger(u32 _color)       
        {
			m_color = _color;
        }

        void serialize(class ArchiveMemory& _archive);

        ITF_INLINE u8 getU8Alpha()const  {return m_rgba.m_a;}
        ITF_INLINE u8 getU8Red()const    {return m_rgba.m_r;}
        ITF_INLINE u8 getU8Green()const  {return m_rgba.m_g;}
        ITF_INLINE u8 getU8Blue()const   {return m_rgba.m_b;}

        ITF_INLINE f32 getF32Alpha()const  {return ITFCOLOR_U8_TO_F32(m_rgba.m_a);}
        ITF_INLINE f32 getF32Red()const    {return ITFCOLOR_U8_TO_F32(m_rgba.m_r);}
        ITF_INLINE f32 getF32Green()const  {return ITFCOLOR_U8_TO_F32(m_rgba.m_g);}
        ITF_INLINE f32 getF32Blue()const   {return ITFCOLOR_U8_TO_F32(m_rgba.m_b);}

        ITF_INLINE u32 getAsU32()const {
            return m_color;
        }

        ITF_INLINE static ColorInteger Interpolate( const ColorInteger& _colA, const ColorInteger& _colB, f32 _t )
        {
            return ColorInteger(ITFCOLOR_BLEND( _colA.getAsU32(), _colB.getAsU32(), _t));
        }

        ITF_INLINE  bbool operator==(const ColorInteger &_color) const {return (m_color == _color.m_color);}
    };
}

#endif // _ITF_COLOR_H_
