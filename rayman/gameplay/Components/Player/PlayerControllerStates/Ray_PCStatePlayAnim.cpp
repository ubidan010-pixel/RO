#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StatePlayAnim)

////////////////////
// Idle State
////////////////////
Ray_PlayerControllerComponent::StatePlayAnim::StatePlayAnim()
{
}

void Ray_PlayerControllerComponent::StatePlayAnim::onExit()
{
    Super::onExit();

    m_playAnim.invalidate();
}

bbool Ray_PlayerControllerComponent::StatePlayAnim::checkStateChange( f32 _dt )
{

    return bfalse;
}

}
