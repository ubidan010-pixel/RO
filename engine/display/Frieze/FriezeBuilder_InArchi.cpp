
#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

namespace ITF
{   
    static i32 idTexConfig_InArchi;

    static const u32 idTopZoneUv       = 0;
    static const u32 idRightZoneUv     = 1;
    static const u32 idLeftZoneUv      = 2;
    static const u32 idBottomZoneUv    = 3;

    static u32 ZoneContinuityBefore_Out[]=
    {
        idLeftZoneUv,      // TOP
        idTopZoneUv,       // RIGHT
        idBottomZoneUv,    // LEFT
        idRightZoneUv      // BOTTOM
    };

    static u32 ZoneContinuityBefore_In[]=
    {
        idRightZoneUv,     // TOP
        idBottomZoneUv,    // RIGHT
        idTopZoneUv,       // LEFT
        idLeftZoneUv,      // BOTTOM
    };

    static u32 ZoneContinuityNext_Out[]=
    {
        idRightZoneUv,      // TOP
        idBottomZoneUv,     // RIGHT
        idTopZoneUv,        // LEFT
        idLeftZoneUv        // BOTTOM
    };

    static u32 ZoneContinuityNext_In[]=
    {
        idLeftZoneUv,       // TOP
        idTopZoneUv,        // RIGHT
        idBottomZoneUv,     // LEFT
        idRightZoneUv,      // BOTTOM
    };

    struct VarianceCornerOut_TexSwitch
    {
        VarianceCornerOut_TexSwitch( f32 _inStart, f32 _inStop )
        {
            m_inStart = _inStart;
            m_inStop = _inStop;
        }

        f32 m_inStart;
        f32 m_inStop;
    };

    static VarianceCornerOut_TexSwitch varianceZone[]=
    {
        VarianceCornerOut_TexSwitch( 0.f, 1.f ),       // TOP
        VarianceCornerOut_TexSwitch( 1.f, 0.f ),     // RIGHT
        VarianceCornerOut_TexSwitch( 1.f, 0.f ),      // LEFT
        VarianceCornerOut_TexSwitch( 0.f, 1.f ),    // BOTTOM
    };

    // choix de la bande d'uv sur un edge
    struct UvsByAngle_InArchi
    {
        UvsByAngle_InArchi( f32 _start, f32 _stop, u32 _index )
        {
            m_start = _start;
            m_stop  = _stop;
            m_index = _index;
        }

        f32 m_start;
        f32 m_stop;
        u32 m_index;
    };

    static UvsByAngle_InArchi indexZoneByAngle[]=
    {
        UvsByAngle_InArchi( -MTH_PIBY4,         MTH_PIBY4       , idTopZoneUv ),       // TOP
        UvsByAngle_InArchi( -3.f *MTH_PIBY4,    -MTH_PIBY4      , idRightZoneUv ),     // RIGHT
        UvsByAngle_InArchi( MTH_PIBY4,          3.f *MTH_PIBY4  , idLeftZoneUv ),      // LEFT
        UvsByAngle_InArchi( 3.f *MTH_PIBY4,     MTH_PI          , idBottomZoneUv ),    // BOTTOM
        UvsByAngle_InArchi( -MTH_PI,            -3.f *MTH_PIBY4 , idBottomZoneUv ),    // BOTTOM        
    };


    void setUvByAngleInArchi( FriseConfig* _config)
    {
        if ( !_config )
            return;

        f32 angle = _config->m_wallAngle;

        if ( angle == indexZoneByAngle[0].m_stop )
            return;

        // clamp angle
        angle = Clamp( angle, 0.f, MTH_PIBY2);

        f32 angleStep = MTH_PIBY2 -angle;

        // top
        indexZoneByAngle[0].m_start = -angle;
        indexZoneByAngle[0].m_stop = angle;

        // right
        indexZoneByAngle[1].m_start = -MTH_PIBY2 -angleStep;
        indexZoneByAngle[1].m_stop = -angle;

        // left
        indexZoneByAngle[2].m_start = angle;
        indexZoneByAngle[2].m_stop = MTH_PIBY2 +angleStep;

        // bottom
        indexZoneByAngle[3].m_start = indexZoneByAngle[2].m_stop;
        indexZoneByAngle[4].m_stop = indexZoneByAngle[1].m_start;
    }

