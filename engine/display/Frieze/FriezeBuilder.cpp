#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_FRIEZEPLUGIN_H_
#include "engine/display/Frieze/FriezePlugin.h"
#endif //_ITF_FRIEZEPLUGIN_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#if !defined(ITF_FINAL)
    #ifndef _ITF_WARNINGMANAGER_H_
    #include "engine/common/WarningManager.h"
    #endif // _ITF_WARNINGMANAGER_H_
#endif

namespace ITF
{

    FriezeRecomputeData_Static::FriezeRecomputeData_Static()
    {
        m_edgeListCount = 0;
        m_edgeListCountMax = 0;
        m_isHoleVisual = btrue;
        m_isUvFlipY = btrue;
        m_texRatio = 1.0f;
        m_anim = bfalse;
        m_uvXsign = 1.0f;
        m_animSyncCur = 0.0f;
        m_animSyncXCur = 0.0f;
        m_animSyncYCur = 0.0f;
        m_loopWanted = bfalse;
        m_alphaBorderWanted = bfalse;
        m_heightScale = 2.0f;
        m_widthScale = 2.0f;
        m_zVtxUp = 0.0f;
        m_zVtxDown = 0.0f;
        m_edgeRunStartBuildVB = bfalse;
        m_flexibility = 0.75f;
        m_heightScaleInv = 1.0f;
        m_uvYoffSet = 0.0f;
        m_uvXoffSet = 0.0f;

        for(u32 i = 0; i < MeshElementRouter::MeshElementTable_Count; ++i)
            m_MeshElementTable_ElemCount[0] = 0;
    }

    ITF::FriezeRecomputeData_Static Frise::m_recomputeData;
   
    void Frise::initDatas()
    {
        clearMeshFluidData();
        clearMeshOverlayData();
        clearMeshAnimData();
        clearMeshStaticData();
        
        m_collisionInGlobalSpace = bfalse;

        // clear wireframe
#ifdef ITF_WINDOWS
        EditorWireframeMesh.clear();
#endif

        m_recomputeData.m_edgeListCount = 0;
        m_recomputeData.m_anim = bfalse;
        m_recomputeData.m_alphaBorderWanted = bfalse;
        m_recomputeData.m_edgeRunStartBuildVB = bfalse;      
        m_isFilling = bfalse;
        m_recomputeData.m_isHoleVisual = btrue;

        // frieze data complex
        if ( m_pFriezeFullData )
        {
            m_pFriezeFullData->m_AABB_Mesh_Dynamic = Vec2d::Zero;
            m_pFriezeFullData->m_AABB_Anim_Dynamic = Vec2d::Zero;   
            m_pFriezeFullData->m_AABB_Overlay_Dynamic = Vec2d::Zero; 
            m_pFriezeFullData->m_AABB_Collision.reset();
        }

        //  flip uvs from propterty editor
        if ( m_uvX_flip )
            m_recomputeData.m_uvXsign = -1.f;
        else
            m_recomputeData.m_uvXsign = 1.f;

        m_recomputeData.m_isUvFlipY = m_uvY_flip;

        m_recomputeData.m_uvXoffSet = m_uvXoffSet;
        m_recomputeData.m_uvYoffSet = m_uvYoffSet;


        //Friseconfig dependent code

        FriseConfig* config = getConfig();
        if ( !config )
            return;
        
        //  flip uvs from fcg
        if ( config->m_isUvFlipX )
            m_recomputeData.m_uvXsign *= -1.f;
        if ( config->m_isUvFlipY )
            m_recomputeData.m_isUvFlipY = !m_recomputeData.m_isUvFlipY;

        m_recomputeData.m_flexibility = Clamp( config->m_flexibility, 0.f, 1.f );
        m_recomputeData.m_heightScale = config->m_height;
        m_recomputeData.m_widthScale = config->m_width;

        f32 zVtxExtrudeDown = config->m_zVtxExtrudeDown;
        f32 zVtxExtrudeUp = config->m_zVtxExtrudeUp;

        if ( config->m_useFriezeScale )
        {
            f32 scaleY = getScale().m_y *m_thickness;
            f32 scaleX = getScale().m_x *m_thickness;

            m_recomputeData.m_heightScale *= scaleY;
            m_recomputeData.m_widthScale *= scaleX;

            zVtxExtrudeDown *= scaleY; 
            zVtxExtrudeUp *= scaleY;
        }
        else
        {
            m_recomputeData.m_heightScale *= m_thickness;
            m_recomputeData.m_widthScale *= m_thickness;
        }
       
        // vtx
        m_recomputeData.m_zVtxDown = zVtxExtrudeDown;
        m_recomputeData.m_zVtxUp = zVtxExtrudeUp;

        i32 texConfigCount = config->m_textureConfigs.size();
        for (i32 i = 0; i < texConfigCount; i++ )
        {
            // alphaBorder
            u8 alphaTexture = config->m_textureConfigs[i].m_color.m_rgba.m_a;
            if ( alphaTexture != config->m_textureConfigs[i].m_alphaBorder && i != config->m_fillFriseTextureConfigIndex )
            {
                m_recomputeData.m_alphaBorderWanted = btrue;
                break;
            }
        }

        clearVisualPolyline();        
    }

