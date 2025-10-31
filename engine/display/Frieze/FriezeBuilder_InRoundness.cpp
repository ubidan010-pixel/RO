#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{
    f32 step_correspondanceTexture  = MTH_PIBY8;
    f32 angleBreak = MTH_PIBY6;

    static f32 angleRoundness[] = 
    {   
        3.f * MTH_PIBY4,
        3.f * MTH_PIBY4,
        MTH_PIBY2,
        MTH_PIBY4,        
        0.f,
        MTH_PIBY4, 
        MTH_PIBY2,
        3.f * MTH_PIBY4,
        3.f * MTH_PIBY4,
    };

    static const f32 uvOffSet = cosf(MTH_PIBY4) * 0.25f;

    struct UvsByAngle_InRoundness
    {
        UvsByAngle_InRoundness(){}

        UvsByAngle_InRoundness(const Vec2d& _center, const Vec2d& _start, const Vec2d& _stop)
        {
            m_center      = _center;
            m_start       = _start;
            m_stop        = _stop;
        }

        Vec2d   m_center;
        Vec2d   m_start;
        Vec2d   m_stop;
    };

    static UvsByAngle_InRoundness uvLeft[]=
    {
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f -uvOffSet,0.75f +uvOffSet), Vec2d(0.75f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f -uvOffSet,0.75f +uvOffSet), Vec2d(0.75f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.50f),  Vec2d(0.00f,0.50f),                     Vec2d(0.25f,0.25f) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f -uvOffSet,0.75f -uvOffSet), Vec2d(0.25f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d::Zero,         Vec2d::Zero,                            Vec2d::Zero        ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f,1.00f), Vec2d(0.75f -uvOffSet,0.75f +uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.50f,0.25f),  Vec2d(0.50f,0.50f), Vec2d(0.25f,0.25f) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,1.00f), Vec2d(0.25f -uvOffSet,0.75f -uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,1.00f), Vec2d(0.25f -uvOffSet,0.75f -uvOffSet) ),
    };

    static UvsByAngle_InRoundness uvRight[]=
    {        
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,0.50f), Vec2d(0.25f +uvOffSet,0.75f +uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f,0.50f), Vec2d(0.25f +uvOffSet,0.75f +uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.50f),  Vec2d(0.75f,0.25f), Vec2d(1.00f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f,0.50f), Vec2d(0.75f +uvOffSet,0.75f -uvOffSet) ),
        UvsByAngle_InRoundness( Vec2d::Zero,         Vec2d::Zero,        Vec2d::Zero        ),
        UvsByAngle_InRoundness( Vec2d(0.25f,0.75f),  Vec2d(0.25f +uvOffSet,0.75f +uvOffSet), Vec2d(0.25f,1.00f) ),
        UvsByAngle_InRoundness( Vec2d(0.50f,0.25f),  Vec2d(0.75f,0.25f),                     Vec2d(0.50f,0.50f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f +uvOffSet,0.75f -uvOffSet), Vec2d(0.75f,1.00f) ),
        UvsByAngle_InRoundness( Vec2d(0.75f,0.75f),  Vec2d(0.75f +uvOffSet,0.75f -uvOffSet), Vec2d(0.75f,1.00f) ),
    };

    u32 Frise::buildRoundness_getIndexUv(f32 _angle) const
    {
        if ( _angle >= 0 )
            _angle =  Max( _angle, MTH_PIBY6 );
        else
            _angle =  Min( _angle, -MTH_PIBY6 );

        u32 indexRoundness = u32( 8.f -floor( 0.5f + 8.f *( MTH_PI +_angle ) /MTH_2PI ) );
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT(indexRoundness<9 && indexRoundness!=4);
#endif

        return indexRoundness;
    }

    i32 Frise::getTexIdBySlope_InRoundness( Vec2d _sight, FriseConfig* _pConfig ) const
    {
        f32 angle = atan2f( _sight.m_y, _sight.m_x) +MTH_PI;

        u32 indexTexture = u32(floor( 0.5f +angle /step_correspondanceTexture) );

        return _pConfig->m_FriseTextureConfigIndexBySlope[indexTexture];
    }

    u32 Frise::getIdStartEdge_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* _pConfig ) const
    {
        if ( !m_pointsList.isLooping() )
            return 0;

        if ( _edgeList[0].m_cornerAngle >= angleBreak || _edgeList[0].m_cornerAngle <= -angleBreak )
            return 0;

        i32 idTexEdgeStart = getTexIdBySlope_InRoundness( _edgeList[0].m_sightNormalized, _pConfig );

        for (u32 i=1; i<m_recomputeData.m_edgeListCount; i++)
        {
            if ( _edgeList[i].m_cornerAngle >= angleBreak || _edgeList[i].m_cornerAngle <= -angleBreak )
                return i;

            if ( idTexEdgeStart != getTexIdBySlope_InRoundness( _edgeList[i].m_sightNormalized, _pConfig ) )
                return i;
        }

        return 0;
    } 

    void Frise::buildFrieze_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static ) const
    {
        copyEdgeFromPolyline(_edgeList);

        m_recomputeData.m_texRatio = 0.25f;
        m_recomputeData.m_flexibility = 0.f;

        buildEdgeList( _edgeList);

        if ( m_recomputeData.m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InRoundness( _edgeList, _edgeRunList ) )
            return;

        if ( !setEdgeRunListCoeffUv( _edgeList, _edgeRunList ) )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeRun( _edgeRunList );             
#endif

        buildVB_InRoundness_Static(_edgeList, _edgeRunList, _vertexTempList_Static, _indexTempList_Static );      
    }

    bbool Frise::isAngleBreak_InRoundness( edgeFrieze& _edge ) const
    {
        return _edge.m_cornerAngle <= -angleBreak || _edge.m_cornerAngle >= angleBreak;
    }

    bbool Frise::buildEdgeRunList_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(m_recomputeData.m_edgeListCount);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_recomputeData.m_edgeListCount;
       
        if ( getFirstNoFillTextureIndex( config ) == -1 )
        {
            _edgeRunList.push_back(edgeRunCur);
            return bfalse;
        }

        u32 idEdgeStart = getIdStartEdge_InRoundness(_edgeList, config );
        i32 idTex = getTexIdBySlope_InRoundness( _edgeList[idEdgeStart].m_sightNormalized, config );
        
        for ( u32 countEdge=0, idCurEdge=idEdgeStart; countEdge<m_recomputeData.m_edgeListCount; )
        {
            edgeRunCur.m_idEdgeStart = idCurEdge;            
            edgeRunCur.m_idTex = idTex;

            ++countEdge;
            ++idCurEdge;
            edgeRunCur.m_edgeCount = 1;

            for( ; countEdge<m_recomputeData.m_edgeListCount; countEdge++, idCurEdge++ )
            {
                idCurEdge = idCurEdge % m_recomputeData.m_edgeListCount;

                idTex = getTexIdBySlope_InRoundness( _edgeList[idCurEdge].m_sightNormalized, config );

                if ( edgeRunCur.m_idTex != idTex || isAngleBreak_InRoundness( _edgeList[idCurEdge] ) )
                    break;

                edgeRunCur.m_edgeCount++;
            }

            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    void Frise::buildVB_InRoundness_Static( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;

        const u32 edgeRunListCount = _edgeRunList.size();
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            const edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];

            if ( edgeRunCur.m_idTex == -1 )
                continue;

            const MeshElementRouter* elemRouterCur = getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, edgeRunCur.m_idTex);
            ITF_ASSERT(elemRouterCur);
            ITF_ASSERT(elemRouterCur->m_tableID == MeshElementRouter::MeshElementTable_Static);

            //  colors vertexs 
            u32 vtxColorDown, vtxColorUp;
            setVtxColor( config->m_textureConfigs[edgeRunCur.m_idTex], vtxColorUp, vtxColorDown ); 

            // start edge
            edgeFrieze& edge = _edgeList[edgeRunCur.m_idEdgeStart];

            // build roundness on start edge
            if ( edge.m_cornerAngle != 0.f )
            {
                // edgeRun Last
                u32 idEdgeRunLast = ( idEdgeRun +edgeRunListCount -1 ) %edgeRunListCount;
                const edgeRun& edgeRunLast = _edgeRunList[idEdgeRunLast];

                const MeshElementRouter* elemRouterLast = NULL;

                // edge Last
                u32 idEdgeLast = ( edgeRunLast.m_idEdgeStart +edgeRunLast.m_edgeCount +m_recomputeData.m_edgeListCount -1 ) %m_recomputeData.m_edgeListCount;
                const edgeFrieze& edgeLast = _edgeList[idEdgeLast];

                //  colors vertexs edgeRun Last
                u32 vtxColorUpLast = 0;
                u32 vtxColorDownLast = 0;

                if ( edgeRunLast.m_idTex != -1 )
                {
                   elemRouterLast  = getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, edgeRunLast.m_idTex);
                   ITF_ASSERT(elemRouterLast);
                   ITF_ASSERT(elemRouterLast->m_tableID == MeshElementRouter::MeshElementTable_Static);

                    // down
                    vtxColorDownLast = config->m_textureConfigs[edgeRunLast.m_idTex].m_color.getAsU32();

                    // up
                    vtxColorUpLast = ITFCOLOR_SET_A(config->m_textureConfigs[edgeRunLast.m_idTex].m_color.getAsU32(), config->m_textureConfigs[edgeRunLast.m_idTex].m_alphaUp);
                }

                if ( edge.m_cornerAngle >0.f )
                {    
                    Vec2d  posStart = edgeLast.m_points[3];
                    Vec2d  posStop = edge.m_points[1];;

                    // roundness end edge last
                    if ( edgeRunLast.m_idTex != -1 )
                        buildRoundness_Static( config, _vertexTempList, _indexTempList[elemRouterLast->m_indexInTable], uvRight, edge.m_interDown, posStart, posStop, edge.m_cornerAngle, vtxColorDownLast, vtxColorUpLast );                   

                    // roundness start edge cur
                    buildRoundness_Static( config, _vertexTempList, _indexTempList[elemRouterCur->m_indexInTable], uvLeft, edge.m_interDown, posStart, posStop, edge.m_cornerAngle, vtxColorDown, vtxColorUp );
                }
                else
                {    
                    Vec2d posStart = edge.m_points[0];
                    Vec2d posStop = edgeLast.m_points[2];

                    // roundness end edge last
                    if ( edgeRunLast.m_idTex != -1 )
                        buildRoundness_Static( config, _vertexTempList, _indexTempList[elemRouterLast->m_indexInTable], uvRight, edge.m_interUp, posStart, posStop, edge.m_cornerAngle, vtxColorUpLast, vtxColorDownLast );

                    // roundness start edge cur
                    buildRoundness_Static( config, _vertexTempList, _indexTempList[elemRouterCur->m_indexInTable], uvLeft, edge.m_interUp, posStart, posStop, edge.m_cornerAngle, vtxColorUp, vtxColorDown );
                }
            }

            // build VB edgeRun
            Vec2d uvUp( getUvX_Start(), 0.f );
            Vec2d uvDown( uvUp.m_x, 0.25f );
            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, _vertexTempList, _indexTempList[elemRouterCur->m_indexInTable], vtxColorUp, vtxColorDown, vtxColorUp, vtxColorDown, indexVtxUp, indexVtxDown, uvUp, uvDown, m_recomputeData.m_uvXsign );             
        }
    }

    void Frise::buildRoundness_Static( FriseConfig* config, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, UvsByAngle_InRoundness* _uvTab, Vec2d& _vCenter, Vec2d& _vStart, Vec2d& _vStop, f32 _angle, u32 _vtxColorCenter, u32 _vtxColorCorner ) const
    {       
        //  uvs
        u32 indexRoundness = buildRoundness_getIndexUv(_angle);

        u16 indexVtxCenter = (u16)_vertexTempList.size();
        u16 indexVtxCorner = indexVtxCenter +1;

        f32 zVtxCenter;
        f32 zVtxCorner;

        if ( _angle > 0.f )
        {
            zVtxCenter = m_recomputeData.m_zVtxDown;
            zVtxCorner = m_recomputeData.m_zVtxUp;
        }
        else
        {
            zVtxCenter = m_recomputeData.m_zVtxUp;
            zVtxCorner = m_recomputeData.m_zVtxDown;
        }

        //  step count
        _angle = f32_Abs(_angle);
        u32 stepCount = getCornerStepNb( _angle, config->m_smoothFactorVisual, 1.f);

        //  vertex center 
        VertexPCT& vtxCenter = _vertexTempList.incrSize( stepCount +2);
        vtxCenter.m_pos.m_x = _vCenter.m_x;
        vtxCenter.m_pos.m_y = _vCenter.m_y;
        vtxCenter.m_pos.m_z = zVtxCenter;
        vtxCenter.m_uv = _uvTab[indexRoundness].m_center;
        vtxCenter.setColor(_vtxColorCenter);

        //  vertex start 
        VertexPCT& vtxStart = _vertexTempList[indexVtxCorner];
        vtxStart.m_pos.m_x = _vStart.m_x;
        vtxStart.m_pos.m_y = _vStart.m_y;
        vtxStart.m_pos.m_z = zVtxCorner;
        vtxStart.m_uv = _uvTab[indexRoundness].m_start;
        vtxStart.setColor(_vtxColorCorner);

        _indexTempList.push_back(indexVtxCenter);
        _indexTempList.push_back(indexVtxCorner);
        indexVtxCorner++;

        if ( stepCount > 1 )
        {
            Vec2d uvRot  = _uvTab[indexRoundness].m_start;

            f32 stepFan = -_angle /(f32)stepCount;               
            f32 stepUv = angleRoundness[indexRoundness] / (f32)stepCount;

            f32 fanRotCos, fanRotSin;
            f32_CosSin(stepFan, &fanRotCos, &fanRotSin);

            f32 uvRotCos, uvRotSin;
            f32_CosSin(stepUv, &uvRotCos, &uvRotSin);

            Vec2d vCorner = _vStart;
            for ( u32 i = 1; i < stepCount; i++ )
            {
                // rotation
                vCorner = vCorner.RotateAroundCS(_vCenter, fanRotCos, fanRotSin);
                uvRot = uvRot.RotateAroundCS( _uvTab[indexRoundness].m_center, uvRotCos, uvRotSin );

                VertexPCT& vtxCorner = _vertexTempList[indexVtxCorner];                
                vtxCorner.m_pos.m_x = vCorner.m_x;
                vtxCorner.m_pos.m_y = vCorner.m_y;
                vtxCorner.m_pos.m_z = zVtxCorner;
                vtxCorner.m_uv = uvRot;
                vtxCorner.setColor( _vtxColorCorner);

                //  add vertex to create a triangle
                _indexTempList.push_back(indexVtxCorner);

                // add two index for next triangle
                _indexTempList.push_back(indexVtxCenter);
                _indexTempList.push_back(indexVtxCorner);

                indexVtxCorner++;
            }
        }

        //  vertex stop
        VertexPCT& vtxStop = _vertexTempList[indexVtxCorner];
        vtxStop.m_pos.m_x = _vStop.m_x;
        vtxStop.m_pos.m_y = _vStop.m_y;
        vtxStop.m_pos.m_z = zVtxCorner;
        vtxStop.m_uv = _uvTab[indexRoundness].m_stop;
        vtxStop.setColor( _vtxColorCorner);

        //  add vertex to create last triangle
        _indexTempList.push_back(indexVtxCorner);
    }

    void Frise::buildCollision_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {        
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;
        
        buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

    bbool Frise::isEdgeRun_InRoundness( ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* _config, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const
    {      
        i32 texIdLastEdge = getTexIdBySlope_InRoundness( _edgeList[_idLastEdge].m_sightNormalized, _config );        

        if ( getTexIdBySlope_InRoundness( _edgeList[_idLastLastEdge].m_sightNormalized, _config ) == texIdLastEdge && !isAngleBreak_InRoundness( _edgeList[_idLastEdge]) )
            return bfalse;

        if ( getTexIdBySlope_InRoundness( _edgeList[_idCurEdge].m_sightNormalized, _config ) == texIdLastEdge && !isAngleBreak_InRoundness( _edgeList[_idCurEdge]) )
            return bfalse;

        return btrue;
    }

} // namespace ITF
