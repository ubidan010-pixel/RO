#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DARKARENADOORCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_DarkArenaDoorComponent.h"
#endif //_ITF_RAY_DARKARENADOORCOMPONENT_H_

#ifndef _ITF_WINDCOMPONENT_H_
#include "gameplay/components/misc/WindComponent.h"
#endif //_ITF_WINDCOMPONENT_H_

#define STATE_IDLE      0
#define STATE_HIT       1


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_DarkArenaDoorComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_DarkArenaDoorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_DarkArenaDoorComponent)
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_DarkArenaDoorComponent::Ray_DarkArenaDoorComponent()
    : m_windComponent(NULL)
    , m_state(STATE_IDLE)
    {
        // none
    }

    //*****************************************************************************

    Ray_DarkArenaDoorComponent::~Ray_DarkArenaDoorComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_DarkArenaDoorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_windComponent = m_actor->GetComponent<WindComponent>(); // not mandatory 

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

        m_state = STATE_IDLE;
    }

    //*****************************************************************************


    void Ray_DarkArenaDoorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (!m_isOpen && m_state == STATE_HIT)
        {
            if (m_animComponent->isSubAnimFinished())
            {
                m_state = STATE_IDLE;
                m_animComponent->setAnim(getTemplate()->getIdleAnim());
            }
        }

    }

    //*****************************************************************************

    void Ray_DarkArenaDoorComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (!m_isOpen && m_state != STATE_HIT)
        {
            if (EventGeneric* eventGen = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)))
            {
                if (eventGen->getId() == ITF_GET_STRINGID_CRC(DarkDoor_HitFromChild,1773128499))
                {
                    m_state = STATE_HIT;
                    m_animComponent->setAnim(getTemplate()->getHitAnim());
                }
            }
        }

    }

    //*****************************************************************************

    void Ray_DarkArenaDoorComponent::openDoor()
    {
        Super::openDoor();

        if (m_windComponent)
        {
            m_windComponent->pause();                
        }
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************


    IMPLEMENT_OBJECT_RTTI(Ray_DarkArenaDoorComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_DarkArenaDoorComponent_Template)
        SERIALIZE_MEMBER("hitAnim", m_hitAnim);
        SERIALIZE_MEMBER("idleAnim", m_idleAnim);
    END_SERIALIZATION()

    //*****************************************************************************


    Ray_DarkArenaDoorComponent_Template::Ray_DarkArenaDoorComponent_Template()
    {
        // none
    }

};