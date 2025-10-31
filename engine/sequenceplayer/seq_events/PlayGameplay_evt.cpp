#include "precompiled_engine.h"

#ifndef _ITF_PLAYGAMEPLAYEVENT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayGameplay_evt.h"
#endif //_ITF_PLAYGAMEPLAYEVENT_EVT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PlayGameplay_evt)

    PlayGameplay_evt::PlayGameplay_evt() 
        : m_hasExecuted(bfalse)
    {
    }

    SequenceEvent * PlayGameplay_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
    {
        PlayGameplay_evt *pNew  = newAlloc(mId_Gameplay, PlayGameplay_evt);
        pNew->m_ownerSequence   = _owner;
        pNew->m_template        = _template;
        return pNew;
    }

    void PlayGameplay_evt::init(const Vec3d& _pos)
    {

    }


    void    PlayGameplay_evt::forceCurrentFrame(   i32 _frame  )
    {
        PlayGameplay_evtTemplate* evtTemplate = getTemplate();

        // send event onEnter
        if (  evtTemplate->getOnEnterEvent() && 
            (!evtTemplate->isExecuteOnce() || !m_hasExecuted) )
        {
            if (evtTemplate->getObjectPath().isValid())
            {
                if(!m_ownerSequence) return;
                Actor* actor = NULL;
                actor = getBindedActor();

                if(actor)
                    actor->onEvent(evtTemplate->getOnEnterEvent());
            }
            else
            {
                //if (evtTemplate->isBroadcastEvent())
                //{
                     EVENTMANAGER->broadcastEvent(evtTemplate->getOnEnterEvent());
                //}
                //else
                //{
                //    GAMEMANAGER->onEvent(evtTemplate->getOnEnterEvent());
                //}
            }

            m_hasExecuted = btrue;
        }
    }

    void PlayGameplay_evt::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    void PlayGameplay_evt::onBecomeInactive()
    {
        Super::onBecomeInactive();

        PlayGameplay_evtTemplate* evtTemplate = getTemplate();

        // send event onExit
        if ( evtTemplate->getOnExitEvent())
        {
            if (evtTemplate->getObjectPath().isValid())
            {
                if(!m_ownerSequence) return;
                Actor* actor = NULL;
                actor = getBindedActor();
                if(actor)
                    actor->onEvent(evtTemplate->getOnExitEvent());
            }
            else
            {
                //if (evtTemplate->isBroadcastEvent())
                //{
                    EVENTMANAGER->broadcastEvent(evtTemplate->getOnExitEvent());
                //}
                //else
                //{
                //    GAMEMANAGER->onEvent(evtTemplate->getOnExitEvent());
                //}
            }
        }
        m_hasExecuted = bfalse;
    }


    /////////////////////////////////////////////////////
    /// TEMPLATE
    /////////////////////////////////////////////////////


    IMPLEMENT_OBJECT_RTTI(PlayGameplay_evtTemplate)

    BEGIN_SERIALIZATION_CHILD(PlayGameplay_evtTemplate)

        SERIALIZE_MEMBER("onEnterEventName",m_onEnterEventName);
        SERIALIZE_OBJECT_WITH_FACTORY("onEnterEvent",m_onEnterEvent,ACTORSMANAGER->getEventFactory());

        SERIALIZE_MEMBER("onExitEventName",m_onExitEventName);
        SERIALIZE_OBJECT_WITH_FACTORY("onExitEvent",m_onExitEvent,ACTORSMANAGER->getEventFactory());

        SERIALIZE_MEMBER("executeOnce",m_executeOnce);
        SERIALIZE_MEMBER("broadcast",m_broadcastEvent);
    
    END_SERIALIZATION()


    PlayGameplay_evtTemplate::PlayGameplay_evtTemplate()
    : m_executeOnce(btrue) 
    , m_onEnterEventName("")
    , m_onEnterEvent(NULL)
    , m_onExitEventName("")
    , m_onExitEvent(NULL)
    , m_broadcastEvent(bfalse)
    {
    }

    PlayGameplay_evtTemplate::~PlayGameplay_evtTemplate()
    {
        SF_DEL(m_onEnterEvent);
        SF_DEL(m_onExitEvent); 
    }

