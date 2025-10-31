#ifndef _ITF_RAY_SWARMCHASENODECOMPONENT_H_
#define _ITF_RAY_SWARMCHASENODECOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_SwarmChaseNodeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SwarmChaseNodeComponent, ActorComponent,3618604596)
        DECLARE_SERIALIZE();

    public:

        Ray_SwarmChaseNodeComponent();
        ~Ray_SwarmChaseNodeComponent();

        virtual bbool needsDraw() const     { return bfalse;}
        virtual bbool needsDraw2D() const   { return bfalse;}
        virtual bbool needsUpdate() const   { return bfalse;}

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

#ifdef ITF_SUPPORT_EDITOR
        virtual void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

                f32 getRadius() const;

    private:

        ITF_INLINE const class Ray_SwarmChaseNodeComponent_Template*  getTemplate() const;


    };

    //*****************************************************************************
    // Template
    //*****************************************************************************


    class Ray_SwarmChaseNodeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SwarmChaseNodeComponent_Template, TemplateActorComponent,1787337512);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SwarmChaseNodeComponent);

    public:

        Ray_SwarmChaseNodeComponent_Template();
        ~Ray_SwarmChaseNodeComponent_Template();

    private:

    };

    const Ray_SwarmChaseNodeComponent_Template*  Ray_SwarmChaseNodeComponent::getTemplate() const {return static_cast<const Ray_SwarmChaseNodeComponent_Template*>(m_template);}
}

#endif //_ITF_RAY_SWARMCHASENODECOMPONENT_H_
