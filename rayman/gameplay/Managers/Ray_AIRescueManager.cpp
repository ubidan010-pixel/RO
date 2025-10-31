
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIRESCUEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AIRescueManager.h"
#endif //_ITF_RAY_AIRESCUEMANAGER_H_

namespace ITF
{

Ray_AIRescueManager* Ray_AIRescueManager::s_instance = NULL;

Ray_AIRescueManager::Ray_AIRescueManager()
{
}

Ray_AIRescueManager::~Ray_AIRescueManager()
{
}

void Ray_AIRescueManager::create()
{
    if ( s_instance == NULL )
    {
        s_instance = newAlloc(mId_Gameplay, Ray_AIRescueManager());
    }
}

void Ray_AIRescueManager::destroy()
{
    if ( s_instance != NULL )
    {
        delete s_instance;
        s_instance = NULL;
    }
}

void Ray_AIRescueManager::registerActor( ActorRef _ref )
{
    if ( m_registeredActors.find(_ref) == -1 )
    {
        m_registeredActors.push_back(_ref);
    }
}

void Ray_AIRescueManager::unregisterActor( ActorRef _ref )
{
    u32 numRegistered = m_registeredActors.size();

    for ( u32 i = 0; i < numRegistered; i++ )
    {
        if ( m_registeredActors[i] == _ref )
        {
            m_registeredActors.eraseNoOrder(i);
            break;
        }
    }
}

}

