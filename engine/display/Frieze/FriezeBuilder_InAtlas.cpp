#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    void Frise::buildFrieze_InAtlas( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim, u32& _atlasIndex ) const
    {
        FriseConfig* config = getConfig();
        if ( !config || config->m_textureConfigs.size()==0 )
            return;
        
        Vec2d uvUp(Vec2d::Zero);
        Vec2d uvDown(Vec2d::Up);
        bbool isUvAtlas = bfalse;

        if ( !setTextureData_InAtlas( config, &uvUp, &uvDown, isUvAtlas, _atlasIndex))
            return;

        f32 longEdgeListWanted = setRatioFixed_InAtlas( config, _edgeList, &uvUp, &uvDown, isUvAtlas );

        copyEdgeFromPolyline(_edgeList);
        buildEdgeList( _edgeList);

        if ( m_recomputeData.m_edgeListCount == 0 )
            return;
    
        MatchEdgesWithRatioFixed( _edgeList, longEdgeListWanted);

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InGeneric( _edgeList, _edgeRunList ) )
            return;

        edgeRun& edgeRunCur = _edgeRunList[0]; 
        setEdgeRunNormUv( config, _edgeList, edgeRunCur);
        setEdgeRunCoeffUv_InAtlas( _edgeList, edgeRunCur);

        buildVB_InAtlas( _edgeList, edgeRunCur, &uvUp, &uvDown, _vertexTempList_Static, _indexTempList_Static, _vertexTempList_Anim, _indexTempList_Anim );  
    }

    void Frise::MatchEdgesWithRatioFixed( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _longWanted) const
    {
        if ( _longWanted == 0.f || m_recomputeData.m_edgeListCount == 0 )
            return;

        f32 longCurrent = 0.f;
        for ( u32 i=0; i<m_recomputeData.m_edgeListCount; i++)
            longCurrent += _edgeList[i].m_norm;

        f32 deltaLongBy2 = (_longWanted -longCurrent) *0.5f;

        if ( deltaLongBy2 > MTH_EPSILON)
        {
            edgeFrieze& edgeStart = _edgeList[0]; 
            Vec2d offset = deltaLongBy2 *edgeStart.m_sightNormalized;

            edgeStart.m_snap = bfalse;
            edgeStart.m_sight += offset;
            edgeStart.m_norm += deltaLongBy2;
            edgeStart.m_pos -= offset;
            edgeStart.m_points[0] -= offset;
            edgeStart.m_points[1] -= offset;
            edgeStart.m_interDown = edgeStart.m_points[0];
            edgeStart.m_interUp = edgeStart.m_points[1];

            u32 idEdgeLast = m_recomputeData.m_edgeListCount -1;            
            edgeFrieze& edgeLast = _edgeList[idEdgeLast];

            offset = deltaLongBy2 *edgeLast.m_sightNormalized;
            edgeLast.m_snap = bfalse;
            edgeLast.m_sight += offset;
            edgeLast.m_norm += deltaLongBy2;
            edgeLast.m_points[2] += offset;
            edgeLast.m_points[3] += offset;
        }
    }

    f32 Frise::setRatioFixed_InAtlas( const FriseConfig* _config, ITF_VECTOR<edgeFrieze>& _edgeList, Vec2d* _uvUp, Vec2d* _uvDown, bbool _isUvAtlas ) const
    {     
        f32 longEdgeList = 0.f;
        u32 edgeCount = m_pointsList.getPosCount();

        if ( _config->m_isRatioFixed && m_recomputeData.m_texRatio>0.f && edgeCount >1 )
        {
            edgeCount --;

            f32 ratio = m_recomputeData.m_heightScale / m_recomputeData.m_widthScale;
            if ( _isUvAtlas )
            {
                f32 deltaY = f32_Abs( _uvUp->m_y - _uvDown->m_y);
                ratio *= deltaY /m_recomputeData.m_texRatio;
            }
             
            for ( u32 i=0; i<edgeCount; i++)
                longEdgeList += m_pointsList.getEdgeAt(i).m_length;

            m_recomputeData.m_heightScale =  longEdgeList *ratio;
        }

        return longEdgeList;
    }

    bbool Frise::setTextureData_InAtlas( const FriseConfig* _config, Vec2d* _uvUp, Vec2d* _uvDown, bbool& _isUvAtlas, u32& _atlasIndex) const
    {
        m_recomputeData.m_texRatio = 1.f;

        i32 idTex = getFirstNoFillTextureIndex( _config );
        if ( idTex == -1)
            return bfalse;

        const FriseTextureConfig& texConfig = _config->m_textureConfigs[idTex];

        if ( texConfig.getPath().isEmpty() )
            return bfalse;

        Texture* tex = (Texture*)texConfig.m_textureData.getResource();           
        if ( !tex )
            return bfalse;

        const UVAtlas *atlas = tex->getUVAtlas();
        if ( !atlas )
            return btrue;

        u32 uvDataCount = atlas->getNumberUV();
        if ( uvDataCount == 0 )
            return btrue;

        _isUvAtlas = btrue;

        _atlasIndex = Min( _atlasIndex, uvDataCount-1);

        UVdata uvData = atlas->getUVDatabyIndex(_atlasIndex);

        m_recomputeData.m_texRatio = f32_Abs(uvData.getUV1().m_x - uvData.getUV0().m_x);

        *_uvUp = uvData.getUV0();
        _uvDown->m_x = _uvUp->m_x;
        _uvDown->m_y = uvData.getUV1().m_y;

        return btrue;
    }

    void Frise::setEdgeRunCoeffUv_InAtlas( const ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun) const
    {
        f32 normUvCount = 0.f;

        for ( u32 count = 0; count < m_recomputeData.m_edgeListCount; count ++ )        
            normUvCount += _edgeList[count].m_normUv;

        if ( normUvCount >0.f )
            _edgeRun.m_coeff = m_recomputeData.m_texRatio /normUvCount;
    }

    void Frise::buildVB_InAtlas( ITF_VECTOR<edgeFrieze>& _edgeList, edgeRun& _edgeRun, Vec2d* _uvUp, Vec2d* _uvDown, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        if ( m_recomputeData.m_isUvFlipY )
        {
            f32 temp = _uvUp->m_y;
            _uvUp->m_y = _uvDown->m_y;
            _uvDown->m_y = temp;
        }

        if ( m_recomputeData.m_uvXsign <0.f)
        {
            _uvUp->m_x += m_recomputeData.m_texRatio;
            _uvDown->m_x = _uvUp->m_x;
        }

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;

        if ( m_recomputeData.m_edgeListCount >1 )
        {
            setVtxColorWithExtrem( config->m_textureConfigs[_edgeRun.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );
        }
        else
        {
            setVtxColor( config->m_textureConfigs[_edgeRun.m_idTex], vtxColorUp, vtxColorDown);
            vtxColorUpExtremity = vtxColorUp;
            vtxColorDownExtremity = vtxColorDown;
        }

        const MeshElementRouter* elemRouter;
        
        if ( m_recomputeData.m_anim )
        {
            elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagAnimated, _edgeRun.m_idTex);
            ITF_ASSERT(elemRouter);
            ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Dynamic);

            buildVB_Anim_EdgeRun( config, _edgeList, _edgeRun, _vertexTempList_Anim, _indexTempList_Anim[elemRouter->m_indexInTable], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, *_uvUp, *_uvDown, m_recomputeData.m_uvXsign );   
        }
        else
        {
            elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, _edgeRun.m_idTex);
            ITF_ASSERT(elemRouter);
            ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Static);

            buildVB_Static_EdgeRun( config, _edgeList, _edgeRun, _vertexTempList_Static, _indexTempList_Static[elemRouter->m_indexInTable], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, *_uvUp, *_uvDown, m_recomputeData.m_uvXsign );   
        }
    }

} // namespace ITF