    //  settings de chaque zone
    struct UvLinear
    {
         UvLinear( 
             f32 _uvYdown, 
             f32 _uvYup, 
             f32 _uvXsign, 
             Vec2d _uvSquareOut_0,
             Vec2d _uvSquareOut_1,
             Vec2d _uvSquareOut_2,
             Vec2d _uvSquareOut_3,
             Vec2d _uvVarianceOut,
             Vec2d _uvSquareIn_0,
             Vec2d _uvSquareIn_1,
             Vec2d _uvSquareIn_2,
             Vec2d _uvSquareIn_3,
             Vec2d _uvVarianceIn
             )
             : m_uvYdown(_uvYdown)
             , m_uvYup(_uvYup)
             , m_uvXsign(_uvXsign)

         {
             m_uvSquareOut[0] = _uvSquareOut_0;
             m_uvSquareOut[1] = _uvSquareOut_1;
             m_uvSquareOut[2] = _uvSquareOut_2;
             m_uvSquareOut[3] = _uvSquareOut_3;
             m_uvSquareOut[4] = _uvVarianceOut;

             m_uvSquareIn[0] = _uvSquareIn_0;
             m_uvSquareIn[1] = _uvSquareIn_1;
             m_uvSquareIn[2] = _uvSquareIn_2;
             m_uvSquareIn[3] = _uvSquareIn_3;
             m_uvSquareIn[4] = _uvVarianceIn;
         }

        f32 m_uvYdown;
        f32 m_uvYup;
        f32 m_uvXsign;
        Vec2d m_uvSquareOut[5];
        Vec2d m_uvSquareIn[5];        
    };

    static UvLinear uv[]=
    {
        // TOP
        UvLinear( 0.125f,   0.f,    1.f,
        Vec2d( 0.f, 0.625f ),   Vec2d( 0.f, 0.5f ),     Vec2d( 0.25f, 0.625f ), Vec2d( 0.25f, 0.5f ), Vec2d( 1.f, 0.f),      // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.25f, 0.625f ), Vec2d( 0.5f, 0.75f ),   Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.f) ), // IN 

        // RIGHT
        UvLinear( 0.25f,    0.125f, 1.f,
        Vec2d( 0.25f, 0.5f ),   Vec2d( 0.5f, 0.5f ), Vec2d( 0.25f, 0.625f ), Vec2d( 0.5f, 0.625f ), Vec2d( 1.f, 0.f),   // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.5, 0.75f ), Vec2d( 0.25f, 0.875f ), Vec2d( 0.5f, 0.875f ), Vec2d( 0.f, 0.f) ),   // IN  

        // LEFT
        UvLinear( 0.375f,   0.25f,  1.f,
        Vec2d( 0.25f, 1.f ),    Vec2d( 0.f, 1.f ),   Vec2d( 0.25f, 0.875f ), Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f), // OUT
        Vec2d( 0.25f, 0.75f ),  Vec2d( 0.f, 0.75f ), Vec2d( 0.25f, 0.625f ), Vec2d( 0.f, 0.625f ), Vec2d( 1.f, 0.f) ), // IN 

        // BOTTOM
        UvLinear( 0.375f,   0.5f,   -1.f,
        Vec2d( 0.5f, 0.875f ), Vec2d( 0.5f, 1.f ),      Vec2d( 0.25f, 0.875f ), Vec2d( 0.25f, 1.f ), Vec2d( 0.f, 0.f),   // OUT
        Vec2d( 0.25f, 0.75f ), Vec2d( 0.25f, 0.875f ),  Vec2d( 0.f, 0.75f ),    Vec2d( 0.f, 0.875f ), Vec2d( 0.f, 0.f) ), // IN 
    };

    void Frise::buildFrieze_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList_Static, MeshStatic_IndexList* _indexTempList_Static ) const
    {
        copyEdgeFromPolyline(_edgeList);        

        m_recomputeData.m_texRatio = 0.25f;    

        buildEdgeList( _edgeList);

        if ( m_recomputeData.m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        setUvByAngleInArchi( getConfig());

        if ( !buildEdgeRunList_InArchi( _edgeList, _edgeRunList ) )
            return;

        setEdgeRunListCoeffUv( _edgeList, _edgeRunList );

        buildVB_InArchi_Static( _edgeList, _edgeRunList, _vertexTempList_Static, _indexTempList_Static );        
    }

    Frise::edgeFrieze Frise::buildNewEdge_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* config, u32 idCurEdge, u32 idLastEdge) const
    {
        edgeFrieze& edgeCur = _edgeList[idCurEdge];
        edgeFrieze& edgeLast = _edgeList[idLastEdge];
        f32 coeffOffset = edgeCur.m_heightStart *abs(edgeCur.m_cornerAngle) /MTH_PI;
        
        if ( edgeCur.m_cornerAngle <0.f)
            coeffOffset *= Max(0.001f, 1.f-config->m_visualOffset);
        else
            coeffOffset *= Max(0.001f, config->m_visualOffset);

        Vec2d offset = edgeCur.m_cornerNormal.getPerpendicular() *coeffOffset;
        Vec2d posEdgeCur = edgeCur.m_pos;
        Vec2d posEdgeNew = edgeCur.m_pos;

        if ( idCurEdge == 0)
        {
            posEdgeNew += offset *2.f;
        }
        else
        {
            posEdgeCur -= offset;
            posEdgeNew += offset;
        }

#ifdef DEVELOPER_JAY_FRIEZE
        GFX_ADAPTER->drawDBGLine( transformPos(edgeCur.m_pos), transformPos(edgeCur.m_pos +edgeCur.m_cornerNormal), Color::yellow());
        GFX_ADAPTER->drawDBGLine( transformPos(edgeCur.m_pos), transformPos(edgeCur.m_pos +offset), Color::red());
#endif

        // create new edge
        edgeFrieze edgeNew;
        edgeNew.m_pos = posEdgeNew;
        edgeNew.m_holeMode = edgeCur.m_holeMode;
        edgeNew.m_idPoint = edgeCur.m_idPoint;
        edgeNew.m_snap = btrue;
        setScaleNewEdge( edgeNew, edgeLast, edgeCur );
        updateEdgeFriezeVector( edgeNew, posEdgeCur -posEdgeNew);

        // update cur edge
        Vec2d sightDest = edgeCur.m_pos + edgeCur.m_sight;
        edgeCur.m_pos = posEdgeCur;
        updateEdgeFriezeVector( edgeCur, sightDest -posEdgeCur );

        // update last edge
        updateEdgeFriezeVector( edgeLast, posEdgeNew -edgeLast.m_pos);

        return edgeNew;
    }

