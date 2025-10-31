#include "precompiled_engine.h"

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_


#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif

namespace ITF
{

    void EventManager::registerEvent( StringID::StringIdValueType _crc, IEventListener* _listener )
    {
        RegisteredListenerArray* registeredComponents(NULL);

        for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); it++)
        {
            RegisteredEvent& registeredEvent = *it;

            if ( _crc == registeredEvent.m_eventCRC )
            {
                registeredComponents = &registeredEvent.m_listeners;
                break;
            }
        }

        if ( !registeredComponents )
        {
#ifdef ITF_WII
            MEM_M_PushExt(MEM_C_MEM1_ALLOC);
#endif
            m_registeredEvents.push_back(RegisteredEvent());
#ifdef ITF_WII
            MEM_M_PopExt();
#endif
            RegisteredEvent& registeredEvent = m_registeredEvents.back();
            registeredEvent.m_eventCRC = _crc;
            registeredComponents = &registeredEvent.m_listeners;
        }

        RegisteredListenerArray::iterator it = find(registeredComponents->begin(), registeredComponents->end(), _listener);
        if (it == registeredComponents->end())
        {
            registeredComponents->push_back(_listener);
        }
    }

    void EventManager::unregisterEvent( StringID::StringIdValueType _crc, IEventListener* _listener )
    {
        for (RegisteredEventsArray::iterator it = m_registeredEvents.begin(); it != m_registeredEvents.end(); it++)
        {
            RegisteredEvent& registeredEvent = *it;

            if ( _crc == registeredEvent.m_eventCRC )
            {
                RegisteredListenerArray& registeredComponents = registeredEvent.m_listeners;

                RegisteredListenerArray::iterator it = find(registeredComponents.begin(), registeredComponents.end(), _listener);
                if (it != registeredComponents.end())
                {
                    registeredComponents.erase(it);
                }
                // else: assert?

                break;
            }
        }
    }

    void EventManager::broadcastEvent( Event* _event )
    {
        const u32 eventCount = m_registeredEvents.size();
        for ( u32 i = 0; i < eventCount; i++ )
        {
            RegisteredEvent& registeredEvent = m_registeredEvents[i];

            if ( _event->IsClassCRC(registeredEvent.m_eventCRC) )
            {
                const u32 listenerCount = registeredEvent.m_listeners.size();
                for ( u32 j = 0; j < listenerCount; j++ )
                {
                    registeredEvent.m_listeners[j]->onEvent(_event);
                }

                // don't break because components could have registered at different inheritance levels
            }
        }
    }


}; //namespace ITF