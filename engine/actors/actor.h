
#ifndef _ITF_ACTOR_H_
#define _ITF_ACTOR_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
/*
//ugly dependencies
#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_*/

#ifndef _ITF_ACTORTYPES_H_
#include "engine/actors/Actor_Types.h"
#endif //_ITF_ACTORTYPES_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif //SERIALIZEROBJECT_H

#ifndef _ITF_OBJBINDING_H_
#include "engine/actors/ObjBinding.h"
#endif //_ITF_OBJBINDING_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_ACTOR_BIND_H_
#include "engine/actors/ActorBind.h"
#endif // _ITF_ACTOR_BIND_H_

namespace ITF
{

class Event;
class Texture;
class ResourceGroup;
class IEventListener;
class Actor;
class ObjectGroup;

class ActorRef : public ObjectRef
{
public:
    ActorRef() : ObjectRef() {}
    ActorRef(const ObjectRef & _other);
    Actor* getActor() const;
    void setActor( const Actor* _actor );
    ActorRef & operator =(const ObjectRef & _other) ;
};

class ActorDrawEditInterface
{
public:

    virtual void drawPolyLine(const class PolyLine* _pPolyLine, bbool _bSubElement = bfalse) = 0;
    virtual bbool showTextTips() const = 0;
};

struct ActorHotReload
{
    ActorRef        m_actorRef;
    bbool           m_enabled;
    bbool           m_resLoaded;
    bbool           m_skipInstanceData;
    ObjectRefList   m_childrenBeforeClear;
#ifndef ITF_FINAL
    bbool           m_actorWas2DBeforeHotReload;
#endif
};

class Actor : public Pickable
{
public:

    ITF_DECLARE_NEW_AND_DELETE_OPERATORS_CATEGORY(mId_Actor);
    DECLARE_OBJECT_CHILD_RTTI(Actor,Pickable,2546623115);
    DECLARE_SERIALIZE()

    typedef SafeArray<ActorComponent*> ComponentsVector;

    Actor();
    virtual ~Actor();

    static void initOnLoad_static(void * _context);

    virtual void            update( f32 _deltaTime );
    virtual void            draw();
    virtual void            draw2D();
    virtual void            onLoaded(HotReloadType _hotReload = HotReloadType_None);   
    virtual bbool           onLoadedFromScene(HotReloadType _hotReload = HotReloadType_None);   

    virtual void            onSceneCollect(ITF_VECTOR<Pickable*> &_vActor,ITF_VECTOR<Pickable*> &_vFrieze){};
    virtual void            onLoadedCheckpoint();
    void                    clear();
    ITF_INLINE void         clearRegisteredEvents() { m_registeredEvents.clear(); }
    virtual void            onAddedToScene(Scene* _pScene);
    virtual void            onRemovedFromScene(Scene* _pScene,bbool _forDeletion);
    virtual void            onSceneActive();
    virtual void            onSceneInactive();
    void                    onBecomeActive();
    void                    onBecomeInactive();
#ifdef ITF_SUPPORT_EDITOR
    void                    drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags  );
    virtual void            onEditorMove(bbool _modifyInitialPos = btrue);
    virtual void            onEditorCreated( Pickable* _original = NULL );  // the actor has just been created in the editor (drag'n'drop from a lua, ctrl+drag clone, or copy-paste clone)
#endif // ITF_SUPPORT_EDITOR
    virtual void            notifyParentsAndChildren (PickableEditorChangeType _type );
    virtual void            onChildChanged (ObjectRef _changedObject, PickableEditorChangeType _type);
    virtual void            onParentChanged (ObjectRef _changedObject, PickableEditorChangeType _type);
    virtual void            requestDestruction();
    void                    preChangeProperties();
    void                    postChangeProperties();
    void                    preSaveData();
    virtual u32             addObjectInGroupForUpdate(ObjectGroup* _listToFill, bbool _fromParent = bfalse);
    virtual void            addObjectInGroupForDraw();
    virtual void            registerContainedPickables(SafeArray<ActorRef>& _actorList, SafeArray<ObjectRef>& _frizeList) const;
    virtual void            deleteSpawnedActors();

    void                    AddComponent( class ActorComponent* _actorComponent, bbool _udpateLists = btrue );

	template <class T>
    T*                      GetComponent() const;
    template <class T>
    SafeArray<T*>           GetComponents() const;
    const ComponentsVector& GetAllComponents() const {return m_components; }
    ActorComponent*         getComponentFromStaticClassCRC(StringID::StringIdValueType _staticClassCRC) const;

