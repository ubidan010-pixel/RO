#ifndef _ITF_WORLD_H_
#define _ITF_WORLD_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_CELL_H_
#include "engine/scene/cell.h"
#endif //_ITF_CELL_H_

#ifndef _ITF_SCENE_TYPES_H_  
#include "engine/scene/scene_types.h"
#endif //_ITF_SCENE_TYPES_H_

#ifndef _ITF_SCENEEVENT_LISTENER_H_
#include "engine/scene/sceneEventListener.h"
#endif //_ITF_SCENEEVENT_LISTENER_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

#define FAKE_FRAME              1 // start to 1 for each new scene or to load a cell without enable it (prevent update)

namespace ITF
{
    class Scene;
    class Actor;
    class Plane;
    class Camera;
    class AFTERFX;
    class Pickable;
    class WorldFile;
    class ResourceID;
    class SubSceneActor;
    class ArchiveMemory;
    class AFTERFXManager;
    class ITF_ParticleGenerator;
    class GameMaterial_Template;
    class ParticleGeneratorParameters;
    class ResourceGroup;


    class World : public BaseObject, public Interface_SceneEventsListener
    {
        DECLARE_OBJECT_CHILD_RTTI(World, BaseObject, 3307309411)

        // Only WorldManager should create or delete worlds
    private:
        friend class WorldManager;
        friend class CSerializerObject; // for serialisation purpose
        static u32   m_loadResourcesMask;

        World();
        virtual ~World();

    public:
        i32    m_horizCellsCount[CELL_LAYERS_COUNT];
        i32    m_vertCellsCount[CELL_LAYERS_COUNT];
        static const i32    m_maxCellXCoord   =   (LAYER_0_MAX_HORIZ_CELLS / 2) * (int)LAYER_0_CELL_SIZE;
        static const i32    m_minCellXCoord   =   -m_maxCellXCoord;
        static const i32    m_maxCellYCoord   =   (LAYER_0_MAX_VERT_CELLS / 2) * (int)LAYER_0_CELL_SIZE;
        static const i32    m_minCellYCoord   =   -m_maxCellYCoord;
        static const u16    m_invalidCellIndex = 0xffff;

        typedef void (*cellProcessCallback) (u32 _frame, const Cell* _loadedCell, u32 _layer); // Cell process callback to provide editor infos

        static f32    m_fmaxCellXCoord;
        static f32    m_fminCellXCoord;
        static f32    m_fmaxCellYCoord;
        static f32    m_fminCellYCoord;

        static void disableLoadResourcesDelay() {m_loadResourcesMask = 0xffffffff;}
        static void enableLoadResourcesDelay() {m_loadResourcesMask = 7;}

        void        reinit(bbool _bFullReinit);
        void        checkFriezeConnections() {m_checkFriezeConnections = btrue;}        

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// swap active obj lists and clear new list
        void        swapActiveObjectsList();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Build the list of active objets
        void        startFrame();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call flush pending on all scene hierarchy
        void        flushPending();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// update each scene
        /// @param _elapsed elapsed time for a frame
        void        update( f32 _elapsed );
        void        centerOnWorldOrigin();

        void        fillDrawList();
        void        postDraw();

        bbool       isPhysicalReady();
        bbool       isPhysicalReady(const AABB& aabb);
        bbool       isPhysicalReady(const class View& _view);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the root scene
        /// @return the root scene
        Scene*      getRootScene()const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get scene count for this world
        u32         getSceneCount()const                { return m_sceneList.size(); }
		///////////////////////////////////////////////////////////////////////////////////////////
        /// get the scene at given index
        /// @param _i the index
        Scene*      getSceneAt(u32 _i)const             { return m_sceneList[_i]; }
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// search a scene and returns its index, or -1 if not found
        /// @param _pScene the scene
        i32         getSceneIndex(Scene* _pScene)const  { return m_sceneList.find(_pScene); }
       

        // GameMaterial section
        static void                         addGameMaterialFromFile(const Path& _fileName, ResourceGroup* _client);
        static const GameMaterial_Template* getGameMaterial(const Path& _fileName);
        static const GameMaterial_Template* getGameMaterial(const StringID& _fileName);
        
        // Particles / AfterFX section
        void                        CreateFxManager( );
        ITF_INLINE AFTERFXManager*  getAFXManager() {return mp_afterFxManager;};

        static const u32 m_particlePoolNumber = 16;
        ITF_ParticleGenerator * getParticleGenerator(const ResourceID & _textureID);
        ITF_ParticleGenerator * getParticleGeneratorPool() {return m_particleGeneratorPool;}
        static void             fillDefaultParticleGeneratorParams(ParticleGeneratorParameters *_particleGenParams);
        void                    setDefaultParticleGeneratorParams(ITF_ParticleGenerator *_particleGen);


        bbool                   isActive()const                     { return m_bActive; }
        bbool                   isAlwaysActive()const               { return m_bAlwaysActive; }
        void                    setActive(bbool _status);
        void                    setAlwaysActive(bbool _status); // if btrue then setActive(btrue) is called

