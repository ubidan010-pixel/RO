#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Misc/Ray_ShooterDoorComponent.h"
#endif //_ITF_RAY_SHOOTERDOORCOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_ShooterDoorComponent)

BEGIN_SERIALIZATION_CHILD(Ray_ShooterDoorComponent)
END_SERIALIZATION()

//-----------------------------------------------------------------------------------------------------------------------
Ray_ShooterDoorComponent::Ray_ShooterDoorComponent()
: Super()
{
}

Ray_ShooterDoorComponent::~Ray_ShooterDoorComponent()
{
}

//-----------------------------------------------------------------------------------------------------------------------
void Ray_ShooterDoorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153),this);
}

//-----------------------------------------------------------------------------------------------------------------------
void Ray_ShooterDoorComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( Ray_EventQueryOpen* opened = _event->DynamicCast<Ray_EventQueryOpen>(ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153)) )
    {
        opened->setOpen( ( m_currentPos == 1.f ) ? btrue : bfalse );
    }
}

//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(Ray_ShooterDoorComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_ShooterDoorComponent_Template)

END_SERIALIZATION()

}

