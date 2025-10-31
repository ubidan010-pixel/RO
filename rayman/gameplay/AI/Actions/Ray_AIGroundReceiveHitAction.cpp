#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGroundReceiveHitAction.h"
#endif //_ITF_RAY_AIGROUNDRECEIVEHITACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_
#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundReceiveHitAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGroundReceiveHitAction_Template)
        SERIALIZE_MEMBER("unstickMultiplier", m_unstickMultiplier);
    END_SERIALIZATION()

    Ray_AIGroundReceiveHitAction_Template::Ray_AIGroundReceiveHitAction_Template()
    : m_unstickMultiplier(1.0f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundReceiveHitAction)
    BEGIN_VALIDATE_ACTION(Ray_AIGroundReceiveHitAction)
        VALIDATE_ACTION_PARAM("", m_physComponent, "Ray_AIGroundReceiveHitAction requires a StickToPolylinePhysComponent");
    END_VALIDATE_ACTION()

    Ray_AIGroundReceiveHitAction::Ray_AIGroundReceiveHitAction()
        : Super()
        , m_justLanded(bfalse)
        , m_previousUnstickMultiplier(1.0f)
        , m_physComponent(NULL)
        , m_previouslyInAir(bfalse)
    {
    }
    
    void Ray_AIGroundReceiveHitAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    void Ray_AIGroundReceiveHitAction::onActivate()
    {
        Super::onActivate();
        m_justLanded = bfalse;
        m_previousUnstickMultiplier = m_physComponent->getUnstickMinFrictionMultiplier();
        m_physComponent->setUnstickMinFrictionMultiplier(getTemplate()->getUnstickMultiplier());
        m_previouslyInAir = m_physComponent->getStickedPolyline() == NULL;
    }

    void Ray_AIGroundReceiveHitAction::onDeactivate()
    {
        Super::onDeactivate();
        m_physComponent->setUnstickMinFrictionMultiplier(m_previousUnstickMultiplier);

    }
    void  Ray_AIGroundReceiveHitAction::updateHit( f32 _dt )
    {
        bbool onGround = m_physComponent->getStickedPolyline() != NULL;
        m_justLanded = m_previouslyInAir && onGround;
        m_previouslyInAir = !onGround;

    }
}