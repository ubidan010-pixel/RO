#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMSHOWCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_ShowComponent.h"
#endif //_ITF_RAYWMSHOWCOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_ShowComponent_Template)
    BEGIN_SERIALIZATION(Ray_WM_ShowComponent_Template)
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_ShowComponent)

    BEGIN_SERIALIZATION(Ray_WM_ShowComponent)
    END_SERIALIZATION()

    void Ray_WM_ShowComponent::onActorClearComponents()
    {
        if ( EVENTMANAGER )
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventWMShow,2102286749), this);
        }
    }

    void Ray_WM_ShowComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Register global event Ray_EventWMDisplayWorldRecap
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventWMShow,2102286749),this);

        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();
    }

    void Ray_WM_ShowComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        // Check display / hide recap event
        if ( Ray_EventWMShow* evShow = _event->DynamicCast<Ray_EventWMShow>(ITF_GET_STRINGID_CRC(Ray_EventWMShow,2102286749)) )
        {
           show(evShow->show());
        }
    }

    void Ray_WM_ShowComponent::show( bbool _show )
    {
        f32 alpha = 1.0f;
        if ( !_show )
            alpha = 0.0f;

        if ( m_animLightComponent )
            m_animLightComponent->setAlpha(alpha);
    }
};
