#include "precompiled_engine.h"

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_EDITOR_H_ // TODO REMOVE temporary for screen capture
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_ // TODO REMOVE temporary for screen capture

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif // _ITF_PICKABLE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_WORLDFILE_H_
#include "engine/scene/worldFile.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_PLANE_H_
#include "core/math/plane.h"
#endif //_ITF_PLANE_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef ITF_FINAL
#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_
#endif

#ifndef _CELLLAYERSSETTINGS_H_
#include "engine/scene/CellsLayersSettings.h"
#endif // _CELLLAYERSSETTINGS_H_

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL) && defined(DEVELOPER_FRED)
    #define DEBUG_VIEWS_LOADING
#endif

// #define CELL_CONTENT_ENABLE_BY_DISTANCE // update optimization

namespace ITF
{
    #define MAX_I16_RANGE 30000 //around the i16 max range ,don t use the real I16 range for easily debugging
    #define MIN_I16_RANGE -MAX_I16_RANGE

    CellsLayersSettings::Target CellsLayersSettings::m_currentTarget = CELLLAYERSSETTINGS_TARGET;

    f32    World::m_fmaxCellXCoord = (f32)World::m_maxCellXCoord;
    f32    World::m_fminCellXCoord = (f32)World::m_minCellXCoord;
    f32    World::m_fmaxCellYCoord = (f32)World::m_maxCellYCoord;
    f32    World::m_fminCellYCoord = (f32)World::m_minCellYCoord;
    TemplateClientHandler    World::m_templateClientHandler;
    u32     World::m_loadResourcesMask = 7;

    World::cellProcessCallback  World::s_cellProcessCallback = NULL;

    IMPLEMENT_OBJECT_RTTI(World)

    DECLARE_RASTER(world_StartFrame, RasterGroup_Misc, Color::cyan());
    DECLARE_RASTER(world_Update, RasterGroup_Misc, Color::blue());
    DECLARE_RASTER(world_Draw, RasterGroup_Misc, Color::red());

    static  SafeArray<BaseObject*>  tempObjArray;

    ITF_INLINE bbool isInCellsRange(f32 _x, f32 _y)
    {
        f32 fx = floorf(_x);
        f32 fy = floorf(_y);
        return fx < World::m_fmaxCellXCoord && fx > World::m_fminCellXCoord && fy < World::m_fmaxCellYCoord && fy > World::m_fminCellYCoord;
    }

    ITF_INLINE bbool isInCellsRange(i32 _x, i32 _y)
    {
        return _x < World::m_maxCellXCoord && _x > World::m_minCellXCoord && _y < World::m_maxCellYCoord && _y > World::m_minCellYCoord;
    }

    ITF_INLINE bbool isInCellsRange(const Vec2d& _pos)
    {
        f32 fx = floorf(_pos.m_x);
        f32 fy = floorf(_pos.m_y);
        return fx < World::m_fmaxCellXCoord && fx > World::m_fminCellXCoord && fy < World::m_fmaxCellYCoord && fy > World::m_fminCellYCoord;
    }


    World::World()
        : mp_afterFxManager(NULL)
        , m_bSceneAsyncLoadRunning(bfalse)
        , m_bActive(bfalse)
        , m_bAlwaysActive(bfalse)
        , m_activeObjectsListIndex(0)
        , m_bAllowDraw(btrue)
        , m_bAllowUpdate(btrue)
        , m_bPrevAllowDraw(btrue)
        , m_bPrevAllowUpdate(btrue)
        , m_bAllowDrawIsPushed(bfalse)
        , m_bAllowUpdateIsPushed(bfalse)
        , m_bAllowUnload(btrue)
        , m_checkFriezeConnections(bfalse)
        , m_worldFile(NULL)
        , m_minZEverRegistered(MTH_HUGE)
        , m_maxZEverRegistered(-MTH_HUGE)
        , m_asyncLoaded(bfalse)
    {
        for (u32 i = 0; i < CELL_LAYERS_COUNT; i++)
        {
            m_horizCellsCount[i] = LAYER_0_MAX_HORIZ_CELLS;
            m_vertCellsCount[i] = LAYER_0_MAX_VERT_CELLS;
            if (i > 0)
            {
                m_CellSize[i] = m_CellSize[i-1] * CELL_LAYERS_SCALE;
                m_horizCellsCount[i] /= i * CELL_LAYERS_SCALE;
                m_vertCellsCount[i] /= i * CELL_LAYERS_SCALE;
                m_horizCellsCount[i]++;
                m_vertCellsCount[i]++;
            }
            else
                m_CellSize[i]       =   Vec2d(LAYER_0_CELL_SIZE, LAYER_0_CELL_SIZE);

            m_cellsIndexes[i]   =   NULL;
            m_minCellPosEverRegistered[i] = Vec2d(9999999999999999999.f, 9999999999999999999.f);
            m_maxCellPosEverRegistered[i] = Vec2d(-9999999999999999999.f, -9999999999999999999.f);
        }

        m_fmaxCellXCoord   =   (f32)m_maxCellXCoord;
        m_fminCellXCoord   =   (f32)m_minCellXCoord;
        m_fmaxCellYCoord   =   (f32)m_maxCellYCoord;
        m_fminCellYCoord   =   (f32)m_minCellYCoord;

        m_particleGeneratorPool = newAlloc(mId_Particle,ITF_ParticleGenerator[m_particlePoolNumber]);
        SCENE_MANAGER->addListener(this);
    }
    

    World::~World()
    {
        ITF_ASSERT_MSG(m_prefetchList.size() == 0, "Someone didn't call stopPrefetch(), it may be a memory waste...");
        stopPrefetch();
        SF_DEL(m_worldFile);
        SCENE_MANAGER->removeListener(this);
        SF_DEL_ARRAY(m_particleGeneratorPool);
        SF_DEL(mp_afterFxManager);
        for (u32 i = 0; i < CELL_LAYERS_COUNT; i++)
        {
            SF_DEL_ARRAY(m_cellsIndexes[i]);
        }

        ITF_ASSERT_MSG(!isSceneAsyncLoadRunning(), "Find a way to avoid that");
    }

    void    World::centerOnWorldOrigin()
    {
        Scene* pScene = getRootScene();
        if (pScene)
            pScene->centerOnWorldOrigin();
    }

    void World::reinit(bbool _bFullReinit)
    {
        if(isSceneAsyncLoadRunning())
            return;

        Scene* pScene = getRootScene();
        ITF_ASSERT_MSG(pScene, "The world contains a deleted base scene");
        pScene->reinit(_bFullReinit);
    }

    void World::swapActiveObjectsList()
    {
        m_activeObjectsListIndex ^= 1;
        m_activeObjects[m_activeObjectsListIndex].clear();

        for(u32 iScene = 0; iScene < getSceneCount(); ++iScene)
        {
            Scene* pScene = getSceneAt(iScene);
            pScene->clearActiveObjectsList();
        }
    }

    void World::activateAlwaysActiveActors(u32 _frame)
    {
        for(u32 iScene = 0; iScene < getSceneCount(); ++iScene)
        {
            Scene* pScene = getSceneAt(iScene);

            const PickableList& aaa = pScene->getAlwaysActiveActors();
            const u32 uCount = aaa.size();
            for (u32 i = 0; i < uCount; i++)
            {
                aaa[i]->setActiveForFrame(_frame);
            }
        }
    }
    
    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
    void World::buildActiveObjectsList(u32 _iLayer, SafeArray<u16>& _cellsResource, SafeArray<u32>& _flagsArray, u32 _frame)
    #else
    void World::buildActiveObjectsList(u32 _iLayer, SafeArray<u16>& _cellsResource, u32 _frame)
    #endif
    {
        const bbool isProcessingFirstLayer = (_iLayer == 0);

        if (getIgnoreCells()) // if the world is not using cells : ativate all objects for this frame
        {
            if(isProcessingFirstLayer)
            {
                for(u32 iScene = 0; iScene < getSceneCount(); ++iScene)
                {
                    Scene* pScene = getSceneAt(iScene);
                    ITF_ASSERT_CRASH(pScene,"Scene couldn't be null in World::startFrame()");
                    const ObjectRef refScene = pScene->getRef();
                    ITF_UNUSED(refScene);

                    const PickableList& pickables = pScene->getPickableList();
                    const u32 uCount = pickables.size(); 
                    for (u32 j = 0; j < uCount; ++j)
                    {
                        Pickable* pObj = pickables[j];
                        ITF_ASSERT(pObj->getSceneRef() == refScene);
                        if (pObj->isEnabled() )
                        {
                            pObj->setActiveForFrame(_frame);
                        }
                    }
                }
            }
        }
        else
        {
            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                getCellsFromCamera(_iLayer, _cellsResource, &_flagsArray); // Populate m_CellsEnable (cells that are active) and _cellsResource (cells for which resources should be loaded)
            #else
                getCellsFromCamera(_iLayer, _cellsResource); // Populate m_CellsEnable (cells that are active) and _cellsResource (cells for which resources should be loaded)
            #endif

            if(isProcessingFirstLayer)
                activateAlwaysActiveActors(_frame);
        }
    }

