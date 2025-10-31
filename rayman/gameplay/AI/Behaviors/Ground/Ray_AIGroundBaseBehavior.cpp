#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundBaseBehavior_Template)


    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundBaseBehavior)
    BEGIN_VALIDATE_BEHAVIOR(Ray_AIGroundBaseBehavior)
        VALIDATE_BEHAVIOR_PARAM("", m_physComponent, "Ray_AIGroundBaseBehavior requires a StickToPolylinePhysComponent");
    END_VALIDATE_BEHAVIOR()


    Ray_AIGroundBaseBehavior::Ray_AIGroundBaseBehavior()
        : Super()
        , m_physComponent(NULL)
    {
    }

    Ray_AIGroundBaseBehavior::~Ray_AIGroundBaseBehavior()
    {
    }

    void Ray_AIGroundBaseBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated
    }

}