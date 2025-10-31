#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SEQUENCESWITCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SequenceSwitchComponent.h"
#endif //_ITF_RAY_SEQUENCESWITCHCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_
#endif

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SequenceSwitchComponent)
BEGIN_SERIALIZATION_CHILD(Ray_SequenceSwitchComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_SequenceSwitchComponent)
    VALIDATE_COMPONENT_PARAM("", m_linkComponent, "LinkComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_SequenceSwitchComponent::Ray_SequenceSwitchComponent()
    : Super()
    , m_sequencePlayerRef()
    , m_otherActorRef()
    , m_linkComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_SequenceSwitchComponent::~Ray_SequenceSwitchComponent()
{
}

//------------------------------------------------------------------------------
void Ray_SequenceSwitchComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_linkComponent = m_actor->GetComponent<LinkComponent>();

    // listen to trigger event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
}

//------------------------------------------------------------------------------
void Ray_SequenceSwitchComponent::onBecomeActive()
{
    Super::onBecomeActive();

#ifdef ITF_SUPPORT_EDITOR
    // check if there's a single parent to prevent mistakes
    ObjectRefList* parents = LINKMANAGER->getParents(m_actor->getRef());
    ITF_WARNING_CATEGORY(
        LD, m_actor, parents && parents->size()>0,
        "No parent found, "
        "please parent with one trigger box");
    if (parents)
    {
        ITF_WARNING_CATEGORY(
            LD, m_actor, parents->size()==1,
            "Several parents found, "
            "please parent with one trigger box only");
    }
#endif

    // safety check, this is validated so this should not happen...
    ITF_ASSERT(m_linkComponent);
    if (!m_linkComponent)
    {
        return;
    }

    // collect first and second child
    AIUtils::LinkIterator it(m_linkComponent);
    Actor* firstChild = it.getNextRelativeActor();
    Actor* secondChild = it.getNextRelativeActor();
    ITF_WARNING_CATEGORY(
        LD, m_actor, (firstChild && secondChild),
        "No child found, "
        "please link to a sequence player and another actor, then reload");
    if (!(firstChild && secondChild))
    {
        m_sequencePlayerRef.invalidate();
        m_otherActorRef.invalidate();
        return;
    }

    // find the sequence player and the other actor
    bbool firstIsSequence = (NULL != firstChild->GetComponent<SequencePlayerComponent>());
    bbool secondIsSequence = (NULL != secondChild->GetComponent<SequencePlayerComponent>());
    ITF_WARNING_CATEGORY(
        LD, m_actor, !(firstIsSequence && secondIsSequence),
        "Both children are sequence players, "
        "please link to a sequence player and another actor, then reload");
    ITF_WARNING_CATEGORY(
        LD, m_actor, firstIsSequence || secondIsSequence,
        "None of the children is a sequence player, "
        "please link to a sequence player and another actor, then reload");
    if (firstIsSequence == secondIsSequence)
    {
        m_sequencePlayerRef.invalidate();
        m_otherActorRef.invalidate();
        return;
    }
    m_sequencePlayerRef = firstIsSequence
        ? firstChild->getRef()
        : secondChild->getRef();
    m_otherActorRef = firstIsSequence
        ? secondChild->getRef()
        : firstChild->getRef();
}

//------------------------------------------------------------------------------
void Ray_SequenceSwitchComponent::onEvent(Event* _event)
{
    Super::onEvent(_event);

    if (EventTrigger* triggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        processTrigger(triggerEvt);
    }
}

//------------------------------------------------------------------------------
void Ray_SequenceSwitchComponent::processTrigger(EventTrigger* _triggerEvt)
{
    // process activation only
    if (!_triggerEvt->getActivated())
    {
        return;
    }

    // first time triggered: trigger child sequence player
    if (!RAY_GAMEMANAGER->wasSequenceSwitchTriggered(m_actor->getRef()))
    {
        if (Actor* sequencePlayer = m_sequencePlayerRef.getActor())
        {
            sequencePlayer->onEvent(_triggerEvt);
        }
        RAY_GAMEMANAGER->setSequenceSwitchTriggered(m_actor->getRef());
    }
    // then: trigger other actor
    else
    {
        if (Actor* otherActor = m_otherActorRef.getActor())
        {
            otherActor->onEvent(_triggerEvt);
        }
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SequenceSwitchComponent_Template)
BEGIN_SERIALIZATION(Ray_SequenceSwitchComponent_Template)
END_SERIALIZATION()

} // namespace ITF
