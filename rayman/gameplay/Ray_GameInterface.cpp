#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMEINTERFACE_H_
#include "rayman/gameplay/Ray_GameInterface.h"
#endif //_ITF_RAY_GAMEINTERFACE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_GAMEPLAYFACTORYFILLER_H_
#include "gameplay/GameplayFactoryFiller.h"
#endif //_ITF_GAMEPLAYFACTORYFILLER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_FRIEZEPLUGINREGISTRY_H_
#include "engine/display/Frieze/FriezePluginRegistry.h"
#endif //_ITF_FRIEZEPLUGINREGISTRY_H_

/********** frieze plugins***************/
#ifndef _ITF_RAY_DJEMBEFRIEZEPLUGIN_H_
#include "rayman/gameplay/Components/Misc/Ray_DjembeFriezePlugin.h"
#endif


/********** persistent game data***************/
#ifndef _ITF_RAY_PERSISTENTGAMEDATA_H_
#include "rayman/gameplay/Ray_PersistentGameData.h"
#endif //_ITF_RAY_PERSISTENTGAMEDATA_H_



/********* GAME-SPECIFIC MANAGERS ***********/

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AvoidanceManager.h"
#endif //_ITF_RAY_AVOIDANCEMANAGER_H_

#ifndef _ITF_RAY_FIGHTMANAGER_H_
#include "rayman/gameplay/Managers/Ray_FightManager.h"
#endif //_ITF_RAY_FIGHTMANAGER_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_RAY_CAMERACONTROLLERMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CameraControllerManager.h"
#endif //_ITF_RAY_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_RAY_BLACKSWARMZONEMANAGER_H_
#include "Rayman/gameplay/Managers/Ray_BlackSwarmZoneManager.h"
#endif //_ITF_RAY_BLACKSWARMZONEMANAGER_H_

#ifndef _ITF_RAY_AIBUBBLEPRIZEFIXEDBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/BubblePrize/Ray_AIBubblePrizeFixedBehavior.h"
#endif //_ITF_RAY_AIBUBBLEPRIZEFIXEDBEHAVIOR_H_

#ifndef _ITF_RAY_AIBUBBLEPRIZEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/BubblePrize/Ray_AIBubblePrizeBehavior.h"
#endif //_ITF_RAY_AIBUBBLEPRIZEBEHAVIOR_H_

#ifndef _ITF_RAY_AICAGEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Cage/Ray_AICageBehavior.h"
#endif //_ITF_RAY_AICAGEBEHAVIOR_H_

#ifndef _ITF_RAY_AICHEST2BEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Chest2/Ray_AIChest2Behavior.h"
#endif //_ITF_RAY_AICHEST2BEHAVIOR_H_

#ifndef _ITF_RAY_AICORNBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Corn/Ray_AICornBehavior.h"
#endif //_ITF_RAY_AICORNBEHAVIOR_H_

#ifndef _ITF_RAY_AIMEDALBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Medal/Ray_AIMedalBehavior.h"
#endif //_ITF_RAY_AIMEDALBEHAVIOR_H_

#ifndef __RAY_AIELECTOONBEHAVIOR_H__
#include "rayman/gameplay/AI/Behaviors/Electoon/Ray_AIElectoonBehavior.h"
#endif //__RAY_AIELECTOONBEHAVIOR_H__

#ifndef _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_GroundAIControllerComponent.h"
#endif //_ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCOSTUMECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMECOMPONENT_H_

#ifndef _ITF_RAY_CONCEPTGALLERYCOMPONENT_H_
#include "rayman/gameplay/Components/Gallery/Ray_ConceptGalleryComponent.h"
#endif //_ITF_RAY_CONCEPTGALLERYCOMPONENT_H_


#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeManagerComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_


#ifndef _ITF_RAY_PLAYERCOSTUMEPRICECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumePriceComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEPRICECOMPONENT_H_

#ifndef _ITF_RAYWMPANELCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_PanelComponent.h"
#endif //_ITF_RAYWMPANELCOMPONENT_H_

#ifndef _ITF_RAY_CHANGEROOMDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_ChangeRoomDoorComponent.h"
#endif //_ITF_RAY_CHANGEROOMDOORCOMPONENT_H_

#ifndef _ITF_RAY_CHARACTERDBUGGERCOMPONENT_H_
#include "rayman/gameplay/components/Common/Ray_CharacterDebuggerComponent.h"
#endif //_ITF_RAY_CHARACTERDBUGGERCOMPONENT_H_

#ifndef _ITF_RAY_JANODAICOMPONENT_H__
#include "rayman/gameplay/Components/AI/Ray_JanodAIComponent.h"
#endif //_ITF_RAY_JANODAICOMPONENT_H__

#ifndef _ITF_RAY_CAGEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_CageAIComponent.h"
#endif //_ITF_RAY_CAGEAICOMPONENT_H_

#ifndef _ITF_RAY_BUBBLESPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleSpawnerComponent.h"
#endif //_ITF_RAY_BUBBLESPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_AILUMS2COMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AILums2Component.h"
#endif //_ITF_RAY_AILUMS2COMPONENT_H_

#ifndef _ITF_RAY_ELECTOONTEETHCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_ElectoonTeethComponent.h"
#endif //_ITF_RAY_ELECTOONTEETHCOMPONENT_H_

//

#ifndef _ITF_RAY_AIBOUNCEROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIBounceRoamingBehavior.h"
#endif //_ITF_RAY_AIBOUNCEROAMINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Fruit/Ray_AIFruitRoamingBehavior.h"
#endif //_ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

#ifndef _ITF_RAY_FRUITTRAPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/FruitTrap/Ray_AIFruitTrapBehavior.h"
#endif //_ITF_RAY_FRUITTRAPBEHAVIOR_H_

#ifndef _ITF_RAY_AIJANODROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Janod/Ray_AIJanodRoamingBehavior.h"
#endif //_ITF_RAY_AIJANODROAMINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIJANODTWEENEDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Janod/Ray_AIJanodTweenedBehavior.h"
#endif //_ITF_RAY_AIJANODTWEENEDBEHAVIOR_H_

#ifndef _ITF_RAY_LIVINGSTONEAICOMPONENT2_H_
#include "rayman/gameplay/Components/AI/Ray_LivingStoneAIComponent2.h"
#endif //_ITF_RAY_LIVINGSTONEAICOMPONENT2_H_

#ifndef _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundEnemyAIComponent.h"
#endif //_ITF_RAY_AIGROUNDENEMYCOMPONENT_H_

#ifndef _ITF_RAY_DARKBIRDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_DarkBirdAIComponent.h"
#endif //_ITF_RAY_DARKBIRDAICOMPONENT_H_

#ifndef _ITF_RAY_AIPLATFORMENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformEnemyAIComponent.h"
#endif //_ITF_RAY_AIPLATFORMENEMYCOMPONENT_H_

#ifndef _ITF_RAY_LIVINGSTONEHATAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LivingstoneHatAIComponent.h"
#endif //_ITF_RAY_LIVINGSTONEHATAICOMPONENT_H_

#ifndef _ITF_RAY_DOORTELEPORTCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_DoorTeleportComponent.h"
#endif //_ITF_RAY_DOORTELEPORTCOMPONENT_H_

#ifndef _ITF_RAY_LANDOFTHEDEADTELEPORTCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_LandOfTheDeadTeleport.h"
#endif //_ITF_RAY_LANDOFTHEDEADTELEPORTCOMPONENT_H_

#ifndef _ITF_RAY_ENTERDOORCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_EnterDoorComponent.h"
#endif //_ITF_RAY_ENTERDOORCOMPONENT_H_

#ifndef _ITF_RAY_STARGATECOMPONENT_H_
#include "rayman/gameplay/components/Misc/Stargate/Ray_StargateComponent.h"
#endif //_ITF_RAY_STARGATECOMPONENT_H_

#ifndef _ITF_RAY_STARGATENODECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_StargateNodeComponent.h"
#endif //_ITF_RAY_STARGATENODECOMPONENT_H_

#ifndef _ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceStoneComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_

#ifndef _ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceVisualComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_

#ifndef _ITF_RAY_ACTIVATIONSEQUENCETRIGGERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceTriggerComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCETRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_SCALETUNNELCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/ChangeScale/Ray_ScaleTunnelComponent.h"
#endif //_ITF_RAY_SCALETUNNELCOMPONENT_H_

#ifndef _ITF_RAY_SCALEFUNNELCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/ChangeScale/Ray_ScaleFunnelComponent.h"
#endif //_ITF_RAY_SCALEFUNNELCOMPONENT_H_

#ifndef _ITF_RAY_DYNAMICFOGCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_DynamicFogComponent.h"
#endif //_ITF_RAY_DYNAMICFOGCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERDEADSOUL2DCOMPONENT_H_
#include "rayman/gameplay/Components/player/Ray_PlayerDeadSoul2DComponent.h"
#endif //_ITF_RAY_PLAYERDEADSOUL2DCOMPONENT_H_

#ifndef _ITF_RAY_PLAYEROFFSCREENICONCOMPONENT_H_
#include "rayman/gameplay/Components/player/Ray_PlayerOffScreenIconComponent.h"
#endif //_ITF_RAY_PLAYEROFFSCREENICONCOMPONENT_H_

#ifndef _ITF_RAY_WM_CONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Controller/Ray_WM_ControllerComponent.h"
#endif //_ITF_RAY_WM_CONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_RAY_PLATFORMERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_PlatformerCameraComponent.h"
#endif //_ITF_RAY_PLATFORMERCAMERACOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterActorParameterComponent.h"
#endif //_ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterSpawnerComponent.h"
#endif //_ITF_RAY_SHOOTERSPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERSTIMCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterStimComponent.h"
#endif //_ITF_RAY_SHOOTERSTIMCOMPONENT_H_

#ifndef _ITF_RAY_SWINGCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_SwingComponent.h"
#endif //_ITF_RAY_SWINGCOMPONENT_H_

#ifndef _ITF_RAY_SWINGROPECOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_SwingRopeComponent.h"
#endif //_ITF_RAY_SWINGROPECOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERTURRETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterTurretAIComponent.h"
#endif //_ITF_RAY_SHOOTERTURRETAICOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERPIRATESHIPAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterPirateShipAIComponent.h"
#endif //_ITF_RAY_SHOOTERPIRATESHIPAICOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERGARDIANAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterGardianAIComponent.h"
#endif //_ITF_RAY_SHOOTERGARDIANAICOMPONENT_H_

#ifndef _ITF_RAY_HUNTERBOSSAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_HunterBossAIComponent.h"
#endif //_ITF_RAY_HUNTERBOSSAICOMPONENT_H_

#ifndef _ITF_RAY_HUNTERBOSSNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_HunterBossNodeComponent.h"
#endif //_ITF_RAY_HUNTERBOSSNODECOMPONENT_H_

#ifndef _ITF_RAY_CONDITIONALTRIGGERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ConditionalTriggerComponent.h"
#endif //_ITF_RAY_CONDITIONALTRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_HUNTERHOLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_HunterHoleAIComponent.h"
#endif //_ITF_RAY_HUNTERHOLEAICOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterAIComponent.h"
#endif //_ITF_RAY_SHOOTERAICOMPONENT_H_

#ifndef _ITF_RAY_SKULLCOINCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SkullCoinComponent.h"
#endif //_ITF_RAY_SKULLCOINCOMPONENT_H_

#ifndef _ITF_RAY_SPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SpawnerComponent.h"
#endif //_ITF_RAY_SPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERACOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraModifierComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_RAY_SIMPLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SimpleAIComponent.h"
#endif //_ITF_RAY_SIMPLEAICOMPONENT_H_

#ifndef _ITF_RAY_CYCLOPEUSAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_CyclopeusAIComponent.h"
#endif //_ITF_RAY_CYCLOPEUSAICOMPONENT_H_

#ifndef _ITF_RAY_DARKBIRDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_DarkBirdAIComponent.h"
#endif //_ITF_RAY_DARKBIRDAICOMPONENT_H_

#ifndef _ITF_RAY_AIFRUITCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FRUitAIComponent.h"
#endif //_ITF_RAY_AIFRUITCOMPONENT_H_

#ifndef _ITF_RAY_DARKTOONAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_DarkToonAIComponent.h"
#endif //_ITF_RAY_DARKTOONAICOMPONENT_H_

#ifndef _ITF_RAY_DARKTOONTRAPAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_DarktoonTrapAIComponent.h"
#endif //_ITF_RAY_DARKTOONTRAPAICOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Misc/Ray_ShooterCheckPointComponent.h"
#endif //_ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Misc/Ray_ShooterDoorComponent.h"
#endif //_ITF_RAY_SHOOTERDOORCOMPONENT_H_

#ifndef _ITF_RAY_REWARDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_RewardAIComponent.h"
#endif //_ITF_RAY_REWARDAICOMPONENT_H_

#ifndef _ITF_RAY_SCENESETTINGSCOMPONENT_H_
#include "rayman/gameplay/Components/Common/Ray_SceneSettingsComponent.h"
#endif //_ITF_RAY_SCENESETTINGSCOMPONENT_H_

#ifndef _ITF_RAY_REDWIZARDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_RedWizardAIComponent.h"
#endif //_ITF_RAY_REDWIZARDAICOMPONENT_H_

#ifndef _ITF_RAY_FRIENDLYAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FriendlyAIComponent.h"
#endif //_ITF_RAY_FRIENDLYAICOMPONENT_H_

#ifndef _ITF_RAY_AVOIDANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AvoidanceComponent.h"
#endif //_ITF_RAY_AVOIDANCECOMPONENT_H_

#ifndef _ITF_RAY_DANCEDOORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceDoorComponent.h"
#endif //_ITF_RAY_DANCEDOORCOMPONENT_H_

#ifndef _ITF_RAY_DANCESTONECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceStoneComponent.h"
#endif //_ITF_RAY_DANCESTONECOMPONENT_H_

#ifndef _ITF_RAY_DANCEEYECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DanceEyeComponent.h"
#endif //_ITF_RAY_DANCEEYECOMPONENT_H_

#ifndef _ITF_RAY_BULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_BulletAIComponent.h"
#endif //_ITF_RAY_BULLETAICOMPONENT_H_

#ifndef _ITF_RAY_SEEKINGBULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_SeekingBulletAIComponent.h"
#endif //_ITF_RAY_SEEKINGBULLETAICOMPONENT_H_

#ifndef _ITF_RAY_WALKINGBULLETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Bullet/Ray_WalkingBulletAIComponent.h"
#endif //_ITF_RAY_WALKINGBULLETAICOMPONENT_H_

#ifndef _ITF_RAY_BREAKABLECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableAIComponent.h"
#endif //_ITF_RAY_BREAKABLECOMPONENT_H_

#ifndef _ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableStackManagerAIComponent.h"
#endif //_ITF_RAY_BREAKABLESTACKMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableStackElementAIComponent.h"
#endif //_ITF_RAY_BREAKABLESTACKELEMENTAICOMPONENT_H_

#ifndef _ITF_RAY_BABYPIRANHACOMPONENT_H_
#include "rayman/gameplay/Components/Piranha/Ray_BabyPiranhaComponent.h"
#endif //_ITF_RAY_BABYPIRANHACOMPONENT_H_

#ifndef _ITF_RAY_BABYPIRANHAAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BabyPiranhaAIComponent.h"
#endif //_ITF_RAY_BABYPIRANHAAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSBUBOAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBuboAIComponent.h"
#endif //_ITF_RAY_BOSSBUBOAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSMAPENDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossMapEndAIComponent.h"
#endif //_ITF_RAY_BOSSMAPENDAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSPLANTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantAIComponent.h"
#endif //_ITF_RAY_BOSSPLANTAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSPLANTNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantNodeComponent.h"
#endif //_ITF_RAY_BOSSPLANTNODECOMPONENT_H_

#ifndef _ITF_RAY_BOSSPLANTARENAAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossPlantArenaAIComponent.h"
#endif //_ITF_RAY_BOSSPLANTARENAAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSBIRDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBirdAIComponent.h"
#endif //_ITF_RAY_BOSSBIRDAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossBirdPawnAIComponent.h"
#endif //_ITF_RAY_BOSSBIRDPAWNAICOMPONENT_H_

#ifndef _ITF_RAY_BIGMAMAAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BigMamaAIComponent.h"
#endif //_ITF_RAY_BIGMAMAAICOMPONENT_H_

#ifndef _ITF_RAY_MAMAEYESAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_MamaEyesAIComponent.h"
#endif //_ITF_RAY_MAMAEYESAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSMORAYAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayAIComponent.h"
#endif //_ITF_RAY_BOSSMORAYAICOMPONENT_H_

#ifndef _ITF_RAY_BOSSMORAYNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayNodeComponent.h"
#endif //_ITF_RAY_BOSSMORAYNODECOMPONENT_H_

#ifndef _ITF_RAY_BOSSCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_BossCameraComponent.h"
#endif //_ITF_RAY_BOSSCAMERACOMPONENT_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_RAY_CHECKPOINTCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CheckpointComponent.h"
#endif //_ITF_RAY_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

#ifndef _ITF_RAY_TRIGGERBOUNCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_TriggerBounceComponent.h"
#endif //_ITF_RAY_TRIGGERBOUNCECOMPONENT_H_

#ifndef _ITF_RAY_TUTORIALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_TutorialComponent.h"
#endif //_ITF_RAY_TUTORIALCOMPONENT_H_

#ifndef _ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WMContentUnlockComponent.h"
#endif //_ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_

#ifndef _ITF_RAY_LOADINGNOTIFICATIONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_LoadingNotificationComponent.h"
#endif //_ITF_RAY_LOADINGNOTIFICATIONCOMPONENT_H_

#ifndef _ITF_RAY_SKIPLEVELCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SkipLevelComponent.h"
#endif //_ITF_RAY_SKIPLEVELCOMPONENT_H_

#ifndef _ITF_RAY_LEVELTITLECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_LevelTitleComponent.h"
#endif //_ITF_RAY_LEVELTITLECOMPONENT_H_

#ifndef _ITF_RAY_WATERPERTURBATIONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WaterPerturbationComponent.h"
#endif //_ITF_RAY_WATERPERTURBATIONCOMPONENT_H_

#ifndef _ITF_RAY_DARKARENADOORCOMPONENT_H_
#include "rayman/gameplay/components/Misc/Ray_DarkArenaDoorComponent.h"
#endif //_ITF_RAY_DARKARENADOORCOMPONENT_H_

#ifndef _ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DarktoonTrapHoleComponent.h"
#endif //_ITF_RAY_DARKTOONTRAPHOLECOMPONENT_H_

#ifndef _ITF_RAY_DISPENSERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DispenserComponent.h"
#endif //_ITF_RAY_DISPENSERCOMPONENT_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_RAY_PLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformAIComponent.h"
#endif //_ITF_RAY_PLATFORMAICOMPONENT_H_

#ifndef _ITF_RAY_CAMERALIMITERCOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_CameraLimiterComponent.h"
#endif //_ITF_RAY_CAMERALIMITERCOMPONENT_H_

#ifndef _ITF_RAY_PLATFORMTREECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_PlatformTreeComponent.h"
#endif //_ITF_RAY_PLATFORMTREECOMPONENT_H_

#ifndef _ITF_RAY_PUSHBUTTONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_PushButtonComponent.h"
#endif //_ITF_RAY_PUSHBUTTONCOMPONENT_H_

#ifndef _ITF_RAY_PAUSEACTORCOMPONENT_H_
#include "rayman/gameplay/UDC/Ray_PauseActorComponent.h"
#endif //_ITF_RAY_PAUSEACTORCOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEPRIZEBUMPERCOMPONENT_H_
#include "rayman/gameplay/Components/BubblePrize/Ray_BubblePrizeBumperComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZEBUMPERCOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEPRIZEHUBCOMPONENT_H_
#include "rayman/gameplay/Components/BubblePrize/Ray_BubblePrizeHubComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZEHUBCOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEPRIZEPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/BubblePrize/Ray_BubblePrizePlatformComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZEPLATFORMCOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEPRIZESPAWNERCOMPONENT_H_
#include "rayman/gameplay/components/BubblePrize/Ray_BubblePrizeSpawnerComponent.h"
#endif //_ITF_RAY_BUBBLEPRIZESPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_BULLETLAUNCHERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_BulletLauncherComponent.h"
#endif //_ITF_RAY_BULLETLAUNCHERCOMPONENT_H_

#ifndef _ITF_RAY_EYEANIMATIONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_EyeAnimationComponent.h"
#endif //_ITF_RAY_EYEANIMATIONCOMPONENT_H_

#ifndef _ITF_RAY_GEYSERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_GeyserComponent.h"
#endif //_ITF_RAY_GEYSERCOMPONENT_H_

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_RAY_CUSTOMCHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CustomChangePageComponent.h"
#endif //_ITF_RAY_CUSTOMCHANGEPAGECOMPONENT_H_

#ifndef _ITF_RAY_CHANGEPAGEWITHANIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageWithAnimComponent.h"
#endif //_ITF_RAY_CHANGEPAGEWITHANIMCOMPONENT_H_

#ifndef _ITF_RAY_PUSHEDAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_PushedAIComponent.h"
#endif //_ITF_RAY_PUSHEDAICOMPONENT_H_

