
#ifndef _ITF_RAY_AIRESCUEMANAGER_H_
#define _ITF_RAY_AIRESCUEMANAGER_H_

namespace ITF
{

#define RAY_AIRESCUEMANAGER Ray_AIRescueManager::get()

class Ray_AIRescueManager
{
public:

    // singleton
    static void                     create();
    static void                     destroy();
    static Ray_AIRescueManager*     get() { return s_instance; }

    // 
    typedef SafeArray <ActorRef> RegisteredActorsList;

    void                            registerActor( ActorRef _ref );
    void                            unregisterActor( ActorRef _ref );
    const RegisteredActorsList&     getRegisteredActors() const { return m_registeredActors; }

private:

    // singleton
    Ray_AIRescueManager();
    ~Ray_AIRescueManager();
    static Ray_AIRescueManager* s_instance;

    //

    RegisteredActorsList            m_registeredActors;
};

}

#endif // _ITF_RAY_AIRESCUEMANAGER_H_

