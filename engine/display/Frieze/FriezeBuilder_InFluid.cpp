#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

namespace ITF
{
    void Frise::registerFluidShaper(u32 _layerIndex, FluidShaper *_shaper) 
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.addFluidShaper(_layerIndex, _shaper);
        }        
    }

    void Frise::unregisterFluidShaper(u32 _layerIndex, FluidShaper *_shaper) 
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.removeFluidShaper(_layerIndex, _shaper);
        }        
    }

    const SafeArray<ObjectRef>* Frise::getStringWaveGenerators() const 
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
            return &m_pMeshFluidData->m_data.getStringWaveGenerators();

        return NULL;
    }

    void Frise::setFluidLayerCollisionHeightMultiplier(u32 _layerIndex, f32 _multiplier)
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.setLayerCollisionHeightMultiplier(_layerIndex, _multiplier);
        }        
    }

    void Frise::clearMeshFluidData()
    {
        if ( m_pMeshFluidData )
        {
            m_pMeshFluidData->m_data.clear();
            SF_DEL(m_pMeshFluidData);
        }
    }

    void Frise::initDataFluid()
    {
        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        m_pMeshFluidData = newAlloc(mId_Frieze, MeshFluidData);

        m_pMeshFluidData->m_data.setFrieze(this);
        m_pMeshFluidData->m_data.setIsCushion(config->m_collisionFrieze.m_build);
        m_pMeshFluidData->m_data.setLayers(config->m_fluidLayers);
        m_pMeshFluidData->m_data.setWeightMultiplier(config->m_fluidWeightMultiplier);
    }

    void Frise::buildFrieze_InString( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList )
    {
        buildFrieze_InFluid( _edgeList, _edgeRunList, _vertexTempList, _indexTempList );
    }

    void Frise::buildFrieze_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, MeshStatic_VertexList& _vertexTempList, MeshStatic_IndexList* _indexTempList )
    {
        if ( FriseConfig* config = getConfig() )
            config->m_cornerFactor = Max( 10.f, config->m_cornerFactor);

        // reset scale point
        m_pointsList.resetAllPointsScale();

        copyEdgeFromPolyline(_edgeList);

        m_recomputeData.m_texRatio = 0.25f;

        buildEdgeList( _edgeList );

        if ( m_recomputeData.m_edgeListCount == 0 )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeList(_edgeList);        
#endif

        if ( !buildEdgeRunList_InFluid( _edgeList, _edgeRunList ) )
            return;

        if ( !setEdgeRunListCoeffUv( _edgeList, _edgeRunList ) )
            return;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeRun( _edgeRunList );             
#endif
        
       initDataFluid();
       buildEdgeFluidList( _edgeList, _edgeRunList );
       createNewPolylineVisual( _edgeList );

       buildVB_FillGradient( _edgeList, _vertexTempList, _indexTempList ); 
/*
       ITF_VECTOR<EdgeFluid> & edgeFluidList = m_pMeshFluidData->m_pDataFluid.getEdgeFluid();
       u32 edgeFluidCount = edgeFluidList.size();
       for ( u32 i = 0; i<edgeFluidCount; i++ )
       {
           GFX_ADAPTER->drawDBGLine( edgeFluidList[i].m_pos[0], edgeFluidList[i].m_pos[1], Color::black() , 0.f, 5.f, getDepth() );
           GFX_ADAPTER->drawDBGLine( edgeFluidList[i].m_pos[2], edgeFluidList[i].m_pos[3], Color::black() , 0.f, 5.f, getDepth() );
       }              */
    }

    bbool Frise::buildEdgeRunList_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList ) const
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return bfalse;

        _edgeRunList.reserve(m_recomputeData.m_edgeListCount);

        edgeRun edgeRunCur;        
        edgeRunCur.m_idTex = getFirstNoFillTextureIndex( config );

        if ( edgeRunCur.m_idTex == -1 )
        {
            edgeRunCur.m_edgeCount = m_recomputeData.m_edgeListCount;
            _edgeRunList.push_back(edgeRunCur);
            ITF_WARNING(this, 0, "Wrong method or missing texture, to use regionId only use method 1");
            return bfalse;
        }

        // only one edge by edgeRun
        edgeRunCur.m_edgeCount = 1;

        for ( u32 i = 0; i< m_recomputeData.m_edgeListCount; i++ )
        {
            edgeRunCur.m_idEdgeStart = i;
            _edgeRunList.push_back(edgeRunCur);
        }

        return btrue;
    }

    i32 Frise::getIdStartEdge_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList )
    {  
        for ( u32 i = 0; i < m_recomputeData.m_edgeListCount; i++ )
        {
            if ( isEdgeValid_InFluid( _edgeList[i] ) )
                return (i32)i;
        }

        return -1;
    }

    bbool Frise::isEdgeValid_InFluid( edgeFrieze& _edge )
    {
        if ( _edge.m_snap )
            return bfalse;

        if ( isEdgeWithHoleVisual( _edge ) )
            return bfalse;

        return btrue;        
    }

    void Frise::buildEdgeFluidList( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList)
    {
        FriseConfig* config = getConfig();
        if ( !config || _edgeRunList[0].m_idTex == -1 )
            return;
        
        i32 idStartEdge = getIdStartEdge_InFluid( _edgeList );
        if ( idStartEdge == -1 )
            return;

        ITF_VECTOR<EdgeFluid> & edgeFluidList = m_pMeshFluidData->m_data.getEdgeFluid();

        // init uvs
        f32 uvYup = 0.f;
        f32 uvYdown = 0.f;

        if ( m_recomputeData.m_isUvFlipY )
        {
            uvYdown = 0.f + m_recomputeData.m_uvYoffSet;
            uvYup = uvYup + m_uvY_tile;    
        }
        else
        {
            uvYup = 0.f + m_recomputeData.m_uvYoffSet;
            uvYdown = uvYup + m_uvY_tile;
        }
        
        f32 uvXcur = getUvX_Start();
        for ( i32 i = 0; i < idStartEdge; i++ )
        {
            uvXcur += _edgeList[i].m_normUv * _edgeRunList[i].m_coeff * m_recomputeData.m_uvXsign;
        }

        /*
                1__3 
                |\ | 
                |_\| 
                0  2 
        */
                              
        // Start
        edgeFrieze& edgeStart = _edgeList[idStartEdge];
        EdgeFluid   edgeFluidCur;
        u32         flag = EDGEFLUID_MAIN_DATA | EDGEFLUID_VISUAL;

        edgeFluidCur.m_texture = (Texture*)config->m_textureConfigs[_edgeRunList[0].m_idTex].m_textureData.getResource();
        edgeFluidCur.m_blendMode = config->m_textureConfigs[_edgeRunList[0].m_idTex].m_gfxMat.m_blendMode;
        edgeFluidCur.m_uvAnimTrans = config->m_textureConfigs[_edgeRunList[0].m_idTex].m_scrollingSpeedTrans;
        edgeFluidCur.m_depth = getDepth();
        edgeFluidCur.m_height = edgeStart.m_heightStart;
        edgeFluidCur.m_delta    = config->m_fluidLevelDelta;
        edgeFluidCur.m_deltaUV  = config->m_fluidUVDelta;
        edgeFluidCur.m_visualProcess.m_id = idStartEdge;

        edgeFluidCur.m_pos[0] = edgeStart.m_interDown;           
        edgeFluidCur.m_pos[1] = edgeStart.m_interUp;

        edgeFluidCur.m_uv[0] = Vec2d( uvXcur, uvYdown );     
        edgeFluidCur.m_uv[1] = Vec2d( uvXcur, uvYup );        
        edgeFluidCur.m_uv[2].m_y = uvYdown;     
        edgeFluidCur.m_uv[3].m_y = uvYup;    

        if ( config->m_collisionFrieze.m_build && !isEdgeWithHoleCollision( edgeStart ) )
        {
            edgeFluidCur.m_collisionProcesses[0].m_id = 0; 
            edgeFluidCur.m_collisionProcesses[1].m_id = 0;
            flag |= EDGEFLUID_COLLISION;
        }
        else
        {
            edgeFluidCur.m_collisionProcesses[0].m_id = -1;
            edgeFluidCur.m_collisionProcesses[1].m_id = -1;

        }

        // update uv
        uvXcur += edgeStart.m_normUv * _edgeRunList[idStartEdge].m_coeff * m_recomputeData.m_uvXsign;

        // list
        bbool addEdgeFluidInList = btrue;
        for ( u32 i = idStartEdge +1; i<m_recomputeData.m_edgeListCount; i++ )
        {
            edgeFrieze& edgeCur = _edgeList[i];
 
            if ( addEdgeFluidInList )
            {
                addEdgeFluidInList = bfalse;

                // stop last
                edgeFluidCur.m_pos[2] = edgeCur.m_interDown;
                edgeFluidCur.m_pos[3] = edgeCur.m_interUp;
                edgeFluidCur.m_uv[2].m_x = uvXcur;     
                edgeFluidCur.m_uv[3].m_x = uvXcur; 
                edgeFluidCur.updateData(config, flag);
                edgeFluidList.push_back(edgeFluidCur);
            }

            if ( isEdgeValid_InFluid( edgeCur ) )
            {
                addEdgeFluidInList = btrue;
                flag = EDGEFLUID_MAIN_DATA | EDGEFLUID_VISUAL;

                // start next
                edgeFluidCur.m_pos[0] = edgeCur.m_interDown;
                edgeFluidCur.m_pos[1] = edgeCur.m_interUp;
                edgeFluidCur.m_uv[0].m_x = uvXcur;     
                edgeFluidCur.m_uv[1].m_x = uvXcur; 
                edgeFluidCur.m_visualProcess.m_id = i;
                edgeFluidCur.m_height = edgeCur.m_heightStart;

                if ( config->m_collisionFrieze.m_build && !isEdgeWithHoleCollision( edgeCur ) )
                {
                    edgeFluidCur.m_collisionProcesses[0].m_id = 0; 
                    edgeFluidCur.m_collisionProcesses[1].m_id = 0; 
                    flag |= EDGEFLUID_COLLISION;
                }
                else
                {
                    edgeFluidCur.m_collisionProcesses[0].m_id = -1;
                    edgeFluidCur.m_collisionProcesses[1].m_id = -1;
                }
            }

            // update uv
            uvXcur += edgeCur.m_normUv * _edgeRunList[i].m_coeff * m_recomputeData.m_uvXsign;
        }

        if ( addEdgeFluidInList )
        {
            // EdgeFluid Stop
            if ( m_pointsList.isLooping() )
            {
                edgeFluidCur.m_pos[2] = _edgeList[0].m_interDown; 
                edgeFluidCur.m_pos[3] = _edgeList[0].m_interUp;
            }
            else
            {
                edgeFrieze& edgeStop = _edgeList[m_recomputeData.m_edgeListCount -1];

                edgeFluidCur.m_pos[2] = edgeStop.m_points[2];
                edgeFluidCur.m_pos[3] = edgeStop.m_points[3];
            }

            edgeFluidCur.m_uv[2].m_x = uvXcur;     
            edgeFluidCur.m_uv[3].m_x = uvXcur; 

            edgeFluidCur.updateData(config, flag);
            if (config->m_methode == InString)
            {
                ITF_WARNING_CATEGORY(LD,this, edgeFluidCur.m_lastIndex<(u32)(NB_FLUID_LEVELS-1), "frieze edge is too long, wave may loose details");                
            }
            edgeFluidList.push_back(edgeFluidCur);
        }        
        
        // set vertexs color
        setEdgeFluidVertexColors( config->m_textureConfigs[_edgeRunList[0].m_idTex], edgeFluidList );

        m_pMeshFluidData->m_data.computeEdgeFluidLevels(config->m_fluidLevelsFront, config->m_fluidLevelsBack,
                                           config->m_fluidPerpendicularBack || config->m_fluidPerpendicularFront);
        if (config->m_fluidPerpendicularBack)
        {
            m_pMeshFluidData->m_data.addEdgeFluidLevelPerpendicular(-1, config->m_fluidPerpendicularBackScale, config->m_fluidPerpendicularBackPos, config->m_fluidPerpendicularBackPosZ, config->m_fluidPerpendicularBackZ);
        }
        if (config->m_fluidPerpendicularFront)
        {
            m_pMeshFluidData->m_data.addEdgeFluidLevelPerpendicular(1, config->m_fluidPerpendicularFrontScale, config->m_fluidPerpendicularFrontPos, config->m_fluidPerpendicularFrontPosZ, config->m_fluidPerpendicularFrontZ);
        }
        m_pMeshFluidData->m_data.computeAABB(&m_pMeshFluidData->m_AABB);
    }

    void Frise::setEdgeFluidVertexColors( FriseTextureConfig& _texConfig, ITF_VECTOR<EdgeFluid>& _edgeFluidList ) const
    {
        // init vertex color
        u32 vtxColorDown, vtxColorUp, vtxColorUpExtremity, vtxColorDownExtremity;
        setVtxColorWithExtrem( _texConfig, vtxColorUp, vtxColorDown, vtxColorUpExtremity, vtxColorDownExtremity, btrue );

        const u32 edgeFluidCount = _edgeFluidList.size();
        ITF_ASSERT( edgeFluidCount );

        FriseConfig *config = getConfig();
        ITF_ASSERT_CRASH(config, "no config!");
        // set colors
        u32 idEdgeLast = edgeFluidCount -1;
        for ( u32 i =0; i<edgeFluidCount; i++ )
        {
            EdgeFluid& edgeCur = _edgeFluidList[i];
            EdgeFluid& edgeLast = _edgeFluidList[idEdgeLast];
            idEdgeLast = i;

            if ( edgeCur.m_pos[0].IsEqual( edgeLast.m_pos[2], MTH_EPSILON ) )
            {
                edgeCur.m_colors[0] = vtxColorDown;
                edgeCur.m_colors[1] = vtxColorUp;
            }
            else
            {
                edgeCur.m_colors[0] = vtxColorDownExtremity;
                edgeCur.m_colors[1] = vtxColorUpExtremity;
            }

            for (u32 layerIndex=1; layerIndex<config->m_fluidLayers.size(); layerIndex++)
            {
                u32 colorIndexBase = 4*layerIndex;
                edgeCur.m_colors[colorIndexBase] = edgeCur.m_colors[colorIndexBase+1] = edgeCur.m_colors[colorIndexBase+2] = edgeCur.m_colors[colorIndexBase+3] = config->m_fluidLayers[layerIndex].m_color.getAsU32();
            }

            edgeLast.m_colors[2] = edgeCur.m_colors[0];
            edgeLast.m_colors[3] = edgeCur.m_colors[1];
        }      

        // check colors 
        for ( u32 i =0; i<edgeFluidCount; i++ )
        {
            EdgeFluid& edgeCur = _edgeFluidList[i];

            if (edgeCur.m_colors[0] == vtxColorDownExtremity && edgeCur.m_colors[2] == vtxColorDownExtremity)
            {
                edgeCur.m_colors[2] = vtxColorDown;
                edgeCur.m_colors[3] = vtxColorUp;
            }
        }
    }

    void Frise::buildCollision_InString( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        if ( buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
        {
            // build collision for edge basics 
            buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );
        }

        FriseConfig* config = getConfig();
        if ( !config || !config->m_collisionFrieze.m_build )
            return;

        clearCollisionData();
        // build collision for edge fluids 
        for (u32 i=0; i<config->m_fluidSideCount; i++)
        {
            buildCollision_EdgeFluidList( config, i );
        }

        finalizeCollisionList();
    }

    void Frise::buildCollision_InFluid( ITF_VECTOR<edgeFrieze>& _edgeList, ITF_VECTOR<edgeRun>& _edgeRunList, ITF_VECTOR<collisionRun>& _collisionRunList )
    {
        if ( buildCollisionRunList( _edgeList, _edgeRunList, _collisionRunList ) )
        {
            // build collision for edge basics 
            buildCollision_EdgeRunList ( _edgeList, _edgeRunList, _collisionRunList );
        }

        FriseConfig* config = getConfig();
        if ( !config || !config->m_collisionFrieze.m_build )
            return;

        // build collision for edge fluids 
        buildCollision_EdgeFluidList( config, 0 );

        finalizeCollisionList();
    }

    void Frise::buildCollision_EdgeFluidList( FriseConfig* _config, u32 _sideIndex )
    {
        ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

        if ( !m_pMeshFluidData )
            return;

        ITF_VECTOR<EdgeFluid>& edgeFluidList = m_pMeshFluidData->m_data.getEdgeFluid();

        u32 edgeFluidListCount = edgeFluidList.size();
        PolyLine *lastPolyLine = NULL;
        const StringID &gameMaterial = _config->m_gameMaterial.getStringID();

        i32 edgeIndex_Start, edgeIndex_End, edgeIndex_Step;

        if (_sideIndex)
        {
            edgeIndex_Start = (i32)edgeFluidListCount-1;
            edgeIndex_End = -1;
            edgeIndex_Step = -1;
        }
        else
        {
            edgeIndex_Start = 0;
            edgeIndex_End = (i32)edgeFluidListCount;
            edgeIndex_Step = 1;
        }


        for ( i32 i = edgeIndex_Start; i != edgeIndex_End; i+=edgeIndex_Step )
        {
            EdgeFluid& edgeFluidCur = edgeFluidList[i];
            EdgeProcessData &collisionProcess = edgeFluidCur.m_collisionProcesses[_sideIndex];

            // no collision wanted
            if ( collisionProcess.m_id == -1 )
            {
                lastPolyLine = NULL;
                continue;
            }

            if ( !m_pFriezeFullData )
                createFriezeFullData();

            // get id collision
            collisionProcess.m_id = getIdEmptyCollision();

            // if any empty collision found, create collision data
            if ( collisionProcess.m_id == -1 )
            {
                collisionProcess.m_id = m_pFriezeFullData->m_staticCollisionData.size();
                createCollisionData();
                PolyLine *newPolyLine = m_pFriezeFullData->m_staticCollisionData.back();
                if (lastPolyLine)
                {
                    newPolyLine->m_connection.setPrevious_DontConnectPoints( lastPolyLine );    
                }
                lastPolyLine = newPolyLine;

            }

            // fill collision polyline
            PolyLine *polyLine = m_pFriezeFullData->m_staticCollisionData[collisionProcess.m_id];
            edgeFluidCur.fillPolyline(polyLine, &m_pFriezeFullData->m_AABB_Collision, EDGEFLUID_COLLISION, _sideIndex);
            polyLine->setGameMaterial( gameMaterial );
        }
    }

    void Frise::createNewPolylineVisual(const ITF_VECTOR<edgeFrieze>& _edgeList)
    {
        FriseConfig *config = getConfig();
        if (!config)
            return;

        if ( config->m_regionId != s_RegionName_Swim ||
             m_pMeshFluidData->m_data.getEdgeFluid().size() == 0 || _edgeList.size() == 0)
            return;

        PolyLine * visualPoly = newAlloc(mId_Frieze,PolyLine());
        visualPoly->setLoop(btrue);
        visualPoly->setDepth(getDepth());
        visualPoly->onLoaded();
        visualPoly->m_ownerFrise = this;

        // process points
        i32 currentIndex = 0;
        ITF_VECTOR<EdgeFluid>& edgeFluidList = m_pMeshFluidData->m_data.getEdgeFluid();

        u32 edgeFluidListCount = edgeFluidList.size();

        for ( u32 i = 0; i < edgeFluidListCount; i++, currentIndex++ )
        {
            EdgeFluid& edgeFluidCur = edgeFluidList[i];
            for (;currentIndex < edgeFluidCur.m_visualProcess.m_id; currentIndex++)
            {
                visualPoly->addPoint(_edgeList[currentIndex].m_interDown);
                visualPoly->setHoleAt(visualPoly->getPosCount()-1, _edgeList[currentIndex].m_holeMode);
            }
            edgeFluidCur.m_visualProcess.m_indexStart   = visualPoly->getPosCount();
            edgeFluidCur.fillPolyline(visualPoly, NULL, EDGEFLUID_VISUAL, 0);
            edgeFluidCur.m_visualProcess.m_indexEnd     = visualPoly->getPosCount() - 1;
        }
        if (currentIndex < (i32)_edgeList.size())
        {
            for (;currentIndex < (i32)_edgeList.size(); currentIndex++)
            {
                visualPoly->addPoint(_edgeList[currentIndex].m_interDown);
                visualPoly->setHoleAt(visualPoly->getPosCount()-1, _edgeList[currentIndex].m_holeMode);
            }
            u32 lastEdgeIndex = _edgeList.size()-1;
            if( isLooping())
                visualPoly->addPoint(_edgeList[0].m_interDown);
            else
                visualPoly->addPoint(_edgeList[lastEdgeIndex].m_points[2]);
            visualPoly->setHoleAt(visualPoly->getPosCount()-1, _edgeList[lastEdgeIndex].m_holeMode);
        }
        for (u32 i = 0; i < visualPoly->getPosCount(); i++)
            visualPoly->setPosAt(transformPos(visualPoly->getPosAt(i).to3d(), btrue).truncateTo2D(), i);
        // make it cycle
        visualPoly->setLoop(btrue);
        visualPoly->forceRecomputeData();
        visualPoly->setGameMaterial(config->m_gameMaterial.getStringID());

        m_visualPoly = visualPoly->getRef();
    }
} // namespace ITF
