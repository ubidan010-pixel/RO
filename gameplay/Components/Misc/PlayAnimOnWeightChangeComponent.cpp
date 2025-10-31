#include "precompiled_gameplay.h"

#ifndef _ITF_PLAYANIMONWEIGHTCHANGECOMPONENT_H_
#include "gameplay/components/misc/PlayAnimOnWeightChangeComponent.h"
#endif //_ITF_PLAYANIMONWEIGHTCHANGECOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_EVENTDELAYHANDLER_H_
#include "gameplay/AI/Utils/EventDelayHandler.h"
#endif //_ITF_EVENTDELAYHANDLER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PlayAnimOnWeightChangeComponent)

    BEGIN_SERIALIZATION_CHILD(PlayAnimOnWeightChangeComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(PlayAnimOnWeightChangeComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "PlayAnimOnWeightChangeComponent requires an AnimLightComponent/AnimatedComponent");
    END_VALIDATE_COMPONENT()

    PlayAnimOnWeightChangeComponent::PlayAnimOnWeightChangeComponent()
    : Super()
    , m_animComponent(NULL)
    , m_animatedComponent(NULL)
    , m_soundComponent(NULL)
    , m_linkComponent(NULL)
    , m_disabled(bfalse)
    , m_currentWeight(0.0f)
    , m_newWeight(0.f)
    , m_isEntering(bfalse)
    , m_isExiting(bfalse)
    , m_onEnterEvent(NULL)
    , m_onExitEvent(NULL)
    {
        // none
    }

    PlayAnimOnWeightChangeComponent::~PlayAnimOnWeightChangeComponent()
    {
        SF_DEL(m_onEnterEvent);
        SF_DEL(m_onExitEvent);
    }

    void PlayAnimOnWeightChangeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDead,634720568),this);

        if (getTemplate()->getListenToTrigger())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        }

        if ( getTemplate()->getListenToEvent() )
        {
            m_actor->registerEvent(getTemplate()->getListenToEvent()->GetObjectClassCRC(), this);
        }

        if (getTemplate()->getListenToWeight())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
        }

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

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();  // mandatory, validated
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_soundComponent = m_actor->GetComponent<SoundComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();

    }

	void PlayAnimOnWeightChangeComponent::onBecomeInactive()
	{
		Super::onBecomeInactive();
		m_currentWeight = 0.f;
		m_newWeight = 0.f;
		m_isEntering = bfalse;
		m_isExiting = bfalse;
	}

    void PlayAnimOnWeightChangeComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (m_disabled)
        {
            return;
        }

        if ( getTemplate()->getListenToEvent() && _event->IsClassCRC(getTemplate()->getListenToEvent()->GetObjectClassCRC()) )
        {
            onTrigger(btrue);
        }

        if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventDead,634720568)) )
        {
            m_disabled = btrue;
        }
        else if (EventTrigger* evtOnTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (getTemplate()->getListenToTrigger())
            {
                onTrigger(evtOnTrigger->getActivated());
            }
        }
        else if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            if ( stickEvent->isSticked() )
            {
                m_newWeight += stickEvent->getWeight();
            }
            else
            {
                m_newWeight -= stickEvent->getWeight();
            }
        }
        else if ( EventHanging* hanging = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
        {
            if ( hanging->isHanging() )
            {
                m_newWeight += hanging->getWeight();
            }
            else
            {
                m_newWeight -= hanging->getWeight();
            }
        }
    }

    void PlayAnimOnWeightChangeComponent::Update( f32 _dt )
    {
        if (m_disabled)
        {
            return;
        }

        if ( getTemplate()->getStayOnEnter() && m_isEntering )
        {
            return;
        }

        if ( m_isEntering && isAnimFinished() )
        {
            m_isEntering = bfalse;
            playAnim(getTemplate()->getEnterAnimIdle());
        }
        else if ( m_isExiting && isAnimFinished() )
        {
            m_isExiting = bfalse;
            playAnim(getTemplate()->getExitAnimIdle());
        }

        if (getTemplate()->getListenToWeight() )
        {
            if ( m_newWeight )
            {
                m_currentWeight += m_newWeight;

                if ( m_currentWeight >= getTemplate()->getWeightThreshold() )
                {
                    onTrigger(m_newWeight > 0.f);
                }

                m_newWeight = 0.f;
            }

            if ( m_animatedComponent )
            {
                m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(WeightOnPolyLine,419111941), m_currentWeight);
            }

            if ( m_soundComponent )
            {
                m_soundComponent->setInput(ITF_GET_STRINGID_CRC(WeightOnPolyLine,419111941), m_currentWeight);
            }
        }
    }

    void PlayAnimOnWeightChangeComponent::onTrigger(bbool _enter)
    {
        if (_enter)
        {
            if ( playAnim(getTemplate()->getEnterAnim()) )
            {
                m_isEntering = getTemplate()->getEnterAnimIdle().isValid();

                if (m_onEnterEvent)
                {
                    sendEventToChilds(_enter);
                }

                m_isExiting = bfalse;
            }
        }
        else
        {
            if ( playAnim(getTemplate()->getExitAnim()) )
            {
                m_isExiting = getTemplate()->getExitAnimIdle().isValid();

                if (m_onExitEvent)
                {
                    sendEventToChilds(_enter);
                }

                m_isEntering = bfalse;
            }
        }
    }

    void PlayAnimOnWeightChangeComponent::sendEventToChilds(bbool _enter)
    {
        if (_enter)
        {
            m_onEnterEvent->setSender(m_actor->getRef());
            EVENTDELAY_HANDLER->sendEventToChildren(m_linkComponent, m_onEnterEvent, btrue);
        }
        else
        {                             
            m_onExitEvent->setSender(m_actor->getRef());
            EVENTDELAY_HANDLER->sendEventToChildren(m_linkComponent, m_onExitEvent, btrue);
        }
    }

    bbool PlayAnimOnWeightChangeComponent::playAnim( const StringID& _anim )
    {
        if (_anim.isValid())
        {
            m_animComponent->setAnim(_anim);
            return btrue;
        }

        return bfalse;
    }

    bbool PlayAnimOnWeightChangeComponent::isAnimFinished() const
    {
        if ( m_animatedComponent )
        {
            return m_animatedComponent->isMainNodeFinished();
        }
        else if ( m_animComponent )
        {
            return m_animComponent->isSubAnimFinished();
        }
        else
        {
            // There should be one or the other.
            // If this asserts it means there is a problem with the postload
            // Maybe an inherited action is not calling Super::onActorLoaded(_hotReload)?
            ITF_ASSERT_MSG(0, "This action has no AnimLightComponent/AnimatedComponent! Did you forget to add one? Did you forget to call Super::onActorLoaded(_hotReload)?");
            return bfalse;
        }
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(PlayAnimOnWeightChangeComponent_Template)
    BEGIN_SERIALIZATION(PlayAnimOnWeightChangeComponent_Template)

        SERIALIZE_MEMBER("enterAnim", m_enterAnim);
        SERIALIZE_MEMBER("enterAnimIdle", m_enterAnimIdle);
        SERIALIZE_MEMBER("exitAnim", m_exitAnim);
        SERIALIZE_MEMBER("exitAnimIdle", m_exitAnimIdle);
        SERIALIZE_MEMBER("listenToTrigger", m_listenToTrigger);
        SERIALIZE_MEMBER("listenToWeight", m_listenToWeight);
        SERIALIZE_OBJECT_WITH_FACTORY("listenToEvent", m_listenToEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("stayOnEnter", m_stayOnEnter);

        SERIALIZE_MEMBER("weightThreshold", m_weightThreshold);

        SERIALIZE_OBJECT_WITH_FACTORY("onEnterEvent", m_onEnterEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("onExitEvent", m_onExitEvent, ACTORSMANAGER->getEventFactory());

    END_SERIALIZATION()

    PlayAnimOnWeightChangeComponent_Template::PlayAnimOnWeightChangeComponent_Template()
    : m_listenToTrigger(btrue)
    , m_listenToWeight(btrue)
    , m_listenToEvent(NULL)
    , m_stayOnEnter(bfalse)
    , m_weightThreshold(0.f)
    , m_onEnterEvent(NULL)
    , m_onExitEvent(NULL)
    {
        // none
    }

    PlayAnimOnWeightChangeComponent_Template::~PlayAnimOnWeightChangeComponent_Template()
    {
        SF_DEL(m_listenToEvent);
        SF_DEL(m_onEnterEvent);
        SF_DEL(m_onExitEvent);
    }

}