#ifdef ITF_SUPPORT_BLOBEVENT
    SequenceEvent_Template*   PlayGameplay_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
    {
        PlayGameplay_evtTemplate* pNew = newAlloc(mId_GameplayTemplate, PlayGameplay_evtTemplate);
        pNew->setOwnerSequence(_owner);
        pNew->fillFromBlob(_blob);

        return pNew;
    }

    void PlayGameplay_evtTemplate::serializeToBlob (Blob* _blob)
    {
        serializeCommonToBlob(_blob);
        serializeFriendlyToBlob(_blob);

       _blob->pushUInt32( m_executeOnce);

        // onEnter event
        _blob->pushString( m_onEnterEventName );
        if (m_onEnterEvent && !m_onEnterEventName.isEmpty())
        {
            ITF_ASSERT(m_onEnterEventName == StringID(m_onEnterEvent->GetObjectClassCRC()).getDebugString());
            SerializerToolXML serializer;
            char* buffer = NULL;
            uSize size = 0;

            serializer.OpenToWrite(&buffer, &size);

            //serializer.SerializeObject("onEnterEvent",*m_onEnterEvent,ESerialize_PropertyEdit_Save);
            m_onEnterEvent->Serialize(&serializer,ESerialize_PropertyEdit_Save);

            serializer.Close();

            _blob->pushString(buffer);

            if(size > 0)
                SF_DEL_ARRAY(buffer);
        }
        else
        {
            _blob->pushString("");
        }


        // onExit event
        _blob->pushString( m_onExitEventName);
        if (m_onExitEvent && !m_onExitEventName.isEmpty())
        {
            ITF_ASSERT(m_onExitEventName ==  StringID(m_onExitEvent->GetObjectClassCRC()).getDebugString());
            SerializerToolXML serializer;
            char* buffer = NULL;
            uSize size = 0;

            serializer.OpenToWrite(&buffer,&size);

            //serializer.SerializeObject("onExitEvent",*m_onExitEvent,ESerialize_PropertyEdit_Save);
            m_onExitEvent->Serialize(&serializer,ESerialize_PropertyEdit_Save);

            serializer.Close();

            _blob->pushString(buffer);

            if(size > 0)
                SF_DEL_ARRAY(buffer);
        }
        else
        {
            _blob->pushString("");
        }

    }

    void PlayGameplay_evtTemplate::fillFromBlob(Blob* _blob)
    {
        fillCommonFromBlob(_blob);
        fillFriendlyFromBlob(_blob);


        m_executeOnce = _blob->extractUint32();

        
        // onEnter event
        {        
            String eventName;
            _blob->extractString(eventName);
            String buffer;
            _blob->extractString(buffer);

            String8 eventName8 = String8(eventName.cStr());

            if (eventName8.isEmpty())
            {
                m_onEnterEventName = eventName8;
                SF_DEL(m_onEnterEvent);
            }
            else if (!(m_onEnterEventName== eventName8))
            {
                m_onEnterEventName = eventName8;

                SF_DEL(m_onEnterEvent);
                //new request
                //ITF_ASSERT_MSG(buffer.isEmpty(),"Maybe old data send with new event");

                //just create a new one
                m_onEnterEvent = ACTORSMANAGER->getEventFactory()->CreateObject<Event>(m_onEnterEventName.cStr());
            }
            else if (m_onEnterEventName == eventName8)
            {

                ITF_ASSERT_MSG(!buffer.isEmpty(),"received event with no params");
                if (!buffer.isEmpty())
                {
                    StringConverter converted(buffer);

                    SerializerToolXML serializer;
                    serializer.OpenToRead(converted.getChar());

                    m_onEnterEvent->Serialize(&serializer, ESerialize_PropertyEdit_Load);
                    //serializer.SerializeObject("onEnterEvent",*m_onEnterEvent,ESerialize_PropertyEdit_Load);

                    serializer.Close();
                }
            }
        }


        // onExit event
        {        
            String eventName;
            _blob->extractString(eventName);
            String buffer;
            _blob->extractString(buffer);

            String8 eventName8 = String8(eventName.cStr());

            if (eventName8.isEmpty())
            {
                m_onExitEventName = eventName8;
                SF_DEL(m_onExitEvent);
            }
            else if (!(m_onExitEventName== eventName8))
            {
                m_onExitEventName = eventName8;

                SF_DEL(m_onExitEvent);
                //new request
                //ITF_ASSERT_MSG(buffer.isEmpty(),"Maybe old data send with new event");

                //just create a new one
                m_onExitEvent = ACTORSMANAGER->getEventFactory()->CreateObject<Event>(m_onExitEventName.cStr());
            }
            else if (m_onExitEventName == eventName8)
            {
                ITF_ASSERT_MSG(!buffer.isEmpty(),"received event with no params");
                if (!buffer.isEmpty())
                {
                    StringConverter converted(buffer);

                    SerializerToolXML serializer;
                    serializer.OpenToRead(converted.getChar());

                    m_onExitEvent->Serialize(&serializer, ESerialize_PropertyEdit_Load);
                    //serializer.SerializeObject("onExitEvent",*m_onExitEvent,ESerialize_PropertyEdit_Load);

                    serializer.Close();
                }
            }
        }
    }

#endif //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


