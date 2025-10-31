#include "precompiled_engine.h"

#ifndef _ITF_PROCEDURALPATCH_H_
#include "engine/display/ProceduralPatch.h"
#endif //_ITF_PROCEDURALPATCH_H_

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
    ProceduralPatch::ProceduralPatch()
        // Voluntary inversion !
        : m_hdiv(MESH_DEFAULT_VDIV)
        , m_vdiv(MESH_DEFAULT_HDIV)
#ifdef ANIMUSESHADER
        , m_texture(NULL)
        , m_z(0.f)
#endif
    {
#ifdef ANIMUSESHADER
#else
        m_mesh.emptyIndexedMesh();
        m_mesh.initStaticIndexMesh(VertexFormat_PT, GFX_ADAPTER->getIndexBuffer(m_hdiv, m_vdiv), (m_hdiv + 1)*(m_vdiv + 1));
#endif
    }

    ProceduralPatch::~ProceduralPatch()
    {
#ifndef ANIMUSESHADER
        m_mesh.emptyIndexedMesh();
#endif
    }


    void ProceduralPatch::Draw( const Vec2d* _pointsAndUVs, Texture* _texture, f32 _z, const ColorInteger* _colors /*= NULL*/ )
    {
        ITF_ASSERT(_texture);
        if (!_texture)
        {
            return;
        }

#ifdef ANIMUSESHADER
        // save data and push in z list, draw later

        m_texture = _texture;
        ITF_MEMCOPY(m_pointsAndUVs, _pointsAndUVs, 16 * sizeof(Vec2d));
        m_z = _z;

        if (_colors)
        {
            ITF_MEMCOPY(m_colors, _colors, 4 * sizeof(ColorInteger));
        }
        else
        {
            const ColorInteger* defaultColors = GFX_ADAPTER->getDefault4WhiteColorTab();
            ITF_MEMCOPY(m_colors, defaultColors, 4 * sizeof(ColorInteger));
        }

        GFX_ADAPTER->addPrimitiveInZList(this, GFX_ITF_PROCEDURALPATCH, NULL, m_z,0,ObjectRef::InvalidRef);

#else
        // draw immediate

        Patch32Data* pData = ANIM_MANAGER->getPatchDataBuffer();
        pData->Init(&m_mesh, VertexFormat_PT);

#if PATCH32_WORKINGBUFF == 0
        pData->vertexListPT = (VertexPT*)pData->LockVBuff();
        GFX_ADAPTER->drawPatch32PT(pData, _texture, _pointsAndUVs, m_hdiv, m_vdiv, _z);
        pData->UnlockVBuff();
#else // PATCH32_WORKINGBUFF
        GFX_ADAPTER->drawPatch32PT(pData, _texture, _pointsAndUVs, m_hdiv, m_vdiv, _z);
        pData->CopyToMesh();
#endif // PATCH32_WORKINGBUFF
        GFX_ADAPTER->addPrimitiveInZList(&m_mesh, GFX_ITF_MESH, NULL, _z,0,ObjectRef::InvalidRef);

#endif
    }

    void ProceduralPatch::DrawShader()
    {
#ifdef ANIMUSESHADER    
        ITF_ASSERT(m_texture);
        if (!m_texture)
        {
            return;
        }


        //if (_matrix != NULL)
        //{
        //    GFX_ADAPTER->prepareShaderPatch(_matrix, m_z, m_hdiv, m_vdiv);
        //}
        //else
        //{
        //    GFX_ADAPTER->prepareShaderPatch((GMatrix44 *)&GMatrixIdentity, m_z, m_hdiv, m_vdiv);
        //}
        GFX_ADAPTER->prepareShaderPatch((GMatrix44 *)&GMatrixIdentity, m_z, m_hdiv, m_vdiv);

        GFX_ADAPTER->shaderDrawPatch32(m_texture, m_pointsAndUVs, m_colors);
        GFX_ADAPTER->shaderPatchFlush(m_hdiv, m_vdiv, 0);
        GFX_ADAPTER->endShaderPatch();
#endif        
    }

}
