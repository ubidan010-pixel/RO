#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYCAGESTONECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CageStoneComponent.h"
#endif //_ITF_RAYCAGESTONECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_CageStoneComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_CageStoneComponent)
    END_SERIALIZATION()

    Ray_CageStoneComponent::~Ray_CageStoneComponent()
    {
        for (u32 i = 0 ; i < m_spawnedActors.size() ; ++i )
        {
            ObjectRef objRef = m_spawnedActors[i];
            if ( objRef.getObject() )
            {
                Actor * anActor = static_cast<Actor *>(objRef.getObject());
                if ( anActor )
                    anActor->requestDestruction();
            }
        }

        m_spawnedActors.clear();
    }

    void Ray_CageStoneComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        if (!getTemplate()->getActorToSpawn().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &(m_spawneeGen), getTemplate()->getActorToSpawn());
        }

        m_nbActorToSpawn = getTemplate()->getCageID() + 1;
        m_spawnedActors.reserve(m_nbActorToSpawn);

        m_interval = getTemplate()->getdeltaPosX();
        m_posXStart = m_actor->getPos().m_x - (m_interval * (m_nbActorToSpawn-1) * 0.5f);
        m_posXEnd   = m_actor->getPos().m_x + (m_interval * (m_nbActorToSpawn-1) * 0.5f);
    }

    //----------------------------------------------------------------------------------------------------------------------------

    void Ray_CageStoneComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( m_spawnedActors.size() > 0 )
        {
            for (u32 i = 0 ; i < m_spawnedActors.size() ; ++i )
            {
                ObjectRef objRef = m_spawnedActors[i];
                if ( objRef.getObject() )
                {
                    Actor * anActor = static_cast<Actor *>(objRef.getObject());
                    if ( anActor )
                        anActor->enable();
                }
            }
        }
        else
        {
            // Cage is already broken ?
            if ( RAY_GAMEMANAGER->getIsCageBroken(getTemplate()->getCageID()) )
            {
                for (u32 i = 0 ; i < m_nbActorToSpawn ; ++i )
                {
                    Actor * anActor = NULL;
                    Vec3d newPos    = m_actor->getPos();
                    newPos.m_x      = m_posXStart + (i * m_interval);
                    anActor = m_spawneeGen.getSpawnee( m_actor->getScene(),newPos );
                    ITF_ASSERT(anActor != NULL);
                    if ( anActor != NULL )
                    {
                        u32 flip = Seeder::GetUnseededU32(0,2);
                        anActor->setFlipped(bbool(flip));
                        m_spawnedActors.push_back(anActor->getRef());
                    }
                }
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------

    void Ray_CageStoneComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        for (u32 i = 0 ; i < m_spawnedActors.size() ; ++i )
        {
            ObjectRef objRef = m_spawnedActors[i];
            if ( objRef.getObject() )
            {
                Actor * anActor = static_cast<Actor *>(objRef.getObject());
                if ( anActor )
                    anActor->disable();
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_CageStoneComponent_Template)
    BEGIN_SERIALIZATION(Ray_CageStoneComponent_Template)
    SERIALIZE_MEMBER("cageID",m_cageID);
    SERIALIZE_MEMBER("actorToSpawn",m_actorToSpawn);
    SERIALIZE_MEMBER("deltaPosX",m_deltaPosX);
     END_SERIALIZATION()

    Ray_CageStoneComponent_Template::Ray_CageStoneComponent_Template()
    : m_cageID(U32_INVALID)
    , m_deltaPosX(1.0f)
    {
    }

    Ray_CageStoneComponent_Template::~Ray_CageStoneComponent_Template()
    {
    }

}
