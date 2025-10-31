#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    static const f32 stepOverlay = 2.f;

    void Frise::buildFrieze_InOverlay( ITF_VECTOR<edgeFrieze>& _edgeList, MeshOverlay_VertexList*& _vertexTempList_Overlay, MeshOverlay_IndexList*& _indexTempList_Overlay) const
    {
        const FriseConfig* config = getConfig();
        if ( !config || !m_recomputeData.m_anim )
            return;

        i32 idTexConfig = getFirstNoFillTextureIndex( config ); 
        if ( idTexConfig == -1 )
            return;

        const FriseTextureConfig& texConfig = config->m_textureConfigs[idTexConfig];
        if ( (texConfig.getPath().isEmpty()) ) 
            return;

        copyEdgeFromPolyline(_edgeList);
        buildEdgeList( _edgeList);

        if ( m_recomputeData.m_edgeListCount  == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        buildVB_InOverlay( config, texConfig, _edgeList, _vertexTempList_Overlay, _indexTempList_Overlay );  
    }

    void Frise::buildVB_InOverlay( const FriseConfig* _config, const FriseTextureConfig& _texConfig, const ITF_VECTOR<edgeFrieze>& _edgeList, MeshOverlay_VertexList *& _vertexTempList_Overlay, MeshOverlay_IndexList *& _indexTempList_Overlay ) const
    {        
        const Texture* tex = (const Texture*)_texConfig.m_textureData.getResource();           
        if ( !tex )
            return;

        const UVAtlas *atlas = tex->getUVAtlas();
        if ( !atlas )
        {
#ifndef ITF_FINAL
            String8 msg("Overlay's atlas file is missing for : ");
            msg += tex->getPath().getString().cStr();
            ITF_WARNING_CATEGORY(Graph,this, 0, msg.cStr());
#endif
            return;
        }

        const u32 uvDataCount = atlas->getNumberUV();
        if ( uvDataCount == 0 )
        {
#ifndef ITF_FINAL
            String8 msg("Overlay's atlas file is empty for : ");
            msg += tex->getPath().getString().cStr();
            ITF_WARNING_CATEGORY(Graph,this, 0, msg.cStr());                
#endif
            return;
        }

        // mesh temp allocation
        _vertexTempList_Overlay = newAlloc(mId_Frieze, MeshOverlay_VertexList);
        _indexTempList_Overlay = newAlloc(mId_Frieze, MeshOverlay_IndexList);
        
        const f32 scale = _config->m_useFriezeScale ? _config->m_scale *( getScale().m_x + getScale().m_y) *0.5f : _config->m_scale;        
        const f32 density = 1.f -Clamp( _config->m_density, 0.f, 1.f);
        u32 indexAnim = 0;

        Vec2d pos[4];
        Vec2d uv[4];
        u32 color[4];
        setVtxColor( _texConfig, color[1], color[0]); 
        color[2] = color[0];
        color[3] = color[1];

        for (u32 idEdge=0; idEdge< m_recomputeData.m_edgeListCount; idEdge++)
        {
            const edgeFrieze& edge = _edgeList[idEdge];

            if ( edge.m_snap || isEdgeWithHoleVisual(edge))
                continue;

            Seeder seed( (u32)(f32_Abs(edge.m_points[0].m_x)*edge.m_norm) );

            // get uvData
            u32 uvDataId = seed.GetU32( 0, uvDataCount -1);
            UVdata uvData = atlas->getUVDatabyIndex(uvDataId);
            f32 longUvDataX = uvData.getUV1().m_x - uvData.getUV0().m_x;
            f32 longUvDataY = uvData.getUV1().m_y - uvData.getUV0().m_y;
            f32 longOverlay = longUvDataX *scale;
            f32 heightOverlay = longUvDataY *scale;
            f32 uvXStart = longUvDataX *(f32)seed.GetU32( 0, 100);
            f32 uvXsens = seed.GetFloat(-10.f, 10.f);
            f32 heightOffset = m_recomputeData.m_heightScale *f32_Modulo(seed.GetFloat( 0.f, 10.f), 1.f);
            f32 startAccumulate = seed.GetFloat( 0.f, density) *stepOverlay;


            while ( startAccumulate + longOverlay <= edge.m_norm )
            {
                const Vec2d normalHeightOffset = edge.m_normal *heightOffset;
                pos[0] = edge.m_points[0] +edge.m_sightNormalized *startAccumulate +normalHeightOffset;
                pos[1] = pos[0] +edge.m_normal *heightOverlay;
                pos[2] = pos[0] +edge.m_sightNormalized*longOverlay;
                pos[3] = pos[2] +edge.m_normal *heightOverlay;

                Vec2d uv0 = uvData.getUV0();
                Vec2d uv1 = uvData.getUV1();
                uv0.m_x += uvXStart;
                uv1.m_x += uvXStart;
                if ( uvXsens <= 0.f )
                {
                    const f32 temp = uv0.m_x;
                    uv0.m_x = uv1.m_x;
                    uv1.m_x = temp;
                }

                uv[0] = Vec2d(uv0.m_x, uv1.m_y);
                uv[1] = uv0;
                uv[2] = uv1;
                uv[3] = Vec2d(uv1.m_x, uv0.m_y);

                buildVB_Overlay_Quad( _vertexTempList_Overlay, _indexTempList_Overlay, &pos[0], &uv[0], &color[0], 1 +indexAnim );
                indexAnim = ( indexAnim +1) %8;

                startAccumulate += Max( longOverlay*0.5f, seed.GetFloat( 0.f, density) *stepOverlay );

                uvDataId = seed.GetU32( 0, 1000)%uvDataCount;
                uvData = atlas->getUVDatabyIndex(uvDataId);
                longUvDataX = uvData.getUV1().m_x - uvData.getUV0().m_x;
                longUvDataY = uvData.getUV1().m_y - uvData.getUV0().m_y;
                longOverlay = longUvDataX *scale *seed.GetFloat(0.8f,1.2f);
                heightOverlay = longUvDataY *scale;
                uvXStart = longUvDataX *(f32)seed.GetU32( 0, 100);
                uvXsens = seed.GetFloat(-10.f, 10.f);
                heightOffset = m_recomputeData.m_heightScale *f32_Modulo(seed.GetFloat( 0.f, 10.f), 1.f);
            }               
        }
    }

    void Frise::initOverlay( const VertexAnim* _anim )
    {
        if ( !m_pMeshOverlayData )
            return;

        f32_CosSin( _anim->m_angle.ToRadians(), &m_pMeshOverlayData->m_angle.m_x, &m_pMeshOverlayData->m_angle.m_y);

        for ( u32 i=0; i<8; i++)
        {
            m_pMeshOverlayData->m_syncVtx[i].m_x = f32_Cos( m_recomputeData.m_animSyncXCur );
            m_pMeshOverlayData->m_syncVtx[i].m_y = f32_Sin( m_recomputeData.m_animSyncYCur );

            f32_CosSin( m_recomputeData.m_animSyncCur, &m_pMeshOverlayData->m_sync[i].m_x, &m_pMeshOverlayData->m_sync[i].m_y);

            m_recomputeData.m_animSyncCur += _anim->m_sync;
            m_recomputeData.m_animSyncXCur += _anim->m_syncX;
            m_recomputeData.m_animSyncYCur += _anim->m_syncY;
        }
    }
} // namespace ITF

