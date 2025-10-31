
#ifndef _ITF_WINDCOMPONENT_H_
#define _ITF_WINDCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

namespace ITF
{
    class WindComponent : public ActorComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(WindComponent,ActorComponent,1340424959);
        DECLARE_SERIALIZE()

        WindComponent();
        ~WindComponent();

        virtual bbool               needsUpdate() const { return bfalse; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                onEvent( class Event* _event );
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class WindComponent_Template*  getTemplate() const;

        void                        clear();

        ForceModifierInstanceArray  m_modifiers;
        bbool                       m_registered;
    };

    //---------------------------------------------------------------------------------------------------

    class WindComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(WindComponent_Template,TemplateActorComponent,3412567697);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(WindComponent);

    public:

        WindComponent_Template() {}
        ~WindComponent_Template() {}

        const ForceModifierArray&   getForceModifiers() const { return m_modifiers; }

    private:

        ForceModifierArray          m_modifiers;
    };

    const WindComponent_Template*  WindComponent::getTemplate() const
    {return static_cast<const WindComponent_Template*>(m_template);}
}

#endif // _ITF_WINDCOMPONENT_H_
