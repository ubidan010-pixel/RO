#ifndef _ITF_INTERPOLATORCOMPONENT_H_
#define _ITF_INTERPOLATORCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class LinkComponent;

    class InterpolatorComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(InterpolatorComponent, ActorComponent,1526088346);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()
    public:

        InterpolatorComponent();
        ~InterpolatorComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void Update( f32 _dt );
        virtual bbool canUpdate() { return true; }

    protected:

        virtual f32 getInputValue( f32 _dt ) = 0;
        Vec2d getInputPos() const;

        f32 m_inputValue;

    private:
        ITF_INLINE const class InterpolatorComponent_Template*  getTemplate() const;
        LinkComponent* m_linkComponent;
    };

    class InterpolatorComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(InterpolatorComponent_Template, TemplateActorComponent,3527017563);
        DECLARE_SERIALIZE_VIRTUAL();

    public:

        InterpolatorComponent_Template();
        ~InterpolatorComponent_Template();

        ITF_INLINE StringID getInput() const { return m_input; }
        ITF_INLINE bbool getUseMainCharacter() const { return m_useMainCharacter; }

    private:

        StringID m_input;
        bbool m_useMainCharacter; // true uses main character as trigger, otherwise uses center of screen
    };
    const InterpolatorComponent_Template*  InterpolatorComponent::getTemplate() const {return static_cast<const InterpolatorComponent_Template*>(m_template);} 

}

#endif // _ITF_INTERPOLATORCOMPONENT_H_