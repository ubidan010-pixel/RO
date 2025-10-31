#ifndef _ITF_RAY_SEQUENCESWITCHCOMPONENT_H_
#define _ITF_RAY_SEQUENCESWITCHCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF {

class LinkComponent;
class Ray_SequenceSwitchComponent_Template;

//------------------------------------------------------------------------------
class Ray_SequenceSwitchComponent : public ActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SequenceSwitchComponent,ActorComponent,3421407123)
    DECLARE_VALIDATE_COMPONENT()
    DECLARE_SERIALIZE()

public:

    Ray_SequenceSwitchComponent();
    virtual ~Ray_SequenceSwitchComponent();

    virtual bbool needsUpdate() const { return bfalse; }
    virtual bbool needsDraw() const { return bfalse; }
    virtual bbool needsDraw2D() const { return bfalse; }

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onBecomeActive();
    virtual void onEvent(Event* _event);
   
private:
    ITF_INLINE const Ray_SequenceSwitchComponent_Template* getTemplate() const;

    void processTrigger(EventTrigger* _triggerEvt);

    ActorRef m_sequencePlayerRef;
    ActorRef m_otherActorRef;
    LinkComponent* m_linkComponent;
};

//------------------------------------------------------------------------------
class Ray_SequenceSwitchComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_SequenceSwitchComponent_Template,TemplateActorComponent,1190719862)
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SequenceSwitchComponent);
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_SequenceSwitchComponent_Template*
Ray_SequenceSwitchComponent::getTemplate() const
{
    return static_cast<const Ray_SequenceSwitchComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_SEQUENCESWITCHCOMPONENT_H_
