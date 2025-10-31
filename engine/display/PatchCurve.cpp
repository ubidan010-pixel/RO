#include "precompiled_engine.h"

#ifndef _ITF_PATCHCURVE_H_
#include "engine/display/PatchCurve.h"
#endif //_ITF_PATCHCURVE_H_

namespace ITF
{
    PatchCurve::PatchCurve()
    {
    }

    PatchCurve::~PatchCurve()
    {
    }

    void PatchCurve::Draw( const Params& _params, Texture* _texture, const f32 _z, const ColorInteger* _colors /*= NULL */ )
    {
        ITF_ASSERT(_texture);
        if (!_texture)
        {
            return;
        }

#ifdef ANIMUSESHADER
        computePoints(m_pointsAndUVs, _params);
        ProceduralPatch::Draw(m_pointsAndUVs, _texture, _z, _colors);
#else
        Vec2d pointsAndUVs[16];
        computePoints(pointsAndUVs, _params);
        ProceduralPatch::Draw(pointsAndUVs, _texture, _z, _colors);
#endif
    }

    void PatchCurve::computePoints( Vec2d* _pointsAndUVs, const Params& _params )
    {
        // POINTS
        {
            Vec2d* points = &_pointsAndUVs[0];

            Vec2d perpN1 = _params.m_n1.getPerpendicular();
            Vec2d perpN2 = _params.m_n2.getPerpendicular()*-1.f;
            const f32 halfWidth = _params.m_width * 0.5f;        
            perpN1 = perpN1.normalize() * halfWidth;
            perpN2 = perpN2.normalize() * halfWidth;

            points[0] = _params.m_p1 - perpN1;
            points[3] = _params.m_p2 - perpN2;
            points[4] = _params.m_p1 + perpN1;
            points[7] = _params.m_p2 + perpN2;

            const f32 lengthP1P2 = (points[0] - points[3]).norm()*.5f;
            const f32 lengthP3P4 = (points[4] - points[7]).norm()*.5f;

            points[1] = points[0] + _params.m_n1 * lengthP1P2;
            points[2] = points[3] + _params.m_n2 * lengthP1P2;

            points[5] = points[4] + _params.m_n1 * lengthP3P4;
            points[6] = points[7] + _params.m_n2 * lengthP3P4;
        }

        // UVs
        {
            Vec2d* uvs = &_pointsAndUVs[8];

            uvs[0]  = _params.m_startUV;
            uvs[3] = Vec2d(_params.m_endUV.m_x, _params.m_startUV.m_y);
            uvs[4] = Vec2d(_params.m_startUV.m_x, _params.m_endUV.m_y);
            uvs[7] = _params.m_endUV;

            const f32 uvLength = f32_Abs(_params.m_endUV.m_x - _params.m_startUV.m_x);
            const Vec2d uvN = Vec2d(uvLength * 0.5f, 0);

            uvs[1] = uvs[0] + uvN;
            uvs[2] = uvs[3] - uvN;

            uvs[5] = uvs[4] + uvN;
            uvs[6] = uvs[7] - uvN;
        }
    }

}