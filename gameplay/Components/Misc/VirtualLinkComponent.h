#ifndef _ITF_VIRTUALLINKCOMPONENT_H_
#define _ITF_VIRTUALLINKCOMPONENT_H_

namespace ITF
{
    class VirtualLinkComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(VirtualLinkComponent,ActorComponent,2794321058);
        DECLARE_SERIALIZE()

    public:

        VirtualLinkComponent();
        ~VirtualLinkComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event * _event);
        //virtual void        Update( f32 _deltaTime );
        virtual void        onBecomeActive();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE ActorRef getLastBroadcastEmitter() const   { return m_lastBroadcastEmitter; }

        static void         broadcastEventToChannel( ActorRef _emitter, const StringID & _channelID, Event * _broadcastEvent, bbool _childQuery = bfalse );
        
    protected:
        void                        requestChildren();
        ITF_INLINE const StringID & getChannelID() const;
        void                        broadcastEventToChildren( Event * _event );

    private:

        ITF_INLINE const class VirtualLinkComponent_Template*  getTemplate() const;

        ActorRef                m_lastBroadcastEmitter;
        SafeArray<ActorRef>     m_childrenQueryList;
        bbool                   m_childrenQueryDone;
        SafeArray<ActorRef>     m_childrenQueryParentList;
        StringID                m_instanceChannelID;
        
    };

    //---------------------------------------------------------------------------------------------------

    class VirtualLinkComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(VirtualLinkComponent_Template,TemplateActorComponent,4122908951);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(VirtualLinkComponent);

    public:

        VirtualLinkComponent_Template();
        ~VirtualLinkComponent_Template();

        ITF_INLINE const StringID&              getChannelID() const            { return m_channelID; }
        ITF_INLINE const ITF_VECTOR<Event*> &   getBroadcastEventList() const   { return m_broadcastEventList; }
        ITF_INLINE const bbool                  isEmitter() const               { return m_emitter; }
        ITF_INLINE const bbool                  isReceiver() const              { return m_receiver; }
        bbool                                   isEventInBoadcastList( Event * _event ) const;
        
    private:

        bbool                       m_emitter;
        bbool                       m_receiver;
        StringID                    m_channelID;
        ITF_VECTOR<Event*>          m_broadcastEventList;
    };



    ITF_INLINE const VirtualLinkComponent_Template*  VirtualLinkComponent::getTemplate() const  { return static_cast<const VirtualLinkComponent_Template*>(m_template);}
    ITF_INLINE const StringID &                      VirtualLinkComponent::getChannelID() const { return ( m_instanceChannelID.isValid() ? m_instanceChannelID : getTemplate()->getChannelID() ); }

}

#endif // _ITF_VIRTUALLINKCOMPONENT_H_