#ifndef _ITF_RAY_SWARMCHASEAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_SwarmChaseAIComponent.h"
#endif //_ITF_RAY_SWARMCHASEAICOMPONENT_H_

#ifndef _ITF_RAY_SWARMCHASENODECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SwarmChaseNodeComponent.h"
#endif //_ITF_RAY_SWARMCHASENODECOMPONENT_H_

#ifndef _ITF_RAY_FISHSWARMAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_FishSwarmAIComponent.h"
#endif //_ITF_RAY_FISHSWARMAICOMPONENT_H_

#ifndef _ITF_RAY_GOLEMAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_GolemAIComponent.h"
#endif //_ITF_RAY_GOLEMAICOMPONENT_H_

#ifndef _ITF_RAY_CRYSTALKINGAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_CrystalKingAIComponent.h"
#endif //_ITF_RAY_CRYSTALKINGAICOMPONENT_H_

#ifndef _ITF_RAY_STARGATEMANAGERAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_StargateManagerAIComponent.h"
#endif //_ITF_RAY_STARGATEMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_CREDITSMANAGERAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_CreditsManagerAIComponent.h"
#endif //_ITF_RAY_CREDITSMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_ELEVATORAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorAIComponent.h"
#endif //_ITF_Ray_ElevatorAIComponent_H_

#ifndef _ITF_RAY_ELEVATORWHEELAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorWheelAIComponent.h"
#endif //_ITF_RAY_ELEVATORWHEELAICOMPONENT_H_

#ifndef _ITF_RAY_ELEVATORMONSTERAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_ElevatorMonsterAIComponent.h"
#endif //_ITF_RAY_ELEVATORMONSTERAICOMPONENT_H_

#ifndef _ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LumMusicManagerAIComponent.h"
#endif //_ITF_RAY_LUMMUSICMANAGERAICOMPONENT_H_

#ifndef _ITF_RAY_WATERHANDSAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_WaterHandsAIComponent.h"
#endif //_ITF_RAY_WATERHANDSAICOMPONENT_H_

#ifndef _ITF_RAY_HANDSAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_HandsAIComponent.h"
#endif //_ITF_RAY_HANDSAICOMPONENT_H_

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

#ifndef _ITF_RAY_NAUTILUSAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_NautilusAIComponent.h"
#endif //_ITF_RAY_NAUTILUSAICOMPONENT_H_

#ifndef _ITF_RAY_STRINGWAVEGENERATORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StringWaveGeneratorComponent.h"
#endif //_ITF_RAY_STRINGWAVEGENERATORCOMPONENT_H_

#ifndef _ITF_RAY_SUBANCHORCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SubAnchorComponent.h"
#endif //_ITF_RAY_SUBANCHORCOMPONENT_H_

#ifndef _ITF_RAY_MUSICSCOREAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreAIComponent.h"
#endif //_ITF_RAY_MUSICSCOREAICOMPONENT_H_

#ifndef _ITF_RAY_MUSICSCORESNAPCOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_MusicScoreSnapComponent.h"
#endif //_ITF_RAY_MUSICSCORESNAPCOMPONENT_H_

#ifndef _ITF_RAY_FLUIDFALLAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_FluidFallAIComponent.h"
#endif //_ITF_RAY_FLUIDFALLAICOMPONENT_H_

#ifndef _ITF_RAY_FIREPATCHAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_FirePatchAIComponent.h"
#endif //_ITF_RAY_FIREPATCHAICOMPONENT_H_

#ifndef _ITF_RAY_JALAPENOKINGAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_JalapenoKingAiComponent.h"
#endif //_ITF_RAY_JALAPENOKINGAICOMPONENT_H_

#ifndef _ITF_RAY_FORCEFIELDCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ForceFieldComponent.h"
#endif //_ITF_RAY_FORCEFIELDCOMPONENT_H_

#ifndef _ITF_RAYMENUE3COMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MenuE3Component.h"
#endif //_ITF_RAYMENUE3COMPONENT_H_

#ifndef _ITF_RAYSCOREBOARDCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ScoreBoardComponent.h"
#endif //_ITF_RAYSCOREBOARDCOMPONENT_H_

#ifndef _ITF_RAY_STRINGWAVEFADERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StringWaveFaderComponent.h"
#endif //_ITF_RAY_STRINGWAVEFADERCOMPONENT_H_

#ifndef _ITF_RAY_BREAKABLECAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_BreakableCageComponent.h"
#endif //_ITF_RAY_BREAKABLECAGECOMPONENT_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERGARDIANMORAY_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterGardianMoray.h"
#endif //_ITF_RAY_SHOOTERGARDIANMORAY_H_

#ifndef _ITF_RAY_BLACKSWARMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmComponent.h"
#endif //_ITF_RAY_BLACKSWARMCOMPONENT_H_

#ifndef _ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmRepellerComponent.h"
#endif //_ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_

#ifndef _ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmSpawnerComponent.h"
#endif //_ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_PLATFORMCHAINAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformChainAIComponent.h"
#endif //_ITF_RAY_PLATFORMCHAINAICOMPONENT_H_


#ifndef _ITF_RAY_ELECTOONTARGETCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ElectoonTargetComponent.h"
#endif //_ITF_RAY_ELECTOONTARGETCOMPONENT_H_


#ifndef _ITF_RAY_LASTPLAYERTRIGGERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LastPlayerTriggerComponent.h"
#endif //_ITF_RAY_LASTPLAYERTRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_WAVEBALLCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WaveBallComponent.h"
#endif //_ITF_RAY_WAVEBALLCOMPONENT_H_

#ifndef _ITF_RAY_CAGMAPAMBIANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CageMapAmbianceComponent.h"
#endif //_ITF_RAY_CAGMAPAMBIANCECOMPONENT_H_

#ifndef _ITF_RAYSINKINGPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SinkingPlatformComponent.h"
#endif //_ITF_RAYSINKINGPLATFORMCOMPONENT_H_

#ifndef _ITF_RAYBASCULEPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_BasculePlatformComponent.h"
#endif //_ITF_RAYBASCULEPLATFORMCOMPONENT_H_

#ifndef _ITF_RAYWATERFLOATINGPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WaterFloatingPlatformComponent.h"
#endif //_ITF_RAYWATERFLOATINGPLATFORMCOMPONENT_H_

#ifndef _ITF_RAYCAGESTONECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CageStoneComponent.h"
#endif //_ITF_RAYCAGESTONECOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleAiComponent.h"
#endif //_ITF_RAY_BUBBLEAICOMPONENT_H_

#ifndef _ITF_RAY_ALTRANQUILOAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_alTranquiloAiComponent.h"
#endif //_ITF_RAY_ALTRANQUILOAICOMPONENT_H_

#ifndef _ITF_RAY_BEZIERTREEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BezierTreeAIComponent.h"
#endif //_ITF_RAY_BEZIERTREEAICOMPONENT_H_

#ifndef _ITF_RAY_SPIKYSHELLTRAPCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SpikyShellTrapComponent.h"
#endif //_ITF_RAY_SPIKYSHELLTRAPCOMPONENT_H_

#ifndef _ITF_RAY_OCEANSPIDERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_OceanSpiderAIComponent.h"
#endif //_ITF_RAY_OCEANSPIDERAICOMPONENT_H_

#ifndef _ITF_RAY_LIGHTNINGGENERATORAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_LightningGeneratorAIComponent.h"
#endif //_ITF_RAY_LIGHTNINGGENERATORAICOMPONENT_H_

#ifndef _ITF_RAY_ANGLERFISHAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AnglerFishAIComponent.h"
#endif //_ITF_RAY_ANGLERFISHAICOMPONENT_H_

#ifndef _ITF_RAY_ABYSSALHANDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AbyssalHandAIComponent.h"
#endif //_ITF_RAY_ABYSSALHANDAICOMPONENT_H_

#ifndef _ITF_RAY_ABYSSLIGHTCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AbyssLightComponent.h"
#endif //_ITF_RAY_ABYSSLIGHTCOMPONENT_H_

#ifndef _ITF_RAY_FIREFLYKRILLAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FireFlyKrillAIComponent.h"
#endif //_ITF_RAY_FIREFLYKRILLAICOMPONENT_H_

#ifndef _ITF_RAY_SCREENSIDETRAJECTORYFOLLOWERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ScreenSideTrajectoryFollowerComponent.h"
#endif //_ITF_RAY_SCREENSIDETRAJECTORYFOLLOWERCOMPONENT_H_

#ifndef _ITF_RAY_SEQUENCESWITCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SequenceSwitchComponent.h"
#endif //_ITF_RAY_SEQUENCESWITCHCOMPONENT_H_

#ifndef _ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FireFlyKrillSpawnerComponent.h"
#endif //_ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_LightningPatchAIComponent.h"
#endif //_ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_

#ifndef _ITF_RAY_BOULDERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BoulderAIComponent.h"
#endif //_ITF_RAY_BOULDERAICOMPONENT_H_

#ifndef _ITF_RAY_SCOREGUAGECOMPONENT_H_
#include "rayman/gameplay/Components/score/Ray_ScoreGuageComponent.h"
#endif //_ITF_RAY_SCOREGUAGECOMPONENT_H_

#ifndef _ITF_RAY_PLAYERSCORECOMPONENT_H_
#include "rayman/gameplay/Components/score/Ray_PlayerScoreComponent.h"
#endif //_ITF_RAY_PLAYERSCORECOMPONENT_H_

#ifndef _ITF_RAY_STONEMANAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_StoneManAIComponent.h"
#endif //_ITF_RAY_STONEMANAICOMPONENT_H_

#ifndef _ITF_RAY_ELECTOONSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ElectoonSpawnerComponent.h"
#endif //_ITF_RAY_ELECTOONSPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_SCORELUMAICOMPONENT_H_
#include "rayman/gameplay/Components/Lums/Ray_ScoreLumAIComponent.h"
#endif //_ITF_RAY_SCORELUMAICOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEDREAMERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleDreamerAIComponent.h"
#endif //_ITF_RAY_BUBBLEDREAMERAICOMPONENT_H_

#ifndef _ITF_RAY_SPIKYPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SpikyPlatformAIComponent.h"
#endif //_ITF_RAY_SPIKYPLATFORMAICOMPONENT_H_


/********* GAMEPLAY AI Behaviours ***********/

#ifndef _ITF_RAY_AILUMS2BEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AILums2Behavior.h"
#endif //_ITF_RAY_AILUMS2BEHAVIOR_H_

#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_AIScoreLumBehavior.h"
#endif //_ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneWindBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEWINDBEHAVIOR_H_

#ifndef _ITF_RAY_AILIVINGSTONESTATICBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneStaticBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONESTATICBEHAVIOR_H_

#ifndef _ITF_RAY_AINOTEPIAFBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/NotePiaf/Ray_AINotePiafBehavior.h"
#endif //_ITF_RAY_AINOTEPIAFBEHAVIOR_H_

#ifndef _ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterAttackBehavior.h"
#endif //_ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIHunterAttackBehavior.h"
#endif //_ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AISHOOTEREJECTEDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterEjectedBehavior.h"
#endif //_ITF_RAY_AISHOOTEREJECTEDBEHAVIOR_H_

#ifndef _ITF_RAY_AISHOOTERSTEPDAMAGEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterStepDamageBehavior.h"
#endif //_ITF_RAY_AISHOOTERSTEPDAMAGEBEHAVIOR_H_

#ifndef _ITF_RAY_AISHOOTERFLYINGBOMBBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterFlyingBombBehavior.h"
#endif //_ITF_RAY_AISHOOTERFLYINGBOMBBEHAVIOR_H_

#ifndef _ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterVacuumBehavior.h"
#endif //_ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AILIVINGSTONEROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/LivingStone/Ray_AILivingStoneRoamBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AISLEEPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AISleepBehavior.h"
#endif //_ITF_RAY_AISLEEPBEHAVIOR_H_

#ifndef _ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonTrapBehavior.h"
#endif //_ITF_RAY_AIDARKTOONTRAPBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRunAwayBehavior.h"
#endif //_ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIDARKTOONATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonAttackBehavior.h"
#endif //_ITF_RAY_AIDARKTOONATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonLaunchBehavior.h"
#endif //_ITF_RAY_AIDARKTOONLAUNCHBEHAVIOR_H_

#ifndef _ITF_RAY_AIREDWIZARDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIRedWizardRoamBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIDeathBehavior.h"
#endif //_ITF_RAY_AIDEATHBEHAVIOR_H_

#ifndef _ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIBubbleDeathBehavior.h"
#endif //_ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundFollowBehavior.h"
#endif //_ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_

#ifndef _ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIRedWizardDanceBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_

#ifndef _ITF_RAY_AIREDWIZARDPEDESTALBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIRedWizardPedestalBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDPEDESTALBEHAVIOR_H_

#ifndef _ITF_RAY_AINPC_WAITFORPLAYERBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIFriendly_WaitBehavior.h"
#endif //_ITF_RAY_AINPC_WAITFORPLAYERBEHAVIOR_H_

#ifndef _ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIFriendly_GoToTargetBehavior.h"
#endif //_ITF_RAY_AINPC_GOTOTARGETBEHAVIOR_H_

#ifndef _ITF_RAY_AIFRIENDLY_JUMPTOTARGETBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIFriendly_JumpToTargetBehavior.h"
#endif //_ITF_RAY_AIFRIENDLY_JUMPTOTARGETBEHAVIOR_H_

#ifndef _ITF_RAY_AINETWORKCOMPONENT_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkComponent.h"
#endif //_ITF_RAY_AINETWORKCOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDDARKTOONEDBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundDarktoonedBehavior.h"
#endif //_ITF_RAY_AIGROUNDDARKTOONEDBEHAVIOR_H_

#ifndef _ITF_RAY_AIRELICBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Relic/Ray_AIRelicBehavior.h"
#endif //_ITF_RAY_AIRELICBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERLOATINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterFloatingBehavior.h"
#endif //_ITF_RAY_AIWATERLOATINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIREDWIZARDWATERSWIMBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIRedWizardWaterSwimBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDWATERSWIMBEHAVIOR_H_

#ifndef _ITF_RAY_AIORANGEFLOATINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIOrangeFloatingBehavior.h"
#endif //_ITF_RAY_AIORANGEFLOATINGBEHAVIOR_H_

#ifndef _ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/LivingStone/Ray_AILivingStoneDrowningBehavior.h"
#endif //_ITF_RAY_AILIVINGSTONEDROWNINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/DarktoonusCyclopeus/Ray_AIDarktoonusCyclopeusBehavior.h"
#endif //_ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_

#ifndef _ITF_RAY_AIHITWALLBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIHitWallBehavior.h"
#endif //_ITF_RAY_AIHITWALLBEHAVIOR_H_

#ifndef _ITF_RAY_AIBONUSSKULLBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/SkullTeeth/Ray_AIBonusSkullBehavior.h"
#endif //_ITF_RAY_AIBONUSSKULLBEHAVIOR_H_

#ifndef _ITF_RAY_UTENSILTRAPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/UtensilTrap/Ray_AIUtensilTrapBehavior.h"
#endif //_ITF_RAY_UTENSILTRAPBEHAVIOR_H_

#ifndef _ITF_RAY_AIRESTOREANGLEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIRestoreAngleBehavior.h"
#endif //_ITF_RAY_AIRESTOREANGLEBEHAVIOR_H_

#ifndef _ITF_RAY_AIALINFERNOSTATICBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/AlInferno/Ray_AIAlInfernoStaticBehavior.h"
#endif //_ITF_RAY_AIALINFERNOSTATICBEHAVIOR_H_

#ifndef _ITF_RAY_AIALINFERNOPATROLBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/AlInferno/Ray_AIAlInfernoPatrolBehavior.h"
#endif //_ITF_RAY_AIALINFERNOPATROLBEHAVIOR_H_

#ifndef _ITF_RAY_AILOOKATBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AILookatBehavior.h"
#endif //_ITF_RAY_AILOOKATBEHAVIOR_H_

#ifndef _ITF_RAY_AIBLOWINGBIRDLOOKATBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/BlowingBird/Ray_AIBlowingBirdLookatBehavior.h"
#endif //_ITF_RAY_AIBLOWINGBIRDLOOKATBEHAVIOR_H_

#ifndef _RAY_AI_HARISSATOGGLEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Harissa/Ray_AIHarissaToggleBehavior.h"
#endif //_RAY_AI_HARISSATOGGLEBEHAVIOR_H_

#ifndef _RAY_AI_UMBRELLABEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Umbrella/Ray_AIUmbrellaBehavior.h"
#endif //_RAY_AI_UMBRELLABEHAVIOR_H_

#ifndef _ITF_RAY_AIALINFERNOWAITERBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/AlInferno/Ray_AIAlInfernoWaiterBehavior.h"
#endif //_ITF_RAY_AIALINFERNOWAITERBEHAVIOR_H_

#ifndef _ITF_RAY_AIBLOWFISHBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/BlowFish/Ray_AIBlowFishBehavior.h"
#endif //_ITF_RAY_AIBLOWFISHBEHAVIOR_H_

#ifndef _ITF_RAY_AIFISHBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/BlowFish/Ray_AIFishBehavior.h"
#endif //_ITF_RAY_AIFISHBEHAVIOR_H_

#ifndef _ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/OceanSpider/Ray_AIOceanSpiderBehavior.h"
#endif //_ITF_RAY_AIOCEANSPIDERBEHAVIOR_H_

#ifndef _ITF_RAY_AISWORDFISHBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/SwordFish/Ray_AISwordFishBehavior.h"
#endif //_ITF_RAY_AISWORDFISHBEHAVIOR_H_

#ifndef _ITF_RAY_AIROBOTPATROLBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Robot/Ray_AIRobotPatrolBehavior.h"
#endif //_ITF_RAY_AIROBOTPATROLBEHAVIOR_H_

#ifndef _ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Boulder/Ray_AIBoulderSentBackBehavior.h"
#endif //_ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_

#ifndef _ITF_RAY_AISTONEMANATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/StoneMan/Ray_AIStoneManAttackBehavior.h"
#endif //_ITF_RAY_AISTONEMANATTACKBEHAVIOR_H_

#ifndef _RAY_TEXTBUBBLEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Textbubble/Ray_TextBubbleBehavior.h"
#endif //_RAY_TEXTBUBBLEBEHAVIOR_H_


/********* GAMEPLAY AI Actions ***********/

#ifndef _ITF_RAY_AIBASICBULLETACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIBasicBulletAction.h"
#endif //_ITF_RAY_AIBASICBULLETACTION_H_

#ifndef _ITF_RAY_AIBLOWOFFACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIBlowOffAction.h"
#endif //_ITF_RAY_AIBLOWOFFACTION_H_

#ifndef _ITF_RAY_AIFLYIDLEACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIFlyIdleAction.h"
#endif //_ITF_RAY_AIFLYIDLEACTION_H_

#ifndef _ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIFollowBezierCurveAction.h"
#endif //_ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPUNCHACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPolylinePunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPOLYLINEPUNCHACTION_H_

#ifndef _ITF_RAY_AIGEYSERACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGeyserAction.h"
#endif //_ITF_RAY_AIGEYSERACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITPOLYLINEJUMPPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPolylineJumpPunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPOLYLINEJUMPPUNCHACTION_H_

#ifndef _ITF_RAY_AIDARKTOONIFICATIONACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIDarktoonificationAction.h"
#endif //_ITF_RAY_AIDARKTOONIFICATIONACTION_H_

#ifndef _ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Shooter/Ray_AIShooterLaunchBulletAction.h"
#endif //_ITF_RAY_AISHOOTERLAUNCHBULLETACTION_H_

#ifndef _ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Shooter/Ray_AIHunterLaunchBulletAction.h"
#endif //_ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_

#ifndef _ITF_RAY_AILIGHTNINGACTION_H_
#include "rayman/gameplay/ai/Actions/Shooter/Ray_AILightningAction.h"
#endif //_ITF_RAY_AILIGHTNINGACTION_H_

#ifndef _ITF_RAY_AIPERFORMLIGHTNINGACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AiPerformLightningAction.h"
#endif //_ITF_RAY_AIPERFORMLIGHTNINGACTION_H_

#ifndef _ITF_RAY_AICARTOONFALLACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AICartoonFallAction.h"
#endif //_ITF_RAY_AICARTOONFALLACTION_H_

#ifndef _ITF_RAY_AICORNFLOATACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AICornFloatAction.h"
#endif //_ITF_RAY_AICORNFLOATACTION_H_

#ifndef _ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveNormalHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVENORMALHITACTION_H_

#ifndef _ITF_RAY_AIGROUND_RECEIVEUPPERHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveUpperHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEUPPERHITACTION_H_

#ifndef _ITF_RAY_AIGROUND_RECEIVEINAIRHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveInAirHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEINAIRHITACTION_H_

#ifndef _ITF_RAY_AIRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIReceiveHitAction.h"
#endif //_ITF_RAY_AIRECEIVEHITACTION_H_

#ifndef _ITF_RAY_AIGROUND_RECEIVEEARTHQUAKEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveEarthquakeHitAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEEARTHQUAKEHITACTION_H_

#ifndef _ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIReceiveCameraEjectHitAction.h"
#endif //_ITF_RAY_AIRECEIVECAMERAEJECTHITACTION_H_

#ifndef _ITF_RAY_AISHOOTERRECEIVEHITACTION_H_
#include "rayman/gameplay/ai/Actions/Shooter/Ray_AIShooterReceiveHitAction.h"
#endif //_ITF_RAY_AISHOOTERRECEIVEHITACTION_H_

