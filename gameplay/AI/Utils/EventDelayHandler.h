#ifndef _ITF_EVENTDELAYHANDLER_H_
#define _ITF_EVENTDELAYHANDLER_H_

namespace ITF
{
    #define EVENTDELAY_HANDLER EventDelayHandler::get()

    class EventDelayHandler
    {
        static EventDelayHandler*      s_instance;

    public:

        static void                 create()    { s_instance = newAlloc(mId_Singleton, EventDelayHandler()); }
        static void                 destroy()   { SF_DEL(s_instance); }
        static EventDelayHandler*   get()       { return s_instance; } 

        void sendEventToChildren( const class LinkComponent* _linkComponent, Event* _event, bbool _discardChildWithTags = bfalse );
        void update( const f32 _dt );
        void reset();

    private:
        EventDelayHandler();
        ~EventDelayHandler();

        class DelayedChild
        {
        public:
            ObjectRef m_ref;
            f32 m_delay;
        };
        typedef ITF_VECTOR<DelayedChild> DelayedChildList;

        class DelayedEvent
        {
        public:
            DelayedEvent()
                : m_event(NULL)
                , m_timer(0.f)
            {}

            Event* m_event;
            DelayedChildList m_children;
            f32 m_timer;

            void update( const f32 _dt );
        };
        typedef ITF_VECTOR<DelayedEvent> DelayedEventList;

        DelayedEventList m_delayedEvents;
        DelayedEventList m_delayedEvents_pending;
    };

};

#endif //_ITF_EVENTDELAYHANDLER_H_