#include "precompiled_gameplay.h"

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(TriggerComponent)

    BEGIN_SERIALIZATION_CHILD(TriggerComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("retriggerOnCheckpoint",m_retriggerOnCheckpoint);
        END_CONDITION_BLOCK()
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("triggerOnceDone",m_triggerOnceDone);
            SERIALIZE_MEMBER("activator",m_activator);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(TriggerComponent)
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getTriggerOnDetector() || m_detectorComponent, "TriggerComponent requires a DetectorComponent to triggerOnDetector");
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getTriggerChildren() || m_linkComponent, "TriggerComponent requires a LinkComponent to triggeChildren");
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getActivateChildren() || m_linkComponent, "TriggerComponent requires a LinkComponent to activateChildren");
    END_VALIDATE_COMPONENT()

    TriggerComponent::TriggerComponent()
        : m_detectorComponent(NULL)
        , m_shapeDetectorComponent(NULL)
        , m_playerDetectorComponent(NULL)
        , m_linkComponent(NULL)
        , m_disabled(bfalse)
        , m_eventActivateHandler()
        , m_onEnterEvent(NULL)
        , m_onExitEvent(NULL)
        , m_triggerOnceDone(bfalse)
        , m_retriggerOnCheckpoint(bfalse)
    {
    }

    TriggerComponent::~TriggerComponent()
    {
        clear();
        
        SF_DEL(m_onEnterEvent);
        SF_DEL(m_onExitEvent);

        // TODO: reactivate children on editor destroy
    }

    void TriggerComponent::clear()
    {
        m_triggerOnceDone = bfalse;
        m_actorsInsideLastFrame.clear();
    }

    void TriggerComponent::convertInstanceDataToLinkComponent()
    {
        if ( !m_linkedChildrenIds.size() && !m_children.size() )
        {
            return;
        }

        LinkComponent* linkComponent = m_actor->GetComponent<LinkComponent>();
        if (!linkComponent)
        {
            ITF_WARNING(m_actor, 0, "Cannot convert: this trigger has links to children but no LinkComponent!");
            return;
        }


        if ( m_linkedChildrenIds.size() )
        {
            convertFromOldFormat();
        }

        for (u32 i = 0; i < m_children.size(); i++)
        {
            linkComponent->addChild(m_children[i]);
        }

        m_children.clear();
        m_actor->replaceReinitDataByCurrent();
    }

    void TriggerComponent::convertFromOldFormat()
    {
        u32 numChildren = m_linkedChildrenIds.size();
        Scene* scene = m_actor->getScene();
        Actor* child;

        for ( u32 i = 0; i < numChildren; i++ )
        {
            child = WorldManager::getActor(scene, m_linkedChildrenIds[i]);

            if ( child )
            {
                ObjectPath relPath;

                SceneObjectPathUtils::getRelativePathFromObject(m_actor,child,relPath);

                if ( relPath.isValid() )
                {
                    //ITF_ASSERT(!hasChild(relPath));
                    m_children.push_back(relPath);
                }
            }
        }

        m_linkedChildrenIds.clear();

        //m_actor->replaceReinitDataByCurrent();
    }

    void TriggerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // clone template events
        if ( getTemplate()->getOnEnterEvent() )
        {
            m_onEnterEvent = static_cast<Event*>(getTemplate()->getOnEnterEvent()->CreateNewObject());
            BinaryClone(const_cast<Event*>(getTemplate()->getOnEnterEvent()),m_onEnterEvent);
        }
        if ( getTemplate()->getOnExitEvent() )
        {
            m_onExitEvent = static_cast<Event*>(getTemplate()->getOnExitEvent()->CreateNewObject());
            BinaryClone(const_cast<Event*>(getTemplate()->getOnExitEvent()),m_onExitEvent);
        }

        convertInstanceDataToLinkComponent();

        m_detectorComponent = m_actor->GetComponent<DetectorComponent>();   // only needed if triggerOnDetector is true
        if (m_detectorComponent)
        {
            m_shapeDetectorComponent = m_detectorComponent->DynamicCast<ShapeDetectorComponent>(ITF_GET_STRINGID_CRC(ShapeDetectorComponent,1842589967)); // only needed to set the refPos on EventTriggers
            m_playerDetectorComponent = m_detectorComponent->DynamicCast<PlayerDetectorComponent>(ITF_GET_STRINGID_CRC(PlayerDetectorComponent,683675795)); // needed to filter playerOnly hits
        }
        m_linkComponent = m_actor->GetComponent<LinkComponent>();           // only needed if triggerChildren is true

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874),this);

        if (getTemplate()->getTriggerable())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        }
        if (getTemplate()->getTriggerOnHit())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
        }
        if (getTemplate()->getTriggerOnWind())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(WindStim,2026477007),this);
        }
        if (getTemplate()->getTriggerOnCrush())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623),this);
        }
        if (getTemplate()->getActivateChildren())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventOnLink,1558255516),this);
        }
    }

    void TriggerComponent::onSceneActive()
    {
        Super::onSceneActive();

        if (getTemplate()->getActivateChildren())
        {
            /* We need to deactivate children only once, but cannot do it onActorLoaded
               because the linked children may not be ready (EventActivate not registered).
               On normal scene load, we do it onSceneActive (everybody's loaded).
               On F5, we do it onCheckpointLoaded.
            */
            m_eventActivateHandler.deactivateChildren(m_linkComponent);
        }
    }

    void TriggerComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        bbool triggered = m_triggerOnceDone;

        if (getTemplate()->getResetOnCheckpoint())
        {
            m_triggerOnceDone = bfalse;
            m_actorsInsideLastFrame.clear();
        }
        if (getTemplate()->getActivateChildren())
        {
            m_eventActivateHandler.deactivateChildren(m_linkComponent);
        }
        if (m_retriggerOnCheckpoint)
        {
            if (triggered)
            {
                sendEvent(m_activator, btrue, btrue);
            }
        }
    }

    void TriggerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_disabled)
        {
            return;
        }

        if (!getTemplate()->getTriggerOnDetector())
        {
            return;
        }

        ActorRefList& actorsInsideThisFrame = m_detectorComponent->getActorsInside();
        checkSendEvent(actorsInsideThisFrame);
        m_actorsInsideLastFrame = actorsInsideThisFrame;

    }

    void TriggerComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        // this an "editor" event, that's why it should remain processed
        // *before* any early return
        if (getTemplate()->getActivateChildren())
        {
            m_eventActivateHandler.onEvent(_event);
        }
        
        if ( EventDisableCollision* collision = _event->DynamicCast<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
        {
            m_disabled = collision->getDisable();
            if (m_disabled)
            {
                m_actorsInsideLastFrame.clear();
            }
            return;
        }

        if (m_disabled)
        {
            return;
        }

        if ( !canTrigger() )
        {
            return;
        }

        ActorRef sender = _event->getSender();
        bbool isHitStim = ( getTemplate()->getTriggerOnHit() && _event->IsClassCRC(ITF_GET_STRINGID_CRC(HitStim,2121727796)) ) ? btrue : bfalse;

        if ( isHitStim )
        {
            HitStim * stim = static_cast<HitStim *>( _event );
            if ( stim->getOriginalSender().isValid() )
            {
                sender = stim->getOriginalSender();            
            }
        }

        if ( isHitStim )
        {
            sendEvent(sender, btrue);

            Actor * senderActor = AIUtils::getActor( _event->getSender() );
            ITF_ASSERT( senderActor != NULL );
            if ( senderActor != NULL )
            {
                EventHitSuccessful hitSuccess;
                hitSuccess.setSender( m_actor->getRef() );
                senderActor->onEvent( &hitSuccess );
            }            
        }
        else if( getTemplate()->getTriggerable() && _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            Actor * senderActor = AIUtils::getActor( _event->getSender() );
            if ( senderActor != m_actor )
            {
                sendEvent(sender, btrue);
            }
        }
        else if ( getTemplate()->getTriggerOnWind() && _event->IsClassCRC(ITF_GET_STRINGID_CRC(WindStim,2026477007)) )
        {
            sendEvent(sender, btrue);
        }
        else if ( getTemplate()->getTriggerOnCrush() )
        {
            if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
            {
                // quick & dirty fix for http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-4095
                // update, using 45� now http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-4656
                f32 angle = m_actor->getAngle();
                NormalizeAngle2PI(angle);                
                static const f32 epsilon = 0.01f;
                if (angle <= (MTH_PIBY4 - epsilon) || angle >= ((7.0f * MTH_PIBY4) + epsilon))
                {
                    query->setInteraction(CharacterInteractionType_Crush);
                }
            }
            else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
            {
                sendEvent(sender, btrue);
            }
        }

    }

    void TriggerComponent::sendEvent( ActorRef _activator, bbool _enter, const bbool _fromRetriggerOnCheckpoint /*= bfalse*/ )
    {
        Event* evt = _enter ? m_onEnterEvent : m_onExitEvent;
        if (!evt && !getTemplate()->getActivateChildren())
        {
            return;
        }
        sendEvent(_activator, evt, _fromRetriggerOnCheckpoint);
    }

    void TriggerComponent::sendEvent( ActorRef _activator, Event* evt, const bbool _fromRetriggerOnCheckpoint /*= bfalse*/ )
    {
        m_activator = _activator;

        if (getTemplate()->getActivateChildren())
        {
            m_eventActivateHandler.activateChildrenIfNeeded(m_linkComponent);
        }

        m_triggerOnceDone = btrue;

        if (!evt)
            return;

        Actor* activatorActor = _activator.getActor();
        ITF_ASSERT(activatorActor);

        evt->setSender(m_actor->getRef());
        evt->setActivator(_activator);
        evt->setFromRetriggerOnCheckpoint(_fromRetriggerOnCheckpoint);

        if ( EventTrigger* evtTrigger = evt->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if ( activatorActor && m_shapeDetectorComponent )
            {
                evtTrigger->setRefPos(m_shapeDetectorComponent->getRefPos(activatorActor->get2DPos()));
            }
        }


        if (getTemplate()->getTriggerSelf())
        {
            m_actor->onEvent(evt);
        }

        if (getTemplate()->getTriggerActivator() && activatorActor)
        {
            activatorActor->onEvent(evt);
        }

        if (getTemplate()->getTriggerChildren() && m_linkComponent)
        {
            EVENTDELAY_HANDLER->sendEventToChildren(m_linkComponent, evt, btrue);
        }

        if (getTemplate()->getTriggerParent())
        {
            // TODO: cleanup once ActorSpawnComponent uses ActorBind instead of ObjectBinding
            // (ObjectBinding would only be used for polyline attachment, and we wouldn't notify those)
            Actor* parent = m_actor->getParent().getActor();
            if (!parent)
            {
                BaseObject* obj = m_actor->getBinding()->getParent().getObject();
                if (obj)
                {
                    parent = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
                }
            }

            if ( parent )
            {
                parent->onEvent(evt);
            }
        }

        if (getTemplate()->getTriggerGameManager())
        {
            GAMEMANAGER->onEvent(evt);
        }

        if (getTemplate()->getTriggerBroadcast())
        {
            EVENTMANAGER->broadcastEvent(evt);
        }
    }

    void TriggerComponent::checkSendEvent( ActorRefList& actorsInsideThisFrame )
    {
        if (canTrigger())
        {
            if (getTemplate()->getTriggerEachActor())
            {
                // check who just entered
                bbool sendEveryFrame = getTemplate()->getSendEventEveryFrame();

                for (u32 i = 0; i < actorsInsideThisFrame.size(); i++)
                {
                    ActorRef& actor = actorsInsideThisFrame[i];
                    if ( sendEveryFrame || m_actorsInsideLastFrame.find(actor) == -1 )
                    {
                        sendEvent(actor, btrue);
                    }
                }
            }
            else
            {
                if ( m_actorsInsideLastFrame.size() == 0 && actorsInsideThisFrame.size() > 0)
                {
                    // Send event only to the first one (arbitrary)
                    sendEvent(actorsInsideThisFrame[0], btrue);

                    //for (u32 i = 0; i < actorsInsideThisFrame.size(); i++)
                    //{
                    //    ActorRef& actor = actorsInsideThisFrame[i];
                    //    sendEvent(actor, btrue);
                    //}
                }
            }
        }

        // check who just exited
        bbool reset = bfalse;
        if (getTemplate()->getTriggerEachActor())
        {
            for (u32 i = 0; i < m_actorsInsideLastFrame.size(); i++)
            {
                ActorRef& actor = m_actorsInsideLastFrame[i];
                if ( actorsInsideThisFrame.find(actor) == -1 )
                {
                    sendEvent(actor, bfalse);
                    if (getTemplate()->getResetOnExit())
                    {
                        reset = btrue;
                        m_triggerOnceDone = bfalse;
                    }
                }
            }
        }
        else
        {
            if ( m_actorsInsideLastFrame.size() > 0 && actorsInsideThisFrame.size() == 0)
            {
                for (u32 i = 0; i < m_actorsInsideLastFrame.size(); i++)
                {
                    ActorRef& actor = m_actorsInsideLastFrame[i];
                    sendEvent(actor, bfalse);
                    if (getTemplate()->getResetOnExit())
                    {
                        reset = btrue;
                        m_triggerOnceDone = bfalse;
                    }
                }
            }
        }
        if (reset)
        {
            actorsInsideThisFrame.clear();
        }
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(TriggerComponent_Template)
    BEGIN_SERIALIZATION(TriggerComponent_Template)

        SERIALIZE_MEMBER("activateChildren",m_activateChildren);
        SERIALIZE_MEMBER("triggerOnce", m_triggerOnce);
        
        SERIALIZE_MEMBER("resetOnCheckpoint", m_resetOnCheckpoint);
        SERIALIZE_MEMBER("resetOnExit", m_resetOnExit);

        SERIALIZE_MEMBER("triggerOnDetector", m_triggerOnDetector);
        SERIALIZE_MEMBER("triggerOnHit", m_triggerOnHit);
        SERIALIZE_MEMBER("triggerOnWind", m_triggerOnWind);
        SERIALIZE_MEMBER("triggerOnCrush", m_triggerOnCrush);

        SERIALIZE_MEMBER("triggerSelf",m_triggerSelf);
        SERIALIZE_MEMBER("triggerChildren",m_triggerChildren);
        SERIALIZE_MEMBER("triggerParent",m_triggerParent);
        SERIALIZE_MEMBER("triggerActivator", m_triggerActivator);
        SERIALIZE_MEMBER("triggerGameManager", m_triggerGameManager);
        SERIALIZE_MEMBER("triggerBroadcast", m_triggerBroadcast);
        SERIALIZE_MEMBER("triggerEachActor", m_triggerEachActor);

        SERIALIZE_MEMBER("triggerable", m_triggerable);

        SERIALIZE_OBJECT_WITH_FACTORY("onEnterEvent", m_onEnterEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("onExitEvent", m_onExitEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("sendEventEveryFrame", m_sendEventEveryFrame);

    END_SERIALIZATION()

    TriggerComponent_Template::TriggerComponent_Template()
    : m_activateChildren(bfalse)
    , m_triggerOnce(btrue)
    , m_resetOnCheckpoint(bfalse)
    , m_resetOnExit(bfalse)
    , m_triggerOnDetector(btrue)
    , m_triggerOnHit(bfalse)
    , m_triggerOnWind(bfalse)
    , m_triggerOnCrush(bfalse)
    , m_triggerSelf(btrue)
    , m_triggerChildren(bfalse)
    , m_triggerParent(bfalse)
    , m_triggerActivator(bfalse)
    , m_triggerGameManager(bfalse)
    , m_triggerBroadcast(bfalse)
    , m_triggerEachActor(btrue)
    , m_onEnterEvent(NULL)
    , m_onExitEvent(NULL)
    , m_sendEventEveryFrame(bfalse)
    , m_triggerable(bfalse)
    {
    }

    TriggerComponent_Template::~TriggerComponent_Template()
    {
        SF_DEL(m_onEnterEvent);
        SF_DEL(m_onExitEvent);
    }

} // namespace ITF
