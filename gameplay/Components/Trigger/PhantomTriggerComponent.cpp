#include "precompiled_gameplay.h"

#ifndef _ITF_PHANTOMTRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/PhantomTriggerComponent.h"
#endif //_ITF_PHANTOMTRIGGERCOMPONENT_H_

#ifndef _ITF_PHANTOMDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PhantomDetectorComponent.h"
#endif //_ITF_PHANTOMDETECTORCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PhantomTriggerComponent_Template)

    BEGIN_SERIALIZATION_CHILD(PhantomTriggerComponent_Template)
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(PhantomTriggerComponent)

    BEGIN_SERIALIZATION_CHILD(PhantomTriggerComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(PhantomTriggerComponent)
    VALIDATE_COMPONENT_PARAM("", m_detectorComponent && m_detectorComponent->IsClassCRC(ITF_GET_STRINGID_CRC(PhantomDetectorComponent,2329854250)), "PhantomTriggerComponent requires a PhantomDetectorComponent");
    END_VALIDATE_COMPONENT()

    PhantomTriggerComponent::PhantomTriggerComponent()
    {
    }

    PhantomTriggerComponent::~PhantomTriggerComponent()
    {
    }


    void PhantomTriggerComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_detectorComponent = m_actor->GetComponent<PhantomDetectorComponent>();
        if (m_detectorComponent)
        {
            m_shapeDetectorComponent = static_cast<ShapeDetectorComponent*>(m_detectorComponent);
            m_playerDetectorComponent = NULL;
        }
    }

};