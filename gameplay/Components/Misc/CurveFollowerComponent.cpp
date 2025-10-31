#include "precompiled_gameplay.h"

#ifndef _ITF_CURVEFOLLOWERCOMPONENT_H_
#include "gameplay/Components/Misc/CurveFollowerComponent.h"
#endif //_ITF_CURVEFOLLOWERCOMPONENT_H_

#ifndef _ITF_CURVECOMPONENT_H_
#include "gameplay/Components/Misc/CurveComponent.h"
#endif //_ITF_CURVECOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CurveFollowerComponent)

    BEGIN_SERIALIZATION_CHILD(CurveFollowerComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(CurveFollowerComponent)
        VALIDATE_COMPONENT_PARAM("", m_curveComponent,              "CurveFollowerComponent requires an CurveComponent.");
        VALIDATE_COMPONENT_PARAM("", !m_actor->getParentBind(),     "This actor shouldn't be binding because it's moving by itself in the world");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    CurveFollowerComponent::CurveFollowerComponent()
    : m_curveComponent(NULL)
    , m_dist(0.0f)
    , m_disabled(bfalse)
    , m_playing(bfalse)
    , m_isTrig(bfalse)
    {
        // none
    }

    //*****************************************************************************

    CurveFollowerComponent::~CurveFollowerComponent()
    {
        //none
    }

    //*****************************************************************************

    void CurveFollowerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);

        // Get components
        m_curveComponent = m_actor->GetComponent<CurveComponent>(); // Mandatory
        ITF_ASSERT(m_curveComponent);

        // Init
        m_isTrig = bfalse;
 
    }

    //*****************************************************************************

    void CurveFollowerComponent:: onBecomeActive()
    {
        // Start auto if there not trigger.
        if (!m_isTrig)
        {
            start();
        }
    }
      
    //*****************************************************************************

    void CurveFollowerComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);

        if (EventActivate* trigger = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            if (!trigger->getActivated() && !m_isTrig)
            {
                m_isTrig = btrue;
            }
            else if (trigger->getActivated() && m_isTrig)
            {
                start();
            }
        }
    }
    //*****************************************************************************

    void CurveFollowerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_disabled || !m_playing)
        {
            return;
        }

        m_dist += getTemplate()->getSpeed() * _dt;

        if (m_dist > m_curveComponent->getDistCurveTotal())
        {
            if (getTemplate()->getLoop())
            {
                m_dist = 0.0f;
            }
            else
            {
                m_dist    -= getTemplate()->getSpeed() * _dt;
                m_playing = bfalse;
            }           
        }

        m_actor->setPos(m_curveComponent->getPosAtDist(m_dist));

    }

    //*****************************************************************************

    void CurveFollowerComponent::start()
    {
        m_dist = 0.0f;
        m_playing = btrue;
    }

    //*****************************************************************************

    void CurveFollowerComponent::setDisabled( bbool _value )
    {
        m_disabled = _value;
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(CurveFollowerComponent_Template)
    BEGIN_SERIALIZATION(CurveFollowerComponent_Template)
        SERIALIZE_MEMBER("speed",   m_speed);
        SERIALIZE_MEMBER("loop",    m_loop);
    END_SERIALIZATION()

    //*****************************************************************************

    CurveFollowerComponent_Template::CurveFollowerComponent_Template()
    : m_speed(1.0f)
    , m_loop(bfalse)
    {
        // none
    }

    //*****************************************************************************

    CurveFollowerComponent_Template::~CurveFollowerComponent_Template()
    {
        // none
    }

    //*****************************************************************************


};
