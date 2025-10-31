#include "precompiled_engine.h"

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

namespace ITF
{
    //*****************************************************************************

#ifdef ANIMUSESHADER
    BezierPatch::BezierPatch()
        : m_hdiv(MESH_DEFAULT_HDIV)
        , m_vdiv(MESH_DEFAULT_VDIV)
        , m_vdivToDraw(U32_INVALID)
        , m_texture(NULL)
        , m_z(0.f)
		, m_blendMode(GFX_BLEND_ALPHA)
    {
        // Init

        for (i32 i = 0; i < 8; i++)
        {
            m_pointsAndUVs[i] = Vec2d::Zero;
        }

        for (i32 i = 0; i < 4; i++)
        {
            m_widths[i] = 0.0f;
            m_widths2[i] = 0.0f;
            m_alphas[i] = 0.0f;
        }
    }
#else // ANIMUSESHADER
    BezierPatch::BezierPatch()
        : mp_Data(NULL)
    {
        m_mesh.m_colorFog    = Color::zero();
        m_mesh.m_globalColor = Color::white();
    }
#endif // ANIMUSESHADER

    //*****************************************************************************

    BezierPatch::BezierPatch(const BezierPatch& _src)
    {
        *this = _src;
    }

    //*****************************************************************************

    BezierPatch& BezierPatch::operator =(const BezierPatch& _src)
    {
#ifdef ANIMUSESHADER
        m_hdiv      = _src.m_hdiv;
        m_vdiv      = _src.m_vdiv;
        m_vdivToDraw = _src.m_vdivToDraw;

        m_texture   = _src.m_texture;
        m_z         = _src.m_z;
        m_fogColor  = _src.m_fogColor;
        m_blendMode = _src.m_blendMode;

        for (i32 i = 0; i < 8; i++)
        {
            m_pointsAndUVs[i] = _src.m_pointsAndUVs[i];
        }
        for (i32 i = 0; i < 2; i++)
        {
            m_colors[i] = _src.m_colors[i];
        }
        for (i32 i = 0; i < 4; i++)
        {
            m_widths[i] = _src.m_widths[i];
            m_widths2[i] = _src.m_widths2[i];
            m_alphas[i] = _src.m_alphas[i];
        }
#else // ANIMUSESHADER
        mp_Data = NULL;

        m_mesh.m_colorFog    = Color::zero();
        m_mesh.m_globalColor = Color::white();
#endif // ANIMUSESHADER

        return *this;
    }

    //*****************************************************************************

    BezierPatch::~BezierPatch()
    {
#ifndef ANIMUSESHADER
        #ifdef ITF_WII
        if(mp_Data) GFX_ADAPTER->syncGPU();
        #endif // ITF_WII
        m_mesh.emptyIndexedMesh();
        SF_DEL(mp_Data);
#endif // ANIMUSESHADER
    }

    //*****************************************************************************