	void                    ClearComponents(bbool _clearMem,bbool _clearComponents);

    bbool                   isUpdateAllowed() const;
    
    const class Actor_Template* getTemplate() const { return m_templateActor; }
    virtual const Path*     getTemplatePath() const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// create from an archive which contains all data to create an object
    /// @param _archive The archive
    /// @param _newScene The scene of the new object
    /// @param _needLoadTemplateFromACT btrue if the object should load the ACT first. Should be false if the archive contains template and instance data.
    /// @param _needsRegistering Set to btrue to have the object serialized into the ACTORMANAGER (for when we want to reinitialize it)
    static Actor *          createFromDataArchive(ArchiveMemory &_archive, Scene *_newScene, bbool _needLoadTemplateFromACT, bbool _needsRegistering);

    virtual Pickable *      clone( Scene* _pDstScene, bbool _callOnLoaded ) const;

    bbool                   isStartPaused() const;

    void                    setLua( const Path& _lua ) { m_lua = _lua; }
    const Path&             getLua() const { return m_lua; }

    f32                     getRadius() const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Change position of an object, and update all related lists accordingly if the new position is different.
    /// Also calls onForceMove() callback.
    /// @param _newPos The new position
    void                    forceMove(const Vec3d &_newPos);

    virtual void            onForceMove();
    void                    updateComponentPosAngle( const Vec3d& _pos, f32 _angle, class ActorComponent* _skip );
    ITF_INLINE void         setIsSpawned(bbool _spawned) { m_isSpawned = _spawned; }
    ITF_INLINE bbool        isSpawned() const { return m_isSpawned; }
    void                    unloadCells();

    bool                    isLuaLoaded() const { return m_components.size() > 0; }

    ActorBindHandler&        getChildrenBindHandler()        { return m_childrenHandler; }
    const ActorBindHandler&  getChildrenBindHandler()const   { return m_childrenHandler; }
    ActorBind*               getParentBind()const            { return m_pParentBind; }
    void                     setParentBind(ActorBind* _newParent);
    ActorRef                 getParent()const                { return m_pParentBind ? m_pParentBind->m_runtimeParent : ITF_INVALID_OBJREF; }
    void                     createParentBind( bbool _temporaryBind, Actor* _parent, ActorBind::Type _type, u32 _typeData, bbool _useParentFlip =btrue, bbool _useParentScale=bfalse, bbool _removeWithParent=bfalse );
    ITF_INLINE bbool         isParentBindRunTimeDisabled() const { return m_pParentBind && m_pParentBind->isRuntimeDisabled(); }
    
    void                    clearBinds(bbool _clearInitial);

    ObjBinding*             getBinding()                    { return &m_binding; }
    const ObjBinding*       getBinding() const              { return &m_binding; }

    virtual const Vec2d&    getLocalInitialScale() const;
    virtual f32             getLocalInitialZ() const;

    virtual void            setScale                        (const Vec2d & _scale );
    virtual     void        setDepth                        (   f32 _depth  );
    virtual bbool           isZForced                       ()const;
    virtual bbool           onBeginCellEnable               (){return bfalse;}

    void                    getLocalToWorldTransform( Vec3d& _parentPos, f32& _parentAngle ) const;
    Vec2d                   transformToWorld( const Vec2d& _localPos ) const;
    Vec3d                   transformToWorld( const Vec3d& _localPos ) const;

    Vec3d                   getBoundWorldInitialPos() const;
    Vec3d                   getBoundLocalInitialPos() const;
    void                    setBoundWorldInitialPos( const Vec3d& _pos );
    void                    setBoundWorldPos( const Vec3d& _pos );
    Vec3d                   getBoundLocalPos() const;
    void                    setBoundLocalPos( const Vec3d& _pos );

    const Vec2d&            getBoundLocalInitial2DPos() const;
    Vec2d                   getBoundLocal2DPos() const;
    void                    setBoundLocal2DPos( const Vec2d& _pos );

    f32                     getBoundWorldInitialAngle() const;
    f32                     getBoundLocalInitialAngle() const;
    void                    setBoundWorldInitialAngle(f32 _angle );
    void                    setBoundWorldAngle(f32 _angle );
    f32                     getBoundLocalAngle() const;
    void                    setBoundLocalAngle( f32 _angle );

    virtual void            setUpdateType(UpdateType _t, bbool _propagate = bfalse);



    virtual void            pauseComponent(StringID::StringIdValueType _crc);
    virtual void            unpauseComponent(StringID::StringIdValueType _crc);
    void                    pauseAllComponents();
    void                    unpauseAllComponents();
    virtual void            pauseUpdate() {m_updatePaused  = btrue;}
    virtual void            unpauseUpdate() {m_updatePaused = bfalse;}
    bbool                   isUpdatePaused() const { return m_updatePaused; }