    void World::desactivateOutOfRangeObjects(u32 _frame)
    {
        const BaseObjectList& LastFrameActiveList    = getLastFrameActiveObjects();
        tempObjArray.clear();
        ID_SERVER->getObjectListNoNullV2(LastFrameActiveList, &tempObjArray);
        const u32 uLastFrameCount                   = tempObjArray.size();

        for (u32 index = 0;index < uLastFrameCount;++index)
        {
            Pickable* pObj = (Pickable*)tempObjArray[index];

            if ( (pObj->getLastActiveFrame() < _frame) && pObj->isActive() )
            {
                pObj->onBecomeInactive();
            }
        }

        for (u32 index = 0; index < m_objectsToInactivate.size(); index++)
        {
            Pickable* pick = (Pickable*)m_objectsToInactivate[index].getObject();
            if (pick)
                pick->onBecomeInactive();
        }
        m_objectsToInactivate.clear();
    }

    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
    void World::loadInRangeResources(u32 _iLayer, SafeArray<u16>& _cellsResource, const SafeArray<u32>& _flagsArray)
    #else
    void World::loadInRangeResources(u32 _iLayer, SafeArray<u16>& _cellsResource)
    #endif
    {
        #ifdef USE_LOAD_RESOURCE_MAX_RANGE
            bbool thisLayerUsesMaxDist = bfalse;
            Vec2d camPos(CAMERA->getX(), CAMERA->getY());
            f32 sqCamDist = (f32)CellsLayersSettings::getLoadResourceMaxRange(_iLayer);
            if (sqCamDist > MTH_EPSILON)
            {
                thisLayerUsesMaxDist = btrue;
                sqCamDist *= sqCamDist;
            }
        #endif //USE_LOAD_RESOURCE_MAX_RANGE
        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            static ObjectRefList objInCell;
        #endif
        const u32 curFrame = CURRENTFRAME;
        const u32 uCellResCount = _cellsResource.size();
        
        const int nbMaxResourceLoadAtFrame = 5;
        int nbResourceLoaded = 0;
        for (u32 index = 0; index < uCellResCount; index++)
        {
            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                objInCell.clear();
            #endif
            tempObjArray.clear();

            u16 cellIndex = _cellsResource[index];
            ITF_VECTOR<Cell>& container = m_cellsContainer[_iLayer];
            ITF_ASSERT(cellIndex < container.size());
            Cell* pCurCell = &container[cellIndex];
            i32 wasActiveLastFrame = m_CellsToUnload[_iLayer].find(cellIndex);
            if (wasActiveLastFrame >= 0)
                m_CellsToUnload[_iLayer].eraseNoOrder(wasActiveLastFrame);

            ITF_ASSERT_MSG( pCurCell, "a cell can't be null at this point");
            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                pCurCell->getObjects(objInCell, _flagsArray[index]);
            #else
                //pCurCell->getObjects(objInCell);
                const SafeArray<ObjectRef>& objInCell = pCurCell->getAllObjects();
            #endif
            if (pCurCell->getIgnoreLoadResourceMask() || (m_loadResourcesMask == 0xffffffff) || (pCurCell->getLoadResourcesMask() == (curFrame & m_loadResourcesMask)))
            {
                pCurCell->clearIgnoreLoadResourceMask();
                ID_SERVER->getObjectListV2(objInCell, tempObjArray);
                for (u32 objIdx=0;objIdx<objInCell.size();++objIdx)
                {
                    Pickable* pObj = static_cast<Pickable*>(tempObjArray[objIdx]);
                    if(pObj)
                    {
                        //if (pObj->m_dontUnloadResourceAtFrame != curFrame)
                        {
                            #ifdef USE_LOAD_RESOURCE_MAX_RANGE
                                if (thisLayerUsesMaxDist)
                                {
                                    if (pObj->isRequestedLoading())
                                    {
                                        pObj->m_dontUnloadResourceAtFrame = curFrame;
                                        continue;
                                    }
                                    if (pObj->isLastCellUpdateDataValid())
                                    {
                                        const AABB& aabb = pObj->getLastCellUpdateData().m_aabb;
                                        if ((camPos-aabb.getCenter()).sqrnorm() > sqCamDist)
                                            continue;
                                    }
                                }
                            #endif //USE_LOAD_RESOURCE_MAX_RANGE
                            pObj->m_dontUnloadResourceAtFrame = curFrame;
                            if (!pObj->isRequestedLoading()) // object may have been deleted since last cell update
                                pObj->loadResources();
                        }

                        if (pObj->getObjectType() == BaseObject::eActor)
                        {
                            Actor *pActor = (Actor*)pObj;
                            if (nbResourceLoaded < nbMaxResourceLoadAtFrame && !pActor->isPhysicalReady())
                            {
                                if (pActor->updatePhysicalReady())
                                    nbResourceLoaded++;
                            }
                        }
                    }
                }
            }
/*
            static u32 peak = 0;
            if (_objLoadedResource.size() > peak)
            {
                peak = _objLoadedResource.size();
                LOG("obj to load peak: %d", peak);
            }
*/
        }
    }
    
    void World::unloadOutOfRangeResources(int _iLayer)
    {
        const u32 curFrame = CURRENTFRAME;

        // unload resources
        if (m_bAllowUnload)
        {
            SafeArray<u16>& cellsToUnload = m_CellsToUnload[_iLayer];
            ITF_VECTOR<Cell>& cellsContainer = m_cellsContainer[_iLayer];

            for (u32 iUnload = 0; iUnload < cellsToUnload.size(); iUnload++)
            {
                u32 localCellIndex = cellsToUnload[iUnload];
                ITF_ASSERT(localCellIndex < cellsContainer.size());
                #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                const SafeArray<Cell::CellObjData>& objInCells = cellsContainer[localCellIndex].getAllObjects();
                #else
                const ObjectRefList& objInCells = cellsContainer[localCellIndex].getAllObjects();
                #endif
                for (u32 objIdx=0;objIdx<objInCells.size();++objIdx)
                {
                    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                    const ObjectRef& objref = objInCells[objIdx].m_obj;
                    #else
                    const ObjectRef& objref = objInCells[objIdx];
                    #endif

                    Pickable* pObj = static_cast<Pickable*>(GETOBJECT(objref));
                    if( pObj && pObj->isAllowedForCell() )
                    {
                        //if (pObj->getObjectType()==BaseObject::eActor)
                        //{
                        //    Actor *actor = (Actor*)pObj;
                        //}
                        if (curFrame == pObj->m_dontUnloadResourceAtFrame)//don't unload resources shared by active scene
                            continue;

                        pObj->unloadResources();
                    }
                }                
            }
        }
    }


