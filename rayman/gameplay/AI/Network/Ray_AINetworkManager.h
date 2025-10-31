
#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#define _ITF_RAY_AINETWORKMANAGER_H_

namespace ITF
{

#define RAY_AINETWORKMANAGER Ray_AINetworkManager::get()

class Ray_AINetworkManager
{
public:

    // singleton
    static void                     create();
    static void                     destroy();
    static Ray_AINetworkManager*    get() { return s_instance; }

    // 
    struct NetworkActorInfo
    {
        ActorRef        m_actor;
        ActorRef        m_waypoint;
    };

    typedef SafeArray <NetworkActorInfo> RegisteredActorsList;

    void                            registerActor( ActorRef _ref );
    void                            unregisterActor( ActorRef _ref );
    void                            updateWaypoint( ActorRef _actor, ActorRef _waypoint );
    const RegisteredActorsList&     getRegisteredActors() const { return m_registeredActors; }
    ActorRef                        getWaypointFromActor( ActorRef _ref ) const;

private:

    // singleton
    Ray_AINetworkManager();
    ~Ray_AINetworkManager();
    static Ray_AINetworkManager* s_instance;

    //

    RegisteredActorsList            m_registeredActors;
};

}

#endif // _ITF_RAY_AINETWORKMANAGER_H_

