#include "precompiled_engine.h"

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FRIEZESTATS_H_
#include "engine/display/Frieze/FriezeStats.h"
#endif // _ITF_FRIEZESTATS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_FRIEZEPLUGIN_H_
#include "engine/display/Frieze/FriezePlugin.h"
#endif //_ITF_FRIEZEPLUGIN_H_

#ifndef _ITF_FRIEZEPLUGINREGISTRY_H_
#include "engine/display/Frieze/FriezePluginRegistry.h"
#endif //_ITF_FRIEZEPLUGINREGISTRY_H_

#ifndef _ITF_PROFILERMACRO_H_
#include "engine/profiler/profilerMacro.h"
#endif //_ITF_PROFILERMACRO_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_


namespace ITF
{
    SafeArray<Frise::FreeVBData>    Frise::m_VBDataToFree;
    SafeArray<Frise::FreeIBData>    Frise::m_IBDataToFree;
    SafeArray<Frise::FreeGroupData> Frise::m_groupDataToFree;

    
    IMPLEMENT_OBJECT_RTTI(Frise)
    BEGIN_SERIALIZATION_CHILD(Frise)
        SERIALIZE_FUNCTION(preProperties,ESerialize_PropertyEdit);
        
    
///////////////////////////////////////////////////////////////////////////////////////////

        // Serialize new format points (relative to initial pos)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable|ESerialize_Editor)
            SERIALIZE_CONTAINER_OBJECT("LOCAL_POINTS", m_pointsList.getMutableEdgeData());
        END_CONDITION_BLOCK()

        // Serialize old format points (relative to first point)-  To be removed
        BEGIN_CONDITION_BLOCK(ESerialize_Data_Load|ESerialize_Deprecate)
            if (!m_pointsList.getPosCount())
            {
                SERIALIZE_CONTAINER_OBJECT("POINTS", m_pointsList.getMutableEdgeData());
                SERIALIZE_FUNCTION(convertFromOldDataFormat, ESerialize_Data_Load);
            }
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_ENUM_BEGIN("PivotMode",m_pivotMode);
            SERIALIZE_ENUM_END();
            SERIALIZE_MEMBER("LOOP", m_pointsList.m_loop);
            SERIALIZE_MEMBER("USERPIVOT", m_customUserPivot);
        END_CONDITION_BLOCK()


///////////////////////////////////////////////////////////////////////////////////////////

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_MEMBER("ConfigName",m_ConfigName);
            SERIALIZE_MEMBER("SwitchTexturePipeExtremity",m_switchTexturePipeExtremity);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)

            BEGIN_CONDITION_BLOCK(ESerialize_Deprecate)
            f32 r =0.f;
            f32 g =0.f;
            f32 b =0.f;
            f32 a =0.f;
            SERIALIZE_MEMBER("ColorFactor_Red",r);
            SERIALIZE_MEMBER("ColorFactor_Green",g);
            SERIALIZE_MEMBER("ColorFactor_Blu",b);
            SERIALIZE_MEMBER("ColorFactor_Alpha",a);   
            m_colorFactor = Color(a/255.0f, r/255.0f, g/255.0f, b/255.0f);

            SERIALIZE_MEMBER("ColorFog_Red",r);
            SERIALIZE_MEMBER("ColorFog_Green",g);
            SERIALIZE_MEMBER("ColorFog_Blu",b);
            SERIALIZE_MEMBER("fogfactor",a); 
            m_colorFog = Color(a/255.0f, r/255.0f, g/255.0f, b/255.0f);
            END_CONDITION_BLOCK()

            SERIALIZE_MEMBER("ColorFactor", m_colorFactor);
            SERIALIZE_MEMBER("ColorFog", m_colorFog);
            SERIALIZE_MEMBER("ColorComputerTagId", m_colorComputerTagId);
            SERIALIZE_MEMBER("renderInTarget", m_renderInTarget);

            SERIALIZE_MEMBER("UvX_Offset",m_uvXoffSet);
            SERIALIZE_MEMBER("UvY_Offset",m_uvYoffSet);
            SERIALIZE_MEMBER("UvX_Flip",m_uvX_flip);
            SERIALIZE_MEMBER("UvY_Flip",m_uvY_flip);
            SERIALIZE_MEMBER("uvY_Tile",m_uvY_tile);
            SERIALIZE_MEMBER("Filling_OffSetUv",m_fillOffSetUv);
            SERIALIZE_MEMBER("Anim_SyncGlobal",m_animGlobalSync);
            SERIALIZE_MEMBER("IsDynamicFrieze", m_isDynamicFrieze);
            SERIALIZE_MEMBER("AtlasIndex", m_atlasIndex);
            SERIALIZE_MEMBER("Thickness", m_thickness);

            SERIALIZE_ENUM_BEGIN("LockTexture",m_lockTexture);
                SERIALIZE_ENUM_VAR(LockTexture_None);
                SERIALIZE_ENUM_VAR(LockTexture_Top);
                SERIALIZE_ENUM_VAR(LockTexture_Right);
                SERIALIZE_ENUM_VAR(LockTexture_Left);
                SERIALIZE_ENUM_VAR(LockTexture_Bottom);
                SERIALIZE_ENUM_VAR(LockTexture_Switch);
            SERIALIZE_ENUM_END();

        END_CONDITION_BLOCK()

#ifdef ITF_SUPPORT_EDITOR
        BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
            SERIALIZE_MEMBER("ConfigName",m_editConfigName);
        END_CONDITION_BLOCK()

        SERIALIZE_FUNCTION(postloadProperties,ESerialize_PropertyEdit_Load);
#endif
        SERIALIZE_FUNCTION(postDataLoad,ESerialize_Data_Load);


    END_SERIALIZATION()

    ITF_VECTOR<ResourceID> Frise::m_configsUsedThisFrame;
    ITF_VECTOR<ResourceID> Frise::m_configsNeedingAnimDraw;

    DECLARE_RASTER(frize_Update, RasterGroup_Misc, Color::blue());
    DECLARE_RASTER(frize_Draw, RasterGroup_Misc, Color::red());        

    bool PrefetchFcg( const Path& path, PrefetchFactory::Params& params )
    {
        ResourceID  id;
        bbool       alreadyLoaded = bfalse;

        params.type = Resource::ResourceType_FriezeConfig;
        params.refType = PrefetchFactory::Params::Logical;
        {
            csAutoLock cs( RESOURCE_MANAGER->getQueryOpsCs() );

            alreadyLoaded = RESOURCE_MANAGER->getResourceIdFromFile( id, path.getStringID(), Resource::ResourceType_FriezeConfig );
            if ( alreadyLoaded )
            {
                params.res = id.getResource();
                RESOURCE_MANAGER->queryAddUserToResourceLogicalData( id );
                return btrue;
            }
        }
        params.res = Frise::loadFriezeConfig( path, bfalse );
        return btrue;
    }

    void Frise::clearComputedData()
    {
        if ( m_regionRegistered )
        {
            REGIONS_MANAGER->removeRegion(m_visualPoly);
            m_regionRegistered = bfalse;
        }

        clearMeshAnimData();
        clearMeshStaticData();      
        clearCollisionData();
        clearVisualPolyline();
        clearMeshFluidData();
        clearMeshOverlayData();
        deleteFriezeFullData();

        // clear picking triangle
#ifdef ITF_WINDOWS
        EditorWireframeMesh.clear();
#endif
    }

    Frise::~Frise()
    {
        unloadResources();

        FRIEZESTAT_DEL_FRIEZE(getRef());

        RESOURCE_MANAGER->removeUserToResourceLogicalData(m_resourceGroup);

        clearComputedData();

        SF_DEL(m_friezePlugin);
    }

    void Frise::flushGraphicData()
    {
        if ( CHEATMANAGER->getPause() )
            return;

        for (u32 i = 0; i < m_VBDataToFree.size();)
        {
            if (m_VBDataToFree[i].m_timeToGo <= 0)
            {
                FRIEZESTAT_DEL_VB(m_VBDataToFree[i].m_buffer);
                GFX_ADAPTER->removeVertexBuffer(m_VBDataToFree[i].m_buffer);
                m_VBDataToFree.eraseNoOrder(i);
            }
            else
            {
                m_VBDataToFree[i].m_timeToGo--;
                i++;
            }
        }

        for (u32 i = 0; i < m_IBDataToFree.size();)
        {
            if (m_IBDataToFree[i].m_timeToGo <= 0)
            {
                GFX_ADAPTER->removeIndexBuffer(m_IBDataToFree[i].m_buffer);
                m_IBDataToFree.eraseNoOrder(i);
            }
            else
            {
                m_IBDataToFree[i].m_timeToGo--;
                i++;
            }
        }

        for (u32 i = 0; i < m_groupDataToFree.size();)
        {
            if (m_groupDataToFree[i].m_timeToGo <= 0)
            {               
                u32 physCount = m_groupDataToFree[i].m_group->getPhysicalUsersCount();
                for (u32 removePhys = 0; removePhys < physCount; removePhys++)
                    RESOURCE_MANAGER->queryRemoveUserToResourcePhysicalData(m_groupDataToFree[i].m_group);

                m_groupDataToFree[i].m_group->clearAll();

                RESOURCE_MANAGER->queryRemoveUserToResourceLogicalData(m_groupDataToFree[i].m_group);
                m_groupDataToFree.eraseNoOrder(i);
            }
            else
            {
                m_groupDataToFree[i].m_timeToGo--;
                i++;
            }
        }
    }

    void Frise::FreeVB( class ITF_VertexBuffer* _buffer   )
    {
        FreeVBData& data = m_VBDataToFree.incrSize();
        data.m_timeToGo = 3;
        data.m_buffer = _buffer;
    }
    
    void Frise::FreeIB( class ITF_IndexBuffer* _buffer  )
    {
        FreeIBData& data = m_IBDataToFree.incrSize();
        data.m_timeToGo = 3;
        data.m_buffer = _buffer;
    }

    void Frise::freeResourceGroup( ResourceGroup* _group)
    {
        ResourceID resourceID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

        FreeGroupData& data = m_groupDataToFree.incrSize();
        data.m_timeToGo = 3;
        data.m_group = (ResourceGroup*)resourceID.getResource();

        const ResourceGroup::ResourceList& list = _group->getResourcesList();
        u32 listCount = list.size();
        for ( u32 i=0; i<listCount; i++)
        {
            data.m_group->addResource( list[i].getResource());
        }
         
         RESOURCE_MANAGER->queryAddUserToResourcePhysicalData(data.m_group);
         RESOURCE_MANAGER->flushPendingOps();
    }

    void Frise::setTaggedForDeletion()
    {
        Super::setTaggedForDeletion();
        m_querriedRecomputeData = bfalse;
    }

    Frise::Frise() 
        : Pickable()        
        , m_friezePlugin(NULL)
        , m_timeCur(0.f)
        , m_atlasIndex(0)

        // uv
        , m_uvX_flip(bfalse)
        , m_uvY_flip(bfalse)
        , m_uvY_tile(1.f)
        , m_uvXoffSet(0.f)
        , m_uvYoffSet(0.f)

        //  Fill
        , m_fillOffSetUv(Vec2d::Zero)

        // color
        , m_colorFog(0.f, Color::white().m_r, Color::white().m_g, Color::white().m_b)
        , m_colorFactor(Color::white())
        
        , m_colorComputerTagId(0)
        , m_renderInTarget(0)

        , m_LightColorComputed(Color::white())

        // vtx
        , m_thickness(1.f)

        //dynamics
        , m_isDynamicFrieze(bfalse)
        , m_bIsDynamic(bfalse)
        , m_realTimeLighting(bfalse)
        , m_lightComputed(bfalse)

        // line data
        ,m_querriedRecomputeData(btrue)

        , m_switchTexturePipeExtremity(0)
        , m_lockTexture(LockTexture_None)

