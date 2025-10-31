#include "precompiled_gameplay.h"

#ifndef _ITF_ANIMTRIGGEREDCOMPONENT_H_
#include "gameplay/components/misc/AnimTriggeredComponent.h"
#endif //_ITF_ANIMTRIGGEREDCOMPONENT_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(AnimTriggeredComponent)

BEGIN_SERIALIZATION_CHILD(AnimTriggeredComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(AnimTriggeredComponent)
    VALIDATE_COMPONENT_PARAM("", m_animComponent, "AnimTriggeredComponent requires an AnimatedComponent");
    VALIDATE_COMPONENT_PARAM("", m_detectorComponent, "AnimTriggeredComponent requires a DetectorComponent");
END_SERIALIZATION()

AnimTriggeredComponent::AnimTriggeredComponent()
: m_triggered(bfalse)
{
}


AnimTriggeredComponent::~AnimTriggeredComponent()
{
}

void AnimTriggeredComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_animComponent = m_actor->GetComponent<AnimatedComponent>();
    m_detectorComponent = m_actor->GetComponent<DetectorComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setAnim(getTemplate()->getUntriggerActionId());
    }
}

void AnimTriggeredComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    if ( m_animComponent->isInTransition() )
    {
        return;
    }

    bbool shouldTrigger = (m_detectorComponent->getActorsInside().size() > 0);

    if ( m_triggered != shouldTrigger )
    {
        m_triggered = shouldTrigger;

        if ( m_triggered )
        {
            m_animComponent->setAnim(getTemplate()->getTriggerActionId());
        }
        else
        {
            m_animComponent->setAnim(getTemplate()->getUntriggerActionId());
        }
    }
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(AnimTriggeredComponent_Template)
BEGIN_SERIALIZATION(AnimTriggeredComponent_Template)

    SERIALIZE_MEMBER("TriggeredAction",m_triggerActionId);
    SERIALIZE_MEMBER("UntriggeredAction",m_untriggerActionId);

END_SERIALIZATION()

}