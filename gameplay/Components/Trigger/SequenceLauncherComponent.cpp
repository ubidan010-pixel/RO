#include "precompiled_gameplay.h"

#ifndef _ITF_SEQUENCETRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/SequenceLauncherComponent.h"
#endif //_ITF_SEQUENCETRIGGERCOMPONENT_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(SequenceLauncherComponent_Template)

    BEGIN_SERIALIZATION(SequenceLauncherComponent_Template)
        SERIALIZE_OBJECT("eventPlaySequence", m_eventPlaySequence);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(SequenceLauncherComponent)

    BEGIN_SERIALIZATION_CHILD(SequenceLauncherComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(SequenceLauncherComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "SequenceLauncherComponent requires a LinkComponent");
    END_VALIDATE_COMPONENT()

    SequenceLauncherComponent::SequenceLauncherComponent()
        : m_linkComponent(NULL)
        , m_locked(bfalse)
    {
    }

    SequenceLauncherComponent::~SequenceLauncherComponent()
    {
        if (EVENTMANAGER)
        {
            // release lock on F5
            if (m_locked)
            {
                setLock(bfalse);
            }
        }
    }


    void SequenceLauncherComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    }

    void SequenceLauncherComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if (trigger->getActivated())
            {
                m_locked = btrue;
                setLock(btrue);
            }
        }
    }

    void SequenceLauncherComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_locked)
        {
            if (arePlayersReady())
            {
                m_locked = bfalse;
                playSequence();
                setLock(bfalse);
            }
        }
    }


    void SequenceLauncherComponent::setLock( bbool _lock )
    {
        EventLockPlayers lockPlayers;
        lockPlayers.setLock(_lock);
        EVENTMANAGER->broadcastEvent(&lockPlayers);


        EventSequenceActorPrepare eventPrepare;
        eventPrepare.setLockInput(_lock);
        
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            if ( Actor* player = GAMEMANAGER->getActivePlayer(i) )
            {
                player->onEvent(&eventPrepare);
            }
        }
    }

    bbool SequenceLauncherComponent::arePlayersReady()
    {
        EventSequenceActorReady eventReady;

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            if ( Actor* player = GAMEMANAGER->getActivePlayer(i) )
            {
                player->onEvent(&eventReady);
                if (!eventReady.isReady())
                {
                    return bfalse;
                }
            }
        }

        return btrue;
    }

    void SequenceLauncherComponent::playSequence()
    {
        EventSequenceControl eventPlay = getTemplate()->getEventPlaySequence();
        eventPlay.setSender(m_actor->getRef());
        m_linkComponent->sendEventToChildren(&eventPlay, btrue);
    }

};