#ifdef OPTIM_COMPUTE_LIGHT
		// Philippe Optims wii
		, m_ComputeLightCounter(0)
#endif // OPTIM_COMPUTE_LIGHT

        // mesh data
        , m_pFriezeFullData(NULL)
        , m_pMeshOverlayData(NULL)
        , m_pMeshAnimData(NULL)
        , m_pMeshStaticData(NULL)
        , m_pMeshFluidData(NULL)

#ifdef SKIP_NODRAW_UPDATE
        , m_skippedDt(0.f)
#endif // SKIP_NODRAW_UPDATE
    {
        m_needUpdateMeshElemFromConfig = 0;
        m_updateFlags = FriezeUpdateFlag_None;
        m_globalAlphaFade = 1.f;

        m_collisionInGlobalSpace = bfalse;        

        setObjectType(BaseObject::eFrise);
        FRIEZESTAT_ADD_FRIEZE(getRef());

        m_ConfigName            = String();
        m_regionRegistered = bfalse;

        // animation vertex
        m_animGlobalRotCur.SetRadians(0.f);
        m_animGlobalSync = Seeder::getSharedSeeder().GetFloat( 0.f, MTH_2PI );

        // Create the resource group for the friseconfig
        m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);

        // visual
        m_visualPoly.invalidate();     
        
        m_pivotMode = PivotMode_Center;
        setPhysicalReady(bfalse);
        m_disablePhysic = bfalse;
        m_physicRegistered = bfalse;
        m_needUpdate = bfalse;

        setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);

