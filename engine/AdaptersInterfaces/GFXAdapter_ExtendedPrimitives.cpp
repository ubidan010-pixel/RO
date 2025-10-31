#include "precompiled_engine.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SPRITE_H_
#include "engine/display/Sprite.h"
#endif //_ITF_SPRITE_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_SPRITE_H_
#include "engine/display/Sprite.h"
#endif //_ITF_SPRITE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif

#ifdef __CWCC__
//#pragma ppc_unroll_instructions_limit 20000
#endif

#if defined( ITF_WII ) && !defined( _DEBUG )
#define USE_FLOAT2
#endif // defined( ITF_WII ) && !defined( _DEBUG )

namespace ITF
{
#ifndef ITF_CONSOLE_FINAL
    void GFXAdapter::drawArrow(const Vec2d& pos1, const Vec2d& pos2, float _r, float _g, float _b, float _rad, float _z, float _duration, bbool _forceRad  )
    {
        if (_forceRad)
        {
            _rad *= f32_Sqrt(CAMERA->getZ());
            if (_z<10.f)
                _rad *= (10.f - _z) / 10.f;
        }

        Vec2d vec = pos2-pos1;
        Vec2d nVec = vec;
        nVec.normalize();
        nVec *= _rad;
        Vec2d horiz = nVec.getPerpendicular();
        Vec2d pt1 = pos1 - horiz * 0.8f;
        Vec2d pt2 = pos1 + horiz * 0.8f;
        Vec2d pt3 = pos2 + horiz * 0.5f;
        Vec2d pt4 = pos2 - horiz * 0.5f;
        
        drawDBGTriangle(Vec2d(pos2 - horiz), Vec2d(pos2 + horiz), Vec2d(pos2 + nVec), _r, _g, _b, 1.f, _duration, _z);
        drawDBGTriangle(pt1, pt2, pt3, _r, _g, _b, 1.f, _duration, _z);
        drawDBGTriangle(pt1, pt3, pt4, _r, _g, _b, 1.f, _duration, _z);
    }

    void GFXAdapter::drawArrow(const Vec3d& pos1, const Vec3d& pos2, float _r, float _g, float _b, float _rad, float _duration, bbool _forceRad  )
    {
        Vec2d p1, p2;
        if(pos1.m_z - pos2.m_z < 0.00001f)
        { 
            p1.truncateVec3d(pos1);
            p2.truncateVec3d(pos2);
        }
        else
        {
            Vec3d v1, v2, pos3;
            compute3DTo2D(pos1, v1);
            compute3DTo2D(pos2, v2);
            v2.m_z = v1.m_z;
            compute2DTo3D(v2, pos3);

            p1= pos1.truncateTo2D();
            p2= pos3.truncateTo2D();
        }

        drawArrow(p1, p2, _r, _g, _b, _rad, pos1.m_z, _duration, _forceRad  );
    }
#endif // ITF_CONSOLE_FINAL

#define MAX_CIRCLE_DISC 256   
#define MAX_DIVH        32
#define MAX_DIVV        32

    /// Patch
    
    bbool GFXAdapter::drawPatch32PT(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, f32 _z)
    {
#undef PATCH32_USE_PCT
#undef PATCH32_USE_COLOR
#include "engine/AdaptersInterfaces/GFXAdapter_DrawPatch.h"
    }
    
    bbool GFXAdapter::drawPatch32PCT(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, ColorInteger *_color, f32 _z)
    {
#define PATCH32_USE_PCT
#define PATCH32_USE_COLOR
#include "engine/AdaptersInterfaces/GFXAdapter_DrawPatch.h"
#undef PATCH32_USE_COLOR
#undef PATCH32_USE_PCT
    }

    bbool GFXAdapter::drawPatch32PCTOneColor(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, u32 _generalColor, f32 _z)
    {
#define PATCH32_USE_PCT
#undef PATCH32_USE_COLOR
#include "engine/AdaptersInterfaces/GFXAdapter_DrawPatch.h"
#undef PATCH32_USE_PCT
    }


    /// Bezier

    bbool GFXAdapter::drawBezierPatch32PT(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, f32 _z)
    {
#undef PATCH32_USE_PCT
#undef PATCH32_USE_COLOR
#include "engine/AdaptersInterfaces/GFXAdapter_DrawBezierPatch.h"
    }

    bbool GFXAdapter::drawBezierPatch32PCT(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, const ColorInteger *_color, f32 _z)
    {
#define PATCH32_USE_PCT
#define PATCH32_USE_COLOR
#include "engine/AdaptersInterfaces/GFXAdapter_DrawBezierPatch.h"
#undef PATCH32_USE_COLOR
#undef PATCH32_USE_PCT
    }

    bbool GFXAdapter::drawBezierPatch32PCTOneColor(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, u32 _generalColor, f32 _z)
    {
#define PATCH32_USE_PCT
#undef PATCH32_USE_COLOR
#include "engine/AdaptersInterfaces/GFXAdapter_DrawBezierPatch.h"
#undef PATCH32_USE_PCT
    }

} //namespace ITF
