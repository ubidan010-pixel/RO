#ifndef _ITF_EVENTMANAGER_H_
#define _ITF_EVENTMANAGER_H_

namespace ITF
{

    class Event;
    class IEventListener;

    class EventManager
    {
    public:
        void            registerEvent( StringID::StringIdValueType _crc, IEventListener* _listener );
        void            unregisterEvent( StringID::StringIdValueType _crc, IEventListener* _listener );
        void            broadcastEvent( Event* _event );
    private:

        typedef ITF_VECTOR<IEventListener*> RegisteredListenerArray;

        struct RegisteredEvent
        {
            StringID::StringIdValueType    m_eventCRC;
            RegisteredListenerArray        m_listeners;
        };

        typedef ITF_VECTOR<RegisteredEvent> RegisteredEventsArray;
        RegisteredEventsArray   m_registeredEvents;
    };


#define EVENTMANAGER_REGISTER_EVENT_LISTENER(crc,listener) EVENTMANAGER->registerEvent(crc,listener);

} // namespace ITF

#endif //_ITF_EVENTMANAGER_H_