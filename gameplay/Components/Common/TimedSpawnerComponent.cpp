#include "precompiled_gameplay.h"

#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#include "gameplay/Components/Common/TimedSpawnerComponent.h"
#endif //_ITF_TIMEDSPAWNERCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_


namespace ITF
{

    //---------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION(TimedSpawnerData)
        SERIALIZE_MEMBER("spawnDelay",                  m_spawnDelay);
        SERIALIZE_MEMBER("spawnRate",                   m_spawnRate);
        SERIALIZE_MEMBER("burstElementsCount",          m_burstElementsCount);
        SERIALIZE_MEMBER("burstCount",                  m_burstCount);
        SERIALIZE_MEMBER("burstDelay",                  m_burstDelay);
    END_SERIALIZATION()

    //---------------------------------------------------------------------------------------------
    TimedSpawner::TimedSpawner()
    : m_actor( NULL )
    , m_data(NULL)
    , m_onSpawnEvent(NULL)
    , m_createSpawneeCB(NULL)
    , m_spawneeValid( bfalse )
    , m_recycleSpawnee( bfalse )
    , m_useSpawnerScale( bfalse )
    , m_spawnFinished( bfalse )
    , m_timer(0)
    , m_spawnTimer(0)
    , m_activated( bfalse )
    , m_burstNb( 0 )
    , m_burstElementsNb( 0 )
    , m_burstTimer( 0.0f )
    , m_warmSpawnOnly( bfalse )
    , m_warmNeedSpawn( bfalse )
    , m_justSpawnedActor( NULL )
    {
    }

