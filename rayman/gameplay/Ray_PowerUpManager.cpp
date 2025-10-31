#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_POWERUPMANAGER_H_
#include "rayman/gameplay/Ray_PowerUpManager.h"
#endif //_ITF_RAY_POWERUPMANAGER_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

namespace ITF
{

//------------------------------------------------------------------------------
Ray_PerPlayerPowerUpData::Ray_PerPlayerPowerUpData()
    : m_enabled(bfalse)
    , m_exclusive(bfalse)
    , m_ammo(U32_INVALID)
    , m_maxAmmo(U32_INVALID)
    , m_lifetime(F32_INFINITY)
    , m_maxLifetime(F32_INFINITY)
{
}

//------------------------------------------------------------------------------
Ray_PerPlayerPowerUpData::Ray_PerPlayerPowerUpData(bbool _exclusive, u32 _ammo, f32 _lifetime)
    : m_enabled(bfalse)
    , m_exclusive(_exclusive)
    , m_ammo(_ammo)
    , m_maxAmmo(_ammo)
    , m_lifetime(_lifetime)
    , m_maxLifetime(_lifetime)
{
}

//------------------------------------------------------------------------------
ITF_INLINE void Ray_PerPlayerPowerUpData::setEnabled(bbool _val)
{
    // enable/disable
    m_enabled = _val;
    
    // reset ammo & lifetime
    m_ammo = m_maxAmmo;
    m_lifetime = m_maxLifetime;
}

//------------------------------------------------------------------------------
ITF_INLINE void Ray_PerPlayerPowerUpData::useAmmo()
{
    // use ammo, disable when out of it
    ITF_ASSERT(m_enabled && m_ammo>0);
    if ((m_ammo != U32_INVALID) &&
        (--m_ammo == 0))
    {
        setEnabled(bfalse);
    }
}

//------------------------------------------------------------------------------
ITF_INLINE void Ray_PerPlayerPowerUpData::updateLifetime(f32 _dt)
{
    // update, disable when timed out
    ITF_ASSERT(m_enabled && m_lifetime>=0.0f);
    if ((m_lifetime != F32_INFINITY) &&
        (m_lifetime -= _dt) <= 0.0f)
    {
        setEnabled(bfalse);
    }
}


BEGIN_SERIALIZATION(Ray_GlobalPowerUpData)

    SERIALIZE_MEMBER("enabled",m_enabled);

END_SERIALIZATION()

BEGIN_SERIALIZATION(Ray_PerPlayerPowerUpData)

    SERIALIZE_MEMBER("enabled",m_enabled);

END_SERIALIZATION()

BEGIN_SERIALIZATION(Ray_PowerUpManager)

