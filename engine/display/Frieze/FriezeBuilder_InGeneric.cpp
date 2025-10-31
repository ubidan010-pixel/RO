#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    void Frise::buildFrieze_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const
    {
        copyEdgeFromPolyline(_edgeList);

        m_recomputeData.m_texRatio = 1.f;

        buildEdgeList( _edgeList);

        if ( m_recomputeData.m_edgeListCount  == 0 )
            return;
    
#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InGeneric( _edgeList, _edgeRunList ) )
        {
            buildVB_FillGradient( _edgeList, _vertexTempList_Static, _indexTempList_Static );
            return;
        }

        FriseConfig* config = getConfig();
        if ( ! config )
            return;

        if ( m_pointsList.isLooping() )
            setEdgeRunCoeffUv( config, _edgeList, _edgeRunList[0] );
        else
        {

            setEdgeRunNormUv( config, _edgeList, _edgeRunList[0] );
            _edgeRunList[0].m_coeff = m_recomputeData.m_texRatio / m_recomputeData.m_widthScale;
        }       

        buildVB_InGeneric( _edgeList, _edgeRunList, _vertexTempList_Static, _indexTempList_Static, _vertexTempList_Anim, _indexTempList_Anim );  
    }

    bbool Frise::buildEdgeRunList_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(1);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_recomputeData.m_edgeListCount;
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        _edgeRunList.push_back(edgeRunCur);

        if ( edgeRunCur.m_idTex == -1 )
            return bfalse;

        return btrue;
    }

    void Frise::buildVB_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;

        const edgeRun& edgeRunCur = _edgeRunList[0];

        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( config->m_textureConfigs[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );

        // uv
        f32 uvYup = m_recomputeData.m_uvYoffSet;
        f32 uvYdown = uvYup + m_uvY_tile;

        if ( m_recomputeData.m_isUvFlipY )
        {
            f32 temp = uvYup;
            uvYup = uvYdown;
            uvYdown = temp;
        }

        Vec2d uvUp(m_recomputeData.m_uvXoffSet, uvYup);
        Vec2d uvDown(m_recomputeData.m_uvXoffSet, uvYdown);

        const MeshElementRouter* elemRouter;
        
        if ( m_recomputeData.m_anim )
        {
            elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagAnimated, edgeRunCur.m_idTex);
            ITF_ASSERT(elemRouter);
            ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Dynamic);

            buildVB_Anim_EdgeRun( config, _edgeList, edgeRunCur, _vertexTempList_Anim, _indexTempList_Anim[elemRouter->m_indexInTable], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, uvUp, uvDown, m_recomputeData.m_uvXsign );   
        }
        else
        {
            elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, edgeRunCur.m_idTex);
            ITF_ASSERT(elemRouter);
            ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Static);

            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, _vertexTempList_Static, _indexTempList_Static[elemRouter->m_indexInTable], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, indexVtxUp, indexVtxDown, uvUp, uvDown, m_recomputeData.m_uvXsign );   
        }
    }

    void Frise::buildCollision_InGeneric( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

} // namespace ITF
