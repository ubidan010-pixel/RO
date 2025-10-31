#ifndef _ITF_TRIGGERCOMPONENT_H_
#define _ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_EVENTACTIVATEHANDLER_H_
#include "gameplay/AI/Utils/EventActivateHandler.h"
#endif //_ITF_EVENTACTIVATEHANDLER_H_

namespace ITF
{
    class AnimLightComponent;
    class DetectorComponent;
    class LinkComponent;
    class PlayerDetectorComponent;
    class ShapeDetectorComponent;

    class TriggerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TriggerComponent, ActorComponent,1580214848);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        TriggerComponent();
        virtual ~TriggerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onSceneActive();
        virtual void        onCheckpointLoaded();
        virtual void        Update( f32 _dt );

        virtual void        onEvent( Event* _event );

        void                resetTriggerDone() { m_triggerOnceDone = bfalse; }
        bbool               isTriggerOnceDone() const {return m_triggerOnceDone;}

    protected:

        bbool               canTrigger() const;
        virtual void        sendEvent( ActorRef _activator, bbool _enter, const bbool _fromRetriggerOnCheckpoint = bfalse );
        void                sendEvent( ActorRef _activator, Event* _evt, const bbool _fromRetriggerOnCheckpoint = bfalse );
        virtual void        checkSendEvent( ActorRefList &actorsInsideThisFrame );

        DetectorComponent*  m_detectorComponent;
        ShapeDetectorComponent* m_shapeDetectorComponent;
        PlayerDetectorComponent* m_playerDetectorComponent;
        LinkComponent*      m_linkComponent;

        bbool               m_disabled;
        ActorRefList        m_actorsInsideLastFrame;

        EventActivateHandler m_eventActivateHandler;

        Event*              m_onEnterEvent;
        Event*              m_onExitEvent;

    private:

        void                clear();

        bbool               m_triggerOnceDone;
        ObjectRef           m_activator;

    private:

        ITF_INLINE const class TriggerComponent_Template*  getTemplate() const;

        void                convertInstanceDataToLinkComponent();
        void                convertFromOldFormat();

        // deprecated, this won't be used anymore
        typedef ITF_VECTOR<ObjectPath> ChildrenList;
        ChildrenList        m_children;
        ObjectIdList        m_linkedChildrenIds;
        bbool               m_retriggerOnCheckpoint;
    };

    //---------------------------------------------------------------------------------------------------

    class TriggerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TriggerComponent_Template,TemplateActorComponent,249157773);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TriggerComponent);

    public:

        TriggerComponent_Template();
        ~TriggerComponent_Template();

        bbool               getActivateChildren() const { return m_activateChildren; }
        bbool               getTriggerOnce() const { return m_triggerOnce; }

        bbool               getResetOnCheckpoint() const { return m_resetOnCheckpoint; }
        bbool               getResetOnExit() const { return m_resetOnExit; }

        bbool               getTriggerOnDetector() const { return m_triggerOnDetector; }
        bbool               getTriggerOnHit() const { return m_triggerOnHit; }
        bbool               getTriggerOnWind() const { return m_triggerOnWind; }
        bbool               getTriggerOnCrush() const { return m_triggerOnCrush; }


        bbool               getTriggerSelf() const { return m_triggerSelf; }
        bbool               getTriggerParent() const { return m_triggerParent; }
        bbool               getTriggerChildren() const { return m_triggerChildren; }
        bbool               getTriggerActivator() const { return m_triggerActivator; }
        bbool               getTriggerGameManager() const { return m_triggerGameManager; }
        bbool               getTriggerBroadcast() const { return m_triggerBroadcast; }
        bbool               getTriggerEachActor() const { return m_triggerEachActor; }

        bbool               getTriggerable() const { return m_triggerable; }

        const class Event*  getOnEnterEvent() const { return m_onEnterEvent; }
        const class Event*  getOnExitEvent() const { return m_onExitEvent; }
        bbool               getSendEventEveryFrame() const { return m_sendEventEveryFrame; }

    private:

        bbool               m_activateChildren;             // previously ActivateTriggerComponent, sends EventActivate (off) to children when linked and EventActivate (on) on any sendEvent()
        bbool               m_triggerOnce;

        bbool               m_resetOnCheckpoint;
        bbool               m_resetOnExit;

        bbool               m_triggerOnDetector;
        bbool               m_triggerOnHit;
        bbool               m_triggerOnWind;
        bbool               m_triggerOnCrush;

        bbool               m_triggerSelf;
        bbool               m_triggerChildren;
        bbool               m_triggerParent;
        bbool               m_triggerActivator;
        bbool               m_triggerGameManager;
        bbool               m_triggerBroadcast;             // broadcast to anyone who registered via EventManager
        bbool               m_triggerEachActor;

        bbool               m_triggerable;

        Event*              m_onEnterEvent;                 // applies for any triggerOn flag
        Event*              m_onExitEvent;                  // applies only with triggerOnDetector set, when actors exit the detector
        bbool               m_sendEventEveryFrame;          // If we send the event even if the actor was there last frame
    };

    ITF_INLINE const TriggerComponent_Template*  TriggerComponent::getTemplate() const {return static_cast<const TriggerComponent_Template*>(m_template);}

    ITF_INLINE bbool TriggerComponent::canTrigger() const { return !getTemplate()->getTriggerOnce() || !m_triggerOnceDone;}

} // namespace ITF

#endif //_ITF_TRIGGERCOMPONENT_H_