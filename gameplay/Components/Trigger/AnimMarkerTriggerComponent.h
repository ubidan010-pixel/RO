#ifndef _ITF_ANIMMARKERANIMMARKERTRIGGERCOMPONENT_H_
#define _ITF_ANIMMARKERANIMMARKERTRIGGERCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    //---------------------------------------------------------------------------------------------------
    struct MarkerEvent
    {
        MarkerEvent() : m_event(NULL){}
        DECLARE_SERIALIZE();
        StringID m_marker;
        Event * m_event;
    };


    class AnimMarkerTriggerComponent : public TriggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMarkerTriggerComponent, TriggerComponent,1235306837);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        AnimMarkerTriggerComponent();
        virtual ~AnimMarkerTriggerComponent();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );
        void clear();

    protected:
        AnimLightComponent * m_animComponent;
    private:
        ITF_INLINE const class AnimMarkerTriggerComponent_Template*  getTemplate() const ;
        SafeArray<MarkerEvent> m_markerEvents;
    private:
    };



    class AnimMarkerTriggerComponent_Template : public TriggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AnimMarkerTriggerComponent_Template,TriggerComponent_Template,1569578840);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AnimMarkerTriggerComponent);

    public:

        AnimMarkerTriggerComponent_Template();
        ~AnimMarkerTriggerComponent_Template();

        ITF_INLINE const SafeArray<MarkerEvent> & getMarkerEvents() const { return m_markerEvents; }
    private:


        SafeArray<MarkerEvent> m_markerEvents;
    };

    ITF_INLINE const AnimMarkerTriggerComponent_Template*  AnimMarkerTriggerComponent::getTemplate() const {return static_cast<const AnimMarkerTriggerComponent_Template*>(m_template);}


} // namespace ITF

#endif //_ITF_AnimMarkerTriggerComponent_H_