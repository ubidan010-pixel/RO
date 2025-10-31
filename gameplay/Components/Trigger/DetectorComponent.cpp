#include "precompiled_gameplay.h"

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(DetectorComponent)

    BEGIN_SERIALIZATION_CHILD(DetectorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(DetectorComponent)
    END_VALIDATE_COMPONENT()


    DetectorComponent::DetectorComponent()
        : m_disabled(bfalse)
    {
    }

    DetectorComponent::~DetectorComponent()
    {
    }

    void DetectorComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        if (getTemplate()->getResetOnCheckpoint())
        {
            m_actorsInside.clear();
        }
    }

    void DetectorComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874),this);
    }

    void DetectorComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        m_actorsInside.clear();
    }

    void DetectorComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventDisableCollision* collision = _event->DynamicCast<EventDisableCollision>(ITF_GET_STRINGID_CRC(EventDisableCollision,3761196874)) )
        {
            m_disabled = collision->getDisable();
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(DetectorComponent_Template)
    BEGIN_SERIALIZATION(DetectorComponent_Template)

        SERIALIZE_MEMBER("resetOnCheckpoint", m_resetOnCheckpoint);

    END_SERIALIZATION()

    DetectorComponent_Template::DetectorComponent_Template()
        : m_resetOnCheckpoint(btrue)
    {
    }
};