#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionNames.h"
#endif //_ITF_RAY_GAMEOPTIONNAMES_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_WORLDMANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLDMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_AABBPREFETCH_COMPONENT_H_
#include "gameplay/components/misc/AABBPrefetchComponent.h"
#endif //_ITF_AABBPREFETCH_COMPONENT_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_UI_H_
#include "rayman/gameplay/Ray_UI.h"
#endif //_ITF_RAY_UI_H_

#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_Gameplay.h"
#endif //_ITF_RAY_GAMESCREEN_GAMEPLAY_H_

#ifndef _ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_SplashScreens.h"
#endif //_ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_RAY_LEVELSTATS_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_LevelStats.h"
#endif //_ITF_RAY_LEVELSTATS_H_

#ifndef _ITF_RAY_CHECKPOINTSCORE_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_CheckpointScore.h"
#endif //_ITF_RAY_CHECKPOINTSCORE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_RAY_STARGATENODECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_StargateNodeComponent.h"
#endif //_ITF_RAY_STARGATENODECOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_RAY_CAGEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_CageAIComponent.h"
#endif //_ITF_RAY_CAGEAICOMPONENT_H_

#ifndef _ITF_RAY_AICAGEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Cage/Ray_AICageBehavior.h"
#endif //_ITF_RAY_AICAGEBEHAVIOR_H_

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_NETWORKSERVICES_
#include "engine/networkservices/NetworkServices.h"
#endif //_ITF_NETWORKSERVICES_

#ifndef _ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__
#include "rayman/gameplay/RichPresence/NetworkServices_PresenceMessages.h"
#endif //_ITF_NETWORKSERVICES_PRESENCEMESSAGES_H__

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_CHANGEROOMDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_ChangeRoomDoorComponent.h"
#endif //_ITF_RAY_CHANGEROOMDOORCOMPONENT_H_

#ifndef _ITF_ZINPUT_MANAGER_H
#include "engine/zinput/ZInputManager.h"
#endif //_ITF_ZINPUT_MANAGER_H

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_


#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_
#ifndef _ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMapLoading.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_RAY_AIRESCUEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AIRescueManager.h"
#endif //_ITF_RAY_AIRESCUEMANAGER_H_

//[RETAIL] TODO REMOVE FOR RETAIL GAME
#ifndef _ITF_FILEMANAGER_H_
#include "core/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef SERIALIZEROBJECTSIGNATURE_H
#include "core/serializer/ZSerializerObjectSignature.h"
#endif // SERIALIZEROBJECTSIGNATURE_H

#ifndef _ITF_RAY_UIFADESCREENCOMPONENT_H_
#include "rayman/gameplay/Ray_UIFadeScreenComponent.h"
#endif //_ITF_RAY_UIFADESCREENCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeManagerComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_

#ifndef _ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WMContentUnlockComponent.h"
#endif //_ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_

#ifndef _ITF_RAY_SKIPLEVELCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SkipLevelComponent.h"
#endif //_ITF_RAY_SKIPLEVELCOMPONENT_H_

#ifndef _ITF_RAY_TRCHELPER_H_
#include "rayman/gameplay/Ray_TRCHelper.h"
#endif //_ITF_RAY_TRCHELPER_H_

#ifndef _ITF_RAY_MURPHYASSISTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTAICOMPONENT_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"
#include <algorithm>

#define SAVEGAME_MAX_SIZE   65536 //Does not include extra storage data

#define SAVE_MAX_ELEC       246
#define SAVE_MAX_TOOTH      10

#define MAX_PREFETCH_TIMEOUT 10.f
#define MAX_FADETIME_FORICON 2.f

namespace ITF
{
#ifdef ITF_SUPPORT_CHEAT
    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryLoadMap : public DebugMenuEntry
    {
    public:
        DebugMenuEntryLoadMap(const String& _name) : DebugMenuEntry(_name)
        {
        }

        virtual void select(u32 _player) const
        {
#ifdef ITF_SUPPORT_EDITOR
            if (EDITOR->checkAndAskForUnsavedWork())
#endif //ITF_SUPPORT_EDITOR
            {
                RAY_GAMEMANAGER->loadGameplayMap(m_name, btrue, bfalse);
                UI_MENUMANAGER->setIsInDBGMenu(bfalse, U32_INVALID);
            }
        }

        virtual const String getName(u32 _player) const
        {
            return FilePath::getFilenameWithoutExtension(m_name);
        }
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryChangeActivation : public DebugMenuEntry
    {
    public:
        DebugMenuEntryChangeActivation(const String& _name)
            : DebugMenuEntry(_name)
        {
        }

        virtual void select(u32 _playerIndex) const
        {
            RAY_GAMEMANAGER->changePlayerActivation(_playerIndex, bfalse);
        }

    private:
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryEraseGameState : public DebugMenuEntry
    {
    public:
        DebugMenuEntryEraseGameState(const String& _name)
            : DebugMenuEntry(_name)
        {
        }

        virtual void select(u32 _playerIndex) const
        {
            RAY_GAMEMANAGER->clearWholeGameState();
            GAMEMANAGER->emptyCurrentLevelGameState();
            GAMEMANAGER->resetToLastCheckpointAndSave();
            RAY_GAMEMANAGER->saveGameState(btrue);
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class DebugMenuEntryRevertToLastCheckpoint : public DebugMenuEntry
    {
    public:
        DebugMenuEntryRevertToLastCheckpoint(const String& _name) : DebugMenuEntry(_name)
        {
        }

        virtual void select(u32 _player) const
        {
            RAY_GAMEMANAGER->resetToLastCheckpoint();
        }
    };
#endif //ITF_SUPPORT_CHEAT
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////

    Ray_Player::Ray_Player()
        : m_isLeaving(bfalse)
          , m_invincible(bfalse)
          , m_hitPoints(0)
          , m_maxHitPoints(5)
          , m_wasHitInThisLevel(bfalse)
          , m_sprintReleased(bfalse)
          , m_sprintDuration(0.0f)
          , m_heartTier(HeartTier_None)
          , m_consecutiveHits(0)
          , m_consecutiveHitTimer(0.0f)
    {
        reset();
    }

    void Ray_Player::reset()
    {
        if (m_hitPoints == 0)
        {
            setHitPoints(RAY_GAMEMANAGER->getStartHitPoints());
        }

        setMaxHitPoints(RAY_GAMEMANAGER->getStartMaxHitPoints());

        if (m_currentActorIndex != U32_INVALID)
        {
            Actor* actor = getActor();
            if (actor)
            {
                EventReset resetPlayer;
                actor->onEvent(&resetPlayer);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void Ray_Player::addHitPoints(i32 _numHP, bbool _evenIfInvincible /*= bfalse*/, bbool _broadcastEvent /*= bfalse*/)
    {
        RAY_GAMEMANAGER->addHitPoints(m_index, _numHP, _evenIfInvincible, _broadcastEvent);
    }

    void Ray_Player::resetConsecutiveHits()
    {
        m_consecutiveHits = 0;
        m_consecutiveHitTimer = 0.0f;
    }

    void Ray_Player::updateConsecutiveHitTimer(f32 _dt)
    {
        if (m_consecutiveHitTimer > 0.0f)
        {
            m_consecutiveHitTimer -= _dt;
            if (m_consecutiveHitTimer <= 0.0f)
            {
                resetConsecutiveHits();
            }
        }
    }

    void Ray_Player::onTakeDamage()
    {
        if (m_heartTier == HeartTier_Golden)
        {
            m_heartTier = HeartTier_Red;
            m_hitPoints = std::max(m_hitPoints, m_maxHitPoints);
        }
        else if (m_heartTier == HeartTier_Diamond)
        {
            if (m_consecutiveHits == 0)
            {
                m_consecutiveHitTimer = DIAMOND_HEART_CONSECUTIVE_HIT_WINDOW;
            }

            m_consecutiveHits++;
            m_consecutiveHits = Min(m_consecutiveHits, DIAMOND_HEART_MAX_CONSECUTIVE_HITS + 1);
            if (m_consecutiveHits >= DIAMOND_HEART_MAX_CONSECUTIVE_HITS)
            {
                setHitPoints(0);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void Ray_Player::addMaxHitPoints(i32 _numHP)
    {
        m_maxHitPoints = Clamp(m_maxHitPoints + _numHP, RAY_GAMEMANAGER->getMinHitPoints(),
                               RAY_GAMEMANAGER->getMaxHitPoints());
        m_hitPoints = Min(m_hitPoints, m_maxHitPoints);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_Player::revive()
    {
        EventPlayerHpChanged eventHP;
        eventHP.setPlayer(this);
        setHitPoints(RAY_GAMEMANAGER->getStartHitPointsAfterDeath());
        eventHP.setJustRevived(btrue);
        EVENTMANAGER->broadcastEvent(&eventHP);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_Player::heartNeeded() const
    {
        return getActive() && !isDead() && (getHitPoints() < getMaxHitPoints());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_Player::toggleDeadMode()
    {
        //if (getHitPoints() == 0)
        //{
        //    addHitPoints(RAY_GAMEMANAGER->getStartHitPointsAfterDeath(), btrue, btrue);
        //}
        //else
        //{
        //    addHitPoints(-static_cast<i32>(m_hitPoints), btrue, btrue);
        //}

        // set HP
        EventPlayerHpChanged eventHP;
        eventHP.setPlayer(this);
        if (isDead())
        {
            revive();
        }
        else
        {
            setHitPoints(0);
            eventHP.setJustDied(btrue);
            EVENTMANAGER->broadcastEvent(&eventHP);
        }
    }

    BEGIN_SERIALIZATION_SUBCLASS(Ray_GameManagerConfig_Template, MedalSlotConfig)
        SERIALIZE_ENUM_BEGIN("type", m_type);
        SERIALIZE_ENUM_VAR(Cage);
        SERIALIZE_ENUM_VAR(TimeAttack);
        SERIALIZE_ENUM_VAR(LumAttack);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("value", m_value);
        SERIALIZE_MEMBER("cupValue", m_cupValue);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_GameManagerConfig_Template, MissionConfig)
        SERIALIZE_MEMBER("type", m_type);
        SERIALIZE_CONTAINER_OBJECT("medalSlots", m_medalSlots);
        SERIALIZE_MEMBER("medalPath", m_medalPath);
        SERIALIZE_MEMBER("lumAttack3", m_lumAttack3);
    END_SERIALIZATION()


    BEGIN_SERIALIZATION_SUBCLASS(Ray_GameManagerConfig_Template, MapConfig)
        SERIALIZE_MEMBER("tag", m_tag);
        SERIALIZE_MEMBER("worldTag", m_worldTag);
        SERIALIZE_MEMBER("type", m_type);
        SERIALIZE_MEMBER("music", m_music);
        SERIALIZE_MEMBER("powerUnlocked", m_powerUnlocked);
        SERIALIZE_LEVEL_STATE("defaultState", m_defaultState);
        SERIALIZE_MEMBER("defaultDisplayName", m_defaultDisplayName);
        SERIALIZE_MEMBER("titleId", m_titleId);
        SERIALIZE_MEMBER("path", m_path);
        SERIALIZE_MEMBER("loadingScreen", m_loadingScreen);
        SERIALIZE_MEMBER("minTeeth", m_minTeeth);
        SERIALIZE_MEMBER("minElectoons", m_minElectoons);
        SERIALIZE_CONTAINER("unlock", m_unlock);
        SERIALIZE_CONTAINER("unlockedBy", m_unlockedBy);
        SERIALIZE_MEMBER("lastLevel", m_lastLevel);
        SERIALIZE_MEMBER("isDefaultLevel", m_isDefaultLevel);
        SERIALIZE_MEMBER("musicVolume", m_musicVolume);
        SERIALIZE_MEMBER("lumAttack1", m_lumAttack1);
        SERIALIZE_MEMBER("lumAttack2", m_lumAttack2);
        SERIALIZE_MEMBER("lumAttack3", m_lumAttack3);
        SERIALIZE_MEMBER("timeAttack1", m_timeAttack1);
        SERIALIZE_MEMBER("timeAttack2", m_timeAttack2);
        SERIALIZE_MEMBER("richPresenceIndex", m_richPresenceIndex);
        SERIALIZE_MEMBER("disableRewards", m_disableRewards);
        SERIALIZE_MEMBER("disableMenuToWM", m_disableMenuToWM);
        SERIALIZE_MEMBER("isSkippable", m_isSkippable);
        SERIALIZE_MEMBER("maxRelics", m_maxRelics);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_GameManagerConfig_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_GameManagerConfig_Template)
        SERIALIZE_MEMBER("playerMinHitPoints", m_minHitPoints);
        SERIALIZE_MEMBER("playerMaxHitPoints", m_maxHitPoints);
        SERIALIZE_MEMBER("playerStartHitPoints", m_startHitPoints);
        SERIALIZE_MEMBER("playerStartHitPointsAfterDeath", m_startHitPointsAfterDeath);
        SERIALIZE_MEMBER("playerStartMaxHitPoints", m_startMaxHitPoints);
        SERIALIZE_MEMBER("hiddenAreaWaitPlayersLength", m_hiddenAreaWaitPlayersLength);
        SERIALIZE_MEMBER("hiddenAreaWaitLoadLength", m_hiddenAreaWaitLoadLength);
        SERIALIZE_MEMBER("checkpointWaitPlayersLength", m_checkpointSeqWaitPlayersLength);
        SERIALIZE_MEMBER("lumPath", m_lumPath);
        SERIALIZE_MEMBER("lumSpawnRadius", m_lumSpawnRadius);
        SERIALIZE_MEMBER("heartPath", m_heartPath);
        SERIALIZE_MEMBER("defaultLumMusicManagerPath", m_defaultLumMusicManagerPath);
        SERIALIZE_MEMBER("electoonToothScore", m_electoonToothScore);
        SERIALIZE_MEMBER("debugSaves", m_debugSaves);

        // Stargate
        SERIALIZE_MEMBER("stargateWaitForPlayersTime", m_stargateWaitForPlayersTime);
        SERIALIZE_MEMBER("stargateWaitDistFromDoor", m_stargateWaitDistFromDoor);
        SERIALIZE_MEMBER("stargateWaitAngleOffset", m_stargateWaitAngleOffset);
        SERIALIZE_MEMBER("stargateWaitPointZOffset", m_stargateWaitPointZOffset);
        SERIALIZE_MEMBER("stargateStartToDoorTimeMultiplier", m_stargateStartToDoorTimeMultiplier);
        SERIALIZE_MEMBER("stargateCameraLookAtOffset", m_stargateCameraLookAtOffset);

        // Scale door
        SERIALIZE_MEMBER("scaleDoorWaitDelay", m_scaleDoorWaitDelay);
        SERIALIZE_MEMBER("scaleDoorMoveK", m_scaleDoorMoveK);
        SERIALIZE_MEMBER("scaleDoorMoveD", m_scaleDoorMoveD);

        // Bounce to layer
        SERIALIZE_MEMBER("bounceToLayerOffsetDistFromTarget", m_bounceToLayerOffsetDistFromTarget);
        SERIALIZE_MEMBER("bounceToLayerAngleOffset", m_bounceToLayerAngleOffset);

        // Throw teeth
        SERIALIZE_MEMBER("throwTeethSurpriseCameraOffset", m_throwTeethSurpriseCameraOffset);
        SERIALIZE_MEMBER("throwTeethThrowCameraOffset", m_throwTeethThrowCameraOffset);
        SERIALIZE_MEMBER("throwTeethPlayerOffset", m_throwTeethPlayerOffset);

        // Powerup data
        SERIALIZE_OBJECT("powerUps", m_powerUpManager);

        // WorldMap
        SERIALIZE_CONTAINER_OBJECT("levelsInfo", m_levelsInfo);
        SERIALIZE_CONTAINER_OBJECT("missionTypes", m_missionTypes);
        SERIALIZE_FUNCTION(fillLevelsInfo, ESerialize_Data_Load);

        // sprint tutorial
        SERIALIZE_MEMBER("sprintTutorialFailureCount", m_sprintTutorialFailureCount);
        SERIALIZE_MEMBER("sprintTutorialRequiredDuration", m_sprintTutorialRequiredDuration);

        // Fade types
        SERIALIZE_MEMBER("fadeDeath", m_fadeDeath);
        SERIALIZE_MEMBER("fadeChangePage", m_fadeChangePage);
        SERIALIZE_MEMBER("fadeHiddenArea", m_fadeHiddenArea);
        SERIALIZE_MEMBER("fadeTeleport", m_fadeTeleport);
        SERIALIZE_MEMBER("fadeCostume", m_fadeCostume);
        SERIALIZE_MEMBER("fadeLoadMap", m_fadeLoadMap);
        SERIALIZE_MEMBER("fadeChangePageWithAnim", m_fadeChangePageWithAnim);
        SERIALIZE_MEMBER("fadeWorldMapTeleport", m_fadeWorldMapTeleport);
        SERIALIZE_MEMBER("fadeFlash", m_fadeFlash);
        SERIALIZE_MEMBER("fadePrologue", m_fadePrologue);

        // initial node for new game
        SERIALIZE_MEMBER("wmStartNode", m_wmStartNode);

        SERIALIZE_MEMBER("loadingMinFrames", m_loadingMinFrames);
        // Skip
        SERIALIZE_MEMBER("nbDeathBeforeFirstSkip", m_nbDeathBeforeFirstSkip);
        SERIALIZE_MEMBER("nbDeathBeforeSecondSkip", m_nbDeathBeforeSecondSkip);

        // End sequence
        SERIALIZE_MEMBER("endSequencePlayerPosition", m_endSequencePlayerPosition);

        //Tarik::Fresco
        SERIALIZE_CONTAINER("frescoGamePlayProgressionInfo", m_frescoGamePlayProgressionInfo);

        //    SERIALIZE_FUNCTION(postLoadInit, ESerialize_Data_Load);
    END_SERIALIZATION()

#ifndef ITF_FINAL
    void Ray_GameManagerConfig_Template::checkUnlockConsistency()
    {
        ITF_MAP<StringID, ITF_VECTOR<StringID>>::iterator it = m_unlockedByList.begin();
        ITF_MAP<StringID, ITF_VECTOR<StringID>>::iterator it_end = m_unlockedByList.end();

        for (; it != it_end; it++)
        {
            for (u32 j = 0; j < it->second.size(); j++)
            {
                // Checking if the current level needs to be unlocked by itself
                ITF_ASSERT_MSG(it->first != it->second[j], "Impossible unlock rule : check %s unlock rules !",
                               it->first.getDebugString());

                // Check if level that unlocks the current level are not unlocked by the curent level
                ITF_MAP<StringID, ITF_VECTOR<StringID>>::iterator it_testedLevel = m_unlockedByList.find(it->second[j]);
                if (it_testedLevel != it_end)
                {
                    for (u32 k = 0; k < it_testedLevel->second.size(); k++)
                    {
                        ITF_ASSERT_MSG(it_testedLevel->second[k] != it->first,
                                       "Impossible unlock rule : check %s and %s unlock rules !",
                                       it->first.getDebugString(), it->second[j].getDebugString());
                    }
                }
            }
        }
    }
#endif //ITF_FINAL

    void Ray_GameManagerConfig_Template::fillLevelsInfo()
    {
        for (u32 i = 0; i < m_levelsInfo.size(); ++i)
        {
            ITF_MAP<StringID, u32>::iterator it = m_levelsInfoMap.find(m_levelsInfo[i].m_tag);
            ITF_WARNING_CATEGORY(GPP, NULL, it == m_levelsInfoMap.end(),
                                 "%s is defined more than one time, remove them in mission.ilu file",
                                 m_levelsInfo[i].m_tag.getDebugString());
            if (it == m_levelsInfoMap.end())
            {
                m_levelsInfoMap[m_levelsInfo[i].m_tag] = i;

                // Fill array of levels unlocked by others
                for (u32 j = 0; j < m_levelsInfo[i].m_unlock.size(); j++)
                {
                    if (m_levelsInfo[i].m_unlock[j].isValid())
                        m_unlockedByList[m_levelsInfo[i].m_unlock[j]].push_back(m_levelsInfo[i].m_tag);
                }

                for (u32 j = 0; j < m_levelsInfo[i].m_unlockedBy.size(); j++)
                {
                    if (m_levelsInfo[i].m_unlockedBy[j].isValid())
                        m_unlockedByList[m_levelsInfo[i].m_tag].push_back(m_levelsInfo[i].m_unlockedBy[j]);
                }
            }
        }
#ifndef ITF_FINAL
        checkUnlockConsistency();
#endif //ITF_FINAL
    }

    Ray_GameManagerConfig_Template::Ray_GameManagerConfig_Template()
        : m_minHitPoints(1)
          , m_maxHitPoints(12)
          , m_startHitPoints(5)
          , m_startHitPointsAfterDeath(3)
          , m_startMaxHitPoints(8)
          , m_hiddenAreaWaitPlayersLength(3.f)
          , m_hiddenAreaWaitLoadLength(1.f)
          , m_checkpointSeqWaitPlayersLength(3.f)
          , m_lumSpawnRadius(1.0f)
          , m_stargateWaitForPlayersTime(2.f)
          , m_stargateWaitDistFromDoor(4.f)
          , m_stargateWaitAngleOffset(0.3f)
          , m_stargateStartToDoorTimeMultiplier(3.f)
          , m_stargateWaitPointZOffset(3.f)
          , m_bounceToLayerOffsetDistFromTarget(2.f)
          , m_bounceToLayerAngleOffset(0.3f)
          , m_throwTeethSurpriseCameraOffset(Vec3d::Zero)
          , m_throwTeethThrowCameraOffset(Vec3d::Zero)
          , m_throwTeethPlayerOffset(7.f, 1.5f)
          , m_debugSaves(bfalse)
          , m_stargateCameraLookAtOffset(0.f, 0.f, 10.f)
          , m_scaleDoorWaitDelay(2.f)
          , m_scaleDoorMoveK(100.f)
          , m_scaleDoorMoveD(20.f)
          , m_sprintTutorialFailureCount(5)
          , m_sprintTutorialRequiredDuration(2.0f)
          , m_fadeDeath(s_FadeTypeNormalBlack)
          , m_fadeChangePage(s_FadeTypeNormalBlack)
          , m_fadeHiddenArea(s_FadeTypeNormalBlack)
          , m_fadeTeleport(s_FadeTypeNormalBlack)
          , m_fadeCostume(s_FadeTypeNormalBlack)
          , m_fadeLoadMap(s_FadeTypeNormalBlack)
          , m_fadeChangePageWithAnim(s_FadeTypeNormalBlack)
          , m_fadeWorldMapTeleport(s_FadeTypeNormalBlack)
          , m_nbDeathBeforeFirstSkip(8)
          , m_nbDeathBeforeSecondSkip(4)
          , m_loadingMinFrames(0)
          , m_endSequencePlayerPosition(0.f, 0.f, 0.f)
    {
    }

    Ray_GameManagerConfig_Template::Ray_GameManagerConfig_Template(const Path& _path)
        : Super(_path)
          , m_minHitPoints(1)
          , m_maxHitPoints(12)
          , m_startHitPoints(5)
          , m_startHitPointsAfterDeath(3)
          , m_startMaxHitPoints(8)
          , m_hiddenAreaWaitPlayersLength(3.f)
          , m_hiddenAreaWaitLoadLength(1.f)
          , m_checkpointSeqWaitPlayersLength(3.f)
          , m_stargateWaitForPlayersTime(2.f)
          , m_stargateWaitDistFromDoor(4.f)
          , m_stargateWaitAngleOffset(0.3f)
          , m_stargateStartToDoorTimeMultiplier(3.f)
          , m_stargateWaitPointZOffset(3.f)
          , m_bounceToLayerOffsetDistFromTarget(2.f)
          , m_bounceToLayerAngleOffset(0.3f)
          , m_throwTeethSurpriseCameraOffset(Vec3d::Zero)
          , m_throwTeethThrowCameraOffset(Vec3d::Zero)
          , m_throwTeethPlayerOffset(7.f, 1.5f)
          , m_debugSaves(bfalse)
          , m_stargateCameraLookAtOffset(0.f, 0.f, 10.f)
          , m_scaleDoorWaitDelay(2.f)
          , m_scaleDoorMoveK(100.f)
          , m_scaleDoorMoveD(20.f)
          , m_sprintTutorialFailureCount(5)
          , m_sprintTutorialRequiredDuration(2.0f)
          , m_fadeDeath(s_FadeTypeNormalBlack)
          , m_fadeChangePage(s_FadeTypeNormalBlack)
          , m_fadeHiddenArea(s_FadeTypeNormalBlack)
          , m_fadeTeleport(s_FadeTypeNormalBlack)
          , m_fadeCostume(s_FadeTypeNormalBlack)
          , m_fadeLoadMap(s_FadeTypeNormalBlack)
          , m_fadeChangePageWithAnim(s_FadeTypeNormalBlack)
          , m_fadeWorldMapTeleport(s_FadeTypeNormalBlack)
          , m_nbDeathBeforeFirstSkip(8)
          , m_nbDeathBeforeSecondSkip(4)
          , m_endSequencePlayerPosition(0.f, 0.f, 0.f)
    {
    }

    const Path& Ray_GameManagerConfig_Template::getLevelPath(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_path;
        }

        return Path::EmptyPath;
    }

    const Path& Ray_GameManagerConfig_Template::getLoadingScreen(const StringID& _tag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_tag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_loadingScreen;
        }

        return Path::EmptyPath;
    }

    void Ray_GameManagerConfig_Template::getLoadingScreens(ITF_VECTOR<String>& _screens) const
    {
        PathString_t pathName;

        _screens.clear();
        for (u32 i = 0; i < m_levelsInfo.size(); ++i)
            if (!m_levelsInfo[i].m_loadingScreen.isEmpty())
            {
                m_levelsInfo[i].m_loadingScreen.getString(pathName);

                _screens.push_back(pathName);
            }
    }

    const bbool Ray_GameManagerConfig_Template::isOfficialMap(const Path& _map) const
    {
        bbool isOfficial = bfalse;
        for (u32 i = 0; i < m_levelsInfo.size() && !isOfficial; i++)
        {
            isOfficial = m_levelsInfo[i].m_path.isEmpty() ? bfalse : m_levelsInfo[i].m_path == _map;
        }

        return isOfficial;
    }

    const StringID& Ray_GameManagerConfig_Template::getLevelTag(const Path& _path) const
    {
        if (_path.isEmpty())
            return StringID::Invalid;

        ITF_VECTOR<MapConfig>::const_iterator it = m_levelsInfo.begin();
        ITF_VECTOR<MapConfig>::const_iterator it_end = m_levelsInfo.end();

        while (it != m_levelsInfo.end())
        {
            if (it->m_path == _path)
                return it->m_tag;
            ++it;
        }

        return StringID::Invalid;
    }

    const Ray_GameManagerConfig_Template::MapConfig* Ray_GameManagerConfig_Template::getMapConfig(
        const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return &m_levelsInfo[it->second];
        }
        return NULL;
    }

    const StringID& Ray_GameManagerConfig_Template::getMapConfigType(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_type;
        }

        return StringID::Invalid;
    }

    const StringID& Ray_GameManagerConfig_Template::getLevelWorldTag(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_worldTag;
        }

        return StringID::Invalid;
    }

    const String8& Ray_GameManagerConfig_Template::getDefaultDisplayName(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_defaultDisplayName;
        }

        return String8::emptyString;
    }

    const LocalisationId Ray_GameManagerConfig_Template::getTitleId(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_titleId;
        }

        return LocalisationId::Invalid;
    }

    const StringID& Ray_GameManagerConfig_Template::getMapConfigMusic(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_music;
        }

        return StringID::Invalid;
    }

    const u32 Ray_GameManagerConfig_Template::getMinTeeth(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_minTeeth;
        }

        return 0;
    }

    const u32 Ray_GameManagerConfig_Template::getMinElectoons(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_minElectoons;
        }

        return 0;
    }

    const u32 Ray_GameManagerConfig_Template::getMaxRelics(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_maxRelics;
        }

        return 0;
    }

    ESpot_State Ray_GameManagerConfig_Template::getDefaultState(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_defaultState;
        }

        return SPOT_STATE_CLOSED;
    }

    const bbool Ray_GameManagerConfig_Template::isLastLevel(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_lastLevel;
        }

        return bfalse;
    }

    const bbool Ray_GameManagerConfig_Template::isSkippable(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_isSkippable;
        }

        return btrue;
    }

    const StringID& Ray_GameManagerConfig_Template::getDefaultLevelTag() const
    {
        for (u32 i = 0; i < m_levelsInfo.size(); i++)
        {
            if (m_levelsInfo[i].m_isDefaultLevel)
                return m_levelsInfo[i].m_tag;
        }

        return StringID::Invalid;
    }

    const f32 Ray_GameManagerConfig_Template::getMusicVolume(const StringID& _levelTag) const
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_levelsInfoMap.find(_levelTag);
        if (it != m_levelsInfoMap.end())
        {
            return m_levelsInfo[it->second].m_musicVolume.ratio();
        }

        return 1.0f;
    }

    const Ray_GameManagerConfig_Template::MissionConfig* Ray_GameManager::findMissionConfigByType(
        const StringID& _type, bbool _useDefaultWhenNone) const
    {
        const ITF_VECTOR<Ray_GameManagerConfig_Template::MissionConfig>& missions = m_configTemplate->
            getMissionConfigs();
        ITF_VECTOR<Ray_GameManagerConfig_Template::MissionConfig>::const_iterator iterator;

        for (iterator = missions.begin(); iterator != missions.end(); iterator++)
        {
            const Ray_GameManagerConfig_Template::MissionConfig& missionConfig = *iterator;
            if (missionConfig.m_type == _type)
            {
                return &missionConfig;
            }
        }

        if (_useDefaultWhenNone)
        {
            return RAY_GAMEMANAGER->findMissionConfigByType(s_MapConfig_6cages, bfalse);
            //This is for compatibility, in case mission is undefined
        }

        return NULL;
    }

    void Ray_GameManagerConfig_Template::getMissionMaps(ITF_VECTOR<String>& maps) const
    {
        PathString_t pathName;

        maps.clear();
        for (u32 i = 0; i < m_levelsInfo.size(); ++i)
            if (!m_levelsInfo[i].m_path.isEmpty())
            {
                m_levelsInfo[i].m_path.getString(pathName);
                maps.push_back(pathName);
            }
    }

    bbool Ray_GameManager::isMenuToWMEnable(const StringID& _levelTag)
    {
        const Ray_GameManagerConfig_Template::MapConfig* mapConfig = RAY_GAMEMANAGER->getMapConfig(_levelTag);
        if (!mapConfig)
            return btrue;

        return (!mapConfig->m_disableMenuToWM);
    }

    u32 Ray_GameManager::getRichPresenceIndex(const StringID& _levelTag)
    {
        const Ray_GameManagerConfig_Template::MapConfig* mapConfig = RAY_GAMEMANAGER->getMapConfig(_levelTag);
        if (!mapConfig)
            return 0;

        return mapConfig->m_richPresenceIndex;
    }

    i32 Ray_GameManager::getTimeAttackThreshold(const StringID& _levelTag, bbool _forCup)
    {
        i32 res = -1;

        const Ray_GameManagerConfig_Template::MapConfig* mapConfig = RAY_GAMEMANAGER->getMapConfig(_levelTag);
        if (!mapConfig)
            return -1;

        //
        if (_forCup)
        {
            if (mapConfig->m_timeAttack2 >= 0)
            {
                return mapConfig->m_timeAttack2;
            }
        }
        else
        {
            if (mapConfig->m_timeAttack1 >= 0)
            {
                return mapConfig->m_timeAttack1;
            }
        }

        //
        StringID missionType = mapConfig->m_type;
        const Ray_GameManagerConfig_Template::MissionConfig* config = RAY_GAMEMANAGER->findMissionConfigByType(
            missionType, btrue);

        if (config)
        {
            const ITF_VECTOR<Ray_GameManagerConfig_Template::MedalSlotConfig>& slots = config->m_medalSlots;
            ITF_VECTOR<Ray_GameManagerConfig_Template::MedalSlotConfig>::const_iterator iter;
            for (iter = slots.begin(); iter != slots.end(); iter++)
            {
                const Ray_GameManagerConfig_Template::MedalSlotConfig& slot = *iter;
                if (slot.m_type == Ray_GameManagerConfig_Template::MedalSlotConfig::TimeAttack)
                {
                    if (_forCup)
                        res = slot.m_cupValue;
                    else
                        res = slot.m_value;
                    break;
                }
            }
        }
        return res;
    }

    i32 Ray_GameManager::getLumAttackThreshold(const StringID& _levelTag, u32 _lumAttackIndex/*0 or 1*/)
    {
        i32 res = I32_MAX;
        const Ray_GameManagerConfig_Template::MapConfig* mapConfig = RAY_GAMEMANAGER->getMapConfig(_levelTag);
        if (!mapConfig)
            return -1;

        i32 instanceValue = -1;
        switch (_lumAttackIndex)
        {
        case 0: instanceValue = mapConfig->m_lumAttack1;
            break;
        case 1: instanceValue = mapConfig->m_lumAttack2;
            break;
        case 2: instanceValue = mapConfig->m_lumAttack3;
            break;
        }

        if (instanceValue >= 0)
        {
            return instanceValue;
        }

        const Ray_GameManagerConfig_Template::MissionConfig* config = RAY_GAMEMANAGER->findMissionConfigByType(
            mapConfig->m_type, btrue);

        if (config)
        {
            if (_lumAttackIndex == 2)
                return config->m_lumAttack3;

            u32 currentIndex = 0;
            const ITF_VECTOR<Ray_GameManagerConfig_Template::MedalSlotConfig>& slots = config->m_medalSlots;
            ITF_VECTOR<Ray_GameManagerConfig_Template::MedalSlotConfig>::const_iterator iter;
            for (iter = slots.begin(); iter != slots.end(); iter++)
            {
                const Ray_GameManagerConfig_Template::MedalSlotConfig& slot = *iter;
                if (slot.m_type == Ray_GameManagerConfig_Template::MedalSlotConfig::LumAttack)
                {
                    if (currentIndex == _lumAttackIndex)
                    {
                        res = slot.m_value;
                        break;
                    }

                    currentIndex++;
                }
            }
        }
        return res;
    }

    BEGIN_SERIALIZATION_CHILD(Ray_GameManager)

        SERIALIZE_OBJECT("powerupManager", m_powerUpManager);

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("playersCurrentScale", m_playersCurrentScale);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

    Ray_GameManager::Ray_GameManager()
        : Super()
          , m_configTemplate(NULL)
          , m_totalLums(0)
          , m_totalTuns(0)
          , m_wasPostLoadInitCalled(bfalse)
          , m_requestedGameplayPause(GameplayPause_NoChange)
          , m_allowFriendlyFire(btrue)
          , m_allowPlayerCrush(bfalse)
          , m_allowNpcCrush(btrue)
          , m_powerUpManager()
          , m_pauseType(0)
          , m_maxPlayers(0)
          , m_hideJoinMessage(0)
          , m_changeRoomDoor(ITF_INVALID_OBJREF)
          , m_costumeManager(ITF_INVALID_OBJREF)
          , m_isTimeAttack(bfalse)
          , m_isTimeAttackFinished(bfalse)
          , m_triggerTimeAttackEndSequence(bfalse)
          , m_timeAttackTimer(0.f)
          , m_timeAttackActorTimer(ITF_INVALID_OBJREF)
          , m_timeAttackActorFlag(ITF_INVALID_OBJREF)
          , m_timeAttackActorResults(ITF_INVALID_OBJREF)
          , m_timeAttackActorChrono(ITF_INVALID_OBJREF)
          , m_worldMapUnlockMsgActor(ITF_INVALID_OBJREF)
          , m_skipLevelActor(ITF_INVALID_OBJREF)
          , m_loadNotificationActor(ITF_INVALID_OBJREF)
          , m_showingLoadNotificationActor(bfalse)
          , m_playersCurrentScale(1.f)
          , m_sprintTutorialRegistered(bfalse)
          , m_sprintTutorialFailureCount(0)
          , m_currentSlotIndex(0)
          , m_uiFadeActorRef(ITF_INVALID_OBJREF)
          , m_deathCounterOnSameCheckpoint(U32_INVALID)
          , m_deathCounterCheckpointRef(ObjectRef::InvalidRef)
          , m_hasRejectedSkip(bfalse)
          , m_preloadedPrologueReady(bfalse)
          , m_gameOptionPersistence(NULL)
          , m_trcHelper(NULL)
    {
        ITF_MemSet(m_lastPadType, U32_INVALID, sizeof(m_lastPadType));

        m_gameManagerType = "Ray_GameManager";
        m_persistentGameData = newAlloc(mId_Gameplay, Ray_PersistentGameData_Universe);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventGoToWorldMap,3334813547), this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventTakeTooth,2760419748), this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventWorldMapUnlock,1931876316), this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventEndSequence,3991450529), this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventFxScreen,362679351), this);
    }


    void Ray_GameManager::destroyInternal()
    {
        clearWholeGameState();

        Super::destroyInternal();

        ITF_ASSERT(!m_spawnedRewardResourceGroup.isValidResourceId());
        SF_DEL(m_persistentGameData);

        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventGoToWorldMap, 3334813547), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventTakeTooth, 2760419748), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventWorldMapUnlock, 1931876316), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventEndSequence, 3991450529), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventFxScreen, 362679351), this);

        SF_DEL(m_gameOptionPersistence);
        SF_DEL(m_trcHelper);
        if (TRC_ADAPTER)
            TRC_ADAPTER->unRegisterTRCHelper();
    }

    bbool Ray_GameManager::createRewardResources(const Path& _lumPath, const Path& _heartPath)
    {
        if (m_spawnedRewardResourceGroup.isValidResourceId())
        {
            return bfalse;
        }

        m_spawnedRewardResourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(
            Resource::ResourceType_ResourceGroup, Path::EmptyPath);
        bbool success = btrue;
        success = success && SPAWNER->declareNeedsSpawnee((ResourceGroup*)m_spawnedRewardResourceGroup.getResource(),
                                                          &m_lumGenerator, _lumPath);
        success = success && SPAWNER->declareNeedsSpawnee((ResourceGroup*)m_spawnedRewardResourceGroup.getResource(),
                                                          &m_heartGenerator, _heartPath);

        RESOURCE_MANAGER->addUserToResourcePhysicalData(m_spawnedRewardResourceGroup);

        return success;
    }

    bbool Ray_GameManager::isAllGameLevelsCompleted()
    {
        return getWMSpotState(s_MO_SHOOTER_B) == SPOT_STATE_COMPLETED &&
            getWMSpotState(s_DE_A1) == SPOT_STATE_COMPLETED;
    }

    void Ray_GameManager::releaseRewardResources()
    {
        if (!m_spawnedRewardResourceGroup.isValidResourceId())
        {
            return;
        }
        SPAWNER->declareNoLongerNeedsSpawnee(&m_lumGenerator);
        SPAWNER->declareNoLongerNeedsSpawnee(&m_heartGenerator);
        RESOURCE_MANAGER->removeUserToResourcePhysicalData(m_spawnedRewardResourceGroup);
        RESOURCE_MANAGER->removeUserToResourceLogicalData(m_spawnedRewardResourceGroup);
        m_spawnedRewardResourceGroup.invalidateResourceId();
    }

    u32 Ray_GameManager::getLevelTotalCageCount(const StringID& _levelTag) const
    {
        const StringID type = RAY_GAMEMANAGER->getMapConfigType(_levelTag);
        const Ray_GameManagerConfig_Template::MissionConfig* config = RAY_GAMEMANAGER->findMissionConfigByType(type, btrue);

        i32 count = 0;
        for (auto slot : config->m_medalSlots)
        {
            if (slot.m_type == Ray_GameManagerConfig_Template::MedalSlotConfig::Cage)
                count++;
        }

        return count;
    }

    u32 Ray_GameManager::getBrokenCageCount() const
    {
        return m_currentLevelData.getBrokenCageCount();
    }

    u32 Ray_GameManager::getTotalTakenToothCount() const
    {
#ifdef ITF_SUPPORT_CHEAT
        if (RAY_CHEATMANAGER->getAllSkullTeeth())
        {
            return 10;
        }
#endif
        Ray_PersistentGameData_Universe* universe = static_cast<Ray_PersistentGameData_Universe*>(m_persistentGameData);
        if (!universe)
            return 0;
        return universe->getTotalTakenToothCount(&m_currentLevelData, getCurrentLevelName());
    }

    u32 Ray_GameManager::getReturnedTeethCount() const
    {
        return getPersistentGameData()->getTeethReturned();
    }

    void Ray_GameManager::returnTeeth()
    {
        getPersistentGameData()->returnTeeth();
        saveGameState(bfalse, getCurrentSlotIndex());
    }

    bbool Ray_GameManager::hasAllTeethBeenReturned() const
    {
#ifdef ITF_SUPPORT_CHEAT
        if (RAY_CHEATMANAGER->getIsCheatTeethSequence())
        {
            return btrue;
        }
#endif
        u32 teethReturned = getPersistentGameData()->getTeethReturned();

        return teethReturned >= 10;
    }

    u32 Ray_GameManager::getCurrentRelicFoundMask() const
    {
        return m_currentLevelData.getTakenRelicMask();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setChallengeWon(EChallengeType _challenge)
    {
        if (_challenge == CHALLENGE_CUP && !isChallengeWon(_challenge))
        {
            REWARD_MANAGER->Action_New(Ray_RewardManager::Action_TimeAttackWon, getMainIndexPlayer());
        }
        return m_currentLevelData.setChallengeWon(_challenge);
    }

    bbool Ray_GameManager::isChallengeWon(EChallengeType _challenge)
    {
        return m_currentLevelData.isChallengeWon(_challenge);
    }

    bbool Ray_GameManager::isChallengeWon(const StringID& _levelName, EChallengeType _challenge)
    {
        Ray_PersistentGameData_Level* pPgdl = getLevelData(_levelName);
        if (pPgdl)
            return pPgdl->isChallengeWon(_challenge);

        return bfalse;
    }

    void Ray_GameManager::setIsTimeAttack(bbool _isTimeAttack)
    {
        m_isTimeAttack = _isTimeAttack;
        m_isTimeAttackFinished = bfalse;
        m_timeAttackTimer = 0.f;
        m_triggerTimeAttackEndSequence = bfalse;
    }

    void Ray_GameManager::setTriggerTimeAttackSequence()
    {
        Actor* results = m_timeAttackActorResults.getActor();

        if (results)
        {
            results->enable();
        }
    }

    void Ray_GameManager::disableTimeAttackForLevel()
    {
        Actor* chrono = m_timeAttackActorChrono.getActor();

        if (chrono)
        {
            chrono->disable();
        }

        Actor* flag = m_timeAttackActorFlag.getActor();

        if (flag)
        {
            flag->disable();
        }
    }

    void Ray_GameManager::disableTimeAttackHUD()
    {
        Actor* results = m_timeAttackActorResults.getActor();

        if (results)
        {
            results->disable();
        }

        Actor* timer = m_timeAttackActorTimer.getActor();

        if (timer)
        {
            timer->disable();
        }
    }

    void Ray_GameManager::enableMurphyAssistForLevel()
    {
        Actor* murphyAssistActor = m_murphyAssist.getActor();

        if (murphyAssistActor)
        {
            Ray_MurphyAssistAIComponent* pComp = murphyAssistActor->GetComponent<Ray_MurphyAssistAIComponent>();
            pComp->requestAppear();
        }
    }

    void Ray_GameManager::disableMurphyAssistForLevel()
    {
        Actor* murphyAssistActor = m_murphyAssist.getActor();

        if (murphyAssistActor)
        {
            murphyAssistActor->disable();
        }
    }

    void Ray_GameManager::setMurphyAssistFollowPlayer()
    {
        Actor* murphyAssistActor = m_murphyAssist.getActor();

        if (murphyAssistActor)
        {
            Ray_MurphyAssistAIComponent* pComp = murphyAssistActor->GetComponent<Ray_MurphyAssistAIComponent>();
            pComp->followPlayer();
        }
    }

    void Ray_GameManager::markMurphyAssistTargetCollected(const ActorRef& _actorRef)
    {
        Actor* murphyAssistActor = m_murphyAssist.getActor();

        if (murphyAssistActor)
        {
            Ray_MurphyAssistAIComponent* pComp = murphyAssistActor->GetComponent<Ray_MurphyAssistAIComponent>();
            pComp->markTargetCollected(_actorRef);
        }
    }

    void Ray_GameManager::showLoadNotificationActor()
    {
        if (!m_showingLoadNotificationActor)
        {
            Actor* loadIcon = getLoadNotificationActor().getActor();

            if (loadIcon)
            {
                loadIcon->enable();
            }

            m_showingLoadNotificationActor = btrue;
        }
    }

    void Ray_GameManager::hideLoadNotificationActor()
    {
        if (m_showingLoadNotificationActor)
        {
            Actor* loadIcon = getLoadNotificationActor().getActor();

            if (loadIcon)
            {
                loadIcon->disable();
            }

            m_showingLoadNotificationActor = bfalse;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Scene* Ray_GameManager::getSceneForPersistentActor(Actor* _obj) const
    {
        World* world = _obj->getWorld();
        if (!world)
            return NULL;
        Scene* scene = world->getRootScene();
        if (!scene)
            return NULL;

        if (_obj->isSpawned())
        {
            return NULL; //If the object is spawned, return NULL as an error
        }
        return scene;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setCageMapPassedDoor(const PackedObjectPath& _packedObjectPath, bbool _passed)
    {
        //check if map cage has already been discovered
        Ray_PersistentGameData_Universe* const persistentGameData = getPersistentGameData();
        if (_passed && persistentGameData && !persistentGameData->isDiscoveredCageMap(
            _packedObjectPath, getCurrentLevelName()))
        {
            //inform reward manager that a new discover has been made
            REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GoThroughCageMapDoor, getMainIndexPlayer());
            //store the discover
            persistentGameData->addDiscoveredCageMap(_packedObjectPath, getCurrentLevelName());
        }

        //update current level data
        m_currentLevelData.setCageMapPassedDoor(_packedObjectPath, _passed);

        // Reset skip counter as we're entering in a secret cave and we don't want to count death in it
        resetSkipCounter(getCurrentCheckpoint()->getRef(), btrue);
    }

    bbool Ray_GameManager::didPassDoorToCageMap(const PackedObjectPath& _packedObjectPath)
    {
        return m_currentLevelData.didPassDoorToCageMap(_packedObjectPath);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    u32 Ray_GameManager::getLevelBrokenCageCount(const StringID& _tag)
    {
        if (_tag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(getPersistentGameData()
                ->getLevel(_tag));
            if (pgdLevel != NULL)
            {
                return pgdLevel->getBrokenCageCount();
            }
        }

        return 0;
    }

    u32 Ray_GameManager::getLevelToothCount(const StringID& _tag)
    {
        if (_tag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(getPersistentGameData()
                ->getLevel(_tag));
            if (pgdLevel != NULL)
            {
                return pgdLevel->getTakenToothCount();
            }
        }

        return 0;
    }

    u32 Ray_GameManager::getLevelLumCount(const StringID& _tag)
    {
        if (_tag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(getPersistentGameData()
                ->getLevel(_tag));
            if (pgdLevel != NULL)
            {
                const Path& path = getLevelPath(_tag);
                ITF_WARNING_CATEGORY(GPP, NULL, !path.isEmpty(),
                                     "Unable to get %s taken lum count because its path is empty",
                                     _tag.getDebugString());
                if (!path.isEmpty())
                    return pgdLevel->getTakenLumCount(path);
            }
        }

        return 0;
    }

    u32 Ray_GameManager::getTakenRelicCount(const StringID& _levelTag)
    {
        Ray_PersistentGameData_Level* pPgdl = getLevelData(_levelTag);
        if (pPgdl)
            return pPgdl->getTakenRelicCount();

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_GameManager::getWMLastElectoonCount() const
    {
        const Ray_PersistentGameData_Universe* universe = getPersistentGameData();
        return universe->getWorldMapData().getLastElectoonCount();
    }

    u32 Ray_GameManager::getWMLastToothCount() const
    {
        const Ray_PersistentGameData_Universe* universe = getPersistentGameData();
        return universe->getWorldMapData().getLastToothCount();
    }

    void Ray_GameManager::setWMLastElectoonCount(u32 _count)
    {
        Ray_PersistentGameData_Universe* universe = getPersistentGameData();
        universe->getWorldMapData().setLastElectoonCount(_count);
    }

    void Ray_GameManager::setWMLastToothCount(u32 _count)
    {
        Ray_PersistentGameData_Universe* universe = getPersistentGameData();
        universe->getWorldMapData().setLastToothCount(_count);
    }

    void Ray_GameManager::setWMUnlockRelicMessageIsDisplayed(bbool _isUnlockRelicMessageDisplayed)
    {
        Ray_PersistentGameData_Universe* universe = getPersistentGameData();
        universe->getWorldMapData().setIsUnlockRelicMessageDisplayed(_isUnlockRelicMessageDisplayed);
    }

    bbool Ray_GameManager::getWMUnlockRelicMessageIsDisplayed() const
    {
        const Ray_PersistentGameData_Universe* universe = getPersistentGameData();
        return universe->getWorldMapData().getIsUnlockRelicMessageDisplayed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    ESpot_State Ray_GameManager::getWMSpotState(const StringID& _tag)
    {
#ifdef ITF_SUPPORT_CHEAT
        if (RAY_CHEATMANAGER->getAllWorldMap())
        {
            return SPOT_STATE_COMPLETED;
        }
#endif

        if (_tag != StringID::Invalid)
        {
            if (getMapConfigType(_tag) == ITF_GET_STRINGID_CRC(world, 3307309411))
            {
                Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
                if (pgd != NULL)
                {
                    return pgd->getWorldState(_tag);
                }
            }
            else
            {
                Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(
                    getPersistentGameData()->getLevel(_tag));
                if (pgdLevel != NULL)
                {
                    return pgdLevel->getLevelState();
                }
            }
        }

        return SPOT_STATE_CLOSED;
    }

    void Ray_GameManager::setWMSpotState(const StringID& _tag, const ESpot_State _state)
    {
        if ((_state == SPOT_STATE_COMPLETED) && (getWMSpotState(_tag) != SPOT_STATE_COMPLETED))
        {
            if (_tag == s_TRIGGER_MOUNTAIN)
            {
                // B worlds special case: unroll paths to B worlds only
                m_worldMapUnlockSequence.setJustCompletedSpot(s_JU_A, btrue);
                m_worldMapUnlockSequence.setJustCompletedSpot(s_MU_A, btrue);
                m_worldMapUnlockSequence.setJustCompletedSpot(s_FO_A, btrue);
                m_worldMapUnlockSequence.setJustCompletedSpot(s_OC_A, btrue);
            }
            else if (_tag == s_MO_B3 || _tag == s_MO_B4)
            {
                // those don't need a path unroll
                // NB[LaurentCou]: don't mark as just completed, magic, they're forced open
            }
            else
            {
                // newly completed => unroll paths
                m_worldMapUnlockSequence.setJustCompletedSpot(_tag, bfalse);
            }
        }


        if (_tag != StringID::Invalid)
        {
            if (getMapConfigType(_tag) == ITF_GET_STRINGID_CRC(world, 3307309411))
            {
                Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
                if (pgd != NULL)
                {
                    pgd->setWorldState(_tag, _state);
                }
            }
            else
            {
                Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(
                    getPersistentGameData()->getLevel(_tag));
                if (pgdLevel != NULL)
                {
                    pgdLevel->setLevelState(_state);
                }
            }
        }

        // Update other spot state if this one unlocks something
        if (_state == SPOT_STATE_COMPLETED)
        {
            updateWMStates();
        }
    }

    void Ray_GameManager::setWMSpotHasWarning(const StringID& _tag, const bbool _hasWarning)
    {
        if (_tag != StringID::Invalid)
        {
            if (getMapConfigType(_tag) == ITF_GET_STRINGID_CRC(world, 3307309411))
            {
                Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
                if (pgd != NULL)
                {
                    pgd->setWorldHasWarning(_tag, _hasWarning);
                }
            }
            else
            {
                Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(
                    getPersistentGameData()->getLevel(_tag));
                if (pgdLevel != NULL)
                {
                    pgdLevel->setLevelHasWarning(_hasWarning);

                    updateWMWorldHasWarning(getTemplate()->getLevelWorldTag(_tag));
                }
            }
        }
    }

    void Ray_GameManager::updateWMWorldHasWarning(const StringID& _worldTag)
    {
        ITF_MAP<StringID, WorldMapLevels>::iterator world = m_worldMapWorlds.find(_worldTag);
        if (world != m_worldMapWorlds.end())
        {
            ITF_VECTOR<StringID>::iterator level = world->second.m_levelTags.begin();
            ITF_VECTOR<StringID>::iterator level_end = world->second.m_levelTags.end();
            bbool hasWarning = bfalse;
            for (; level != level_end && !hasWarning; level++)
            {
                if (getWMSpotHasWarning(*level))
                    hasWarning = btrue;
            }

            setWMSpotHasWarning(_worldTag, hasWarning);
        }
    }

    bbool Ray_GameManager::getWMSpotHasWarning(const StringID& _tag)
    {
        if (_tag != StringID::Invalid)
        {
            if (getMapConfigType(_tag) == ITF_GET_STRINGID_CRC(world, 3307309411))
            {
                Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
                if (pgd != NULL)
                {
                    return pgd->getWorldHasWarning(_tag);
                }
            }
            else
            {
                Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(
                    getPersistentGameData()->getLevel(_tag));
                if (pgdLevel != NULL)
                {
                    return pgdLevel->getLevelHasWarning();
                }
            }
        }

        return bfalse;
    }

    void Ray_GameManager::setCurrentLevelIsSkipped(const bbool _isSkipped)
    {
        m_currentLevelData.setLevelIsSkipped(_isSkipped);
    }

    void Ray_GameManager::setWMLevelIsSkipped(const StringID& _tag, const bbool _isSkipped)
    {
        if (_tag.isValid())
        {
            Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(getPersistentGameData()
                ->getLevel(_tag));
            if (pgdLevel != NULL)
            {
                pgdLevel->setLevelIsSkipped(_isSkipped);
            }
        }
        m_currentLevelData.setLevelIsSkipped(_isSkipped);
    }

    bbool Ray_GameManager::getWMLevelIsSkipped(const StringID& _levelTag)
    {
        if (_levelTag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(getPersistentGameData()
                ->getLevel(_levelTag));
            if (pgdLevel != NULL)
            {
                return pgdLevel->getLevelIsSkipped();
            }
        }

        return bfalse;
    }

    bbool Ray_GameManager::isLevelCompletedOnce(const StringID& _tag)
    {
        return (getWMSpotState(_tag) == SPOT_STATE_COMPLETED) && !getWMLevelIsSkipped(_tag);
    }

    void Ray_GameManager::skipCurrentLevel()
    {
        const StringID& tag = getCurrentLevelName();

        ITF_WARNING_CATEGORY(GPP, NULL, isLevelSkippable(tag), "Skipping %s, but it's not a skippable level !!!",
                             tag.getDebugString());

        if (tag.isValid())
        {
            // Mark current level as skipped
            setWMLevelIsSkipped(tag, btrue);

            // set current level as completed
            REWARD_MANAGER->Session_Stop();

            //get map config
            StringID tag = getCurrentLevelName();

            // mark level as completed if not done already
            if (tag != StringID::Invalid)
            {
                ESpot_State state = getCurrentLevelState();
                if (state != SPOT_STATE_COMPLETED)
                {
                    setCurrentLevelState(SPOT_STATE_COMPLETED);

                    // after a completed last level in current world
                    // - mark the current world as completed too
                    // - go to the *world* map (= invalidate current WM level)
                    if (isLastLevel(tag))
                    {
                        setWMSpotState(getWMCurrentWorldTag(), SPOT_STATE_COMPLETED);
                        setWMCurrentLevel(ObjectPath(), StringID::Invalid);
                    }
                }
            }

            // Save gamestate
            saveGameState(bfalse, getCurrentSlotIndex());

            // Go to worldmap
            resetToWorldMap(bfalse);
            //this is done because we get one visible frame when choosing to leave in skip option...
            RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeLoadMap(), btrue);
        }
    }

    void Ray_GameManager::resetSkipCounter(ObjectRef _newCheckpoint, bbool _forceReset /*= bfalse*/)
    {
        if (m_deathCounterCheckpointRef != _newCheckpoint || _forceReset)
        {
            // Changed page, then reset the overall max death counter
            if (m_deathCounterCheckpointRef != _newCheckpoint)
                setHasRejectedSkip(bfalse);

            m_deathCounterCheckpointRef = _newCheckpoint;
            m_deathCounterOnSameCheckpoint = 0;
        }
    }

    void Ray_GameManager::resetSkipData()
    {
        resetSkipCounter(ObjectRef::InvalidRef, btrue);

        setHasRejectedSkip(bfalse);
    }

    void Ray_GameManager::increaseSkipCounter()
    {
        const StringID& tag = getCurrentLevelName();
        if (tag.isValid() && isLevelSkippable(tag) && !isLevelCompletedOnce(tag))
        {
            ITF_ASSERT(m_deathCounterCheckpointRef.isValid());
            ITF_ASSERT(m_deathCounterCheckpointRef == m_currentCheckpoint);
            m_deathCounterOnSameCheckpoint++;
        }
    }

    const u32 Ray_GameManager::getCurrentDeathMaxCount() const
    {
        if (isLevelSkippable(getCurrentLevelName()))
            return (m_hasRejectedSkip
                        ? m_configTemplate->getNbDeathBeforeSecondSkip()
                        : m_configTemplate->getNbDeathBeforeFirstSkip());
        else
            return U32_INVALID;
    }

    void Ray_GameManager::updateWMStates()
    {
        const ITF_MAP<StringID, ITF_VECTOR<StringID>> unlockedByList = m_configTemplate->getUnlockedByList();
        ITF_MAP<StringID, ITF_VECTOR<StringID>>::const_iterator it = unlockedByList.begin();
        ITF_MAP<StringID, ITF_VECTOR<StringID>>::const_iterator it_end = unlockedByList.end();

        for (; it != it_end; it++)
        {
            // This point is not accessible yet, then look if conditions are ok to open it
            if (getWMSpotState(it->first) == SPOT_STATE_CLOSED)
            {
                bbool canBeOpened = btrue;
                const ITF_VECTOR<StringID>& unlockerList = it->second;
                for (u32 i = 0; i < unlockerList.size(); i++)
                {
                    if (getWMSpotState(unlockerList[i]) != SPOT_STATE_COMPLETED)
                        canBeOpened = bfalse;
                }

                if (canBeOpened)
                    setWMSpotState(it->first, SPOT_STATE_NEW);
            }
        }
    }

    void Ray_GameManager::setCurrentLevelState(const ESpot_State _state)
    {
        m_currentLevelData.setLevelState(_state);

        setWMSpotState(getCurrentLevelName(), _state);

        // Update other spot state if this one unlocks something
        if (_state == SPOT_STATE_COMPLETED)
        {
            updateWMStates();
        }
    }

    ESpot_State Ray_GameManager::getCurrentLevelState()
    {
        return m_currentLevelData.getLevelState();
    }

    void Ray_GameManager::setCurrentLevelTimeAttackTime(u32 _time)
    {
        m_currentLevelData.setBestTimeAttack(_time);
    }

    u32 Ray_GameManager::getCurrentLevelTimeAttackTime() const
    {
        return m_currentLevelData.getBestTimeAttack();
    }

    void Ray_GameManager::setCurrentLevelBestLumAttack(u32 _lumCount)
    {
        m_currentLevelData.setBestLumAttack(_lumCount);
    }

    u32 Ray_GameManager::getCurrentLevelBestLumAttack() const
    {
        return m_currentLevelData.getBestLumAttack();
    }


    void Ray_GameManager::markWorldMapNodesAsCompleted(const ITF_VECTOR<StringID>& _unlocks)
    {
        for (u32 i = 0; i < _unlocks.size(); i++)
        {
            // complete given spot
            setWMSpotState(_unlocks[i], SPOT_STATE_COMPLETED);

            // NB[LaurentCou]: MO_SHOOTER_A can be exited without being completed
            // and needs to show unlocking of the B worlds -> to world selection
            // + moved here to fix RO-17074
            if (_unlocks[i] == s_TRIGGER_MOUNTAIN)
            {
                setWMCurrentLevel(ObjectPath(), StringID::Invalid);
            }

            // special case for MO_B3, MO_B4 and MO_SHOOTER_B
            // -> when the unlock trigger is passed, player has to spawn on the next
            // world map spot
            if (_unlocks[i] == s_MO_B3)
            {
                setWMCurrentLevel(getWMSpotObjectPath(s_MO_B4), s_MO_B4);
                if (getWMSpotState(s_MO_B4) == SPOT_STATE_CLOSED)
                {
                    setWMSpotState(s_MO_B4, SPOT_STATE_NEW);
                }
            }
            else if (_unlocks[i] == s_MO_B4)
            {
                setWMCurrentLevel(getWMSpotObjectPath(s_MO_SHOOTER_B), s_MO_SHOOTER_B);
                if (getWMSpotState(s_MO_SHOOTER_B) == SPOT_STATE_CLOSED)
                {
                    setWMSpotState(s_MO_SHOOTER_B, SPOT_STATE_NEW);
                }
            }
        }
    }

    const bbool Ray_GameManager::isStillUnderLockCondition(const StringID& _tag)
    {
        bbool stillLocked = bfalse;

        const ITF_MAP<StringID, ITF_VECTOR<StringID>> unlockedByList = m_configTemplate->getUnlockedByList();
        ITF_MAP<StringID, ITF_VECTOR<StringID>>::const_iterator it = unlockedByList.find(_tag);

        if (it != unlockedByList.end())
        {
            const ITF_VECTOR<StringID>& unlockerList = it->second;
            for (u32 i = 0; i < unlockerList.size(); i++)
            {
                if (getWMSpotState(unlockerList[i]) != SPOT_STATE_COMPLETED)
                    stillLocked = btrue;
            }
        }

        return stillLocked;
    }

    void Ray_GameManager::PickUpAllRelics()
    {
        const ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>& mapConfigs = m_configTemplate->getLevelsInfo();

        for (u32 i = 0; i < mapConfigs.size(); i++)
        {
            for (u32 j = 0; j < mapConfigs[i].m_maxRelics; j++)
            {
                setIsRelicTaken(mapConfigs[i].m_tag, i, btrue);
                u32 frescoId = getPersistentGameData()->GetDebugFrescoId();
                getPersistentGameData()->UpdateInGameRelicsInfo(frescoId);
            }
        }
    }

    void Ray_GameManager::fillWorldMapWorldsList()
    {
        const ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>& mapConfigs = m_configTemplate->getLevelsInfo();

        for (u32 i = 0; i < mapConfigs.size(); i++)
        {
            const StringID& levelWorld = mapConfigs[i].m_worldTag;
            if (levelWorld.isValid())
            {
                m_worldMapWorlds[levelWorld].m_levelTags.push_back(mapConfigs[i].m_tag);

                if (getMinElectoons(mapConfigs[i].m_tag) > 0)
                {
                    m_lockedLevelsWithElectoons.push_back(mapConfigs[i].m_tag);
                }
            }
        }

        m_worldMapWorlds[s_HomeMap];
    }

    void Ray_GameManager::getLevelTagsFromWorld(const StringID& _worldTag, ITF_VECTOR<StringID>& _levelTags)
    {
        _levelTags.clear();

        ITF_MAP<StringID, WorldMapLevels>::iterator world = m_worldMapWorlds.find(_worldTag);
        if (world != m_worldMapWorlds.end())
        {
            _levelTags = world->second.m_levelTags;
        }
    }

    void Ray_GameManager::getWorldsWithWarnings(ITF_VECTOR<StringID>& _worlds)
    {
        _worlds.clear();

        ITF_MAP<StringID, WorldMapLevels>::iterator it = m_worldMapWorlds.begin();
        ITF_MAP<StringID, WorldMapLevels>::iterator it_end = m_worldMapWorlds.end();

        for (; it != it_end; it++)
        {
            u32 worldSize = it->second.m_levelTags.size();
            bbool bfoundNew = bfalse;
            for (u32 i = 0; i < worldSize && !bfoundNew; i++)
            {
                StringID type = getMapConfigType(it->second.m_levelTags[i]);
                if (type.isValid())
                {
                    // Found a level that is new and type "chest" or "bonus"
                    if (getWMSpotHasWarning(it->second.m_levelTags[i]))
                    {
                        _worlds.push_back(it->first);
                        bfoundNew = btrue;
                    }
                }
            }

            // Specific case for empty world such as HomeMap
            if (worldSize == 0 && getWMSpotHasWarning(it->first))
            {
                _worlds.push_back(it->first);
            }
        }
    }

    bbool Ray_GameManager::hasNewCostumeAvailable()
    {
        Actor* actor = AIUtils::getActor(RAY_GAMEMANAGER->getCostumeManager());

        if (actor)
        {
            Ray_PlayerCostumeManagerComponent* manager = actor->GetComponent<Ray_PlayerCostumeManagerComponent>();

            if (manager)
            {
                u32 highestPrice = 0;
                if (manager->getNextCostumeAvailable(highestPrice) == ITF_INVALID_OBJREF
                    && getPersistentGameData()->getCostumeLastPrice() != U32_INVALID
                    && getPersistentGameData()->getCostumeLastPrice() > 0)
                    return btrue;

                // costume with price 0 are not considered as new, so memorize the price
                // of the cheapest costume (>0)
                if (getPersistentGameData()->getCostumeLastPrice() == 0)
                    getPersistentGameData()->setCostumeLastPrice(highestPrice);

                if (getPersistentGameData()->getCostumeLastPrice() > 0
                    && getPersistentGameData()->getCostumeLastPrice() < highestPrice)
                    return btrue;
            }
        }

        return bfalse;
    }

    void Ray_GameManager::onWorldMapElectoonsEarned()
    {
        u32 size = m_lockedLevelsWithElectoons.size();
        for (u32 i = 0; i < size; i++)
        {
            // This level has been discovered but we can't enter yet (SPOT_STATE_CANNOT_ENTER)
            // and we've reach at least its electoon count limit -> change its warning flag to true then
            const StringID& levelTag = m_lockedLevelsWithElectoons[i];
            if ((getWMSpotState(levelTag) == SPOT_STATE_CANNOT_ENTER
                    || getWMSpotState(levelTag) == SPOT_STATE_NEW)
                && getMinElectoons(levelTag) <= getElectoonsCount())
            {
                setWMSpotHasWarning(levelTag, btrue);
            }
        }

        // Check for costumes
        if (hasNewCostumeAvailable())
        {
            setWMSpotHasWarning(ITF_GET_STRINGID_CRC(HomeMap, 1612706167), btrue);
        }
    }

    const ObjectPath Ray_GameManager::getWMSpotObjectPath(const StringID& _tag)
    {
        WMSpotList::iterator it = m_wmSpots.find(_tag);
        if (it != m_wmSpots.end())
        {
            return (*it).second;
        }
        return ObjectPath();
    }

    void Ray_GameManager::setWMSpotObjectPath(const StringID& _tag, const ObjectPath& _path)
    {
        m_wmSpots[_tag] = _path;
    }

    Actor* Ray_GameManager::getWMSpot(const StringID& _tag)
    {
        ObjectPath path = getWMSpotObjectPath(_tag);
        Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
        if (pickable)
        {
            return pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
        }
        return NULL;
    }

    const ObjectPath Ray_GameManager::getWMCurrentWorldObjectPath()
    {
        Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
        if (pgd != NULL)
        {
            return pgd->getCurrentWorldObjectPath();
        }

        return ObjectPath();
    }

    const StringID& Ray_GameManager::getWMCurrentWorldTag()
    {
        Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
        if (pgd != NULL)
        {
            return pgd->getCurrentWorldTag();
        }

        return StringID::Invalid;
    }

    void Ray_GameManager::setWMCurrentWorld(const ObjectPath& _path, const StringID& _tag)
    {
        Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
        if (pgd != NULL)
        {
            return pgd->setCurrentWorld(_path, _tag);
        }
    }

    const ObjectPath Ray_GameManager::getWMCurrentLevelObjectPath()
    {
        Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
        if (pgd != NULL)
        {
            return pgd->getCurrentLevelObjectPath();
        }

        return ObjectPath();
    }

    const StringID& Ray_GameManager::getWMCurrentLevelTag()
    {
        Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
        if (pgd != NULL)
        {
            return pgd->getCurrentLevelTag();
        }

        return StringID::Invalid;
    }

    void Ray_GameManager::setWMCurrentLevel(const ObjectPath& _path, const StringID& _tag)
    {
        Ray_PersistentGameData_Universe* pgd = getPersistentGameData();
        if (pgd != NULL)
        {
            return pgd->setCurrentLevel(_path, _tag);
        }
    }

    bbool Ray_GameManager::isJustCompletedSpot(const StringID& _tag) const
    {
        return m_worldMapUnlockSequence.isJustCompletedSpot(_tag);
    }

    bbool Ray_GameManager::isJustCompletedSpotBWorldOnly(const StringID& _tag) const
    {
        return m_worldMapUnlockSequence.isJustCompletedSpotBWorldOnly(_tag);
    }

    void Ray_GameManager::setCurrentLevelName(const Path& _map)
    {
        PathString_t baseName;
        _map.getBasenameWithoutExtension(baseName);
        m_currentLevelName = baseName;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setIsCutSceneAlreadySeen(Actor* _obj, bbool _alreadySeen)
    {
        Scene* scene = getSceneForPersistentActor(_obj);
        if (!scene)
            return;
        PackedObjectPath objPath(_obj);
        return m_currentLevelData.setAlreadySeenCutScene(scene->getPath(), objPath, _alreadySeen);
    }

    bbool Ray_GameManager::getIsCutSceneAlreadySeen(Actor* _obj) const
    {
        Scene* scene = getSceneForPersistentActor(_obj);
        if (!scene)
            return bfalse;
        PackedObjectPath objPath(_obj);
        return m_currentLevelData.isCutSceneAlreadySeen(scene->getPath(), objPath);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::getIsCageBroken(Actor* _obj) const
    {
        ITF_ASSERT_CRASH(_obj, "invalid pointer");
        Ray_CageAIComponent* cageComponent = _obj->GetComponent<Ray_CageAIComponent>();
        i32 cageIndex = cageComponent->getCageIndex();

        if (cageIndex < 0)
            return bfalse;

        return getIsCageBroken(cageIndex);
    }

    bbool Ray_GameManager::getIsCageBroken(u32 _cageIndex) const
    {
        return m_currentLevelData.isCageBroken(_cageIndex);
    }

    void Ray_GameManager::setIsCageBroken(Actor* _obj, bbool _isTaken)
    {
        Scene* scene = getSceneForPersistentActor(_obj);
        if (!scene)
            return;

        Ray_CageAIComponent* cageComponent = _obj->GetComponent<Ray_CageAIComponent>();
        i32 cageIndex = -1;
        if (cageComponent)
            cageIndex = cageComponent->getCageIndex();

        if (cageIndex < 0)
            return;

        // 4 is the index of the final cage, which is not hidden
        if (cageIndex != 4)
        {
            REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetHiddenCage, getMainIndexPlayer());
        }

        m_currentLevelData.setBrokenCageFlag(scene->getPath(), cageIndex, _isTaken);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::getIsToothTaken(Actor* _obj) const
    {
        Scene* scene = getSceneForPersistentActor(_obj);
        if (!scene)
            return bfalse;
        PackedObjectPath objPath(_obj);
        return m_currentLevelData.isToothTaken(scene->getPath(), objPath);
    }

    void Ray_GameManager::setIsToothTaken(Actor* _obj, bbool _isTaken)
    {
        Scene* scene = getSceneForPersistentActor(_obj);
        if (!scene)
            return;

        PackedObjectPath objPath(_obj);
        m_currentLevelData.setTakenTooth(scene->getPath(), objPath, _isTaken);
        REWARD_MANAGER->Action_Set(Ray_RewardManager::Action_GetSkullTooth, getMainIndexPlayer(),
                                   f32(getTotalTakenToothCount()));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setCurrentLevelIsRelicTaken(u32 _index, bbool _isTaken)
    {
        m_currentLevelData.setRelicTaken(_index, _isTaken);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::getIsRelicTaken(const StringID& _tag, u32 _index)
    {
        ITF_WARNING(NULL, _tag != StringID::Invalid, "Nothing will be saved as it's not a valid map");
        if (_tag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* level = getLevelData(_tag);
            if (level)
            {
                return level->isRelicTaken(_index);
            }
        }

        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setIsRelicTaken(const StringID& _tag, u32 _index, bbool _isTaken)
    {
        ITF_WARNING(NULL, _tag != StringID::Invalid, "Nothing will be saved as it's not a valid map");

        if (_tag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* level = getLevelData(_tag);
            if (level)
            {
                level->setRelicTaken(_index, _isTaken);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_PersistentGameData_Level* Ray_GameManager::getLevelData(const StringID& _levelName)
    {
        Ray_PersistentGameData_Level* pgdLevel = static_cast<Ray_PersistentGameData_Level*>(getPersistentGameData()->
            getLevel(_levelName));
        return pgdLevel;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_GameManager::getLevelBestTime(const StringID& _tag)
    {
        Ray_PersistentGameData_Level* level = getLevelData(_tag);
        if (level)
        {
            return level->getBestTimeAttack();
        }
        return U32_INVALID;
    }

    //////////////////////////////////////////////////////////////////////////
    u32 Ray_GameManager::getLevelBestLumAttack(const StringID& _tag)
    {
        Ray_PersistentGameData_Level* level = getLevelData(_tag);
        if (level)
        {
            return level->getBestLumAttack();
        }
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setMedalActor(Actor* _actor)
    {
        if (_actor)
        {
            m_medalActor.setActor(_actor);
        }
        else
        {
            m_medalActor.invalidate();
            m_currentMedal.invalidate();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Actor* Ray_GameManager::getMedalActor() const
    {
        return m_medalActor.getActor();
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setHideJoinMessage(bbool _hide)
    {
        if (_hide)
        {
            m_hideJoinMessage++;
        }
        else
            m_hideJoinMessage > 0 ? m_hideJoinMessage-- : m_hideJoinMessage = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::isCurrentWorldPaused()
    {
        if (isInPause())
            return btrue;
        if (m_rCurrentWorld != ITF_INVALID_OBJREF)
        {
            World* pWorld = (World*)GETOBJECT(m_rCurrentWorld);
            ITF_ASSERT(pWorld);
            if (pWorld && !pWorld->getAllowUpdate())
                return btrue;
        }

        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::enterPause(bbool _bShowMenu)
    {
        Super::enterPause(_bShowMenu);

        m_pauseType |= PAUSEFLAG_INGAME;
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::leavePause(bbool _bHideMenu)
    {
        Super::leavePause(_bHideMenu);

        m_pauseType = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::pauseGameplay()
    {
        bbool wasPaused = isCurrentWorldPaused();
        pauseMap(btrue);
        CAMERACONTROLLERMANAGER->pauseMode(btrue);

        m_pauseType |= PAUSEFLAG_GAMEPLAY;
#if defined ITF_SUPPORT_RAKI
        if (!wasPaused)
        {
            SOUND_ADAPTER->getPauseSensitivePlayingVoices(m_voicesPausedWhenEnteringGameplayPause);

            for (u32 index = 0; index < m_voicesPausedWhenEnteringGameplayPause.size(); index++)
            {
                SOUND_ADAPTER->pause(m_voicesPausedWhenEnteringGameplayPause[index]);
            }
        }
#else
        if (!wasPaused)
        {
            AUDIO_ADAPTER->getPauseSensitivePlayingVoices(m_voicesPausedWhenEnteringGameplayPause);

            for (u32 index = 0; index < m_voicesPausedWhenEnteringGameplayPause.size(); index++)
            {
                AUDIO_ADAPTER->pause(SoundHandle(m_voicesPausedWhenEnteringGameplayPause[index]));
            }
        }

#endif
    }

    void Ray_GameManager::unpauseGameplay()
    {
        bbool wasPaused = isCurrentWorldPaused();

        m_pauseType &= ~PAUSEFLAG_GAMEPLAY;

        pauseMap(bfalse);
        CAMERACONTROLLERMANAGER->pauseMode(bfalse);
#if defined ITF_SUPPORT_RAKI
        if (wasPaused)
        {
            for (u32 index = 0; index < m_voicesPausedWhenEnteringGameplayPause.size(); index++)
                SOUND_ADAPTER->resume(m_voicesPausedWhenEnteringGameplayPause[index]);
            m_voicesPausedWhenEnteringGameplayPause.clear();
        }
#else
        if (wasPaused)
        {
            for (u32 index = 0; index < m_voicesPausedWhenEnteringGameplayPause.size(); index++)
                AUDIO_ADAPTER->resume(SoundHandle(m_voicesPausedWhenEnteringGameplayPause[index]));
            m_voicesPausedWhenEnteringGameplayPause.clear();
        }

#endif
    }

    void Ray_GameManager::updatePauseMode()
    {
        if (m_worldPhysicallyReady)
        {
            switch (m_requestedGameplayPause)
            {
            case GameplayPause_Pause: pauseGameplay();
                break;
            case GameplayPause_Unpause: unpauseGameplay();
                break;
            };

            m_requestedGameplayPause = GameplayPause_NoChange;

            Super::updatePauseMode();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::createMedalMap()
    {
        m_medalWorldRef = WORLD_MANAGER->createAndLoadEmptyWorld(btrue);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::enableMedalWorld(bbool _enable)
    {
        ObjectRef baseRef = getMedalWorldRef();
        World* world = (World*)GETOBJECT(baseRef);
        ITF_ASSERT(world!=NULL);
        if (world)
        {
            world->setActive(_enable);
            world->setAllowUpdate(_enable);
            world->setAllowDraw(_enable);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::destroyMedalMap()
    {
        World* world = (World*)GETOBJECT(m_medalWorldRef);
        if (world)
        {
            WORLD_MANAGER->deleteWorld(world);
            m_medalWorldRef.invalidate();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::unloadGameResources()
    {
        releaseRewardResources();
        destroyMedalMap();
        Super::unloadGameResources();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::cutSceneAlreadySeenCallback(Actor* _cutSceneActor)
    {
        return RAY_GAMEMANAGER->setIsCutSceneAlreadySeen(_cutSceneActor, btrue);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::canInterruptCutSceneCallback(Actor* _cutSceneActor)
    {
        return RAY_GAMEMANAGER->getIsCutSceneAlreadySeen(_cutSceneActor);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::postLoadInit()
    {
        Super::postLoadInit();

        m_powerUpManager.init(&m_configTemplate->getPowerUpManagerData());

        GAMEINTERFACE->setCutSceneAlreadySeenCallback(cutSceneAlreadySeenCallback);
        GAMEINTERFACE->setCanInterruptCutSceneCallback(canInterruptCutSceneCallback);

        Ray_WorldFile* worldFileData = newAlloc(mId_Gameplay, Ray_WorldFile);
        //worldFileData->setLumMusicManagerACT(m_configTemplate->getDefaultLumMusicManagerPath());
        m_defaultWorldFile.setProjectData(worldFileData);

        WORLD_MANAGER->setDefaultWorldFile(&m_defaultWorldFile);
        createMedalMap();

#ifdef ITF_SUPPORT_CHEAT
        for (ITF_VECTOR<String>::const_iterator it = m_configTemplate->getMaps().begin(); it != m_configTemplate->
             getMaps().end(); ++it)
        {
            UI_MENUMANAGER->addDebugMenuMap(newAlloc(mId_UI, DebugMenuEntryLoadMap(*it)));
        }

        // Debug UI
        UI_MENUMANAGER->addDebugMenuCheat(newAlloc(mId_UI, DebugMenuEntryChangeActivation("Join / leave game")));
        UI_MENUMANAGER->addDebugMenuCheat(
            newAlloc(mId_UI, DebugMenuEntryRevertToLastCheckpoint("Revert to last checkpoint")));
        UI_MENUMANAGER->addDebugMenuCheat(
            newAlloc(mId_UI, DebugMenuEntryEraseGameState("Reinitialize game state and savegame")));

        DebugMenu* cheatMenu = newAlloc(mId_UI, DebugMenu);
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleFriendlyFire("")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryTogglePlayerCrush("")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleNpcCrush("")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleAllPowerUps("All Global PowerUps")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleDive("Dive PowerUp")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleWalkOnWallsGlobal("Walk on Walls PowerUp Global")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleReduction("Reduction PowerUp")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleHairlicopter("Helicopter PowerUp")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleFight("Fight PowerUp")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleHeartShieldExclusive("Heart Shield Exclusive")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleHeartShield("Heart Shield PowerUp (per-player)")));
        cheatMenu->addEntry(
            newAlloc(mId_UI, DebugMenuEntryToggleSuperPunchBasic("Super Punch Basic PowerUp (per-player)")));
        cheatMenu->addEntry(
            newAlloc(mId_UI, DebugMenuEntryToggleSuperPunchSeeking("Super Punch Seeking PowerUp (per-player)")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleInvincibleEnemies("")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleInvincibilityAll("")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleInvincibility("Toggle invincibility per player")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryAddHP("Add HP",1)));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryAddHP("Remove HP",-1)));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryAddMaxHP("Add max HP",1)));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryAddMaxHP("Remove max HP",-1)));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryToggleShowHUDs("Force HUD display")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryAllWorldMap("All world map")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryAllSkullTeeth("All skull teeth")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryUnlockAllCostumes("Unlock all costumes")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryTriggerToothSequence("Trigger teeth sequence")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryEnableTimeAttack("Enable Time Attack")));
        cheatMenu->addEntry(newAlloc(mId_UI, DebugMenuEntryUnlockAllBoss("Unlock all Boss")));

        UI_MENUMANAGER->addDebugMenu(cheatMenu);
#endif //ITF_SUPPORT_CHEAT

        // GAMEMODE CONTROLLERS
        //-----------------------
        m_GameModeControllers.resize(RAY_GAMEMODE_COUNT);
        m_GameModeControllers.fill(NULL);
        // Create controllers
        registerGameModeController<GameModeController>(RAY_GAMEMODE_PLATFORMER);
        registerGameModeController<Ray_ShooterGameModeController>(RAY_GAMEMODE_SHOOTER);
        registerGameModeController<Ray_WorldMapGameModeController>(RAY_GAMEMODE_WORLDMAP);

        getGameModeParametersFactory()->RegisterObject<Ray_ShooterGameModeParameters>(
            ITF_GET_STRINGID_CRC(Ray_ShooterGameModeParameters, 1675665911));

        // REWARDS MANAGER
        //-----------------------
        if (REWARD_MANAGER->isEnable())
        {
            ObjectFactory* templatesFactory = TEMPLATEDATABASE->getClassTemplateFactory();
            ObjectFactory* triggerFactory = RAY_REWARD_MANAGER->getTriggerFactory();

            templatesFactory->RegisterObject<Ray_RewardContainer_Template>(
                ITF_GET_STRINGID_CRC(Ray_RewardContainer_Template, 3983479571));

            // Rewards
            triggerFactory->RegisterObject<Ray_RewardTrigger_Sum>(
                ITF_GET_STRINGID_CRC(Ray_RewardTrigger_Sum, 1124065120));
            triggerFactory->RegisterObject<Ray_RewardTrigger_SumWithTimer>(
                ITF_GET_STRINGID_CRC(Ray_RewardTrigger_SumWithTimer, 2171631799));
            triggerFactory->RegisterObject<Ray_RewardTrigger_FinishLevel>(
                ITF_GET_STRINGID_CRC(Ray_RewardTrigger_FinishLevel, 2436977909));
            triggerFactory->RegisterObject<Ray_RewardTrigger_Timer>(
                ITF_GET_STRINGID_CRC(Ray_RewardTrigger_Timer, 1659492541));
            triggerFactory->RegisterObject<Ray_RewardTrigger_Event>(
                ITF_GET_STRINGID_CRC(Ray_RewardTrigger_Event, 3694137488));

            m_rewardManager.init();
        }

        m_wasPostLoadInitCalled = btrue;
    }

    void Ray_GameManager::stopAllSequences()
    {
        u32 numPlayingSeqs = m_currentSequences.size();

        // Do not stop end sequence !
        u32 seqIndex = isPlayingSeq(&m_endGameSequence);

        for (u32 i = 0; i < numPlayingSeqs; i++)
        {
            if (i != seqIndex)
                m_currentSequences[i]->stop();
        }

        m_currentSequences.clear();

        // Restore end sequence if it's currently in use
        if (seqIndex != U32_INVALID)
            setSequence(&m_endGameSequence);
    }

    void Ray_GameManager::stopTimeAttack()
    {
        if (m_triggerTimeAttackEndSequence)
        {
            RAY_GAMEMANAGER->requestGameplayUnpause();
        }

        setIsTimeAttack(bfalse);
    }

    void Ray_GameManager::emptyCurrentLevelGameState()
    {
        Super::emptyCurrentLevelGameState();

        stopAllSequences();
        stopTimeAttack();
        disableTimeAttackHUD();
        m_currentLevelData.clear();
        m_currentLevelData_Checkpoint.clear();
    }

    void Ray_GameManager::clearWholeGameState()
    {
        if (m_persistentGameData)
        {
            m_persistentGameData->clear();
        }
        emptyCurrentLevelGameState();

        // clear hit points and heart shields on new/load savegame (RO-14974)
        for (u32 i = 0; i < s_maxPlayers && i < m_players.size(); ++i)
        {
            Ray_Player* player = static_cast<Ray_Player*>(m_players[i]);
            ITF_ASSERT(player);
            if (player)
            {
                player->setHitPoints(getStartHitPoints());
            }
            m_powerUpManager.setEnabled(Ray_PowerUp_HeartShield, i, bfalse);
        }

        // fix stuck on world map after sign out in level ending (X360)
        m_worldMapUnlockSequence.clearJustCompletedSpots();
    }

    void Ray_GameManager::checkpointLoad()
    {
        Super::checkpointLoad();


        Ray_PlayerHudScoreComponent::setIsRelicCaptionEnabled(bfalse);

#ifdef ITF_SUPPORT_ONLINETRACKING
        m_onlineTrackingManager.m_timeElapsedSinceLastCheckPoint = 0.0f;
#endif // ITF_SUPPORT_ONLINETRACKING

        onCheckpointLoadUpdateTimeAttack();
        onCheckpointLoadUpdateRescued();
        onCheckpointLoadUpdateSprintTutorial();
        restoreHealthModifierForAllPlayers();
        onCheckpointLoadUpdateMurphyAssist();
    }

    void Ray_GameManager::onCheckpointLoadUpdateRescued()
    {
        m_totalRescuedFriendlies.clear();
        m_friendliesRescued.clear();

        u32 numFriendlies = m_storedFriendliesRescued.size();

        if (numFriendlies > 0)
        {
            Actor* checkpoint = AIUtils::getActor(m_currentCheckpoint);

            if (checkpoint)
            {
                f32 sceneZ = AIUtils::getSceneZFromPickable(checkpoint);

                Actor* p = getActivePlayer(getMainIndexPlayer());

                if (p)
                {
                    Vec2d pos = p->get2DPos();

                    for (u32 i = 0; i < numFriendlies; i++)
                    {
                        const FriendlyRescuedInfo& rescuedInfo = m_storedFriendliesRescued[i];

                        m_totalRescuedFriendlies.push_back(rescuedInfo);

                        if (rescuedInfo.m_teleport)
                        {
                            Actor* friendlyActor = AIUtils::getActor(rescuedInfo.m_ref);

                            if (!friendlyActor)
                            {
                                continue;
                            }

                            Vec3d targetPos = pos.to3d(sceneZ);

                            if (friendlyActor->getTemplate()->isZForced())
                            {
                                targetPos.m_z += friendlyActor->getTemplate()->getForcedZ();
                            }

                            EventTeleport teleport(targetPos, checkpoint->getAngle(), btrue);
                            friendlyActor->onEvent(&teleport);
                        }
                    }
                }
            }
        }
    }

    void Ray_GameManager::onCheckpointLoadUpdateSprintTutorial()
    {
        // restore sprint tutorial if some player is dying too much
        if (m_sprintTutorialRegistered && m_persistentGameData)
        {
            bbool failure = bfalse;
            for (u32 i = 0; i < s_maxPlayers; ++i)
            {
                Ray_Player* player = static_cast<Ray_Player*>(m_players[i]);
                if (player->getActive() &&
                    player->getSprintDuration() <= m_configTemplate->getSprintTutorialRequiredDuration())
                {
                    failure = btrue;
                    break;
                }
                player->resetSprintDuration();
            }
            if (failure)
            {
                m_sprintTutorialFailureCount = Min(
                    m_sprintTutorialFailureCount + 1,
                    m_configTemplate->getSprintTutorialFailureCount());
            }
            else
            {
                m_sprintTutorialFailureCount = 0;
            }
            if (m_sprintTutorialFailureCount >= m_configTemplate->getSprintTutorialFailureCount()
                && getPersistentGameData()->getSprintTutorialDisabled())
            {
                getPersistentGameData()->setSprintTutorialDisabled(bfalse);
            }
        }
    }
#ifdef ITF_SUPPORT_CHEAT
    void Ray_GameManager::cheatUnlockAllLevel()
    {
        setWMSpotState(s_JU_A, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MU_A, SPOT_STATE_COMPLETED);
        setWMSpotState(s_FO_A, SPOT_STATE_COMPLETED);
        setWMSpotState(s_OC_A, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MO_A, SPOT_STATE_COMPLETED);

        setWMSpotState(s_JU_B, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MU_B, SPOT_STATE_COMPLETED);
        setWMSpotState(s_FO_B, SPOT_STATE_COMPLETED);
        setWMSpotState(s_OC_B, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MO_B, SPOT_STATE_COMPLETED);

        // Unlock boss levels
        setWMSpotState(s_ju_bossmaster, SPOT_STATE_COMPLETED);
        setWMSpotState(s_BossBird, SPOT_STATE_COMPLETED);
        setWMSpotState(s_OC_B3, SPOT_STATE_COMPLETED);
        setWMSpotState(s_FO_B3, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MO_B3, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MO_B4, SPOT_STATE_COMPLETED);

        // Unlock special levels
        setWMSpotState(s_OC_A1, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MO_SHOOTER_A, SPOT_STATE_COMPLETED);
        setWMSpotState(s_MO_SHOOTER_B, SPOT_STATE_COMPLETED);
        setWMSpotState(s_DE_A1, SPOT_STATE_COMPLETED);

        if (const Ray_GameManagerConfig_Template* configTemplate = getTemplate())
        {
            const ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>& levelsInfo = configTemplate->getLevelsInfo();
            u32 unlockedCount = 0;
            for (const auto& i : levelsInfo)
            {
                const StringID& levelTag = i.m_tag;
                if (levelTag.isValid())
                {
                    setWMSpotState(levelTag, SPOT_STATE_COMPLETED);
                    unlockedCount++;
                }
            }
        }
        LOG("[CHEAT] - unlock all level");
        RAY_GAMEMANAGER->saveGameState(false, RAY_GAMEMANAGER->getCurrentSlotIndex());
    }
    void Ray_GameManager::cheatMaxCurrency()
    {
        Ray_PersistentGameData_Universe* universe = getPersistentGameData();
        if (!universe)
            return;
        const ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>& levels = m_configTemplate->getLevelsInfo();

        u32 totalCagesNeeded = 246;
        u32 cagesMarked = 0;

        for (u32 i = 0; i < levels.size() && cagesMarked < totalCagesNeeded; ++i)
        {
            const StringID& levelTag = levels[i].m_tag;
            const Path& levelPath = levels[i].m_path;

            if (levelTag.isValid() && !levelPath.isEmpty())
            {
                Ray_PersistentGameData_Level* levelData = static_cast<Ray_PersistentGameData_Level*>(universe->getLevel(levelTag));
                if (levelData)
                {
                    u32 remaining = totalCagesNeeded - cagesMarked;
                    u32 cagesToMark = (remaining < 32) ? remaining : 32;
                    for (u32 cageIndex = 0; cageIndex < cagesToMark; ++cageIndex)
                    {
                        levelData->setBrokenCageFlag(levelPath, cageIndex, btrue);
                    }
                    cagesMarked += cagesToMark;
                }
            }
        }
        LOG("[CHEAT] - Max currency (Electoons = %d)", cagesMarked);
    }
#endif
    void Ray_GameManager::onMapLoadedUpdateTimeAttack()
    {
        if (!RAY_GAMEMANAGER->isLevelCompletedOnce(RAY_GAMEMANAGER->getCurrentLevelName())
#ifdef ITF_SUPPORT_CHEAT
            && !RAY_CHEATMANAGER->getIsCheatTimeAttack()
#endif // #ifdef ITF_SUPPORT_CHEAT
        )
        {
            disableTimeAttackForLevel();
        }
        else
        {
            // The flag doesn't get enabled until the time attack starts
            Actor* flag = m_timeAttackActorFlag.getActor();

            if (flag)
            {
                flag->disable();
            }
        }

        disableTimeAttackHUD();
    }

    void Ray_GameManager::onCheckpointLoadUpdateTimeAttack()
    {
        if (isTimeAttack())
        {
            Actor* firstCheckpoint = getFirstCheckpoint(getCurrentWorld());

            if (firstCheckpoint && m_currentCheckpoint == firstCheckpoint->getRef())
            {
                // If we are at the first checkpoint we stop the time attack
                stopTimeAttack();
                disableTimeAttackHUD();

                Actor* flag = m_timeAttackActorFlag.getActor();

                if (flag)
                {
                    flag->disable();
                }
            }
            else
            {
                // If we continue in time attack mode we disable the chrono
                Actor* chrono = m_timeAttackActorChrono.getActor();

                if (chrono)
                {
                    chrono->disable();
                }
            }
        }
        else
        {
            if (!RAY_GAMEMANAGER->isLevelCompletedOnce(RAY_GAMEMANAGER->getCurrentLevelName())
#ifdef ITF_SUPPORT_CHEAT
                && !RAY_CHEATMANAGER->getIsCheatTimeAttack()
#endif // #ifdef ITF_SUPPORT_CHEAT
            )
            {
                // No time attack in this level
                disableTimeAttackForLevel();
            }
            else
            {
                Actor* firstCheckpoint = getFirstCheckpoint(getCurrentWorld());

                if (firstCheckpoint && m_currentCheckpoint != firstCheckpoint->getRef())
                {
                    disableTimeAttackForLevel();
                }
                else
                {
                    Actor* flag = m_timeAttackActorFlag.getActor();

                    if (flag)
                    {
                        flag->disable();
                    }
                }
            }
        }
    }

    void Ray_GameManager::onMapLoadedUpdateMurphyAssist()
    {
        StringID currentLevel = RAY_GAMEMANAGER->getCurrentLevelName();
        i32 takenRelic = RAY_GAMEMANAGER->getTakenRelicCount(currentLevel);
        i32 maxRelic = RAY_GAMEMANAGER->getMaxRelics(currentLevel);

        i32 maxHiddenCage = Max((i32)RAY_GAMEMANAGER->getLevelTotalCageCount(currentLevel) - 1, 0);
        i32 brokenHiddenCage = Max((i32)RAY_GAMEMANAGER->getLevelBrokenCageCount(currentLevel) - 1, 0);

        bbool canEnable = RAY_GAMEMANAGER->isLevelCompletedOnce(currentLevel)
            && (maxRelic > 0 || maxHiddenCage > 0)
            && (maxRelic > takenRelic || maxHiddenCage > brokenHiddenCage);

        if (canEnable)
        {
            enableMurphyAssistForLevel();
        }
        else
        {
            disableMurphyAssistForLevel();
        }
    }

    void Ray_GameManager::onCheckpointLoadUpdateMurphyAssist()
    {
        if (RAY_GAMEMANAGER->isLevelCompletedOnce(RAY_GAMEMANAGER->getCurrentLevelName()))
        {
            setMurphyAssistFollowPlayer();
        }
    }

    void Ray_GameManager::checkpointLoadScene()
    {
        Actor* checkpoint = AIUtils::getActor(m_currentCheckpoint);
        FixedArray<Scene*, 32> scenesToReload;

        if (checkpoint)
        {
            LinkComponent* linkComponent = checkpoint->GetComponent<LinkComponent>();

            if (linkComponent)
            {
                const LinkComponent::ChildrenList& children = linkComponent->getChildren();
                u32 numChildren = children.size();

                for (u32 i = 0; i < numChildren; i++)
                {
                    const ChildEntry& child = children[i];

                    if (child.hasTag(ITF_GET_STRINGID_CRC(checkpointLoadScene, 2525935102)))
                    {
                        const ObjectPath& path = child.getPath();
                        Pickable* target;

                        if (path.getIsAbsolute())
                        {
                            target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                        }
                        else
                        {
                            target = SceneObjectPathUtils::getObjectFromRelativePath(checkpoint, path);
                        }

                        if (target)
                        {
                            SubSceneActor* subSceneActor = target->DynamicCast<SubSceneActor>(
                                ITF_GET_STRINGID_CRC(SubSceneActor, 1336151817));

                            if (subSceneActor)
                            {
                                Scene* scene = subSceneActor->getSubScene();

                                if (scene)
                                {
                                    scenesToReload.push_back(scene);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (scenesToReload.empty())
        {
            Super::checkpointLoadScene();
        }
        else
        {
            u32 numScenes = scenesToReload.size();

            for (u32 i = 0; i < numScenes; i++)
            {
                Scene* scene = scenesToReload[i];

                // Make sure we don't reload scenes inside scenes unnecesarily
                Actor* subSceneActor = scene->getSubSceneActorHolder();
                bbool found = bfalse;

                if (subSceneActor)
                {
                    Scene* parentScene = subSceneActor->getScene();

                    while (parentScene && !found)
                    {
                        for (u32 sceneIndex = 0; sceneIndex < numScenes; sceneIndex++)
                        {
                            if (sceneIndex != i && parentScene == scenesToReload[sceneIndex])
                            {
                                // This scene is already contained inside another scene that is going to be reloaded,
                                // so we can skip it
                                found = btrue;
                                break;
                            }
                        }

                        subSceneActor = parentScene->getSubSceneActorHolder();
                        parentScene = subSceneActor ? subSceneActor->getScene() : NULL;
                    }
                }

                if (!found)
                {
                    scenesToReload[i]->forcedActorsLoadCheckpoint();
                }
            }
        }
    }

    bbool Ray_GameManager::loadGameplayMap(const Path& _map, bbool _unloadBaseMap, bbool _isReload, ObjectRef _worldRef)
    {
        if (_map == getWorldMap())
        {
            // always reset volatile per-player power-ups on return to world map (RO-11293)
            m_powerUpManager.resetVolatilePerPlayerData();
            return postGameScreenChange<Ray_GameScreen_WorldMap>(bfalse);
        }

        if (_isReload)
        {
            // always reset heart shields on reload (RO-9463)
            for (u32 i = 0; i < s_maxPlayers; ++i)
            {
                m_powerUpManager.setEnabled(Ray_PowerUp_HeartShield, i, bfalse);
            }
        }

        mapLoadResetInfo();


        Ray_GameScreen_Gameplay::InitializationParameters initData;
        initData.m_mapName = _map;
        initData.m_forceUnloadBaseMap = _unloadBaseMap;
        initData.m_worldRef = _worldRef;
        if (!isLoadingVisible())
            LOADINGSCREEN->show();
        return postGameScreenChange<Ray_GameScreen_Gameplay>(bfalse, &initData);
    }

    bbool Ray_GameManager::switchToGameScreenGameplay(ObjectRef _mapRef)
    {
        if (_mapRef.isValid())
        {
            Ray_GameScreen_Gameplay::InitializationParameters initData;
            initData.m_worldRef = _mapRef;
            return postGameScreenChange<Ray_GameScreen_Gameplay>(bfalse, &initData);
        }

        return bfalse;
    }

    void Ray_GameManager::onBaseMapLoaded()
    {
        Super::onBaseMapLoaded();
    }

    void Ray_GameManager::updateLoading()
    {
        Super::updateLoading();
    }

    void Ray_GameManager::createMainMenuWorld()
    {
        ITF_ASSERT_MSG(m_mainMenuWorld == ITF_INVALID_OBJREF, "Main menu world already created");
        if (m_mainMenuWorld == ITF_INVALID_OBJREF)
        {
            ITF_VECTOR<ObjectPath> prefetch;

            StringID mainMenuBackground = ITF_GET_STRINGID_CRC(menuMainBackground, 3706178635);
            if (!getLevelPath(mainMenuBackground).isEmpty())
            {
                m_mainMenuWorld = preLoadMap(getLevelPath(mainMenuBackground), prefetch);
                ITF_ASSERT(m_mainMenuWorld.isValid());
            }
        }
    }

    void Ray_GameManager::commitLevelStateToPersistentUniverse()
    {
        const StringID& currentLevelTag = getCurrentLevelName();
        ITF_WARNING(NULL, currentLevelTag != StringID::Invalid, "Nothing will be saved as it's not a valid map");

        if (currentLevelTag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* level = (Ray_PersistentGameData_Level*)m_persistentGameData->getLevel(
                currentLevelTag);
            level->setFrom(&m_currentLevelData, bfalse, bfalse);

            static_cast<Ray_PersistentGameData_Universe*>(m_persistentGameData)->setScore(m_currentScore);
        }
    }

    void Ray_GameManager::commitFrescoStateToPersistentUniverse()
    {
        const StringID& currentLevelTag = getCurrentLevelName();
        ITF_WARNING(NULL, currentLevelTag != StringID::Invalid, "Nothing will be saved as it's not a valid map");

        if (currentLevelTag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* level = (Ray_PersistentGameData_Level*)m_persistentGameData->getLevel(currentLevelTag);
            level->setFoundRelicMaskFrom(&m_currentLevelData);
        }
    }

    void Ray_GameManager::makeLevelStateFromPersistentUniverse()
    {
        const StringID& currentLevelTag = getCurrentLevelName();

        if (currentLevelTag != StringID::Invalid)
        {
            Ray_PersistentGameData_Level* level = (Ray_PersistentGameData_Level*)m_persistentGameData->getLevel(
                currentLevelTag);
            m_currentLevelData.setFrom(level);

            m_currentScore = static_cast<Ray_PersistentGameData_Universe*>(m_persistentGameData)->getScore();
            for (u32 playerIndex = 0; playerIndex < s_maxPlayers; playerIndex++)
                m_currentScore.setLums(playerIndex, 0);
            m_currentScore_Checkpoint = m_currentScore;
            m_currentLevelData_Checkpoint.setFrom(level);

            // reset triggered sequence switches
            m_sequenceSwitchesTriggered.clear();
        }

        Ray_PlayerHudScoreComponent::clearAllLumScore();
    }

    void Ray_GameManager::saveCheckpointLevelState()
    {
        m_currentScore_Checkpoint = m_currentScore;
        m_currentLevelData_Checkpoint.setFrom(&m_currentLevelData);
    }

    void Ray_GameManager::loadCheckpointLevelState()
    {
        m_currentScore.setFrom(m_currentScore_Checkpoint);
        m_currentLevelData.setFrom(&m_currentLevelData_Checkpoint,
                                   btrue/*we want to be able to interrupt a cut scene even if was not saved*/,bfalse/* _copyFoundRelicMask */);
    }

    void Ray_GameManager::updateDisconnectedPlayersStatus()
    {
        if (canOptInOptOut())
        {
            for (u32 player = 0; player < getMaxPlayerCount(); player++)
            {
                if (getPlayer(player)->getIsDisconnected())
                {
                    getPlayer(player)->setIsDisconnected(bfalse);

                    bbool triggerSeq = btrue;
                    for (u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
                    {
                        if (i == player)
                        {
                            continue;
                        }

                        Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
                        if (otherPlayer && otherPlayer->getActive() && !otherPlayer->isDead() && !otherPlayer->
                            getIsDisconnected())
                        {
                            triggerSeq = bfalse;
                            break;
                        }
                    }

                    if (triggerSeq)
                    {
                        Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(player));
                        if (otherPlayer && otherPlayer->getActor())
                        {
                            triggerRestoreDeath(otherPlayer->getActor()->getRef());
                        }
                    }

                    changePlayerActivation(player, bfalse);
                }
            }
        }
    }

    void Ray_GameManager::update(f32 _dt)
    {
        Super::update(_dt);

        // Update game option persistence (async save/load operations)
        if (m_gameOptionPersistence)
            m_gameOptionPersistence->update();

        if (m_preloadedPrologueWorld.isValid())
            m_preloadedPrologueReady = GAMEMANAGER->isPreloadedMapLoaded(m_preloadedPrologueWorld);

        if (_dt)
        {
            u32 seqIndex = 0;
            u32 playingSeqs = m_currentSequences.size();

            while (seqIndex < playingSeqs)
            {
                Ray_GameManagerSequenceBase* seq = m_currentSequences[seqIndex];

                if (!m_isInPause || (m_isInPause && !seq->isSensitiveToPause()))
                {
                    seq->update(_dt);

                    if (seq->isFinished())
                    {
                        m_currentSequences.eraseKeepOrder(seqIndex);
                        playingSeqs--;
                    }
                    else
                    {
                        seqIndex++;
                    }
                }
                else
                {
                    seqIndex++;
                }
            }
        }

        updateDisconnectedPlayersStatus();

        updateInputConfig();

        // update power ups lifetime
        m_powerUpManager.update(_dt);

        // update reward render/activity
        m_rewardManager.update();

        updateBubblizerAchievement();

        if (m_isTimeAttack &&
            !m_isTimeAttackFinished &&
            !m_isInPause &&
            CINEMATICMANAGER->getLockPlayers() == 0 &&
            m_currentSequences.size() == 0)
        {
            m_timeAttackTimer = Min(m_timeAttackTimer + LOGICDT, 600.f);
        }

#ifdef ITF_SUPPORT_ONLINETRACKING
        // update onlineTracking manager
        m_onlineTrackingManager.update(_dt);
#endif // ITF_SUPPORT_ONLINETRACKING
        if (!m_isInPause && _dt > 0.0f)
        {
            for (u32 i = 0; i < getMaxPlayerCount(); ++i)
            {
                Ray_Player* player = static_cast<Ray_Player*>(getPlayer(i));
                if (player && player->getActive() && player->getHeartTier() == HeartTier_Diamond)
                {
                    player->updateConsecutiveHitTimer(_dt);
                }
            }
        }

#ifdef ITF_SUPPORT_CHEAT //CASA::VITA::TARIK::FRESCO GAMEPLAY
        if (RAY_CHEATMANAGER->getActivateDebugFresco())
        {
            getPersistentGameData()->DebugRelicsInfo();

            InputAdapter::PressStatus buttons[JOY_MAX_BUT];
            f32 axes[JOY_MAX_AXES];
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, 0, buttons, JOY_MAX_BUT);
            INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentEngine, 0, axes, JOY_MAX_AXES);

            if (axes[m_joyTrigger_Right] > 0.f && axes[m_joyTrigger_Left] > 0.f)
            {
                if (buttons[m_joyButton_DPadU] == InputAdapter::JustPressed)
                {
                    u32 frescoId = getPersistentGameData()->GetDebugFrescoId();
                    getPersistentGameData()->UpdateInGameRelicsInfo(frescoId);
                }
                //else if (buttons[m_joyButton_DPadD] == InputAdapter::JustPressed)
                //{
                //    u32 frescoId = 0;
                //    u32 frescoRandomValue = Seeder::getSharedSeeder().GetU32(100);
                //    if (frescoRandomValue > 50)
                //    {
                //        frescoId = 1;
                //    }
                //    u32 relicId = 0;
                //    getPersistentGameData()->UpdateNearRelicsInfo(frescoId, relicId);
                //}
                else if (buttons[m_joyButton_DPadR] == InputAdapter::JustPressed)
                {
                    getPersistentGameData()->ReinitAllInfo();
                }
            }
        }
#endif
    }

    u32 Ray_GameManager::getElectoonsCount() const
    {
        return static_cast<Ray_PersistentGameData_Universe*>(m_persistentGameData)->computeElectoonCount(
            &m_currentLevelData, getCurrentLevelName());
    }

    u32 Ray_GameManager::computeCompleteMedalCount() const
    {
        u32 result = 0;
        //for each level in level list,
        const ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>& mapConfigs = m_configTemplate->getLevelsInfo();

        ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>::const_iterator mapIter;
        SafeArray<u32> alreadyFoundMaps;
        alreadyFoundMaps.reserve(mapConfigs.size());

        for (mapIter = mapConfigs.begin(); mapIter != mapConfigs.end(); mapIter++)
        {
            //find its mission and electoon count
            const Ray_GameManagerConfig_Template::MapConfig& mapConfig = *mapIter;
            const StringID& tag = mapConfig.m_tag;

            //fail safe : check map was not declared twice
            if (alreadyFoundMaps.find(tag.GetValue()) >= 0)
            {
                ITF_WARNING(NULL, bfalse, "duplicate map configuration for %s!", tag.getDebugString());
                continue;
            }
            alreadyFoundMaps.push_back(tag.GetValue());

            const Ray_GameManagerConfig_Template::MissionConfig* missionConfig = findMissionConfigByType(
                mapConfig.m_type, bfalse);

            if (missionConfig && (missionConfig->m_medalSlots.size() > 0))
            {
                u32 electoonCount = 0;

                if (tag == getCurrentLevelName() && getCurrentLevelName().isValid())
                //special case if it's current level
                {
                    electoonCount = m_currentLevelData.computeElectoonCount();
                }
                else
                {
                    Ray_PersistentGameData_Level* level = (Ray_PersistentGameData_Level*)m_persistentGameData->
                        getLevelConst(tag);
                    if (level)
                    {
                        electoonCount = level->computeElectoonCount();
                    }
                }
                if (electoonCount == missionConfig->m_medalSlots.size())
                {
                    result++;
                }
            }
        }

        return result;
    }

    void Ray_GameManager::spawnMedal(const StringID& _levelName)
    {
        const StringID& type = getMapConfigType(_levelName);

        unspawnMedal();
        if (!type.isValid())
            return;

        const Ray_GameManagerConfig_Template::MissionConfig* config = findMissionConfigByType(type, btrue);
        Actor* medal = NULL;

        ITF_WARNING(NULL, config, "Cannot spawn medal because map config type was not found");
        if (config)
        {
            ActorsManager::SpawnInfo spawnInfo;

            World* medalWorld = IRTTIObject::SafeDynamicCast<World>(getMedalWorldRef().getObject(),
                                                                    ITF_GET_STRINGID_CRC(World, 3307309411));
            Scene* medalScene = NULL;
            if (medalWorld)
            {
                medalScene = medalWorld->getRootScene();
            }

            ITF_WARNING_CATEGORY(GPP, NULL, medalScene, "Cannot find MedalScene");
            if (medalScene)
            {
                if (!config->m_medalPath.isEmpty())
                {
                    if (config->m_medalPath.getStringID() == m_currentMedal)
                        return;
                    spawnInfo.m_pScene = medalScene;
                    spawnInfo.m_luaPath = config->m_medalPath;
                    spawnInfo.m_init = btrue;
                    spawnInfo.m_serializable = btrue; // so that we can save on checkpoints

                    medal = ACTORSMANAGER->spawnActor(spawnInfo);

                    ITF_WARNING(NULL, medal, "cannot spawn medal with path=%s",
                                spawnInfo.m_luaPath.getString8().cStr());
                    if (medal)
                    {
                        medal->loadResources();
                        m_currentMedal = config->m_medalPath.getStringID();
                    }
                }
            }
        }

        setMedalActor(medal);
    }

    void Ray_GameManager::unspawnMedal()
    {
        Actor* medal = RAY_GAMEMANAGER->getMedalActor();
        if (medal)
        {
            medal->requestDestruction();
        }
        RAY_GAMEMANAGER->setMedalActor(NULL);
    }

    u32 Ray_GameManager::setSequence(Ray_GameManagerSequenceBase* _sequence)
    {
        ITF_ASSERT(_sequence);
        i32 index = m_currentSequences.find(_sequence);

        // sequence not found
        if (index < 0)
        {
            // Array is full
            if (m_currentSequences.size() >= (i32)m_currentSequences.capacity())
            {
                Ray_GameManagerSequenceBase* seq = m_currentSequences[0];
                if (seq) seq->stop();
                m_currentSequences.eraseNoOrder(0);
            }

            // push the sequence in array
            m_currentSequences.push_back(_sequence);
            return m_currentSequences.size() - 1;
        }

        return (u32)index;
    }

    u32 Ray_GameManager::isPlayingSeq(const Ray_GameManagerSequenceBase* _sequence) const
    {
        u32 numSeqs = m_currentSequences.size();

        for (u32 i = 0; i < numSeqs; i++)
        {
            if (m_currentSequences[i] == _sequence)
            {
                return i;
            }
        }

        return U32_INVALID;
    }


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::onReachedCheckpoint(ObjectRef _checkpoint, bbool _force)
    {
        if (isTimeAttack())
        {
            // On time attack we ignore every checkpoint except the first
            ObjectRef currentWorld = getCurrentWorld();
            Actor* chk = getFirstCheckpoint(currentWorld);

            if (chk && _checkpoint != chk->getRef())
            {
                return;
            }
        }

        GameManager::onReachedCheckpoint(_checkpoint);

        // Reset skip counter as we changed checkpoint
        resetSkipCounter(_checkpoint);

        if (m_currentCheckpoint == _checkpoint && !_force)
        {
            return;
        }

        m_currentCheckpoint = _checkpoint;

        Actor* actor = AIUtils::getActor(_checkpoint);

        if (actor)
        {
            // update current chekpoint data
            setCurrentCheckpoint(actor);

            CheckpointComponent* checkpoint = actor->GetComponent<CheckpointComponent>();
            if (checkpoint)
            {
                m_nextOperation = GAMEOPERATION_SAVECHECKPOINT;

                //reactivate all other checkpoints
                for (WorldCheckpointList::iterator it = m_checkpoints.begin(); it != m_checkpoints.end(); ++it)
                {
                    if (it->second != _checkpoint)
                    {
                        Actor* actor = static_cast<Actor*>(GETOBJECT(it->second));
                        if (actor)
                        {
                            EventActivateCheckpoint evt(btrue);
                            actor->onEvent(&evt);
                        }
                    }
                }

                //deactivate current checkpoint
                EventActivateCheckpoint evt(bfalse);
                actor->onEvent(&evt);
            }
        }

        storeRescuedFriendlies();

#ifdef ITF_SUPPORT_ONLINETRACKING
        m_onlineTrackingManager.onReachedCheckpoint();
#endif // ITF_SUPPORT_ONLINETRACKING
    }

    void Ray_GameManager::endOfLevelReached(bbool _saveGameState /*= btrue*/, bbool _levelCompleted /*= btrue*/)
    {
        Super::endOfLevelReached(_saveGameState, _levelCompleted);

        REWARD_MANAGER->Session_Stop();

        if (_levelCompleted)
        {
            //get map config
            StringID tag = getCurrentLevelName();
            const Ray_GameManagerConfig_Template::MapConfig* const mapConfig = RAY_GAMEMANAGER->getMapConfig(tag);
            const bbool rewardsEnabled = (mapConfig != NULL) && !mapConfig->m_disableRewards;

            // mark level as completed if not done already
            if (tag.isValid())
            {
                // Mark current level as completed full run
                setCurrentLevelIsSkipped(bfalse);

                ESpot_State state = getCurrentLevelState();
                if (state != SPOT_STATE_COMPLETED)
                {
                    setCurrentLevelState(SPOT_STATE_COMPLETED);

                    // after a completed shooter map
                    // - mark the current world as completed too
                    // - go to the *world* map (= invalidate current WM level)
                    if (isLastLevel(tag))
                    {
                        setWMSpotState(getWMCurrentWorldTag(), SPOT_STATE_COMPLETED);
                        setWMCurrentLevel(ObjectPath(), StringID::Invalid);

                        // Check for rewards
                        REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Event_CompleteWorld,
                                                           getWMCurrentWorldTag());
                    }
                }
                else
                {
                    if (rewardsEnabled)
                    {
                        // We are replaying the map
                        // Assume only the first index needs this tag
                        REWARD_MANAGER->Action_New(Ray_RewardManager::Action_ReplayingAMap,
                                                   GAMEMANAGER->getMainIndexPlayer());
                    }
                }
            }

            if (rewardsEnabled)
            {
                Ray_Player* player = static_cast<Ray_Player*>(RAY_GAMEMANAGER->getPlayer(
                    GAMEMANAGER->getMainIndexPlayer()));

                if (player)
                {
                    if (!player->getWasHitInLevel())
                    {
                        REWARD_MANAGER->Action_New(Ray_RewardManager::Action_FinishLevelNoHit,
                                                   GAMEMANAGER->getMainIndexPlayer());
                    }

                    if (!player->getSprintWasReleased())
                    {
                        REWARD_MANAGER->Action_New(Ray_RewardManager::Action_FinishLevelSprintNotReleased,
                                                   GAMEMANAGER->getMainIndexPlayer());
                    }
                }

                REWARD_MANAGER->Action_GlobalEvent(Ray_RewardManager::Event_EndOfLevel);

                const PlayerIDInfo* playerIdInfo = RAY_GAMEMANAGER->getPlayerIDInfoFromPlayerIndex(
                    GAMEMANAGER->getMainIndexPlayer());
                if (playerIdInfo && RAY_GAMEMANAGER->isCustomPlayerIDInfo(playerIdInfo))
                {
                    REWARD_MANAGER->Action_New(Ray_RewardManager::Action_FinishedLevelWithCostume,
                                               GAMEMANAGER->getMainIndexPlayer());
                }
            }
        }

        if (_saveGameState)
        {
            commitAndSaveGameState();
        }
    }

    void Ray_GameManager::commitAndSaveGameState()
    {
        commitLevelStateToPersistentUniverse();

        saveGameState(bfalse, getCurrentSlotIndex());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Launch preloading of next world used to end each level
    ObjectRef Ray_GameManager::preloadEndLevelMap()
    {
        const Path& nextWorldPath = getEndLevelMap();
        if (!nextWorldPath.isEmpty())
        {
            Vector<ObjectPath> prefetchList;
            m_preloadedEndLevelMapRef = preLoadMap(nextWorldPath, prefetchList);
            return m_preloadedEndLevelMapRef;
        }

        return ITF_INVALID_OBJREF;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Ask to switch to end level map
    void Ray_GameManager::switchToPreloadedEndLevelMap()
    {
        if (m_preloadedEndLevelMapRef.isValid())
        {
            switchToPreloadedMap(m_preloadedEndLevelMapRef);
            m_preloadedEndLevelMapRef.invalidate();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Launch preloading of next world used to end each level
    ObjectRef Ray_GameManager::preloadWorldMap()
    {
        const Path& nextWorldPath = getWorldMap();
        if (!nextWorldPath.isEmpty())
        {
            Vector<ObjectPath> prefetchList;
            m_preloadedWorldMap = preLoadMap(nextWorldPath, prefetchList);
            return m_preloadedWorldMap;
        }

        return ITF_INVALID_OBJREF;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_GameManager::getSavegameMaxSize()
    {
        return SAVEGAME_MAX_SIZE;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::goToTheMainMenu()
    {
        clearNextOperation();
        stopAllSequences();

        u32 seqIndex = isPlayingSeq(&m_endGameSequence);
        if (seqIndex != U32_INVALID)
        {
            m_endGameSequence.stop();
        }

        // fix world map (and other preloads) staying in memory after sign-out (X360
        // only - RO-17325)
        deleteAllPreloadedMaps();

#define USE_PROLOGUE_PRELOAD

#ifdef USE_PROLOGUE_PRELOAD
        preloadPrologue();
#endif // USE_PROLOGUE_PRELOAD

#ifdef ITF_WII
        // Reset the main player only on WII to avoid some issues
        setMainIndexPlayer(U32_INVALID);
#endif //ITF_WII
        unspawnMedal();
        postGameScreenChange<Ray_GameScreen_MainMenu>(bfalse);
    }

    void Ray_GameManager::goToLevelStats()
    {
        postGameScreenChange<Ray_GameScreen_LevelStats>(bfalse);
    }

    void Ray_GameManager::goToCheckpointScore()
    {
        postGameScreenChange<Ray_GameScreen_CheckpointScore>(bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /*
    void Ray_GameManager::savePersistentWorldArchives(bbool _newGame)
    {
        ArchiveMemory *desc = new ArchiveMemory();
        const u32 playerIndex = 0;
        desc->reserve(getSavegameMaxSize());

        u32 archiveCount = (u32)m_persistentWorldArchives.size();
        desc->serialize(archiveCount);
        ITF_MAP<String, ArchiveMemory*>::iterator iter;
        for (iter=m_persistentWorldArchives.begin(); iter!=m_persistentWorldArchives.end(); iter++)
        {
            iter->first.serialize(*desc);
            u32 dataSize = (u32)iter->second->getSize();
            desc->serialize(dataSize);
            desc->serializeBlock8(iter->second->getData(), dataSize);
        }
        ITF_ASSERT_MSG((u32)desc->getSize()<GAMEMANAGER->getSavegameMaxSize(), "Savegame is too big");
        SAVEGAME_ADAPTER->startSave(playerIndex, SAVEGAME_BASENAME, "Savegame", desc->getData(), (u32)desc->getSize(), _newGame);
        if (m_theGameScreen)
            m_theGameScreen->onEndOfLevelReached();

        delete desc;
    }
    */
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::loadGameState(const u8* _data, u32 _dataSize)
    {
        ArchiveMemory* desc = newAlloc(mId_Gameplay, ArchiveMemory(_data, _dataSize));

        clearWholeGameState();

        SF_DEL(m_persistentGameData);
        CSerializerObjectBinary serializer(desc);
        m_persistentGameData = newAlloc(mId_Gameplay, Ray_PersistentGameData_Universe);
        serializer.SerializeObject("CONTENT", m_persistentGameData, ESerialize_Data_Load);
        SF_DEL(desc);

        if (getMainIndexPlayer() != U32_INVALID)
        {
            const StringID& playerIDInfo = getPersistentGameData()->getUsedPlayerIDInfo();

            if (playerIDInfo.isValid())
            {
                Player* p = getPlayer(getMainIndexPlayer());

                if (p)
                {
                    u32 numPlayers = m_players.size();

                    for (u32 i = 0; i < numPlayers; i++)
                    {
                        Player* otherP = m_players[i];

                        if (otherP != p && otherP->getCurrentPlayerIDInfo() == playerIDInfo)
                        {
                            otherP->setCurrentPlayerIDInfo(getPlayerIDInfo(p->getCurrentPlayerIDInfo()));
                            break;
                        }
                    }

                    p->setCurrentPlayerIDInfo(getPlayerIDInfo(playerIDInfo));
                }
            }
        }

        if (!m_persistentGameData)
        {
            // Corrupted save ?
            clearWholeGameState();
            return bfalse;
        }

        RewardManager::SaveSession* pRewardsSaveSession = m_persistentGameData->getRewardSession();
        if (!REWARD_MANAGER->setSaveSessionFromSave(pRewardsSaveSession, getMainIndexPlayer()))
        {
            clearWholeGameState();
            return bfalse;
        }

        return btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::getSaveDisplayedGameName(String& _gameName)
    {
        // Progression
        /*f32 f32_progress = getPersistentGameData()->getTotalGameProgress() * 100.f;
        u32 progress  = (u32)floor( f32_progress );*/

        _gameName.setTextFormat("%d/%d [icon:GPE_ELECTOON]   %d/%d [icon:GPE_TOOTH]",
                                getElectoonsCount(),
                                SAVE_MAX_ELEC,
                                getTotalTakenToothCount(),
                                SAVE_MAX_TOOTH);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::getSaveEmptyDisplayedName(String& _gameName)
    {
        _gameName = "empty";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::getSaveBaseName(String8& _baseName, u32 _slot)
    {
        _baseName.setTextFormat("%s_%d", SAVEGAME_BASENAME, _slot);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::saveGameState(bbool _newGame, u32 _slotIndex, bbool _resetSave)
    {
        String gameSaveName;
        u32 dataSize;
        u8* dataPt;
        ArchiveMemory* desc = NULL;

        if (_newGame)
        {
            // Delete previous game session
            SF_DEL(m_persistentGameData);
            m_persistentGameData = newAlloc(mId_Gameplay, Ray_PersistentGameData_Universe);

            u32 numPlayers = m_players.size();

            for (u32 i = 0; i < numPlayers && i < m_configTemplate->getPlayersIDInfo().size(); i++)
            {
                static_cast<Ray_PersistentGameData_Universe*>(m_persistentGameData)->addCostumeUsed(
                    m_configTemplate->getPlayersIDInfo()[i]->getId());
            }
        }

        if (_resetSave)
        {
            getSaveEmptyDisplayedName(gameSaveName);
            dataSize = 0;
            dataPt = NULL;
        }
        else
        {
            if (_newGame)
            {
                setWMCurrentWorld(getTemplate()->getWmStartNode(), s_HomeMap);
            }

            getSaveDisplayedGameName(gameSaveName);

            // backup rewards
            RewardManager::SaveSession* pRewardsSaveSession = m_persistentGameData->getRewardSession();
            REWARD_MANAGER->backupSaveSession(pRewardsSaveSession, getMainIndexPlayer());

            desc = newAlloc(mId_Gameplay, ArchiveMemory());
            desc->reserve(getSavegameMaxSize());
            ITF_ASSERT_CRASH(m_persistentGameData!=NULL, "no persistent game data");
            CSerializerObjectBinary serializer(desc);
            serializer.SerializeObject("CONTENT", m_persistentGameData, ESerialize_Data_Save);

            ITF_WARNING(NULL, (u32)desc->getSize()<GAMEMANAGER->getSavegameMaxSize(),
                        "Savegame is too big (%d/%d bytes) - increase maximum size",
                        desc->getSize(), GAMEMANAGER->getSavegameMaxSize()
            );


            dataPt = desc->getData();
            dataSize = (u32)desc->getSize();

            SAVEGAME_ADAPTER->setDataCRC(computeSaveDataCRC(dataPt, dataSize));
        }

        String8 baseName;
        getSaveBaseName(baseName, _slotIndex);
        Adapter_Savegame::ErrorCode err = Adapter_Savegame::Error_Ok;

        if (SAVEGAME_ADAPTER->IsSaveSystemEnable())
            err = SAVEGAME_ADAPTER->startSave(getMainIndexPlayer(), _slotIndex, baseName, gameSaveName, dataPt,
                                              dataSize, bfalse /* Handle by Menus */);

        SF_DEL(desc);

        if (_newGame)
        {
            REWARD_MANAGER->Reset();
            m_currentSlotIndex = _slotIndex;
        }
        return (err == Adapter_Savegame::Error_Ok);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    /*
    void Ray_GameManager::loadPersistentWorldArchives(const u8 *_data, u32 _dataSize)
    {
        ArchiveMemory *desc = new ArchiveMemory(_data, _dataSize);
        u32 archiveCount = 0;

        clearGameState();

        desc->serialize(archiveCount);
        for (u32 index=0; index<archiveCount; index++)
        {
            String name;
            name.serialize(*desc);
            u32 dataSize = 0;
            desc->serialize(dataSize);

            u8 *buffer = newAlloc(mId_Gameplay, u8[dataSize]);
            desc->serializeBlock8(buffer, dataSize);
            ArchiveMemory *worldArchive = new ArchiveMemory(buffer, dataSize);
            setPersitentWorldArchive(name, worldArchive);
            delete[] buffer;
        }
        delete desc;
    }
    */

    void Ray_GameManager::setSkipLevelActor(ObjectRef _ref)
    {
        m_skipLevelActor = _ref;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::onEvent(Event* _event)
    {
        GameManager::onEvent(_event);

        if (Ray_EventSpawnRewardLum* onSpawnReward = _event->DynamicCast<Ray_EventSpawnRewardLum>(
            ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum, 812488722)))
        {
            SpawnRewardLum(onSpawnReward);
        }
        else if (Ray_EventTakeTooth* onTakeTooth = _event->DynamicCast<Ray_EventTakeTooth>(
            ITF_GET_STRINGID_CRC(Ray_EventTakeTooth, 2760419748)))
        {
            Actor* taker = IRTTIObject::SafeDynamicCast<Actor>(onTakeTooth->getSender().getObject(),
                                                               ITF_GET_STRINGID_CRC(Actor, 2546623115));
            if (taker)
            {
                setIsToothTaken(taker, btrue);
            }
        }
        else if (Ray_EventSpawnRewardHeart* onSpawnReward = _event->DynamicCast<Ray_EventSpawnRewardHeart>(
            ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardHeart, 1546601933)))
        {
            SpawnRewardHeart(onSpawnReward);
        }
        else if (Ray_EventGoToWorldMap* onGoToWorldMap = _event->DynamicCast<Ray_EventGoToWorldMap>(
            ITF_GET_STRINGID_CRC(Ray_EventGoToWorldMap, 3334813547)))
        {
            endOfLevelReached(onGoToWorldMap->getSaveGameState(), onGoToWorldMap->getMarkCurrentLevelAsCompleted());
            resetToWorldMap();
        }
        else if (Ray_EventWorldMapUnlock* onWorldMapUnlock = _event->DynamicCast<Ray_EventWorldMapUnlock>(
            ITF_GET_STRINGID_CRC(Ray_EventWorldMapUnlock, 1931876316)))
        {
            // mark current level as completed
            markWorldMapNodesAsCompleted(onWorldMapUnlock->getUnlocks());

            // Change current level name ?
            if (onWorldMapUnlock->getChangeCurrentLevelName())
            {
                // Change current level state in currentLevelPersistentData
                setCurrentLevelState(SPOT_STATE_COMPLETED);

                // Commit level state before changing current level name
                commitLevelStateToPersistentUniverse();

                m_currentLevelName = onWorldMapUnlock->getLevelName();

                // set world map spot on which we should be if we exit level
                setWMCurrentLevel(getWMSpotObjectPath(m_currentLevelName), m_currentLevelName);

                // Create a new empty level state with the new current level name
                makeLevelStateFromPersistentUniverse();
            }

            // Save game state ?
            if (onWorldMapUnlock->getSaveGameState())
                saveGameState(bfalse, getCurrentSlotIndex(), bfalse);
        }
        else if (Ray_EventEndSequence* onEndSequence = _event->DynamicCast<Ray_EventEndSequence>(
            ITF_GET_STRINGID_CRC(Ray_EventEndSequence, 3991450529)))
        {
            triggerEndGameSequence(onEndSequence->getPlayMovie());
        }
        else if (Ray_EventFxScreen* onEventFxScreen = _event->DynamicCast<Ray_EventFxScreen>(
            ITF_GET_STRINGID_CRC(Ray_EventFxScreen, 362679351)))
        {
            triggerFxScreen(onEventFxScreen);
        }


        u32 numSeqs = m_currentSequences.size();

        for (u32 i = 0; i < numSeqs; i++)
        {
            m_currentSequences[i]->onEvent(_event);
        }
    }

    Vec2d Ray_GameManager::computeRewardEjectionForce(Ray_EventSpawnReward* _onSpawnReward)
    {
        f32 force = _onSpawnReward->getEjectionForce();
        force += Seeder::getSharedSeeder().GetFloat(-_onSpawnReward->getEjectionRandomForce(),
                                                    _onSpawnReward->getEjectionRandomForce());
        Vec2d dir = _onSpawnReward->getEjectionDir();
        f32 angle = Seeder::getSharedSeeder().GetFloat(-_onSpawnReward->getEjectionRandomAngle().Radians(),
                                                       _onSpawnReward->getEjectionRandomAngle().Radians());
        return dir.Rotate(angle) * force;
    }

    void Ray_GameManager::SpawnRewardLum(Ray_EventSpawnRewardLum* onSpawnReward)
    {
        Scene* scene = onSpawnReward->getScene();
        const Vec3d& pos = onSpawnReward->getPos();

        // Autopickup and not valid player -> no reward
        if (onSpawnReward->getAutoPickup() &&
            (onSpawnReward->getRewardReceiver() == ITF_INVALID_OBJREF || !getPlayerFromActor(
                onSpawnReward->getRewardReceiver(), bfalse))
        )
        {
            return;
        }

        if (onSpawnReward->getNumRewards() == 1)
        {
            Actor* instance = m_lumGenerator.getSpawnee(scene, pos);
            if (instance)
            {
                if (onSpawnReward->getEjectionDuration() != 0)
                {
                    Ray_EventLumEjection eject;
                    eject.setup(onSpawnReward->getRewardReceiver(),
                                computeRewardEjectionForce(onSpawnReward),
                                onSpawnReward->getEjectionDuration(),
                                onSpawnReward->getEjectionGravityAngle(),
                                onSpawnReward->getAutoPickup());
                    instance->onEvent(&eject);
                }
                else if (onSpawnReward->getAutoPickup() &&
                    onSpawnReward->getRewardReceiver() != ITF_INVALID_OBJREF)
                {
                    Ray_EventRewardPickedUp pickedUp;
                    pickedUp.setForceYellowLum(onSpawnReward->getForceYellow());
                    pickedUp.setSender(onSpawnReward->getRewardReceiver());
                    pickedUp.setActivator(onSpawnReward->getRewardReceiver());
                    instance->onEvent(&pickedUp);
                }

                onSpawnReward->getRewards().push_back(instance->getRef());
            }
        }
        else
        {
            f32 angleStep = MTH_2PI / onSpawnReward->getNumRewards();
            const f32 radius = m_configTemplate->getLumSpawnRadius();
            f32 mul = 0.0f;
            for (u32 i = 0; i < onSpawnReward->getNumRewards(); ++i)
            {
                Vec3d spawnPos = pos + Vec2d::Up.Rotate(-angleStep * mul).to3d(0.0f) * radius;
                Actor* instance = m_lumGenerator.getSpawnee(scene, spawnPos);
                if (instance)
                {
                    if (onSpawnReward->getEjectionDuration() != 0)
                    {
                        Ray_EventLumEjection eject;
                        eject.setup(onSpawnReward->getRewardReceiver(),
                                    computeRewardEjectionForce(onSpawnReward),
                                    onSpawnReward->getEjectionDuration(),
                                    onSpawnReward->getEjectionGravityAngle(),
                                    onSpawnReward->getAutoPickup());
                        instance->onEvent(&eject);
                    }
                    else if (onSpawnReward->getAutoPickup() &&
                        onSpawnReward->getRewardReceiver() != ITF_INVALID_OBJREF)
                    {
                        // will rotate lums and send them sequentially (RO-5767)
                        ITF_ASSERT_CRASH(i==0, "particle lums : lum ring is handled by 1 lum only");
                        Ray_EventRewardPickedUp pickedUp(
                            pos, // center pos
                            radius, // spawn circle radius
                            i, // rank
                            onSpawnReward->getNumRewards()); // max rank
                        pickedUp.setForceYellowLum(onSpawnReward->getForceYellow());
                        pickedUp.setSender(onSpawnReward->getRewardReceiver());
                        pickedUp.setActivator(onSpawnReward->getRewardReceiver());
                        instance->onEvent(&pickedUp);
                        break; //lum will generate particles
                    }

                    onSpawnReward->getRewards().push_back(instance->getRef());
                }
                mul += 1.0f;
            }
        }
    }

    void Ray_GameManager::SpawnRewardHeart(Ray_EventSpawnRewardHeart* onSpawnReward)
    {
        Scene* scene = onSpawnReward->getScene();
        const Vec3d& pos = onSpawnReward->getPos();

        for (u32 i = 0; i < onSpawnReward->getNumRewards(); ++i)
        {
            Actor* instance = m_heartGenerator.getSpawnee(scene, pos);
            if (instance)
            {
                instance->enable();
                if (onSpawnReward->getAutoPickup() &&
                    onSpawnReward->getRewardReceiver() != ITF_INVALID_OBJREF)
                {
                    Ray_EventRewardPickedUp pickedUp;
                    pickedUp.setSender(onSpawnReward->getRewardReceiver());
                    pickedUp.setActivator(onSpawnReward->getRewardReceiver());
                    instance->onEvent(&pickedUp);
                }
                Vec2d finalForce = computeRewardEjectionForce(onSpawnReward);
                EventAddForce addForce;
                addForce.setForce(finalForce);
                instance->onEvent(&addForce);
                onSpawnReward->getRewards().push_back(instance->getRef());
            }
        }
    }

    void Ray_GameManager::changePlayerActivation(u32 _playerIndex, bbool _startLevel)
    {
        if (m_maxPlayers && _playerIndex >= m_maxPlayers)
        {
            return;
        }

        GameManager::changePlayerActivation(_playerIndex, _startLevel);

        Ray_Player* player = static_cast<Ray_Player*>(getPlayer(_playerIndex));

        player->setIsLeaving(bfalse);

        if (player->getActive())
        {
            // Stats ..
            REWARD_MANAGER->Action_TimerStart(Ray_RewardManager::Timer_IsAlive, _playerIndex);
            updateRichPresenceForPlayer(m_lastRichPresenceIndex, player->getIndex());
        }
        else
        {
            updateRichPresenceForPlayer(m_inactiveRichPresenceIndex, player->getIndex());
        }

        Ray_PlayerHudScoreComponent* playerHud = Ray_PlayerHudScoreComponent::getInstance(_playerIndex);

        if (playerHud)
        {
            if (player->getActive())
            {
                // Put our icon at the end of the queue
                playerHud->setVisualOrder(getNumActivePlayers() - 1);

                // And now the next available after us
                if (m_availablePlayersIndexes.size() > 0)
                {
                    u32 nextAvailable = m_availablePlayersIndexes[0];
                    u32 numPlayers = m_players.size();

                    for (u32 i = 0; i < numPlayers; i++)
                    {
                        Player* p = getPlayer(i);

                        if (p->getOriginalPlayerIDInfoIndex() == nextAvailable)
                        {
                            Ray_PlayerHudScoreComponent* nextPlayerHud = Ray_PlayerHudScoreComponent::getInstance(i);
                            nextPlayerHud->setVisualOrder(getNumActivePlayers());
                            break;
                        }
                    }
                }
            }
            else
            {
                if (m_availablePlayersIndexes.size() > 0)
                {
                    u32 nextAvailable = m_availablePlayersIndexes[0];
                    u32 numPlayers = m_players.size();
                    u32 firstHudPos = playerHud->getVisualOrder();
                    u32 lastVisualOrder = U32_INVALID;

                    // First we shift everyone to the left, from the player that left
                    for (u32 i = 0; i < numPlayers; i++)
                    {
                        Player* p = getPlayer(i);

                        if (p->getActive())
                        {
                            Ray_PlayerHudScoreComponent* hud = Ray_PlayerHudScoreComponent::getInstance(i);

                            u32 visualOrder = hud->getVisualOrder();

                            if (visualOrder > firstHudPos)
                            {
                                visualOrder -= 1;
                                hud->setVisualOrder(visualOrder);
                            }

                            if (lastVisualOrder == U32_INVALID || lastVisualOrder < visualOrder)
                            {
                                lastVisualOrder = visualOrder;
                            }
                        }
                    }

                    // And now let's put the next available at the end of the queue
                    for (u32 i = 0; i < numPlayers; i++)
                    {
                        Player* p = getPlayer(i);

                        if (p->getOriginalPlayerIDInfoIndex() == nextAvailable)
                        {
                            Ray_PlayerHudScoreComponent* hud = Ray_PlayerHudScoreComponent::getInstance(i);
                            hud->setVisualOrder(lastVisualOrder + 1);
                            break;
                        }
                    }
                }
            }
        }

        //manage sound mix
        //only set new bus mix if game is not in pause to not break pause specific bus mix
        //if game is in pause default bus mix will be set on leaving pause
        if (!isInPause())
        {
            setDefaultBusMix();
        }

#ifdef ITF_SUPPORT_ONLINETRACKING
        if (_startLevel == bfalse)
        {
            m_onlineTrackingManager.SendPlayerIntervalTag();
        }
#endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::setDefaultBusMix()
    {
        const StringID s_onePlayer(3929318372); //"onePlayer"
        const StringID s_twoPlayer(1718446935); //"twoPlayer"
        const StringID s_threePlayer(3257508928); //"threePlayer"

        //count active players
        u32 count = GAMEMANAGER->getNumActivePlayers();
        if (count < 2)
        {
            SOUND_MANAGER()->playBusMix(s_onePlayer, btrue);
        }
        else if (count == 2)
        {
            SOUND_MANAGER()->playBusMix(s_twoPlayer, btrue);
        }
        else
        {
            SOUND_MANAGER()->playBusMix(s_threePlayer, btrue);
        }
    }

    void Ray_GameManager::stopDefaultBusMix()
    {
        //count active players
        u32 count = GAMEMANAGER->getNumActivePlayers();
        if (count < 2)
        {
            SOUND_MANAGER()->stopBusMix(ITF_GET_STRINGID_CRC(onePlayer, 3929318372), btrue);
        }
        else if (count == 2)
        {
            SOUND_MANAGER()->stopBusMix(ITF_GET_STRINGID_CRC(twoPlayer, 1718446935), btrue);
        }
        else
        {
            SOUND_MANAGER()->stopBusMix(ITF_GET_STRINGID_CRC(threePlayer, 3257508928), btrue);
        }
    }

    void Ray_GameManager::setPauseBusMix()
    {
        SOUND_MANAGER()->playBusMix(SOUND_BUS_MIX_PAUSE, btrue);
    }

    void Ray_GameManager::stopPauseBusMix()
    {
        SOUND_MANAGER()->stopBusMix(SOUND_BUS_MIX_PAUSE, btrue);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::onBeginToLoadMap()
    {
        m_storedFriendliesRescued.clear();
        m_friendliesRescued.clear();
        m_totalRescuedFriendlies.clear();
        setIsTimeAttack(bfalse);
    }

    void Ray_GameManager::addHitPoints(u32 _playerIndex, i32 _numHP, bbool _evenIfInvincible, bbool _broadcastEvent)
    {
        Ray_Player* player = static_cast<Ray_Player*>(getPlayer(_playerIndex));
        ITF_ASSERT(player->getActive());

        const u32 prevHP = player->getHitPoints();

        if (_numHP > 0)
        {
            // add HP
            player->setHitPoints(Min(player->getHitPoints() + _numHP, player->getMaxHitPoints()));

            EHealthModifier modifier = getHealthModifier();
            if (modifier == HealthModifier_GoldenHeart && player->getHeartTier() == HeartTier_Red)
            {
                player->setHeartTier(HeartTier_Golden);
            }
        }
        else if (_evenIfInvincible || !player->getInvincible())
        {
            if (player->getHeartTier() == HeartTier_Diamond)
            {
                player->onTakeDamage();
            }
            else
            {
                i32 newHP = static_cast<i32>(player->getHitPoints()) + _numHP;
                if (newHP > 0)
                {
                    // remove HP
                    player->setHitPoints(static_cast<u32>(newHP));
                }
                else
                {
                    // no more HP
                    player->setHitPoints(0);
                }

                if (player->getHitPoints() < prevHP)
                {
                    player->onTakeDamage();
                }
            }
        }


        if (_broadcastEvent)
        {
            const u32 newHP = player->getHitPoints();
            if (newHP != prevHP)
            {
                EventPlayerHpChanged eventHP;
                eventHP.setPlayer(player);
                eventHP.setPreviousHP(prevHP);
                eventHP.setJustDied(newHP == 0);
                eventHP.setJustRevived(prevHP == 0);

                EVENTMANAGER->broadcastEvent(&eventHP);
            }
        }
    }

    bbool Ray_GameManager::heartNeeded()
    {
        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            Ray_Player* player = static_cast<Ray_Player*>(m_players[i]);
            if (player->heartNeeded())
            {
                return btrue;
            }
        }
        return bfalse;
    }

    void Ray_GameManager::pickupLums(Actor* _obj, bbool _pickedUp)
    {
        World* world = _obj->getWorld();
        if (!world)
            return;
        Scene* scene = world->getRootScene();
        if (!scene)
            return;

        ITF_ASSERT(_pickedUp);

        if (_obj->isSpawned())
        {
            return;
        }
        PackedObjectPath objPath(_obj);
        m_currentLevelData.setTakenLum(scene->getPath(), objPath, _pickedUp);
    }

    bbool Ray_GameManager::isLumPickedUp(Actor* _obj) const
    {
        World* world = _obj->getWorld();
        if (!world)
            return bfalse;
        Scene* scene = world->getRootScene();
        if (!scene)
            return bfalse;

        if (_obj->isSpawned())
        {
            return bfalse;
        }
        PackedObjectPath objPath(_obj);
        return m_currentLevelData.isLumTaken(scene->getPath(), objPath);
    }

    void Ray_GameManager::fxFade(const Color& _from, const Color& _to, f32 _duration, bbool _removeOnFinished)
    {
        m_fxHandler.startFade(_from, _to, _duration, _removeOnFinished);
    }

    u32 Ray_GameManager::findRescuedFriendly(ObjectRef _ref, const FriendliesList& _list) const
    {
        u32 numTotal = _list.size();

        for (u32 i = 0; i < numTotal; i++)
        {
            if (_list[i].m_ref == _ref)
            {
                return i;
            }
        }

        return U32_INVALID;
    }

    void Ray_GameManager::addFriendlyRescued(ObjectRef _ref, bbool _teleport)
    {
        FriendlyRescuedInfo info;

        info.m_ref = _ref;
        info.m_teleport = _teleport;

        if (findRescuedFriendly(_ref, m_friendliesRescued) == U32_INVALID)
        {
            m_friendliesRescued.push_back(info);
        }

        if (findRescuedFriendly(_ref, m_totalRescuedFriendlies) == U32_INVALID)
        {
            m_totalRescuedFriendlies.push_back(info);

            if (getCurrentLevelName() == s_OC_A1)
            {
                REWARD_MANAGER->resetTotalOf(Ray_RewardManager::Action_GetRescuedRedWizardsOceanA1,
                                             GAMEMANAGER->getMainIndexPlayer());

                const Ray_AIRescueManager::RegisteredActorsList& rescueActor = RAY_AIRESCUEMANAGER->
                    getRegisteredActors();
                u32 numRescue = rescueActor.size();
                bbool allRescued = btrue;

                for (u32 i = 0; i < numRescue; i++)
                {
                    Actor* actor = rescueActor[i].getActor();

                    if (actor)
                    {
                        Ray_EventQueryIsRescued eventIsRescued;

                        actor->onEvent(&eventIsRescued);

                        if (!eventIsRescued.getIsRescued())
                        {
                            allRescued = bfalse;
                            break;
                        }
                    }
                }

                if (allRescued)
                {
                    REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetRescuedRedWizardsOceanA1,
                                               GAMEMANAGER->getMainIndexPlayer());
                }
            }
        }
    }

    void Ray_GameManager::teleportRescuedFriendlies(Ray_EventTeleportUsed* _event)
    {
        u32 numFriendlies = m_totalRescuedFriendlies.size();

        for (u32 i = 0; i < numFriendlies; i++)
        {
            const FriendlyRescuedInfo& info = m_totalRescuedFriendlies[i];

            if (info.m_teleport)
            {
                Actor* friendly = AIUtils::getActor(info.m_ref);

                if (friendly)
                {
                    friendly->onEvent(_event);
                }
            }
        }
    }

    void Ray_GameManager::storeRescuedFriendlies()
    {
        u32 numFriendlies = m_friendliesRescued.size();

        for (u32 i = 0; i < numFriendlies; i++)
        {
            const FriendlyRescuedInfo& info = m_friendliesRescued[i];

            if (findRescuedFriendly(info.m_ref, m_storedFriendliesRescued) == U32_INVALID)
            {
                m_storedFriendliesRescued.push_back(info);
            }
        }

        m_friendliesRescued.clear();
    }

    void Ray_GameManager::resetRescuedFriendlies()
    {
        m_friendliesRescued.clear();
        m_storedFriendliesRescued.clear();
        m_totalRescuedFriendlies.clear();
    }

    void Ray_GameManager::resetRescuedFriendlyTeleport(ObjectRef _ref)
    {
        u32 foundIndex = findRescuedFriendly(_ref, m_friendliesRescued);

        if (foundIndex != U32_INVALID)
        {
            m_friendliesRescued[foundIndex].m_teleport = bfalse;
        }

        foundIndex = findRescuedFriendly(_ref, m_storedFriendliesRescued);

        if (foundIndex != U32_INVALID)
        {
            m_storedFriendliesRescued[foundIndex].m_teleport = bfalse;
        }

        foundIndex = findRescuedFriendly(_ref, m_totalRescuedFriendlies);

        if (foundIndex != U32_INVALID)
        {
            m_totalRescuedFriendlies[foundIndex].m_teleport = bfalse;
        }
    }

    void Ray_GameManager::resetPlayerGlobalPowers()
    {
        m_powerUpManager.resetGlobalPowerups();

        const ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>& levelsInfo = m_configTemplate->getLevelsInfo();
        u32 numLevels = levelsInfo.size();

        for (u32 i = 0; i < numLevels; i++)
        {
            const Ray_GameManagerConfig_Template::MapConfig& mapConfig = levelsInfo[i];

            if (mapConfig.m_powerUnlocked.isValid())
            {
                if (getWMSpotState(mapConfig.m_tag) == SPOT_STATE_COMPLETED)
                {
                    m_powerUpManager.setEnabled(mapConfig.m_powerUnlocked, btrue);
                }
            }
        }
    }

    void Ray_GameManager::onLoadMapReached(const ObjectPath& _path, bbool _victory, bbool _goToWorldMap)
    {
        if (_victory)
        {
            m_victorySequence.start(_path, _goToWorldMap);
            setSequence(&m_victorySequence);
        }
        else
        {
            if (_goToWorldMap)
            {
                m_theGameScreen->onEndOfLevelReached();
            }
            else
            {
                String8 mapName;
                Ray_GameScreen_Gameplay::InitializationParameters data;

                SceneObjectPathUtils::getWorldFromAbsolutePath(_path, mapName);

                data.m_checkPointPath = _path;
                data.m_mapName = mapName;

                postGameScreenChange<Ray_GameScreen_Gameplay>(btrue, &data);
            }
        }
    }

    void Ray_GameManager::onHiddenAreaReached(ObjectRef _start, ObjectRef _end, ObjectRef _player)
    {
        u32 seqIndex = isPlayingSeq(&m_hiddenAreaSequence);

        if (seqIndex == U32_INVALID)
        {
            m_hiddenAreaSequence.start(_start, _end, m_configTemplate->getHiddenAreaWaitPlayersLength(),
                                       m_configTemplate->getHiddenAreaWaitLoadLength());
            seqIndex = setSequence(&m_hiddenAreaSequence);
        }

        if (_start == m_hiddenAreaSequence.getStart() &&
            _end == m_hiddenAreaSequence.getEnd())
        {
            m_hiddenAreaSequence.addPlayerEntering(_player);
        }
    }

    void Ray_GameManager::triggerRestoreDeath(ObjectRef _playerRef)
    {
        u32 seqIndex = isPlayingSeq(&m_restoreDeath);

        if (seqIndex == U32_INVALID)
        {
            m_restoreDeath.start();
            setSequence(&m_restoreDeath);
        }
    }

    void Ray_GameManager::playSeqActor(const Path& _act, const Vec3d& _pos, Scene* _scene)
    {
        u32 seqIndex = isPlayingSeq(&m_restoreDeath);

        if (seqIndex == U32_INVALID)
        {
            m_playSeqActor.start();
            setSequence(&m_playSeqActor);
        }

        m_playSeqActor.addSequence(_act, _pos, _scene);
    }

    void Ray_GameManager::triggerCheckpointSequence(ObjectRef _checkpoint, ObjectRef _player, const Path& _sequence)
    {
        // dead players cannot trigger the sequence
        if (Ray_Player* player = static_cast<Ray_Player*>(getPlayerFromActor(_player)))
        {
            if (!player->getHitPoints())
            {
                return;
            }
        }

        u32 seqIndex = isPlayingSeq(&m_checkpointSequence);

        if (seqIndex == U32_INVALID)
        {
            m_checkpointSequence.start(_checkpoint, _sequence, m_configTemplate->getCheckpointSeqWaitPlayersLength());
            setSequence(&m_checkpointSequence);
        }

        m_checkpointSequence.addPlayerSaving(_player);
    }

    void Ray_GameManager::resumeCheckpointSequence()
    {
        m_checkpointSequence.resume();
    }

    bbool Ray_GameManager::isPlayerInSequence(ObjectRef _player) const
    {
        for (u32 i = 0; i < m_currentSequences.size(); i++)
        {
            Ray_GameManagerSequenceBase* sequence = m_currentSequences[i];
            if (sequence->isPlayerInSequence(_player))
            {
                return btrue;
            }
        }

        return bfalse;
    }

    void Ray_GameManager::triggerChangePageSequence(Vec3d const& _enterPoint,
                                                    Vec3d const& _exitPoint,
                                                    Vec3d const& _finalPoint,
                                                    bbool _verticalEjectToFinalPoint,
                                                    ObjectRef _player,
                                                    ActorRef _teleportPage,
                                                    ActorRef _destinationPage,
                                                    ActorRef _checkpoint,
                                                    f32 _waitPlayersDuration /*=0.f*/,
                                                    bbool _useFade, /*= bfalse */
                                                    bbool _isCageMap
    )
    {
        u32 seqIndex = isPlayingSeq(&m_changePageSequence);


    if (seqIndex == U32_INVALID)
    {
        m_changePageSequence.start(_enterPoint,
                                   _exitPoint,
                                       _finalPoint,
                                       _verticalEjectToFinalPoint,
                                       _player,
                                       _teleportPage,
                                       _destinationPage,
                                       _checkpoint,
                                       _waitPlayersDuration,
                                       _useFade,
                                       _isCageMap);
            setSequence(&m_changePageSequence);
        }
        else
        {
            m_changePageSequence.addPlayer(_player);
        }
    }

    void Ray_GameManager::triggerBounceToLayer(Actor* _player, ObjectRef _target, const Vec2d& _offset, f32 _totalTime)
    {
        u32 seqIndex = isPlayingSeq(&m_bounceToLayerSequence);

        if (seqIndex == U32_INVALID)
        {
            m_bounceToLayerSequence.start(_player, _target, _offset,
                                          m_configTemplate->getBounceToLayerOffsetDistFromTarget(), _totalTime,
                                          m_configTemplate->getBounceToLayerAngleOffset());
            setSequence(&m_bounceToLayerSequence);
        }
    }

    void Ray_GameManager::triggerStartTimeAttackSequence(f32 _duration)
    {
        u32 seqIndex = isPlayingSeq(&m_startTimeAttackSequence);

        if (seqIndex == U32_INVALID)
        {
            m_startTimeAttackSequence.start(_duration);
            setSequence(&m_startTimeAttackSequence);
        }
    }

    u32 Ray_GameManager::getNumDeadPlayers() const
    {
        u32 count = 0;
        for (u32 index = 0; index < getMaxPlayerCount(); ++index)
        {
            const Ray_Player* const player = static_cast<Ray_Player*>(m_players[index]);
            if (player != NULL)
            {
                if (player->getActive() && player->isDead())
                {
                    ++count;
                }
            }
        }
        return count;
    }

    bbool Ray_GameManager::getIsAnyPlayerAlive() const
    {
        for (u32 index = 0; index < getMaxPlayerCount(); ++index)
        {
            const Ray_Player* const player = static_cast<Ray_Player*>(m_players[index]);
            if (player != NULL)
            {
                if (player->getActive() && !player->isDead() && !player->getIsLeaving())
                {
                    return btrue;
                }
            }
        }
        return bfalse;
    }

    bbool Ray_GameManager::canOptInOptOut() const
    {
        return isSequencePlaying() == bfalse &&
            CINEMATICMANAGER->getLockPlayers() == 0 &&
            CINEMATICMANAGER->getIsCameraLocked() == 0 &&
            getIsAnyPlayerAlive() &&
            TRC_ADAPTER->isDisplayingError() == bfalse &&
            Ray_AICageBehavior::getGlobalLockOptOut() == bfalse;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::triggerStargateSequence(ObjectRef _stargate, const Vec3d& _start, f32 _speed)
    {
        u32 seqIndex = isPlayingSeq(&m_stargateSequence);

        if (seqIndex == U32_INVALID)
        {
            m_stargateSequence.start(_stargate, _start, _speed, m_configTemplate->getStargateWaitForPlayersTime(),
                                     m_configTemplate->getStargateWaitDistFromDoor(),
                                     m_configTemplate->getStargateWaitAngleOffset(),
                                     m_configTemplate->getStargateWaitPointZOffset(),
                                     m_configTemplate->getStargateStartToDoorTimeMultiplier(),
                                     m_configTemplate->getStargateCameraLookAtOffset());
            setSequence(&m_stargateSequence);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::triggerScaleDoorSequence(ActorRef _door, ActorRef _targetActor, f32 _scale, bbool _allPlayers,
                                                   const Vec2d& _targetPos)
    {
        u32 seqIndex = isPlayingSeq(&m_scaleDoorSequence);

        if (seqIndex == U32_INVALID)
        {
            setSequence(&m_scaleDoorSequence);
        }

        m_scaleDoorSequence.start(_door, _targetActor, _scale, _allPlayers, _targetPos,
                                  m_configTemplate->getScaleDoorWaitDelay(),
                                  m_configTemplate->getScaleDoorMoveK(), m_configTemplate->getScaleDoorMoveD());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::triggerTeleportCheatSequence(GameManager::teleportTarget _target)
    {
        const u32 seqIndex = isPlayingSeq(&m_teleportCheatSequence);

        if (seqIndex == U32_INVALID)
        {
            m_teleportCheatSequence.start(_target);

            setSequence(&m_teleportCheatSequence);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::triggerChangeRoomSequence(bbool _on)
    {
        if (m_changeRoomDoor != ITF_INVALID_OBJREF)
        {
            u32 seqIndex = isPlayingSeq(&m_changeRoomSeq);

            if (seqIndex == U32_INVALID)
            {
                m_changeRoomSeq.start(_on, m_changeRoomDoor);
                setSequence(&m_changeRoomSeq);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::triggerReturnTeeth(Player* _player, Actor* _deadMan)
    {
        u32 seqIndex = isPlayingSeq(&m_returnTeethSeq);

        if (seqIndex == U32_INVALID)
        {
            m_returnTeethSeq.start(_player, _deadMan, m_configTemplate->getThrowTeethSurpriseCameraOffset(),
                                   m_configTemplate->getThrowTeethThrowCameraOffset(),
                                   m_configTemplate->getThrowTeethPlayerOffset());
            setSequence(&m_returnTeethSeq);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::triggerFadeAndLoadMap(const StringID& _mapTag)
    {
        u32 seqIndex = isPlayingSeq(&m_fadeAndLoadMapSeq);

        if (seqIndex == U32_INVALID)
        {
            m_fadeAndLoadMapSeq.start(_mapTag);
            setSequence(&m_fadeAndLoadMapSeq);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::returnTeethThrowStart()
    {
        u32 seqIndex = isPlayingSeq(&m_returnTeethSeq);

        if (seqIndex != U32_INVALID)
        {
            m_returnTeethSeq.startThrow();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::returnTeethThrowFinished()
    {
        u32 seqIndex = isPlayingSeq(&m_returnTeethSeq);

        if (seqIndex != U32_INVALID)
        {
            m_returnTeethSeq.throwFinished();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::triggerChangePageWithAnim(ActorRef _startPage, ActorRef _endPage, ActorRef _checkpoint,
                                                    const StringID& _anim, const Vec3d& _targetPos)
    {
        u32 seqIndex = isPlayingSeq(&m_changePageWithAnimSequence);

        if (seqIndex == U32_INVALID)
        {
            m_changePageWithAnimSequence.start(_startPage, _endPage, _checkpoint, _anim, _targetPos);
            setSequence(&m_changePageWithAnimSequence);
        }
    }

    void Ray_GameManager::triggerFxScreen(Ray_EventFxScreen* _onEventFxScreen)
    {
        if (_onEventFxScreen->getIsStart())
        {
            StringID type = _onEventFxScreen->getFadeType();
            if (type == StringID(""))
            {
                type = RAY_GAMEMANAGER->getTemplate()->getFadeTypeFlash();
            }
            RAY_GAMEMANAGER->startFade(type);
        }
        else
        {
            RAY_GAMEMANAGER->stopFade(bfalse);
        }
    }

    void Ray_GameManager::triggerEndGameSequence(bbool _playMovie)
    {
        u32 seqIndex = isPlayingSeq(&m_endGameSequence);

        if (seqIndex == U32_INVALID)
        {
            endOfLevelReached(btrue, btrue);

            m_endGameSequence.start(_playMovie);
            setSequence(&m_endGameSequence);
        }
    }

    void Ray_GameManager::onRestartLevelDuringEndSequence()
    {
        u32 seqIndex = isPlayingSeq(&m_endGameSequence);

        if (seqIndex != U32_INVALID)
        {
            m_endGameSequence.stop();
        }
    }

    bbool Ray_GameManager::launchCredits()
    {
        u32 seqIndex = isPlayingSeq(&m_endGameSequence);

        if (seqIndex != U32_INVALID)
        {
            m_endGameSequence.launchCredits();
            return btrue;
        }

        return bfalse;
    }

    bbool Ray_GameManager::skipEndMovie()
    {
        u32 seqIndex = isPlayingSeq(&m_endGameSequence);

        if (seqIndex != U32_INVALID)
        {
            m_endGameSequence.skipMovie();
            return btrue;
        }

        return bfalse;
    }

    bbool Ray_GameManager::canPauseDuringEndSequence()
    {
        u32 seqIndex = isPlayingSeq(&m_endGameSequence);

        if (seqIndex != U32_INVALID)
        {
            return m_endGameSequence.canPauseGame();
        }

        return btrue;
    }

    bbool Ray_GameManager::canPauseWorldMap()
    {
        u32 seqIndex = isPlayingSeq(&m_changePageSequence);
        if (seqIndex != U32_INVALID)
        {
            return m_changePageSequence.canPauseGame();
        }

        seqIndex = isPlayingSeq(&m_changeRoomSeq);
        if (seqIndex != U32_INVALID)
        {
            return bfalse;
        }

        return btrue;
    }

    void Ray_GameManager::skipLevelRequested(bbool _skip)
    {
        u32 seqIndex = isPlayingSeq(&m_restoreDeath);

        if (seqIndex != U32_INVALID)
        {
            if (_skip)
                m_restoreDeath.skipLevel();
            else
                m_restoreDeath.cancelSkipLevel();
        }
    }

    void Ray_GameManager::triggerWorldMapUnlockSequence(const ActorRef& _nodeRef)
    {
        u32 seqIndex = isPlayingSeq(&m_worldMapUnlockSequence);
        ITF_ASSERT(seqIndex == U32_INVALID);
        if (seqIndex == U32_INVALID)
        {
            m_worldMapUnlockSequence.start(_nodeRef);
            setSequence(&m_worldMapUnlockSequence);
        }
    }

    void Ray_GameManager::triggerWorldMapTeleportSequence(const ActorRef& _spotFromRef, const ActorRef& _spotToRef)
    {
        u32 seqIndex = isPlayingSeq(&m_worldMapTeleportSequence);
        ITF_ASSERT(seqIndex == U32_INVALID);
        if (seqIndex == U32_INVALID)
        {
            m_worldMapTeleportSequence.start(_spotFromRef, _spotToRef);
            setSequence(&m_worldMapTeleportSequence);
        }
    }

    void Ray_GameManager::changePlayerIdInfoForPlayer(Player* _player, const PlayerIDInfo* _newPlayerId)
    {
        Super::changePlayerIdInfoForPlayer(_player, _newPlayerId);

        if (_player->getIndex() == getMainIndexPlayer())
        {
            getPersistentGameData()->setUsedPlayerIDInfo(_newPlayerId->getId());
        }

        getPersistentGameData()->addCostumeUsed(_newPlayerId->getId());
    }

    bbool Ray_GameManager::hasCostumeBeenUsed(const StringID& _id) const
    {
        return getPersistentGameData()->isCostumeUsed(_id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Ray_GameManagerSequenceVictory::start(const ObjectPath& _path, bbool _goToWorldMap)
    {
        m_path = _path;
        m_goToWorldMap = _goToWorldMap;
        m_isFinished = bfalse;

        Ray_EventTriggerVictory victoryEvent;
        EVENTMANAGER->broadcastEvent(&victoryEvent);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::resetToWorldMap(bbool _fade /*= btrue*/)
    {
        clearNextOperation();
        stopAllSequences();

        // Stop end sequence if it was running
        if (isPlayingSeq(&m_endGameSequence) != U32_INVALID)
            m_endGameSequence.stop();

        MUSICMANAGER->stopAll(1.0f);

        if (m_preloadedWorldMap.isValid())
        {
            Ray_GameScreen_WorldMap::InitializationParameters data;
            data.m_worldRef = m_preloadedWorldMap;
            m_preloadedWorldMap.invalidate();
            GAMEMANAGER->postGameScreenChange<Ray_GameScreen_WorldMap>(btrue, &data);
        }
        else
        {
            Ray_GameScreen_WorldMapLoading::InitializationParameters data;
            data.m_mapName = RAY_GAMEMANAGER->getWorldMap();
            data.m_startFade = _fade;

            GAMEMANAGER->postGameScreenChange<Ray_GameScreen_WorldMapLoading>(btrue, &data);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    const GameManagerConfig_Template* Ray_GameManager::loadConfig(const Path& _path)
    {
        m_templateClientHandler.addUsedTemplate(_path.getStringID());
        m_configTemplate = TEMPLATEDATABASE->getTemplate<Ray_GameManagerConfig_Template>(
            &m_templateClientHandler, _path);
        return m_configTemplate;
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::init()
    {
        GameManager::init();

        if (m_inputManager)
        {
            // WIN32 + X360 support
            m_inputManager->addX360pad_device(JOY_MAX_COUNT);
            // PS3 support
            m_inputManager->addPS3pad_device(JOY_MAX_COUNT);
            // WII support
            m_inputManager->addWiiclassic_device(JOY_MAX_COUNT);
            m_inputManager->addWiimote_device(JOY_MAX_COUNT);
            m_inputManager->addWiinunchuck_device(JOY_MAX_COUNT);

            m_inputManager->addPS5pad_device(JOY_MAX_COUNT);

            m_inputManager->addNintendopad_device(JOY_MAX_COUNT);

            m_inputManager->addXBoxSeries_device(JOY_MAX_COUNT);
        }

        fillWorldMapWorldsList();
        registerAllGameOptions();

        m_gameOptionPersistence = newAlloc(mId_Singleton, Ray_GameOptionPersistence(&m_gameOptionManager));
        m_gameOptionPersistence->init();
        loadGameOptions();

        m_trcHelper = newAlloc(mId_Singleton, Ray_TRCHelper());
        if (TRC_ADAPTER)
            TRC_ADAPTER->registerTRCHelper((TRCHelper*)(m_trcHelper));
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::onMapLoaded(World* _pWorld)
    {
        Super::onMapLoaded(_pWorld);

        onMapLoadedUpdateTimeAttack();
        onMapLoadedUpdateMurphyAssist();

#ifdef ITF_SUPPORT_ONLINETRACKING

        m_onlineTrackingManager.m_currentLevelGameOverCount = 0;

#endif

        if (_pWorld->getRef() == m_preloadedPrologueWorld)
        {
            m_preloadedPrologueWorld.invalidate();
            m_preloadedPrologueReady = bfalse;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::onSceneAsyncLoadFinished(const Scene* _pScene)
    {
        Super::onSceneAsyncLoadFinished(_pScene);

        if (getLevelPath(getCurrentLevelName()) == _pScene->getPath())
            spawnMedal(getCurrentLevelName());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Ray_GameManager::setSequenceSwitchTriggered(const ActorRef& _ref)
    {
        if (m_sequenceSwitchesTriggered.size() >= s_maxSequenceSwitchesPerMap)
        {
            ITF_WARNING(NULL, bfalse, "Too many sequence switches in this map");
            return;
        }
        m_sequenceSwitchesTriggered.push_back(_ref);
    }

    bbool Ray_GameManager::wasSequenceSwitchTriggered(const ActorRef& _ref) const
    {
        u32 count = m_sequenceSwitchesTriggered.size();
        for (u32 i = 0; i < count; ++i)
        {
            if (m_sequenceSwitchesTriggered[i] == _ref)
            {
                return btrue;
            }
        }
        return bfalse;
    }

    void Ray_GameManager::bubblizerAchievement(ActorRef _victim, ActorRef _perpetrator, u32 _identifer)
    {
        Player* player = getPlayerFromActor(_perpetrator, bfalse);

        if (player && player->getIndex() == GAMEMANAGER->getMainIndexPlayer() && _victim.isValid() && _victim.
            getActor())
        {
            Bubblizer::iterator it = m_bubblizer.find(_identifer);
            if (it == m_bubblizer.end())
            {
                m_bubblizer[_identifer] = ActorRefList();
                m_bubblizer[_identifer].push_back(_victim);
            }
            else
            {
                ActorRefList& list = it->second;
                i32 i = list.find(_victim);
                if (i == -1)
                    list.push_back(_victim);
            }
        }
    }

    void Ray_GameManager::updateBubblizerAchievement()
    {
        for (Bubblizer::iterator it = m_bubblizer.begin(); it != m_bubblizer.end();)
        {
            ActorRefList& list = it->second;
            if (list.size() >= 4)
            {
                //reward
                REWARD_MANAGER->Action_New(Ray_RewardManager::Action_ChainBubble, GAMEMANAGER->getMainIndexPlayer());
                m_bubblizer.clear();
                return;
            }
            else
            {
                bbool enabled = btrue;
                for (u32 i = 0; i < list.size(); ++i)
                {
                    Actor* victim = list[i].getActor();
                    if (!victim || (victim && !victim->isEnabled()))
                    {
                        enabled = bfalse;
                        break;
                    }
                }
                if (!enabled)
                {
                    list.clear();
                    break;
                }
            }
            if (list.size() == 0)
            {
                it = m_bubblizer.erase(it);
            }
            else
                ++it;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::showRewards()
    {
        if (m_rewardManager.isRewardsListDisplayed())
            m_rewardManager.hideRewards();
        else
            m_rewardManager.displayRewards();
    }

    void Ray_GameManager::loadMapThroughLoadingScreen(StringID _tag)
    {
        const Path& path = RAY_GAMEMANAGER->getLevelPath(_tag);
        if (!path.isEmpty())
        {
            Ray_GameScreen_WorldMapLoading::InitializationParameters data;
            data.m_mapName = path;

            GAMEMANAGER->postGameScreenChange<Ray_GameScreen_WorldMapLoading>(btrue, &data);
        }
    }

    void Ray_GameManager::loadLoadingScreen(StringID _tag)
    {
        const Path& path = getLoadingScreen(_tag);
        if (!path.isEmpty())
            Ray_GameScreen_WorldMapLoading::preload(path);
    }

    void Ray_GameManager::createRewardResources()
    {
        //rewards
        createRewardResources(m_configTemplate->getLumPath(), m_configTemplate->getHeartPath());
    }

    ITF::bbool Ray_GameManager::isLoadingVisible()
    {
        return Super::isLoadingVisible() || m_theGameScreen->DynamicCast<Ray_GameScreen_WorldMapLoading>(
            ITF_GET_STRINGID_CRC(Ray_GameScreen_WorldMapLoading, 1826584106)) != NULL;
    }

    ITF::bbool Ray_GameManager::postGameScreenChange_String(const StringID& _screen, bbool _evenIfOtherQueued,
                                                            GameScreenBase::InitializationParameters* _data /*= NULL */)
    {
        if (Super::postGameScreenChange_String(_screen, _evenIfOtherQueued, _data))
        {
            //stopFade(bfalse);
            return btrue;
        }
        return bfalse;
    }

    u32 Ray_GameManager::computeSaveCodeCRC()
    {
        ArchiveMemory* pArchiveMemory = newAlloc(mId_System, ArchiveMemory);
        Ray_PersistentGameData_Universe emptyUniverse;
        CSerializerObjectSignature serSignature(pArchiveMemory);
        serSignature.SerializeObject("CONTENT", emptyUniverse, ESerialize_Data_Save);
        u32 saveCodeCRC = serSignature.computeCRC() + PersistentGameData_Universe::Version;
        SF_DEL(pArchiveMemory);

        LOG("computeSaveCodeCRC >> 0x%08x", saveCodeCRC);
        return saveCodeCRC;
    }

    u32 Ray_GameManager::computeSaveDataCRC(const u8* _data, u32 _dataSize)
    {
        ITF_ASSERT_MSG(_data!=NULL, "computeSaveDataCRC, data is NULL");
        ITF_ASSERT_MSG(_dataSize!=0, "computeSaveDataCRC, _dataSize is NULL");
        if (!_dataSize || !_data)
            return 0;

        ArchiveMemory* pArchiveMemory = newAlloc(mId_Gameplay, ArchiveMemory(_data, _dataSize));
        CSerializerObjectSignature serializer(pArchiveMemory);

        PersistentGameData_Universe* pPersistentGameData = newAlloc(mId_Gameplay, Ray_PersistentGameData_Universe);
        serializer.SerializeObject("CONTENT", pPersistentGameData, ESerialize_Data_Load);
        u32 saveDataCRC = serializer.computeCRC();
        SF_DEL(pArchiveMemory);
        SF_DEL(pPersistentGameData);

        LOG("computeSaveDataCRC >> 0x%08x", saveDataCRC);
        return saveDataCRC;
    }

    //////////////////////////////////////////////////////////////////////////
    // Fade
    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::startFade(const StringID& _type, bbool _force)
    {
        Ray_UIFadeScreenComponent* fadeComponent = getUIFade();

        if (fadeComponent)
        {
            fadeComponent->startFade(_type, _force);
        }
    }

    bbool Ray_GameManager::isFadeOutFinished() const
    {
        Ray_UIFadeScreenComponent* fadeComponent = getUIFade();

        if (fadeComponent)
        {
            return fadeComponent->isFadeOutFinished();
        }
        else
        {
            return btrue;
        }
    }

    bbool Ray_GameManager::isFadeInFinished() const
    {
        Ray_UIFadeScreenComponent* fadeComponent = getUIFade();

        if (fadeComponent)
        {
            return fadeComponent->isFadeInFinished();
        }
        else
        {
            return btrue;
        }
    }

    void Ray_GameManager::stopFade(bbool _force)
    {
        Ray_UIFadeScreenComponent* fadeComponent = getUIFade();

        if (fadeComponent)
        {
            fadeComponent->stopFade(_force);
        }
    }

    Ray_UIFadeScreenComponent* Ray_GameManager::getUIFade() const
    {
        Actor* fadeActor = m_uiFadeActorRef.getActor();

        if (fadeActor)
        {
            Ray_UIFadeScreenComponent* fadeComponent = fadeActor->GetComponent<Ray_UIFadeScreenComponent>();

            if (fadeComponent)
            {
                return fadeComponent;
            }
        }

        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::mapLoadResetInfo()
    {
        // always reset volatile per-player power-ups on map loading (RO-11293)
        m_powerUpManager.resetVolatilePerPlayerData();

        // sprint tutorial: reset infos
        m_sprintTutorialFailureCount = 0;
        m_sprintTutorialRegistered = bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    bbool Ray_GameManager::on_ChangeExtensionPreStartTRCMessage(TRCMessage_Base* pMessage, void* pParams)
    {
        u32 pad = (u32)uPtr(pParams);
        if (!INPUT_ADAPTER->isPadConnected(pad))
            return bfalse;

        InputAdapter::PadType padType = INPUT_ADAPTER->getPadType(pad);

        switch (pMessage->getContexte())
        {
        case TRCManagerAdapter::Pad_ExtensionNunchukDetected:
            return (padType == InputAdapter::Pad_WiiNunchuk);
        case TRCManagerAdapter::Pad_ExtensionClassicDetected:
            return (padType == InputAdapter::Pad_WiiClassic);
        case TRCManagerAdapter::Pad_NoExtensionDetected:
            return (padType == InputAdapter::Pad_WiiSideWay);
        }

        return false; // something wrong?
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::updateInputConfig()
    {
        // Update the wii input
        for (u32 player = 0; player < GAMEMANAGER->getMaxPlayerCount(); player++)
        {
            if (m_lastPadType[player] != (u32)INPUT_ADAPTER->getPadType(player))
            {
                bbool addTRCMessage = (m_players[player]->getActiveAndPersistent()) || (player ==
                    getMainIndexPlayer_Internal());

                switch (INPUT_ADAPTER->getPadType(player))
                {
                case InputAdapter::Pad_WiiClassic:
                    m_inputManager->ForceConfigForPlayer(player, ITF_GET_STRINGID_CRC(CLASSIC, 352657140));
                    if (addTRCMessage)
                    {
                        TRC_ADAPTER->setCustomParam_u32(player);
                        TRC_ADAPTER->addMessage(TRCManagerAdapter::Pad_ExtensionClassicDetected,
                                                on_ChangeExtensionPreStartTRCMessage,
                                                reinterpret_cast<void*>(uPtr(player)));
                    }
                    break;
                case InputAdapter::Pad_WiiNunchuk:
                    m_inputManager->ForceConfigForPlayer(player, ITF_GET_STRINGID_CRC(NUNCHUCK, 633222557));
                    if (addTRCMessage)
                    {
                        TRC_ADAPTER->setCustomParam_u32(player);
                        TRC_ADAPTER->addMessage(TRCManagerAdapter::Pad_ExtensionNunchukDetected,
                                                on_ChangeExtensionPreStartTRCMessage,
                                                reinterpret_cast<void*>(uPtr(player)));
                    }
                    break;
                case InputAdapter::Pad_WiiSideWay:
                    m_inputManager->ForceConfigForPlayer(player, ITF_GET_STRINGID_CRC(SIDEWAY, 855951513));
                    if (addTRCMessage)
                    {
                        TRC_ADAPTER->setCustomParam_u32(player);
                        TRC_ADAPTER->addMessage(TRCManagerAdapter::Pad_NoExtensionDetected,
                                                on_ChangeExtensionPreStartTRCMessage,
                                                reinterpret_cast<void*>(uPtr(player)));
                    }
                    break;
                }

                m_lastPadType[player] = (u32)INPUT_ADAPTER->getPadType(player);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::forceInputConfigReset(u32 _padIndex)
    {
        if (_padIndex < getMaxPlayerCount())
            m_lastPadType[_padIndex] = U32_INVALID;
    }

    void Ray_GameManager::onPlayerDisconnected(u32 _padIndex)
    {
        Super::onPlayerDisconnected(_padIndex);
    }

    void Ray_GameManager::preloadPrologue()
    {
        ITF_ASSERT(!m_preloadedPrologueWorld.isValid());

        Vector<ObjectPath> prefetchList;
        m_preloadedPrologueWorld = GAMEMANAGER->preLoadMap(getPrologueMap(), prefetchList);
        m_preloadedPrologueReady = bfalse;
    }

    void Ray_GameManager::onDeleteWorld(World* _pWorld)
    {
        Super::onDeleteWorld(_pWorld);

        if (_pWorld->getRef() == m_preloadedPrologueWorld)
        {
            m_preloadedPrologueWorld.invalidate();
            m_preloadedPrologueReady = bfalse;
        }
    }

    void Ray_GameManager::onPreloadedAsyncFinished(World* _world)
    {
        Super::onPreloadedAsyncFinished(_world);
        const Path& path = _world->getRootScene()->getPath();
        if (getLevelPath(getCurrentLevelName()) == path ||
            path == getWorldMap() ||
            path == getMainMenuBackMapForDebugSaves() ||
            path == getCreditsMap())
        {
            if (path == getWorldMap())
            {
                setMusicTheme(ITF_GET_STRINGID_CRC(WorldMap, 1734041925));
            }
            else if (path == getMainMenuBackMapForDebugSaves())
            {
                setMusicTheme(ITF_GET_STRINGID_CRC(menu, 105712373));
            }
            else if (path == getCreditsMap())
            {
                setMusicTheme(ITF_GET_STRINGID_CRC(credits, 2384050872));
            }
            else
            {
                setMusicTheme(getMapConfigMusic(getWMCurrentLevelTag()));
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManagerSequenceVictory::onEvent(Event* _event)
    {
        if (Ray_EventSequenceFinished* seqFinished = _event->DynamicCast<Ray_EventSequenceFinished>(
            ITF_GET_STRINGID_CRC(Ray_EventSequenceFinished, 2287469510)))
        {
            Ray_GameManager* gm = RAY_GAMEMANAGER;

            if (gm)
            {
                if (m_goToWorldMap)
                {
                    RAY_GAMEMANAGER->resetToWorldMap();
                }
                else
                {
                    Ray_GameScreen_Gameplay::InitializationParameters data;
                    String8 mapFile;

                    SceneObjectPathUtils::getWorldFromAbsolutePath(m_path, mapFile);

                    data.m_checkPointPath = m_path;
                    data.m_mapName = mapFile;

                    gm->postGameScreenChange<Ray_GameScreen_Gameplay>(btrue, &data);
                }
            }

            m_isFinished = btrue;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameManagerSequenceHiddenArea::Ray_GameManagerSequenceHiddenArea()
        : m_start(ITF_INVALID_OBJREF)
          , m_end(ITF_INVALID_OBJREF)
          , m_state(STATE_FADEOUT)
          , m_entry(Vec2d::Zero)
          , m_exit(Vec2d::Zero)
          , m_timeCounter(0.f)
          , m_waitPlayersDuration(3.f)
          , m_waitLoadDuration(1.f)
          , m_playersProcessed(0)
          , m_noEntry(bfalse)
    {
    }

    void Ray_GameManagerSequenceHiddenArea::start(ObjectRef _start, ObjectRef _end, f32 _waitPlayersDuration,
                                                  f32 _waitLoadDuration)
    {
        m_start = _start;
        m_end = _end;
        m_waitPlayersDuration = _waitPlayersDuration;
        m_waitLoadDuration = _waitLoadDuration;
        m_playersMoving.clear();
        m_isFinished = bfalse;
        m_playersProcessed = 0;

        Actor* startActor = AIUtils::getActor(_start);

        if (startActor)
        {
            Ray_EventQueryDoorData doorData;
            startActor->onEvent(&doorData);
            m_entry = doorData.getEntry();
            m_exit = doorData.getExit();
            m_noEntry = doorData.getIsNoEntry();
            // Wait some time or until all players are in
            m_state = STATE_WAITFORPLAYERSENTRY;
            m_timeCounter = _waitPlayersDuration;
        }
        else
        {
            BaseObject* endObj = _end.getObject();
            if (endObj)
            {
                Pickable* pickable = endObj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable, 594660361));
                if (pickable)
                {
                    m_exit = pickable->get2DPos();
                    m_entry = m_exit;
                    m_noEntry = btrue;
                    startFadeOut();
                }
            }
        }
    }

    void Ray_GameManagerSequenceHiddenArea::stop()
    {
        EventStopPlayer stopPlayer(bfalse);

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (player->getActive() && !player->getIsLeaving())
            {
                Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                if (playerActor)
                {
                    playerActor->onEvent(&stopPlayer);
                }
            }
        }

        RAY_GAMEMANAGER->stopFade(btrue);
    }

    void Ray_GameManagerSequenceHiddenArea::addPlayerEntering(ObjectRef _ref)
    {
        if (m_playersMoving.find(_ref) == U32_INVALID)
        {
            Actor* playerEntering = AIUtils::getActor(_ref);

            if (playerEntering)
            {
                if (m_noEntry)
                {
                    EventStopPlayer stopPlayer(btrue);
                    playerEntering->onEvent(&stopPlayer);
                }
                else
                {
                    Ray_EventMoveToPoint moveTo(m_entry);

                    playerEntering->onEvent(&moveTo);
                    m_playersMoving.push_back(_ref);
                }

                m_playersProcessed++;
            }
        }
    }

    void Ray_GameManagerSequenceHiddenArea::update(f32 _dt)
    {
        switch (m_state)
        {
        case STATE_WAITFORPLAYERSENTRY:
#ifdef ITF_SUPPORT_CHEAT
            GFX_ADAPTER->drawDBGText("Waiting for players entry");
#endif // ITF_SUPPORT_CHEAT
            updateWaitForPlayersEntry(_dt);
            break;
        case STATE_FADEOUT:
#ifdef ITF_SUPPORT_CHEAT
            GFX_ADAPTER->drawDBGText("Fade out");
#endif // ITF_SUPPORT_CHEAT
            updateFadeOut(_dt);
            break;
        case STATE_WAIT:
#ifdef ITF_SUPPORT_CHEAT
            GFX_ADAPTER->drawDBGText("Wait load");
#endif // ITF_SUPPORT_CHEAT
            updateWait(_dt);
            break;
        case STATE_FADEIN:
#ifdef ITF_SUPPORT_CHEAT
            GFX_ADAPTER->drawDBGText("Fade in");
#endif // ITF_SUPPORT_CHEAT
            updateFadeIn(_dt);
            break;
        }
    }

    void Ray_GameManagerSequenceHiddenArea::updateMoving()
    {
        u32 i = 0;
        bbool exiting = m_state == STATE_FADEIN;
        Vec2d destination = exiting ? m_exit : m_entry;
        EventStopPlayer stopPlayer(btrue);
        Ray_EventResetMoveToPoint resetMove;

        while (i < m_playersMoving.size())
        {
            Actor* actorEntering = AIUtils::getActor(m_playersMoving[i]);

            if (actorEntering)
            {
                Vec2d distance = destination - actorEntering->get2DPos();

                if (distance.sqrnorm() > 1.f)
                {
                    i++;
                }
                else
                {
                    if (!exiting)
                    {
                        actorEntering->onEvent(&stopPlayer);
                    }

                    actorEntering->onEvent(&resetMove);
                    m_playersMoving.eraseNoOrder(i);
                }
            }
        }
    }

    void Ray_GameManagerSequenceHiddenArea::startFadeOut()
    {
        m_state = STATE_FADEOUT;
        RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeHiddenArea());
    }

    void Ray_GameManagerSequenceHiddenArea::updateWaitForPlayersEntry(f32 _dt)
    {
        // In this state we wait for the players to enter the door
        // As soon as all of them are in, or moving into it or the timeout ends, we start the fade out
        m_timeCounter = Max(m_timeCounter - _dt, 0.f);

        updateMoving();

        if (m_timeCounter == 0.f ||
            m_playersProcessed == GAMEMANAGER->getNumActivePlayers())
        {
            startFadeOut();
        }
    }

    void Ray_GameManagerSequenceHiddenArea::updateFadeOut(f32 _dt)
    {
        updateMoving();

        if (RAY_GAMEMANAGER->isFadeOutFinished())
        {
            // Once the fade is finished we can teleport everyone and wait a bit for the loading
            Actor* endActor = AIUtils::getActor(m_end);

            if (endActor)
            {
                Ray_EventQueryDoorData doorData;

                endActor->onEvent(&doorData);
                m_entry = doorData.getEntry();
                m_exit = doorData.getExit();
                m_noEntry = doorData.getIsNoEntry();
                m_state = STATE_WAIT;

                f32 sceneZ = AIUtils::getSceneZFromPickable(endActor);
                Vec3d entry3d = m_entry.to3d(endActor->getPos().m_z);

                EventStopPlayer stopPlayer(btrue);
                EventDetach detachEvent;

                GAMEMANAGER->setCurrentSceneReference(endActor->getScene());
                CAMERACONTROLLERMANAGER->teleport(entry3d);

                ObjectRef teleportedPlayer = ITF_INVALID_OBJREF;

                for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

                    if (player->getActive() && !player->getIsLeaving())
                    {
                        Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                        if (playerActor)
                        {
                            entry3d = m_entry.to3d(sceneZ);

                            if (playerActor->getTemplate()->isZForced())
                            {
                                entry3d.m_z += playerActor->getTemplate()->getForcedZ();
                            }

                            EventTeleport teleportEvent(entry3d, endActor->getAngle(), bfalse);

                            teleportedPlayer = playerActor->getRef();
                            playerActor->onEvent(&detachEvent);
                            playerActor->onEvent(&teleportEvent);
                            playerActor->onEvent(&stopPlayer);
                        }
                    }
                }

                if (teleportedPlayer != ITF_INVALID_OBJREF)
                {
                    Ray_EventTeleportUsed teleportUsed(teleportedPlayer, m_end, m_entry);

                    RAY_GAMEMANAGER->teleportRescuedFriendlies(&teleportUsed);
                }

                m_timeCounter = m_waitLoadDuration;
            }
            else
            {
                // Something went wrong, the end actor doesn't exist so we must abort
                EventStopPlayer stopPlayer(bfalse);

                for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

                    if (player->getActive() && !player->getIsLeaving())
                    {
                        Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                        if (playerActor)
                        {
                            playerActor->onEvent(&stopPlayer);
                        }
                    }
                }

                m_isFinished = btrue;
            }
        }
    }

    void Ray_GameManagerSequenceHiddenArea::updateWait(f32 _dt)
    {
        m_timeCounter -= _dt;

        if (m_timeCounter <= 0.f)
        {
            // Ready to start the fade-in and go out of the door
            m_state = STATE_FADEIN;

            m_playersMoving.clear();

            RAY_GAMEMANAGER->stopFade(bfalse);

            EventStopPlayer stopEvent(bfalse);
            Ray_EventMoveToPoint moveTo(m_exit);

            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

                if (player->getActive() && !player->getIsLeaving())
                {
                    Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                    if (playerActor)
                    {
                        playerActor->onEvent(&stopEvent);

                        if (!m_noEntry)
                        {
                            playerActor->onEvent(&moveTo);
                            m_playersMoving.push_back(playerActor->getRef());
                        }
                    }
                }
            }
        }
    }

    void Ray_GameManagerSequenceHiddenArea::updateFadeIn(f32 _dt)
    {
        updateMoving();

        m_timeCounter = Max(m_timeCounter - _dt, 0.f);

        if (RAY_GAMEMANAGER->isFadeInFinished() && m_playersMoving.size() == 0)
        {
            m_isFinished = btrue;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Ray_GameManagerRestoreDeath::Ray_GameManagerRestoreDeath()
        : m_currentState(STATE_FADE_BLACK)
          , m_prefetchView(NULL)
          , m_prefetchFailSafe(0.f)
          , m_askToSkip(bfalse)
          , m_skipLevel(bfalse)
          , m_soundFaded(bfalse)
          , m_hasChangedInput(bfalse)
    {
    }

    Ray_GameManagerRestoreDeath::~Ray_GameManagerRestoreDeath()
    {
        freePrefetch();
    }

    void Ray_GameManagerRestoreDeath::start()
    {
        m_isFinished = bfalse;
        m_askToSkip = bfalse;
        m_prefetchFailSafe = 0.f;
        m_currentState = STATE_FADE_BLACK;

        Ray_PlayerHudScoreComponent::setIsRelicCaptionEnabled(bfalse);

        // Increase death counter so that we can skip this level if it is allowed
        RAY_GAMEMANAGER->increaseSkipCounter();
        if (RAY_GAMEMANAGER->getDeathCounterOnSameCheckpoint() >= RAY_GAMEMANAGER->getCurrentDeathMaxCount())
        {
            RAY_GAMEMANAGER->resetSkipCounter(RAY_GAMEMANAGER->getCurrentCheckpoint()->getRef(), btrue);
            m_askToSkip = btrue;
        }

        // All players are dead or offscreen, so let's go back to the last checkpoint
        Color startColor = Color::black();
        startColor.m_a = 0.f;

        RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeDeath());

        startSoundFadeOut();

        Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

        if (checkpoint)
        {
            if (!m_prefetchView)
            {
                m_prefetchView = View::createView("Ray_GameManagerRestoreDeath", bfalse, btrue);
            }

            const f32 deltaX = CAMERA->getDeltaX();
            const f32 deltaY = CAMERA->getDeltaY();
            Vec3d checkpointPos = checkpoint->getPos();
            AABB targetAABB(checkpointPos);
            static const f32 viewDefaultSize = 10.f;

            targetAABB.grow(checkpointPos + Vec3d(-viewDefaultSize, -viewDefaultSize, 0.f));
            targetAABB.grow(checkpointPos + Vec3d(viewDefaultSize, viewDefaultSize, 0.f));
            targetAABB.projectTo2D(deltaX, deltaY, checkpoint->getDepth());

            m_prefetchView->setTargetAABB(targetAABB);
            m_prefetchView->setActive(btrue);
            World::disableLoadResourcesDelay();
        }

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Player* player = GAMEMANAGER->getPlayer(i);

            if (player->getActive())
            {
                Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                if (playerActor)
                {
                    CAMERACONTROLLERMANAGER->unregisterSubject(playerActor->getRef());
                }
            }
        }

#ifdef ITF_SUPPORT_ONLINETRACKING
        RAY_GAMEMANAGER->getOnlineTrackingManager()->onGameRestart(bfalse);
#endif //ITF_SUPPORT_ONLINETRACKING
    }

    void Ray_GameManagerRestoreDeath::stop()
    {
        freePrefetch();
        m_prefetchFailSafe = 0.f;

        showSkipActor(bfalse, btrue);

        if (m_hasChangedInput)
        {
            RAY_GAMEMANAGER->setInputModeForMenu(bfalse);
            RAY_GAMEMANAGER->setInputModeForGamePlay(btrue);
            m_hasChangedInput = bfalse;
        }

        RAY_GAMEMANAGER->stopFade(btrue);

        u32 numPlayers = GAMEMANAGER->getMaxPlayerCount();

        for (u32 playerIndex = 0; playerIndex < numPlayers; playerIndex++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(playerIndex));

            if (!player || !player->getActive() || player->getIsLeaving())
            {
                continue;
            }

            // found a live player, check if he's onscreen
            if (Actor* playerActor = player->getActor())
            {
                EventReset resetEvent;
                playerActor->onEvent(&resetEvent);
            }
        }

        RAY_GAMEMANAGER->hideLoadNotificationActor();

        startSoundFadeIn();
    }

    void Ray_GameManagerRestoreDeath::update(f32 _dt)
    {
        switch (m_currentState)
        {
        case STATE_FADE_BLACK:
            updateFadeBlack(_dt);
            break;
        case STATE_ASK_TO_SKIP:
            break;
        case STATE_WAIT_FOR_ACTOR:
            if (isSkipActorDone())
            {
                if (m_hasChangedInput)
                {
                    RAY_GAMEMANAGER->setInputModeForMenu(bfalse);
                    RAY_GAMEMANAGER->setInputModeForGamePlay(btrue);
                    m_hasChangedInput = bfalse;
                }

                // unpause game
                RAY_GAMEMANAGER->requestGameplayUnpause();
                if (m_skipLevel)
                {
                    RAY_GAMEMANAGER->skipCurrentLevel();
                }
                else
                {
                    RAY_GAMEMANAGER->setHasRejectedSkip(btrue);

                    m_currentState = STATE_FADE_GAME;
                    RAY_GAMEMANAGER->stopFade(bfalse);
                    GAMEMANAGER->resetToLastCheckpoint();
                }
            }
            break;
        case STATE_FADE_GAME:
            updateFadeToGame(_dt);
            break;
        }
    }

    void Ray_GameManagerRestoreDeath::updateFadeBlack(f32 _dt)
    {
        bbool nextState = bfalse;

        if (RAY_GAMEMANAGER->isFadeOutFinished())
        {
            if (m_prefetchView)
            {
                Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

                if (checkpoint)
                {
                    ITF_ASSERT(m_prefetchFailSafe < MAX_PREFETCH_TIMEOUT);
                    if (checkpoint->getWorld()->isPhysicalReady(*m_prefetchView) || m_prefetchFailSafe >=
                        MAX_PREFETCH_TIMEOUT)
                    {
                        nextState = btrue;
                    }
                    else
                    {
                        m_prefetchFailSafe += _dt;

                        if (m_prefetchFailSafe >= MAX_FADETIME_FORICON)
                        {
                            RAY_GAMEMANAGER->showLoadNotificationActor();
                        }
                    }
                }
                else
                {
                    nextState = btrue;
                }
            }
            else
            {
                nextState = btrue;
            }
        }

        if (nextState)
        {
            if (!m_askToSkip
                || (m_askToSkip && !showSkipLevelActor()))
            {
                m_currentState = STATE_FADE_GAME;
                RAY_GAMEMANAGER->stopFade(bfalse);
                startSoundFadeIn();
                GAMEMANAGER->resetToLastCheckpoint();
                RAY_GAMEMANAGER->hideLoadNotificationActor();
            }
        }
    }

    bbool Ray_GameManagerRestoreDeath::showSkipLevelActor()
    {
        // Pause game
        RAY_GAMEMANAGER->requestGameplayPause();

        ActorRef skipActorRef = RAY_GAMEMANAGER->getSkipLevelActor();

        ITF_WARNING_CATEGORY(GPP, NULL, skipActorRef.isValid(),
                             "Can't get skip level actor, check if it has really been spawned");

        showSkipActor(btrue, bfalse);

        m_currentState = STATE_ASK_TO_SKIP;

        if (!m_hasChangedInput)
        {
            RAY_GAMEMANAGER->setInputModeForMenu(btrue);
            RAY_GAMEMANAGER->setInputModeForGamePlay(bfalse);
            m_hasChangedInput = btrue;
        }

        GAMEMANAGER->setIndexPauseOwner(GAMEMANAGER->getMainIndexPlayer());
        UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getPauseMenuPriority(),
                                     ITF_GET_STRINGID_CRC(menuskiplevel, 3731260785), btrue, this);

        LocalisationId id;
#ifdef ITF_WII
        id = 4508;
#elif defined(ITF_PS3) || defined(ITF_PS5)
        id = 4507;
#else
        id = 4506;
#endif
        String displayName = LOCALISATIONMANAGER->getText(id);

        UI_MENUMANAGER->changeMenuItemFriendlyByID(
            ITF_GET_STRINGID_CRC(menuconfirmationskip, 1507098929),
            ITF_GET_STRINGID_CRC(textbox, 352599092),
            StringConverter(displayName).getChar());

        return btrue;
    }

    void Ray_GameManagerRestoreDeath::cancelSkipLevel()
    {
        m_skipLevel = bfalse;
        m_currentState = STATE_WAIT_FOR_ACTOR;
        showSkipActor(bfalse, bfalse);
    }

    void Ray_GameManagerRestoreDeath::skipLevel()
    {
        m_skipLevel = btrue;
        m_currentState = STATE_WAIT_FOR_ACTOR;
        showSkipActor(bfalse, bfalse);
    }

    void Ray_GameManagerRestoreDeath::freePrefetch()
    {
        if (m_prefetchView)
        {
            m_prefetchView->setActive(bfalse);
            World::enableLoadResourcesDelay();
            View::deleteView(m_prefetchView);
            m_prefetchView = NULL;
        }
    }

    void Ray_GameManagerRestoreDeath::updateFadeToGame(f32 _dt)
    {
        if (!m_isFinished)
        {
            m_isFinished = btrue;
            startSoundFadeIn();
            freePrefetch();
        }
    }

    void Ray_GameManagerRestoreDeath::onMenuItemAction(UIComponent* _UIComponent)
    {
        if (!_UIComponent) return;

        const StringID id = _UIComponent->getID();

        if (m_currentState == STATE_ASK_TO_SKIP)
        {
            if (id == ITF_GET_STRINGID_CRC(CONFIRM_YES, 3118550926))
            {
                UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getPauseMenuPriority());
                skipLevel();
            }
            else if (id == ITF_GET_STRINGID_CRC(CONFIRM_VALIDATION_STAY, 3285194972) || id == ITF_GET_STRINGID_CRC(
                CONFIRM_NO, 893466327))
            {
                UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getPauseMenuPriority());
                cancelSkipLevel();
            }
        }
    }

    void Ray_GameManagerRestoreDeath::showSkipActor(bbool _show, bbool _force)
    {
        ActorRef skipActorRef = RAY_GAMEMANAGER->getSkipLevelActor();
        ITF_WARNING_CATEGORY(GPP, NULL, skipActorRef.isValid(),
                             "Can't get skip level actor, check if it has really been spawned");
        Actor* skipActor = skipActorRef.getActor();
        if (skipActor)
        {
            Ray_SkipLevelComponent* skip = skipActor->GetComponent<Ray_SkipLevelComponent>();
            if (skip)
            {
                if (_show)
                    skip->start();
                else
                    skip->stop(_force);
            }
        }
    }

    bbool Ray_GameManagerRestoreDeath::isSkipActorDone()
    {
        ActorRef skipActorRef = RAY_GAMEMANAGER->getSkipLevelActor();
        ITF_WARNING_CATEGORY(GPP, NULL, skipActorRef.isValid(),
                             "Can't get skip level actor, check if it has really been spawned");
        Actor* skipActor = skipActorRef.getActor();
        bbool res = btrue;
        if (skipActor)
        {
            Ray_SkipLevelComponent* skip = skipActor->GetComponent<Ray_SkipLevelComponent>();
            if (skip)
            {
                res = skip->isDone();
            }
        }
        return res;
    }

    void Ray_GameManagerRestoreDeath::startSoundFadeOut()
    {
        if (!m_soundFaded)
        {
            // Stop all music
            MUSICMANAGER->stopAll(1.f);
            //stop default bus mix
            RAY_GAMEMANAGER->stopDefaultBusMix();
            //play fade out mix
            SOUND_MANAGER()->playBusMix(SOUND_BUS_MIX_FADEOUT, btrue);
            //update flag
            m_soundFaded = btrue;
        }
    }

    void Ray_GameManagerRestoreDeath::startSoundFadeIn()
    {
        if (m_soundFaded)
        {
            // Stop no sound bus mix
            SOUND_MANAGER()->stopBusMix(SOUND_BUS_MIX_FADEOUT, btrue);
            //set default bus mix
            RAY_GAMEMANAGER->setDefaultBusMix();
            m_soundFaded = bfalse;
        }
    }

    ITF::StringID Ray_GameManagerRestoreDeath::onMenuPageAction(UIMenu*, const StringID& _action,
                                                                const StringID& _defaultAction)
    {
        if (_action != ITF_GET_STRINGID_CRC(MENU_VALID, 1084313942))
            return UI_MENUMANAGER->getMenuPageAction_Nothing();
        else
            return _defaultAction;
    }

    ITF::bbool Ray_GameManagerRestoreDeath::canPause()
    {
        return m_currentState == STATE_FADE_BLACK || m_currentState == STATE_FADE_GAME;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Ray_GameManagerPlaySeqActor::start()
    {
        m_isFinished = bfalse;
    }

    void Ray_GameManagerPlaySeqActor::stop()
    {
        m_isFinished = bfalse;

        u32 numSeqs = m_playingSeqs.size();

        for (u32 i = 0; i < numSeqs; i++)
        {
            SeqInfo& seq = m_playingSeqs[i];
            Actor* actor = seq.m_actorRef.getActor();

            if (actor)
            {
                BaseObject* obj = GETOBJECT(seq.m_sceneRef);

                if (obj)
                {
                    Scene* scene = obj->DynamicCast<Scene>(ITF_GET_STRINGID_CRC(Scene, 209039730));

                    if (scene)
                    {
                        scene->unregisterPickable(actor);
                    }
                }
            }
        }
    }

    void Ray_GameManagerPlaySeqActor::addSequence(const Path& _path, const Vec3d& _pos, Scene* _scene)
    {
        Actor* seqActor = ACTORSMANAGER->spawnActor(_pos, _scene, _path, btrue,NULL);

        if (seqActor)
        {
            SequencePlayerComponent* seqComponent = seqActor->GetComponent<SequencePlayerComponent>();

            ITF_ASSERT(seqComponent);

            if (seqComponent)
            {
                EventSequenceControl playEvent;
                playEvent.setState(SequencePlayerComponent::State_Playing);
                seqComponent->onEvent(&playEvent);

                SeqInfo newSeq;

                newSeq.m_actorRef = seqActor->getRef();
                newSeq.m_sceneRef = _scene->getRef();

                m_playingSeqs.push_back(newSeq);
                ITF_WARNING(seqActor, m_playingSeqs.size() < 10, "Playing too much sequences at the same time!");
            }
        }
    }

    void Ray_GameManagerPlaySeqActor::update(f32 _dt)
    {
        u32 seqIndex = 0;
        u32 numSeqs = m_playingSeqs.size();

        while (seqIndex < numSeqs)
        {
            if (updateSeq(m_playingSeqs[seqIndex]))
            {
                seqIndex++;
            }
            else
            {
                m_playingSeqs.eraseKeepOrder(seqIndex);
                numSeqs--;
            }
        }

        m_isFinished = m_playingSeqs.size() == 0;
    }

    bbool Ray_GameManagerPlaySeqActor::updateSeq(const SeqInfo& _seq)
    {
        Actor* actor = _seq.m_actorRef.getActor();

        if (actor)
        {
            SequencePlayerComponent* sequence = actor->GetComponent<SequencePlayerComponent>();

            if (!sequence || sequence->getState() == SequencePlayerComponent::State_Stopped)
            {
                BaseObject* obj = GETOBJECT(_seq.m_sceneRef);

                if (obj)
                {
                    Scene* scene = obj->DynamicCast<Scene>(ITF_GET_STRINGID_CRC(Scene, 209039730));

                    if (scene)
                    {
                        scene->unregisterPickable(actor);
                    }
                }

                return bfalse;
            }
            else
            {
                return btrue;
            }
        }
        else
        {
            return bfalse;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Ray_GameManagerSequenceCheckpoint::Ray_GameManagerSequenceCheckpoint()
        : m_checkpoint(ITF_INVALID_OBJREF)
          , m_spawnedSeq(ITF_INVALID_OBJREF)
          , m_spawnedSeqScene(ITF_INVALID_OBJREF)
          , m_waitCounter(0.f)
          , m_state(STATE_WAITFORPLAYERSENTRY)
    {
    }

    void Ray_GameManagerSequenceCheckpoint::start(ObjectRef _checkpoint, const Path& _sequencePath, f32 _waitLength)
    {
        m_state = STATE_WAITFORPLAYERSENTRY;
        m_sequenceActorPath = _sequencePath;
        m_checkpoint = _checkpoint;
        m_waitCounter = _waitLength;
        m_spawnedSeq = ITF_INVALID_OBJREF;
        m_spawnedSeqScene = ITF_INVALID_OBJREF;
        m_playersInside.clear();
        m_playersInPosition.clear();
        m_isFinished = bfalse;
    }

    void Ray_GameManagerSequenceCheckpoint::stop()
    {
    }

    void Ray_GameManagerSequenceCheckpoint::resume()
    {
        Actor* seqActor = m_spawnedSeq.getActor();

        if (seqActor)
        {
            SequencePlayerComponent* seqComponent = seqActor->GetComponent<SequencePlayerComponent>();

            ITF_ASSERT(seqComponent);

            if (seqComponent)
            {
                EventSequenceControl playEvent;
                playEvent.setState(SequencePlayerComponent::State_Playing);
                seqComponent->onEvent(&playEvent);
            }
        }
    }

    void Ray_GameManagerSequenceCheckpoint::addPlayerSaving(ObjectRef _ref)
    {
        if (m_playersInside.find(_ref) != U32_INVALID)
        {
            return;
        }

        // tell the player that entered the trigger to move to its position
        Player* player = GAMEMANAGER->getPlayerFromActor(_ref);

        if (player)
        {
            Actor* playerActor = player->getActor();

            if (playerActor)
            {
                Vec2d targetPoint = getTargetPointForPlayer(player->getIndex());

                Ray_EventMoveToPoint moveTo(targetPoint);

                playerActor->onEvent(&moveTo);

                m_playersInside.push_back(_ref);
                m_playersInPosition.push_back(bfalse);
            }
        }

        // tell dead players to move to their position
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!player)
            {
                continue;
            }

            if (!player->getActive() || player->getIsLeaving())
            {
                continue;
            }

            if (player->getHitPoints())
            {
                continue;
            }

            if (player->getCurrentActor() == _ref)
            {
                continue;
            }

            if (Actor* playerActor = player->getActor())
            {
                Vec2d targetPoint = getTargetPointForPlayer(player->getIndex());
                Ray_EventMoveToPoint moveTo(targetPoint);
                playerActor->onEvent(&moveTo);
            }
        }
    }

    Vec2d Ray_GameManagerSequenceCheckpoint::getTargetPointForPlayer(u32 _playerIndex) const
    {
        Actor* checkpointActor = AIUtils::getActor(m_checkpoint);
        Vec2d targetP = Vec2d::Zero;

        if (checkpointActor)
        {
            CheckpointComponent* checkpointC = checkpointActor->GetComponent<CheckpointComponent>();

            if (checkpointC)
            {
                targetP = checkpointC->getPlayerSpawnPos(_playerIndex);
            }
            else
            {
                targetP = checkpointActor->get2DPos();
            }
        }

        return targetP;
    }

    void Ray_GameManagerSequenceCheckpoint::update(f32 _dt)
    {
        switch (m_state)
        {
        case STATE_WAITFORPLAYERSENTRY:
            updateWaitForPlayersEntry(_dt);
            break;
        case STATE_PLAYSEQUENCE:
            updatePlaySequence(_dt);
            break;
        }
    }

    void Ray_GameManagerSequenceCheckpoint::updateWaitForPlayersEntry(f32 _dt)
    {
        bbool finish = bfalse;

        m_waitCounter = Max(m_waitCounter - _dt, 0.f);

        if (m_waitCounter)
        {
            u32 insidePlayers = m_playersInside.size();
            bbool allInPosition = btrue;

            for (u32 i = 0; i < insidePlayers; i++)
            {
                if (!m_playersInPosition[i])
                {
                    Actor* playerActor = AIUtils::getActor(m_playersInside[i]);

                    if (playerActor)
                    {
                        Ray_EventQueryReachedMoveToPoint reachedPoint;

                        playerActor->onEvent(&reachedPoint);

                        if (!reachedPoint.getHasReached())
                        {
                            allInPosition = bfalse;
                        }
                        else
                        {
                            m_playersInPosition[i] = btrue;

                            EventStopPlayer stopPlayer(btrue);

                            playerActor->onEvent(&stopPlayer);
                        }
                    }
                }
            }

            if (m_playersInside.size() == GAMEMANAGER->getNumActivePlayers() && allInPosition)
            {
                finish = btrue;
            }
        }
        else
        {
            finish = btrue;
        }

        if (finish)
        {
            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

                if (!player || !player->getActive() || !player->isDead() || player->getIsLeaving())
                {
                    continue;
                }

                Actor* playerActor = player->getActor();

                if (!playerActor)
                {
                    continue;
                }

                player->revive();
            }

            Actor* checkpointActor = AIUtils::getActor(m_checkpoint);

            if (checkpointActor)
            {
                Actor* seqActor = ACTORSMANAGER->spawnActor(checkpointActor->getPos(),
                                                            checkpointActor->getScene(),
                                                            m_sequenceActorPath, btrue,NULL);

                if (seqActor)
                {
                    SequencePlayerComponent* seqComponent = seqActor->GetComponent<SequencePlayerComponent>();

                    ITF_ASSERT(seqComponent);

                    if (seqComponent)
                    {
                        // force link actor because many actor checkpoint (friendlyName is the same for all)
                        // solution: for this friendlyName link to this actor (by Ref)
                        ObjectPath objPath(SequencePlayerComponent::getSpawnerKey());
                        seqComponent->forceLinkActorByObjectPath(objPath, checkpointActor->getRef());

                        EventSequenceControl playEvent;
                        playEvent.setState(SequencePlayerComponent::State_Playing);
                        seqComponent->onEvent(&playEvent);

                        m_spawnedSeq = seqActor->getRef();
                        m_spawnedSeqScene = checkpointActor->getSceneRef();
                        m_state = STATE_PLAYSEQUENCE;

                        GAMEMANAGER->onReachedCheckpoint(m_checkpoint, btrue);
                    }
                }
            }
            else
            {
                m_isFinished = btrue;
            }
        }
    }

    void Ray_GameManagerSequenceCheckpoint::updatePlaySequence(f32 _dt)
    {
        Actor* actor = m_spawnedSeq.getActor();
        bbool finished = btrue;

        if (actor)
        {
            SequencePlayerComponent* sequence = actor->GetComponent<SequencePlayerComponent>();

            if (sequence)
            {
                if (sequence->getState() == SequencePlayerComponent::State_Stopped)
                {
                    BaseObject* obj = GETOBJECT(m_spawnedSeqScene);

                    if (obj)
                    {
                        Scene* scene = obj->DynamicCast<Scene>(ITF_GET_STRINGID_CRC(Scene, 209039730));

                        if (scene)
                        {
                            scene->unregisterPickable(actor);
                        }
                    }
                }
                else
                {
                    finished = bfalse;
                }
            }
        }

        if (finished)
        {
            Ray_EventResetMoveToPoint resetMove;

            u32 numInside = m_playersInside.size();

            for (u32 insideIndex = 0; insideIndex < numInside; insideIndex++)
            {
                Actor* playerActor = AIUtils::getActor(m_playersInside[insideIndex]);

                if (playerActor)
                {
                    playerActor->onEvent(&resetMove);
                }
            }

            m_isFinished = btrue;
        }
    }


    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    Ray_GameManagerSequenceChangePage::Ray_GameManagerSequenceChangePage()
        : m_prefetchRef(ITF_INVALID_OBJREF)
          , m_prefetchView(NULL)
          , m_travelFinished(bfalse)
          , m_verticalEjectToFinalPoint(bfalse)
          , m_state(STATE_NONE)
          , m_waitPlayerTimer(0.f)
          , m_useFade(bfalse)
          , m_waitTeleport(0)
    {
        m_destinationPage = ITF_INVALID_OBJREF;
    }

    Ray_GameManagerSequenceChangePage::~Ray_GameManagerSequenceChangePage()
    {
        if (m_prefetchView)
        {
            m_prefetchView->setActive(bfalse);
            World::enableLoadResourcesDelay();
            View::deleteView(m_prefetchView);
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    void Ray_GameManagerSequenceChangePage::addPlayer(ActorRef _playerRef)
    {
        if (m_state != STATE_WAIT_BEFORE_TRAVEL)
        {
            return;
        }

        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayerFromActor(_playerRef));

        if (!player)
        {
            return;
        }

        if (isActorOnList(player->getCurrentActor()))
        {
            return;
        }

        PlayerTravelInfo tmpTravelInfo;
        initPlayerTravel(player, tmpTravelInfo);
        m_playersTravelInfo.push_back(tmpTravelInfo);

        Actor* playerActor = AIUtils::getActor(player->getCurrentActor());
        ITF_ASSERT(playerActor != NULL); // player actor can't be null here !
        if (playerActor != NULL)
        {
            Ray_EventChangePage evt;

            evt.airTravelActivate();
            evt.setPageActor(m_teleportPage);
            evt.setTargetPageActor(m_destinationPage);

            playerActor->onEvent(&evt);
        }

        u32 numPlayers = GAMEMANAGER->getMaxPlayerCount();
        bbool keepWait = bfalse;

        for (u32 playerIndex = 0; playerIndex < numPlayers; playerIndex++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(playerIndex));

            if (player &&
                player->getActive() &&
                !player->isDead() &&
                player->getCurrentActor() != _playerRef &&
                !player->getIsLeaving())
            {
                if (!isActorOnList(player->getCurrentActor()))
                {
                    keepWait = btrue;
                }
            }
        }

        if (!keepWait)
        {
            m_waitPlayerTimer = 0.0f;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    bbool Ray_GameManagerSequenceChangePage::isActorOnList(ActorRef _ref) const
    {
        u32 numTravelling = m_playersTravelInfo.size();

        for (u32 i = 0; i < numTravelling; i++)
        {
            if (m_playersTravelInfo[i].m_player == _ref)
            {
                return btrue;
            }
        }

        return bfalse;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void Ray_GameManagerSequenceChangePage::initPlayerTravel(Ray_Player* _player,
                                                             PlayerTravelInfo& _travelInfo)
    {
        _travelInfo.m_player = _player->getCurrentActor();
        _travelInfo.m_dead = _player->isDead();
        _travelInfo.m_speed = 0.0f;
        _travelInfo.m_moveDir = Vec3d::Zero;
        _travelInfo.m_travelfinished = bfalse;
        _travelInfo.m_enterMovefinished = bfalse;
        _travelInfo.m_enterEjectForce = Vec2d::Zero;
        _travelInfo.m_spline.clear();

        Actor* actor = static_cast<Actor*>(GETOBJECT(_travelInfo.m_player));
        ITF_ASSERT(actor != NULL);
        if (actor == NULL)
        {
            return;
        }
        // Compute actor pos on screen borders
        const ICameraController* cameraController = CAMERACONTROLLERMANAGER->getMainCameraController();
        if ((_travelInfo.m_player != m_triggerPlayer) && (cameraController != NULL))
        {
            Vec3d camPos = cameraController->getCameraPosition();
            f32 camHeight = tanf(cameraController->getCameraFocale() * 0.5f) * cameraController->getDepth();
            f32 camWidth = camHeight * CAMERACONTROLLERMANAGER->m_screenRatio;
            Vec3d actorPos = actor->getPos();
            Vec3d vec = camPos - actorPos;
            vec.m_y = 0.0f;
            f32 horizDistFromCamPos = (vec).dot(Vec3d::Right);
            vec = camPos - actorPos;
            vec.m_x = 0.0f;
            f32 vertiDistFromCamPos = (vec).dot(Vec3d::Up);

            Vec3d newStartPos = actorPos;
            if (f32_Abs(horizDistFromCamPos) - camWidth > 0.f)
            {
                newStartPos += Vec3d::Right * (f32_Abs(horizDistFromCamPos) - camWidth) * getSign(horizDistFromCamPos);
            }
            if (f32_Abs(vertiDistFromCamPos) - camHeight > 0.f)
            {
                newStartPos += Vec3d::Up * (f32_Abs(vertiDistFromCamPos) - camHeight) * getSign(vertiDistFromCamPos);
            }
            if (!newStartPos.IsEqual(actorPos))
            {
                EventTeleport evtTeleport(newStartPos, 0.0f, bfalse);
                actor->onEvent(&evtTeleport);
            }
        }

        // Compute the spline
        //--------------------
        f32 decelDuration = 0.5f;
        f32 accelDuration = 1.0f;

        Vec3d nextPoint;
        _travelInfo.m_spline.AddSplinePoint(actor->getPos(), Spline::interp_spline, 0.0f);
        Vec3d playerExitPoint = m_travelExitPoint;
        AIUtils::getTravelExitPointWithActorFeet(m_travelExitPoint, playerExitPoint, actor);

        // Is not the triggered player
        //------------------------------
        if (_travelInfo.m_player != m_triggerPlayer)
        {
            if (StickToPolylinePhysComponent* actorPhysComponent = actor->GetComponent<StickToPolylinePhysComponent>())
            {
                const f32 speedMax = 5.0f;
                Vec2d speed = actorPhysComponent->getSpeed();
                f32 speedNorm = speed.norm();
                nextPoint = actor->getPos() + (speed.normalize() * Min(speedNorm, 2.0f)).to3d(actor->getPos().m_z);
                _travelInfo.m_spline.AddSplinePoint(nextPoint, Spline::interp_spline,
                                                    (Min(speedNorm, speedMax) / speedMax) * decelDuration);
            }

            // To destination point
            //
            Vec3d enterPoint = m_travelEnterPoint;
            enterPoint.m_z = actor->getDepth();

            // Check if the actor is between enter and exit point
            Vec3d vecToExit = playerExitPoint - actor->getPos();
            if (vecToExit.dot(enterPoint - actor->getPos()) < 0.0f)
            {
                Vec3d vecEnterToExit = playerExitPoint - enterPoint;
                vecEnterToExit.normalize();
                vecToExit.normalize();
                if (vecEnterToExit.dot(vecToExit) > 0.7f)
                {
                    enterPoint = playerExitPoint;
                }
            }

            _travelInfo.m_spline.AddSplinePoint(enterPoint, Spline::interp_spline, decelDuration + accelDuration);
            _travelInfo.m_spline.AddSplinePoint(playerExitPoint, Spline::interp_spline,
                                                decelDuration + accelDuration + accelDuration);
        }
        else
        {
            _travelInfo.m_spline.AddSplinePoint(playerExitPoint, Spline::interp_spline, accelDuration);
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    // Update player position according to travel infos
    //
    void Ray_GameManagerSequenceChangePage::updatePlayerTravel(f32 _dt, PlayerTravelInfo& _travelInfo)
    {
        Actor* actor = static_cast<Actor*>(GETOBJECT(_travelInfo.m_player));
        ITF_ASSERT(actor != NULL);
        if (actor == NULL)
        {
            return;
        }
        i32 p1, p2, p3;
        _travelInfo.m_spline.getPointsAtTime(_travelInfo.m_speed, p1, p2);
        p3 = p2;
        if (p1 >= 0)
        {
            p3 = Min(p1 + 2, _travelInfo.m_spline.GetNumPoints() - 1);
        }
        Vec3d p3Pos = _travelInfo.m_spline.GetNthPoint(p3);
        Vec3d dir = (p3Pos - actor->getPos()).normalize();
        f32 factor = Max(0.0f, _travelInfo.m_moveDir.dot(dir));
        f32 accel = 0.0;
        if (p1 > 0)
        {
            factor = 1.0f;
            accel = _travelInfo.m_speed * 3.0f;
        }

#ifndef ITF_FINAL
        //DebugDraw::arrow2D( m_actor->get2DPos(), m_actor->get2DPos() + dir, 0.0f );
#endif // ITF_FINAL
        // Compute the move speed
        f32 interPointDuration = 0.0;
        //f32 minSpeed = 0.0f;
        if (p1 >= 0)
        {
            interPointDuration = (_travelInfo.m_spline.getTimeAt(Min(p1 + 1, _travelInfo.m_spline.GetNumPoints() - 1)) -
                _travelInfo.m_spline.getTimeAt(p1));
            //minSpeed = interPointDuration * 0.2f;
        }
        //f32 curSpeed = decelDuration - minSpeed;

        f32 wantedSpeed = 1.0f;
        f32 speed = interPointDuration / wantedSpeed;
        f32 speedAdd = 0.0f;
        if (interPointDuration != 0.0f)
        {
            speedAdd = (accel / interPointDuration);
        }

        //_travelInfo.m_speed += ( _dt * interPointDuration / 1.0f )  +  ( _dt * speed * factor ) + ( ( accel / interPointDuration ) * _dt );
        _travelInfo.m_speed += _dt + (_dt * speed * factor) + (speedAdd * _dt);

        // get the interpolated position
        Vec3d newPos;
        _travelInfo.m_spline.GetInterpolatedAtTime(_travelInfo.m_speed, newPos);

        _travelInfo.m_moveDir = newPos - actor->getPos();
        _travelInfo.m_moveDir.normalize();
#ifndef ITF_FINAL
        //DebugDraw::arrow2D( newPos.truncateTo2D(), newPos.truncateTo2D() + _travelInfo.m_moveDir, 0.0f );
#endif // ITF_FINAL

        EventTeleport evtTeleport(newPos, 0.0f, bfalse);
        actor->onEvent(&evtTeleport);
        //actor->setPos( newPos );

        // Check if the travel is finished ( if the previous spline point is invalid)
        if (!_travelInfo.m_travelfinished)
        {
            if ((p1 < 0) || (p1 == p2))
            {
                _travelInfo.m_travelfinished = btrue;
                //LOG( "----- SPLINE FINISHED ");
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    //
    void Ray_GameManagerSequenceChangePage::start(Vec3d const& _enterPoint,
                                                  Vec3d const& _exitPoint,
                                                  Vec3d const& _finalPoint,
                                                  bbool _verticalEjectToFinalPoint,
                                                  ActorRef _triggerPlayer,
                                                  ActorRef _teleportPage,
                                                  ActorRef _destinationPage,
                                                  ActorRef _checkpoint,
                                                  f32 _waitPlayersDuration,
                                                  bbool _useFade,
                                                  bbool _isCageMap)
    {
        // Init vars
        m_playersTravelInfo.clear();
        m_triggerPlayer = _triggerPlayer;
        m_travelEnterPoint = _enterPoint;
        m_travelExitPoint = _exitPoint;
        m_travelFinalPoint = _finalPoint;
        m_verticalEjectToFinalPoint = _verticalEjectToFinalPoint;
        m_teleportPage = _teleportPage;
        m_destinationPage = _destinationPage;
        m_checkpoint = _checkpoint;
        m_waitPlayerTimer = _waitPlayersDuration;
        m_useFade = _useFade;
        m_isFinished = bfalse;
        m_travelFinished = bfalse;

        Actor* srcPageActor = m_teleportPage.getActor();
        if (srcPageActor && _isCageMap)
        {
            m_cageMapEntry = PackedObjectPath(srcPageActor);
        }
        else
        {
            m_cageMapEntry = PackedObjectPath();
        }
        // Check if the destination page is active, if not, force fade out
        Actor* destPage = m_destinationPage.getActor();
        if ((destPage != NULL) && !destPage->isActive())
        {
            m_useFade = btrue;
        }

        // Disable time attack
        Actor* timeAttackChrono = RAY_GAMEMANAGER->getTimeAttackActorChrono().getActor();

        if (timeAttackChrono)
        {
            timeAttackChrono->disable();
        }

        if (srcPageActor)
        {
            LinkComponent* linkComponent = srcPageActor->GetComponent<LinkComponent>();

            if (linkComponent)
            {
                const LinkComponent::ChildrenList& children = linkComponent->getChildren();
                u32 numChildren = children.size();

                for (u32 i = 0; i < numChildren; i++)
                {
                    const ChildEntry& child = children[i];

                    if (!child.getTagValueList().empty())
                    {
                        continue;
                    }

                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if (path.getIsAbsolute())
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(srcPageActor, path);
                    }

                    if (target)
                    {
                        Actor* targetActor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));

                        if (targetActor)
                        {
                            EventTrigger triggerOn;
                            triggerOn.setSender(srcPageActor->getRef());
                            triggerOn.setActivated(btrue);
                            targetActor->onEvent(&triggerOn);
                        }
                    }
                }
            }
        }

        if ((m_waitPlayerTimer > 0.0f) && needWaitForPlayers())
        {
            changeState(STATE_WAIT_BEFORE_TRAVEL);
        }
        else
        {
            changeState(STATE_TRAVELLING);
        }

        setPrefetch();
    }

    bbool Ray_GameManagerSequenceChangePage::needWaitForPlayers() const
    {
        u32 numPlayers = GAMEMANAGER->getMaxPlayerCount();
        u32 activePlayers = 0;

        for (u32 playerIndex = 0; playerIndex < numPlayers; playerIndex++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(playerIndex));

            if (!player || !player->getActive() || player->getIsLeaving())
            {
                continue;
            }

            Actor* playerActor = GAMEMANAGER->getPlayer(playerIndex)->getActor();

            if (!playerActor)
            {
                continue;
            }

            EventQueryIsDead deadCheck;

            playerActor->onEvent(&deadCheck);

            if (!deadCheck.getIsDead())
            {
                activePlayers++;
            }
        }

        return activePlayers > 1;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //
    void Ray_GameManagerSequenceChangePage::changeState(ChangePageState _newState)
    {
        m_state = _newState;

        switch (_newState)
        {
        //****************************************
        case STATE_WAIT_BEFORE_TRAVEL:
            {
                // travel only the trigger player
                Player* triggerPlayer = GAMEMANAGER->getPlayerFromActor(m_triggerPlayer);

                if (triggerPlayer != NULL)
                {
                    PlayerTravelInfo tmpTravelInfo;
                    initPlayerTravel(static_cast<Ray_Player*>(triggerPlayer), tmpTravelInfo);
                    m_playersTravelInfo.push_back(tmpTravelInfo);

                    Ray_EventChangePage evt;
                    evt.airTravelActivate();
                    evt.setPageActor(m_teleportPage);
                    evt.setTargetPageActor(m_destinationPage);
                    Actor* playerActor = AIUtils::getActor(triggerPlayer->getCurrentActor());
                    ITF_ASSERT(playerActor != NULL); // player actor can't be null here !
                    if (playerActor != NULL)
                    {
                        playerActor->onEvent(&evt);
                    }
                }
                break;
            }
        //****************************************
        case STATE_TRAVELLING:
            {
                m_travelFinished = bfalse;

                // Get all playing actors
                u32 numPlayers = GAMEMANAGER->getMaxPlayerCount();
                PlayerTravelInfo tmpTravelInfo;

                for (u32 playerIndex = 0; playerIndex < numPlayers; playerIndex++)
                {
                    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(playerIndex));

                    if (player &&
                        player->getActive() &&
                        !player->getIsLeaving() &&
                        !isActorOnList(player->getCurrentActor()))
                    {
                        // page system only in platformer mode
                        if (player->getCurrentMode() != RAY_GAMEMODE_PLATFORMER)
                        {
                            GAMEMANAGER->changePlayerGameMode(player, RAY_GAMEMODE_PLATFORMER);
                        }

                        initPlayerTravel(player, tmpTravelInfo);
                        m_playersTravelInfo.push_back(tmpTravelInfo);

                        Actor* playerActor = player->getActor();

                        if (playerActor)
                        {
                            Ray_EventChangePage changePageEvent;
                            changePageEvent.airTravelActivate();
                            changePageEvent.setPageActor(m_teleportPage);
                            changePageEvent.setTargetPageActor(m_destinationPage);
                            playerActor->onEvent(&changePageEvent);
                        }
                    }
                }
                break;
            }
        //****************************************
        case STATE_TRAVELLING_FINISHED:
            {
                Ray_AIScoreLumBehavior* behavior = Ray_AIUtils::fetchScoreLumBehavior();
                if (behavior)
                {
                    behavior->clearRedMode();
                }

                if (m_useFade)
                {
                    changeState(STATE_FADE_OUT);
                }
                else
                {
                    changeState(STATE_ENTER);
                }
                break;
            }

        //****************************************
        case STATE_FADE_OUT:
            {
                RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeChangePage());
                break;
            }
        //****************************************
        case STATE_TELEPORT:
            {
                Actor* destPage = m_destinationPage.getActor();

                if (destPage != NULL)
                {
                    u32 numPlayers = m_playersTravelInfo.size();

                    for (u32 i = 0; i < numPlayers; i++)
                    {
                        PlayerTravelInfo& info = m_playersTravelInfo[i];
                        Actor* targetActor = info.m_player.getActor();

                        if (targetActor)
                        {
                            Vec3d targetPos = destPage->getPos();

                            targetPos.m_z = AIUtils::getSceneZFromPickable(destPage);

                            GAMEMANAGER->setCurrentSceneReference(destPage->getScene());

                            if (targetActor->getTemplate()->isZForced())
                            {
                                targetPos.m_z += targetActor->getTemplate()->getForcedZ();
                            }

                            EventTeleport evtTeleport(targetPos, 0.f, btrue);

                            targetActor->onEvent(&evtTeleport);
                        }
                    }

                    CAMERACONTROLLERMANAGER->teleport(destPage->getPos());
                    m_waitTeleport = 1;
                }
                else
                {
                    m_waitTeleport = 0;
                }
                break;
            }
        //****************************************
        case STATE_FADE_IN:
            {
                RAY_GAMEMANAGER->stopFade(bfalse);
                changeState(STATE_ENTER);
                break;
            }
        //****************************************
        case STATE_ENTER:
            {
                // Check destination page object
                //----------------------------------
                Actor* destPage = m_destinationPage.getActor();
                bbool needEjectProcess = bfalse;

                if (destPage != NULL)
                {
                    Ray_EventChangePageQueryPoints evtQueryPoints;
                    destPage->onEvent(&evtQueryPoints);
                    m_travelExitPoint = evtQueryPoints.getExitPoint();
                    m_travelFinalPoint = evtQueryPoints.getFinalPoint();
                    needEjectProcess = evtQueryPoints.isFinalPointNeedEject();
                }
                else
                {
                    //Actor no more valid !!, the situation must be checked
                    ITF_ASSERT(m_destinationPage == ITF_INVALID_OBJREF);
                    if (m_destinationPage != ITF_INVALID_OBJREF)
                    {
                        m_travelFinalPoint = m_travelExitPoint;
                    }
                }

                // Make everyone have the same scale
                Actor* triggerer = AIUtils::getActor(m_triggerPlayer);
                f32 setScaleValue = 0.f;

                if (triggerer)
                {
                    setScaleValue = triggerer->getScale().m_x / triggerer->getWorldInitialScale().m_x;

                    RAY_GAMEMANAGER->setPlayersCurrentScale(setScaleValue);
                }

                // Send move to final point
                //--------------------------
                ObjectRef teleportedPlayer = ITF_INVALID_OBJREF;
                Vec2d teleportedPlayerPos = Vec2d::Zero;
                Vec3d moveToPoint;
                Vec3d dirToExit = m_travelExitPoint - m_travelFinalPoint;

                dirToExit.normalize();

                for (u32 i = 0; i < m_playersTravelInfo.size(); ++i)
                {
                    // Update travel
                    PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];
                    Actor* actor = static_cast<Actor*>(GETOBJECT(tmpTravelInfo.m_player));
                    ITF_ASSERT(actor != NULL);
                    if (actor != NULL)
                    {
                        // Reset the scale
                        if (setScaleValue)
                        {
                            actor->setScale(Vec2d(setScaleValue, setScaleValue) * actor->getWorldInitialScale().m_x);
                        }

                        // has destination page
                        //----------------------
                        moveToPoint = m_travelFinalPoint; // + (dirToExit*static_cast<f32>(i)*0.5f); RO-9400

                        if (destPage != NULL)
                        {
                            // teleport to exit point
                            Vec3d actorPos;
                            AIUtils::getTravelExitPointWithActorFeet(m_travelExitPoint, actorPos, actor);
                            EventTeleport evtTeleport(actorPos, 0.0f, bfalse);
                            actor->onEvent(&evtTeleport);

                            if (teleportedPlayer == ITF_INVALID_OBJREF)
                            {
                                teleportedPlayerPos = actorPos.truncateTo2D();
                            }

                            // Compute eject force
                            if (needEjectProcess)
                            {
                                tmpTravelInfo.m_enterEjectForce = AIUtils::computeChangePageVerticalEjectForce(
                                    actor, m_travelFinalPoint.truncateTo2D());
                            }
                        }
                        else if (teleportedPlayer == ITF_INVALID_OBJREF)
                        {
                            teleportedPlayerPos = actor->get2DPos();
                        }

                        teleportedPlayer = tmpTravelInfo.m_player;

                        Ray_EventChangePage changePageEvent;
                        changePageEvent.setPageActor(destPage != NULL ? m_destinationPage : m_teleportPage);
                        changePageEvent.enterMovePosActivate(moveToPoint, tmpTravelInfo.m_enterEjectForce);
                        actor->onEvent(&changePageEvent);
                    }
                }

                if (destPage)
                {
                    CAMERACONTROLLERMANAGER->teleport(destPage->getPos());
                }

                if (teleportedPlayer != ITF_INVALID_OBJREF)
                {
                    Ray_EventTeleportUsed teleportUsed(teleportedPlayer, m_teleportPage, teleportedPlayerPos);

                    RAY_GAMEMANAGER->teleportRescuedFriendlies(&teleportUsed);
                }

                break;
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    //
    void Ray_GameManagerSequenceChangePage::updateTravellingState(f32 _dt)
    {
        u32 numPlayer = m_playersTravelInfo.size();
        ITF_ASSERT(numPlayer != 0);

        // Players travel update
        //--------------------------
        if (!m_travelFinished)
        {
            bbool travelFinished = btrue;

            for (u32 i = 0; i < numPlayer; ++i)
            {
                // Update travel
                PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];
                bbool playerTravelFinished = tmpTravelInfo.m_travelfinished;
                updatePlayerTravel(_dt, tmpTravelInfo);

#ifndef ITF_FINAL
                /*for ( i32 ptIdx = 0; ptIdx < tmpTravelInfo.m_spline.GetNumPoints() ; ptIdx++ )
                {
                    DebugDraw::circle( tmpTravelInfo.m_spline.GetNthPoint( ptIdx ), 0.2f );
                }*/
#endif // ITF_FINAL
                travelFinished = travelFinished && tmpTravelInfo.m_travelfinished;

                // player travel finished, compute vertical eject force if requested
                //
                if (m_verticalEjectToFinalPoint && !playerTravelFinished && tmpTravelInfo.m_travelfinished)
                {
                    Actor* actor = tmpTravelInfo.m_player.getActor();
                    tmpTravelInfo.m_enterEjectForce = AIUtils::computeChangePageVerticalEjectForce(
                        actor, m_travelFinalPoint.truncateTo2D());
                }
            }

            // All players are at the exit point
            //
            if (travelFinished)
            {
                if (m_state == STATE_TRAVELLING && m_playersTravelInfo.size() > 1)
                {
                    Actor* actor = m_playersTravelInfo[1].m_player.getActor();

                    if (actor)
                    {
                        Ray_EventChangePage changePageFx;
                        changePageFx.setSender(m_teleportPage);
                        changePageFx.setPlayFx();
                        actor->onEvent(&changePageFx);
                    }
                }

                m_travelFinished = btrue;
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    //
    void Ray_GameManagerSequenceChangePage::updateEnterState(f32 _dt)
    {
        bbool enterMoveFinished = btrue;
        Actor* actor;

        for (u32 i = 0; i < m_playersTravelInfo.size(); ++i)
        {
            PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];

            // Move not finished
            if (!tmpTravelInfo.m_enterMovefinished)
            {
                actor = static_cast<Actor*>(GETOBJECT(tmpTravelInfo.m_player));
                ITF_ASSERT(actor != NULL);
                if (actor != NULL)
                {
                    Player* player = GAMEMANAGER->getPlayerFromActor(tmpTravelInfo.m_player);

                    if (player && player->isDead() && !tmpTravelInfo.m_dead)
                    {
                        tmpTravelInfo.m_enterMovefinished = btrue;
                        tmpTravelInfo.m_dead = btrue;
                    }
                    else
                    {
                        Ray_EventQueryReachedMoveToPoint moveFinishedEvt;
                        actor->onEvent(&moveFinishedEvt);
                        tmpTravelInfo.m_enterMovefinished = moveFinishedEvt.getHasReached();
                    }

                    enterMoveFinished &= tmpTravelInfo.m_enterMovefinished;
                }
            }
        }

        // FINISHED
        if (enterMoveFinished)
        {
            clearPrefetch();

            if (m_checkpoint != ITF_INVALID_OBJREF)
            {
                RAY_GAMEMANAGER->onReachedCheckpoint(m_checkpoint, btrue);
            }

            Ray_EventChangePage changePageEvent;
            changePageEvent.setFinished();
            changePageEvent.setPageActor(m_destinationPage.getActor() != NULL ? m_destinationPage : m_teleportPage);

            for (u32 i = 0; i < m_playersTravelInfo.size(); ++i)
            {
                PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];

                // Move not finished
                actor = static_cast<Actor*>(GETOBJECT(tmpTravelInfo.m_player));
                ITF_ASSERT(actor != NULL);
                if (actor != NULL)
                {
                    actor->onEvent(&changePageEvent);
                }
            }

            Actor* finalPage;

            if (m_destinationPage != ITF_INVALID_OBJREF)
            {
                finalPage = m_destinationPage.getActor();
            }
            else
            {
                finalPage = m_teleportPage.getActor();
            }

            if (finalPage != NULL)
            {
                Ray_EventChangePage changePageEvent;

                changePageEvent.setFinished();
                changePageEvent.setPageActor(finalPage->getRef());

                finalPage->onEvent(&changePageEvent);
            }

            changeState(STATE_POST_ENTER);
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    void Ray_GameManagerSequenceChangePage::updatePostEnterState(f32 _dt)
    {
        if (m_cageMapEntry.isValid())
        {
            RAY_GAMEMANAGER->setCageMapPassedDoor(m_cageMapEntry, btrue);
            //RAY_GAMEMANAGER->setMurphyAssistFollowPlayer();
        }

        m_isFinished = true;

        changeState(STATE_NONE);
    }

    //---------------------------------------------------------------------------------------------------------------------
    //
    void Ray_GameManagerSequenceChangePage::update(f32 _dt)
    {
        if (RAY_GAMEMANAGER->getNumDeadPlayers() == RAY_GAMEMANAGER->getNumActivePlayers())
        {
            m_isFinished = btrue;
        }

        switch (m_state)
        {
        case STATE_WAIT_BEFORE_TRAVEL:
            {
                // update trigger player
                updateTravellingState(_dt);

                m_waitPlayerTimer -= _dt;
                if (m_waitPlayerTimer <= 0.0f)
                {
                    m_waitPlayerTimer = 0.0f;
                    changeState(STATE_TRAVELLING);
                }
                break;
            }
        case STATE_TRAVELLING:
            {
                updateTravellingState(_dt);

                if (m_travelFinished)
                {
                    changeState(STATE_TRAVELLING_FINISHED);
                }
                break;
            }
        case STATE_FADE_OUT:
            {
                if (RAY_GAMEMANAGER->isFadeOutFinished())
                {
                    if (checkPrefetchFinished())
                    {
                        clearPrefetch();
                        changeState(STATE_TELEPORT);
                    }
                }
                break;
            }
        case STATE_TELEPORT:
            {
                if (m_waitTeleport > 0)
                {
                    m_waitTeleport--;
                }
                else
                {
                    changeState(STATE_FADE_IN);
                }
            }
            break;
        case STATE_ENTER:
            {
                updateEnterState(_dt);
                break;
            }
        case STATE_POST_ENTER:
            {
                updatePostEnterState(_dt);
                break;
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    //
    void Ray_GameManagerSequenceChangePage::clearPrefetch()
    {
        if (m_prefetchRef != ITF_INVALID_OBJREF)
        {
            Actor* prefetchActor = m_prefetchRef.getActor();

            if (prefetchActor)
            {
                AABBPrefetchComponent* prefetchComponent = prefetchActor->GetComponent<AABBPrefetchComponent>();

                if (prefetchComponent)
                {
                    prefetchComponent->forceStart(bfalse);
                }
            }

            m_prefetchRef = ITF_INVALID_OBJREF;
        }
        else if (m_prefetchView)
        {
            m_prefetchView->setActive(bfalse);
            World::enableLoadResourcesDelay();
            View::deleteView(m_prefetchView);
            m_prefetchView = NULL;
        }
    }

    void Ray_GameManagerSequenceChangePage::setPrefetch()
    {
        clearPrefetch();

        Actor* page = m_teleportPage.getActor();

        if (page)
        {
            LinkComponent* linkComponent = page->GetComponent<LinkComponent>();

            if (linkComponent)
            {
                const LinkComponent::ChildrenList& children = linkComponent->getChildren();
                u32 numChildren = children.size();

                for (u32 i = 0; i < numChildren; i++)
                {
                    const ChildEntry& child = children[i];

                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if (path.getIsAbsolute())
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(page, path);
                    }

                    if (target)
                    {
                        Actor* targetActor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));

                        if (targetActor)
                        {
                            AABBPrefetchComponent* prefetchComponent = targetActor->GetComponent<
                                AABBPrefetchComponent>();

                            if (prefetchComponent)
                            {
                                m_prefetchRef = targetActor->getRef();
                                prefetchComponent->forceStart(btrue);
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (m_prefetchRef == ITF_INVALID_OBJREF)
        {
            Actor* destPage = m_destinationPage.getActor();

            if (destPage)
            {
                if (!m_prefetchView)
                    m_prefetchView = View::createView("Ray_GameManagerSequenceTeleportCheat", bfalse, btrue);

                const f32 deltaX = CAMERA->getDeltaX();
                const f32 deltaY = CAMERA->getDeltaY();
                const Vec3d& destPos = destPage->getPos();
                AABB targetAABB(destPos);

                targetAABB.grow(destPos + Vec3d(-5.f, -5.f, 0.f));
                targetAABB.grow(destPos + Vec3d(5.f, 5.f, 0.f));
                targetAABB.projectTo2D(deltaX, deltaY, destPage->getDepth());
                m_prefetchView->setTargetAABB(targetAABB);
                m_prefetchView->setActive(btrue);
                World::disableLoadResourcesDelay();
            }
        }
    }

    bbool Ray_GameManagerSequenceChangePage::checkPrefetchFinished() const
    {
        Actor* prefetchActor = m_prefetchRef.getActor();

        if (prefetchActor)
        {
            AABBPrefetchComponent* prefetchComponent = prefetchActor->GetComponent<AABBPrefetchComponent>();

            if (prefetchComponent)
            {
                return prefetchComponent->prefetchComplete();
            }
        }
        else if (m_prefetchView)
        {
            World* pWorld = CURRENTWORLD;

            if (pWorld)
            {
                return pWorld->isPhysicalReady(*m_prefetchView);
            }
        }

        return btrue;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //
    void Ray_GameManagerSequenceChangePage::stop()
    {
        if (m_state == STATE_FADE_OUT || m_state == STATE_FADE_OUT_WAIT)
        {
            Color endColor = Color::black();
            endColor.m_a = 0.f;

            RAY_GAMEMANAGER->stopFade(btrue);
        }

        EventReset resetPlayer;
        Ray_EventChangePage changePageStop;

        for (u32 i = 0; i < m_playersTravelInfo.size(); ++i)
        {
            PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];

            Actor* actor = AIUtils::getActor(tmpTravelInfo.m_player);

            if (actor != NULL)
            {
                actor->onEvent(&changePageStop);
                actor->onEvent(&resetPlayer);
            }
        }

        clearPrefetch();
    }


    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    Ray_GameManagerSequenceStargate::Ray_GameManagerSequenceStargate()
        : m_speed(0.f)
          , m_timer(0.f)
          , m_state(SEQSTATE_TAKEOFF)
          , m_currentNode(ITF_INVALID_OBJREF)
          , m_currentPrefetch(ITF_INVALID_OBJREF)
          , m_lastTrip(bfalse)
          , m_fadeToBlack(bfalse)
          , m_fadeToGame(bfalse)
          , m_fadeDuration(0.f)
          , m_fadeTimer(0.f)
          , m_waitForPlayersTime(2.f)
          , m_waitDistFromDoor(4.f)
          , m_waitAngleOffset(0.3f)
          , m_waitPointZOffset(3.f)
          , m_startToDoorTimeMultiplier(3.f)
          , m_cameraLookAtOffset(0.f, 0.f, 10.f)
    {
    }

    Ray_GameManagerSequenceStargate::~Ray_GameManagerSequenceStargate()
    {
    }

    void Ray_GameManagerSequenceStargate::start(ActorRef _stargate, const Vec3d& _start, f32 _speed,
                                                f32 _waitForPlayersTime,
                                                f32 _waitDistFromDoor, f32 _waitAngleOffset, f32 _waitPointZOffset,
                                                f32 _startToDoorTimeMultiplier, const Vec3d& _cameraOffset)
    {
        m_start = _start;
        m_speed = _speed;
        m_state = SEQSTATE_TAKEOFF;
        m_timer = 0.f;
        m_isFinished = bfalse;
        m_waitForPlayersTime = _waitForPlayersTime;
        m_waitDistFromDoor = _waitDistFromDoor;
        m_waitAngleOffset = _waitAngleOffset;
        m_waitPointZOffset = _waitPointZOffset;
        m_startToDoorTimeMultiplier = _startToDoorTimeMultiplier;
        m_currentNode = m_stargate = _stargate;
        m_currentPrefetch = ITF_INVALID_OBJREF;
        m_lastTrip = bfalse;
        m_fadeToBlack = bfalse;
        m_fadeToGame = bfalse;
        m_fadeDuration = 0.f;
        m_fadeTimer = 0.f;
        m_cameraLookAtOffset = _cameraOffset;

        Actor* stargateActor = _stargate.getActor();

        if (stargateActor)
        {
            Ray_EventTriggerStargate activatedEvent;
            stargateActor->onEvent(&activatedEvent);
        }

        FixedArray<Actor*, 32> actors;

        for (u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!otherPlayer->getActive() || otherPlayer->getIsLeaving())
            {
                continue;
            }

            Actor* playerActor = otherPlayer->getActor();

            if (!playerActor)
            {
                continue;
            }

            actors.push_back(playerActor);
        }

        RAY_GAMEMANAGER->resetRescuedFriendlies();

        u32 totalActors = actors.size();

        m_playerData.resize(actors.size());

        for (u32 i = 0; i < totalActors; i++)
        {
            setupPlayer(i, actors[i], _start, _speed);
        }
    }

    void Ray_GameManagerSequenceStargate::setupPlayer(u32 _index, Actor* _actor, const Vec3d& _start, f32 _speed)
    {
        ActorInfo& info = m_playerData[_index];

        info.m_actor = _actor->getRef();
        info.m_currentState = ACTORSTATE_SUCKIN;
        info.m_finished = bfalse;

        Vec3d suckedInPosition;

        calculateStartPosForPlayer(_index, _actor, _start, suckedInPosition);

        info.m_startDif = (suckedInPosition - _start).truncateTo2D();

        Ray_EventStargateUse stargateStart;

        stargateStart.setStage(STARGATESTAGE_SUCKIN);
        stargateStart.setSuckPoint(suckedInPosition);
        stargateStart.setStargateStart(_start);

        _actor->onEvent(&stargateStart);
    }

    void Ray_GameManagerSequenceStargate::calculateStartPosForPlayer(u32 _index, Actor* _actor, const Vec3d& _start,
                                                                     Vec3d& _ret)
    {
        f32 angle;

        if ((m_playerData.size() % 2) == 0)
        {
            if ((_index % 2) == 0)
            {
                angle = -(m_waitAngleOffset * static_cast<f32>(_index + 1));
            }
            else
            {
                angle = (m_waitAngleOffset * static_cast<f32>(_index + 1));
            }
        }
        else
        {
            if (_index == 0)
            {
                angle = 0.f;
            }
            else if ((_index % 2) == 0)
            {
                angle = -(m_waitAngleOffset * static_cast<f32>(_index));
            }
            else
            {
                angle = (m_waitAngleOffset * static_cast<f32>(_index));
            }
        }

        Vec2d offsetDir = Vec2d::Up.Rotate(angle);

        _ret = _start + (offsetDir * m_waitDistFromDoor).to3d();
        _ret.m_z = _actor->getPos().m_z;
        _ret.m_z += m_waitPointZOffset;
    }

    Actor* Ray_GameManagerSequenceStargate::getNextNode(Actor* _currentNode) const
    {
        if (_currentNode->GetComponent<Ray_StargateNodeComponent>() == NULL && _currentNode->getRef() != m_stargate)
        {
            return NULL;
        }
        else
        {
            LinkComponent* linkC = _currentNode->GetComponent<LinkComponent>();

            const LinkComponent::ChildrenList& children = linkC->getChildren();
            u32 numChildren = children.size();
            Actor* retActor = NULL;

            for (u32 i = 0; i < numChildren; i++)
            {
                const ObjectPath& objPath = children[i].getPath();
                Pickable* pickable;

                if (objPath.getIsAbsolute())
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(objPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(_currentNode, objPath);
                }

                if (pickable)
                {
                    Actor* child = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));

                    if (child)
                    {
                        if (child->GetComponent<Ray_StargateNodeComponent>() != NULL)
                        {
                            retActor = child;
                            break;
                        }
                        else if (!retActor)
                        {
                            retActor = child;
                        }
                    }
                }
            }

            return retActor;
        }
    }

    void Ray_GameManagerSequenceStargate::setPlayerSpline(Actor* _player, ActorInfo& _actorInfo,
                                                          bbool _updateCurrentNode, Spline& _spline)
    {
        f32 dist;
        Vec3d prevP, temp;
        f32 t = 0.f;
        Actor* currentNode = AIUtils::getActor(m_currentNode);

        if (m_currentNode == m_stargate)
        {
            // - At the start
            _spline.AddSplinePoint(_player->getPos(), Spline::interp_spline, t);
            prevP = _player->getPos();

            // - At the door itself
            dist = (m_start - prevP).norm();
            t += (dist / m_speed) * m_startToDoorTimeMultiplier;
            _spline.AddSplinePoint(m_start, Spline::interp_spline, t);
            prevP = m_start;
        }
        else
        {
            _spline.AddSplinePoint(currentNode->getPos() + _actorInfo.m_startDif.to3d(), Spline::interp_spline, t);
            prevP = currentNode->getPos();
        }

        if (currentNode)
        {
            Actor* nextNode = getNextNode(currentNode);
            bbool finished = nextNode == NULL;

            if (finished)
            {
                m_lastTrip = btrue;
            }
            else
            {
                while (!finished)
                {
                    Ray_StargateNodeComponent* stargateNode = nextNode->GetComponent<Ray_StargateNodeComponent>();

                    currentNode = nextNode;
                    temp = nextNode->getPos();
                    temp += _actorInfo.m_startDif.to3d();

                    if (stargateNode)
                    {
                        // Another node. If it's a teleporter we will stop there
                        if (stargateNode->getType() == STARGATENODETYPE_TELEPORT)
                        {
                            finished = btrue;
                            m_fadeToBlack = btrue;
                            m_fadeToGame = bfalse;
                            m_fadeDuration = stargateNode->getFadeLength();
                        }
                    }
                    else
                    {
                        // This is the last point
                        temp.m_z = AIUtils::getSceneZFromPickable(nextNode);

                        if (_player->getTemplate()->isZForced())
                        {
                            temp.m_z += _player->getTemplate()->getForcedZ();
                        }

                        m_lastTrip = btrue;
                        finished = btrue;
                    }

                    dist = (temp - prevP).norm();
                    f32 time = (dist / m_speed);

                    if (stargateNode)
                    {
                        time *= stargateNode->getTimeMultiplier();
                    }

                    t += time;
                    _spline.AddSplinePoint(temp, Spline::interp_spline, t);
                    prevP = temp;

                    if (!finished)
                    {
                        nextNode = getNextNode(currentNode);
                        finished = nextNode == NULL;
                    }
                    else if (m_fadeToBlack && stargateNode)
                    {
                        m_fadeTimer = Max(_spline.getTimeAt(_spline.GetNumPoints() - 1) - stargateNode->getFadeLength(),
                                          0.f);
                    }
                }

                if (_updateCurrentNode)
                {
                    m_currentNode = currentNode->getRef();
                    findCurrentPrefetch();
                }
            }
        }
    }

    void Ray_GameManagerSequenceStargate::stop()
    {
        Ray_EventStargateUse stargateFinish;

        stargateFinish.setStage(STARGATESTAGE_END);

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (player->getActive() || player->getIsLeaving())
            {
                Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                if (playerActor)
                {
                    playerActor->onEvent(&stargateFinish);
                }
            }
        }

        clearPrefetch();

        CAMERACONTROLLERMANAGER->stopCameraSequence(0.05f);
    }

    void Ray_GameManagerSequenceStargate::onEvent(Event* _event)
    {
        if (Ray_EventStargateStageFinished* stargateStageFinishedEvent = _event->DynamicCast<
            Ray_EventStargateStageFinished>(ITF_GET_STRINGID_CRC(Ray_EventStargateStageFinished, 211966772)))
        {
            for (ITF_VECTOR<ActorInfo>::iterator it = m_playerData.begin(); it != m_playerData.end(); ++it)
            {
                ActorInfo& info = *it;

                if (info.m_actor != _event->getSender())
                {
                    continue;
                }

                if (stargateStageFinishedEvent->getAbort())
                {
                    m_playerData.erase(it);
                }
                else
                {
                    info.m_currentState = ACTORSTATE_WAIT;
                }

                break;
            }
        }
    }

    void Ray_GameManagerSequenceStargate::preparePlayersTrip()
    {
        u32 numPlayers = m_playerData.size();

        for (u32 i = 0; i < numPlayers; i++)
        {
            ActorInfo& info = m_playerData[i];
            Actor* actor = info.m_actor.getActor();

            if (!actor)
            {
                continue;
            }

            info.m_currentState = ACTORSTATE_TRAVEL;

            Ray_EventStargateUse stargateUse;

            stargateUse.setStage(STARGATESTAGE_TRAVEL);
            setPlayerSpline(actor, info, i == (numPlayers - 1), stargateUse.getSpline());
            stargateUse.setIsLastTrip(m_lastTrip);

            actor->onEvent(&stargateUse);
        }
    }

    void Ray_GameManagerSequenceStargate::update(f32 _dt)
    {
        if (m_playerData.size() == 0)
        {
            m_isFinished = btrue;
            return;
        }

        switch (m_state)
        {
        case SEQSTATE_TAKEOFF:
            updateTakeOff(_dt);
            break;
        case SEQSTATE_TRAVEL:
            updateTravel(_dt);
            break;
        }
    }

    void Ray_GameManagerSequenceStargate::updateTakeOff(f32 _dt)
    {
        u32 numPlayers = m_playerData.size();
        bbool allwaiting = btrue;

        for (u32 i = 0; i < numPlayers; i++)
        {
            ActorInfo& info = m_playerData[i];

            if (info.m_currentState != ACTORSTATE_WAIT)
            {
                allwaiting = bfalse;
                break;
            }
        }

        if (allwaiting)
        {
            m_timer += _dt;

            if (m_timer >= m_waitForPlayersTime)
            {
                m_state = SEQSTATE_TRAVEL;
                m_timer = 0.f;

                preparePlayersTrip();

                CAMERACONTROLLERMANAGER->startCameraSequence(m_start + m_cameraLookAtOffset, 0.05f,
                                                             abs(m_cameraLookAtOffset.m_z), btrue);
            }
        }
        else
        {
            m_timer = 0.f;
        }
    }

    void Ray_GameManagerSequenceStargate::updateTravel(f32 _dt)
    {
        updateFade(_dt);

        u32 numPlayers = m_playerData.size();
        bbool allwaiting = btrue;
        Vec3d cameraPos = Vec3d::Zero;
        u32 camCounter = 0;

        for (u32 i = 0; i < numPlayers; i++)
        {
            ActorInfo& info = m_playerData[i];

            Actor* playerActor = info.m_actor.getActor();

            if (playerActor)
            {
                camCounter++;
                cameraPos += playerActor->getPos();
            }

            if (allwaiting && info.m_currentState != ACTORSTATE_WAIT)
            {
                allwaiting = bfalse;
            }
        }

        if (camCounter)
        {
            cameraPos /= static_cast<f32>(camCounter);

            CAMERACONTROLLERMANAGER->setCameraSequencePosition(cameraPos + m_cameraLookAtOffset);
        }

        if (allwaiting)
        {
            if (m_lastTrip)
            {
                clearPrefetch();

                m_timer += _dt;

                if (m_timer >= m_waitForPlayersTime)
                {
                    m_isFinished = btrue;
                    m_timer = 0.f;

                    CAMERACONTROLLERMANAGER->stopCameraSequence(0.03f);

                    Ray_EventStargateUse stargateEvent;

                    stargateEvent.setStage(STARGATESTAGE_END);

                    for (u32 i = 0; i < numPlayers; i++)
                    {
                        ActorInfo& info = m_playerData[i];
                        Actor* actor = info.m_actor.getActor();

                        if (actor)
                        {
                            actor->onEvent(&stargateEvent);
                        }
                    }
                }
            }
            else
            {
                if (!updateTeleport())
                {
                    // It seems the teleport is not linked properly, so abort here
                    Color endColor = Color::black();
                    endColor.m_a = 0.f;

                    RAY_GAMEMANAGER->fxFade(Color::black(), endColor, m_fadeDuration, btrue);

                    m_lastTrip = btrue;
                }
            }
        }
        else
        {
            m_timer += _dt;
        }
    }

    void Ray_GameManagerSequenceStargate::updateFade(f32 _dt)
    {
        if (m_fadeTimer)
        {
            m_fadeTimer -= _dt;

            if (m_fadeTimer <= 0.f)
            {
                m_fadeTimer = 0.f;

                if (m_fadeToBlack)
                {
                    Color startColor = Color::black();
                    startColor.m_a = 0.f;

                    RAY_GAMEMANAGER->fxFade(startColor, Color::black(), m_fadeDuration, bfalse);
                }
                else if (m_fadeToGame)
                {
                    Color endColor = Color::black();
                    endColor.m_a = 0.f;

                    RAY_GAMEMANAGER->fxFade(Color::black(), endColor, m_fadeDuration, btrue);
                }

                m_fadeToBlack = bfalse;
                m_fadeToGame = bfalse;
            }
        }
    }

    bbool Ray_GameManagerSequenceStargate::updateTeleport()
    {
        if (waitPrefetch())
        {
            return btrue;
        }

        Actor* node = m_currentNode.getActor();

        if (node)
        {
            LinkComponent* linkComponent = node->GetComponent<LinkComponent>();

            if (linkComponent)
            {
                const LinkComponent::ChildrenList& children = linkComponent->getChildren();
                u32 numChildren = children.size();

                for (u32 i = 0; i < numChildren; i++)
                {
                    const ChildEntry& child = children[i];

                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if (path.getIsAbsolute())
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(node, path);
                    }

                    if (target)
                    {
                        Actor* targetActor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));

                        if (targetActor)
                        {
                            Ray_StargateNodeComponent* stargateNode = targetActor->GetComponent<
                                Ray_StargateNodeComponent>();

                            if (stargateNode)
                            {
                                m_fadeToGame = btrue;
                                m_fadeTimer = stargateNode->getFadeTime();
                                m_fadeDuration = stargateNode->getFadeLength();
                                m_currentNode = target->getRef();
                                preparePlayersTrip();
                                return btrue;
                            }
                        }
                    }
                }
            }
        }

        return bfalse;
    }

    void Ray_GameManagerSequenceStargate::clearPrefetch()
    {
        if (m_currentPrefetch != ITF_INVALID_OBJREF)
        {
            Actor* prefetchActor = m_currentPrefetch.getActor();

            if (prefetchActor)
            {
                AABBPrefetchComponent* prefetchComponent = prefetchActor->GetComponent<AABBPrefetchComponent>();

                if (prefetchComponent)
                {
                    prefetchComponent->forceStart(bfalse);
                }
            }

            m_currentPrefetch = ITF_INVALID_OBJREF;
        }
    }

    void Ray_GameManagerSequenceStargate::findCurrentPrefetch()
    {
        clearPrefetch();

        Actor* node = m_currentNode.getActor();

        if (node)
        {
            LinkComponent* linkComponent = node->GetComponent<LinkComponent>();

            if (linkComponent)
            {
                const LinkComponent::ChildrenList& children = linkComponent->getChildren();
                u32 numChildren = children.size();

                for (u32 i = 0; i < numChildren; i++)
                {
                    const ChildEntry& child = children[i];

                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if (path.getIsAbsolute())
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(node, path);
                    }

                    if (target)
                    {
                        Actor* targetActor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));

                        if (targetActor)
                        {
                            AABBPrefetchComponent* prefetchComponent = targetActor->GetComponent<
                                AABBPrefetchComponent>();

                            if (prefetchComponent)
                            {
                                m_currentPrefetch = targetActor->getRef();
                                prefetchComponent->forceStart(btrue);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    bbool Ray_GameManagerSequenceStargate::waitPrefetch() const
    {
        if (m_currentPrefetch != ITF_INVALID_OBJREF)
        {
            Actor* prefetchActor = m_currentNode.getActor();

            if (prefetchActor)
            {
                AABBPrefetchComponent* prefetchComponent = prefetchActor->GetComponent<AABBPrefetchComponent>();

                if (prefetchComponent)
                {
                    return prefetchComponent->prefetchComplete() == bfalse;
                }
            }
        }

        return bfalse;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    Ray_GameManagerSequenceScaleDoor::Ray_GameManagerSequenceScaleDoor()
        : m_moveK(100.f)
          , m_moveD(20.f)
    {
    }

    Ray_GameManagerSequenceScaleDoor::~Ray_GameManagerSequenceScaleDoor()
    {
    }

    void Ray_GameManagerSequenceScaleDoor::start(ActorRef _door, ActorRef _targetActor, f32 _scale, bbool _allPlayers,
                                                 const Vec2d& _targetPos, f32 _waitDelay,
                                                 f32 _moveK, f32 _moveD)
    {
        if (m_doors.size() > 0 && m_doors[0].m_allPlayers)
        {
            // If we are playing a sequence that scales all players:
            // * If it's another door, ignore it
            // * If it's the same door, make the player go inside
            DoorSeq& playingSeq = m_doors[0];

            if (playingSeq.m_door != _door)
            {
                return;
            }

            setPlayerGoIn(playingSeq, _targetActor);
            return;
        }

        if (_allPlayers)
        {
            // Everyone goes inside
            m_doors.clear();
            m_doors.push_back(DoorSeq());

            DoorSeq& newSeq = m_doors[0];

            newSeq.m_state = SEQSTATE_WAIT_ALLIN;
            newSeq.m_allPlayers = _allPlayers;
            newSeq.m_door = _door;
            newSeq.m_timeCounter = _waitDelay;
            newSeq.m_endPoint = _targetPos;
            newSeq.m_scale = _scale;
            newSeq.m_playerData.clear();

            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

                if (player->getActive() && !player->getIsLeaving())
                {
                    Actor* playerActor = player->getActor();

                    if (playerActor)
                    {
                        PlayerData playerData;

                        playerData.m_actor = playerActor->getRef();
                        playerData.m_state = PLAYERSTATE_START_DELAY;

                        newSeq.m_playerData.push_back(playerData);
                    }
                }
            }

            setPlayerGoIn(newSeq, _targetActor);
        }
        else
        {
            // Individual sequence
            m_doors.push_back(DoorSeq());

            DoorSeq& newSeq = m_doors[m_doors.size() - 1];

            newSeq.m_state = SEQSTATE_WAIT_ALLIN;
            newSeq.m_allPlayers = _allPlayers;
            newSeq.m_door = _door;
            newSeq.m_playerData.clear();
            newSeq.m_timeCounter = 0.f;
            newSeq.m_endPoint = _targetPos;
            newSeq.m_scale = _scale;

            setPlayerGoIn(newSeq, _targetActor);
        }

        m_moveK = _moveK;
        m_moveD = _moveD;
    }

    void Ray_GameManagerSequenceScaleDoor::stop()
    {
        EventStopPlayer stopPlayer(bfalse);
        EventTrail showTrail(bfalse);
        EventShow alphaChange(1.0f, 0.f);

        for (ITF_VECTOR<DoorSeq>::iterator it = m_doors.begin(); it != m_doors.end(); ++it)
        {
            DoorSeq& doorSeq = *it;
            u32 numPlayers = doorSeq.m_playerData.size();

            for (u32 i = 0; i < numPlayers; i++)
            {
                PlayerData& playerData = doorSeq.m_playerData[i];

                if (playerData.m_state != PLAYERSTATE_START_DELAY)
                {
                    Actor* playerActor = playerData.m_actor.getActor();

                    if (playerActor)
                    {
                        playerActor->onEvent(&stopPlayer);
                        playerActor->onEvent(&showTrail);
                        playerActor->onEvent(&alphaChange);
                    }
                }
            }
        }

        m_doors.clear();
    }

    void Ray_GameManagerSequenceScaleDoor::setPlayerGoIn(DoorSeq& _seq, ActorRef _player)
    {
        PlayerData* playerData = NULL;
        u32 numPlayers = _seq.m_playerData.size();

        for (u32 i = 0; i < numPlayers; i++)
        {
            if (_seq.m_playerData[i].m_actor == _player)
            {
                playerData = &_seq.m_playerData[i];
                break;
            }
        }

        if (!playerData)
        {
            PlayerData newData;

            newData.m_actor = _player;

            _seq.m_playerData.push_back(newData);

            playerData = &_seq.m_playerData[_seq.m_playerData.size() - 1];
        }

        playerData->m_state = PLAYERSTATE_GOING_IN;

        Actor* playerActor = _player.getActor();

        if (playerActor)
        {
            EventTrail showTrail(btrue);
            EventShow alphaChange(0.0f, 0.25f);
            EventStopPlayer stopPlayer(btrue);

            playerActor->onEvent(&alphaChange);
            playerActor->onEvent(&showTrail);
            playerActor->onEvent(&stopPlayer);
        }
    }

    void Ray_GameManagerSequenceScaleDoor::performScaleAndExit(DoorSeq& _seq)
    {
        u32 numPlayers = _seq.m_playerData.size();
        EventShow alphaChange(1.0f, 0.2f);

        for (u32 i = 0; i < numPlayers; i++)
        {
            PlayerData& playerData = _seq.m_playerData[i];

            Actor* playerActor = playerData.m_actor.getActor();

            if (playerActor)
            {
                Vec2d originalScale = playerActor->getWorldInitialScale();
                playerActor->setScale(originalScale * _seq.m_scale);
                playerActor->onEvent(&alphaChange);
            }

            playerData.m_state = PLAYERSTATE_GOING_OUT;
        }

        _seq.m_state = SEQSTATE_WAIT_ALLOUT;
    }

    void Ray_GameManagerSequenceScaleDoor::update(f32 _dt)
    {
        ITF_VECTOR<DoorSeq>::iterator it = m_doors.begin();

        while (it != m_doors.end())
        {
            DoorSeq& seq = *it;

            updateDoorSeq(seq, _dt);

            if (seq.m_state == SEQSTATE_DONE)
            {
                it = m_doors.erase(it);
            }
            else
            {
                ++it;
            }
        }

        m_isFinished = m_doors.size() == 0;
    }

    void Ray_GameManagerSequenceScaleDoor::updateDoorSeq(DoorSeq& _seq, f32 _dt)
    {
        Actor* doorActor = _seq.m_door.getActor();

        if (doorActor)
        {
            u32 numPlayers = _seq.m_playerData.size();
            bbool allWait = btrue;

            _seq.m_timeCounter = Max(_seq.m_timeCounter - _dt, 0.f);

            for (u32 i = 0; i < numPlayers; i++)
            {
                PlayerData& playerData = _seq.m_playerData[i];
                Actor* playerActor = playerData.m_actor.getActor();

                if (playerActor)
                {
                    switch (playerData.m_state)
                    {
                    case PLAYERSTATE_START_DELAY:
                        {
                            if (_seq.m_timeCounter == 0.f)
                            {
                                setPlayerGoIn(_seq, playerData.m_actor);
                            }
                            allWait = bfalse;
                        }
                        break;
                    case PLAYERSTATE_GOING_IN:
                        {
                            PhysComponent* phys = playerActor->GetComponent<PhysComponent>();
                            Vec2d currentSpeed = phys->getSpeed();
                            Vec2d vecToPoint = doorActor->get2DPos() - playerActor->get2DPos();
                            Vec2d force = (vecToPoint * m_moveK) - (currentSpeed * m_moveD);
                            Vec2d speed = currentSpeed + (force * _dt);
                            Vec2d newPos = playerActor->get2DPos() + (speed * _dt);

                            EventTeleport evtTeleport(newPos.to3d(playerActor->getPos().m_z), 0.0f, bfalse);
                            playerActor->onEvent(&evtTeleport);

                            phys->setSpeed(speed);

                            f32 distToTarget = (newPos - doorActor->get2DPos()).sqrnorm();

                            if (distToTarget < 0.1f)
                            {
                                playerData.m_state = PLAYERSTATE_WAIT;
                            }
                            else
                            {
                                allWait = bfalse;
                            }
                        }
                        break;
                    case PLAYERSTATE_WAIT:
                        {
                        }
                        break;
                    case PLAYERSTATE_GOING_OUT:
                        {
                            PhysComponent* phys = playerActor->GetComponent<PhysComponent>();
                            Vec2d currentSpeed = phys->getSpeed();
                            Vec2d vecToPoint = _seq.m_endPoint - playerActor->get2DPos();
                            Vec2d force = (vecToPoint * m_moveK) - (currentSpeed * m_moveD);
                            Vec2d speed = currentSpeed + (force * _dt);
                            Vec2d newPos = playerActor->get2DPos() + (speed * _dt);

                            EventTeleport evtTeleport(newPos.to3d(playerActor->getPos().m_z), 0.0f, bfalse);
                            playerActor->onEvent(&evtTeleport);

                            phys->setSpeed(speed);

                            f32 distToTarget = (newPos - _seq.m_endPoint).sqrnorm();

                            if (distToTarget < 0.1f)
                            {
                                playerData.m_state = PLAYERSTATE_WAIT;

                                EventTrail showTrail(bfalse);
                                playerActor->onEvent(&showTrail);
                            }
                            else
                            {
                                allWait = bfalse;
                            }
                        }
                        break;
                    }
                }
            }

            switch (_seq.m_state)
            {
            case SEQSTATE_WAIT_ALLIN:
                {
                    if (allWait)
                    {
                        performScaleAndExit(_seq);
                    }
                }
                break;
            case SEQSTATE_WAIT_ALLOUT:
                {
                    if (allWait)
                    {
                        _seq.m_state = SEQSTATE_DONE;

                        for (u32 i = 0; i < numPlayers; i++)
                        {
                            PlayerData& playerData = _seq.m_playerData[i];
                            Actor* playerActor = playerData.m_actor.getActor();

                            if (playerActor)
                            {
                                EventStopPlayer stopPlayer(bfalse);
                                playerActor->onEvent(&stopPlayer);
                            }
                        }
                    }
                }
                break;
            case SEQSTATE_DONE:
                {
                }
                break;
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    Ray_GameManagerSequenceBounceToLayer::Ray_GameManagerSequenceBounceToLayer()
        : m_playerTrigger(ITF_INVALID_OBJREF)
          , m_target(ITF_INVALID_OBJREF)
          , m_offset(Vec2d::Zero)
          , m_targetOffsetDist(0.f)
          , m_totalTime(0.f)
          , m_timeCounter(0.f)
          , m_waitAngleOffset(0.1f)
    {
    }

    Ray_GameManagerSequenceBounceToLayer::~Ray_GameManagerSequenceBounceToLayer()
    {
    }

    void Ray_GameManagerSequenceBounceToLayer::start(Actor* _playerTrigger, ObjectRef _target, const Vec2d& _offset,
                                                     f32 _targetOffsetDist, f32 _totalTime, f32 _waitAngleOffset)
    {
        m_playerTrigger = _playerTrigger->getRef();
        m_target = _target;
        m_offset = _offset;
        m_targetOffsetDist = _targetOffsetDist;
        m_totalTime = _totalTime;
        m_timeCounter = 0.f;
        m_isFinished = bfalse;
        m_waitAngleOffset = _waitAngleOffset;
        m_playerInfo.clear();

        BaseObject* obj = _target.getObject();

        if (obj)
        {
            Pickable* pickable = obj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable, 594660361));

            if (pickable)
            {
                GAMEMANAGER->setCurrentSceneReference(pickable->getScene());
            }
        }

        u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();
        EventStopPlayer stopPlayerEvent(btrue);
        EventTrail showTrail(btrue);
        EventShow alphaChange(0.0f, 0.25f);
        EventDetach detachEvent;

        for (u32 i = 0; i < numPlayers; i++)
        {
            Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!otherPlayer->getActive() || otherPlayer->getIsLeaving())
            {
                continue;
            }

            Actor* playerActor = otherPlayer->getActor();

            if (!playerActor || playerActor == _playerTrigger)
            {
                continue;
            }

            playerActor->onEvent(&detachEvent);
            playerActor->onEvent(&stopPlayerEvent);
            playerActor->onEvent(&showTrail);
            playerActor->onEvent(&alphaChange);

            PlayerInfo pInfo;

            m_playerInfo.push_back(pInfo);

            u32 playerIndex = m_playerInfo.size() - 1;

            setupPlayer(m_playerInfo[playerIndex], playerIndex, playerActor, _playerTrigger);
        }
    }

    void Ray_GameManagerSequenceBounceToLayer::setupPlayer(PlayerInfo& _info, u32 _playerIndex, Actor* _playerActor,
                                                           Actor* _playerTrigger)
    {
        _info.m_ref = _playerActor->getRef();
        _info.m_spline.clear();

        Vec3d targetP = getTargetPoint(_playerIndex, _playerActor);

        _info.m_spline.AddSplinePoint(_playerActor->getPos(), Spline::interp_spline, 0.0f);
        _info.m_spline.AddSplinePoint(_playerTrigger->getPos(), Spline::interp_spline, m_totalTime * 0.25f);
        _info.m_spline.AddSplinePoint(targetP, Spline::interp_spline, m_totalTime);
        _info.m_prevPos = _playerActor->getPos();
    }

    Vec3d Ray_GameManagerSequenceBounceToLayer::getTargetPoint(u32 _playerIndex, Actor* _playerActor) const
    {
        Pickable* target = static_cast<Pickable*>(GETOBJECT(m_target));
        Vec3d retPos = Vec3d::Zero;

        if (target)
        {
            Vec2d targetPos = target->get2DPos() + m_offset;
            f32 targetZ = AIUtils::getSceneZFromPickable(target);
            retPos = Vec3d(targetPos.m_x, targetPos.m_y, targetZ);
        }

        f32 angle;

        if ((m_playerInfo.size() % 2) == 0)
        {
            if ((_playerIndex % 2) == 0)
            {
                angle = -(m_waitAngleOffset * static_cast<f32>(_playerIndex + 1));
            }
            else
            {
                angle = (m_waitAngleOffset * static_cast<f32>(_playerIndex + 1));
            }
        }
        else
        {
            if (_playerIndex == 0)
            {
                angle = 0.f;
            }
            else if ((_playerIndex % 2) == 0)
            {
                angle = -(m_waitAngleOffset * static_cast<f32>(_playerIndex));
            }
            else
            {
                angle = (m_waitAngleOffset * static_cast<f32>(_playerIndex));
            }
        }

        Vec2d offsetDir = Vec2d::Up.Rotate(angle);

        retPos = retPos + (offsetDir * m_targetOffsetDist).to3d();

        if (_playerActor->getTemplate()->isZForced())
        {
            retPos.m_z += _playerActor->getTemplate()->getForcedZ();
        }

        return retPos;
    }

    void Ray_GameManagerSequenceBounceToLayer::stop()
    {
        restorePlayers();
    }

    void Ray_GameManagerSequenceBounceToLayer::restorePlayers()
    {
        u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();
        EventStopPlayer stopPlayerEvent(bfalse);
        EventTrail showTrail(bfalse);
        EventShow alphaChange(1.0f, 0.f);

        for (u32 i = 0; i < numPlayers; i++)
        {
            Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!otherPlayer->getActive() || otherPlayer->getIsLeaving())
            {
                continue;
            }

            Actor* playerActor = otherPlayer->getActor();

            if (!playerActor || playerActor->getRef() == m_playerTrigger)
            {
                continue;
            }

            playerActor->onEvent(&stopPlayerEvent);
            playerActor->onEvent(&showTrail);
            playerActor->onEvent(&alphaChange);
        }
    }

    void Ray_GameManagerSequenceBounceToLayer::update(f32 _dt)
    {
        m_timeCounter = Min(m_timeCounter + _dt, m_totalTime);

        u32 numPlayers = m_playerInfo.size();

        for (u32 i = 0; i < numPlayers; i++)
        {
            PlayerInfo& info = m_playerInfo[i];
            Actor* playerActor = info.m_ref.getActor();

            if (!playerActor)
            {
                continue;
            }

            Vec3d target = getTargetPoint(i, playerActor);

            info.m_spline.setSplinePointAt(target, Spline::interp_spline, m_totalTime,
                                           info.m_spline.GetNumPoints() - 1);

            Vec3d newP;

            info.m_spline.GetInterpolatedAtTime(m_timeCounter, newP);

            playerActor->setPos(newP);
        }

        if (m_timeCounter == m_totalTime)
        {
            restorePlayers();
            m_isFinished = btrue;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------------------------------------
    Ray_GameManagerSequenceStartTimeAttack::Ray_GameManagerSequenceStartTimeAttack()
        : m_playerTrigger(ITF_INVALID_OBJREF)
          , m_totalTime(0.f)
          , m_timeCounter(0.f)
          , m_destination(Vec3d::Zero)
    {
    }

    Ray_GameManagerSequenceStartTimeAttack::~Ray_GameManagerSequenceStartTimeAttack()
    {
    }

    void Ray_GameManagerSequenceStartTimeAttack::start(f32 _totalTime)
    {
        m_totalTime = _totalTime;
        m_timeCounter = 0.f;
        m_isFinished = bfalse;
        m_destination = Vec3d::Zero;
        m_playerInfo.clear();

        ObjectRef currentWorld = GAMEMANAGER->getCurrentWorld();
        Actor* checkpoint = GAMEMANAGER->getFirstCheckpoint(currentWorld);

        if (checkpoint)
        {
            GAMEMANAGER->setCurrentSceneReference(checkpoint->getScene());

            CheckpointComponent* comp = checkpoint->GetComponent<CheckpointComponent>();

            if (comp)
            {
                f32 sceneZ = AIUtils::getSceneZFromPickable(checkpoint);
                m_destination = comp->getPlayerSpawnPos(RAY_GAMEMANAGER->getMainIndexPlayer()).to3d(sceneZ);
            }
        }

        u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();
        Ray_EventTriggerStartTimeAttack startSeq(btrue);

        for (u32 i = 0; i < numPlayers; i++)
        {
            Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!otherPlayer->getActive() || otherPlayer->getIsLeaving())
            {
                continue;
            }

            Actor* playerActor = otherPlayer->getActor();

            if (playerActor)
            {
                playerActor->onEvent(&startSeq);

                PlayerInfo pInfo;

                m_playerInfo.push_back(pInfo);

                u32 playerIndex = m_playerInfo.size() - 1;

                setupPlayer(m_playerInfo[playerIndex], playerIndex, playerActor);
            }
        }
    }

    void Ray_GameManagerSequenceStartTimeAttack::setupPlayer(PlayerInfo& _info, u32 _playerIndex, Actor* _playerActor)
    {
        _info.m_ref = _playerActor->getRef();
        _info.m_spline.clear();

        EventQueryPhysicsData queryEvent;

        _playerActor->onEvent(&queryEvent);

        _info.m_spline.AddSplinePoint(_playerActor->getPos(), Spline::interp_spline, 0.0f);

        if (queryEvent.getSpeed().sqrnorm() > 1)
        {
            _info.m_spline.AddSplinePoint(_playerActor->getPos() + queryEvent.getSpeed().to3d(0.f),
                                          Spline::interp_spline, m_totalTime * 0.25f);
        }

        _info.m_spline.AddSplinePoint(m_destination + Vec3d(0.f, 0.f, _playerActor->getTemplate()->getForcedZ()),
                                      Spline::interp_spline, m_totalTime);
        _info.m_prevPos = _playerActor->getPos();
    }

    void Ray_GameManagerSequenceStartTimeAttack::stop()
    {
        restorePlayers();
    }

    void Ray_GameManagerSequenceStartTimeAttack::restorePlayers()
    {
        u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();
        Ray_EventTriggerStartTimeAttack stopSeq(bfalse);

        for (u32 i = 0; i < numPlayers; i++)
        {
            Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!otherPlayer->getActive() || otherPlayer->getIsLeaving())
            {
                continue;
            }

            Actor* playerActor = otherPlayer->getActor();

            if (!playerActor)
            {
                continue;
            }

            playerActor->onEvent(&stopSeq);
        }
    }

    void Ray_GameManagerSequenceStartTimeAttack::update(f32 _dt)
    {
        m_timeCounter = Min(m_timeCounter + _dt, m_totalTime);

        u32 numPlayers = m_playerInfo.size();

        for (u32 i = 0; i < numPlayers; i++)
        {
            PlayerInfo& info = m_playerInfo[i];
            Actor* playerActor = info.m_ref.getActor();

            if (!playerActor)
            {
                continue;
            }

            Vec3d newP;

            info.m_spline.GetInterpolatedAtTime(m_timeCounter, newP);

            playerActor->setPos(newP);
        }

        if (m_timeCounter == m_totalTime)
        {
            restorePlayers();
            m_isFinished = btrue;
        }
    }

    Actor* getTargetCheckpoint(GameManager::teleportTarget _target)
    {
        Actor* actor = NULL;
        const ObjectRef& worldRef = GAMEMANAGER->getCurrentWorld();

        switch (_target)
        {
        case GameManager::NextCheckPoint:
            {
                const Actor* refCheckpoint = GAMEMANAGER->getCurrentCheckpoint();
                if (!refCheckpoint)
                    refCheckpoint = GAMEMANAGER->getFirstCheckpoint(worldRef);

                if (refCheckpoint)
                {
                    const CheckpointComponent* comp = refCheckpoint->GetComponent<const CheckpointComponent>();

                    if (comp)
                    {
                        u32 nextIndex = comp->getIndex();
                        nextIndex += 1;

                        actor = GAMEMANAGER->getCheckpointFromIndex(worldRef, nextIndex, btrue);
                    }
                }
            }
            break;

        case GameManager::PrevCheckPoint:
            {
                const Actor* refCheckpoint = GAMEMANAGER->getCurrentCheckpoint();
                if (!refCheckpoint)
                    refCheckpoint = GAMEMANAGER->getFirstCheckpoint(worldRef);

                if (refCheckpoint)
                {
                    const CheckpointComponent* comp = refCheckpoint->GetComponent<const CheckpointComponent>();

                    if (comp)
                    {
                        u32 nextIndex = comp->getIndex();
                        nextIndex -= 1;

                        actor = GAMEMANAGER->getCheckpointFromIndex(worldRef, nextIndex, bfalse);
                    }
                }
            }
            break;

        case GameManager::FirstCheckPoint:
            actor = GAMEMANAGER->getFirstCheckpoint(worldRef);
            break;

        case GameManager::LastCheckPoint:
            actor = GAMEMANAGER->getCurrentCheckpoint();
            break;

        default:
            break;
        }

        return actor;
    }


    Ray_GameManagerSequenceTeleportCheat::Ray_GameManagerSequenceTeleportCheat()
        : m_prefetchView(NULL)
          , m_state(State_Idle)
          , m_target(GameManager::None)
    {
    }

    void Ray_GameManagerSequenceTeleportCheat::start(GameManager::teleportTarget _target)
    {
        ITF_ASSERT(!m_prefetchView);
        m_target = _target;
        m_isFinished = bfalse;
        m_state = State_FadeIn;

        // Direct teleport NO PREFETCH
        /*
        switch(_target)
        {
        case GameManager::NextCheckPoint:
            GAMEMANAGER->teleportToNextCheckpoint(btrue);
            break;

        case GameManager::PrevCheckPoint:
            GAMEMANAGER->teleportToNextCheckpoint(bfalse);
            break;

        case GameManager::FirstCheckPoint:
            GAMEMANAGER->teleportToFirstCheckpoint(bfalse);
            break;

        case GameManager::LastCheckPoint:
            GAMEMANAGER->resetToLastCheckpoint();
            break;

        default:
            ITF_ASSERT(0);
            break;
        }

        m_isFinished = btrue;
        */
    }

    void Ray_GameManagerSequenceTeleportCheat::update(f32 _dt)
    {
        switch (m_state)
        {
        case State_FadeIn:
            {
                RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeTeleport());

                if (!m_prefetchView)
                    m_prefetchView = View::createView("Ray_GameManagerSequenceTeleportCheat", bfalse, btrue);

                if (Actor* pCheckpoint = getTargetCheckpoint(m_target))
                {
                    const f32 deltaX = CAMERA->getDeltaX();
                    const f32 deltaY = CAMERA->getDeltaY();
                    AABB targetAABB = pCheckpoint->getAABB();
                    targetAABB.projectTo2D(deltaX, deltaY, pCheckpoint->getDepth());
                    m_prefetchView->setTargetAABB(targetAABB);
                    m_prefetchView->setActive(btrue);
                    World::disableLoadResourcesDelay();
                }

                m_state = State_WaitFadeAndPrefetchEnd;
            }
            break;

        case State_WaitFadeAndPrefetchEnd:
            {
                const bbool fadeOver = RAY_GAMEMANAGER->isFadeOutFinished();
                bbool prefetchReady = bfalse;
                World* pWorld = CURRENTWORLD;
                if (m_prefetchView && pWorld)
                {
                    prefetchReady = pWorld->isPhysicalReady(*m_prefetchView);
                }
                else
                {
                    prefetchReady = btrue; // prevent infinite black screen
                }

                if (fadeOver && prefetchReady)
                {
                    switch (m_target)
                    {
                    case GameManager::NextCheckPoint:
                        GAMEMANAGER->teleportToNextCheckpoint(btrue);
                        break;

                    case GameManager::PrevCheckPoint:
                        GAMEMANAGER->teleportToNextCheckpoint(bfalse);
                        break;

                    case GameManager::FirstCheckPoint:
                        GAMEMANAGER->teleportToFirstCheckpoint(bfalse);
                        break;

                    case GameManager::LastCheckPoint:
                        GAMEMANAGER->resetToLastCheckpoint();
                        break;

                    default:
                        ITF_ASSERT(0);
                        break;
                    }

                    m_state = State_FadeOut;
                }
            }
            break;

        case State_FadeOut:
            {
                RAY_GAMEMANAGER->stopFade(bfalse);
                m_state = State_WaitFadeOutEnd;
            }
            break;

        case State_WaitFadeOutEnd:
            {
                if (RAY_GAMEMANAGER->isFadeInFinished())
                {
                    stop();
                }
            }
            break;

        default:
            ITF_ASSERT(0);
            break;
        }
    }

    void Ray_GameManagerSequenceTeleportCheat::stop()
    {
        if (m_prefetchView)
        {
            m_prefetchView->setActive(bfalse);
            World::enableLoadResourcesDelay();
            View::deleteView(m_prefetchView);
            m_prefetchView = NULL;
        }

        RAY_GAMEMANAGER->stopFade(btrue);
        m_isFinished = btrue;
        m_state = State_Idle;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Ray_GameManagerSequenceChangeRoom::Ray_GameManagerSequenceChangeRoom()
        : m_doorRef(ITF_INVALID_OBJREF)
          , m_currentState(STATE_FADE_BLACK)
          , m_prefetchView(NULL)
          , m_prefetchFailSafe(0.f)
          , m_goIn(bfalse)
    {
    }

    Ray_GameManagerSequenceChangeRoom::~Ray_GameManagerSequenceChangeRoom()
    {
        freePrefetch();
    }

    void Ray_GameManagerSequenceChangeRoom::start(bbool _goIn, ObjectRef _door)
    {
        m_doorRef = _door;
        m_goIn = _goIn;
        m_isFinished = bfalse;
        m_prefetchFailSafe = 0.f;
        m_currentState = STATE_FADE_BLACK;

        RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeCostume());
        // stop playing music
        MUSICMANAGER->stop(METRONOME_TYPE_GAMEPLAY, 1.0f);

        // Hide WM info
        if (_goIn)
        {
            if (!RAY_GAMEMANAGER->getWMCurrentLevelTag().isValid())
            {
                // Hide world recap
                Ray_EventWMDisplayWorldRecap evDisplayRecap;
                evDisplayRecap.setDisplay(bfalse);
                evDisplayRecap.setWorldTag(RAY_GAMEMANAGER->getWMCurrentWorldTag());
                EVENTMANAGER->broadcastEvent(&evDisplayRecap);
            }

            // Don't allow new notification
            Ray_EventWMShowNewNotification evshowNew;
            evshowNew.setShow(bfalse);
            EVENTMANAGER->broadcastEvent(&evshowNew);
        }

        Actor* targetActor;

        if (_goIn)
        {
            targetActor = AIUtils::getActor(m_doorRef);
        }
        else
        {
            // Set last known value of next costume price (in order to highlight homemap when player unlock
            // a new costume
            Actor* actor = AIUtils::getActor(RAY_GAMEMANAGER->getCostumeManager());
            if (actor)
            {
                Ray_PlayerCostumeManagerComponent* manager = actor->GetComponent<Ray_PlayerCostumeManagerComponent>();

                if (manager)
                {
                    u32 highestPrice = 0;
                    if (manager->getNextCostumeAvailable(highestPrice) == ITF_INVALID_OBJREF)
                        RAY_GAMEMANAGER->getPersistentGameData()->setCostumeLastPrice(U32_INVALID);
                    else
                        RAY_GAMEMANAGER->getPersistentGameData()->setCostumeLastPrice(highestPrice);
                    RAY_GAMEMANAGER->setWMSpotHasWarning(ITF_GET_STRINGID_CRC(HomeMap, 1612706167), bfalse);
                }
            }

            // let's find the current world map spot
            targetActor = NULL;
            const ObjectPath& world = RAY_GAMEMANAGER->getWMCurrentWorldObjectPath();
            const ObjectPath& level = RAY_GAMEMANAGER->getWMCurrentLevelObjectPath();
            if (level.isValid())
            {
                Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(level);
                targetActor = pickable ? pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115)) : NULL;
            }
            else
            {
                Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(world);
                targetActor = pickable ? pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115)) : NULL;
            }
            ITF_ASSERT_MSG(targetActor, "Can't find current world map spot");
        }

        if (targetActor)
        {
            if (!m_prefetchView)
            {
                m_prefetchView = View::createView("Ray_GameManagerSequenceChangeRoom", bfalse, btrue);
            }

            const f32 deltaX = CAMERA->getDeltaX();
            const f32 deltaY = CAMERA->getDeltaY();
            Vec3d checkpointPos = targetActor->getPos();
            AABB targetAABB(checkpointPos);

            targetAABB.grow(checkpointPos + Vec3d(-10.f, -10.f, 0.f));
            targetAABB.grow(checkpointPos + Vec3d(10.f, 10.f, 0.f));
            targetAABB.projectTo2D(deltaX, deltaY, targetActor->getDepth());

            m_prefetchView->setTargetAABB(targetAABB);
            m_prefetchView->setActive(btrue);
            World::disableLoadResourcesDelay();
        }

        u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();
        EventStopPlayer stopPlayer(btrue);

        for (u32 i = 0; i < numPlayers; i++)
        {
            Ray_Player* p = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!p || !p->getActive() || p->getIsLeaving())
            {
                continue;
            }

            Actor* currentActor = p->getCurrentActor().getActor();

            if (currentActor)
            {
                currentActor->onEvent(&stopPlayer);
            }
        }
    }

    void Ray_GameManagerSequenceChangeRoom::stop()
    {
        freePrefetch();
        m_prefetchFailSafe = 0.f;

        EventStopPlayer resumePlayer(bfalse);
        u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();

        RAY_GAMEMANAGER->stopFade(btrue);
        RAY_GAMEMANAGER->hideLoadNotificationActor();

        for (u32 i = 0; i < numPlayers; i++)
        {
            Ray_Player* p = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!p)
            {
                continue;
            }

            RAY_GAMEMANAGER->changePlayerGameMode(p, m_goIn ? RAY_GAMEMODE_PLATFORMER : RAY_GAMEMODE_WORLDMAP, btrue);

            if (p->getActive() && !p->getIsLeaving())
            {
                Actor* currentActor = p->getCurrentActor().getActor();

                if (currentActor)
                {
                    currentActor->onEvent(&resumePlayer);
                }
            }
        }
    }

    void Ray_GameManagerSequenceChangeRoom::update(f32 _dt)
    {
        switch (m_currentState)
        {
        case STATE_FADE_BLACK:
            updateFadeBlack(_dt);
            break;
        case STATE_FADE_GAME:
            updateFadeToGame(_dt);
            break;
        }
    }

    void Ray_GameManagerSequenceChangeRoom::updateFadeBlack(f32 _dt)
    {
        bbool nextState = bfalse;

        if (RAY_GAMEMANAGER->isFadeOutFinished())
        {
            if (m_prefetchView)
            {
                Actor* checkpoint = AIUtils::getActor(m_doorRef);

                if (checkpoint)
                {
                    ITF_ASSERT(m_prefetchFailSafe < MAX_PREFETCH_TIMEOUT);
                    if (checkpoint->getWorld()->isPhysicalReady(*m_prefetchView) || m_prefetchFailSafe >=
                        MAX_PREFETCH_TIMEOUT)
                    {
                        nextState = btrue;
                    }
                    else
                    {
                        m_prefetchFailSafe += _dt;

                        if (m_prefetchFailSafe >= MAX_FADETIME_FORICON)
                        {
                            RAY_GAMEMANAGER->showLoadNotificationActor();
                        }
                    }
                }
                else
                {
                    nextState = btrue;
                }
            }
            else
            {
                nextState = btrue;
            }
        }

        if (nextState)
        {
            m_currentState = STATE_FADE_GAME;

            RAY_GAMEMANAGER->hideLoadNotificationActor();
            RAY_GAMEMANAGER->stopFade(bfalse);

            // Display / Hide paper screen effect of the worldmap
            Ray_EventWMShow evShow;
            evShow.setShow(!m_goIn);
            EVENTMANAGER->broadcastEvent(&evShow);

            u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();
            Actor* door = AIUtils::getActor(m_doorRef);
            f32 sceneZ;
            Vec2d pos;

            if (door)
            {
                sceneZ = AIUtils::getSceneZFromPickable(door);
                pos = door->get2DPos();
                GAMEMANAGER->setCurrentSceneReference(door->getScene());
            }
            else
            {
                sceneZ = 0.f;
                pos = Vec2d::Zero;
            }

            Vec3d pos3D = pos.to3d(sceneZ);
            Vec3d camTeleport = pos3D;

            for (u32 i = 0; i < numPlayers; i++)
            {
                Ray_Player* p = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

                if (!p || p->getIsLeaving())
                {
                    continue;
                }

                if (p->getActive())
                {
                    EventStopPlayer resumePlayer(bfalse);

                    Actor* currentActor = p->getCurrentActor().getActor();

                    if (currentActor)
                    {
                        currentActor->onEvent(&resumePlayer);
                    }
                }

                RAY_GAMEMANAGER->changePlayerGameMode(p, m_goIn ? RAY_GAMEMODE_PLATFORMER : RAY_GAMEMODE_WORLDMAP,
                                                      btrue);

                if (p->getActive())
                {
                    Actor* currentActor = p->getCurrentActor().getActor();

                    if (currentActor)
                    {
                        if (door && m_goIn)
                        {
                            if (currentActor)
                            {
                                Vec3d targetPos = pos3D;

                                if (currentActor->getTemplate()->isZForced())
                                {
                                    targetPos.m_z += currentActor->getTemplate()->getForcedZ();
                                }

                                EventTeleport teleport(targetPos, door->getAngle(), btrue);
                                currentActor->onEvent(&teleport);
                            }

                            CAMERACONTROLLERMANAGER->registerSubject(currentActor->getRef());
                        }
                        else
                        {
                            camTeleport = currentActor->getPos();
                        }
                    }
                }
            }

            if (door && m_goIn)
            {
                Ray_ChangeRoomDoorComponent* doorComponent = door->GetComponent<Ray_ChangeRoomDoorComponent>();

                if (doorComponent)
                {
                    doorComponent->setJustUsed();
                }
            }

            CAMERACONTROLLERMANAGER->teleport(camTeleport);
        }
    }

    void Ray_GameManagerSequenceChangeRoom::freePrefetch()
    {
        if (m_prefetchView)
        {
            m_prefetchView->setActive(bfalse);
            World::enableLoadResourcesDelay();
            View::deleteView(m_prefetchView);
            m_prefetchView = NULL;
        }
    }

    void Ray_GameManagerSequenceChangeRoom::updateFadeToGame(f32 _dt)
    {
        if (RAY_GAMEMANAGER->isFadeInFinished())
        {
            // Hide WM info
            if (!m_goIn)
            {
                if (!RAY_GAMEMANAGER->getWMCurrentLevelTag().isValid())
                {
                    // Hide world recap
                    Ray_EventWMDisplayWorldRecap evDisplayRecap;
                    evDisplayRecap.setDisplay(btrue);
                    evDisplayRecap.setWorldTag(RAY_GAMEMANAGER->getWMCurrentWorldTag());
                    EVENTMANAGER->broadcastEvent(&evDisplayRecap);
                }

                // Don't allow new notification
                Ray_EventWMShowNewNotification evshowNew;
                evshowNew.setShow(btrue);
                EVENTMANAGER->broadcastEvent(&evshowNew);
            }

            m_isFinished = btrue;

            freePrefetch();
        }
    }

    Ray_GameManagerSequenceReturnTeeth::Ray_GameManagerSequenceReturnTeeth()
        : m_deadMan(ITF_INVALID_OBJREF)
          , m_playerTrigger(U32_INVALID)
          , m_teethReturning(0)
          , m_cameraOffsetSurprise(Vec3d::Zero)
          , m_cameraOffsetThrow(Vec3d::Zero)
          , m_playerOffset(Vec2d::Zero)
          , m_throwing(bfalse)
    {
    }

    Ray_GameManagerSequenceReturnTeeth::~Ray_GameManagerSequenceReturnTeeth()
    {
    }

    void Ray_GameManagerSequenceReturnTeeth::start(Player* _triggerer, Actor* _deadMan,
                                                   const Vec3d& _cameraOffsetSurprise, const Vec3d& _cameraOffsetThrow,
                                                   const Vec2d& _playerOffset)
    {
        m_playerTrigger = _triggerer->getIndex();
        m_deadMan = _deadMan->getRef();
        m_isFinished = bfalse;
        m_throwing = bfalse;
        m_cameraOffsetSurprise = _cameraOffsetSurprise;
        m_cameraOffsetThrow = _cameraOffsetThrow;
        m_playerOffset = _playerOffset;

        m_teethReturning = RAY_GAMEMANAGER->getTotalTakenToothCount() - RAY_GAMEMANAGER->getReturnedTeethCount();

#ifdef ITF_SUPPORT_CHEAT
        if (RAY_CHEATMANAGER->getIsCheatTeethSequence() && m_teethReturning == 0)
        {
            m_teethReturning = Seeder::getSharedSeeder().GetU32(1, 10);
            RAY_CHEATMANAGER->setCheatTeethSequence(bfalse);
        }
#endif

        Ray_EventReturnTeeth returnTeeth(m_teethReturning, Ray_EventReturnTeeth::STAGE_SURPRISE);

        _deadMan->onEvent(&returnTeeth);

        EventSequenceActorPrepare seqPrepare(btrue);

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (player->getActive() && !player->getIsLeaving())
            {
                Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                if (playerActor)
                {
                    playerActor->onEvent(&seqPrepare);
                }
            }
        }

        Vec3d camPos = getCameraPos();

        if (camPos != Vec3d::Zero)
        {
            CAMERACONTROLLERMANAGER->startCameraSequence(camPos, 1.f);
        }
    }

    Vec3d Ray_GameManagerSequenceReturnTeeth::getCameraPos() const
    {
        Actor* actor = m_deadMan.getActor();

        if (actor)
        {
            Vec3d camPos = actor->getPos();

            if (m_throwing)
            {
                return camPos + m_cameraOffsetThrow;
            }
            else
            {
                return camPos + m_cameraOffsetSurprise;
            }
        }
        else
        {
            return Vec3d::Zero;
        }
    }

    void Ray_GameManagerSequenceReturnTeeth::stop()
    {
        EventSequenceActorPrepare seqPrepare(bfalse);
        EventStopPlayer stopPlayer(bfalse);

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (player->getActive() && !player->getIsLeaving())
            {
                Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                if (playerActor)
                {
                    playerActor->onEvent(&seqPrepare);
                    playerActor->onEvent(&stopPlayer);
                }
            }
        }

        Actor* deadMan = m_deadMan.getActor();

        if (deadMan)
        {
            Ray_EventReturnTeeth returnStop(m_teethReturning, Ray_EventReturnTeeth::STAGE_STOP);
            deadMan->onEvent(&returnStop);
        }

        CAMERACONTROLLERMANAGER->stopCameraSequence(1.f);
        RAY_GAMEMANAGER->returnTeeth();
    }

    void Ray_GameManagerSequenceReturnTeeth::update(f32 _dt)
    {
        if (!m_isFinished)
        {
            Vec3d camPos = getCameraPos();

            CAMERACONTROLLERMANAGER->setCameraSequencePosition(camPos);
        }
    }

    void Ray_GameManagerSequenceReturnTeeth::startThrow()
    {
        EventStopPlayer stopPlayer(btrue);
        Actor* deadMan = m_deadMan.getActor();

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (player->getActive() && !player->getIsLeaving())
            {
                Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                if (playerActor)
                {
                    playerActor->onEvent(&stopPlayer);

                    if (i == m_playerTrigger)
                    {
                        if (deadMan)
                        {
                            Vec2d targetPos = deadMan->get2DPos() + m_playerOffset;

                            Vec2d rayCastDir = Vec2d(0, -5.f);
                            const Vec2d raycastP0 = targetPos;
                            const Vec2d raycastP1 = targetPos + rayCastDir;
                            PhysRayCastContactsContainer contacts;

                            if (PHYSWORLD->rayCastEnvironment(raycastP0, raycastP1, ECOLLISIONFILTER_ENVIRONMENT,
                                                              playerActor->getDepth(), contacts))
                            {
                                for (u32 i = 0; i < contacts.size(); i++)
                                {
                                    SRayCastContact& c = contacts[i];

                                    if (c.m_collidableUserData == playerActor->getRef())
                                    {
                                        continue;
                                    }

                                    PolyLine* poly = AIUtils::getPolyLine(c.m_collidableUserData);

                                    if (!poly || poly->getOwnerActor() == playerActor)
                                    {
                                        continue;
                                    }

                                    if (c.m_edgeIndex0 < poly->getVectorsCount())
                                    {
                                        targetPos = targetPos + (rayCastDir * c.m_t0);
                                        targetPos += Vec2d::Up * playerActor->getRadius();
                                        break;
                                    }
                                }
                            }

                            EventTeleport teleport(targetPos.to3d(playerActor->getPos().m_z), 0.f, bfalse);
                            playerActor->onEvent(&teleport);
                        }

                        Ray_EventReturnTeeth returnTeeth(m_teethReturning, Ray_EventReturnTeeth::STAGE_THROW);
                        playerActor->onEvent(&returnTeeth);
                    }
                    else
                    {
                        Ray_EventReturnTeeth returnTeeth(m_teethReturning, Ray_EventReturnTeeth::STAGE_CHEER);
                        playerActor->onEvent(&returnTeeth);
                    }

                    if (deadMan)
                    {
                        Vec2d dirToDeadMan = deadMan->get2DPos() - playerActor->get2DPos();
                        playerActor->setFlipped(dirToDeadMan.dot(Vec2d::Right) < 0.f);
                    }
                }
            }
        }

        m_throwing = btrue;
    }

    void Ray_GameManagerSequenceReturnTeeth::throwFinished()
    {
        EventStopPlayer stopPlayer(bfalse);

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (player->getActive() && !player->getIsLeaving())
            {
                Actor* playerActor = AIUtils::getActor(player->getCurrentActor());

                if (playerActor)
                {
                    Ray_EventReturnTeeth returnTeeth(m_teethReturning, Ray_EventReturnTeeth::STAGE_FINISHED);
                    playerActor->onEvent(&returnTeeth);
                    playerActor->onEvent(&stopPlayer);
                }
            }
        }

        m_isFinished = btrue;
        RAY_GAMEMANAGER->returnTeeth();
        CAMERACONTROLLERMANAGER->stopCameraSequence(1.f);
    }

    Ray_GameManagerSequenceFadeAndLoadMap::Ray_GameManagerSequenceFadeAndLoadMap()
    {
    }

    Ray_GameManagerSequenceFadeAndLoadMap::~Ray_GameManagerSequenceFadeAndLoadMap()
    {
    }

    void Ray_GameManagerSequenceFadeAndLoadMap::start(const StringID& _mapTag)
    {
        m_mapTagToLoad = _mapTag;
        m_isFinished = bfalse;

        RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeLoadMap());
    }

    void Ray_GameManagerSequenceFadeAndLoadMap::stop()
    {
        RAY_GAMEMANAGER->stopFade(btrue);
    }

    void Ray_GameManagerSequenceFadeAndLoadMap::update(f32 _dt)
    {
        if (RAY_GAMEMANAGER->isFadeOutFinished())
        {
            m_isFinished = btrue;
            //const Path& path = RAY_GAMEMANAGER->getLevelPath(m_mapTagToLoad);
            //GAMEMANAGER->loadGameplayMap(path, btrue, bfalse);
            RAY_GAMEMANAGER->loadMapThroughLoadingScreen(m_mapTagToLoad);
        }
    }

    Ray_GameManagerSequenceChangePageWithAnim::Ray_GameManagerSequenceChangePageWithAnim()
        : m_startPage(ITF_INVALID_OBJREF)
          , m_endPage(ITF_INVALID_OBJREF)
          , m_checkpoint(ITF_INVALID_OBJREF)
          , m_stage(STAGE_ENTER)
          , m_prefetchRef(ITF_INVALID_OBJREF)
          , m_prefetchView(NULL)
    {
    }

    Ray_GameManagerSequenceChangePageWithAnim::~Ray_GameManagerSequenceChangePageWithAnim()
    {
        clearPrefetch();
    }

    void Ray_GameManagerSequenceChangePageWithAnim::start(ActorRef _startPage, ActorRef _endPage, ActorRef _checkpoint,
                                                          const StringID& _anim, const Vec3d& _targetPos)
    {
        m_stage = STAGE_ENTER;
        m_startPage = _startPage;
        m_endPage = _endPage;
        m_checkpoint = _checkpoint;
        m_targetPos = _targetPos;
        m_anim = _anim;
        m_playersTravelInfo.clear();

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));

            if (!player || !player->getActive() || player->isDead() || player->getIsLeaving())
            {
                continue;
            }

            setupPlayer(player);
        }

        setPrefetch();
    }

    void Ray_GameManagerSequenceChangePageWithAnim::stop()
    {
        if (m_stage == STAGE_FADEBLACK)
        {
            RAY_GAMEMANAGER->stopFade(btrue);
        }

        clearPrefetch();
    }

    void Ray_GameManagerSequenceChangePageWithAnim::setupPlayer(Player* _player)
    {
        Actor* currentActor = _player->getCurrentActor().getActor();

        if (currentActor)
        {
            PlayerTravelInfo travelInfo;

            initPlayerTravel(_player, currentActor, m_targetPos, travelInfo);

            m_playersTravelInfo.push_back(travelInfo);

            Ray_EventChangePage evt;
            evt.airTravelActivate();
            evt.setPageActor(m_startPage);
            evt.setHideActor(bfalse);
            evt.setTargetPageActor(m_endPage);
            currentActor->onEvent(&evt);

            Ray_EventPlayAnimState evtPlayAnim(m_anim);
            currentActor->onEvent(&evtPlayAnim);
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    void Ray_GameManagerSequenceChangePageWithAnim::initPlayerTravel(Player* _player, Actor* _actor,
                                                                     const Vec3d& _targetPoint,
                                                                     PlayerTravelInfo& _travelInfo)
    {
        _travelInfo.m_player = _player->getCurrentActor();
        _travelInfo.m_dead = _player->isDead();
        _travelInfo.m_speed = 0.0f;
        _travelInfo.m_moveDir = Vec3d::Zero;
        _travelInfo.m_travelfinished = bfalse;
        _travelInfo.m_enterMovefinished = bfalse;
        _travelInfo.m_enterEjectForce = Vec2d::Zero;
        _travelInfo.m_spline.clear();

        // Compute the spline
        //--------------------
        f32 decelDuration = 0.5f;
        f32 accelDuration = 1.0f;

        Vec3d nextPoint;
        _travelInfo.m_spline.AddSplinePoint(_actor->getPos(), Spline::interp_spline, 0.0f);

        if (StickToPolylinePhysComponent* actorPhysComponent = _actor->GetComponent<StickToPolylinePhysComponent>())
        {
            if (actorPhysComponent->getSpeed().sqrnorm() > 1.f)
            {
                const f32 speedMax = 5.0f;
                Vec2d speed = actorPhysComponent->getSpeed();
                f32 speedNorm = speed.norm();
                nextPoint = _actor->getPos() + (speed.normalize() * Min(speedNorm, 2.0f)).to3d(_actor->getPos().m_z);
                _travelInfo.m_spline.AddSplinePoint(nextPoint, Spline::interp_spline,
                                                    (Min(speedNorm, speedMax) / speedMax) * decelDuration);
            }
        }
        Vec3d playerExitPoint = _targetPoint;

        playerExitPoint.m_z = _actor->getPos().m_z;

        _travelInfo.m_spline.AddSplinePoint(playerExitPoint, Spline::interp_spline,
                                            decelDuration + accelDuration + accelDuration);
    }

    //---------------------------------------------------------------------------------------------------------------------
    // Update player position according to travel infos
    //
    void Ray_GameManagerSequenceChangePageWithAnim::updatePlayerTravel(f32 _dt, PlayerTravelInfo& _travelInfo)
    {
        Actor* actor = static_cast<Actor*>(GETOBJECT(_travelInfo.m_player));
        ITF_ASSERT(actor != NULL);
        if (actor == NULL)
        {
            return;
        }
        i32 p1, p2, p3;
        _travelInfo.m_spline.getPointsAtTime(_travelInfo.m_speed, p1, p2);
        p3 = p2;
        if (p1 >= 0)
        {
            p3 = Min(p1 + 2, _travelInfo.m_spline.GetNumPoints() - 1);
        }
        Vec3d p3Pos = _travelInfo.m_spline.GetNthPoint(p3);
        Vec3d dir = (p3Pos - actor->getPos()).normalize();
        f32 factor = Max(0.0f, _travelInfo.m_moveDir.dot(dir));
        f32 accel = 0.0;
        if (p1 > 0)
        {
            factor = 1.0f;
            accel = _travelInfo.m_speed * 3.0f;
        }

#ifndef ITF_FINAL
        //DebugDraw::arrow2D( m_actor->get2DPos(), m_actor->get2DPos() + dir, 0.0f );
#endif // ITF_FINAL
        // Compute the move speed
        f32 interPointDuration = 0.0;
        //f32 minSpeed = 0.0f;
        if (p1 >= 0)
        {
            interPointDuration = (_travelInfo.m_spline.getTimeAt(Min(p1 + 1, _travelInfo.m_spline.GetNumPoints() - 1)) -
                _travelInfo.m_spline.getTimeAt(p1));
            //minSpeed = interPointDuration * 0.2f;
        }
        //f32 curSpeed = decelDuration - minSpeed;

        f32 wantedSpeed = 1.0f;
        f32 speed = interPointDuration / wantedSpeed;
        f32 speedAdd = 0.0f;
        if (interPointDuration != 0.0f)
        {
            speedAdd = (accel / interPointDuration);
        }

        //_travelInfo.m_speed += ( _dt * interPointDuration / 1.0f )  +  ( _dt * speed * factor ) + ( ( accel / interPointDuration ) * _dt );
        _travelInfo.m_speed += _dt + (_dt * speed * factor) + (speedAdd * _dt);

        // get the interpolated position
        Vec3d newPos;
        _travelInfo.m_spline.GetInterpolatedAtTime(_travelInfo.m_speed, newPos);

        _travelInfo.m_moveDir = newPos - actor->getPos();
        _travelInfo.m_moveDir.normalize();
#ifndef ITF_FINAL
        //DebugDraw::arrow2D( newPos.truncateTo2D(), newPos.truncateTo2D() + _travelInfo.m_moveDir, 0.0f );
#endif // ITF_FINAL

        EventTeleport evtTeleport(newPos, 0.0f, bfalse);
        actor->onEvent(&evtTeleport);
        //actor->setPos( newPos );

        // Check if the travel is finished ( if the previous spline point is invalid)
        if (!_travelInfo.m_travelfinished)
        {
            if ((p1 < 0) || (p1 == p2))
            {
                _travelInfo.m_travelfinished = btrue;
                //LOG( "----- SPLINE FINISHED ");
            }
        }
    }

    void Ray_GameManagerSequenceChangePageWithAnim::setFinalMove()
    {
        Actor* destPage = m_endPage.getActor();

        if (!destPage)
        {
            return;
        }

        bbool needEjectProcess = bfalse;

        Ray_EventChangePageQueryPoints evtQueryPoints;
        destPage->onEvent(&evtQueryPoints);
        Vec3d exitPoint = evtQueryPoints.getExitPoint();
        Vec3d finalPoint = evtQueryPoints.getFinalPoint();
        needEjectProcess = evtQueryPoints.isFinalPointNeedEject();

        // Send move to final point
        //--------------------------
        ObjectRef teleportedPlayer = ITF_INVALID_OBJREF;
        Vec2d teleportedPlayerPos = Vec2d::Zero;
        Vec3d moveToPoint;
        Vec3d dirToExit = exitPoint - finalPoint;

        dirToExit.normalize();

        for (u32 i = 0; i < m_playersTravelInfo.size(); ++i)
        {
            // Update travel
            PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];
            Actor* actor = static_cast<Actor*>(GETOBJECT(tmpTravelInfo.m_player));
            ITF_ASSERT(actor != NULL);
            if (actor != NULL)
            {
                // has destination page
                //----------------------
                moveToPoint = finalPoint + (dirToExit * static_cast<f32>(i) * 0.5f);

                // teleport to exit point
                Vec3d actorPos;
                AIUtils::getTravelExitPointWithActorFeet(exitPoint, actorPos, actor);
                EventTeleport evtTeleport(actorPos, 0.0f, bfalse);
                actor->onEvent(&evtTeleport);

                if (teleportedPlayer == ITF_INVALID_OBJREF)
                {
                    teleportedPlayerPos = actorPos.truncateTo2D();
                }

                // Compute eject force
                if (needEjectProcess)
                {
                    tmpTravelInfo.m_enterEjectForce = AIUtils::computeChangePageVerticalEjectForce(
                        actor, finalPoint.truncateTo2D());
                }

                teleportedPlayer = tmpTravelInfo.m_player;

                Ray_EventChangePage changePageEvent;
                changePageEvent.setHideActor(bfalse);
                changePageEvent.setPageActor(m_endPage);
                changePageEvent.enterMovePosActivate(moveToPoint, tmpTravelInfo.m_enterEjectForce);
                actor->onEvent(&changePageEvent);
            }
        }

        if (destPage)
        {
            CAMERACONTROLLERMANAGER->teleport(destPage->getPos());
        }

        if (teleportedPlayer != ITF_INVALID_OBJREF)
        {
            Ray_EventTeleportUsed teleportUsed(teleportedPlayer, m_endPage, teleportedPlayerPos);

            RAY_GAMEMANAGER->teleportRescuedFriendlies(&teleportUsed);
        }
    }

    void Ray_GameManagerSequenceChangePageWithAnim::clearPrefetch()
    {
        if (m_prefetchRef != ITF_INVALID_OBJREF)
        {
            Actor* prefetchActor = m_prefetchRef.getActor();

            if (prefetchActor)
            {
                AABBPrefetchComponent* prefetchComponent = prefetchActor->GetComponent<AABBPrefetchComponent>();

                if (prefetchComponent)
                {
                    prefetchComponent->forceStart(bfalse);
                }
            }

            m_prefetchRef = ITF_INVALID_OBJREF;
        }
        else if (m_prefetchView)
        {
            m_prefetchView->setActive(bfalse);
            World::enableLoadResourcesDelay();
            View::deleteView(m_prefetchView);
            m_prefetchView = NULL;
        }
    }

    void Ray_GameManagerSequenceChangePageWithAnim::setPrefetch()
    {
        clearPrefetch();

        Actor* page = m_startPage.getActor();

        if (page)
        {
            LinkComponent* linkComponent = page->GetComponent<LinkComponent>();

            if (linkComponent)
            {
                const LinkComponent::ChildrenList& children = linkComponent->getChildren();
                u32 numChildren = children.size();

                for (u32 i = 0; i < numChildren; i++)
                {
                    const ChildEntry& child = children[i];

                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if (path.getIsAbsolute())
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(page, path);
                    }

                    if (target)
                    {
                        Actor* targetActor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));

                        if (targetActor)
                        {
                            AABBPrefetchComponent* prefetchComponent = targetActor->GetComponent<
                                AABBPrefetchComponent>();

                            if (prefetchComponent)
                            {
                                m_prefetchRef = targetActor->getRef();
                                prefetchComponent->forceStart(btrue);
                                break;
                            }
                        }
                    }
                }
            }
        }

        if (m_prefetchRef == ITF_INVALID_OBJREF)
        {
            Actor* destPage = m_endPage.getActor();

            if (destPage)
            {
                if (!m_prefetchView)
                    m_prefetchView = View::createView("Ray_GameManagerSequenceTeleportCheat", bfalse, btrue);

                const f32 deltaX = CAMERA->getDeltaX();
                const f32 deltaY = CAMERA->getDeltaY();
                const Vec3d& destPos = destPage->getPos();
                AABB targetAABB(destPos);

                targetAABB.grow(destPos + Vec3d(-5.f, -5.f, 0.f));
                targetAABB.grow(destPos + Vec3d(5.f, 5.f, 0.f));
                targetAABB.projectTo2D(deltaX, deltaY, destPage->getDepth());
                m_prefetchView->setTargetAABB(targetAABB);
                m_prefetchView->setActive(btrue);
                World::disableLoadResourcesDelay();
            }
        }
    }

    bbool Ray_GameManagerSequenceChangePageWithAnim::checkPrefetchFinished() const
    {
        Actor* prefetchActor = m_prefetchRef.getActor();

        if (prefetchActor)
        {
            AABBPrefetchComponent* prefetchComponent = prefetchActor->GetComponent<AABBPrefetchComponent>();

            if (prefetchComponent)
            {
                return prefetchComponent->prefetchComplete();
            }
        }
        else if (m_prefetchView)
        {
            World* pWorld = CURRENTWORLD;

            if (pWorld)
            {
                return pWorld->isPhysicalReady(*m_prefetchView);
            }
        }

        return btrue;
    }

    void Ray_GameManagerSequenceChangePageWithAnim::update(f32 _dt)
    {
        switch (m_stage)
        {
        case STAGE_ENTER:
            updateEnter(_dt);
            break;
        case STAGE_FADEBLACK:
            updateFadeBlack(_dt);
            break;
        case STAGE_FADEGAME:
            updateFadeGame(_dt);
            break;
        }
    }

    void Ray_GameManagerSequenceChangePageWithAnim::updateEnter(f32 _dt)
    {
        u32 numPlayer = m_playersTravelInfo.size();
        ITF_ASSERT(numPlayer != 0);

        // Players travel update
        //--------------------------
        bbool travelFinished = btrue;

        for (u32 i = 0; i < numPlayer; ++i)
        {
            // Update travel
            PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];

            if (tmpTravelInfo.m_travelfinished == bfalse)
            {
                updatePlayerTravel(_dt, tmpTravelInfo);
                travelFinished = travelFinished && tmpTravelInfo.m_travelfinished;
            }
        }

        // All players are at the exit point
        //
        if (travelFinished)
        {
            RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeChangePageWithAnim());
            m_stage = STAGE_FADEBLACK;
        }
    }

    void Ray_GameManagerSequenceChangePageWithAnim::updateFadeBlack(f32 _dt)
    {
        if (RAY_GAMEMANAGER->isFadeOutFinished())
        {
            if (checkPrefetchFinished())
            {
                setFinalMove();

                RAY_GAMEMANAGER->stopFade(bfalse);
                m_stage = STAGE_FADEGAME;
                clearPrefetch();
            }
        }
    }

    void Ray_GameManagerSequenceChangePageWithAnim::updateFadeGame(f32 _dt)
    {
        if (RAY_GAMEMANAGER->isFadeInFinished())
        {
            bbool enterMoveFinished = btrue;
            Actor* actor;

            for (u32 i = 0; i < m_playersTravelInfo.size(); ++i)
            {
                PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];

                // Move not finished
                if (!tmpTravelInfo.m_enterMovefinished)
                {
                    actor = static_cast<Actor*>(GETOBJECT(tmpTravelInfo.m_player));
                    ITF_ASSERT(actor != NULL);
                    if (actor != NULL)
                    {
                        Player* player = GAMEMANAGER->getPlayerFromActor(tmpTravelInfo.m_player);

                        if (player && player->isDead() && !tmpTravelInfo.m_dead)
                        {
                            tmpTravelInfo.m_enterMovefinished = btrue;
                            tmpTravelInfo.m_dead = btrue;
                        }
                        else
                        {
                            Ray_EventQueryReachedMoveToPoint moveFinishedEvt;
                            actor->onEvent(&moveFinishedEvt);
                            tmpTravelInfo.m_enterMovefinished = moveFinishedEvt.getHasReached();
                        }

                        enterMoveFinished &= tmpTravelInfo.m_enterMovefinished;
                    }
                }
            }

            // FINISHED
            if (enterMoveFinished)
            {
                if (m_checkpoint != ITF_INVALID_OBJREF)
                {
                    RAY_GAMEMANAGER->onReachedCheckpoint(m_checkpoint, btrue);
                }

                Ray_EventChangePage changePageEvent;
                changePageEvent.setFinished();
                changePageEvent.setPageActor(m_endPage);

                for (u32 i = 0; i < m_playersTravelInfo.size(); ++i)
                {
                    PlayerTravelInfo& tmpTravelInfo = m_playersTravelInfo[i];

                    // Move not finished
                    actor = static_cast<Actor*>(GETOBJECT(tmpTravelInfo.m_player));
                    ITF_ASSERT(actor != NULL);
                    if (actor != NULL)
                    {
                        actor->onEvent(&changePageEvent);
                    }
                }

                Actor* finalPage = m_endPage.getActor();

                if (finalPage != NULL)
                {
                    Ray_EventChangePage changePageEvent;

                    changePageEvent.setFinished();
                    changePageEvent.setPageActor(finalPage->getRef());

                    finalPage->onEvent(&changePageEvent);
                }

                m_isFinished = true;
            }
        }
    }


    void Ray_GameManagerSequenceEndGame::start(bbool _playMovie)
    {
        // Reset values set for this sequence on CinematicManager
        CINEMATICMANAGER->resetPlayersChannels();

        m_isFinished = bfalse;
        // Stop all music before playing video
        MUSICMANAGER->stopAll(0.5f);

        m_playMovie = _playMovie;

        m_goToEndLevel = bfalse;
        const StringID& currentLevel = RAY_GAMEMANAGER->getCurrentLevelName();
        if (currentLevel.isValid())
        {
            StringID type = RAY_GAMEMANAGER->getMapConfigType(currentLevel);
            const Ray_GameManagerConfig_Template::MissionConfig* config = RAY_GAMEMANAGER->findMissionConfigByType(
                type, bfalse);
            if (config && config->m_medalSlots.size() > 0)
            {
                m_goToEndLevel = btrue;
            }
        }

        // Make sure players are not on mouskitos
        reinitPlayers();

        // don't allow player inputs during this sequence
        RAY_GAMEMANAGER->setInputModeForGamePlay(bfalse);

        //start play movie
        if (_playMovie)
        {
            // Force all streaming music to be paused (which includes ambiances)
            MUSICMANAGER->pause(btrue);
            MUSICMANAGER->disableStreamingThread();

            playMovie();
            return;
        }

        // start to preload credits
        Vector<ObjectPath> prefetchList;
        m_preloadedCreditMapRef = RAY_GAMEMANAGER->preLoadMap(RAY_GAMEMANAGER->getCreditsMap(), prefetchList);
        m_stage = Stage_WaitingCredits;
        m_playersTeleported = bfalse;
    }

    void Ray_GameManagerSequenceEndGame::playMovie()
    {
        //start play movie
        m_moviePlayer.setListener(this);
        m_moviePlayer.play("cine_fin", bfalse);

        // Mute sound
        GAMEMANAGER->stopDefaultBusMix();
        SOUND_MANAGER()->playBusMix(SOUND_BUS_MIX_PLAYINGMOVIE, btrue);

        // As the end sequence has not been done seriously, We need to wait "enough time" to let
        // players being teleported to another place while playing movie. The camera needs time to
        // focus on where the players are teleported. So this overall shit seems to need AT LEAST 4s...
        m_waitBeforeSkipVideo = 4.0f;

        m_stage = Stage_PlayMovie;
    }

    void Ray_GameManagerSequenceEndGame::stop()
    {
        m_goToEndLevel = bfalse;

        if (m_playMovie)
        {
            m_playMovie = bfalse;
            m_moviePlayer.stop();
        }

        if (m_preloadedEndLevelMapRef.isValid())
            RAY_GAMEMANAGER->deletePreloadedMap(m_preloadedEndLevelMapRef);

        if (m_preloadedCreditMapRef.isValid())
            RAY_GAMEMANAGER->deletePreloadedMap(m_preloadedCreditMapRef);

        // restore player inputs
        RAY_GAMEMANAGER->setInputModeForGamePlay(btrue);

        m_isFinished = btrue;
    }

    bbool Ray_GameManagerSequenceEndGame::canPauseGame()
    {
        return (m_stage != Stage_PlayMovie);
    }

    void Ray_GameManagerSequenceEndGame::update(f32 _dt)
    {
        switch (m_stage)
        {
        case Stage_PlayMovie:
            {
                m_waitBeforeSkipVideo -= _dt;
                // Check for pads
                bbool stopVideo = bfalse;
                if (!m_playersTeleported)
                {
                    m_playersTeleported = btrue;
                    Vec3d pos3D = RAY_GAMEMANAGER->getEndSequencePlayerPosition();
                    Vec3d posOffset(Vec3d::Zero);

                    Scene* scene = NULL;
                    StringID moShooterB = RAY_GAMEMANAGER->getLevelPath(s_MO_SHOOTER_B).getStringID();
                    const u32 uWorldCount = WORLD_MANAGER->getWorldCount();
                    for (u32 i = 0; i < uWorldCount; ++i)
                    {
                        World* pWorld = WORLD_MANAGER->getWorldAt(i);
                        if (pWorld->isActive()) // world must be active
                        {
                            Scene* pScene = pWorld->getRootScene();
                            if (pScene->findScene(moShooterB, scene, posOffset))
                                break;
                        }
                    }

                    pos3D += posOffset;
                    Vec2d pos2D = pos3D.truncateTo2D();

                    for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i)
                    {
                        Player* player = GAMEMANAGER->getPlayer(i);
                        if (player)
                        {
                            RAY_GAMEMANAGER->teleportPlayer(i, pos2D, pos3D.m_z, btrue);
                        }
                    }
                }
                else if (m_waitBeforeSkipVideo <= 0.0f)
                {
                    for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount() && !stopVideo; i++)
                    {
                        // Buttons
                        InputAdapter::PressStatus buts[JOY_MAX_BUT];
                        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua, i, buts, JOY_MAX_BUT);

                        if (buts[m_joyButton_Start] == InputAdapter::JustReleased)
                        {
                            stopVideo = btrue;
                        }
                    }
                }

                if (stopVideo)
                    m_moviePlayer.stop();
                else
                    m_moviePlayer.update(_dt);
            }
            break;
        case Stage_WaitBeforeSpawnMedal:
            {
                m_waitSpawnMedalFrame--;
                if (m_waitSpawnMedalFrame == 0)
                {
                    m_preloadedEndLevelMapRef = RAY_GAMEMANAGER->preloadEndLevelMap();

                    spawnMedal();

                    m_stage = Stage_SpawnMedal;
                }
            }
            break;
        case Stage_SpawnMedal:
            {
                if (!RAY_GAMEMANAGER->isInGameplayPause())
                {
                    Ray_EventFxScreen eventFade;
                    eventFade.setIsStart(btrue);
                    EVENTMANAGER->broadcastEvent(&eventFade);
                    m_stage = Stage_WaitingEndFade;
                }
            }
            break;
        case Stage_WaitingEndFade:
            {
                if (RAY_GAMEMANAGER->isFadeOutFinished())
                {
                    m_stage = Stage_WaitingLevelEnding;
                }
            }
            break;
        case Stage_WaitingLevelEnding:
            if (GAMEMANAGER->isPreloadedMapLoaded(m_preloadedEndLevelMapRef))
            {
                switchToLevelEnding();
                m_stage = Stage_LevelEnding;
            }
            break;
        case Stage_LevelEnding:
            break;
        case Stage_WaitingCredits:
            if (GAMEMANAGER->isPreloadedMapLoaded(m_preloadedCreditMapRef))
            {
                //// Set music theme
                //World * world = static_cast<World *>(m_preloadedCreditMapRef.getObject());
                //if ( world )
                //{
                //    u32 sceneCount = world->getSceneCount();
                //    for (u32 i = 0 ; i < sceneCount ; ++i)
                //    {
                //        Scene * pScene = world->getSceneAt(i);
                //        if ( pScene && pScene->getMusicTheme() != StringID::Invalid )
                //            RAY_GAMEMANAGER->setMusicTheme(pScene->getMusicTheme());
                //    }
                //}

                RAY_GAMEMANAGER->setCurrentLevelName(RAY_GAMEMANAGER->getCreditsMap());

                RAY_GAMEMANAGER->switchToPreloadedMap(m_preloadedCreditMapRef);
                m_preloadedCreditMapRef.invalidate();
                m_stage = Stage_Credits;
            }
            break;
        case Stage_Credits:
            stop();
            break;
        }
    }


    void Ray_GameManagerSequenceEndGame::onMovieStopped()
    {
        // Restore sound
        SOUND_MANAGER()->stopBusMix(SOUND_BUS_MIX_PLAYINGMOVIE, btrue);
        GAMEMANAGER->setDefaultBusMix();

        // Unpause musics
        MUSICMANAGER->enableStreamingThread();
        MUSICMANAGER->resume();

        if (m_goToEndLevel)
        {
            m_waitSpawnMedalFrame = 10;
            m_stage = Stage_WaitBeforeSpawnMedal;
            return;
        }

        // start to preload credits
        preloadCredits();
        m_stage = Stage_WaitingCredits;
    }

    void Ray_GameManagerSequenceEndGame::spawnMedal()
    {
        // spawn medal
        Ray_EventShowElectoonMedal showMedal;

        showMedal.m_forBrokenCage = bfalse;
        showMedal.m_playVictorySequence = bfalse;
        showMedal.m_forBeatenBoss = btrue;
        showMedal.m_playEndSequence = bfalse;
        showMedal.m_autoEndLevel = bfalse;
        showMedal.m_forLumAndTimeAttacks = bfalse;
        showMedal.m_dontSetLumAttack1 = btrue;
        showMedal.m_dontSetLumAttack2 = btrue;
        showMedal.m_dontSetTimeAttack = btrue;

        EVENTMANAGER->broadcastEvent(&showMedal);
    }

    void Ray_GameManagerSequenceEndGame::switchToLevelEnding()
    {
        // Switch to end level
        RAY_GAMEMANAGER->switchToPreloadedMap(m_preloadedEndLevelMapRef);
        m_preloadedEndLevelMapRef.invalidate();

        // start to preload credits
        preloadCredits();
    }

    void Ray_GameManagerSequenceEndGame::reinitPlayers()
    {
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i)
        {
            Player* player = GAMEMANAGER->getPlayer(i);
            if (player)
            {
                GAMEMANAGER->changePlayerGameMode(player, RAY_GAMEMODE_PLATFORMER);
            }
        }
    }

    void Ray_GameManagerSequenceEndGame::onMovieStarted()
    {
    }

    void Ray_GameManagerSequenceEndGame::launchCredits()
    {
        if (!m_preloadedCreditMapRef.isValid())
            preloadCredits();

        m_stage = Stage_WaitingCredits;
    }

    void Ray_GameManagerSequenceEndGame::skipMovie()
    {
        if (m_playMovie)
        {
            m_moviePlayer.stop();
        }

        if (m_goToEndLevel)
            m_stage = Stage_WaitingLevelEnding;
        else
        {
            preloadCredits();
            m_stage = Stage_WaitingCredits;
        }
    }

    void Ray_GameManagerSequenceEndGame::preloadCredits()
    {
        // start to preload credits
        Vector<ObjectPath> prefetchList;
        m_preloadedCreditMapRef = RAY_GAMEMANAGER->preLoadMap(RAY_GAMEMANAGER->getCreditsMap(), prefetchList);
    }

    //------------------------------------------------------------------------------
    Ray_GameManagerSequenceWorldMapUnlock::Ray_GameManagerSequenceWorldMapUnlock()
        : Ray_GameManagerSequenceBase()
          , m_pending(bfalse)
          , m_stage(Stage_NA)
          , m_nodeRef()
          , m_waitCountdown(0.0f)
          , m_displayingUnlocks(bfalse)
          , m_justCompletedSpots()
    {
    }

    //------------------------------------------------------------------------------
    Ray_GameManagerSequenceWorldMapUnlock::~Ray_GameManagerSequenceWorldMapUnlock()
    {
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapUnlock::start(const ActorRef& _nodeRef)
    {
        // guard
        ITF_ASSERT(_nodeRef.getActor());
        if (!_nodeRef.getActor())
        {
            stop();
            return;
        }
        m_nodeRef = _nodeRef;

        // we're starting
        m_isFinished = bfalse;

        // wait a bit to prevent players from accidentally launching the map
        m_waitCountdown = 0.5f;
        m_stage = Stage_Wait;
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapUnlock::update(f32 _dt)
    {
        switch (m_stage)
        {
        case Stage_Wait:
            {
                m_waitCountdown = Max(m_waitCountdown - _dt, 0.0f);
                if (0.0f == m_waitCountdown)
                {
                    // start electoon / tooth visual reward
                    if (Actor* node = m_nodeRef.getActor())
                    {
                        Ray_EventShowElectoonToothScore showElectoonToothScore;
                        showElectoonToothScore.setSourcePos(node->getPos());
                        showElectoonToothScore.setShow(btrue);
                        EVENTMANAGER->broadcastEvent(&showElectoonToothScore);
                        u32 electoonDest = RAY_GAMEMANAGER->getElectoonsCount();
                        u32 toothDest = RAY_GAMEMANAGER->getTotalTakenToothCount();
                        RAY_GAMEMANAGER->setWMLastElectoonCount(electoonDest);
                        RAY_GAMEMANAGER->setWMLastToothCount(toothDest);
                        if (showElectoonToothScore.getReceived())
                        {
                            m_stage = Stage_ElectoonTooth;
                        }
                        else
                        {
                            ITF_WARNING(NULL, bfalse,
                                        "Nobody listens to electoon/teeth event, is the actor in the world map scene?");
                            Actor* unlockMsg = RAY_GAMEMANAGER->getWMUnlockMessageActor().getActor();
                            if (unlockMsg)
                            {
                                unlockMsg->enable();
                                m_stage = Stage_DisplayUnlocks;
                            }
                            else
                            {
                                startOpenPaths();
                            }
                        }
                    }
                    else
                    {
                        ITF_ASSERT_MSG(bfalse, "Can't retrieve node");
                        stop();
                    }
                }
            }
            break;
        case Stage_ElectoonTooth:
            {
                // fetch electoon / tooth visual reward state
                Ray_EventFetchElectoonToothScoreInfos fetchElectoonToothScore;
                EVENTMANAGER->broadcastEvent(&fetchElectoonToothScore);
                if (!fetchElectoonToothScore.getHasIconsInFlow())
                {
                    Actor* unlockMsg = RAY_GAMEMANAGER->getWMUnlockMessageActor().getActor();

                    if (unlockMsg)
                    {
                        unlockMsg->enable();
                        m_stage = Stage_DisplayUnlocks;
                    }
                    else
                    {
                        startOpenPaths();
                    }
                }
            }
            break;
        case Stage_DisplayUnlocks:
            {
                Actor* unlockMsg = RAY_GAMEMANAGER->getWMUnlockMessageActor().getActor();

                if (unlockMsg)
                {
                    Ray_WMContentUnlockComponent* unlockComponent = unlockMsg->GetComponent<
                        Ray_WMContentUnlockComponent>();

                    if (!unlockComponent || unlockComponent->isFinished())
                    {
                        unlockMsg->disable();
                        startOpenPaths();
                        m_displayingUnlocks = bfalse;
                    }
                    else
                    {
                        m_displayingUnlocks = btrue;
                    }
                }
                else
                {
                    startOpenPaths();
                }
            }
            break;
        case Stage_OpenPaths:
            {
                // fetch open paths state
                bbool allDone = btrue;
                u32 count = m_justCompletedSpots.size();
                for (u32 i = 0; i < count; ++i)
                {
                    ObjectPath spotPath = RAY_GAMEMANAGER->getWMSpotObjectPath(m_justCompletedSpots[i].m_tag);
                    if (spotPath.isValid())
                    {
                        if (Pickable* spot = SceneObjectPathUtils::getObjectFromAbsolutePath(spotPath))
                        {
                            Ray_EventFetchWMOpenPaths fetchOpenPaths;
                            spot->onEvent(&fetchOpenPaths);
                            if (!fetchOpenPaths.isDone())
                            {
                                allDone = bfalse;
                            }
                        }
                        else
                        {
                            ITF_ASSERT_MSG(bfalse, "Can't retrieve spot");
                            stop();
                            break; // crash fix RO-14644, get out of this evil loop
                        }
                    }
                }
                if (allDone)
                {
                    // done!
                    m_stage = Stage_Done;
                    stop();
                }
            }
            break;
        }
    }

    void Ray_GameManagerSequenceWorldMapUnlock::startOpenPaths()
    {
        u32 count = m_justCompletedSpots.size();
        for (u32 i = 0; i < count; ++i)
        {
            ObjectPath spotPath = RAY_GAMEMANAGER->getWMSpotObjectPath(m_justCompletedSpots[i].m_tag);
            if (spotPath.isValid())
            {
                if (Pickable* spot = SceneObjectPathUtils::getObjectFromAbsolutePath(spotPath))
                {
                    Ray_EventWMOpenPaths openPaths;
                    spot->onEvent(&openPaths);
                }
                else
                {
                    ITF_ASSERT_MSG(bfalse, "Can't retrieve spot");
                    stop();
                    break; // crash fix RO-14644, get out of this evil loop
                }
            }
        }
        m_stage = Stage_OpenPaths;

        RAY_GAMEMANAGER->onWorldMapElectoonsEarned();
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapUnlock::stop()
    {
        m_isFinished = btrue;
        m_stage = Stage_NA;
        m_nodeRef.invalidate();
        m_justCompletedSpots.clear();
        m_displayingUnlocks = bfalse;
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapUnlock::clearJustCompletedSpots()
    {
        m_justCompletedSpots.clear();
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapUnlock::setJustCompletedSpot(const StringID& _tag, bbool _bWorldOnly)
    {
        if (m_justCompletedSpots.size() == s_maxCompletedSpots)
        {
            ITF_ASSERT_MSG(bfalse, "Too many spots completed at the same time, increase s_maxCompletedSpots");
        }
        else
        {
            CompletedSpot completedSpot;
            completedSpot.m_tag = _tag;
            completedSpot.m_bWorldOnly = _bWorldOnly;
            m_justCompletedSpots.push_back(completedSpot);
        }
    }

    //------------------------------------------------------------------------------
    bbool Ray_GameManagerSequenceWorldMapUnlock::isJustCompletedSpot(const StringID& _tag) const
    {
        u32 spotsCount = m_justCompletedSpots.size();
        for (u32 i = 0; i < spotsCount; ++i)
        {
            if (m_justCompletedSpots[i].m_tag == _tag)
            {
                return btrue;
            }
        }
        return bfalse;
    }

    //------------------------------------------------------------------------------
    bbool Ray_GameManagerSequenceWorldMapUnlock::isJustCompletedSpotBWorldOnly(const StringID& _tag) const
    {
        u32 spotsCount = m_justCompletedSpots.size();
        for (u32 i = 0; i < spotsCount; ++i)
        {
            if (m_justCompletedSpots[i].m_tag == _tag)
            {
                return m_justCompletedSpots[i].m_bWorldOnly;
            }
        }
        ITF_ASSERT_MSG(bfalse, "Can't retrieve spot");
        return bfalse;
    }

    //------------------------------------------------------------------------------
    Ray_GameManagerSequenceWorldMapTeleport::Ray_GameManagerSequenceWorldMapTeleport()
        : Ray_GameManagerSequenceBase()
          , m_spotFromRef()
          , m_spotToRef()
          , m_currentState(STATE_FADE_BLACK)
          , m_prefetchView(NULL)
          , m_prefetchFailSafe(0.f)
    {
    }

    //------------------------------------------------------------------------------
    Ray_GameManagerSequenceWorldMapTeleport::~Ray_GameManagerSequenceWorldMapTeleport()
    {
        freePrefetch();
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapTeleport::start(const ActorRef& _spotFromRef, const ActorRef& _spotToRef)
    {
        // safety check
        ITF_ASSERT(RAY_GAMEMANAGER->areAllActivePlayersInGameMode(RAY_GAMEMODE_WORLDMAP));

        // fill members
        m_spotFromRef = _spotFromRef;
        m_spotToRef = _spotToRef;
        m_isFinished = bfalse;
        m_prefetchFailSafe = 0.0f;
        m_currentState = STATE_FADE_BLACK;
        // Stop music
        MUSICMANAGER->stop(METRONOME_TYPE_GAMEPLAY, 1.0f);

        // start fade
        RAY_GAMEMANAGER->startFade(RAY_GAMEMANAGER->getTemplate()->getFadeTypeWorldMapTeleport());

        // start prefetch
        Actor* spotTo = _spotToRef.getActor();
        ITF_ASSERT_MSG(spotTo, "Can't retrieve spot");
        if (!spotTo)
        {
            stop();
            return;
        }
        if (!m_prefetchView)
        {
            m_prefetchView = View::createView("Ray_GameManagerSequenceWorldMapTeleport", bfalse, btrue);
        }
        const f32 deltaX = CAMERA->getDeltaX();
        const f32 deltaY = CAMERA->getDeltaY();
        Vec3d targetPos = spotTo->getPos();
        AABB targetAABB(targetPos);
        targetAABB.grow(targetPos + Vec3d(-10.f, -10.f, 0.f));
        targetAABB.grow(targetPos + Vec3d(10.f, 10.f, 0.f));
        targetAABB.projectTo2D(deltaX, deltaY, spotTo->getDepth());
        m_prefetchView->setTargetAABB(targetAABB);
        m_prefetchView->setActive(btrue);
        World::disableLoadResourcesDelay();
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapTeleport::stop()
    {
        // safety check
        ITF_ASSERT(RAY_GAMEMANAGER->areAllActivePlayersInGameMode(RAY_GAMEMODE_WORLDMAP));

        // clear prefetch
        freePrefetch();

        // stop fade
        RAY_GAMEMANAGER->stopFade(btrue);
        RAY_GAMEMANAGER->hideLoadNotificationActor();

        // start pending unlock sequence
        if (RAY_GAMEMANAGER->isPendingWorldMapUnlockSequence())
        {
            RAY_GAMEMANAGER->triggerWorldMapUnlockSequence(m_spotToRef);
        }

        // clear members
        m_spotFromRef.invalidate();
        m_spotToRef.invalidate();
        m_isFinished = btrue;
        m_prefetchFailSafe = 0.0f;
        m_currentState = STATE_FADE_BLACK;
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapTeleport::update(f32 _dt)
    {
        switch (m_currentState)
        {
        case STATE_FADE_BLACK:
            updateFadeBlack(_dt);
            break;
        case STATE_FADE_GAME:
            updateFadeToGame(_dt);
            break;
        }
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapTeleport::updateFadeBlack(f32 _dt)
    {
        bbool nextState = bfalse;

        // guard
        Actor* spotTo = m_spotToRef.getActor();
        ITF_ASSERT_MSG(spotTo, "Can't retrieve spot");
        if (!spotTo)
        {
            stop();
            return;
        }

        // wait for fade out and prefetch to end
        // + prefetch time out
        if (RAY_GAMEMANAGER->isFadeOutFinished())
        {
            if (m_prefetchView)
            {
                ITF_ASSERT(m_prefetchFailSafe < MAX_PREFETCH_TIMEOUT);
                if (spotTo->getWorld()->isPhysicalReady(*m_prefetchView) || m_prefetchFailSafe >= MAX_PREFETCH_TIMEOUT)
                {
                    nextState = btrue;
                }
                else
                {
                    m_prefetchFailSafe += _dt;

                    if (m_prefetchFailSafe >= MAX_FADETIME_FORICON)
                    {
                        RAY_GAMEMANAGER->showLoadNotificationActor();
                    }
                }
            }
            else
            {
                nextState = btrue;
            }
        }

        if (nextState)
        {
            // start fade to game
            m_currentState = STATE_FADE_GAME;
            RAY_GAMEMANAGER->stopFade(bfalse);
            RAY_GAMEMANAGER->hideLoadNotificationActor();

            // teleport players
            ITF_ASSERT(RAY_GAMEMANAGER->areAllActivePlayersInGameMode(RAY_GAMEMODE_WORLDMAP));
            u32 numPlayers = RAY_GAMEMANAGER->getMaxPlayerCount();
            EventTeleportToActor teleport(m_spotToRef);
            for (u32 i = 0; i < numPlayers; ++i)
            {
                Ray_Player* p = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
                if (!p || !p->getActive() || p->getIsLeaving())
                {
                    continue;
                }
                if (Actor* currentActor = p->getCurrentActor().getActor())
                {
                    currentActor->onEvent(&teleport);
                }
            }

            // teleport camera
            CAMERACONTROLLERMANAGER->teleport(spotTo->getPos());
        }
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapTeleport::freePrefetch()
    {
        if (m_prefetchView)
        {
            m_prefetchView->setActive(bfalse);
            World::enableLoadResourcesDelay();
            View::deleteView(m_prefetchView);
            m_prefetchView = NULL;
        }
    }

    //------------------------------------------------------------------------------
    void Ray_GameManagerSequenceWorldMapTeleport::updateFadeToGame(f32 _dt)
    {
        if (RAY_GAMEMANAGER->isFadeInFinished())
        {
            stop();
        }
    }

    //------------------------------------------------------------------------------
    // Game Options / Accessibility
    //------------------------------------------------------------------------------

    void Ray_GameManager::registerResolutionOption()
    {
        m_availableResolutions.clear();
#ifdef ITF_WINDOWS
        DEVMODE mode = {};
        mode.dmSize = sizeof(mode);
        for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &mode) != 0; ++iModeNum)
        {
            const i32 w = static_cast<i32>(mode.dmPelsWidth);
            const i32 h = static_cast<i32>(mode.dmPelsHeight);
            if (w < 1024) continue;
            bbool found = bfalse;
            for (u32 i = 0; i < m_availableResolutions.size(); ++i)
            {
                if (m_availableResolutions[i].width == w && m_availableResolutions[i].height == h)
                {
                    found = btrue;
                    break;
                }
            }
            if (!found)
            {
                ResolutionEntry entry; entry.width = w; entry.height = h;
                m_availableResolutions.push_back(entry);
            }
        }
#endif
        if (m_availableResolutions.empty())
        {
            ResolutionEntry defaults[] = {{1280,720},{1920,1080},{2560,1440},{3840,2160}};
            for (auto& e : defaults) m_availableResolutions.push_back(e);
        }
        Vector<i32> choices;
        choices.reserve(m_availableResolutions.size());
        for (u32 i = 0; i < m_availableResolutions.size(); ++i) choices.push_back(static_cast<i32>(i));
        u32 curW = GFX_ADAPTER ? GFX_ADAPTER->getScreenWidth() : 1920;
        u32 curH = GFX_ADAPTER ? GFX_ADAPTER->getScreenHeight() : 1080;
        i32 defaultIndex = 0;
        for (u32 i = 0; i < m_availableResolutions.size(); ++i)
        {
            if (m_availableResolutions[i].width == static_cast<i32>(curW) && m_availableResolutions[i].height == static_cast<i32>(curH))
            {
                defaultIndex = static_cast<i32>(i);
                break;
            }
        }
        m_gameOptionManager.registerIntListOption(OPTION_RESOLUTION, choices, defaultIndex);
    }

    void Ray_GameManager::registerWindowedOption()
    {
        m_gameOptionManager.registerBoolOption(OPTION_WINDOWED, bfalse); // Default: Fullscreen
    }

    void Ray_GameManager::registerLanguageOption()
    {
        Vector<ITF_LANGUAGE> supported;
        supported.push_back(ITF_LANGUAGE_ENGLISH);
        supported.push_back(ITF_LANGUAGE_FRENCH);
        supported.push_back(ITF_LANGUAGE_ITALIAN);
        supported.push_back(ITF_LANGUAGE_GERMAN);
        supported.push_back(ITF_LANGUAGE_SPANISH);
        supported.push_back(ITF_LANGUAGE_PORTUGUESE);
        supported.push_back(ITF_LANGUAGE_JAPANESE);
        supported.push_back(ITF_LANGUAGE_TRADITIONALCHINESE);
        supported.push_back(ITF_LANGUAGE_SIMPLIFIEDCHINESE);
        supported.push_back(ITF_LANGUAGE_RUSSIAN);
        supported.push_back(ITF_LANGUAGE_POLISH);

        const ITF_LANGUAGE systemLang = SYSTEM_ADAPTER->getSystemLanguage(
            supported.data(),
            static_cast<i32>(supported.size()),
            ITF_LANGUAGE_ENGLISH
        );

        Vector<i32> languages;
        languages.reserve(supported.size());
        for (const auto l : supported)
        {
            languages.push_back(l);
        }
        m_gameOptionManager.registerIntListOption(OPTION_LANGUAGE, languages, systemLang);
    }

    void Ray_GameManager::registerStartWithHeartOption()
    {
        Vector<i32> healthModifierOptions;
        healthModifierOptions.push_back(HealthModifier_Default);
        healthModifierOptions.push_back(HealthModifier_RedHeart);
        healthModifierOptions.push_back(HealthModifier_GoldenHeart);
        healthModifierOptions.push_back(HealthModifier_DiamondHeart);
        m_gameOptionManager.registerIntListOption(OPTION_START_WITH_HEART, healthModifierOptions, HealthModifier_Default);
    }

    void Ray_GameManager::registerRunButtonOption()
    {
        Vector<i32> runButtonModes;
        runButtonModes.push_back(RunButtonMode_Hold);
        runButtonModes.push_back(RunButtonMode_Toggle);
        m_gameOptionManager.registerIntListOption(OPTION_RUN_BUTTON, runButtonModes, RunButtonMode_Hold);
    }

    void Ray_GameManager::registerMurfyAssistOption()
    {
        m_gameOptionManager.registerBoolOption(OPTION_MURFY_ASSIST, bfalse); // Default: Off
    }

    void Ray_GameManager::registerVibrationOption()
    {
        Vector<i32> vibrationModes;
        vibrationModes.push_back(VibrationMode_Off);
        vibrationModes.push_back(VibrationMode_On);
        m_gameOptionManager.registerIntListOption(OPTION_VIBRATIONS, vibrationModes, VibrationMode_On); // Default: On
    }

    void Ray_GameManager::registerMasterVolumeOption()
    {
        m_gameOptionManager.registerFloatOption(OPTION_MASTER_VOLUME, 1.0f, 0.0f, 1.0f); // Default: Full volume
    }

    void Ray_GameManager::registerMusicVolumeOption()
    {
        m_gameOptionManager.registerFloatOption(OPTION_MUSIC_VOLUME, 1.0f, 0.0f, 1.0f); // Default: Full volume
    }

    void Ray_GameManager::registerSFXVolumeOption()
    {
        m_gameOptionManager.registerFloatOption(OPTION_SFX_VOLUME, 1.0f, 0.0f, 1.0f); // Default: Full volume
    }

    void Ray_GameManager::registerIntensityOption()
    {
        m_gameOptionManager.registerFloatOption(OPTION_INTENSITY, 1.0f, 0.0f, 1.0f); // Default: Full intensity
    }

    void Ray_GameManager::registerLastPlayTime()
    {
        m_gameOptionManager.registerFloatOption(LAST_PLAY_TIME, 0.0f, 0.0f, FLT_MAX);
    }

#if defined(ITF_WINDOWS)
    void Ray_GameManager::registerPCKeyboardControllerSharingOption()
    {
        m_gameOptionManager.registerBoolOption(OPTION_PC_KEYBOARD_CONTROLLER_SHARING, btrue);
    }
#endif

    void Ray_GameManager::registerAllGameOptions()
    {
        m_gameOptionManager.init();
        registerResolutionOption();
        registerWindowedOption();
        registerLanguageOption();
        registerStartWithHeartOption();
        registerRunButtonOption();
        registerMurfyAssistOption();
        registerVibrationOption();
        registerMasterVolumeOption();
        registerMusicVolumeOption();
        registerSFXVolumeOption();
        registerIntensityOption();
        registerLastPlayTime();
#if defined(ITF_WINDOWS)
        registerPCKeyboardControllerSharingOption();
#endif
    }

    EHealthModifier Ray_GameManager::getHealthModifier() const
    {
        return static_cast<EHealthModifier>(m_gameOptionManager.getIntListOptionValue(OPTION_START_WITH_HEART, HealthModifier_Default));
    }

    void Ray_GameManager::setHealthModifier(EHealthModifier _modifier)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_START_WITH_HEART, _modifier);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_GameManager::restoreHealthModifierForAllPlayers()
    {
        EHealthModifier modifier = getHealthModifier();
        if (modifier == HealthModifier_Default)
            return;

        for (u32 i = 0; i < getMaxPlayerCount(); ++i)
        {
            Ray_Player* player = static_cast<Ray_Player*>(getPlayer(i));
            if (player && !player->isDead())
            {
                switch (modifier)
                {
                case HealthModifier_RedHeart:
                    player->setHeartTier(HeartTier_Red);
                    if (player->getHitPoints() < player->getMaxHitPoints())
                    {
                        player->addHitPoints(1);
                    }
                    break;

                case HealthModifier_GoldenHeart:
                    player->setHeartTier(HeartTier_Golden);
                    if (player->getHitPoints() < player->getMaxHitPoints())
                    {
                        player->addHitPoints(1);
                    }
                    break;

                case HealthModifier_DiamondHeart:
                    player->setHeartTier(HeartTier_Diamond);
                    if (player->getHitPoints() < player->getMaxHitPoints())
                    {
                        player->addHitPoints(1);
                    }
                    player->resetConsecutiveHits();
                    break;

                default:
                    break;
                }

                // Enable HeartShield component through PowerUpManager
                m_powerUpManager.setEnabled(Ray_PowerUp_HeartShield, i, btrue);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // OPTION MENU - DISPLAY OPTIONS
    ///////////////////////////////////////////////////////////////////////////////////////////

    i32 Ray_GameManager::getResolutionIndex() const
    {
        return m_gameOptionManager.getListOptionIndex(OPTION_RESOLUTION);
    }

    void Ray_GameManager::setResolutionIndex(i32 index)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_RESOLUTION, index);
    }

    const char* Ray_GameManager::getResolutionDisplayName(i32 index) const
    {
        static String8 s;
        if (index >= 0 && index < static_cast<i32>(m_availableResolutions.size()))
        {
            s.setTextFormat("%d x %d", m_availableResolutions[index].width, m_availableResolutions[index].height);
            return s.cStr();
        }
        return "Unknown";
    }

    bbool Ray_GameManager::isWindowed() const
    {
        return m_gameOptionManager.getBoolOption(OPTION_WINDOWED);
    }

    void Ray_GameManager::setWindowed(bbool windowed)
    {
        m_gameOptionManager.setBoolOption(OPTION_WINDOWED, windowed);
    }

    i32 Ray_GameManager::getLanguageIndex() const
    {
        return m_gameOptionManager.getListOptionIndex(OPTION_LANGUAGE);
    }

    void Ray_GameManager::setLanguageIndex(i32 index)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_LANGUAGE, index);

        // Apply language change immediately
        ITF_LANGUAGE newLanguage = static_cast<ITF_LANGUAGE>(m_gameOptionManager.getIntListOptionValue(OPTION_LANGUAGE));
        if (LOCALISATIONMANAGER && LOCALISATIONMANAGER->getCurrentLanguage() != newLanguage)
        {
            LOG("[Language] Changing language from %d to %d", LOCALISATIONMANAGER->getCurrentLanguage(), newLanguage);
            LOCALISATIONMANAGER->changeLanguage(newLanguage);
        }
    }

    const char* Ray_GameManager::getLanguageDisplayName(i32 index) const
    {
        // Use ITF_LANGUAGE enum values
        switch (index)
        {
        case ITF_LANGUAGE_ENGLISH:              return "English";
        case ITF_LANGUAGE_FRENCH:               return "French";
        case ITF_LANGUAGE_ITALIAN:              return "Italian";
        case ITF_LANGUAGE_GERMAN:               return "German";
        case ITF_LANGUAGE_SPANISH:              return "Spanish";
        case ITF_LANGUAGE_PORTUGUESE:           return "Portuguese";
        case ITF_LANGUAGE_JAPANESE:             return "Japanese";
        case ITF_LANGUAGE_TRADITIONALCHINESE:   return "Traditional Chinese";
        case ITF_LANGUAGE_SIMPLIFIEDCHINESE:    return "Simplified Chinese";
        case ITF_LANGUAGE_RUSSIAN:              return "Russian";
        case ITF_LANGUAGE_POLISH:               return "Polish";
        case ITF_LANGUAGE_KOREAN:               return "Korean";
        case ITF_LANGUAGE_DUTCH:                return "Dutch";
        case ITF_LANGUAGE_CZECH:                return "Czech";
        case ITF_LANGUAGE_HUNGARIAN:            return "Hungarian";
        default: return "Unknown";
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // OPTION MENU - ACCESSIBILITY OPTIONS
    ///////////////////////////////////////////////////////////////////////////////////////////

    i32 Ray_GameManager::getStartWithHeartIndex() const
    {
        return m_gameOptionManager.getListOptionIndex(OPTION_START_WITH_HEART);
    }

    void Ray_GameManager::setStartWithHeartIndex(i32 index)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_START_WITH_HEART, index);
    }

    const char* Ray_GameManager::getStartWithHeartDisplayName(i32 index) const
    {
        switch (index)
        {
        case HealthModifier_Default:      return "No";
        case HealthModifier_RedHeart:     return "Red";
        case HealthModifier_GoldenHeart:  return "Golden";
        case HealthModifier_DiamondHeart: return "Diamond";
        default: return "Unknown";
        }
    }

    i32 Ray_GameManager::getStartWithHeartLineId(i32 index) const
    {
        switch (index)
        {
            case HealthModifier_Default:      return LINEID_HEART_NONE;
            case HealthModifier_RedHeart:     return LINEID_HEART_RED;
            case HealthModifier_GoldenHeart:  return LINEID_HEART_GOLDEN;
            case HealthModifier_DiamondHeart: return LINEID_HEART_DIAMOND;
            default:                          return LINEID_HEART_NONE;
        }
    }

    i32 Ray_GameManager::getRunButtonMode() const
    {
        return m_gameOptionManager.getIntListOptionValue(OPTION_RUN_BUTTON);
    }

    void Ray_GameManager::setRunButtonMode(i32 mode)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_RUN_BUTTON, mode);
    }

    const char* Ray_GameManager::getRunButtonDisplayName(i32 index) const
    {
        switch (index)
        {
        case RunButtonMode_Hold: return "Hold";
        case RunButtonMode_Toggle: return "Toggle";
        default: return "Unknown";
        }
    }

    i32 Ray_GameManager::getRunButtonLineId(i32 index) const
    {
        return (index == RunButtonMode_Toggle) ? LINEID_RUN_TOGGLE : LINEID_RUN_HOLD;
    }

    bbool Ray_GameManager::isMurfyAssistEnabled() const
    {
        return m_gameOptionManager.getBoolOption(OPTION_MURFY_ASSIST);
    }

    void Ray_GameManager::setMurfyAssist(bbool enabled)
    {
        m_gameOptionManager.setBoolOption(OPTION_MURFY_ASSIST, enabled);
    }

    i32 Ray_GameManager::getVibrationMode() const
    {
        return m_gameOptionManager.getListOptionIndex(OPTION_VIBRATIONS);
    }

    void Ray_GameManager::setVibrationMode(i32 mode)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_VIBRATIONS, mode);
    }

    const char* Ray_GameManager::getVibrationDisplayName(i32 index) const
    {
        switch (index)
        {
        case VibrationMode_Off: return "Off";
        case VibrationMode_On: return "On";
        default: return "Unknown";
        }
    }

    u32 Ray_GameManager::getVibrationLineId(i32 index) const
    {
        switch (index)
        {
        case VibrationMode_Off: return LINEID_VIBRATION_OFF;
        case VibrationMode_On: return LINEID_VIBRATION_ON;
        default: return 4294967295;
        }
    }

    bbool Ray_GameManager::areVibrationsEnabled() const
    {
        return m_gameOptionManager.getIntListOptionValue(OPTION_VIBRATIONS, VibrationMode_Off) == VibrationMode_On;
    }

    void Ray_GameManager::setVibrations(bbool enabled)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_VIBRATIONS, enabled ? VibrationMode_On : VibrationMode_Off);
    }

#if defined(ITF_WINDOWS)
    bbool Ray_GameManager::IsKeyboardControllerSharingEnabled() const
    {
        return m_gameOptionManager.getBoolOption(OPTION_PC_KEYBOARD_CONTROLLER_SHARING);
    }

    void Ray_GameManager::setKeyboardControllerSharing(bbool enabled)
    {
        m_gameOptionManager.setListOptionIndex(OPTION_PC_KEYBOARD_CONTROLLER_SHARING, enabled);
    }
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////
    // OPTION MENU - SOUND OPTIONS
    ///////////////////////////////////////////////////////////////////////////////////////////

    f32 Ray_GameManager::getMasterVolume() const
    {
        return m_gameOptionManager.getFloatOption(OPTION_MASTER_VOLUME, 1.0f);
    }

    void Ray_GameManager::setMasterVolume(f32 volume)
    {
        volume = std::max(volume, 0.0f);
        volume = std::min(volume, 1.0f);
        m_gameOptionManager.setFloatOption(OPTION_MASTER_VOLUME, volume);
    }

    f32 Ray_GameManager::getMusicVolume() const
    {
        return m_gameOptionManager.getFloatOption(OPTION_MUSIC_VOLUME, 1.0f);
    }

    void Ray_GameManager::setMusicVolume(f32 volume)
    {
        volume = std::max(volume, 0.0f);
        volume = std::min(volume, 1.0f);
        m_gameOptionManager.setFloatOption(OPTION_MUSIC_VOLUME, volume);
    }

    f32 Ray_GameManager::getSFXVolume() const
    {
        return m_gameOptionManager.getFloatOption(OPTION_SFX_VOLUME, 1.0f);
    }

    void Ray_GameManager::setSFXVolume(f32 volume)
    {
        volume = std::max(volume, 0.0f);
        volume = std::min(volume, 1.0f);
        m_gameOptionManager.setFloatOption(OPTION_SFX_VOLUME, volume);
    }

    f32 Ray_GameManager::getIntensity() const
    {
        return m_gameOptionManager.getFloatOption(OPTION_INTENSITY, 1.0f);
    }

    void Ray_GameManager::setIntensity(f32 intensity)
    {
        intensity = std::max(intensity, 0.0f);
        intensity = std::min(intensity, 1.0f);
        m_gameOptionManager.setFloatOption(OPTION_INTENSITY, intensity);
    }

    void Ray_GameManager::applyDisplayOptions()
    {
    }

    void Ray_GameManager::applyLanguageOption()
    {
        ITF_LANGUAGE language = static_cast<ITF_LANGUAGE>(m_gameOptionManager.getIntListOptionValue(OPTION_LANGUAGE));
        if (LOCALISATIONMANAGER && LOCALISATIONMANAGER->getCurrentLanguage() != language)
        {
            LOG("[Language] Applying saved language: %d", language);
            LOCALISATIONMANAGER->changeLanguage(language);
        }
    }

    void Ray_GameManager::applyStartWithHeartOption()
    {
        i32 heartIndex = getStartWithHeartIndex();
        EHealthModifier heartModifier = getHealthModifier();
        LOG("[OptionMenu] Start With Heart: %s (index: %d, modifier: %d)", getStartWithHeartDisplayName(heartIndex), heartIndex, heartModifier);
    }

    void Ray_GameManager::applyRunButtonOption()
    {
        i32 runMode = getRunButtonMode();
        LOG("[OptionMenu] Run Button Mode: %s (value: %d)", getRunButtonDisplayName(runMode), runMode);
    }

#if defined(ITF_WINDOWS)
    void Ray_GameManager::applyPCKeyboardControllerSharingOption()
    {
        LOG("[OptionMenu] input mode Mode: %s", IsKeyboardControllerSharingEnabled() ? "ON" : "OFF");
    }
#endif

    void Ray_GameManager::applyMurfyAssistOption()
    {
        bbool murfyAssist = isMurfyAssistEnabled();
        LOG("[OptionMenu] Murfy's Assist: %s", murfyAssist ? "ON" : "OFF");
        // TODO: Apply to gameplay when needed
    }

    void Ray_GameManager::applyVibrationOption()
    {
        i32 vibrationMode = getVibrationMode();
        bbool vibrations = (vibrationMode == VibrationMode_On);
        LOG("[OptionMenu] Vibrations: %s", vibrations ? "ON" : "OFF");
        // TODO: Apply to INPUT_ADAPTER when needed
        // INPUT_ADAPTER->setVibrationEnabled(vibrations);
    }

    void Ray_GameManager::applyMasterVolumeOption()
    {
        f32 masterVol = getMasterVolume();
        LOG("[OptionMenu] Master Volume: %.2f (%.0f%%)", masterVol, masterVol * 100.0f);
#if defined(ITF_SUPPORT_WWISE)
        Adapter_AudioMiddleware *audioAdapter = Adapter_AudioMiddleware::getptr();
        if (audioAdapter)
            audioAdapter->setMasterVolume(Volume(masterVol, false));
#endif
    }

    void Ray_GameManager::applyMusicVolumeOption()
    {
        f32 musicVol = getMusicVolume();
        LOG("[OptionMenu] Music Volume: %.2f (%.0f%%)", musicVol, musicVol * 100.0f);
#if defined(ITF_SUPPORT_WWISE)
        Adapter_AudioMiddleware *audioAdapter = Adapter_AudioMiddleware::getptr();
        if (audioAdapter)
        {
            audioAdapter->setBusVolume(SOUND_BUS_MUSIC, Volume(musicVol, false), 0.0f);
        }
#endif
    }

    void Ray_GameManager::applySFXVolumeOption()
    {
        f32 sfxVol = getSFXVolume();
        LOG("[OptionMenu] SFX Volume: %.2f (%.0f%%)", sfxVol, sfxVol * 100.0f);
#if defined(ITF_SUPPORT_WWISE)
        Adapter_AudioMiddleware *audioAdapter = Adapter_AudioMiddleware::getptr();
        if (audioAdapter)
        {
            audioAdapter->setBusVolume(StringID("SFX"), Volume(sfxVol, false), 0.0f);
        }
#endif
    }

    void Ray_GameManager::applyIntensityOption()
    {
        f32 intensity = getIntensity();
        LOG("[OptionMenu] Intensity: %.2f (%.0f%%)", intensity, intensity * 100.0f);
        if (PADRUMBLEMANAGER)
        {
            PADRUMBLEMANAGER->setIntensityMultiplier(intensity);
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    // Save/Load Options
    ///////////////////////////////////////////////////////////////////////////////////////////

    void Ray_GameManager::saveGameOptions()
    {
        if (!m_gameOptionPersistence)
        {
            LOG("[GameOptions] ERROR: Persistence not initialized!");
            return;
        }

        LOG("[GameOptions] Starting save operation...");
        m_gameOptionPersistence->startSaveOptions(0, onSaveOptionsComplete);
    }

    void Ray_GameManager::loadGameOptions()
    {
        if (!m_gameOptionPersistence)
        {
            LOG("[GameOptions] ERROR: Persistence not initialized!");
            return;
        }

        LOG("[GameOptions] Starting load operation...");
        m_gameOptionPersistence->startLoadOptions(0, onLoadOptionsComplete);
    }

    void Ray_GameManager::onSaveOptionsComplete(Ray_GameOptionPersistence::Result result)
    {
        if (result == Ray_GameOptionPersistence::Result_SaveSuccess)
        {
            LOG("[GameOptions] Save completed successfully");
        }
        else if (result == Ray_GameOptionPersistence::Result_SaveFailed)
        {
            LOG("[GameOptions] ERROR: Save failed!");
        }
    }

    void Ray_GameManager::onLoadOptionsComplete(Ray_GameOptionPersistence::Result result)
    {
        switch (result)
        {
        case Ray_GameOptionPersistence::Result_LoadSuccess:
            LOG("[GameOptions] Load completed successfully");
            break;
        case Ray_GameOptionPersistence::Result_LoadFailed:
            LOG("[GameOptions] Load failed - using default options");
            break;
        case Ray_GameOptionPersistence::Result_LoadNotFound:
            LOG("[GameOptions] No saved options found - using defaults");
            break;
        default:
            LOG("[GameOptions] Unknown result - applying safe defaults");
            break;
        }

        auto* gm = RAY_GAMEMANAGER;
        gm->applyDisplayOptions();
        gm->applyMasterVolumeOption();
        gm->applyMusicVolumeOption();
        gm->applySFXVolumeOption();
#if defined(ITF_WINDOWS)
        gm->applyPCKeyboardControllerSharingOption();
#endif

        if (result == Ray_GameOptionPersistence::Result_LoadSuccess)
        {
            gm->applyLanguageOption();
            gm->applyStartWithHeartOption();
            gm->applyRunButtonOption();
            gm->applyMurfyAssistOption();
            gm->applyVibrationOption();
            gm->applyIntensityOption();
        }
        if (gm->m_onGameSettingLoaded)
        {
            auto cb = gm->m_onGameSettingLoaded;
            gm->m_onGameSettingLoaded = nullptr;
            cb();
        }
    }
} //namespace ITF
