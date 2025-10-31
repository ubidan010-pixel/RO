#include "precompiled_gameplay.h"

#ifndef _ITF_AISENDEVENTACTION_H_
#include "gameplay/ai/Actions/AISendEventAction.h"
#endif //_ITF_AISENDEVENTACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_OBJBINDING_H_
#include "engine/actors/ObjBinding.h"
#endif //_ITF_OBJBINDING_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AISendEventAction_Template)
    IMPLEMENT_OBJECT_RTTI(AISendEventAction)

    BEGIN_SERIALIZATION_CHILD(AISendEventAction_Template)
        SERIALIZE_OBJECT_WITH_FACTORY   ( "eventToSend",        m_eventToSend,     ACTORSMANAGER->getEventFactory() );
        SERIALIZE_MEMBER                ( "triggerSelf",        m_triggerSelf  );
        SERIALIZE_MEMBER                ( "triggerChildren",    m_triggerChildren  );
        SERIALIZE_MEMBER                ( "triggerBinded",      m_triggerBinded  );
        SERIALIZE_MEMBER                ( "broadcast",          m_broadcast  );
        SERIALIZE_MEMBER                ( "virtualChild",       m_virtualChild  );
    END_SERIALIZATION()

        //-----------------------------------------------------------------------------------------------------
    AISendEventAction_Template::AISendEventAction_Template()
    : Super()
    , m_eventToSend(NULL) 
    , m_triggerSelf( btrue )
    , m_triggerChildren( btrue )
    , m_broadcast( bfalse )
    , m_triggerBinded( bfalse )
    {
    }
    AISendEventAction_Template::~AISendEventAction_Template()
    { 
        SF_DEL( m_eventToSend ); 
    }

    //-----------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------
    AISendEventAction::AISendEventAction()
        : Super()
        , m_eventToSend(NULL)
    {
    }

    //-----------------------------------------------------------------------------------------------------
    AISendEventAction::~AISendEventAction()
    {
        SF_DEL( m_eventToSend );
    }

    //-----------------------------------------------------------------------------------------------------
    void AISendEventAction::onActivate()
    {
        Super::onActivate();

        if ( m_eventToSend != NULL )
        {
            m_eventToSend->setSender( m_actor->getRef() );

            if ( getTemplate()->getTriggerSelf() )
            {
                m_actor->onEvent( m_eventToSend );
            }
            if ( getTemplate()->getTriggerChildren() )
            {
                LinkComponent * linkComp = m_actor->GetComponent<LinkComponent>();           // only needed if triggerChildren is true
                if ( linkComp )
                {
                    linkComp->sendEventToChildren( m_eventToSend );
                }
            }
            if ( getTemplate()->getBroadcast() )
            {
                EVENTMANAGER->broadcastEvent( m_eventToSend );
            }
            if ( getTemplate()->getTriggerBinded() )
            {
                const ObjectRefList& childrenBindList = m_actor->getChildrenBindHandler().getChildren();
                for ( u32 i = 0 ; i < childrenBindList.size(); ++i )
                {
                    if ( BaseObject * obj = childrenBindList[i].getObject() )
                    {
                        if ( Actor * actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                        {
                            actor->onEvent( m_eventToSend );
                        }
                    }
                }

                const ObjBinding::ChildrenList& aiBindList = m_actor->getBinding()->getChildren();
                
                for ( ObjBinding::ChildrenList::const_iterator it = aiBindList.begin(); it != aiBindList.end(); ++it )
                {
                    const BindData& data = *it;

                    if ( BaseObject * obj = data.m_child.getObject() )
                    {
                        if ( Actor * actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                        {
                            actor->onEvent( m_eventToSend );
                        }
                    }
                }
            }
        }
        if ( getTemplate()->getVirtualChild().isValid() )
        {
            EventVirtualLinkBroadcast broadcastVirtualChild( m_actor->getRef(), getTemplate()->getVirtualChild(), m_eventToSend, bfalse );
            EVENTMANAGER->broadcastEvent( &broadcastVirtualChild );
        }

        m_aiBehavior->onActionFinished();
    }

    //-----------------------------------------------------------------------------------------------------
    void AISendEventAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if ( getTemplate()->getEventToSend() != NULL )
        {
            m_eventToSend = static_cast<Event*>(getTemplate()->getEventToSend()->CreateNewObject() );
            BinaryClone(const_cast<Event*>(getTemplate()->getEventToSend()),m_eventToSend );
        }
    }
}