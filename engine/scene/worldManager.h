#ifndef _ITF_WORLD_MANAGER_H_
#define _ITF_WORLD_MANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_SCENEEVENT_LISTENER_H_
#include "engine/scene/sceneEventListener.h"
#endif //_ITF_SCENEEVENT_LISTENER_H_

#ifndef _ITF_LOAD_INFO_H_
#include "engine/scene/loadInfo.h"
#endif //_ITF_LOAD_INFO_H_

#ifndef _ITF_OBJECTGROUP_H_
#include "engine/scene/ObjectGroup.h"
#endif //_ITF_OBJECTGROUP_H_

namespace ITF
{
    class Actor;
    class World;
    class WorldFile;
    class ArchiveMemory;
    class Interface_WorldEventsListener;
    class Pickable;
    class Interface_WorldFile_ProjectData;

    class WorldManager: public Interface_SceneEventsListener,public TemplateSingleton<WorldManager>
    {
        public:
            WorldManager();
            virtual ~WorldManager();

            // Use newWorld + loadWorld with empty data
            ObjectRef       createAndLoadEmptyWorld(bbool _ignoreCells);
            ObjectRef       createAndLoadWorld(LoadInfo& _sLoadInfo);

            // When you want to load a world, you have to create it before using newWorld()
            ObjectRef       newWorld();

            // Returns if the loading is started
            bbool           loadWorld(ObjectRef& _worldToLoad, LoadInfo& _sLoadInfo);            
            bbool           deleteWorld(World* _pWorld);

            void            setDefaultWorldFile(WorldFile *_file) {m_defaultWorldFile = _file;}
            WorldFile*      getDefaultWorldFile() const {return m_defaultWorldFile;}

            void            init();
            void            destroy();

            ObjectRef       getGlobalWorld()const                       { return m_rGlobalWorld; }
            ObjectRef       getSleepWorld()const                        { return m_rSleepWorld; }
            ObjectRef       getCurrentWorld()const                      { return m_rCurrentWorld; }

            u32             getWorldCount()const                        { return m_worldList.size(); }
            World*          getWorldAt(u32 _i)const                     { return m_worldList[_i]; }
            World*          getWorldFromStringID(const StringID &_id) const;
            World*          getWorldFromUniqueName(const String& _uName)const;

            void            setAutoUnloadPreviousWorld(bbool _status)   { m_autoUnloadPreviousWorld = _status; }
            bbool           getAutoUnloadPreviousWorld()const           { return m_autoUnloadPreviousWorld;}

            // Set "_wRef" as the new current world
            // Disable and delete the previous world if AutoUnloadPreviousWorld if true
            void            switchToWorld(ObjectRef _wRef);

            // Called on all active worlds in the world list
            void            reinit(bbool _bFullReinit);
            void            startFrame();

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// update Scene Graph (actors, frises, physics, UI,...)
            /// @param dt
            void            updateSceneGraph(f32 dt);
            void            draw();
            void            draw2D()const;
            void            checkPickableCanBeDeleted(const Pickable* _pick,u32 step = 0);
            void            addEventsListener(Interface_WorldEventsListener* _listener);
            void            removeEventsListener(Interface_WorldEventsListener* _listener);
            void            pauseAllDraws();
            void            pauseAllUpdates();
            void            unpauseAllDraws();
            void            unpauseAllUpdates();

            void            flushPending();

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// Load a world file
            /// @param _relativePath (const String &) the relative path to the worldfile
            /// @return (WorldFile*) the worldFile, NULL if the process failed
            WorldFile*      loadWorldFile(const Path& _relativePath)const;

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// Retreive an object from it's object ID
            /// @param _pContextScene The context scene, if null the object will be searched over all scenes in memory
            /// @param _id The objectID of the searched object
            /// @return The found object, null if not found
            static BaseObject* getObject( const Scene* _pContextScene, const ObjectId& _id);

            ///////////////////////////////////////////////////////////////////////////////////////////
            /// Retreive an object from it's object ID (if registered in this scene)
            /// @param _pContextScene The context scene, if null search is done over all worlds
            /// @param _id The objectID of the searched object
            /// @return The found object of actor type
            static Actor*   getActor( const Scene* _pContextScene, const ObjectId& _id );