    void BezierPatch::drawPatch( const BezierPatchParams& _params )
    {
        ITF_ASSERT_MSG( !(_params.m_p0.IsEqual(_params.m_p1) && _params.m_p0.IsEqual(_params.m_p2) && _params.m_p0.IsEqual(_params.m_p3)), "drawing an empty bezier!" );

#ifdef ANIMUSESHADER
        const f32 textureRatio = _params.m_texture->getSizeX() ? f32(_params.m_texture->getSizeY()) / f32(_params.m_texture->getSizeX()) : 1.f;
#else // ANIMUSESHADER
        const f32 textureRatio = 1.f;
#endif // ANIMUSESHADER

        const f32 widths[4] = {_params.m_startWidth, _params.m_endWidth,    // mesh width
             _params.m_endUV.m_y - _params.m_startUV.m_y, _params.m_endUV.m_y - _params.m_startUV.m_y};  // uv width

        const f32 widths2[4] = {_params.m_widthLimitA, _params.m_widthLimitB,
             _params.m_widthValueA, _params.m_widthValueB};

        const ColorInteger colors[2] = { _params.m_startColor.getAsU32(), _params.m_endColor.getAsU32() };

        // Alpha 
        const f32 alphas[4] = {_params.m_alphaLimitA, _params.m_alphaLimitB, _params.m_alphaValueA, _params.m_alphaValueB};


        Vec2d pointsAndUVs[8];

        // Compute Points
        {
            pointsAndUVs[0] = _params.m_p0;
            pointsAndUVs[1] = _params.m_p1;
            pointsAndUVs[2] = _params.m_p2;
            pointsAndUVs[3] = _params.m_p3;
        }

        // Compute UVs
        {
            Vec2d* uvs = &pointsAndUVs[4];

            if (_params.m_uvMode == UV_MODE_SPEED)
            {

                Vec2d startUV = Vec2d(_params.m_startUV.m_x, _params.m_startUV.m_y * textureRatio);
                Vec2d endUV  = Vec2d(_params.m_endUV.m_x, _params.m_endUV.m_y * textureRatio);

                uvs[0] = Vec2d(startUV.m_x                      , (startUV.m_y + endUV.m_y) * 0.5f);
                uvs[3] = Vec2d(endUV.m_x                        , (startUV.m_y + endUV.m_y) * 0.5f);

                uvs[1] = Vec2d((startUV.m_x + endUV.m_x) * 0.5f , (startUV.m_y + endUV.m_y) * 0.5f);
                uvs[2] = Vec2d((startUV.m_x + endUV.m_x) * 0.5f , (startUV.m_y + endUV.m_y) * 0.5f);

            }
            else // UV_MODE_OPTIMUM
            {
                const Vec2d uv1(_params.m_startUV.m_x, (_params.m_startUV.m_y + _params.m_endUV.m_y) * 0.5f * textureRatio);
                const Vec2d uv2(_params.m_endUV.m_x, (_params.m_startUV.m_y + _params.m_endUV.m_y) * 0.5f * textureRatio);

                const f32 uvLength = f32_Abs(_params.m_endUV.m_x - _params.m_startUV.m_x);

                // cheap bezier length integration
                const f32 n0 = (pointsAndUVs[1] - pointsAndUVs[0]).norm();
                const f32 n1 = (pointsAndUVs[2] - pointsAndUVs[1]).norm();
                const f32 n2 = (pointsAndUVs[3] - pointsAndUVs[2]).norm();
                ITF_ASSERT_MSG((n0 + n1 + n2) != 0, "divide by zero coming, are you drawing an empty bezier?");
                const f32 nRatio = uvLength / (n0 + n1 + n2);

                uvs[0] = uv1;
                uvs[1] = uv1 + Vec2d(n0 * nRatio, 0);
                uvs[2] = uv2 - Vec2d(n2 * nRatio, 0);
                uvs[3] = uv2;
            }
        }

        // Draw bezier patch
        Draw(pointsAndUVs, widths, widths2, alphas, colors, _params);
    }

    //*****************************************************************************

