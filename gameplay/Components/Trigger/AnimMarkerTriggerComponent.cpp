#include "precompiled_gameplay.h"

#ifndef _ITF_ANIMMARKERANIMMARKERTRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/AnimMarkerTriggerComponent.h"
#endif //_ITF_ANIMMARKERANIMMARKERTRIGGERCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AnimMarkerTriggerComponent)

    BEGIN_SERIALIZATION_CHILD(AnimMarkerTriggerComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(AnimMarkerTriggerComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "AnimMarkerTriggerComponent requires an AnimLightComponent/AnimatedCommponent");
    END_VALIDATE_COMPONENT()

    AnimMarkerTriggerComponent::AnimMarkerTriggerComponent()
    : m_animComponent(NULL)
    {
    }

    AnimMarkerTriggerComponent::~AnimMarkerTriggerComponent()
    {
        clear();
    }



    void AnimMarkerTriggerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        clear();

        const SafeArray<MarkerEvent> & markerEvents = getTemplate()->getMarkerEvents();
        for (u32 i = 0; i < markerEvents.size(); ++i)
        {
            const MarkerEvent & markerEvent = markerEvents[i];
            if (markerEvent.m_marker.isValid() && markerEvent.m_event)
            {
                Event * event = static_cast<Event*>(markerEvent.m_event->CreateNewObject());
                BinaryClone(const_cast<Event*>(markerEvent.m_event),event);
                MarkerEvent me;
                me.m_event = event;
                me.m_marker = markerEvent.m_marker;
                m_markerEvents.push_back(me);
            }
        }

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
    }


    void AnimMarkerTriggerComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (AnimGameplayEvent* gpEvt = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            for (u32 i =0; i < m_markerEvents.size(); ++i)
            {
                MarkerEvent & me = m_markerEvents[i];

                if (me.m_marker == gpEvt->getName())
                {
                    sendEvent(m_actor->getRef(),me.m_event);
                }
            }
        }
    }

    void AnimMarkerTriggerComponent::clear()
    {
        for (u32 i =0; i < m_markerEvents.size(); ++i)
        {
            SF_DEL(m_markerEvents[i].m_event);
        }
        m_markerEvents.clear();
    }



    //-------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION(MarkerEvent)
    SERIALIZE_MEMBER("marker",m_marker);
    SERIALIZE_OBJECT_WITH_FACTORY("event", m_event, ACTORSMANAGER->getEventFactory());
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(AnimMarkerTriggerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(AnimMarkerTriggerComponent_Template)
    SERIALIZE_CONTAINER_OBJECT("markerEvents",m_markerEvents);
    END_SERIALIZATION()

    AnimMarkerTriggerComponent_Template::AnimMarkerTriggerComponent_Template()
    {
    }

    AnimMarkerTriggerComponent_Template::~AnimMarkerTriggerComponent_Template()
    {
    }

} // namespace ITF
