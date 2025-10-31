

#include "precompiled_gameplay.h"

#ifndef _ITF_TOGGLEANIMONEVENTCOMPONENT_H_
#include "gameplay/components/misc/ToggleAnimOnEventComponent.h"
#endif //_ITF_TOGGLEANIMONEVENTCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(ToggleAnimOnEventComponent)

BEGIN_SERIALIZATION_CHILD(ToggleAnimOnEventComponent)
BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
SERIALIZE_MEMBER("startOpen",   m_startOpen);
END_CONDITION_BLOCK()
END_SERIALIZATION()

ToggleAnimOnEventComponent::ToggleAnimOnEventComponent()
: m_animComponent(NULL)
, m_startOpen(btrue)
, m_open(btrue)
{
}

ToggleAnimOnEventComponent::~ToggleAnimOnEventComponent()
{
}


void ToggleAnimOnEventComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687),this);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    if (m_startOpen)
    {
        m_animComponent->setAnim(getTemplate()->getOpenAnim());
        m_open = btrue;
    }
    else
    {
        m_animComponent->setAnim(getTemplate()->getCloseAnim());
        m_open = bfalse;
    }
}


void ToggleAnimOnEventComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);
    if (EventTrigger * onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        m_open = !m_open;
        if (m_open)
            m_animComponent->setAnim(getTemplate()->getOpenAnim());
        else
            m_animComponent->setAnim(getTemplate()->getCloseAnim());
    }
    else if (EventGeneric * generic = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)))
    {
        if (generic->getId() == ITF_GET_STRINGID_CRC(Open,812432275))
        {
            m_open = btrue;
            m_animComponent->resetCurTime();
            m_animComponent->setAnim(getTemplate()->getOpenAnim());
        }
        else if (generic->getId() == ITF_GET_STRINGID_CRC(Close,3883599349))
        {   
            m_open = bfalse;
            m_animComponent->resetCurTime();
            m_animComponent->setAnim(getTemplate()->getCloseAnim());
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(ToggleAnimOnEventComponent_Template)
BEGIN_SERIALIZATION(ToggleAnimOnEventComponent_Template)

    SERIALIZE_MEMBER("openAnim",m_openAnim);
    SERIALIZE_MEMBER("closeAnim",m_closeAnim);


END_SERIALIZATION()

ToggleAnimOnEventComponent_Template::~ToggleAnimOnEventComponent_Template()
{
}

}
