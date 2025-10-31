#include "precompiled_core.h"

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_


#if defined (_DEBUG) && defined (ITF_WINDOWS)
#include <iostream>
#include <limits>
const    float NaN = std::numeric_limits<float>::signaling_NaN();
#endif 

namespace ITF
{

    const Vec2d Vec2d::XAxis    (1.0f, 0.0f);
    const Vec2d Vec2d::YAxis    (0.0f, 1.0f);
    const Vec2d Vec2d::Zero     (0.0f, 0.0f);
    const Vec2d Vec2d::Up       (0.0f, 1.0f);
    const Vec2d Vec2d::Right    (1.0f, 0.0f);
    const Vec2d Vec2d::One      (1.0f, 1.0f);
    const Vec2d Vec2d::Infinity (F32_INFINITY, F32_INFINITY);

#if defined (_DEBUG) && defined (ITF_WINDOWS)
    Vec2d::Vec2d(): m_x(NaN), m_y(NaN) {}//: m_x(0.0f), m_y(0.0f) {}
#endif 

#ifdef COVERITY_CHECK
    Vec2d::Vec2d(){}
#endif //COVERITY_CHECK

#if defined( ITF_PSI ) && defined( F32_WII )


    void Vec2d::SinCos(register Vec2d* _dst, register f32 _angle)
    {
        register __vec2x32float__   aa;
        register __vec2x32float__   result;
        
        _angle = modulo2Pi(_angle);
        
        asm {
            ps_merge00 aa, _angle, _angle
        }
        
        result = sincosf_a(aa);
        
        asm {
            psq_st     result,  0(_dst), 0, 0;
        }
    
    }

    void Vec2d::SinCosOpt(register Vec2d* _dst, register f32 _angle)
    {
        register __vec2x32float__   aa;
        register __vec2x32float__   result;
        
        _angle = modulo2Pi(_angle);
        
        asm {
            ps_merge00 aa, _angle, _angle
        }
        
        result = sincosf_f(aa);
        
        asm {
            psq_st     result,  0(_dst), 0, 0;
        }
    }
    
    void Vec2d::CosSin(register Vec2d* _dst, register f32 _angle)
    {
        register __vec2x32float__   aa;
        register __vec2x32float__   result;
        
        _angle = modulo2Pi(_angle);
        
        asm {
            ps_merge00 aa, _angle, _angle
        }
        
        result = sincosf_a(aa);
        
        asm {
            ps_merge10 result, result, result
            psq_st     result,  0(_dst), 0, 0
        }
    }

    void Vec2d::CosSinOpt(register Vec2d* _dst, register f32 _angle)
    {
        register __vec2x32float__   aa;
        register __vec2x32float__   result;
        
        _angle = modulo2Pi(_angle);
        
        asm {
            ps_merge00 aa, _angle, _angle
        }
        
        result = sincosf_f(aa);
        
        asm {
            ps_merge10 result, result, result
            psq_st     result,  0(_dst), 0, 0
        }
    }
#endif // ITF_PSI && F32_WII

} // namespace ITF