    void                    disableDraw( bbool _val ) { m_drawDisabled = _val; }
    bbool                   isDrawDisabled() const { return m_drawDisabled; }

    virtual bbool           updatePhysicalReady();

    virtual void            unloadResources();
    virtual void            onResourceLoaded();
    static void             onResourceLoadedCallback(void * _context);;

    void                    startComputingNextAABB() {m_isComputingNextAABB = btrue;}
    void                    swapNextAABB();
    
    virtual void            resetNextAABB() {m_previousAABB = getRelativeAABB();m_nextRelativeAABB.setMin(Vec2d::Zero);m_nextRelativeAABB.setMax(Vec2d::Zero);}
    void                    growRelativeAABB (const AABB& _aabb );
    void                    growRelativeAABBFromAbsolute(const AABB& _absoluteAABB);

    void                    registerEvent( StringID::StringIdValueType _crc, IEventListener* _actorComponent );
    void                    unregisterEvent( StringID::StringIdValueType _crc, IEventListener* _listener );
    bbool                   isEventRegistered( StringID::StringIdValueType _crc );
    virtual void            onEvent(Event* _event);

    ITF_INLINE bbool        is2DActor() const {return m_is2DActor;}
    ITF_INLINE void         set2DActor(bbool _set = btrue) {m_is2DActor = _set;}
    Vec3d                   getBase() const;

    virtual bbool           canSerializeComponents() const {return btrue;}

    ResourceID              addResource( Resource::ResourceType _type, const Path& _path );
    void                    removeResource(const ResourceID &_resource);
    
	ResourceID				addResourceInTemplate(Resource::ResourceType _type, const Path& _path);
	void					removeResourceInTemplate(const ResourceID &_resource);
    
    ResourceGroup*          getResourceGroup();
    ResourceGroup*			getTemplateResourceGroup() { return getTemplate()->getTemplateResourceGroup(); }

    bbool                   startFlipped()const                     { return m_startFlipped; }
    void                    setStartFlipped( bbool _value );
    bbool                   isFlipped()const                        { return m_isFlipped; }
    virtual void            setFlipped( bbool _value );
    bbool                   isCleared() { return m_components.size() == 0; }

    void                    oneStepHotReload(bbool _skipInstanceData = bfalse);
    void                    fillHotReloadData(ActorHotReload & hotreloadInfo, bbool _skipInstanceData = bfalse);

    // Reload the actor completely from the file, keeping the same instance data
    virtual void            fillHotReloadDataAndHotReload( ActorHotReload& _hotreloadInfo, bbool _skipInstanceData, bbool _delayPostreload);
    virtual void            hotReload( ActorHotReload& _reloadInfo, bbool _useTwoStepHotreload = bfalse   );
    virtual void            postHotReload( const ActorHotReload& _reloadInfo, HotReloadType _hotReload = HotReloadType_None );
    // Reload the actor from an archive
    virtual void            hotReloadFromMem( ArchiveMemory* _archive );
    // Returns true if he needs to register his serializable data
    virtual bbool           needsRegisteringSerializableData() const { return m_needsRegisteringSerializableData; }   
    virtual void            saveCheckpointData();
    virtual void            postLoadCheckpointData();

    virtual void            replaceReinitDataByCurrent();
    virtual void            disable();

    virtual void            regenerateObjectId  (   );
    
    virtual void            setDataError (const String& _str);

#ifndef ITF_FINAL
    bbool                   validate();
#endif //!ITF_FINAL

    u32                     getLastUpdatedFrame()const { return m_lastUpdateFrame; }

    bbool                   isDestructionRequested() const {return m_destructionRequested;}

#ifndef ITF_FINAL   // for debug readability
    u32                     getUpdateComponentsCount() const {return m_updateComponents.size();}
    u32                     computeNbBones() const;
    u32                     getNbBones() const {return m_nbBones;}
#endif
    void                    setOverrideTemplate(Actor_Template*  _overrideTemplateActor, bbool _deleteWithActor = bfalse) { m_overrideTemplateActor = _overrideTemplateActor; m_deleteOverrideTemplateWithActor = _deleteWithActor; }
    Actor_Template*         getOverrideTemplate(  ) { return m_overrideTemplateActor; }
    void                    clearOverrideTemplate() { SF_DEL(m_overrideTemplateActor); }