            static void   addListener(Interface_WorldEventsListener* _listener);
           
            static bbool   removeListener(Interface_WorldEventsListener* _listener);
           

        public:
            static bbool is2DPass() { return m_is2DPass;}
            // Interface_SceneEventsListener inheritence. Please keep empty methods, they must remain pure virtual in the interface
            virtual void onSceneAsyncLoadFinished(const Scene* _pScene);
            virtual void onScenePreDestroy(Scene* /*_pScene*/) {};
            virtual void onDeleteScene(const Scene* /*_pScene*/) {};
            virtual void onObjectAdded(const Scene* /*_pScene*/, BaseObject* /*_pObj*/) {};
            virtual void onObjectRemoved(const Scene* /*_pScene*/, BaseObject* /*_pObj*/, bbool /*_delete*/) {};
            virtual void onPostSceneActivation(const Scene* /*_pScene*/, bbool /*_bActive*/) {};

            void                    onStartDrawProcess();
            void                    onEndDrawProcess();

            //void                  onObjectBecomeInactive(Pickable* pObject);
            ITF_INLINE void         addObjectInGroupForDraw(Pickable* pObject, u32 _group)  {ITF_ASSERT(_group < ObjectGroup::GroupIndex_Max); m_drawGroups[_group].addElement(pObject);}
            ITF_INLINE void         addActiveLight(class LightComponent* _light)            {m_activeLights.push_back(_light);}
            ObjectGroup*            getUpdateGroups()                                       {return &m_updateGroups[0];}
            ObjectGroup*            getUpdate2DGroups()                                     {return &m_update2DGroups[0];}
        
            bbool                   isPrefetching()const;

            void                    getAll2DObjects(ObjectRefList& _list2D, bbool _activeScenesOnly = bfalse);
            SafeArray<class ITF_ParticleGenerator*>& getParticleGenToLight() { return m_particleGeneratorToLight; }
            ITF_INLINE void         addParticleGenToLight(class ITF_ParticleGenerator* _light)            {m_particleGeneratorToLight.push_back(_light);}

        private:
            static bbool    m_is2DPass;

            // ------------ Static world functions -------------- //
            bbool           m_bInitStaticWorldsCalledOnce;

            void            initStaticWorlds(); // global and sleep
            void            destroyStaticWorlds();

            ObjectRef       newStaticWorld(const String& _name, bbool _bAlwaysActive, bbool _ignoreCells);
            bbool           deleteStaticWorld(ObjectRef _wRef);
            // ---------------------------------------------------//


            // ------------ World list functions ---------------- //
            void            addWorldToList(World* _pWorld);
            bbool           removeWorldFromList(World* _pWorld);

            WorldFile       *m_defaultWorldFile;

            SafeArray<World*>   m_worldList; // all worlds excepts LOADING_SCREEN's world
            // ---------------------------------------------------//

            ObjectRef       m_rGlobalWorld;     // always drawed & updated
            ObjectRef       m_rSleepWorld;      // never draw & updated
            ObjectRef       m_rCurrentWorld;    // could be INVALID

            SafeArray<Interface_WorldEventsListener*>    m_pWorldEventsListeners;
            ITF_THREAD_CRITICAL_SECTION         m_csWorldEvents; // for listeners registration from multiple threads

            bbool           m_autoUnloadPreviousWorld; // Used to skip the deletion of the world in switchToWorld(...)

#if defined(_DEBUG)
            void            check()const;
#endif // _DEBUG

#ifdef ITF_SUPPORT_EDITOR
            void            handleResourceErrors();
#endif //ITF_SUPPORT_EDITOR
        ObjectGroup             m_updateGroups[ObjectGroup::GroupIndex_Max];
        ObjectGroup             m_update2DGroups[ObjectGroup::GroupIndex_Max];
        ObjectGroup             m_drawGroups[ObjectGroup::GroupIndex_Max];
        SafeArray<class LightComponent*> m_activeLights;
        SafeArray<ITF_ParticleGenerator *>  m_particleGeneratorToLight;
    };

#define WORLD_MANAGER WorldManager::getptr()

} // namespace ITF

#endif // _ITF_WORLD_MANAGER_H_