    TimedSpawner::~TimedSpawner()
    {
        SF_DEL( m_onSpawnEvent );
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawner::initialize( Actor * _actor, const TimedSpawnerData * _data, const Event * _onSpawnEvent  )
    {
        ITF_ASSERT( _actor != NULL );
        if ( _actor == NULL )
        {
            return;
        }
        m_actor = _actor;
        // store data
        m_data = _data;
        
        // Clone event
        if ( _onSpawnEvent != NULL )
        {
            m_onSpawnEvent = static_cast<Event*>(_onSpawnEvent->CreateNewObject() );
            BinaryClone(const_cast<Event*>(_onSpawnEvent),m_onSpawnEvent );
        }
        reset();
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawner::setActivate( bbool _activate, bbool _reset /*=btrue*/ )
    {
        ITF_ASSERT( m_actor != NULL );
        ITF_ASSERT( m_data != NULL );
        if ( ( m_actor == NULL ) || ( m_data == NULL ) )
        {
            return;
        }

        if ( m_activated != _activate )
        {
            if ( _activate && _reset )
            {
                reset();
            }
        }
        m_activated = _activate;
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawner::declareSpawnee( const Path & _path )
    {
        if ( !m_warmSpawnOnly )
        {
            ITF_WARNING( m_actor, !_path.isEmpty(), "## TimedSpawner ## has invalid actor spawn path !" );
            if ( !_path.isEmpty() )
            {
                m_spawneeValid = SPAWNER->declareNeedsSpawnee( m_actor, &m_generator, _path );
            }              
        }
    }

    //---------------------------------------------------------------------------------------------
    Actor * TimedSpawner::createSpawnee()
    {
        if ( m_spawneeValid )
        {
            Actor * spawnee = m_spawnedActor.getActor();
            if ( ( spawnee == NULL ) || !m_recycleSpawnee )
            {
                Scene * actorScene = m_actor->getScene();
                SubSceneActor* subSceneActor = actorScene->getSubSceneActorHolder();
                if ( subSceneActor != NULL )
                {
                    actorScene = subSceneActor->getScene();
                }

                spawnee = m_generator.getSpawnee( actorScene, m_actor->getPos(), m_actor->getAngle() );

                if ( ( spawnee != NULL ) && m_recycleSpawnee )
                {
                    m_spawnedActor = spawnee->getRef();
                }            
            }
            // reset the spawnee
            else if ( m_recycleSpawnee )
            {
                spawnee->enable(); 
                EventReset reset;
                spawnee->onEvent( &reset );
            }
            if ( spawnee != NULL )
            {
                if ( m_useSpawnerScale )
                {
                    spawnee->setScale( m_actor->getScale() );
                }
                spawnee->setPos( m_actor->getPos() );
                spawnee->setAngle( m_actor->getAngle() );
            }

            // CB call
            //-----------
            if ( m_createSpawneeCB != NULL )
            {
                m_createSpawneeCB( m_actor, spawnee );
            }
            // Send spawn event 
            //------------------
            if ( ( spawnee != NULL ) && ( m_onSpawnEvent != NULL ) )
            {   
                m_onSpawnEvent->setSender( m_actor->getRef() );
                spawnee->onEvent( m_onSpawnEvent );
            }

            return spawnee;
        }
        return NULL;            
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawner::update( f32 _deltaTime )
    {
        m_justSpawnedActor = NULL;
        m_warmNeedSpawn = bfalse;

        if ( m_activated )
        {
            m_timer += _deltaTime;
            if ( m_timer > m_data->m_spawnDelay )
            {
                if (    ( m_data->m_burstElementsCount >= 0 ) && ( m_burstElementsNb >= m_data->m_burstElementsCount ) &&
                        ( ( m_data->m_burstCount < 0 ) || ( m_burstNb < m_data->m_burstCount - 1 ) )
                    )
                {
                    m_burstTimer += _deltaTime;
                    if ( m_burstTimer >= m_data->m_burstDelay )
                    {
                        m_burstNb++;
                        m_burstTimer = 0.f;
                        m_burstElementsNb = 0;
                        m_spawnTimer = m_data->m_spawnRate;
                    }
                }

                if ( ( m_data->m_burstElementsCount < 0 ) || ( m_burstElementsNb < m_data->m_burstElementsCount ) )
                {
                    m_spawnTimer += _deltaTime;
                    if ( m_spawnTimer > m_data->m_spawnRate )
                    {
                        m_spawnTimer = 0;

                        if ( !m_warmSpawnOnly )
                        {
                            m_justSpawnedActor = createSpawnee();
                        }
                        else
                        {
                            m_warmNeedSpawn = btrue;
                        }
                        m_burstElementsNb++;
                    }
                }
            }

            // Disable if no more burst to do
            if (    ( m_data->m_burstCount >= 0 ) && ( m_burstNb >= m_data->m_burstCount - 1 ) &&
                    ( m_burstElementsNb >= m_data->m_burstElementsCount ) 
                )
            {
                m_spawnFinished = btrue;
                setActivate( bfalse );
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawner::reset()
    {
        m_timer = 0;
        m_spawnTimer = ( m_data != NULL )? m_data->m_spawnRate : 0.0f; // set to rate to get the first spawn just after delay
        m_burstTimer = 0.f;
        m_burstElementsNb = 0;
        m_burstNb = 0;
        m_spawnFinished = bfalse;
        m_justSpawnedActor = NULL;
        m_warmNeedSpawn = bfalse;
    }

    //---------------------------------------------------------------------------------------------
    bbool TimedSpawner::isBurstFinished() const                     
    { 
        if ( m_data == NULL )
        {
            return btrue;
        }

        if (    ( m_data->m_burstElementsCount >= 0 ) && ( m_burstElementsNb >= m_data->m_burstElementsCount ) &&
                ( ( m_data->m_burstCount < 0 ) || ( m_burstNb < m_data->m_burstCount - 1 ) )
            )
        {
            return btrue;
        }
        
        return ( m_data->m_burstElementsCount < 0 ) ? btrue : bfalse ; 
    }

    //---------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------


    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(TimedSpawnerComponent_Template)
    BEGIN_SERIALIZATION(TimedSpawnerComponent_Template)
        SERIALIZE_MEMBER("actorToSpawn",                m_actorToSpawn);
        SERIALIZE_MEMBER("spawnDelay",                  m_spawnDelay);
        SERIALIZE_MEMBER("spawnRate",                   m_spawnRate);
        SERIALIZE_MEMBER("burstElementsCount",          m_burstElementsCount);
        SERIALIZE_MEMBER("burstCount",                  m_burstCount);
        SERIALIZE_MEMBER("burstDelay",                  m_burstDelay);
        SERIALIZE_MEMBER("useInstanceValues",           m_useInstanceValues);
        SERIALIZE_MEMBER("recycling",                   m_recycling);
        SERIALIZE_OBJECT_WITH_FACTORY("startEvent",     m_startEvent,       ACTORSMANAGER->getEventFactory() );
		SERIALIZE_OBJECT_WITH_FACTORY("stopEvent",      m_stopEvent,        ACTORSMANAGER->getEventFactory() );		
        SERIALIZE_OBJECT_WITH_FACTORY("onSpawnEvent",   m_onSpawnEvent,     ACTORSMANAGER->getEventFactory() );		
    END_SERIALIZATION()

    TimedSpawnerComponent_Template::TimedSpawnerComponent_Template()
    : m_spawnDelay(0)
    , m_spawnRate(1)
    , m_burstCount(-1)
    , m_burstElementsCount(-1)
    , m_burstDelay( 0.0f )
	, m_startEvent( NULL )
	, m_stopEvent( NULL )
    , m_onSpawnEvent( NULL )
    , m_useInstanceValues( bfalse )
    , m_recycling( bfalse )
    {
    }

    TimedSpawnerComponent_Template::~TimedSpawnerComponent_Template()
    {
        SF_DEL( m_startEvent );
        SF_DEL( m_stopEvent );
        SF_DEL( m_onSpawnEvent );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TimedSpawnerComponent)

    BEGIN_SERIALIZATION(TimedSpawnerComponent)
        SERIALIZE_MEMBER("spawnDelay",          m_instance_SpawnDelay);
        SERIALIZE_MEMBER("spawnRate",           m_instance_spawnRate);
        SERIALIZE_MEMBER("burstElementsCount",  m_instance_burstElementsCount);
        SERIALIZE_MEMBER("burstCount",          m_instance_burstCount);
        SERIALIZE_MEMBER("burstDelay",          m_instance_burstDelay);
    END_SERIALIZATION()

    TimedSpawnerComponent::TimedSpawnerComponent()
        : Super()
        , m_instance_SpawnDelay( 0.f )
        , m_instance_spawnRate( 1.f )
        , m_instance_burstElementsCount( -1 )
        , m_instance_burstCount( -1 )
        , m_instance_burstDelay( 0.f )
        , m_modifierUsed( bfalse )
    {
    }

    TimedSpawnerComponent::~TimedSpawnerComponent()
    {
    }

    //---------------------------------------------------------------------------------------------
    const Path & TimedSpawnerComponent::getSpawnActorPath() const 
    { 
        return getTemplate()->getActorToSpawn(); 
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerComponent::declareSpawnee()
    {
        // Declare actor to spawn 
        const Path & actorToSpawnPath = getSpawnActorPath();
        m_timedSpawner.declareSpawnee( actorToSpawnPath );
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_timedSpawnerData.m_spawnDelay = getSpawnDelay();
        m_timedSpawnerData.m_spawnRate = getSpawnRate();
        m_timedSpawnerData.m_burstElementsCount = getBurstElementCount();
        m_timedSpawnerData.m_burstCount = getBurstCount();
        m_timedSpawnerData.m_burstDelay = getBurstDelay();

        m_timedSpawner.initialize( m_actor, &m_timedSpawnerData, getTemplate()->getOnSpawnEvent() );

        // Declare actor to spawn 
        declareSpawnee();
        
        // get Events
        //
		if ( getTemplate()->getStartEvent() != NULL )
		{
            m_timedSpawner.setActivate( bfalse );
            m_actor->disable();
			m_actor->registerEvent( getTemplate()->getStartEvent()->GetClassCRCStatic(), this );
		}
        else
        {
            m_timedSpawner.setActivate( btrue );
        }

		if ( getTemplate()->getStopEvent() != NULL )
		{
			m_actor->registerEvent( getTemplate()->getStopEvent()->GetClassCRCStatic(), this );
		}

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTimedSpawnerDataSet,436096372),this);
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerComponent::onCheckpointLoaded()
    {
        if ( getTemplate()->getStartEvent() != NULL )
        {
             m_actor->disable();
        }
    }

    //---------------------------------------------------------------------------------------------
	void TimedSpawnerComponent::onEvent( Event * _event)
	{
		if ( ( getTemplate()->getStartEvent() != NULL ) && _event->IsClassCRC( getTemplate()->getStartEvent()->GetObjectClassCRC() ) )
		{
            processStartEvent( _event );
		}
        else if ( ( getTemplate()->getStopEvent() != NULL ) && _event->IsClassCRC( getTemplate()->getStopEvent()->GetObjectClassCRC() ) )
		{
			processStopEvent( _event );
        }
        else if ( EventTimedSpawnerDataSet * _modifier = _event->DynamicCast<EventTimedSpawnerDataSet>(ITF_GET_STRINGID_CRC(EventTimedSpawnerDataSet,436096372)))
        {
            m_modifierUsed = btrue;
            TimedSpawnerData::EventToData( *_modifier, m_timedSpawnerData );
        }
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerComponent::processStartEvent( Event * _event )
    {
        m_timedSpawner.setActivate( btrue );
        m_actor->enable();
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerComponent::processStopEvent( Event * _event )
    {
        m_timedSpawner.setActivate( bfalse );
        disable();
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerComponent::Update( f32 _deltaTime )
    {
        if ( !m_modifierUsed )
        {
            m_timedSpawnerData.m_spawnDelay = getSpawnDelay();
            m_timedSpawnerData.m_spawnRate = getSpawnRate();
            m_timedSpawnerData.m_burstElementsCount = getBurstElementCount();
            m_timedSpawnerData.m_burstCount = getBurstCount();
            m_timedSpawnerData.m_burstDelay = getBurstDelay();
        }

        m_timedSpawner.update( _deltaTime );
        
        if ( m_timedSpawner.isSpawnFinished() )
        {
            disable();
        }
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerComponent::disable()
    {
         m_actor->disable();
    }

    //-------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(TimedSpawnerModifierComponent)
    IMPLEMENT_OBJECT_RTTI(TimedSpawnerModifierComponent_Template)

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerModifierComponent::onActorLoaded(Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        if ( getTemplate()->isTriggered() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        }

    }
    //---------------------------------------------------------------------------------------------
    void TimedSpawnerModifierComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if ( EventTrigger * evt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            sendEventToChildren();
        }
    }

    //---------------------------------------------------------------------------------------------
    void TimedSpawnerModifierComponent::sendEventToChildren()
    {
        LinkComponent * linkComp = m_actor->GetComponent<LinkComponent>();
        ITF_ASSERT( linkComp != NULL );
        if ( linkComp != NULL )
        {
            EventTimedSpawnerDataSet evtData;
            evtData.setSender( m_actor->getRef() );
            TimedSpawnerData::DataToEvent( getTimedSpawnerData(), evtData );
            linkComp->sendEventToChildren( &evtData );
        }
    }

    //---------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION(TimedSpawnerModifierComponent)
        SERIALIZE_OBJECT( "timedSpawnerData", m_timedSpawnerData );
    END_SERIALIZATION()
    
    BEGIN_SERIALIZATION(TimedSpawnerModifierComponent_Template)
        SERIALIZE_OBJECT( "timedSpawnerData",   m_timedSpawnerData );
        SERIALIZE_MEMBER( "useInstanceValues",  m_useInstanceValues );
        SERIALIZE_MEMBER( "isTriggered",        m_isTriggered );
    END_SERIALIZATION()


} // namespace
