#include "precompiled_gameplay.h"

#ifndef _ITF_VIRTUALLINKCOMPONENT_H_
#include "gameplay/components/misc/VirtualLinkComponent.h"
#endif //_ITF_VIRTUALLINKCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_


namespace ITF
{

IMPLEMENT_OBJECT_RTTI(VirtualLinkComponent)

BEGIN_SERIALIZATION_CHILD(VirtualLinkComponent)
    SERIALIZE_MEMBER( "channelID", m_instanceChannelID );
END_SERIALIZATION()

//------------------------------------------------------------------------------------------------------------------------
VirtualLinkComponent::VirtualLinkComponent()
: Super()
, m_childrenQueryDone( bfalse )
{
}

//------------------------------------------------------------------------------------------------------------------------
VirtualLinkComponent::~VirtualLinkComponent()
{
    EVENTMANAGER->unregisterEvent( ITF_GET_STRINGID_CRC(EventVirtualLinkBroadcast,3923486039), this );
}

//------------------------------------------------------------------------------------------------------------------------
void VirtualLinkComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    // receiver registers to virtual link broadcast event
    //
    if ( getTemplate()->isReceiver() )
    {
        // register in manager and actor to allow mutli way to receive this event 
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventVirtualLinkBroadcast,3923486039),this);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventVirtualLinkBroadcast,3923486039),this);
    }

    // Transmitter registers to broadcast events
    //
    if ( getTemplate()->isEmitter() )
    {
        for ( u32 i = 0; i < getTemplate()->getBroadcastEventList().size(); ++i )
        {
            const Event * evt = getTemplate()->getBroadcastEventList()[i];
            ITF_ASSERT( evt != NULL );
            if ( evt != NULL )
            {
                m_actor->registerEvent( evt->GetObjectClassCRC(), this );
            }        
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventVirtualLinkChildQuery,940758855),this);
        m_childrenQueryList.clear();
        m_childrenQueryDone = bfalse;
    }
}

//------------------------------------------------------------------------------------------------------------------------
void VirtualLinkComponent::onBecomeActive()
{
    Super::onBecomeActive();

    if ( getTemplate()->isEmitter() && !m_childrenQueryDone )
    {
        requestChildren();
    }
}

//------------------------------------------------------------------------------------------------------------------------
void VirtualLinkComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    // receiver check virtual link event
    //
    if ( getTemplate()->isReceiver() )
    {
        if ( EventVirtualLinkBroadcast* evt = _event->DynamicCast<EventVirtualLinkBroadcast>(ITF_GET_STRINGID_CRC(EventVirtualLinkBroadcast,3923486039)) )
        {
            if ( ( evt->getChannelID() == getChannelID() ) && ( m_actor->getRef() != evt->getEmitter() ) )
            {
                // child query
                if ( evt->isChildQuery() )
                {
                    EventVirtualLinkChildQuery query( m_actor->getRef() );
                    Actor * emitter = evt->getEmitter().getActor();
                    ITF_ASSERT( emitter != NULL );
                    if ( emitter != NULL )
                    {
                        if ( m_childrenQueryParentList.find( evt->getEmitter()) < 0 )
                        {
                            m_childrenQueryParentList.push_back(evt->getEmitter());
                        }
                        emitter->onEvent( &query );
                    }                
                }
                // broadcast event
                else
                {
                    // save the emitter
                    m_lastBroadcastEmitter = evt->getEmitter();
                    
                    ITF_ASSERT( evt->getBroadcastEvent() != NULL );
                    if ( evt->getBroadcastEvent() != NULL )
                    {
                        m_actor->onEvent( evt->getBroadcastEvent() );
                    }
                }
            }
        }
    }

    if ( getTemplate()->isEmitter() )
    {
        // child query result
        if ( EventVirtualLinkChildQuery* childQueryRes = _event->DynamicCast<EventVirtualLinkChildQuery>(ITF_GET_STRINGID_CRC(EventVirtualLinkChildQuery,940758855)) )
        {
            ITF_ASSERT( childQueryRes->getChild().isValid() );
            i32 idx = m_childrenQueryList.find( childQueryRes->getChild() );
            if ( idx < 0 )
            {
                m_childrenQueryList.push_back( childQueryRes->getChild() );
            }        
        }
        else if ( getTemplate()->isEventInBoadcastList(_event) )
        {
            broadcastEventToChildren( _event );
        }
    }    
}