    void BezierPatch::Draw(
        const Vec2d* _pointsAndUVs, const f32* _widths,
        const f32* _widths2,
        const f32* _alphas, const ColorInteger* _colors, 
        const BezierPatchParams& _params )
    {
        ITF_ASSERT(_params.m_texture);
        if (!_params.m_texture)
        {
            return;
        }

#ifdef ANIMUSESHADER
        // save data and push in z list, draw later

        m_blendMode = _params.m_blendMode;
        m_fogColor = _params.m_fogColor;

        m_texture = _params.m_texture;
        ITF_MEMCOPY(m_pointsAndUVs, _pointsAndUVs, 8 * sizeof(Vec2d));
        ITF_MEMCOPY(m_widths, _widths, 4 * sizeof(f32));
        ITF_MEMCOPY(m_alphas, _alphas, 4 * sizeof(f32));
        ITF_MEMCOPY(m_widths2, _widths2, 4 * sizeof(f32));

        m_z = _params.m_z;

        if (_colors)
        {
            ITF_MEMCOPY(m_colors, _colors, 2 * sizeof(ColorInteger));
        }
        else
        {
            const ColorInteger* defaultColors = GFX_ADAPTER->getDefault4WhiteColorTab();
            ITF_MEMCOPY(m_colors, defaultColors, 2 * sizeof(ColorInteger));
        }

        GFX_ADAPTER->addPrimitiveInZList(this, GFX_ITF_BEZIERPATCH, NULL, m_z,0,ObjectRef::InvalidRef);

        m_hdiv = _params.m_hdiv;
        m_vdiv = _params.m_vdiv;
        m_vdivToDraw = _params.m_vdivToDraw;
#else // ANIMUSESHADER
        u32 hIndex =  _params.m_hdiv;
        u32 vIndex =  _params.m_vdiv;

        u32 hdiv = _params.m_hdiv;
        u32 vdiv = Min(_params.m_vdiv, _params.m_vdivToDraw);

        if(_colors && ((_colors[0].getAsU32() != 0xFFFFFFFF) || (_colors[0].getAsU32() != _colors[1].getAsU32()) || (_alphas[2] != 1.f) || (_alphas[3] != 1.f)))
        {
            if(!mp_Data || (m_mesh.m_type != ITF_Mesh::meshType_PTC))
            {
                #ifdef ITF_WII
                if(mp_Data) GFX_ADAPTER->syncGPU();
                #endif // ITF_WII

                m_mesh.emptyIndexedMesh();
                m_mesh.initStaticIndexMesh(VertexFormat_PCT, GFX_ADAPTER->getIndexBuffer(hIndex, vIndex), (hIndex + 1)*(vIndex + 1));

#if PATCH32_WORKINGBUFF == 0
                if(!mp_Data)
#endif // PATCH32_WORKINGBUFF
                {
                    SF_DEL(mp_Data);
                    mp_Data = newAlloc(mId_Animation, Patch32Data(VertexFormat_PCT));
                }
            }
            mp_Data->Init(&m_mesh, VertexFormat_PCT);

#if PATCH32_WORKINGBUFF == 0
            mp_Data->vertexListPCT = (VertexPCT*)mp_Data->LockVBuff();
#endif // PATCH32_WORKINGBUFF
            if((_colors[0] == _colors[1]) && (_colors[0].getF32Alpha() == _alphas[2]) && (_colors[0].getF32Alpha() == _alphas[3]))
                GFX_ADAPTER->drawBezierPatch32PCTOneColor(mp_Data, _params.m_texture, _pointsAndUVs, _widths, _widths2, _alphas, vdiv, hdiv, _colors[0].getAsU32(), _params.m_z);
            else
                GFX_ADAPTER->drawBezierPatch32PCT(mp_Data, _params.m_texture, _pointsAndUVs, _widths, _widths2, _alphas, vdiv, hdiv, _colors, _params.m_z);
        }
        else
        {
            if(!mp_Data || (m_mesh.m_type != ITF_Mesh::meshType_PTa))
            {
                #ifdef ITF_WII
                if(mp_Data) GFX_ADAPTER->syncGPU();
                #endif // ITF_WII

                m_mesh.emptyIndexedMesh();
                m_mesh.initStaticIndexMesh(VertexFormat_PT, GFX_ADAPTER->getIndexBuffer(hIndex, vIndex), (hIndex + 1)*(vIndex + 1));

#if PATCH32_WORKINGBUFF == 0
                if(!mp_Data)
#endif // PATCH32_WORKINGBUFF
                {
                    SF_DEL(mp_Data);
                    mp_Data = newAlloc(mId_Animation, Patch32Data(VertexFormat_PT));
                }
            }
            mp_Data->Init(&m_mesh, VertexFormat_PTa);

#if PATCH32_WORKINGBUFF == 0
            mp_Data->vertexListPT = (VertexPT*)mp_Data->LockVBuff();
#endif // PATCH32_WORKINGBUFF
            GFX_ADAPTER->drawBezierPatch32PT(mp_Data, _params.m_texture, _pointsAndUVs, _widths, _widths2,  _alphas, vdiv, hdiv, _params.m_z);
        }

#if PATCH32_WORKINGBUFF == 0
        mp_Data->UnlockVBuff();
#else // PATCH32_WORKINGBUFF
        mp_Data->CopyToMesh();
#endif // PATCH32_WORKINGBUFF

        ITF_ASSERT(m_mesh.m_ElementList.size() > 0);
        m_mesh.m_ElementList[0].m_material.m_blendMode = _params.m_blendMode;

        m_mesh.m_colorFog = _params.m_fogColor;
        GFX_ADAPTER->addPrimitiveInZList(&m_mesh, GFX_ITF_MESH, NULL, _params.m_z,0,ObjectRef::InvalidRef);
#endif // ANIMUSESHADER
    }

    //*****************************************************************************

    void BezierPatch::DrawShader()
    {
#ifdef ANIMUSESHADER    
        ITF_ASSERT(m_texture);
        if (!m_texture)
        {
            return;
        }

        GFX_ADAPTER->prepareShaderBezierPatch((GMatrix44 *)&GMatrixIdentity, m_z, m_hdiv, m_vdiv, m_blendMode);
        GFX_ADAPTER->shaderDrawBezierPatch32(m_texture, m_pointsAndUVs, m_colors, m_widths, m_alphas, m_widths2);
        GFX_ADAPTER->shaderBezierPatchFlush(m_hdiv, m_vdiv, 0, m_vdivToDraw);
        GFX_ADAPTER->endShaderPatch();
#endif // ANIMUSESHADER
    }

    //*****************************************************************************
}
