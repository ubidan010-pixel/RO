#ifndef _ITF_ACTIVEBROADCASTCOMPONENT_H_
#define _ITF_ACTIVEBROADCASTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{
    class ActiveBroadcastComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ActiveBroadcastComponent, ActorComponent, 239394893);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
        //BEGIN_SERIALIZATION_CHILD(ActiveBroadcastComponent) END_SERIALIZATION()
    public:
        virtual bbool                       needsUpdate() const { return bfalse; }
        virtual bbool                       needsDraw() const { return bfalse; }
        virtual bbool                       needsDraw2D() const { return bfalse; }

        void onBecomeActive() override
        {
             Super::onBecomeActive();

             EventActorActiveChanged evt(GetActor(), btrue);
             EVENTMANAGER->broadcastEvent(&evt);
        }

        void onBecomeInactive() override {
            Super::onBecomeInactive();

            EventActorActiveChanged evt(GetActor(), bfalse);
            EVENTMANAGER->broadcastEvent(&evt);
        }


        ITF_INLINE const class ActiveBroadcastComponent_Template* getTemplate() const;
    };

    //---------------------------------------------------------------------------------------------------

    class ActiveBroadcastComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ActiveBroadcastComponent_Template, TemplateActorComponent, 4009445990);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ActiveBroadcastComponent);

    public:

        ActiveBroadcastComponent_Template() {}
        ~ActiveBroadcastComponent_Template() {}
    };


    const ActiveBroadcastComponent_Template* ActiveBroadcastComponent::getTemplate() const { return static_cast<const ActiveBroadcastComponent_Template*>(m_template); }

    //---------------------------------------------------------------------------------------------------
}
#endif
