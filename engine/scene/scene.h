#ifndef _ITF_SCENE_H_
#define _ITF_SCENE_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_SCENE_TYPES_H_  
#include "engine\scene\scene_types.h"
#endif //_ITF_SCENE_TYPES_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H

#ifndef _ITF_LOADING_SEQUENCE_H_
#include "engine/boot/LoadingSequence.h"
#endif

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{
    class World;
    class Actor;
    class Frise;
    class ArchiveMemory;
    class Pickable;
    class SubSceneActor;
    struct LightInfo;
    class ObjectGroup;
    class Resource;


    class Scene : public BaseObject
    {
        friend class XMLAll;

    public:
        ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(mId_Scene);
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(Scene, BaseObject,209039730);

        static const u32 cooked_collisions_magic = 0x1A2B3C;
        // Scene loading flags (MUST be powers of 2)
        static const u32 flag_isCooking = 1;
        static const u32 flag_isLoadedFromBinary = 2;
        static const u32 flag_CollisionsFoundInBinary = 4;
        static const u32 flag_dependency = 8;

        Scene();
        virtual ~Scene();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Check if two depth are in the same range, and should collide...
        /// @param _z1 the depth of the first object
        /// @param _z2 the depth of the second object
        /// @return if the depth are in the same range
        static bbool    isSameDepthRange(f32 _z1, f32 _z2)  { return i32(_z1) == i32(_z2); }
        static f32      getLowerRange(f32 _z)               { return floorf(_z); }
        static f32      getUpperRange(f32 _z)               { return ceilf(_z); }

        f32         getReferenceDepth()const;
        void        centerOnWorldOrigin();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Remove from list the objects that are not serializable
        void        preSerializeSave();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Repush into list the non-serializable objects
        void        postSerializeSave();

        void        destroy();
        void        checkPickableCanBeDeleted(const Pickable* _pick,u32 step = 0);

        void        fillUpdateList(ObjectGroup* _listToFill);
        void        fill2DUpdateList(ObjectGroup* _listToFill);

        void        fillDrawList();

        Scene*      clone();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call reinit on all frises (used to recompute VB
        /// @param _bFullReinit (bbool)
        void        reinit(bbool _bFullReinit);

        void        registerPickable(Pickable* _pObj);

        void        transferPickable(Pickable* _pObj, const Scene& _destination );

        void        forcedActorsSaveCheckpoint();
        void        forcedActorsLoadCheckpoint();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// destroy spawnees in that scene
        /// @param _recursive : if btrue, recurse in subscenes
        void        destroySpawnees(bbool _recursive);

        void        unregisterPickable(Pickable* _pObj, bbool _delete = btrue);

        void        unregisterActorAvailableForSpawn(Actor* _pObj);

        ITF_INLINE const PickableList&  getActors() const               { return m_ActorsList; }
        ITF_INLINE const PickableList&  get2DActors() const             { return m_2DActorsList; }
        ITF_INLINE PickableList&        getActorsMutable()              { return m_ActorsList; }
        ITF_INLINE const PickableList&  getAlwaysActiveActors() const   { return m_AlwaysActiveActors; }
        ITF_INLINE const PickableList&  getFrises() const               { return m_FriseList; }
        ITF_INLINE const PickableList&  getPickableList() const         { return m_PickableList; }
        ITF_INLINE void                 getFrises( PickableList& _list ){ _list.mergeArray(m_FriseList); }

        void        clear();

        void        onSceneLoaded();

        void        removeUnserializableFromLists();
        void        restoreUnserializableFromLists();
        bbool       reorderSerializedLists();

        void        onSceneCollect(ITF_VECTOR<Pickable*> &_vActor,ITF_VECTOR<Pickable*> &_vFrieze);//Pickable* because is called from everywhere

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// force resource (texture...) loading for Frises and Actors. Resources will be loaded as soon as possible.
        void        forceLoadResourcesAndDisableUnload();

        bbool       isActive() const    {return m_bActive;}
        void        switchToActive();
        void        setActive(bbool _bActive); //Set activation level of a scene : add/remove object from physics, call onSceneActive() or onSceneInactive() for each actor,...

        bbool       isPhysicalReady() const;

        const String&       getUniqueName()const                      { return m_sUniqueName; }
        void                setUniqueName(const String& _uniquename)  {m_sUniqueName= _uniquename;m_sUniqueName.toLower(); }
        const Path&         getPath()const                      { return m_szPath; }
        void                setPath(const Path& _path)        { m_szPath= _path; }

        const BaseObjectList& getActiveObjects() const       {return m_activeObjects;}
        void  getActiveObjectsRecursive(BaseObjectList& _listToFill)const;

        ITF_INLINE void clearActiveObjectsList()  {m_activeObjects.clear();}
        ITF_INLINE void addActiveObject(const ObjectRef &_ref) 
        {
            // You you reach this assert:
            // 1. Normally, Pickable::setActiveForFrame() tests the last active frame before calling addActiveObject(). Check that this is still true
            // 2. Check the callstack: if addActiveObject() if called by another method than Pickable::setActiveForFrame(), then you have doublons. Maybe the
            //    calling method should call Pickable::setActiveForFrame() instead of directly calling addActiveObject() 
            ITF_ASSERT(!m_activeObjects.hasObject(_ref)); 
            m_activeObjects.addObject(_ref, btrue);
        }

        ITF_INLINE void removeActiveObject(const ObjectRef &_ref) 
        {
            m_activeObjects.removeObject(_ref);
        }

        ITF_INLINE void addActiveLight(const ObjectRef &_ref) 
        {
            ITF_ASSERT(m_activeLights.find(_ref) == -1); 
            m_activeLights.push_back(_ref);
        }
        
        ITF_INLINE void removeActiveLight(const ObjectRef &_ref) 
        {
            i32 index = m_activeLights.find(_ref);
            if(index >= 0)
                m_activeLights.eraseNoOrder(index);
        }

        LightInfo* getLightAt(u32 _i);

        ITF_INLINE ObjectRefList& getActiveLightList() {return m_activeLights;}

        void    activeAll(bbool _clearActiveListFirst = bfalse);

        void    addSerializationFlag(u32 _flag) {m_serializationFlags |= _flag;}
        void    removeSerializationFlag(u32 _flag) {m_serializationFlags &= (~_flag);}
        bbool   hasSerializationFlag(u32 _flag) {return ((m_serializationFlags & _flag)!=0);}

        static void    addStaticSerializationFlag( u32 _flag ) {m_staticSerializationFlags |= _flag;}
        static void    removeStaticSerializationFlag(u32 _flag) {m_staticSerializationFlags &= (~_flag);}
        static bbool   hasStaticSerializationFlag(u32 _flag) {return ((m_staticSerializationFlags & _flag)!=0);}

        /*
        void   registerObjectToActiveList(ObjectRef _ref);
        void   unregisterObjectFromActiveList(ObjectRef _ref);
        */
        bbool  isAlwaysActiveActor(Actor* _pActor);
        void   setAlwaysActiveActor(Actor* _pActor, bbool _alwaysActive = btrue);


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// storeCurrentToInitial for all pickables
        void                        storeCurrentToInitial();
        void                        replaceReinitDataByCurrent();

        bbool                       isEmpty()const;
        void                        offsetPosition(const Vec3d& _delta, bbool _applyToInitialPosition, bbool _updateFriezes = btrue);
        void                        rotateContent(const Vec3d& _rotationOrigin, f32 _currentAngle, f32 _delta);
        bbool                       getForceLoadResources() const { return m_forceLoadResources; }
        void                        setForceLoadResources(bbool val) { m_forceLoadResources = val; }
        void                        disconnectFromCells()const;
        bbool                       getIsAlwaysActive() const { return m_isAlwaysActive;}
        void                        setIsAlwaysActive( bbool _val);
        void                        onLoadReposition();
        void                        add2DActor(Actor* _pAct);
        void                        remove2DActor(Actor* _pAct);
        void                        getAll2DObjects(ObjectRefList& _list2D);
        void                        draw2D();
        bool                        findScene(StringID _sceneName, Scene *& _scene, Vec3d & pos);

    private:

        void        registerFrise(Pickable* _pObj);
        void        registerActor(Pickable* _pObj);
        void        unregisterFrise(Pickable* _pObj, bbool _delete = btrue);
        void        unregisterActor(Pickable* _pObj, bbool _delete = btrue);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Finish the serialization load process by populating pickableList and setScene... etc
        void                    finishSerializationLoad();
        
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Fix content before/after loading
        void                    fixSerializableContent();

        //////////////////////////////////////////////////////////////////////////
        /////////////////////////  Object containers  ////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        PickableList                m_FriseList;
        PickableList                m_ActorsList;
        PickableList                m_2DActorsList;

        PickableList                m_AlwaysActiveActors;
        PickableList                m_PickableList;

        bbool                       m_bActive;
        bbool                       m_bAllowUnload;
        u32                         m_serializationFlags;
        static u32                  m_staticSerializationFlags;

        String                      m_sUniqueName;
        Path                        m_szPath;

        BaseObjectList              m_activeObjects;
        ObjectRefList               m_activeLights;


        //////////////////////////////////////////////////////////////////////////
        ////////////////////////  Delayed list changes   /////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call pending Registers, and pending Unregisters
        void                    flushPending(bbool _recursive = bfalse);
        //May have to call this fn after loading a scene, to add pending Pickables
        void                    processRegisterPending(); 
        void                    processUnregisterPending();

        const ObjectRefList&    getSubSceneActors()const { return m_SubSceneActors; }

    private:        
        PickableList    m_Pending_RegisterFrise;
        PickableList    m_Pending_RegisterActor;
        PickableList    m_Pending_RegisterActorAlwaysActive;

        PendingRequestList m_Pending_UnregisterFrise;
        PendingRequestList m_Pending_UnregisterActor;
        PendingRequestList m_Pending_UnregisterActorAlwaysActive;
        PendingRequestList m_Pending_UnregisterActorAvailableForSpawn;

        PendingTransferList m_Pending_Tranfert;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        ObjectRefList   m_SubSceneActors;

#ifdef _DEBUG
        void checkConsistency()const;
#endif // _DEBUG

    private:
        friend class World; // Normal way to set the world ID
        friend class SceneManager; // Used to differentiate a main scene loading from a sub scene loading
        friend class CSerializerObject; // For undo and checkpoint system
        void        setWorldID(ObjectRef _ID); // Recursive
        

    public:
        ObjectRef   getWorldID()const  { return m_worldID; };
        World*      getWorld()const;
        void        setAsyncLoadingPending(bbool _state) {m_isAsyncLoadingPending = _state;}
        bbool       isAsyncLoadingPending() const {return m_isAsyncLoadingPending;}

        const StringID& getMusicTheme() const { return m_musicTheme; }
        void            setMusicTheme(const StringID& _theme) { m_musicTheme = _theme; }

        const Path& getMusicThemePath() const { return m_musicThemePath; }
        void        setMusicThemePath( const Path& path ) { m_musicThemePath = path; }

        TemplateClientHandler* getTemplateClientHandler() { return &m_templateClientHandler; }
        void        loadSubScenes();

    private:

        ObjectRef   m_worldID;
        bbool       m_forceLoadResources;
        bbool       m_isAlwaysActive;
        bbool       m_isAsyncLoadingPending;
        bbool       m_loaded;

        StringID    m_musicTheme;
        Path        m_musicThemePath;

        SubSceneActor*          m_SubSceneActorHolder;
        TemplateClientHandler   m_templateClientHandler;


    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Tell to the scene wich subsceneactor is it's origin (if this scene is a subscene)
        /// @param _SSAH
        void            setSubSceneActorHolder(SubSceneActor* _SSAH) { m_SubSceneActorHolder = _SSAH; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Return the subsceneactor holder, which is the subscene origin (in case of subscene...)
        /// @return (ObjectRef)
        SubSceneActor*  getSubSceneActorHolder()const                { return m_SubSceneActorHolder; }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Return the version of the engine when this scene was saved
        u32             getEngineVersionWhenSaved()const                { return m_fileEngineVersion; }

    private:
        // Used in serialization save, holds all the object that must not be serialized (only during the serialization)
        PickableList                    m_FrisesNotSerialized;
        PickableList                    m_ActorsNotSerialized;

        void                            processUnregisterActorAvailableForSpawnPending();

        u32                             m_fileEngineVersion;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Clear the ObjectID map
        /// @return (void)
        void                            clearObjectIDMap();

        ITF_MAP<ObjectId,BaseObject*>   m_objectIdMap;

    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get an actor from it's friendly (if registered in this scene)
        /// @param _friendly The friendly of the searched actor
        /// @return The found actor
        Actor*                          getActorFromUserFriendly(const String8& _friendly)const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Get a pickable from it's friendly (if registered in this scene)
        /// @param _friendly The friendly of the searched pickable
        /// @return The found actor
        Pickable*                       getPickableFromUserFriendly(const String8& _friendly)const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Register the object in this scene with it's object ID
        /// @param _object The object pointer with an attribued ObjectID
        void                            registerObjectWithID        ( BaseObject* _object );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Unregister an object ID from this scene
        /// @param _id The objectID of the object to be unregistered
        void                            unregisterObjectWithID      ( const ObjectId& _id );

        void                            checkPickableIsNotInList(const Pickable* _pick, const PickableList& _list);
        void                            checkPickableIsNotInPendingList(const Pickable* _pick, const PendingRequestList& _list);


        static void                     setBinaryTarget(const String& _p)   { s_CurrentBinaryTarget = _p; }
        static const String&            getBinaryTarget()                   { return s_CurrentBinaryTarget; }

        void                            filterObject(const String8& _friendly, const String& _platform);
        void                            unfilterObject(const String8& _friendly, const String& _platform);
        bbool                           isFilteringObject(const String8& _friendly, const String& _platform)const;
        u32                             getFilteredPlatformsForObject(const String8& _friendly, ITF_VECTOR<String>& _results)const;

    private:

        void                            deleteSpawnedActors();
        friend class WorldManager;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Retreive an object from it's object ID (if registered in this scene) USE WorldManager::getObject(...)
        /// @param _id The objectID of the searched object
        /// @return The found object
        BaseObject*                     getObject                   ( const ObjectId& _id )const;

#if !defined(ITF_CONSOLE)
    private:
        Vector<Path>                    m_associatedTabs;
        f32                             m_gridUnit;

    public:
        Vector<Path>&                   getAssociatedTabs()                 { return m_associatedTabs; }
        f32                             getGridUnit()const                  { return m_gridUnit; }
        void                            setGridUnit(f32 _f)                 { m_gridUnit = _f; }
#endif // !ITF_CONSOLE

    private:
        PickableList                                    m_2DActorsToDraw;
        static String                                   s_CurrentBinaryTarget;
        ITF_VECTOR<TargetFilterList>                    m_filterByPlatform;
        ITF_VECTOR<Path>                                m_dependencies;                     // filled by cooking, used on console
        ITF_VECTOR<LoadingSequence::PrefetchResource>   m_prefetched;
        ITF_VECTOR<FriezeConnectionResult>              m_friezeConnections;

        ITF_VECTOR<ActorHotReload>                      m_forcedActorLoadCP_Tmp;

    public:
        ITF_VECTOR<FriezeConnectionResult>&             getFriezeConnectionsRef()           { return m_friezeConnections; }
        ITF_VECTOR<Path>&                               getDependenciesRef()                { return m_dependencies; }
        ITF_VECTOR<LoadingSequence::PrefetchResource>&  getPrefetchedList()                 { return m_prefetched; }
    };

} // namespace ITF
#endif // _ITF_SCENE_H_
