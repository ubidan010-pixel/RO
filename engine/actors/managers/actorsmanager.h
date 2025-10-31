#ifndef _ITF_ACTORSMANAGER_H_
#define _ITF_ACTORSMANAGER_H_

#ifndef _ITF_WATCHDIRECTORYLISTENER_H
#include "engine/file/WatchDirectoryListener.h"
#endif  //_ITF_WATCHDIRECTORYLISTENER_H

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_



namespace ITF
{
    class Scene;
    class ResourceGroup;
    class SubSceneActor;
    class ObjectFactory;

    class ActorsManager :public WatchDirectoryListener
    {
    public:
        
        typedef void (*hotReloadCallback) (Actor* _actor, bbool _before);

        struct SpawnInfo
        {
            SpawnInfo();

            bbool           m_init;             // init from lua ?
            Vec3d           m_pos;              // initial position
            Path            m_luaPath;          // lua template path
            Scene*          m_pScene;           // container scene
            bbool           m_isInternal;       // is fully serialized in the scene ? (do not depend of lua file)
            bbool           m_serializable;     // is saved in the scene ?
            ResourceGroup*  m_pGroup;           // include resources in this group
        };

        struct ActorDataContainer
        {
            ActorDataContainer() : m_instance(NULL), m_internal(NULL), m_checkpoint(NULL) {}

            void dispose() { SF_DEL(m_instance); SF_DEL(m_internal); SF_DEL(m_checkpoint); }
            void clearCheckpointArchive() {SF_DEL(m_checkpoint);}

            ArchiveMemory*  m_instance;
            ArchiveMemory*  m_internal;
            ArchiveMemory*  m_checkpoint;
        };
        typedef ITF_MAP<ActorRef,ActorDataContainer> ActorDataContainerMap;


        ActorsManager();
        ~ActorsManager();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Spawn an actor and register it to a scene
        /// @param _pos The actor position
        /// @param _scene The container scene
        /// @param _luaPath The actor description
        /// @param _init 
        /// @return The actor spawned
        Actor*                          spawnActor( const Vec3d& _pos, Scene* _scene, const Path& _luaPath, bbool _init, ResourceGroup* _group = NULL );
        Actor*                          spawnActor( const SpawnInfo& _spawnInfo );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Spawn a subSceneActor and register it to a scene
        /// @param _pos The subSceneActor position
        /// @param _scene The container scene
        /// @param _luaPath The subSceneActor actor path
        /// @return The subSceneActor spawned
        SubSceneActor*                  spawnSubsceneActor( const Vec3d& _pos, Scene* _scene, const Path& _luaPath, bbool _init = btrue, ResourceGroup* _group = NULL );

        void                            update(f32 _elapsed);

        ObjectFactory*                  getActorComponentsFactory() const { return m_ActorComponentsFactory; }
        ObjectFactory*                  getAIBehaviorsFactory() const { return m_AIBehaviorsFactory; }
        ObjectFactory*                  getAIActionsFactory() const { return m_AIActionsFactory; }
        ObjectFactory*                  getSequenceEventFactory() const { return m_SeqEventFactory; }
        ObjectFactory*                  getEventFactory() const { return m_EventFactory; }
        ObjectFactory*                  getAnimTreeNodeFactory() const { return m_AnimTreeNodeFactory; }
        ObjectFactory*                  getMusicTreeNodeFactory() const { return m_MusicTreeNodeFactory; }

#ifdef ITF_SUPPORT_EDITOR
        void                            onModifiedFile(const String& _absoluteFilename);
#endif //ITF_SUPPORT_EDITOR

        bbool                           isActorRegistered(Actor * _actor)const;
        void                            registerSerializeDataActor(Actor * _actor);
        void                            unregisterSerializeDataActor(Actor * _actor);
        ActorDataContainer*             getSerializeDataActor(Actor * _actor);
        ActorDataContainerMap::iterator getActorDataContainerMapStart() {return m_serializeActorData.begin();}
        ActorDataContainerMap::iterator getActorDataContainerMapEnd() {return m_serializeActorData.end();}

        void                            setHotreloadCallback(hotReloadCallback _callback)   { m_hotReloadFct = _callback; }

#ifdef ITF_WINDOWS
        bbool                           isActorCooking()                     {return m_actorCooking;}
        void                            setActorCooking(bbool _actorCooking) {m_actorCooking = _actorCooking;}
#endif //ITF_WINDOWS

    private:

        template <class T>
        T*                              spawnActorImpl( const SpawnInfo& _spawnInfo );

        void                            postload();

#ifdef ITF_SUPPORT_EDITOR
        void                            hotReloadFromFile(const String & _filename);
#endif //ITF_SUPPORT_EDITOR

        void                            hotReloadActorsUsingResource(const ResourceID & _res);

        ObjectFactory*                  m_ActorComponentsFactory;
        ObjectFactory*                  m_AIBehaviorsFactory;
        ObjectFactory*                  m_AIActionsFactory;
        ObjectFactory*                  m_SeqEventFactory;
        ObjectFactory*                  m_EventFactory;
        ObjectFactory*                  m_AnimTreeNodeFactory;
        ObjectFactory*                  m_MusicTreeNodeFactory;

        ITF_MAP<ActorRef,ActorDataContainer>    m_serializeActorData;

        hotReloadCallback               m_hotReloadFct;

#ifdef ITF_WINDOWS
        bbool                           m_actorCooking;
#endif //ITF_WINDOWS
    };

}

#endif // _ITF_ACTORSMANAGER_H_