    void Frise::createElementInRoutingTable(MeshElementRouter::MeshElementTableID _tableID, u32& _index, u8 _flags, u32 _textureConfigIndex )const
    {
        MeshElementRouter element;
        element.m_tableID = _tableID;
        element.m_indexInTable = _index;
        _index++;
        u32 key = (_textureConfigIndex & 65535) | (_flags << 16);
        m_recomputeData.m_ConfigToMeshElemTable[key] = element;
    }

    const MeshElementRouter* Frise::getRoutingElement(u8 _flags, u32 _textureConfigIndex)
    {
        u32 key = (_textureConfigIndex & 65535) | (_flags << 16);
        ConfigToMeshElem::iterator it = m_recomputeData.m_ConfigToMeshElemTable.find(key);
        if (it != m_recomputeData.m_ConfigToMeshElemTable.end())
            return &((*it).second);
        return NULL;
    }

	void Frise::executeRecomputeLineData()
	{
		ITF_ASSERT(m_querriedRecomputeData);
      
		m_querriedRecomputeData = bfalse;
        checkLoop();
		m_pointsList.executeRecomputeData();
		changeRelativeAABB(m_pointsList.getAABB());
	}

    void Frise::executeRecomputeData(   )
    {
#ifndef ITF_WII
        if (GFX_ADAPTER->isDeviceLost())
            return;
#endif // ITF_WII

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u Frise::executeRecomputeData()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME

#if 0 // Infinite loop to fix RO-4855
#ifndef ITF_FINAL   // TO REMOVE FROM HERE
        if ( !isDynamic() && !m_isDynamicFrieze && CAMERA->getMode() == Camera::Camera_InGame)
        {
            Scene* sceneCur = getScene();

            if ( sceneCur && sceneCur->isPhysicalReady())
            {
                World* worldCur = sceneCur->getWorld();

                if ( worldCur && worldCur->isPhysicalReady())
                {
                    String8 msg("Missing tag isDynamic on moving frieze -> important performance issue");
                    ITF_WARNING( this, 0, msg.cStr());
                    setDataError(msg);
                }
            }
        }
#endif //ITF_FINAL
#endif // 0

        ITF_ASSERT(m_querriedRecomputeData);

#if !defined(ITF_FINAL)
        WarningManager::clearWarningsForObject(this);
#endif

#define ADD_ROUTING_ELEM(_index_, _flags_) createElementInRoutingTable(MeshElementRouter::_index_, m_recomputeData.m_MeshElementTable_ElemCount[(u32)(MeshElementRouter::_index_)], MeshElementRouter::_flags_, (u32)i )
        m_recomputeData.m_ConfigToMeshElemTable.clear();
        memset(m_recomputeData.m_MeshElementTable_ElemCount, 0, sizeof(m_recomputeData.m_MeshElementTable_ElemCount));

        if ( m_regionRegistered )
        {
            // In case the config name changed
            REGIONS_MANAGER->removeRegion(m_visualPoly);
            m_regionRegistered = bfalse;
        }

        FriseConfig* config = getConfig();
        if ( config && config->m_posZForced )
        {
            setLocalInitialZ( config->m_posZ );
            Super::setDepth( getWorldInitialZ() ); // do not use Frise::setDepth() here, Frise::setDepth() can call forceRecomputeData() if depth has changed
        }
        
        executeRecomputeLineData();

        if ( !config )
        {
            clearComputedData();
            return;
        }

#ifndef ITF_FINAL
        if (hasDataError())
        {
            clearComputedData();
            return;
        }
#endif

        if ( ( m_isDynamicFrieze || isDynamic() ) && !m_pFriezeFullData )
            createFriezeFullData();

        Matrix44* pMatrixTransfo = NULL;
        if (!isDynamic())
        {
            pMatrixTransfo = (Matrix44*)Memory::alignedMallocCategory(sizeof(Matrix44), 16,MemoryId::mId_Frieze);
            pMatrixTransfo->reset();
            computeStaticDisplayMatrix(*pMatrixTransfo);              
        }

        if (!config->m_bInitializedAfterLoading)
        {
            config->onLoaded();
        }

        //  reinit des variables
        initDatas();

        checkDataConsistency();

		bbool doRegisterPhysic = bfalse;

		if (isActive())
		{
			doRegisterPhysic = btrue;
			unregisterPhysic();
		}

        // settings polyline
        m_pointsList.setGameMaterial( config->m_gameMaterial.getStringID() );

        i32 i = config->m_fillFriseTextureConfigIndex;
        if (i != -1) // if has fill
        {
            ADD_ROUTING_ELEM( MeshElementTable_Static, MeshElementTable_FlagFill);
        }

        i32 texConfigCount = (i32)config->m_textureConfigs.size();
        for ( i = 0; i < texConfigCount; i++ )
        {
            if ( i == config->m_fillFriseTextureConfigIndex )
                continue;

            const FriseTextureConfig& texConfig = config->m_textureConfigs[i];

            if ( (texConfig.getPath().isEmpty()) ) 
                continue;

            if ( m_recomputeData.m_anim && ( config->m_methode==InGeneric 
                || config->m_methode==InExtremity 
                || config->m_methode==InExtremitySimple
                || config->m_methode==InAtlas ) )
            {
                ADD_ROUTING_ELEM(MeshElementTable_Dynamic, MeshElementTable_FlagAnimated);
            }
            else
            {
                ADD_ROUTING_ELEM(MeshElementTable_Static, MeshElementTable_FlagDefault);
            }
        }

        // mesh static
        MeshStatic_VertexList vertexStatic_TempList;
        MeshStatic_IndexList  *indexStatic_TempList = newAlloc(mId_Frieze, MeshStatic_IndexList[m_recomputeData.m_MeshElementTable_ElemCount[MeshElementRouter::MeshElementTable_Static]]);

        // mesh anim
        MeshAnim_VertexList vertexAnim_TempList;
        MeshAnim_IndexList  *indexAnim_TempList = newAlloc(mId_Frieze, MeshAnim_IndexList[m_recomputeData.m_MeshElementTable_ElemCount[MeshElementRouter::MeshElementTable_Dynamic]]);

        // edgeList
        ITF_VECTOR<edgeFrieze> edgeList;
        // edgeRunList
        ITF_VECTOR<edgeRun> edgeRunList;
        // collisionRunList
        ITF_VECTOR<collisionRun> collisionRunList;

        switch(config->m_methode)
        {
        case InRoundness:
            buildFrieze_InRoundness( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList );
            orderEdgeRunListByTextureId( edgeRunList );
            buildCollision_InRoundness( edgeList, edgeRunList, collisionRunList );
            break;

        case InGeneric:
            buildFrieze_InGeneric( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList, vertexAnim_TempList, indexAnim_TempList );                      
            buildCollision_InGeneric( edgeList, edgeRunList, collisionRunList );
            break;

        case InExtremity:          
            buildFrieze_InExtremity( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList, vertexAnim_TempList, indexAnim_TempList );                      
            buildCollision_InExtremity( edgeList, edgeRunList, collisionRunList );
            break;

        case InExtremitySimple:          
            buildFrieze_InExtremitySimple( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList, vertexAnim_TempList, indexAnim_TempList );                      
            buildCollision_InExtremity( edgeList, edgeRunList, collisionRunList );
            break;

        case InArchitecture:
            buildFrieze_InArchi( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList ); 
            orderEdgeRunListByTextureId( edgeRunList );
            buildCollision_InArchi( edgeList, edgeRunList, collisionRunList );
            break;

        case InFluid:
            buildFrieze_InFluid( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList );            
            buildCollision_InFluid( edgeList, edgeRunList, collisionRunList );
            orderEdgeRunListByTextureId( edgeRunList ); // order must be called after collision here
            m_recomputeData.m_isHoleVisual = bfalse;
            break;

        case InString:
            buildFrieze_InString( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList );            
            buildCollision_InString( edgeList, edgeRunList, collisionRunList );
            orderEdgeRunListByTextureId( edgeRunList ); // order must be called after collision here
            break;

        case InPipe:
            buildFrieze_InPipe( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList ); 
            orderEdgeRunListByTextureId( edgeRunList );
            buildCollision_InPipe( edgeList, edgeRunList, collisionRunList );
            break;

        case InAtlas:
            buildFrieze_InAtlas( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList, vertexAnim_TempList, indexAnim_TempList, m_atlasIndex ); 
            initCollisionData(0); // to clear collision data and test if is dynamic frieze
            break;

        case InOverlay:
            {
                // mesh overlay
                MeshOverlay_VertexList* vertexOverlay_TempList = NULL;
                MeshOverlay_IndexList*  indexOverlay_TempList = NULL;

                buildFrieze_InOverlay( edgeList, vertexOverlay_TempList, indexOverlay_TempList);                      
                initCollisionData(0); // to clear collision data and test if is dynamic frieze

                buildMesh_Overlay( vertexOverlay_TempList, indexOverlay_TempList, pMatrixTransfo );
                initOverlay( &config->m_vtxAnim );
            }
            break;

        default:
            ITF_ASSERT_MSG(0, "Unknown method!");
            break;
        }

        m_uvXoffSet = m_recomputeData.m_uvXoffSet;
        m_uvYoffSet = m_recomputeData.m_uvYoffSet;

#ifdef DEVELOPER_JAY_FRIEZE
        debugDrawEdgeRun( edgeRunList );  
        m_recomputeCount++;

        for ( u32 i = 0; i < m_edgeListCount; i++ )
        {
            Vec2d pos = edgeList[i].m_interUp;
            debugDrawInt( transformPos(pos), edgeList[i].m_idPoint, Color::yellow() );
        }
#endif

        m_isFilling = buildFilling( edgeList, edgeRunList, vertexStatic_TempList, indexStatic_TempList );

        switchCollisionsToGlobalSpace();

        //Call plugin right before actual mesh generation. Collision data, and game materials should be available at this point.
        if (m_friezePlugin)
        {
            m_friezePlugin->buildFrieze_static(vertexStatic_TempList, indexStatic_TempList );
        }

        //build meshes
        buildMesh_Anim( vertexAnim_TempList, indexAnim_TempList, pMatrixTransfo );        
        buildMesh_Static( vertexStatic_TempList, indexStatic_TempList, pMatrixTransfo );        

        // desalloc
        SF_DEL_ARRAY(indexAnim_TempList);
        SF_DEL_ARRAY(indexStatic_TempList);   
        if ( pMatrixTransfo )
        {
            Memory::alignedFree(pMatrixTransfo);
        }

        applyColorFactors();

        updateAABB( config);

        const SafeArray<PolyLine*>* pCollisionData = getCollisionData_WorldSpace();
        if ( pCollisionData )
        {
            for (u32 iterPolyline = 0; iterPolyline < pCollisionData->size(); iterPolyline++)
            {
                (*pCollisionData)[iterPolyline]->setDepth(getDepth());
            } 
        }     

        //build visual polyline
        if ( config->m_regionId.isValid() )
        {					
            PolyLine* pPol = (PolyLine*)m_visualPoly.getObject();
			if (!pPol)
			{
                pPol = newAlloc(mId_Frieze,PolyLine());
				for (u32 itPt = 0; itPt < getPosCount(); itPt++)
					pPol->addPoint(transformPos(getPosAt(itPt).to3d()).truncateTo2D());
				m_visualPoly = pPol->getRef();
                pPol->forceRecomputeData();
                pPol->m_ownerFrise = this;
                pPol->setGameMaterial(config->m_gameMaterial.getStringID());
			}
            if (isActive())
            {
                REGIONS_MANAGER->addRegion(m_visualPoly, config->m_regionId, getDepth());
                m_regionRegistered = btrue;
            }
        }

#ifdef  ITF_SUPPORT_EDITOR
		if (!config || !config->m_objectFamily)
		{
			if ( m_pFriezeFullData && m_pFriezeFullData->m_staticCollisionData.size())
				setObjectFamily(ObjectFamily_LevelDesign);
			else
				setObjectFamily(ObjectFamily_Art);
		}
		else
		{
			setObjectFamily(config->m_objectFamily);
		}
#endif //ITF_SUPPORT_EDITOR

        if ( m_pFriezeFullData )
        {
            if (isDynamic())
            {
                for (u32 i = 0; i < m_pFriezeFullData->m_dynamicCollisionData.size(); i++)
                {
                    if (m_pFriezeFullData->m_dynamicCollisionData[i]->m_physBody)
                        m_pFriezeFullData->m_dynamicCollisionData[i]->m_physBody->updateAABB();
                }
            }
            else
            {
                for (u32 i = 0; i < m_pFriezeFullData->m_staticCollisionData.size(); i++)
                {
                    if (m_pFriezeFullData->m_staticCollisionData[i]->m_physBody)
                        m_pFriezeFullData->m_staticCollisionData[i]->m_physBody->updateAABB();
                } 
            }

            udpateCollisionData(0.f);
        }

        getCellEnableData().m_isStatic = bfalse;
        
		if (doRegisterPhysic)
			registerPhysic();

		registerInWorldCells();

        getCellEnableData().m_isStatic = !isDynamic();

        if (!isDynamic())
        {
            if(SubSceneActor* pSSA = getScene()->getSubSceneActorHolder())
            {
                pSSA->growRelativeAABBFromAbsolute(getAABB());
                pSSA->registerStaticFriezeAABB(getAABB());
            }
        }

        m_needUpdate = config && isDynamic();
        m_updateFlags = FriezeUpdateFlag_None;
        switch ( config->m_methode )
        {
        case InFluid:
        case InString:
            m_updateFlags |= FriezeUpdateFlag_NeeedsConfig;
            m_needUpdate = btrue;
            break;
        default:
            break;
        }
        if (!config->m_vtxAnim.m_globalRotSpeed.IsZero())
        {
            m_updateFlags |= FriezeUpdateFlag_NeeedsRotation;
            m_needUpdate = btrue;
        }

        if(m_friezePlugin)
            m_needUpdate = btrue;

        if( m_pMeshAnimData )
            m_needUpdate = btrue;

        if( m_pMeshOverlayData )
            m_needUpdate = btrue;

        m_needUpdateMeshElemFromConfig = 0;
        for (u32 itConf = 0; itConf < config->m_textureConfigs.size(); itConf++)
        {
            FriseTextureConfig& rconf = config->m_textureConfigs[itConf];
            if ( rconf.m_configType == FriseTextureConfig::FriseConfig_Animation)
            {
                AnimTrackResource* animationTextureSource =(AnimTrackResource*) rconf.m_textureData.getResource();
                if (animationTextureSource)
                {
                    m_needUpdateMeshElemFromConfig = 1;
                    break;
                }
            }
        }

        // Compute the aabb once so the cell don't have to do it each frame
        if(!isDynamic())
        {
            const f32 deltaX = CAMERA->getDeltaX();
            const f32 deltaY = CAMERA->getDeltaY();
            getCellEnableData().m_aabb = getAABB();
            getCellEnableData().m_aabb.projectTo2D(deltaX,deltaY, getDepth());
        }
        else
        {
            applyDynamicTransform();
        }

        if ( m_pFriezeFullData && ( m_pFriezeFullData->m_staticCollisionData.size() || m_pFriezeFullData->m_dynamicCollisionData.size() ) )
        {
            getCellEnableData().m_mustBeInitializedWithCell = 1;
        }

        // delete complex data if not used
        if ( m_pFriezeFullData )
        {
            if ( !m_isDynamicFrieze && !isDynamic())
            {
                if ( m_pFriezeFullData->m_staticCollisionsLocalSpace.size() == 0 
                    && m_pFriezeFullData->m_staticCollisionData.size() == 0 
                    && m_pFriezeFullData->m_dynamicCollisionData.size() == 0 )
                {
                    deleteFriezeFullData();
                }
            }
        }
    }

