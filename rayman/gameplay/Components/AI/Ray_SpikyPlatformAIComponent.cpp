#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SPIKYPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SpikyPlatformAIComponent.h"
#endif //_ITF_RAY_SPIKYPLATFORMAICOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_SpikyPlatformAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_SpikyPlatformAIComponent_Template)
        SERIALIZE_MEMBER("setDown", m_setDown);
        SERIALIZE_MEMBER("setUp", m_setUp);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_SpikyPlatformAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SpikyPlatformAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_SpikyPlatformAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_tweenComponent, "Ray_SpikyPlatformAIComponent requires a TweenComponent");
        VALIDATE_COMPONENT_PARAM("", m_polylineComponent, "Ray_SpikyPlatformAIComponent requires a PolylineComponent");
    END_VALIDATE_COMPONENT()

    Ray_SpikyPlatformAIComponent::Ray_SpikyPlatformAIComponent()
        : m_tweenComponent(NULL)
        , m_polylineComponent(NULL)
        , m_isUp(btrue)
    {
    }

    Ray_SpikyPlatformAIComponent::~Ray_SpikyPlatformAIComponent()
    {
    }


    void Ray_SpikyPlatformAIComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_tweenComponent = m_actor->GetComponent<TweenComponent>();
        m_polylineComponent = m_actor->GetComponent<PolylineComponent>();
    }

    void Ray_SpikyPlatformAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_tweenComponent->getCurrentSetIndex() != U32_INVALID)
        {
            // in transition
            return;
        }

        if (m_isUp)
        {
            if (m_polylineComponent->getUserCount() > 0)
            {
                // go down
                m_isUp = bfalse;
                m_tweenComponent->playSet(getTemplate()->getSetDown());
            }
        }
        else
        {
            if (m_polylineComponent->getUserCount() == 0)
            {
                // go back up
                m_isUp = btrue;
                m_tweenComponent->playSet(getTemplate()->getSetUp());
            }
        }
    }

};