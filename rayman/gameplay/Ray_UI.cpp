#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_UI_H_
#include "rayman/gameplay/Ray_UI.h"
#endif //_ITF_RAY_UI_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

namespace ITF
{
#ifdef ITF_SUPPORT_CHEAT
    void DebugMenuEntryToggleFriendlyFire::select( u32 _player ) const
    {
        RAY_GAMEMANAGER->setAllowFriendlyFire( !RAY_GAMEMANAGER->getAllowFriendlyFire() );
    }

    const String DebugMenuEntryToggleFriendlyFire::getName(u32 _player) const
    {
        String name;name.setTextFormat(
            "Friendly fire %s", RAY_GAMEMANAGER->getAllowFriendlyFire() ? "ON" : "OFF");
        return name;
    }


    void DebugMenuEntryTogglePlayerCrush::select( u32 _player ) const
    {
        RAY_GAMEMANAGER->setAllowPlayerCrush( !RAY_GAMEMANAGER->getAllowPlayerCrush() );
    }

    const String DebugMenuEntryTogglePlayerCrush::getName(u32 _player) const
    {
        String name;name.setTextFormat(
            "Friendly crush %s", RAY_GAMEMANAGER->getAllowPlayerCrush() ? "ON" : "OFF");
        return name;
    }


    void DebugMenuEntryToggleNpcCrush::select( u32 _player ) const
    {
        RAY_GAMEMANAGER->setAllowNpcCrush( !RAY_GAMEMANAGER->getAllowNpcCrush() );
    }

    const String DebugMenuEntryToggleNpcCrush::getName(u32 _player) const
    {
        String name;name.setTextFormat(
            "NPC crush %s", RAY_GAMEMANAGER->getAllowNpcCrush() ? "ON" : "OFF");
        return name;
    }

    void DebugMenuEntryToggleAllPowerUps::select(u32 _player) const
    {
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        powerUpManager.enableAll();
    }

    const String DebugMenuEntryToggleAllPowerUps::getName(u32 _player) const
    {
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        bbool allGlobalPowerUpsEnabled = powerUpManager.areAllEnabled();
        String name;name.setTextFormat("All Global PowerUps: %s", allGlobalPowerUpsEnabled ? "ON" : "OFF");
        return name;
    }

    void DebugMenuEntryToggleShowHUDs::select(u32 _player) const
    {
        bbool wasON = Ray_PlayerHudScoreComponent::areAllHUDsAlwaysVisible();
        Ray_PlayerHudScoreComponent::setAllHUDsAlwaysVisible(!wasON);
    }

    const String DebugMenuEntryToggleShowHUDs::getName(u32 _player) const
    {
        String result;
        result.setTextFormat("%s : %s", StringToUTF8(m_name).get(), (Ray_PlayerHudScoreComponent::areAllHUDsAlwaysVisible()?"ON":"OFF"));
        return result;
    }

    
    DebugMenuEntryToggleSinglePowerUp::DebugMenuEntryToggleSinglePowerUp( const String & _name, const StringID& _powerUpID, const char* _text )
    : DebugMenuEntry(_name)
    , m_powerUpId(_powerUpID)
    , m_text(_text)
    {
    }

    void DebugMenuEntryToggleSinglePowerUp::select(u32 _player) const
    {
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        if (powerUpManager.isGlobal(m_powerUpId))
        {
            bbool powerUpEnabled = powerUpManager.isEnabled(m_powerUpId);
            powerUpManager.setEnabled(m_powerUpId, !powerUpEnabled);
        }
        else
        {
            bbool powerUpEnabled = powerUpManager.isEnabled(m_powerUpId, _player);
            powerUpManager.setEnabled(m_powerUpId, _player, !powerUpEnabled);
        }
    }

    const String DebugMenuEntryToggleSinglePowerUp::getName(u32 _player) const
    {
        bbool powerUpEnabled = bfalse;
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        if (powerUpManager.isGlobal(m_powerUpId))
        {
            powerUpEnabled = powerUpManager.isEnabled(m_powerUpId);
        }
        else
        {
            powerUpEnabled = powerUpManager.isEnabled(m_powerUpId, _player);
        }
        String name;
        name.setTextFormat("%s %s", m_text, powerUpEnabled ? "ON" : "OFF");
        return name;
    }

