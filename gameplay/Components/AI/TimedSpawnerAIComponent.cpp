#include "precompiled_gameplay.h"

#ifndef _ITF_TIMEDSPAWNERAICOMPONENT_H_
#include "gameplay/Components/AI/TimedSpawnerAIComponent.h"
#endif //_ITF_TIMEDSPAWNERAICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(TimedSpawnerAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(TimedSpawnerAIComponent)
        SERIALIZE_OBJECT( "spawnData", m_timedSpawnerDataInstance );
    END_SERIALIZATION()

    //--------------------------------------------------------------------------------------------------------------------------
    TimedSpawnerAIComponent::TimedSpawnerAIComponent()
    : m_disableBehavior(NULL)
    , m_spawnBehavior(NULL)
    , m_activateBehavior(NULL)
    , m_anticipateBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    , m_deathBehavior(NULL)
    {
    }

    TimedSpawnerAIComponent::~TimedSpawnerAIComponent()
    {
    }

    //--------------------------------------------------------------------------------------------------------------------------
    void TimedSpawnerAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_disableBehavior   = createAiBehavior(getTemplate()->getDisableBehavior());
        m_activateBehavior  = createAiBehavior(getTemplate()->getActivateBehavior());
        m_spawnBehavior     = createAiBehavior(getTemplate()->getSpawnBehavior());
        m_anticipateBehavior= createAiBehavior(getTemplate()->getAnticipateBehavior());
        m_receiveHitBehavior= static_cast<AIReceiveHitBehavior*>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));
        m_deathBehavior     = createAiBehavior(getTemplate()->getDeathBehavior());
        

        ITF_ASSERT(m_spawnBehavior);

        Super::onActorLoaded(_hotReload);

        if ( !getTemplate()->getTimedSpawnerTemplate()->useInstanceValues() )
        {
            m_timedSpawnerData.m_spawnDelay             = getTemplate()->getTimedSpawnerTemplate()->getSpawnDelay();
            m_timedSpawnerData.m_spawnRate              = getTemplate()->getTimedSpawnerTemplate()->getSpawnRate();
            m_timedSpawnerData.m_burstElementsCount     = getTemplate()->getTimedSpawnerTemplate()->getBurstElementCount();
            m_timedSpawnerData.m_burstCount             = getTemplate()->getTimedSpawnerTemplate()->getBurstCount();
            m_timedSpawnerData.m_burstDelay             = getTemplate()->getTimedSpawnerTemplate()->getBurstDelay();
        }

        m_timedSpawner.initialize( m_actor, &m_timedSpawnerData, getTemplate()->getTimedSpawnerTemplate()->getOnSpawnEvent() );
        m_timedSpawner.warmSpawnOnly( getTemplate()->getTimedSpawnerTemplate()->getActorToSpawn().isEmpty() ? btrue : bfalse );
        m_timedSpawner.declareSpawnee( getTemplate()->getTimedSpawnerTemplate()->getActorToSpawn() );
        m_timedSpawner.recycleSpawnee( getTemplate()->getTimedSpawnerTemplate()->getRecycling() );

        // get Events
        //
        if ( getTemplate()->getTimedSpawnerTemplate()->getStartEvent() != NULL )
        {
            m_actor->registerEvent( getTemplate()->getTimedSpawnerTemplate()->getStartEvent()->GetObjectClassCRC(), this );
            m_timedSpawner.setActivate( bfalse );

            if ( m_disableBehavior )
            {
                setBehavior(m_disableBehavior, btrue );
            }            
        }
        else
        {
            m_timedSpawner.setActivate( btrue );
            if ( m_activateBehavior )
            {
                setBehavior(m_activateBehavior, btrue );
            }  
        }

        if ( getTemplate()->getTimedSpawnerTemplate()->getStopEvent() != NULL )
        {
            m_actor->registerEvent( getTemplate()->getTimedSpawnerTemplate()->getStopEvent()->GetObjectClassCRC(), this );
        }

        if ( m_receiveHitBehavior != NULL )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------
    void TimedSpawnerAIComponent::onEvent( Event* _event)
    {
        Super::onEvent( _event );

        bbool activate = bfalse;
        bbool desactivate = bfalse;

        if ( ( getTemplate()->getTimedSpawnerTemplate()->getStartEvent() != NULL ) && _event->IsClassCRC( getTemplate()->getTimedSpawnerTemplate()->getStartEvent()->GetObjectClassCRC() ) )
        {
            activate = btrue;

            if (EventTrigger * trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
            {
                if ( !trigger->getActivated() )
                {
                    activate = bfalse;
                }
            }        
        }
        
        if ( !activate && ( getTemplate()->getTimedSpawnerTemplate()->getStopEvent() != NULL ) && _event->IsClassCRC( getTemplate()->getTimedSpawnerTemplate()->getStopEvent()->GetObjectClassCRC() ) )
        {
            desactivate = btrue;

            if (EventTrigger * trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
            {
                if ( trigger->getActivated() )
                {
                    desactivate = bfalse;
                }
            }
        }

        if ( activate && ( m_currentBehavior != m_deathBehavior ) )
        {
            m_timedSpawner.setActivate( btrue );

            if ( m_activateBehavior )
            {
                setBehavior( m_activateBehavior );
            } 
        }
        if ( desactivate && ( m_currentBehavior != m_deathBehavior ) )
        {      
            m_timedSpawner.setActivate( bfalse );

            if ( m_disableBehavior )
            {
                setBehavior( m_disableBehavior );
            } 
        }

        if ( m_receiveHitBehavior != NULL )
        {
            if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
            {
                processHit(hit);
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------
    void TimedSpawnerAIComponent::processHit( HitStim * _hitStim )
    {
        if ( m_currentBehavior == m_deathBehavior )
        {
            return;
        }

        if ( _hitStim->getSender() == m_actor->getRef() )
        {
            // don't get hit by our own stims
            return;
        }

        // can't get hit by friendlies, except for domino hits
        if ( !m_receiveHitBehavior->canReceiveHit(_hitStim))
        {
            return;
        }

        setBehavior(m_receiveHitBehavior);

        if ( m_receiveHitBehavior == m_currentBehavior )
        {
            i32 prevHealth = m_health_current;
            m_receiveHitBehavior->setData(_hitStim);

            // notify the sender that he touched us
            Actor* sender = static_cast<Actor*>(_hitStim->getSender().getObject());
            if (sender)
            {
                EventHitSuccessful hitSuccessful;
                hitSuccessful.setSender( m_actor->getRef() );
                hitSuccessful.setHasBeenDamaged( prevHealth != m_health_current );
                sender->onEvent(&hitSuccessful);
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------
    void TimedSpawnerAIComponent::Update( f32 _dt )
    {
        Super::Update( _dt );

        if ( getTemplate()->getTimedSpawnerTemplate()->useInstanceValues() )
        {
            m_timedSpawnerData.m_spawnDelay         = m_timedSpawnerDataInstance.m_spawnDelay;
            m_timedSpawnerData.m_spawnRate          = m_timedSpawnerDataInstance.m_spawnRate;
            m_timedSpawnerData.m_burstElementsCount = m_timedSpawnerDataInstance.m_burstElementsCount;
            m_timedSpawnerData.m_burstCount         = m_timedSpawnerDataInstance.m_burstCount;
            m_timedSpawnerData.m_burstDelay         = m_timedSpawnerDataInstance.m_burstDelay;
        }

        // don't update the timed spawner if in hit or death
        //
        if ( !( ( m_currentBehavior != NULL ) && ( ( m_currentBehavior == m_receiveHitBehavior ) || ( m_currentBehavior == m_deathBehavior ) ) ) )
        {
            m_timedSpawner.update( _dt );

            if ( ( m_anticipateBehavior != NULL )
                && m_timedSpawner.isActivated()
                && ( m_timedSpawner.getNextSpawnDelay() <= getTemplate()->getAnticipateDuration() )
                && ( m_currentBehavior != m_anticipateBehavior )
                )
            {
                setBehavior( m_anticipateBehavior, btrue );
            }

            if ( ( m_spawnBehavior != NULL ) && ( m_timedSpawner.warmIsSpawnNeeded() || ( m_timedSpawner.getJustSpawnedActor() != NULL ) ) )
            {
                setBehavior( m_spawnBehavior, btrue );
            }
        }        
    }

    //--------------------------------------------------------------------------------------------------------------------------
    void TimedSpawnerAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if ( m_currentBehavior == m_receiveHitBehavior )
        {
            if ( ( getHealth() == 0 ) && ( m_deathBehavior != NULL ) )
            {
                setBehavior( m_deathBehavior );
            }
            else if ( m_timedSpawner.isActivated() )
            {
                setBehavior( m_activateBehavior );
            }
            else if ( m_disableBehavior != NULL )
            {
                setBehavior( m_disableBehavior );
            }
        }
        else if ( ( m_disableBehavior != NULL ) && m_timedSpawner.isSpawnFinished() )
        {
            setBehavior( m_disableBehavior );
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(TimedSpawnerAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(TimedSpawnerAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("disableBehavior",        m_disableBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("activateBehavior",       m_activateBehavior, ACTORSMANAGER->getAIBehaviorsFactory()); 
        SERIALIZE_OBJECT_WITH_FACTORY("anticipateBehavior",     m_anticipateBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("spawnBehavior",          m_spawnBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior",     m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior",          m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());

        SERIALIZE_OBJECT( "timedSpawner",                       m_timedSpawnerTemplate );
        SERIALIZE_MEMBER( "anticipateDuration",                 m_anticipateDuration );
    END_SERIALIZATION()

        TimedSpawnerAIComponent_Template::TimedSpawnerAIComponent_Template()
        : m_disableBehavior(NULL)
        , m_activateBehavior(NULL)
        , m_spawnBehavior(NULL)
        , m_anticipateBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_anticipateDuration( 0.0f )
    {
    }

    TimedSpawnerAIComponent_Template::~TimedSpawnerAIComponent_Template()
    {
        SF_DEL(m_disableBehavior);
        SF_DEL(m_activateBehavior);
        SF_DEL(m_spawnBehavior);
        SF_DEL(m_anticipateBehavior);
        SF_DEL(m_receiveHitBehavior);
        SF_DEL(m_deathBehavior);
    }



}
