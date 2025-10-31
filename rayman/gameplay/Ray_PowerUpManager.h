#ifndef _ITF_RAY_POWERUPMANAGER_H_
#define _ITF_RAY_POWERUPMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

namespace ITF
{

class Ray_EventPowerUp;

//------------------------------------------------------------------------------
// Info for global power ups (unlockable abilities)
struct Ray_GlobalPowerUpData
{
    DECLARE_SERIALIZE();

public:
    Ray_GlobalPowerUpData() : m_enabled(bfalse) {}

    ITF_INLINE bbool isEnabled() const { return m_enabled; }
    ITF_INLINE void setEnabled(bbool _val) { m_enabled = _val; }

 private:
    bbool m_enabled;
};

//------------------------------------------------------------------------------
// Info for per-player power ups
struct Ray_PerPlayerPowerUpData
{
    DECLARE_SERIALIZE()

public:
    Ray_PerPlayerPowerUpData();
    Ray_PerPlayerPowerUpData(bbool _exclusive, u32 _ammo, f32 _lifetime);

    ITF_INLINE bbool isEnabled() const { return m_enabled; }
    ITF_INLINE void setEnabled(bbool _val);

    ITF_INLINE bbool isExclusive() const { return m_exclusive; }
    ITF_INLINE void setExclusive(bbool _val) { m_exclusive = _val; }

    ITF_INLINE u32 getAmmo() const { return m_ammo; }
    ITF_INLINE void useAmmo();
    ITF_INLINE u32 getMaxAmmo() const { return m_maxAmmo; }
    ITF_INLINE void setMaxAmmo(u32 _val) { m_ammo = m_maxAmmo = _val; }
    ITF_INLINE void refillAmmo() { m_ammo = m_maxAmmo; }

    ITF_INLINE f32 getLifetime() const { return m_lifetime; }
    ITF_INLINE void updateLifetime(f32 _dt);
    ITF_INLINE f32 getMaxLifetime() const { return m_maxLifetime; }
    ITF_INLINE void setMaxLifetime(f32 _val) { m_lifetime = m_maxLifetime = _val; }

private:
    bbool m_enabled;
    bbool m_exclusive;
    u32 m_ammo;
    u32 m_maxAmmo;
    f32 m_lifetime;
    f32 m_maxLifetime;
};

//------------------------------------------------------------------------------
// Power up manager
class Ray_PowerUpManager
{
    DECLARE_SERIALIZE()

public:
    Ray_PowerUpManager();

    void init( const class Ray_PowerUpManager_Template* _template );
    
    // enable/disable power-up through event
    void processPowerUp(const Ray_EventPowerUp* _powerUp, u32 _playerIndex);

    // global power-ups
    bbool isGlobal(const StringID& _id);
    bbool areAllEnabled() const;
    void enableAll();
    bbool isEnabled(const StringID& _id) const;
    void setEnabled(const StringID& _id, bbool _val);
    
    // per-player power ups
    bbool isPerPlayer(const StringID& _id);
    bbool isEnabled(const StringID& _id, u32 _playerIndex) const;
    void setEnabled(const StringID& _id, u32 _playerIndex, bbool _val);
    bbool isExclusive(const StringID& _id) const;
    void setExclusive(const StringID& _id, bbool _val);
    u32 getAmmo(const StringID& _id, u32 _playerIndex) const;
    void useAmmo(const StringID& _id, u32 _playerIndex);

    void update(f32 _dt);

    void resetVolatilePerPlayerData(bbool _superPunchesOnly = bfalse);

    void resetGlobalPowerups();

private:
    typedef ITF_MAP<StringID, Ray_GlobalPowerUpData> GlobalPowerUpsMap;
    typedef ITF_MAP<StringID, FixedArray<Ray_PerPlayerPowerUpData, s_maxPlayers> > PerPlayerPowerUpsMap;
    
    bbool updateHitPoints(u32 _playerIndex, bbool _enable) const;
    void serializePerPlayerData(CSerializerObject* _serializer, ESerializeFlags _flags);

    GlobalPowerUpsMap m_globalData; // global power-ups map
    PerPlayerPowerUpsMap m_playersData;  // per-player power-ups map
    bbool m_updateHitPointsNeeded[s_maxPlayers];
};

class Ray_PowerUpManager_Template
{
    DECLARE_SERIALIZE()

public:

    Ray_PowerUpManager_Template();
    ~Ray_PowerUpManager_Template();

    u32                 getSuperPunchBasicMaxAmmo() const { return m_superPunchBasicMaxAmmo; }
    u32                 getSuperPunchSeekingMaxAmmo() const { return m_superPunchSeekingMaxAmmo; }
    u32                 getSwarmRepellerMaxAmmo() const { return m_swarmRepellerMaxAmmo; }

private:

    u32                 m_superPunchBasicMaxAmmo;
    u32                 m_superPunchSeekingMaxAmmo;
    u32                 m_swarmRepellerMaxAmmo;
};

} // namespace ITF

#endif //_ITF_RAY_POWERUPMANAGER_H_