//------------------------------------------------------------------------------------------------------------------------
void VirtualLinkComponent::requestChildren()
{
     if ( getTemplate()->isEmitter() )
     {
         if ( !m_childrenQueryDone )
         {
             m_childrenQueryList.clear();
             EventVirtualLinkBroadcast broadcast( m_actor->getRef(), getChannelID(), NULL, btrue );
             EVENTMANAGER->broadcastEvent(&broadcast);
             m_childrenQueryDone = btrue;
         }
     }
}

//------------------------------------------------------------------------------------------------------------------------
void VirtualLinkComponent::broadcastEventToChildren( Event * _event )
{
    if ( getTemplate()->isEmitter() )
    {
        // request children before if not yet
        if ( !m_childrenQueryDone )
        {
            requestChildren();
        }

        EventVirtualLinkBroadcast broadcast( m_actor->getRef(), getChannelID(), _event, bfalse );

        if ( !m_childrenQueryDone )
        {
            EVENTMANAGER->broadcastEvent( &broadcast );
        }
        else
        {
            for ( u32 i = 0; i < m_childrenQueryList.size(); ++i )
            {
                if ( Actor * child = AIUtils::getActor( m_childrenQueryList[i] ) )
                {
                    child->onEvent( &broadcast );
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------
void VirtualLinkComponent::broadcastEventToChannel( ActorRef _emitter, const StringID & _channelID, Event * _broadcastEvent, bbool _childQuery /*= bfalse*/ )
{
    EventVirtualLinkBroadcast broadcast( _emitter, _channelID, _broadcastEvent, _childQuery );
    EVENTMANAGER->broadcastEvent( &broadcast );
}

////------------------------------------------------------------------------------------------------------------------------
//void VirtualLinkComponent::Update( f32 _deltaTime )
//{
//    if ( getTemplate()->isEmitter() && !m_childrenQueryDone )
//    {
//        requestChildren();
//    }
//}

#ifdef ITF_SUPPORT_EDITOR
//------------------------------------------------------------------------------------------------------------------------
void VirtualLinkComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    Super::drawEdit( _drawInterface, _flags );

    if ( getTemplate()->isEmitter() && m_childrenQueryDone )
    {
        for( u32 i = 0; i < m_childrenQueryList.size(); ++i )
        {
            Actor * child = m_childrenQueryList[i].getActor();
            if ( child != NULL )
            {
                DebugDraw::arrow3D( m_actor->getPos(), child->getPos(), Color(1.f, 1.f, 1.f, 0.f ), 5.f, 30.0, 40.0 );
            }
        }
    }
    else if ( getTemplate()->isReceiver() )
    {
        for( u32 i = 0; i < m_childrenQueryParentList.size(); ++i )
        {
            Actor * parent = m_childrenQueryParentList[i].getActor();
            if ( parent != NULL )
            {
                DebugDraw::arrow3D( parent->getPos(), m_actor->getPos(), Color(1.f, 1.f, 1.f, 0.f ), 5.f, 30.0, 40.0 );
            }
        }
    }
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(VirtualLinkComponent_Template)
BEGIN_SERIALIZATION(VirtualLinkComponent_Template)
    SERIALIZE_MEMBER                    ( "channelID",          m_channelID );
    SERIALIZE_MEMBER                    ( "emitter",            m_emitter );
    SERIALIZE_MEMBER                    ( "receiver",           m_receiver );
    SERIALIZE_CONTAINER_WITH_FACTORY    ( "broadcastEventList", m_broadcastEventList, ACTORSMANAGER->getEventFactory() );
END_SERIALIZATION()

//------------------------------------------------------------------------------------------------------------------------
VirtualLinkComponent_Template::VirtualLinkComponent_Template()
: Super()
, m_emitter( bfalse )
, m_receiver( btrue )
{
}

VirtualLinkComponent_Template::~VirtualLinkComponent_Template()
{
    for ( u32 i = 0; i < m_broadcastEventList.size(); ++i )
    {
        SF_DEL( m_broadcastEventList[i] );
    }
    m_broadcastEventList.clear();
}
//------------------------------------------------------------------------------------------------------------------------
bbool VirtualLinkComponent_Template::isEventInBoadcastList( Event * _event ) const
{
    for ( u32 i = 0; i < m_broadcastEventList.size(); ++i )
    {
        if ( _event->GetObjectClassCRC() == m_broadcastEventList[i]->GetObjectClassCRC() )
            return btrue;
    }
    return bfalse;
}

}