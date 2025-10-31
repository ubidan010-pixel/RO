#include "precompiled_gameplay.h"

#ifndef _ITF_SIMPLEAICOMPONENT_H_
#include "gameplay/Components/AI/SimpleAIComponent.h"
#endif //_ITF_SIMPLEAICOMPONENT_H_

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

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(SimpleAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(SimpleAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()

    SimpleAIComponent_Template::SimpleAIComponent_Template()
    : m_deathBehavior(NULL)
    , m_receiveHitBehavior(NULL)
    {
    }

    SimpleAIComponent_Template::~SimpleAIComponent_Template()
    {
        SF_DEL(m_deathBehavior);
        SF_DEL(m_receiveHitBehavior);
    }


    IMPLEMENT_OBJECT_RTTI(SimpleAIComponent)
    BEGIN_SERIALIZATION_CHILD(SimpleAIComponent)
    END_SERIALIZATION()

    SimpleAIComponent::SimpleAIComponent()
        : Super()
        , m_deathBehavior(NULL)
        , m_receiveHitBehavior(NULL)
    {
    }

    SimpleAIComponent::~SimpleAIComponent()
    {
    }

    void SimpleAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_deathBehavior = createAiBehavior(getTemplate()->getDeathBehavior());
        m_receiveHitBehavior = static_cast<AIReceiveHitBehavior*>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));

        if (m_receiveHitBehavior)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);

        Super::onActorLoaded(_hotReload);
    }

    void SimpleAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        setBehavior(m_behavior);
    }

    void SimpleAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if (getHealth() <= 0 && m_deathBehavior && m_currentBehavior != m_deathBehavior )
        {
            setBehavior(m_deathBehavior);
        }
    }

    void SimpleAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if (m_currentBehavior == m_receiveHitBehavior)
        {
            setBehavior(m_behavior);
        }
    }

    void SimpleAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventReset,2551104629)) )
        {
            setBehavior(m_behavior);
        }
        else if (m_receiveHitBehavior)
        {
            if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
            {
                processHit(hit);
            }
        }
    }

    void SimpleAIComponent::processHit( HitStim* _hit )
    {
        if (m_currentBehavior == m_receiveHitBehavior ||
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
        if ( !AIUtils::isEnemyHit(_hit, getFaction()) )
        {
            return;
        }

        // notify the sender that he touched us
        Actor* sender = static_cast<Actor*>(_hit->getSender().getObject());
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }

        setBehavior(m_receiveHitBehavior);
        m_receiveHitBehavior->setData(_hit);
    }
}