    SERIALIZE_CONTAINER_OBJECT("globalData",m_globalData);
    serializePerPlayerData(serializer, flags);

END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_PowerUpManager::Ray_PowerUpManager()
{
    for (int i=0; i<s_maxPlayers; ++i)
    {
        m_updateHitPointsNeeded[i] = bfalse;
    }
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::init( const class Ray_PowerUpManager_Template* _template )
{
    // fill global power-ups map
    m_globalData[Ray_PowerUp_Dive] = Ray_GlobalPowerUpData();
    m_globalData[Ray_PowerUp_Reduction] = Ray_GlobalPowerUpData();
    m_globalData[Ray_PowerUp_Helicopter] = Ray_GlobalPowerUpData();
    m_globalData[Ray_PowerUp_Fight] = Ray_GlobalPowerUpData();
    m_globalData[Ray_PowerUp_WalkOnWallsGlobal] = Ray_GlobalPowerUpData();

    // fill per-player power-ups map
    for (u32 i=0; i<s_maxPlayers; ++i)
    {
        m_playersData[Ray_PowerUp_HeartShield].push_back(Ray_PerPlayerPowerUpData(bfalse, U32_INVALID, F32_INFINITY));
        m_playersData[Ray_PowerUp_SuperPunchBasic].push_back(Ray_PerPlayerPowerUpData(btrue, _template->getSuperPunchBasicMaxAmmo(), F32_INFINITY));
        m_playersData[Ray_PowerUp_SuperPunchSeeking].push_back(Ray_PerPlayerPowerUpData(btrue, _template->getSuperPunchSeekingMaxAmmo(), F32_INFINITY));
        m_playersData[Ray_PowerUp_SwarmRepeller].push_back(Ray_PerPlayerPowerUpData(bfalse, _template->getSwarmRepellerMaxAmmo(), F32_INFINITY));
    }
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::resetGlobalPowerups()
{
    GlobalPowerUpsMap::iterator it;
    for (it = m_globalData.begin(); it != m_globalData.end(); ++it)
    {
        it->second.setEnabled(bfalse);
    }
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::processPowerUp(const Ray_EventPowerUp* _powerUp, u32 _playerIndex)
{
    if (isGlobal(_powerUp->getId()))
    {
        setEnabled(_powerUp->getId(), _powerUp->enable());
    }
    else if (isPerPlayer(_powerUp->getId()))
    {
        setEnabled(_powerUp->getId(), _playerIndex, _powerUp->enable());
    }
    else
    {
        ITF_WARNING(
            NULL, bfalse,
            "Power-up %s not found",
            _powerUp->getId().getDebugString());
    }
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpManager::isGlobal(const StringID& _id)
{
    // search in global map
    GlobalPowerUpsMap::iterator it = m_globalData.find(_id);
    if (it == m_globalData.end())
    {
        return bfalse;
    }
    return btrue;
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpManager::areAllEnabled() const
{
    // if one global is disabled, return false (for cheats)
    GlobalPowerUpsMap::const_iterator it;
    for (it = m_globalData.begin(); it != m_globalData.end(); ++it)
    {
        if (!it->second.isEnabled())
        {
            return bfalse;
        }
    }
    return btrue;
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::enableAll()
{
    // enable all globals (for cheats)
    GlobalPowerUpsMap::iterator it;
    for (it = m_globalData.begin(); it != m_globalData.end(); ++it)
    {
        it->second.setEnabled(btrue);
    }
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpManager::isEnabled(const StringID& _id) const
{
    // find in global map
    GlobalPowerUpsMap::const_iterator it = m_globalData.find(_id);
    if (it == m_globalData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a global power-up: %s",
            _id.getDebugString());
        return bfalse;
    }

    // enabled or disabled?
    return it->second.isEnabled();
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::setEnabled(const StringID& _id, bbool _val)
{
    // find in global map
    GlobalPowerUpsMap::iterator it = m_globalData.find(_id);
    if (it == m_globalData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a global power-up: %s",
            _id.getDebugString());
        return;
    }
    
    // enable or disable
    it->second.setEnabled(_val);
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpManager::isPerPlayer(const StringID& _id)
{
    // search in per-player map
    PerPlayerPowerUpsMap::iterator pit = m_playersData.find(_id);
    if (pit == m_playersData.end())
    {
        return bfalse;
    }
    return btrue;
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpManager::isEnabled(const StringID& _id, u32 _playerIndex) const
{
    // find in per-player map
    PerPlayerPowerUpsMap::const_iterator it = m_playersData.find(_id);
    if (it == m_playersData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a per-player power-up: %s",
            _id.getDebugString());
        return bfalse;
    }

    // guard
    ITF_ASSERT(_playerIndex<s_maxPlayers);
    if (_playerIndex>=s_maxPlayers)
    {
        return bfalse;
    }

    // enabled or disabled?
    return it->second[_playerIndex].isEnabled();
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::setEnabled(const StringID& _id, u32 _playerIndex, bbool _val)
{
    // find in per-player map
    PerPlayerPowerUpsMap::iterator it = m_playersData.find(_id);
    if (it == m_playersData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a per-player power-up: %s",
            _id.getDebugString());
        return;
    }
    
    // guard
    ITF_ASSERT(_playerIndex<s_maxPlayers);
    if (_playerIndex>=s_maxPlayers)
    {
        return;
    }

    // enable or disable for player
    (it->second)[_playerIndex].setEnabled(_val);
    if (_id == Ray_PowerUp_HeartShield)
    {
        updateHitPoints(_playerIndex, _val);
    }
    else if(_id == Ray_PowerUp_SwarmRepeller)
    {
        (it->second)[_playerIndex].refillAmmo();
    }
    
    // if exclusive and just enabled, disable all other exclusives on player
    if ((it->second)[_playerIndex].isExclusive() && _val)
    {
        for (it = m_playersData.begin(); it != m_playersData.end(); ++it)
        {
            if (it->first != _id &&
                it->second[_playerIndex].isExclusive() &&
                it->second[_playerIndex].isEnabled())
            {
                it->second[_playerIndex].setEnabled(bfalse);
                if (it->first == Ray_PowerUp_HeartShield)
                {
                    updateHitPoints(_playerIndex, bfalse);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpManager::isExclusive(const StringID& _id) const
{
    // find in per-player map
    PerPlayerPowerUpsMap::const_iterator it = m_playersData.find(_id);
    if (it == m_playersData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a per-player power-up: %s",
            _id.getDebugString());
        return bfalse;
    }

    // NB[LaurentCou]: this is kept by player for handiness but conceptually
    // should be the name for all of them; return true if at least one is set
    for (int i=0; i<s_maxPlayers; ++i)
    {
        if (it->second[i].isExclusive())
        {
            return btrue;
        }
    }
    return bfalse;
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::setExclusive(const StringID& _id, bbool _val)
{
    // find in per-player map
    PerPlayerPowerUpsMap::iterator it = m_playersData.find(_id);
    if (it == m_playersData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a per-player power-up: %s",
            _id.getDebugString());
        return;
    }
    
    // set exclusive for all players
    for (int i=0; i<s_maxPlayers; ++i)
    {
        it->second[i].setExclusive(_val);
    }
}

//------------------------------------------------------------------------------
u32 Ray_PowerUpManager::getAmmo(const StringID& _id, u32 _playerIndex) const
{
    // find in per-player map
    PerPlayerPowerUpsMap::const_iterator it = m_playersData.find(_id);
    if (it == m_playersData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a per-player power-up: %s",
            _id.getDebugString());
        return U32_INVALID;
    }
    
    // guard
    ITF_ASSERT(_playerIndex<s_maxPlayers);
    if (_playerIndex>=s_maxPlayers)
    {
        return U32_INVALID;
    }

    // return ammo
    return (it->second)[_playerIndex].getAmmo();
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::useAmmo(const StringID& _id, u32 _playerIndex)
{
    // find in per-player map
    PerPlayerPowerUpsMap::iterator it = m_playersData.find(_id);
    if (it == m_playersData.end())
    {
        ITF_WARNING(
            NULL, bfalse, "Not a per-player power-up: %s",
            _id.getDebugString());
        return;
    }
    
    // guard
    ITF_ASSERT(_playerIndex<s_maxPlayers);
    if (_playerIndex>=s_maxPlayers)
    {
        return;
    }

    // use ammo for player
    (it->second)[_playerIndex].useAmmo();
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::update(f32 _dt)
{
    // update lifetime of all per-player power ups
    PerPlayerPowerUpsMap::iterator it;
    for (it = m_playersData.begin(); it != m_playersData.end(); ++it)
    {
        for (int i=0; i<s_maxPlayers; ++i)
        {
            if (it->second[i].isEnabled())
            {
                it->second[i].updateLifetime(_dt);
            }
        }
    }

    // delayed HP update if needed
    // it happens that the Player* is null on a checkpoint load, so the hit
    // points can't get updated directly (RO-10900)
    for (int i=0; i<s_maxPlayers; ++i)
    {
        if (m_updateHitPointsNeeded[i])
        {
            if (updateHitPoints(i, m_playersData[Ray_PowerUp_HeartShield][i].isEnabled()))
            {
                m_updateHitPointsNeeded[i] = bfalse;
            }
        }
    }

    // loose volatile per-player power-ups on locking sequence (RO-11293)
    // disable tutorials when a ("locking") sequence is playing (RO-10460)
    if (CINEMATICMANAGER->getLockPlayers())
    {
        resetVolatilePerPlayerData();
    }    
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::resetVolatilePerPlayerData(bbool _superPunchesOnly /*= bfalse*/)
{
    for (u32 i=0; i<s_maxPlayers; ++i)
    {
        m_playersData[Ray_PowerUp_SuperPunchBasic][i].setEnabled(bfalse);
        m_playersData[Ray_PowerUp_SuperPunchSeeking][i].setEnabled(bfalse);

        if (_superPunchesOnly)
        {
            continue;
        }

        m_playersData[Ray_PowerUp_SwarmRepeller][i].setEnabled(bfalse);
    }
}

//------------------------------------------------------------------------------
bbool Ray_PowerUpManager::updateHitPoints(u32 _playerIndex, bbool _enable) const
{
    // heart shields set hit points to the maximum
    // NB[LaurentCou]: a bit hacky bust the best solution without completely
    // wiping hit points out
    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(_playerIndex));
    ITF_ASSERT(player);
    if (player && player->getActive() && !player->isDead())
    {
        player->setHitPoints(_enable ? player->getMaxHitPoints() : 1);
        return btrue;
    }
    return bfalse;
}

//------------------------------------------------------------------------------
void Ray_PowerUpManager::serializePerPlayerData(CSerializerObject* _serializer, ESerializeFlags _flags)
{
    // heart shields are saved/reset on checkpoints
    for (u32 i=0; i<s_maxPlayers; ++i)
    {
        m_playersData[Ray_PowerUp_HeartShield][i].Serialize(_serializer, _flags);
        if (!updateHitPoints(i, m_playersData[Ray_PowerUp_HeartShield][i].isEnabled()))
        {
            m_updateHitPointsNeeded[i] = btrue;
        }
    }

    // other power ups are just disabled on checkpoint load
    if (0 != (_flags & ESerialize_Checkpoint_Load))
    {
        resetVolatilePerPlayerData();
    }
}

BEGIN_SERIALIZATION(Ray_PowerUpManager_Template)

    SERIALIZE_MEMBER("superPunchBasicMaxAmmo",m_superPunchBasicMaxAmmo);
    SERIALIZE_MEMBER("superPunchSeekingMaxAmmo",m_superPunchSeekingMaxAmmo);
    SERIALIZE_MEMBER("swarmRepellerMaxAmmo",m_swarmRepellerMaxAmmo);

END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_PowerUpManager_Template::Ray_PowerUpManager_Template()
: m_superPunchBasicMaxAmmo(3)
, m_superPunchSeekingMaxAmmo(3)
, m_swarmRepellerMaxAmmo(2)
{
}

Ray_PowerUpManager_Template::~Ray_PowerUpManager_Template()
{
}

} // namespace ITF

