#include "precompiled_gameplay.h"

#ifndef _ITF_SYNCHRONIZEDANIMCOMPONENT_H_
#include "gameplay/Components/Misc/SynchronizedAnimComponent.h"
#endif //_ITF_SYNCHRONIZEDANIMCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(SynchronizedAnimComponent_Template)

    BEGIN_SERIALIZATION(SynchronizedAnimComponent_Template)
        SERIALIZE_MEMBER("inactiveAnim", m_inactiveAnim);
        SERIALIZE_MEMBER("activeAnim", m_activeAnim);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(SynchronizedAnimComponent)

    BEGIN_SERIALIZATION_CHILD(SynchronizedAnimComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(SynchronizedAnimComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "SynchronizedAnimComponent requires an AnimLightComponent/AnimatedComponent");
        VALIDATE_COMPONENT_PARAM("inactiveAnim", getTemplate()->getInactiveAnim().isValid(), "please set an anim");
        VALIDATE_COMPONENT_PARAM("activeAnim", getTemplate()->getActiveAnim().isValid(), "please set an anim");
    END_VALIDATE_COMPONENT()

    SynchronizedAnimComponent::SynchronizedAnimComponent()
        : m_animComponent(NULL)
    {
    }

    SynchronizedAnimComponent::~SynchronizedAnimComponent()
    {
    }


    void SynchronizedAnimComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();  // mandatory


        if (m_animComponent)
        {
            m_animComponent->setAnim(getTemplate()->getActiveAnim());
        }
    }

    void SynchronizedAnimComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if ( EventActivate* eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
        {
            if ( eventActivate->getActivated() )
            {
                m_animComponent->setAnim(getTemplate()->getActiveAnim());
            }
            else
            {
                m_animComponent->setAnim(getTemplate()->getInactiveAnim());
            }
        }
    }

};