        bbool                   slow_isObjectInAnyCell_slow(ObjectRef _objRef);
        void                    setPrefetchTargets(Vector<ObjectPath> & _prefetchTargets) { m_prefetchTargets = _prefetchTargets;}
    public:
        const BaseObjectList&   getActiveObjects() const            { return m_activeObjects[m_activeObjectsListIndex]; }
        const BaseObjectList&   getLastFrameActiveObjects()         { return m_activeObjects[m_activeObjectsListIndex ^ 1]; }

        void                    setAllowDraw(bbool _bStatus)        { m_bAllowDraw = _bStatus; }
        void                    setAllowUpdate(bbool _bStatus)      { m_bAllowUpdate = _bStatus; }
        void                    pushAllowDraw(bbool _bStatus);
        void                    pushAllowUpdate(bbool _bStatus);
        void                    popAllowDraw();
        void                    popAllowUpdate();

        bbool                   getAllowDraw () const               { return m_bAllowDraw; }
        bbool                   getAllowUpdate () const             { return m_bAllowUpdate; }


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Gets the unique name of the world
        /// @return the unique name of the world
        const String&           getUniqueName()const                { return m_uniqueName; }

        void                    addScene(Scene* _pScene);
        bbool                   hasScene(const Scene* _pScene)const;

