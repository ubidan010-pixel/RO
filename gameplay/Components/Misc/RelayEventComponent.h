#ifndef _ITF_RELAYEVENTCOMPONENT_H_
#define _ITF_RELAYEVENTCOMPONENT_H_

namespace ITF
{
    class LinkComponent;

    /*
        I listen to events and relay them, to myself, my children (requires a LinkComponent) or for broadcast.
        I can change the event type (optional eventToRelay, relay the original if null).
    */
    class RelayEventComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RelayEventComponent, ActorComponent,1922165291);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        RelayEventComponent();
        virtual ~RelayEventComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );
        virtual void        Update( f32 _dt );

    private:

        const class RelayEventComponent_Template* getTemplate() const;

        struct RelayData;
        typedef ITF_VECTOR<RelayData> RelayDataList;

        LinkComponent*          m_linkComponent;
        RelayDataList           m_relays;
        bbool                   m_sendingEvent; // for EventActivate only

        void    sendEventToChildren( Event* _event );
    };

    //-------------------------------------------------------------------------------------

    class RelayEventComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RelayEventComponent_Template,TemplateActorComponent,101416385);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(RelayEventComponent);

    public:

        struct RelayData;
        typedef ITF_VECTOR<RelayData> RelayDataList;

        RelayEventComponent_Template();
        RelayEventComponent_Template(const RelayEventComponent_Template &) = delete;
        ~RelayEventComponent_Template();

        RelayEventComponent_Template & operator = (const RelayEventComponent_Template&) = delete;

        const RelayDataList& getRelayData() const { return m_relays; }

    private:

        RelayDataList                   m_relays;
    };

    ITF_INLINE const RelayEventComponent_Template* RelayEventComponent::getTemplate() const
    {
        return static_cast<const RelayEventComponent_Template*>(m_template);
    }



    struct RelayEventComponent_Template::RelayData
    {
        DECLARE_SERIALIZE()

        RelayData()
            : m_eventToListen(NULL)
            , m_eventToRelay(NULL)
            , m_delay(0.f)
            , m_replaceSender(bfalse)
            , m_replaceSenderByActivator(bfalse)
            , m_triggerSelf(btrue)
            , m_triggerChildren(btrue)
            , m_triggerBroadcast(bfalse)
        {}

        ~RelayData();

        Event*          m_eventToListen;
        Event*          m_eventToRelay;
        f32             m_delay;
        bbool           m_replaceSender;
        bbool           m_replaceSenderByActivator;
        bbool           m_triggerSelf;
        bbool           m_triggerChildren;
        bbool           m_triggerBroadcast;
    };

    typedef RelayEventComponent_Template::RelayData RelayDataTemplate;

    struct RelayEventComponent::RelayData
    {
        RelayData()
            : m_owner(NULL)
            , m_template(NULL)
            , m_eventToListen(NULL)
            , m_eventToRelay(NULL)
            , m_timer(-1.f)
            , m_sendingEvent(bfalse)
        {}
        ~RelayData();

        void            init( RelayEventComponent* _owner, const RelayDataTemplate* _template );
        void            onEvent( Event* _event );
        void            update( f32 _dt );
        void            relayEvent();

        RelayEventComponent*    m_owner;
        const RelayDataTemplate* m_template;
        Event*          m_eventToListen;
        Event*          m_eventToRelay;
        f32             m_timer;
        bbool           m_sendingEvent;
    };

};

#endif //_ITF_RELAYEVENTCOMPONENT_H_
