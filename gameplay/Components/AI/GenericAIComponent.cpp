#include "precompiled_gameplay.h"

#ifndef _ITF_GENERICAICOMPONENT_H_
#include "gameplay/Components/AI/GenericAIComponent.h"
#endif //_ITF_GENERICAICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(GenericAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(GenericAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("genericBehavior", m_behavior, ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()

    GenericAIComponent_Template::GenericAIComponent_Template()
    : m_behavior(NULL)
    {
    }

    GenericAIComponent_Template::~GenericAIComponent_Template()
    {
        SF_DEL(m_behavior);
    }

    IMPLEMENT_OBJECT_RTTI(GenericAIComponent)
    BEGIN_SERIALIZATION_CHILD(GenericAIComponent)
    END_SERIALIZATION()

    GenericAIComponent::GenericAIComponent()
    : m_behavior(NULL)
    {
    }

    GenericAIComponent::~GenericAIComponent()
    {
    }


    void GenericAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_behavior = createAiBehavior(getTemplate()->getBehavior());

        ITF_ASSERT(m_behavior);

        Super::onActorLoaded(_hotReload);

        setBehavior(m_behavior);
    }


}