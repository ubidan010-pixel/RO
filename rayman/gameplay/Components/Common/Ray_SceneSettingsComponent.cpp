#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SCENESETTINGSCOMPONENT_H_
#include "rayman/gameplay/Components/Common/Ray_SceneSettingsComponent.h"
#endif //_ITF_RAY_SCENESETTINGSCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_SceneSettingsComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SceneSettingsComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_CONTAINER_OBJECT("players", m_players);
        END_CONDITION_BLOCK()

        SERIALIZE_FUNCTION(postLoadProperties,ESerialize_PropertyEdit_Load);

    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_SceneSettingsComponent,PlayerData)
        SERIALIZE_MEMBER("active", m_active);
        SERIALIZE_MEMBER("invincible", m_invincible);
        // http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-3431
        // for the moment I'll keep the serialized data just in case, but just hide it in the PropertyEditor
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_MEMBER("HP", m_HP);
            SERIALIZE_MEMBER("maxHP", m_maxHP);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    Ray_SceneSettingsComponent::Ray_SceneSettingsComponent()
    {
        m_players.resize(GAMEMANAGER->getMaxPlayerCount());
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            m_players[i] = PlayerData();
        }
        m_players[0].m_active = btrue;
    }

    Ray_SceneSettingsComponent::~Ray_SceneSettingsComponent()
    {
    }


    /***********************************/


    IMPLEMENT_OBJECT_RTTI(Ray_SceneSettingsComponent_Template)

    BEGIN_SERIALIZATION(Ray_SceneSettingsComponent_Template)
        SERIALIZE_MEMBER("maxPlayers", m_maxPlayers);
        SERIALIZE_MEMBER("allowFriendlyFire", m_allowFriendlyFire);
        SERIALIZE_MEMBER("allowPlayerCrush", m_allowPlayerCrush);
        SERIALIZE_MEMBER("allowNpcCrush", m_allowNpcCrush);
        SERIALIZE_MEMBER("invincibleEnemies", m_invincibleEnemies);
        SERIALIZE_MEMBER("enableCheatMode", m_enableCheatMode);
        SERIALIZE_MEMBER("powerupDive", m_powerupDive);
        SERIALIZE_MEMBER("powerupWalkOnWalls", m_powerupWalkOnWalls);
        SERIALIZE_MEMBER("powerupReduction", m_powerupReduction);
        SERIALIZE_MEMBER("powerupHelicopter", m_powerupHelicopter);
        SERIALIZE_MEMBER("powerupFight", m_powerupFight);
    END_SERIALIZATION()

    Ray_SceneSettingsComponent_Template::Ray_SceneSettingsComponent_Template()
    : m_maxPlayers(0)
    , m_allowFriendlyFire(bfalse)
    , m_allowPlayerCrush(bfalse)
    , m_allowNpcCrush(btrue)
    , m_invincibleEnemies(bfalse)
    , m_enableCheatMode(btrue)
    , m_powerupDive(bfalse)
    , m_powerupWalkOnWalls(bfalse)
    , m_powerupReduction(bfalse)
    , m_powerupHelicopter(bfalse)
    , m_powerupFight(bfalse)
    {
    }

    /***********************************/


    void Ray_SceneSettingsComponent::postLoadProperties()
    {
        applyInstanceSettings();
    }

    void Ray_SceneSettingsComponent::applyTemplateSettings()
    {

#ifdef ITF_DEMO
        RAY_GAMEMANAGER->setMaxPlayers(getTemplate()->getMaxPlayers());
#endif

        RAY_GAMEMANAGER->setAllowFriendlyFire(getTemplate()->getAllowFriendlyFire());
        RAY_GAMEMANAGER->setAllowPlayerCrush(getTemplate()->getAllowPlayerCrush());
        RAY_GAMEMANAGER->setAllowNpcCrush(getTemplate()->getAllowNpcCrush());
        RAY_GAMEMANAGER->setMaxPlayers(getTemplate()->getMaxPlayers());
        CHEATMANAGER->setActive(getTemplate()->getEnableCheatMode());
        CHEATMANAGER->setEnableInvincibleEnemies(getTemplate()->getInvincibleEnemies());
        
        if ( getTemplate()->getPowerupDive() )
        {
            enablePowerup(Ray_PowerUp_Dive,         getTemplate()->getPowerupDive());
        }

        if ( getTemplate()->getPowerupWalkOnWalls() )
        {
            enablePowerup(Ray_PowerUp_WalkOnWallsGlobal,  getTemplate()->getPowerupWalkOnWalls());
        }

        if ( getTemplate()->getPowerupReduction() )
        {
            enablePowerup(Ray_PowerUp_Reduction,    getTemplate()->getPowerupReduction());
        }

        if ( getTemplate()->getPowerupHelicopter() )
        {
            enablePowerup(Ray_PowerUp_Helicopter,   getTemplate()->getPowerupHelicopter());
        }

        if ( getTemplate()->getPowerupFight() )
        {
            enablePowerup(Ray_PowerUp_Fight,        getTemplate()->getPowerupFight());
        }
    }

    void Ray_SceneSettingsComponent::resetSettings()
    {
        RAY_GAMEMANAGER->setAllowFriendlyFire(btrue);
        RAY_GAMEMANAGER->setAllowPlayerCrush(bfalse);
        RAY_GAMEMANAGER->setAllowNpcCrush(btrue);
        RAY_GAMEMANAGER->setMaxPlayers(0);
        CHEATMANAGER->setActive(btrue);
        CHEATMANAGER->setEnableInvincibleEnemies(bfalse);
        enablePowerup(Ray_PowerUp_Dive,         btrue);
        enablePowerup(Ray_PowerUp_WalkOnWallsGlobal,  btrue);
        enablePowerup(Ray_PowerUp_Reduction,    btrue);
        enablePowerup(Ray_PowerUp_Helicopter,   btrue);
        enablePowerup(Ray_PowerUp_Fight,        btrue);
    }

    void Ray_SceneSettingsComponent::applyInstanceSettings()
    {
        const u32 playerCount = GAMEMANAGER->getMaxPlayerCount();
        const u32 dataCount = m_players.size();

        if ( dataCount < playerCount )
        {
            // grow to playerCount
            m_players.resize(playerCount);
            for (u32 i = dataCount; i < playerCount; i++)
            {
                m_players[i] = PlayerData();
            }
        }
        else if (dataCount > playerCount)
        {
            ITF_WARNING(m_actor, dataCount <= playerCount, "you've defined too many players!");
        }

        for (u32 i = 0; i < playerCount; i++)
        {
            PlayerData& data = m_players[i];
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            player->setInvincible(data.m_invincible);

            // http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-3431
            //if (data.m_HP)
            //{
            //    player->setHitPoints(data.m_HP);
            //}
            //if (data.m_maxHP)
            //{
            //    player->setMaxHitPoints(data.m_maxHP);
            //}
        }
    }

    void Ray_SceneSettingsComponent::enablePowerup(const StringID& _id, bbool _enabled )
    {
        // enable or disable global power-up
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        if (!powerUpManager.isGlobal(_id))
        {
            ITF_WARNING(NULL, bfalse, "Power-up %s isn't global", _id.getDebugString());
        }
        else
        {
            powerUpManager.setEnabled(_id, _enabled);
        }
    }
};