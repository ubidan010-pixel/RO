#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PICKUPCOMPONENT_H_
#include "war/gameplay/Components/Misc/War_PickupComponent.h"
#endif //_ITF_WAR_PICKUPCOMPONENT_H_

#ifndef _ITF_ACTORSPAWNCOMPONENT_H_
#include "gameplay/Components/Common/ActorSpawnComponent.h"
#endif //_ITF_ACTORSPAWNCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_PickupComponent)

    BEGIN_SERIALIZATION_CHILD(War_PickupComponent)
    END_SERIALIZATION()


    War_PickupComponent::War_PickupComponent()
        : Super()
        , m_pickedUp(bfalse)
    {
    }

    War_PickupComponent::~War_PickupComponent()
    {
    }

    void War_PickupComponent::onActorLoaded()
    {
        Super::onActorLoaded();

        m_actor->registerEvent(EventStickOnPolyline::GetClassNameStatic(), this);
    }

    void War_PickupComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventStickOnPolyline* evtOnStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            if (!m_pickedUp)
            {
                m_pickedUp = btrue;
                onPickup();
            }
        }
    }

    void War_PickupComponent::onPickup()
    {
        War_EventPickup evtPickup;
        GAMEMANAGER->getPlayer(0)->getActor()->onEvent(&evtPickup);

        ActorSpawnComponent* spawner = m_actor->GetComponent<ActorSpawnComponent>();
        spawner->destroySpawns();
    }

};