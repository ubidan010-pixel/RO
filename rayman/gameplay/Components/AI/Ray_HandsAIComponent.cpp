#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_HANDSAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_HandsAIComponent.h"
#endif //_ITF_RAY_HANDSAICOMPONENT_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_HandsAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_HandsAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_HandsAIComponent)
            VALIDATE_COMPONENT_PARAM("", m_playerDetectorComponent, "Ray_WaterHandsAIComponent requires a PlayerDetectorComponent");
    END_VALIDATE_COMPONENT()

    // ****************************************************************************

    Ray_HandsAIComponent::Ray_HandsAIComponent()
    : m_playerDetectorComponent(NULL)
    {
        // none
    }

    // ****************************************************************************

    Ray_HandsAIComponent::~Ray_HandsAIComponent()
    {
        // none
    }

    // ****************************************************************************

    void Ray_HandsAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get Components
        m_playerDetectorComponent = m_actor->GetComponent<PlayerDetectorComponent>();   // mandatory, validated

        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);



    }

    // ****************************************************************************
  
    void Ray_HandsAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event); 
     
        if (AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            const StringID s_MRKCatch = ITF_GET_STRINGID_CRC(MRK_Catch,14748549);
            const StringID s_MRKFullopen = ITF_GET_STRINGID_CRC(MRK_FullOpen,3291432407);

            if (animEvent->getName() == s_MRKCatch || animEvent->getName() == s_MRKFullopen)
            {
               Actor* parentActor = AIUtils::getActor(m_actor->getBinding()->getParent());
               if (parentActor)
               {
                    animEvent->setSender(m_actor->getRef());
                    parentActor->onEvent(_event);
               }
            }

        }

    }

    //*****************************************************************************
        
    Actor* Ray_HandsAIComponent::getPlayerDetected() const
    {
        // TODO : remplacer par detectorComponent tout cours ???
        ActorRefList& listPlayer = m_playerDetectorComponent->getActorsInside();

        if (listPlayer.size() > 0)
        {
            return listPlayer[0].getActor();
        }

        return NULL;
 
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_HandsAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_HandsAIComponent_Template)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_HandsAIComponent_Template::Ray_HandsAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************

    Ray_HandsAIComponent_Template::~Ray_HandsAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************




}