    void Frise::insertNewEdgeInEdgeList( ITF_VECTOR<edgeFrieze>& _edgeList, edgeFrieze& _newEdge, u32 _index ) const
    {
        if ( _index == 0 )
        {
            _edgeList.push_back( _newEdge );
        }
        else
        {
            ITF_VECTOR<edgeFrieze>::iterator it;
            it = _edgeList.begin() +_index;
            _edgeList.insert( it, _newEdge );
        }

        m_recomputeData.m_edgeListCount++;
    }
       
    bbool Frise::insertNewEdge_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, FriseConfig* config, u32 idCurEdge, u32 idLastEdge ) const
    {
        if ( m_lockTexture != LockTexture_None )
            return bfalse;

        edgeFrieze& edgeCur = _edgeList[idCurEdge];
        edgeFrieze& edgeLast = _edgeList[idLastEdge];

        u32 idZoneCurEdge = getZoneId_InArchi( edgeCur.m_sightNormalized );
        u32 idZoneLastEdge = getZoneId_InArchi( edgeLast.m_sightNormalized );
        u32 idZoneRequiered = 0;

        // same uv id
        if ( idZoneLastEdge == idZoneCurEdge )
            return bfalse;

        // check continuity
        if ( edgeCur.m_cornerAngle > 0.f )
        {
            idZoneRequiered = ZoneContinuityBefore_Out[idZoneCurEdge];

            if ( idZoneLastEdge == idZoneRequiered )
                return bfalse;
        }
        else 
        {
            idZoneRequiered = ZoneContinuityBefore_In[idZoneCurEdge];

            if( idZoneLastEdge == idZoneRequiered )
                return bfalse;
        }

        // back up edge cur and last
        edgeFrieze edgeCurBackUp = edgeCur;
        edgeFrieze edgeLastBackUp = edgeLast;
               
        // build edge new
        edgeFrieze edgeNew = buildNewEdge_InArchi( _edgeList, config, idCurEdge, idLastEdge);

        // check result
        if ( edgeNew.m_sightNormalized == Vec2d::Zero || edgeCur.m_sightNormalized == Vec2d::Zero || edgeLast.m_sightNormalized == Vec2d::Zero 
            || idZoneRequiered != getZoneId_InArchi( edgeNew.m_sightNormalized )
            || idZoneCurEdge != getZoneId_InArchi( edgeCur.m_sightNormalized ) 
            || idZoneLastEdge != getZoneId_InArchi( edgeLast.m_sightNormalized) )
        {
            edgeCur = edgeCurBackUp;
            edgeLast = edgeLastBackUp;
            return bfalse;
        }
        else
        {
            insertNewEdgeInEdgeList( _edgeList, edgeNew, idCurEdge );
            return btrue;
        }
    }

    u32 Frise::getZoneId_InArchi( const Vec2d& _sight ) const
    {
        switch ( m_lockTexture )
        {
            case LockTexture_Top :
            case LockTexture_Right :
            case LockTexture_Left :
            case LockTexture_Bottom :
                return m_lockTexture;
                break;

            default:
                break;
        }

        f32 angle = atan2f( _sight.m_y, _sight.m_x );

        for ( u32 i = 0; i < 5; i++ )
        {
            if ( angle >= indexZoneByAngle[i].m_start && angle <= indexZoneByAngle[i].m_stop )
                return indexZoneByAngle[i].m_index;
        }

        return 0;
    }

    u32 Frise::getIdStartEdge_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, f32 _nornMin ) const
    {
        if ( !m_pointsList.isLooping() || m_lockTexture == LockTexture_Switch )
            return 0;

        u32 idZoneEdgeStart = getZoneId_InArchi( _edgeList[0].m_sightNormalized );

        for (u32 i=1; i<m_recomputeData.m_edgeListCount; i++)
        {
            if ( idZoneEdgeStart != getZoneId_InArchi( _edgeList[i].m_sightNormalized ) )
                return i;
        }

        return 0;
    } 

    bbool Frise::buildEdgeRunList_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(m_recomputeData.m_edgeListCount);

        edgeRun edgeRunCur;
        edgeRunCur.m_edgeCount = m_recomputeData.m_edgeListCount; 
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        // frieze texture
        idTexConfig_InArchi = edgeRunCur.m_idTex;

        if ( idTexConfig_InArchi == -1 )
        {
            _edgeRunList.push_back(edgeRunCur);
            return bfalse;
        }

        f32 edgeRunNornMin = getEdgeRunNormMin( config );
        u32 idEdgeStart = getIdStartEdge_InArchi( _edgeList, edgeRunNornMin );
        u32 idZone = getZoneId_InArchi( _edgeList[idEdgeStart].m_sightNormalized );  
        
