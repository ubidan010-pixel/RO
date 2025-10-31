#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_GroundAIControllerComponent.h"
#endif //_ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_GroundAIControllerComponent)
BEGIN_SERIALIZATION_CHILD(Ray_GroundAIControllerComponent)
END_SERIALIZATION()

Ray_GroundAIControllerComponent::Ray_GroundAIControllerComponent()
{
}

Ray_GroundAIControllerComponent::~Ray_GroundAIControllerComponent()
{
}



///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_GroundAIControllerComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_GroundAIControllerComponent_Template)

    

END_SERIALIZATION()

Ray_GroundAIControllerComponent_Template::Ray_GroundAIControllerComponent_Template()

{
}

Ray_GroundAIControllerComponent_Template::~Ray_GroundAIControllerComponent_Template()
{
}
}
