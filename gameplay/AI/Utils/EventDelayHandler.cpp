#include "precompiled_gameplay.h"

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    EventDelayHandler* EventDelayHandler::s_instance = NULL;

    EventDelayHandler::EventDelayHandler()
    {
        m_delayedEvents_pending.reserve(10);
        m_delayedEvents.reserve(10);
    }

    EventDelayHandler::~EventDelayHandler()
    {
        reset();
    }

    void EventDelayHandler::reset()
    {
        for (u32 i = 0; i < m_delayedEvents_pending.size(); i++)
        {
            SF_DEL(m_delayedEvents_pending[i].m_event);
        }
        m_delayedEvents_pending.clear();

        for (u32 i = 0; i < m_delayedEvents.size(); i++)
        {
            SF_DEL(m_delayedEvents[i].m_event);
        }
        m_delayedEvents.clear();
    }

    void EventDelayHandler::sendEventToChildren( const LinkComponent* _linkComponent, Event* _event, bbool _discardChildWithTags /*= bfalse */ )
    {
        DelayedEvent delayedEvent;

        const LinkComponent::ChildrenList& children = _linkComponent->getChildren();
        for (u32 i = 0; i < children.size(); i++)
        {
            const ChildEntry& childEntry = children[i];

            f32 delay;
            bbool hasDelay = childEntry.getTagValue(ITF_GET_STRINGID_CRC(Delay,161465807), delay);

            if (!hasDelay)
            {
                if ( _discardChildWithTags && !childEntry.getTagValueList().empty() )
                {
                    continue;
                }
            }

            if ( Pickable* child = SceneObjectPathUtils::getObjectFromRelativePath(_linkComponent->GetActor(), childEntry.getPath()) )
            {
                if ( hasDelay && delay > 0.f )
                {
                    DelayedChild delayedChild;
                    delayedChild.m_ref = child->getRef();
                    delayedChild.m_delay = delay;
                    delayedEvent.m_children.push_back(delayedChild);
                }
                else
                {
                    child->onEvent(_event);
                }
            }
        }

        if (delayedEvent.m_children.size())
        {
            delayedEvent.m_event = static_cast<Event*>(_event->CreateNewObject());
            BinaryClone(_event, delayedEvent.m_event);
            m_delayedEvents_pending.push_back(delayedEvent);
        }
    }

    void EventDelayHandler::update( const f32 _dt )
    {
        for (u32 i = 0; i < m_delayedEvents_pending.size(); i++)
            m_delayedEvents.push_back(m_delayedEvents_pending[i]);
        m_delayedEvents_pending.clear();

        for (i32 i = 0; i < (i32)m_delayedEvents.size(); i++)
        {
            DelayedEvent& delayedEvent = m_delayedEvents[i];

            delayedEvent.update(_dt);

            if (delayedEvent.m_children.size() == 0)
            {
                SF_DEL(delayedEvent.m_event);
                m_delayedEvents.removeAt(i);
                i--;
            }
        }
    }

    void EventDelayHandler::DelayedEvent::update( const f32 _dt )
    {
        m_timer += _dt;

        for (i32 i = 0; i < (i32)m_children.size(); i++)
        {
            DelayedChild& delayedChild = m_children[i];

            if (m_timer > delayedChild.m_delay)
            {
                Pickable* child = static_cast<Pickable*>(delayedChild.m_ref.getObject());

                m_children.removeAt(i);
                i--;

                if (child)
                    child->onEvent(m_event);
            }
        }
    }

};