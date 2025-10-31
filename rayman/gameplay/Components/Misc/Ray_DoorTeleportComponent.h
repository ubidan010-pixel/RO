#ifndef _ITF_RAY_DOORTELEPORTCOMPONENT_H_
#define _ITF_RAY_DOORTELEPORTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_DoorTeleportComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DoorTeleportComponent, ActorComponent,1015048068)

    public:
        DECLARE_SERIALIZE()

        Ray_DoorTeleportComponent();
        virtual ~Ray_DoorTeleportComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent(Event * _event);
        virtual void        onResourceReady();
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

    private:
        ITF_INLINE const class Ray_DoorTeleportComponent_Template * getTemplate() const;

        void                setExitPointPos();
        Vec2d               getExitPointPos() const;

        void                triggered( class EventTrigger* _trigger );

        Vec2d               m_exitPointPos;
        AnimLightComponent* m_animComponent;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_DoorTeleportComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_DoorTeleportComponent_Template,TemplateActorComponent,1716901842);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_DoorTeleportComponent);

    public:

        Ray_DoorTeleportComponent_Template() {}
        ~Ray_DoorTeleportComponent_Template() {}

        const StringID&     getExitPoint() const { return m_exitPointId; }

    private:

        StringID            m_exitPointId;
    };
    ITF_INLINE const class Ray_DoorTeleportComponent_Template * Ray_DoorTeleportComponent::getTemplate() const
    {
        return static_cast<const class Ray_DoorTeleportComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_DOORTELEPORTCOMPONENT_H_
