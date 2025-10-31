#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraModifierComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterCameraModifierComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterCameraModifierComponent)
        SERIALIZE_OBJECT( "camModifier",    m_camModifier );
        SERIALIZE_OBJECT( "transitionIN",   m_transitionIN );
    END_SERIALIZATION()

    Ray_ShooterCameraModifierComponent::Ray_ShooterCameraModifierComponent()
        : Super()
    {
    }

    Ray_ShooterCameraModifierComponent::~Ray_ShooterCameraModifierComponent()
    {
    }

    void Ray_ShooterCameraModifierComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
    }

    void Ray_ShooterCameraModifierComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);
        
        if ( EventTrigger* triggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if ( triggerEvt->getActivated() )
            {
                f32 depth = m_actor->getDepth();
                if ( SHOOTERCAMERA != NULL )
                {
                    ObjectRefList * children = LINKMANAGER->getChildren( m_actor->getRef() );
                    if ( ( children != NULL ) && ( children->size() > 0 ) )
                    {
                        BaseObject * baseobj = ((*children)[0]).getObject();
                        if ( baseobj != NULL )
                        {
                            Pickable * obj = baseobj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
                            if ( obj != NULL ) 
                            {
                                depth = obj->getDepth();
                            }
                        }
                    }

                    m_camModifier.m_depth = depth;
                    SHOOTERCAMERA->registerModifier( m_camModifier, m_transitionIN );
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterCameraModifierComponent_Template)
    BEGIN_SERIALIZATION(Ray_ShooterCameraModifierComponent_Template)
    END_SERIALIZATION()

    Ray_ShooterCameraModifierComponent_Template::Ray_ShooterCameraModifierComponent_Template()
    {
    }

} // namespace ITF