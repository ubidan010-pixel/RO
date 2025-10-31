#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLESPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleSpawnerComponent.h"
#endif //_ITF_RAY_BUBBLESPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleAiComponent.h"
#endif //_ITF_RAY_BUBBLEAICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BubbleSpawnerComponent_Template)

    BEGIN_SERIALIZATION(Ray_BubbleSpawnerComponent_Template)
        SERIALIZE_MEMBER("bubblePath", m_bubblePath);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BubbleSpawnerComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BubbleSpawnerComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("bubbleLifetime", m_bubbleLifetime);
            SERIALIZE_MEMBER("bubbleFloatForceX", m_bubbleFloatForceX);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BubbleSpawnerComponent)
        VALIDATE_COMPONENT_PARAM_CATEGORY(LD, "bubblePath", m_bubbleSpawner.isInitialized(), "invalid actor path for spawning");
    END_VALIDATE_COMPONENT()

    Ray_BubbleSpawnerComponent::Ray_BubbleSpawnerComponent()
        : m_bubbleLifetime(10.f)
        , m_bubbleFloatForceX(0.5f)
    {
    }

    Ray_BubbleSpawnerComponent::~Ray_BubbleSpawnerComponent()
    {
    }


    void Ray_BubbleSpawnerComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

        SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner , getTemplate()->getBubblePath());
    }

    void Ray_BubbleSpawnerComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);
        
        if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if (trigger->getActivated())
            {
                spawn();
            }
        }
    }

    void Ray_BubbleSpawnerComponent::spawn()
    {
        Scene* scene = m_actor->getWorld()->getRootScene(); // don't use the actor's scene because he may be grouped and we don't want the bubble to be grouped
        if ( Actor* bubble = m_bubbleSpawner.getSpawnee(scene, m_actor->getPos()) )
        {
            Ray_BubbleAiComponent* bubbleComponent = bubble->GetComponent<Ray_BubbleAiComponent>();
            bubbleComponent->setOwner(m_actor->getRef());
            //bubbleComponent->setScale(m_fastScale);
            bubbleComponent->setLifetime(m_bubbleLifetime);
            bubbleComponent->setFloatForceX(m_bubbleFloatForceX);
        }
    }

};