        void                    cellHasObjects(Cell* _Cell, u32 _iLayer);
        void                    cellIsEmpty(Cell* _Cell);
        bbool                   cellEverDataIsValid(u32 _iLayer) {return m_minCellPosEverRegistered[_iLayer].m_x < 999999999999999999.f;}
        const Vec2d&            getminCellPosEverRegistered(u32 _iLayer) const  {return m_minCellPosEverRegistered[_iLayer];}
        const Vec2d&            getmaxCellPosEverRegistered(u32 _iLayer) const  {return m_maxCellPosEverRegistered[_iLayer];}
        bbool                   isSceneAsyncLoadRunning()const       { return m_bSceneAsyncLoadRunning; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Assign the world file to the world, it's assigned if the world was loaded from an worldFile
        /// @param _pWorldFile (WorldFile *) the worldFile
        void                    setWorldFile(WorldFile* _pWorldFile) { ITF_ASSERT(!m_worldFile); m_worldFile = _pWorldFile; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Apply WorldFileSettings if possible
        void                    applyWorldFileSettings();

        static void             setCellProcessCB(cellProcessCallback _cb) { s_cellProcessCallback = _cb; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Used to get loading info 
        ITF_INLINE bbool        isAsyncLoadedWorld() const { return m_asyncLoaded; }
        ITF_INLINE void         setAsyncLoadedWorld(bbool _asyncLoaded) { m_asyncLoaded = _asyncLoaded; }

    public: //Interface_SceneEventsListener inheritence
        virtual void onSceneAsyncLoadFinished(const Scene* /*_pScene*/){};
        virtual void onScenePreDestroy(Scene* /*_pScene*/){};
        virtual void onDeleteScene(const Scene* _pScene);
        virtual void onObjectAdded(const Scene* /*_pScene*/, BaseObject* /*_pObj*/) {};
        virtual void onObjectRemoved(const Scene* /*_pScene*/, BaseObject* /*_pObj*/, bbool /*_delete*/) {};
        virtual void onPostSceneActivation(const Scene* /*_pScene*/, bbool /*_bActive*/){};

    private:
        static TemplateClientHandler    m_templateClientHandler;
        static cellProcessCallback      s_cellProcessCallback;
        String                          m_uniqueName;
        bbool                           m_bActive;
        bbool                           m_bAlwaysActive;
        SafeArray<Scene*>               m_sceneList;
        bbool                           m_bAllowDraw, m_bPrevAllowDraw, m_bAllowDrawIsPushed;
        bbool                           m_bAllowUpdate, m_bPrevAllowUpdate, m_bAllowUpdateIsPushed;
        ITF_ParticleGenerator*          m_particleGeneratorPool;
        AFTERFXManager*                 mp_afterFxManager;
        Vec2d                           m_minCellPosEverRegistered[CELL_LAYERS_COUNT], m_maxCellPosEverRegistered[CELL_LAYERS_COUNT];

        void                            updateFriezeConnections();

        bbool                           m_checkFriezeConnections;
        void                            setSceneAsyncLoadRunning(bbool _bstatus)    { m_bSceneAsyncLoadRunning = _bstatus; }
        bbool                           m_bSceneAsyncLoadRunning;

        WorldFile*                      m_worldFile;

        u32                             m_activeObjectsListIndex;
        static const u32 ACTIVEOBJ_LIST_SIZE = 2;
        BaseObjectList                  m_activeObjects[ACTIVEOBJ_LIST_SIZE];
        SafeArray<u16>                  m_CellsToUnload[CELL_LAYERS_COUNT];
        Vector<ObjectPath>              m_prefetchTargets;
        ///////////////////////////////////////////////////////////////////////////////////////////
        // Cells section
        ///////////////////////////////////////////////////////////////////////////////////////////

    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Prefetch resource for objectPath in list, if empty the first checkpoint will be used
        /// @param objectPath the ObjectPath to prefetch at
        void                    prefetchResource();

        void                    stopPrefetch();

        Cell*                   getCellAtPos(const u32 _layerIndex, const Vec2d& _atpos);
        Cell*                   getCellAtIndex(const u32 _layerIndex, u16 _x, u16 _y);
        const ITF_VECTOR<Cell>& getCells(const u32 _layerIndex) const {return m_cellsContainer[_layerIndex];}
        const Vec2d&            getCellsSize(const u32 _layerIndex) {return m_CellSize[_layerIndex];}


        enum RegisterInCellAction
        {
            AddInCells,
            RemoveFromCells
        };

        void                    registerOrRemoveObjectInCells(Pickable* pObj, RegisterInCellAction _action, const AABB* _exclusionAABB = NULL, const i8 _exclusionLayer = -1);

        bbool                   getIgnoreCells() const              { return m_cellsIndexes[0] == NULL; }
        void                    setIgnoreCells( bbool _val);
        
        void                    activeAll();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// force resource (texture...) loading for Frises and Actors. Resources will be loaded as soon as possible.
        void                    forceLoadResourcesAndDisableUnload();

        void                    update2D(f32 _elapsed);
        void                    draw2D();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Search the baseobj in the world hierarchy, returns the first found
        BaseObject*             getObject(const ObjectId& _objID)const;
        bbool                   getCellIndexAtPos(const u32 _layerIndex, const Vec2d& _atpos, i32& indexX, i32& indexY);

        u32                     getCellIndexAtPos(const u32 _layerIndex, u16 _x, u16 _y);

        void                    addObjectToInactivate(ObjectRef _ref) {m_objectsToInactivate.push_back(_ref);}

    private:
        bbool                   projectViewToAABB(class View* view, AABB& screenProjectionOnGrid);
        void                    clearCellsEnable(u32 _iLayer);
        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            void                    buildActiveObjectsList(u32 _iLayer, SafeArray<u16>& _cellsResource, SafeArray<u32>& _flagsArray, u32 _frame);
            void                    getCellsFromCamera(u32 _iLayer, SafeArray<u16>& _cellsResource, SafeArray<u32>* _flagsArray);
            void                    loadInRangeResources(u32 _iLayer, SafeArray<u16>& _cellsResource, const SafeArray<u32>& _flagsArray);
            void                    getCellsFromBox(u32 _iLayer, const Vec2d& _bordLetfUp,const Vec2d& _bordRightDown, SafeArray<u16>& _cellsResource, SafeArray<u32>* _flagsArray, u32 _frame, bbool _checkCellDuplicate = bfalse, bbool _autoEnable = btrue);
        #else
            void                    buildActiveObjectsList(u32 _iLayer, SafeArray<u16>& _cellsResource, u32 _frame);
            void                    getCellsFromCamera(u32 _iLayer, SafeArray<u16>& _cellsResource);
            void                    loadInRangeResources(u32 _iLayer, SafeArray<u16>& _cellsResource);
            void                    getCellsFromBox(u32 _iLayer, const Vec2d& _bordLetfUp,const Vec2d& _bordRightDown, SafeArray<u16>& _cellsResource, u32 _frame, bbool _checkCellDuplicate = bfalse, bbool _autoEnable = btrue);
        #endif
        void                    desactivateOutOfRangeObjects(u32 _frame);
        void                    unloadOutOfRangeResources(int _iLayer);
        //find cells from a box. Also activate objects for this frame.
        void                    activateAlwaysActiveActors(u32 _frame);
        void                    intersectPolygonWithNegativePlane(const FixedArray<Vec3d, 64> &_in, FixedArray<Vec3d,64> &_out, const Plane &_plane);
        bbool                   projectFrustumOnPlane(Camera *_cam, f32 _Z, bbool _useNear, AABB &_screenProjectionOnGrid);

        void                    ignoreCells();
        void                    createCellIndexes();

        Vec2d                   m_CellSize[CELL_LAYERS_COUNT];
        SafeArray<u16>          m_CellsEnable[CELL_LAYERS_COUNT];
        bbool                   m_bAllowUnload;
        SafeArray<View*>        m_prefetchList; // Contains AABB used to prefetch the level so isPhysicallyready() can use it
        f32                     m_minZEverRegistered, m_maxZEverRegistered;
        u16*                    m_cellsIndexes[CELL_LAYERS_COUNT];
        ITF_VECTOR<Cell>        m_cellsContainer[CELL_LAYERS_COUNT];

        Cell&                   createCellAt(const u32 _layerIndex, int _cellX, int _cellY);

        bbool                   m_asyncLoaded;

        SafeArray<ObjectRef>    m_objectsToInactivate;

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG)
        void            check()const;
#endif // _DEBUG
    };

}

#endif // _ITF_WORLD_H_
