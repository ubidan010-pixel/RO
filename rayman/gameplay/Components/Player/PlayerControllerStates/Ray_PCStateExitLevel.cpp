#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateExitLevel)

////////////////////
// Hanging To Idle State
////////////////////
void Ray_PlayerControllerComponent::StateExitLevel::onEnter()
{
    Super::onEnter();

    m_finished = bfalse;
}

void Ray_PlayerControllerComponent::StateExitLevel::onExit()
{
    Super::onExit();

}

void Ray_PlayerControllerComponent::StateExitLevel::update(f32 _dt)
{
    Super::update(_dt);

    if ( m_finished )
    {
        Ray_EventSequenceFinished finished;

        RAY_GAMEMANAGER->onEvent(&finished);
    }
}

void Ray_PlayerControllerComponent::StateExitLevel::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == MRK_Finish_Anim)
        {
            m_finished = btrue;
        }
    }
}

}