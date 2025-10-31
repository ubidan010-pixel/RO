

#include "precompiled_gameplay.h"

#ifndef _ITF_PLAYANIMONEVENTRECEIVECOMPONENT_H_
#include "gameplay/components/misc/PlayAnimOnEventReceiveComponent.h"
#endif //_ITF_PLAYANIMONEVENTRECEIVECOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PlayAnimOnEventReceiveComponent)

BEGIN_SERIALIZATION_CHILD(PlayAnimOnEventReceiveComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("disabledAfterEvent",m_disabledAfterEvent);
        SERIALIZE_MEMBER("playingEventCheckpointSave",m_playingEventCheckpointSave);
    END_CONDITION_BLOCK()
END_SERIALIZATION()

PlayAnimOnEventReceiveComponent::PlayAnimOnEventReceiveComponent()
: m_animComponent(NULL)
, m_animatedComponent(NULL)
, m_fxController(NULL)
, m_playingEvent(bfalse)
, m_playingEventCheckpointSave(bfalse)
, m_eventReceived(bfalse)
, m_disabledAfterEvent(bfalse)
{
}

PlayAnimOnEventReceiveComponent::~PlayAnimOnEventReceiveComponent()
{
}


void PlayAnimOnEventReceiveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    const u32 numEvents = getTemplate()->getListenEvents().size();

    for ( u32 i = 0; i < numEvents; i++ )
    {
        m_actor->registerEvent( getTemplate()->getListenEvents()[i]->GetObjectClassCRC(),this);
    }

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();
    m_fxController = m_actor->GetComponent<FXControllerComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setAnim(getTemplate()->getIdleAnim());
    }

    // NB[LaurentCou]: a bit hacky but that's to play safe: this component
    // didn't check for anim endings properly, use this flag to fix this
    if (getTemplate()->getUseAnimatedComponentForAnimsFinish())
    {
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    }
}

void PlayAnimOnEventReceiveComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    if (m_disabledAfterEvent)
    {
        ITF_ASSERT(getTemplate()->getDisableAfterEvent());
        m_actor->disable();
    }

    if (m_playingEventCheckpointSave && getTemplate()->getStayOnEventCheckpointSave())
    {
        ITF_ASSERT(getTemplate()->getStayOnEvent());
        m_animComponent->setAnim(getTemplate()->getEventAnim());
        m_playingEvent = btrue;

        // set event anim at end
        for (u32 i=0; i<m_animComponent->getNumPlayingSubAnims(); ++i)
        {
            const SubAnim* subAnim = m_animComponent->getCurSubAnim(i);
            if (subAnim)
            {
                m_animComponent->setCurTime(subAnim->getDuration(), i);
            }
        }
    }
}

void PlayAnimOnEventReceiveComponent::Update( f32 _dt )
{
    if ( !m_animComponent )
    {
        return;
    }

    if ( m_eventReceived )
    {
        m_animComponent->setAnim(getTemplate()->getEventAnim());

        if (getTemplate()->getResetOnRetrigger())
        {
            m_animComponent->resetCurTime();
        }
        else if ( !m_animComponent->isAnimLooped() && AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
        {
            m_animComponent->resetCurTime();
        }

        m_playingEvent = btrue;
        m_playingEventCheckpointSave = btrue;
        m_eventReceived = bfalse;
    }
    else if ( m_playingEvent )
    {
        if ( !m_animComponent->isAnimLooped() &&
              AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
        {
            if (getTemplate()->getDisableAfterEvent())
            {
                if (!m_fxController || m_fxController->IsAllFXStopped())
                {
                    m_actor->disable();
                    m_disabledAfterEvent = btrue;
                }
            }
            else
            {
                if ( getTemplate()->getEventIdleAnim().isValid() )
                {
                    m_animComponent->setAnim(getTemplate()->getEventIdleAnim());
                }
                else if ( !getTemplate()->getStayOnEvent() )
                {
                    m_animComponent->setAnim(getTemplate()->getIdleAnim());
                }

                if ( !getTemplate()->getStayOnEvent() )
                {
                    m_playingEvent = bfalse;
                    m_playingEventCheckpointSave = bfalse;
                }
            }
        }
    }
}

void PlayAnimOnEventReceiveComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( getTemplate()->getStayOnEvent() &&
         m_playingEvent )
    {
        return;
    }

    if (getTemplate()->getDisableAfterEvent() && m_disabledAfterEvent)
    {
        return;
    }

    if ( m_animComponent )
    {
        const u32 numEvents = getTemplate()->getListenEvents().size();

        for ( u32 i = 0; i < numEvents; i++ )
        {
            Event* listenEvent = getTemplate()->getListenEvents()[i];

            if ( _event->IsClassCRC(listenEvent->GetObjectClassCRC()) )
            {
                m_eventReceived = btrue;
                break;
            }
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PlayAnimOnEventReceiveComponent_Template)
BEGIN_SERIALIZATION(PlayAnimOnEventReceiveComponent_Template)

    SERIALIZE_CONTAINER_WITH_FACTORY("listenEvents",m_listenEvents,ACTORSMANAGER->getEventFactory());
    SERIALIZE_MEMBER("idleAnim",m_idleAnim);
    SERIALIZE_MEMBER("eventAnim",m_eventAnim);
    SERIALIZE_MEMBER("eventIdleAnim",m_eventIdleAnim);
    SERIALIZE_MEMBER("resetOnRetrigger",m_resetOnRetrigger);
    SERIALIZE_MEMBER("stayOnEvent",m_stayInEvent);
    SERIALIZE_MEMBER("stayOnEventCheckpointSave",m_stayInEventCheckpointSave);
    SERIALIZE_MEMBER("disableAfterEvent",m_disableAfterEvent);
    SERIALIZE_MEMBER("useAnimatedComponentForAnimsFinish",m_useAnimatedComponentForAnimsFinish);

END_SERIALIZATION()

PlayAnimOnEventReceiveComponent_Template::~PlayAnimOnEventReceiveComponent_Template()
{
    u32 numEvents = m_listenEvents.size();

    for ( u32 i = 0; i < numEvents; i++ )
    {
        SF_DEL(m_listenEvents[i]);
    }

    m_listenEvents.clear();
}

}
