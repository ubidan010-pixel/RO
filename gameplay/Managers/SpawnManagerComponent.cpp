#include "precompiled_gameplay.h"

#ifndef _ITF_SPAWNMANAGERCOMPONENT_H_
#include "gameplay/Managers/SpawnManagerComponent.h"
#endif //_ITF_SPAWNMANAGERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(SpawnManagerComponent)

    BEGIN_SERIALIZATION_CHILD(SpawnManagerComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(SpawnManagerComponent)
    END_VALIDATE_COMPONENT()

    SpawnManagerComponent::SpawnManagerComponent()
        : Super()
    {
    }

    SpawnManagerComponent::~SpawnManagerComponent()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void SpawnManagerComponent::clear()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void SpawnManagerComponent::unspawnAll()
    {
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void SpawnManagerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        ITF_WARNING(m_actor, bfalse, "deprecated SpawnManagerComponent - remove it");
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(SpawnManagerComponent_Template)
    BEGIN_SERIALIZATION(SpawnManagerComponent_Template)
    END_SERIALIZATION()

    SpawnManagerComponent_Template::SpawnManagerComponent_Template()
    {
    }
};