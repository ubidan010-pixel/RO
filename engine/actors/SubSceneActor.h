#ifndef _ITF_SUBSCENEACTOR_H_
#define _ITF_SUBSCENEACTOR_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/Actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{
    class Scene;
    class SubSceneComponent;

    class SubSceneActor : public Actor
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(SubSceneActor,Actor,1336151817);
        DECLARE_SERIALIZE()

        SubSceneActor();
        ~SubSceneActor();

        void            postSerializeLoad();
        void            loadExternalScene();

        virtual Actor*  clone(Scene* _pDstScene, bbool _callOnLoaded)const;
        virtual void    update( f32 _deltaTime );

        virtual void    replaceReinitDataByCurrent();
        virtual void    storeCurrentToInitial();
        virtual void    pauseUpdate();
        virtual void    unpauseUpdate();
        virtual void    finalizeReload(bbool _wasResLoaded, bbool _wasActive, bbool _wasActiveInScene, bbool _wasAllowedForCell);
        virtual bbool   needsRegisteringSerializableData() const        { return bfalse; }

        virtual void    saveCheckpointData();
        virtual void    postLoadCheckpointData();
        virtual void    loadResources();
        virtual void    onLoadedCheckpoint();
        virtual u32     addObjectInGroupForUpdate(ObjectGroup* _listToFill, bbool _fromParent /*= bfalse*/);
        virtual void    addObjectInGroupForDraw();
        virtual bbool   updatePhysicalReady();
        void            growInternalRelativeAABBFromAbsolute(const AABB &_aabb, f32 _fZ);

        virtual void    setUpdateType(UpdateType _t, bbool _propagate = bfalse);

        virtual void    onSceneActive();

        //virtual void    resetNextAABB() {};

        Scene*          setSubScene(const String& _sceneRelativePath);
        void            setLoadedSubScene(Scene* _pScene);
        Scene*          getSubScene()const;
        bbool           canSerializeComponents() const                  { return bfalse; }
        const Path&     getSceneRelativePath()const                     { return m_SceneRelativePath; }
        void            setSceneRelativePath(const String& _relPath)    { m_SceneRelativePath = _relPath; }

        bbool           getEmbedScene()const                            { return m_embedScene;}
        void            setEmbedScene(bbool _b)                         { m_embedScene = _b; }

        virtual void    onLoaded(HotReloadType _hotReload = HotReloadType_None); // from actor/pickable
        virtual bbool   onLoadedFromScene(HotReloadType _hotReload = HotReloadType_None); // from actor/pickable
        void    onFinishLoadedFromScene(HotReloadType _hotReload = HotReloadType_None); // from actor/pickable

        virtual void    onSceneCollect(ITF_VECTOR<Pickable*> &_vActor,ITF_VECTOR<Pickable*> &_vFrieze);
        virtual void    hotReload( ActorHotReload& _hotreloadInfo, bbool _skipInstance = bfalse, bbool _delayPostreload = bfalse );
        virtual void    hotReload( ActorHotReload& _reloadInfo, bbool _delayPostreload );
        virtual void    registerContainedPickables(SafeArray<ActorRef>& _list, SafeArray<ObjectRef>& _frizeList) const;
        virtual void    deleteSpawnedActors();
        virtual void    postHotReload( const ActorHotReload& /*_reloadInfo*/, HotReloadType _hotReload = HotReloadType_None ) {}

        void            setAlpha(f32 _alpha);
        virtual void    onEvent(Event* _event);
        
        virtual void    setPos( const Vec3d& _pos );
	    virtual void    offsetPosition (   const Vec3d& _delta, bbool _applyToInitialValue );

        virtual void    setAngle(f32 _angle);
        virtual void    setScale(const Vec2d & _scale);

        virtual void    setFlipped(bbool _value);

        ITF_INLINE bbool isSinglePiece() const                          { return m_isSinglePiece; }
        ITF_INLINE void setSinglePiece(bbool _set)                      { m_isSinglePiece = _set; }

        void            onLoadReposition();
        
        virtual bbool   isZForced() const                               { return m_zLock; }
        void            setZLock(bbool _b);

        void            updateCurrentWorldDataContentFromLocal();

        void            flipContent();
        void            registerStaticFriezeAABB(const AABB& _aabb);

        void            setSinglePieceFromHierarchy()                   { m_isSinglePieceFromHierarchy = 1; }

    private:
        void            processAlpha(f32 _deltaTime); // process alpha when override Update

        void            offsetSubScene(const Vec3d& _delta, bbool _applyToInitialValue);
        void            rotateSubScene(f32 _deltaAngle, bbool _applyToInitialValue, Vec3d* _pOrigin);
        void            scaleSubScene( const Vec2d& _scale, bbool _applyToInitialValue );

        bbool               m_embedScene;
        bbool               m_isSinglePiece;
        Path                m_SceneRelativePath;
        ObjectRef           m_sceneRef;
        SubSceneComponent*  m_subsceneComponent;

        // alpha
        f32                 m_alpha;
        f32                 m_alphaSrc;
        f32                 m_alphaDst;
        f32                 m_faidingTime;
        f32                 m_faidingValue;
        bbool               m_zLock;
        AABB                m_staticFriezesAABB;
        u32                 m_repositionedOnLoad : 1,
                            m_isSinglePieceFromHierarchy : 1;
    };

}

#endif
