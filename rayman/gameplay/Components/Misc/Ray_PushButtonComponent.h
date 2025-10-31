#ifndef _ITF_RAY_PUSHBUTTONCOMPONENT_H_
#define _ITF_RAY_PUSHBUTTONCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_EVENTACTIVATEHANDLER_H_
#include "gameplay/AI/Utils/EventActivateHandler.h"
#endif //_ITF_EVENTACTIVATEHANDLER_H_

#ifndef _ITF_INPUTCRITERIAS_H_
#include "engine/blendTree/InputCriterias.h"
#endif //_ITF_INPUTCRITERIAS_H_

namespace ITF
{

class AnimatedComponent;
class EventStickOnPolyline;
class LinkComponent;
class Ray_PushButtonComponent_Template;

//------------------------------------------------------------------------------
/// Push-button: on/off anim + wait for re-trigger delay.
class Ray_PushButtonComponent : public ActorComponent, public IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_PushButtonComponent, ActorComponent,2244274439)
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    Ray_PushButtonComponent();
    virtual ~Ray_PushButtonComponent();

    virtual bbool needsUpdate() const { return btrue; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onSceneActive();
    virtual void onCheckpointLoaded();
    virtual void Update(f32 _dt);
    virtual void onEvent(Event* _event);
    virtual void updateAnimInput();

private:
    enum State_t { State_Off, State_OffToOn, State_On, State_OnToOff, State_ProgressiveToOn };

    ITF_INLINE const Ray_PushButtonComponent_Template* getTemplate() const;
    void processStick(const EventStickOnPolyline* _stick);
    void processHit(const HitStim* _hitStim);
    bbool getAndResetTriggered();
    void trigger(bbool _count = btrue);
    
    State_t m_state;
    f32 m_onOffCursor;
    f32 m_countdown;
    u32 m_stickedActorsCount;
    bbool m_wasHit;
    
    ActorRef m_activator;
    EventTrigger m_eventTrigger;
    EventActivateHandler m_eventActivateHandler;
    u32 m_triggerCount;
    
    LinkComponent* m_linkComponent;
    AnimatedComponent* m_animatedComponent;

    u32 m_progressiveCursor;
};

//------------------------------------------------------------------------------
class Ray_PushButtonComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_PushButtonComponent_Template, TemplateActorComponent,2466817767);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PushButtonComponent);

public:
    Ray_PushButtonComponent_Template();
    virtual ~Ray_PushButtonComponent_Template();
    
    ITF_INLINE f32 getOnOffDuration() const { return m_onOffDuration; }
    ITF_INLINE f32 getDelay() const { return m_delay; }

    ITF_INLINE bbool getActivateChildren() const { return m_activateChildren; }
    ITF_INLINE bbool getTriggerOnStick() const { return m_triggerOnStick; }
    ITF_INLINE bbool getTriggerOnHit() const { return m_triggerOnHit; }
    ITF_INLINE bbool getIsProgressive() const { return m_isProgressive; }
    ITF_INLINE const SafeArray<u32> &getProgressiveHitLevels() const { return m_progressiveHitLevels; }
    ITF_INLINE bbool getStayOn() const { return m_stayOn; }
    ITF_INLINE f32   getProgressiveSpeed() const { return m_progressiveSpeed; }
    
    
private:
    f32 m_onOffDuration;
    f32 m_delay;
    
    bbool m_activateChildren;
    bbool m_triggerOnStick;
    bbool m_triggerOnHit;
    bbool m_isProgressive;
    bbool m_stayOn;
    SafeArray <u32>     m_progressiveHitLevels;
    f32 m_progressiveSpeed;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_PushButtonComponent_Template*
Ray_PushButtonComponent::getTemplate() const
{
    return static_cast<const Ray_PushButtonComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_PUSHBUTTONCOMPONENT_H_
