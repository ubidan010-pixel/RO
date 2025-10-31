#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeManagerComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCOSTUMECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

//[RETAIL] TODO REMOVE FOR RETAIL GAME
#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_


namespace ITF
{
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PlayerCostumeManagerComponent)

BEGIN_SERIALIZATION_CHILD(Ray_PlayerCostumeManagerComponent)
END_SERIALIZATION()

Ray_PlayerCostumeManagerComponent::Ray_PlayerCostumeManagerComponent()
{
}

Ray_PlayerCostumeManagerComponent::~Ray_PlayerCostumeManagerComponent()
{
}

void Ray_PlayerCostumeManagerComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if (m_actor)
    { 
        RAY_GAMEMANAGER->resetCostumeManager(m_actor->getRef());
    }

}

void Ray_PlayerCostumeManagerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    RAY_GAMEMANAGER->setCostumeManager(m_actor->getRef());
}

bbool Ray_PlayerCostumeManagerComponent::isCostumeAvailable( const StringID& _id ) const
{
    const SafeArray <CostumeInfo>& costumes = getTemplate()->getCostumes();
    u32 numCostumes = costumes.size();

    for ( u32 i = 0; i < numCostumes; i++ )
    {
        const CostumeInfo& costumeInfo = costumes[i];

        if ( costumeInfo.m_id == _id )
        {
            return RAY_GAMEMANAGER->getElectoonsCount() >= costumeInfo.m_prize;
        }
    }

    return bfalse;
}

bbool Ray_PlayerCostumeManagerComponent::isCostumeShown( const StringID& _id ) const
{
    u32 index;
    return getIdNextCostumeAvailable(index) == _id;
}

ObjectRef Ray_PlayerCostumeManagerComponent::getNextCostumeAvailable( u32& _prize ) const
{
    u32 index;
    StringID idNextCostume = getIdNextCostumeAvailable(index);

    if ( idNextCostume.isValid() )
    {
        u32 numRegistered = m_costumes.size();

        for ( u32 i = 0; i < numRegistered; i++ )
        {
            Actor* actor = m_costumes[i].getActor();

            if ( !actor )
            {
                continue;
            }

            Ray_PlayerCostumeComponent* costume = actor->GetComponent<Ray_PlayerCostumeComponent>();

            if ( !costume )
            {
                continue;
            }

            if ( costume->getCostumeID() == idNextCostume )
            {
                _prize = getTemplate()->getCostumes()[index].m_prize;
                return m_costumes[i];
            }
        }
    }

    return ITF_INVALID_OBJREF;
}

StringID Ray_PlayerCostumeManagerComponent::getIdNextCostumeAvailable( u32& _index ) const
{
    _index = U32_INVALID;
    StringID nextCostume;
    u32 minDif = 0;
    const SafeArray <CostumeInfo>& costumes = getTemplate()->getCostumes();
    u32 numCostumes = costumes.size();
    u32 numElectoons = RAY_GAMEMANAGER->getElectoonsCount();

    for ( u32 i = 0; i < numCostumes; i++ )
    {
        const CostumeInfo& costumeInfo = costumes[i];

        if ( numElectoons >= costumeInfo.m_prize )
        {
            // already have it
            continue;
        }

        u32 dif = costumeInfo.m_prize - numElectoons;

        if ( minDif == 0 || dif < minDif )
        {
            minDif = dif;
            nextCostume = costumeInfo.m_id;
            _index = i;
        }
    }

    return nextCostume;
}

void Ray_PlayerCostumeManagerComponent::registerCostume( ObjectRef _ref )
{
    if ( m_costumes.find(_ref) == -1 )
    {
        m_costumes.push_back(_ref);
    }
}

void Ray_PlayerCostumeManagerComponent::unregisterCostume( ObjectRef _ref )
{
    u32 index = m_costumes.find(_ref);

    if ( index != -1 )
    {
        m_costumes.eraseNoOrder(index);
    }
}

//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PlayerCostumeManagerComponent_Template)
BEGIN_SERIALIZATION(Ray_PlayerCostumeManagerComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("costumes",m_costumes);

END_SERIALIZATION()

BEGIN_SERIALIZATION(CostumeInfo)

    SERIALIZE_MEMBER("id",m_id);
    SERIALIZE_MEMBER("prize",m_prize);

END_SERIALIZATION()

Ray_PlayerCostumeManagerComponent_Template::Ray_PlayerCostumeManagerComponent_Template()
{
}

Ray_PlayerCostumeManagerComponent_Template::~Ray_PlayerCostumeManagerComponent_Template()
{

}

}
