#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WM_BASEABILITY_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_BaseAbility.h"
#endif //_ITF_RAY_WM_BASEABILITY_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_BaseAbility)
BEGIN_VALIDATE_OBJECT(Ray_WM_BaseAbility)
END_VALIDATE_OBJECT()

//------------------------------------------------------------------------------
Ray_WM_BaseAbility::Ray_WM_BaseAbility()
    : Super()
    , m_template(NULL)
    , m_controller(NULL)
    , m_actor(NULL)
    , m_active(bfalse)
    , m_playerIndex(U32_INVALID)
{
}

//------------------------------------------------------------------------------
Ray_WM_BaseAbility::~Ray_WM_BaseAbility()
{
}

//------------------------------------------------------------------------------
void Ray_WM_BaseAbility::onActorLoaded(
    Pickable::HotReloadType /*_hotReload*/,
    const Ray_WM_BaseAbility_Template* _template,
    Ray_WM_ControllerComponent* _controller,
    Actor* _actor)
{
    ITF_ASSERT(_template && _controller && _actor);

    m_template = _template;
    m_controller = _controller;
    m_actor= _actor;
}

//------------------------------------------------------------------------------
void Ray_WM_BaseAbility::onBecomeActive(u32 _playerIndex)
{
    ITF_ASSERT(!m_active);
    m_active = btrue;
    m_playerIndex = _playerIndex;
}

//------------------------------------------------------------------------------
void Ray_WM_BaseAbility::onPlayerIndexChange(u32 _newIndex)
{
    ITF_ASSERT(m_active);
    m_playerIndex = _newIndex;
}

//------------------------------------------------------------------------------
void Ray_WM_BaseAbility::onBecomeInactive()
{
    ITF_ASSERT(m_active);
    m_active = bfalse;
    m_playerIndex = U32_INVALID;
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WM_BaseAbility_Template)
BEGIN_SERIALIZATION(Ray_WM_BaseAbility_Template)
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_WM_BaseAbility_Template::Ray_WM_BaseAbility_Template()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_WM_BaseAbility_Template::~Ray_WM_BaseAbility_Template()
{
}

} // namespace ITF