#ifdef DEVELOPER_JAY_FRIEZE
        ITF_ASSERT(edgeRunNornMin >= 0.f);
#endif

        for ( u32 countEdge=0, idCurEdge=idEdgeStart; countEdge<m_recomputeData.m_edgeListCount; )
        {
            edgeRunCur.m_idEdgeStart = idCurEdge;            
            edgeRunCur.m_idZone = idZone;
#ifdef DEVELOPER_JAY_FRIEZE
            ITF_ASSERT(edgeRunCur.m_idZone <4);
#endif

            if ( config->m_FriseTextureConfigIndexByZone[edgeRunCur.m_idZone] != -1 )
                edgeRunCur.m_idTex = config->m_FriseTextureConfigIndexByZone[edgeRunCur.m_idZone];
            else
                edgeRunCur.m_idTex = idTexConfig_InArchi;

            ++countEdge;
            ++idCurEdge;
            edgeRunCur.m_edgeCount = 1;

            if ( m_lockTexture == LockTexture_Switch )
            {
                idCurEdge = idCurEdge % m_recomputeData.m_edgeListCount;

                if ( _edgeList[idCurEdge].m_cornerAngle >0.f )
                    idZone = ZoneContinuityNext_Out[idZone];
                else
                    idZone = ZoneContinuityNext_In[idZone];                
            }
            else if ( !isEdgeWithHoleVisual(_edgeList[edgeRunCur.m_idEdgeStart]))
            {
                for( ; countEdge<m_recomputeData.m_edgeListCount; countEdge++, idCurEdge++ )
                {
                    idCurEdge = idCurEdge % m_recomputeData.m_edgeListCount;
                    const edgeFrieze& edgeCur = _edgeList[idCurEdge];

                    if ( isEdgeWithHoleVisual( edgeCur))
                        break;

                    idZone = getZoneId_InArchi( edgeCur.m_sightNormalized );

                    if ( edgeRunCur.m_idZone != idZone )    
                        break;

                    edgeRunCur.m_edgeCount++;
                }
            }
            else
            {
                idCurEdge = idCurEdge % m_recomputeData.m_edgeListCount;
                idZone = getZoneId_InArchi( _edgeList[idCurEdge].m_sightNormalized );
            }

            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    void Frise::buildVB_InArchi_Static( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;

        const MeshElementRouter* elemRouter = getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, idTexConfig_InArchi);
        ITF_ASSERT(elemRouter);
        ITF_ASSERT(elemRouter->m_tableID == MeshElementRouter::MeshElementTable_Static);

        const MeshElementRouter* elemRouterSwitch = NULL;

        u16 indexVtxUp = 0;
        u16 indexVtxDown = 0;
        const u32 edgeRunListCount = _edgeRunList.size();

        // init uv corner variance
        uv[idTopZoneUv].m_uvSquareIn[4].m_y = 0.f;
        uv[idTopZoneUv].m_uvSquareOut[4].m_y = 0.f;
        uv[idRightZoneUv].m_uvSquareOut[4].m_y = 0.f;
        uv[idLeftZoneUv].m_uvSquareIn[4].m_y = 0.f;
        
        //  colors vertexs 
        u32 vtxColorDownInit, vtxColorUpInit;
        setVtxColor( config->m_textureConfigs[idTexConfig_InArchi], vtxColorUpInit, vtxColorDownInit ); 

        // tex switch
        bbool texHasSwitched = bfalse;
        u32 vtxColorDownSwitch = 0;
        u32 vtxColorUpSwitch = 0;
       
        if ( config->m_idTexSwitch != -1 )
        {
            if ( config->m_textureConfigs[config->m_idTexSwitch].getPath().isEmpty() )
            {
                config->m_idTexSwitch = -1;
            }
            else
            {
                elemRouterSwitch = getRoutingElement(MeshElementRouter::MeshElementTable_FlagDefault, config->m_idTexSwitch);
                ITF_ASSERT(elemRouterSwitch);
                ITF_ASSERT(elemRouterSwitch->m_tableID == MeshElementRouter::MeshElementTable_Static);

                setVtxColor( config->m_textureConfigs[config->m_idTexSwitch], vtxColorUpSwitch, vtxColorDownSwitch ); 

                if ( m_pointsList.isLooping() && edgeRunListCount > 1 )
                {
                    if ( isEdgeRunWithSwitchTexture( _edgeRunList[edgeRunListCount-1], _edgeList ) )
                        texHasSwitched = btrue;
                }
            }
        }
        
        for( u32 idEdgeRun = 0; idEdgeRun < edgeRunListCount; idEdgeRun ++ )
        {
            edgeRun& edgeRunCur = _edgeRunList[idEdgeRun];            

            // start edge
            edgeFrieze& edgeCur = _edgeList[edgeRunCur.m_idEdgeStart];

            // id Tex
            const MeshElementRouter* elemRouterEdgeRun = elemRouter;
            const MeshElementRouter* elemRouterExt = elemRouter;

            // color
            u32 vtxColorDownEdgeRun = vtxColorDownInit;
            u32 vtxColorUpEdgeRun = vtxColorUpInit;
            u32 vtxColorDownExt = vtxColorDownInit;
            u32 vtxColorUpExt = vtxColorUpInit;

            f32 forceVariance = -1.f;

            if ( config->m_idTexSwitch != -1 )
            {
                if ( isEdgeRunWithSwitchTexture( edgeRunCur, _edgeList ) )
                {
                    elemRouterEdgeRun = elemRouterSwitch;
                    elemRouterExt = elemRouterSwitch;

                    texHasSwitched = btrue;

                    // color edgeRun
                    vtxColorDownEdgeRun = vtxColorDownSwitch;
                    vtxColorUpEdgeRun = vtxColorUpSwitch;

                    // color ext
                    vtxColorDownExt = vtxColorDownSwitch;
                    vtxColorUpExt = vtxColorUpSwitch;

                    // set variance in texSwitch
                    forceVariance = varianceZone[edgeRunCur.m_idZone].m_inStart;

                }
                else if ( texHasSwitched )
                {
                    texHasSwitched = bfalse;
                    elemRouterExt = elemRouterSwitch;

                    // color ext
                    vtxColorDownExt = vtxColorDownSwitch;
                    vtxColorUpExt = vtxColorUpSwitch;

                    // set variance in texSwitch
                    forceVariance = varianceZone[edgeRunCur.m_idZone].m_inStop;
                }
            }
                

            // build extremity on start edge
            if ( edgeCur.m_cornerAngle != 0.f && !isEdgeWithHoleVisual(edgeCur))
            {
                // edge Last
                u32 idEdgeLast = ( edgeRunCur.m_idEdgeStart +m_recomputeData.m_edgeListCount -1 ) %m_recomputeData.m_edgeListCount;
                edgeFrieze& edgeLast = _edgeList[idEdgeLast];
 
                if ( !isEdgeWithHoleVisual(edgeLast))
                {
                    if ( edgeCur.m_cornerAngle >0.f )
                        buildVB_Static_CornerSquareExt( config, _vertexTempList, _indexTempList[elemRouterExt->m_indexInTable], edgeCur, edgeLast, &uv[edgeRunCur.m_idZone].m_uvSquareOut[0], vtxColorUpExt, vtxColorDownExt, btrue, forceVariance );
                    else
                        buildVB_Static_CornerSquareExt( config, _vertexTempList, _indexTempList[elemRouterExt->m_indexInTable], edgeCur, edgeLast, &uv[edgeRunCur.m_idZone].m_uvSquareIn[0], vtxColorUpExt, vtxColorDownExt, bfalse, forceVariance );
                }                               
            }

            // build VB edgeRun
            Vec2d uvUp( getUvX_Start(), uv[edgeRunCur.m_idZone].m_uvYup );
            Vec2d uvDown( uvUp.m_x, uv[edgeRunCur.m_idZone].m_uvYdown );

            buildVB_Static_EdgeRun( config, _edgeList, edgeRunCur, _vertexTempList, _indexTempList[elemRouterEdgeRun->m_indexInTable], vtxColorUpEdgeRun, vtxColorDownEdgeRun, vtxColorUpEdgeRun, vtxColorDownEdgeRun, indexVtxUp, indexVtxDown, uvUp, uvDown, uv[edgeRunCur.m_idZone].m_uvXsign * m_recomputeData.m_uvXsign );             
        }
    } 

    void Frise::buildVB_Static_CornerSquareExt( FriseConfig* config, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList& _indexTempList, edgeFrieze& _edgeCur, edgeFrieze& _edgeLast, Vec2d* _uv, u32 _colorUp, u32 _colorDown, bbool _flipEdge, f32 _varianceForced ) const
    {        
    /*  edgeLast
        .    . 
        .    .
        .    .
        v1 . v3 . .
        .    .
        .    .   --> edgeCur
        .    .      
        v0 . v2 . .
        .    .
        .    .
        .    . 
        edgeLast    */
       
        u16 indexVtxDown = (u16)_vertexTempList.size();
        u16 indexVtxUp = indexVtxDown +1;

        f32 zVtx[4];
        Vec2d *v0, *v3, *v2, *v1;

        // uv variance
        Vec2d uvVariance(Vec2d::Zero);
        if ( _varianceForced != -1.f )
        {
            uvVariance.m_x = 0.5f *_varianceForced;
        }
        else if ( _uv[4].m_x != 0.f )
        {
            uvVariance.m_x = 0.5f *_uv[4].m_y;
            _uv[4].m_y = f32_Modulo( _uv[4].m_y +1.f, 2.f );
        }

        if ( _edgeCur.m_cornerAngle >0.f )
        {
            v0 = &_edgeLast.m_points[3];
            v1 = &_edgeCur.m_interUp;

            zVtx[0] = m_recomputeData.m_zVtxUp;
            zVtx[1] = m_recomputeData.m_zVtxUp;
            zVtx[2] = m_recomputeData.m_zVtxDown;
            zVtx[3] = m_recomputeData.m_zVtxUp;
        }
        else
        {
            v0 = &_edgeCur.m_interDown;
            v1 = &_edgeLast.m_points[2];

            zVtx[0] = m_recomputeData.m_zVtxDown;
            zVtx[1] = m_recomputeData.m_zVtxDown;
            zVtx[2] = m_recomputeData.m_zVtxDown;
            zVtx[3] = m_recomputeData.m_zVtxUp;
        }
                
        v2 = &_edgeCur.m_points[0];
        v3 = &_edgeCur.m_points[1];
                
        VertexPCT& vtx0 = _vertexTempList.incrSize(4);
        vtx0.m_pos.m_x = v0->m_x;
        vtx0.m_pos.m_y = v0->m_y;
        vtx0.m_pos.m_z = zVtx[0];
        vtx0.m_uv = _uv[0] +uvVariance;        

        VertexPCT& vtx1 = _vertexTempList[ indexVtxUp ];
        vtx1.m_pos.m_x = v1->m_x;
        vtx1.m_pos.m_y = v1->m_y;
        vtx1.m_pos.m_z = zVtx[1];
        vtx1.m_uv = _uv[1] +uvVariance;
        
        VertexPCT& vtx2 = _vertexTempList[ indexVtxDown +2];
        vtx2.m_pos.m_x = v2->m_x;
        vtx2.m_pos.m_y = v2->m_y;
        vtx2.m_pos.m_z = zVtx[2];
        vtx2.m_uv = _uv[2] +uvVariance; 
        
        VertexPCT& vtx3 = _vertexTempList[ indexVtxUp +2];
        vtx3.m_pos.m_x = v3->m_x;
        vtx3.m_pos.m_y = v3->m_y;
        vtx3.m_pos.m_z = zVtx[3];
        vtx3.m_uv = _uv[3] +uvVariance;       

        // color
        if ( _edgeCur.m_cornerAngle > 0.f )
        {
            vtx2.setColor( _colorDown);
            vtx0.setColor( _colorUp);
            vtx1.setColor( _colorUp);
            vtx3.setColor( _colorUp);            
        }
        else
        {
            vtx3.setColor( _colorUp);            
            vtx0.setColor( _colorDown);         
            vtx1.setColor( _colorDown);
            vtx2.setColor( _colorDown);
        }

        addIndex_Static_Quad( _vertexTempList, _indexTempList, indexVtxDown, indexVtxUp, _flipEdge );
    }

    void Frise::buildCollision_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList  )
    {
        if ( !buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
            return;

        buildCollision_InArchi_CornerSquare( _edgeList, _edgeRunList, _collisionRunList );

        finalizeCollisionList();
    }

    void Frise::buildCollision_InArchi_CornerSquare( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        SafeArray<Vec2d> collision;
        collision.reserve( m_recomputeData.m_edgeListCount *3 +3 );

        edgeRun* edgeRunLast = NULL;
        f32 offsetLast = -1.f;
        u32 edgeRunListCount = _edgeRunList.size();
        u32 collisionRunListCount = _collisionRunList.size();

        for ( u32 idCol = 0; idCol < collisionRunListCount; idCol++ )
        {
            collisionRun& colRun = _collisionRunList[idCol];            
            edgeRun* edgeRunCur = &_edgeRunList[colRun.m_idEdgeRunStart];

            // init offset position
            f32 offsetCur = getCollisionOffSet( config, edgeRunCur, colRun);

            ITF_VECTOR<CollisionByGameMat> collisionList;
            CollisionByGameMat collisionByGMat;
            collisionByGMat.m_startId = 0;
            collisionByGMat.m_gameMat = getCollisionGameMaterial( config, edgeRunCur->m_idTex );

            // countOffsets
            u32 edgeCountStartOffset = colRun.m_edgeCountStartOffset;

            // build start points   
            if ( edgeRunCur->m_idTex != -1 && colRun.m_collisionOffsetLast != -1.f )       
            {
                bbool build = bfalse;
                if ( colRun.m_idEdgeRunStart>0 || m_pointsList.isLooping())
                {
                    u32 idEdgeRunLast = ( colRun.m_idEdgeRunStart -1 +edgeRunListCount) % edgeRunListCount;
                    u32 idEdgeLastOfEdgeRunLast = ( _edgeRunList[idEdgeRunLast].m_idEdgeStart +_edgeRunList[idEdgeRunLast].m_edgeCount +m_recomputeData.m_edgeListCount -1) % m_recomputeData.m_edgeListCount;
                    const edgeFrieze& edgeLast = _edgeList[idEdgeLastOfEdgeRunLast];

                    if ( isEdgeWithHoleCollision(edgeLast) && isEdgeWithHoleVisual(edgeLast) )
                    {
                        build = btrue;
                        const edgeFrieze& edgeCur = _edgeList[edgeRunCur->m_idEdgeStart];

                        buildOutline_PosStartOnEdgeWithHoleCollision(config, collision, edgeCur, offsetCur);
                    }
                }

                if ( !build)
                    buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunCur, colRun, offsetCur, colRun.m_collisionOffsetLast, btrue );
            }
            else
                buildCollision_CornerSquare_StartEdgeRun( config, _edgeList, collision, &_edgeList[colRun.m_idEdgeStart], colRun.m_idEdgeStart, offsetCur, colRun );


            for ( u32 edgeRunCount = 0; edgeRunCount < colRun.m_edgeRunCount; edgeRunCount++ )
            {
                u32 idEdgeRun = ( edgeRunCount + colRun.m_idEdgeRunStart ) % edgeRunListCount;
                edgeRunCur = &_edgeRunList[idEdgeRun];

                // update offset position
                offsetCur = getCollisionOffSet( config, edgeRunCur, colRun);         

                if ( edgeRunLast )
                {
                    u32 buildCount = collision.size();

                    // transition between two edgeRun
                    buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunCur, colRun, offsetCur, offsetLast );

                    const Path& gameMat = getCollisionGameMaterial(config, edgeRunCur->m_idTex );

                    // add collision
                    if ( gameMat != collisionByGMat.m_gameMat )
                    {
                        u32 buildAddBy2 = Max( 0, i32(collision.size() -buildCount) -2);
                        collisionByGMat.m_stopId = buildCount + buildAddBy2;

                        collisionList.push_back(collisionByGMat);

                        collisionByGMat.m_startId = collisionByGMat.m_stopId;
                        collisionByGMat.m_gameMat = gameMat;
                    }
                }

                // outline edgeRun
                u32 countStopOffset = 0;
                if ( edgeRunCount == colRun.m_edgeRunCount -1)
                    countStopOffset = colRun.m_edgeCountStopOffset;

                if ( config->m_smoothCollision )
                    buildOutline_EdgeRunWithCornerRounder( config, collision, _edgeList, edgeRunCur, offsetCur, config->m_smoothFactorCollision, edgeCountStartOffset, countStopOffset );
                else
                    buildOutline_EdgeRunWithCornerSquare( config, collision, _edgeList, edgeRunCur, offsetCur, edgeCountStartOffset, countStopOffset );

                edgeRunLast = edgeRunCur;
                offsetLast = offsetCur;
                edgeCountStartOffset = 0;
            }
 
            // build stop points  
            u32 idEdgeRunNext = ( colRun.m_idEdgeRunStart +colRun.m_edgeRunCount ) % edgeRunListCount;
            edgeRun* edgeRunNext = &_edgeRunList[idEdgeRunNext];
                      
            if ( edgeRunNext->m_idTex != -1 && colRun.m_collisionOffsetNext != -1.f /*&& colRun.m_collisionOffsetNext != offsetCurReal*/ )
            {
                bbool build = bfalse;

                edgeFrieze& edgeNext = _edgeList[ _edgeRunList[idEdgeRunNext].m_idEdgeStart];
                if ( isEdgeWithHoleCollision(edgeNext) && isEdgeWithHoleVisual(edgeNext) )
                {
                    build = btrue;
                    u32 idEdgeCur = ( edgeRunLast->m_idEdgeStart +edgeRunLast->m_edgeCount +m_recomputeData.m_edgeListCount -1)%m_recomputeData.m_edgeListCount;
                    const edgeFrieze& edgeCur = _edgeList[idEdgeCur];

                    buildOutline_PosStopOnEdgeWithHoleCollision( config, collision, edgeCur, offsetCur, edgeNext.m_idPoint);
                }                

                if( !build)
                    buildCollision_ExtremityWithCoord( config, collision, _edgeList, edgeRunNext, colRun, colRun.m_collisionOffsetNext, offsetCur, bfalse );
            }
            else    
            {
                u32 idStopEdge = ( colRun.m_idEdgeStart + colRun.m_edgeCount +m_recomputeData.m_edgeListCount -1 ) % m_recomputeData.m_edgeListCount;
                buildCollision_CornerSquare_StopEdgeRun( config, collision, _edgeList, idStopEdge, offsetCur, colRun );
            }

            // add collision
            collisionByGMat.m_stopId = collision.size();
            collisionList.push_back(collisionByGMat);

            addPointsCollision( config, collision, collisionList, colRun );
            collision.clear();

            edgeRunLast = NULL;            
        }       
    }

    bbool Frise::isEdgeRun_InArchi( ITF_VECTOR<edgeFrieze>& _edgeList, u32 _idCurEdge, u32 _idLastEdge, u32 _idLastLastEdge ) const
    {        
        if ( m_lockTexture == LockTexture_Switch )
            return btrue;

        u32 idZoneLastEdge = getZoneId_InArchi( _edgeList[_idLastEdge].m_sightNormalized );

        if ( getZoneId_InArchi( _edgeList[_idLastLastEdge].m_sightNormalized ) == idZoneLastEdge )
            return bfalse;

        if ( getZoneId_InArchi( _edgeList[_idCurEdge].m_sightNormalized ) == idZoneLastEdge )
            return bfalse;

        return btrue;
    }

} // namespace ITF
