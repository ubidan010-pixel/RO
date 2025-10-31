#ifndef _ITF_TRIGGERONBUTTONPRESSEDCOMPONENT_H_
#define _ITF_TRIGGERONBUTTONPRESSEDCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    class TriggerOnButtonPressedComponent : public TriggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TriggerOnButtonPressedComponent, TriggerComponent,3116106140);
        DECLARE_SERIALIZE()

    public:

        TriggerOnButtonPressedComponent();
        virtual ~TriggerOnButtonPressedComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        checkSendEvent( ActorRefList &actorsInsideThisFrame );
        void clear();

    private:
        ITF_INLINE const class TriggerOnButtonPressedComponent_Template*  getTemplate() const ;
        SafeArray<u32>  m_inputs;

    };



    class TriggerOnButtonPressedComponent_Template : public TriggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(TriggerOnButtonPressedComponent_Template,TriggerComponent_Template,1700547359);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TriggerOnButtonPressedComponent);

    public:

        TriggerOnButtonPressedComponent_Template();
        ~TriggerOnButtonPressedComponent_Template();

        ITF_INLINE const ITF_VECTOR<u32> & getInputs() const { return m_inputs; }

    private:
        ITF_VECTOR<u32>  m_inputs;
    };

    ITF_INLINE const TriggerOnButtonPressedComponent_Template*  TriggerOnButtonPressedComponent::getTemplate() const {return static_cast<const TriggerOnButtonPressedComponent_Template*>(m_template);}


} // namespace ITF

#endif //_ITF_TRIGGERONBUTTONPRESSEDCOMPONENT_H_