#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLEPRIZESPAWNERCOMPONENT_H_
#include "rayman/gameplay/components/BubblePrize/Ray_BubblePrizeSpawnerComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZESPAWNERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizeSpawnerComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BubblePrizeSpawnerComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BubblePrizeSpawnerComponent)
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_BubblePrizeSpawnerComponent::Ray_BubblePrizeSpawnerComponent()
    : m_curCountBubbleSpawned(0)
    , m_alreadyHit(bfalse)
    , m_state(STATE_DISABLED)
    , m_time(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_BubblePrizeSpawnerComponent::~Ray_BubblePrizeSpawnerComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_BubblePrizeSpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);  

        // Event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);

        // Spawner
        SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner, getTemplate()->getBubblePath() );

    }

    //*****************************************************************************

    void Ray_BubblePrizeSpawnerComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( PunchStim* evtOnHit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            receiveHit();
        }
    }

    //*****************************************************************************

    void Ray_BubblePrizeSpawnerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_state == STATE_SPAWNING)
        {
            m_time -= _dt;

            if (m_time <= 0.0f)
            {
                spawnBubble();
                m_curCountBubbleSpawned++;
                m_time = getTemplate()->getTimeBetweenBubble();
            }

            if (m_curCountBubbleSpawned >= getTemplate()->getBubbleCount())
            {
                m_state = STATE_SPAWNED;
            }
        }

    }

    //*****************************************************************************

    void Ray_BubblePrizeSpawnerComponent::receiveHit()
    {
        if (m_state == STATE_DISABLED)
        {
            bbool spawn = btrue;

            if ( getTemplate()->getSpawnBubbleOnDeath() )
            {
                spawn = bfalse;
                EventQueryIsDead isDead;
                m_actor->onEvent( &isDead );
                if ( isDead.getIsDead() )
                {
                    spawn = btrue;
                }
            }

            if ( spawn )
            {
                m_time = getTemplate()->getTimeBetweenBubble();
                m_state = STATE_SPAWNING;
            }
        }        
    }

    //*****************************************************************************

    void Ray_BubblePrizeSpawnerComponent::spawnBubble()
    {
        Actor* bubble = m_bubbleSpawner.getSpawnee(CURRENTWORLD->getRootScene(), m_actor->getPos() + Vec3d(1.0f, 1.0f, 0.0f));

        if (bubble)
        {
            if ( getTemplate()->getInternalRewardNumber() != U32_INVALID )
            {
                Ray_EventSetBubblePrizeRewardNumber rewardNumChangeEvt( getTemplate()->getInternalRewardNumber() );
                bubble->onEvent( &rewardNumChangeEvt );
            }

            Ray_EventChildLaunch launchBubble(m_actor->getPos());
            launchBubble.setSender(m_actor->getRef());
            bubble->onEvent(&launchBubble);
        }

    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_BubblePrizeSpawnerComponent_Template)
    BEGIN_SERIALIZATION(Ray_BubblePrizeSpawnerComponent_Template)
        SERIALIZE_MEMBER( "bubblePath",             m_bubblePath );
        SERIALIZE_MEMBER( "bubbleCount",            m_bubbleCount );
        SERIALIZE_MEMBER( "timeBetweenBubble",      m_timeBetweenBubble );
        SERIALIZE_MEMBER( "spawnBubbleOnDeath",     m_spawnBubbleOnDeath );
        SERIALIZE_MEMBER( "internalRewardNumber",   m_internalRewardNumber );
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_BubblePrizeSpawnerComponent_Template::Ray_BubblePrizeSpawnerComponent_Template()
    : m_bubbleCount(1)
    , m_timeBetweenBubble(0.2f)
    , m_spawnBubbleOnDeath( bfalse )
    , m_internalRewardNumber( U32_INVALID )
    {
        // none
    }

    //*****************************************************************************

    Ray_BubblePrizeSpawnerComponent_Template::~Ray_BubblePrizeSpawnerComponent_Template()
    {
        // none
    }

    //*****************************************************************************


}