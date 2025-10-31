#include "precompiled_gameplay.h"

#ifndef _ITF_PHANTOMDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PhantomDetectorComponent.h"
#endif //_ITF_PHANTOMDETECTORCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PhantomDetectorComponent)

    BEGIN_SERIALIZATION_CHILD(PhantomDetectorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(PhantomDetectorComponent)
    END_VALIDATE_COMPONENT()


    PhantomDetectorComponent::PhantomDetectorComponent()
    {
    }

    PhantomDetectorComponent::~PhantomDetectorComponent()
    {
    }

    void PhantomDetectorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_actorsInside.clear();

        if ( !m_currentShape || m_disabled )
        {
            return;
        }

        // get collisions
        PhysContactsContainer contacts;
        PHYSWORLD->collidePhantoms(m_shapePos, m_shapePos, m_actor->getAngle(),
            m_currentShape, m_actor->getDepth(), ECOLLISIONFILTER_CHARACTERS,
            contacts);

        const u32 contactCount = contacts.size();
        for ( u32 i = 0; i < contactCount; i++ )
        {
            ActorRef otherActorRef = contacts[i].m_collidableUserData;

            if (otherActorRef == m_actor->getRef())
            {
                continue;
            }

            Actor* otherActor = otherActorRef.getActor();
            if (!otherActor)
            {
                continue;
            }

            Faction factionToDetect = getTemplate()->getFactionToDetect();
            if ( factionToDetect != FACTION_UNKNOWN &&
                 factionToDetect != AIUtils::getFaction(otherActor) )
            {
                continue;
            }

            if ( PlayerControllerComponent* playerController = otherActor->GetComponent<PlayerControllerComponent>() )
            {
                // direct access for players
                if (!getTemplate()->getAllowDeadActors())
                {
                    if (playerController->isDead())
                    {
                        continue;
                    }
                }

                if ( !playerController->isDetectable(m_actor->getRef()) )
                {
                    continue;
                }
            }
            else if (!getTemplate()->getAllowDeadActors())
            {
                // query for AIs
                EventQueryIsDead query;
                otherActor->onEvent(&query);
                if (query.getIsDead())
                {
                    continue;
                }
            }

            if (m_actorsInside.find(otherActorRef) == -1)
            {
                m_actorsInside.push_back(otherActorRef);
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PhantomDetectorComponent_Template)
    BEGIN_SERIALIZATION_CHILD(PhantomDetectorComponent_Template)
        SERIALIZE_MEMBER("factionToDetect", m_factionToDetect);
        SERIALIZE_MEMBER("allowDeadActors", m_allowDeadActors);
    END_SERIALIZATION()

};