#ifndef _ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterCloseAttackBehavior.h"
#endif //_ITF_RAY_AISHOOTERCLOSEATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERFLOATACTION_H_
#include "rayman/gameplay/ai/Actions/Water/Ray_AIWaterFloatAction.h"
#endif //_ITF_RAY_AIWATERFLOATACTION_H_

#ifndef _ITF_RAY_AISHOOTERPROJECTILEACTION_H_
#include "rayman/gameplay/ai/Actions/Shooter/Ray_AIShooterProjectileAction.h"
#endif //_ITF_RAY_AISHOOTERPROJECTILEACTION_H_

#ifndef _ITF_RAY_AIEXPLODEACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIExplodeAction.h"
#endif //_ITF_RAY_AIEXPLODEACTION_H_

#ifndef _ITF_RAY_AIGROUND_RECEIVEHITLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGround_ReceiveHitLaunchBulletAction.h"
#endif //_ITF_RAY_AIGROUND_RECEIVEHITLAUNCHBULLETACTION_H_

// Anim Nodes
#ifndef _ITF_RAYANIMNODESWING_H_
#include "rayman/gameplay/AnimNodes/Ray_AnimNodeSwing.h"
#endif //_ITF_RAYANIMNODESWING_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_WORLDFILE_H_
#include "rayman/gameplay/Ray_WorldFile.h"
#endif //_ITF_RAY_WORLDFILE_H_

/********* Game screens ***********/
#ifndef _ITF_GAMESCREEN_INITIAL_H_
#include "gameplay/Managers/GameScreens/GameScreen_Initial.h"
#endif //_ITF_GAMESCREEN_INITIAL_H_

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_Gameplay.h"
#endif //_ITF_RAY_GAMESCREEN_GAMEPLAY_H_

#ifndef _ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_SplashScreens.h"
#endif //_ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_RAY_LEVELSTATS_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_LevelStats.h"
#endif //_ITF_RAY_LEVELSTATS_H_

#ifndef _ITF_RAY_CHECKPOINTSCORE_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_CheckpointScore.h"
#endif //_ITF_RAY_CHECKPOINTSCORE_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_RAY_FRIENDLYBTAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_FriendlyBTAIComponent.h"
#endif //_ITF_RAY_FRIENDLYBTAICOMPONENT_H_

#ifndef _ITF_RAY_DEADGUYBTAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_DeadGuyBTAIComponent.h"
#endif //_ITF_RAY_DEADGUYBTAICOMPONENT_H_

#ifndef _ITF_RAY_TALKINGHATAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingHatAIComponent.h"
#endif //_ITF_RAY_TALKINGHATAICOMPONENT_H_

#ifndef _ITF_RAY_MURPHYAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_MurphyAIComponent.h"
#endif //_ITF_RAY_MURPHYAICOMPONENT_H_

#ifndef _ITF_RAY_MURPHYASSISTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTAICOMPONENT_H_

#ifndef _ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistBubbleAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_

#ifndef _ITF_RAY_CHRONOAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_ChronoAIComponent.h"
#endif //_ITF_RAY_CHRONOAICOMPONENT_H_

#ifndef _ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_TimeAttackHUDTimerComponent.h"
#endif //_ITF_RAY_TIMEATTACKHUDTIMERCOMPONENT_H_

#ifndef _ITF_RAY_TIMEATTACKHUDRESULTSCOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_TimeAttackHUDResultsComponent.h"
#endif //_ITF_RAY_TIMEATTACKHUDRESULTSCOMPONENT_H_

#ifndef _ITF_RAY_TIMEATTACKFLAGCOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_TimeAttackFlagComponent.h"
#endif //_ITF_RAY_TIMEATTACKFLAGCOMPONENT_H_

#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingBubbleComponent.h"
#endif //_ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_RAY_TEENSYROBOTAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TeensyRobotAIComponent.h"
#endif //_ITF_RAY_TEENSYROBOTAICOMPONENT_H_

#ifndef _ITF_RAY_AIBTORDERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIBTOrderComponent.h"
#endif //_ITF_RAY_AIBTORDERCOMPONENT_H_

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

#ifndef _ITF_RAY_BTACTIONWAYPOINTPATHFIND_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionWaypointPathFind.h"
#endif // _ITF_RAY_BTACTIONWAYPOINTPATHFIND_H_

#ifndef _ITF_RAY_BTACTIONRESCUEDDISAPPEAR_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionRescuedDisappear.h"
#endif // _ITF_RAY_BTACTIONRESCUEDDISAPPEAR_H_

#ifndef _ITF_RAY_BTACTIONUSETELEPORT_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionUseTeleport.h"
#endif // _ITF_RAY_BTACTIONUSETELEPORT_H_

#ifndef _ITF_RAY_BTACTIONDANCEONDOOR_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionDanceOnDoor.h"
#endif // _ITF_RAY_BTACTIONDANCEONDOOR_H_

#ifndef _ITF_Ray_BTActionWaitForDanceOnDoor_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionWaitForDanceOnDoor.h"
#endif // _ITF_RAY_BTACTIONWAITFORDANCEONDOOR_H_

#ifndef _ITF_RAY_BTACTIONACTIVATESTONE_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionActivateStone.h"
#endif // _ITF_RAY_BTACTIONACTIVATESTONE_H_

#ifndef _ITF_RAY_BTACTIONPERFORMPEDESTAL_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionPerformPedestal.h"
#endif // _ITF_RAY_BTACTIONPERFORMPEDESTAL_H_

#ifndef _ITF_RAY_BTACTIONHITTARGET_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionHitTarget.h"
#endif // _ITF_RAY_BTACTIONHITTARGET_H_

#ifndef _ITF_RAY_BTACTIONWALKTOTARGETSPRINTWITHPLAYER_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionWalkToTargetSprintWithPlayer.h"
#endif // _ITF_RAY_BTACTIONWALKTOTARGETSPRINTWITHPLAYER_H_

#ifndef _ITF_RAY_BTACTIONFOLLOWTARGET_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionFollowTarget.h"
#endif // _ITF_RAY_BTACTIONFOLLOWTARGET_H_

#ifndef _ITF_RAY_BTACTIONRECEIVEHIT_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionReceiveHit.h"
#endif // _ITF_RAY_BTACTIONRECEIVEHIT_H_

#ifndef _ITF_RAY_BTACTIONRUNDARKTOONIFIED_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionRunDarktoonified.h"
#endif // _ITF_RAY_BTACTIONRUNDARKTOONIFIED_H_

#ifndef _ITF_RAY_BTACTIONFINDATTACKTARGET_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionFindAttackTarget.h"
#endif // _ITF_RAY_BTACTIONFINDATTACKTARGET_H_

#ifndef _ITF_RAY_BTDECIDERTARGETINRANGETOATTACK_H_
#include "rayman/gameplay/ai/BTs/Ray_BTDeciderTargetInRangeToAttack.h"
#endif // _ITF_RAY_BTDECIDERTARGETINRANGETOATTACK_H_

#ifndef _ITF_RAY_BTDECIDERFINDTARGETATTACKPOS_H_
#include "rayman/gameplay/ai/BTs/Ray_BTDeciderFindTargetAttackPos.h"
#endif // _ITF_RAY_BTDECIDERFINDTARGETATTACKPOS_H_

#ifndef _ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MultiPiecesActorAIComponent.h"
#endif //_ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_


#ifndef _ITF_RAY_SPIKYBALLCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SpikyBallComponent.h"
#endif //_ITF_RAY_SPIKYBALLCOMPONENT_H_

#ifndef _ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SeekingJellyfishAIComponent.h"
#endif //_ITF_RAY_SEEKINGJELLYFISHAICOMPONENT_H_


#ifndef _ITF_RAY_ROCKFISHAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_RockFishAIComponent.h"
#endif //_ITF_RAY_ROCKFISHAICOMPONENT_H_

#ifndef _ITF_RAY_MOVIEPLAYERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MoviePlayerComponent.h"
#endif //_ITF_RAY_MOVIEPLAYERCOMPONENT_H_

/********* WORLDMAP **************/

#ifndef _ITF_RAYWMSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_SpotComponent.h"
#endif //_ITF_RAYWMSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMLEVELSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_WorldSpotComponent.h"
#endif //_ITF_RAYWMLEVELSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMLEVELSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LevelSpotComponent.h"
#endif //_ITF_RAYWMLEVELSPOTCOMPONENT_H_

#ifndef _ITF_RAYWMGAMESTATSCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_GameStatsComponent.h"
#endif //_ITF_RAYWMGAMESTATSCOMPONENT_H_

#ifndef _ITF_RAY_WM_LOADINGSCREENCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LoadingScreenComponent.h"
#endif //_ITF_RAY_WM_LOADINGSCREENCOMPONENT_H_

#ifndef _ITF_RAYWMBONUSLEVELSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_BonusLevelSpotComponent.h"
#endif //_ITF_RAYWMBONUSLEVELSPOTCOMPONENT_H_

#ifndef _ITF_RAY_WM_FOOTPATHCOMPONENT_H_
#include "rayman/gameplay/Components/Worldmap/Ray_WM_FootpathComponent.h"
#endif //_ITF_RAY_WM_FOOTPATHCOMPONENT_H_

#ifndef _ITF_RAY_UIFADESCREENCOMPONENT_H_
#include "rayman/gameplay/Ray_UIFadeScreenComponent.h"
#endif //_ITF_RAY_UIFADESCREENCOMPONENT_H_

#ifndef _ITF_RAYWMWORLDRECAPCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_WorldRecapComponent.h"
#endif //_ITF_RAYWMWORLDRECAPCOMPONENT_H_

#ifndef _ITF_RAYWMLEVELRECAPCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LevelRecapComponent.h"
#endif //_ITF_RAYWMLEVELRECAPCOMPONENT_H_

#ifndef _ITF_RAYWMNEWCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_NewComponent.h"
#endif //_ITF_RAYWMNEWCOMPONENT_H_

#ifndef _ITF_RAYWMSHOWCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_ShowComponent.h"
#endif //_ITF_RAYWMSHOWCOMPONENT_H_

/********* GPP BEN ***************/

#ifndef _ITF_RAY_ASPINETWORKCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AspiNetworkComponent.h"
#endif //_ITF_RAY_ASPINETWORKCOMPONENT_H_

/********* END GPP BEN ***************/

#ifndef _ITF_RAY_AIMANAGER_H_
#include "rayman/gameplay/Ray_AIManager.h"
#endif

#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkManager.h"
#endif //_ITF_RAY_AINETWORKMANAGER_H_

#ifndef _ITF_RAY_AIRESCUEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AIRescueManager.h"
#endif //_ITF_RAY_AIRESCUEMANAGER_H_

#ifndef _ITF_RAY_MUSICMANAGER_H_
#include "rayman/gameplay/Ray_MusicManager.h"
#endif //_ITF_RAY_MUSICMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMapLoading.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_