    bbool                   isUsingTemplate(const class TemplateObj* _templateActor) { return m_templateActor == (const class Actor_Template *)_templateActor; }

    bbool                   isTemplateChanged()const                                { return m_templateChanged; }
    void                    setTemplateChanged(bbool _b)                            { m_templateChanged = _b; }

	const StringID&			getArchetype() const { return m_templateActor->getArchetype(); }
	const StringID&			getType() const { return m_templateActor->getType(); }

#ifdef ASSERT_ENABLED
    const HotReloadType&    getHotReloadingStatus() {return  m_hotReloadingStatus;}
#endif  //ASSERT_ENABLED

    ITF_INLINE void         needComponentDestructionValidation( bbool _needed )     { m_componentDestructionValidation = _needed; }
    ITF_INLINE bbool        isComponentDestructionValidationNeeded() const          { return m_componentDestructionValidation; }

protected:
    bbool                   m_startFlipped;

private:

    void                    unbindFromParent(bbool _changeInitialBind = bfalse);
    void                    setLUAAsDataError();

    // set the m_needsRegisteringSerializableData flag. If you call it, you must do it right after mallocating the object. The flag is not serialized.
    void                    setNeedsRegisteringSerializableData(bbool _register) {m_needsRegisteringSerializableData = _register;}

    void                    onLoadProcessTemplate();

    // call this after the local pos/angle have changed to recompute the world pos/angle
    void                    updateWorldCoordinates();

    typedef SafeArray<IEventListener*> RegisteredListenerArray;
    struct RegisteredEvent
    {
        const ObjectFactory::ClassInfo*     m_eventClass;
        RegisteredListenerArray             m_listeners;
    };
    typedef ITF_VECTOR<RegisteredEvent> RegisteredEventsArray;

    const class Actor_Template*  m_templateActor;
    class Actor_Template*       m_overrideTemplateActor;
    bbool                       m_deleteOverrideTemplateWithActor;

    ComponentsVector        m_components;
    ComponentsVector        m_updateComponents;
    ComponentsVector        m_drawComponents;
    ComponentsVector        m_draw2DComponents;
    RegisteredEventsArray   m_registeredEvents;
    ResourceID              m_resourceGroup;
    Path                    m_lua;
    ObjBinding              m_binding;
    u32                     m_lastUpdateFrame;

    AABB                    m_previousAABB;
    bbool                   m_isFlipped;
    bbool                   m_needsRegisteringSerializableData; //not serialized, because we can clone an actor (which has not this flag) and want the clone to be registered
    u32                     m_isSpawned : 1,
                            m_is2DActor : 1,
                            m_updatePaused : 1,
                            m_drawDisabled : 1,
                            m_templateChanged : 1,
                            m_componentDestructionValidation : 1,
                            m_templateClientAdded : 1;
	bbool					m_destructionRequested; // Flag used to avoid multi destruction request at the same frame

    ActorBindHandler        m_childrenHandler;  // runtime children list filled from children loading
	
#ifdef ASSERT_ENABLED
    HotReloadType           m_hotReloadingStatus;
#endif  //ASSERT_ENABLEDonLoadedFromScene
    static u32              s_groupCount;
#ifndef ITF_FINAL
    public:
    u32                     m_drawCount;
    mutable u32             m_nbBones;
#endif // ITF_FINAL

protected:
    ActorBind*              m_pParentBind;      // parent bind info to parent
};

template <class T>
T* Actor::GetComponent() const
{
     for ( u32 it = 0; it < m_components.size(); ++it )
     {
         ActorComponent* pComponent = m_components[it];
         ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");

         if ( pComponent && pComponent->IsClassCRC(T::GetClassCRCStatic()) )
         {
             return static_cast<T*>(pComponent);
         }
     }

     return NULL;
}

template <class T>
SafeArray<T*> Actor::GetComponents() const
{
    SafeArray<T*> componentsFound;

    for ( u32 it = 0; it < m_components.size(); ++it )
    {
        ActorComponent* pComponent = m_components[it];
        ITF_ASSERT_MSG(pComponent, "An actor could not have a NULL component");

        if ( pComponent && pComponent->IsClassCRC(T::GetClassCRCStatic()) )
        {
            componentsFound.push_back(static_cast<T*>(pComponent));
        }
    }

    return componentsFound;
}

ITF_INLINE bbool Actor::isStartPaused() const { return ( m_templateActor ? m_templateActor->getStartPaused() : bfalse ); }

#define ACTOR_REGISTER_EVENT_COMPONENT(actor,crc,component) actor->registerEvent(crc,component);

}

#endif // _ITF_ACTOR_H_