#ifdef DEVELOPER_JAY_FRIEZE
m_recomputeCount = 0;
#endif
    }

    void Frise::FriezeFullData::init()
    {
        m_dynamicTransform = GMatrixIdentity;
        m_AABB_Collision.reset();
        m_AABB_Mesh_Dynamic = Vec2d::Zero;
        m_AABB_Anim_Dynamic = Vec2d::Zero;    
        m_AABB_Overlay_Dynamic = Vec2d::Zero;
    }

    void Frise::createFriezeFullData()
    {
        m_pFriezeFullData = newAlloc(mId_Frieze, FriezeFullData);
        m_pFriezeFullData->init();        
    }

    void Frise::deleteFriezeFullData()
    {
        SF_DEL(m_pFriezeFullData);
    }

    void Frise::unloadResources()
    {
        Pickable::unloadResources(); 
//        m_physicalReady = btrue;
    }


    void Frise::offsetCollisionData(PolyLine* pol, const Vec2d& _offset)
    {
        Matrix44 transf = getTransformMatrix(btrue);
        for (u32 i = 0; i < pol->getPosCount(); i++)
            pol->setPosAt(transformPos(transf, pol->getPosAt(i)), i);
    }

    void Frise::onLoaded(HotReloadType _hotReload)
    {
        m_disablePhysic = bfalse;
        m_globalAlphaFade = 1.f;

        if (PivotMode_Invalid == m_pivotMode)
            m_pivotMode = PivotMode_Center;

        setFriseConfig(m_ConfigName);

		if(!isUpdateTypeFromTemplateOverridden())
            if(const FriseConfig* fcfg = getConfig())
                setUpdateType(fcfg->m_updateType);

        Super::onLoaded(_hotReload);

        if (HotReloadType_Checkpoint != _hotReload || isDynamic())
        {
            setPos(getWorldInitialPos());
            setAngle(getWorldInitialRot());
        }

        if (HotReloadType_Checkpoint != _hotReload)
            forceRecomputeData();


        if (isDynamic())
        {
            setDynamicTransform(GMatrixIdentity);
            applyDynamicTransform();
        }
        applyColorFactors();
        updatePhysicalReady();

        if( m_pMeshFluidData )
            m_pMeshFluidData->m_data.resetAllLayerSettings();
    }

    void Frise::setDynamicTransform(const GMatrix44 &_m)
    {
        ITF_WARNING(this, isDynamic(), "trying to apply a dynamic transform to a non-dynamic frieze");
        m_pFriezeFullData->m_dynamicTransform = _m;
    }

    Matrix44 Frise::getTransformMatrix( bbool _useTranslation /*= btrue*/ ) const
    {
        Matrix44 transfo;
        if (isDynamic())
            computeDynamicDisplayMatrix(transfo);
        else
            computeStaticDisplayMatrix(transfo);

        if (!_useTranslation)
        {
            transfo.fa16[12] = 0.f;
            transfo.fa16[13] = 0.f;
            transfo.fa16[14] = 0.f;
        }

        return transfo;
    }

    Vec3d Frise::transformPos( const Matrix44& _matrix, const Vec3d& _pos ) const
    {
        Vec3d ret;
        M44_matrixCoordTransform(&ret, &_matrix, &_pos);
        return ret;
    }

    Vec2d Frise::transformPos( const Matrix44& _matrix, const Vec2d& _pos ) const
    {
        Vec2d ret;
        M44_matrixCoordTransform(&ret, &_matrix, &_pos);
        return ret;
    }

    Vec3d Frise::transformPos(const Vec3d& _pos, bbool _useTranslation) const
    {
        return transformPos(getTransformMatrix(_useTranslation), _pos);
    }

    Vec2d Frise::transformPos(const Vec2d& _pos, bbool _useTranslation) const
    {       
        Vec2d ret;
        Matrix44 transf = getTransformMatrix(_useTranslation);
        M44_matrixCoordTransform(&ret, &transf, &_pos);
        return ret;        
    }

    void Frise::computeStaticDisplayMatrix(Matrix44& _dest) const
    {
        M44_setMatrixRotationZ(&_dest, getAngle());
        _dest.fa16[12] = getPos().m_x;
        _dest.fa16[13] = getPos().m_y;
        _dest.fa16[14] = getPos().m_z;
    }

    void Frise::computeDynamicDisplayMatrix(Matrix44& _dest) const
    {
        ITF_ASSERT(isDynamic());

        Matrix44 dynamic(m_pFriezeFullData->m_dynamicTransform);
        Matrix44 staticMat;
        computeStaticDisplayMatrix(staticMat);
        M44_mul4x4Out(&_dest, &staticMat, &dynamic);
    }

    void Frise::applyDynamicTransform()
    {
        ITF_ASSERT(isDynamic());
        Matrix44 mtx;
        computeDynamicDisplayMatrix(mtx);

        if ( m_pMeshStaticData )
            m_pMeshStaticData->m_mesh.setMatrix(mtx);  

        if ( m_pMeshAnimData )
            m_pMeshAnimData->m_mesh.setMatrix(mtx);

        if ( m_pMeshOverlayData )
            m_pMeshOverlayData->m_mesh.setMatrix(mtx);

        makeDynamicCollisionsFromStatic();
        makeDynamicGraphicAABB();

        bbool fromAbsolute = bfalse;
        if(!m_pFriezeFullData->m_AABB_Mesh_Dynamic.isZero())
        {
            m_absoluteAABB = m_pFriezeFullData->m_AABB_Mesh_Dynamic;
            fromAbsolute = btrue;
        }

        if ( m_pMeshAnimData )
        {
            m_absoluteAABB.grow(m_pFriezeFullData->m_AABB_Anim_Dynamic);
            fromAbsolute = btrue;
        }

        //m_absoluteAABB.grow(m_pFriezeComplexDatas->m_AABB_Collision); collision not yet ok
        //m_absoluteAABB.grow(m_AABB_Fluid); fluids are not updated, they would embed world's origin there

        if(fromAbsolute)
            changeRelativeAABBFromAbsolute(m_absoluteAABB);
        else
        {
            if(m_pFriezeFullData->m_staticCollisionsLocalSpace.size())
            {
                AABB aabb;
                for(u32 i = 0; i < m_pFriezeFullData->m_staticCollisionsLocalSpace.size(); ++i)
                {
                    const AABB& relativeAABB = m_pFriezeFullData->m_staticCollisionsLocalSpace[i].getAABB();

                    if(i == 0)
                    {
                        aabb = relativeAABB;
                    }
                    else
                    {
                        aabb.grow(relativeAABB);
                    }
                }
                changeRelativeAABB(aabb);
            }
        }
    }
    
    void Frise::makeDynamicGraphicAABB()
    {
        Matrix44 m;
        ITF_ASSERT(isDynamic());
        Matrix44 dynamicTransform44;
        computeDynamicDisplayMatrix(dynamicTransform44);

        if ( m_pMeshStaticData && m_pMeshStaticData->m_AABB.isValid())
            computeDynamicAABB( m_pMeshStaticData->m_AABB.get(), m_pFriezeFullData->m_AABB_Mesh_Dynamic, dynamicTransform44);

        if ( m_pMeshAnimData && m_pMeshAnimData->m_AABB.isValid())
            computeDynamicAABB( m_pMeshAnimData->m_AABB.get(), m_pFriezeFullData->m_AABB_Anim_Dynamic, dynamicTransform44);

        if ( m_pMeshOverlayData && m_pMeshOverlayData->m_AABB.isValid())
            computeDynamicAABB( m_pMeshOverlayData->m_AABB.get(), m_pFriezeFullData->m_AABB_Overlay_Dynamic, dynamicTransform44);
    }

    void Frise::computeDynamicAABB(const AABB &_src, AABB &_dst, const Matrix44 &_m)
    {
        Float4 v;
        Float4 vt1;
        Float4 vt2;
        Float4 vt3;
        Float4 vt4;

        f32 depth = getDepth();
        setFloat4(v, _src.getMin().m_x, _src.getMin().m_y, depth, 1);
        M44_matrixCoordTransform(&vt1, &_m, &v);
        setFloat4(v, _src.getMax().m_x, _src.getMax().m_y, depth, 1);
        M44_matrixCoordTransform(&vt2, &_m, &v);
        setFloat4(v, _src.getMinXMaxY().m_x, _src.getMinXMaxY().m_y, depth, 1);
        M44_matrixCoordTransform(&vt3, &_m, &v);
        setFloat4(v, _src.getMaxXMinY().m_x, _src.getMaxXMinY().m_y, depth, 1);
        M44_matrixCoordTransform(&vt4, &_m, &v);
        Float4 vmin = min4(min4(vt1, vt2), min4(vt3,vt4));
        Float4 vmax = max4(max4(vt1, vt2), max4(vt3,vt4));

        Vec2d &vMin2 = (Vec2d&)vmin;
        Vec2d &vMax2 = (Vec2d&)vmax;

        _dst.setMin(vMin2);
        _dst.setMax(vMax2);
    }

    FriseConfig* Frise::loadFriezeConfig(const Path& _path, bbool _reload)
    {
        // Editor exception, hot reload, etc...
        /*
        ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_SceneManager].m_threadID, "PERFORMANCE ISSUE : Function should be called from LOADING thread NOT MAIN");
        */
        PathString_t pathName;
        FriseConfig* pConfig = NULL;
        File* pFile = NULL;

        _path.getString(pathName);

        // Initialize values (and delete already existing data)

        bbool allowedToUseCook = btrue;//by default we use the cook file,on win we check if the fcg is present

#if defined(ITF_SUPPORT_COOKING)
        if (RESOURCE_MANAGER)
        {
            bbool bRes = RESOURCE_MANAGER->cookNeeded(pathName);
            if (bRes)
                RESOURCE_MANAGER->requestCooking(pathName);
        }

        if (!FILEMANAGER->fileExists(pathName))
        {
            allowedToUseCook = bfalse;
        }
#endif  // ITF_SUPPORT_COOKING
       
        if (allowedToUseCook)
        {
            const String filename = FILESERVER->getCookedName(pathName);    
            pFile = FILEMANAGER->openFile(filename,ITF_FILE_ATTR_READ);
            ITF_ASSERT(pFile);
        }
            
        //on mode win32,we must check if the original exist 
        
        if(pFile)
        {
            ResourceID ID;

            // Create the config from its path (or retrieve the already existing one)
            if ( RESOURCE_MANAGER->getResourceIdFromFile(ID,_path.getStringID(), Resource::ResourceType_FriezeConfig) )
            {
                pConfig = static_cast<FriseConfig*>(ID.getResource());
            }
            else
            {
                ITF_ASSERT(!_reload);
                ID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_FriezeConfig, _path);
                pConfig = static_cast<FriseConfig*>(ID.getResource());
            }
            ITF_ASSERT(pConfig);

            pConfig->startSerialization_load();

            const u32 length = (u32) pFile->getLength();

            ArchiveMemory rd(length,length,btrue);
            pFile->read(rd.getPtrForWrite(),length);
            rd.rewindForReading();
            pConfig->serialize(rd);
            FILEMANAGER->closeFile(pFile);

            if (_reload)
            {
                RESOURCE_MANAGER->flushPendingOps();
                ResourceGroup* pGrp = pConfig->getResourceGroup();
                for (u32 iWorld = 0; iWorld < WORLD_MANAGER->getWorldCount(); iWorld++)
                {
                    World* curWorld = WORLD_MANAGER->getWorldAt(iWorld);
                    if (!curWorld)
                        continue;

                    for (u32 iScene = 0; iScene < curWorld->getSceneCount(); iScene++)
                    {
                        Scene* curScene = curWorld->getSceneAt(iScene);
                        if (!curScene)
                            continue;
                        
                        const PickableList&  friezes = curScene->getFrises();
                        for (u32 itFrieze = 0; itFrieze < friezes.size(); itFrieze++)
                        {
                            Frise* pFrieze = static_cast<Frise*>(friezes[itFrieze]);
                            if (pFrieze->getConfig() == pConfig)
                            {

#ifdef  ITF_SUPPORT_EDITOR
                                pFrieze->resetResourceError();
#endif //ITF_SUPPORT_EDITOR
                                pFrieze->resetDataError();
                                if (pGrp && pFrieze->isRequestedLoading())
                                    RESOURCE_MANAGER->queryAddUserToResourcePhysicalData(pGrp);
                                pFrieze->setFriseConfig(_path);
                                pFrieze->recomputeData();
                            }
                        }
                    }
                }
            }
        }
        else
        {
            String8 msg("Missing file: ");
            PathString8_t pathFilename;
            _path.getString(pathFilename);
            msg += pathFilename;
            //msg += "\nTechno may crash or try to load forever if this file is mandatory";
            ITF_WARNING_CATEGORY(Graph,NULL, NULL, msg.cStr());
        }

        if (pConfig)
            pConfig->hijackPhysicalLoaded(btrue);  // pretend load() was called without changing physical users

        return pConfig;
    }

    void Frise::udpateCollisionData(f32 _elapsed)
    {
        if ( m_pFriezeFullData )
        {
            for (u32 i = 0; i < m_pFriezeFullData->m_dynamicCollisionData.size(); i++)
                m_pFriezeFullData->m_dynamicCollisionData[i]->update(_elapsed);
            for (u32 i = 0; i < m_pFriezeFullData->m_staticCollisionData.size(); i++)
                m_pFriezeFullData->m_staticCollisionData[i]->update(_elapsed);
        }
    }
    
    u32 Frise::addObjectInGroupForUpdate( ObjectGroup* _listToFill, bbool _fromParent )
    {
        if (isTaggedForDeletion())
            return getObjectGroupIndex();

        if(m_needUpdate || m_querriedRecomputeData || m_binding.hasChildren())
            return Super::addObjectInGroupForUpdate(_listToFill, _fromParent);

        return getObjectGroupIndex();
    }

    void Frise::update(f32 _elapsed)
    {
#ifndef ITF_FINAL
        if (hasDataError())
            return;
#endif
        PROFILER_UPDATE_PICKABLE(this,getRef());

        TIMERASTER_SCOPE(frize_Update);

        STATS_FRIEZE_UPDATEINC

#ifdef SKIP_NODRAW_UPDATE
        if(CURRENTFRAME - getLastVisibleFrame() > 1)
        {
            if(m_skippedDt < SKIP_NODRAW_UPDATE)
            {
                m_skippedDt += _elapsed;
                return;
            }
        }
        _elapsed += m_skippedDt;
        m_skippedDt = 0.f;
#endif // SKIP_NODRAW_UPDATE

        if (isDynamic())
            applyDynamicTransform();

        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        if (m_updateFlags & FriezeUpdateFlag_NeeedsConfig)
        {
            switch ( config->m_methode )
            {
            case InFluid :
                {
                    ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

                    if ( m_pMeshFluidData )
                    {
                        m_pMeshFluidData->m_data.ProcessEdgeFluidList_InFluid(_elapsed);
                    }
                    break;
                }
            case InString:
                {
                    ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

                    if ( m_pMeshFluidData )
                    {
                        m_pMeshFluidData->m_data.ProcessEdgeFluidList_InString(_elapsed);
                    }
                    break;
                }
                break;
            default:
                break;
            }
        }

        if (m_querriedRecomputeData)
            executeRecomputeData();

        if (!getCellEnableData().m_isStatic)
            udpateCollisionData(_elapsed);

        m_timeCur += _elapsed;
        if (m_updateFlags & FriezeUpdateFlag_NeeedsRotation)
        {
            m_animGlobalRotCur += config->m_vtxAnim.m_globalRotSpeed;
            m_animGlobalRotCur.Snap();
        }
        
        m_binding.updateChildren(_elapsed);

        //frieze plugin : it is important to execute this near the end of the update. See it as a child class which calls its Super at 1st
        if (m_friezePlugin!=NULL)
        {
            m_friezePlugin->update(_elapsed);
        }

// #ifdef DEVELOPER_JAY_FRIEZE
//         debugDrawInt( getAABB().getCenter(), m_recomputeCount, Color::red() );
// #endif
    }

    void Frise::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        for (u32 i = 0; i < m_eventListeners.size(); ++i)
        {
            Actor * actor = ActorRef(m_eventListeners[i]).getActor();
            if(actor)
            {
                actor->onEvent(_event);
            }
        }

        if ( EventDisableCollision* collision = _event->DynamicCast<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
        {
            m_disablePhysic = collision->getDisable();
            if ( m_disablePhysic )
            {
                if (m_physicRegistered)
                {
                    unregisterPhysic();
                }
            }
            else // restore
            {
                if (!m_physicRegistered && isActive())
                {
                    registerPhysic();
                }
            }
            return;
        }

        FriseConfig* config = getConfig();
        if ( !config )
            return;

        switch ( config->m_methode )
        {
        case InFluid:
        case InString:

            ITF_ASSERT_MSG( m_pMeshFluidData, "No data fluid");

            if ( !m_pMeshFluidData )
                break;

            if (EventStickOnPolyline * stickOnPolyEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
            {
                //case : cushion
                ITF_ASSERT(m_pMeshFluidData->m_data.isCushion());
                if (stickOnPolyEvent->isSticked())
                {
                    Actor * act = (Actor *)stickOnPolyEvent->getActor().getObject();
                    if (act)
                        m_pMeshFluidData->m_data.addActor(stickOnPolyEvent->getActor(), Vec2d::Zero, act->getRadius(), 1.f, stickOnPolyEvent->getWeight(), bfalse );
                }
                else
                    m_pMeshFluidData->m_data.removeActor(stickOnPolyEvent->getActor(), Vec2d::Zero);
            } else if (EventSwiming * swiming = _event->DynamicCast<EventSwiming>(ITF_GET_STRINGID_CRC(EventSwiming,2469288979)))
            {
                if (swiming->isSwiming())
                    m_pMeshFluidData->m_data.addActor(swiming->getActor(), swiming->getSpeed(), swiming->getRadius(), swiming->getWaterMultiplier(), swiming->getWeight(), swiming->getQueryPosition() );
                else
                    m_pMeshFluidData->m_data.removeActor(swiming->getActor(), swiming->getSpeed());
            } else if (EventSwimingPlouf * plouf = _event->DynamicCast<EventSwimingPlouf>(ITF_GET_STRINGID_CRC(EventSwimingPlouf,3882552276)))
            {
                ITF_ASSERT(!m_pMeshFluidData->m_data.isCushion());
                f32 fakeMass=1;
                m_pMeshFluidData->m_data.sendPerturbation(plouf->getPos(), plouf->getAlteration(), plouf->getRadius(), plouf->isRandom(), fakeMass);
            }
            else if (EventOnLinkedToWaveGenerator *linkedToWaveGenerator=_event->DynamicCast<EventOnLinkedToWaveGenerator>(ITF_GET_STRINGID_CRC(EventOnLinkedToWaveGenerator,3839654009)))
            {
                if (linkedToWaveGenerator->getIsLinked())
                {
                    m_pMeshFluidData->m_data.addStringWaveGenerator(linkedToWaveGenerator->getWaveGenerator());
                }
                else
                {
                    m_pMeshFluidData->m_data.removeStringWaveGenerator(linkedToWaveGenerator->getWaveGenerator());
                }
            }
            break;
        default:
            break;
        }

        //Frieze plugin. Generic policy is to call at end of function
        if (m_friezePlugin)
        {
            m_friezePlugin->onEvent(_event);
        }
    }

    void Frise::requestDestruction()
    {
        Scene *scene = getScene();
        ITF_ASSERT(scene);
        if (scene)
        {
            scene->unregisterPickable(this);
        }
    }

    void Frise::clearVisualPolyline()
    {
        PolyLine *poly = (PolyLine *)m_visualPoly.getObject();
        SF_DEL(poly);

        m_visualPoly.invalidate();
    }

    Vec3d Frise::getAnchorPos() const
    {
        Vec2d result;
        AABB baseAABB = getPointsData().getAABB();
/*
        if (isDynamic())
            baseAABB = m_relativeAABB;
*/
        switch (m_pivotMode)
        {
            case PivotMode_Center:
                result = baseAABB.getCenter();//m_relativeAABB.getCenter().m_x;
            break;
            case PivotMode_TopLeft:
                result = baseAABB.getMin();
            break;
            case PivotMode_TopRight:
                result = baseAABB.getMax();
            break;
            case PivotMode_BottomLeft:
                result = baseAABB.getMin();
            break;
            case PivotMode_BottomRight:
                result = baseAABB.getMax();
            break;
            case PivotMode_UserCustom:
                result = m_customUserPivot;
            break;

            default:
                result = Vec2d::Zero;
                ITF_ASSERT_MSG(bfalse, "bad pivot mode");
            break;
        }

        Vec3d result3d = transformPos(result.to3d());
        return result3d;
    }

    void Frise::draw()
    {        
        PROFILER_DRAW_PICKABLE(this,getRef());
        TIMERASTER_SCOPE(frize_Draw);

        renderFrieze();
    }

    void Frise::applyColorFactors()
    {
        setGlobalColor(m_colorFactor);
    }

    void Frise::computeFinalColor()
    {
        // process Lightning.
#ifdef OPTIM_COMPUTE_LIGHT
        if(m_ComputeLightCounter == 0)
        {
#endif // OPTIM_COMPUTE_LIGHT
            lightProcess();
#ifdef OPTIM_COMPUTE_LIGHT
            m_ComputeLightCounter = 8;
        }
        m_ComputeLightCounter--;
#endif // OPTIM_COMPUTE_LIGHT

        f32 alpha = m_globalAlphaFade * m_colorFactor.m_a;
        Color color( alpha, m_colorFactor.m_r * m_LightColorComputed.getRed(), m_colorFactor.m_g * m_LightColorComputed.getGreen(), m_colorFactor.m_b * m_LightColorComputed.getBlue() );
        setGlobalColorToMesh(color);

        // fog.
        setFog(m_colorFog);
    }

    void Frise::setColorFactor(const Color& _color)
    {
        m_colorFactor = _color;
        setGlobalColor(_color);
    }

    void Frise::setColorFog(const Color& _color)
    {
        m_colorFog = _color;
        setFog(_color);
    }

    void Frise::lightProcess()
    {
        u32 numberLight = m_lights.size();

        f32 nl = 0.f;
        if (numberLight)
        {
            m_LightColorComputed = Color::zero();

            for (u32 i = 0; i < numberLight; i++)
            {
                LightInfo* light = &m_lights[i];
                f32 sqLightDist = light->m_radFar * light->m_radFar;
                f32 sqDist;
                if (light->m_useBV)
                {
                    f32 distx = getAABB().getDistMinPosAABB(light->m_position.truncateTo2D());
                    f32 disty = getDepth() - light->m_position.m_z;
                    sqDist = Vec2d(distx, disty).sqrnorm();
                }
                else
                {
                    Vec3d FrizePos = Vec3d(getAABB().getCenter().m_x, getAABB().getCenter().m_y, getDepth());
                    sqDist = Vec3d(light->m_position - FrizePos).sqrnorm();
                }

                if (sqDist < sqLightDist)
                {
                    switch (light->m_type)
                    {
                    case LightInfo::AMBIENT:
                        m_LightColorComputed += light->m_color;
                        break;
                    case LightInfo::LINEAR:
                        if (sqDist < (light->m_radNear*light->m_radNear))
                        {
                            m_LightColorComputed += light->m_color;
                            nl = 1.f;
                        }
                        else
                        {
                            sqDist = f32_Sqrt(sqDist);
                            f32 lerp =  1.f - (sqDist - light->m_radNear) / (light->m_radFar - light->m_radNear);
                            nl += lerp;
                            m_LightColorComputed += light->m_color * lerp;
                        }
                        break;
                    }
                }
            }

            if (nl > 1.f)
                nl = 1.f;

            //lerp final
            Color one(1.f, 1.f, 1.f, 1.f);
            Color co = Color::colorBlend(&m_LightColorComputed, &one, nl);
            m_LightColorComputed = co;

            FriseConfig* config = getConfig();
            if (config)
            {
                Color illum(config->m_selfIllumColor.getAsU32());
                m_LightColorComputed += illum;
            }
        }
        else
            m_LightColorComputed = Color::white();

        m_LightColorComputed.setAlpha(1.f);
        m_LightColorComputed.ClampColor();

        m_lights.clear();
        m_lightComputed = btrue;
    }

    void Frise::addLight(const LightInfo &_light)
    {
        m_lights.push_back(_light);
    }

    void Frise::setGlobalColor(const Color& _color)
    {  
        m_colorFactor = _color;
        setGlobalColorToMesh(_color);
    }

    void Frise::setGlobalColorToMesh(const Color& _color)
    {  
        if ( m_pMeshStaticData )
            m_pMeshStaticData->m_mesh.m_globalColor = _color;

        if ( m_pMeshAnimData )
            m_pMeshAnimData->m_mesh.m_globalColor = _color;

        if ( m_pMeshOverlayData )
            m_pMeshOverlayData->m_mesh.m_globalColor = _color;
    }

    void Frise::setFog(const Color& _color)
    {  
        m_colorFog = _color;

        if ( m_pMeshStaticData )
            m_pMeshStaticData->m_mesh.m_colorFog = _color;

        if ( m_pMeshAnimData )
            m_pMeshAnimData->m_mesh.m_colorFog = _color;

        if ( m_pMeshOverlayData )
            m_pMeshOverlayData->m_mesh.m_colorFog = _color;
    }

    void Frise::updateMeshElemFromConfig(ITF_Mesh* _pmesh, SafeArray<u32>& _elemconfigIndexlist)
    {
        FriseConfig* config = getConfig();
        if ( !config )
            return;
        
        for (u32 j = 0; j < _pmesh->m_ElementList.size(); j++)
        {
            u32 index = _elemconfigIndexlist[j];
            FriseTextureConfig& rconf = config->m_textureConfigs[index];

            if ( rconf.m_configType == FriseTextureConfig::FriseConfig_Animation)
            {
                AnimTrackResource* animationTextureSource =(AnimTrackResource*) rconf.m_textureData.getResource();
                ResourceID friseTexture;

                if (animationTextureSource)
                {
                    if (animationTextureSource->getPhysicalUsersCount() == 0)
                        RESOURCE_MANAGER->addUserToResourcePhysicalData(animationTextureSource);

                    if (rconf.mp_animMeshScene)
                    {
                        AnimTrack* pTrack = animationTextureSource->getTrack();
                        if ( pTrack && rconf.mp_animMeshScene->m_pAnimDraw.getRenderTargetImpostor())
                        {
                            Texture *tex = rconf.mp_animMeshScene->m_pAnimDraw.getRenderTargetImpostor()->getTexture();
                            if (tex)
                                friseTexture = tex->getID();
                        }
                    }
                }
                _pmesh->m_ElementList[j].m_material.m_textureDiffuse = friseTexture;
            }
        }
    }

    void  Frise::getStaticMeshAABB(AABB& _dest)
    {
        if (isDynamic())
            _dest = m_pFriezeFullData->m_AABB_Mesh_Dynamic;
        else
        {
            _dest = m_pMeshStaticData->m_AABB.get();
            _dest.Translate(get2DPos());
        }
    }

    void  Frise::getAnimMeshAABB(AABB& _dest)
    {
        if (isDynamic())
            _dest = m_pFriezeFullData->m_AABB_Anim_Dynamic;
        else
        {
            _dest = m_pMeshAnimData->m_AABB.get();
            _dest.Translate(get2DPos());
        }
    }

    void  Frise::getOverlayMeshAABB(AABB& _dest)
    {
        if (isDynamic())
            _dest = m_pFriezeFullData->m_AABB_Overlay_Dynamic;
        else
        {
            _dest = m_pMeshOverlayData->m_AABB.get();
            _dest.Translate(get2DPos());
        }
    }

    void Frise::renderFrieze( )
    {
#ifndef ITF_FINAL
        if (hasDataError())
            return;
#endif
        if (isTaggedForDeletion())
            return;

        const FriseConfig* config = getConfig();
        if ( !config )
            return;

        if (m_globalAlphaFade<0.001f)
            return;

        bbool configvisible = bfalse;

        AABB aabb;
        
        //  mesh static
        if( m_pMeshStaticData )
        {
            getStaticMeshAABB(aabb);
            if ( CAMERA->isRectVisible_NotInlined(aabb, getDepth()) )
            {
                if (m_needUpdateMeshElemFromConfig)
                    updateMeshElemFromConfig( &m_pMeshStaticData->m_mesh, m_pMeshStaticData->m_elemConfig_IndexList);

                GFX_ADAPTER->addPrimitiveInZList((void*)&m_pMeshStaticData->m_mesh, GFX_ITF_MESH, NULL, getDepth(), m_renderInTarget,getRef());
                configvisible = btrue;
            }
        }

        // mesh anim
        if ( m_pMeshAnimData )
        {
            getAnimMeshAABB(aabb);
            if ( CAMERA->isRectVisible_NotInlined(aabb, getDepth()) )
            {
				{
					if (m_needUpdateMeshElemFromConfig)
						updateMeshElemFromConfig( &m_pMeshAnimData->m_mesh, m_pMeshAnimData->m_elemConfig_IndexList);
                    
                    ITF_ASSERT_CRASH( m_pMeshAnimData->m_mesh.m_ElementList.size() == 1, "No element in this mesh anim");

                    m_pMeshAnimData->m_mesh.m_ElementList[0].m_material.m_matParams.setvParamsAt(0, getvParamsMeshAnim( &config->m_vtxAnim ));
				}

                f32 zpos = getDepth() +0.0001f;
                GFX_ADAPTER->addPrimitiveInZList((void*)&m_pMeshAnimData->m_mesh, GFX_ITF_MESH, NULL, zpos, m_renderInTarget ,getRef());
                configvisible = btrue;
            }
        }
        
        // mesh overlay
        if ( m_pMeshOverlayData )
        {
            getOverlayMeshAABB(aabb);

            if ( CAMERA->isRectVisible_NotInlined(aabb, getDepth()) )
            {
                setMeshOverlayDataMatrix( &config->m_vtxAnim );                
                GFX_ADAPTER->addPrimitiveInZList((void*)&m_pMeshOverlayData->m_mesh, GFX_ITF_MESH, NULL, getDepth(), m_renderInTarget ,getRef());
                configvisible = btrue;
            }
        }

        if ( m_pMeshFluidData && m_pMeshFluidData->m_data.getEdgeFluidLevels().size())
        {
            // first compute "true" depth, well, all depth
            f32 minDepth = getDepth();
            f32 maxDepth = getDepth();
            AABB friezeAABB(m_pMeshFluidData->m_AABB.get());
            friezeAABB.Translate(get2DPos());

            const ITF_VECTOR<EdgeFluidLevel> & edgeFluidListLevel = m_pMeshFluidData->m_data.getEdgeFluidLevels();
            u32 edgeFluidListCount = edgeFluidListLevel.size();
            for ( u32 i = 0; i<edgeFluidListCount ; i++ )
            {
                const EdgeFluidLevel & edge = edgeFluidListLevel[i];
                f32 depth = edge.getTrueDepth();
                if (minDepth > depth)
                    minDepth = depth;
                if (maxDepth < depth)
                    maxDepth = depth;
            }

            if (CAMERA->isRectVisible_NotInlined(friezeAABB, minDepth) || CAMERA->isRectVisible_NotInlined(friezeAABB, maxDepth))
            {
                ITF_VECTOR<EdgeFluidLevel> & edgeFluidListLevel = m_pMeshFluidData->m_data.getEdgeFluidLevels();
                u32 edgeFluidListCount = edgeFluidListLevel.size();
                for ( u32 i = 0; i<edgeFluidListCount ; i++ )
                {
                    EdgeFluidLevel & edge = edgeFluidListLevel[i];
                    edge.m_color = m_colorFactor;
                    edge.m_pivot = get2DPos();

                    AABB edgeAABB(edge.m_edgeFluid->m_aabb);
                    edgeAABB.Translate(edge.m_pivot);
                    
                    if (CAMERA->isRectVisible_NotInlined(edgeAABB, minDepth) || CAMERA->isRectVisible_NotInlined(edgeAABB, maxDepth))
                    {
                        if ( m_needUpdateMeshElemFromConfig && m_pMeshStaticData )
                            updateMeshElemFromConfig( &m_pMeshStaticData->m_mesh, m_pMeshStaticData->m_elemConfig_IndexList);      

                        GFX_ADAPTER->addPrimitiveInZList( &edge, GFX_ITF_FLUID, NULL, edgeFluidListLevel[i].getDepth(), m_renderInTarget,getRef() );
                        configvisible = btrue;
                    }
                }
            }
        }

        if ( configvisible )
        {
            computeFinalColor();
            m_configsUsedThisFrame.push_back(m_ConfigResourceID);
            STATS_FRIEZE_DRAWINC

            setLastVisibleFrame(CURRENTFRAME);
        }
    }

    Pickable* Frise::clone(Scene* _pDstScene, bbool _callOnLoaded) const
    {
        ITF_ASSERT_MSG(_pDstScene, "A valid scene must be provided");

        Frise* localFrieze = newAlloc(mId_Frieze, Frise());
        BaseObjectClone(const_cast<Frise*>(this), localFrieze);

        localFrieze->setFriseConfig(m_ConfigName);

        // Register into the scene
        if (_pDstScene)
        {
            _pDstScene->registerPickable(localFrieze);
            _pDstScene->processRegisterPending();
        }

        if(_callOnLoaded)
            localFrieze->onLoaded();
        else
            localFrieze->resetCurrentInfos();

        return localFrieze;
    }

    void Frise::setDepth( f32 _depth )
    {
        const SafeArray<PolyLine*>* pCollisionData = getCollisionData_WorldSpace();
        if ( pCollisionData )
        {
            for(u32 i = 0; i < pCollisionData->size(); ++i)
            {
                PolyLine* pCol = (*pCollisionData)[i];
                ITF_ASSERT(pCol);

                pCol->setDepth(_depth);
            }
        }

        BaseObject* obj = GETOBJECT(m_visualPoly);

        if ( obj )
        {
            PolyLine* visualPoly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

            if ( visualPoly )
            {
                visualPoly->setDepth(_depth);
            }
        }

        if ( getDepth() != _depth )
        {
            Super::setDepth(_depth);

            if ( isActive() )
            {
                unregisterPhysic();
                registerPhysic();
            }
        }
    }

    void Frise::setFriseConfig( const Path& _name )
    {
        bbool isMainThread = (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        ResourceGroup* grp = (ResourceGroup*)(m_resourceGroup.getResource());
        if (grp && !grp->isEmpty())
        { 
            const ResourceGroup::ResourceList& ress = grp->getResourcesList();
            ResourceGroup::ResourceList::const_iterator it = ress.begin();
            ResourceGroup::ResourceList::const_iterator itEnd = ress.end();

            while(it != itEnd)
            {
                if(Resource* pRes = it->getResource())
                {
                    if(pRes->getType() == Resource::ResourceType_FriezeConfig)
                    {
                        if(pRes->getPath() == _name)
                            return;
                        grp->removeResource(*it);
                    }
                }

                ++it;
            }
        }

        m_ConfigName = _name;

        if (SCENE_MANAGER->isInMinimalLoadingMode())
            return;

        FriseConfig* newConfig = NULL;

        if ( RESOURCE_MANAGER->getResourceIdFromFile(m_ConfigResourceID, m_ConfigName.getStringID(), Resource::ResourceType_FriezeConfig) )
        {
            newConfig = m_ConfigResourceID.getResource()->DynamicCast<FriseConfig>(ITF_GET_STRINGID_CRC(FriseConfig,4277131661));
            if (isMainThread)
            newConfig->addLogicalDataUser();
            else
                RESOURCE_MANAGER->queryAddUserToResourceLogicalData(newConfig);
        }
        else
        {
            // If the resource doesn't exist we have to create it and read the file
            newConfig = loadFriezeConfig(m_ConfigName, bfalse); // this adds a logic user
        }

        if (!newConfig)
        {
            if ( RESOURCE_MANAGER->getResourceIdFromFile(m_ConfigResourceID, FriseConfig::defaultFriseConfig, Resource::ResourceType_FriezeConfig) )
            {
                newConfig = m_ConfigResourceID.getResource()->DynamicCast<FriseConfig>(ITF_GET_STRINGID_CRC(FriseConfig,4277131661));
                if (isMainThread)
                newConfig->addLogicalDataUser();
                else
                    RESOURCE_MANAGER->queryAddUserToResourceLogicalData(newConfig);
            }
            else
            {
                newConfig = loadFriezeConfig(FriseConfig::defaultFriseConfig, bfalse);
                if ( newConfig != NULL )
                newConfig->setStatic(btrue);
            }

            LOG("[FriseConfig] Frise: \"%s\" (id: %llX) unable to find FriezeConfig : \"%s\"",  getUserFriendly().cStr(), getObjectId().m_id, _name.getString8().cStr());

            m_ConfigName = FriseConfig::defaultFriseConfig;
        }

        ITF_ASSERT(newConfig);

        if (newConfig)
        {
            m_ConfigResourceID = newConfig->getID();

            ITF_ASSERT(newConfig == m_ConfigResourceID.getResource());

            if (getUserFriendly().getLen()==0)
            {
                PathString_t configName;

                m_ConfigName.getString(configName);

                generateNewUserFriendly(configName.cStr());
            }

            ResourceGroup* grp = (ResourceGroup*)(m_resourceGroup.getResource());
            if (grp)
            {
                if (isMainThread)
                    grp->addResource(newConfig, bfalse); // don't add logical user, it's already done
                else
                    RESOURCE_MANAGER->queryAddResourceToGroup( grp->getID(), newConfig->getID() );
            }
        }
        else
        {
            ITF_WARNING_CATEGORY(Graph,this, bfalse, "[FriseConfig] Frise: \"%s\" (id: %llX) unable to find FriezeConfig : \"%s\"",  getUserFriendly().cStr(), getObjectId().m_id, _name.getString8().cStr());
            ITF_WARNING_CATEGORY(Graph,this, bfalse, "Could replace frise config by default frise config:\n%s\nin:\n%s\nFile not found!", FriseConfig::defaultFriseConfig, StringToUTF8(DATA_FOLDER).get());
            m_ConfigResourceID.invalidateResourceId();
        }

        //frieze plugin
        SF_DEL(m_friezePlugin);
        if (newConfig)
        {
            const StringID &plugin =  newConfig->m_plugin;

            if (plugin.isValid() && plugin!=StringID(""))
            {
                m_friezePlugin = FRIEZE_PLUGIN_REGISTRY->createPluginInstance(newConfig->m_plugin, this);
                ITF_WARNING_CATEGORY(LD, this, m_friezePlugin!=NULL, "unknown frieze plugin");
            }
        }
    }

    ResourceGroup*  Frise::getResourceGroup()
    {
        return static_cast<ResourceGroup*>(m_resourceGroup.getResource());
    }

    bbool Frise::updatePhysicalReady()
    {
        //Super::updatePhysicalReady();
        ResourceGroup* grp = (ResourceGroup*)(m_resourceGroup.getResource());
        if (!grp)
        {
            ITF_ASSERT(0);
            return btrue;
        }
    //    if (!grp->loadingQuerried())
        //    loadResources();
        grp->updatePhysicallyLoaded();
        bbool groupIsLoaded = grp->isPhysicallyLoaded();
        bbool collisionsRegistered = m_physicRegistered;
        if (!collisionsRegistered)
            registerPhysic();

        bbool physicalReady = groupIsLoaded && collisionsRegistered;
        setPhysicalReady(physicalReady);
        return physicalReady;
    }

    void Frise::RemoveFromUpdateAnimList(const ResourceID _id)
    {
        {
        ITF_VECTOR<ResourceID>::iterator it = m_configsUsedThisFrame.begin();

        while (it != m_configsUsedThisFrame.end())
        {
            if ( *it == _id)
                it = m_configsUsedThisFrame.erase(it);
            else
                ++it;
        }
    }

        {
            ITF_VECTOR<ResourceID>::iterator it = m_configsNeedingAnimDraw.begin();

            while (it != m_configsNeedingAnimDraw.end())
            {
                if ( *it == _id)
                    it = m_configsNeedingAnimDraw.erase(it);
                else
                    ++it;
            }
        }

    }

    void Frise::UpdateAllAnimations ( f32 _ellapsed )
    {
#define FRISE_H_DIV 2
#define FRISE_V_DIV 2
        flushGraphicData();

        if (_ellapsed == 0.f)
        {
            m_configsUsedThisFrame.clear();
            return;
        }

        ITF_VECTOR<ResourceID>::iterator it = m_configsUsedThisFrame.begin();
        ITF_VECTOR<ResourceID>::iterator end = m_configsUsedThisFrame.end();

        for (; it != end; ++it)
        {
            FriseConfig* pConf = (FriseConfig*)((*it).getResource());
            if ((!pConf) ||(pConf->m_lastUpdateFrame == CURRENTFRAME))
                continue;

            ResourceGroup * pConfGroup = pConf->getGroup();
            if (pConfGroup)
                pConfGroup->updatePhysicallyLoaded();
            pConf->m_lastUpdateFrame = CURRENTFRAME;

            u32     texConfCount = pConf->m_textureConfigs.size();
            bbool   needAnimDraw = bfalse;
            for (u32 texIt = 0; texIt < texConfCount; texIt++)
            {
                FriseTextureConfig& rconf = pConf->m_textureConfigs[texIt];
                if (rconf.m_configType == FriseTextureConfig::FriseConfig_Animation)
                {
                    if (!rconf.mp_animMeshScene)
                    {
                        u32 hdiv = FRISE_H_DIV;
                        u32 vdiv = FRISE_V_DIV;

                        rconf.mp_animMeshScene = AnimMeshScene::AllocNewMesh(VertexFormat_PCT, 
                            GraphicComponent::getStaticIndexBuffer(hdiv,vdiv));
                        
                        AnimInfo *pAnimInfoIn = &rconf.mp_animMeshScene->m_AnimInfo;

                        // set fix anim info data
                        pAnimInfoIn->m_vertexFormat       = VertexFormat_PCT;
                        pAnimInfoIn->m_useRootRotation    = bfalse;
                        pAnimInfoIn->m_hDiv               = hdiv;
                        pAnimInfoIn->m_vDiv               = vdiv;
                    }     

                    AnimTrackResource* pAnim = (AnimTrackResource*)rconf.m_textureData.getResource();
                    if (pAnim && pAnim->isPhysicallyLoaded())
                    {
                        AnimTrack* pTrack = pAnim->getTrack();
                        if (pTrack)
                        {
                            if (!rconf.m_animTrackExt)
                            {
                                rconf.m_animTrackExt = newAlloc(mId_Frieze, AnimTrackExt());
                            }

                            rconf.m_animTrackExt->m_trackResourceID = rconf.m_textureData;
                            AnimInfo *pAnimInfo = &rconf.mp_animMeshScene->m_AnimInfo;
                            
                            // set dynamic data
                            f32 frame = ELAPSEDTIME / LOGICDT + rconf.m_animPhase;
                            pAnimInfo->m_frameInfo[0].m_pTrackExt   = rconf.m_animTrackExt;
                            pAnimInfo->m_frameInfo[0].m_fFrame      = f32_Modulo(frame,rconf.m_animTrackExt->getTrack()->m_endFrame);
                            pAnimInfo->m_Pos                = Vec3d(rconf.m_AnimTranslationX, rconf.m_AnimTranslationY,0.f);
                            pAnimInfo->m_Scale              = Vec2d(rconf.m_AnimScaleX, rconf.m_AnimScaleY);

                            pAnimInfo->m_IsLookingRight     = (pTrack->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT) != 0; 
                            pAnimInfo->m_Angle              = rconf.m_AnimRot;
                            if (rconf.m_animAABB.getMin() == Vec2d::Zero &&
                                rconf.m_animAABB.getMax() == Vec2d::Zero)
                                pAnimInfo->m_visualAABB     = pTrack->m_bounding;
                            else
                                pAnimInfo->m_visualAABB     = rconf.m_animAABB;

                            /// no impostor ? then create it.
                            if (!rconf.mp_animMeshScene->m_pAnimDraw.getRenderTargetImpostor())
                                rconf.mp_animMeshScene->m_pAnimDraw.setImpostor(rconf.m_AnimTexSizeX, rconf.m_AnimTexSizeY);
                            
                            /// resize impostor for HD screenshot;
                            rconf.mp_animMeshScene->m_pAnimDraw.checkIfImpostorSizeMustChange();

                            rconf.mp_animMeshScene->m_pAnimDraw.getRenderTargetImpostor()->setBackGroundColor(rconf.m_AnimBackGroundColor.getAsU32());
                            pAnimInfo->m_globalData.clear();

                            pAnimInfo->SetFrame();
                            //with impostor no need for reprocess patches
                            //rconf.mp_animMeshScene->m_pAnimPatches.processPatchs(rconf.mp_animMeshScene);
                            needAnimDraw = btrue;
                        }
                    }
                }
            }
            if (needAnimDraw)
                m_configsNeedingAnimDraw.push_back((*it));
        }
        m_configsUsedThisFrame.clear();
    }

    void Frise::DrawAllAnimations ( )
    {
        ITF_VECTOR<ResourceID>::iterator it = m_configsNeedingAnimDraw.begin();
        ITF_VECTOR<ResourceID>::iterator end = m_configsNeedingAnimDraw.end();

        for (; it != end; ++it)
        {
            FriseConfig* pConf = (FriseConfig*)((*it).getResource());
            if (pConf)
            {
                u32     texConfCount = pConf->m_textureConfigs.size();
                for (u32 texIt = 0; texIt < texConfCount; texIt++)
                {
                    FriseTextureConfig& rconf = pConf->m_textureConfigs[texIt];
                    if (rconf.m_configType == FriseTextureConfig::FriseConfig_Animation &&
                        rconf.mp_animMeshScene)
                    {
                        rconf.mp_animMeshScene->m_pAnimDraw.Draw(rconf.mp_animMeshScene);
                    }
                }
            }
        }
        m_configsNeedingAnimDraw.clear();
    }
    
    void Frise::preProperties()
    {
#ifdef ITF_SUPPORT_EDITOR
        m_editConfigName = m_ConfigName;
#endif
    }

    void Frise::postDataLoad()
    {
    }

#ifdef ITF_SUPPORT_EDITOR
    void Frise::postloadProperties()
    {
        if ( !( getConfigNamePath() == m_editConfigName ) )
        {
            setFriseConfig(m_editConfigName);
        }
        recomputeData();
    }
#endif

    void Frise::preloadFileDependencies(   )
    {
        if ( !RESOURCE_MANAGER->getResourceIdFromFile(m_ConfigResourceID, m_ConfigName.getStringID(), Resource::ResourceType_FriezeConfig) )
            setFriseConfig(m_ConfigName);
    }

    const Path* Frise::getTemplatePath() const
    {
        return &getConfigNamePath();
    }

    bbool Frise::isZForced() const
    {
        if(FriseConfig* config = getConfig())
            if(config->m_posZForced)
                return btrue;

        return bfalse;
    }

    void Frise::deleteOwnedPolyline(PolyLine* _pol)
    {
        for (u32 i = 0; i < m_pFriezeFullData->m_dynamicCollisionData.size(); i++)
        {
            if (_pol == m_pFriezeFullData->m_dynamicCollisionData[i])
            {
                SF_DEL(m_pFriezeFullData->m_dynamicCollisionData[i]);
                m_pFriezeFullData->m_dynamicCollisionData.eraseKeepOrder(i);
                return;
            }
        }
        for (u32 i = 0; i < m_pFriezeFullData->m_staticCollisionData.size(); i++)
        {
            if (_pol == m_pFriezeFullData->m_staticCollisionData[i])
            {
                SF_DEL(m_pFriezeFullData->m_staticCollisionData[i]);
                m_pFriezeFullData->m_staticCollisionData.eraseKeepOrder(i);
                m_pFriezeFullData->m_staticCollisionsLocalSpace.erase(m_pFriezeFullData->m_staticCollisionsLocalSpace.begin()+i);
                return;
            }
        }

        ITF_ASSERT_MSG(bfalse, "deleteOwnedPolyline : polyline not found");
    }

    void Frise::switchCollisionsToGlobalSpace()
    {
        if ( !m_pFriezeFullData )
            return;

        if (m_collisionInGlobalSpace && !isDynamic())
            return;
        m_collisionInGlobalSpace = btrue;
        while (m_pFriezeFullData->m_staticCollisionsLocalSpace.size() > m_pFriezeFullData->m_staticCollisionData.size())
            m_pFriezeFullData->m_staticCollisionsLocalSpace.erase(m_pFriezeFullData->m_staticCollisionsLocalSpace.begin() + (m_pFriezeFullData->m_staticCollisionsLocalSpace.size()-1));

        ITF_ASSERT(m_pFriezeFullData->m_staticCollisionData.size() == m_pFriezeFullData->m_staticCollisionsLocalSpace.size());
        for (u32 i = 0; i < m_pFriezeFullData->m_staticCollisionData.size(); i++)
        {
            if (m_pFriezeFullData->m_staticCollisionData[i]->getPosCount() != m_pFriezeFullData->m_staticCollisionsLocalSpace[i].getPosCount())
            {
                m_pFriezeFullData->m_staticCollisionsLocalSpace[i].clear();
                for (u32 cpy = 0; cpy < m_pFriezeFullData->m_staticCollisionData[i]->getPosCount(); cpy++)
                {
                    m_pFriezeFullData->m_staticCollisionsLocalSpace[i].addPoint(m_pFriezeFullData->m_staticCollisionData[i]->getPosAt(cpy));
                }
            }
            for (u32 j = 0; j < m_pFriezeFullData->m_staticCollisionsLocalSpace[i].getPosCount(); j++)
                m_pFriezeFullData->m_staticCollisionData[i]->setPosAt(m_pFriezeFullData->m_staticCollisionsLocalSpace[i].getPosAt(j), j);
            offsetCollisionData(m_pFriezeFullData->m_staticCollisionData[i], get2DPos());
        }
    }

    void Frise::registerPhysic()
    {
        if (m_querriedRecomputeData)
            forceRecomputeData();
        m_physicRegistered = btrue;

        if ( m_pFriezeFullData && !m_disablePhysic)
        {
            switchCollisionsToGlobalSpace();

            if (isDynamic())
            {
                for (u32 i = 0; i < m_pFriezeFullData->m_dynamicCollisionData.size(); i++)
                {
                    if (!m_pFriezeFullData->m_dynamicCollisionData[i]->m_physBody)
                        m_pFriezeFullData->m_dynamicCollisionData[i]->onAddedToScene(getScene());
                    else
                        m_pFriezeFullData->m_dynamicCollisionData[i]->registerPhysic();
                }
            }
            else
            {
                for (u32 i = 0; i < m_pFriezeFullData->m_staticCollisionData.size(); i++)
                {
                    if (!m_pFriezeFullData->m_staticCollisionData[i]->m_physBody)
                        m_pFriezeFullData->m_staticCollisionData[i]->onAddedToScene(getScene());
                    else
                        m_pFriezeFullData->m_staticCollisionData[i]->registerPhysic();
                }
            }
        }
    }

    void Frise::unregisterPhysic()
    {
        m_physicRegistered = bfalse;

        if ( m_pFriezeFullData )
        {
            for (u32 i = 0; i < m_pFriezeFullData->m_dynamicCollisionData.size(); i++)
                m_pFriezeFullData->m_dynamicCollisionData[i]->unregisterPhysic();
            for (u32 i = 0; i < m_pFriezeFullData->m_staticCollisionData.size(); i++)
                m_pFriezeFullData->m_staticCollisionData[i]->unregisterPhysic();
        }
    }

    void Frise::onBecomeActive()
    {
        //m_physicalReady = btrue;
        Super::onBecomeActive();
        registerPhysic();
        
        FriseConfig* config = getConfig();
        if( !m_regionRegistered && config && config->m_regionId.isValid())
        {
            if(BaseObject* pPol = m_visualPoly.getObject())
            {
                REGIONS_MANAGER->addRegion(m_visualPoly, config->m_regionId, getDepth());
                m_regionRegistered = btrue;
            }
        }
    }
    
    void Frise::onBecomeInactive()
    {
        Super::onBecomeInactive();
        unregisterPhysic();

        if ( m_regionRegistered )
        {
            REGIONS_MANAGER->removeRegion(m_visualPoly);
            m_regionRegistered = bfalse;
        }
    }

    void Frise::onSceneActive()
    {
        Super::onSceneActive();
        
        registerInWorldCells();
    }

    void Frise::onSceneInactive()
    {
        Super::onSceneInactive();

        if ( m_regionRegistered )
        {
            // In case the config name changed
            REGIONS_MANAGER->removeRegion(m_visualPoly);
            m_regionRegistered = bfalse;
        }
    }

    void Frise::onAddedToScene( Scene* _pScene )
    {
        Super::onAddedToScene(_pScene);
        recomputeData();
    }

    void Frise::onRemovedFromScene( Scene* _pScene, bbool _forDeletion )
    {
        if ( m_pFriezeFullData )
        {
            for (u32 i = 0; i < m_pFriezeFullData->m_dynamicCollisionData.size(); i++)
                m_pFriezeFullData->m_dynamicCollisionData[i]->onRemovedFromScene(_pScene, _forDeletion);
            for (u32 i = 0; i < m_pFriezeFullData->m_staticCollisionData.size(); i++)
                m_pFriezeFullData->m_staticCollisionData[i]->onRemovedFromScene(_pScene, _forDeletion);
        }

        clearCollisionData();
        Super::onRemovedFromScene(_pScene, _forDeletion);
    }

    bbool Frise::canConnectToOtherFrieze() const 
    {
        ITF_ASSERT_CRASH(getConfig(), "no config"); 
        if (isDynamic()) 
            return bfalse; 

        u32 methode = getConfig()->m_methode;
        if (methode==InFluid || methode==InString) 
            return bfalse;
        return btrue;
    }

    void Frise::forceRecomputeData()
    {
        m_querriedRecomputeData = btrue;
        executeRecomputeData();
    }

    void Frise::offsetPosition(const Vec3d& _delta, bbool _applyToInitialPosition )
    {
        if( _delta.sqrnorm() < MTH_EPSILON && !isDynamic())
            return;

        Super::offsetPosition(_delta, _applyToInitialPosition);
        if (isDynamic() && (!_applyToInitialPosition))
            return;
        #if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
            if (isActive() && !isDynamic() && CAMERA->getMode() != Camera::Camera_Editor)
                ITF_WARNING(this, bfalse, "Moving non dynamic frieze");
        #endif
        recomputeData();
    }

    void Frise::set2DPos( const Vec2d& _pos )
    {
        Super::set2DPos(_pos);
        recomputeData();
    }

    void Frise::setPos( const Vec3d& _pos )
    {
        Super::setPos(_pos);
        if (!isDynamic())          
            recomputeData();
    }

    Vec2d Frise::inverseTransform(const Vec2d& _v) const
    {
        Vec2d res;
        Matrix44 transfo;
        if (isDynamic())
            computeDynamicDisplayMatrix(transfo);
        else
            computeStaticDisplayMatrix(transfo);

        Matrix44 transfoNoTrans(transfo);
        Matrix44 inverseMatrix;
        transfoNoTrans.setLineAt(3,0,0,0,1);
        M44_matrixTranspose(&inverseMatrix, &transfoNoTrans);
        inverseMatrix.setLineAt(3,0,0,0,1.f);
        M44_matrixCoordTransform(&res, &inverseMatrix, &_v);
        return res;
    }

    Vec3d Frise::inverseTransform(const Vec3d& _v) const
    {
        Vec3d res;
        Matrix44 transfo;
        if (isDynamic())
            computeDynamicDisplayMatrix(transfo);
        else
            computeStaticDisplayMatrix(transfo);

        Matrix44 transfoNoTrans(transfo);
        Matrix44 inverseMatrix;
        transfoNoTrans.setLineAt(3,0,0,0,1);
        M44_matrixTranspose(&inverseMatrix, &transfoNoTrans);
        inverseMatrix.setLineAt(3,0,0,0,1.f);
        M44_matrixCoordTransform(&res, &inverseMatrix, &_v);
        return res;
    }

    void Frise::scale(const Vec2d & _deltaScale, bbool _applyToInitialValue)
    {
        Super::scale(_deltaScale, _applyToInitialValue);
    }

    void Frise::scale( const Vec2d& _center, const Vec2d& _deltaInPercent, bbool /*_applyToInitialValue*/ )
    {
        Vec2d xx = Vec2d::XAxis;//.Rotate(getAngle());
        Vec2d yy = xx.getPerpendicular();
/*
        {
            Vec3d dbgPos;
            dbgPos.m_x = _center.m_x;
            dbgPos.m_y = _center.m_y;
            dbgPos.m_z = getDepth();
            GFX_ADAPTER->drawDBGCircle(_center, 1.f, 1, 1, 1, 1);
        }
        */
        Vec2d scaleOrigin = inverseTransform(_center - transformPos(Vec3d::Zero).truncateTo2D());

        for(u32 i = 0; i < getPosCount(); ++i)
        {
            Vec2d pointPos   = getPosAt(i);
            Vec2d direction  = (pointPos - scaleOrigin);
 
            Vec2d finalPoint = scaleOrigin;

            if(_deltaInPercent.m_y == 0.0f) // SCALE X
            {
                f32 dot = yy.dot(direction);
                Vec2d basey =  scaleOrigin + dot * yy;
                Vec2d newDirX = (pointPos - basey) * _deltaInPercent.m_x;
                finalPoint = basey + newDirX;
            }
            else if(_deltaInPercent.m_x == 0.0f) // SCALE Y
            {
                f32 dot = (-xx).dot(direction);
                Vec2d basex =  scaleOrigin + dot * (-xx);
                Vec2d newDirY = (pointPos - basex) * _deltaInPercent.m_y;
                finalPoint = basex + newDirY;
            }
            else // UNIFORM SCALE (using Y)
            {
                const f32 coef = _deltaInPercent.m_y;
                finalPoint = scaleOrigin + direction * coef;
            }

            setPosAt(finalPoint, i);
        }
    }

    bbool Frise::erasePosAt(u32 _index)
    {
        bbool ret = m_pointsList.erasePosAt(_index); 
        if (ret)
        {
            ITF_ASSERT(getScene());
            getScene()->unregisterPickable(this);
            return btrue;
        }

#ifdef  ITF_SUPPORT_EDITOR
        m_pickingInfo2D = NULL;
#endif //ITF_SUPPORT_EDITOR

        recomputeData();

        return bfalse;
    }

    void Frise::invertPoints()
    {
        u32 count = getPosCount();
        if ( isLooping() && count>2 )
            count--; // to not affect the loop point

        u32 half  = (u32)(((f32)count) * 0.5f + 0.5f);

        ITF_VECTOR<PolyLineEdge>& edgeData = m_pointsList.getMutableEdgeData();
        for ( u32 index = 0; index < half; index++)
        {
            PolyLineEdge edg = edgeData[index];
            edgeData[index] = edgeData[count-index-1];
            edgeData[count-index-1] = edg;
        }
        recomputeData();
    }

    void Frise::setAsFirstPoint( u32 _indexOffset)
    {        
        u32 count = getPosCount();

        if ( !isLooping() || _indexOffset == 0 || count <= 2 || _indexOffset >= count -1) // point with index count -1 is the loop point
            return;

        count--;
        ITF_VECTOR<PolyLineEdge>& edgeData = m_pointsList.getMutableEdgeData();
        ITF_VECTOR<PolyLineEdge> edgeDataCopy = m_pointsList.getMutableEdgeData();
         
        for ( u32 i = 0; i < count; i++)   
        {
            u32 index = ( i +_indexOffset) %count;
            edgeData[i] = edgeDataCopy[index];
        }

        recomputeData();
    }

    void Frise::convertFromOldDataFormat()
    {
/*        for(u32 i = 1; i < getPosCount(); ++i)
            setPosAt(getPosAt(i) + getPosAt(0), i);*/

        if (m_pointsList.getPosCount()==0)
            return;

        m_initialPos = m_pointsList.getPosAt(0);
        m_pointsList.setPosAt(Vec2d::Zero, 0);
        setPos(getWorldInitialPos());
        setWorldInitialRot(0);
    }

    void Frise::rotate(f32 _deltaAngle, bbool _applyToInitialValue, const Vec3d* _pOrigin)
    {
        Super::rotate(_deltaAngle, _applyToInitialValue, _pOrigin);
            if (_applyToInitialValue)
                    recomputeData();
    }

    void Frise::notifyParentsAndChildren(PickableEditorChangeType _type )
    {
    }

    void Frise::getNearestEdgeFromPos(const Vec2d& _pos, PolyLineEdge& _bestEdge, i32& _bestIndex, f32& _bestRatio) const
    {
        Matrix44 matrix = getTransformMatrix(btrue);

        const ITF_VECTOR<PolyLineEdge>& edgeData = m_pointsList.getEdgeData();
        i32 size = edgeData.size();
        f32 bestDist = F32_INFINITY;
        for (i32 i = 0; i < size-1; i++)
        {
            f32 distToEdge = 0.f;
            const PolyLineEdge& edg1 = edgeData[i];
            const PolyLineEdge& edg2 = edgeData[i+1];
            Vec2d p1 = transformPos(matrix, edg1.getPos());
            Vec2d p2 = transformPos(matrix, edg2.getPos());
            Vec2d v(p2 - p1);
            v.normalize();
            Vec2d vToP = _pos - p1;
            f32 dot = vToP.dot(v);
            f32 ratio;
            if (dot < 0.0f)
            {
                distToEdge = (p1-_pos).norm();
                ratio = 0.f;
            }
            else
            {
                if (dot > edg1.m_length)
                {
                    distToEdge = (p2-_pos).norm();
                    ratio = 1.f;
                }
                else
                {
                    distToEdge = f32_Abs(v.getPerpendicular().dot(vToP));
                    ratio = dot / edg1.m_length;
                }
            }
            
            if (distToEdge < bestDist)
            {
                bestDist = distToEdge;
                _bestEdge = edg1;
                _bestIndex = i;
                _bestRatio = ratio;
            }
        }
    }


    void Frise::repairPivot()
    {
        Vec2d _pivot = getAnchorPos().truncateTo2D();
        Vec2d delta = _pivot - get2DPos();
        for (u32 i = 0; i< getPosCount(); i++)
        {
            setPosAt(getPosAt(i)-delta, i);
        }
        set2DPos(_pivot);
        float z = getDepth();
        setWorldInitialPos(_pivot.to3d(z));
        executeRecomputeData();
    }


    void Frise::setUserCustomPivot(const Vec2d& _pivot)
    {
        m_customUserPivot = inverseTransform(_pivot - transformPos(Vec3d::Zero).truncateTo2D());
        setPivotMode(PivotMode_UserCustom);
    }

    void Frise::onParentChanged (ObjectRef _changedObject, PickableEditorChangeType _type)
    {
    /*    setPos(getWorldInitialPos());
        setAngle(getWorldInitialRot());
        forceRecomputeData();


        if (isDynamic())
        { 
            setDynamicTransform(GMatrixIdentity);
            applyDynamicTransform();
        }*/
    }



/*
    void Frise::setFlipped( bbool _value )
    {
        if(isFlipped() != _value)
        {
            Super::setFlipped(_value);
        }
    }
*/

    void Frise::preCheckPoint()
    {
    }

    void Frise::postCheckPoint()
    {
        onLoaded(HotReloadType_Checkpoint);
        m_querriedRecomputeData = bfalse;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Frise::onEditorCreated( Pickable* /*_original = NULL*/ )
    {
        forceRecomputeData();
    }

    void    Frise::onEditorMove( bbool _modifyInitialPos  )
    {
        Super::onEditorMove(_modifyInitialPos);

        unregisterPhysic();
        registerPhysic();
        forceRecomputeData();

        if (isDynamic())
        {
            applyDynamicTransform();
            //if (_modifyInitialPos)
                //postCheckPoint();   // important to avoid dynamic friezes issues when linked to groups with tweenings
        }
    }
#endif // ITF_SUPPORT_EDITOR

    void Frise::mirror( bbool _onX, bbool _onY, const Vec2d& _pivot )
    {
        Vec2d pivot = inverseTransform(_pivot - transformPos(Vec2d::Zero));
        if (!_onX)
            pivot.m_x = 0.f;
        if (!_onY)
            pivot.m_y = 0.f;

        for (u32 i = 0; i < getPosCount(); i++)
        {
            Vec2d pos = getPosAt(i) - pivot;
            if (_onX) 
                pos.m_x   = pivot.m_x - pos.m_x;
            if (_onY)
                pos.m_y   = pivot.m_y - pos.m_y;
            setPosAt(pos, i);
        }
        invertPoints();
    }

} // namespace ITF

