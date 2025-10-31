
#ifndef _ITF_TOGGLEANIMONEVENTCOMPONENT_H_
#define _ITF_TOGGLEANIMONEVENTCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class ToggleAnimOnEventComponent : public ActorComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(ToggleAnimOnEventComponent,ActorComponent,1085435387);
        DECLARE_SERIALIZE()

        ToggleAnimOnEventComponent();
        ~ToggleAnimOnEventComponent();

        virtual bbool               needsUpdate() const { return bfalse; }
        virtual bbool               needsDraw() const { return bfalse; }
        virtual bbool               needsDraw2D() const { return bfalse; }

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onEvent( Event* _event );

    private:

        ITF_INLINE const class ToggleAnimOnEventComponent_Template*  getTemplate() const;


        class AnimLightComponent*   m_animComponent;
        bbool                       m_startOpen;
        bbool                       m_open;
    };

    //---------------------------------------------------------------------------------------------------

    class ToggleAnimOnEventComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ToggleAnimOnEventComponent_Template,TemplateActorComponent,1671062590);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ToggleAnimOnEventComponent);

    public:

        ToggleAnimOnEventComponent_Template(){}
        ~ToggleAnimOnEventComponent_Template();

        ITF_INLINE ITF::StringID getOpenAnim() const { return m_openAnim; }
        ITF_INLINE ITF::StringID getCloseAnim() const { return m_closeAnim; }

    private:

        StringID                    m_openAnim;
        StringID                    m_closeAnim;
    };


    ITF_INLINE const ToggleAnimOnEventComponent_Template*  ToggleAnimOnEventComponent::getTemplate() const {return static_cast<const ToggleAnimOnEventComponent_Template*>(m_template);}
}

#endif // _ITF_TOGGLEANIMONEVENTCOMPONENT_H_
