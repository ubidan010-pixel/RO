#ifndef _ITF_RAY_SHOOTERDOORCOMPONENT_H_
#define _ITF_RAY_SHOOTERDOORCOMPONENT_H_

#ifndef _ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_
#include "gameplay/components/misc/EventReceiveAnimPlayComponent.h"
#endif //_ITF_EVENTRECEIVEPLAYANIMCOMPONENT_H_

namespace ITF
{
    class Ray_ShooterDoorComponent : public EventReceivePlayAnimComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterDoorComponent, EventReceivePlayAnimComponent,2820965407)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterDoorComponent();
        virtual ~Ray_ShooterDoorComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onEvent( Event * _event);

    private: 
        ITF_INLINE const class Ray_ShooterDoorComponent_Template * getTemplate() const;

    };

    //---------------------------------------------------------------------------------------------------

    class Ray_ShooterDoorComponent_Template : public EventReceivePlayAnimComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterDoorComponent_Template,EventReceivePlayAnimComponent_Template,937978001);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterDoorComponent);

    public:

        Ray_ShooterDoorComponent_Template() {}
        ~Ray_ShooterDoorComponent_Template() {}
    private:
    };
    ITF_INLINE const class Ray_ShooterDoorComponent_Template * Ray_ShooterDoorComponent::getTemplate() const
    {
        return static_cast<const class Ray_ShooterDoorComponent_Template *>(m_template);
    }

}

#endif //_ITF_RAY_SHOOTERDOORCOMPONENT_H_