    DebugMenuEntryToggleDive::DebugMenuEntryToggleDive(const String & _name)
    : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_Dive,"Power-up Dive:")
    {
    }

    DebugMenuEntryToggleWalkOnWallsGlobal::DebugMenuEntryToggleWalkOnWallsGlobal(const String & _name)
    : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_WalkOnWallsGlobal,"Power-up Walk on Walls:")
    {
    }

    DebugMenuEntryToggleReduction::DebugMenuEntryToggleReduction(const String & _name)
    : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_Reduction,"Power-up Reduction:")
    {
    }

    DebugMenuEntryToggleHairlicopter::DebugMenuEntryToggleHairlicopter(const String & _name)
    : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_Helicopter,"Power-up Hairlicopter:")
    {
    }

    DebugMenuEntryToggleFight::DebugMenuEntryToggleFight(const String & _name)
        : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_Fight,"Power-up Fighting:")
    {
    }

    void DebugMenuEntryToggleHeartShieldExclusive::select(u32 _player) const
    {
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        bbool heartShieldExclusive = powerUpManager.isExclusive(Ray_PowerUp_HeartShield);
        powerUpManager.setExclusive(Ray_PowerUp_HeartShield, !heartShieldExclusive);
    }

    const String DebugMenuEntryToggleHeartShieldExclusive::getName(u32 _player) const
    {
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        bbool heartShieldExclusive = powerUpManager.isExclusive(Ray_PowerUp_HeartShield);
        String name;name.setTextFormat("Heart Shield Exclusive %s", heartShieldExclusive ? "ON" : "OFF");
        return name;
    }

    DebugMenuEntryToggleHeartShield::DebugMenuEntryToggleHeartShield(const String & _name)
        : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_HeartShield,"Power-up Heart Shield (per-player):")
    {
    }

    DebugMenuEntryToggleSuperPunchBasic::DebugMenuEntryToggleSuperPunchBasic(const String & _name)
        : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_SuperPunchBasic,"Power-up Super Punch Basic (per-player):")
    {
    }

    DebugMenuEntryToggleSuperPunchSeeking::DebugMenuEntryToggleSuperPunchSeeking(const String & _name)
        : DebugMenuEntryToggleSinglePowerUp(_name,Ray_PowerUp_SuperPunchSeeking,"Power-up Super Punch Seeking (per-player):")
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenuEntryAddMaxHP::select(u32 _player) const { static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(_player))->addMaxHitPoints(numHp);}

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenuEntryAddHP::select(u32 _player) const { static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(_player))->addHitPoints(numHp, btrue, btrue);}    

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenuEntryToggleInvincibility::select(u32 _player) const
    {
        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(_player));
        player->setInvincible(!player->getInvincible());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void DebugMenuEntryToggleInvincibilityAll::select(u32 _player) const
    {
        bbool allInvincible = getAllInvincible();
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i))->setInvincible(!allInvincible);
    }
    const String DebugMenuEntryToggleInvincibilityAll::getName(u32 _player) const
    {
        bbool allInvincible = getAllInvincible();
        String name;name.setTextFormat("Invincibility %s (all players)", allInvincible ? "ON" : "OFF");
        return name;
    }
    bbool DebugMenuEntryToggleInvincibilityAll::getAllInvincible() const
    {
        bbool allInvincible = btrue;
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            if (!static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i))->getInvincible())
                allInvincible = bfalse;
        return allInvincible;
    }

    void DebugMenuEntryToggleInvincibleEnemies::select( u32 _player ) const
    {
        CHEATMANAGER->setEnableInvincibleEnemies( !CHEATMANAGER->getEnableInvincibleEnemies() );
    }

    const String DebugMenuEntryToggleInvincibleEnemies::getName(u32 _player) const
    {
        String name;name.setTextFormat("Invincible enemies %s", CHEATMANAGER->getEnableInvincibleEnemies() ? "ON" : "OFF");
        return name;
    }

    void DebugMenuEntryAllWorldMap::select( u32 _player ) const
    {
        RAY_CHEATMANAGER->setAllWorldMap(!RAY_CHEATMANAGER->getAllWorldMap());
    }

    const String DebugMenuEntryAllWorldMap::getName(u32 _player) const
    {
        bbool on = RAY_CHEATMANAGER->getAllWorldMap();
        String name;name.setTextFormat("All world map %s", on ? "ON" : "OFF");
        return name;
    }

    void DebugMenuEntryAllSkullTeeth::select( u32 _player ) const
    {
        RAY_CHEATMANAGER->setAllSkullTeeth(!RAY_CHEATMANAGER->getAllSkullTeeth());
    }

    const String DebugMenuEntryAllSkullTeeth::getName(u32 _player) const
    {
        bbool on = RAY_CHEATMANAGER->getAllSkullTeeth();
        String name;name.setTextFormat("All skull teeth %s", on ? "ON" : "OFF");
        return name;
    }

    void DebugMenuEntryUnlockAllCostumes::select( u32 _player ) const
    {
        RAY_CHEATMANAGER->setCheatAllCostumesUnlocked(!RAY_CHEATMANAGER->getIsCheatAllCostumesUnlocked());
    }

    void DebugMenuEntryTriggerToothSequence::select( u32 _player ) const
    {
        RAY_CHEATMANAGER->setCheatTeethSequence(btrue);
    }

    void DebugMenuEntryEnableTimeAttack::select( u32 _player ) const
    {
        RAY_CHEATMANAGER->setCheatTimeAttack(btrue);
    }


	void DebugMenuEntryUnlockAllBoss::select( u32 _player ) const
	{
		RAY_GAMEMANAGER->setWMSpotState(s_ju_bossmaster, SPOT_STATE_COMPLETED);
		RAY_GAMEMANAGER->setWMSpotState(s_BossBird, SPOT_STATE_COMPLETED);
		RAY_GAMEMANAGER->setWMSpotState(s_OC_B3, SPOT_STATE_COMPLETED);
		RAY_GAMEMANAGER->setWMSpotState(s_FO_B3, SPOT_STATE_COMPLETED);

		RAY_GAMEMANAGER->setWMSpotState(s_JU_B, SPOT_STATE_COMPLETED);
		RAY_GAMEMANAGER->setWMSpotState(s_MU_B, SPOT_STATE_COMPLETED);
		RAY_GAMEMANAGER->setWMSpotState(s_OC_B, SPOT_STATE_COMPLETED);
		RAY_GAMEMANAGER->setWMSpotState(s_FO_B, SPOT_STATE_COMPLETED);
	}

	const String DebugMenuEntryUnlockAllBoss::getName(u32 _player) const
	{
		String name;
		name.setTextFormat("Unlock all Boss (+world B)");
		return name;
	}


#endif //ITF_SUPPORT_CHEAT
} // namespace ITF