    void World::startFrame()
    {
        if(isSceneAsyncLoadRunning())
            return;

        TIMERASTER_SCOPE(world_StartFrame);
        u32 currentFrame = CURRENTFRAME;

        static SafeArray<u16> cellsResource[CELL_LAYERS_COUNT];
    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
        static SafeArray<u32> flagsResource[CELL_LAYERS_COUNT];
    #endif

        for (u32 iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
        {
            cellsResource[iLayer].clear();
        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            flagsResource[iLayer].clear();
        #endif

            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                buildActiveObjectsList(iLayer, cellsResource[iLayer], flagsResource[iLayer], currentFrame);
            #else
                buildActiveObjectsList(iLayer, cellsResource[iLayer], currentFrame);
            #endif
        }
        
        // called before desactivateOutOfRangeObjects() because this function can add new active objects        
        Scene* pScene = getRootScene();
        
        if(isActive())
        {
            if (m_bAllowUpdate)
                pScene->fillUpdateList(WORLD_MANAGER->getUpdateGroups());
            else if (GAMEMANAGER->isInPause())
            {
                ObjectGroup grp[ObjectGroup::GroupIndex_Max];                
                pScene->fillUpdateList(&grp[0]); // call fillUpdateList for nothing : made to have Synchronized content (SinglePiece groups) be active for current frame
            }
        }

        pScene->getActiveObjectsRecursive(m_activeObjects[m_activeObjectsListIndex]);

        for (u32 iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
        {
            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                loadInRangeResources(iLayer, cellsResource[iLayer], flagsResource[iLayer]);
            #else
                loadInRangeResources(iLayer, cellsResource[iLayer]);
            #endif

            unloadOutOfRangeResources(iLayer); // may unload previously loaded cells

            m_CellsToUnload[iLayer] = cellsResource[iLayer]; // copy current loaded cells for next frame
        }

        desactivateOutOfRangeObjects(currentFrame);
    }
    

    void World::flushPending()
    {
        if(Scene* rootScene = getRootScene())
            rootScene->flushPending(btrue);
    }

    void World::update( f32 _elapsed )
    {
        if(!m_bAllowUpdate || isSceneAsyncLoadRunning())
            return;

        TIMERASTER_SCOPE(world_Update);

#ifdef ITF_SUPPORT_PLUGIN
        if(PLUGINGATEWAY)
            PLUGINGATEWAY->OnWorldUpdated();
#endif
        for (u32 i=0; i<m_particlePoolNumber; ++i)
            m_particleGeneratorPool[i].update(_elapsed, btrue);

        if (mp_afterFxManager)
            mp_afterFxManager->update();

        updateFriezeConnections();
    }

    void World::update2D( f32 _elapsed )
    {
        if(isSceneAsyncLoadRunning())
            return;

        Scene* pScene = getRootScene();
        ITF_ASSERT_MSG(pScene, "The world contains a deleted base scene");
        pScene->fill2DUpdateList(WORLD_MANAGER->getUpdate2DGroups());
    }

    void World::fillDrawList() 
    {
        if(!m_bAllowDraw ||isSceneAsyncLoadRunning())
           return;

        TIMERASTER_SCOPE(world_Draw);

        Scene* pScene = getRootScene();
        ITF_ASSERT_MSG(pScene, "The world contains a deleted base scene");
        pScene->fillDrawList();
    }

    void World::postDraw() 
    {
        if(!m_bAllowDraw ||isSceneAsyncLoadRunning())
           return;

        for (u32 i=0; i<m_particlePoolNumber; ++i)
            m_particleGeneratorPool[i].render();

        if (mp_afterFxManager && !LOADINGSCREEN->isVisible())
            mp_afterFxManager->addFxPrimitive();
    }

    bbool World::isPhysicalReady()
    {
        if(isSceneAsyncLoadRunning())
            return bfalse;

        // Check is physicall ready for boxes requested at prefetch time
        const u32 aabbCount = m_prefetchList.size();
        for(u32 i = 0; i < aabbCount; ++i)
            if(!isPhysicalReady(m_prefetchList[i]->getTargetAABB()))
                return bfalse;

        return getRootScene()->isPhysicalReady();
    }
    
    bbool World::isPhysicalReady(const View& _view)
    {
        return isPhysicalReady(_view.getScreenProjAABB());
    }

    bbool World::isPhysicalReady(const AABB& _aabb)
    {
        if(isSceneAsyncLoadRunning())
            return bfalse;

        // Check is physicall ready for boxes requested at prefetch time

        for (u32 iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
        {
            SafeArray<u16> cellsToLoad;
            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            getCellsFromBox(iLayer, _aabb.getMin(), _aabb.getMax(), cellsToLoad, NULL, FAKE_FRAME, btrue, bfalse);
            #else
            getCellsFromBox(iLayer, _aabb.getMin(), _aabb.getMax(), cellsToLoad, FAKE_FRAME, btrue, bfalse);
            #endif
       
            ITF_VECTOR<Cell>& container = m_cellsContainer[iLayer];
            const u32 numCells = cellsToLoad.size();
            for(u32 i = 0; i < numCells; ++i)
            {
                u32 localCellIndex = cellsToLoad[i];
                ITF_ASSERT(localCellIndex < container.size());
                Cell* pCell = &container[localCellIndex];

                if (!pCell->checkForPhysicalReady())
                    return bfalse;
            }
        }

        return getRootScene()->isPhysicalReady();
    }

    
    bbool   World::slow_isObjectInAnyCell_slow(ObjectRef _objRef)
    {
        /*for ( u32 i = 0; i < CELL_LAYERS_COUNT; i++ )
        {
            ITF_VECTOR<Cell>::iterator it = m_cellsContainer[i].begin();
            ITF_VECTOR<Cell>::iterator end = m_cellsContainer[i].end();

            for (; it != end; ++it)
            {
                Cell & pCell = (*it);
                if (pCell.hasObject(_objRef) )
                    return btrue;
            }
        }*/
        return bfalse;
    }


    Scene* World::getRootScene() const
    {
        if(getSceneCount())
            return getSceneAt(0);
        return NULL;
    }
    
    void World::addScene(Scene* _pScene)
    {
        ITF_ASSERT(_pScene);
        ITF_ASSERT_CRASH(m_sceneList.find(_pScene) < 0, "DAMN");

        _pScene->setWorldID(getRef());
        m_sceneList.push_back(_pScene);

        // If it's the first scene that is added
        if(m_sceneList.size() == 1)
            m_uniqueName = _pScene->getUniqueName();

        SafeArray<BaseObject*> resolvedRefs;
        ID_SERVER->getObjectListNoNull(_pScene->getSubSceneActors(), resolvedRefs);
        for(u32 i = 0; i < resolvedRefs.size(); ++i)
        {
            SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[i]);

            if(Scene* pScene = pSSA->getSubScene())
                addScene(pScene);
            else
            {
                ITF_ASSERT(0);
            }
        }

#ifdef _DEBUG
        check();
#endif // _DEBUG
    }

    void World::addGameMaterialFromFile(const Path& _fileName, ResourceGroup* _client)
    {
        const GameMaterial_Template* gmat = getGameMaterial(_fileName);

        if (!gmat)
            gmat = GAMEINTERFACE->allocateGameMaterial(_fileName);

        if ( !gmat )
        {
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
            ITF_ERROR("Can not create game material %s", _fileName.getString8().cStr());
#endif
            return;
        }

        ITF_ASSERT(_client);
        if (_client)
            _client->addResource(gmat->getResourceGroup().getResource());
    }

    const GameMaterial_Template* World::getGameMaterial(const Path& _fileName)
    {        
        if ( TEMPLATEDATABASE->hasTemplate(_fileName) )
        {
            m_templateClientHandler.addUsedTemplate(_fileName.getStringID());
            return TEMPLATEDATABASE->getTemplate<GameMaterial_Template>(&m_templateClientHandler, _fileName);
        }

        return NULL;
    }

    const GameMaterial_Template* World::getGameMaterial(const StringID& _id)
    {
        m_templateClientHandler.addUsedTemplate(_id);
        return TEMPLATEDATABASE->getTemplate<GameMaterial_Template>(&m_templateClientHandler, _id);
    }

    void World::CreateFxManager( )
    {
        if (!mp_afterFxManager)
           mp_afterFxManager = newAlloc(mId_Scene, AFTERFXManager());
    }

    ITF_ParticleGenerator * World::getParticleGenerator(const ResourceID & _textureID)
    {
        u32 firstEmpty = U32_INVALID;
        for (u32 idx=0; idx < m_particlePoolNumber; idx++)
        {
            if (m_particleGeneratorPool[idx].getActive())
            {
                if ( m_particleGeneratorPool[idx].getParameters()->getTexture() == _textureID)
                    return &m_particleGeneratorPool[idx];
            } else if (firstEmpty == U32_INVALID)
            {
                firstEmpty = idx;
            }
        }
        if (firstEmpty == U32_INVALID)
            return NULL;

        setDefaultParticleGeneratorParams(m_particleGeneratorPool);
        m_particleGeneratorPool[firstEmpty].getParameters()->setTexture(_textureID);
        return &m_particleGeneratorPool[firstEmpty];
    }

    void World::fillDefaultParticleGeneratorParams(ParticleGeneratorParameters *_particleGenParams)
    {
        _particleGenParams->m_maxParticles = 256;
        _particleGenParams->m_grav = Vec3d::Zero;
        _particleGenParams->m_acc = Vec3d::Zero;
        _particleGenParams->m_velocityVar = 0.5f;
        _particleGenParams->m_freq = 9999999.f;
        _particleGenParams->m_initAngle.SetDegrees(0.0f);
        //_particleGenParams->m_angleDelta = 0.f;
        //_particleGenParams->m_angularSpeed = 0.f;
        //_particleGenParams->m_angularSpeedDelta = 0.f;
        _particleGenParams->m_angleDelta.SetDegrees(1000.f);
        _particleGenParams->m_angularSpeed.SetDegrees(100.f);
        _particleGenParams->m_angularSpeedDelta.SetDegrees(500.f);
        _particleGenParams->m_timeTarget = 0.f;
        _particleGenParams->m_blendMode = GFX_BLEND_ALPHA;
        _particleGenParams->m_genGenType = PARGEN_GEN_POINTS;
        _particleGenParams->m_genMode = PARGEN_MODE_COMPLEX;

        _particleGenParams->setNumberPhase(1);
        //_particleGenParams->setNumberPhase(3);

        //_particleGenParams->setPhaseTime(0, 0.2f);
        //_particleGenParams->setPhaseColorMin(0, 0);
        //_particleGenParams->setPhaseColorMax(0, 0);

        //_particleGenParams->setPhaseTime(1, 0.5f);
        //_particleGenParams->setPhaseColorMin(1, 1);
        //__particleGenParams->setPhaseColorMax(1, 1);

        //_particleGenParams->setPhaseTime(2, 0.2f);
        //_particleGenParams->setPhaseColorMin(2, 0);
        //_particleGenParams->setPhaseColorMax(2, 0);
    }

    void World::setDefaultParticleGeneratorParams(ITF_ParticleGenerator *_particleGen)
    {
        _particleGen->stopGeneration();
        _particleGen->reInit();
        ParticleGeneratorParameters* params =  _particleGen->getParameters();
        
        fillDefaultParticleGeneratorParams(params);

        _particleGen->setMaxParticles(params->m_maxParticles);

    }

    void World::onDeleteScene( const Scene* _pScene )
    {
        for(u32 i = 0; i < getSceneCount(); ++i)
        {
            if(getSceneAt(i) == _pScene)
            {
                m_sceneList.eraseNoOrder(i);
                break;
            }
        }
    }


    void World::setActive(bbool _status)
    {
        if(!isSceneAsyncLoadRunning())
        {
            Scene* pScene = getRootScene();
            ITF_ASSERT_MSG(pScene, "The world contains a deleted base scene");
            pScene->setActive(_status);
        }

        m_bActive = _status;
    }

    void World::setAlwaysActive(bbool _status)
    {
        if(!isSceneAsyncLoadRunning())
        {
            Scene* pScene = getRootScene();
            ITF_ASSERT_MSG(pScene, "The world contains a deleted base scene");
            pScene->setIsAlwaysActive(_status);
        }

        m_bAlwaysActive = _status;

        if(m_bAlwaysActive)
        {
            setActive(m_bAlwaysActive);
        }
    }

    Cell* World::getCellAtPos(const u32 _layerIndex, const Vec2d& _atpos)
    {
        i32 x = 0, y = 0;

        if (!getCellIndexAtPos(_layerIndex, _atpos, x, y))
            return NULL;


        u16* indexes = m_cellsIndexes[_layerIndex];
        ITF_VECTOR<Cell>& container = m_cellsContainer[_layerIndex];

        u32 index  = indexes[y * m_horizCellsCount[_layerIndex] + x];
        if (index == m_invalidCellIndex)
            return NULL;

        ITF_ASSERT(index < container.size());
        return &container[index];
    }
    
    ITF_INLINE f32 getClampMin(const f32 v)
    {
        return floorf(v);
    }

    ITF_INLINE f32 getClampMax(const f32 v)
    {
        return ceilf(v);
    }

    void World::prefetchResource()
    {
        stopPrefetch();
        m_activeObjects[0].clear();
        m_activeObjects[1].clear();
        SafeArray<u16> cellsResource[CELL_LAYERS_COUNT];
        for (int iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
            clearCellsEnable(iLayer);

        for(u32 i = 0; i < getSceneCount(); ++i)
        {
            Scene* pScene = getSceneAt(i);

            const PickableList &lst = pScene->getPickableList();
            for(u32 k= 0; k < lst.size(); ++k)
            {
                Pickable* pObj = lst[k];

                pObj->registerInWorldCells();
            }
        }

        const u32 uPrefetchDestCount = m_prefetchTargets.size();
        for(u32 i = 0; i < uPrefetchDestCount; ++i)
        {
            const ObjectPath& objPath = m_prefetchTargets[i];
            Pickable* pObj = SceneObjectPathUtils::getObjectFromAbsolutePath(objPath);

            if(pObj)
            {
                AABB aabb;
                CAMERACONTROLLERMANAGER->predictViewAABB(aabb, pObj->get2DPos());
                // GFX_ADAPTER->drawDBGAABB(aabb, Color::green(), 20.f, 5.f);

                for (int iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
                {
                    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                        getCellsFromBox(iLayer, aabb.getMin(), aabb.getMax(), cellsResource[iLayer], NULL, FAKE_FRAME);
                    #else // !ENABLE_CELL_FLAGS_FAST_DISCARD
                        getCellsFromBox(iLayer, aabb.getMin(), aabb.getMax(), cellsResource[iLayer], FAKE_FRAME);
                    #endif // !ENABLE_CELL_FLAGS_FAST_DISCARD
                }

                View* pView = View::createView("Prefetch", bfalse, btrue);
                pView->setTargetAABB(aabb);
                m_prefetchList.push_back(pView);
            }
            else
            {
                ITF_STDSTRING str;
                objPath.toString(str);
                ITF_ERROR("Prefetch: can't find object from ObjPath : '%s'", str.c_str());
            }
        }

        if(uPrefetchDestCount == 0)
        {
            const Actor*  pStartCheckPoint = GAMEMANAGER->getCheckpointFromIndex(getRef(), 0, btrue);
            //ITF_ASSERT_MSG(pStartCheckPoint, "Can't prefetch without first checkpoint"); // Some scenes doesn't have any checkpoint
            if (!pStartCheckPoint)
                return;
            
            AABB aabb;
            CAMERACONTROLLERMANAGER->predictViewAABB(aabb, pStartCheckPoint->get2DPos());
            //GFX_ADAPTER->drawDBGAABB(aabb, Color::green(), 20.f, 5.f);

            for (int iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
            {
                #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                    getCellsFromBox(iLayer, aabb.getMin(), aabb.getMax(), cellsResource[iLayer], NULL, FAKE_FRAME);
                #else // !ENABLE_CELL_FLAGS_FAST_DISCARD
                    getCellsFromBox(iLayer, aabb.getMin(), aabb.getMax(), cellsResource[iLayer], FAKE_FRAME);
                #endif // !ENABLE_CELL_FLAGS_FAST_DISCARD
            }
            
            View* pView = View::createView("Default prefetch to first checkpoint", bfalse, btrue);
            pView->setTargetAABB(aabb);
            m_prefetchList.push_back(pView);
        }

        BaseObjectList objectToLoad;
        for (int iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
        {
            SafeArray<u16>& cellsres = cellsResource[iLayer]; 
            const ITF_VECTOR<Cell>& container = m_cellsContainer[iLayer];
            const u32 uCellResCount = cellsres.size();
            for (u32 index = 0; index < uCellResCount; ++index)
            {
                u32 localCellIndex = cellsres[index];
                ITF_ASSERT(localCellIndex < container.size());
                const Cell* pCurCell = &container[localCellIndex];

                ITF_ASSERT_MSG( pCurCell, "a cell can't be null at this point");

            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                const SafeArray<Cell::CellObjData>& objInCells = pCurCell->getAllObjects();
            #else
                const ObjectRefList& objInCells = pCurCell->getAllObjects();
            #endif
                for (u32 objIdx=0;objIdx<objInCells.size();++objIdx)
                {
                #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                    const ObjectRef& obj = objInCells[objIdx].m_obj;
                #else
                    const ObjectRef& obj = objInCells[objIdx];
                #endif
                    objectToLoad.addObject(obj, bfalse);
                }
            }
        }
        
        tempObjArray.clear();
        ID_SERVER->getObjectListNoNullV2(objectToLoad, &tempObjArray);
        for (u32 index = 0;index< tempObjArray.size();++index)
        {
            ((Pickable*)tempObjArray[index])->loadResources();
        }
        if (LOADINGSCREEN->isVisible())
            RESOURCE_MANAGER->flushResourcesForDestroy();
    }

    void World::stopPrefetch()
    {
        // Check is physicall ready for boxes requested at prefetch time
        const u32 aabbCount = m_prefetchList.size();
        for(u32 i = 0; i < aabbCount; ++i)
        {
            View* v = m_prefetchList[i];
            View::deleteView(v);
        }
        m_prefetchList.clear();
    }

    void World::registerOrRemoveObjectInCells(Pickable* pObj, RegisterInCellAction _action, const AABB* _exclusionAABB, i8 _exclusionLayer)
    {
        i8 iChosenLayer = 0;
        
        bbool isCellUpdateDataValid = pObj->isLastCellUpdateDataValid();
        ITF_ASSERT_MSG(pObj->getObjectType() != BaseObject::ePolyLine, "polylines must be updated by their owner");
		ITF_ASSERT_CRASH(pObj->isCellObjectType(), "This object type is not supposed to be registered in cells.");

        AABB localExclusionAABBForAdd;
        if (!pObj->isAllowedForCell())
        {
            ITF_ASSERT(!slow_isObjectInAnyCell_slow(pObj->getRef()));
            return;
        }

        AABB aabb;
        f32 deltaX = 0.f;
        f32 deltaY = 0.f;
        if (CAMERA)
        {
            deltaX = CAMERA->getDeltaX();
            deltaY = CAMERA->getDeltaY();
        }

        if (_action == AddInCells)
        {
            aabb = pObj->getAABB();
            if(SubSceneActor* pSSA = pObj->getScene()->getSubSceneActorHolder())
            {
                pSSA->growInternalRelativeAABBFromAbsolute(aabb, pObj->getDepth());
            }
            aabb.projectTo2D(deltaX,deltaY,pObj->getDepth());

            iChosenLayer = 0;
            for(; iChosenLayer < (i8)CELL_LAYERS_COUNT-1; iChosenLayer++)
            {
                if (
                    (aabb.getWidth() < m_CellSize[iChosenLayer].m_x) &&
                    (aabb.getHeight() < m_CellSize[iChosenLayer].m_y)
                    )
                {
                    if(iChosenLayer > 0)
                    {
                        const f32 curLayerArea = m_CellSize[iChosenLayer].m_x * m_CellSize[iChosenLayer].m_y;
                        const f32 prevLayerArea = curLayerArea * 0.25f;
                        const f32 midArea = (curLayerArea + prevLayerArea) * 0.5f;
                        const f32 objAABBArea = aabb.getArea();

                        if(objAABBArea < midArea)
                            iChosenLayer--;
                    }

                    break;
                }
            }

            if ( !pObj->updateCellsNeeded(aabb, iChosenLayer ) )
                return;

        //    GFX_ADAPTER->drawDBGAABB(pObj->getAABB(), 1, 0, 0, 0, 1, pObj->getDepth());

            if (isCellUpdateDataValid)
                registerOrRemoveObjectInCells(pObj, RemoveFromCells, &aabb, iChosenLayer);

            /* TODO check
            if (pObj->isLastCellUpdateDataValid())
            {
                localExclusionAABBForAdd = pObj->getLastCellUpdateData();
                _exclusionAABB = &localExclusionAABBForAdd;
            }
            */

    #ifdef TRACK_PICKABLE_FRIENDLY_NAME
            if (pObj->getUserFriendly().equals(TRACK_PICKABLE_FRIENDLY_NAME, btrue))
            {
                LOG("[Track pickable activation] Frame: %u World::registerOrRemoveObjectInCells. Layer %d", CURRENTFRAME, iChosenLayer);
            }
    #endif

            pObj->updateCellUpdateData(aabb, iChosenLayer);
        }
        else if (_action == RemoveFromCells)
        {
            if (!isCellUpdateDataValid)
                return;
            aabb = pObj->getLastCellUpdateData().m_aabb;
            iChosenLayer = (u8)pObj->getLastCellUpdateData().m_layerIndex;
     //       GFX_ADAPTER->drawDBGAABB(aabb, 0, 0, 1);

            if(iChosenLayer < 0)
                return;
        }

        ITF_ASSERT(iChosenLayer >= 0);
        ITF_ASSERT(iChosenLayer < CELL_LAYERS_COUNT);

        f32 sizeGridX = m_CellSize[iChosenLayer].m_x;
        f32 sizeGridY = m_CellSize[iChosenLayer].m_y;


        i32 parseCellsX  = (i32)getClampMin(((aabb.getMin().m_x + fabsf(m_fminCellXCoord))/sizeGridX));
        i32 parseCellsY  = (i32)getClampMin(((aabb.getMin().m_y + fabsf(m_fminCellYCoord))/sizeGridY));
        i32 endParseCellsX  = (i32)getClampMax(((aabb.getMax().m_x + fabsf(m_fminCellXCoord))/sizeGridX));
        i32 endParseCellsY  = (i32)getClampMax(((aabb.getMax().m_y + fabsf(m_fminCellYCoord))/sizeGridY));
        
        if (parseCellsX < 0)
            parseCellsX = 0;

        if (endParseCellsX > m_horizCellsCount[iChosenLayer]-1)
            endParseCellsX = m_horizCellsCount[iChosenLayer]-1;

        if (parseCellsY < 0)
            parseCellsY = 0;

        if (endParseCellsY > m_vertCellsCount[iChosenLayer]-1)
            endParseCellsY = m_vertCellsCount[iChosenLayer]-1;

        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
        u32 cellflags = Cell::cellObjectFlag_FirstLine;
        #endif

        i32 startParseCellsX = parseCellsX;

        i32 exclusionAABBMinCellX = 1000000;
        i32 exclusionAABBMinCellY = 1000000;
        i32 exclusionAABBMaxCellX = -1000000;
        i32 exclusionAABBMaxCellY = -1000000;
        if (_exclusionAABB && _exclusionLayer == iChosenLayer)
        {
            exclusionAABBMinCellX  = (i32)getClampMin(((_exclusionAABB->getMin().m_x + fabsf(m_fminCellXCoord))/sizeGridX));
            exclusionAABBMinCellY  = (i32)getClampMin(((_exclusionAABB->getMin().m_y + fabsf(m_fminCellYCoord))/sizeGridY));
            exclusionAABBMaxCellX  = (i32)getClampMax(((_exclusionAABB->getMax().m_x + fabsf(m_fminCellXCoord))/sizeGridX));
            exclusionAABBMaxCellY  = (i32)getClampMax(((_exclusionAABB->getMax().m_y + fabsf(m_fminCellYCoord))/sizeGridY));
        }

/*        if (pObj->getObjectType() == BaseObject::eActor)
        {
            if (!pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                if (abs(endParseCellsY-parseCellsY) > 3)
                {
                    LOG("!!WARNING!! frame %d Action:%d Actor %s (%d) is across %d cells in Y (aabb height: %f, unprojected:%f)\n", CURRENTFRAME, (u32)_action, pObj->getUserFriendly().cStr(), (u32)pObj, abs(endParseCellsY-parseCellsY), aabb.getHeight(), pObj->getAABB().getHeight());
                }
                if (abs(endParseCellsX-parseCellsX) > 3)
                {
                    LOG("!!WARNING!! frame %d Action:%d Actor %s (%d) is across %d cells in X (aabb width: %f, unprojected:%f)\n", CURRENTFRAME, (u32)_action, pObj->getUserFriendly().cStr(), (u32)pObj, abs(endParseCellsX-parseCellsX), aabb.getWidth(), pObj->getAABB().getWidth());
                }
            }
        }*/
        for (; parseCellsY <= endParseCellsY; parseCellsY++)
        {
            const bbool skipY = (parseCellsY >= exclusionAABBMinCellY && parseCellsY <= exclusionAABBMaxCellY);

        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
        cellflags |= Cell::cellObjectFlag_FirstColumn;
        #endif
        for (parseCellsX = startParseCellsX; parseCellsX <= endParseCellsX; parseCellsX++)
        {
            if(skipY && (parseCellsX >= exclusionAABBMinCellX && parseCellsX <= exclusionAABBMaxCellX))
                continue;

            Cell* pCellFound = getCellAtIndex(iChosenLayer, (u16)parseCellsX, (u16)parseCellsY);            
            if (_action == RemoveFromCells)
            {
                if (pCellFound)
                    pCellFound->removeObject(pObj->getRef());
            }
            else
            {
                if (!pCellFound)
                    pCellFound = &createCellAt(iChosenLayer, parseCellsX, parseCellsY);
                f32 depth = pObj->getDepth();

                if (depth<m_minZEverRegistered)
                    m_minZEverRegistered = depth;
                if (depth>m_maxZEverRegistered)
                    m_maxZEverRegistered = depth;

                #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                pCellFound->addObject(pObj->getRef(), iChosenLayer, cellflags, pObj->getCellEnableData().m_hasEverBeenRegisteredInCells);
                cellflags &= ~Cell::cellObjectFlag_FirstColumn;
                #else
                pCellFound->addObject(pObj->getRef(), iChosenLayer, pObj->getCellEnableData().m_hasEverBeenRegisteredInCells);
                #endif
                if (!isCellUpdateDataValid && pObj->getObjectType()==BaseObject::eActor)
                {
                    Actor *act = (Actor*)pObj;
                    if (act->isSpawned())
                    {
                        pCellFound->signalIgnoreLoadResourceMask();

                    }
                }
            }
        }
        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
        cellflags &= ~Cell::cellObjectFlag_FirstLine;
        #endif
    }
#ifdef ASSERT_ENABLED
        if (_action != AddInCells)
        {
            //ITF_ASSERT(!slow_isObjectInAnyCell_slow(pObj->getRef()));
       /*     if (slow_isObjectInAnyCell_slow(pObj->getRef()))
            {
                LOG("dirty cells for %s", pObj->getUserFriendly().cStr());
            }*/
        }
#endif //ASSERT_ENABLED

        if (_action == AddInCells)
            pObj->getCellEnableData().m_hasEverBeenRegisteredInCells = 1;
    }


    void World::createCellIndexes()
    {
        ITF_ASSERT_MSG(LAYER_0_MAX_HORIZ_CELLS * LAYER_0_MAX_VERT_CELLS < 65536, "cells count overrides u16 max");
        for (int iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
        {
            if (!m_cellsIndexes[iLayer])
                m_cellsIndexes[iLayer] = newAlloc(mId_Scene, u16[m_horizCellsCount[iLayer] * m_vertCellsCount[iLayer]]); 
            
            ITF_MemSet(m_cellsIndexes[iLayer], 0xff, sizeof(u16) * m_horizCellsCount[iLayer] * m_vertCellsCount[iLayer]);
        }
    }

    void World::setIgnoreCells(bbool _val)
    { 
        if(_val)
        {
            ignoreCells();
        }
        else
        {
            createCellIndexes();
        }
    }
    
    void World::activeAll()
    {
        for(u32 i = 0; i < getSceneCount(); ++i)
            getSceneAt(i)->activeAll();
    }
    
    void World::forceLoadResourcesAndDisableUnload()
    {
        m_bAllowUnload = bfalse;

        for(u32 i = 0; i < getSceneCount(); ++i)
            getSceneAt(i)->forceLoadResourcesAndDisableUnload();
    }

    void World::ignoreCells()
    {
        for(u32 i = 0 ; i < getSceneCount(); ++i)
            getSceneAt(i)->disconnectFromCells();

        for (int iLayer = 0; iLayer < CELL_LAYERS_COUNT; iLayer++)
            SF_DEL_ARRAY(m_cellsIndexes[iLayer]);
    }

    void World::clearCellsEnable(u32 _iLayer)
    {
        SafeArray<u16>& enableList = m_CellsEnable[_iLayer];
        ITF_VECTOR<Cell>& cellsContainer = m_cellsContainer[_iLayer];

        for (u32 i = 0; i < enableList.size(); ++i)
        {
            u32 localCellIndex = enableList[i];
            ITF_ASSERT(localCellIndex < cellsContainer.size());
            cellsContainer[localCellIndex].disable();
        }
        enableList.clear();
    }

    void World::intersectPolygonWithNegativePlane(const FixedArray<Vec3d, 64> &_in, FixedArray<Vec3d,64> &_out, const Plane &_plane)
    {
        u32 inSize = _in.size();
        _out.clear();

        if (inSize==0)
            return;


        const Vec3d *prevPoint = &_in[0];
        bbool prevSide = !_plane.isPointInPositiveSide(*prevPoint);

        if (inSize==1)
        {
            if (prevSide)
                _out.push_back(*prevPoint);

            return;
        }

        f32 dummyK = 0;
        Vec3d inter;

        for (u32 i=1; i<=inSize; i++)
        {

            if (prevSide)
                _out.push_back(*prevPoint);
            const Vec3d &pt = (i==inSize)?_in[0]:_in[i]; //we don't care about optimization here
            bbool currentSide = !_plane.isPointInPositiveSide(pt);
            if (currentSide!=prevSide)
            {
                //intersect
                if (_plane.testLineIntersection(*prevPoint, pt-*prevPoint, inter, dummyK))
                {
                    _out.push_back(inter);
                }
            }

            prevSide = currentSide;
            prevPoint = &pt;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool World::projectFrustumOnPlane(Camera *_cam, f32 _Z, bbool _useNear, AABB &_screenProjectionOnGrid)
    {
        static FixedArray<Vec3d,64> cellShapes[2];

        const Plane &left = _cam->m_frustumPlanes[Camera::CameraPlane_Left];
        const Plane &right = _cam->m_frustumPlanes[Camera::CameraPlane_Right];
        const Plane &bottom = _cam->m_frustumPlanes[Camera::CameraPlane_Bottom];
        const Plane &top = _cam->m_frustumPlanes[Camera::CameraPlane_Top];

        cellShapes[0].clear();
        cellShapes[0].push_back(Vec3d(MIN_I16_RANGE,MIN_I16_RANGE,_Z));
        cellShapes[0].push_back(Vec3d(MAX_I16_RANGE,MIN_I16_RANGE,_Z));
        cellShapes[0].push_back(Vec3d(MAX_I16_RANGE,MAX_I16_RANGE,_Z));
        cellShapes[0].push_back(Vec3d(MIN_I16_RANGE,MAX_I16_RANGE,_Z));
        /*
        cellShapes[0].push_back(Vec3d(m_fminCellXCoord, m_fminCellYCoord,_Z));
        cellShapes[0].push_back(Vec3d(m_fmaxCellXCoord,m_fminCellYCoord,_Z));
        cellShapes[0].push_back(Vec3d(m_fmaxCellXCoord,m_fmaxCellYCoord,_Z));
        cellShapes[0].push_back(Vec3d(m_fminCellXCoord,m_fmaxCellYCoord,_Z));
*/
        ITF_ASSERT_MSG(left.m_normal.sqrnorm()!=0, "frustum planes are not implemented or NULL - they are mandatory");

        FixedArray<Vec3d,64> *inputBuffer = &cellShapes[0];
        FixedArray<Vec3d,64> *outputBuffer = &cellShapes[1];

        if (_useNear)
        {
            const Plane &nearPlane = _cam->m_frustumPlanes[Camera::CameraPlane_Near];
            intersectPolygonWithNegativePlane(*inputBuffer, *outputBuffer, nearPlane);
            std::swap(inputBuffer, outputBuffer);
        }

        intersectPolygonWithNegativePlane(*inputBuffer, *outputBuffer, left);
        std::swap(inputBuffer, outputBuffer);
        intersectPolygonWithNegativePlane(*inputBuffer, *outputBuffer, right);
        std::swap(inputBuffer, outputBuffer);
        intersectPolygonWithNegativePlane(*inputBuffer, *outputBuffer, top);
        std::swap(inputBuffer, outputBuffer);
        intersectPolygonWithNegativePlane(*inputBuffer, *outputBuffer, bottom);


        AABB screenProjectionOnGrid;
        u32 cellShapesSize=cellShapes[0].size();
        if (cellShapesSize)
        {
            _screenProjectionOnGrid.setMin(cellShapes[0][0]);
            _screenProjectionOnGrid.setMax(cellShapes[0][0]);
            for (u32 i=0; i<cellShapesSize; i++) 
            {
                _screenProjectionOnGrid.grow(cellShapes[0][i]);
            }

            _screenProjectionOnGrid.projectTo2D(_cam->getDeltaX(), _cam->getDeltaY(),_Z);
            return btrue;
        }
        else
        {
            const Vec3d &pos = _cam->getCorrectedPos();
            _screenProjectionOnGrid.setMin(pos);
            _screenProjectionOnGrid.setMax(pos);
            _screenProjectionOnGrid.projectTo2D(_cam->getDeltaX(), _cam->getDeltaY(), pos.m_z);
            return bfalse;
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool World::projectViewToAABB(View* view, AABB& screenProjectionOnGrid)
    {
            if (view->isLocked())
            {
                screenProjectionOnGrid = view->getScreenProjAABB();
                return btrue;
            }
            screenProjectionOnGrid.invalidate();

            switch(view->getViewType())
            {
            case View::viewType_Camera:
                {
                    Camera* camera = view->getCamera();
                    ITF_ASSERT(camera);
                    if(!camera)
                    return false;

                    if (camera->isBiased())
                    {
                        AABB intersectionWithPlane;

                        if (projectFrustumOnPlane(camera, 0, bfalse, intersectionWithPlane))
                        {
                            screenProjectionOnGrid.grow(intersectionWithPlane);
                        }

                        if (projectFrustumOnPlane(camera, m_minZEverRegistered, bfalse, intersectionWithPlane))
                        {
                            screenProjectionOnGrid.grow(intersectionWithPlane);
                        }

                        if (projectFrustumOnPlane(camera, m_maxZEverRegistered, bfalse, intersectionWithPlane))
                        {
                            screenProjectionOnGrid.grow(intersectionWithPlane);
                        }

                        const Vec3d &camPos = camera->getCorrectedPos();
                        AABB cameraPosBB(camPos);
                        cameraPosBB.projectTo2D(camera->getDeltaX(), camera->getDeltaY(), camPos.m_z);

                        screenProjectionOnGrid.grow(cameraPosBB);
                    }
                    else
                    {
                        projectFrustumOnPlane(camera, 0, bfalse, screenProjectionOnGrid);
                        ITF_ASSERT_MSG(screenProjectionOnGrid.isValid(), 
                            "screenProjectionOnGrid bounding box is invalid in unbiased mode. This is not good at all");
                    }
                }
                break;

            case View::viewType_AABB:
                    screenProjectionOnGrid = view->getTargetAABB();
                break;

            default:
                    ITF_ASSERT(0);
                break;
            }

            ITF_ASSERT(screenProjectionOnGrid.isValid());
            if (screenProjectionOnGrid.isValid())
            {
#if 0
                //debug display
                for (u32 pointIndex = 0; pointIndex<cellShapesSize; pointIndex++)
                {
                    GFX_ADAPTER->drawDBG3DLine(cellShapes[0][pointIndex],cellShapes[0][(pointIndex+1)%cellShapesSize]);
                }
#endif
        }
        view->updateScreenProjAABB(screenProjectionOnGrid);
        return btrue;
    }



    #define WORLD_AABB_PROCESS 0
    #define WORLD_AABB_NO_PROCESS 1
    #define WORLD_AABB_RESIZE 2
    ITF_INLINE u32 getAABBOverlapType(const AABB& _main, const AABB& _b2)
    {
        f32 minX1 = _main.getMin().m_x;
        f32 minY1 = _main.getMin().m_y;
        f32 maxX1 = _main.getMax().m_x;
        f32 maxY1 = _main.getMax().m_y;

        f32 minX2 = _b2.getMin().m_x;
        f32 minY2 = _b2.getMin().m_y;
        f32 maxX2 = _b2.getMax().m_x;
        f32 maxY2 = _b2.getMax().m_y;

        const float triggeringSurface = ((maxX2-minX2) * (maxY2-minY2)) * 0.5f; // Half of box 2 surface


        if (minX2 > maxX1 || minX1 > maxX2 || minY2 > maxY1 || minY1 > maxY2)
            return WORLD_AABB_PROCESS; // boxes don't overlap --> process box 2

        if (minX2 >= minX1 && maxX2 <= maxX1 && minY2 >= minY1 && maxY2 <= maxY1)   // early escape : 2 is inside main
            return WORLD_AABB_NO_PROCESS;  // box 2 is inside main --> don't process box 2

        if (minX1 >= minX2 && maxX1 <= maxX2 && minY1 >= minY2 && maxY1 <= maxY2) 
            return WORLD_AABB_PROCESS;  // main is inside box 2 --> process box 2

        f32 delta = minX1 - minX2;
        if (delta >= 0.f)
        {
            f32 surface = delta * (maxY2-minY2);
            if (surface > triggeringSurface)
                return WORLD_AABB_PROCESS; // boxes overlap : process box 2
        }

        delta = maxX2 - maxX1;
        if (delta >= 0.f)
        {
            f32 surface = delta * (maxY2-minY2);
            if (surface > triggeringSurface)
                return WORLD_AABB_PROCESS; // boxes overlap : process box 2
        }

        delta = minY1 - minY2;
        if (delta >= 0.f)
        {
            f32 surface = delta * (maxX2-minX2);
            if (surface > triggeringSurface)
                return WORLD_AABB_PROCESS; // boxes overlap : process box 2
        }

        delta = maxY2 - maxY1;
        if (delta >= 0.f)
        {
            f32 surface = delta * (maxX2-minX2);
            if (surface > triggeringSurface)
                return WORLD_AABB_PROCESS; // boxes overlap : process box 2
        }

        return WORLD_AABB_RESIZE; // boxes overlap too much to process box 2 --> just update box 2
    }

    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
    void World::getCellsFromCamera(u32 _iLayer, SafeArray<u16>& _cellsResource, SafeArray<u32>* _flagsArray)
    #else
    void World::getCellsFromCamera(u32 _iLayer, SafeArray<u16>& _cellsResource)
    #endif
    {
        clearCellsEnable(_iLayer);
        const SafeArray<View*>& views = View::getViews();
        View* pMainView = View::getMainView();
        ITF_ASSERT(pMainView || !views.size());
        AABB mainViewProjectionOnGrid;
        mainViewProjectionOnGrid.invalidate();
        if (pMainView)
        {
            ITF_ASSERT(pMainView == views[0]);
            projectViewToAABB(pMainView, mainViewProjectionOnGrid);
        }
	
		View* buffer[8];		SafeArray<View*> activeViews(8,MemoryId::mId_Temporary,buffer);
        for (u32 i = 0; i < views.size(); i++)
        {
            View* view = views[i];
            if(view->isActive())
                activeViews.push_back(view);
        }
        activeViews.mergeArray(m_prefetchList); // use prefetch views only for this world

        for (u32 i = 0; i < activeViews.size(); i++)
        {
            View* view = activeViews[i];

            const u32 frame = (view->getPrefetchDataOnly() ? FAKE_FRAME : CURRENTFRAME);

            AABB screenProjectionOnGrid;
            
            if (view == pMainView)
            {
                screenProjectionOnGrid = mainViewProjectionOnGrid;
                #ifdef DEBUG_VIEWS_LOADING
                    GFX_ADAPTER->drawDBGAABB(mainViewProjectionOnGrid, 1, 0, 0, 0, 3, 0);
                #endif
            }
            else
            {
                if (!projectViewToAABB(view, screenProjectionOnGrid))
                    continue;
                if (u32 overlap = getAABBOverlapType(mainViewProjectionOnGrid, screenProjectionOnGrid))
                {
                    if (pMainView && (overlap == WORLD_AABB_RESIZE))
                    {
                        view->updateScreenProjAABB(mainViewProjectionOnGrid);
                        #ifdef DEBUG_VIEWS_LOADING
                            if (IsDebuggerPresent() && 0 == _iLayer)
                            {
                                String8 viewName;
                                viewName.setTextFormat("discard view: %s: too close from main view", view->getName().cStr());
                                GFX_ADAPTER->drawDBGText(viewName.cStr());
                            }
                        #endif
                    }
                    #ifdef DEBUG_VIEWS_LOADING
                        GFX_ADAPTER->drawDBGAABB(screenProjectionOnGrid, 0, 1, 0, 0, 3, 0);
                    #endif
                    continue;
                }

                #ifdef DEBUG_VIEWS_LOADING
                    if (IsDebuggerPresent() && 0 == _iLayer)
                    {
                        String8 viewName;
                        viewName.setTextFormat("extra view: %s", view->getName().cStr());
                        GFX_ADAPTER->drawDBGText(viewName.cStr());
                        GFX_ADAPTER->drawDBGAABB(screenProjectionOnGrid, 0, 0, 1, 0, 3, 0);
                    }
                #endif
            }


            const Vec2d bordLetfUp = screenProjectionOnGrid.getMin();
            const Vec2d bordRightDown = screenProjectionOnGrid.getMax();

            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            getCellsFromBox(_iLayer, bordLetfUp,bordRightDown,_cellsResource, _flagsArray, frame);
            #else
            getCellsFromBox(_iLayer, bordLetfUp,bordRightDown,_cellsResource, frame);
            #endif
        }
    }



#define CHECK_BORDERS\
    if (finalBorderLeftUp.m_x > finalBorderRightDown.m_x)\
    {\
    f32 save = finalBorderRightDown.m_x;\
    finalBorderRightDown.m_x = finalBorderLeftUp.m_x;\
    finalBorderLeftUp.m_x = save;\
    }\
    if (finalBorderLeftUp.m_y < finalBorderRightDown.m_y)\
    {\
    f32 save = finalBorderRightDown.m_y;\
    finalBorderRightDown.m_y = finalBorderLeftUp.m_y;\
    finalBorderLeftUp.m_y = save;\
    }

    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
    void World::getCellsFromBox(u32 _iLayer, const Vec2d& _bordLetfUp, const Vec2d& _bordRightDown, SafeArray<u16>& _cellsResource, SafeArray<u32>* _flagsArray, u32 _frame, bbool _checkCellDuplicate/* = bfalse*/, bbool _autoEnable/* = btrue*/)
    #else
    void World::getCellsFromBox(u32 _iLayer, const Vec2d& _bordLetfUp, const Vec2d& _bordRightDown, SafeArray<u16>& _cellsResource, u32 _frame, bbool _checkCellDuplicate/* = bfalse*/, bbool _autoEnable/* = btrue*/)
    #endif
    {
        const i32       MAX_WINDOW_SIZE = 100;

        if (!cellEverDataIsValid(_iLayer))
            return;

        u16* cellsIndexes = m_cellsIndexes[_iLayer];

        // update world boundaries
        if (m_minCellPosEverRegistered[_iLayer].m_x > m_maxCellPosEverRegistered[_iLayer].m_x)
        {
            f32 save = m_maxCellPosEverRegistered[_iLayer].m_x;
            m_maxCellPosEverRegistered[_iLayer].m_x = m_minCellPosEverRegistered[_iLayer].m_x;
            m_minCellPosEverRegistered[_iLayer].m_x = save;
        }

        if (m_minCellPosEverRegistered[_iLayer].m_y > m_maxCellPosEverRegistered[_iLayer].m_y)
        {
            f32 save = m_maxCellPosEverRegistered[_iLayer].m_y;
            m_maxCellPosEverRegistered[_iLayer].m_y = m_minCellPosEverRegistered[_iLayer].m_y;
            m_minCellPosEverRegistered[_iLayer].m_y = save;
        }

        Vec2d finalBorderLeftUp, finalBorderRightDown;


        finalBorderLeftUp = _bordLetfUp;                // left up : smallest X, highest Y
        finalBorderRightDown = _bordRightDown;          // right sown : biggest x, smallest Y

        CHECK_BORDERS
            
        if (finalBorderLeftUp.m_x < m_minCellPosEverRegistered[_iLayer].m_x)
            finalBorderLeftUp.m_x = m_minCellPosEverRegistered[_iLayer].m_x;
        if (finalBorderLeftUp.m_y > m_maxCellPosEverRegistered[_iLayer].m_y)
            finalBorderLeftUp.m_y = m_maxCellPosEverRegistered[_iLayer].m_y;
        if (finalBorderRightDown.m_x > m_maxCellPosEverRegistered[_iLayer].m_x)
            finalBorderRightDown.m_x = m_maxCellPosEverRegistered[_iLayer].m_x;
        if (finalBorderRightDown.m_y < m_minCellPosEverRegistered[_iLayer].m_y)
            finalBorderRightDown.m_y = m_minCellPosEverRegistered[_iLayer].m_y;

        CHECK_BORDERS // because of above tests

        i32 indexLeftX = 0, indexLeftY = 0, indexRightX = 0, indexRightY = 0;

        
        if (getCellIndexAtPos(_iLayer, finalBorderLeftUp,indexLeftX,indexLeftY) &&
            getCellIndexAtPos(_iLayer, finalBorderRightDown,indexRightX,indexRightY)) 
        {
            if(FAKE_FRAME != _frame)
            {
                //for the activation range
                {
                    ITF_ASSERT(indexRightX >= indexLeftX);
                    ITF_ASSERT(indexRightY <= indexLeftY);

                    if (indexRightX - indexLeftX > MAX_WINDOW_SIZE) // avoids activating too many cells (happens with bias camera mode)
                    {
                        i32 center = (indexLeftX + indexRightX)/2;
                        indexLeftX = center - MAX_WINDOW_SIZE / 2;
                        indexRightX = center + MAX_WINDOW_SIZE / 2;
                    }

                    if (indexLeftY - indexRightY > MAX_WINDOW_SIZE)  // avoids activating too many cells (happens with bias camera mode)
                    {
                        i32 center = (indexLeftY + indexRightY)/2;
                        indexLeftY = center + MAX_WINDOW_SIZE / 2;
                        indexRightY = center - MAX_WINDOW_SIZE / 2;
                    }

                    i32 startX    = indexLeftX   - CellsLayersSettings::getUpdateCellsRange(_iLayer);
                    i32 endX      = indexRightX  + CellsLayersSettings::getUpdateCellsRange(_iLayer);

                    i32 startY    = indexRightY  - CellsLayersSettings::getUpdateCellsRange(_iLayer);
                    i32 endY      = indexLeftY   + CellsLayersSettings::getUpdateCellsRange(_iLayer);

                    if (startY < 0)
                        startY = 0;
                    if (startX < 0)
                        startX = 0;
                    if (endY >= m_vertCellsCount[_iLayer])
                        endY = m_vertCellsCount[_iLayer]-1;
                    if (endX >=  m_horizCellsCount[_iLayer])
                        endX = m_horizCellsCount[_iLayer]-1;

#ifdef CELL_CONTENT_ENABLE_BY_DISTANCE
                    f32 updateRangeOutsideViewAABB = 1.0f;
#endif // CELL_CONTENT_ENABLE_BY_DISTANCE

                    for (i32 y = startY;y<=endY;++y)
                    {
                        for (i32 x = startX;x<=endX;++x)
                        {
    //                        Vec2d worldPos((x-MAX_I16_RANGE) * m_CellSize.m_x, (y-MAX_I16_RANGE) * m_CellSize.m_y);
      //                      GFX_ADAPTER->drawDBGCircle(worldPos.m_x, worldPos.m_y, 3.f, Color::green());
                            Cell* pCell = getCellAtIndex(_iLayer, (u16)x, (u16)y);
                            if (pCell)
                            {
#ifdef CELL_CONTENT_ENABLE_BY_DISTANCE
                                bbool cellIsClose = bfalse;
                                for(u32 i = 0; i < View::getViews().size(); ++i)
                                {
                                    View* v = View::getViews()[i];
                                    if(v && v->isActive() && !v->getPrefetchDataOnly())
                                    {

                                        if(v->getScreenProjAABB().checkOverlap(pCell->getAABB(), updateRangeOutsideViewAABB))
                                        {
                                            cellIsClose = btrue;
                                            break;
                                        }
                                    }
                                }

                                if(!cellIsClose)
                                    continue;
#endif // CELL_CONTENT_ENABLE_BY_DISTANCE

                                if(_autoEnable)
                                    pCell->enable(_frame);

                                if (pCell->getAllObjects().size())
                                {
                                    u32 localIdex = cellsIndexes[y * m_horizCellsCount[_iLayer] + x];
                                    ITF_ASSERT(localIdex < m_cellsContainer[_iLayer].size());
                                    m_CellsEnable[(u16)_iLayer].push_back((u16)localIdex);
                                }
                            }
                        }
                    }
                }
            }
        

            //Don't merge them for a better visibility,resource activation
            {
                i32 startX    = indexLeftX  - CellsLayersSettings::getResourceCellsRange(_iLayer);
                i32 endX      = indexRightX + CellsLayersSettings::getResourceCellsRange(_iLayer);

                i32 startY    = indexRightY - CellsLayersSettings::getResourceCellsRange(_iLayer);
                i32 endY      = indexLeftY  + CellsLayersSettings::getResourceCellsRange(_iLayer);

                ITF_ASSERT(indexRightX >= indexLeftX);
                ITF_ASSERT(indexRightY <= indexLeftY);

                if (indexRightX - indexLeftX > MAX_WINDOW_SIZE) // happens with bias camera mode
                {
                    i32 center = (indexLeftX + indexRightX)/2;
                    indexLeftX = center - MAX_WINDOW_SIZE / 2;
                    indexRightX = center + MAX_WINDOW_SIZE / 2;
                }

                if (indexLeftY - indexRightY > MAX_WINDOW_SIZE)  // happens with bias camera mode
                {
                    i32 center = (indexLeftY + indexRightY)/2;
                    indexLeftY = center + MAX_WINDOW_SIZE / 2;
                    indexRightY = center - MAX_WINDOW_SIZE / 2;
                }

                if (startY < 0)
                    startY = 0;
                if (startX < 0)
                    startX = 0;
                if (endY >= m_vertCellsCount[_iLayer])
                    endY = m_vertCellsCount[_iLayer]-1;
                if (endX >=  m_horizCellsCount[_iLayer])
                    endX = m_horizCellsCount[_iLayer]-1;

                //for the resource range
                #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                u32 cellsflag = Cell::cellObjectFlag_FirstLine;
                #endif
                for (i32 y = startY;y<=endY;++y)
                {
                    bbool onceCellFoundInThisLine = bfalse;
                    ITF_UNUSED(onceCellFoundInThisLine);
                    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                    cellsflag |= Cell::cellObjectFlag_FirstColumn;
                    #endif

                    for (i32 x = startX;x<=endX;++x)
                    {
                        u32 localCellIndex = cellsIndexes[y * m_horizCellsCount[_iLayer] + x];
                        Cell* pCell = getCellAtIndex(_iLayer, (u16)x, (u16)y);
                        if (pCell && pCell->getAllObjects().size() )
                        {
                            if (_checkCellDuplicate)
                            {
                                if(_cellsResource.find((u16)localCellIndex) == -1)
                                {
#ifndef ITF_CONSOLE_FINAL
                                    if(s_cellProcessCallback != NULL)
                                        s_cellProcessCallback(_frame, pCell, _iLayer);
#endif // ITF_CONSOLE_FINAL

                                    _cellsResource.push_back((u16)localCellIndex);
                                    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                                    if (_flagsArray)
                                        _flagsArray->push_back(cellsflag);
                                    #endif
                                    onceCellFoundInThisLine = btrue;
                                }
                            }
                            else
                            {
#ifndef ITF_CONSOLE_FINAL
                                if(s_cellProcessCallback != NULL)
                                    s_cellProcessCallback(_frame, pCell, _iLayer);
#endif // ITF_CONSOLE_FINAL

                                _cellsResource.push_back((u16)localCellIndex);
                                #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                                if (_flagsArray)
                                    _flagsArray->push_back(cellsflag);
                                #endif
                                onceCellFoundInThisLine = btrue;
                            }
                            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                            if (onceCellFoundInThisLine)
                                cellsflag &= ~Cell::cellObjectFlag_FirstColumn;
                            #endif
                        }
                        }
                        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                        if (onceCellFoundInThisLine)
                            cellsflag &= ~Cell::cellObjectFlag_FirstLine;
                        #endif
                    }
            }
        }
    }

    bbool World::getCellIndexAtPos(const u32 _layerIndex, const Vec2d& _atpos,i32& indexX,i32& indexY)
    {
        if (!isInCellsRange(_atpos))
            return bfalse;

        i32 x = (i32) floor((_atpos.m_x + fabs(m_fminCellXCoord))/m_CellSize[_layerIndex].m_x);
        i32 y = (i32) floor((_atpos.m_y + fabs(m_fminCellYCoord))/m_CellSize[_layerIndex].m_y);

        ITF_ASSERT(y>=0 && x>=0 && y<m_vertCellsCount[_layerIndex] && x<m_horizCellsCount[_layerIndex]);

        indexX = x;
        indexY = y;

        return btrue;
    }

#if defined(_DEBUG)

    void World::check()const
    {
        for(u32 i = 0; i < m_sceneList.size(); ++i)
        {
            Scene* pScene = m_sceneList[i];
            ITF_ASSERT_MSG(pScene, "This world contains a deleted scene");
            if(!pScene)
                continue;

            ITF_ASSERT_MSG(pScene->getWorldID() == getRef(), "The world contains a scene with an incorrect world ID");
            ITF_ASSERT_MSG(getSceneIndex(pScene) == i32(i), "Basic function test failed");
        }
    }

#endif // _DEBUG

    void World::pushAllowDraw(bbool _bStatus)
    {
        ITF_ASSERT_MSG(!m_bAllowDrawIsPushed, "push / pop allowDraw mismatch"); // solving this would require having a stack of m_bPrevAllowDraw instead of a single member
        m_bPrevAllowDraw = m_bAllowDraw;
        setAllowDraw(_bStatus);
        m_bAllowDrawIsPushed = btrue;
    }

    void World::pushAllowUpdate(bbool _bStatus)
    {
        ITF_ASSERT_MSG(!m_bAllowUpdateIsPushed, "push / pop allowUpdate mismatch"); // solving this would require having a stack of m_bPrevAllowUpdate instead of a single member
        m_bPrevAllowUpdate = m_bAllowUpdate;
        setAllowUpdate(_bStatus);
        m_bAllowUpdateIsPushed = btrue;
    }

    void World::popAllowDraw()
    {
        ITF_ASSERT_MSG(m_bAllowDrawIsPushed, "push / pop allowDraw mismatch"); // solving this would require having a stack of m_bPrevAllowDraw instead of a single member
        setAllowDraw(m_bPrevAllowDraw);
        m_bAllowDrawIsPushed = bfalse;
    }

    void World::popAllowUpdate()
    {
        ITF_ASSERT_MSG(m_bAllowUpdateIsPushed, "push / pop allowUpdate mismatch"); // solving this would require having a stack of m_bPrevAllowUpdate instead of a single member
        setAllowUpdate(m_bPrevAllowUpdate);
        m_bAllowUpdateIsPushed = bfalse;
    }

    bbool World::hasScene( const Scene* _pScene )const
    {
        return m_sceneList.find((Scene*)_pScene) >= 0;
    }

    void    World::cellHasObjects(Cell* _Cell, u32 _iLayer)
    {
        Vec2d min = _Cell->getAABB().getMin();
        Vec2d max = _Cell->getAABB().getMax();

        if (min.m_x < m_minCellPosEverRegistered[_iLayer].m_x)
            m_minCellPosEverRegistered[_iLayer].m_x = min.m_x;
        if (min.m_y < m_minCellPosEverRegistered[_iLayer].m_y)
            m_minCellPosEverRegistered[_iLayer].m_y = min.m_y;
        if (max.m_x > m_maxCellPosEverRegistered[_iLayer].m_x)
            m_maxCellPosEverRegistered[_iLayer].m_x = max.m_x;
        if (max.m_y > m_maxCellPosEverRegistered[_iLayer].m_y)
            m_maxCellPosEverRegistered[_iLayer].m_y = max.m_y;
    }
    
    void World::updateFriezeConnections()
    {
        if ( !m_checkFriezeConnections )
            return;

        m_checkFriezeConnections = bfalse;

        Scene* rootScene = getRootScene();

        ITF_VECTOR<FriezeConnectionResult>& results = rootScene->getFriezeConnectionsRef();

        // On editor: always generate the list
//#if defined(ITF_SUPPORT_EDITOR)  // temp RO-14355
        results.reserve(200);
        results.clear();

        Pickable* friezeListBuffer[1024];
        PickableList friezeList(sizeof(friezeListBuffer)/sizeof(Pickable*),MemoryId::mId_Temporary,friezeListBuffer);

        for (u32 iScene = 0; iScene < getSceneCount(); iScene++)
        {
            Scene* curScene = getSceneAt(iScene);
            curScene->getFrises(friezeList);
        }
        Frise::checkConnections( friezeList, results );
//#endif // ITF_SUPPORT_EDITOR // temp RO-14355
        
        for(u32 i = 0; i < results.size(); ++i)
        {
            FriezeConnectionResult& connection = results[i];
/* // temp RO-14355
#if defined(ITF_SUPPORT_EDITOR)
            connection.generateSerializationDataFromRuntime();
#else //!ITF_SUPPORT_EDITOR
            connection.generateRuntimeDataFromSerialization();
#endif
*/

            connection.applyResult();
        }

//#if !defined(ITF_SUPPORT_EDITOR) // temp RO-14355
        results.clear();
//#endif // !ITF_SUPPORT_EDITOR
    }

    BaseObject* World::getObject( const ObjectId& _objID ) const
    {
        for(u32 i = 0; i < m_sceneList.size(); ++i)
        {
            Scene* pScene = m_sceneList[i];
            if(!pScene)
                continue;

            BaseObject* pObj = m_sceneList[i]->getObject(_objID);
            if(pObj)
                return pObj;
        }
        return NULL;
    }

    void World::applyWorldFileSettings()
    {
        if(m_worldFile)
            m_worldFile->applySettings();
        else
        {
            WorldFile *worldFile = WORLD_MANAGER->getDefaultWorldFile();
            ITF_ASSERT(worldFile);
            if (worldFile)
            {
                worldFile->applySettings();
            }
        }
    }

    Cell& World::createCellAt(const u32 _layerIndex, int _cellX, int _cellY)
    { 
        u16* cellsIndexes = m_cellsIndexes[_layerIndex];
        ITF_VECTOR<Cell>& cellsContainer = m_cellsContainer[_layerIndex];

        ITF_ASSERT_CRASH(abs(_cellX) < 65535 && abs(_cellY) < 65535, "World's too big");
        ITF_ASSERT (_cellX < m_horizCellsCount[_layerIndex] && _cellX < m_vertCellsCount[_layerIndex] && _cellX>=0 && _cellY>=0);
        ITF_ASSERT (cellsIndexes[_cellY * m_horizCellsCount[_layerIndex] + _cellX] == m_invalidCellIndex);
        u32 size = cellsContainer.size();
        ITF_ASSERT(size < 65535);
        cellsIndexes[_cellY * m_horizCellsCount[_layerIndex] + _cellX] = (u16)size;
        f32 minX = floorf((f32)(m_minCellXCoord + _cellX * (i32)m_CellSize[_layerIndex].m_x));
        f32 minY = floorf((f32)(m_minCellYCoord + _cellY * (i32)m_CellSize[_layerIndex].m_y));
        Cell cell;
        Vec2d minPos(minX, minY);
        cell.init(minPos, m_CellSize[_layerIndex], (i16)_cellX, (i16)_cellY, this);

#ifdef ASSERT_ENABLED
 /*       for (u32 i = 0; i < size; i++)
        {
            ITF_ASSERT ((cellsContainer[i].getAABB().getMin()- minPos).sqrnorm() > 1.f);
        }*/
#endif

        cellsContainer.push_back(cell);
        return cellsContainer[size];
    }

    u32 World::getCellIndexAtPos(const u32 _layerIndex, u16 _x, u16 _y)
    {
        ITF_ASSERT(_x < m_horizCellsCount[_layerIndex] && _y < m_vertCellsCount[_layerIndex]);
        if (_x >= m_horizCellsCount[_layerIndex] || _y >= m_vertCellsCount[_layerIndex])
            return m_invalidCellIndex;
        u16* indexes = m_cellsIndexes[_layerIndex];
        return indexes[_y * m_horizCellsCount[_layerIndex] + _x];
    }

    Cell* World::getCellAtIndex(const u32 _layerIndex, u16 _x, u16 _y)
    {
        u32 cellIndex = getCellIndexAtPos(_layerIndex, _x, _y);
        if (m_invalidCellIndex == cellIndex)
            return NULL;

        ITF_VECTOR<Cell>& cellsContainer = m_cellsContainer[_layerIndex];
        ITF_ASSERT(cellIndex < cellsContainer.size());

        return &cellsContainer[cellIndex];
    }


    void World::draw2D()
    {
        for(u32 i = 0; i < getSceneCount(); ++i)
        {
            Scene* pScene = getSceneAt(i);
            pScene->draw2D();
        }
    }

} //namespace ITF
