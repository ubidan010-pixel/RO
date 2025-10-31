#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_POWERUPDISPLAY_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_PowerUpDisplay.h"
#endif //_ITF_RAY_POWERUPDISPLAY_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_POWERUPMANAGER_H_
#include "rayman/gameplay/Ray_PowerUpManager.h"
#endif //_ITF_RAY_POWERUPMANAGER_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PowerUpDisplay)
BEGIN_VALIDATE_OBJECT(Ray_PowerUpDisplay)
    VALIDATE_PARAM("id", m_template->getId().isValid(), "Power-up id mandatory");
END_VALIDATE_OBJECT()

//------------------------------------------------------------------------------
Ray_PowerUpDisplay::Ray_PowerUpDisplay()
    : Super()
    , m_actor(NULL)
    , m_playerIndex(U32_INVALID)
    , m_template(NULL)
{
}

//------------------------------------------------------------------------------
void Ray_PowerUpDisplay::init(
    Actor * _actor,
    u32 _playerIndexparent,
    const Ray_PowerUpDisplay_Template* _template
    )
{
    ITF_ASSERT(_actor);
    ITF_ASSERT(_template);
    m_actor = _actor;
    m_playerIndex = _playerIndexparent;
    m_template = _template;
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpDisplay::isEnabled() const
{
    ITF_ASSERT(m_actor && m_template);

    // ease query from subclasses
    Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
    if (powerUpManager.isGlobal(m_template->getId()))
    {
        return powerUpManager.isEnabled(m_template->getId());
    }
    if (powerUpManager.isPerPlayer(m_template->getId()))
    {
        return powerUpManager.isEnabled(m_template->getId(), m_playerIndex);
    }
    ITF_WARNING(m_actor, bfalse, "Power-up not found");
    return bfalse;
}

//------------------------------------------------------------------------------
void Ray_PowerUpDisplay::setEnabled(bbool _val) const
{
    ITF_ASSERT(m_actor && m_template);

    // ease setup from subclasses
    Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
    if (powerUpManager.isGlobal(m_template->getId()))
    {
        powerUpManager.setEnabled(m_template->getId(), _val);
    }
    else if (powerUpManager.isPerPlayer(m_template->getId()))
    {
        powerUpManager.setEnabled(m_template->getId(), m_playerIndex, _val);
    }
    else
    {
        ITF_WARNING(m_actor, bfalse, "Power-up not found");
    }
}

//------------------------------------------------------------------------------
u32 Ray_PowerUpDisplay::getAmmo() const
{
    ITF_ASSERT(m_actor && m_template);

    // ease ammo retrieval from subclasses
    Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
    if (powerUpManager.isPerPlayer(m_template->getId()))
    {
        return powerUpManager.getAmmo(m_template->getId(), m_playerIndex);
    }
    ITF_WARNING(m_actor, bfalse, "Power-up not found or not per-player");
    return U32_INVALID;
}

//------------------------------------------------------------------------------
void Ray_PowerUpDisplay::useAmmo() const
{
    // ease ammo usage from subclasses
    Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
    if (powerUpManager.isPerPlayer(m_template->getId()))
    {
        powerUpManager.useAmmo(m_template->getId(), m_playerIndex);
    }
    else
    {
        ITF_WARNING(m_actor, bfalse, "Power-up not found or not per-player");
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PowerUpDisplay_Template)
BEGIN_SERIALIZATION(Ray_PowerUpDisplay_Template)
    SERIALIZE_MEMBER("id", m_id);
END_SERIALIZATION()

} // namespace ITF
