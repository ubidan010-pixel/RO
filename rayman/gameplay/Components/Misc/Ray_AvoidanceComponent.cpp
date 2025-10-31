#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AVOIDANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AvoidanceComponent.h"
#endif //_ITF_RAY_AVOIDANCECOMPONENT_H_

#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AvoidanceManager.h"
#endif //_ITF_RAY_AVOIDANCEMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AvoidanceComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_AvoidanceComponent)
    END_SERIALIZATION()

    Ray_AvoidanceComponent::Ray_AvoidanceComponent() 
        : Super()
        , m_squadData(NULL)
        , m_enabled(bfalse)
    {
    }

    Ray_AvoidanceComponent::~Ray_AvoidanceComponent()
    {
        unregisterAvoidance();
    }

    void Ray_AvoidanceComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_enabled = getTemplate()->getEnabled();
    }

    void Ray_AvoidanceComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        registerAvoidance();
    }

    void Ray_AvoidanceComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        unregisterAvoidance();
    }

    void Ray_AvoidanceComponent::registerAvoidance()
    {
        m_squadData = RAY_AVOIDANCEMANAGER->addMember(m_actor->getRef());
        m_squadData->setEnabled(m_enabled);
        m_squadData->setRadius(getTemplate()->getAvoidanceRadius());
        m_squadData->setMinDelta(getTemplate()->getAvoidanceMinDelta());
        m_squadData->setPosition(m_actor->get2DPos());
        m_squadData->setDestination(m_actor->get2DPos());
    }

    void Ray_AvoidanceComponent::unregisterAvoidance()
    {
        if (m_squadData)
        {
            RAY_AVOIDANCEMANAGER->removeMember(m_actor->getRef());
            m_squadData = NULL;
        }
    }

    void Ray_AvoidanceComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_squadData)
        {
            m_squadData->setEnabled(m_enabled);
            m_squadData->setPosition(m_actor->get2DPos());
            m_squadData->setDestination(m_actor->get2DPos());
            m_squadData->setIsBlocked(btrue);   // when in doubt...
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_AvoidanceComponent_Template)
    BEGIN_SERIALIZATION(Ray_AvoidanceComponent_Template)
        SERIALIZE_MEMBER("enabled", m_enabled);
        SERIALIZE_MEMBER("radius", m_avoidanceRadius);
        SERIALIZE_MEMBER("minDelta", m_avoidanceMinDelta);
        SERIALIZE_MEMBER("anticipationCoeff", m_avoidanceAnticipationCoeff);
    END_SERIALIZATION()

    Ray_AvoidanceComponent_Template::Ray_AvoidanceComponent_Template()
    : m_enabled(bfalse)
    , m_avoidanceRadius(1.f)
    , m_avoidanceMinDelta(0.1f)
    , m_avoidanceAnticipationCoeff(1.f)
    {
    }

} // namespace ITF