    i32 Frise::getFirstNoFillTextureIndex( const FriseConfig* config ) const
    {
        i32 textureConfigCount = config->m_textureConfigs.size(); 
        for ( i32 i = 0; i < textureConfigCount; i++)
        {
            if ( i != config->m_idTexSwitch && i != config->m_fillFriseTextureConfigIndex && !config->m_textureConfigs[i].getPath().isEmpty() )
                return i;
        }

        return -1;
    }

    void Frise::checkDataConsistency()
    {
        m_recomputeData.m_heightScale = Max( f32_Abs(m_recomputeData.m_heightScale), 0.01f );
        m_recomputeData.m_widthScale = Max( f32_Abs(m_recomputeData.m_widthScale), 0.01f );
        m_recomputeData.m_heightScaleInv = 1.f/m_recomputeData.m_heightScale;

        FriseConfig* config = getConfig();

        if ( !config )
            return;

        if ( config->m_methode >= BuildFriezeCount )
            config->m_methode = InGeneric;
        else if ( config->m_methode == InExtremity || config->m_methode == InExtremitySimple )
        {
            if ( m_pointsList.isLooping() )
            {
                m_recomputeData.m_loopWanted = btrue;
                m_pointsList.setLooping(bfalse);
            }            
        }
        else if ( m_recomputeData.m_loopWanted )
        {
            m_recomputeData.m_loopWanted = bfalse;
            m_pointsList.setLooping(btrue);
        }

        config->m_snapCoeff = Max( 0.f, config->m_snapCoeff );
        config->m_snapCoeffUv = Max( 0.f, config->m_snapCoeffUv );
        config->m_smoothFactorVisual = Clamp( config->m_smoothFactorVisual, 0.f, 10.f );
        config->m_smoothFactorFilling = Clamp( config->m_smoothFactorFilling, 0.f, 10.f);
        config->m_smoothFactorCollision = Clamp( config->m_smoothFactorCollision, 0.f, 10.f);

        config->m_cornerFactor = Max( 1.f, config->m_cornerFactor );
        if ( config->m_cornerFactor >= 1.4f && config->m_cornerFactor <= 1.51f )
            config->m_cornerFactor = 1.41421356f;

        // anims vertexs
        if ( config->m_vtxAnim.m_globalSpeed != 0.f && ( config->m_vtxAnim.m_amplitudeX!=0.f || config->m_vtxAnim.m_amplitudeY!=0.f ) )
        {
            m_recomputeData.m_anim = btrue;
            m_recomputeData.m_animSyncCur = 0.f;
            m_recomputeData.m_animSyncXCur = 0.f;
            m_recomputeData.m_animSyncYCur = 0.f;

            if ( config->m_vtxAnim.m_globalRotSpeed.ToRadians() == 0.f )
                m_animGlobalRotCur.SetRadians(0.f);
        }

        config->m_visualOffset = Clamp( config->m_visualOffset, 0.f, 1.f );        
    }

