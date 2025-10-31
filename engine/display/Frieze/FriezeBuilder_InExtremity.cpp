#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    f32 Frise::getExtremityLong( f32 _ratio ) const
    {
        return m_recomputeData.m_widthScale *_ratio;
    }

    void Frise::subtractOffsetExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _offset ) const
    {
        if ( m_recomputeData.m_edgeListCount == 0 )
            return;
        
        const f32 epsilon = 0.001f;

        // start edge
        edgeFrieze& startEdge = _edgeList[0];
        Vec2d newPos;
        f32 edgeNorm = startEdge.m_sight.norm();    
        f32 startOffset = _offset *startEdge.m_scaleCur;

        if (m_recomputeData.m_edgeListCount == 1 && startOffset > edgeNorm *0.5f +epsilon)
        {
            newPos = startEdge.m_pos + startEdge.m_sightNormalized *(edgeNorm *0.5f -epsilon); 
        }
        else
        {
            if ( edgeNorm > startOffset +epsilon )
            {
                newPos = startEdge.m_pos + startEdge.m_sightNormalized *startOffset;            
            }
            else
            {
                newPos = startEdge.m_pos + startEdge.m_sightNormalized *( edgeNorm -epsilon);
            }
        }
        updateEdgeFriezePosition( startEdge, newPos );

        // stop edge
        edgeFrieze& stopEdge = _edgeList[m_recomputeData.m_edgeListCount -1];
        Vec2d newSight;
        f32 stopOffset = _offset *stopEdge.m_heightStop * m_recomputeData.m_heightScaleInv;

        if (  stopEdge.m_sight.norm() > stopOffset +epsilon )
        {
            newSight = stopEdge.m_sight -stopEdge.m_sightNormalized *stopOffset;
        }
        else
        {
            newSight = stopEdge.m_sightNormalized *epsilon;
        }

        updateEdgeFriezeVector( stopEdge, newSight );
    }

    void Frise::buildFrieze_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        copyEdgeFromPolyline(_edgeList);        

        m_recomputeData.m_texRatio = 0.5f;
        f32 extremityRatio = 0.25f;

        subtractOffsetExtremity( _edgeList, getExtremityLong(extremityRatio) );

        buildEdgeList( _edgeList );

        if ( m_recomputeData.m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InExtremity( _edgeList, _edgeRunList ) )
            return;

        setEdgeRunCoeffUv( config, _edgeList, _edgeRunList[0] );
        
        buildVB_InExtremity( _edgeList, _edgeRunList, _vertexTempList_Static, _indexTempList_Static, _vertexTempList_Anim, _indexTempList_Anim, extremityRatio );
    }

    bbool Frise::buildEdgeRunList_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
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

    void Frise::buildVB_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static, MeshAnim_VertexList& _vertexTempList_Anim, MeshAnim_IndexList* _indexTempList_Anim, f32 _extremityRatio ) const
    {        
        FriseConfig* config = getConfig();
        if( ! config )
            return;
        
        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;

        const edgeRun& edgeRunCur = _edgeRunList[0];

        //  colors vertexs 
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( config->m_textureConfigs[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity );

        // uv
        f32 uvYup = 0.f;
        f32 uvYdown = 0.5f;

        f32 uvYupExt = 0.5f;
        f32 uvYdownExt = 1.f;

        if ( m_recomputeData.m_isUvFlipY )
        {
            uvYup = 0.5f;
            uvYdown = 0.f;

            uvYupExt = 1.f;
            uvYdownExt = 0.5f;
        }

        Vec2d uvDown( getUvX_Start(), uvYdown);
        Vec2d uvUp( uvDown.m_x, uvYup);
        
        // Extremity
        f32 extremityLong = getExtremityLong( _extremityRatio );
        f32 uvXOffsetExtremity = _extremityRatio * m_recomputeData.m_uvXsign;
        Vec2d posExtremity[4];
        Vec2d uvExtremity[4];
        u32 colorExtremity[4];

        // scale extremity
        f32 heightMidExtremity = _edgeList[0].m_heightStart;
        bbool snapExtremity = bfalse;
        if ( m_recomputeData.m_edgeListCount == 1 && _edgeList[0].m_snap )
        {
            snapExtremity = btrue;
            heightMidExtremity = ( _edgeList[0].m_heightStart + _edgeList[0].m_heightStop) *0.5f;
        }

        // build start extremity   
        f32 extremityLongStart = extremityLong *_edgeList[0].m_heightStart * m_recomputeData.m_heightScaleInv;
        setQuadPos_StartExtremity( _edgeList[0], &posExtremity[0], config->m_visualOffset, heightMidExtremity, extremityLongStart/*longExtremity*/, snapExtremity );
        setQuadColor_StartExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );

        uvExtremity[0] = Vec2d( uvDown.m_x -uvXOffsetExtremity, uvYdownExt );
        uvExtremity[1] = Vec2d( uvExtremity[0].m_x, uvYupExt );
        uvExtremity[2] = Vec2d( uvDown.m_x, uvYdownExt );
        uvExtremity[3] = Vec2d( uvUp.m_x, uvYupExt );

        const MeshElementRouter* elemRouter;
        if ( m_recomputeData.m_anim )
        {
            elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagAnimated, edgeRunCur.m_idTex);
            ITF_ASSERT(elemRouter);
            ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Dynamic);

            buildVB_Anim_Quad( config->m_vtxAnim, _vertexTempList_Anim, _indexTempList_Anim[elemRouter->m_indexInTable], &posExtremity[0], &uvExtremity[0], &colorExtremity[0], btrue, btrue, snapExtremity );    
            buildVB_Anim_EdgeRun( config, _edgeList, edgeRunCur, _vertexTempList_Anim, _indexTempList_Anim[elemRouter->m_indexInTable], vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, m_recomputeData.m_uvXsign );   
        }
        else
        {
            elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, edgeRunCur.m_idTex);
            ITF_ASSERT(elemRouter);
            ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Static);

            buildVB_Static_Quad( _vertexTempList_Static, _indexTempList_Static[elemRouter->m_indexInTable], &posExtremity[0], &uvExtremity[0], &colorExtremity[0], btrue );
            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, _vertexTempList_Static, _indexTempList_Static[elemRouter->m_indexInTable], vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, m_recomputeData.m_uvXsign );   
        }

        // build stop extremity
        u32 idLastEdge = m_recomputeData.m_edgeListCount -1;

        if ( !snapExtremity )
            heightMidExtremity = _edgeList[idLastEdge].m_heightStop;

        f32 extremityLongStop = extremityLong *_edgeList[idLastEdge].m_heightStop * m_recomputeData.m_heightScaleInv;
        setQuadPos_StopExtremity( _edgeList[idLastEdge], &posExtremity[0], config->m_visualOffset, heightMidExtremity, extremityLongStop/*longExtremity*/, snapExtremity );
        setQuadColor_StopExtremity( &colorExtremity[0], vtxColorDown, vtxColorUp, vtxColorDownExtremity, vtxColorUpExtremity );

        uvExtremity[0].m_x = uvDown.m_x;
        uvExtremity[1].m_x = uvExtremity[0].m_x;
        uvExtremity[2].m_x = uvExtremity[0].m_x +uvXOffsetExtremity;
        uvExtremity[3].m_x = uvExtremity[2].m_x;

        if ( m_recomputeData.m_anim )
        {
            buildVB_Anim_Quad( config->m_vtxAnim, _vertexTempList_Anim, _indexTempList_Anim[elemRouter->m_indexInTable], &posExtremity[0], &uvExtremity[0], &colorExtremity[0], bfalse, bfalse, btrue );

            // start extremity
            if ( !snapExtremity)
            {
                copyVtxAnim( _vertexTempList_Anim[3], _vertexTempList_Anim[5] );
            }

            // stop extremity
            u32 vertexCount = _vertexTempList_Anim.size();
            copyVtxAnim( _vertexTempList_Anim[ vertexCount -3], _vertexTempList_Anim[ vertexCount -5] );
        }
        else
        {
            buildVB_Static_Quad( _vertexTempList_Static, _indexTempList_Static[elemRouter->m_indexInTable], &posExtremity[0], &uvExtremity[0], &colorExtremity[0], bfalse );
        }        
    }

    void Frise::buildCollision_InExtremity( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {       
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

} // namespace ITF
