#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SIMPLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SimpleAIComponent.h"
#endif //_ITF_RAY_SIMPLEAICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_SimpleAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_SimpleAIComponent)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_SimpleAIComponent::Ray_SimpleAIComponent()
    : m_behavior(NULL)
    , m_deathBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    {
        // none
    }

    //*****************************************************************************

    Ray_SimpleAIComponent::~Ray_SimpleAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_behavior              = createAiBehavior(getTemplate()->getBehavior());
        m_deathBehavior         = createAiBehavior(getTemplate()->getDeathBehavior());
        m_receiveHitBehavior    = static_cast<AIReceiveHitBehavior*>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));

        if (m_receiveHitBehavior)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);

        Super::onActorLoaded(_hotReload);
        setBehavior(m_behavior);
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        setBehavior(m_behavior);
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        updateBehaviors(_dt);

    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::updateBehaviors( f32 _dt )
    {
        if (getHealth() <= 0)
        {
            updateDeath(_dt);
            return;
        }
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::updateDeath( f32 _dt )
    {
        if (m_currentBehavior == m_receiveHitBehavior)
        {
            return;
        }
        else if (m_currentBehavior != m_deathBehavior)
        {
            startDeath();
        }
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::startDeath()
    {
        setBehavior(m_deathBehavior);
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if (m_currentBehavior == m_receiveHitBehavior)
        {
            onReceiveHitBehaviorFinished();
        }
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::onReceiveHitBehaviorFinished()
    {
        if (getHealth() <= 0)
        {
            startDeath();
            return;
        }

        setBehavior(m_behavior);
    }

    //*****************************************************************************

    void Ray_SimpleAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventReset,2551104629)) )
        {
            setBehavior( m_behavior, btrue );
        }
        else if (m_receiveHitBehavior)
        {
            if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
            {
                processHit(hit);
            }
            else if (AnimGameplayEvent* onGameplayEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
            {
                if (getHealth() <= 0 && onGameplayEvent->getName() == MRK_Death)
                {
                    startDeath();
                }
            }
        }
     }

    //*****************************************************************************

    void Ray_SimpleAIComponent::processHit( HitStim* _hit )
    {
        if ( ( ( m_currentBehavior == m_receiveHitBehavior ) && !getTemplate()->getAllowMultiHit() ) ||
            m_currentBehavior == m_deathBehavior)
        {
            return;
        }

        if ( _hit->getSender() == m_actor->getRef() )
        {
            // don't get hit by our own stims
            return;
        }

        // can't get hit by friendlies, except for domino hits
        if ( !m_receiveHitBehavior->canReceiveHit(_hit))
        {
            return;
        }

        setBehavior(m_receiveHitBehavior);

        if ( m_receiveHitBehavior == m_currentBehavior )
        {
            i32 prevHealth = m_health_current;
            m_receiveHitBehavior->setData(_hit);

            // notify the sender that he touched us
            Actor* sender = static_cast<Actor*>(_hit->getSender().getObject());
            if (sender)
            {
                EventHitSuccessful hitSuccessful;
                hitSuccessful.setSender( m_actor->getRef() );
                hitSuccessful.setHasBeenDamaged( prevHealth != m_health_current );
                sender->onEvent(&hitSuccessful);
            }
        }        
    }


    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_SimpleAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_SimpleAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("genericBehavior",    m_behavior,             ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior,   ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior",      m_deathBehavior,        ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER( "allowMultiHit", m_allowMultiHit );
    END_SERIALIZATION()

    Ray_SimpleAIComponent_Template::Ray_SimpleAIComponent_Template()
    : m_behavior(NULL)
    , m_deathBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    , m_allowMultiHit( bfalse )
    {
        // none
    }

    //*****************************************************************************

    Ray_SimpleAIComponent_Template::~Ray_SimpleAIComponent_Template()
    {
        SF_DEL(m_behavior);
        SF_DEL(m_deathBehavior);
        SF_DEL(m_receiveHitBehavior);
    }

    //*****************************************************************************


}