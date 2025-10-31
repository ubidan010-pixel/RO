#include "precompiled_gameplay.h"

#ifndef _ITF_POOLACTORCOMPONENT_H_
#include "gameplay/Components/Common/PoolActorComponent.h"
#endif //_ITF_POOLACTORCOMPONENT_H_

namespace ITF
{
    u32 ActorPool::ms_Pool_RefId = 0;

    //---------------------------------------------------------------------------------------------------------------------------------
    ActorPool::ActorPool()
    : m_useDynamicAlloc( bfalse )
    , m_initReservedSize( 0 )
    , m_loadResources(bfalse)
    {
        m_poolID = getNewPoolID();
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    ActorPool::~ActorPool()
    {
        destroy();
        m_poolID = U32_INVALID;
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void ActorPool::destroy()
    {
        for ( u32 i = 0; i < m_actorRefList.size(); ++i )
        {
            Actor * actor = m_actorRefList[i].getActor();
            if ( actor != NULL )
            {
                EventPoolActorRegistration poolEvt( bfalse, ActorRef::InvalidRef, U32_INVALID );
                actor->onEvent( &poolEvt );
                actor->requestDestruction();
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void ActorPool::onEvent( Event * _evt )
    {
        if ( EventPoolActorRecycle * recycle = _evt->DynamicCast<EventPoolActorRecycle>(ITF_GET_STRINGID_CRC(EventPoolActorRecycle,1859888857)) )
        {
            ITF_ASSERT( recycle->getPoolID() != U32_INVALID );
            if ( recycle->getPoolID() == m_poolID )
            {
                recycleActor( recycle->getRegisteredActor() );
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void ActorPool::recycleActor( const ActorRef & _actorRef )
    {
#ifndef ITF_FINAL
        ITF_ASSERT( m_actorRefList.find(_actorRef) >= 0 );      // not in the init list, maybe another actor ??
#endif //ITF_FINAL

        Actor * actor = _actorRef.getActor();
        ITF_ASSERT( actor != NULL );

        if ( actor != NULL )
        {
            actor->disable();
            actor->unregisterFromWorldCells();
            if ( actor->isActive() )
            {
                actor->onBecomeInactive();
            }
            actor->setAllowedCell( bfalse );
        }

        m_freeActorRef.push_back( _actorRef );
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    Actor * ActorPool::getActor( const Vec3d & _pos, f32 _angle )
    {
        Actor * actor = NULL;

        if ( m_freeActorRef.size() > 0  )
        {
            ActorRef actorRef = m_freeActorRef.back();
            m_freeActorRef.pop_back();

            actor = actorRef.getActor();
            ITF_ASSERT( actor != NULL );

            if ( actor != NULL )
            {
                actor->setWorldInitialPos( _pos );
                actor->forceMove( _pos );
                actor->setWorldInitialRot( _angle );
                actor->setAngle( _angle );
                actor->resetCurrentInfos();
                actor->setAllowedCell( btrue );
                actor->registerInWorldCells();                
                actor->enable();
            }
        }
        else if ( m_useDynamicAlloc )
        {
            createActor();
            ITF_ASSERT( m_freeActorRef.size() != 0 );
            if ( m_freeActorRef.size() != 0 )
            {
                actor = getActor( _pos, _angle );
            }

            ITF_ASSERT_MSG( m_actorRefList.size() < ( m_initReservedSize + 10 ), "Pool dynamic actor creation, are you sure to not have a leak ? or not enought init reserved space ?" );
        }
        return actor;
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void ActorPool::initialize( Actor * _owner, u32 _reservedSize, const Path & _actorLua, bbool _allowDynamicAlloc /*= bfalse */, bbool _loadResources /*= bfalse */ )
    {
        ITF_ASSERT( !_actorLua.isEmpty() );
        ITF_ASSERT( _owner != NULL );
        ITF_ASSERT( m_poolID != U32_INVALID );

        if ( _owner == NULL )
            return;
        
        m_actorRefList.reserve( _reservedSize );
        m_freeActorRef.reserve( _reservedSize );

        m_owner = _owner->getRef();
        m_useDynamicAlloc = _allowDynamicAlloc;
        m_initReservedSize = _reservedSize;
        m_loadResources = _loadResources;

        SPAWNER->declareNeedsSpawnee( _owner, &m_generator, _actorLua );
        
        // create all actors
        for ( u32 i = 0; i < _reservedSize; ++i )
        {
            createActor();
        }

        // check if the pool owner has need events
#ifndef ITF_FINAL
        ITF_ASSERT_MSG( _owner->isEventRegistered( EventPoolActorRecycle::GetClassCRCStatic() ), "'Actor Pool' owner must manage and broadcast 'EventPoolActorRecycle' to his pool. Please register this event and broadcast it to pools !" );
#endif //ITF_FINAL

    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void ActorPool::createActor()
    {
        Actor * owner = m_owner.getActor();
        ITF_ASSERT( owner != NULL );
        if ( owner == NULL )
            return;

        Actor * theActor = m_generator.getSpawnee( owner->getScene(), Vec3d::Zero, 0.f  );
        ITF_ASSERT ( theActor != NULL );

        if ( theActor != NULL )
        {
            theActor->setIsSpawned( bfalse );
            theActor->needComponentDestructionValidation( btrue );
            theActor->disable();
            theActor->setAllowedCell( bfalse );

            if ( m_loadResources )
            {
                theActor->loadResources();
            }

            m_actorRefList.push_back( theActor->getRef() );
            m_freeActorRef.push_back( theActor->getRef() );

            // check if the created actor has a pool actor component
#ifndef ITF_FINAL
            ITF_ASSERT_MSG( theActor->GetComponent<PoolActorComponent>() != NULL, "Actor created by a pool must have the 'PoolActorComponent' !! " );
#endif //ITF_FINAL

            EventPoolActorRegistration registerEvt( btrue, m_owner, m_poolID );
            theActor->onEvent( &registerEvt );
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PoolActorComponent)

    BEGIN_SERIALIZATION_CHILD(PoolActorComponent)
    END_SERIALIZATION()

    //---------------------------------------------------------------------------------------------------------------------------------
    PoolActorComponent::PoolActorComponent()
    : Super()
    , m_poolID( U32_INVALID )
    {
        m_poolOwner.invalidate();
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    PoolActorComponent::~PoolActorComponent()
    {
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void PoolActorComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPoolActorRegistration,3036909603),this);
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void PoolActorComponent::onEvent( Event * _event )
    {
        Super::onEvent( _event );

        if ( EventPoolActorRegistration * poolEvt = _event->DynamicCast<EventPoolActorRegistration>(ITF_GET_STRINGID_CRC(EventPoolActorRegistration,3036909603)) )
        {
            if ( poolEvt->isRegistered() )
            {
                m_poolOwner = poolEvt->getPoolOwner();
                m_poolID = poolEvt->getPoolID();
            }
            else
            {
                m_poolOwner = ITF_INVALID_OBJREF;
                m_poolID = U32_INVALID;
            }
        }
    }

    //-------------------------------------------------------------------------------------
    bbool PoolActorComponent::isActorDestructionValidated() 
    { 
        if ( m_poolID != U32_INVALID )
        {
            Actor * owner = m_poolOwner.getActor();
            if ( owner != NULL )
            {
                EventPoolActorRecycle recycle( m_actor->getRef(), m_poolID );
                owner->onEvent( &recycle );
                return bfalse;
            }
        }
        return btrue;
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PoolActorComponent_Template)
    BEGIN_SERIALIZATION(PoolActorComponent_Template)
    END_SERIALIZATION()

    PoolActorComponent_Template::PoolActorComponent_Template()
    {
    }
};