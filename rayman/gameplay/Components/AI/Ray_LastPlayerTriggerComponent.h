#ifndef _ITF_RAY_LASTPLAYERTRIGGERCOMPONENT_H_
#define _ITF_RAY_LASTPLAYERTRIGGERCOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

namespace ITF
{
    class Ray_LastPlayerTriggerComponent : public TriggerComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LastPlayerTriggerComponent, TriggerComponent,1954106939)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_LastPlayerTriggerComponent();
        virtual ~Ray_LastPlayerTriggerComponent();
        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );

    protected:

        virtual Event* chooseEvent(ActorRef _activator, bbool _enter);
        virtual void checkSendEvent( ActorRefList &actorsInsideThisFrame );
    
    private:
        ITF_INLINE const class Ray_LastPlayerTriggerComponent_Template* getTemplate() const;

        ActorRefList m_activatedActors;
        f32 m_timer;
        bbool m_eventSent;
        f32     m_timeBeforeActivation;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_LastPlayerTriggerComponent_Template : public TriggerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_LastPlayerTriggerComponent_Template, TriggerComponent_Template,2510214116);
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_LastPlayerTriggerComponent);
        DECLARE_SERIALIZE();

    public:

        Ray_LastPlayerTriggerComponent_Template();
        virtual ~Ray_LastPlayerTriggerComponent_Template() {}

        ITF_INLINE bbool getUseTimeBeforeActivation() const { return m_useTimeBeforeActivation; }

    private:
        bbool m_useTimeBeforeActivation;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_LastPlayerTriggerComponent_Template* Ray_LastPlayerTriggerComponent::getTemplate() const
    {
        return static_cast<const Ray_LastPlayerTriggerComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_LASTPLAYERTRIGGERCOMPONENT_H_