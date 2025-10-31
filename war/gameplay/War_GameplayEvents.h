#ifndef _ITF_WAR_GAMEPLAYEVENTS_H_
#define _ITF_WAR_GAMEPLAYEVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

namespace ITF
{
    class War_EventPickup : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(War_EventPickup, Event,3375386123)

    public:

        War_EventPickup(){}
        virtual ~War_EventPickup(){}
    };

    class War_EventAttackRequestGranted : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(War_EventAttackRequestGranted, Event,855934952)

    public:

        War_EventAttackRequestGranted(){}
        virtual ~War_EventAttackRequestGranted(){}
    };

} // namespace ITF

#endif // _ITF_WAR_GAMEPLAYEVENTS_H_