	void rotateAABB(AABBBuilder& _aabb, const Frise& _frise, bbool _useTranslation)
	{
		Vec3d pts[4];
		pts[0] = _aabb.getMin().to3d();
		pts[1] = _aabb.getMax().to3d();
		pts[2].m_x = _aabb.getMin().m_x;
		pts[2].m_y = _aabb.getMax().m_y;
		pts[2].m_z = 0;
		pts[3].m_x = _aabb.getMax().m_x;
		pts[3].m_y = _aabb.getMin().m_y;
		pts[3].m_z = 0;
		for (u32 i = 0; i < 4; i++)
		{
			pts[i] = _frise.transformPos(pts[i], _useTranslation);
            if(i == 0)
                _aabb.set(pts[i].truncateTo2D());
            else
			    _aabb.grow(pts[i].truncateTo2D());
        }
	}


    void Frise::updateAABB( FriseConfig* _config)
    {
        //  Grow AABB Mesh Anim Vertex        
        if( m_pMeshAnimData )
        {
            f32 amplitudeMax = Max( f32_Abs(_config->m_vtxAnim.m_amplitudeX), f32_Abs(_config->m_vtxAnim.m_amplitudeY));
            Vec2d offset( amplitudeMax, amplitudeMax);

            m_pMeshAnimData->m_AABB.grow( m_pMeshAnimData->m_AABB.getMin() - offset );
            m_pMeshAnimData->m_AABB.grow( m_pMeshAnimData->m_AABB.getMax() + offset );
        }

        // Grow AABB with Z extrude 
        if ( _config->m_zVtxExtrudeUp || _config->m_zVtxExtrudeDown )
        {
            f32 absExtrude = f32_Abs(_config->m_zVtxExtrudeUp) +f32_Abs(_config->m_zVtxExtrudeDown);
            Vec2d extrude( absExtrude, absExtrude );

            // l'extrude ne concerne pas le remplissage
            // ici seul le mesh animé est concerné
            if ( m_pMeshAnimData )
            {
                m_pMeshAnimData->m_AABB.grow( m_pMeshAnimData->m_AABB.getMin() - extrude );
                m_pMeshAnimData->m_AABB.grow( m_pMeshAnimData->m_AABB.getMax() + extrude );
            }
            // pas de mesh animé, s'il y a un mesh static on scale son AABB
            else if ( m_pMeshStaticData )
            {
                m_pMeshStaticData->m_AABB.grow( m_pMeshStaticData->m_AABB.getMin() - extrude );
                m_pMeshStaticData->m_AABB.grow( m_pMeshStaticData->m_AABB.getMax() + extrude );
            }
        }
/*
        AABB relativeAABB = m_pointsList.getAABB();
        rotateAABB(relativeAABB, *this, bfalse);*/
        // get the AABB out of the points
        AABB relativeAABB;
        for (u32 i = 0; i<getPosCount(); i++)
        {
			Vec3d rot = transformPos(getPosAt(i).to3d(), bfalse);
            if(i == 0)
                relativeAABB = rot;
            else
			    relativeAABB.grow(rot);
        }


        if (isDynamic())
        {
            makeDynamicGraphicAABB();
            const bbool applyDynMesh = !m_pFriezeFullData->m_AABB_Mesh_Dynamic.isZero();
            if(applyDynMesh)
            {
                AABB relAABB = m_pFriezeFullData->m_AABB_Mesh_Dynamic;
                relAABB.Translate(-get2DPos());
                relativeAABB.grow(relAABB);
            }

            if(m_pMeshAnimData)
            {
                AABB relAABB = m_pFriezeFullData->m_AABB_Anim_Dynamic;
			    relAABB.Translate(-get2DPos());
                relativeAABB.grow(relAABB);
            }

            if(m_pMeshOverlayData && !m_pFriezeFullData->m_AABB_Overlay_Dynamic.isZero())
            {
                AABB relAABB = m_pFriezeFullData->m_AABB_Overlay_Dynamic;
			    relAABB.Translate(-get2DPos());
                relativeAABB.grow(relAABB);
            }
        }
        else
        {
            const u32 methode = _config->m_methode;
			if (InFluid == methode || InString == methode)
            {
                if ( m_pMeshFluidData && m_pMeshFluidData->m_AABB.isValid())
				    relativeAABB.grow(m_pMeshFluidData->m_AABB.get());
            }
			else
			{
                if ( m_pMeshStaticData && m_pMeshStaticData->m_AABB.isValid())
                {
                    rotateAABB(m_pMeshStaticData->m_AABB, *this, bfalse);
                    relativeAABB.grow(m_pMeshStaticData->m_AABB.get()); 
                }

                if( m_pMeshAnimData && m_pMeshAnimData->m_AABB.isValid())
                {
				    rotateAABB(m_pMeshAnimData->m_AABB, *this, bfalse);
				    relativeAABB.grow(m_pMeshAnimData->m_AABB.get());
                }

                if ( m_pFriezeFullData && m_pFriezeFullData->m_AABB_Collision.isValid())
                {
				    rotateAABB(m_pFriezeFullData->m_AABB_Collision, *this, bfalse);				
				    relativeAABB.grow(m_pFriezeFullData->m_AABB_Collision.get());
                }

                if ( m_pMeshOverlayData && m_pMeshOverlayData->m_AABB.isValid())
                {
                    rotateAABB( m_pMeshOverlayData->m_AABB, *this, bfalse);				
                    relativeAABB.grow( m_pMeshOverlayData->m_AABB.get());
                }
			}
			if (relativeAABB.getWidth() < 1.f || relativeAABB.getHeight() < 1.f )
			{
				relativeAABB.setMin(relativeAABB.getMin() - Vec2d::One);
				relativeAABB.setMax(relativeAABB.getMax() + Vec2d::One);
			}
        }
        changeRelativeAABB(relativeAABB);
    }
} // namespace ITF