#ifndef _ITF_RayVita_FrescoManagerAIComponent_H_
#include "rayman/gameplay/GamePlayVita/Managers/RayVita_FrescoManagerAIComponent.h"
#endif //_ITF_RayVita_FrescoManagerAIComponent_H_
#ifndef _ITF_RayVita_AIRelicBehavior_H_
#include "rayman/gameplay/GamePlayVita/ai/Behaviors/Relic/RayVita_AIRelicBehavior.h"
#endif //_ITF_RayVita_AIRelicBehavior_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////


    Ray_GameInterface::Ray_GameInterface()
    {
        Ray_AIManager::create();
        Ray_AINetworkManager::create();
        Ray_AIRescueManager::create();
        Ray_AvoidanceManager::create();
        Ray_FightManager::create();
        Ray_BlackSwarmZoneManager::createSingleton();
        SINGLETONS.setCheatManager( newAlloc(mId_Singleton, Ray_CheatManager) );
        Ray_CameraControllerManager::create();
        Ray_GameManager::create();
        BodyPartFactory_Template::create();
        BodyPartFactory::create();
    }

    Ray_GameInterface::~Ray_GameInterface()
    {
        Ray_GameManager::destroy();
        Ray_BlackSwarmZoneManager::destroySingleton();
        Ray_FightManager::destroy();
        Ray_AvoidanceManager::destroy();
        Ray_AIRescueManager::destroy();
        Ray_AINetworkManager::destroy();
        Ray_AIManager::destroy();
    }

    void Ray_GameInterface::init()
    {
        GameInterface::init();

        RAY_GAMEMANAGER->init();
        RAY_AVOIDANCEMANAGER->init();
        RAY_FIGHTMANAGER->init();
    }

    void Ray_GameInterface::preWorldUpdate( f32 _dt )
    {
        RAY_AVOIDANCEMANAGER->update(_dt);
    }
    void Ray_GameInterface::postWorldUpdate( f32 _dt )
    {
        Super::postWorldUpdate(_dt);
        RAY_FIGHTMANAGER->update(_dt);
		Ray_BlackSwarmZoneManager::getSingleton()->update(_dt);
    }

    void registerActorComponentsFactory_Player(ObjectFactory* actorComponentsFactory)
    {
        // Player
        actorComponentsFactory->RegisterObject<Ray_PlayerControllerComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerControllerComponent,2850198285));
        actorComponentsFactory->RegisterObject<Ray_PlayerCostumeComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerCostumeComponent,3635797832));
        actorComponentsFactory->RegisterObject<Ray_PlayerCostumeManagerComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerCostumeManagerComponent,3402508516));
        actorComponentsFactory->RegisterObject<Ray_PlayerCostumePriceComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerCostumePriceComponent,760770928));
        actorComponentsFactory->RegisterObject<Ray_ConceptGalleryComponent>(ITF_GET_STRINGID_CRC(Ray_ConceptGalleryComponent,4291508693));
        actorComponentsFactory->RegisterObject<Ray_ChangeRoomDoorComponent>(ITF_GET_STRINGID_CRC(Ray_ChangeRoomDoorComponent,4156471626));
        actorComponentsFactory->RegisterObject<Ray_CharacterDebuggerComponent>(ITF_GET_STRINGID_CRC(Ray_CharacterDebuggerComponent,1706648044));
        actorComponentsFactory->RegisterObject<Ray_PlayerOffScreenIconComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerOffScreenIconComponent,892597551));
        actorComponentsFactory->RegisterObject<Ray_PlayerDeadSoul2DComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerDeadSoul2DComponent,1239346062));
        actorComponentsFactory->RegisterObject<Ray_PlayerHudScoreComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerHudScoreComponent,2384152513));
        actorComponentsFactory->RegisterObject<Ray_ShooterControllerComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterControllerComponent,3971257716));
        actorComponentsFactory->RegisterObject<Ray_ShooterStimComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterStimComponent,1054562797));
        actorComponentsFactory->RegisterObject<Ray_ShooterSpawnerComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterSpawnerComponent,4291761801));
        actorComponentsFactory->RegisterObject<Ray_ShooterSpawnerModifierComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterSpawnerModifierComponent,3122658323));
        actorComponentsFactory->RegisterObject<Ray_WM_ControllerComponent>(ITF_GET_STRINGID_CRC(Ray_WM_ControllerComponent,2120802388));
    }

    void registerActorComponentsFactory_Misc(ObjectFactory* actorComponentsFactory)
    {
        //Misc
        actorComponentsFactory->RegisterObject<Ray_PlatformerCameraComponent>(ITF_GET_STRINGID_CRC(Ray_PlatformerCameraComponent,920536816));
        actorComponentsFactory->RegisterObject<Ray_SwingComponent>(ITF_GET_STRINGID_CRC(Ray_SwingComponent,3074882342));
        actorComponentsFactory->RegisterObject<Ray_SwingRopeComponent>(ITF_GET_STRINGID_CRC(Ray_SwingRopeComponent,1789060767));
        actorComponentsFactory->RegisterObject<Ray_SkullCoinComponent>(ITF_GET_STRINGID_CRC(Ray_SkullCoinComponent,1458755036));
        actorComponentsFactory->RegisterObject<Ray_ShooterCameraComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterCameraComponent,1772393706));
        actorComponentsFactory->RegisterObject<Ray_ShooterCameraModifierComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterCameraModifierComponent,1893778059));
        actorComponentsFactory->RegisterObject<Ray_ShooterCheckPointComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterCheckPointComponent,362659308));
        actorComponentsFactory->RegisterObject<Ray_ShooterDoorComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterDoorComponent,2820965407));
        actorComponentsFactory->RegisterObject<Ray_SceneSettingsComponent>(ITF_GET_STRINGID_CRC(Ray_SceneSettingsComponent,129898962));
        actorComponentsFactory->RegisterObject<Ray_AvoidanceComponent>(ITF_GET_STRINGID_CRC(Ray_AvoidanceComponent,2384145511));
        actorComponentsFactory->RegisterObject<Ray_DanceDoorComponent>(ITF_GET_STRINGID_CRC(Ray_DanceDoorComponent,3935928756));
        actorComponentsFactory->RegisterObject<Ray_DanceStoneComponent>(ITF_GET_STRINGID_CRC(Ray_DanceStoneComponent,1997048734));
        actorComponentsFactory->RegisterObject<Ray_DanceEyeComponent>(ITF_GET_STRINGID_CRC(Ray_DanceEyeComponent,1624614149));
        actorComponentsFactory->RegisterObject<Ray_DoorTeleportComponent>(ITF_GET_STRINGID_CRC(Ray_DoorTeleportComponent,1015048068));
        actorComponentsFactory->RegisterObject<Ray_LandOfTheDeadTeleportComponent>(ITF_GET_STRINGID_CRC(Ray_LandOfTheDeadTeleportComponent,1349661976));
        actorComponentsFactory->RegisterObject<Ray_DynamicFogComponent>(ITF_GET_STRINGID_CRC(Ray_DynamicFogComponent,822399796));
        actorComponentsFactory->RegisterObject<Ray_EnterDoorComponent>(ITF_GET_STRINGID_CRC(Ray_EnterDoorComponent,34510304));
        actorComponentsFactory->RegisterObject<Ray_StargateComponent>(ITF_GET_STRINGID_CRC(Ray_StargateComponent,3525399849));
        actorComponentsFactory->RegisterObject<Ray_StargateNodeComponent>(ITF_GET_STRINGID_CRC(Ray_StargateNodeComponent,2612773739));
        actorComponentsFactory->RegisterObject<Ray_ActivationSequenceStoneComponent>(ITF_GET_STRINGID_CRC(Ray_ActivationSequenceStoneComponent,2008432726));
        actorComponentsFactory->RegisterObject<Ray_ActivationSequenceVisualComponent>(ITF_GET_STRINGID_CRC(Ray_ActivationSequenceVisualComponent,3789500893));
        actorComponentsFactory->RegisterObject<Ray_ActivationSequenceTriggerComponent>(ITF_GET_STRINGID_CRC(Ray_ActivationSequenceTriggerComponent,2552097327));
        actorComponentsFactory->RegisterObject<Ray_ScaleTunnelComponent>(ITF_GET_STRINGID_CRC(Ray_ScaleTunnelComponent,1405566658));
        actorComponentsFactory->RegisterObject<Ray_ScaleFunnelComponent>(ITF_GET_STRINGID_CRC(Ray_ScaleFunnelComponent,3096730962));
        actorComponentsFactory->RegisterObject<Ray_BabyPiranhaComponent>(ITF_GET_STRINGID_CRC(Ray_BabyPiranhaComponent,1117370284));
        actorComponentsFactory->RegisterObject<Ray_BabyPiranhaAIComponent>(ITF_GET_STRINGID_CRC(Ray_BabyPiranhaAIComponent,3844595021));
        actorComponentsFactory->RegisterObject<Ray_SpawnerComponent>(ITF_GET_STRINGID_CRC(Ray_SpawnerComponent,1510308342));
        actorComponentsFactory->RegisterObject<Ray_DarkArenaDoorComponent>(ITF_GET_STRINGID_CRC(Ray_DarkArenaDoorComponent,3781071830));
        actorComponentsFactory->RegisterObject<Ray_DarktoonTrapHoleComponent>(ITF_GET_STRINGID_CRC(Ray_DarktoonTrapHoleComponent,3015066619));
        actorComponentsFactory->RegisterObject<Ray_DispenserComponent>(ITF_GET_STRINGID_CRC(Ray_DispenserComponent,2759118564));
        actorComponentsFactory->RegisterObject<Ray_PlatformAIComponent>(ITF_GET_STRINGID_CRC(Ray_PlatformAIComponent,1208901062));
        actorComponentsFactory->RegisterObject<Ray_FixedAIComponent>(ITF_GET_STRINGID_CRC(Ray_FixedAIComponent,3817174854));
        actorComponentsFactory->RegisterObject<Ray_PauseActorComponent>(ITF_GET_STRINGID_CRC(Ray_PauseActorComponent,1841835503));
        actorComponentsFactory->RegisterObject<Ray_BulletLauncherComponent>(ITF_GET_STRINGID_CRC(Ray_BulletLauncherComponent,1330138928));
        actorComponentsFactory->RegisterObject<Ray_EyeAnimationComponent>(ITF_GET_STRINGID_CRC(Ray_EyeAnimationComponent,2959174830));
        actorComponentsFactory->RegisterObject<Ray_GeyserComponent>(ITF_GET_STRINGID_CRC(Ray_GeyserComponent,722708767));
        actorComponentsFactory->RegisterObject<Ray_ShooterActorParameterComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterActorParameterComponent,4085444954));
        actorComponentsFactory->RegisterObject<Ray_LumMusicManagerAIComponent>(ITF_GET_STRINGID_CRC(Ray_LumMusicManagerAIComponent,2185561851));
        actorComponentsFactory->RegisterObject<Ray_StringWaveGeneratorComponent>(ITF_GET_STRINGID_CRC(Ray_StringWaveGeneratorComponent,2754583878));
        actorComponentsFactory->RegisterObject<Ray_SubAnchorComponent>(ITF_GET_STRINGID_CRC(Ray_SubAnchorComponent,571243035));
        actorComponentsFactory->RegisterObject<Ray_MenuE3Component>(ITF_GET_STRINGID_CRC(Ray_MenuE3Component,786489758));
        actorComponentsFactory->RegisterObject<Ray_ScoreBoardComponent>(ITF_GET_STRINGID_CRC(Ray_ScoreBoardComponent,2003984371));
        actorComponentsFactory->RegisterObject<Ray_LastPlayerTriggerComponent>(ITF_GET_STRINGID_CRC(Ray_LastPlayerTriggerComponent,1954106939));
		actorComponentsFactory->RegisterObject<Ray_BlackSwarmComponent>(ITF_GET_STRINGID_CRC(Ray_BlackSwarmComponent,3647179052));
		actorComponentsFactory->RegisterObject<Ray_BlackSwarmSpawnerComponent>(ITF_GET_STRINGID_CRC(Ray_BlackSwarmSpawnerComponent,3056762993));
        actorComponentsFactory->RegisterObject<Ray_BlackSwarmRepellerAIComponent>(ITF_GET_STRINGID_CRC(Ray_BlackSwarmRepellerAIComponent,533755526));
        actorComponentsFactory->RegisterObject<Ray_CageMapAmbianceComponent>(ITF_GET_STRINGID_CRC(Ray_CageMapAmbianceComponent,1901195588));
        actorComponentsFactory->RegisterObject<Ray_CageStoneComponent>(ITF_GET_STRINGID_CRC(Ray_CageStoneComponent,190727526));
        actorComponentsFactory->RegisterObject<Ray_BezierTreeAIComponent>(ITF_GET_STRINGID_CRC(Ray_BezierTreeAIComponent,989843734));
        actorComponentsFactory->RegisterObject<Ray_PlatformChainAIComponent>(ITF_GET_STRINGID_CRC(Ray_PlatformChainAIComponent,1486053090));
        actorComponentsFactory->RegisterObject<Ray_ElectoonSpawnerComponent>(ITF_GET_STRINGID_CRC(Ray_ElectoonSpawnerComponent,872832010));

        // Bubble prizes
        actorComponentsFactory->RegisterObject<Ray_BubblePrizeBumperComponent>(ITF_GET_STRINGID_CRC(Ray_BubblePrizeBumperComponent,542355520));
        actorComponentsFactory->RegisterObject<Ray_BubblePrizeHubComponent>(ITF_GET_STRINGID_CRC(Ray_BubblePrizeHubComponent,162474712));
        actorComponentsFactory->RegisterObject<Ray_BubblePrizePlatformComponent>(ITF_GET_STRINGID_CRC(Ray_BubblePrizePlatformComponent,1615332505));
        actorComponentsFactory->RegisterObject<Ray_BubblePrizeSpawnerComponent>(ITF_GET_STRINGID_CRC(Ray_BubblePrizeSpawnerComponent,1701732717));
    }

    void registerActorComponentsFactory_AI(ObjectFactory* actorComponentsFactory)
    {
        // AI
        actorComponentsFactory->RegisterObject<Ray_GroundAIControllerComponent>(ITF_GET_STRINGID_CRC(Ray_GroundAIControllerComponent,1159656342));
        actorComponentsFactory->RegisterObject<Ray_LivingStoneAIComponent2>(ITF_GET_STRINGID_CRC(Ray_LivingStoneAIComponent2,2988881290));
        actorComponentsFactory->RegisterObject<Ray_GroundEnemyAIComponent>(ITF_GET_STRINGID_CRC(Ray_GroundEnemyAIComponent,3660733364));
        actorComponentsFactory->RegisterObject<Ray_ShooterTurretAIComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterTurretAIComponent,3379650764));
        actorComponentsFactory->RegisterObject<Ray_ShooterPirateShipAIComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterPirateShipAIComponent,1339744900));
        actorComponentsFactory->RegisterObject<Ray_ShooterGardianAIComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterGardianAIComponent,66886853));
        actorComponentsFactory->RegisterObject<Ray_HunterHoleAIComponent>(ITF_GET_STRINGID_CRC(Ray_HunterHoleAIComponent,3494158620));
        actorComponentsFactory->RegisterObject<Ray_HunterBossAIComponent>(ITF_GET_STRINGID_CRC(Ray_HunterBossAIComponent,3955300393));
        actorComponentsFactory->RegisterObject<Ray_HunterBossNodeComponent>(ITF_GET_STRINGID_CRC(Ray_HunterBossNodeComponent,3283634905));
        actorComponentsFactory->RegisterObject<Ray_ConditionalTriggerComponent>(ITF_GET_STRINGID_CRC(Ray_ConditionalTriggerComponent,3398893856));
        actorComponentsFactory->RegisterObject<Ray_GroundAIComponent>(ITF_GET_STRINGID_CRC(Ray_GroundAIComponent,2230428483));
        actorComponentsFactory->RegisterObject<Ray_SimpleAIComponent>(ITF_GET_STRINGID_CRC(Ray_SimpleAIComponent,2237723057));
        actorComponentsFactory->RegisterObject<Ray_CyclopeusAIComponent>(ITF_GET_STRINGID_CRC(Ray_CyclopeusAIComponent,1994508162));
        actorComponentsFactory->RegisterObject<Ray_DarkBirdAIComponent>(ITF_GET_STRINGID_CRC(Ray_DarkBirdAIComponent,519987365));
        actorComponentsFactory->RegisterObject<Ray_DarkToonAIComponent>(ITF_GET_STRINGID_CRC(Ray_DarkToonAIComponent,3931431627));
        actorComponentsFactory->RegisterObject<Ray_DarktoonTrapAIComponent>(ITF_GET_STRINGID_CRC(Ray_DarktoonTrapAIComponent,4253825461));
        actorComponentsFactory->RegisterObject<Ray_RewardAIComponent>(ITF_GET_STRINGID_CRC(Ray_RewardAIComponent,4028072561));
        actorComponentsFactory->RegisterObject<Ray_RedWizardAIComponent>(ITF_GET_STRINGID_CRC(Ray_RedWizardAIComponent,329209979));
        actorComponentsFactory->RegisterObject<Ray_FriendlyAIComponent>(ITF_GET_STRINGID_CRC(Ray_FriendlyAIComponent,3278201406));
        actorComponentsFactory->RegisterObject<Ray_FruitAIComponent>(ITF_GET_STRINGID_CRC(Ray_FruitAIComponent,1618051749));
        actorComponentsFactory->RegisterObject<Ray_JanodAIComponent>(ITF_GET_STRINGID_CRC(Ray_JanodAIComponent,1405091922));
        actorComponentsFactory->RegisterObject<Ray_BulletAIComponent>(ITF_GET_STRINGID_CRC(Ray_BulletAIComponent,2316478316));
        actorComponentsFactory->RegisterObject<Ray_WalkingBulletAIComponent>(ITF_GET_STRINGID_CRC(Ray_WalkingBulletAIComponent,580133949));
        actorComponentsFactory->RegisterObject<Ray_SeekingBulletAIComponent>(ITF_GET_STRINGID_CRC(Ray_SeekingBulletAIComponent,996090817));
        actorComponentsFactory->RegisterObject<Ray_BreakableAIComponent>(ITF_GET_STRINGID_CRC(Ray_BreakableAIComponent,1847158746));
        actorComponentsFactory->RegisterObject<Ray_BreakableStackManagerAIComponent>(ITF_GET_STRINGID_CRC(Ray_BreakableStackManagerAIComponent,1979560305));
        actorComponentsFactory->RegisterObject<Ray_BreakableStackElementAIComponent>(ITF_GET_STRINGID_CRC(Ray_BreakableStackElementAIComponent,1985991899));
        actorComponentsFactory->RegisterObject<Ray_BossBuboAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossBuboAIComponent,1566657424));
        actorComponentsFactory->RegisterObject<Ray_BossMapEndAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossMapEndAIComponent,1038493720));
        actorComponentsFactory->RegisterObject<Ray_BossPlantAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossPlantAIComponent,203747753));
        actorComponentsFactory->RegisterObject<Ray_BossPlantNodeComponent>(ITF_GET_STRINGID_CRC(Ray_BossPlantNodeComponent,4281200548));
        actorComponentsFactory->RegisterObject<Ray_BossPlantArenaAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossPlantArenaAIComponent,2592008217));
        actorComponentsFactory->RegisterObject<Ray_BossBirdAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossBirdAIComponent,2632013408));
        actorComponentsFactory->RegisterObject<Ray_BossBirdPawnAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossBirdPawnAIComponent,281372016));
        actorComponentsFactory->RegisterObject<Ray_BigMamaAIComponent>(ITF_GET_STRINGID_CRC(Ray_BigMamaAIComponent,1359314854));
        actorComponentsFactory->RegisterObject<Ray_MamaEyesAIComponent>(ITF_GET_STRINGID_CRC(Ray_MamaEyesAIComponent,1789470946));
        actorComponentsFactory->RegisterObject<Ray_BossMorayAIComponent>(ITF_GET_STRINGID_CRC(Ray_BossMorayAIComponent,2045799600));
        actorComponentsFactory->RegisterObject<Ray_BossMorayNodeComponent>(ITF_GET_STRINGID_CRC(Ray_BossMorayNodeComponent,733639517));
        actorComponentsFactory->RegisterObject<Ray_BossCameraComponent>(ITF_GET_STRINGID_CRC(Ray_BossCameraComponent,3307596908));
        actorComponentsFactory->RegisterObject<Ray_StimComponent>(ITF_GET_STRINGID_CRC(Ray_StimComponent,4139393211));
        actorComponentsFactory->RegisterObject<Ray_CheckpointComponent>(ITF_GET_STRINGID_CRC(Ray_CheckpointComponent,1094572706));
        actorComponentsFactory->RegisterObject<Ray_ChildLaunchComponent>(ITF_GET_STRINGID_CRC(Ray_ChildLaunchComponent,3515647608));
        actorComponentsFactory->RegisterObject<Ray_TriggerBounceComponent>(ITF_GET_STRINGID_CRC(Ray_TriggerBounceComponent,4162442951));
        actorComponentsFactory->RegisterObject<Ray_TutorialComponent>(ITF_GET_STRINGID_CRC(Ray_TutorialComponent,2778069029));
        actorComponentsFactory->RegisterObject<Ray_WMContentUnlockComponent>(ITF_GET_STRINGID_CRC(Ray_WMContentUnlockComponent,654430470));
        actorComponentsFactory->RegisterObject<Ray_LoadNotificationComponent>(ITF_GET_STRINGID_CRC(Ray_LoadNotificationComponent,1404551269));
        actorComponentsFactory->RegisterObject<Ray_SkipLevelComponent>(ITF_GET_STRINGID_CRC(Ray_SkipLevelComponent,1847282724));
        actorComponentsFactory->RegisterObject<Ray_LevelTitleComponent>(ITF_GET_STRINGID_CRC(Ray_LevelTitleComponent,347754987));
        actorComponentsFactory->RegisterObject<Ray_WaterPerturbationComponent>(ITF_GET_STRINGID_CRC(Ray_WaterPerturbationComponent,4194052678));
        actorComponentsFactory->RegisterObject<Ray_LivingstoneHatAIComponent>(ITF_GET_STRINGID_CRC(Ray_LivingstoneHatAIComponent,3806885714));
        actorComponentsFactory->RegisterObject<Ray_CameraLimiterComponent>(ITF_GET_STRINGID_CRC(Ray_CameraLimiterComponent,1301298598));
        actorComponentsFactory->RegisterObject<Ray_PlatformTreeComponent>(ITF_GET_STRINGID_CRC(Ray_PlatformTreeComponent,4107374076));
        actorComponentsFactory->RegisterObject<Ray_PushButtonComponent>(ITF_GET_STRINGID_CRC(Ray_PushButtonComponent,2244274439));
        actorComponentsFactory->RegisterObject<Ray_PushedAIComponent>(ITF_GET_STRINGID_CRC(Ray_PushedAIComponent,747720257));
        actorComponentsFactory->RegisterObject<Ray_FishSwarmAIComponent>(ITF_GET_STRINGID_CRC(Ray_FishSwarmAIComponent,2501834615));
        actorComponentsFactory->RegisterObject<Ray_GolemAIComponent>(ITF_GET_STRINGID_CRC(Ray_GolemAIComponent,1489747707));
        actorComponentsFactory->RegisterObject<Ray_CrystalKingAIComponent>(ITF_GET_STRINGID_CRC(Ray_CrystalKingAIComponent,4136222629));
        actorComponentsFactory->RegisterObject<Ray_StargateManagerAIComponent>(ITF_GET_STRINGID_CRC(Ray_StargateManagerAIComponent,2484063737));
        actorComponentsFactory->RegisterObject<Ray_CreditsManagerAIComponent>(ITF_GET_STRINGID_CRC(Ray_CreditsManagerAIComponent,2075960340));
        actorComponentsFactory->RegisterObject<Ray_SwarmChaseAIComponent>(ITF_GET_STRINGID_CRC(Ray_SwarmChaseAIComponent,2796589410));
        actorComponentsFactory->RegisterObject<Ray_SwarmChaseNodeComponent>(ITF_GET_STRINGID_CRC(Ray_SwarmChaseNodeComponent,3618604596));
	 	actorComponentsFactory->RegisterObject<Ray_ShooterAIComponent>(ITF_GET_STRINGID_CRC(Ray_ShooterAIComponent,3358339901));
		actorComponentsFactory->RegisterObject<Ray_ChangePageComponent>(ITF_GET_STRINGID_CRC(Ray_ChangePageComponent,1321490025));
		actorComponentsFactory->RegisterObject<Ray_CustomChangePageComponent>(ITF_GET_STRINGID_CRC(Ray_CustomChangePageComponent,3338454047));
        actorComponentsFactory->RegisterObject<Ray_ChangePageWithAnimComponent>(ITF_GET_STRINGID_CRC(Ray_ChangePageWithAnimComponent,4229378487));
        actorComponentsFactory->RegisterObject<Ray_ElevatorAIComponent>(ITF_GET_STRINGID_CRC(Ray_ElevatorAIComponent,3597187499));
        actorComponentsFactory->RegisterObject<Ray_ElevatorWheelAIComponent>(ITF_GET_STRINGID_CRC(Ray_ElevatorWheelAIComponent,3471739185));
        actorComponentsFactory->RegisterObject<Ray_ElevatorMonsterAIComponent>(ITF_GET_STRINGID_CRC(Ray_ElevatorMonsterAIComponent,3963479976));
        actorComponentsFactory->RegisterObject<Ray_WaterHandsAIComponent>(ITF_GET_STRINGID_CRC(Ray_WaterHandsAIComponent,687583937));
        actorComponentsFactory->RegisterObject<Ray_HandsAIComponent>(ITF_GET_STRINGID_CRC(Ray_HandsAIComponent,184634122));
        actorComponentsFactory->RegisterObject<Ray_GeyserPlatformAIComponent>(ITF_GET_STRINGID_CRC(Ray_GeyserPlatformAIComponent,176644665));
        actorComponentsFactory->RegisterObject<Ray_NautilusAIComponent>(ITF_GET_STRINGID_CRC(Ray_NautilusAIComponent,3430587283));
        actorComponentsFactory->RegisterObject<Ray_FriendlyBTAIComponent>(ITF_GET_STRINGID_CRC(Ray_FriendlyBTAIComponent,896114598));
        actorComponentsFactory->RegisterObject<Ray_DeadGuyBTAIComponent>(ITF_GET_STRINGID_CRC(Ray_DeadGuyBTAIComponent,3677113079));
        actorComponentsFactory->RegisterObject<Ray_TalkingHatAIComponent>(ITF_GET_STRINGID_CRC(Ray_TalkingHatAIComponent,3439430031));
        actorComponentsFactory->RegisterObject<Ray_MurphyAIComponent>(ITF_GET_STRINGID_CRC(Ray_MurphyAIComponent,4141462231));
        actorComponentsFactory->RegisterObject<Ray_MurphyAssistAIComponent>(ITF_GET_STRINGID_CRC(Ray_MurphyAssistAIComponent,2664383627));
        actorComponentsFactory->RegisterObject<Ray_MurphyAssistBubbleAIComponent>(ITF_GET_STRINGID_CRC(Ray_MurphyAssistBubbleAIComponent, 2616088894));
        actorComponentsFactory->RegisterObject<Ray_ChronoAIComponent>(ITF_GET_STRINGID_CRC(Ray_ChronoAIComponent,1392119711));
        actorComponentsFactory->RegisterObject<Ray_TimeAttackHUDTimerComponent>(ITF_GET_STRINGID_CRC(Ray_TimeAttackHUDTimerComponent,2795731935));
        actorComponentsFactory->RegisterObject<Ray_TimeAttackHUDResultsComponent>(ITF_GET_STRINGID_CRC(Ray_TimeAttackHUDResultsComponent,3725071531));
        actorComponentsFactory->RegisterObject<Ray_TimeAttackFlagComponent>(ITF_GET_STRINGID_CRC(Ray_TimeAttackFlagComponent,1215046538));
        actorComponentsFactory->RegisterObject<Ray_TalkingBubbleComponent>(ITF_GET_STRINGID_CRC(Ray_TalkingBubbleComponent,2023492281));
        actorComponentsFactory->RegisterObject<Ray_TeensyRobotAIComponent>(ITF_GET_STRINGID_CRC(Ray_TeensyRobotAIComponent,47198029));
        actorComponentsFactory->RegisterObject<Ray_AIBTOrderComponent>(ITF_GET_STRINGID_CRC(Ray_AIBTOrderComponent,3178466555));
        actorComponentsFactory->RegisterObject<Ray_AINetworkComponent>(ITF_GET_STRINGID_CRC(Ray_AINetworkComponent,1696315433));
        actorComponentsFactory->RegisterObject<Ray_MusicScoreAIComponent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreAIComponent,1123749061));
        actorComponentsFactory->RegisterObject<Ray_FluidFallAIComponent>(ITF_GET_STRINGID_CRC(Ray_FluidFallAIComponent,2143327929));
        actorComponentsFactory->RegisterObject<Ray_FirePatchAIComponent>(ITF_GET_STRINGID_CRC(Ray_FirePatchAIComponent,838986340));
        actorComponentsFactory->RegisterObject<Ray_JalapenoKingAiComponent>(ITF_GET_STRINGID_CRC(Ray_JalapenoKingAiComponent,2116341111));
        actorComponentsFactory->RegisterObject<Ray_MusicScoreSnapComponent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreSnapComponent,1990836487));
        actorComponentsFactory->RegisterObject<Ray_ForceFieldComponent>(ITF_GET_STRINGID_CRC(Ray_ForceFieldComponent,873468901));
        actorComponentsFactory->RegisterObject<Ray_StringWaveFaderComponent>(ITF_GET_STRINGID_CRC(Ray_StringWaveFaderComponent,144433532));
        actorComponentsFactory->RegisterObject<Ray_BreakableCageComponent>(ITF_GET_STRINGID_CRC(Ray_BreakableCageComponent,2851407257));
        actorComponentsFactory->RegisterObject<Ray_SnakeAIComponent>(ITF_GET_STRINGID_CRC(Ray_SnakeAIComponent,883253688));
        actorComponentsFactory->RegisterObject<Ray_ShooterGardianMoray>(ITF_GET_STRINGID_CRC(Ray_ShooterGardianMoray,3395627550));
        actorComponentsFactory->RegisterObject<Ray_ElectoonTargetComponent>(ITF_GET_STRINGID_CRC(Ray_ElectoonTargetComponent,3976685342));
        actorComponentsFactory->RegisterObject<Ray_WaveBallComponent>(ITF_GET_STRINGID_CRC(Ray_WaveBallComponent,310418336));
		actorComponentsFactory->RegisterObject<Ray_MultiPiecesActorAIComponent>(ITF_GET_STRINGID_CRC(Ray_MultiPiecesActorAIComponent,758641620));
        actorComponentsFactory->RegisterObject<Ray_SpikyBallComponent>(ITF_GET_STRINGID_CRC(Ray_SpikyBallComponent,936470761));
        actorComponentsFactory->RegisterObject<Ray_SeekingJellyfishAIComponent>(ITF_GET_STRINGID_CRC(Ray_SeekingJellyfishAIComponent,3961152351));
        actorComponentsFactory->RegisterObject<Ray_SinkingPlatformComponent>(ITF_GET_STRINGID_CRC(Ray_SinkingPlatformComponent,4096536880));
        actorComponentsFactory->RegisterObject<Ray_BasculePlatformComponent>(ITF_GET_STRINGID_CRC(Ray_BasculePlatformComponent,2501389937));
        actorComponentsFactory->RegisterObject<Ray_WaterFloatingPlatformComponent>(ITF_GET_STRINGID_CRC(Ray_WaterFloatingPlatformComponent,2757467585));
        actorComponentsFactory->RegisterObject<Ray_CageAIComponent>(ITF_GET_STRINGID_CRC(Ray_CageAIComponent,521364948));
        actorComponentsFactory->RegisterObject<Ray_BubbleAiComponent>(ITF_GET_STRINGID_CRC(Ray_BubbleAiComponent,2615864492));
        actorComponentsFactory->RegisterObject<Ray_alTranquiloAiComponent>(ITF_GET_STRINGID_CRC(Ray_alTranquiloAiComponent,3434288248));
        actorComponentsFactory->RegisterObject<Ray_PlatformEnemyAIComponent>(ITF_GET_STRINGID_CRC(Ray_PlatformEnemyAIComponent,1233007951));
        actorComponentsFactory->RegisterObject<Ray_SpikyShellTrapComponent>(ITF_GET_STRINGID_CRC(Ray_SpikyShellTrapComponent,2579463222));
        actorComponentsFactory->RegisterObject<Ray_BubbleSpawnerComponent>(ITF_GET_STRINGID_CRC(Ray_BubbleSpawnerComponent,4097717947));
        actorComponentsFactory->RegisterObject<Ray_OceanSpiderAIComponent>(ITF_GET_STRINGID_CRC(Ray_OceanSpiderAIComponent,2810138667));
        actorComponentsFactory->RegisterObject<Ray_LightningGeneratorAIComponent>(ITF_GET_STRINGID_CRC(Ray_LightningGeneratorAIComponent,2701334616));
        actorComponentsFactory->RegisterObject<Ray_AnglerFishAIComponent>(ITF_GET_STRINGID_CRC(Ray_AnglerFishAIComponent,418892941));
        actorComponentsFactory->RegisterObject<Ray_AbyssalHandAIComponent>(ITF_GET_STRINGID_CRC(Ray_AbyssalHandAIComponent,4177230497));
        actorComponentsFactory->RegisterObject<Ray_AbyssLightComponent>(ITF_GET_STRINGID_CRC(Ray_AbyssLightComponent,3139200749));
        actorComponentsFactory->RegisterObject<Ray_FireFlyKrillAIComponent>(ITF_GET_STRINGID_CRC(Ray_FireFlyKrillAIComponent,737561430));
        actorComponentsFactory->RegisterObject<Ray_ScreenSideTrajectoryFollowerComponent>(ITF_GET_STRINGID_CRC(Ray_ScreenSideTrajectoryFollowerComponent,2509899776));
        actorComponentsFactory->RegisterObject<Ray_SequenceSwitchComponent>(ITF_GET_STRINGID_CRC(Ray_SequenceSwitchComponent,3421407123));
        actorComponentsFactory->RegisterObject<Ray_RockFishAIComponent>(ITF_GET_STRINGID_CRC(Ray_RockFishAIComponent,1602147762));
        actorComponentsFactory->RegisterObject<Ray_FireFlyKrillSpawnerComponent>(ITF_GET_STRINGID_CRC(Ray_FireFlyKrillSpawnerComponent,3215081539));
        actorComponentsFactory->RegisterObject<Ray_LightningPatchAIComponent>(ITF_GET_STRINGID_CRC(Ray_LightningPatchAIComponent,2300483196));
        actorComponentsFactory->RegisterObject<Ray_BoulderAIComponent>(ITF_GET_STRINGID_CRC(Ray_BoulderAIComponent,2361173509));
        actorComponentsFactory->RegisterObject<Ray_ScoreGuageComponent>(ITF_GET_STRINGID_CRC(Ray_ScoreGuageComponent,635084377));
        actorComponentsFactory->RegisterObject<Ray_PlayerScoreComponent>(ITF_GET_STRINGID_CRC(Ray_PlayerScoreComponent,3889959905));
        actorComponentsFactory->RegisterObject<Ray_StoneManAIComponent>(ITF_GET_STRINGID_CRC(Ray_StoneManAIComponent,3804759324));
        actorComponentsFactory->RegisterObject<Ray_AILums2Component>(ITF_GET_STRINGID_CRC(Ray_AILums2Component,82903916));
        actorComponentsFactory->RegisterObject<Ray_MoviePlayerComponent>(ITF_GET_STRINGID_CRC(Ray_MoviePlayerComponent,1142662804));
        actorComponentsFactory->RegisterObject<Ray_ScoreLumAIComponent>(ITF_GET_STRINGID_CRC(Ray_ScoreLumAIComponent,2472225950));
        actorComponentsFactory->RegisterObject<Ray_ElectoonTeethComponent>(ITF_GET_STRINGID_CRC(Ray_ElectoonTeethComponent,498150873));
        actorComponentsFactory->RegisterObject<Ray_BubbleDreamerAIComponent>(ITF_GET_STRINGID_CRC(Ray_BubbleDreamerAIComponent,694843223));
        actorComponentsFactory->RegisterObject<Ray_SpikyPlatformAIComponent>(ITF_GET_STRINGID_CRC(Ray_SpikyPlatformAIComponent,3383273419));

		// <CASA::VITA
		actorComponentsFactory->RegisterObject<RayVita_FrescoManagerAIComponent>(ITF_GET_STRINGID_CRC(RayVita_FrescoManagerAIComponent,1859524894));
		// CASA::VITA>

    }

    void registerActorComponentsFactory_Misc2(ObjectFactory* actorComponentsFactory)
    {
        // World Map
        actorComponentsFactory->RegisterObject<Ray_WM_SpotComponent>(ITF_GET_STRINGID_CRC(Ray_WM_SpotComponent,3442082762));
        actorComponentsFactory->RegisterObject<Ray_WM_WorldSpotComponent>(ITF_GET_STRINGID_CRC(Ray_WM_WorldSpotComponent,3142377630));
        actorComponentsFactory->RegisterObject<Ray_WM_LevelSpotComponent>(ITF_GET_STRINGID_CRC(Ray_WM_LevelSpotComponent,4045004918));
        actorComponentsFactory->RegisterObject<Ray_WM_BonusLevelSpotComponent>(ITF_GET_STRINGID_CRC(Ray_WM_BonusLevelSpotComponent,3994185942));
        actorComponentsFactory->RegisterObject<Ray_WM_GameStatsComponent>(ITF_GET_STRINGID_CRC(Ray_WM_GameStatsComponent,1541137795));
        actorComponentsFactory->RegisterObject<Ray_WM_LoadingScreenComponent>(ITF_GET_STRINGID_CRC(Ray_WM_LoadingScreenComponent,1852067234));
        actorComponentsFactory->RegisterObject<Ray_WM_FootpathComponent>(ITF_GET_STRINGID_CRC(Ray_WM_FootpathComponent,4241412189));
        actorComponentsFactory->RegisterObject<Ray_WM_PanelComponent>(ITF_GET_STRINGID_CRC(Ray_WM_PanelComponent,1061832985));
        actorComponentsFactory->RegisterObject<Ray_UIFadeScreenComponent>(ITF_GET_STRINGID_CRC(Ray_UIFadeScreenComponent,1402917130));
        actorComponentsFactory->RegisterObject<Ray_WM_WorldRecapComponent>(ITF_GET_STRINGID_CRC(Ray_WM_WorldRecapComponent,1108271511));
        actorComponentsFactory->RegisterObject<Ray_WM_LevelRecapComponent>(ITF_GET_STRINGID_CRC(Ray_WM_LevelRecapComponent,1226660666));
        actorComponentsFactory->RegisterObject<Ray_WM_NewComponent>(ITF_GET_STRINGID_CRC(Ray_WM_NewComponent,167288612));
        actorComponentsFactory->RegisterObject<Ray_WM_ShowComponent>(ITF_GET_STRINGID_CRC(Ray_WM_ShowComponent,2143071717));
    }

    void registerActorComponentsFactory(ObjectFactory* actorComponentsFactory)
    {
        registerActorComponentsFactory_Player(actorComponentsFactory);
        registerActorComponentsFactory_Misc(actorComponentsFactory);
        registerActorComponentsFactory_AI(actorComponentsFactory);
        registerActorComponentsFactory_Misc2(actorComponentsFactory);
    }

    void registerAiBehaviorsFactory( ObjectFactory* aiBehaviorsFactory )
    {
        // AI Behaviours
        aiBehaviorsFactory->RegisterObject<Ray_AIBounceRoamingBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBounceRoamingBehavior_Template,1001718509));
        aiBehaviorsFactory->RegisterObject<Ray_AIBubblePrizeBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBubblePrizeBehavior_Template,500512533));
        aiBehaviorsFactory->RegisterObject<Ray_AIBubblePrizeFixedBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBubblePrizeFixedBehavior_Template,4089474871));
        aiBehaviorsFactory->RegisterObject<Ray_AICageBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AICageBehavior_Template,3603496838));
        aiBehaviorsFactory->RegisterObject<Ray_AIChest2Behavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIChest2Behavior_Template,860147291));
        aiBehaviorsFactory->RegisterObject<Ray_AICornBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AICornBehavior_Template,1666465776));
        aiBehaviorsFactory->RegisterObject<Ray_AIElectoonBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIElectoonBehavior_Template,2056904193));
        aiBehaviorsFactory->RegisterObject<Ray_AILums2Behavior_Template>(ITF_GET_STRINGID_CRC(Ray_AILums2Behavior_Template,4099824222));
        aiBehaviorsFactory->RegisterObject<Ray_AIScoreLumBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIScoreLumBehavior_Template,2806870689));
        aiBehaviorsFactory->RegisterObject<Ray_AILivingStoneWindBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AILivingStoneWindBehavior_Template,3646410645));
        aiBehaviorsFactory->RegisterObject<Ray_AILivingStoneDrowningBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AILivingStoneDrowningBehavior_Template,224911302));
        aiBehaviorsFactory->RegisterObject<Ray_AILivingStoneStaticBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AILivingStoneStaticBehavior_Template,2904103013));
        aiBehaviorsFactory->RegisterObject<Ray_AIMedalBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIMedalBehavior_Template,3301873927));
        aiBehaviorsFactory->RegisterObject<Ray_AINotePiafBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AINotePiafBehavior_Template,1272807503));
        aiBehaviorsFactory->RegisterObject<Ray_AIShooterAttackBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterAttackBehavior_Template,2518636371));
        aiBehaviorsFactory->RegisterObject<Ray_AIHunterAttackBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIHunterAttackBehavior_Template,117685005));
        aiBehaviorsFactory->RegisterObject<Ray_AIShooterCloseAttackBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterCloseAttackBehavior_Template,1467164194));
        aiBehaviorsFactory->RegisterObject<Ray_AIGroundRoamBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIGroundRoamBehavior_Template,3869081513));
        aiBehaviorsFactory->RegisterObject<Ray_AILivingStoneRoamBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AILivingStoneRoamBehavior_Template,2140270652));
        aiBehaviorsFactory->RegisterObject<Ray_AISleepBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AISleepBehavior_Template,3655251520));
        aiBehaviorsFactory->RegisterObject<Ray_AIGroundRunAwayBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIGroundRunAwayBehavior_Template,466747632));
        aiBehaviorsFactory->RegisterObject<Ray_AIGroundAttackBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIGroundAttackBehavior_Template,609353726));
        aiBehaviorsFactory->RegisterObject<Ray_AIDarktoonAttackBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIDarktoonAttackBehavior_Template,4282596837));
        aiBehaviorsFactory->RegisterObject<Ray_AIDarktoonLaunchBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIDarktoonLaunchBehavior_Template,547595480));
        aiBehaviorsFactory->RegisterObject<Ray_AIDarktoonTrapBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIDarktoonTrapBehavior_Template,3979299286));
        aiBehaviorsFactory->RegisterObject<Ray_AIRedWizardRoamBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIRedWizardRoamBehavior_Template,3383994418));
        aiBehaviorsFactory->RegisterObject<Ray_AIReceiveHitBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIReceiveHitBehavior_Template,2856657059));
        aiBehaviorsFactory->RegisterObject<Ray_AIDeathBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIDeathBehavior_Template,2099194204));
        aiBehaviorsFactory->RegisterObject<Ray_AIBubbleDeathBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBubbleDeathBehavior_Template,4112732321));
        aiBehaviorsFactory->RegisterObject<Ray_AIGroundFollowBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIGroundFollowBehavior_Template,1128740171));
        aiBehaviorsFactory->RegisterObject<Ray_AIRedWizardDanceBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIRedWizardDanceBehavior_Template,1990234418));
        aiBehaviorsFactory->RegisterObject<Ray_AIRedWizardPedestalBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIRedWizardPedestalBehavior_Template,2561050822));
        aiBehaviorsFactory->RegisterObject<Ray_AIRelicBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIRelicBehavior_Template,2405251001));
        aiBehaviorsFactory->RegisterObject<Ray_AIGroundDarktoonedBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIGroundDarktoonedBehavior_Template,3449732300));
        aiBehaviorsFactory->RegisterObject<Ray_AIFruitRoamingBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIFruitRoamingBehavior_Template,2586393688));
        aiBehaviorsFactory->RegisterObject<Ray_AIFruitTrapBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIFruitTrapBehavior_Template,3020317045));
        aiBehaviorsFactory->RegisterObject<Ray_AIJanodRoamingBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIJanodRoamingBehavior_Template,1071796122));
        aiBehaviorsFactory->RegisterObject<Ray_AIJanodTweenedBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIJanodTweenedBehavior_Template,64776580));
        aiBehaviorsFactory->RegisterObject<Ray_AIWaterFloatingBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIWaterFloatingBehavior_Template,4149133277));
        aiBehaviorsFactory->RegisterObject<Ray_AIRedWizardWaterSwimBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIRedWizardWaterSwimBehavior_Template,3238344605));
        aiBehaviorsFactory->RegisterObject<Ray_AIOrangeFloatingBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIOrangeFloatingBehavior_Template,46232455));
        aiBehaviorsFactory->RegisterObject<Ray_AIDarktoonusCyclopeusBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIDarktoonusCyclopeusBehavior_Template,617789785));
		aiBehaviorsFactory->RegisterObject<Ray_AIShooterEjectedBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterEjectedBehavior_Template,1940359400));
        aiBehaviorsFactory->RegisterObject<Ray_AIShooterStepDamageBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterStepDamageBehavior_Template,2847399885));
        aiBehaviorsFactory->RegisterObject<Ray_AIShooterFlyingBombBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterFlyingBombBehavior_Template,2272987957));
        aiBehaviorsFactory->RegisterObject<Ray_AIShooterVacuumBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterVacuumBehavior_Template,3152160776));
        aiBehaviorsFactory->RegisterObject<Ray_AIHitWallBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIHitWallBehavior_Template,2941850052));
        aiBehaviorsFactory->RegisterObject<Ray_AIBonusSkullBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBonusSkullBehavior_Template,3820934562));
        aiBehaviorsFactory->RegisterObject<Ray_AIUtensilTrapBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIUtensilTrapBehavior_Template,1432217607));
        aiBehaviorsFactory->RegisterObject<Ray_AIFriendly_WaitBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIFriendly_WaitBehavior_Template,4187183285));
        aiBehaviorsFactory->RegisterObject<Ray_AIFriendly_GoToTargetBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIFriendly_GoToTargetBehavior_Template,3839109425));
        aiBehaviorsFactory->RegisterObject<Ray_AIFriendly_JumpToTargetBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIFriendly_JumpToTargetBehavior_Template,4048975673));
        aiBehaviorsFactory->RegisterObject<Ray_AIRestoreAngleBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIRestoreAngleBehavior_Template,3042698962));
        aiBehaviorsFactory->RegisterObject<Ray_AIAlInfernoStaticBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIAlInfernoStaticBehavior_Template,190919790));
        aiBehaviorsFactory->RegisterObject<Ray_AIAlInfernoPatrolBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIAlInfernoPatrolBehavior_Template,753511999));
        aiBehaviorsFactory->RegisterObject<Ray_AILookatBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AILookatBehavior_Template,1462428637));
        aiBehaviorsFactory->RegisterObject<Ray_AIBlowingBirdLookatBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBlowingBirdLookatBehavior_Template,927686986));
        aiBehaviorsFactory->RegisterObject<Ray_AIHarissaToggleBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIHarissaToggleBehavior_Template,101396024));
        aiBehaviorsFactory->RegisterObject<Ray_AIUmbrellaBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIUmbrellaBehavior_Template,1503834881));
        aiBehaviorsFactory->RegisterObject<Ray_AIAlInfernoWaiterBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIAlInfernoWaiterBehavior_Template,3313947922));
        aiBehaviorsFactory->RegisterObject<Ray_AIBlowFishBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBlowFishBehavior_Template,3094051622));
        aiBehaviorsFactory->RegisterObject<Ray_AIOceanSpiderBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIOceanSpiderBehavior_Template,1883528063));
        aiBehaviorsFactory->RegisterObject<Ray_AISwordFishBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AISwordFishBehavior_Template,3772646552));
        aiBehaviorsFactory->RegisterObject<Ray_AIFishBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIFishBehavior_Template,2455270684));
        aiBehaviorsFactory->RegisterObject<Ray_AIRobotPatrolBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIRobotPatrolBehavior_Template,126022324));
        aiBehaviorsFactory->RegisterObject<Ray_AIBoulderSentBackBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIBoulderSentBackBehavior_Template,3800231555));
        aiBehaviorsFactory->RegisterObject<Ray_AIStoneManAttackBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_AIStoneManAttackBehavior_Template,2380287857));
        aiBehaviorsFactory->RegisterObject<Ray_TextBubbleBehavior_Template>(ITF_GET_STRINGID_CRC(Ray_TextBubbleBehavior_Template,300230979));

        // <CASA::VITA
        aiBehaviorsFactory->RegisterObject<RayVita_AIRelicBehavior_Template>(ITF_GET_STRINGID_CRC(RayVita_AIRelicBehavior_Template, 357578962));
        // CASA::VITA>
    }

    void registerAiActionsFactory(ObjectFactory* aiActionsFactory)
    {
        // AI Actions
        aiActionsFactory->RegisterObject<Ray_AIBasicBulletAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIBasicBulletAction_Template,2719645349));
        aiActionsFactory->RegisterObject<Ray_AIBlowOffAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIBlowOffAction_Template,1849869871));
        aiActionsFactory->RegisterObject<Ray_AIFlyIdleAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIFlyIdleAction_Template,3616031432));
        aiActionsFactory->RegisterObject<Ray_AIFollowBezierCurveAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIFollowBezierCurveAction_Template,1031333392));
        aiActionsFactory->RegisterObject<Ray_AIPerformHitPunchAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIPerformHitPunchAction_Template,2190342275));
        aiActionsFactory->RegisterObject<Ray_AIPerformHitPolylinePunchAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIPerformHitPolylinePunchAction_Template,2136054497));
        aiActionsFactory->RegisterObject<Ray_AIPerformHitPolylineJumpPunchAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIPerformHitPolylineJumpPunchAction_Template,1280052077));
        aiActionsFactory->RegisterObject<Ray_AIDarktoonificationAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIDarktoonificationAction_Template,347652341));
        aiActionsFactory->RegisterObject<Ray_AIShooterLaunchBulletAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterLaunchBulletAction_Template,546109621));
        aiActionsFactory->RegisterObject<Ray_AIHunterLaunchBulletAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIHunterLaunchBulletAction_Template,4139579893));
        aiActionsFactory->RegisterObject<Ray_AICartoonFallAction_Template>(ITF_GET_STRINGID_CRC(Ray_AICartoonFallAction_Template,586916384));
        aiActionsFactory->RegisterObject<Ray_AICornFloatAction_Template>(ITF_GET_STRINGID_CRC(Ray_AICornFloatAction_Template,3649626779));
        aiActionsFactory->RegisterObject<Ray_AIReceiveCameraEjectHitAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIReceiveCameraEjectHitAction_Template,383277539));
        aiActionsFactory->RegisterObject<Ray_AIReceiveHitAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIReceiveHitAction_Template,419071238));
        aiActionsFactory->RegisterObject<Ray_AIGround_ReceiveNormalHitAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIGround_ReceiveNormalHitAction_Template,2224777414));
        aiActionsFactory->RegisterObject<Ray_AIGround_ReceiveUpperHitAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIGround_ReceiveUpperHitAction_Template,4226911414));
        aiActionsFactory->RegisterObject<Ray_AIGround_ReceiveInAirHitAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIGround_ReceiveInAirHitAction_Template,2127135186));
        aiActionsFactory->RegisterObject<Ray_AIGround_ReceiveEarthquakeHitAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIGround_ReceiveEarthquakeHitAction_Template,1744635826));
        aiActionsFactory->RegisterObject<Ray_AIShooterReceiveHitAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterReceiveHitAction_Template,656935159));
        aiActionsFactory->RegisterObject<Ray_AIWaterFloatAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIWaterFloatAction_Template,1089754944));
        aiActionsFactory->RegisterObject<Ray_AIShooterProjectileAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIShooterProjectileAction_Template,2097305397));
        aiActionsFactory->RegisterObject<Ray_AIGeyserAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIGeyserAction_Template,1308112739));
        aiActionsFactory->RegisterObject<Ray_AIExplodeAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIExplodeAction_Template,3552256988));
        aiActionsFactory->RegisterObject<Ray_AILightningAction_Template>(ITF_GET_STRINGID_CRC(Ray_AILightningAction_Template,302679119));
        aiActionsFactory->RegisterObject<Ray_AiPerformLightningAction_Template>(ITF_GET_STRINGID_CRC(Ray_AiPerformLightningAction_Template,1604504864));
        aiActionsFactory->RegisterObject<Ray_AIGround_ReceiveHitLaunchBulletAction_Template>(ITF_GET_STRINGID_CRC(Ray_AIGround_ReceiveHitLaunchBulletAction_Template,648514903));
    }

    void registerEvents_Object1(ObjectFactory* eventsFactory)
    {
        // Events
        eventsFactory->RegisterObject<Ray_EventFruitAttachQuery>(ITF_GET_STRINGID_CRC(Ray_EventFruitAttachQuery,1115792865));
        eventsFactory->RegisterObject<Ray_EventFruitDetach>(ITF_GET_STRINGID_CRC(Ray_EventFruitDetach,60508415));
        eventsFactory->RegisterObject<Ray_EventPowerUp>(ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764));
        eventsFactory->RegisterObject<Ray_EventPlayLockAnim>(ITF_GET_STRINGID_CRC(Ray_EventPlayLockAnim,3679647184));
        eventsFactory->RegisterObject<Ray_EventSwingLatch>(ITF_GET_STRINGID_CRC(Ray_EventSwingLatch,2013309652));
        eventsFactory->RegisterObject<Ray_EventSwingDetach>(ITF_GET_STRINGID_CRC(Ray_EventSwingDetach,3105940814));
        eventsFactory->RegisterObject<Ray_EventQueryIsAccrobatic>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsAccrobatic,1083236336));
        eventsFactory->RegisterObject<Ray_EventQueryIsReceivingHit>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsReceivingHit,1252094534));
        eventsFactory->RegisterObject<Ray_EventHangOnCharacter>(ITF_GET_STRINGID_CRC(Ray_EventHangOnCharacter,1277553679));
        eventsFactory->RegisterObject<Ray_EventQueryHangInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryHangInfo,4077208499));
        eventsFactory->RegisterObject<Ray_EventQueryStanceInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryStanceInfo,1627894922));
        eventsFactory->RegisterObject<Ray_EventHangCoopJump>(ITF_GET_STRINGID_CRC(Ray_EventHangCoopJump,930438087));
        eventsFactory->RegisterObject<Ray_EventSetPickedUp>(ITF_GET_STRINGID_CRC(Ray_EventSetPickedUp,2186198130));
        eventsFactory->RegisterObject<Ray_EventReuse>(ITF_GET_STRINGID_CRC(Ray_EventReuse,3973034730));
        eventsFactory->RegisterObject<Ray_EventSpawnRewardLum>(ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722));
        eventsFactory->RegisterObject<Ray_EventSpawnRewardTun>(ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardTun,1506323374));
        eventsFactory->RegisterObject<Ray_EventSpawnRewardHeart>(ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardHeart,1546601933));
        eventsFactory->RegisterObject<Ray_EventLumEjection>(ITF_GET_STRINGID_CRC(Ray_EventLumEjection,368805757));
        eventsFactory->RegisterObject<Ray_EventBossSpawnActor>(ITF_GET_STRINGID_CRC(Ray_EventBossSpawnActor,2063507162));
        eventsFactory->RegisterObject<Ray_EventRewardPickedUp>(ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225));
        eventsFactory->RegisterObject<Ray_EventSetDeathReward>(ITF_GET_STRINGID_CRC(Ray_EventSetDeathReward,2743373648));
        eventsFactory->RegisterObject<Ray_EventMagnetTrigger>(ITF_GET_STRINGID_CRC(Ray_EventMagnetTrigger,106997702));
        eventsFactory->RegisterObject<Ray_EventQueryCanBeAttacked>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644));
        eventsFactory->RegisterObject<Ray_EventAttackGranted>(ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789));
        eventsFactory->RegisterObject<Ray_EventMainLumDestroyed>(ITF_GET_STRINGID_CRC(Ray_EventMainLumDestroyed,2883039676));
        eventsFactory->RegisterObject<Ray_EventQueryDarktoonificationPosition>(ITF_GET_STRINGID_CRC(Ray_EventQueryDarktoonificationPosition,785050175));
        eventsFactory->RegisterObject<Ray_EventQueryCanDarktoonify>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanDarktoonify,1194771444));
        eventsFactory->RegisterObject<Ray_EventUndarktoonify>(ITF_GET_STRINGID_CRC(Ray_EventUndarktoonify,2297042636));
        eventsFactory->RegisterObject<Ray_EventDance>(ITF_GET_STRINGID_CRC(Ray_EventDance,83576490));
        eventsFactory->RegisterObject<Ray_DoorTeleport>(ITF_GET_STRINGID_CRC(Ray_DoorTeleport,3845549645));
        eventsFactory->RegisterObject<Ray_EventSetInitialNode>(ITF_GET_STRINGID_CRC(Ray_EventSetInitialNode,3466697001));
        eventsFactory->RegisterObject<Ray_EventSetNodeSelector>(ITF_GET_STRINGID_CRC(Ray_EventSetNodeSelector,116563250));
        eventsFactory->RegisterObject<Ray_EventQueryCameraLimiter>(ITF_GET_STRINGID_CRC(Ray_EventQueryCameraLimiter,358012532));
        eventsFactory->RegisterObject<Ray_EventChestUpsideDown>(ITF_GET_STRINGID_CRC(Ray_EventChestUpsideDown,186946345));
        eventsFactory->RegisterObject<Ray_EventNodeReached>(ITF_GET_STRINGID_CRC(Ray_EventNodeReached,2473939155));
        eventsFactory->RegisterObject<Ray_EventBossMorayNodeReached>(ITF_GET_STRINGID_CRC(Ray_EventBossMorayNodeReached,491301045));
        eventsFactory->RegisterObject<Ray_EventBossMorayFireMissile>(ITF_GET_STRINGID_CRC(Ray_EventBossMorayFireMissile,1956304621));
        eventsFactory->RegisterObject<Ray_EventTriggerVictory>(ITF_GET_STRINGID_CRC(Ray_EventTriggerVictory,1674293021));
        eventsFactory->RegisterObject<Ray_EventChangePage>(ITF_GET_STRINGID_CRC(Ray_EventChangePage,1004490573));
        eventsFactory->RegisterObject<Ray_EventChangePageQueryPoints>(ITF_GET_STRINGID_CRC(Ray_EventChangePageQueryPoints,1998255083));
        eventsFactory->RegisterObject<Ray_EventMoveToPoint>(ITF_GET_STRINGID_CRC(Ray_EventMoveToPoint,1570511527));
        eventsFactory->RegisterObject<Ray_EventResetMoveToPoint>(ITF_GET_STRINGID_CRC(Ray_EventResetMoveToPoint,918485638));
        eventsFactory->RegisterObject<Ray_EventQueryDoorData>(ITF_GET_STRINGID_CRC(Ray_EventQueryDoorData,2144329540));
        eventsFactory->RegisterObject<Ray_EventShowUI>(ITF_GET_STRINGID_CRC(Ray_EventShowUI,3318916553));
        eventsFactory->RegisterObject<Ray_EventShowElectoonMedal>(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonMedal,1324502185));
        eventsFactory->RegisterObject<Ray_EventReleaseChild>(ITF_GET_STRINGID_CRC(Ray_EventReleaseChild,251461974));
        eventsFactory->RegisterObject<Ray_EventUturn>(ITF_GET_STRINGID_CRC(Ray_EventUturn,121408696));
        eventsFactory->RegisterObject<Ray_EventSpawn>(ITF_GET_STRINGID_CRC(Ray_EventSpawn,3403067276));
        eventsFactory->RegisterObject<Ray_EventAddLum>(ITF_GET_STRINGID_CRC(Ray_EventAddLum,4227734836));
        eventsFactory->RegisterObject<Ray_EventLumReachesScore>(ITF_GET_STRINGID_CRC(Ray_EventLumReachesScore,1881707556));
        eventsFactory->RegisterObject<Ray_EventQueryHunterNode>(ITF_GET_STRINGID_CRC(Ray_EventQueryHunterNode,210353395));
        eventsFactory->RegisterObject<Ray_EventQueryReachedMoveToPoint>(ITF_GET_STRINGID_CRC(Ray_EventQueryReachedMoveToPoint,1707458262));
        eventsFactory->RegisterObject<Ray_EventQueryLumStage>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumStage,4112170611));
        eventsFactory->RegisterObject<Ray_EventBounceToLayer>(ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527));
        eventsFactory->RegisterObject<Ray_EventTriggerBounce>(ITF_GET_STRINGID_CRC(Ray_EventTriggerBounce,1890727905));
        eventsFactory->RegisterObject<Ray_EventJumpToPos>(ITF_GET_STRINGID_CRC(Ray_EventJumpToPos,981487617));
        eventsFactory->RegisterObject<Ray_EventShooterVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumed,1513162250));
        eventsFactory->RegisterObject<Ray_EventShooterSwallow>(ITF_GET_STRINGID_CRC(Ray_EventShooterSwallow,48834168));
        eventsFactory->RegisterObject<Ray_EventShooterVacuumReleased>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumReleased,1672774138));
        eventsFactory->RegisterObject<Ray_EventShooterEjectActor>(ITF_GET_STRINGID_CRC(Ray_EventShooterEjectActor,937196133));
        eventsFactory->RegisterObject<Ray_EventCanBeVacuum>(ITF_GET_STRINGID_CRC(Ray_EventCanBeVacuum,774659105));
        eventsFactory->RegisterObject<Ray_EventQueryHasBeenVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventQueryHasBeenVacuumed,2297394390));
        eventsFactory->RegisterObject<Ray_EventIsInfluencedByVacuum>(ITF_GET_STRINGID_CRC(Ray_EventIsInfluencedByVacuum,1958637106));
        eventsFactory->RegisterObject<Ray_EventHasBeenVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventHasBeenVacuumed,1042014523));
        eventsFactory->RegisterObject<Ray_EventShooterCamera_TransitionNotify>(ITF_GET_STRINGID_CRC(Ray_EventShooterCamera_TransitionNotify,501414184));
        eventsFactory->RegisterObject<Ray_EventShooterLandingRequested>(ITF_GET_STRINGID_CRC(Ray_EventShooterLandingRequested,492783699));
        eventsFactory->RegisterObject<Ray_EventShooterVacuumBhvChangeState>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumBhvChangeState,2152722158));
        eventsFactory->RegisterObject<Ray_EventShooterInvulnerableActor>(ITF_GET_STRINGID_CRC(Ray_EventShooterInvulnerableActor,1840838639));
        eventsFactory->RegisterObject<Ray_EventShooterSpawnerDataSet>(ITF_GET_STRINGID_CRC(Ray_EventShooterSpawnerDataSet,538828432));
        eventsFactory->RegisterObject<Ray_EventShooterBossDataSet>(ITF_GET_STRINGID_CRC(Ray_EventShooterBossDataSet,3071700792));
        eventsFactory->RegisterObject<Ray_EventQueryChildLaunch>(ITF_GET_STRINGID_CRC(Ray_EventQueryChildLaunch,3130623924));
        eventsFactory->RegisterObject<Ray_EventChildLaunch>(ITF_GET_STRINGID_CRC(Ray_EventChildLaunch,2714500499));
        eventsFactory->RegisterObject<Ray_EventChildLaunchAll>(ITF_GET_STRINGID_CRC(Ray_EventChildLaunchAll,1581939056));
        eventsFactory->RegisterObject<Ray_EventBulletLaunch>(ITF_GET_STRINGID_CRC(Ray_EventBulletLaunch,1261820746));
        eventsFactory->RegisterObject<Ray_EventSetBubblePrizeRewardNumber>(ITF_GET_STRINGID_CRC(Ray_EventSetBubblePrizeRewardNumber,2143348728));
        eventsFactory->RegisterObject<Ray_EventHunterBirdBind>(ITF_GET_STRINGID_CRC(Ray_EventHunterBirdBind,135728216));
        eventsFactory->RegisterObject<Ray_EventHunterBirdUnbind>(ITF_GET_STRINGID_CRC(Ray_EventHunterBirdUnbind,1029386227));
    }

    void registerEvents_Object2(ObjectFactory* eventsFactory)
    {
        // Events
        eventsFactory->RegisterObject<Ray_EventQueryAIData>(ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000));
        eventsFactory->RegisterObject<Ray_EventSuperPunchLaunch>(ITF_GET_STRINGID_CRC(Ray_EventSuperPunchLaunch,1975677917));
        eventsFactory->RegisterObject<Ray_EventAIOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIOrder,1055366852));
        eventsFactory->RegisterObject<Ray_EventAIOrderProcessed>(ITF_GET_STRINGID_CRC(Ray_EventAIOrderProcessed,733417094));
        eventsFactory->RegisterObject<Ray_EventAIQueryNextOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIQueryNextOrder,2468917623));
        eventsFactory->RegisterObject<Ray_EventInstantKill>(ITF_GET_STRINGID_CRC(Ray_EventInstantKill,2400145155));
        eventsFactory->RegisterObject<Ray_EventSnapDance>(ITF_GET_STRINGID_CRC(Ray_EventSnapDance,806811309));
        eventsFactory->RegisterObject<Ray_EventHandsCaught>(ITF_GET_STRINGID_CRC(Ray_EventHandsCaught,3210089466));
        eventsFactory->RegisterObject<Ray_EventHandsRelease>(ITF_GET_STRINGID_CRC(Ray_EventHandsRelease,579391720));
        eventsFactory->RegisterObject<Ray_EventCaughtInPipe>(ITF_GET_STRINGID_CRC(Ray_EventCaughtInPipe,1030315255));
        eventsFactory->RegisterObject<Ray_EventTeleportUsed>(ITF_GET_STRINGID_CRC(Ray_EventTeleportUsed,75030505));
        eventsFactory->RegisterObject<Ray_EventTriggerChangePage>(ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481));
        eventsFactory->RegisterObject<Ray_EventTriggerEnterDoor>(ITF_GET_STRINGID_CRC(Ray_EventTriggerEnterDoor,849805494));
        eventsFactory->RegisterObject<Ray_EventTriggerStargate>(ITF_GET_STRINGID_CRC(Ray_EventTriggerStargate,3125962121));
        eventsFactory->RegisterObject<Ray_EventCanDanceOnStone>(ITF_GET_STRINGID_CRC(Ray_EventCanDanceOnStone,1732983345));
        eventsFactory->RegisterObject<Ray_EventQueryEmptyStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899));
        eventsFactory->RegisterObject<Ray_EventStoneActivationFinished>(ITF_GET_STRINGID_CRC(Ray_EventStoneActivationFinished,1905511856));
        eventsFactory->RegisterObject<Ray_EventQueryIsInPedestal>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsInPedestal,2146089474));
        eventsFactory->RegisterObject<Ray_MusicScoreOpenEvent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreOpenEvent,2109898583));
        eventsFactory->RegisterObject<Ray_MusicScoreCloseEvent>(ITF_GET_STRINGID_CRC(Ray_MusicScoreCloseEvent,2878829646));
        eventsFactory->RegisterObject<Ray_EventRestoreDeath>(ITF_GET_STRINGID_CRC(Ray_EventRestoreDeath,3768337963));
        eventsFactory->RegisterObject<Ray_EventQueryCanJumpFromCurrent>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanJumpFromCurrent,1690201300));
        eventsFactory->RegisterObject<Ray_EventActivateStone>(ITF_GET_STRINGID_CRC(Ray_EventActivateStone,2310010701));
        eventsFactory->RegisterObject<Ray_EventQueryCanActivateStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanActivateStone,1176450728));
        eventsFactory->RegisterObject<Ray_EventStargateUse>(ITF_GET_STRINGID_CRC(Ray_EventStargateUse,3921841867));
        eventsFactory->RegisterObject<Ray_EventStargateStageFinished>(ITF_GET_STRINGID_CRC(Ray_EventStargateStageFinished,211966772));
        eventsFactory->RegisterObject<Ray_EventBouncePropagate>(ITF_GET_STRINGID_CRC(Ray_EventBouncePropagate,889182957));
        eventsFactory->RegisterObject<Ray_EventAIOrderBT>(ITF_GET_STRINGID_CRC(Ray_EventAIOrderBT,2497541985));
        eventsFactory->RegisterObject<Ray_EventAIBTSetDetectionArea>(ITF_GET_STRINGID_CRC(Ray_EventAIBTSetDetectionArea,208226879));
        eventsFactory->RegisterObject<Ray_EventDisableAIOrderBT>(ITF_GET_STRINGID_CRC(Ray_EventDisableAIOrderBT,2752846623));
        eventsFactory->RegisterObject<Ray_EventAINetworkWaypoint>(ITF_GET_STRINGID_CRC(Ray_EventAINetworkWaypoint,3032038608));
        eventsFactory->RegisterObject<Ray_EventMedalCinematicEnded>(ITF_GET_STRINGID_CRC(Ray_EventMedalCinematicEnded,1196816250));
        eventsFactory->RegisterObject<Ray_EventMedalNonCinematicEnded>(ITF_GET_STRINGID_CRC(Ray_EventMedalNonCinematicEnded,694861796));
        eventsFactory->RegisterObject<Ray_EventSpawnWave>(ITF_GET_STRINGID_CRC(Ray_EventSpawnWave,588272106));
        eventsFactory->RegisterObject<Ray_EventWaveGeneratorNotification>(ITF_GET_STRINGID_CRC(Ray_EventWaveGeneratorNotification,1312585498));
        eventsFactory->RegisterObject<Ray_EventDynamicFogParamSet>(ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamSet,2084909099));
        eventsFactory->RegisterObject<Ray_EventDynamicFogParamQuery>(ITF_GET_STRINGID_CRC(Ray_EventDynamicFogParamQuery,2314851366));
        eventsFactory->RegisterObject<Ray_EventQueryWindTunnelInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryWindTunnelInfo,3603812346));
        eventsFactory->RegisterObject<Ray_EventQueryGeyserBehavior>(ITF_GET_STRINGID_CRC(Ray_EventQueryGeyserBehavior,2840355382));
        eventsFactory->RegisterObject<Ray_EventRescueFriendly>(ITF_GET_STRINGID_CRC(Ray_EventRescueFriendly,777320120));
        eventsFactory->RegisterObject<Ray_EventQueryIsRescued>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsRescued,3718040811));
        eventsFactory->RegisterObject<Ray_EventBreakableBreak>(ITF_GET_STRINGID_CRC(Ray_EventBreakableBreak,4257664379));
        eventsFactory->RegisterObject<Ray_EventBreakableQuery>(ITF_GET_STRINGID_CRC(Ray_EventBreakableQuery,4136568804));
        eventsFactory->RegisterObject<Ray_EventSetSpikyFlower>(ITF_GET_STRINGID_CRC(Ray_EventSetSpikyFlower,4136114510));
        eventsFactory->RegisterObject<Ray_EventQueryOpen>(ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153));
        eventsFactory->RegisterObject<Ray_EventAddSoftCollForce>(ITF_GET_STRINGID_CRC(Ray_EventAddSoftCollForce,2624804693));
        eventsFactory->RegisterObject<Ray_EventScaleDoorTrigger>(ITF_GET_STRINGID_CRC(Ray_EventScaleDoorTrigger,3012042865));
        eventsFactory->RegisterObject<Ray_EventScaleFunnelTrigger>(ITF_GET_STRINGID_CRC(Ray_EventScaleFunnelTrigger,2532379169));
        eventsFactory->RegisterObject<Ray_EventScaleTunnelTrigger>(ITF_GET_STRINGID_CRC(Ray_EventScaleTunnelTrigger,2369480741));
        eventsFactory->RegisterObject<Ray_EventCheckPedestalSupport>(ITF_GET_STRINGID_CRC(Ray_EventCheckPedestalSupport,1578355683));
        eventsFactory->RegisterObject<Ray_EventQueryPedestalInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryPedestalInfo,2107418028));
        eventsFactory->RegisterObject<Ray_EventReleasePedestal>(ITF_GET_STRINGID_CRC(Ray_EventReleasePedestal,4284763324));
        eventsFactory->RegisterObject<Ray_EventReinitializeNPC>(ITF_GET_STRINGID_CRC(Ray_EventReinitializeNPC,1176517542));
        eventsFactory->RegisterObject<Ray_EventQueryPaintTrail>(ITF_GET_STRINGID_CRC(Ray_EventQueryPaintTrail,3721333601));
        eventsFactory->RegisterObject<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482));
        eventsFactory->RegisterObject<Ray_EventRegisterBubblePrizeHub>(ITF_GET_STRINGID_CRC(Ray_EventRegisterBubblePrizeHub,3108685932));
        eventsFactory->RegisterObject<Ray_EventNotifyBubblePrizeHub>(ITF_GET_STRINGID_CRC(Ray_EventNotifyBubblePrizeHub,3264814460));
        eventsFactory->RegisterObject<Ray_EventQueryHitType>(ITF_GET_STRINGID_CRC(Ray_EventQueryHitType,4146626226));
        eventsFactory->RegisterObject<Ray_EventScoreGuageAddLum>(ITF_GET_STRINGID_CRC(Ray_EventScoreGuageAddLum,2833738877));
        eventsFactory->RegisterObject<Ray_EventScoreSetup>(ITF_GET_STRINGID_CRC(Ray_EventScoreSetup,427760862));
        eventsFactory->RegisterObject<Ray_EventGoToWorldMap>(ITF_GET_STRINGID_CRC(Ray_EventGoToWorldMap,3334813547));
        eventsFactory->RegisterObject<Ray_EventStoneManLaunch>(ITF_GET_STRINGID_CRC(Ray_EventStoneManLaunch,3722106216));
        eventsFactory->RegisterObject<Ray_EventTakeTooth>(ITF_GET_STRINGID_CRC(Ray_EventTakeTooth,2760419748));
        eventsFactory->RegisterObject<Ray_EventQueryDisableAirControlInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryDisableAirControlInfo,1557318448));
        eventsFactory->RegisterObject<Ray_EventReturnTeeth>(ITF_GET_STRINGID_CRC(Ray_EventReturnTeeth,810560716));
        eventsFactory->RegisterObject<Ray_EventHunterBulletLaunched>(ITF_GET_STRINGID_CRC(Ray_EventHunterBulletLaunched,1767723773));
        eventsFactory->RegisterObject<Ray_EventPlayAnimState>(ITF_GET_STRINGID_CRC(Ray_EventPlayAnimState,4089549703));
        eventsFactory->RegisterObject<Ray_EventDyingStoneMan>(ITF_GET_STRINGID_CRC(Ray_EventDyingStoneMan,1834526668));
        eventsFactory->RegisterObject<Ray_EventWorldMapUnlock>(ITF_GET_STRINGID_CRC(Ray_EventWorldMapUnlock,1931876316));
        eventsFactory->RegisterObject<Ray_EventEndSequence>(ITF_GET_STRINGID_CRC(Ray_EventEndSequence,3991450529));
        eventsFactory->RegisterObject<Ray_EventFxScreen>(ITF_GET_STRINGID_CRC(Ray_EventFxScreen,362679351));
        eventsFactory->RegisterObject<Ray_EventTriggerStartTimeAttack>(ITF_GET_STRINGID_CRC(Ray_EventTriggerStartTimeAttack,3727680095));
        eventsFactory->RegisterObject<Ray_EventShowElectoonToothScore>(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonToothScore,116023213));
        eventsFactory->RegisterObject<Ray_EventFetchElectoonToothScoreInfos>(ITF_GET_STRINGID_CRC(Ray_EventFetchElectoonToothScoreInfos,3242513602));
        eventsFactory->RegisterObject<Ray_EventWMOpenPaths>(ITF_GET_STRINGID_CRC(Ray_EventWMOpenPaths,2918906348));
        eventsFactory->RegisterObject<Ray_MoveOnPolylineFromNautilus>(ITF_GET_STRINGID_CRC(Ray_MoveOnPolylineFromNautilus,1238635980));
        eventsFactory->RegisterObject<Ray_EventFetchWMOpenPaths>(ITF_GET_STRINGID_CRC(Ray_EventFetchWMOpenPaths,2134285703));
        eventsFactory->RegisterObject<Ray_EventWMDisplayWorldRecap>(ITF_GET_STRINGID_CRC(Ray_EventWMDisplayWorldRecap,554350578));
    }

    void registerEvents_Misc(ObjectFactory* eventsFactory)
    {
        // HUD
        eventsFactory->RegisterObject<Ray_EventHUDDisplayElem>(ITF_GET_STRINGID_CRC(Ray_EventHUDDisplayElem,2571036057));
        eventsFactory->RegisterObject<Ray_EventHUDSetText>(ITF_GET_STRINGID_CRC(Ray_EventHUDSetText,1340493890));

        // CASA::CTR::SFS
        eventsFactory->RegisterObject<EventHurtNotification>(ITF_GET_STRINGID_CRC(EventHurtNotification,758119956));
        eventsFactory->RegisterObject<EventPickRewardNotification>(ITF_GET_STRINGID_CRC(EventPickRewardNotification,1782638909));
        // CASA>

        // Stims
        eventsFactory->RegisterObject<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519));
        eventsFactory->RegisterObject<BounceStim>(ITF_GET_STRINGID_CRC(BounceStim,819805069));
        eventsFactory->RegisterObject<RehitStim>(ITF_GET_STRINGID_CRC(RehitStim,1008690800));
        eventsFactory->RegisterObject<ScareStim>(ITF_GET_STRINGID_CRC(ScareStim,499118231));
        eventsFactory->RegisterObject<VacuumStim>(ITF_GET_STRINGID_CRC(VacuumStim,593568508));
    }

    void registerEvents(ObjectFactory* eventsFactory)
    {
        registerEvents_Object1(eventsFactory);
        registerEvents_Object2(eventsFactory);
        registerEvents_Misc(eventsFactory);
    }

    void registerTemplatesFactory_Template_1(ObjectFactory* templatesFactory)
    {
        // Templates
        templatesFactory->RegisterObject<Ray_GroundAIControllerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_GroundAIControllerComponent_Template,748333989));
        templatesFactory->RegisterObject<Ray_PlayerControllerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerControllerComponent_Template,2964336576));
        templatesFactory->RegisterObject<Ray_PlayerCostumeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerCostumeComponent_Template,823202235));
        templatesFactory->RegisterObject<Ray_PlayerCostumeManagerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerCostumeManagerComponent_Template,3433372874));
        templatesFactory->RegisterObject<Ray_PlayerCostumePriceComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerCostumePriceComponent_Template,3369407398));
        templatesFactory->RegisterObject<Ray_ConceptGalleryComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ConceptGalleryComponent_Template,1792344745));
        templatesFactory->RegisterObject<Ray_ChangeRoomDoorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ChangeRoomDoorComponent_Template,1528141307));
        templatesFactory->RegisterObject<Ray_CharacterDebuggerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CharacterDebuggerComponent_Template,2645970871));
        templatesFactory->RegisterObject<Ray_SwingComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SwingComponent_Template,190444179));
        templatesFactory->RegisterObject<Ray_SwingRopeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SwingRopeComponent_Template,3572690544));
        templatesFactory->RegisterObject<Ray_DoorTeleportComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DoorTeleportComponent_Template,1716901842));
        templatesFactory->RegisterObject<Ray_LandOfTheDeadTeleportComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LandOfTheDeadTeleportComponent_Template,657266927));
        templatesFactory->RegisterObject<Ray_DynamicFogComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DynamicFogComponent_Template,2089509264));
        templatesFactory->RegisterObject<Ray_EnterDoorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_EnterDoorComponent_Template,2048296090));
        templatesFactory->RegisterObject<Ray_StargateComponent_Template>(ITF_GET_STRINGID_CRC(Ray_StargateComponent_Template,2993411321));
        templatesFactory->RegisterObject<Ray_StargateNodeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_StargateNodeComponent_Template,521951556));
        templatesFactory->RegisterObject<Ray_ActivationSequenceStoneComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ActivationSequenceStoneComponent_Template,1879566921));
        templatesFactory->RegisterObject<Ray_ActivationSequenceVisualComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ActivationSequenceVisualComponent_Template,25680642));
        templatesFactory->RegisterObject<Ray_ActivationSequenceTriggerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ActivationSequenceTriggerComponent_Template,4164320951));
        templatesFactory->RegisterObject<Ray_ScaleTunnelComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ScaleTunnelComponent_Template,436792055));
        templatesFactory->RegisterObject<Ray_ScaleFunnelComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ScaleFunnelComponent_Template,1393552913));
        templatesFactory->RegisterObject<Ray_PauseActorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PauseActorComponent_Template,1478000500));
        templatesFactory->RegisterObject<Ray_BulletLauncherComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BulletLauncherComponent_Template,3782398456));
        templatesFactory->RegisterObject<Ray_CheckpointComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CheckpointComponent_Template,1279601388));
        templatesFactory->RegisterObject<Ray_ChildLaunchComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ChildLaunchComponent_Template,3683817819));
        templatesFactory->RegisterObject<Ray_TriggerBounceComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TriggerBounceComponent_Template,557222769));
        templatesFactory->RegisterObject<Ray_TutorialComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TutorialComponent_Template,3930255658));
        templatesFactory->RegisterObject<Ray_WMContentUnlockComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WMContentUnlockComponent_Template,1150594739));
        templatesFactory->RegisterObject<Ray_LoadNotificationComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LoadNotificationComponent_Template,3491700814));
        templatesFactory->RegisterObject<Ray_SkipLevelComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SkipLevelComponent_Template,2063844644));
        templatesFactory->RegisterObject<Ray_LevelTitleComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LevelTitleComponent_Template,3531595838));

        templatesFactory->RegisterObject<Ray_WaterPerturbationComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WaterPerturbationComponent_Template,377221835));
        templatesFactory->RegisterObject<Ray_PlayerOffScreenIconComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerOffScreenIconComponent_Template,2699213758));
        templatesFactory->RegisterObject<Ray_PlayerDeadSoul2DComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerDeadSoul2DComponent_Template,373239380));
        templatesFactory->RegisterObject<Ray_WM_ControllerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_ControllerComponent_Template,723916428));
        templatesFactory->RegisterObject<Ray_PlayerHudScoreComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerHudScoreComponent_Template,4042481333));
        templatesFactory->RegisterObject<Ray_ShooterControllerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterControllerComponent_Template,687932757));
        templatesFactory->RegisterObject<Ray_ShooterStimComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterStimComponent_Template,3632123545));
        templatesFactory->RegisterObject<Ray_ShooterCameraComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterCameraComponent_Template,4112142190));
        templatesFactory->RegisterObject<Ray_ShooterCameraModifierComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterCameraModifierComponent_Template,1908345317));
        templatesFactory->RegisterObject<Ray_ShooterCheckPointComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterCheckPointComponent_Template,2750908377));
        templatesFactory->RegisterObject<Ray_ShooterDoorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterDoorComponent_Template,937978001));
        templatesFactory->RegisterObject<Ray_AvoidanceComponent_Template>(ITF_GET_STRINGID_CRC(Ray_AvoidanceComponent_Template,2626459762));
        templatesFactory->RegisterObject<Ray_SceneSettingsComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SceneSettingsComponent_Template,1819284364));
        templatesFactory->RegisterObject<Ray_DanceDoorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DanceDoorComponent_Template,2116776617));
        templatesFactory->RegisterObject<Ray_DanceStoneComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DanceStoneComponent_Template,2698312311));
        templatesFactory->RegisterObject<Ray_DanceEyeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DanceEyeComponent_Template,1445965177));
        templatesFactory->RegisterObject<Ray_PlatformAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlatformAIComponent_Template,3933484897));
        templatesFactory->RegisterObject<Ray_EyeAnimationComponent_Template>(ITF_GET_STRINGID_CRC(Ray_EyeAnimationComponent_Template,2374979028));
        templatesFactory->RegisterObject<Ray_RewardAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_RewardAIComponent_Template,281197817));
        templatesFactory->RegisterObject<Ray_SkullCoinComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SkullCoinComponent_Template,2707418192));
        templatesFactory->RegisterObject<Ray_BreakableAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BreakableAIComponent_Template,1253773257));
        templatesFactory->RegisterObject<Ray_BreakableStackManagerAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BreakableStackManagerAIComponent_Template,2521435442));
        templatesFactory->RegisterObject<Ray_BreakableStackElementAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BreakableStackElementAIComponent_Template,3047820808));
        templatesFactory->RegisterObject<Ray_BossBuboAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossBuboAIComponent_Template,805555326));
        templatesFactory->RegisterObject<Ray_BossMapEndAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossMapEndAIComponent_Template,410900912));
        templatesFactory->RegisterObject<Ray_BossPlantAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossPlantAIComponent_Template,1969325865));
        templatesFactory->RegisterObject<Ray_BossPlantNodeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossPlantNodeComponent_Template,1110965916));
        templatesFactory->RegisterObject<Ray_BossPlantArenaAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossPlantArenaAIComponent_Template,2688442976));
        templatesFactory->RegisterObject<Ray_BossBirdAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossBirdAIComponent_Template,2152536748));
        templatesFactory->RegisterObject<Ray_BossBirdPawnAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossBirdPawnAIComponent_Template,91834156));
        templatesFactory->RegisterObject<Ray_BossMorayAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossMorayAIComponent_Template,1024689649));
        templatesFactory->RegisterObject<Ray_BigMamaAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BigMamaAIComponent_Template,2869252870));
        templatesFactory->RegisterObject<Ray_MamaEyesAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MamaEyesAIComponent_Template,2499315905));
        templatesFactory->RegisterObject<Ray_BossMorayNodeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossMorayNodeComponent_Template,1261188335));
        templatesFactory->RegisterObject<Ray_BossCameraComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BossCameraComponent_Template,2656542915));
        templatesFactory->RegisterObject<Ray_StimComponent_Template>(ITF_GET_STRINGID_CRC(Ray_StimComponent_Template,4173636078));
        templatesFactory->RegisterObject<Ray_CameraLimiterComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CameraLimiterComponent_Template,4158279952));
        templatesFactory->RegisterObject<Ray_PlatformTreeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlatformTreeComponent_Template,485283312));
        templatesFactory->RegisterObject<Ray_PushButtonComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PushButtonComponent_Template,2466817767));
        templatesFactory->RegisterObject<Ray_ShooterAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterAIComponent_Template,4046682542));
        templatesFactory->RegisterObject<Ray_RedWizardAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_RedWizardAIComponent_Template,147929559));
        templatesFactory->RegisterObject<Ray_FriendlyAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FriendlyAIComponent_Template,610976079));
        templatesFactory->RegisterObject<Ray_BabyPiranhaAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BabyPiranhaAIComponent_Template,2202515726));
        templatesFactory->RegisterObject<Ray_DarkToonAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DarkToonAIComponent_Template,4087876507));
        templatesFactory->RegisterObject<Ray_DarktoonTrapAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DarktoonTrapAIComponent_Template,3610061445));
        templatesFactory->RegisterObject<Ray_LivingstoneHatAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LivingstoneHatAIComponent_Template,2261019325));
        templatesFactory->RegisterObject<Ray_BulletAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BulletAIComponent_Template,419291702));
        templatesFactory->RegisterObject<Ray_SeekingBulletAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SeekingBulletAIComponent_Template,1299182097));
        templatesFactory->RegisterObject<Ray_WalkingBulletAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WalkingBulletAIComponent_Template,3144728615));
        templatesFactory->RegisterObject<Ray_FruitAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FruitAIComponent_Template,392815706));
        templatesFactory->RegisterObject<Ray_GroundAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_GroundAIComponent_Template,981568444));
    }

    void registerTemplatesFactory_Template_2(ObjectFactory* templatesFactory)
    {
        // Templates
        templatesFactory->RegisterObject<Ray_SimpleAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SimpleAIComponent_Template,2631517923));
        templatesFactory->RegisterObject<Ray_CyclopeusAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CyclopeusAIComponent_Template,3214622220));
        templatesFactory->RegisterObject<Ray_DarkBirdAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DarkBirdAIComponent_Template,634680799));
        templatesFactory->RegisterObject<Ray_HunterBossNodeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_HunterBossNodeComponent_Template,2218509339));
        templatesFactory->RegisterObject<Ray_ConditionalTriggerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ConditionalTriggerComponent_Template,3900043245));
        templatesFactory->RegisterObject<Ray_HunterBossAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_HunterBossAIComponent_Template,3752367210));
        templatesFactory->RegisterObject<Ray_HunterHoleAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_HunterHoleAIComponent_Template,1634055986));
        templatesFactory->RegisterObject<Ray_ShooterTurretAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterTurretAIComponent_Template,3392503840));
        templatesFactory->RegisterObject<Ray_ShooterPirateShipAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterPirateShipAIComponent_Template,4045220477));
        templatesFactory->RegisterObject<Ray_ShooterGardianAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterGardianAIComponent_Template,2815141000));
        templatesFactory->RegisterObject<Ray_LivingStoneAIComponent2_Template>(ITF_GET_STRINGID_CRC(Ray_LivingStoneAIComponent2_Template,4086570798));
        templatesFactory->RegisterObject<Ray_GroundEnemyAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_GroundEnemyAIComponent_Template,1079235816));
        templatesFactory->RegisterObject<Ray_JanodAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_JanodAIComponent_Template,4131929867));
        templatesFactory->RegisterObject<Ray_PushedAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PushedAIComponent_Template,535185699));
        templatesFactory->RegisterObject<Ray_FishSwarmAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FishSwarmAIComponent_Template,1320950508));
        templatesFactory->RegisterObject<Ray_GolemAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_GolemAIComponent_Template,3787101293));
        templatesFactory->RegisterObject<Ray_CrystalKingAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CrystalKingAIComponent_Template,591226493));
        templatesFactory->RegisterObject<Ray_StargateManagerAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_StargateManagerAIComponent_Template,816597063));
        templatesFactory->RegisterObject<Ray_CreditsManagerAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CreditsManagerAIComponent_Template,2491834364));
        templatesFactory->RegisterObject<Ray_SwarmChaseAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SwarmChaseAIComponent_Template,4027380365));
        templatesFactory->RegisterObject<Ray_SwarmChaseNodeComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SwarmChaseNodeComponent_Template,1787337512));
        templatesFactory->RegisterObject<Ray_ElevatorAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ElevatorAIComponent_Template,674588016));
        templatesFactory->RegisterObject<Ray_ElevatorWheelAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ElevatorWheelAIComponent_Template,1816481909));
        templatesFactory->RegisterObject<Ray_ElevatorMonsterAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ElevatorMonsterAIComponent_Template,1241043754));
        templatesFactory->RegisterObject<Ray_LumMusicManagerAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LumMusicManagerAIComponent_Template,2203457311));
        templatesFactory->RegisterObject<Ray_WaterHandsAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WaterHandsAIComponent_Template,249265877));
        templatesFactory->RegisterObject<Ray_HandsAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_HandsAIComponent_Template,1264202302));
        templatesFactory->RegisterObject<Ray_GeyserPlatformAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_GeyserPlatformAIComponent_Template,2991729266));
        templatesFactory->RegisterObject<Ray_NautilusAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_NautilusAIComponent_Template,101315663));
        templatesFactory->RegisterObject<Ray_FriendlyBTAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FriendlyBTAIComponent_Template,1728992287));
        templatesFactory->RegisterObject<Ray_DeadGuyBTAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DeadGuyBTAIComponent_Template,2137520839));
        templatesFactory->RegisterObject<Ray_TalkingHatAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TalkingHatAIComponent_Template,4160581289));
        templatesFactory->RegisterObject<Ray_MurphyAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MurphyAIComponent_Template, 654353777));
        templatesFactory->RegisterObject<Ray_MurphyAssistAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MurphyAssistAIComponent_Template, 907154879));
        templatesFactory->RegisterObject<Ray_MurphyAssistBubbleAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MurphyAssistBubbleAIComponent_Template, 1710378469));
        templatesFactory->RegisterObject<Ray_ChronoAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ChronoAIComponent_Template,2735339634));
        templatesFactory->RegisterObject<Ray_TimeAttackHUDTimerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TimeAttackHUDTimerComponent_Template,2746967258));
        templatesFactory->RegisterObject<Ray_TimeAttackHUDResultsComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TimeAttackHUDResultsComponent_Template,900451392));
        templatesFactory->RegisterObject<Ray_TimeAttackFlagComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TimeAttackFlagComponent_Template,2516661283));
        templatesFactory->RegisterObject<Ray_TalkingBubbleComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TalkingBubbleComponent_Template,3566362660));
        templatesFactory->RegisterObject<Ray_TeensyRobotAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_TeensyRobotAIComponent_Template,842322961));
        templatesFactory->RegisterObject<Ray_AIBTOrderComponent_Template>(ITF_GET_STRINGID_CRC(Ray_AIBTOrderComponent_Template,1269576061));
        templatesFactory->RegisterObject<Ray_AINetworkComponent_Template>(ITF_GET_STRINGID_CRC(Ray_AINetworkComponent_Template,1829036950));
        templatesFactory->RegisterObject<Ray_MusicScoreAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MusicScoreAIComponent_Template,3374490094));
        templatesFactory->RegisterObject<Ray_FluidFallAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FluidFallAIComponent_Template,1727238213));
        templatesFactory->RegisterObject<Ray_FirePatchAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FirePatchAIComponent_Template,1625357308));
        templatesFactory->RegisterObject<Ray_JalapenoKingAiComponent_Template>(ITF_GET_STRINGID_CRC(Ray_JalapenoKingAiComponent_Template,405634581));
        templatesFactory->RegisterObject<Ray_MusicScoreSnapComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MusicScoreSnapComponent_Template,1953999640));
        templatesFactory->RegisterObject<Ray_ForceFieldComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ForceFieldComponent_Template,900118042));
        templatesFactory->RegisterObject<Ray_BreakableCageComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BreakableCageComponent_Template,2375806396));
        templatesFactory->RegisterObject<Ray_MultiPiecesActorAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MultiPiecesActorAIComponent_Template,3527430123));
        templatesFactory->RegisterObject<Ray_StringWaveFaderComponent_Template>(ITF_GET_STRINGID_CRC(Ray_StringWaveFaderComponent_Template,1145134707));
        templatesFactory->RegisterObject<Ray_SnakeAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SnakeAIComponent_Template,340373408));
        templatesFactory->RegisterObject<Ray_ShooterGardianMoray_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterGardianMoray_Template,1927756513));
		templatesFactory->RegisterObject<Ray_BlackSwarmComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BlackSwarmComponent_Template,344870632));
		templatesFactory->RegisterObject<Ray_BlackSwarmRepellerAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BlackSwarmRepellerAIComponent_Template,2331876365));
		templatesFactory->RegisterObject<Ray_BlackSwarmSpawnerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BlackSwarmSpawnerComponent_Template,3193127414));
        templatesFactory->RegisterObject<Ray_ElectoonTargetComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ElectoonTargetComponent_Template,853125459));
        templatesFactory->RegisterObject<Ray_WaveBallComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WaveBallComponent_Template,1561038693));
        templatesFactory->RegisterObject<Ray_SpikyBallComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SpikyBallComponent_Template,1442888449));
        templatesFactory->RegisterObject<Ray_SeekingJellyfishAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SeekingJellyfishAIComponent_Template,348013504));
        templatesFactory->RegisterObject<Ray_CageMapAmbianceComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CageMapAmbianceComponent_Template,1855819352));
        templatesFactory->RegisterObject<Ray_SinkingPlatformComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SinkingPlatformComponent_Template,2233895274));
        templatesFactory->RegisterObject<Ray_BasculePlatformComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BasculePlatformComponent_Template,3575633478));
        templatesFactory->RegisterObject<Ray_WaterFloatingPlatformComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WaterFloatingPlatformComponent_Template,2886171535));
        templatesFactory->RegisterObject<Ray_BubbleAiComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BubbleAiComponent_Template,3255795298));
        templatesFactory->RegisterObject<Ray_alTranquiloAiComponent_Template>(ITF_GET_STRINGID_CRC(Ray_alTranquiloAiComponent_Template,3178361393));
        templatesFactory->RegisterObject<Ray_SpikyShellTrapComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SpikyShellTrapComponent_Template,3861753903));
        templatesFactory->RegisterObject<Ray_OceanSpiderAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_OceanSpiderAIComponent_Template,3273024315));
        templatesFactory->RegisterObject<Ray_LightningGeneratorAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LightningGeneratorAIComponent_Template,1969464090));
        templatesFactory->RegisterObject<Ray_AnglerFishAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_AnglerFishAIComponent_Template,2006657463));
        templatesFactory->RegisterObject<Ray_AbyssalHandAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_AbyssalHandAIComponent_Template,959313734));
        templatesFactory->RegisterObject<Ray_AbyssLightComponent_Template>(ITF_GET_STRINGID_CRC(Ray_AbyssLightComponent_Template,1607664492));
        templatesFactory->RegisterObject<Ray_FireFlyKrillAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FireFlyKrillAIComponent_Template,4063470096));
        templatesFactory->RegisterObject<Ray_RockFishAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_RockFishAIComponent_Template,1700857009));
        templatesFactory->RegisterObject<Ray_FireFlyKrillSpawnerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FireFlyKrillSpawnerComponent_Template,3115126182));
        templatesFactory->RegisterObject<Ray_LightningPatchAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LightningPatchAIComponent_Template,302749681));
        templatesFactory->RegisterObject<Ray_BoulderAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BoulderAIComponent_Template,1007983663));
        templatesFactory->RegisterObject<Ray_ScoreGuageComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ScoreGuageComponent_Template,579944772));
        templatesFactory->RegisterObject<Ray_PlayerScoreComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlayerScoreComponent_Template,3944500506));
        templatesFactory->RegisterObject<Ray_StoneManAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_StoneManAIComponent_Template,3113601795));
        templatesFactory->RegisterObject<Ray_ScoreLumAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ScoreLumAIComponent_Template,3713735267));
        templatesFactory->RegisterObject<Ray_BubbleDreamerAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BubbleDreamerAIComponent_Template,1952946068));
        templatesFactory->RegisterObject<Ray_SpikyPlatformAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SpikyPlatformAIComponent_Template,2629081214));

		// <CASA::Vita
		templatesFactory->RegisterObject<RayVita_FrescoManagerAIComponent_Template>(ITF_GET_STRINGID_CRC(RayVita_FrescoManagerAIComponent_Template,1672407068));    
		// CASA::Vita>

    }

    void registerTemplatesFactory_Misc(ObjectFactory* templatesFactory)
    {
        // World Map
        templatesFactory->RegisterObject<Ray_WM_SpotComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_SpotComponent_Template,655616801));
        templatesFactory->RegisterObject<Ray_WM_WorldSpotComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_WorldSpotComponent_Template,2534612080));
        templatesFactory->RegisterObject<Ray_WM_LevelSpotComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_LevelSpotComponent_Template,657886431));
        templatesFactory->RegisterObject<Ray_WM_BonusLevelSpotComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_BonusLevelSpotComponent_Template,995170804));
        templatesFactory->RegisterObject<Ray_WM_GameStatsComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_GameStatsComponent_Template,2451436856));
        templatesFactory->RegisterObject<Ray_WM_LoadingScreenComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_LoadingScreenComponent_Template,3735061406));
        templatesFactory->RegisterObject<Ray_WM_FootpathComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_FootpathComponent_Template,2256037681));
        templatesFactory->RegisterObject<Ray_WM_PanelComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_PanelComponent_Template,615166642));
        templatesFactory->RegisterObject<Ray_UIFadeScreenComponent_Template>(ITF_GET_STRINGID_CRC(Ray_UIFadeScreenComponent_Template,3100936897));
        templatesFactory->RegisterObject<Ray_WM_WorldRecapComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_WorldRecapComponent_Template,2596140885));
        templatesFactory->RegisterObject<Ray_WM_LevelRecapComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_LevelRecapComponent_Template,1212497558));
        templatesFactory->RegisterObject<Ray_WM_NewComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_NewComponent_Template,1677538008));
        templatesFactory->RegisterObject<Ray_WM_ShowComponent_Template>(ITF_GET_STRINGID_CRC(Ray_WM_ShowComponent_Template,1699972322));

        //Misc
        templatesFactory->RegisterObject<Ray_PlatformerCameraComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlatformerCameraComponent_Template,714787660));
        templatesFactory->RegisterObject<Ray_BabyPiranhaComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BabyPiranhaComponent_Template,4085371851));
        templatesFactory->RegisterObject<Ray_SpawnerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SpawnerComponent_Template,1732933546));
        templatesFactory->RegisterObject<Ray_DarkArenaDoorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DarkArenaDoorComponent_Template,457719623));
        templatesFactory->RegisterObject<Ray_DarktoonTrapHoleComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DarktoonTrapHoleComponent_Template,1117493895));
        templatesFactory->RegisterObject<Ray_DispenserComponent_Template>(ITF_GET_STRINGID_CRC(Ray_DispenserComponent_Template,3725050653));
        templatesFactory->RegisterObject<Ray_FixedAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_FixedAIComponent_Template,3524586378));
        templatesFactory->RegisterObject<Ray_GeyserComponent_Template>(ITF_GET_STRINGID_CRC(Ray_GeyserComponent_Template,2330498979));
		templatesFactory->RegisterObject<Ray_ChangePageComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ChangePageComponent_Template,4146314714));
		templatesFactory->RegisterObject<Ray_CustomChangePageComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CustomChangePageComponent_Template,2001252747));
        templatesFactory->RegisterObject<Ray_ChangePageWithAnimComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ChangePageWithAnimComponent_Template,2263342643));
		templatesFactory->RegisterObject<Ray_ShooterActorParameterComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterActorParameterComponent_Template,2034283474));
        templatesFactory->RegisterObject<Ray_ShooterSpawnerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterSpawnerComponent_Template,2485681820));
        templatesFactory->RegisterObject<Ray_ShooterSpawnerModifierComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ShooterSpawnerModifierComponent_Template,153243771));
        templatesFactory->RegisterObject<Ray_StringWaveGeneratorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_StringWaveGeneratorComponent_Template,2558825937));
        templatesFactory->RegisterObject<Ray_SubAnchorComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SubAnchorComponent_Template,2416740633));
        templatesFactory->RegisterObject<Ray_MenuE3Component_Template>(ITF_GET_STRINGID_CRC(Ray_MenuE3Component_Template,3239430128));
        templatesFactory->RegisterObject<Ray_ScoreBoardComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ScoreBoardComponent_Template,2670213952));
        templatesFactory->RegisterObject<Ray_LastPlayerTriggerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_LastPlayerTriggerComponent_Template,2510214116));
        templatesFactory->RegisterObject<Ray_CageAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CageAIComponent_Template,2067105519));
        templatesFactory->RegisterObject<Ray_CageStoneComponent_Template>(ITF_GET_STRINGID_CRC(Ray_CageStoneComponent_Template,2185410257));
        templatesFactory->RegisterObject<Ray_PlatformEnemyAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlatformEnemyAIComponent_Template,2213022351));
        templatesFactory->RegisterObject<Ray_BezierTreeAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BezierTreeAIComponent_Template,1286437410));
        templatesFactory->RegisterObject<Ray_PlatformChainAIComponent_Template>(ITF_GET_STRINGID_CRC(Ray_PlatformChainAIComponent_Template,1666631592));
        templatesFactory->RegisterObject<Ray_BubbleSpawnerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BubbleSpawnerComponent_Template,3018169251));
        templatesFactory->RegisterObject<Ray_ScreenSideTrajectoryFollowerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ScreenSideTrajectoryFollowerComponent_Template,1140262903));
        templatesFactory->RegisterObject<Ray_SequenceSwitchComponent_Template>(ITF_GET_STRINGID_CRC(Ray_SequenceSwitchComponent_Template,1190719862));
        templatesFactory->RegisterObject<Ray_AILums2Component_Template>(ITF_GET_STRINGID_CRC(Ray_AILums2Component_Template,3392457114));
        templatesFactory->RegisterObject<Ray_MoviePlayerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_MoviePlayerComponent_Template,509384819));
        templatesFactory->RegisterObject<Ray_ElectoonSpawnerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ElectoonSpawnerComponent_Template,1990605819));
        templatesFactory->RegisterObject<Ray_ElectoonTeethComponent_Template>(ITF_GET_STRINGID_CRC(Ray_ElectoonTeethComponent_Template,1750584553));

        // Bubble prizes
        templatesFactory->RegisterObject<Ray_BubblePrizeBumperComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BubblePrizeBumperComponent_Template,2167902645));
        templatesFactory->RegisterObject<Ray_BubblePrizeHubComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BubblePrizeHubComponent_Template,3041818536));
        templatesFactory->RegisterObject<Ray_BubblePrizePlatformComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BubblePrizePlatformComponent_Template,3959825116));
        templatesFactory->RegisterObject<Ray_BubblePrizeSpawnerComponent_Template>(ITF_GET_STRINGID_CRC(Ray_BubblePrizeSpawnerComponent_Template,2131467178));
        //
    }

    void registerTemplatesFactory(ObjectFactory* templatesFactory)
    {
        registerTemplatesFactory_Template_1(templatesFactory);
        registerTemplatesFactory_Template_2(templatesFactory);
        registerTemplatesFactory_Misc(templatesFactory);
    }

    void Ray_GameInterface::registerObjects() const
    {
        // include generic components
        GameplayFactoryFiller::fillFactories();


        ObjectFactory* animNodesFactory = ACTORSMANAGER->getAnimTreeNodeFactory();
        ObjectFactory* btNodesFactory = TEMPLATEDATABASE->getBTNodesFactory();
        ObjectFactory* templatesClassFactory    = TEMPLATEDATABASE->getClassTemplateFactory();

        //frieze plugins
        FRIEZE_PLUGIN_REGISTRY->registerPlugin("djembe", Ray_DjembeFriezePlugin::creationFunction);

        ObjectFactory* actorComponentsFactory = ACTORSMANAGER->getActorComponentsFactory();
        registerActorComponentsFactory(ACTORSMANAGER->getActorComponentsFactory());
        registerEvents(ACTORSMANAGER->getEventFactory());

        // Anim nodes
        animNodesFactory->RegisterObject<AnimNodeSwing_Template>(ITF_GET_STRINGID_CRC(AnimNodeSwing_Template,4285655022));

        // game screens
        m_gameScreenFactory->RegisterObject<GameScreen_Initial>(ITF_GET_STRINGID_CRC(GameScreen_Initial,2831340246));
        m_gameScreenFactory->RegisterObject<Ray_GameScreen_Gameplay>(ITF_GET_STRINGID_CRC(Ray_GameScreen_Gameplay,4191223421));
        m_gameScreenFactory->RegisterObject<Ray_GameScreen_MainMenu>(ITF_GET_STRINGID_CRC(Ray_GameScreen_MainMenu,1338236129));
        m_gameScreenFactory->RegisterObject<Ray_GameScreen_Framework>(ITF_GET_STRINGID_CRC(Ray_GameScreen_Framework,3413566858));
        m_gameScreenFactory->RegisterObject<Ray_GameScreen_WorldMap>(ITF_GET_STRINGID_CRC(Ray_GameScreen_WorldMap,3566109816));
        m_gameScreenFactory->RegisterObject<Ray_GameScreen_LevelStats>(ITF_GET_STRINGID_CRC(Ray_GameScreen_LevelStats,2786744658));
        m_gameScreenFactory->RegisterObject<Ray_GameScreen_CheckpointScore>(ITF_GET_STRINGID_CRC(Ray_GameScreen_CheckpointScore,2049621634));
        m_gameScreenFactory->RegisterObject<Ray_GameScreen_WorldMapLoading>(ITF_GET_STRINGID_CRC(Ray_GameScreen_WorldMapLoading,1826584106));

        ObjectFactory* templatesFactory = TEMPLATEDATABASE->getComponentsFactory();
        registerTemplatesFactory(TEMPLATEDATABASE->getComponentsFactory());
        registerAiBehaviorsFactory(ACTORSMANAGER->getAIBehaviorsFactory());
        registerAiActionsFactory(ACTORSMANAGER->getAIActionsFactory());

        btNodesFactory->RegisterObject<Ray_BTActionWaypointPathFind_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionWaypointPathFind_Template,1736178641));
        btNodesFactory->RegisterObject<Ray_BTActionRescuedDisappear_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionRescuedDisappear_Template,929322276));
        btNodesFactory->RegisterObject<Ray_BTActionUseTeleport_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionUseTeleport_Template,519951568));
        btNodesFactory->RegisterObject<Ray_BTActionDanceOnDoor_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionDanceOnDoor_Template,1082371704));
        btNodesFactory->RegisterObject<Ray_BTActionWaitForDanceOnDoor_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionWaitForDanceOnDoor_Template,2133261745));
        btNodesFactory->RegisterObject<Ray_BTActionActivateStone_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionActivateStone_Template,616072720));
        btNodesFactory->RegisterObject<Ray_BTActionPerformPedestal_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionPerformPedestal_Template,171296680));
        btNodesFactory->RegisterObject<Ray_BTActionHitTarget_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionHitTarget_Template,1589912172));
        btNodesFactory->RegisterObject<Ray_BTActionWalkToTargetSprintWithPlayer_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionWalkToTargetSprintWithPlayer_Template,3775712275));
        btNodesFactory->RegisterObject<Ray_BTActionFollowTarget_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionFollowTarget_Template,1106777397));
        btNodesFactory->RegisterObject<Ray_BTActionReceiveHit_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionReceiveHit_Template,104441640));
        btNodesFactory->RegisterObject<Ray_BTActionFindAttackTarget_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionFindAttackTarget_Template,994221613));
        btNodesFactory->RegisterObject<Ray_BTActionRunDarktoonified_Template>(ITF_GET_STRINGID_CRC(Ray_BTActionRunDarktoonified_Template,3833787209));
        btNodesFactory->RegisterObject<Ray_BTDeciderTargetInRangeToAttack_Template>(ITF_GET_STRINGID_CRC(Ray_BTDeciderTargetInRangeToAttack_Template,40903667));
        btNodesFactory->RegisterObject<Ray_BTDeciderFindTargetAttackPos_Template>(ITF_GET_STRINGID_CRC(Ray_BTDeciderFindTargetAttackPos_Template,647085612));

        // World File
        WorldFile::registerProjectData<Ray_WorldFile>();

        // Persistent game data
        m_persistentGameDataFactory->RegisterObject<Ray_PersistentGameData_ISD>(ITF_GET_STRINGID_CRC(Ray_PersistentGameData_ISD,3883514709));
        m_persistentGameDataFactory->RegisterObject<Ray_PersistentGameData_Level>(ITF_GET_STRINGID_CRC(Ray_PersistentGameData_Level,2267624090));
        m_persistentGameDataFactory->RegisterObject<Ray_PersistentGameData_Universe>(ITF_GET_STRINGID_CRC(Ray_PersistentGameData_Universe,991986087));



        /********* GPP BEN ***************/

        actorComponentsFactory->RegisterObject<Ray_AspiNetworkComponent>(ITF_GET_STRINGID_CRC(Ray_AspiNetworkComponent,4207808234));
        templatesFactory->RegisterObject<Ray_AspiNetworkComponent_Template>(ITF_GET_STRINGID_CRC(Ray_AspiNetworkComponent_Template,502739156));

        /********* END GPP BEN ***************/

        templatesClassFactory->RegisterObject<Ray_GameManagerConfig_Template>(ITF_GET_STRINGID_CRC(Ray_GameManagerConfig_Template,2754889544));
        templatesClassFactory->RegisterObject<Ray_GameMaterial_Template>(ITF_GET_STRINGID_CRC(Ray_GameMaterial_Template,2777394804));

        templatesClassFactory->RegisterObject<Ray_MusicManager_Template>(ITF_GET_STRINGID_CRC(Ray_MusicManager_Template,3881661542));


    }

    void Ray_GameInterface::onActivatePlayer(u32 _playerIndex, Actor* _player)
    {
#ifdef  USE_SPLITSCREEN
        if (GAMEMANAGER->getNumActivePlayers() > 1)
        {
            View* pSubView     =   View::createView();
            pSubView->createCamera();
            // TODO : Create camera behavior component
        }
#endif // USE_SPLITSCREEN
    }

	void Ray_GameInterface::updateViewsBehavior(f32 dt)
	{
		GameInterface::updateViewsBehavior(dt);
#ifdef  USE_SPLITSCREEN
		const SafeArray<View*>& views = View::getViews();
		if (views.size() > 1 && GAMEMANAGER->getMaxPlayerCount() > 1)
		{
#ifndef ITF_FINAL
			// Handle editor mode
			if (CAMERA->getMode() == Camera::Camera_Editor)
			{
				views[1]->setActive(bfalse);
				CAMERA->m_topLeft = Vec2d::Zero;
				CAMERA->m_bottomRight = Vec2d::One;
				return;
			}
#endif // ITF_FINAL
			LOG("WARNING : PROOF OF CONCEPT CODE!! ");
			Player* player = GAMEMANAGER->getPlayer(1);
			Camera* subCam = views[1]->getCamera();
			Vec3d newPos = player->getActor()->getPos();
			newPos.m_z += 7.f ;
			CAMERA->m_bottomRight.m_x = 0.5f;
			subCam->m_topLeft.m_x = 0.5f;
			views[1]->setActive(btrue);
			subCam->teleport(newPos);
			subCam->forcePosition(newPos);
	   }
#endif // USE_SPLITSCREEN
	}

	void Ray_GameInterface::onPostSceneActivation( const Scene* _pScene, bbool _bActive )
	{
		ITF_ASSERT(_pScene);

		if(_pScene && _bActive)
		{
            SafeArray<const Scene*> sceneToScan;
            bbool settingsApplied = bfalse;

            sceneToScan.push_back(_pScene);

            while(sceneToScan.size())
            {
                const Scene* pScene = sceneToScan[0];
                sceneToScan.eraseKeepOrder(0);

                // Recursive feed the while monster
                ObjectRefList ssas = pScene->getSubSceneActors();
                for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
                    if(BaseObject* pBO = ssas[iSSA].getObject())
                        if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                            if(Scene* pSubScene = pSSA->getSubScene())
                                sceneToScan.push_back(pSubScene);

                const PickableList& actors = pScene->getActors();
			    for(u32 i = 0; i < actors.size(); ++i)
                {
				    Actor* pActor = static_cast<Actor*>(actors[i]);
				    ITF_ASSERT(pActor);
				    if(!pActor || pActor->hasDataError() )
					    continue;

				    if(Ray_SceneSettingsComponent* pRSSC = pActor->GetComponent<Ray_SceneSettingsComponent>())
                    {
                        ITF_WARNING(pActor, !settingsApplied, "Multiple scene settings found");

					    pRSSC->applyTemplateSettings();
                        settingsApplied = btrue;
                    }
			    }
            }

#ifndef ITF_FINAL
            if ( !settingsApplied )
            {
                Ray_SceneSettingsComponent::resetSettings();
            }
#endif
		}
	}

	const GameMaterial_Template* Ray_GameInterface::allocateGameMaterial( const Path& _fileName, bool prefetch /* = false */ )
	{
        m_templateClientHandler.addUsedTemplate(_fileName.getStringID());
        return TEMPLATEDATABASE->getTemplate<Ray_GameMaterial_Template>(&m_templateClientHandler, _fileName, prefetch);
	}

    void Ray_GameInterface::reloadGameMaterial( const Path& _fileName ) const
    {
#ifdef ITF_SUPPORT_HOTRELOAD
        TEMPLATEDATABASE->reloadTemplate<Ray_GameMaterial_Template>(_fileName);
#endif //ITF_SUPPORT_HOTRELOAD
    }
}
