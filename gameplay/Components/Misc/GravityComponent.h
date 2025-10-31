
#ifndef _ITF_GRAVITYCOMPONENT_H_
#define _ITF_GRAVITYCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

namespace ITF
{

    class Event;
    class Texture;

    class GravityComponent : public ActorComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(GravityComponent,ActorComponent,3211267022);
        DECLARE_SERIALIZE()

        GravityComponent();
        ~GravityComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual bbool               needsUpdate() const { return bfalse; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                onEvent( class Event* _event );
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class GravityComponent_Template*  getTemplate() const;

        void                        clear();

        bbool                       m_registered;
        ForceModifierInstanceArray  m_modifiers;
    };


    //---------------------------------------------------------------------------------------------------

    class GravityComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(GravityComponent_Template,TemplateActorComponent,3412637300);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(GravityComponent);

    public:

        GravityComponent_Template() {}
        ~GravityComponent_Template() {}

        const ForceModifierArray&   getForceModifiers() const { return m_modifiers; }

    private:

        ForceModifierArray          m_modifiers;
    };



    const GravityComponent_Template*  GravityComponent::getTemplate() const
    {return static_cast<const GravityComponent_Template*>(m_template);}
}

#endif // _ITF_GRAVITYCOMPONENT_H_
