#include "precompiled_gameplay.h"

#ifndef _ITF_RELAYEVENTCOMPONENT_H_
#include "gameplay/Components/Misc/RelayEventComponent.h"
#endif //_ITF_RELAYEVENTCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_



namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(RelayEventComponent)

    BEGIN_SERIALIZATION_CHILD(RelayEventComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(RelayEventComponent)
        bbool requiresLinkComponent = bfalse;
        for (u32 i = 0; i < getTemplate()->getRelayData().size(); i++)
        {
            if (getTemplate()->getRelayData()[i].m_triggerChildren)
            {
                requiresLinkComponent = btrue;
                break;
            }
        }
        VALIDATE_COMPONENT_PARAM("", !requiresLinkComponent || m_linkComponent, "RelayEventComponent requires a LinkComponent to trigger children.");
    END_VALIDATE_COMPONENT()


    RelayEventComponent::RelayEventComponent()
        : m_linkComponent(NULL)
        , m_sendingEvent(bfalse)
    {
    }

    RelayEventComponent::~RelayEventComponent()
    {
    }

    RelayEventComponent::RelayData::~RelayData()
    {
        SF_DEL(m_eventToListen);
        SF_DEL(m_eventToRelay);
    }


    void RelayEventComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        if (m_linkComponent)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);
        }

        m_relays.resize(getTemplate()->getRelayData().size());

        for (u32 i = 0; i < m_relays.size(); i++)
        {
            const RelayEventComponent_Template::RelayData& relayTemplate = getTemplate()->getRelayData()[i];
            m_relays[i].init(this, &relayTemplate);
        }
    }

    void RelayEventComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        // It's difficult to handle EventActivate with a data-driver relays,
        // because we have to deactivate immediately, but activate with delay.
        // We don't know if we should replaceSender though, not sure if it's important...
        if ( EventActivate* eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
        {
            ITF_ASSERT(m_linkComponent);
            if (m_linkComponent)
            {
                ITF_WARNING_CATEGORY(LD, m_actor, !m_sendingEvent, "[RelayEventComponent] Infinite loop detected! You're reacting to the event you're sending; did you want to triggerChildren and not triggerSelf?");
                if (m_sendingEvent)
                {
                    return;
                }

                m_sendingEvent = btrue;


                if (eventActivate->getActivated())
                {
                    // activate with delay
                    sendEventToChildren(eventActivate);
                }
                else
                {
                    // deactivate directly
                    m_linkComponent->sendEventToChildren(eventActivate);
                }


                m_sendingEvent = bfalse;
            }
        }
        else
        {
            for (u32 i = 0; i < m_relays.size(); i++)
            {
                m_relays[i].onEvent(_event);
            }
        }
    }

    void RelayEventComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        for (u32 i = 0; i < m_relays.size(); i++)
        {
            m_relays[i].update(_dt);
        }
    }

    void RelayEventComponent::sendEventToChildren( Event* _event )
    {
        if (m_linkComponent)
        {
            EVENTDELAY_HANDLER->sendEventToChildren(m_linkComponent, _event, btrue);
        }
    }

    void RelayEventComponent::RelayData::init( RelayEventComponent* _owner, const RelayDataTemplate* _template )
    {
        m_template = _template;

        if ( m_template->m_eventToListen )
        {
            m_eventToListen = m_template->m_eventToListen->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
            BinaryClone(const_cast<Event*>(m_template->m_eventToListen), m_eventToListen);
        }

        if ( m_template->m_eventToRelay )
        {
            m_eventToRelay = m_template->m_eventToRelay->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
            BinaryClone(const_cast<Event*>(m_template->m_eventToRelay), m_eventToRelay);

            if ( m_template->m_replaceSender)
            {
                m_eventToRelay->setSender(_owner->GetActor()->getRef());
            }
        }


        if (!m_eventToListen ||
            !m_eventToRelay)
        {
            ITF_WARNING(_owner->GetActor(), 0, "empty event!");
            return;
        }

        m_owner = _owner;
        m_owner->GetActor()->registerEvent(m_eventToListen->GetObjectClassCRC(), m_owner);
    }

    void RelayEventComponent::RelayData::onEvent( Event* _event )
    {
        if (!m_eventToListen ||
            !m_eventToRelay)
        {
            return;
        }

        if ( _event->IsClassCRC(m_eventToListen->GetObjectClassCRC()) )
        {
            if (!m_template->m_replaceSender)
            {
                m_eventToRelay->setSender( _event->getSender() );
            }
            if (m_template->m_replaceSenderByActivator)
            {
                m_eventToRelay->setSender( _event->getActivator() );
            }

            m_eventToRelay->setActivator( _event->getActivator() );

            if ( m_template->m_delay <= 0)
            {
                relayEvent();
            }
            else
            {
                m_timer = m_template->m_delay;
            }
        }
    }

    void RelayEventComponent::RelayData::update( f32 _dt )
    {
        if (m_timer < 0)
        {
            return;
        }

        m_timer -= _dt;

        if (m_timer < 0)
        {
            relayEvent();
            m_timer = -1;
        }
    }

    void RelayEventComponent::RelayData::relayEvent()
    {
        ITF_WARNING_CATEGORY(LD, m_owner->GetActor(), !m_sendingEvent, "[RelayEventComponent] Infinite loop detected! You're reacting to the event you're sending; did you want to triggerChildren and not triggerSelf?");
        if (m_sendingEvent)
        {
            return;
        }

        m_sendingEvent = btrue;


        if (m_template->m_triggerSelf)
        {
            m_owner->GetActor()->onEvent(m_eventToRelay);
        }

        if (m_template->m_triggerChildren)
        {
            m_owner->sendEventToChildren(m_eventToRelay);
        }

        if (m_template->m_triggerBroadcast)
        {
            EVENTMANAGER->broadcastEvent(m_eventToRelay);
        }


        m_sendingEvent = bfalse;
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(RelayEventComponent_Template)
    BEGIN_SERIALIZATION(RelayEventComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("relays", m_relays);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(RelayEventComponent_Template, RelayData)
        SERIALIZE_OBJECT_WITH_FACTORY("eventToListen", m_eventToListen, ACTORSMANAGER->getEventFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("eventToRelay", m_eventToRelay, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("delay", m_delay);
        SERIALIZE_MEMBER("triggerSelf", m_triggerSelf);
        SERIALIZE_MEMBER("triggerChildren", m_triggerChildren);
        SERIALIZE_MEMBER("triggerBroadcast", m_triggerBroadcast);
        SERIALIZE_MEMBER("replaceSender", m_replaceSender);
        SERIALIZE_MEMBER("replaceSenderByActivator", m_replaceSenderByActivator);
    END_SERIALIZATION()

    RelayEventComponent_Template::RelayEventComponent_Template() = default;
    RelayEventComponent_Template::~RelayEventComponent_Template() = default;

    RelayEventComponent_Template::RelayData::~RelayData()
    {
        SF_DEL(m_eventToListen);
        SF_DEL(m_eventToRelay);
    }

};
