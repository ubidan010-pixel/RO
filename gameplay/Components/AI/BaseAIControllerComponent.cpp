#include "precompiled_gameplay.h"

#ifndef _ITF_BASEAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/BaseAIControllerComponent.h"
#endif //_ITF_BASEAICONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BaseAIControllerComponent)

BaseAIControllerComponent::BaseAIControllerComponent()
: m_phys(NULL)
, m_moveDir(Vec2d::Zero)
, m_moveDirThisFrame(Vec2d::Zero)
{
}

BaseAIControllerComponent::~BaseAIControllerComponent()
{
}

void BaseAIControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_phys = m_actor->GetComponent<PhysComponent>();
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(BaseAIControllerComponent_Template)

BaseAIControllerComponent_Template::BaseAIControllerComponent_Template()
{
}

BaseAIControllerComponent_Template::~BaseAIControllerComponent_Template()
{
}
}
