#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_FixedAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_FixedAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("genericBehavior", m_behavior, ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()

    Ray_FixedAIComponent_Template::Ray_FixedAIComponent_Template()
    :m_behavior(NULL)
    {
    }

    Ray_FixedAIComponent_Template::~Ray_FixedAIComponent_Template()
    {
        SF_DEL(m_behavior);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_FixedAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_FixedAIComponent)
    END_SERIALIZATION()

    void Ray_FixedAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_behavior = createAiBehavior(getTemplate()->getBehavior());

        Super::onActorLoaded(_hotReload);

        if (m_behavior) setBehavior(m_behavior);
    }
} // namespace ITF

