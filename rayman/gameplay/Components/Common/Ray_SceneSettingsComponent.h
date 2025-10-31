#ifndef _ITF_RAY_SCENESETTINGSCOMPONENT_H_
#define _ITF_RAY_SCENESETTINGSCOMPONENT_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

namespace ITF
{
    // Set player state, powerups, cheats etc, per-scene, for testing purposes.

    class Ray_SceneSettingsComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SceneSettingsComponent, ActorComponent,129898962)
        DECLARE_SERIALIZE();

    public:

        Ray_SceneSettingsComponent();
        virtual ~Ray_SceneSettingsComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        void                applyTemplateSettings();
        static void         resetSettings();

    private:

        const class Ray_SceneSettingsComponent_Template* getTemplate() const;
        void                postLoadProperties();
        void                applyInstanceSettings();

        struct PlayerData
        {
            DECLARE_SERIALIZE();

            PlayerData()
                : m_active(bfalse)
                , m_invincible(bfalse)
                , m_HP(0)       // 0 = don't change
                , m_maxHP(0)
            {
            }

            bbool   m_active;
            bbool   m_invincible;
            u32     m_HP;
            u32     m_maxHP;
        };

        static void enablePowerup(const StringID& _id, bbool _val);

        SafeArray<PlayerData> m_players;
    };

    class Ray_SceneSettingsComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SceneSettingsComponent_Template, TemplateActorComponent,1819284364);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SceneSettingsComponent);

    public:

        class Ray_CheatSettingsPlayerData
        {
            DECLARE_SERIALIZE();

        public:

            Ray_CheatSettingsPlayerData()
                : m_active(btrue)
                , m_invincible(bfalse)
                , m_HP(0)       // 0 = don't change
                , m_maxHP(0)
            {
            }

            bbool   getIsActive() const { return m_active; }
            bbool   getIsInvincible() const { return m_invincible; }
            u32     getHP() const { return m_HP; }
            u32     getMaxHP() const { return m_maxHP; }

        private:

            bbool   m_active;
            bbool   m_invincible;
            u32     m_HP;
            u32     m_maxHP;
        };

        Ray_SceneSettingsComponent_Template();
        virtual ~Ray_SceneSettingsComponent_Template() {}

        ITF_INLINE u32 getMaxPlayers() const { return m_maxPlayers; }
        ITF_INLINE bbool getAllowFriendlyFire() const { return m_allowFriendlyFire; }
        ITF_INLINE bbool getAllowPlayerCrush() const { return m_allowPlayerCrush; }
        ITF_INLINE bbool getAllowNpcCrush() const { return m_allowNpcCrush; }
        ITF_INLINE bbool getInvincibleEnemies() const { return m_invincibleEnemies; }
        ITF_INLINE bbool getEnableCheatMode() const { return m_enableCheatMode; }
        ITF_INLINE bbool getPowerupDive() const { return m_powerupDive; }
        ITF_INLINE bbool getPowerupWalkOnWalls() const { return m_powerupWalkOnWalls; }
        ITF_INLINE bbool getPowerupReduction() const { return m_powerupReduction; }
        ITF_INLINE bbool getPowerupHelicopter() const { return m_powerupHelicopter; }
        ITF_INLINE bbool getPowerupFight() const { return m_powerupFight; }

    private:

        u32     m_maxPlayers;
        bbool   m_allowFriendlyFire;
        bbool   m_allowPlayerCrush;
        bbool   m_allowNpcCrush;
        bbool   m_invincibleEnemies;
        bbool   m_enableCheatMode;
        bbool   m_powerupDive;
        bbool   m_powerupWalkOnWalls;
        bbool   m_powerupReduction;
        bbool   m_powerupHelicopter;
        bbool   m_powerupFight;
    };

    ITF_INLINE const Ray_SceneSettingsComponent_Template* Ray_SceneSettingsComponent::getTemplate() const
    {
        return static_cast<const Ray_SceneSettingsComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_SCENESETTINGSCOMPONENT_H_