
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkManager.h"
#endif //_ITF_RAY_AINETWORKMANAGER_H_

namespace ITF
{

Ray_AINetworkManager* Ray_AINetworkManager::s_instance = NULL;

Ray_AINetworkManager::Ray_AINetworkManager()
{
}

Ray_AINetworkManager::~Ray_AINetworkManager()
{
}

void Ray_AINetworkManager::create()
{
    if ( s_instance == NULL )
    {
        s_instance = newAlloc(mId_Gameplay, Ray_AINetworkManager());
    }
}

void Ray_AINetworkManager::destroy()
{
    if ( s_instance != NULL )
    {
        delete s_instance;
        s_instance = NULL;
    }
}

void Ray_AINetworkManager::registerActor( ActorRef _ref )
{
    NetworkActorInfo newActor;
    
    newActor.m_actor = _ref;
    newActor.m_waypoint = ITF_INVALID_OBJREF;

    m_registeredActors.push_back(newActor);
}

void Ray_AINetworkManager::unregisterActor( ActorRef _ref )
{
    u32 numRegistered = m_registeredActors.size();

    for ( u32 i = 0; i < numRegistered; i++ )
    {
        if ( m_registeredActors[i].m_actor == _ref )
        {
            m_registeredActors.eraseNoOrder(i);
            break;
        }
    }
}

void Ray_AINetworkManager::updateWaypoint( ActorRef _actor, ActorRef _waypoint )
{
    u32 numRegistered = m_registeredActors.size();

    for ( u32 i = 0; i < numRegistered; i++ )
    {
        NetworkActorInfo& actorInfo = m_registeredActors[i];

        if ( actorInfo.m_actor == _actor )
        {
            actorInfo.m_waypoint = _waypoint;
            break;
        }
    }
}

ActorRef Ray_AINetworkManager::getWaypointFromActor( ActorRef _ref ) const
{
    u32 numRegistered = m_registeredActors.size();

    for ( u32 i = 0; i < numRegistered; i++ )
    {
        const NetworkActorInfo& actorInfo = m_registeredActors[i];

        if ( actorInfo.m_actor == _ref )
        {
            return actorInfo.m_waypoint;
        }
    }

    return ActorRef::InvalidRef;
}

}

