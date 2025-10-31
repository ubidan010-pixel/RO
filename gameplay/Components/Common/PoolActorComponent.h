#ifndef _ITF_POOLACTORCOMPONENT_H_
#define _ITF_POOLACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    //-------------------------------------------------------------------------------------
    //                              ACTOR POOL CLASS
    //-------------------------------------------------------------------------------------
    // Allocate and manage a given number of actors
    //-------------------------------------------------------------------------------------
    class ActorPool
    {
    public:
        ActorPool();
        virtual ~ActorPool();

        // the initialize function create the given number of actors
        // this function even check if the pool owner has usefull pool event registered
        void            initialize( Actor * _owner, u32 _reservedSize, const Path & _actorLua, bbool _allowDynamicAlloc = bfalse, bbool _loadResources = bfalse );

        // Pool manages 'essential' events like 'recycle' event.
        void            onEvent( Event * _evt );

        Actor *         getActor( const Vec3d & _pos, f32 _angle );
        void            recycleActor( const ActorRef & _actorRef );

        ITF_INLINE bbool isInitialized() const      { return m_generator.isInitialized(); }

        // This static function increment a global pool ID used by each pool class to have a unique identifier.
        static u32      getNewPoolID()              { if ( ( ms_Pool_RefId + 1 ) == U32_INVALID ) { ms_Pool_RefId = 0; return ms_Pool_RefId; } else {return (ms_Pool_RefId++); } }

    protected:

        void            destroy();
        void            createActor();
    
        SpawneeGenerator        m_generator;
        ActorRef                m_owner;
        u32                     m_poolID;
        bbool                   m_useDynamicAlloc;
        u32                     m_initReservedSize;

        SafeArray<ActorRef>     m_actorRefList;
        SafeArray<ActorRef>     m_freeActorRef;

        bbool                   m_loadResources;

    private:
        // static actor pool ID
        static u32              ms_Pool_RefId;
    };

    //-------------------------------------------------------------------------------------
    //                              POOL ACTOR COMPONENT
    //-------------------------------------------------------------------------------------
    // Component which must be on pool actor
    //-------------------------------------------------------------------------------------
    class PoolActorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PoolActorComponent, ActorComponent,1247828894);
        DECLARE_SERIALIZE()
    public:

        PoolActorComponent();
        virtual ~PoolActorComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onEvent( Event * _event );

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual bbool       isActorDestructionValidated();

    private:

        ITF_INLINE const class PoolActorComponent_Template*  getTemplate() const;

        ActorRef            m_poolOwner;
        u32                 m_poolID;

    };

    //-------------------------------------------------------------------------------------
    class PoolActorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PoolActorComponent_Template,TemplateActorComponent,3923000469);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PoolActorComponent);

    public:

        PoolActorComponent_Template();
        ~PoolActorComponent_Template() {}
    };


    const PoolActorComponent_Template*  PoolActorComponent::getTemplate() const {return static_cast<const PoolActorComponent_Template*>(m_template);}
};

#endif //_ITF_POOLACTORCOMPONENT_H_