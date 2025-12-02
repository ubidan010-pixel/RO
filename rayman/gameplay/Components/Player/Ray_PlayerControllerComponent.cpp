#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_PHYSCOLLISIONSOLVER_H
#include "engine/physics/PhysCollisionSolver.h"
#endif //_ITF_PHYSCOLLISIONSOLVER_H

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AvoidanceManager.h"
#endif //_ITF_RAY_AVOIDANCEMANAGER_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_RAY_BOSSCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_BossCameraComponent.h"
#endif //_ITF_RAY_BOSSCAMERACOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_RAY_CAMERACONTROLLERMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CameraControllerManager.h"
#endif //_ITF_RAY_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

// Input specific platform
#ifdef ITF_WII
	#ifndef _ITF_INPUTADAPTER_WII_H_
	#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
	#endif //_ITF_INPUTADAPTER_WII_H_
#endif //ITF_WII

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkManager.h"
#endif //_ITF_RAY_AINETWORKMANAGER_H_

#ifndef _ITF_RAY_AIBTEVENTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTEvents.h"
#endif //_ITF_RAY_AIBTEVENTS_H_

#ifndef _ITF_RAY_BABYPIRANHACOMPONENT_H_
#include "rayman/gameplay/Components/Piranha/Ray_BabyPiranhaComponent.h"
#endif //_ITF_RAY_BABYPIRANHACOMPONENT_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_
#include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#endif //_ITF_RAY_SHOOTERGAMEMODECONTROLLER_H_

#ifndef _ITF_RAY_PLATFORMERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_PlatformerCameraComponent.h"
#endif //_ITF_RAY_PLATFORMERCAMERACOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_PlayerControllerComponent)

BEGIN_VALIDATE_COMPONENT(Ray_PlayerControllerComponent)
    m_heartShield.validate(_isComponentValidated);
    m_superPunchBasicGauge.validate(_isComponentValidated);
    m_superPunchSeekingGauge.validate(_isComponentValidated);
	m_swarmRepellerPowerUp.validate(_isComponentValidated);
END_VALIDATE_COMPONENT()

BEGIN_SERIALIZATION_CHILD(Ray_PlayerControllerComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_PropertyEdit)
#ifndef ITF_FINAL
        SERIALIZE_MEMBER("displayHangDebug",m_displayHangDebug);
        SERIALIZE_MEMBER("drawAnimInputs",m_drawAnimInputs);
        SERIALIZE_MEMBER("controllerSwingDrawDebug",m_swingDrawDebug);
#endif // ITF_FINAL
    END_CONDITION_BLOCK()

END_SERIALIZATION()

const f32 Ray_PlayerControllerComponent::s_achievedSpeedCalculationDelay = 0.5f;
const f32 Ray_PlayerControllerComponent::s_onPedestalInterpolateDuration = 0.25f;
const f32 Ray_PlayerControllerComponent::SURVIVE_PIRANHAS_MIN_TIME_INSIDE = 0.2f;
const f32 Ray_PlayerControllerComponent::SURVIVE_PIRANHAS_MIN_TIME_OUTSIDE = 0.5f;
const f32 Ray_PlayerControllerComponent::SURVIVE_PIRANHAS_COUNTDOWN_AFTER_HIT = 3.0f;

Ray_PlayerControllerComponent::Ray_PlayerControllerComponent()
: Super()
, m_hangingPolyline(ITF_INVALID_OBJREF)
, m_hangingActor(ITF_INVALID_OBJREF)
, m_previousHangingPolyline(ITF_INVALID_OBJREF)
, m_previousHangingActor(ITF_INVALID_OBJREF)
, m_hangingEdge(U32_INVALID)
, m_climbingPolyline(ITF_INVALID_OBJREF)
, m_previousClimbingPolyline(ITF_INVALID_OBJREF)
, m_climbingEdge(U32_INVALID)
, m_climbingT(0.f)
, m_climbingVertical(bfalse)
, m_climbingSlide(bfalse)
, m_climbingForceSidewaysJump(bfalse)
, m_climbingConstrainedEdge(U32_INVALID)
, m_climbingConstrainedT(0.f)
, m_climbingIntertia(0.f)
, m_climbingPrevMoveSign(0.f)
, m_climbingOnBottomCorner(bfalse)
, m_climbingAllowDropVertical(bfalse)
, m_climbingAllowExitHorizontal(bfalse)
, m_climbingJustStarted(bfalse)
, m_climbingAllowHangCoop(bfalse)
, m_currentJumpMultiplier(1.f)
, m_currentJumpMultiplierNoImpulse(1.f)
, m_currentJumpImpulseX(1.f)
, m_currentJumpSpeedX(Vec2d::Zero)
, m_currentJumpUseWallDir(bfalse)
, m_currentJumpForceSideways(bfalse)
, m_currentJumpWallRun(bfalse)
, m_currentJumpQueueCrushAttack(bfalse)
, m_currentJumpKeepSpeedX(bfalse)
, m_fallFromRoof(bfalse)
, m_currentJumpForceDisableAirControl(0.f)
, m_swimingPolyline(ITF_INVALID_OBJREF)
, m_swimingWaterResistance(5.f)
, m_swimingApplyEnterWaterResistance(bfalse)
, m_swimingClosestEdge(U32_INVALID)
, m_swimingSurfaceEdge(U32_INVALID)
, m_swimingSurfaceT(0.f)
, m_swimingClosestEdgeT(0.f)
, m_swimingTime(0.f)
, m_swimingTargetRotation(0.f)
, m_swimingCurrentAngularSpeed(0.f)
, m_swimingCurveCursor(0.5f)
, m_swimingCurveTarget(0.5f)
, m_swimingCurveSpeed(0.f)
, m_swimingStandPose(0.f)
, m_swimingSprintJumpGravityCounter(0.f)
, m_swimingDashCounter(0.f)
, m_swimingFloatForce(Vec2d::Zero)
, m_swimingIsInside(bfalse)
, m_swimingIsOnSurface(bfalse)
, m_swimingIsOnJumpRange(bfalse)
, m_swimingEnterDown(bfalse)
, m_swimingTwist(bfalse)
, m_survivePiranhaPondInside(bfalse)
, m_survivePiranhaPondOutside(bfalse)
, m_survivePiranhaPondTimer(0.0f)
, m_survivePiranhaPondLastHitTimer(0.0f)
, m_phantomShapeScaled(NULL)
, m_airApplyBrake(bfalse)
, m_airControlDisableDelay(0.f)
, m_airControlDisableTotal(0.f)
, m_airControlDisableAllowHelico(bfalse)
, m_airControlDisableBlend(bfalse)
, m_airControlDisableAmount(0.f)
, m_airControlJustFinished(bfalse)
, m_climbActivationCounter(0.f)
, m_wallSlideDir(Vec2d::Right)
, m_wallSlideStickDelayCounter(0.f)
, m_wallSlidePrevPoly(ITF_INVALID_OBJREF)
, m_wallSlidePrevEdge(U32_INVALID)
, m_wallSlideAirControl(btrue)
, m_wallSlideTestSpeed(Vec2d::Zero)
, m_wallJumpDisableTimer(0.f)
, m_disableCrouchTimer(0.f)
, m_unCrouchCounter(0.5f)
, m_sprintActivationDelay(0.f)
, m_sprintMaxSpeedBlendTimer(0.f)
, m_releaseSprintForce(0.f)
, m_releaseSprintActive(bfalse)
, m_releaseSprintWasSprinting(bfalse)
, m_wasOnGroundPrevFrame(bfalse)
, m_moveTargetBlendTimer(0.f)
, m_currentMoveTargetMultiplier(1.f)
, m_jumpQueueTimer(0.f)
, m_hitQueueTimer(0.f)
, m_hitDisabledTimer(0.f)
, m_jumpOnAirTimer(0.f)
, m_surfingTimer(0.f)
, m_currentEfficiency(1.f)
, m_achievedSpeedCalculationTimer(s_achievedSpeedCalculationDelay)
, m_achievedSpeedFactor(1.f)
, m_targetAchievedSpeedFactor(1.f)
, m_achievedSpeedFactorV(0.f)
, m_onCorner(CORNERPOSE_NONE)
, m_hangActivationCounter(0.f)
, m_receivedHitLevel(0)
, m_receivedHitDamage(0)
, m_receivedHitDir(Vec2d::Right)
, m_receivedHitType(RECEIVEDHITTYPE_FRONTPUNCH)
, m_receivedHitFXPos(Vec3d::Zero)
, m_receivedHitPosition(Vec2d::Zero)
, m_currentTargetForce(Vec2d::Zero)
, m_currentGravity(Vec2d::Zero)
, m_currentGravityDir(Vec2d::Zero)
, m_prevPos(Vec2d::Zero)
, m_prevWasSprint(bfalse)
, m_checkSprintAward(btrue)
, m_helicopterSuspensionTimer(0.f)
, m_helicopterCooldown(0.f)
, m_helicopterCooldownJustFinished(bfalse)
, m_airHitForceApplicationTimer(0.f)
, m_airHitInProgress(bfalse)
, m_airHitPrepared(btrue)
, m_airHitForceMultiplier(1.0f)
, m_bounceSender(ITF_INVALID_OBJREF)
, m_bounceCurrentDir(Vec2d::Zero)
, m_bounceSpeedAtTime(Vec2d::Zero)
, m_bounceCurrentOrigin(Vec2d::Zero)
, m_bounceCurrentType(BOUNCETYPE_NONE)
, m_bounceCurrentReactType(BOUNCEREACTTYPE_NONE)
, m_bounceRepositionTotal(Vec2d::Zero)
, m_bounceRepositionFinalSpeed(Vec2d::Zero)
, m_bounceRepositionTotalTime(0.f)
, m_bounceRepositionCounter(0.f)
, m_bounceMultiplier(1.f)
, m_bounceCurrentDangerLevel(0)
, m_bounceJumpAllowTimer(0.f)
, m_bounceTimer(0.f)
, m_bounceCheckEdgeDir(Vec2d::Zero)
, m_bounceCheckEdgeDangerLevel(U32_INVALID)
, m_bounceCheckEdgePolyRef(ITF_INVALID_OBJREF)
, m_bounceCheckEdgeIndex(U32_INVALID)
, m_bounceCheckEdgeSpeed(Vec2d::Zero)
, m_bouncePrevFramePoly(ITF_INVALID_OBJREF)
, m_windDisableCounter(0.f)
, m_currentAccrobaticStage(0)
, m_nextJumpIsAccrobatic(bfalse)
, m_accrobaticStageKeepCounter(0.f)
, m_receivedHitInstigator(ITF_INVALID_OBJREF)
, m_swingOnActor(ITF_INVALID_OBJREF)
, m_swingLength(0.f)
, m_swingMaxAngle(MTH_PIBY2)
, m_swingGravMultiplier(1.f)
, m_swingJumpSign(0.f)
#ifndef ITF_FINAL
, m_swingDrawDebug(bfalse)
, m_displayHangDebug(bfalse)
, m_drawAnimInputs(bfalse)
#endif // ITF_FINAL
, m_inmunityCounter(0.f)
, m_inmunityCounterReceiveHit(0.f)
, m_currentPunchHitType(PUNCHTYPE_CHARGE)
, m_currentPunchHitLevel(0)
, m_currentPunchResult(PUNCHRELEASERESULT_NOIMPACT)
, m_currentPunchDistance(1.f)
, m_lastJumpTimer(0.f)
, m_stance(STANCE_STAND)
, m_prevStance(STANCE_STAND)
, m_orientationPose(ORIENTATION_HORIZONTAL)
, m_actionPose(ACTIONPOSE_STAND)
, m_moodZone(MOODZONE_NONE)
, m_poseTimeCounter(0.f)
, m_crushHitSuspensionCounter(0.f)
, m_crushHitYMultiplier(2.f)
, m_crushRetriggerCounter(0.f)
, m_hitFxBoneIndex(U32_INVALID)
, m_burstPunchToDeliver(0)
, m_dashAttackCooldown(0.f)
, m_timeSinceDashAttack(0.f)
, m_reducedHitUpDisabledUntilLand(bfalse)
, m_performingUTurn(UTURNTYPE_NONE)
, m_uTurnFlipped(bfalse)
, m_uTurnFinished(bfalse)
, m_flipActor(bfalse)
, m_uTurnComboCounter(0.f)
, m_uTurnCounter(0.f)
, m_frictionMode(FRICTIONMODE_NORMAL)
, m_gravityMode(GRAVITYMODE_NORMAL)
, m_gravityModeValue(1.f)
, m_gravityModeDuration(0.f)
, m_gravityModeCounter(0.f)
, m_desiredLookDir(MOVEDIR_NONE)
, m_moveInput(Vec2d::Zero)
, m_actionFlags(0)
, m_idleTimer(0.f)
, m_previousSpeed(Vec2d::Zero)
, m_helicoFxHandle(U32_INVALID)
, m_lockPrevBinding(ITF_INVALID_OBJREF)
, m_lockSnapActor(ITF_INVALID_OBJREF)
, m_actorLocked(bfalse)
, m_lockPrevPhysics(bfalse)
, m_lockPhantomRemoved(bfalse)
, m_coopActionMode(ACTIONCOOPMODE_NONE)
, m_coopActionVictim(ITF_INVALID_OBJREF)
, m_coopActionMain(ITF_INVALID_OBJREF)
, m_squadData(NULL)
, m_currentMagnet(ITF_INVALID_OBJREF)
, m_magnetForce(0)
, m_currentPunchOrientation(0.f)
, m_playerDisabled(bfalse)
, m_inputDisabled(bfalse)
, m_moveToPoint(bfalse)
, m_moveToPointFinished(bfalse)
, m_moveToPointTarget(Vec2d::Zero)
, m_moveToPointFinishedToler( 0.5f )
, m_moveToPointFirstTime(bfalse)
, m_bounceToLayer(bfalse)
, m_tinyFXRef(ITF_INVALID_OBJREF)
, m_tinyTrailDisabled(btrue)
, m_tinyTrailCurrentColor(Color::white())
, m_tinyTrailTargetColor(Color::white())
, m_currentDeathType(DEATHTYPE_NONE)
, m_lastHitNature(HITNATURE_UNKNOWN)
, m_moveCursor(0.f)
, m_slideFxHandle(U32_INVALID)
, m_wallSlideFxHandle(U32_INVALID)
, m_deadSoulFxHandle(U32_INVALID)
, m_lianaSlideFx(U32_INVALID)
, m_sprintFxHandle(U32_INVALID)
, m_sprintReducedFxHandle(U32_INVALID)
, m_wallRunFxHandle(U32_INVALID)
, m_wallRunReducedFxHandle(U32_INVALID)
, m_cameraSubjectComponent(NULL)
, m_cameraLimiterTimer(0.f)
, m_offscreenTimer(0.f)
, m_lastOnscreenPlayerKillsEveryone(btrue)
, m_walkOnWallsDisableTimer(0.f)
, m_walkOnWallsDisableUntilLand(bfalse)
, m_walkOnWallKeepDirFromSpeed(bfalse)
, m_prevCurrentRegion(ITF_INVALID_OBJREF)
, m_prevCurrentMove(Vec2d::Zero)
, m_squashed(bfalse)
, m_handsCaught(bfalse)
, m_handsReleased(bfalse)
, m_handsRef(ITF_INVALID_OBJREF)
, m_caughtInPipe(bfalse)
, m_pipeRef(ITF_INVALID_OBJREF)
, m_instantKill(bfalse)
, m_justActivated(bfalse)
, m_justActivatedFrame(0)
, m_baseInfoPrevPoly(ITF_INVALID_OBJREF)
, m_baseInfoPrevEdgeIndex(U32_INVALID)
, m_baseInfoPrevRelDist(0.f)
, m_baseInfoPrevActor(ITF_INVALID_OBJREF)
, m_baseInfoPrevMultiplierX(0.f)
, m_baseInfoPrevMultiplierY(0.f)
, m_baseInfoSpeed(Vec2d::Zero)
, m_baseInfoPrevPos(Vec2d::Zero)
, m_pageAirTravelling( bfalse )
, m_pageActive( bfalse )
, m_pedestalBoneIndex(U32_INVALID)
, m_onPedestal(ITF_INVALID_OBJREF)
, m_onPedestalValid(bfalse)
, m_onPedestalAllowFall(btrue)
, m_onPedestalPos(Vec2d::Zero)
, m_onPedestalStartPos(Vec2d::Zero)
, m_onPedestalLastPos(Vec2d::Zero)
, m_onPedestalInterpolateCounter(0.f)
, m_onPedestalDisableCounter(0.f)
, m_onPedestalDisableRef(ITF_INVALID_OBJREF)
, m_enterDoorReceived(bfalse)
, m_changePageReceived(bfalse)
, m_stargateReceived(bfalse)
, m_scaleDoorReceived(bfalse)
, m_scaleTunnelReceived(bfalse)
, m_scaleTunnelScale(1.f)
, m_scaleTunnelT(0.f)
, m_jumpToPosReceived(bfalse)
, m_snapDanceReceived(bfalse)
, m_throwTeethReceived(bfalse)
, m_playAnimReceived(bfalse)
, m_punchEventReceived(bfalse)
, m_softCollisionRadiusMultiplier(1.f)
, m_softCollisionForceMultiplier(1.f)
, m_softCollisionRestoreForceTimer(0.f)
, m_softCollisionDisabled(bfalse)
, m_heartShield()
, m_superPunchBasicGauge()
, m_superPunchSeekingGauge()
, m_swarmRepellerPowerUp()
, m_InputLastTimePressed(0.0)
, m_InputIsDoublePressed(bfalse)
, m_stateDeadSoulTimer(0.0f)
, m_forceFindGround(bfalse)
, m_listenerAdded(bfalse)
, m_listenerMove(Vec2d::Zero)
, m_listenerAttack(bfalse)
, m_listenerAttackHold(bfalse)
, m_listenerJump(bfalse)
, m_listenerJumpHold(bfalse)
, m_listenerHelico(bfalse)
, m_listenerHelicoHold(bfalse)
, m_listenerSprint(bfalse)
, m_listenerLeave(bfalse)
#ifdef ITF_SUPPORT_CHEAT
, m_listenerFlyCheat(bfalse)
#endif
, m_aiNetworkRegistered(bfalse)
, m_aiNetworkCurrentWaypoint(ITF_INVALID_OBJREF)
, m_listenerSprintTrigger(bfalse)
, m_sprintTrigger()
, m_runToggleActive(bfalse)
, m_prevRunButtonPressed(bfalse)
, m_bubblizerIdentifier(0)
, m_checkRewardBounce(bfalse)
{

}

Ray_PlayerControllerComponent::~Ray_PlayerControllerComponent()
{
}

void Ray_PlayerControllerComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208), this);
    EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerHpChanged,1555193400), this);
    EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), this);
    EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventTriggerVictory,1674293021), this);

    RAY_AVOIDANCEMANAGER->removeMember(m_actor->getRef());

    if ( m_listenerAdded )
    {
        removeListener();
    }

    if ( m_aiNetworkRegistered )
    {
        RAY_AINETWORKMANAGER->unregisterActor(m_actor->getRef());
    }

    Actor* trail = m_tinyFXRef.getActor();

    if ( trail )
    {
        trail->requestDestruction();
    }

    if ( m_currentState )
    {
        m_prevState = NULL;
        m_nextState = NULL;
        m_currentState->onExit();
    }

    SF_DEL(m_phantomShapeScaled);
}

void Ray_PlayerControllerComponent::sendWindStim( const Vec2d& _dir, const PhysForceModifier_Instance& _forceModifier )
{
    if ( _forceModifier.getPolyline().m_polygon.isValid() )
    {
        WindStim* windStim = STIMSMANAGER->requestStim<WindStim>(&_forceModifier.getPolyline().m_polygon);

        if ( windStim )
        {
            windStim->setPos(m_actor->get2DPos());
            windStim->setPrevPos(m_actor->get2DPos());
            windStim->setSender(m_actor->getRef());
            windStim->setAngle(m_actor->getAngle());
            windStim->setDepth(m_actor->getDepth());
            windStim->setDir(_dir);

            STIMSMANAGER->sendStim(windStim);
        }
    }
}

void Ray_PlayerControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_stateList.push_back(&m_stateIdle);
    m_stateList.push_back(&m_statePrepareHit);
    m_stateList.push_back(&m_stateReleaseHit);
    m_stateList.push_back(&m_stateJumping);
    m_stateList.push_back(&m_stateHelicopter);
    m_stateList.push_back(&m_stateFalling);
    m_stateList.push_back(&m_stateHanging);
    m_stateList.push_back(&m_stateIdleToHang);
    m_stateList.push_back(&m_stateHangToIdle);
    m_stateList.push_back(&m_stateHangToJump);
    m_stateList.push_back(&m_stateCoopHangClimb);
    m_stateList.push_back(&m_stateReceiveHit);
    m_stateList.push_back(&m_stateReceiveHitCrash);
    m_stateList.push_back(&m_stateClimbing);
#ifdef ITF_SUPPORT_CHEAT
    m_stateList.push_back(&m_stateFlyMode);
#endif
    m_stateList.push_back(&m_statePunchMitraille);
    m_stateList.push_back(&m_stateWallSlide);
    m_stateList.push_back(&m_stateCrushedByCharacter);
    m_stateList.push_back(&m_stateDeadSoul);
    m_stateList.push_back(&m_stateRevive);
    m_stateList.push_back(&m_stateHangToStand);
    m_stateList.push_back(&m_stateLock);
    m_stateList.push_back(&m_stateSnapDance);
    m_stateList.push_back(&m_statePlayAnim);
    m_stateList.push_back(&m_stateSwimIdle);
    m_stateList.push_back(&m_stateSwing);
    m_stateList.push_back(&m_stateStartLevel);
    m_stateList.push_back(&m_stateExitLevel);
    m_stateList.push_back(&m_stateDeath);
    m_stateList.push_back(&m_stateCaught);
    m_stateList.push_back(&m_stateCaughtInPipe);
    m_stateList.push_back(&m_stateBounceToLayer);
    m_stateList.push_back(&m_stateJumpToPos);
    m_stateList.push_back(&m_stateStargate);
    m_stateList.push_back(&m_stateOnPedestal);

    // store power-up displays in list
    m_powerUpDisplays.push_back(&m_heartShield);
    m_powerUpDisplays.push_back(&m_superPunchBasicGauge);
    m_powerUpDisplays.push_back(&m_superPunchSeekingGauge);
    m_powerUpDisplays.push_back(&m_swarmRepellerPowerUp);

    EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208),this);
    EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerHpChanged,1555193400),this);
    EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),this);
    EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventTriggerVictory,1674293021),this);

    Super::onActorLoaded(_hotReload);

    resetStandPhantomShapeScaled();
    m_currentMoveTargetMultiplier = getTemplate()->getMoveTargetMultiplierMin();

    changeState(&m_stateIdle);

    m_cameraSubjectComponent= m_actor->GetComponent<CameraSubjectComponent>();
    ITF_ASSERT(m_cameraSubjectComponent);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_DoorTeleport,3845549645),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolylineUpdate,2702146949),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryHitType,4146626226),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDetach,889341216), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsSprinting,2326390223), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryIsAccrobatic,1083236336),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQuerySnapData,747808288),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventPlayLockAnim,3679647184),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSwingLatch,2013309652),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHangOnCharacter,1277553679),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryHangInfo,4077208499),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventMagnetTrigger,106997702),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHangCoopJump,930438087),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryStanceInfo,1627894922),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanDarktoonify,1194771444),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryDarktoonificationPosition,785050175),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(BounceStim,819805069),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventMoveToPoint,1570511527),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventResetMoveToPoint,918485638),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryReachedMoveToPoint,1707458262),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStopPlayer,3369505273),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsDisabled,1476509545),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsCaught,1045544516),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorPrepare,3178520967),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorReady,1601154857),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventJumpToPos,981487617),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerModeSet,722903700), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPhysicsUpdated,2632338894), this);
	ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChangePage,1004490573), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventInstantKill,2400145155), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHandsCaught,3210089466), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHandsRelease,579391720), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventSnapDance,806811309), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventCaughtInPipe,1030315255), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerEnterDoor,849805494), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerStargate,3125962121), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventPlayAnimState,4089549703), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventScaleDoorTrigger,3012042865), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventScaleTunnelTrigger,2369480741), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryIsInPedestal,2146089474), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventStargateUse,3921841867), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAddSoftCollForce,2624804693), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventCheckPedestalSupport,1578355683), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryPedestalInfo,2107418028), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventReleasePedestal,4284763324), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryPaintTrail,3721333601), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventScaleFunnelTrigger,2532379169), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAINetworkWaypoint,3032038608), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventReturnTeeth,810560716), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_MoveOnPolylineFromNautilus,1238635980), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerStartTimeAttack,3727680095), this);

    if ( !getTemplate()->getDeathFXActorSpawn().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_generator, getTemplate()->getDeathFXActorSpawn());
    }

    if ( !getTemplate()->getTinyFXFile().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_tinySpawnGenerator, getTemplate()->getTinyFXFile());
    }

    if ( !getTemplate()->getBounceToLayerFXFile().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_bounceToLayerSpawnGenerator, getTemplate()->getBounceToLayerFXFile());
    }

    processScaleChanged();

    m_characterPhys->setApplyWindSpeedLimiter(getTemplate()->getApplyWindSpeedLimitation());

    setMaxSpeedLimit(btrue);

    m_softCollisionRadiusMultiplier = getTemplate()->getSoftCollisionRadiusMultiplier();

    // init power-up displays
    m_heartShield.init(m_actor,m_playerIndex, &getTemplate()->getHeartShield());
    m_heartShield.onActorLoaded(_hotReload);
    m_superPunchBasicGauge.init(m_actor,m_playerIndex, &getTemplate()->getSuperPunchBasicGauge());
    m_superPunchBasicGauge.onActorLoaded(_hotReload);
    m_superPunchSeekingGauge.init(m_actor,m_playerIndex, &getTemplate()->getSuperPunchSeekingGauge());
    m_superPunchSeekingGauge.onActorLoaded(_hotReload);
	m_swarmRepellerPowerUp.init(m_actor,m_playerIndex, &getTemplate()->getSwarmRepellerPowerUp());
	m_swarmRepellerPowerUp.onActorLoaded(_hotReload);

    if ( !getTemplate()->getTinyFXFile().isEmpty() )
    {
        Actor* tinyTrail = m_tinySpawnGenerator.getSpawnee(m_actor->getScene(),m_actor->getPos(),0.0f);

        if ( tinyTrail )
        {
            m_tinyFXRef = tinyTrail->getRef();
            tinyTrail->loadResources();

            EventTrail trailEvent(bfalse);
            tinyTrail->onEvent(&trailEvent);
            m_tinyTrailDisabled = btrue;

            m_actor->getBinding()->bindChild(m_tinyFXRef);
        }
    }

    m_prevPos = m_actor->get2DPos();
}

void Ray_PlayerControllerComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( m_animInputIndex.size() == 0 )
    {
        StringID s_Speed = ITF_GET_STRINGID_CRC(Speed,4218325830);                                                // Absolute speed value
        StringID s_SpeedX = ITF_GET_STRINGID_CRC(SpeedX,3996234170);                                              // Speed on X (positive in the direction of the character)
        StringID s_SpeedY = ITF_GET_STRINGID_CRC(SpeedY,461177981);                                               // Speed on Y (positive in the opposite direction of the gravity)
        StringID s_AverageSpeedX = ITF_GET_STRINGID_CRC(AverageSpeedX,1159787462);                                // Average world speed on X (positive in the direction of the character)
        StringID s_AverageSpeedY = ITF_GET_STRINGID_CRC(AverageSpeedY,1630529258);                                // Average world speed on Y (positive in the opposite direction of the gravity)
        StringID s_Move = ITF_GET_STRINGID_CRC(Move,2544723026);                                                  // Stick Push (absolute)
        StringID s_MoveX = ITF_GET_STRINGID_CRC(MoveX,965019289);                                                 // Stick Push on X (positive in the direction of the character)
        StringID s_MoveY = ITF_GET_STRINGID_CRC(MoveY,951160567);                                                 // Stick Push on Y (positive in the opposite direction of the gravity)
        StringID s_Stance = ITF_GET_STRINGID_CRC(Stance,2021520747);                                              // Standing, Hanging, Climbing or in Helico mode
        StringID s_PrevStance = ITF_GET_STRINGID_CRC(PrevStance,1321175521);                                      // The previous stance
        StringID s_InAir = ITF_GET_STRINGID_CRC(InAir,1518932154);                                                // If the character is not sticked to anything
        StringID s_PunchLevel = ITF_GET_STRINGID_CRC(PunchLevel,3044141386);                                      // Level or stage of the current punch
        StringID s_ReceiveHitLevel = ITF_GET_STRINGID_CRC(ReceivedHitLevel,3435787350);                           // Vertical or Horizontal
        StringID s_OrientationPose = ITF_GET_STRINGID_CRC(OrientationPose,3454433435);                            // Angle of the character
        StringID s_ActionPose = ITF_GET_STRINGID_CRC(ActionPose,1861693421);                                      // Action pose of the character (stand, fight...)
        StringID s_MoodZone = ITF_GET_STRINGID_CRC(MoodZone,3143571277);                                          // Mood of the region where the character is in
        StringID s_Angle = ITF_GET_STRINGID_CRC(Angle,2573643044);                                                // Level of the last received hit
        StringID s_UTurn = ITF_GET_STRINGID_CRC(UTurn,3271188893);                                                // If the character is performing a UTurn
        StringID s_AchievedSpeedFactor = ITF_GET_STRINGID_CRC(AchievedSpeedFactor,3216667165);                    // How close am I to the speed I want to achieve
        StringID s_PunchHitType = ITF_GET_STRINGID_CRC(PunchHitType,1970003562);                                  // What type of punch we are performing
        StringID s_ReceivedHitType = ITF_GET_STRINGID_CRC(ReceivedHitType,490560957);                             // The type of the received hit
        StringID s_Supporting = ITF_GET_STRINGID_CRC(Supporting,3662594002);                                      // If the character is supporting another
        StringID s_CoopMode = ITF_GET_STRINGID_CRC(CoopMode,3809187602);                                          // Coop mode of the current action
        StringID s_CoopModePartnerStance = ITF_GET_STRINGID_CRC(CoopPartnerStance,954102759);                     // Stance of the coop partner
        StringID s_CoopModePartnerOrientationPose = ITF_GET_STRINGID_CRC(CoopPartnerOrientationPose,2585810441);  // Orientation of the coop partner
        StringID s_IsSprinting = ITF_GET_STRINGID_CRC(IsSprinting,4259516180);                                    // If the character is trying to sprint
        StringID s_IdleTime = ITF_GET_STRINGID_CRC(IdleTime,3421885816);                                          // Time the player hasn't entered any input
        StringID s_GroundAngle = ITF_GET_STRINGID_CRC(GroundAngle,1783688046);                                    // The angle of the ground
        StringID s_IsPathBlocked = ITF_GET_STRINGID_CRC(IsPathBlocked,2033351183);
        StringID s_DeathType = ITF_GET_STRINGID_CRC(DeathType,2047844758);
        StringID s_punchOrientation = ITF_GET_STRINGID_CRC(PunchOrientation,2576735204);                          // If we are punching up or forward
        StringID s_moveCursor = ITF_GET_STRINGID_CRC(MoveCursor,2402187973);                                      // Cursor for moving animations
        StringID s_SlideAction = ITF_GET_STRINGID_CRC(SlideAction,693047192);
        StringID s_IsCrouched = ITF_GET_STRINGID_CRC(IsCrouched,2926399429);
        StringID s_FallFromRoof = ITF_GET_STRINGID_CRC(FallFromRoof,677916036);
        StringID s_onCorner = ITF_GET_STRINGID_CRC(OnCorner,2452559197);
        StringID s_AirPose = ITF_GET_STRINGID_CRC(AirPose,2797818131);
        StringID s_FallAngleCursor = ITF_GET_STRINGID_CRC(FallAngleCursor,3810434062);
        StringID s_HangToJump = ITF_GET_STRINGID_CRC(HangToJumpCursor,873565322);
        StringID s_PrepareHitCursor = ITF_GET_STRINGID_CRC(PrepareHitCursor,3895578209);
        StringID s_ActionFromUTurn = ITF_GET_STRINGID_CRC(ActionFromUTurn,4183762490);
        StringID s_CrushTravelTime = ITF_GET_STRINGID_CRC(CrushTravelTime,4206142348);
        StringID s_CrushReleased = ITF_GET_STRINGID_CRC(CrushReleased,936475657);
        StringID s_PunchResult = ITF_GET_STRINGID_CRC(PunchReleaseResult,1869303082);
        StringID s_PunchDistance = ITF_GET_STRINGID_CRC(PunchDistance,719749371);
        StringID s_IsLowRoof = ITF_GET_STRINGID_CRC(IsLowRoof,554261504);
        StringID s_IsRolling = ITF_GET_STRINGID_CRC(IsRolling,2793359660);
        StringID s_Interacting = ITF_GET_STRINGID_CRC(Interacting,1372170697);
        StringID s_LayingDown = ITF_GET_STRINGID_CRC(LayingDown,1362607715);
        StringID s_AccrobaticStage = ITF_GET_STRINGID_CRC(AccrobaticStage,3177669209);
        StringID s_JumpFromWallRun = ITF_GET_STRINGID_CRC(JumpFromWallRun,1353773196);
        StringID s_PunchBurstStage = ITF_GET_STRINGID_CRC(PunchBurstStage,1704106144);
        StringID s_ReviveCursor = ITF_GET_STRINGID_CRC(ReviveCursor,3327073526);
        StringID s_StargateStage = ITF_GET_STRINGID_CRC(StargateStage,326515793);
        StringID s_diveMode = ITF_GET_STRINGID_CRC(DiveMode,3149106639);
        StringID s_swimEnterHead = ITF_GET_STRINGID_CRC(SwimEnterHead,288414836);
        StringID s_SwimTwist = ITF_GET_STRINGID_CRC(SwimTwist,570692086);
        StringID s_swimCurveCursor = ITF_GET_STRINGID_CRC(SwimCurveCursor,3090384440);
        StringID s_SwingQuadrant = ITF_GET_STRINGID_CRC(SwingQuadrant,3855359827);

        m_animInputIndex.resize(ANIMINPUTMAX);

        m_animInputIndex[ANIMINPUT_SPEED] = m_animComponent->findInputIndex(s_Speed);
        m_animInputIndex[ANIMINPUT_SPEEDX] = m_animComponent->findInputIndex(s_SpeedX);
        m_animInputIndex[ANIMINPUT_SPEEDY] = m_animComponent->findInputIndex(s_SpeedY);
        m_animInputIndex[ANIMINPUT_AVGSPEEDX] = m_animComponent->findInputIndex(s_AverageSpeedX);
        m_animInputIndex[ANIMINPUT_AVGSPEEDY] = m_animComponent->findInputIndex(s_AverageSpeedY);
        m_animInputIndex[ANIMINPUT_MOVE] = m_animComponent->findInputIndex(s_Move);
        m_animInputIndex[ANIMINPUT_MOVEX] = m_animComponent->findInputIndex(s_MoveX);
        m_animInputIndex[ANIMINPUT_MOVEY] = m_animComponent->findInputIndex(s_MoveY);
        m_animInputIndex[ANIMINPUT_STANCE] = m_animComponent->findInputIndex(s_Stance);
        m_animInputIndex[ANIMINPUT_PREVSTANCE] = m_animComponent->findInputIndex(s_PrevStance);
        m_animInputIndex[ANIMINPUT_INAIR] = m_animComponent->findInputIndex(s_InAir);
        m_animInputIndex[ANIMINPUT_PUNCHLEVEL] = m_animComponent->findInputIndex(s_PunchLevel);
        m_animInputIndex[ANIMINPUT_RECEIVEHITLEVEL] = m_animComponent->findInputIndex(s_ReceiveHitLevel);
        m_animInputIndex[ANIMINPUT_ORIENTATIONPOSE] = m_animComponent->findInputIndex(s_OrientationPose);
        m_animInputIndex[ANIMINPUT_ACTIONPOSE] = m_animComponent->findInputIndex(s_ActionPose);
        m_animInputIndex[ANIMINPUT_MOODZONE] = m_animComponent->findInputIndex(s_MoodZone);
        m_animInputIndex[ANIMINPUT_ANGLE] = m_animComponent->findInputIndex(s_Angle);
        m_animInputIndex[ANIMINPUT_UTURN] = m_animComponent->findInputIndex(s_UTurn);
        m_animInputIndex[ANIMINPUT_ACHIEVEDSPEEDFACTOR] = m_animComponent->findInputIndex(s_AchievedSpeedFactor);
        m_animInputIndex[ANIMINPUT_PUNCHHITTYPE] = m_animComponent->findInputIndex(s_PunchHitType);
        m_animInputIndex[ANIMINPUT_RECEIVEDHITTYPE] = m_animComponent->findInputIndex(s_ReceivedHitType);
        m_animInputIndex[ANIMINPUT_SUPPORTING] = m_animComponent->findInputIndex(s_Supporting);
        m_animInputIndex[ANIMINPUT_COOPMODE] = m_animComponent->findInputIndex(s_CoopMode);
        m_animInputIndex[ANIMINPUT_COOPMODEPARTNERSTANCE] = m_animComponent->findInputIndex(s_CoopModePartnerStance);
        m_animInputIndex[ANIMINPUT_COOPMODEPARTNERORIENTATIONPOSE] = m_animComponent->findInputIndex(s_CoopModePartnerOrientationPose);
        m_animInputIndex[ANIMINPUT_ISSPRINTING] = m_animComponent->findInputIndex(s_IsSprinting);
        m_animInputIndex[ANIMINPUT_IDLETIME] = m_animComponent->findInputIndex(s_IdleTime);
        m_animInputIndex[ANIMINPUT_GROUNDANGLE] = m_animComponent->findInputIndex(s_GroundAngle);
        m_animInputIndex[ANIMINPUT_ISPATHBLOCKED] = m_animComponent->findInputIndex(s_IsPathBlocked);
        m_animInputIndex[ANIMINPUT_DEATHTYPE] = m_animComponent->findInputIndex(s_DeathType);
        m_animInputIndex[ANIMINPUT_PUNCHORIENTATION] = m_animComponent->findInputIndex(s_punchOrientation);
        m_animInputIndex[ANIMINPUT_MOVECURSOR] = m_animComponent->findInputIndex(s_moveCursor);
        m_animInputIndex[ANIMINPUT_SLIDEACTION] = m_animComponent->findInputIndex(s_SlideAction);
        m_animInputIndex[ANIMINPUT_ISCROUCHED] = m_animComponent->findInputIndex(s_IsCrouched);
        m_animInputIndex[ANIMINPUT_FALLFROMROOF] = m_animComponent->findInputIndex(s_FallFromRoof);
        m_animInputIndex[ANIMINPUT_ONCORNER] = m_animComponent->findInputIndex(s_onCorner);
        m_animInputIndex[ANIMINPUT_AIRPOSE] = m_animComponent->findInputIndex(s_AirPose);
        m_animInputIndex[ANIMINPUT_FALLANGLECURSOR] = m_animComponent->findInputIndex(s_FallAngleCursor);
        m_animInputIndex[ANIMINPUT_HANGTOJUMPCURSOR] = m_animComponent->findInputIndex(s_HangToJump);
        m_animInputIndex[ANIMINPUT_PREPAREHITCURSOR] = m_animComponent->findInputIndex(s_PrepareHitCursor);
        m_animInputIndex[ANIMINPUT_ACTIONFROMUTURN] = m_animComponent->findInputIndex(s_ActionFromUTurn);
        m_animInputIndex[ANIMINPUT_CRUSHTRAVELTIME] = m_animComponent->findInputIndex(s_CrushTravelTime);
        m_animInputIndex[ANIMINPUT_CRUSHRELEASED] = m_animComponent->findInputIndex(s_CrushReleased);
        m_animInputIndex[ANIMINPUT_PUNCHRESULT] = m_animComponent->findInputIndex(s_PunchResult);
        m_animInputIndex[ANIMINPUT_PUNCHDISTANCE] = m_animComponent->findInputIndex(s_PunchDistance);
        m_animInputIndex[ANIMINPUT_ROLLING] = m_animComponent->findInputIndex(s_IsRolling);
        m_animInputIndex[ANIMINPUT_INTERACTING] = m_animComponent->findInputIndex(s_Interacting);
        m_animInputIndex[ANIMINPUT_LAYINGDOWN] = m_animComponent->findInputIndex(s_LayingDown);
        m_animInputIndex[ANIMINPUT_ISLOWROOF] = m_animComponent->findInputIndex(s_IsLowRoof);
        m_animInputIndex[ANIMINPUT_ACCROBATICSTAGE] = m_animComponent->findInputIndex(s_AccrobaticStage);
        m_animInputIndex[ANIMINPUT_JUMPFROMWALLRUN] = m_animComponent->findInputIndex(s_JumpFromWallRun);
        m_animInputIndex[ANIMINPUT_PUNCHBURSTSTAGE] = m_animComponent->findInputIndex(s_PunchBurstStage);
        m_animInputIndex[ANIMINPUT_REVIVECURSOR] = m_animComponent->findInputIndex(s_ReviveCursor);
        m_animInputIndex[ANIMINPUT_STARGATESTAGE] = m_animComponent->findInputIndex(s_StargateStage);
        m_animInputIndex[ANIMINPUT_DIVEMODE] = m_animComponent->findInputIndex(s_diveMode);
        m_animInputIndex[ANIMINPUT_SWIMENTERHEAD] = m_animComponent->findInputIndex(s_swimEnterHead);
        m_animInputIndex[ANIMINPUT_SWIMCURVECURSOR] = m_animComponent->findInputIndex(s_swimCurveCursor);
        m_animInputIndex[ANIMINPUT_SWIMTWIST] = m_animComponent->findInputIndex(s_SwimTwist);
        m_animInputIndex[ANIMINPUT_SWINGQUADRANT] = m_animComponent->findInputIndex(s_SwingQuadrant);
    }
}

void Ray_PlayerControllerComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    m_actor->setScale(m_actor->getWorldInitialScale()*RAY_GAMEMANAGER->getPlayersCurrentScale());
}

void Ray_PlayerControllerComponent::setMaxSpeedLimit( bbool _val )
{
    if ( _val )
    {
        m_characterPhys->enableMaxSpeedClamping(getTemplate()->getClampingMaxSpeedTotal(),
                                                getTemplate()->getClampingMaxSpeedX(),
                                                getTemplate()->getClampingMaxSpeedY());
    }
    else
    {
        m_characterPhys->disableMaxSpeed();
    }
}

void Ray_PlayerControllerComponent::checkSetAirBrake()
{
    f32 xSpeed = m_currentGravityDir.getPerpendicular().dot(m_characterPhys->getSpeed());

    m_airApplyBrake = f32_Abs(xSpeed) <= getTemplate()->getAirBrakeMaxSpeedX();
}

void Ray_PlayerControllerComponent::computeCurrentRegions()
{
    AIUtils::PolylineRegionList regions;

    AIUtils::getPolylineRegions(m_actor->getDepth(),m_actor->get2DPos(),regions);

    u32 numRegions = regions.size();

    m_currentRegions.resize(numRegions);

    for ( u32 i = 0; i < numRegions; i++ )
    {
        const AIUtils::PolylineRegionData& data = regions[i];
        PolylineRegion& newRegion = m_currentRegions[i];

        newRegion.m_regionId = data.m_regionId;
        newRegion.m_polyRef = data.m_polyline->getRef();
    }
}

bbool Ray_PlayerControllerComponent::isPosInWater( const Vec3d& _pos ) const
{
    AIUtils::PolylineRegionList regions;

    AIUtils::getPolylineRegions(_pos.m_z,_pos.truncateTo2D(),regions);

    u32 numRegions = regions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        const AIUtils::PolylineRegionData& data = regions[i];

        if ( data.m_regionId == s_RegionName_Swim )
        {
            return btrue;
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::Update( f32 _deltaTime )
{
    if ( m_forceFindGround )
    {
        m_characterPhys->forceFindStickEdge();
        m_forceFindGround = bfalse;
    }

    m_actor->setWorldInitialPos(m_actor->getPos());

    m_bouncePrevFramePoly = ITF_INVALID_OBJREF;
    m_currentTargetForce = Vec2d::Zero;

    Super::Update(_deltaTime);

    if (_deltaTime == 0.f) // do nothing on pause
        return;

#ifdef ITF_SUPPORT_CHEAT
    if ( RAY_CHEATMANAGER->getIsForceAllPlayersTogether() )
    {
        updateCheatAllPlayersTogether();
    }
#endif

    if ( m_instantKill )
    {
        processInstantKill();
        m_instantKill = bfalse;
    }

    if ( m_bounceCheckEdgeDir != Vec2d::Zero )
    {
        checkBounceEdge();
    }

    if ( m_wasOnGroundPrevFrame )
    {
        if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
        {
            checkSetAirBrake();
        }
        else
        {
            if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
            {
                REWARD_MANAGER->resetTotalOf(Ray_RewardManager::Action_GetBouncedEnemiesSameJump, m_playerIndex);
            }

            if ( m_currentState == &m_stateReleaseHit ||
                 m_currentState == &m_statePunchMitraille )
            {
                m_jumpOnAirTimer = getTemplate()->getJumpOnAirTime();
            }
        }
    }
    else
    {
        if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
        {
            PolyLine * poly = NULL;
            const PolyLineEdge* edge = NULL;

            AIUtils::getPolyLine(m_characterPhys->getStickedPolylineRef(),m_characterPhys->getStickedEdgeIndex(),poly,edge);

            if ( poly && edge )
            {
                const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));
                startLandFx(mat);
            }

            m_kungFuComboTarget.clear();
        }
    }

    updateBaseInfo(_deltaTime);
    computeCurrentRegions();

    m_currentWindForces.clear();
    PHYSWORLD->getWindForces(m_actor->get2DPos(), m_characterPhys->getSpeed(), m_actor->getDepth(), m_currentWindForces);

    // Temporary code to keep the wiimote on WIN32 ON
    // No time to handle it with the new mapping for now
#if defined (ITF_WINDOWS)
    updateWiiInput();
#endif

    updateMoodZone();
    updateRegionCurrent(_deltaTime);
    updateActionPose(_deltaTime);

    m_currentGravity = m_characterPhys->getCurrentGravity();
    m_currentGravityDir = m_characterPhys->getCurrentGravityDir();
    m_currentGravityDir.normalize();

    if ( !isDead() )
    {
        if ( isInDeadRegion() )
        {
#ifdef ITF_SUPPORT_CHEAT
            if (m_currentState != &m_stateFlyMode)
#endif
            {
                triggerDeath(DEATHTYPE_INFINITEFALL);
            }
        }
        else
        {
            if ( m_swingOnActor != ITF_INVALID_OBJREF &&
                 m_currentState != &m_stateSwing )
            {
                changeState(&m_stateSwing);
            }
            else
            {
                processPendingEvents();
            }

            if ( m_animComponent->isLockMagicBoxInterpolating() )
            {
                m_characterPhys->resetWorldSpeed();
            }
        }
    }
    else
    {
        processDeadPendingEvents();
    }

    resetPendingEvents();

    if ( isSupporting() )
    {
    UpdateSupport(_deltaTime);
    }

    if ( isOnPedestal() )
    {
        UpdateSupported(_deltaTime);
    }

    updateStates(_deltaTime);

    updateMoveCursor(_deltaTime);

    //FX CONTROLLER
    updateFx(_deltaTime);

    updateTimers(_deltaTime);

    updateAccrobaticStage(_deltaTime);

    updateSquadData(_deltaTime);
    updateIdleTimer(_deltaTime,bfalse);
    updateGravityMode(_deltaTime);
    updateCameraLimiter(_deltaTime);

    m_wasOnGroundPrevFrame = m_characterPhys->getStickedEdgeIndex() != U32_INVALID;
    m_wallSlideTestSpeed = Vec2d::Zero;

    if ( m_wasOnGroundPrevFrame )
    {
        m_walkOnWallsDisableUntilLand = bfalse;

        if ( m_stance != STANCE_WALLSLIDE )
        {
            m_reducedHitUpDisabledUntilLand = bfalse;
        }
    }

    // update power-ups lifetime
    RAY_GAMEMANAGER->getPowerUpManager().update(_deltaTime);

    // update power-up displays
    for (u32 i=0; i<m_powerUpDisplays.size(); ++i)
    {
        m_powerUpDisplays[i]->update(_deltaTime);
    }

    if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() && m_moveInput.m_x != 0.f )
    {
        if ( isSprinting() && m_currentState == &m_stateIdle )
        {
            f32 distLastPos = ( m_actor->get2DPos() - m_prevPos ).norm();

            REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetSprintDistance, m_playerIndex, distLastPos );
        }
        else if ( isSwimming() && m_currentState == &m_stateSwimIdle )
        {
            f32 distLastPos = ( m_actor->get2DPos() - m_prevPos ).norm();

            REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetSwimDistance, m_playerIndex, distLastPos );
        }
    }

    m_prevPos = m_actor->get2DPos();
    m_justActivated = bfalse;

    updateIceCompensate();

    if (!isDead())
    {
        if (m_moveInput == Vec2d::Zero)
        {
            if (m_softCollisionDisabled)
                restoreSoftCollision();
        }
        else
        {
            if (!m_softCollisionDisabled)
                disableSoftCollision();
        }
    }

    if ( m_checkSprintAward && m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() && !m_playerDisabled && !m_moveToPoint )
    {
        bbool sprintPressed = m_listenerSprint || m_sprintTrigger.m_actif;

        if ( m_prevWasSprint && !sprintPressed )
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

            player->setSprintReleased(btrue);
            m_checkSprintAward = bfalse;
        }
        else if ( m_moveInput.m_x != 0.f && !sprintPressed )
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

            player->setSprintReleased(btrue);
            m_checkSprintAward = bfalse;
        }

        m_prevWasSprint = m_listenerSprint || m_sprintTrigger.m_actif;
    }

    // sprint tutorial: record sprint duration
    if (isActionFlagSet(ACTIONFLAG_SPRINTING))
    {
        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));
        if (player)
        {
            player->addSprintDuration(_deltaTime);
        }
    }

    //piranhas : update timers
    if( m_survivePiranhaPondInside )
    {
        m_survivePiranhaPondTimer += _deltaTime;
    }
    if( m_survivePiranhaPondLastHitTimer > 0.f )
    {
        m_survivePiranhaPondLastHitTimer = f32_Max(0.f, m_survivePiranhaPondLastHitTimer - _deltaTime);
    }
    if( m_survivePiranhaPondOutside )
    {
        if( m_survivePiranhaPondTimer < SURVIVE_PIRANHAS_MIN_TIME_OUTSIDE )
        {
            m_survivePiranhaPondTimer += _deltaTime;
        }
        else
        {
            RewardManager::getptr()->Action_New(Ray_RewardManager::Action_SurvivedPiranhaPond, m_playerIndex);
            m_survivePiranhaPondOutside = bfalse;
            m_survivePiranhaPondTimer = 0.0f;
        }
    }

    cleanListener();
}

void Ray_PlayerControllerComponent::updateTimers( f32 _dt )
{
    m_sprintActivationDelay = Max(m_sprintActivationDelay-_dt,0.f);
    m_climbActivationCounter = Max(m_climbActivationCounter-_dt,0.f);
    m_hangActivationCounter = Max(m_hangActivationCounter-_dt,0.f);
    m_jumpQueueTimer = Max(m_jumpQueueTimer-_dt,0.f);
    m_jumpOnAirTimer = Max(m_jumpOnAirTimer-_dt,0.f);
    m_hitQueueTimer = Max(m_hitQueueTimer-_dt,0.f);
    m_hitDisabledTimer = Max(m_hitDisabledTimer-_dt,0.f);
    m_disableCrouchTimer = Max(m_disableCrouchTimer-_dt,0.f);
    m_bounceTimer = Max(m_bounceTimer-_dt,0.f);
    m_bounceJumpAllowTimer = Max(m_bounceJumpAllowTimer-_dt,0.f);
    m_uTurnComboCounter = Max(m_uTurnComboCounter-_dt,0.f);
    m_crushRetriggerCounter = Max(m_crushRetriggerCounter-_dt,0.f);
    m_wallSlideStickDelayCounter = Max(m_wallSlideStickDelayCounter-_dt,0.f);
    m_inmunityCounter = Max(m_inmunityCounter-_dt,0.f);
    m_inmunityCounterReceiveHit = Max(m_inmunityCounterReceiveHit-_dt,0.f);
    m_onPedestalDisableCounter = Max(m_onPedestalDisableCounter-_dt,0.f);
    m_swimingDashCounter = Max(m_swimingDashCounter-_dt,0.f);
    m_wallJumpDisableTimer = Max(m_wallJumpDisableTimer-_dt,0.f);
    m_lastJumpTimer += _dt;

    m_softCollisionForceMultiplier = 1.f;
    if (m_softCollisionRestoreForceTimer > 0.f)
    {
        const f32 duration = getTemplate()->getSoftCollisionRestoreForceDuration();
        if (m_softCollisionRestoreForceTimer < duration)
        {
            m_softCollisionForceMultiplier = 1 - (m_softCollisionRestoreForceTimer) / duration;
        }
        else // delay
        {
            m_softCollisionForceMultiplier = 0.f;
        }

        m_softCollisionRestoreForceTimer -= _dt;
    }

    if ( m_airHitForceApplicationTimer > 0.f )
    {
        m_airHitForceApplicationTimer -= _dt;

        if ( m_airHitForceApplicationTimer<=0.0f )
        {
            m_airHitForceApplicationTimer = 0.f;
            cancelPhysicAirHit();
        }
    }

    if ( m_releaseSprintActive && m_characterPhys->getStickedEdgeIndex() == U32_INVALID )
    {
        m_releaseSprintActive = bfalse;
    }

    if ( isPerformingUTurn() )
    {
        m_uTurnCounter += _dt;
    }

    if ( m_surfingTimer )
    {
        m_surfingTimer -= _dt;

        if ( m_surfingTimer <= 0.f )
        {
            m_surfingTimer = 0.f;
            m_characterPhys->setDisableWindForces(bfalse);
            m_windDisableCounter = 0.f;
        }
    }

    if ( m_helicopterCooldown )
    {
        m_helicopterCooldown -= _dt;

        if ( m_helicopterCooldown <= 0.f )
        {
            m_helicopterCooldown = 0.f;
            m_helicopterCooldownJustFinished = btrue;
        }
        else
        {
            m_helicopterCooldownJustFinished = bfalse;
        }
    }
    else
    {
        m_helicopterCooldownJustFinished = bfalse;
    }

    if ( m_airControlDisableDelay )
    {
        m_airControlDisableDelay -= _dt;

        if ( m_airControlDisableDelay <= 0.f )
        {
            m_airControlDisableDelay = 0.f;
            m_airControlJustFinished = btrue;
        }
        else
        {
            m_airControlJustFinished = bfalse;
        }
    }
    else
    {
        m_airControlJustFinished = bfalse;
    }

    if ( m_dashAttackCooldown > 0.f )
    {
        m_timeSinceDashAttack = 0.f;
        m_dashAttackCooldown = Max(m_dashAttackCooldown-_dt,0.f);
    }
    else
    {
        m_timeSinceDashAttack = Min(m_timeSinceDashAttack+_dt,100.f);
    }

    if ( m_windDisableCounter > 0.f )
    {
        m_windDisableCounter -= _dt;

        if ( m_windDisableCounter <= 0.f )
        {
            m_windDisableCounter = 0.f;
            m_characterPhys->setDisableWindForces(bfalse);
        }
    }

	if (m_currentState == &m_stateDeadSoul)
    {
		m_stateDeadSoulTimer += _dt;
	}
}

void Ray_PlayerControllerComponent::updateWiiInput()
{
    f32 _axes[ JOY_MAX_AXES ];
    InputAdapter::PressStatus _buttons[JOY_MAX_BUT];

    INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine,m_playerIndex, _buttons, JOY_MAX_BUT);
    INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentEngine,m_playerIndex, _axes, JOY_MAX_AXES);

    InputAdapter::PressStatus   _cpyButtons [JOY_MAX_BUT] ;
    ITF_Memcpy(_cpyButtons, _buttons, sizeof(_cpyButtons));

    const static f32 sf_maxJoyValue = 0.85f;
    const static f32 sf_maxAnalogicalValue = 255.0f;
    const static f32 sf_minAnalogicalValue = 0.0f;

    InputAdapter::PadType padType = INPUT_ADAPTER->getPadType( m_playerIndex );

    if ( padType != InputAdapter::Pad_WiiSideWay &&
         padType != InputAdapter::Pad_WiiClassic &&
         padType != InputAdapter::Pad_WiiNunchuk )
    {
        return;
    }

    switch ( padType )
    {
    case InputAdapter::Pad_WiiSideWay:
    {
    	 // Specific mapping for sideWay (1/2 buttons)
        _buttons[m_joyButton_A] = _cpyButtons[m_joyButton_2];   // 2
        _buttons[m_joyButton_B] = InputAdapter::Released;
        _buttons[m_joyButton_X] = _cpyButtons[m_joyButton_1];

        f32 xValue = sf_maxJoyValue;
        f32 yValue = sf_maxJoyValue;


        // DirPad => joystick
        if (_cpyButtons[m_joyButton_DPadD] == InputAdapter::Pressed)
            _axes[m_joyStickLeft_X] = xValue;
        else if (_cpyButtons[m_joyButton_DPadU] == InputAdapter::Pressed)
            _axes[m_joyStickLeft_X] = -xValue;
        else
            _axes[m_joyStickLeft_X] = 0.0f;

        if (_cpyButtons[m_joyButton_DPadR] == InputAdapter::Pressed)
            _axes[m_joyStickLeft_Y] = yValue;
        else if (_cpyButtons[m_joyButton_DPadL] == InputAdapter::Pressed)
            _axes[m_joyStickLeft_Y] = -yValue;
        else
            _axes[m_joyStickLeft_Y] = 0.0f;
        // !--


#if defined (ITF_WINDOWS)
        if(m_currentState == &m_stateSwimIdle)
        {
            f32 smooth = INPUT_ADAPTER->getSwimSmooth();
            static f32 prevX = _axes[m_joyStickLeft_X];
            static f32 prevY = _axes[m_joyStickLeft_Y];

            _axes[m_joyStickLeft_X] = prevX + (_axes[m_joyStickLeft_X] - prevX ) * smooth ;
            _axes[m_joyStickLeft_Y] = prevY + (_axes[m_joyStickLeft_Y] - prevY ) * smooth ;

            prevX = _axes[m_joyStickLeft_X];
            prevY = _axes[m_joyStickLeft_Y];
        }
#endif

        // Fan activation => B pressed
        if( _cpyButtons[m_joyButton_B] == InputAdapter::Pressed )
            _axes[m_joyTrigger_Left]    = sf_maxAnalogicalValue;
        else
            _axes[m_joyTrigger_Left]    = sf_minAnalogicalValue;

        // Sprint activation => double pressing
        bbool isJustPressing = _cpyButtons[m_joyButton_DPadD] == InputAdapter::JustPressed
            || _cpyButtons[m_joyButton_DPadU] == InputAdapter::JustPressed;

        bbool isPressing = _cpyButtons[m_joyButton_DPadD] == InputAdapter::Pressed
            || _cpyButtons[m_joyButton_DPadU] == InputAdapter::Pressed;

        if(!isPressing && !isJustPressing)
            m_InputIsDoublePressed = bfalse;

        if(isJustPressing)
        {
            f64 currentTime = SYSTEM_ADAPTER->getTime();
            if(m_InputLastTimePressed + InputAdapter::fDoublePressMaxDuration >= currentTime)
            {
                m_InputLastTimePressed = 0.0;
                m_InputIsDoublePressed = btrue;
            }
            else
            {
                m_InputLastTimePressed     = currentTime;
            }
        }


#if defined (ITF_WINDOWS)
        if (INPUT_ADAPTER->getRunUseShake())
        {
            static bbool tempWasDoublePressed = bfalse;

            if(tempWasDoublePressed)
            {
                m_InputIsDoublePressed = _cpyButtons[m_joyButton_DPadD] == InputAdapter::Pressed
                    || _cpyButtons[m_joyButton_DPadU] == InputAdapter::Pressed
                    || _cpyButtons[m_joyButton_DPadL] == InputAdapter::Pressed
                    || _cpyButtons[m_joyButton_DPadR] == InputAdapter::Pressed;

                static f64 timerStopped = 0  ;


                if(!m_InputIsDoublePressed)
                {
                    if(!timerStopped)
                        timerStopped = SYSTEM_ADAPTER->getTime();

                    if(SYSTEM_ADAPTER->getTime() - timerStopped < INPUT_ADAPTER->getRunTimerStop())
                        m_InputIsDoublePressed = btrue;
                    else
                        timerStopped = 0.0;
                } else
                    timerStopped = 0.0;
            } else
            {
                m_InputIsDoublePressed = (_cpyButtons[m_joyActionShaked] == InputAdapter::Pressed);
            }

            tempWasDoublePressed =  m_InputIsDoublePressed;
        }
        else if (INPUT_ADAPTER->getRunUseB())
        {
            m_InputIsDoublePressed = (_cpyButtons[m_joyButton_B] == InputAdapter::Pressed);
        }
#endif

        if ( m_InputIsDoublePressed )
        {
            _axes[m_joyTrigger_Right] = sf_maxAnalogicalValue;
        }
        // !--
        break;
    }
    case InputAdapter::Pad_WiiClassic:
    {
        _buttons[m_joyButton_A] = _cpyButtons[m_joyButton_B];
        _buttons[m_joyButton_X] = _cpyButtons[m_joyButton_Y];
        _buttons[m_joyButton_Y] = _cpyButtons[m_joyButton_X];
        _buttons[m_joyButton_B] = _cpyButtons[m_joyButton_A];

        // No analogical buttons on the classic..
        _axes[m_joyTrigger_Right] = (_cpyButtons[m_joyButton_ZR] == InputAdapter::Pressed)? sf_maxAnalogicalValue : sf_minAnalogicalValue;
        _axes[m_joyTrigger_Left]  = (_cpyButtons[m_joyButton_ZL] == InputAdapter::Pressed)? sf_maxAnalogicalValue : sf_minAnalogicalValue;

        break;
    }
    case InputAdapter::Pad_WiiNunchuk:
    {
        _buttons[m_joyButton_B] = _cpyButtons[m_joyButton_X];
        _buttons[m_joyButton_X] = _cpyButtons[m_joyButton_B];
        _buttons[m_joyButton_Y] = _cpyButtons[m_joyButton_DPadD];

        _axes[m_joyTrigger_Right] = (_cpyButtons[m_joyButton_C] == InputAdapter::Pressed)? sf_maxAnalogicalValue : sf_minAnalogicalValue;
        _axes[m_joyTrigger_Left]  = (_cpyButtons[m_joyButton_Z] == InputAdapter::Pressed)? sf_maxAnalogicalValue : sf_minAnalogicalValue;

        break;
    }
    }

    if ( _buttons[m_joyButton_A] == InputAdapter::JustPressed )
    {
        m_listenerJump = btrue;
        m_listenerHelico = btrue;
    }
    else if ( _buttons[m_joyButton_A] == InputAdapter::Pressed )
    {
        m_listenerJumpHold = btrue;
        m_listenerHelicoHold = btrue;
    }

    if ( _buttons[m_joyButton_B] == InputAdapter::JustPressed ||
         _buttons[m_joyButton_X] == InputAdapter::JustPressed )
    {
        m_listenerAttack = btrue;
    }
    else if ( _buttons[m_joyButton_B] == InputAdapter::Pressed ||
              _buttons[m_joyButton_X] == InputAdapter::Pressed )
    {
        m_listenerAttackHold = btrue;
    }

    if ( _buttons[m_joyButton_Back]==InputAdapter::JustPressed )
    {
        m_listenerLeave = btrue;
    }

    m_listenerMove.m_x = _axes[m_joyStickLeft_X];
    m_listenerMove.m_y = _axes[m_joyStickLeft_Y];
}

void Ray_PlayerControllerComponent::processDeadPendingEvents()
{
    if ( m_punchEventReceived )
    {
        receivePunch(&m_punchEvent);
        m_punchEventReceived = bfalse;
    }
}

void Ray_PlayerControllerComponent::processPendingEvents()
{
    if ( m_squashed )
    {
        triggerDeath(DEATHTYPE_REGULARDEATH);
        m_squashed = bfalse;
    }
    else if ( m_punchEventReceived )
    {
        receivePunch(&m_punchEvent);
        m_punchEventReceived = bfalse;
    }
    else if ( m_handsCaught )
    {
        if ( m_currentState != &m_stateCaught )
        {
			disablePlayer(btrue);
            changeState(&m_stateCaught);
        }

        m_handsCaught = bfalse;
    }
    else if ( m_handsReleased )
    {
        disablePlayer(bfalse);

        if ( m_currentState == &m_stateCaught )
        {
            setIdleState();
        }

        m_handsReleased = bfalse;
    }
    else if ( m_caughtInPipe )
    {
        if ( m_currentState != &m_stateCaughtInPipe )
        {
            changeState(&m_stateCaughtInPipe);
        }

        m_caughtInPipe = bfalse;
    }
    else if ( m_enterDoorReceived )
    {
        RAY_GAMEMANAGER->onHiddenAreaReached(m_enterDoorEvent.getSender(),
                                             m_enterDoorEvent.getDestination(),
                                             m_actor->getRef());

        m_enterDoorReceived = bfalse;
    }
    else if ( m_changePageReceived )
    {
        if ( !RAY_GAMEMANAGER->isRestoreDeathPlaying() && !CINEMATICMANAGER->getLockPlayers() )
        {
            if ( !RAY_GAMEMANAGER->isPlayingPageSequence() )
            {
                Actor* page = AIUtils::getActor(m_changePageEvent.getSender());

                if ( page )
                {
                    Ray_ChangePageComponent *changePageComponent = page->GetComponent<Ray_ChangePageComponent>();
                    if (changePageComponent && !changePageComponent->isWarpZoneExit())
                    {
                        m_fxController->playFX(ITF_GET_STRINGID_CRC(ChangePageFx,4241451235));
                    }
                }
            }

            RAY_GAMEMANAGER->triggerChangePageSequence( m_changePageEvent.getEnterPoint(),
                                                        m_changePageEvent.getExitPoint(),
                                                        m_changePageEvent.getFinalPoint(),
                                                        m_changePageEvent.getIsVerticalEject(),
                                                        m_actor->getRef(),
                                                        m_changePageEvent.getSender(),
                                                        m_changePageEvent.getDestinationPage(),
                                                        m_changePageEvent.getActivateCheckpoint(),
                                                        m_changePageEvent.getWaitPlayerDuration(),
                                                        m_changePageEvent.getUseFade(),
                                                        m_changePageEvent.getIsCageDoor());
        }

        m_changePageReceived = bfalse;
    }
    else if ( m_stargateReceived )
    {
        RAY_GAMEMANAGER->triggerStargateSequence( m_stargateEvent.getSender(),
                                                  m_stargateEvent.getStart(),
                                                  m_stargateEvent.getSpeed() );

        m_stargateReceived = bfalse;
    }
    else if ( m_scaleDoorReceived )
    {
        if ( isPowerUpEnabled(Ray_PowerUp_Reduction) )
        {
            RAY_GAMEMANAGER->triggerScaleDoorSequence( m_scaleDoorTriggerEvent.getSender(),
                                                       m_scaleDoorTriggerEvent.getTargetActor(),
                                                       m_scaleDoorTriggerEvent.getScale(),
                                                       m_scaleDoorTriggerEvent.getApplyAll(),
                                                       m_scaleDoorTriggerEvent.getTargetPos() );
        }

        m_scaleDoorReceived = bfalse;
    }
    else if ( m_scaleTunnelReceived )
    {
        if ( isPowerUpEnabled(Ray_PowerUp_Reduction) )
        {
            f32 originalScale = m_actor->getWorldInitialScale().m_x;
            f32 targetScale = m_actor->getWorldInitialScale().m_x*m_scaleTunnelScale;
            f32 newScale = Interpolate(originalScale,targetScale,m_scaleTunnelT);

            m_actor->setScale(Vec2d(newScale,newScale));
        }

        m_scaleTunnelReceived = bfalse;
    }
    else if ( m_jumpToPosReceived )
    {
        if ( m_jumpToPos.getStop() )
        {
            if ( m_currentState == &m_stateJumpToPos )
            {
                changeState(&m_stateFalling);
            }
        }
        else
        {
            if ( m_jumpToPos.getPos() != Vec2d::Zero )
            {
                m_stateJumpToPos.setTargetPos(m_jumpToPos.getPos());
            }
            else if ( m_jumpToPos.getObjRef() != ITF_INVALID_OBJREF )
            {
                m_stateJumpToPos.setTargetObj(m_jumpToPos.getObjRef());
            }
            else
            {
                m_stateJumpToPos.setTargetPoly(m_jumpToPos.getPolyRef(),m_jumpToPos.getEdgeIndex(),m_jumpToPos.getEdgeT());
            }

            if ( m_currentState == &m_stateJumping ||
                 m_currentState == &m_stateFalling ||
                 m_currentState == &m_stateHelicopter )
            {
                changeState(&m_stateJumpToPos);
            }
        }

        m_jumpToPosReceived = bfalse;
    }
    else if ( m_snapDanceReceived )
    {
        changeState(&m_stateSnapDance);
        m_snapDanceReceived = bfalse;
    }
    else if ( m_throwTeethReceived )
    {
        if ( m_returnTeeth.getStage()==Ray_EventReturnTeeth::STAGE_FINISHED )
        {
            if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID )
            {
                m_characterPhys->forceFindStickEdge();
            }

            changeState(&m_stateIdle);
        }
        else if ( m_returnTeeth.getStage()==Ray_EventReturnTeeth::STAGE_THROW )
        {
            m_statePlayAnim.setAnim(ITF_GET_STRINGID_CRC(THROWTEETH,1781526446));
            changeState(&m_statePlayAnim);
        }
        else
        {
            m_statePlayAnim.setAnim(ITF_GET_STRINGID_CRC(CHEER,319293990));
            changeState(&m_statePlayAnim);
        }

        m_throwTeethReceived = bfalse;
    }
    else if ( m_playAnimReceived )
    {
        m_statePlayAnim.setAnim(m_playAnimEvent.getAnim());
        changeState(&m_statePlayAnim);
        m_playAnimReceived = bfalse;
    }
}

void Ray_PlayerControllerComponent::updateMoveCursor( f32 _dt )
{
    if ( getMoveData().getMoveDirection() == MOVEDIR_NONE ||
         m_animComponent->isInTransition() ||
         m_currentState != &m_stateIdle ||
         isPerformingUTurn() )
    {
        m_moveCursor = 0.f;
    }
    else
    {
        f32 groundAngle = f32_Abs(getVec2Angle(m_characterPhys->getCurrentGroundDir()));
        f32 dtMultiplier = 1.f;

        if ( groundAngle <= getTemplate()->getMoveCursorEfficiencyAngle() )
        {
            // Use efficiency to calculate the time multiplier
            if ( m_achievedSpeedFactor <= getTemplate()->getMoveCursorMinEfficiency() )
            {
                dtMultiplier = getTemplate()->getMoveCursorMaxMultiplier();
            }
            else if ( m_achievedSpeedFactor >= getTemplate()->getMoveCursorMaxEfficiency() )
            {
                dtMultiplier = getTemplate()->getMoveCursorMaxMultiplier();
            }
            else
            {
                f32 range = ( getTemplate()->getMoveCursorMaxEfficiency() - getTemplate()->getMoveCursorMinEfficiency() ) * 0.5f;
                f32 midEfficiency = range + getTemplate()->getMoveCursorMinEfficiency();

                f32 dif = f32_Abs( m_achievedSpeedFactor - midEfficiency );
                f32 t = dif/range;

                dtMultiplier = Interpolate(getTemplate()->getMoveCursorMinMultiplier(),getTemplate()->getMoveCursorMaxMultiplier(),t);
            }
        }
        else
        {
            // Use ground angle to calculate the time multiplier
            if ( groundAngle <= getTemplate()->getMoveCursorMinAngle() )
            {
                dtMultiplier = getTemplate()->getMoveCursorMinMultiplier();
            }
            else if ( groundAngle >= getTemplate()->getMoveCursorMaxAngle() )
            {
                dtMultiplier = getTemplate()->getMoveCursorMaxMultiplier();
            }
            else
            {
                f32 range = ( getTemplate()->getMoveCursorMaxAngle() - getTemplate()->getMoveCursorMinAngle() ) * 0.5f;
                f32 midAngle = range + getTemplate()->getMoveCursorMinAngle();

                f32 dif = f32_Abs( groundAngle - midAngle );
                f32 t = dif/range;

                dtMultiplier = Interpolate(getTemplate()->getMoveCursorMinMultiplier(),getTemplate()->getMoveCursorMaxMultiplier(),t);
            }
        }

        m_moveCursor += _dt * dtMultiplier;
    }
}

void Ray_PlayerControllerComponent::updateIdleTimer(f32 _dt, bbool _analog )
{
    bbool reset;

    if ( isHitQueued() || isJumpQueued() || isSprinting() )
    {
        reset = btrue;
    }
    else if ( _analog && getMoveDataAnalog().getMove() != Vec2d::Zero )
    {
        reset = btrue;
    }
    else if ( !_analog && getMoveData().getMove() != Vec2d::Zero )
    {
        reset = btrue;
    }
    else if ( m_characterPhys->getWorldSpeedAverage() != Vec2d::Zero )
    {
        reset = btrue;
    }
    else
    {
        reset = bfalse;
    }

    if ( reset )
    {
        m_idleTimer = 0.f;
    }
    else
    {
        m_idleTimer += _dt;
    }
}


void Ray_PlayerControllerComponent::updateAccrobaticStage( f32 _dt )
{
    if ( !m_accrobaticStageKeepCounter ||
         m_currentState == &m_stateJumping ||
         isRolling() ||
         isPerformingSlide() ||
         isPerformingUTurn() )
    {
        return;
    }

    m_accrobaticStageKeepCounter = Max(m_accrobaticStageKeepCounter-_dt,0.f);

    if ( !m_accrobaticStageKeepCounter )
    {
        resetAccrobaticStage();
    }
}

void Ray_PlayerControllerComponent::addAccrobaticStage()
{
    m_currentAccrobaticStage++;
    m_accrobaticStageKeepCounter = getTemplate()->getAccrobaticStageKeepTime();
}

void Ray_PlayerControllerComponent::updateCheatAllPlayersTogether()
{
    if ( m_playerIndex == 0 )
    {
        if ( !m_playerDisabled )
        {
            for (u32 i=0; i<GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                if ( i != m_playerIndex && !GAMEMANAGER->getPlayer(i)->getActive() )
                {
                    GAMEMANAGER->changePlayerActivation(i,bfalse);
                }
            }
        }
        return;
    }

    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(0));

    if ( player )
    {
        Actor* playerActor = player->getActor();

        if ( playerActor )
        {
            if ( !m_playerDisabled )
            {
                EventStopPlayer stopPlayer(btrue);

                m_actor->onEvent(&stopPlayer);
            }

            EventTeleport teleport(playerActor->get2DPos().to3d(m_actor->getPos().m_z),playerActor->getAngle(),bfalse);

            m_actor->onEvent(&teleport);
        }
    }
}

void Ray_PlayerControllerComponent::processInstantKill()
{
    if ( m_currentState != &m_stateDeath &&
         m_currentState != &m_stateDeadSoul )
    {
        triggerDeath(DEATHTYPE_INFINITEFALL);
    }
}

void Ray_PlayerControllerComponent::clearSwing()
{
    if ( m_swingOnActor == ITF_INVALID_OBJREF )
    {
        return;
    }

    m_animComponent->resetLockMagicBox();

    if ( m_characterPhys->getDisabled() )
    {
        m_characterPhys->setDisabled(bfalse);
    }

    Actor* swingActor = AIUtils::getActor(m_swingOnActor);

    if ( swingActor )
    {
        Ray_EventSwingDetach detachEvent(m_actor->getRef());
        swingActor->onEvent(&detachEvent);
    }

    m_swingOnActor = ITF_INVALID_OBJREF;
}

void Ray_PlayerControllerComponent::updateStickOnWalls( f32 _dt )
{
    bbool stickOnWalls = isSprinting() &&
                         m_stance == STANCE_STAND &&
                       ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID || m_currentJumpWallRun ) &&
                         isPowerUpEnabled(Ray_PowerUp_WalkOnWallsGlobal) &&
                        !m_walkOnWallsDisableUntilLand;

    if ( stickOnWalls )
    {
        m_walkOnWallsDisableTimer = getTemplate()->getWalkOnWallsDisableDelay();
    }
    else
    {
        m_walkOnWallsDisableTimer = Max(m_walkOnWallsDisableTimer-_dt,0.f);
    }

    if ( m_currentState != &m_stateWallSlide )
    {
        m_characterPhys->setStickOnWalls(m_walkOnWallsDisableTimer!=0.f);
        m_characterPhys->setStickOnWallsInputDir(getMoveData().getMove());
    }

    bbool resetStickTimer = btrue;

    if ( m_walkOnWallsDisableTimer != 0.f &&
         m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();

        if ( currentEdge )
        {
            StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector);

            if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL ||
                 edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF )
            {
                resetStickTimer = getMoveData().getMoveDirection() != MOVEDIR_NONE;

                if ( !m_wasOnGroundPrevFrame )
                {
                    setCrouch(bfalse);

                    if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
                    {
                        f32 dotProd = m_currentGravityDir.dot(m_characterPhys->getSpeed());

                        if ( dotProd >= 0.f )
                        {
                            if ( dotProd <= getTemplate()->getWallRunUpThreshold() )
                            {
                                Vec2d newSpeed = -m_characterPhys->getSpeed();
                                bbool goRight = currentEdge->m_normalizedVector.dot(newSpeed) > 0.f;
                                m_characterPhys->setSpeed(newSpeed);
                                m_actor->setFlipped(!goRight);
                                resetDesiredLookDir();
                            }
                            else
                            {
                                m_stateWallSlide.forceWallSlideEdge(m_characterPhys->getStickedPolylineRef(),m_characterPhys->getStickedEdgeIndex());
                                changeState(&m_stateWallSlide);
                                return;
                            }
                        }
                    }

                    f32 currentSpeedSq = m_characterPhys->getSpeed().sqrnorm();

                    if ( currentSpeedSq < (getTemplate()->getWallRunMinSpeed()*getTemplate()->getWallRunMinSpeed()) )
                    {
                        Vec2d newSpeed = m_characterPhys->getSpeed();

                        newSpeed.normalize();
                        newSpeed *= getTemplate()->getWallRunMinSpeed();

                        m_characterPhys->setSpeed(newSpeed);
                    }
                }
                else
                {
                    if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF &&
                         getMoveData().getMoveDirection() == MOVEDIR_DOWN )
                    {
                        resetStickTimer = btrue;
                        m_walkOnWallsDisableTimer = 0.f;
                        m_characterPhys->setStickOnWalls(bfalse);
                        m_characterPhys->unstick();
                    }
                    else
                    {
                        m_walkOnWallKeepDirFromSpeed = btrue;
                    }
                }
            }
        }
    }

    if ( m_walkOnWallKeepDirFromSpeed )
    {
        if ( getMoveData().getMoveSameDir() == bfalse || ( m_listenerSprint == bfalse && m_sprintTrigger.m_actif == bfalse ) )
        {
            m_walkOnWallKeepDirFromSpeed = bfalse;
        }
        else if ( m_currentState != &m_stateIdle &&
                  m_currentState != &m_stateFalling &&
                  m_currentState != &m_stateReleaseHit &&
                  m_currentState != &m_stateJumping )
        {
            m_walkOnWallKeepDirFromSpeed = bfalse;
        }
    }
}

void Ray_PlayerControllerComponent::updateSprint()
{
    // Trigger the sprint (used by sideway)
    // If we are moving, or just triggered, or actif, set the timer to null ..
    if ( ( m_listenerSprintTrigger || m_sprintTrigger.m_actif ) && m_listenerMove != Vec2d::Zero )
    {
        m_sprintTrigger.m_actif = btrue;
        m_sprintTrigger.m_timer = 0.0;

    } else if ( m_sprintTrigger.m_actif && m_listenerMove == Vec2d::Zero )
    {
        if ( !m_sprintTrigger.m_timer )
            m_sprintTrigger.m_timer = SYSTEM_ADAPTER->getTime();

        if(SYSTEM_ADAPTER->getTime() - m_sprintTrigger.m_timer > 0.20)
        {
            m_sprintTrigger.m_actif = bfalse;
        }
    }


    if ( m_stance == STANCE_STAND )
    {
        if ( m_sprintActivationDelay && m_moveInput.m_x )
        {
            m_sprintActivationDelay = 0.f;
        }

        bbool sprinting = m_sprintActivationDelay == 0.f && trySprint();

        setSprinting(sprinting);
    }
    else if ( m_stance == STANCE_SWIM )
    {
        bbool sprinting = m_swimingDashCounter > 0.f || m_listenerSprint || m_sprintTrigger.m_actif;

        setSprinting(sprinting);
    }
    else
    {
        setSprinting(bfalse);
    }
}

void Ray_PlayerControllerComponent::updateStates( f32 _deltaTime )
{
    if (m_playerDisabled && m_currentState != &m_stateDeath )
    {
        return;
    }

    UpdateMoveInput(_deltaTime);

#ifdef ITF_SUPPORT_CHEAT
    if ( CHEATMANAGER->getActive() )
    {
        updateCheats();
    }
#endif

    if ( m_listenerJump )
    {
        m_jumpQueueTimer = getTemplate()->getJumpQueueTime();
    }

    if ( m_listenerAttack &&
         isPowerUpEnabled(Ray_PowerUp_Fight) )
    {
        m_hitQueueTimer = getTemplate()->getHitQueueTime();
    }

    if ( m_listenerLeave &&
        !m_justActivated &&
         m_playerIndex != RAY_GAMEMANAGER->getMainIndexPlayer() &&
        !RAY_GAMEMANAGER->isSequencePlaying() &&
         CINEMATICMANAGER->getLockPlayers() == 0 )
    {
        // die / leave with 'back' button
        if ( m_currentState == &m_stateDeadSoul )
        {
            m_stateDeadSoul.triggerDeath();
        }
        else
        {
            if ( !isDead() &&
                 GAMEMANAGER->getNumActivePlayers() > 1 )  // no suicide in solo
            {
                removeAllHealth();
                triggerDeath(DEATHTYPE_REGULARDEATH);
            }
        }
    }

    if ( m_bounceToLayer )
    {
        m_bounceToLayer = bfalse;
        resetBounce();
        changeState(&m_stateBounceToLayer);
    }
    else
    {
        if ( m_stance != STANCE_SWIM &&
#ifdef ITF_SUPPORT_CHEAT
             m_currentState != &m_stateFlyMode &&
#endif
             !isDead() &&
             m_currentState != &m_stateBounceToLayer &&
             m_currentState != &m_stateReceiveHit )
        {
            trySwim(btrue);
        }
    }

    updateSprint();
    updateStickOnWalls(_deltaTime);
    updateCurrentState(_deltaTime);
    updateCrouch(_deltaTime);

    m_previousSpeed = m_characterPhys->getSpeed();
}

void Ray_PlayerControllerComponent::updateCrouch( f32 _deltaTime )
{
    if ( isCrouch() )
    {
        if ( !isRolling() &&
             !isPerformingSlide() &&
           ( getMoveData().getMoveDirection() == MOVEDIR_UP ||
             getMoveData().getMoveDirection() == MOVEDIR_NONE ) )
        {
            setCrouch(bfalse);
        }

        m_unCrouchCounter = 0.f;
    }
    else
    {
        m_unCrouchCounter = Min(m_unCrouchCounter+_deltaTime,getTemplate()->getCrouchUnhangTime());
    }
}

void Ray_PlayerControllerComponent::updateAnimInput()
{
    if ( m_currentState && !isPaused() && m_animInputIndex.size() > 0 )
    {
        f32 totalSpeed = 0.f;
        f32 speedX = 0.f;
        f32 speedY = 0.f;
        f32 groundAngle = getVec2Angle(m_characterPhys->getCurrentGroundDir());
        u32 inAir = 0;

        const Vec2d& speed = m_characterPhys->getSpeed();
        const PolyLineEdge* edge = m_characterPhys->getStickedEdge();
        Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

        if ( m_actor->isFlipped() )
        {
            dir = -dir;
            groundAngle *= -1.f;
            NormalizeAnglePI(groundAngle);
        }

        totalSpeed = speed.norm();

        speedX = speed.dot(dir);
        speedY = speed.dot(-m_currentGravityDir);

        inAir = edge == NULL && !isOnPedestal();

        if ( m_flipActor )
        {
            setLookRight(m_actor->isFlipped());

            if ( isPerformingUTurn() )
            {
                m_uTurnFlipped = btrue;
            }

            m_flipActor = bfalse;
        }

        if ( m_uTurnFinished )
        {
            resetUTurn();
        }

        f32 localAngle = m_actor->getAngle();
        f32 averageSpeedX = m_characterPhys->getWorldSpeedAverageX();
        f32 averageSpeedY = m_characterPhys->getWorldSpeedAverageY();

        if ( m_actor->isFlipped() )
        {
            localAngle *= -1.f;
            averageSpeedX *= -1.f;
        }

        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_SPEED],totalSpeed);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_SPEEDX],speedX);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_SPEEDY],speedY);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_AVGSPEEDX],averageSpeedX);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_AVGSPEEDY],averageSpeedY);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_MOVE],m_moveInput.norm());
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_MOVEX],m_moveInput.m_x);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_MOVEY],m_moveInput.m_y);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_STANCE],static_cast<u32>(m_stance));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_PREVSTANCE],static_cast<u32>(m_prevStance));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_INAIR],inAir);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_PUNCHLEVEL],m_currentPunchHitLevel);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_RECEIVEHITLEVEL],m_receivedHitLevel);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_ORIENTATIONPOSE],static_cast<u32>(m_orientationPose));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_ACTIONPOSE],static_cast<u32>(m_actionPose));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_MOODZONE],static_cast<u32>(m_moodZone));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_ANGLE],MTH_RADTODEG*localAngle);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_UTURN],static_cast<u32>(m_performingUTurn));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_ACHIEVEDSPEEDFACTOR],m_achievedSpeedFactor);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_PUNCHHITTYPE],static_cast<u32>(m_currentPunchHitType));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_RECEIVEDHITTYPE],static_cast<u32>(m_receivedHitType));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_SUPPORTING],static_cast<u32>(isSupporting()));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_COOPMODE],static_cast<u32>(m_coopActionMode) );
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_ISSPRINTING],static_cast<u32>(isSprinting()));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_IDLETIME],m_idleTimer);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_GROUNDANGLE],groundAngle*MTH_RADTODEG);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_ISPATHBLOCKED],static_cast<u32>(isPathBlocked()));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_DEATHTYPE],static_cast<u32>(m_currentDeathType));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_PUNCHORIENTATION],m_currentPunchOrientation);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_MOVECURSOR],m_moveCursor);
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_SLIDEACTION],static_cast<u32>(isPerformingSlide()));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_ISCROUCHED],static_cast<u32>(isCrouch()));
        m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_FALLFROMROOF],m_fallFromRoof);

        if ( m_coopActionMain != ITF_INVALID_OBJREF )
        {
            Actor* partner = AIUtils::getActor(m_coopActionMain);

            if ( partner )
            {
                Ray_EventQueryStanceInfo query;

                partner->onEvent(&query);

                m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_COOPMODEPARTNERSTANCE],static_cast<u32>(query.getStance()) );
                m_animComponent->setInputFromIndex(m_animInputIndex[ANIMINPUT_COOPMODEPARTNERORIENTATIONPOSE],static_cast<u32>(query.getOrientationPose()) );
            }
        }

        m_currentState->updateAnimInputs();
    }
}

void Ray_PlayerControllerComponent::updateSoundInput()
{
    #define s_Speed ITF_GET_STRINGID_CRC(Speed,4218325830)                                                // Absolute speed value
    #define s_SpeedX ITF_GET_STRINGID_CRC(SpeedX,3996234170)                                              // Speed on X (positive in the direction of the character)
    #define s_SpeedY ITF_GET_STRINGID_CRC(SpeedY,461177981)                                              // Speed on Y (positive in the opposite direction of the gravity)
    #define s_PunchLevel ITF_GET_STRINGID_CRC(PunchLevel,3044141386)

    f32 totalSpeed = 0.f;
    f32 speedX = 0.f;
    f32 speedY = 0.f;

    const Vec2d& speed = m_characterPhys->getSpeed();
    Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

    if ( m_actor->isFlipped() )
    {
        dir = -dir;
    }

    totalSpeed = speed.norm();

    speedX = speed.dot(dir);
    speedY = speed.dot(-m_currentGravityDir);

    m_soundComponent->setInput(s_Speed,totalSpeed);
    m_soundComponent->setInput(s_SpeedX,speedX);
    m_soundComponent->setInput(s_SpeedY,speedY);
    m_soundComponent->setInput(s_PunchLevel,m_currentPunchHitLevel);

    if ( m_currentState && !isPaused() )
    {
        m_currentState->updateSoundInputs();
    }
}

void Ray_PlayerControllerComponent::updateFxInput()
{
    #define s_Speed ITF_GET_STRINGID_CRC(Speed,4218325830)                                                // Absolute speed value
    #define s_SpeedX ITF_GET_STRINGID_CRC(SpeedX,3996234170)                                              // Speed on X (positive in the direction of the character)
    #define s_SpeedY ITF_GET_STRINGID_CRC(SpeedY,461177981)                                              // Speed on Y (positive in the opposite direction of the gravity)
    #define s_PunchLevel ITF_GET_STRINGID_CRC(PunchLevel,3044141386)

    f32 totalSpeed = 0.f;
    f32 speedX = 0.f;
    f32 speedY = 0.f;

    const Vec2d& speed = m_characterPhys->getSpeed();
    Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

    if ( m_actor->isFlipped() )
    {
        dir = -dir;
    }

    totalSpeed = speed.norm();

    speedX = speed.dot(dir);
    speedY = speed.dot(-m_currentGravityDir);


    m_fxBankComponent->setInput(s_Speed,totalSpeed);
    m_fxBankComponent->setInput(s_SpeedX,speedX);
    m_fxBankComponent->setInput(s_SpeedY,speedY);
    m_fxBankComponent->setInput(s_PunchLevel,m_currentPunchHitLevel);

    if ( m_currentState && !isPaused() )
    {
        m_currentState->updateFxInputs();
    }
}

#ifdef ITF_SUPPORT_CHEAT
void Ray_PlayerControllerComponent::updateCheats()
{
    if ( m_listenerFlyCheat )
    {
        detachFromEverything();
        changeState(&m_stateFlyMode);
    }
}
#endif

void Ray_PlayerControllerComponent::processDetachEvent( EventDetach* _detachEvent )
{
    if ( _detachEvent->getPolyLine() == ITF_INVALID_OBJREF )
    {
        detachFromEverything();
    }
    else
    {
        if ( isHanging() )
        {
            if ( m_hangingPolyline == _detachEvent->getPolyLine() )
            {
                if ( _detachEvent->getEdge() == U32_INVALID ||
                     _detachEvent->getEdge() == m_hangingEdge )
                {
                    clearHanging();
                }
            }
        }
        else if ( isClimbing() )
        {
            if ( m_climbingPolyline == _detachEvent->getPolyLine() )
            {
                if ( _detachEvent->getEdge() == U32_INVALID ||
                    _detachEvent->getEdge() == m_climbingEdge )
                {
                    clearClimbing();
                }
            }
        }
        else
        {
            if ( m_characterPhys->getStickedPolylineRef() == _detachEvent->getPolyLine() )
            {
                if ( _detachEvent->getEdge() == U32_INVALID ||
                     _detachEvent->getEdge() == m_characterPhys->getStickedEdgeIndex() )
                {
                    m_characterPhys->unstick();
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::detachFromEverything()
{
    clearHanging();
    clearClimbing();
    clearSwing();
    m_characterPhys->unstick();
}

void Ray_PlayerControllerComponent::updateFx(f32 _deltaTime)
{
    ObjectRef polyLine = ITF_INVALID_OBJREF;
    u32 polyLineEdge = U32_INVALID;

    PolyLine* p = m_characterPhys->getStickedPolyline();
    if (p)
    {
        polyLine        = p->getRef();
        polyLineEdge    = m_characterPhys->getStickedEdgeIndex();
    }


    if (polyLine == ITF_INVALID_OBJREF)
    {
        if (isHanging())
        {
            polyLine       = getHangingPolyline();
            polyLineEdge   = getHangingEdge();
        }
        else if (isClimbing())
        {
            polyLine       = getClimbingPolyline();
            polyLineEdge   = getClimbingEdge();
        }
    }
    StringID matId;
    StringID prevMat = m_fxController->getMaterialID();

    if (polyLineEdge != U32_INVALID)
    {
        BaseObject* obj = GETOBJECT(polyLine);
        if ( obj )
        {
            PolyLine * poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));
            if (poly)
            {
                matId = poly->getEdgeAt(polyLineEdge).getGameMaterial();
            }
        }
    }

	m_fxController->setMaterial(matId);

    if ( isSlideFx() && matId != prevMat )
    {
        stopSlideFx();
        startSlideFx();
    }

    if (p && m_moveInput.m_x == 0.0f && m_characterPhys->getSpeed().norm() > 0.0f && !isSlideFx())
    {
        startSlideFx();
    }
    else if (isSlideFx() && !isActionFlagSet(ACTIONFLAG_SLIDE) && ( !p || m_moveInput.m_x != 0.0f || (m_moveInput.m_x == 0.0f && m_characterPhys->getSpeed().norm() == 0.0f)))
        stopSlideFx();

    Actor* tinyFX = m_tinyFXRef.getActor();

    if ( tinyFX )
    {
        tinyFX->setPos(m_actor->getPos());

        bbool isReduced = isSizeReduced();
        bbool stateChanged = bfalse;

        if ( ( isReduced && m_tinyTrailDisabled ) ||
             ( !isReduced && !m_tinyTrailDisabled ) )
        {
            m_tinyTrailDisabled = !m_tinyTrailDisabled;
            stateChanged = btrue;
        }

        if ( stateChanged || !m_tinyTrailDisabled )
        {
            EventTrail trail(!m_tinyTrailDisabled);

            if ( !m_tinyTrailDisabled )
            {
                if ( m_currentState == &m_stateJumping && m_currentJumpWallRun )
                {
                    m_tinyTrailTargetColor = getTemplate()->getTinyTrailWallRunJumpColor();
                }
                else if ( isSprinting() && m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
                {
                    if ( m_characterPhys->getEdgeOrientationType(m_characterPhys->getCurrentGroundDir()) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
                    {
                        m_tinyTrailTargetColor = getTemplate()->getTinyTrailWallRunColor();
                    }
                    else
                    {
                        m_tinyTrailTargetColor = getTemplate()->getTinyTrailSprintColor();
                    }
                }
                else
                {
                    m_tinyTrailTargetColor = getTemplate()->getTinyTrailNormalColor();
                }
            }

            if ( stateChanged )
            {
                m_tinyTrailCurrentColor = m_tinyTrailTargetColor;
            }
            else
            {
                m_tinyTrailCurrentColor = Interpolate(m_tinyTrailCurrentColor,m_tinyTrailTargetColor,0.1f);
            }

            trail.setColor(m_tinyTrailCurrentColor);

            tinyFX->onEvent(&trail);
        }
    }

    updateSprintFx();
}

void Ray_PlayerControllerComponent::updateSprintFx()
{
    bbool wallrunJump = m_currentState == &m_stateJumping && m_currentJumpWallRun;
    bbool playSprint = isSprinting() &&
                      !isSwimming() &&
                      !isCrouch() &&
                      !isPathBlocked() &&
                      !isSupporting() &&
                       m_characterPhys->getStickedEdgeIndex() != -1 &&
                      !m_playerDisabled;

    if ( isSizeReduced() )
    {
        stopSprintFx();
        stopWallRunFx();

        if ( playSprint || wallrunJump )
        {
            const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();
            bbool wallRun = currentEdge && m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND;

            if ( wallRun || wallrunJump )
            {
                startWallRunReducedFx();
                stopSprintReducedFx();
            }
            else
            {
                startSprintReducedFx();
                stopWallRunReducedFx();
            }
        }
        else
        {
            stopSprintReducedFx();
            stopWallRunReducedFx();
        }
    }
    else
    {
        stopSprintReducedFx();
        stopWallRunReducedFx();

        if ( playSprint || wallrunJump )
        {
            const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();
            bbool wallRun = currentEdge && m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND;

            if ( wallRun || wallrunJump )
            {
                startWallRunFx();
                stopSprintFx();
            }
            else
            {
                startSprintFx();
                stopWallRunFx();
            }
        }
        else
        {
            stopSprintFx();
            stopWallRunFx();
        }
    }
}

void Ray_PlayerControllerComponent::startSprintFx()
{
    if (m_sprintFxHandle == U32_INVALID)
    {
        const GameMaterial_Template* mat = NULL;
        const PolyLineEdge * edge = m_characterPhys->getStickedEdge();
        if (edge)
        {
            mat = World::getGameMaterial(edge->getGameMaterial());
        }

        m_sprintFxHandle = m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(Sprint,183769305),mat);
    }
}

void Ray_PlayerControllerComponent::stopSprintFx()
{
    if (m_sprintFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_sprintFxHandle);
        m_sprintFxHandle = U32_INVALID;
    }
}

void Ray_PlayerControllerComponent::startSprintReducedFx()
{
    if (m_sprintReducedFxHandle == U32_INVALID)
    {
        const GameMaterial_Template* mat = NULL;
        const PolyLineEdge * edge = m_characterPhys->getStickedEdge();
        if (edge)
        {
            mat = World::getGameMaterial(edge->getGameMaterial());
        }

        m_sprintReducedFxHandle = m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(SprintReduced,785441768),mat);
    }
}

void Ray_PlayerControllerComponent::stopSprintReducedFx()
{
    if (m_sprintReducedFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_sprintReducedFxHandle);
        m_sprintReducedFxHandle = U32_INVALID;
    }
}

void Ray_PlayerControllerComponent::startWallRunFx()
{
    if (m_wallRunFxHandle == U32_INVALID)
    {
        const GameMaterial_Template* mat = NULL;
        const PolyLineEdge * edge = m_characterPhys->getStickedEdge();
        if (edge)
        {
            mat = World::getGameMaterial(edge->getGameMaterial());
        }

        m_wallRunFxHandle = m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(WallRun,1710053959),mat);
    }
}

void Ray_PlayerControllerComponent::stopWallRunFx()
{
    if (m_wallRunFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_wallRunFxHandle);
        m_wallRunFxHandle = U32_INVALID;
    }
}

void Ray_PlayerControllerComponent::startWallRunReducedFx()
{
    if (m_wallRunReducedFxHandle == U32_INVALID)
    {
        const GameMaterial_Template* mat = NULL;
        const PolyLineEdge * edge = m_characterPhys->getStickedEdge();
        if (edge)
        {
            mat = World::getGameMaterial(edge->getGameMaterial());
        }

        m_wallRunReducedFxHandle = m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(WallRunReduced,1723926386),mat);
    }
}

void Ray_PlayerControllerComponent::stopWallRunReducedFx()
{
    if (m_wallRunReducedFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_wallRunReducedFxHandle);
        m_wallRunReducedFxHandle = U32_INVALID;
    }
}

bbool Ray_PlayerControllerComponent::internalOnEvent( Event * _event )
{
    bbool retVal = Super::internalOnEvent(_event);

    if ( retVal )
    {
        return btrue;
    }

    if ( BounceStim* bounceStim = _event->DynamicCast<BounceStim>(ITF_GET_STRINGID_CRC(BounceStim,819805069)) )
    {
        processBounceStim(bounceStim);
    }
    else if ( PunchStim* punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
    {
        // ignore domino hits
        if ( !punch->IsClassCRC(ITF_GET_STRINGID_CRC(RehitStim,1008690800)))
        {
            if ( m_currentState == &m_stateDeadSoul )
            {
                m_stateDeadSoul.processPunch(punch);
            }
            else
            {
                m_punchEventReceived = btrue;
                m_punchEvent = *punch;
            }
        }
    }
    else if ( EventQueryIsDead* query = _event->DynamicCast<EventQueryIsDead>(ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443)) )
    {
        query->setIsDead(isDead());
    }
    else if ( EventStickOnPolylineUpdate * stickOnPolyEvent = _event->DynamicCast<EventStickOnPolylineUpdate>(ITF_GET_STRINGID_CRC(EventStickOnPolylineUpdate,2702146949)) )
    {
        processEventPolylineUpdate(stickOnPolyEvent);
    }
    else if ( EventBlockedByPolyline * blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
    {
        processBlockedByPolylineEvent(blockedEvent);
    }
    else if ( AnimGameplayEvent * animTrackEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        handleAnimTrackEvent(animTrackEvent);
    }
    else if ( EventPhysicsUpdated* physUpdated = _event->DynamicCast<EventPhysicsUpdated>(ITF_GET_STRINGID_CRC(EventPhysicsUpdated,2632338894)) )
    {
        processPhysicsUpdated();
    }
    else if ( Ray_EventQueryIsAccrobatic* query = _event->DynamicCast<Ray_EventQueryIsAccrobatic>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsAccrobatic,1083236336)) )
    {
        if (m_currentAccrobaticStage > 0)
            query->setValue(btrue);
    }
    else if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
    {
        processInteractionQuery(query);
    }
    else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
    {
        processCrush(crushed);
    }
    else if ( EventDetach* detachEvent = _event->DynamicCast<EventDetach>(ITF_GET_STRINGID_CRC(EventDetach,889341216)) )
    {
        processDetachEvent(detachEvent);
    }
    else if ( Ray_EventPowerUp* powerUpEvent = _event->DynamicCast<Ray_EventPowerUp>(ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764)) )
    {
        processPowerUp(powerUpEvent);
    }
    else if (EventQuerySnapData* query = _event->DynamicCast<EventQuerySnapData>(ITF_GET_STRINGID_CRC(EventQuerySnapData,747808288)))
    {
        processSnapDataQuery(query);
    }
    else if (Ray_EventPlayLockAnim * lockanim = _event->DynamicCast<Ray_EventPlayLockAnim>(ITF_GET_STRINGID_CRC(Ray_EventPlayLockAnim,3679647184)))
    {
        if (lockanim->getActivate())
        {
            m_lockSnapActor = lockanim->getSnapActor();
            m_stateLock.setData(lockanim->getLookRight());

            if ( &m_stateLock != m_currentState )
            {
                changeState(&m_stateLock);
            }
        }
    }
    else if (Ray_EventSwingLatch * swingLatch = _event->DynamicCast<Ray_EventSwingLatch>(ITF_GET_STRINGID_CRC(Ray_EventSwingLatch,2013309652)))
    {
        processSwingLatchEvent(swingLatch);
    }
    else if (Ray_EventHangOnCharacter* hangOnCharacter = _event->DynamicCast<Ray_EventHangOnCharacter>(ITF_GET_STRINGID_CRC(Ray_EventHangOnCharacter,1277553679)))
    {
        processHangOnCharacter(hangOnCharacter);
    }
    else if (Ray_EventQueryHangInfo* queryHangInfo = _event->DynamicCast<Ray_EventQueryHangInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryHangInfo,4077208499)))
    {
        processQueryHangInfo(queryHangInfo);
    }
    else if (Ray_EventMagnetTrigger* enterMagnet = _event->DynamicCast<Ray_EventMagnetTrigger>(ITF_GET_STRINGID_CRC(Ray_EventMagnetTrigger,106997702)))
    {
        processMagnetTrigger(enterMagnet);
    }
    else if ( EventPlayerActivationChanged* eventActivation = _event->DynamicCast<EventPlayerActivationChanged>(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208)) )
    {
        processPlayerActivationChanged(eventActivation);
    }
    else if ( EventPlayerHpChanged* eventHP = _event->DynamicCast<EventPlayerHpChanged>(ITF_GET_STRINGID_CRC(EventPlayerHpChanged,1555193400)) )
    {
        processPlayerHpChanged(eventHP);
    }
    else if ( EventRevertToLastCheckpoint* eventRevert = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)) )
    {
        processRevertToLastCheckpoint(eventRevert);
    }
	else if ( Ray_EventChangePage* eventChangePage = _event->DynamicCast<Ray_EventChangePage>(ITF_GET_STRINGID_CRC(Ray_EventChangePage,1004490573)) )
	{
		processChangePage(eventChangePage);
	}
    else if ( EventPlayerModeChanged* eventGameMode = _event->DynamicCast<EventPlayerModeChanged>(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275)) )
    {
        processPlayerModeChanged(eventGameMode);
    }
    else if ( EventPlayerModeSet* eventGameModeSet = _event->DynamicCast<EventPlayerModeSet>(ITF_GET_STRINGID_CRC(EventPlayerModeSet,722903700)) )
    {
        processPlayerModeSet(eventGameModeSet);
    }
    else if ( Ray_EventQueryCanBeAttacked* query = _event->DynamicCast<Ray_EventQueryCanBeAttacked>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanBeAttacked,3134229644)) )
    {
        query->setCanBeAttacked( !isImmune() );
    }
    else if ( Ray_EventHangCoopJump* jumpEvent = _event->DynamicCast<Ray_EventHangCoopJump>(ITF_GET_STRINGID_CRC(Ray_EventHangCoopJump,930438087)) )
    {
        if ( m_currentState != &m_stateHanging )
        {
            changeState(&m_stateHanging);
        }
    }
    else if ( Ray_EventQueryStanceInfo* queryStance = _event->DynamicCast<Ray_EventQueryStanceInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryStanceInfo,1627894922)) )
    {
        queryStance->setStance(m_stance);
        queryStance->setOrientationPose(m_orientationPose);
    }
    else if (Ray_EventQueryDarktoonificationPosition* onQueryDktnPos = _event->DynamicCast<Ray_EventQueryDarktoonificationPosition>(ITF_GET_STRINGID_CRC(Ray_EventQueryDarktoonificationPosition,785050175)))
    {
        Vec3d pos = m_actor->getPos();
        Vec2d offset = Vec2d::Up.Rotate(m_actor->getAngle()) * getTemplate()->getDarktoonificationPosOffset();
        pos += offset.to3d();
        onQueryDktnPos->setPos(pos);
    }
    else if (Ray_EventQueryCanDarktoonify* onQueryCanDarktoonify = _event->DynamicCast<Ray_EventQueryCanDarktoonify>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanDarktoonify,1194771444)))
    {
        onQueryCanDarktoonify->setCanDarktoonify(bfalse);
    }
    else if (EventQueryBlocksHits* queryBlocksHits = _event->DynamicCast<EventQueryBlocksHits>(ITF_GET_STRINGID_CRC(EventQueryBlocksHits,133722380)))
    {
        queryBlocksHits->setCanBlockHits(bfalse);
    }
    else if (EventQueryCanStickOrCollide * onQueryCanStick = _event->DynamicCast<EventQueryCanStickOrCollide>(ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404)))
    {
        onQueryCanStick->setCanStick(bfalse);
        onQueryCanStick->setCanCollide(bfalse);

        ObjectRef actorThatWantsToStickRef = onQueryCanStick->getSender();
        Actor* actor = AIUtils::getActor(actorThatWantsToStickRef);

        if ( actor )
        {
            // Using an event here could be a bit overkill, and anyway this code is going to disappear when the pedestal stops being a polyline
            StickToPolylinePhysComponent* physComponent = actor->GetComponent<StickToPolylinePhysComponent>();

            if ( physComponent && physComponent->getStickedEdgeIndex() == U32_INVALID )
            {
                InteractionMask interactions = FACTION_MANAGER->getInteractions(getFaction(), AIUtils::getFaction(actor));

                if ( (interactions&INTERACTION_ALLY) != 0 )
                {
                    onQueryCanStick->setCanStick(btrue);
                    onQueryCanStick->setCanCollide(btrue);
                }
            }
        }
    }
    else if ( Ray_EventTriggerVictory* victoryEvent = _event->DynamicCast<Ray_EventTriggerVictory>(ITF_GET_STRINGID_CRC(Ray_EventTriggerVictory,1674293021)) )
    {
        processTriggerVictory(victoryEvent);
    }
    else if ( Ray_EventMoveToPoint* moveToPoint = _event->DynamicCast<Ray_EventMoveToPoint>(ITF_GET_STRINGID_CRC(Ray_EventMoveToPoint,1570511527)) )
    {
        setMoveToPoint(moveToPoint->getPos());
    }
    else if ( Ray_EventResetMoveToPoint* resetMoveEvent = _event->DynamicCast<Ray_EventResetMoveToPoint>(ITF_GET_STRINGID_CRC(Ray_EventResetMoveToPoint,918485638)) )
    {
        resetMoveToPoint();
    }
    else if ( Ray_EventQueryReachedMoveToPoint* queryReachedPoint = _event->DynamicCast<Ray_EventQueryReachedMoveToPoint>(ITF_GET_STRINGID_CRC(Ray_EventQueryReachedMoveToPoint,1707458262)) )
    {
        queryReachedPoint->setHasReached(m_moveToPointFinished);
    }
    else if ( EventReset* resetEvent = _event->DynamicCast<EventReset>(ITF_GET_STRINGID_CRC(EventReset,2551104629)) )
    {
        processReset();
    }
    else if ( EventStopPlayer* stopEvent = _event->DynamicCast<EventStopPlayer>(ITF_GET_STRINGID_CRC(EventStopPlayer,3369505273)) )
    {
        disablePlayer(stopEvent->isStop());
    }
    else if ( EventQueryIsDisabled* query = _event->DynamicCast<EventQueryIsDisabled>(ITF_GET_STRINGID_CRC(EventQueryIsDisabled,1476509545)) )
    {
        query->setIsDisabled(m_playerDisabled);
    }
    else if ( EventQueryIsCaught* query = _event->DynamicCast<EventQueryIsCaught>(ITF_GET_STRINGID_CRC(EventQueryIsCaught,1045544516)) )
    {
        query->setIsCaught( m_currentState == &m_stateCaught );
    }
    else if ( EventSequenceActorPrepare* sequenceEventPrepare = _event->DynamicCast<EventSequenceActorPrepare>(ITF_GET_STRINGID_CRC(EventSequenceActorPrepare,3178520967)) )
    {
        processSequenceActorPrepare(sequenceEventPrepare);
    }
    else if ( EventSequenceActorActivate* sequenceEvent = _event->DynamicCast<EventSequenceActorActivate>(ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699)) )
    {
        processSequenceActorActivate(sequenceEvent);
    }
    else if ( EventSequenceActorReady* sequenceEventReady = _event->DynamicCast<EventSequenceActorReady>(ITF_GET_STRINGID_CRC(EventSequenceActorReady,1601154857)) )
    {
        processSequenceActorReady(sequenceEventReady);
    }
    else if ( EventScaleChanged* scaleChanged = _event->DynamicCast<EventScaleChanged>(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)) )
    {
        processScaleChanged();
    }
    else if ( Ray_EventBounceToLayer* bounceToLayer = _event->DynamicCast<Ray_EventBounceToLayer>(ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527)) )
    {
        processBounceToLayer(bounceToLayer);
    }
    else if ( Ray_EventJumpToPos* jumpToPos = _event->DynamicCast<Ray_EventJumpToPos>(ITF_GET_STRINGID_CRC(Ray_EventJumpToPos,981487617)) )
    {
        processJumpToPos(jumpToPos);
    }
    else if (Ray_EventQueryAIData * aiQuery = _event->DynamicCast<Ray_EventQueryAIData>(ITF_GET_STRINGID_CRC(Ray_EventQueryAIData,2473202000)))
    {
        processQueryAIData(aiQuery);
    }
    else if (Ray_EventInstantKill * instantKill = _event->DynamicCast<Ray_EventInstantKill>(ITF_GET_STRINGID_CRC(Ray_EventInstantKill,2400145155)))
    {
        m_instantKill = btrue;
    }
    else if (Ray_EventHandsCaught * handsCaught = _event->DynamicCast<Ray_EventHandsCaught>(ITF_GET_STRINGID_CRC(Ray_EventHandsCaught,3210089466)))
    {
        m_handsCaught = btrue;
        m_handsRef = handsCaught->getSender();
    }
    else if (Ray_EventHandsRelease * handsReleased = _event->DynamicCast<Ray_EventHandsRelease>(ITF_GET_STRINGID_CRC(Ray_EventHandsRelease,579391720)))
    {
        m_handsReleased = btrue;
        m_handsRef = handsReleased->getSender();
    }
    else if (Ray_EventCaughtInPipe* eventPipe = _event->DynamicCast<Ray_EventCaughtInPipe>(ITF_GET_STRINGID_CRC(Ray_EventCaughtInPipe,1030315255)))
    {
        if (eventPipe->getCatch())
        {
            eventPipe->setAcceptCatch(m_currentState != &m_stateCaughtInPipe);
            m_caughtInPipe = eventPipe->getAcceptCatch();
            m_pipeRef = eventPipe->getSender();
        }
    }
    else if (Ray_EventTriggerEnterDoor* triggerEnterDoor = _event->DynamicCast<Ray_EventTriggerEnterDoor>(ITF_GET_STRINGID_CRC(Ray_EventTriggerEnterDoor,849805494)))
    {
        m_enterDoorEvent = *triggerEnterDoor;
        m_enterDoorReceived = btrue;
    }
    else if (Ray_EventTriggerStargate* stargateEvent = _event->DynamicCast<Ray_EventTriggerStargate>(ITF_GET_STRINGID_CRC(Ray_EventTriggerStargate,3125962121)))
    {
        m_stargateEvent = *stargateEvent;
        m_stargateReceived = btrue;
    }
    else if (Ray_EventTriggerChangePage* triggerChangePage = _event->DynamicCast<Ray_EventTriggerChangePage>(ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481)))
    {
        m_changePageEvent = *triggerChangePage;
        m_changePageReceived = btrue;
    }
    else if (Ray_EventScaleDoorTrigger* scaleDoorTrigger = _event->DynamicCast<Ray_EventScaleDoorTrigger>(ITF_GET_STRINGID_CRC(Ray_EventScaleDoorTrigger,3012042865)))
    {
        m_scaleDoorTriggerEvent = *scaleDoorTrigger;
        m_scaleDoorReceived = btrue;
    }
    else if (Ray_EventScaleTunnelTrigger* scaleTunnelTrigger = _event->DynamicCast<Ray_EventScaleTunnelTrigger>(ITF_GET_STRINGID_CRC(Ray_EventScaleTunnelTrigger,2369480741)))
    {
        m_scaleTunnelScale = scaleTunnelTrigger->getScale();
        m_scaleTunnelT = scaleTunnelTrigger->getT();
        m_scaleTunnelReceived = btrue;
    }
    else if (Ray_EventQueryIsInPedestal* isInPedestal = _event->DynamicCast<Ray_EventQueryIsInPedestal>(ITF_GET_STRINGID_CRC(Ray_EventQueryIsInPedestal,2146089474)))
    {
        isInPedestal->setIsInPedestal(isSupporting());
    }
    else if (EventQueryIsSprinting* isInSprint = _event->DynamicCast<EventQueryIsSprinting>(ITF_GET_STRINGID_CRC(EventQueryIsSprinting,2326390223)))
    {
        isInSprint->setIsSprinting(isSprinting());
    }
    else if (Ray_EventStargateUse* stargateUseEvent = _event->DynamicCast<Ray_EventStargateUse>(ITF_GET_STRINGID_CRC(Ray_EventStargateUse,3921841867)))
    {
        processStargateUse(stargateUseEvent);
    }
    else if (Ray_EventAddSoftCollForce * onAddForce = _event->DynamicCast<Ray_EventAddSoftCollForce>(ITF_GET_STRINGID_CRC(Ray_EventAddSoftCollForce,2624804693)))
    {
        if (!m_softCollisionDisabled)
        {
            m_characterPhys->addForce(onAddForce->getForce());
        }
    }
    else if (Ray_EventCheckPedestalSupport * pedestalSupportCheck = _event->DynamicCast<Ray_EventCheckPedestalSupport>(ITF_GET_STRINGID_CRC(Ray_EventCheckPedestalSupport,1578355683)))
    {
        processCheckPedestalSupport(pedestalSupportCheck);
    }
    else if (Ray_EventQueryPedestalInfo * pedestalQueryInfo = _event->DynamicCast<Ray_EventQueryPedestalInfo>(ITF_GET_STRINGID_CRC(Ray_EventQueryPedestalInfo,2107418028)))
    {
        processQueryPedestalInfo(pedestalQueryInfo);
    }
    else if (Ray_EventReleasePedestal * pedestalRelease = _event->DynamicCast<Ray_EventReleasePedestal>(ITF_GET_STRINGID_CRC(Ray_EventReleasePedestal,4284763324)))
    {
        processReleasePedestal(pedestalRelease);
    }
    else if (Ray_EventQueryPaintTrail * paintTrailQuery= _event->DynamicCast<Ray_EventQueryPaintTrail>(ITF_GET_STRINGID_CRC(Ray_EventQueryPaintTrail,3721333601)))
    {
        bbool paint = m_currentState == &m_stateIdle ||
            m_currentState == &m_stateJumping ||
            m_currentState == &m_stateHelicopter;
        paintTrailQuery->setPaint(paint);
    }
    else if (Ray_EventScaleFunnelTrigger * scaleFunnelTrigger = _event->DynamicCast<Ray_EventScaleFunnelTrigger>(ITF_GET_STRINGID_CRC(Ray_EventScaleFunnelTrigger,2532379169)))
    {
        processScaleFunnelTrigger(scaleFunnelTrigger);
    }
    else if (Ray_EventAINetworkWaypoint* aiNetworkWaypoint = _event->DynamicCast<Ray_EventAINetworkWaypoint>(ITF_GET_STRINGID_CRC(Ray_EventAINetworkWaypoint,3032038608)))
    {
        processAINetworkWaypointEvent(aiNetworkWaypoint);
    }
    else if ( Ray_EventSnapDance* snapDance = _event->DynamicCast<Ray_EventSnapDance>(ITF_GET_STRINGID_CRC(Ray_EventSnapDance,806811309)) )
    {
        if ( snapDance->getSnap() )
        {
            m_snapDanceReceived = btrue;
        }
    }
    else if ( Ray_EventReturnTeeth* throwTeethEvent = _event->DynamicCast<Ray_EventReturnTeeth>(ITF_GET_STRINGID_CRC(Ray_EventReturnTeeth,810560716)) )
    {
        m_returnTeeth = *throwTeethEvent;
        m_throwTeethReceived = btrue;
    }
    else if ( Ray_EventPlayAnimState* playAnimEvent = _event->DynamicCast<Ray_EventPlayAnimState>(ITF_GET_STRINGID_CRC(Ray_EventPlayAnimState,4089549703)) )
    {
        m_playAnimEvent = *playAnimEvent;
        m_playAnimReceived = btrue;
    }
    else if ( Ray_MoveOnPolylineFromNautilus* moveOnPolyFromNautilus = _event->DynamicCast<Ray_MoveOnPolylineFromNautilus>(ITF_GET_STRINGID_CRC(Ray_MoveOnPolylineFromNautilus,1238635980)) )
    {
        if ( !isWallSliding() )
        {
            m_characterPhys->advanceDistance(moveOnPolyFromNautilus->getDistance());
        }
    }
    else if ( Ray_EventTriggerStartTimeAttack* startTimeAttackEvent = _event->DynamicCast<Ray_EventTriggerStartTimeAttack>(ITF_GET_STRINGID_CRC(Ray_EventTriggerStartTimeAttack,3727680095)) )
    {
        if ( startTimeAttackEvent->getStart() )
        {
            EventStopPlayer stopPlayerEvent(btrue);
            EventTrail showTrail(btrue);
            EventShow alphaChange( 0.0f, 0.25f );
            EventDetach detachEvent;

            m_actor->onEvent(&detachEvent);
            m_actor->onEvent(&stopPlayerEvent);
            m_actor->onEvent(&showTrail);
            m_actor->onEvent(&alphaChange);
        }
        else
        {
            EventStopPlayer stopPlayerEvent(bfalse);
            EventTrail showTrail(bfalse);
            EventShow alphaChange( 1.0f, 0.f );

            m_actor->onEvent(&stopPlayerEvent);
            m_actor->onEvent(&showTrail);
            m_actor->onEvent(&alphaChange);
        }

        m_fxController->playFX(ITF_GET_STRINGID_CRC(MRK_Vortex_Voyage_End,3558498901));
    }

    m_currentState->onEvent(_event);

    // forward event to power-up displays
    for (u32 i=0; i<m_powerUpDisplays.size(); ++i)
    {
        m_powerUpDisplays[i]->onEvent(_event);
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::processScaleChanged()
{
    f32 scaleY = m_actor->getScale().m_y;

    m_hangingCollisionShapeSolid.setExtent(getTemplate()->getHangCollisionExtentSolid()*scaleY);
    m_hangingCollisionShapeCoop.setExtent(getTemplate()->getHangCollisionExtentCoop()*scaleY);
    m_climbingCollisionCheckShape.setExtent(getTemplate()->getClimbingCollisionCheckExtent()*scaleY);

    setPhantomShape();
    resetStandPhantomShapeScaled();

    if ( m_playerIndex != U32_INVALID )
    {
    f32 scale = scaleY/m_actor->getWorldInitialScale().m_y;
    RAY_GAMEMANAGER->setPlayersCurrentScale(scale);
    }
}

void Ray_PlayerControllerComponent::processBounceToLayer( Ray_EventBounceToLayer* _bounce )
{
    if ( isDead() ||
#ifdef ITF_SUPPORT_CHEAT
         m_currentState == &m_stateFlyMode ||
#endif
         m_playerDisabled || RAY_GAMEMANAGER->isPlayingBounceToLayerSequence() )
    {
        _bounce->setSkipped(btrue);
        return;
    }

    if ( _bounce->getHurt() )
    {
        m_receivedHitInstigator = _bounce->getSender();
        m_receivedHitLevel = _bounce->getHurt();
        m_receivedHitDamage = m_receivedHitLevel+1;
        m_receivedHitType = RECEIVEDHITTYPE_HURTBOUNCE;
        m_receivedHitFXPos = m_actor->getPos();
    }

    m_stateBounceToLayer.setData(_bounce->getTarget(),_bounce->getOffset(),_bounce->getHurt());
    m_bounceToLayer = btrue;
}

void Ray_PlayerControllerComponent::processJumpToPos( Ray_EventJumpToPos* _event )
{
    if ( isDead() ||
#ifdef ITF_SUPPORT_CHEAT
         m_currentState == &m_stateFlyMode ||
#endif
         m_playerDisabled ||
         RAY_GAMEMANAGER->isPlayingBounceToLayerSequence() )
    {
        return;
    }

    m_jumpToPos = *_event;
    m_jumpToPosReceived = btrue;
}

void Ray_PlayerControllerComponent::getCurrentBounceData( f32& _height, f32& _airControlDelay) const
{
    _height = 0.f;
    _airControlDelay = 0.f;

    switch(m_bounceCurrentType)
    {
        case BOUNCETYPE_ENEMY:
            switch(m_bounceCurrentReactType)
            {
                case BOUNCEREACTTYPE_AUTO:
                    _height = getTemplate()->getBounceHeightEnemyLevel1();
                    _airControlDelay = getTemplate()->getBounceEnemyAirControlDelayLevel1();
                    break;
                case BOUNCEREACTTYPE_SYNCHRO:
                    _height = getTemplate()->getBounceHeightEnemyLevel2();
                    _airControlDelay = getTemplate()->getBounceEnemyAirControlDelayLevel2();
                    break;
                case BOUNCEREACTTYPE_CRUSH:
                    _height = getTemplate()->getBounceHeightEnemyLevel3();
                    _airControlDelay = getTemplate()->getBounceEnemyAirControlDelayLevel3();
                    break;
            }
            break;
        case BOUNCETYPE_TALKINGHAT:
            switch(m_bounceCurrentReactType)
            {
            case BOUNCEREACTTYPE_AUTO:
                _height = getTemplate()->getBounceHeightTalkingHatLevel1();
                _airControlDelay = getTemplate()->getBounceTalkingHatAirControlDelayLevel1();
                break;
            case BOUNCEREACTTYPE_SYNCHRO:
                _height = getTemplate()->getBounceHeightTalkingHatLevel2();
                _airControlDelay = getTemplate()->getBounceTalkingHatAirControlDelayLevel2();
                break;
            case BOUNCEREACTTYPE_CRUSH:
                _height = getTemplate()->getBounceHeightTalkingHatLevel3();
                _airControlDelay = getTemplate()->getBounceTalkingHatAirControlDelayLevel3();
                break;
            }
            break;
            break;
        case BOUNCETYPE_BUMPER:
            switch(m_bounceCurrentReactType)
            {
                case BOUNCEREACTTYPE_AUTO:
                    _height = getTemplate()->getBounceHeightPlatformLevel1();
                    _airControlDelay = getTemplate()->getBouncePlatformAirControlDelayLevel1();
                    break;
                case BOUNCEREACTTYPE_SYNCHRO:
                    _height = getTemplate()->getBounceHeightPlatformLevel2();
                    _airControlDelay = getTemplate()->getBouncePlatformAirControlDelayLevel2();
                    break;
                case BOUNCEREACTTYPE_CRUSH:
                    _height = getTemplate()->getBounceHeightPlatformLevel3();
                    _airControlDelay = getTemplate()->getBouncePlatformAirControlDelayLevel3();
                    break;
            }
            break;
        case BOUNCETYPE_POLYLINE:
            switch(m_bounceCurrentReactType)
            {
            case BOUNCEREACTTYPE_AUTO:
                _height = getTemplate()->getBounceHeightPolylineLevel1();
                _airControlDelay = 0.f;
                break;
            case BOUNCEREACTTYPE_SYNCHRO:
                _height = getTemplate()->getBounceHeightPolylineLevel2();
                _airControlDelay = 0.f;
                break;
            case BOUNCEREACTTYPE_CRUSH:
                _height = getTemplate()->getBounceHeightPolylineLevel3();
                break;
            }
            _airControlDelay = 0.f;
            break;
        case BOUNCETYPE_WAVE:
            switch(m_bounceCurrentReactType)
            {
            case BOUNCEREACTTYPE_CRUSH:
                _height = getTemplate()->getBounceHeightWaveLevel3();
                break;
            default:
                _height = getTemplate()->getBounceHeightWave();
                break;
            }
            _airControlDelay = 0.f;
            break;
        default:
            ITF_ASSERT_MSG(0,"Trying to bounce with no bounce type");
            break;
    }
}

void Ray_PlayerControllerComponent::performBounce()
{
    if ( m_bounceCurrentType == BOUNCETYPE_WINDTUNEL )
    {
        performBounceWindTunnel();
    }
    else
    {
        performBounceJump();
    }

    m_hitDisabledTimer = getTemplate()->getHitDisableOnBounce();
    m_bounceMultiplier = 1.f;
    m_bounceCurrentDir = Vec2d::Zero;
    resetStance(m_stance);
}

void Ray_PlayerControllerComponent::performBounceWindTunnel()
{
    Actor* bounceActor = m_bounceSender.getActor();

    if ( !bounceActor )
    {
        return;
    }

    f32 exitSpeed;
    Ray_EventQueryWindTunnelInfo queryInfo;

    bounceActor->onEvent(&queryInfo);

    switch(queryInfo.getLevel())
    {
        case 0:
            exitSpeed = getTemplate()->getWindTunnelExitSpeedLevel0();
            break;
        case 1:
            exitSpeed = getTemplate()->getWindTunnelExitSpeedLevel1();
            break;
        default:
            exitSpeed = getTemplate()->getWindTunnelExitSpeedLevel2();
            break;
    }

    Vec2d speedAtExit = m_bounceCurrentDir*exitSpeed;
    Vec2d newSpeed;

    f32 airControlDelay;
    f32 tunnelSize = ( queryInfo.getExitPoint() - queryInfo.getStartPoint() ).norm();
    Vec2d vecToPos = m_actor->get2DPos() - queryInfo.getStartPoint();
    f32 distInTunnel = m_bounceCurrentDir.dot(vecToPos);
    Vec2d currentPosInTunnel = queryInfo.getStartPoint() + ( m_bounceCurrentDir * distInTunnel );

    if ( distInTunnel >= tunnelSize )
    {
        // we are already higher than the exit point, so just put the final speed
        newSpeed = speedAtExit;
        airControlDelay = 0.f;
    }
    else
    {
        // we need to fight gravity to get to the right speed at the end
        // vf^2 = vi^2 + ( 2 * a * d )
        // vi^2 = vf^2 - ( 2 * a * d )
        // vi = sqrtf( vf^2 - ( 2 * a * d ) )
        // a = ( vf^2 - vi^2 ) / ( 2 * d )
        f32 dist = queryInfo.getExitPoint().m_y - currentPosInTunnel.m_y;
        f32 time;

        f32 accel = m_currentGravity.m_y;

        if ( fabs(dist) > MTH_EPSILON )
        {
            f32 val = ( speedAtExit.m_y * speedAtExit.m_y ) - ( 2.f * accel * fabsf(dist) );
            f32 v0 = 0.f;
            bbool changeAccel = bfalse;

            if ( val > 0.f )
            {
                v0 = sqrtf( val );
            }
            else
            {
                changeAccel = btrue;
            }

            if ( ( v0 >= 0.f ) != ( dist >= 0.f ) )
            {
                changeAccel = btrue;
            }

            if ( changeAccel )
            {
                v0 = 0.f;
                accel = (speedAtExit.m_y*speedAtExit.m_y) / ( 2 * fabsf(dist) );

                f32 gravityMultiplier = accel / fabsf(m_currentGravity.m_y);

                m_characterPhys->setGravityMultiplier(gravityMultiplier);

                time = fabsf( speedAtExit.m_y / accel );
            }
            else
            {
                time = fabsf(( speedAtExit.m_y - v0 ) / accel);
            }

            newSpeed.m_y = v0;
            airControlDelay = time;

            dist = queryInfo.getExitPoint().m_x - currentPosInTunnel.m_x;
            newSpeed.m_x = dist/time;
        }
        else
        {
            m_characterPhys->setGravityMultiplier(0.f);
            newSpeed.m_x = speedAtExit.m_x;
            newSpeed.m_y = 0.f;
            dist = queryInfo.getExitPoint().m_x - currentPosInTunnel.m_x;
            time = dist / newSpeed.m_x;
            airControlDelay = time;
            accel = 0.f;
        }

        // Calculate where we reposition our character
        // constant movement
        f32 dX = newSpeed.m_x * time;
        // accelerated movement
        f32 dY = (newSpeed.m_y*time) + ( 0.5f * accel * (time*time) );

        // actor target distance
        f32 posAtEndX = m_actor->getPos().m_x + dX;
        f32 posAtEndY = m_actor->getPos().m_y + dY;
        // bumper target distance
        f32 desiredPosX = currentPosInTunnel.m_x + dX;
        f32 desiredPosY = currentPosInTunnel.m_y + dY;

        m_bounceRepositionTotal = Vec2d(desiredPosX-posAtEndX,desiredPosY-posAtEndY);
        m_bounceRepositionFinalSpeed = speedAtExit;
        m_bounceRepositionTotalTime = time;

        if ( m_bounceRepositionTotal == Vec2d::Zero )
        {
            m_bounceRepositionCounter = time;
        }
        else
        {
            m_bounceRepositionCounter = 0.f;
        }

        setMaxSpeedLimit(bfalse);
    }

    m_characterPhys->setSpeed(newSpeed);
    m_airControlDisableTotal = m_airControlDisableDelay = airControlDelay;
    m_airControlDisableBlend = bfalse;
    m_airControlDisableAmount = 0.f;
    m_airControlDisableAllowHelico = bfalse;
}

void Ray_PlayerControllerComponent::performBounceJump()
{
    Vec2d newSpeed;

    if ( m_bounceCurrentType == BOUNCETYPE_ENEMY )
    {
        Vec2d gravityNorm = m_currentGravityDir.getPerpendicular();
        f32 xSpeed = m_characterPhys->getSpeed().dot(gravityNorm);

        newSpeed = gravityNorm * xSpeed * getTemplate()->getBounceEnemyXSpeedMultiplier();
    }
    else if ( m_bounceCurrentType == BOUNCETYPE_TALKINGHAT )
    {
        Vec2d gravityNorm = m_currentGravityDir.getPerpendicular();
        f32 xSpeed = m_bounceSpeedAtTime.dot(gravityNorm);
        newSpeed = gravityNorm * xSpeed * getTemplate()->getBounceEnemyXSpeedMultiplier();
    }
    else if ( m_bounceCurrentType == BOUNCETYPE_POLYLINE )
    {
        Vec2d bounceNorm = m_bounceCurrentDir.getPerpendicular();
        f32 xSpeed = m_characterPhys->getSpeed().dot(bounceNorm);
        f32 xSign = getSign(xSpeed);

        xSpeed = Min(f32_Abs(xSpeed),getTemplate()->getBouncePolylineMaxXSpeed());

        newSpeed = bounceNorm * xSpeed * xSign;
    }
    else
    {
        newSpeed = Vec2d::Zero;
    }

    if (m_bounceCurrentGameMaterial.isValid())
    {
        const GameMaterial_Template* gameMaterial = World::getGameMaterial(m_bounceCurrentGameMaterial);
        if (gameMaterial)
            startBounceOnLineFx(gameMaterial);
    }

    m_characterPhys->setSpeed(newSpeed);

    f32 targetHeight, airControlDelay;

    getCurrentBounceData(targetHeight,airControlDelay);

    f32 initialSpeed = f32_Sqrt(m_currentGravity.norm()*2.f*targetHeight);
    f32 initialForce = initialSpeed/LOGICDT;

    if ( m_bounceCurrentType == BOUNCETYPE_BUMPER )
    {
        f32 xDir = m_bounceCurrentDir.dot(m_currentGravityDir.getPerpendicular());

        if ( f32_Abs(xDir) > MTH_EPSILON )
        {
            bbool lookRight = xDir > 0.f;

            if ( lookRight != !m_actor->isFlipped() )
            {
                startUTurn(UTURNTYPE_AIR);
            }
        }
    }

    Vec2d force = m_bounceCurrentDir*initialForce*m_bounceMultiplier;

    m_characterPhys->addForce(force);
    m_animComponent->resetTree();

    m_airControlDisableTotal = m_airControlDisableDelay = airControlDelay;
    m_airControlDisableBlend = bfalse;
    m_airControlDisableAmount = 0.f;
    m_airControlDisableAllowHelico = bfalse;

    if ( m_bounceCurrentDangerLevel == 0 && m_bounceCurrentOrigin != Vec2d::Zero )
    {
        // For bumpers, we reposition our character in the middle of the bumper
        Vec2d currentSpeed = force*LOGICDT;
        f32 repositionDuration = getTemplate()->getBounceRepositionDuration();

        // constant movement
        f32 dX = currentSpeed.m_x * repositionDuration;
        // accelerated movement
        f32 dY = (currentSpeed.m_y*repositionDuration) + ( 0.5f * m_currentGravity.m_y * (repositionDuration*repositionDuration) );

        // actor target distance
        f32 posAtEndX = m_actor->getPos().m_x + dX;
        f32 posAtEndY = m_actor->getPos().m_y + dY;
        // bumper target distance
        f32 desiredPosX = m_bounceCurrentOrigin.m_x + dX;
        f32 desiredPosY = m_bounceCurrentOrigin.m_y + dY;

        m_bounceRepositionTotal = Vec2d(desiredPosX-posAtEndX,desiredPosY-posAtEndY);
        m_bounceRepositionTotalTime = repositionDuration;
    }

    CAMERACONTROLLERMANAGER->setSubjectJustBounceJumped( m_actor->getRef());
}

void Ray_PlayerControllerComponent::resetStandPhantomShapeScaled()
{
    if ( getTemplate()->getPhantomShape() )
    {
        if ( !m_phantomShapeScaled || m_phantomShapeScaled->GetObjectClassCRC() != getTemplate()->getPhantomShape()->GetObjectClassCRC() )
        {
            SF_DEL(m_phantomShapeScaled);
            m_phantomShapeScaled = PHYSWORLD->getShapesFactory()->CreateObject<PhysShape>(getTemplate()->getPhantomShape()->GetObjectClassCRC());
        }

        PhysShape::CopyShapeScaled(getTemplate()->getPhantomShape(),m_phantomShapeScaled,m_actor->getScale());
    }
    else
    {
        SF_DEL(m_phantomShapeScaled);
    }
}

void Ray_PlayerControllerComponent::setPhantomShape()
{
    if ( m_stance == STANCE_SWIM )
    {
        setCurrentPhantomShape(getTemplate()->getPhantomShapeSwim());
    }
    else if ( isCrouch() || isLowRoof() )
    {
        setCurrentPhantomShape(getTemplate()->getPhantomShapeCrouch());
    }
    else
    {
        setCurrentPhantomShape(getTemplate()->getPhantomShape());
    }
}

void Ray_PlayerControllerComponent::processBlockedByPolylineEvent( EventBlockedByPolyline* _blockedEvent )
{
    if ( m_bounceCurrentDir == Vec2d::Zero && m_bouncePrevFramePoly != _blockedEvent->getPolyline() )
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(_blockedEvent->getPolyline(),_blockedEvent->getEdge(),poly,edge);

        if ( poly && edge )
        {
            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

            if ( mat && mat->getIsBounce() )
            {
                setBounceEdgeCheck(edge->m_normalizedVector,
                                   mat->getDangerousLevel(),
                                   _blockedEvent->getPolyline(),
                                   _blockedEvent->getEdge(),
                                   _blockedEvent->getSpeed(),
                                   edge->getGameMaterial());
            }
            else
            {
                if ( m_currentState == &m_stateJumping ||
                     m_currentState == &m_stateFalling )
                {
                    m_wallSlideTestSpeed = m_characterPhys->getSpeed();
                }
            }
        }
    }

    if ( m_characterPhys->getStickOnWalls() && m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();

        if ( currentEdge && m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
        {
            setSprinting(bfalse);
            m_characterPhys->setStickOnWalls(bfalse);
            m_walkOnWallsDisableTimer = 0.f;
            m_characterPhys->unstick();
        }
    }
}

void Ray_PlayerControllerComponent::changeState( PlayerControllerState* _newState)
{
    Super::changeState(_newState);

    m_idleTimer = 0.f;
}

void Ray_PlayerControllerComponent::processEventPolylineUpdate( EventStickOnPolylineUpdate* _event )
{
    if ( _event->getPolyline() == ITF_INVALID_OBJREF )
    {
        if ( m_currentState != &m_stateJumping || !m_currentJumpWallRun )
        {
            m_characterPhys->setStickOnWalls(bfalse);
            m_walkOnWallsDisableTimer = 0.f;
        }
    }
    else
    {
        if ( m_bouncePrevFramePoly != _event->getPolyline() )
        {
            PolyLine* poly = NULL;
            const PolyLineEdge* edge = NULL;

            AIUtils::getPolyLine(_event->getPolyline(),_event->getEdge(),poly,edge);

            if ( edge )
            {
                const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

                if ( mat && mat->getIsBounce() )
                {
                    setBounceEdgeCheck(edge->m_normalizedVector,
                                       mat->getDangerousLevel(),
                                       _event->getPolyline(),
                                       _event->getEdge(),
                                       _event->getSpeed(),
                                       edge->getGameMaterial());
                }
                else
                {
                    if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID )
                    {
                        // We are not sticked, but we just sticked into a ground.
                        // In case we unstick in the same physics iteration, we prepare the jump timer so that we can jump
                        // We also reset the stance, because we actually went from air, stick, no stick, in one frame
                        m_jumpOnAirTimer = getTemplate()->getJumpOnAirTime();
                        resetStance(m_stance);
                    }
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::processCrush( EventCrushed* _crushed )
{
    if ( m_currentState == &m_stateDeadSoul )
    {
        m_stateDeadSoul.processCrush(_crushed);
        _crushed->setBounce(bfalse);
    }
    else if ( m_currentState != &m_stateCrushedByCharacter )
    {
        changeState(&m_stateCrushedByCharacter);
    }
    else
    {
        m_stateCrushedByCharacter.restart();
    }
}

void Ray_PlayerControllerComponent::receivePunch( const PunchStim* _punch )
{
    if ( _punch->getSender() == m_actor->getRef() || m_playerDisabled || m_inputDisabled || m_moveToPoint )
    {
        return;
    }

    bbool immune = isImmune();

    if ( isDead() ||
         m_currentState == &m_stateBounceToLayer ||
#ifdef ITF_SUPPORT_CHEAT
         m_currentState == &m_stateFlyMode ||
#endif
       ( immune && m_inmunityCounterReceiveHit ) )
    {
        return;
    }

#ifdef ITF_SUPPORT_ONLINETRACKING

	if (CONFIG->m_enableonlineTracking)
	{

		if (RAY_GAMEMANAGER->isPlayerActor(_punch->getSender()))
		{
			RAY_GAMEMANAGER->getOnlineTrackingManager()->m_lastPaf = 1;
		}
		else
		{
			RAY_GAMEMANAGER->getOnlineTrackingManager()->m_lastPaf = 0;
		}


	}


#endif

    // CASA::CTR::SFS : Add For Arena
    EventHurtNotification eventHurtNotification(_punch->getSender(), m_actor->getRef(), _punch->getLevel());
    EVENTMANAGER->broadcastEvent(&eventHurtNotification);
    // CASA>

    // do we accept hits from the sender's faction? if so, do we receive damage?
    bbool receiveDamage = bfalse;

    if ( !AIUtils::isEnemyHit(_punch, getFaction(), &receiveDamage) )
    {
        return;
    }

    if ( immune )
    {
        receiveDamage = bfalse;
    }

    if ( receiveDamage && m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
    {
        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

        player->setWasHitInLevel(btrue);
    }

    setLastHitNature(HITNATURE_ENEMY);
    m_receivedHitInstigator = _punch->getSender();
    m_receivedHitLevel = _punch->getLevel();
    m_receivedHitDamage = receiveDamage ? m_receivedHitLevel+1 : 0;
    m_receivedHitDir = _punch->getDirection();
    m_receivedHitType = _punch->getReceivedHitType();
    m_receivedHitPosition = _punch->getPos();

    if ( _punch->getLevel() >= 100 )
    {
        m_instantKill = btrue;
        return;
    }

    const PhysContactsContainer& contacts = _punch->getContacts();

    u32 numContacts = contacts.size();

    if (numContacts > 0)
    {
        f32 fnumContacts = 0;
        Vec2d averagePos = Vec2d::Zero;

        for (u32 i = 0; i < numContacts; ++i)
        {
            averagePos += contacts[i].m_contactPoint;
            fnumContacts +=1.0f;
        }

        averagePos = averagePos / fnumContacts;

        f32 depth = m_actor->getDepth();

        m_receivedHitFXPos = averagePos.to3d(depth);
    }
    else
    {
        m_receivedHitFXPos = _punch->getFxPos();
    }

    if ( m_receivedHitType == RECEIVEDHITTYPE_HURTBOUNCE )
    {
        Vec2d bounceDir;

        if ( _punch->getIsRadial() )
        {
            bounceDir = m_actor->get2DPos() - _punch->getPos();
            bounceDir.normalize();
        }
        else
        {
            bounceDir = _punch->getDirection();
        }

        resetBounce();

        m_bounceSender = _punch->getSender();
        m_bounceCurrentDir = bounceDir;
        m_bounceSpeedAtTime = m_characterPhys->getSpeed();
        m_bounceCurrentOrigin = _punch->getPos();
        m_bounceCurrentType = BOUNCETYPE_ENEMY;
        m_bounceCurrentGameMaterial.invalidate();
        m_bounceCurrentDangerLevel = m_receivedHitLevel;
        m_bounceMultiplier = _punch->getBounceMultiplier();
    }

    if ( m_currentState == &m_stateReceiveHit )
    {
        m_stateReceiveHit.restart();
    }
    else
    {
        changeState(&m_stateReceiveHit);
    }
    Actor * sender = ActorRef(_punch->getSender()).getActor();
    if (sender)
    {

        EventHitSuccessful eventHit;
        eventHit.setSender(m_actor->getRef());
        eventHit.setHasBeenDamaged(btrue);
        eventHit.setIsDead(isDead());
        sender->onEvent(&eventHit);
    }

    //update survive piranhas flag
    if( m_survivePiranhaPondInside )
    {
        m_survivePiranhaPondLastHitTimer = SURVIVE_PIRANHAS_COUNTDOWN_AFTER_HIT;
    }
    if( m_survivePiranhaPondOutside )
    {
        m_survivePiranhaPondOutside = bfalse;
    }

    REWARD_MANAGER->Action_New(Ray_RewardManager::Action_ReceiveHit, m_playerIndex);
}

void Ray_PlayerControllerComponent::onForceMove()
{
    if (m_actor)
    {
        if ( m_currentState == &m_stateHanging )
        {
            detachFromEverything();
        }
    }
}

void Ray_PlayerControllerComponent::UpdateMoveInput( f32 _deltaTime )
{
    if ( m_moveToPoint )
    {
        updateMoveToPoint();
    }

    UpdateMoveDir();
    UpdateMoveTargetMultiplier(_deltaTime);
    UpdateDesiredLookDir();
}

bbool Ray_PlayerControllerComponent::tryWallJump( bbool _fromGround )
{
    if ( m_currentState == &m_stateJumping && m_currentJumpWallRun )
    {
        return bfalse;
    }

    PolyLine* wallJumpPolyline = NULL;
    bbool normalJump = bfalse;
    Vec2d contactNormal(Vec2d::Zero);

    if ( _fromGround )
    {
        PolyLine* stickedPoly = m_characterPhys->getStickedPolyline();

        if ( stickedPoly )
        {
            const PolyLineEdge* stickedEdge = m_characterPhys->getStickedEdge();

            if ( stickedEdge )
            {
                if ( isEdgeGoodForWallJump(stickedPoly,m_characterPhys->getStickedEdgeIndex(),stickedEdge,normalJump) ||
                     m_characterPhys->getEdgeOrientationType(stickedEdge->m_normalizedVector) == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF )
                {
                    wallJumpPolyline = stickedPoly;
                    contactNormal = -stickedEdge->m_normalizedVector.getPerpendicular();

                    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(stickedEdge->getGameMaterial()));

                    startWallLandFx(mat);

                    if ( m_wallJumpDisableTimer )
                    {
                        normalJump = btrue;
                    }
                    else if ( normalJump )
                    {
                        if ( m_currentState != &m_stateWallSlide && m_characterPhys->getStickOnWalls() )
                        {
                            // wallrun jump
                            normalJump = bfalse;
                        }
                    }
                }
            }
        }
    }
    else
    {
        f32 scaleY = m_actor->getScale().m_y;
        f32 radius = m_characterPhys->getRadius()+(getTemplate()->getAirWallJumpExtraRadius()*scaleY);
        PhysShapeCircle circle(radius);
        PhysContactsContainer contacts;
        Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());

        PHYSWORLD->checkEncroachment(m_actor->get2DPos(),m_actor->get2DPos(),m_actor->getAngle(),
                                     &circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

        for ( u32 index = 0; index < contacts.size(); index++ )
        {
            const SCollidableContact& contact = contacts[index];

            if ( contact.m_edgeIndex == U32_INVALID )
            {
                continue;
            }

            PolyLine* polyLine;
            const PolyLineEdge* edge;

            AIUtils::getPolyLine(contact.m_collidableUserData,contact.m_edgeIndex,polyLine,edge);

            if ( !polyLine || !edge )
            {
                continue;
            }

            if ( !isEdgeGoodForWallJump(polyLine,contact.m_edgeIndex,edge,normalJump) )
            {
                continue;
            }

            if ( lookDir.dot(edge->m_normalizedVector.getPerpendicular()) >= 0.f &&
                 m_characterPhys->getSpeed().dot(edge->m_normalizedVector.getPerpendicular()) > 0.f )
            {
                continue;
            }
            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

            startWallLandFx(mat);


            wallJumpPolyline = polyLine;
            contactNormal = contact.m_normal;

            if ( m_wallJumpDisableTimer )
            {
                normalJump = btrue;
            }

            break;
        }
    }

    if (wallJumpPolyline)
    {
        if (wallJumpPolyline->m_ownerFrise)
        {
            EventWallJump eventWallJump;
            eventWallJump.setSender(m_actor->getRef());
            wallJumpPolyline->m_ownerFrise->onEvent(&eventWallJump);
        }
        else if (wallJumpPolyline->getOwnerActor())
        {
            EventWallJump eventWallJump;
            eventWallJump.setSender(m_actor->getRef());
            wallJumpPolyline->getOwnerActor()->onEvent(&eventWallJump);
        }
    }

    if ( normalJump )
    {
        resetUTurn();
        setJumpData();

        if ( m_currentState != &m_stateJumping )
        {
            changeState(&m_stateJumping);
        }
        else
        {
            m_stateJumping.restartJump();
            m_animComponent->resetCurTime();
        }

        return btrue;
    }
    else if ( wallJumpPolyline )
    {
        resetUTurn();

        if ( contactNormal.m_x > 0.f )
        {
            setLookRight(btrue);
        }
        else if ( contactNormal.m_x < 0.f )
        {
            setLookRight(bfalse);
        }

        resetStance(STANCE_WALLSLIDE);
        m_wallSlideDir = contactNormal.getPerpendicular();
        m_characterPhys->resetWorldSpeed();
        setJumpData();
        startUTurn(UTURNTYPE_GROUND);

        if ( m_currentState != &m_stateJumping )
        {
            if ( m_characterPhys->getStickOnWalls() )
            {
                m_currentJumpWallRun = btrue;
            }

            changeState(&m_stateJumping);
        }
        else
        {
            m_stateJumping.restartJump();
            m_animComponent->resetCurTime();
        }

        return btrue;
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::isEdgeGoodForWallJump( PolyLine* _poly, u32 _edgeIndex, const PolyLineEdge* _edge, bbool& _normalJump ) const
{
    _normalJump = bfalse;

    const Ray_GameMaterial_Template* mat = static_cast<const Ray_GameMaterial_Template*>(World::getGameMaterial(_edge->getGameMaterial()));

    if ( mat && ( !mat->isSolid() || mat->getDangerousLevel() ) )
    {
        return bfalse;
    }

    if ( m_characterPhys->getEdgeOrientationType(_edge->m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
    {
        return bfalse;
    }

    bbool moveLeft = getMoveData().getMoveDirection() == MOVEDIR_LEFT;
    bbool moveRight = getMoveData().getMoveDirection() == MOVEDIR_RIGHT;

    if ( ( moveLeft && _edge->m_normalizedVector.getPerpendicular().dot(Vec2d::Right) < 0.f ) ||
         ( moveRight && _edge->m_normalizedVector.getPerpendicular().dot(Vec2d::Right) > 0.f ) )
    {
        // Pushing in the opposite direction gives us a walljump
        return btrue;
    }

    f32 cornerCheckDist = getTemplate()->getAirWallJumpCornerCheck() * m_actor->getScale().m_y;
    PolyLine* polyFound;
    u32 edgeFound;

    if ( AIUtils::searchEdgeAngle(m_actor->get2DPos(),_poly,_edgeIndex,bfalse,cornerCheckDist,m_currentGravityDir,
                                  0.f,m_characterPhys->getWallAngle(),&polyFound,&edgeFound) ||
         AIUtils::searchEdgeAngle(m_actor->get2DPos(),_poly,_edgeIndex,btrue,cornerCheckDist,m_currentGravityDir,
                                  0.f,m_characterPhys->getWallAngle(),&polyFound,&edgeFound) )
    {
        if ( polyFound )
        {
            const PolyLineEdge& edge = polyFound->getEdgeAt(edgeFound);
            StickToPolylinePhysComponent::EdgeOrientationType orientation = m_characterPhys->getEdgeOrientationType(edge.m_normalizedVector);

            if ( orientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
            {
                _normalJump = btrue;
            }
        }
    }

    return btrue;
}

void Ray_PlayerControllerComponent::setSurfingTimer()
{
    m_surfingTimer = getTemplate()->getSurfingJumpTime();
    m_characterPhys->setDisableWindForces(btrue);
    m_windDisableCounter = -1.f;
}

void Ray_PlayerControllerComponent::PerformJump()
{
    const Vec2d& gravityDir = m_characterPhys->getCurrentGravityDir();
    Vec2d gravityNorm = gravityDir.getPerpendicular();

    // If the jump is going to be in the same direction as the gravity we scale it by the difference
    Vec2d originalSpeed = m_characterPhys->getSpeed();
    Vec2d setSpeed = Vec2d::Zero;
    Vec2d addForce = gravityDir * (-getTemplate()->getJumpForceY());

    m_airApplyBrake = bfalse;

    performJumpProcessStances(originalSpeed,gravityNorm,setSpeed,addForce,m_airControlDisableTotal,m_airControlDisableAllowHelico,m_wallSlideAirControl,m_airControlDisableBlend,m_airControlDisableAmount);
    performJumpProcessImpulse(gravityDir,gravityNorm,setSpeed,addForce);
    performJumpAddMovingPlatform(gravityDir,gravityNorm,setSpeed,addForce);

    m_airControlDisableDelay = m_airControlDisableTotal;

    m_characterPhys->setSpeed(setSpeed);
    m_characterPhys->addForce(addForce);

    m_characterPhys->resetTorque();

    m_jumpQueueTimer = 0.0f;
    m_jumpOnAirTimer = 0.f;
    m_currentJumpMultiplier = 1.f;
    m_currentJumpMultiplierNoImpulse = 1.f;
    m_currentJumpUseWallDir = bfalse;
    m_currentJumpForceSideways = bfalse;
    m_currentJumpSpeedX = Vec2d::Zero;
    m_currentJumpQueueCrushAttack = bfalse;
    m_currentJumpKeepSpeedX = bfalse;
    m_currentJumpForceDisableAirControl = 0.f;

    CAMERACONTROLLERMANAGER->setSubjectJustJumped( m_actor->getRef());

    if ( !m_currentJumpWallRun )
    {
        m_characterPhys->setStickOnWalls(bfalse);
        m_walkOnWallsDisableTimer = 0.f;
    }
}

void Ray_PlayerControllerComponent::performJumpProcessStances( const Vec2d& _originalSpeed,
                                                               const Vec2d& _gravityNormal,
                                                               Vec2d& _setSpeed,
                                                               Vec2d& _addForce,
                                                               f32& _airControlDelay,
                                                               bbool& _airControlAllowHelico,
                                                               bbool& _wallSlideAirControl,
                                                               bbool& _airControlDisableBlend,
                                                               f32& _airControlDisableAmount ) const
{
    EStance stance = m_stance;

    if ( m_currentJumpUseWallDir )
    {
        stance = STANCE_WALLSLIDE;
    }

    _wallSlideAirControl = bfalse;
    _airControlAllowHelico = bfalse;

    if ( m_currentJumpWallRun )
    {
        // Special jump when running on a wall
        StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(m_wallSlideDir);

        if ( edgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF )
        {
            _setSpeed = _gravityNormal * (Vec2d::Right.dot(_originalSpeed)*getTemplate()->getWallRunJumpKeepSpeedFactor());
            _addForce = m_currentGravityDir * getTemplate()->getWallRunJumpForce();
        }
        else
        {
            f32 forceSign = m_wallSlideDir.getPerpendicular().dot(_gravityNormal) > 0.f ? 1.f : -1.f;
            _setSpeed = m_currentGravityDir * (m_currentGravityDir.dot(_originalSpeed)*getTemplate()->getWallRunJumpKeepSpeedFactor());
            _addForce = _gravityNormal * getTemplate()->getWallRunJumpForce() * forceSign;
        }

        _airControlDelay = getTemplate()->getWallRunJumpAirControlDelay();
    }
    else
    {
        switch(stance)
        {
        case STANCE_WALLSLIDE:
            {
                // When we wall slide we reset the speed and add an extra impulse on the direction of the
                // normal of the wall
                f32 dotP = m_wallSlideDir.getPerpendicular().dot(_gravityNormal);
                Vec2d impulse = _gravityNormal * getTemplate()->getWallSlideJumpXImpulse();

                if ( dotP < 0.f )
                {
                    impulse *= -1.f;
                }

                _addForce += impulse;
                _airControlDelay = getTemplate()->getAirControlDelayWallJump();
                _wallSlideAirControl = btrue;
                m_characterPhys->unstick();
            }
            break;
        case STANCE_SWING:
            {
                _addForce += _gravityNormal*m_swingJumpSign*getTemplate()->getSwingJumpForceX();
                _airControlDelay = getTemplate()->getAirControlDelayWallJump();
            }
            break;
        case STANCE_CLIMB:
        case STANCE_HANG:
            // If we were hanging we don't do anything
            {
                _airControlDelay = m_currentJumpForceDisableAirControl;

                if ( m_currentJumpKeepSpeedX )
                {
                    f32 dotP = _gravityNormal.dot(_originalSpeed);
                    _setSpeed = _gravityNormal*dotP;
                }
            }
            break;
        default:
            {
                // keep X speed
                _airControlDelay = 0.f;

                if ( m_currentJumpSpeedX != Vec2d::Zero )
                {
                    _setSpeed = m_currentJumpSpeedX;
                }
                else
                {
                    f32 dotP = _gravityNormal.dot(_originalSpeed);
                    _setSpeed = _gravityNormal*dotP;
                }

                if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
                {
                    PolyLine* poly = m_characterPhys->getStickedPolyline();

                    if ( poly && poly->getOwnerActor() )
                    {
                        Ray_EventQueryDisableAirControlInfo queryAirControl;

                        poly->getOwnerActor()->onEvent(&queryAirControl);

                        if ( queryAirControl.getDisable() )
                        {
                            _airControlDisableBlend = queryAirControl.getIsBlend();
                            _airControlDisableAmount = queryAirControl.getAmount();
                            _airControlDelay = queryAirControl.getDuration();
                            _airControlAllowHelico = queryAirControl.getAllowHelico();
                            _setSpeed = Vec2d::Zero;
                        }
                    }
                }
            }
            break;
        }
    }
}

void Ray_PlayerControllerComponent::performJumpProcessImpulse( const Vec2d& _gravityDir,
                                                               const Vec2d& _gravityNormal,
                                                               const Vec2d& _setSpeed,
                                                               Vec2d& _addForce ) const
{
    if ( m_stance == STANCE_SWING || m_currentJumpWallRun )
    {
        return;
    }

    f32 dotProdGravities = _gravityDir.dot(_gravityDir);

    if ( dotProdGravities < 0.f )
    {
        _addForce *= 1.f + dotProdGravities;
    }

    // Check if it's a sideways jump, meaning that the player is pressing left or right.
    // For this case we apply a different impulse, to make the jump more sideways than vertical.
    // When jumping from hang we need to check if we are jumping against the wall, because in this
    // case we don't treat it like a sideways jump
    bbool pushingLeft = getMoveData().getMoveDirection() == MOVEDIR_LEFT;
    bbool pushingRight = getMoveData().getMoveDirection() == MOVEDIR_RIGHT;
    bbool sidewaysJump = pushingLeft || pushingRight;

    if ( sidewaysJump && m_stance == STANCE_HANG )
    {
        if ( m_climbingT == 0.f )
        {
            // Hanging on the left
            sidewaysJump = pushingLeft;
        }
        else
        {
            // Hanging on the right
            sidewaysJump = pushingRight;
        }
    }

    // First apply a force multiplier if we press left or right or not, and
    // if we push the stick apply a force sideways
    if ( sidewaysJump || m_currentJumpForceSideways )
    {
        _addForce *= m_currentJumpMultiplier;

        // Impulse in X
        f32 totalImpulse;

        if ( m_currentJumpForceSideways || !sidewaysJump )
        {
            Vec2d speedVec = _setSpeed == Vec2d::Zero ? AIUtils::getLookDir(m_actor) : _setSpeed;
            f32 worldSpeedX = _gravityNormal.dot(speedVec);
            f32 impulseToSpeed = m_currentJumpImpulseX*LOGICDT;

            totalImpulse = getSign(worldSpeedX);

            if ( f32_Abs(worldSpeedX) < impulseToSpeed )
            {
                // If we are going slower then calculate the impulse to match the right speed
                if ( worldSpeedX < 0.f )
                {
                    impulseToSpeed *= -1.f;
                }

                totalImpulse = ( impulseToSpeed - worldSpeedX ) / LOGICDT;
            }
            else
            {
                // If we are going faster than the impulse then there is nothing to do
                totalImpulse = 0.f;
            }
        }
        else
        {
            f32 worldSpeedX = _gravityNormal.dot(_setSpeed);

            totalImpulse = _gravityNormal.dot(getMoveData().getMove());

            if ( ( worldSpeedX >= 0.f ) == ( totalImpulse >= 0.f ) )
            {
                // If the push is done in the same direction as our move we try to reach the speed
                // of the sideways impulse
                f32 impulseToSpeed = m_currentJumpImpulseX*LOGICDT;

                if ( f32_Abs(worldSpeedX) < impulseToSpeed )
                {
                    // If we are going slower then calculate the impulse to match the right speed
                    if ( worldSpeedX < 0.f )
                    {
                        impulseToSpeed *= -1.f;
                    }

                    totalImpulse = ( impulseToSpeed - worldSpeedX ) / LOGICDT;
                }
                else
                {
                    // If we are going faster than the impulse then there is nothing to do
                    totalImpulse = 0.f;
                }
            }
            else
            {
                totalImpulse *= m_currentJumpImpulseX;
            }
        }

        if ( m_currentTargetForce != Vec2d::Zero )
        {
            totalImpulse = m_currentTargetForce.dot(_gravityNormal)*totalImpulse;
        }

        if ( totalImpulse )
        {
            _addForce += _gravityNormal*totalImpulse;
        }
    }
    else
    {
        _addForce *= m_currentJumpMultiplierNoImpulse;
    }
}

void Ray_PlayerControllerComponent::performJumpAddMovingPlatform( const Vec2d& _gravityDir,
                                                                  const Vec2d& _gravityNormal,
                                                                  Vec2d& _setSpeed,
                                                                  Vec2d& _addForce ) const
{
    if ( m_baseInfoSpeed == Vec2d::Zero )
    {
        return;
    }

    if ( m_baseInfoPrevMultiplierX )
    {
        f32 xSpeed = _gravityNormal.dot(m_baseInfoSpeed);
        f32 xForce = xSpeed/LOGICDT;

        _addForce += _gravityNormal * xForce * m_baseInfoPrevMultiplierX;
    }

    if ( m_baseInfoPrevMultiplierY )
    {
        f32 ySpeed = _gravityDir.dot(m_baseInfoSpeed);

        if ( ySpeed < 0.f )
        {
            f32 yForce = ySpeed/LOGICDT;

            _addForce += _gravityDir * yForce * m_baseInfoPrevMultiplierY;
        }
    }
}

void Ray_PlayerControllerComponent::addJumpAirImpulse()
{
    // This adds suspension in the air
    f32 ySpeed = -m_currentGravityDir.dot(m_characterPhys->getSpeed());

    if ( ySpeed > 0.f && ( m_stance == STANCE_WALLSLIDE || !m_characterPhys->getStickedPolyline() ) && ySpeed < getTemplate()->getAirSuspensionMaxYSpeed() )
    {
        Vec2d gravity = -m_currentGravity;

        ySpeed = Clamp(ySpeed,getTemplate()->getAirSuspensionMinYSpeed(),getTemplate()->getAirSuspensionMaxYSpeed());
        f32 t = ( ySpeed - getTemplate()->getAirSuspensionMinYSpeed() ) / ( getTemplate()->getAirSuspensionMaxYSpeed() - getTemplate()->getAirSuspensionMinYSpeed() );
        f32 multiplier = Interpolate(getTemplate()->getAirSuspensionMinMultiplier(),getTemplate()->getAirSuspensionMaxMultiplier(),t);
        m_characterPhys->addForce(gravity*multiplier);
    }
}

#ifdef ITF_SUPPORT_CHEAT
void Ray_PlayerControllerComponent::UpdatePhysicFlyMode( f32 _deltaTime )
{
    const Vec3d newPos = m_actor->getPos() + ( getMoveDataAnalog().getMove() * _deltaTime * getTemplate()->getFlyModeSpeed() ).to3d();

    m_actor->setPos(newPos);
    m_actor->onForceMove();
}
#endif

void Ray_PlayerControllerComponent::updateClimbEdgeOrientation( f32 _dt )
{
    f32 targetAngle = getVec2Angle(m_currentGravityDir.getPerpendicular());
    f32 currentAngle = m_actor->getAngle();
    f32 dif = getShortestAngleDelta(currentAngle,targetAngle);

    if ( dif )
    {
        f32 move = getTemplate()->getClimbEdgeAngularSpeed().ToRadians() * _dt;

        if ( f32_Abs(dif) < move )
        {
            currentAngle = targetAngle;
        }
        else
        {
            if ( dif > 0.f )
            {
                currentAngle += move;
            }
            else
            {
                currentAngle -= move;
            }
        }

        m_actor->setAngle(currentAngle);
    }
}

// Moves the character on the polyline. It will return true if it drops the polyline
void Ray_PlayerControllerComponent::updateClimbEdgeMove( f32 _dt, const PolyLine* _polyLine, const PolyLineEdge* _edge, Vec2d& _moveInput )
{
    // apply the movement based on the input
    f32 move, moveSign;
    EMoveDir climbDir;
    updateClimbEdgeApplyMove(_dt,_edge,move,moveSign,climbDir);

    // adjust the position on the rope. move will keep how much we moved this frame
    updateClimbEdgeAdjustClimbT(_polyLine,_edge,move);

    updateClimbEdgeSetAnimData(_dt,_edge,move,moveSign,_moveInput);

    // Update the position of the actor on the rope
    EventHangUpdate event(GetActor()->getRef(),
        m_climbingPolyline,
        m_climbingEdge,
        m_characterPhys->getWeight(),
        m_characterPhys->getSpeed(),
        m_climbingT);

    if ( _polyLine->getOwnerActor() )
        _polyLine->getOwnerActor()->onEvent(&event);
    else if (_polyLine->m_ownerFrise)
        _polyLine->m_ownerFrise->onEvent(&event);

    if ( m_climbingT != 1.f ||
         m_climbingEdge != (_polyLine->getVectorsCount()-1) )
    {
        m_climbingOnBottomCorner = bfalse;
    }
    else
    {
        m_climbingOnBottomCorner = btrue;
    }

    if ( m_climbingVertical )
    {
        if ( !m_climbingOnBottomCorner )
        {
            if ( m_climbingConstrainedEdge != U32_INVALID )
            {
                m_climbingAllowDropVertical = m_climbingEdge == m_climbingConstrainedEdge &&
                    fabs(m_climbingT-m_climbingConstrainedT) <= 0.001f;
            }
            else
            {
                m_climbingAllowDropVertical = bfalse;
            }
        }
        else
        {
            if ( getMoveData().getMoveDirection() == MOVEDIR_NONE )
            {
                m_climbingAllowDropVertical = btrue;
            }
        }
    }
    else
    {
        if ( !m_climbingAllowExitHorizontal && getMoveData().getMoveDirection() != MOVEDIR_DOWN )
        {
            m_climbingAllowExitHorizontal = btrue;
        }

        if ( !isPerformingUTurn() )
        {
            if ( ( climbDir == MOVEDIR_LEFT && !m_actor->isFlipped() ) ||
                 ( climbDir == MOVEDIR_RIGHT && m_actor->isFlipped() ) )
            {
                startUTurn(UTURNTYPE_CLIMB);
            }
        }
    }
}

void Ray_PlayerControllerComponent::updateClimbEdgeApplyMove( f32 _dt, const PolyLineEdge* _edge, f32& _move, f32& _moveSign, EMoveDir& _climbDir )
{
    updateClimbEdgeGetMoveData(_edge,_moveSign,_climbDir,_move);
    updateClimbEdgeUpdateSprinting(_climbDir);
    updateClimbEdgeUpdateInertia(_dt,_climbDir,_move);
    updateClimbEdgeGetMoveSize(_dt,_moveSign,_climbDir,_move);

    _move *= _dt;

    f32 currentDist = _edge->m_length * m_climbingT;

    currentDist += _move;

    f32 prevClimbT = m_climbingT;
    m_climbingT = currentDist / _edge->m_length;

    if ( m_climbingConstrainedT && m_climbingConstrainedEdge != U32_INVALID && m_climbingEdge == m_climbingConstrainedEdge )
    {
        if ( ( prevClimbT <= m_climbingConstrainedT && m_climbingT > m_climbingConstrainedT ) ||
            ( prevClimbT >= m_climbingConstrainedT && m_climbingT < m_climbingConstrainedT ) )
        {
            m_climbingT = m_climbingConstrainedT;
        }
    }

    m_climbingPrevMoveSign = _moveSign;
}

void Ray_PlayerControllerComponent::updateClimbEdgeSetAnimData( f32 _dt, const class PolyLineEdge* _edge, f32 _move, f32 _moveSign, Vec2d& _moveInput )
{
    f32 currentMove = f32_Abs(_move);

    if ( currentMove < MTH_EPSILON )
    {
        currentMove = 0.f;
        _moveSign = 0.f;
    }

    EMoveDir animMoveDir = updateClimbEdgeGetClimbDirFromMoveSign(_moveSign,m_climbingVertical,_edge);
    Vec2d speed;

    switch(animMoveDir)
    {
    case MOVEDIR_UP:
        speed = (-m_currentGravityDir)*f32_Abs(_move);
        _moveInput.m_y = currentMove;
        break;
    case MOVEDIR_DOWN:
        speed = (m_currentGravityDir)*f32_Abs(_move);
        _moveInput.m_y = -currentMove;
        break;
    case MOVEDIR_LEFT:
        speed = (-m_currentGravityDir.getPerpendicular())*f32_Abs(_move);
        if ( m_climbingSlide )
        {
            _moveInput.m_x = 0.f;
        }
        else
        {
            _moveInput.m_x = -currentMove;
        }
        break;
    case MOVEDIR_RIGHT:
        speed = (m_currentGravityDir.getPerpendicular())*f32_Abs(_move);
        if ( m_climbingSlide )
        {
            _moveInput.m_x = 0.f;
        }
        else
        {
            _moveInput.m_x = currentMove;
        }
        break;
    default:
        speed = Vec2d::Zero;
        break;
    }

    m_characterPhys->setSpeed(speed/_dt);
}

void Ray_PlayerControllerComponent::updateClimbEdgeAdjustClimbT( const PolyLine* _polyLine, const PolyLineEdge* _edge, f32& _move )
{
    bbool constrained = bfalse;

    if ( m_climbingConstrainedEdge != U32_INVALID && m_climbingIntertia >= 0.f )
    {
        if ( m_climbingEdge == m_climbingConstrainedEdge )
        {
            if ( m_climbingT >= 1.f &&
                 m_climbingConstrainedT == 1.f )
            {
                m_climbingT = 1.f;
                _move = 0.f;
                constrained = btrue;
            }
            else if ( m_climbingT <= 0.f &&
                m_climbingConstrainedT == 0.f )
            {
                m_climbingT = 0.f;
                _move = 0.f;
                constrained = btrue;
            }
        }
    }

    if ( constrained )
    {
        return;
    }

    if ( m_climbingT < 0.f )
    {
        while ( m_climbingT < 0.f )
        {
            if ( m_climbingEdge == 0 )
            {
                // We are going out of the polyline, so stop
                // Remove the extra movement from the move counter
                f32 extra = _edge->m_length * -m_climbingT;

                if ( _move > 0.f )
                {
                    _move -= extra;
                }
                else if ( _move < 0.f )
                {
                    _move += extra;
                }

                m_climbingT = 0.f;
            }
            else
            {
                f32 dist = _edge->m_length * m_climbingT;
                m_climbingEdge = m_climbingEdge - 1;
                _edge = &_polyLine->getEdgeAt(m_climbingEdge);
                m_climbingT = ( _edge->m_length + dist ) / _edge->m_length;
            }
        }
    }
    else if ( m_climbingT > 1.f )
    {
        while ( m_climbingT > 1.f )
        {
            if ( m_climbingEdge == _polyLine->getVectorsCount()-1 )
            {
                // We are going out of the polyline, so stop
                // Remove the extra movement from the move counter
                f32 extra = _edge->m_length * (m_climbingT-1.f);

                if ( _move > 0.f )
                {
                    _move -= extra;
                }
                else if ( _move < 0.f )
                {
                    _move += extra;
                }

                m_climbingT = 1.f;
            }
            else
            {
                f32 dist = _edge->m_length * (m_climbingT - 1.f);
                m_climbingEdge = m_climbingEdge + 1;
                _edge = &_polyLine->getEdgeAt(m_climbingEdge);
                m_climbingT = dist / _edge->m_length;
            }
        }
    }
}

void Ray_PlayerControllerComponent::updateClimbEdgeUpdateSprinting( EMoveDir _climbDir )
{
    bbool sprint = trySprint();

    if ( sprint )
    {
        if ( _climbDir == MOVEDIR_NONE )
        {
            sprint = bfalse;
        }
        else if ( m_climbingVertical && _climbDir == MOVEDIR_DOWN )
        {
            sprint = bfalse;
        }
    }

    setSprinting(sprint);
}

Ray_PlayerControllerComponent::EMoveDir Ray_PlayerControllerComponent::updateClimbEdgeGetClimbDirFromMoveSign( f32 _moveSign, bbool _vertical, const class PolyLineEdge* _edge ) const
{
    EMoveDir climbDir;

    if ( !_moveSign )
    {
        climbDir = MOVEDIR_NONE;
    }
    else if ( _vertical )
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_currentGravityDir);

        if ( dotP > 0.f )
        {
            // The polyline is going down

            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_UP;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_DOWN;
            }
        }
        else
        {
            // The polyline is going up
            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_DOWN;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_UP;
            }
        }
    }
    else
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_currentGravityDir.getPerpendicular());

        if ( dotP > 0.f )
        {
            // The polyline is going right

            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_LEFT;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_RIGHT;
            }
        }
        else
        {
            // The polyline is going left
            if ( _moveSign < 0.f )
            {
                // Opposite direction
                climbDir = MOVEDIR_RIGHT;
            }
            else
            {
                // Same direction
                climbDir = MOVEDIR_LEFT;
            }
        }
    }

    return climbDir;
}

f32 Ray_PlayerControllerComponent::updateClimbEdgeGetClimbSignFromClimbDir( EMoveDir _moveDir, bbool _vertical, const class PolyLineEdge* _edge ) const
{
    f32 moveSign;

    if ( _moveDir == MOVEDIR_NONE )
    {
        moveSign = 0.f;
    }
    else if ( _vertical )
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_currentGravityDir);

        if ( _moveDir == MOVEDIR_DOWN )
        {
            // We want to go down. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going down, follow the direction
                moveSign = 1.f;
            }
            else
            {
                // If the polyline is going up, we go in the opposite direction
                moveSign = -1.f;
            }
        }
        else if ( _moveDir == MOVEDIR_UP )
        {
            // We want to go up. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going down, we go in the opposite direction
                moveSign = -1.f;
            }
            else
            {
                // If the polyline is going up, we go in the same direction
                moveSign = 1.f;
            }
        }
        else
        {
            moveSign = 0.f;
        }
    }
    else
    {
        f32 dotP = _edge->m_normalizedVector.dot(m_currentGravityDir.getPerpendicular());

        if ( _moveDir == MOVEDIR_RIGHT )
        {
            // We want to go right. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going right, follow the direction
                moveSign = 1.f;
            }
            else
            {
                // If the polyline is going left, we go in the opposite direction
                moveSign = -1.f;
            }
        }
        else if ( _moveDir == MOVEDIR_LEFT )
        {
            // We want to go left. Check the direction of the polyline
            if ( dotP > 0.f )
            {
                // If the polyline is going right, we go in the opposite direction
                moveSign = -1.f;
            }
            else
            {
                // If the polyline is going left, follow the direction
                moveSign = 1.f;
            }
        }
        else
        {
            moveSign = 0.f;
        }
    }

    return moveSign;
}

void Ray_PlayerControllerComponent::updateClimbEdgeGetMoveData( const PolyLineEdge* _edge, f32& _moveSign, EMoveDir& _climbDir, f32& _move ) const
{
    if ( m_coopActionMode == ACTIONCOOPMODE_MAIN )
    {
        _climbDir = MOVEDIR_NONE;
        _moveSign = 0.f;
        _move = 0.f;
    }
    else
    {
        if ( m_climbingSlide )
        {
            if ( m_climbingVertical )
            {
                if ( m_climbingIntertia >= 0 )
                {
                    _moveSign = 1.f;
                    _climbDir = MOVEDIR_DOWN;
                    _move = 1.f;
                }
                else
                {
                    _moveSign = 0.f;
                    _climbDir = MOVEDIR_NONE;
                    _move = 0.f;
                }
            }
            else
            {
                f32 dotProd = _edge->m_normalizedVector.dot(m_currentGravityDir.getPerpendicular());
                _moveSign = 1.f;
                _climbDir = dotProd > 0.f ? MOVEDIR_RIGHT : MOVEDIR_LEFT;
                _move = 1.f;
            }
        }
        else if ( m_climbingConstrainedEdge != U32_INVALID )
        {
            _moveSign = 0.f;
            _climbDir = MOVEDIR_NONE;
            _move = 0.f;

            if ( m_climbingEdge < m_climbingConstrainedEdge )
            {
                _moveSign = 1.f;
                _climbDir = MOVEDIR_DOWN;
                _move = 1.f;
            }
            else if ( m_climbingEdge > m_climbingConstrainedEdge )
            {
                if ( m_climbingIntertia >= 0.f )
                {
                    _moveSign = -1.f;
                    _climbDir = MOVEDIR_UP;
                    _move = 1.f;
                }
            }
            else
            {
                if ( fabs(m_climbingT-m_climbingConstrainedT) > 0.001f )
                {
                    if ( m_climbingT < m_climbingConstrainedT )
                    {
                        _moveSign = 1.f;
                        _climbDir = MOVEDIR_DOWN;
                        _move = 1.f;
                    }
                    else if ( m_climbingT > m_climbingConstrainedT )
                    {
                        if ( m_climbingIntertia >= 0.f )
                        {
                            _moveSign = -1.f;
                            _climbDir = MOVEDIR_UP;
                            _move = 1.f;
                        }
                    }
                }
            }
        }
        else
        {
            if ( m_climbingJustStarted )
            {
                _move = 0.f;
                _moveSign = 0.f;
                _climbDir = MOVEDIR_NONE;
            }
            else
            {
                _move = getMoveData().getMove().norm();

                if ( getMoveData().getMoveSameDir() && m_climbingPrevMoveSign )
                {
                    // If we move in the same dir, we find out the data from the previous move sign
                    _moveSign = m_climbingPrevMoveSign;
                    _climbDir = updateClimbEdgeGetClimbDirFromMoveSign(_moveSign,m_climbingVertical,_edge);
                }
                else
                {
                    _climbDir = getMoveData().getMoveDirection();
                    _moveSign = updateClimbEdgeGetClimbSignFromClimbDir(_climbDir,m_climbingVertical,_edge);
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::updateClimbEdgeGetMoveSize( f32 _deltaTime, f32 _moveDir, EMoveDir _climbDir, f32& _moveSize ) const
{
    if ( m_coopActionMode == ACTIONCOOPMODE_MAIN )
    {
        _moveSize = 0.f;
    }
    else
    {
        if ( m_climbingVertical )
        {
            if ( _climbDir != MOVEDIR_NONE )
            {
                if ( _climbDir == MOVEDIR_UP )
                {
                    // Moving up
                    _moveSize *= getTemplate()->getClimbVerticalSpeedUp();
                }
                else
                {
                    // Descending, increase inertia
                    _moveSize = m_climbingIntertia;
                }
            }
            else
            {
                _moveSize = m_climbingIntertia;
                _moveDir = 1.f;
            }
        }
        else
        {
            if ( _climbDir == MOVEDIR_LEFT || _climbDir == MOVEDIR_RIGHT )
            {
                // Moving left or right
                if ( m_climbingSlide )
                {
                    _moveSize = m_climbingIntertia;
                }
                else
                {
                    _moveSize *= getTemplate()->getClimbHorizontalSpeed();
                }
            }
            else
            {
                _moveSize = 0.f;
            }
        }
    }

    if ( isSprinting() && !m_climbingSlide )
    {
        _moveSize *= getTemplate()->getClimbSprintSpeedMultiplier();
    }

    _moveSize *= _moveDir;
}

void Ray_PlayerControllerComponent::updateClimbEdgeUpdateInertia( f32 _deltaTime, EMoveDir _climbDir, f32 _move )
{
    if ( m_climbingVertical )
    {
        if ( _climbDir != MOVEDIR_NONE )
        {
            if ( _climbDir == MOVEDIR_DOWN )
            {
                // Descending, increase inertia
                f32 accel;

                if ( m_climbingIntertia < 0.f )
                {
                    accel = _move * getTemplate()->getClimbVerticalAscendAccel();
                }
                else
                {
                    accel = _move * getTemplate()->getClimbVerticalDescendAccel();
                }

                m_climbingIntertia += accel * _deltaTime;

                f32 maxSpeed;

                if ( isSprinting() )
                {
                    maxSpeed = getTemplate()->getClimbVerticalDescendMaxSpeedSprint();
                }
                else
                {
                    maxSpeed = getTemplate()->getClimbVerticalDescendMaxSpeed();
                }

                if ( m_climbingIntertia > maxSpeed )
                {
                    f32 dif = m_climbingIntertia - maxSpeed;

                    m_climbingIntertia -= dif*0.8f;
                }
            }
            else
            {
                m_climbingIntertia = 0.f;
            }
        }
        else if ( m_climbingIntertia > 0.f )
        {
            m_climbingIntertia -= getTemplate()->getClimbEdgeInertiaBrake() * _deltaTime;
            m_climbingIntertia = Max(m_climbingIntertia,0.f);
        }
        else if ( m_climbingIntertia < 0.f )
        {
            m_climbingIntertia += getTemplate()->getClimbEdgeInertiaBrake() * _deltaTime;
            m_climbingIntertia = Min(m_climbingIntertia,0.f);
        }
    }
    else
    {
        if ( m_climbingSlide )
        {
            f32 accel;

            if ( m_climbingIntertia < 0.f )
            {
                accel = _move * getTemplate()->getClimbVerticalAscendAccel();
            }
            else
            {
                accel = _move * getTemplate()->getClimbVerticalDescendAccel();
            }

            m_climbingIntertia += accel * _deltaTime;

            if ( m_climbingIntertia > 0.f )
            {
                f32 maxSpeed;

                if ( isSprinting() )
                {
                    maxSpeed = getTemplate()->getClimbHorizontalDescendMaxSpeedSprint();
                }
                else
                {
                    maxSpeed = getTemplate()->getClimbHorizontalDescendMaxSpeed();
                }

                if ( m_climbingIntertia > maxSpeed )
                {
                    f32 dif = m_climbingIntertia - maxSpeed;

                    m_climbingIntertia -= dif*0.8f;
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::UpdatePhysicClimbingEdge( f32 _dt )
{
    PolyLine* polyLine;
    const PolyLineEdge* edge;
    Vec2d moveInput = Vec2d::Zero;

    getClimbEdgePolyline(&polyLine,&edge);

    if ( polyLine->getOwnerActor() && m_climbingVertical )
    {
        EventGetPolylineConstrainedPosition constrainEvent(m_actor->getRef(),polyLine->getRef());
        polyLine->getOwnerActor()->onEvent(&constrainEvent);

        m_climbingConstrainedEdge = constrainEvent.getEdgeIndex();
        m_climbingConstrainedT = constrainEvent.getT();
    }

    // Orient the character towards the gravity
    updateClimbEdgeOrientation(_dt);

    if ( m_climbingVertical && polyLine->getOwnerActor() )
    {
        // Swing if it's a vertical polyline
        updateClimbSwingImpulse(polyLine,moveInput);
    }

    // Move our character
    updateClimbEdgeMove(_dt,polyLine,edge,moveInput);

    updateClimbEdgeAllowHangCoop(polyLine,m_climbingEdge);

    AIUtils::getPolyLine(m_climbingPolyline,m_climbingEdge,polyLine,edge);

    if ( polyLine && edge )
    {
        Vec2d climbPos = edge->getPos() + ( edge->m_vector *  m_climbingT );
        m_animComponent->lockMagicBox(climbPos.to3d(m_actor->getPos().m_z));
    }

    setMoveInput(moveInput);
    updateLianaSlideFx();
}

void Ray_PlayerControllerComponent::updateClimbEdgeAllowHangCoop( PolyLine* _polyLine, u32 _edgeIndex )
{
    if ( !m_climbingVertical )
    {
        m_climbingAllowHangCoop = btrue;
        return;
    }

    f32 distLeft = AIUtils::getRemainingDistanceOnEdge(m_climbingT,_polyLine,_edgeIndex,bfalse);

    m_climbingAllowHangCoop = distLeft <= getTemplate()->getClimbEdgeCoopHangEdgeRange();
}

Ray_PlayerControllerComponent::EMoveDir Ray_PlayerControllerComponent::getLookDirection() const
{
    if ( m_stance == STANCE_STAND &&
         isCrouch() )
    {
        f32 xSize = getMoveDataAnalog().getMoveReal().dot(m_currentGravityDir.getPerpendicular());

        if ( f32_Abs(xSize) > 0.3f )
        {
            if ( xSize > 0.f )
            {
                return MOVEDIR_RIGHT;
            }
            else
            {
                return MOVEDIR_LEFT;
            }
        }
    }
    else
    {
        if ( getMoveData().getMoveDirection() == MOVEDIR_LEFT || getMoveData().getMoveDirection() == MOVEDIR_RIGHT )
        {
            return getMoveData().getMoveDirection();
        }
    }

    if ( !m_actor->isFlipped() )
    {
        return MOVEDIR_RIGHT;
    }
    else
    {
        return MOVEDIR_LEFT;
    }
}

// This transforms an angle into a move direction. The angle has to be normalized around 2PI
Ray_PlayerControllerComponent::EMoveDir Ray_PlayerControllerComponent::MoveData::getMoveDirectionFromAngle( f32 _angle ) const
{
    EMoveDir dir;
    // favorites right and left move with an epsilon (in case the angle is approximate 45 degree)
    static const f32 s_epsilon = 0.0001f;
    if ( _angle <= ((45.f*MTH_DEGTORAD)+s_epsilon) || _angle >= ((315*MTH_DEGTORAD)-s_epsilon) )
    {
        dir = MOVEDIR_RIGHT;
    }
    else if ( _angle > (45.f*MTH_DEGTORAD) && _angle < (135.f*MTH_DEGTORAD) )
    {
        dir = MOVEDIR_UP;
    }
    else if ( _angle >= ((135.f*MTH_DEGTORAD)-s_epsilon) && _angle <= ((225.f*MTH_DEGTORAD)+s_epsilon) )
    {
        dir = MOVEDIR_LEFT;
    }
    else
    {
        dir = MOVEDIR_DOWN;
    }

    return dir;
}

void Ray_PlayerControllerComponent::MoveData::update( const Vec2d& _move )
{
    m_moveReal = _move;
    m_moveRealNormalized = _move;
    m_moveRealNormalized.normalize();

    bbool isMoving = m_moveRealNormalized != Vec2d::Zero;

    m_moveSameDir = isMoving &&
                    m_moveReal.dot(m_prevMove) > 0.98f;

    if ( !isMoving || !m_moveSameDir )
    {
        m_move = m_moveReal;
        m_moveNormalized = m_moveRealNormalized;
        m_prevMove = m_move;
        m_prevMoveNormalized = m_moveNormalized;

        if ( isMoving )
        {
            f32 moveAngle = getVec2Angle(m_moveNormalized);

            NormalizeAngle2PI(moveAngle);

            m_moveDirection = getMoveDirectionFromAngle(moveAngle);
        }
        else
        {
            m_moveDirection = MOVEDIR_NONE;
        }
    }
}

void Ray_PlayerControllerComponent::MoveData::rotate( f32 _angle )
{
    // Now calculate the gravity-relative values
    m_move = m_move.Rotate(_angle);
    m_moveNormalized = m_moveNormalized.Rotate(_angle);
    m_moveReal = m_move.Rotate(_angle);
    m_moveRealNormalized = m_moveNormalized.Rotate(_angle);
    m_prevMove = m_prevMove.Rotate(_angle);
    m_prevMoveNormalized = m_prevMoveNormalized.Rotate(_angle);

    f32 moveAngle = getVec2Angle(m_moveNormalized);

    NormalizeAngle2PI(moveAngle);

    m_moveDirection = getMoveDirectionFromAngle(moveAngle);
}

void Ray_PlayerControllerComponent::MoveData::flip()
{
    m_move.m_x *= -1.f;
    m_moveNormalized.m_x *= -1.f;

    if ( m_moveDirection == MOVEDIR_RIGHT )
    {
        m_moveDirection = MOVEDIR_LEFT;
    }
    else if ( m_moveDirection == MOVEDIR_LEFT )
    {
        m_moveDirection = MOVEDIR_RIGHT;
    }
}

void Ray_PlayerControllerComponent::UpdateMoveDir()
{
    Vec2d move = m_listenerMove;

    f32 sqSize = move.sqrnorm();

    if ( sqSize < (0.6f*0.6f) )
    {
        move = Vec2d::Zero;
    }
    else if ( sqSize > 1.f )
    {
        move.normalize();
    }

    m_moveAnalog.update(move);

    if ( f32_Abs(move.m_x) < 0.6f )
    {
        move.m_x = 0.f;
    }
    else
    {
        move.m_x = Clamp(move.m_x,-1.f,1.f);
    }

    if ( f32_Abs(move.m_y) < 0.6f )
    {
        move.m_y = 0.f;
    }
    else
    {
        move.m_y = Clamp(move.m_y,-1.f,1.f);
    }

    m_move.update(move);

    m_moveGravity = m_move;
    m_moveGravityAnalog = m_moveAnalog;

    f32 gravityAngle = getVec2Angle(m_currentGravityDir.getPerpendicular());

    m_moveGravity.rotate(gravityAngle);
    m_moveGravityAnalog.rotate(gravityAngle);

    Vec2d dir = Vec2d::Right;

    if ( m_actor->isFlipped() )
    {
        dir = -dir;
    }

    Vec2d moveInput;

    moveInput.m_x = m_moveAnalog.getMoveReal().dot(dir);
    moveInput.m_y = m_moveAnalog.getMoveReal().m_y;

    setMoveInput(moveInput);
}

bbool Ray_PlayerControllerComponent::isTryingMove() const
{
    if ( m_stance == STANCE_SWIM )
    {
        if ( isPowerUpEnabled(Ray_PowerUp_Dive) )
        {
            if ( m_swimingIsOnSurface )
            {
                if ( getMoveData().getMoveDirection() == MOVEDIR_UP ||
                     getMoveData().getMoveDirection() == MOVEDIR_NONE )
                {
                    return bfalse;
                }
                else
                {
                    return btrue;
                }
            }
            else
            {
                return getMoveData().getMoveDirection() != MOVEDIR_NONE;
            }
        }
        else
        {
            if ( getMoveData().getMoveDirection() != MOVEDIR_LEFT &&
                 getMoveData().getMoveDirection() != MOVEDIR_RIGHT )
            {
                return bfalse;
            }
            else
            {
                return btrue;
            }
        }
    }
    else if ( m_stance == STANCE_STAND )
    {
        if ( m_characterPhys->getStickedEdge() )
        {
            return getMoveData().getMove().m_x!=0.f;
        }
        else
        {
            return getMoveData().getMove() != Vec2d::Zero;
        }
    }
    else if ( isHanging() )
    {
        if ( m_currentState == &m_stateHangToIdle )
        {
            return getMoveData().getMoveDirection() == MOVEDIR_LEFT ||
                   getMoveData().getMoveDirection() == MOVEDIR_RIGHT;
        }
        else
        {
            return bfalse;
        }
    }
    else
    {
        return getMoveData().getMove() != Vec2d::Zero;
    }
}

f32 Ray_PlayerControllerComponent::getMoveTargetBlendTime() const
{
    switch(m_stance)
    {
        case STANCE_SWIM:
            return getTemplate()->getSwimingMoveTargetMultiplierBlendTime();
        default:
            return getTemplate()->getMoveTargetMultiplierBlendTime();
    }
}

void Ray_PlayerControllerComponent::UpdateMoveTargetMultiplier( f32 _deltaTime )
{
    bbool moving = isTryingMove();
    f32 blendTime = getMoveTargetBlendTime();
    const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();
    bbool onGround = currentEdge != NULL;
    bbool removeBlend;

    // normal move blender
    if ( isPerformingUTurn() && !m_uTurnFlipped && !m_currentJumpWallRun )
    {
        removeBlend = btrue;
    }
    else if ( m_releaseSprintActive )
    {
        removeBlend = btrue;
    }
    else if ( !moving && !isSprinting() )
    {
        removeBlend = btrue;
    }
    else if ( m_stance != STANCE_SWIM &&
              m_currentState->IsClassCRC(ITF_GET_STRINGID_CRC(StateHitBase,676090084)) &&
              m_currentPunchHitType != PUNCHTYPE_REDUCEDSIZE )
    {
        removeBlend = btrue;
    }
    else
    {
        removeBlend = bfalse;
    }

    if ( removeBlend )
    {
        m_moveTargetBlendTimer = 0.f;
    }
    else
    {
        m_moveTargetBlendTimer = Min(m_moveTargetBlendTimer+_deltaTime,blendTime);
    }

    if ( blendTime )
    {
        m_currentMoveTargetMultiplier = Interpolate(getTemplate()->getMoveTargetMultiplierMin(),getTemplate()->getMoveTargetMultiplierMax(),m_moveTargetBlendTimer/blendTime);
    }
    else
    {
        m_currentMoveTargetMultiplier = getTemplate()->getMoveTargetMultiplierMax();
    }

    // sprint move blender
    if ( isPerformingUTurn() && ( m_currentState != &m_stateJumping || !m_characterPhys->getStickOnWalls() ) &&
       ( !m_uTurnFlipped || !onGround ) )
    {
        // remove the sprint blend if we uturn, in the air all the time, on the ground only while we are not flipped
        removeBlend = btrue;
    }
    else if ( m_releaseSprintActive || m_moveToPoint )
    {
        removeBlend = btrue;
    }
    else
    {
        removeBlend = bfalse;
    }

    if ( removeBlend )
    {
        m_sprintMaxSpeedBlendTimer = 0.f;
    }
    else
    {
        if ( onGround )
        {
            bbool sprinting = isSprinting();

            if ( !m_wasOnGroundPrevFrame )
            {
                if ( trySprint() )
                {
                    f32 xSpeed;

                    if ( m_characterPhys->getStickOnWalls() )
                    {
                        StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector);

                        if ( edgeOrientation != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
                        {
                            xSpeed = m_characterPhys->getSpeed().dot(currentEdge->m_normalizedVector);
                        }
                        else
                        {
                            xSpeed = m_characterPhys->getSpeed().dot(m_currentGravityDir.getPerpendicular());
                        }
                    }
                    else
                    {
                        xSpeed = m_characterPhys->getSpeed().dot(m_currentGravityDir.getPerpendicular());
                    }

                    f32 maxSpeed = getTemplate()->getEfficiencyMaxSpeed() * getTemplate()->getSprintEfficiencyMultiplier();
                    f32 t = f32_Abs(xSpeed) / maxSpeed;

                    m_sprintMaxSpeedBlendTimer = getTemplate()->getSprintMaxSpeedBlendDuration() * t;
                    sprinting = btrue;
                }
            }

            if ( sprinting )
            {
                m_sprintMaxSpeedBlendTimer = Min(m_sprintMaxSpeedBlendTimer+_deltaTime,getTemplate()->getSprintMaxSpeedBlendDuration());
            }
            else
            {
                m_sprintMaxSpeedBlendTimer = Max(m_sprintMaxSpeedBlendTimer-_deltaTime,0.f);
            }
        }
    }
}

// If we press left or right, memorize it so that we end up looking in that direction
void Ray_PlayerControllerComponent::UpdateDesiredLookDir()
{
    // While hanging we don't update the look direction because the player can turn his head
    if ( isHanging() ||
         m_currentState == &m_stateWallSlide ||
         isPerformingUTurn() ||
         m_walkOnWallKeepDirFromSpeed )
    {
        return;
    }

    if ( !getMoveData().getMoveSameDir() )
    {
        EMoveDir moveDirection = getMoveData().getMoveDirection();

        if ( moveDirection == MOVEDIR_LEFT ||
             moveDirection == MOVEDIR_RIGHT )
        {
            m_desiredLookDir = moveDirection;
        }
    }
}

// Try to end up looking in the direction where we were trying to look
void Ray_PlayerControllerComponent::setDesiredLookDir()
{
    if ( m_walkOnWallKeepDirFromSpeed )
    {
        return;
    }

    if ( m_desiredLookDir != MOVEDIR_NONE && !isPerformingUTurn() )
    {
        EUturnType uturnType;

        if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID )
        {
            uturnType = UTURNTYPE_AIR;
        }
        else
        {
            uturnType = UTURNTYPE_GROUND;
        }

        if ( m_desiredLookDir == MOVEDIR_RIGHT )
        {
            if ( m_actor->isFlipped() )
            {
                startUTurn(uturnType);
            }
        }
        else if ( m_desiredLookDir == MOVEDIR_LEFT )
        {
            if ( !m_actor->isFlipped() )
            {
                startUTurn(uturnType);
            }
        }
    }
}

void Ray_PlayerControllerComponent::updateLookRightFromMoveDirection( EMoveDir _moveDir )
{
    if ( _moveDir == MOVEDIR_RIGHT )
    {
        setLookRight(btrue);
    }
    else if ( _moveDir == MOVEDIR_LEFT )
    {
        setLookRight(bfalse);
    }

    resetDesiredLookDir();
}

f32 Ray_PlayerControllerComponent::getWalkMaxSpeed() const
{
    f32 speed = getTemplate()->getEfficiencyMaxSpeed();

    if ( getTemplate()->getSprintMaxSpeedBlendDuration() && m_sprintMaxSpeedBlendTimer )
    {
        f32 t = m_sprintMaxSpeedBlendTimer/getTemplate()->getSprintMaxSpeedBlendDuration();
        speed *= Interpolate(1.f,getTemplate()->getSprintEfficiencyMultiplier(),t);
    }

    return speed;
}

void Ray_PlayerControllerComponent::resetAchievedSpeedFactor()
{
    m_targetAchievedSpeedFactor = getTemplate()->getAchievedSpeedFactorDefault();
    m_achievedSpeedFactor = getTemplate()->getAchievedSpeedFactorDefault();
    m_achievedSpeedFactorV = 0.f;
    m_achievedSpeedCalculationTimer = s_achievedSpeedCalculationDelay;
}

void Ray_PlayerControllerComponent::UpdatePhysicWalk( f32 _deltaTime )
{
    calculateEfficiency();

    Vec2d moveInput = m_moveInput;
    EMoveDir moveDirection = getMoveData().getMoveDirection();
    f32 moveSize = getMoveData().getMove().norm() ? 1.f : 0.f;
    bbool moveSameDir = getMoveData().getMoveSameDir();
    Vec2d groundDir = m_characterPhys->calculateGroundDir();

    if ( m_characterPhys->getEdgeOrientationType(groundDir) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND ||
         m_walkOnWallKeepDirFromSpeed )
    {
        if ( m_characterPhys->getSpeed() != Vec2d::Zero )
        {
            if ( m_characterPhys->getSpeed().dot(groundDir) > 0.f )
            {
                moveDirection = MOVEDIR_RIGHT;
            }
            else
            {
                moveDirection = MOVEDIR_LEFT;
            }
        }
        else
        {
            Vec2d lookDir = AIUtils::getLookDir(m_actor);
            f32 dotProd = groundDir.dot(lookDir);

            if ( dotProd > 0.f )
            {
                moveDirection = MOVEDIR_RIGHT;
            }
            else
            {
                moveDirection = MOVEDIR_LEFT;
            }
        }

        moveSize = 1.f;
        moveSameDir = btrue;
        m_moveInput.m_x = 1.f;
    }

    if ( isPerformingSlide() ||
       ( moveDirection != MOVEDIR_LEFT &&
         moveDirection != MOVEDIR_RIGHT ) )
    {
        updateSprintRelease(_deltaTime);
        resetAchievedSpeedFactor();
        moveInput.m_x = 0.f;
        setMoveInput(moveInput);
        m_characterPhys->addForce(m_currentTargetForce);
        return;
    }

    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

    if ( edge )
    {
        Vec2d worldTargetSpeed;

        moveInput.m_x = moveSize;

        // Check in which direction of the edge we are moving
        f32 moveSign = moveDirection == MOVEDIR_RIGHT ? 1.f : -1.f;
        f32 walkForce = getTemplate()->getWalkForce();

        worldTargetSpeed = groundDir * moveSign * moveSize * walkForce;

        // If we keep the stick in the same direction, move along the edge in the same direction
        if ( !moveSameDir )
        {
            bbool lookRight = moveSign >= 0.f;

            if ( !isPerformingUTurn() )
            {
                if ( !m_actor->isFlipped() != lookRight )
                {
                    if ( isSprinting() )
                    {
                        m_currentMoveTargetMultiplier = 0.f;
                    }

                    startUTurn(UTURNTYPE_GROUND);
                }
            }
            else
            {
                // If we are trying to uturn while doing a uturn, then reset and continue doing it
                if ( !m_uTurnFlipped )
                {
                    if ( !m_actor->isFlipped() == lookRight )
                    {
                        resetUTurn();
                        m_animComponent->resetCurTime();
                        startUTurn(UTURNTYPE_GROUND);
                    }
                    else
                    {
                        // we are already trying to turn towards where we want, so do nothing
                    }
                }
                else
                {
                    if ( !m_actor->isFlipped() != lookRight )
                    {
                        resetUTurn();
                        startUTurn(UTURNTYPE_GROUND);
                        m_animComponent->resetTree();
                    }
                }
            }
        }

        calculateAchievedSpeedFactor(worldTargetSpeed,_deltaTime);
        updateAchievedSpeedFactor(_deltaTime);

        // basic force
        m_currentTargetForce = worldTargetSpeed;
        // multiplied by engine force
        m_currentTargetForce *= m_currentMoveTargetMultiplier;
        // multiplied by efficiency
        m_currentTargetForce *= m_currentEfficiency;

        if ( isSprinting() )
        {
            m_currentTargetForce *= getTemplate()->getSprintForceMultiplier();
        }
        else if (isSupporting())
        {
            m_currentTargetForce *= getTemplate()->getPedestalWalkMultiplier();
        }

        updateSprintRelease(_deltaTime);

        m_characterPhys->addForce(m_currentTargetForce);

        setMoveInput(moveInput);
    }

    if ( isPerformingUTurn() )
    {
        m_move.resetSameDir();
    }
    else
    {
        updateLookRightFromMoveDirection(moveDirection);
    }

    if ( !m_releaseSprintActive && m_wasOnGroundPrevFrame )
    {
        m_releaseSprintForce = Min(m_currentTargetForce.norm(),getTemplate()->getSprintReleaseForceMax());
        m_releaseSprintForceSign = m_currentTargetForce.dot(m_characterPhys->getCurrentGroundDir()) > 0.f ? 1.f : -1.f;
    }
}

void Ray_PlayerControllerComponent::updateSprintRelease( f32 _dt )
{
    if ( m_releaseSprintActive )
    {
        m_releaseSprintForce *= getTemplate()->getSprintReleaseForceMultiplier();

        if ( m_releaseSprintForce <= 10.f ||
             m_characterPhys->getSpeed().IsEqual(Vec2d::Zero,0.1f) )
        {
            m_releaseSprintActive = bfalse;
            m_releaseSprintForce = 0.f;
        }

        m_currentTargetForce = m_characterPhys->getCurrentGroundDir() * m_releaseSprintForce * m_releaseSprintForceSign;
        m_sprintMaxSpeedBlendTimer = 0.f;
    }
    else if ( m_wasOnGroundPrevFrame )
    {
        const PolyLineEdge* edge = m_characterPhys->getStickedEdge();
        bbool walkOnGround = edge && m_characterPhys->getEdgeOrientationType(edge->m_normalizedVector) == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND;

        bbool releaseSprint = walkOnGround &&
                             ( ( isPerformingUTurn() && !m_uTurnFlipped ) ||
                               getMoveData().getMoveDirection() == MOVEDIR_NONE );

        if ( m_releaseSprintWasSprinting && releaseSprint && !isPerformingSlide() )
        {
            m_releaseSprintActive = btrue;
        }
    }

    m_releaseSprintWasSprinting = isSprinting() &&
                                 !isPerformingUTurn() &&
                                 ( getMoveData().getMoveDirection() == MOVEDIR_LEFT || getMoveData().getMoveDirection() == MOVEDIR_RIGHT );
}

bbool Ray_PlayerControllerComponent::tryStepMiniJump() const
{
    // If rayman is stuck because he is hitting a small corner, try to detect if it's a small step and jump over
    // it

    const PhysContactsContainer& contacts = m_characterPhys->getContacts();

    if ( contacts.size() < 3 )
    {
        // We need to have at least 3 contacts:
        // - the current sticked edge
        // - one edge from the corner
        // - the other edge of the corner
        return bfalse;
    }

    PolyLine* horizontalPoly = NULL;
    const PolyLineEdge* horizontalEdge = NULL;
    PolyLine* poly;
    const PolyLineEdge* edge;
    f32 radius = m_characterPhys->getRadius();
    u32 stickedEdge = m_characterPhys->getStickedEdgeIndex();
    ObjectRef stickedPolyline = m_characterPhys->getStickedPolylineRef();
    f32 miniJumpStepDist = getTemplate()->getWalkMiniJumpMaxStep() * m_actor->getScale().m_y;

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID ||
             contact.m_edgeIndex == stickedEdge &&
             contact.m_collidableUserData == stickedPolyline )
        {
            // No need to do anything with the current edge
            continue;
        }

        AIUtils::getPolyLine(contact.m_collidableUserData,contact.m_edgeIndex,
                             poly,edge);

        if ( !poly || !edge )
        {
            continue;
        }

        f32 dotProd = f32_Abs(contact.m_normal.dot(m_currentGravityDir));

        if ( dotProd < 0.3f || dotProd > 0.6f )
        {
            continue;
        }

        Vec2d contactLocal = contact.m_contactPoint - m_actor->get2DPos();

        contactLocal = contactLocal.Rotate(-m_actor->getAngle());
        contactLocal.m_y += radius;

        if ( contactLocal.m_y > miniJumpStepDist )
        {
            // If the step is too high, then pass
            continue;
        }

        horizontalPoly = poly;
        horizontalEdge = edge;

        break;
    }

    if ( !horizontalEdge )
    {
        // if we couldn't find a horizontal edge don't try to jump
        return bfalse;
    }

    m_characterPhys->addForce(-m_currentGravityDir*getTemplate()->getWalkMiniJumpForce());

    return btrue;
}

void Ray_PlayerControllerComponent::calculateAchievedSpeedFactor( const Vec2d& _currentTarget,
                                                                  f32 _deltaTime )
{
    if ( isPerformingUTurn() )
    {
        resetAchievedSpeedFactor();
        return;
    }

    Vec2d speedDir = m_characterPhys->getSpeed();

    speedDir.normalize();

    bbool sameDir;

    if ( _currentTarget == Vec2d::Zero )
    {
        Vec2d myDir = Vec2d::Right.Rotate(m_actor->getAngle());

        if ( m_actor->isFlipped() )
        {
            myDir *= -1.f;
        }

        sameDir = myDir.dot(speedDir) > 0.f;
    }
    else
    {
        Vec2d forceDir = _currentTarget;

        forceDir.normalize();

        sameDir = speedDir.dot(forceDir) > 0.f;
    }

    f32 moveSize = m_moveInput.m_x ? 1.f : 0.f;
    f32 desiredSpeed = moveSize * getWalkMaxSpeed() * m_currentMoveTargetMultiplier;
    f32 currentSpeed = f32_Abs(m_characterPhys->calculateGroundDir().dot(m_characterPhys->getSpeed()));
    f32 speedDif;

    if ( sameDir )
    {
        speedDif = desiredSpeed - currentSpeed;
    }
    else
    {
        speedDif = currentSpeed + desiredSpeed;
    }

    if ( m_achievedSpeedCalculationTimer )
    {
        m_achievedSpeedCalculationTimer = Max(m_achievedSpeedCalculationTimer-_deltaTime,0.f);
    }
    else
    {
        if ( speedDif > 0.f )
        {
            // We want to go faster than what we are going. We are not very efficient
            m_targetAchievedSpeedFactor = 1.f - (speedDif / getWalkMaxSpeed());
            m_targetAchievedSpeedFactor = Clamp(m_targetAchievedSpeedFactor,0.f,1.f);
        }
        else
        {
            // We want to go slower than what we are going. We are over-efficient
            m_targetAchievedSpeedFactor = 1.f + f32_Abs(speedDif / getWalkMaxSpeed());
            m_targetAchievedSpeedFactor = Clamp(m_targetAchievedSpeedFactor,1.f,2.f);
        }
    }
}

void Ray_PlayerControllerComponent::updateAchievedSpeedFactor( f32 _deltaTime )
{
    f32 dif = m_targetAchievedSpeedFactor - m_achievedSpeedFactor;
    f32 f = ( dif * getTemplate()->getAchievedSpeedFactorS() ) - ( m_achievedSpeedFactorV * getTemplate()->getAchievedSpeedFactorD() );

    m_achievedSpeedFactorV += f * _deltaTime;
    m_achievedSpeedFactor += m_achievedSpeedFactorV * _deltaTime;
}

void Ray_PlayerControllerComponent::UpdatePhysicAir( bbool _allowMagnetAttraction )
{
    if ( m_airControlDisableDelay && !m_airControlDisableBlend )
    {
        EMoveDir moveDirection = getMoveData().getMoveDirection();

        if ( !m_fallFromRoof && ( moveDirection == MOVEDIR_LEFT || moveDirection == MOVEDIR_RIGHT ) )
        {
            Vec2d gravXaxis = m_currentGravityDir.getPerpendicular();
            f32 moveSize = gravXaxis.dot(getMoveData().getMove());
            f32 moveDir = moveSize > 0.f ? 1.f : ( moveSize < 0.f ? -1.f : 0.f );

            if ( !isPerformingUTurn() && moveDir )
            {
                if ( m_currentState == &m_stateJumping ||
                     m_currentState == &m_stateFalling )
                {
                    bbool lookRight = moveDir > 0.f;

                    if ( !m_actor->isFlipped() != lookRight )
                    {
                        startUTurn(UTURNTYPE_AIR);
                    }
                }
            }
        }

        m_characterPhys->addForce( getMagnetForce(_allowMagnetAttraction) );
        return;
    }

    f32 forceMultiplier = 1.f;
    f32 airForce;

    if ( m_airControlDisableDelay && m_airControlDisableDelay < m_airControlDisableTotal )
    {
        if (m_airControlDisableBlend)
        {
            const f32 t = m_airControlDisableDelay / m_airControlDisableTotal;
            forceMultiplier = Interpolate(m_airControlDisableAmount, 1.f, 1.f - t);
            //DebugDraw::text("air control: %.0f%%", forceMultiplier * 100);
        }
        else
        {
            f32 t = m_airControlDisableDelay / m_airControlDisableTotal;
            forceMultiplier = 1.f - t;
        }
    }

    if ( m_wallSlideAirControl )
    {
        airForce = getTemplate()->getAirForceWallJump();
    }
    else
    {
        airForce = getTemplate()->getAirForce();
    }

    airForce *= forceMultiplier;

    EMoveDir moveDirection = getMoveData().getMoveDirection();

    if ( moveDirection == MOVEDIR_NONE )
    {
        if ( m_airApplyBrake )
        {
            // If we don't push the stick we apply a force opposite to our speed and proportional to it
            Vec2d forceDir = m_currentGravityDir.getPerpendicular();
            f32 xSpeed = forceDir.dot(m_characterPhys->getSpeed());

            if ( xSpeed )
            {
                m_currentTargetForce = forceDir*-xSpeed*getTemplate()->getAirNoMoveFrictionX();
            }
        }
    }
    else if ( moveDirection == MOVEDIR_LEFT || moveDirection == MOVEDIR_RIGHT )
    {
        // We can only have air control if we are not blocked by anything
        Vec2d gravXaxis = m_currentGravityDir.getPerpendicular();
        f32 moveSize = gravXaxis.dot(getMoveData().getMove());
        f32 moveDir = moveSize > 0.f ? 1.f : ( moveSize < 0.f ? -1.f : 0.f );

        const PhysContactsContainer& contacts = m_characterPhys->getContacts();

        for ( u32 index = 0; index < contacts.size(); index++ )
        {
            const SCollidableContact& contact = contacts[index];

            if ( contact.m_edgeIndex == U32_INVALID )
            {
                continue;
            }

            Vec2d contactLocal = contact.m_contactPoint - m_actor->get2DPos();

            contactLocal = contactLocal.Rotate(-m_actor->getAngle());

            if ( ( moveDirection == MOVEDIR_RIGHT && moveSize > 0.f && contactLocal.m_x > 0.f ) ||
                 ( moveDirection == MOVEDIR_LEFT && moveSize < 0.f && contactLocal.m_x < 0.f ) )
            {
                moveSize = 0.f;
                break;
            }
        }

        // In air our force will be proportionally inverted to the top speed in air
        // The faster we go horizontally the less force we will be able to apply
        Vec2d speedDir = m_characterPhys->getSpeed();

        speedDir.normalize();

        Vec2d move = gravXaxis*moveSize;
        bbool sameDir = move.dot(speedDir) >= 0.f;
        f32 currentSpeed = Min(f32_Abs(gravXaxis.dot(m_characterPhys->getSpeed())),getTemplate()->getAirTopXSpeed());

        if ( sameDir )
        {
            m_currentTargetForce = move * (airForce*(1.f-(currentSpeed/getTemplate()->getAirTopXSpeed())));
        }
        // If we are trying to move in the opposite direction then we apply the force normally
        else
        {
            m_currentTargetForce = move * airForce;
        }

        if ( !isPerformingUTurn() && moveDir && !m_fallFromRoof )
        {
            if ( m_currentState == &m_stateJumping ||
                 m_currentState == &m_stateFalling )
            {
                bbool lookRight = moveDir > 0.f;

                if ( !m_actor->isFlipped() != lookRight )
                {
                    if ( isSupportHolding() )
                    {
                        setLookRight(lookRight);
                    }
                    else
                    {
                        startUTurn(UTURNTYPE_AIR);
                    }
                }
            }
        }
    }

    m_characterPhys->addForce(m_currentTargetForce + getMagnetForce(_allowMagnetAttraction) );
}

void Ray_PlayerControllerComponent::preparePhysicAirHit()
{
    if ( !m_airHitInProgress && m_airHitPrepared )
    {
        m_airHitForceApplicationTimer = getTemplate()->getAirHitForceApplicationTime();
        setGravityMode(GRAVITYMODE_AIRHIT,getTemplate()->getAirHitGravityMultiplier());

        if ( isSizeReduced() )
        {
            m_characterPhys->setSpeedMultiplier(getTemplate()->getAirHitSpeedMultiplierReduced());
        }
        else
        {
            m_characterPhys->setSpeedMultiplier(getTemplate()->getAirHitSpeedMultiplier());
        }

        m_airHitInProgress = btrue;
        m_airHitPrepared = bfalse;
    }
}

void Ray_PlayerControllerComponent::cancelPhysicAirHit()
{
    if ( m_airHitInProgress )
    {
        resetGravityMode(GRAVITYMODE_AIRHIT);
        m_characterPhys->setSpeedMultiplier(Vec2d::One);
        m_airHitInProgress = bfalse;
    }
}

void Ray_PlayerControllerComponent::resetPhysicAirHit()
{
    cancelPhysicAirHit();
    m_airHitPrepared = btrue;
}

void Ray_PlayerControllerComponent::UpdatePhysicCrushHit( f32 _dt )
{
    if ( m_crushHitSuspensionCounter )
    {
        m_crushHitSuspensionCounter = Max(m_crushHitSuspensionCounter-_dt,0.f);

        f32 ySpeed = m_currentGravityDir.dot(m_characterPhys->getSpeed());

        if ( !m_crushHitSuspensionCounter || ySpeed > 0.f )
        {
            // Apply extra force down
            Vec2d force = m_currentGravityDir * getTemplate()->getCrushHitDownForce();
            Vec2d gravityNormal = m_currentGravityDir.getPerpendicular();
            f32 xSpeed = gravityNormal.dot(m_characterPhys->getSpeed());
            Vec2d speed = gravityNormal * xSpeed;
            m_characterPhys->setSpeed(speed);
            m_characterPhys->addForce(force);
            m_crushHitSuspensionCounter = 0.f;
        }
    }
}

void Ray_PlayerControllerComponent::UpdatePhysicHairlicopter( f32 _dt )
{
    Vec2d worldDir = m_airControlDisableDelay && !m_airControlDisableBlend ? Vec2d::Zero : getMoveData().getMove();
    Vec2d speedDir = m_characterPhys->getSpeed();
    Vec2d gravXaxis = m_currentGravityDir.getPerpendicular();
    f32 moveSize = gravXaxis.dot(worldDir);
    Vec2d move = gravXaxis*moveSize;

    worldDir.normalize();
    speedDir.normalize();

    const Vec2d& currentSpeed = m_characterPhys->getSpeed();
    f32 speedProjectedX = gravXaxis.dot(currentSpeed);
    f32 speedProjectedY = -m_currentGravityDir.dot(currentSpeed);

    //pushing in same direction as direction of flight
    bbool sameDir = move.dot(speedDir) >= 0.f;

    // X FORCE
    f32 currentSpeedX = f32_Abs(speedProjectedX);

    f32 xComponent = 0.0f;
    if (sameDir)
    {
        //ADD DRAG
        if (currentSpeedX > getTemplate()->getHelicopterMinDragSpeedX())
        {
            xComponent = (speedProjectedX>0.0f?-1.0f:1.0f) * getTemplate()->getHelicopterDragForceX() * (currentSpeedX -getTemplate()->getHelicopterMinDragSpeedX());
        }
        //AIR CONTROL
        else
        {
            xComponent += moveSize * (getTemplate()->getHelicopterForceX()*(1.f-(currentSpeedX/getTemplate()->getHelicopterMinDragSpeedX())));
        }
    }
    else
    {
        //ADD DRAG
        if (currentSpeedX > getTemplate()->getHelicopterMinDragSpeedX())
        {
            xComponent = (speedProjectedX>0.0f?-1.0f:1.0f) * getTemplate()->getHelicopterDragForceX() * (currentSpeedX -getTemplate()->getHelicopterMinDragSpeedX());
        }
        //AIR CONTROL
        xComponent += moveSize * getTemplate()->getHelicopterForceX();
    }

    if ( !isPerformingUTurn() )
    {
        Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
        bbool uTurn = lookDir.dot(getMoveData().getMove()) < 0.f;

        if ( uTurn )
        {
            startUTurn(UTURNTYPE_AIR);
        }
    }

    if ( m_airControlDisableDelay && m_airControlDisableDelay < m_airControlDisableTotal && m_airControlDisableBlend )
    {
        const f32 t = m_airControlDisableDelay / m_airControlDisableTotal;
        const f32 control = Interpolate(m_airControlDisableAmount, 1.f, 1.f - t);
        //DebugDraw::text("air control: %.0f%%", control * 100);
        xComponent *= control;
    }

    m_currentTargetForce = gravXaxis * xComponent;

    m_characterPhys->addForce(m_currentTargetForce);

    //// Y FORCE
    if ( speedProjectedY <= 0.f )
    {
        // Going down
        f32 speed = -speedProjectedY;

        if ( m_helicopterSuspensionTimer )
        {
            // During a timer we apply suspension based on our current Y speed
            f32 force = (speed*getTemplate()->getHelicopterSuspensionMultiplier())/LOGICDT;
            m_characterPhys->addForce(Vec2d(0.f,force));
        }

        if ( speed >= getTemplate()->getHelicopterMaxSpeedDown() )
        {
            // If we go faster than our max speed, remove the gravity and apply a force going up to counter the speed
            // This force is proportional to the difference between our current speed and the max speed we should have
            m_characterPhys->setGravityMultiplier(0.f);

            f32 speedDif = (speed - getTemplate()->getHelicopterMaxSpeedDown());
            f32 force = (speedDif*0.1f)/LOGICDT;
            m_characterPhys->addForce(Vec2d(0.f,force));
        }
        else
        {
            // If the speed is somewhere in between, we scale the gravity based on how fast we are going down (the more speed, the less gravity)
            f32 t = 1.f - ( speed / getTemplate()->getHelicopterMaxSpeedDown() );

            m_characterPhys->setGravityMultiplier(t*getTemplate()->getHelicopterGravityMultiplier());
        }
    }
    else
    {
        // Going up
        m_characterPhys->setGravityMultiplier(1.f);
    }

    if ( !isPerformingUTurn() )
    {
        updateLookRightFromMoveDirection(getMoveData().getMoveDirection());
    }

    m_helicopterSuspensionTimer = Max(m_helicopterSuspensionTimer-_dt,0.f);

    helicoProcessWind();
}

void Ray_PlayerControllerComponent::helicoProcessWind()
{
    f32 ySpeed = (-m_currentGravityDir).dot(m_characterPhys->getSpeed());
    bbool noExtraWind = ySpeed >= getTemplate()->getHelicopterMaxSpeedUp();

    PhysActorForceArray windForces;
    Vec2d totalWindForce = Vec2d::Zero;
    Vec2d speedMultiplierFromExternalForces = Vec2d::One;
    Vec2d windDir;

    PHYSWORLD->getWindForces(m_actor->get2DPos(), m_characterPhys->getSpeed(), m_actor->getDepth(), windForces);

    for ( u32 i = 0; i < windForces.size(); i++ )
    {
        PhysActorForce& wind = windForces[i];

        if ( m_actor->getBinding()->isDescendant(wind.m_actor) )
        {
            continue;
        }

        windDir = wind.m_force;
        windDir.normalize();

        // helico gets an extra boost from upwards forces
        if ( !noExtraWind )
        {
            f32 dotProd = (-m_currentGravityDir).dot(windDir);
            f32 angle = f32_ACos(dotProd);

            if ( angle <= getTemplate()->getHelicopterWindForcesAngle().ToRadians() )
            {
                totalWindForce += wind.m_force * getTemplate()->getHelicopterWindForcesMultiplier();
            }
        }

        // helico gets extra friction from lateral forces
        {
            f32 dotProd = (m_currentGravityDir.getPerpendicular()).dot(windDir);
            f32 angle = f32_ACos(fabsf(dotProd));

            if ( angle <= getTemplate()->getHelicopterWindSpeedAngle().ToRadians() )
            {
                speedMultiplierFromExternalForces *= wind.m_speedMultiplier * getTemplate()->getHelicopterWindSpeedMultiplier();
            }
        }
    }

    m_characterPhys->addForce(totalWindForce);

    Vec2d speed = m_characterPhys->getSpeed();
    speed *= speedMultiplierFromExternalForces;

    if ( speed.m_y > getTemplate()->getHelicopterMaxSpeedUp() )
    {
        speed.m_y *= 0.9f;
    }

    m_characterPhys->setSpeed(speed);
}

void Ray_PlayerControllerComponent::updateClimbSwingImpulse( const PolyLine* _polyLine, Vec2d& _moveInput )
{
    f32 impulse;

    if ( m_coopActionMode == ACTIONCOOPMODE_MAIN )
    {
        impulse = 0.f;
    }
    else
    {
        if ( isSizeReduced() )
        {
            impulse = getMoveData().getMove().norm() * getTemplate()->getClimbEdgeSwingImpulseReduced();
        }
        else
        {
            impulse = getMoveData().getMove().norm() * getTemplate()->getClimbEdgeSwingImpulse();
        }
    }

    if ( getMoveData().getMoveDirection() == MOVEDIR_LEFT )
    {
        impulse *= -1.f;

        if ( !m_actor->isFlipped() )
        {
            _moveInput.m_x = -1.f;
        }
        else
        {
            _moveInput.m_x = 1.f;
        }
    }
    else if ( getMoveData().getMoveDirection() == MOVEDIR_RIGHT )
    {
        if ( !m_actor->isFlipped() )
        {
            _moveInput.m_x = 1.f;
        }
        else
        {
            _moveInput.m_x = -1.f;
        }
    }
    else
    {
        _moveInput.m_x = 0.f;
        impulse = 0.f;
    }

    if ( impulse )
    {
        Vec2d impulseDir = m_currentGravityDir.getPerpendicular();

        EventRopeSwingImpulse eventImpulse(m_actor->getRef(),_polyLine->getRef(),m_climbingEdge,
            m_climbingT,m_characterPhys->getWeight(),
            impulseDir*impulse,getCharacterSize());

        _polyLine->getOwnerActor()->onEvent(&eventImpulse);
    }
}

void Ray_PlayerControllerComponent::setHangState( struct HangingInfo& _info )
{
    m_hangInfo = _info;

    if ( _info.m_climb )
    {
        changeState(&m_stateClimbing);
    }
    else
    {
        changeState(&m_stateHanging);
    }
}

// With this we try to hang or climb a polyline while we are in the air
bbool Ray_PlayerControllerComponent::tryHanging( HangingInfo & _info ) const
{
    if ( isCameraLimited() )
    {
        return bfalse;
    }

    Vec2d playerDir = Vec2d::Right.Rotate(m_actor->getAngle());

    if ( !m_currentJumpWallRun )
    {
        // Only try to hang when the player is quite horizontal with gravity
        f32 dotGrav = m_currentGravityDir.getPerpendicular().dot(playerDir);

        if ( dotGrav < 0.5f )
        {
            return bfalse;
        }
    }

    PhysContactsContainer contacts;

    const Vec2d& pos2D = m_actor->get2DPos();
    Vec2d upAxis = playerDir.getPerpendicular();
    Vec2d feetPos = pos2D - (upAxis*m_characterPhys->getRadius());
    Vec2d headPos = feetPos + (upAxis*calculateHeight());
    PhysShapeCircle shape(getTemplate()->getHangRadiusCheck()*m_actor->getScale().m_y);

#ifndef ITF_FINAL
    if ( m_displayHangDebug )
    {
        GFX_ADAPTER->drawDBGCircle(headPos.m_x,headPos.m_y,0.2f,0.f,0.f,1.f);
        GFX_ADAPTER->drawDBGCircle(headPos.m_x,headPos.m_y,shape.getRadius(),1.f,1.f,0.f);
    }
#endif // ITF_FINAL

    // First check the environment to see if we can hang on something solid
    PHYSWORLD->checkEncroachment(headPos,headPos,m_actor->getAngle(),&shape,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

    PolyLine* poly;
    const PolyLineEdge* edge;

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& c = contacts[i];

        if ( c.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

        if ( poly && edge && processContactHangSide(c,poly,*edge,c.m_edgeIndex,headPos,_info) )
        {
            return btrue;
        }
    }

    contacts.clear();

    // If there is no polyline to grab in the environment maybe we can grab a phantom, polylines that we don't
    // collide with (like ropes), or actors
    PHYSWORLD->collidePhantoms(headPos,headPos,m_actor->getAngle(),&shape,m_actor->getDepth(),(ECollisionFilter)(ECOLLISIONFILTER_CHARACTERS|ECOLLISIONFILTER_ENVIRONMENT),contacts);

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& c = contacts[i];

        if ( c.m_edgeIndex == U32_INVALID )
        {
            Actor* victim = AIUtils::getActor(c.m_collidableUserData);

            if ( victim && processContactClimbActor(victim,headPos,_info) )
            {
                return btrue;
            }
        }
        else
        {
            AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

            if ( poly && edge && processContactClimbPolyline(c,poly,*edge,headPos,_info) )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::calculateHangPosCheck( PolyLine* _polyLine,
                                                           const PolyLineEdge& _edge, u32 _edgeIndex,
                                                           bbool _rightSide, Vec2d& _pos, f32& _angle ) const
{
    PolyLine* nextPoly;
    u32 nextEdgeIndex;

    AIUtils::getAdjacentEdge(_polyLine,_edgeIndex,_rightSide,nextPoly,nextEdgeIndex);

    if ( !_rightSide )
    {
        if ( nextEdgeIndex == U32_INVALID )
        {
            _angle = getVec2Angle(_edge.m_normalizedVector);
        }
        else
        {
            const PolyLineEdge& prevEdge = nextPoly->getEdgeAt(nextEdgeIndex);
            _angle = getVec2Angle(-prevEdge.m_normalizedVector.getPerpendicular());
        }
    }
    else
    {
        if ( nextEdgeIndex == U32_INVALID )
        {
            _angle = getVec2Angle(_edge.m_normalizedVector);
        }
        else
        {
            const PolyLineEdge& nextEdge = nextPoly->getEdgeAt(nextEdgeIndex);
            _angle = getVec2Angle(nextEdge.m_normalizedVector.getPerpendicular());
        }
    }

    Vec2d dir = Vec2d::Right.Rotate(_angle);
    f32 scaleY = m_actor->getScale().m_y;

    if ( !_rightSide )
    {
        _pos = _edge.getPos();
        _pos -= dir * getTemplate()->getHangingCollisionCheckOffsetSolid().m_x * scaleY;
    }
    else
    {
        _pos = _edge.getPos() + _edge.m_vector;
        _pos += dir * getTemplate()->getHangingCollisionCheckOffsetSolid().m_x * scaleY;
    }

    _pos -= dir.getPerpendicular() * getTemplate()->getHangingCollisionCheckOffsetSolid().m_y * scaleY;
}

bbool Ray_PlayerControllerComponent::isEdgeDirValidToHang( PolyLine* _poly,
                                                           u32 _edgeIndex,
                                                           const PolyLineEdge& _edge,
                                                           bbool _right ) const
{
    PolyLine* nextPoly;
    u32 nextEdgeIndex;

    AIUtils::getAdjacentEdge(_poly,_edgeIndex,_right,nextPoly,nextEdgeIndex);

    if ( !nextPoly || nextEdgeIndex == U32_INVALID )
    {
        return bfalse;
    }

    const PolyLineEdge& nextEdge = nextPoly->getEdgeAt(nextEdgeIndex);

    if ( m_characterPhys->getEdgeOrientationType(_edge.m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND ||
         m_characterPhys->getEdgeOrientationType(nextEdge.m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
    {
        return bfalse;
    }

    Vec2d gravDir = m_currentGravityDir.getPerpendicular();
    Vec2d myDir = _edge.m_normalizedVector;
    f32 angle = f32_ACos(gravDir.dot(myDir));

    if ( angle > getTemplate()->getHangMaxGroundAngle().ToRadians() )
    {
        return bfalse;
    }

    return btrue;
}

// With this we test if we can put the character hanging on the corner from the polyline where he is standing on
bbool Ray_PlayerControllerComponent::tryHangFromPolyline( PolyLine* _poly, const PolyLineEdge* _edge,
                                                          u32 _edgeIndex, bbool _rightSide,
                                                          HangingInfo& _info )
{
    if ( isCameraLimited() )
    {
        return bfalse;
    }

    const Ray_GameMaterial_Template* mat = static_cast<const Ray_GameMaterial_Template*>(World::getGameMaterial(_edge->getGameMaterial()));

    if ( mat && !mat->getHangable() )
    {
        return bfalse;
    }

    if ( isValidPointForHanging(_poly,*_edge,_edgeIndex,_rightSide) )
    {
        Vec2d hangPoint;

        if ( _rightSide )
        {
            hangPoint = _edge->getPos() + _edge->m_vector;
        }
        else
        {
            hangPoint = _edge->getPos();
        }

        _info.m_hangPos = hangPoint;
        _info.m_polyline = _poly->getRef();
        _info.m_edge = _edgeIndex;
        _info.m_lookRight = !_rightSide;
        _info.m_climb = bfalse;
        _info.m_actor = ITF_INVALID_OBJREF;
        _info.m_coopActionMode = ACTIONCOOPMODE_NONE;
        return btrue;
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::processContactHangSide( const struct SCollidableContact& _contact,
                                                             PolyLine* _polyLine, const PolyLineEdge& _edge,
                                                             u32 _edgeIndex, const Vec2d& _headPos,
                                                             HangingInfo& _info ) const
{
    if ( _contact.m_collidableUserData == m_actor->getRef() )
        return bfalse;

    if ( m_hangActivationCounter )
    {
        if ( m_previousHangingPolyline == _contact.m_collidableUserData )
        {
            return bfalse;
        }
    }

    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(_edge.getGameMaterial()));

    if ( mat && ( mat->getDangerousLevel() > 0 || !mat->getHangable() ) )
    {
        return bfalse;
    }

    if ( !_polyLine->m_isCollisionActive )
    {
        return bfalse;
    }

    if ( m_stance != STANCE_WALLSLIDE )
    {
        f32 speedProj = m_currentGravityDir.dot(m_characterPhys->getSpeed());

        if ( speedProj < -getTemplate()->getHangMaxSpeed() )
        {
            return bfalse;
        }
    }

    // Set if we are hanging from the left or the right
    Vec2d rightPos = _edge.getPos() + _edge.m_vector;
    f32 headToLeftSq = ( _edge.getPos() - _headPos ).sqrnorm();
    f32 headToRightSq = ( rightPos - _headPos ).sqrnorm();
    Vec2d hangPoint;
    bbool rightSide;

    if ( headToLeftSq < headToRightSq )
    {
        hangPoint = _edge.getPos();
        rightSide = bfalse;
    }
    else
    {
        hangPoint = rightPos;
        rightSide = btrue;
    }

    // Now we check if the hang point is inside a rectangle because we want to have a big radius vertically
    // but a smaller radius horizontally
    Vec2d localHangPos = hangPoint;

    localHangPos -= m_actor->get2DPos();
    localHangPos = localHangPos.Rotate(-m_actor->getAngle());

    f32 scaleY = m_actor->getScale().m_y;
    Vec2d rectOffsetScaled = getTemplate()->getHangRectOffset() * scaleY;
    Vec2d hangRectSizeScaled = getTemplate()->getHangRectSize() * scaleY;
    bbool lookRight;

    if ( getMoveData().getMoveDirection() == MOVEDIR_RIGHT )
    {
        lookRight = btrue;
    }
    else if ( getMoveData().getMoveDirection() == MOVEDIR_LEFT )
    {
        lookRight = bfalse;
    }
    else
    {
        lookRight = isPerformingUTurn() ? m_actor->isFlipped() : !m_actor->isFlipped();
    }

    if ( !lookRight )
    {
        rectOffsetScaled.m_x *= -1.f;
    }

#ifndef ITF_FINAL
    if ( m_displayHangDebug )
    {
        Vec2d boxPos = rectOffsetScaled.Rotate(m_actor->getAngle()) + m_actor->get2DPos();

        GFX_ADAPTER->drawDBGBox(boxPos,m_actor->getAngle(),hangRectSizeScaled,1.f,0.f,1.f,1.f);
    }
#endif // ITF_FINAL

    if ( localHangPos.m_x < rectOffsetScaled.m_x - hangRectSizeScaled.m_x )
    {
        return bfalse;
    }

    if ( localHangPos.m_x > rectOffsetScaled.m_x + hangRectSizeScaled.m_x )
    {
        return bfalse;
    }

    if ( localHangPos.m_y < rectOffsetScaled.m_y - hangRectSizeScaled.m_y )
    {
        return bfalse;
    }

    if ( localHangPos.m_y > rectOffsetScaled.m_y + hangRectSizeScaled.m_y )
    {
        return bfalse;
    }

    if ( isValidPointForHanging(_polyLine,_edge,_contact.m_edgeIndex,rightSide) )
    {
         _info.m_hangPos = hangPoint;
        _info.m_polyline = _polyLine->getRef();
        _info.m_edge = _contact.m_edgeIndex;
        _info.m_lookRight = !rightSide;
        _info.m_climb = bfalse;
        _info.m_actor = ITF_INVALID_OBJREF;
        _info.m_coopActionMode = ACTIONCOOPMODE_NONE;
        return btrue;
    }

    return bfalse;
}

// This function checks if a specific point is valid for hanging
bbool Ray_PlayerControllerComponent::isValidPointForHanging( PolyLine* _poly,
                                                             const PolyLineEdge& _edge,
                                                             u32 _edgeIndex,
                                                             bbool _right ) const
{
    // The platform must be at a certain angle with the gravity
    if ( !isEdgeDirValidToHang(_poly,_edgeIndex,_edge,_right) )
    {
        return bfalse;
    }

    // Make sure there is a certain angle with the next edge so that we don't hang in the middle of a flat ground
    PolyLine* nextPoly;
    u32 nextEdgeIndex;

    AIUtils::getAdjacentEdge(_poly,_edgeIndex,_right,nextPoly,nextEdgeIndex);

    if ( nextEdgeIndex != U32_INVALID )
    {
        const PolyLineEdge& edge = _poly->getEdgeAt(_edgeIndex);
        const PolyLineEdge& nextEdge = nextPoly->getEdgeAt(nextEdgeIndex);
        f32 crossP;

        if ( _right )
        {
            crossP = edge.m_normalizedVector.cross(nextEdge.m_normalizedVector);
        }
        else
        {
            crossP = nextEdge.m_normalizedVector.cross(edge.m_normalizedVector);
        }

        f32 dotP = edge.m_normalizedVector.dot(nextEdge.m_normalizedVector);

        if ( crossP > 0.f )
        {
            return bfalse;
        }

        if ( dotP > 0.8f )
        {
            return bfalse;
        }
    }
    else
    {
        return bfalse;
    }

    Vec2d hangPointTest;
    f32 hangAngleTest;

    calculateHangPosCheck(_poly,_edge,_edgeIndex,_right,hangPointTest,hangAngleTest);

    if ( isHangingPosColliding(hangPointTest,hangAngleTest,bfalse) )
    {
        return bfalse;
    }

    return btrue;
}

void Ray_PlayerControllerComponent::getHangCoopCollisionData( Actor* supporter, Vec2d& _collisionPos, f32& _angle, Vec2d& _snapPos, bbool& _lookRight ) const
{
    EventQuerySnapData snap;

    supporter->onEvent(&snap);

    _snapPos = _collisionPos = snap.getPos().truncateTo2D();
    _angle = snap.getAngle();
    _lookRight = snap.getLookRight();

    Vec2d dir;
    Vec2d norm;

    if ( !_lookRight )
    {
        f32 angleLocal = _angle - supporter->getAngle();
        dir = Vec2d::Right.Rotate(angleLocal);
        dir.m_x *= -1.f;
        dir = dir.Rotate(supporter->getAngle());
        norm = -dir.getPerpendicular();
    }
    else
    {
        dir = Vec2d::Right.Rotate(_angle);
        norm = dir.getPerpendicular();
    }

    _angle = getVec2Angle(dir);

    f32 scaleY = m_actor->getScale().m_y;

    _collisionPos -= dir * getTemplate()->getHangingCollisionCheckOffsetCoop().m_x * scaleY;
    _collisionPos -= norm * getTemplate()->getHangingCollisionCheckOffsetCoop().m_y * scaleY;
}

bbool Ray_PlayerControllerComponent::isCurrentHangValid() const
{
    if ( m_hangingEdge != U32_INVALID )
    {
        PolyLine* polyLine;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_hangingPolyline,m_hangingEdge,polyLine,edge);

        if ( !polyLine || !edge )
        {
            return bfalse;
        }
    }
    else if ( m_hangingActor != ITF_INVALID_OBJREF )
    {
        BaseObject* obj = GETOBJECT(m_hangingActor);

        if ( !obj || !obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
        {
            return bfalse;
        }
    }
    else
    {
        return bfalse;
    }

    if ( isCurrentHangColliding() )
    {
        return bfalse;
    }

    return btrue;
}

bbool Ray_PlayerControllerComponent::isCurrentHangColliding() const
{
    if ( m_hangingActor != ITF_INVALID_OBJREF )
    {
        Actor* actor = AIUtils::getActor(m_hangingActor);

        if ( actor )
        {
            Vec2d collisionCheckPos;
            f32 collisionCheckAngle;
            Vec2d snapPos;
            bbool lookRight;

            getHangCoopCollisionData(actor,collisionCheckPos,collisionCheckAngle,snapPos,lookRight);

            if ( isHangingPosColliding(collisionCheckPos,collisionCheckAngle,btrue) )
            {
                return btrue;
            }
        }
    }
    else if ( m_hangingPolyline != ITF_INVALID_OBJREF )
    {
        PolyLine* poly;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_hangingPolyline,m_hangingEdge,poly,edge);

        if ( poly && edge )
        {
            PhysContactsContainer contacts;
            Vec2d hangingPos;
            f32 hangingAngle;

            calculateHangPosCheck(poly,*edge,m_hangingEdge,m_actor->isFlipped(),hangingPos,hangingAngle);

            if ( isHangingPosColliding(hangingPos,hangingAngle,bfalse) )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::isHangTopColliding() const
{
    PolyLine* poly;
    const PolyLineEdge* edge;

    AIUtils::getPolyLine(m_hangingPolyline,m_hangingEdge,poly,edge);

    if ( !poly || !edge )
    {
        return bfalse;
    }

    Vec2d testPos = edge->getPos() + ( edge->m_vector * m_climbingT );
    Vec2d dir = m_climbingT < 0.5f ? edge->m_normalizedVector : -edge->m_normalizedVector;
    f32 radius = m_characterPhys->getRadius();

    testPos += dir * radius;
    testPos += edge->m_normalizedVector.getPerpendicular() * radius;

    PhysContactsContainer contacts;
    PhysShapeCircle circle(radius);

    PHYSWORLD->checkEncroachment(testPos,testPos,0.f,&circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

    u32 numContacts = contacts.size();
    f32 radCheck = (radius*radius)-0.001f;

    for ( u32 i = 0; i < numContacts; i++ )
    {
        const SCollidableContact& c = contacts[i];

        f32 dist = ( testPos - c.m_contactPoint ).sqrnorm();

        if ( dist < radCheck )
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::isHangingPosColliding( const Vec2d& _collisionCheckPos,
                                                            f32 _collisionCheckAngle, bbool _actor ) const
{
    // Lastly, we check that we are not colliding with anything else inside an area around _collisionCheckPos
    PhysContactsContainer contacts;
    const PhysShapeBox* shape;

    if ( _actor )
    {
        shape = &m_hangingCollisionShapeCoop;
    }
    else
    {
        shape = &m_hangingCollisionShapeSolid;
    }

#ifndef ITF_FINAL
    if ( m_displayHangDebug )
    {
        GFX_ADAPTER->drawDBGBox(_collisionCheckPos,_collisionCheckAngle,shape->getExtent(),1.0f,1.0f,1.f,1.f);
    }
#endif // ITF_FINAL

    PHYSWORLD->checkEncroachment(_collisionCheckPos,_collisionCheckPos,
        _collisionCheckAngle,
        shape,
        ECOLLISIONFILTER_ENVIRONMENT,
        m_actor->getDepth(),
        contacts);

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& c = contacts[index];

        if ( c.m_edgeIndex != U32_INVALID )
        {
            BaseObject* obj = GETOBJECT(c.m_collidableUserData);

            if ( !obj )
            {
                continue;
            }

            PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

            if ( !poly )
            {
                continue;
            }

            if ( c.m_edgeIndex >= poly->getPosCount() )
            {
                continue;
            }

            const PolyLineEdge& edge = poly->getEdgeAt(c.m_edgeIndex);

            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge.getGameMaterial()));
            if (mat && mat->isCharacter())
                continue;


            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::hangFromPoint( HangingInfo & _info )
{
    Vec2d speed;

    speed = m_characterPhys->getSpeed();
    m_characterPhys->setDisabled(btrue);
    m_characterPhys->resetForces();
    m_characterPhys->setSpeed(Vec2d::Zero);
    m_characterPhys->resetWorldSpeed();
    m_walkOnWallsDisableUntilLand = bfalse;
    m_reducedHitUpDisabledUntilLand = bfalse;

    if ( m_currentState != &m_stateIdleToHang )
    {
        setLookRight(_info.m_lookRight);
    }

    m_hangingEdge = _info.m_edge;
    m_hangingActor = _info.m_actor;
    m_climbingT = _info.m_lookRight ? 0.f : 1.f;
    m_currentTargetForce = Vec2d::Zero;

    if ( _info.m_polyline != ITF_INVALID_OBJREF )
    {
        BaseObject* obj = GETOBJECT(_info.m_polyline);

        if ( obj )
        {
            PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

            if ( poly )
            {
                const PolyLineEdge& edge = poly->getEdgeAt(_info.m_edge);

                m_hangingPolyline = _info.m_polyline;
                m_hangingActor = ITF_INVALID_OBJREF;

                Actor* ownerActor = poly->getOwnerActor();

                bbool left = ( _info.m_hangPos - edge.getPos() ).sqrnorm() < ( _info.m_hangPos - ( edge.getPos() + edge.m_vector ) ).sqrnorm();
                EventHanging event(m_actor->getRef(),m_hangingPolyline,m_hangingEdge,m_characterPhys->getWeight(),speed,left?0.f:1.f,btrue,getCharacterSize());

                if ( ownerActor )
                    poly->getOwnerActor()->onEvent(&event);
                else if (poly->m_ownerFrise)
                    poly->m_ownerFrise->onEvent(&event);
            }
            else
            {
                return bfalse;
            }
        }
        else
        {
            return bfalse;
        }
    }
    else
    {
        m_hangingPolyline = ITF_INVALID_OBJREF;

        Actor* actor = AIUtils::getActor(m_hangingActor);

        if (actor)
        {
            m_coopActionMain = m_hangingActor;
            actor->getBinding()->bindChild(m_actor->getRef());
            Ray_EventHangOnCharacter hangOnCharacter(m_actor->getRef(),btrue);
            actor->onEvent(&hangOnCharacter);
        }
    }

    m_coopActionMode = _info.m_coopActionMode;
    m_animComponent->lockMagicBox(_info.m_hangPos.to3d(m_actor->getPos().m_z));
    playSnapFX();

    REWARD_MANAGER->resetTotalOf(Ray_RewardManager::Action_GetBouncedEnemiesSameJump, m_playerIndex);

    return btrue;
}

void Ray_PlayerControllerComponent::clearHanging()
{
    if ( m_hangingActor != ITF_INVALID_OBJREF )
    {
        Actor* hangingActor = AIUtils::getActor(m_hangingActor);

        if ( hangingActor )
        {
            Ray_EventHangOnCharacter hangOnCharacter(m_actor->getRef(),bfalse);
            hangingActor->onEvent(&hangOnCharacter);
        }

        m_actor->getBinding()->unbindFromParent();
    }
    else if ( m_hangingPolyline != ITF_INVALID_OBJREF )
    {
        const BaseObject* pObj = GETOBJECT(m_hangingPolyline);

        if ( pObj )
        {
            ITF_ASSERT(pObj->IsClassCRC(ITF_GET_STRINGID_CRC(PolyLine,1932163747)));

            const PolyLine* polyLine = static_cast<const PolyLine*>(pObj);

            EventHanging event(m_actor->getRef(),m_hangingPolyline,m_hangingEdge,m_characterPhys->getWeight(),m_characterPhys->getSpeed(),0.f,bfalse,getCharacterSize());

            if ( polyLine->getOwnerActor() )
                polyLine->getOwnerActor()->onEvent(&event);
            else if (polyLine->m_ownerFrise)
                polyLine->m_ownerFrise->onEvent(&event);

        }
    }

    m_hangActivationCounter = getTemplate()->getHangEdgeActivationTime();
    m_previousHangingActor = m_hangingActor;
    m_previousHangingPolyline = m_hangingPolyline;
    m_hangingActor = ITF_INVALID_OBJREF;
    m_hangingPolyline = ITF_INVALID_OBJREF;
    m_coopActionMain = ITF_INVALID_OBJREF;
    m_coopActionVictim = ITF_INVALID_OBJREF;
    m_hangingEdge = U32_INVALID;

    if ( !isPerformingUTurn() )
    {
        resetDesiredLookDir();
    }

    m_characterPhys->setDisabled(bfalse);

    m_animComponent->resetLockMagicBox();

}

bbool Ray_PlayerControllerComponent::getWorldHangPos( Vec2d& _ret, f32& _angle ) const
{
    PolyLine* polyLine;
    const PolyLineEdge* edge;

    AIUtils::getPolyLine(m_hangingPolyline,m_hangingEdge,polyLine,edge);

    if ( polyLine && edge )
    {
        if ( m_climbingT < 0.5f )
        {
            _ret = edge->getPos();

            PolyLine* nextPoly;
            u32 nextEdgeIndex;

            AIUtils::getAdjacentEdge(polyLine,m_hangingEdge,bfalse,nextPoly,nextEdgeIndex);

            if ( nextEdgeIndex != U32_INVALID )
            {
                const PolyLineEdge& prevEdge = nextPoly->getEdgeAt(nextEdgeIndex);
                _angle = getVec2Angle(-prevEdge.m_normalizedVector.getPerpendicular());
            }
        }
        else
        {
            _ret = edge->getPos() + edge->m_vector;

            PolyLine* nextPoly;
            u32 nextEdgeIndex;

            AIUtils::getAdjacentEdge(polyLine,m_hangingEdge,btrue,nextPoly,nextEdgeIndex);

            if ( nextEdgeIndex != U32_INVALID )
            {
                const PolyLineEdge& nextEdge = nextPoly->getEdgeAt(nextEdgeIndex);
                _angle = getVec2Angle(nextEdge.m_normalizedVector.getPerpendicular());
            }
        }
    }
    else
    {
        Actor* hangingActor = AIUtils::getActor(m_hangingActor);

        if ( hangingActor )
        {
            EventQuerySnapData snap;
            hangingActor->onEvent(&snap);
            _ret = snap.getPos().truncateTo2D();
            _angle = snap.getAngle();

            if ( !snap.getLookRight() )
            {
                f32 localAngle = _angle - hangingActor->getAngle();
                Vec2d dir = Vec2d::Right.Rotate(localAngle);
                dir.m_x *= -1.f;
                dir = dir.Rotate(hangingActor->getAngle()+MTH_PI);
                _angle = getVec2Angle(dir);
            }
        }
        else
        {
            return bfalse;
        }
    }

    return btrue;
}

bbool Ray_PlayerControllerComponent::getWorldClimbPos( Vec2d& _ret, f32& _angle ) const
{
    PolyLine* polyLine;
    const PolyLineEdge* edge;
    AIUtils::getPolyLine(m_climbingPolyline,m_climbingEdge,polyLine,edge);

    if ( polyLine && edge )
    {
        _ret = edge->getPos() + ( edge->m_vector *  m_climbingT );
        _angle = getVec2Angle(m_currentGravityDir.getPerpendicular());

        return btrue;
    }
    else
    {
        return bfalse;
    }
}

bbool Ray_PlayerControllerComponent::checkClimbCollision( ObjectRef _climbPoly, u32 _edgeIndex, const Vec2d& _climbPos, f32 _climbAngle ) const
{
    // If we are not climbing already then we need to check for collision on the area where we will be climbing
    PolyLine* poly;
    const PolyLineEdge* edge;

    AIUtils::getPolyLine(_climbPoly,_edgeIndex,poly,edge);

    if ( edge )
    {
        PhysContactsContainer contacts;

        Vec2d boxPos = _climbPos + ( Vec2d::Right.Rotate(_climbAngle-MTH_PIBY2) * m_climbingCollisionCheckShape.getExtent().m_y );

#ifndef ITF_FINAL
        if ( m_displayHangDebug )
        {
            GFX_ADAPTER->drawDBGBox(boxPos,_climbAngle,m_climbingCollisionCheckShape.getExtent(),1.0f,1.0f,1.f,1.f);
        }
#endif // ITF_FINAL

        PHYSWORLD->checkEncroachment(boxPos,boxPos,
                                     _climbAngle,
                                     &m_climbingCollisionCheckShape,
                                     ECOLLISIONFILTER_ENVIRONMENT,
                                     m_actor->getDepth(),
                                     contacts);

        for ( u32 index = 0; index < contacts.size(); index++ )
        {
            const SCollidableContact& c = contacts[index];

            if ( c.m_edgeIndex != U32_INVALID && _climbPoly != c.m_collidableUserData)
            {
                PolyLine* poly;
                const PolyLineEdge* edge;

                AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

                if ( !edge )
                {
                    continue;
                }

                const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));
                if (mat && mat->isCharacter())
                    continue;

                return btrue;
            }
        }


    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::processContactClimbActor( Actor* _victim, const Vec2d& _headPos, HangingInfo& _info ) const
{
    if ( m_hangActivationCounter )
    {
        if ( m_previousHangingActor == _victim->getRef() )
        {
            return bfalse;
        }
    }

    f32 speedProj = m_currentGravityDir.dot(m_characterPhys->getSpeed());

    if ( speedProj >= -getTemplate()->getHangMaxSpeed() )
    {
        // query the victim to see if we can hang on it
        EventInteractionQuery query;
        query.setSender(m_actor->getRef());
        _victim->onEvent(&query);

        if (query.getInteraction() == CharacterInteractionType_Grab)
        {
            _info.m_actor = _victim->getRef();

            Vec2d collisionCheckPos, snapPos;
            f32 collisionCheckAngle;
            bbool lookRight;

            getHangCoopCollisionData(_victim,collisionCheckPos,collisionCheckAngle,snapPos,lookRight);

            // To know if we can climb let's check if the polyline is close enough to the head
            f32 dist = ( snapPos - _headPos ).sqrnorm();
            f32 distCheck = getTemplate()->getHangCoopDistanceCheck() * m_actor->getScale().m_y;

            if ( dist > (distCheck*distCheck) )
            {
                return bfalse;
            }

            if ( !isHangingPosColliding(collisionCheckPos,collisionCheckAngle,btrue) )
            {
                _info.m_hangPos = snapPos;
                _info.m_polyline = ITF_INVALID_OBJREF;
                _info.m_edge = U32_INVALID;
                _info.m_lookRight = lookRight;
                _info.m_climb = bfalse;
                _info.m_coopActionMode = ACTIONCOOPMODE_VICTIM;
                return btrue;
            }
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::processContactClimbPolyline( const SCollidableContact& _contact,
                                                                  PolyLine* _polyLine,
                                                                  const PolyLineEdge& _edge, const Vec2d& _headPos,
                                                                  HangingInfo& _info ) const
{
    if ( _contact.m_collidableUserData == m_actor->getRef() )
    {
        return bfalse;
    }

    if ( _polyLine->m_isCollisionActive )
    {
        return bfalse;
    }

    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(_edge.getGameMaterial()));

    if ( !mat )
    {
        return bfalse;
    }

    if ( mat->getDangerousLevel() > 0 || !mat->getClimbable() )
    {
        return bfalse;
    }

    if ( m_climbActivationCounter )
    {
        if ( m_previousClimbingPolyline == _polyLine->getRef() )
        {
            return bfalse;
        }
    }

    bbool checkSpeed = !mat->isClimbVertical();

    if ( checkSpeed )
    {
        f32 speedProj = m_currentGravityDir.dot(m_characterPhys->getSpeed());

        if ( speedProj < -getTemplate()->getHangMaxSpeed() )
        {
            return bfalse;
        }
    }

    // To know if we can climb let's check if the polyline is close enough to the head
    Vec2d dirToHead = _headPos - _edge.getPos();
    f32 t = _edge.m_normalizedVector.dot(dirToHead) / _edge.m_length;

    t = Clamp(t,0.f,1.f);

    Vec2d closestPoint = _edge.getPos() + ( _edge.m_vector * t );
    f32 dist = ( closestPoint - _headPos ).sqrnorm();

    if ( dist > (getTemplate()->getClimbEdgeDistanceRange()*getTemplate()->getClimbEdgeDistanceRange()) )
    {
        return bfalse;
    }

    if ( checkClimbCollision(_polyLine->getRef(),_contact.m_edgeIndex,closestPoint,getVec2Angle(m_currentGravityDir.getPerpendicular())) )
    {
        return bfalse;
    }

    _info.m_hangPos = closestPoint;
    _info.m_polyline = _polyLine->getRef();
    _info.m_edge = _contact.m_edgeIndex;
    _info.m_lookRight = !m_actor->isFlipped();
    _info.m_climb = btrue;
    _info.m_actor = ITF_INVALID_OBJREF;
    _info.m_coopActionMode = ACTIONCOOPMODE_NONE;

    return btrue;
}

bbool Ray_PlayerControllerComponent::climbFromPoint( HangingInfo & _info )
{
    BaseObject* obj = GETOBJECT(_info.m_polyline);

    if ( !obj )
    {
        return bfalse;
    }

    PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

    if ( !poly )
    {
        return bfalse;
    }

    const PolyLineEdge& edge = poly->getEdgeAt(_info.m_edge);
    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge.getGameMaterial()));

    Vec2d speed = m_characterPhys->getSpeed();

    m_characterPhys->setDisabled(btrue);
    m_characterPhys->resetForces();
    m_characterPhys->setSpeed(Vec2d::Zero);

    Vec2d dirToTarget = _info.m_hangPos - edge.getPos();
    m_climbingT = edge.m_normalizedVector.dot(dirToTarget) / edge.m_length;
    m_climbingT = Clamp(m_climbingT,0.f,1.f);

    m_walkOnWallsDisableUntilLand = bfalse;
    m_reducedHitUpDisabledUntilLand = bfalse;
    m_climbingPolyline = _info.m_polyline;
    m_climbingEdge = _info.m_edge;
    m_currentTargetForce = Vec2d::Zero;
    m_climbingVertical = mat && mat->isClimbVertical();
    m_climbingSlide = mat && mat->isClimbSlide();
    m_climbingForceSidewaysJump = mat && mat->isClimbForceSideJump();
    m_orientationPose = m_climbingVertical ? ORIENTATION_VERTICAL : ORIENTATION_HORIZONTAL;
    m_climbingConstrainedEdge = U32_INVALID;
    m_climbingConstrainedT = 0.f;
    m_climbingJustStarted = btrue;
    m_climbingPrevMoveSign = 0.f;
    m_climbingAllowDropVertical = bfalse;
    m_climbingAllowExitHorizontal = bfalse;
    m_climbingOnBottomCorner = bfalse;
    m_climbingAllowHangCoop = bfalse;

    Vec2d climbPos = edge.getPos() + ( edge.m_vector *  m_climbingT );

    m_animComponent->lockMagicBox(climbPos.to3d(m_actor->getPos().m_z));

    f32 speedProd = edge.m_normalizedVector.dot(speed);

    m_climbingIntertia = speedProd;

    EventHanging event(m_actor->getRef(),m_climbingPolyline,m_climbingEdge,
        m_characterPhys->getWeight(),speed,m_climbingT,btrue,getCharacterSize());

    if ( poly->getOwnerActor() )
        poly->getOwnerActor()->onEvent(&event);
    else if (poly->m_ownerFrise)
        poly->m_ownerFrise->onEvent(&event);


    setStance(STANCE_CLIMB);
    playSnapFX();

    if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
    {
        REWARD_MANAGER->resetTotalOf(Ray_RewardManager::Action_GetBouncedEnemiesSameJump, m_playerIndex);
    }

    return btrue;
}

void Ray_PlayerControllerComponent::clearClimbing()
{
    if ( m_climbingPolyline != ITF_INVALID_OBJREF )
    {
        const BaseObject* pObj = GETOBJECT(m_climbingPolyline);

        if ( pObj )
        {
            ITF_ASSERT(pObj->IsClassCRC(ITF_GET_STRINGID_CRC(PolyLine,1932163747)));

            const PolyLine* polyLine = static_cast<const PolyLine*>(pObj);

            if ( polyLine->getOwnerActor() )
            {
                EventHanging event(m_actor->getRef(),m_climbingPolyline,m_climbingEdge,
                                   m_characterPhys->getWeight(),m_characterPhys->getSpeed(),m_climbingT,bfalse,getCharacterSize());
                polyLine->getOwnerActor()->onEvent(&event);
            }
        }

        m_climbActivationCounter = getTemplate()->getClimbEdgeActivationTime();
        m_previousClimbingPolyline = m_climbingPolyline;
        setStance(STANCE_STAND);
        m_climbingPolyline = ITF_INVALID_OBJREF;
        m_climbingEdge = U32_INVALID;
        m_animComponent->resetLockMagicBox();
        m_animComponent->setPlayRate(1.f);
        m_characterPhys->setDisabled(bfalse);

        resetDesiredLookDir();
    }
}

void Ray_PlayerControllerComponent::getClimbEdgePolyline( PolyLine** _polyLine, const PolyLineEdge** _edge ) const
{
    AIUtils::getPolyLine(m_climbingPolyline,m_climbingEdge,*_polyLine,*_edge);
}

void Ray_PlayerControllerComponent::getHangEdgePolyline( PolyLine** _polyLine, const PolyLineEdge** _edge ) const
{
    AIUtils::getPolyLine(m_hangingPolyline,m_hangingEdge,*_polyLine,*_edge);
}

void Ray_PlayerControllerComponent::calculateEfficiency()
{
    m_currentEfficiency = AIUtils::walkCalculateEfficiency(m_characterPhys->getSpeed().norm(),
                                                           getWalkMaxSpeed(),
                                                           m_characterPhys->getFrictionCoef(),
                                                           m_currentGravity.norm(),
                                                           m_currentGravityDir,
                                                           GetActor()->getAngle(),
                                                           getTemplate()->getMinGroundAngleEfficiencyAngle().ToRadians(),
                                                           getTemplate()->getMaxGroundAngleEfficiencyAngle().ToRadians(),
                                                           getTemplate()->getMinGroundAngleEfficiencyMult(),
                                                           getTemplate()->getMaxGroundAngleEfficiencyMult(),
                                                           !isSprinting());
}

void Ray_PlayerControllerComponent::handleAnimTrackEvent(AnimGameplayEvent* _event)
{
    if ( _event->getName() == MRK_UTurn_Finish )
    {
        m_uTurnFinished = btrue;

        if ( m_flipActor )
        {
            // More than one frame was played together, so we need to flip the character right now or he will
            // appear wrong one frame
            m_actor->setFlipped(!m_actor->isFlipped());
            m_animComponent->forceUpdateFlip();
        }
    }
    else if ( _event->getName() == MRK_Flip_Actor )
    {
        m_flipActor = btrue;
    }
    else if ( _event->getName() == ITF_GET_STRINGID_CRC(MRK_FallRoof_Finished,959526059) )
    {
        m_fallFromRoof = bfalse;
        resetDesiredLookDir();
    }
}

void Ray_PlayerControllerComponent::checkBounceEdge()
{
    if ( m_bounceCurrentDir == Vec2d::Zero )
    {
        BounceType type = BOUNCETYPE_POLYLINE;
        Vec2d bounceDir = m_bounceCheckEdgeDir.getPerpendicular();

        if ( m_bounceCheckEdgeGameMaterial.isValid() )
        {
            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(m_bounceCheckEdgeGameMaterial));

            if ( mat )
            {
                if ( mat->getIsWave() )
                {
                    type = BOUNCETYPE_WAVE;
                    bounceDir = -m_currentGravityDir;
                }
                else if ( mat->getBounceType() != BOUNCETYPE_NONE )
                {
                    type = mat->getBounceType();
                }
            }
        }

        processBounce(m_bounceCheckEdgeDangerLevel,bounceDir,type,m_bounceCheckEdgeSpeed,m_bounceCheckEdgeGameMaterial);

        bbool bounce = m_bounceCurrentDir != Vec2d::Zero;

        if ( bounce )
        {
            if ( m_bounceCheckEdgeDangerLevel )
            {
                setLastHitNature(HITNATURE_ENVIRONMENTDAMAGE);
            }

            PolyLine* poly;
            const PolyLineEdge* edge;

            AIUtils::getPolyLine(m_bounceCheckEdgePolyRef,m_bounceCheckEdgeIndex,poly,edge);

            if ( poly )
            {
                Actor* ownerActor = poly->getOwnerActor();

                if ( ownerActor )
                {
                    EventBouncedOnPolyline bouncedEvent(m_bounceCheckEdgePolyRef,
                        m_bounceCheckEdgeIndex,
                        m_bounceCurrentDir);

                    poly->getOwnerActor()->onEvent(&bouncedEvent);
                }
            }
        }
    }

    resetBounceEdgeCheck();
}

void Ray_PlayerControllerComponent::setBounceEdgeCheck( const Vec2d& _dir, u32 _danger, ObjectRef _poly, u32 _edgeIndex, const Vec2d& _speedAtCollision, const StringID& _gameMat )
{
    m_bounceCheckEdgeDir = _dir;
    m_bounceCheckEdgeDangerLevel = _danger;
    m_bounceCheckEdgePolyRef = _poly;
    m_bounceCheckEdgeIndex = _edgeIndex;
    m_bounceCheckEdgeGameMaterial = _gameMat;
    m_bounceCheckEdgeSpeed = _speedAtCollision;
    m_bounceSender = ITF_INVALID_OBJREF;
}

void Ray_PlayerControllerComponent::resetBounceEdgeCheck()
{
    m_bounceCheckEdgeDir = Vec2d::Zero;
    m_bounceCheckEdgeDangerLevel = U32_INVALID;
    m_bounceCheckEdgePolyRef = ITF_INVALID_OBJREF;
    m_bounceCheckEdgeIndex = U32_INVALID;
    m_bounceCheckEdgeSpeed = Vec2d::Zero;
    m_bounceCheckEdgeGameMaterial.invalidate();
    m_bounceSender = ITF_INVALID_OBJREF;
}

void Ray_PlayerControllerComponent::resetBounce()
{
    m_bounceCurrentDir = Vec2d::Zero;
    m_bounceCurrentOrigin = Vec2d::Zero;
    m_bounceCurrentType = BOUNCETYPE_NONE;
    m_bounceCurrentGameMaterial.invalidate();
    m_bounceCurrentReactType = BOUNCEREACTTYPE_NONE;
    m_bounceCurrentDangerLevel = 0;
    m_bounceSpeedAtTime = Vec2d::Zero;
    m_bounceJumpAllowTimer = 0.f;
    m_bounceTimer = 0.f;
    m_bounceRepositionCounter = 0.f;
    m_bounceRepositionTotal = Vec2d::Zero;
    m_bounceRepositionFinalSpeed = Vec2d::Zero;
    m_bounceRepositionTotalTime = 0.f;
    m_bounceMultiplier = 1.f;
    m_bounceSender = ITF_INVALID_OBJREF;
    resetBounceEdgeCheck();
}

void Ray_PlayerControllerComponent::setCurrentPunch( EPunchType _type, u32 _level )
{
    m_currentPunchHitType = _type;
    m_currentPunchHitLevel = _level;
    updateCurrentPunchOrientation();
}

void Ray_PlayerControllerComponent::updateCurrentPunchOrientation()
{
    if ( m_stance != STANCE_STAND ||
        !m_characterPhys->getStickedEdge() ||
        ( m_currentPunchHitType != PUNCHTYPE_CHARGE &&
          m_currentPunchHitType != PUNCHTYPE_REPEATING ) )
    {
        m_currentPunchOrientation = 0.f;
    }
    else
    {
        if ( m_moveInput.m_y > 0.5f )
        {
            m_currentPunchOrientation = 90.f;
        }
        else
        {
            m_currentPunchOrientation = 0.f;
        }
    }
}

bbool Ray_PlayerControllerComponent::startNormalPunch()
{
    m_hitQueueTimer = 0.f; // Reset the queue since we press hit to get here

    if ( isSizeReduced() )
    {
        if ( m_dashAttackCooldown )
        {
            return bfalse;

        }
        resetUTurn();

        if ( m_moveInput.m_x < 0.f )
        {
            m_actor->setFlipped(!m_actor->isFlipped());
        }

        bbool forceUp = bfalse;

        if ( isHanging() )
        {
            if ( m_moveInput.m_x >= 0.f && m_coopActionMode == ACTIONCOOPMODE_NONE )
            {
                forceUp = btrue;
            }

            clearHanging();
        }
        else if ( isClimbing() )
        {
            clearClimbing();
        }

        m_stateReleaseHit.setForceReducedHitUp(forceUp);
        setStance(STANCE_STAND);
        setCurrentPunch(PUNCHTYPE_REDUCEDSIZE,getTemplate()->getReducedSizeHitLevel());
        changeState(&m_stateReleaseHit);

        return btrue;
    }
    else
    {
        changeState(&m_statePrepareHit);

        return btrue;
    }
}

void Ray_PlayerControllerComponent::resetStance(EStance _stance )
{
    setStance(_stance);
    m_prevStance = m_stance;
    stopHelicoFx();
}

void Ray_PlayerControllerComponent::setStance( EStance _stance )
{
    if ( m_stance != _stance )
    {
        m_prevStance = m_stance;
        m_stance = _stance;

        changeStanceHelico();
        changeStanceStand();
        changeStanceSwim();
        changeStanceWallSlide();
        changeStanceHang();
        changeStanceClimb();
    }
}

void Ray_PlayerControllerComponent::changeStanceHang()
{
    if ( m_prevStance == STANCE_HANG )
    {
        clearHanging();
    }
}

void Ray_PlayerControllerComponent::changeStanceClimb()
{
    if ( m_prevStance == STANCE_CLIMB )
    {
        clearClimbing();
    }
}

void Ray_PlayerControllerComponent::changeStanceWallSlide()
{
    if ( m_stance == STANCE_WALLSLIDE )
    {
        m_characterPhys->resetWorldSpeed();
        resetPhysicAirHit();
    }
}

void Ray_PlayerControllerComponent::changeStanceHelico()
{
    if (m_stance == STANCE_HELICO)
    {
        startHelicoFx();
        m_characterPhys->setAirFrictionMultiplier(getTemplate()->getHelicopterAirFrictionMultiplier());
        m_characterPhys->setUnstickMinFrictionMultiplier(getTemplate()->getHelicopterUnstickMinFrictionMultiplier());
        m_characterPhys->setGravityMultiplier(0.f);
        m_helicopterSuspensionTimer = getTemplate()->getHelicopterSuspensionTime();
    }
    else if (m_prevStance == STANCE_HELICO)
    {
        stopHelicoFx();
        m_characterPhys->setAirFrictionMultiplier(0.f);
        m_characterPhys->setUnstickMinFrictionMultiplier(1.0f);
        m_characterPhys->setGravityMultiplier(1.f);
    }
}

void Ray_PlayerControllerComponent::changeStanceStand()
{
    // If we are not on a stand stance anymore we reset the crouch status
    if ( m_stance != STANCE_STAND )
    {
        if ( isCrouch() )
        {
            setCrouch(bfalse);
        }

        if ( isRolling() )
        {
            setRolling(bfalse);
        }
    }
    else
    {
        if ( m_characterPhys->getStickedEdge() )
        {
            resetPhysicAirHit();
        }
    }

    if ( m_stance != STANCE_STAND &&
         m_stance != STANCE_SWIM )
    {
        if ( isSprinting() )
        {
            setSprinting(bfalse);
        }
    }
}

void Ray_PlayerControllerComponent::changeStanceSwim()
{
    if ( m_prevStance == STANCE_SWIM )
    {
        m_characterPhys->setAirFrictionMultiplier(0.f);
        m_characterPhys->setUpdateRotation(btrue);

        if ( getTemplate()->getApplyWindSpeedLimitation() )
        {
            m_characterPhys->setApplyWindSpeedLimiter(btrue);
        }

        m_characterPhys->setStickingDisabled(bfalse);
        resetGravityMode(GRAVITYMODE_SWIM);
        setSwimingPolyline(NULL);

        if ( !isDead() )
        {
            startDiveOutFx();
        }
    }
    else if ( m_stance == STANCE_SWIM )
    {
        if ( getTemplate()->getApplyWindSpeedLimitation() )
        {
            m_characterPhys->setApplyWindSpeedLimiter(bfalse);
        }

        m_swimingDashCounter = 0.f;
        m_swimingTime = 0.f;
        m_characterPhys->setUpdateRotation(bfalse);
        m_characterPhys->setStickingDisabled(btrue);
        m_characterPhys->unstick();
        setGravityMode(GRAVITYMODE_SWIM,0.f);
        resetPhysicAirHit();

        if ( m_prevState != &m_stateIdle )
        {
            m_swimingEnterDown = getMoveData().getMoveDirection() == MOVEDIR_DOWN;
            m_swimingApplyEnterWaterResistance = btrue;
        }
        else
        {
            m_swimingEnterDown = bfalse;
        }

        startDiveFx();
        cancelPhysicAirHit();

        if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
        {
            REWARD_MANAGER->resetTotalOf(Ray_RewardManager::Action_GetBouncedEnemiesSameJump, m_playerIndex);
        }
    }

    setPhantomShape();
}

void Ray_PlayerControllerComponent::setActionPose( EActionPose _pose )
{
    m_actionPose = _pose;

    switch(m_actionPose)
    {
        case ACTIONPOSE_FIGHT:
            m_poseTimeCounter = getTemplate()->getFightPoseDuration();
            break;
    }
}

void Ray_PlayerControllerComponent::updateActionPose( f32 _dt )
{
    switch(m_actionPose)
    {
        case ACTIONPOSE_FIGHT:
            {
                m_poseTimeCounter = Max(m_poseTimeCounter-_dt,0.f);

                bbool isIdle = playerIsIdle();

                if ( !m_poseTimeCounter )
                {
                    if ( isIdle )
                    {
                        // force the stand cool anim after the fight pose
                        m_idleTimer = 100.f;
                    }

                    setActionPose(ACTIONPOSE_STAND);
                }
                else if ( !isIdle )
                {
                    setActionPose(ACTIONPOSE_STAND);
                }
            }
            break;
        case ACTIONPOSE_STAND:
            {
                if ( m_moodZone == MOODZONE_AFRAID )
                {
                    setActionPose(ACTIONPOSE_AFRAID);
                }
            }
            break;
        case ACTIONPOSE_AFRAID:
            {
                if ( m_moodZone == MOODZONE_NONE )
                {
                    setActionPose(ACTIONPOSE_STAND);
                }
            }
            break;
    }
}

void Ray_PlayerControllerComponent::updateIceCompensate()
{
    if ( m_frictionMode != FRICTIONMODE_NORMAL && m_frictionMode != FRICTIONMODE_ICECOMPENSATE )
    {
        return;
    }

    resetFrictionMode(FRICTIONMODE_ICECOMPENSATE);

    const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();

    if ( !currentEdge || m_moveInput.m_x == 0.f || m_currentState != &m_stateIdle || isCrouch() )
    {
        return;
    }
    else
    {
        f32 edgeFriction = AIUtils::getEdgeFriction(currentEdge);

        if ( edgeFriction >= 1.f )
        {
            return;
        }

        if ( m_characterPhys->getStickOnWalls() )
        {
            StickToPolylinePhysComponent::EdgeOrientationType orientation = m_characterPhys->getEdgeOrientationType(currentEdge->m_normalizedVector);

            if ( orientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF ||
                 orientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
            {
                return;
            }
        }

        bbool speedRight = currentEdge->m_normalizedVector.dot(m_characterPhys->getSpeed()) > 0.f;
        bbool moveRight = getMoveData().getMoveDirection() == MOVEDIR_RIGHT;
        if ( speedRight != moveRight )
        {
            f32 clampedFriction = Clamp(edgeFriction,0.1f,1.f);
            f32 t = ((1.f-clampedFriction)/0.9f);
            f32 mult = Interpolate(1.f,getTemplate()->getWalkIceCompensateFrictionMultiplier(),t);
            setFrictionMode(FRICTIONMODE_ICECOMPENSATE,mult);
        }
    }
}

void Ray_PlayerControllerComponent::updateMoodZone()
{
    m_moodZone = MOODZONE_NONE;

    u32 numRegions = m_currentRegions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        PolylineRegion& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_Frightened )
        {
            m_moodZone = MOODZONE_AFRAID;
            break;
        }
    }
}

void Ray_PlayerControllerComponent::updateRegionCurrent( f32 _dt )
{
    if ( isDead()
#ifdef ITF_SUPPORT_CHEAT
         || m_currentState == &m_stateFlyMode
#endif
          )
    {
        m_prevCurrentRegion = ITF_INVALID_OBJREF;
        m_prevCurrentMove = Vec2d::Zero;
        return;
    }

    u32 numRegions = m_currentRegions.size();
    Vec2d regionMoveThisFrame = Vec2d::Zero;
    ObjectRef regionThisFrame = ITF_INVALID_OBJREF;

    for ( u32 i = 0; i < numRegions; i++ )
    {
        PolylineRegion& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_Current )
        {
            PolyLine* poly = AIUtils::getPolyLine(data.m_polyRef);

            if ( !poly )
            {
                continue;
            }

            const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(poly->getEdgeAt(0).getGameMaterial()));

            if ( mat && mat->getMoveSpeed() )
            {
                Vec2d moveDir;

                if ( poly->m_ownerFrise )
                {
                    moveDir = Vec2d::Right.Rotate(poly->m_ownerFrise->getAngle());
                }
                else if ( poly->getOwnerActor() )
                {
                    moveDir = Vec2d::Right.Rotate(poly->getOwnerActor()->getAngle());
                }
                else
                {
                    continue;
                }

                regionMoveThisFrame = moveDir * mat->getMoveSpeed();
                regionThisFrame = data.m_polyRef;
                break;
            }
        }
    }

    if ( regionThisFrame != m_prevCurrentRegion )
    {
        // The region has changed
        if ( regionThisFrame != ITF_INVALID_OBJREF )
        {
            // We are in a new region
            if ( m_prevCurrentRegion != ITF_INVALID_OBJREF )
            {
                // We were in another region before, so transfer the speed
                Vec2d totalSpeed = m_characterPhys->getSpeed() + m_prevCurrentMove;

                m_characterPhys->setSpeed(totalSpeed);
            }

            // Apply the move from the new region
            Vec2d move = regionMoveThisFrame * _dt;
            Vec2d newPos = m_actor->get2DPos() + move;

            EventTeleport evtTeleport( newPos.to3d(m_actor->getPos().m_z), m_actor->getAngle(), bfalse );
            m_actor->onEvent(&evtTeleport);
        }
        else
        {
            // We are no more in a region, but we were in one before, so apply the move
            Vec2d totalSpeed = m_characterPhys->getSpeed() + m_prevCurrentMove;

            m_characterPhys->setSpeed(totalSpeed);
        }
    }
    else if ( regionThisFrame != ITF_INVALID_OBJREF )
    {
        // We are still in the same region
        Vec2d move = regionMoveThisFrame * _dt;
        Vec2d newPos = m_actor->get2DPos() + move;

        EventTeleport evtTeleport( newPos.to3d(m_actor->getPos().m_z), m_actor->getAngle(), bfalse );
        m_actor->onEvent(&evtTeleport);
    }

    m_prevCurrentMove = regionMoveThisFrame;
    m_prevCurrentRegion = regionThisFrame;
}

void Ray_PlayerControllerComponent::startUTurn( EUturnType _type )
{
    if ( !isPerformingUTurn() )
    {
        if ( m_stance != STANCE_CLIMB || !m_climbingVertical )
        {
            // In climb vertical we handle the uturn in a special node
            m_animComponent->resetTree();
        }

        setSlide(bfalse);
        setRolling(bfalse);

        m_performingUTurn = _type;
        m_uTurnFlipped = bfalse;
        m_uTurnFinished = bfalse;
        m_uTurnComboCounter = getTemplate()->getTurnComboTime();
        m_uTurnCounter = 0.f;

        if ( !m_actor->isFlipped() )
        {
            m_desiredLookDir = MOVEDIR_LEFT;
        }
        else
        {
            m_desiredLookDir = MOVEDIR_RIGHT;
        }
    }
}

void Ray_PlayerControllerComponent::resetUTurn()
{
    if ( isPerformingUTurn() )
    {
        if ( m_moveInput.m_x != 0.f &&
             isSprinting() )
        {
            setCrouch(bfalse);
        }

        m_performingUTurn = UTURNTYPE_NONE;
        m_uTurnFinished = bfalse;

        if ( !m_uTurnFlipped )
        {
            setLookRight(m_actor->isFlipped());
            m_flipActor = bfalse;
        }
    }
}

void Ray_PlayerControllerComponent::setSlide( bbool _val )
{
    if ( isPerformingSlide() != _val )
    {
        toggleActionFlag(ACTIONFLAG_SLIDE);

        if ( _val )
        {
            //m_fxController->startSlide();

            startSlideFx();
            setFrictionMode(FRICTIONMODE_SLIDE,getTemplate()->getSlideFrictionMultiplier());
            setCrouch(btrue);
            setRolling(bfalse);
        }
        else
        {
            //m_fxController->stopSlide();
            stopSlideFx();
            resetFrictionMode(FRICTIONMODE_SLIDE);
        }
    }
}

f32 Ray_PlayerControllerComponent::getMinSlideSpeed( f32 _friction ) const
{
    f32 clampedF = Clamp(_friction,0.f,1.f);
    f32 multiplier = Interpolate(getTemplate()->getSlideMinSpeedMultMinFriction(),getTemplate()->getSlideMinSpeedMultMaxFriction(),clampedF);

    return getTemplate()->getSlideMinSpeedToAllow() * multiplier;
}

void Ray_PlayerControllerComponent::resetDesiredLookDir()
{
    if ( !m_actor->isFlipped() )
    {
        m_desiredLookDir = MOVEDIR_RIGHT;
    }
    else
    {
        m_desiredLookDir = MOVEDIR_LEFT;
    }
}

void Ray_PlayerControllerComponent::setLookRight( bbool _lookRight )
{
    if ( !m_actor->isFlipped() != _lookRight )
    {
        m_actor->setFlipped(!_lookRight);
    }
}

void Ray_PlayerControllerComponent::setPathBlocked( bbool _pathBlocked )
{
    if ( isPathBlocked() != _pathBlocked )
    {
        toggleActionFlag(ACTIONFLAG_PATHBLOCKED);
    }
}

bbool Ray_PlayerControllerComponent::playerIsIdle() const
{
    return m_moveInput == Vec2d::Zero &&
           m_jumpQueueTimer == 0.f &&
           m_listenerJump == bfalse &&
         ( m_listenerAttack == bfalse || !isPowerUpEnabled(Ray_PowerUp_Fight) ) &&
          !isJumpQueued() &&
          !isHitQueued();
}

f32 Ray_PlayerControllerComponent::calculateHeight() const
{
    return getTemplate()->getHeight() * m_actor->getScale().m_y;
}

// Check if we die from being crushed by polylines
void Ray_PlayerControllerComponent::checkSquash()
{
    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

    if ( !player || player->getInvincible() )
    {
        return;
    }

    if ( isDead() ||
#ifdef ITF_SUPPORT_CHEAT
         m_currentState == &m_stateFlyMode ||
#endif
         isHanging() ||
         m_onPedestalInterpolateCounter )
    {
        return;
    }

    f32 radius = m_characterPhys->getRadius();
    const f32 penetratingRadius = radius*getTemplate()->getSquashDeathPenetration();
    const PolyLineEdge * crashingEdge = Ray_AIUtils::getSquashingPolyline(m_actor,m_characterPhys->getStickedPolylineRef(), m_characterPhys->getStickedEdge(), radius, penetratingRadius);
    if ( crashingEdge )
    {
        setLastHitNature(HITNATURE_ENVIRONMENTCRUSH);
        triggerDeath(DEATHTYPE_REGULARDEATH);
    }
}

void Ray_PlayerControllerComponent::setFrictionMode( EFrictionMode _frictionMode, f32 _value )
{
    m_frictionMode = _frictionMode;
    m_characterPhys->setFrictionMultiplier(_value);
}

void Ray_PlayerControllerComponent::resetFrictionMode( EFrictionMode _frictionMode )
{
    if ( m_frictionMode == _frictionMode )
    {
        m_frictionMode = FRICTIONMODE_NORMAL;
        m_characterPhys->setFrictionMultiplier(1.f);
    }
}

void Ray_PlayerControllerComponent::setGravityMode( EGravityMode _gravityMode, f32 _value )
{
    if ( _gravityMode != m_gravityMode )
    {
        resetGravityMode(m_gravityMode);
    }

    m_gravityMode = _gravityMode;
    m_gravityModeValue = _value;
    m_characterPhys->setGravityMultiplier(_value);

    switch(m_gravityMode)
    {
        case GRAVITYMODE_SWIMDOLPHIN:
            m_gravityModeDuration = m_gravityModeCounter = getTemplate()->getSwimingSprintJumpGravityDuration();
            m_characterPhys->setAngularSpeedMultiplier(getTemplate()->getSwimingSprintJumpAngularSpeedMult());
            break;
        default:
            m_gravityModeCounter = 0.f;
            break;
    }
}

void Ray_PlayerControllerComponent::resetGravityMode( EGravityMode _gravityMode )
{
    if ( m_gravityMode == _gravityMode )
    {
        switch(m_gravityMode)
        {
            case GRAVITYMODE_SWIMDOLPHIN:
                m_characterPhys->setAngularSpeedMultiplier(1.f);
                break;
        }

        switch(m_stance)
        {
            case STANCE_SWIM:
                m_gravityMode = GRAVITYMODE_SWIM;
                m_characterPhys->setGravityMultiplier(0.f);
                break;
            default:
                m_gravityMode = GRAVITYMODE_NORMAL;
                m_characterPhys->setGravityMultiplier(1.f);
                break;
        }

        m_gravityModeCounter = 0.f;
    }
}

void Ray_PlayerControllerComponent::updateGravityMode( f32 _dt )
{
    if ( m_gravityModeCounter )
    {
        m_gravityModeCounter = Max(m_gravityModeCounter-_dt,0.f);

        if ( m_gravityModeCounter )
        {
            f32 t = 1.f - (m_gravityModeCounter / m_gravityModeDuration);
            t = Clamp(t,0.f,1.f);
            f32 newValue = Interpolate(m_gravityModeValue,1.f,t);
            m_characterPhys->setGravityMultiplier(newValue);
        }
        else
        {
            resetGravityMode(m_gravityMode);
        }
    }
}

void Ray_PlayerControllerComponent::setCrouch( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_CROUCH) != _val )
    {
        toggleActionFlag(ACTIONFLAG_CROUCH);
        setPhantomShape();
    }
}

void Ray_PlayerControllerComponent::setRolling( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_ROLLING) != _val )
    {
        toggleActionFlag(ACTIONFLAG_ROLLING);

        if ( _val )
        {
            setFrictionMode(FRICTIONMODE_ROLLING,getTemplate()->getRollingFrictionMultiplier());
            setCrouch(btrue);
            setSlide(bfalse);
        }
        else
        {
            resetFrictionMode(FRICTIONMODE_ROLLING);
        }
    }
}

void Ray_PlayerControllerComponent::setSprinting( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_SPRINTING) != _val )
    {
        toggleActionFlag(ACTIONFLAG_SPRINTING);

        if ( _val )
        {
            setCrouch(bfalse);
            setSlide(bfalse);
        }
    }
}

bbool Ray_PlayerControllerComponent::trySprint() const
{
    bbool inputActive = (m_listenerSprint || m_sprintTrigger.m_actif);
    return inputActive &&
          !m_moveToPoint &&
          !isSupporting() &&
          !isPerformingSlide() &&
          !isDead();
}

void Ray_PlayerControllerComponent::setLowRoof( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_LOWROOF) != _val )
    {
        toggleActionFlag(ACTIONFLAG_LOWROOF);
        setPhantomShape();
    }
}


void Ray_PlayerControllerComponent::setSupporting( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_SUPPORTING) != _val )
    {
        toggleActionFlag(ACTIONFLAG_SUPPORTING);

        if ( !_val )
        {
            m_objectsOnPedestal.clear();
        }
    }
}

void Ray_PlayerControllerComponent::setLayingDown( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_LAYING_DOWN) != _val )
    {
        toggleActionFlag(ACTIONFLAG_LAYING_DOWN);

        if ( _val )
        {
            setMoveInput(Vec2d::Zero);
        }
    }
}

bbool Ray_PlayerControllerComponent::tryWallSlide( f32 _dt )
{
    if ( m_wallSlideStickDelayCounter || isOnGeyser() || m_characterPhys->getStickOnWalls() )
    {
        return bfalse;
    }

    PolyLine* poly;
    u32 edge;
    bbool floorCollision = findWallSlidingPolyline(ITF_INVALID_OBJREF,U32_INVALID,&poly,&edge,bfalse,btrue);

    if ( poly && edge != U32_INVALID && !floorCollision )
    {
        m_stateWallSlide.forceWallSlideEdge(poly->getRef(),edge);
        return btrue;
    }
    else
    {
        return bfalse;
    }
}

bbool Ray_PlayerControllerComponent::isOnGeyser() const
{
    bbool onGeyser = bfalse;

    u32 numWindForces = m_currentWindForces.size();
    Ray_EventQueryGeyserBehavior queryGeyser;

    for ( u32 i = 0; i < numWindForces; i++ )
    {
        const PhysActorForce& wind = m_currentWindForces[i];

        if ( m_actor->getBinding()->isDescendant(wind.m_actor) )
        {
            continue;
        }

        Actor* windActor = AIUtils::getActor(wind.m_actor);

        if ( !windActor )
        {
            continue;
        }

        windActor->onEvent(&queryGeyser);

        if ( queryGeyser.getHasGeyserBehavior() )
        {
            onGeyser = btrue;
            break;
        }
    }

    return onGeyser;
}

bbool Ray_PlayerControllerComponent::isPolylineEdgeGoodForWallSlide( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, bbool _fromHang ) const
{
    if ( m_characterPhys->getEdgeOrientationType(_edge->m_normalizedVector) != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
    {
        return bfalse;
    }

    const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(_edge->getGameMaterial()));

    if ( mat && mat->getDangerousLevel() > 0 )
    {
        return bfalse;
    }

    if ( !_fromHang )
    {
        f32 cornerCheckDist = getTemplate()->getWallSlideMinEdgeMargin();

        if ( AIUtils::searchEdgeAngle(m_actor->get2DPos(),_poly,_edgeIndex,bfalse,cornerCheckDist,m_currentGravityDir,
                                      0.f,m_characterPhys->getWallAngle(),NULL,NULL) ||
             AIUtils::searchEdgeAngle(m_actor->get2DPos(),_poly,_edgeIndex,btrue,cornerCheckDist,m_currentGravityDir,
                                      0.f,m_characterPhys->getWallAngle(),NULL,NULL) )
        {
            return bfalse;
        }
    }

    return btrue;
}

bbool Ray_PlayerControllerComponent::checkFallOnCharacter()
{
    // We don't do anything if we are not falling
    if ( m_playerDisabled ||
         m_moveToPoint ||
       ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID && m_currentState != &m_stateWallSlide ) ||
         m_currentGravityDir.dot(m_characterPhys->getSpeed()) < getTemplate()->getFallOnVictimMinSpeed() )
    {
        return bfalse;
    }

    PhysContactsContainer contacts;

    CharacterInteractionType interaction = CharacterInteractionType_None;
    Actor* interactActor = NULL;
    Vec2d interactPoint;

    PHYSWORLD->collidePhantoms(m_actor->get2DPos(),m_actor->get2DPos(),
                               m_actor->getAngle(),m_phantomShapeScaled,m_actor->getDepth(),
                               ECOLLISIONFILTER_CHARACTERS,contacts);

    bbool bounceProcessed = bfalse;

    static ObjectRefList processedActors;
    processedActors.clear();
    processedActors.reserve(contacts.size());

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_collidableUserData != m_actor->getRef() )
        {
            i32 i = processedActors.find(contact.m_collidableUserData);
            if (i != -1)
                continue;

            BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

            if ( obj )
            {
                Actor* victim = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if ( victim )
                {
                    processedActors.push_back(contact.m_collidableUserData);

                    // query the victim to see if it can get crushed
                    EventInteractionQuery query;
                    query.setSender(m_actor->getRef());
                    query.setContact(&contact);
                    victim->onEvent(&query);

                    if (query.getInteraction() == CharacterInteractionType_Crush)
                    {
                        interactActor = victim;
                        interactPoint = contact.m_contactPoint;
                        interaction = query.getInteraction();

                        bbool wasDead = bfalse;

                        if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
                        {
                            EventQueryIsDead isDead;

                            interactActor->onEvent(&isDead);

                            wasDead = isDead.getIsDead();
                        }

                        EventCrushed crushEvent(m_actor->getRef());

                        crushEvent.setDirection(Vec2d(0, -1));  // bottom
                        crushEvent.setFxPos( interactPoint.to3d(interactActor->getPos().m_z) );
                        crushEvent.setCharacterSize(getCharacterSize());

                        interactActor->onEvent(&crushEvent);

                        if (crushEvent.getBounce() && !bounceProcessed)
                        {
                            bounceProcessed = btrue;
                            processBounce(0,-m_currentGravityDir,BOUNCETYPE_ENEMY, m_characterPhys->getSpeed(), StringID::Invalid);
                            m_bounceCurrentOrigin = interactPoint;

                            if ( m_windDisableCounter != -1.f && getTemplate()->getWindDisableOnBounce() )
                            {
                                m_windDisableCounter = getTemplate()->getWindDisableOnBounce();
                                m_characterPhys->setDisableWindForces(btrue);
                            }
                        }

                        if ( !wasDead && m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() && m_checkRewardBounce )
                        {
                            EventQueryIsDead isDead;

                            interactActor->onEvent(&isDead);

                            if ( isDead.getIsDead() == btrue )
                            {
                                REWARD_MANAGER->Action_New(Ray_RewardManager::Action_GetBouncedEnemiesSameJump, m_playerIndex);
                            }
                        }
                    }
                }
            }
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::checkLowRoof()
{
    if ( !m_phantomShapeScaled )
    {
        return;
    }

    PhysContactsContainer contacts;

    PHYSWORLD->checkEncroachment(m_phantom->getPos(),m_phantom->getPos(),
        m_phantom->getAngle(),
        m_phantomShapeScaled,
        ECOLLISIONFILTER_ENVIRONMENT,
        m_actor->getDepth(),
        contacts);

    PhysCollisionSolver solver;
    f32 radius = m_characterPhys->getRadius();
    PhysRayCastContactsContainer rayCastContacts;

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        Vec2d contactLocal = contact.m_contactPoint - m_actor->get2DPos();

        contactLocal = contactLocal.Rotate(-m_actor->getAngle());

        // Discard the contact if it's low and on the outsides
        if ( contactLocal.m_y <= radius*0.8 )
        {
            continue;
        }

        BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

        if ( !obj )
        {
            continue;
        }

        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( !poly || ( poly->getOwnerActor() == m_actor ) )
        {
            continue;
        }

        if ( contact.m_edgeIndex >= poly->getPosCount() )
        {
            continue;
        }

        const PolyLineEdge& edge = poly->getEdgeAt(contact.m_edgeIndex);

        const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge.getGameMaterial()));

        if ( mat && ( mat->ignoreLowRoof() || !mat->isSolid() ) )
        {
            continue;
        }

        f32 dotP = edge.m_normalizedVector.dot(m_currentGravityDir.getPerpendicular());

        if ( f32_Abs(dotP) < 0.5f )
        {
            continue;
        }

        const Vec2d& edgePosA = edge.getPos();
        Vec2d edgePosB = edgePosA + edge.m_vector;

        rayCastContacts.clear();

        if ( !PhysCollisionSolver::rayCast(edgePosA,edgePosB,m_phantomShapeScaled,
                                           m_actor->get2DPos(),m_actor->getAngle(),rayCastContacts) )
        {
            continue;
        }

        f32 dist0 = edge.m_length * rayCastContacts[0].m_t0;
        f32 dist1 = edge.m_length * rayCastContacts[0].m_t1;

        if ( f32_Abs(dist0-dist1) < 0.1f )
        {
            continue;
        }

        setLowRoof(btrue);
        return;
    }

    setLowRoof(bfalse);
}

bbool Ray_PlayerControllerComponent::setWaterPunch()
{
    if ( m_dashAttackCooldown )
    {
        return bfalse;
    }

    if ( isPerformingUTurn() && !m_uTurnComboCounter )
    {
        resetUTurn();
    }

    setCurrentPunch(PUNCHTYPE_TORNADO,getTemplate()->getTornadoHitLevel());
    changeState(&m_stateReleaseHit);

    return btrue;
}

// Prepare the state for a standing punch
bbool Ray_PlayerControllerComponent::setIdlePunch()
{
    setDesiredLookDir();

    if ( isPerformingUTurn() && !m_uTurnComboCounter )
    {
        resetUTurn();
    }

    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID && m_characterPhys->getStickOnWalls() )
    {
        const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

        if ( edge )
        {
            StickToPolylinePhysComponent::EdgeOrientationType orientationType = m_characterPhys->getEdgeOrientationType(edge->m_normalizedVector);

            if ( orientationType != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
            {
                if ( m_dashAttackCooldown )
                {
                    return bfalse;
                }
                else
                {
                    if ( isSizeReduced() )
                    {
                        setCurrentPunch(PUNCHTYPE_REDUCEDSIZE,getTemplate()->getReducedSizeHitLevel());
                    }
                    else
                    {
                        setCurrentPunch(PUNCHTYPE_TORNADO,getTemplate()->getTornadoHitLevel());
                    }

                    changeState(&m_stateReleaseHit);
                    return btrue;
                }
            }
        }
    }

    if ( isSizeReduced() )
    {
        if ( m_dashAttackCooldown == 0.f )
        {
            EMoveDir moveDirection = getMoveData().getMoveDirection();

            if ( moveDirection == MOVEDIR_DOWN && !isOnPedestal() )
            {
                if ( m_crushRetriggerCounter == 0.f )
                {
                    m_hitQueueTimer = 0.f;
                    m_currentJumpQueueCrushAttack = btrue;
                    m_stateReleaseHit.setCrushAttackLimitDistance(getTemplate()->getReducedSizeHitCrushDistance());
                    setJumpData();
                    changeState(&m_stateJumping);
                }

                return bfalse;
            }
            else
            {
                if ( getMoveData().getMoveDirection() == MOVEDIR_UP && m_reducedHitUpDisabledUntilLand )
                {
                    return bfalse;
                }
                else
                {
                    setCurrentPunch(PUNCHTYPE_REDUCEDSIZE,getTemplate()->getReducedSizeHitLevel());
                    changeState(&m_stateReleaseHit);
                    return btrue;
                }
            }
        }
        else
        {
            return bfalse;
        }
    }
    else if ( isPerformingUTurn() )
    {
        return startNormalPunch();
    }
    else if ( isRolling() )
    {
        setCurrentPunch(PUNCHTYPE_SPIN,getTemplate()->getSpinHitLevel());
        changeState(&m_stateReleaseHit);
        return btrue;
    }
    else if ( isCrouch() || isLowRoof() )
    {
        setCurrentPunch(PUNCHTYPE_CROUCHKICK,getTemplate()->getCrouchHitLevel());
        changeState(&m_stateReleaseHit);
        return btrue;
    }
    else
    {
        if ( trySprint() )
        {
            if ( m_dashAttackCooldown )
            {
                return bfalse;
            }
            else
            {
                setCurrentPunch(PUNCHTYPE_TORNADO,getTemplate()->getTornadoHitLevel());
                changeState(&m_stateReleaseHit);
                return btrue;
            }
        }
        else
        {
            return startNormalPunch();
        }
    }
}

void Ray_PlayerControllerComponent::setUTurnKick()
{
    setCurrentPunch(PUNCHTYPE_UTURNKICK,getTemplate()->getUTurnHitLevel());
    changeState(&m_stateReleaseHit);
}

Vec3d Ray_PlayerControllerComponent::getHitFxPos( const Vec3d& _default )
{
    if ( m_hitFxBoneIndex == U32_INVALID )
    {
        if ( getTemplate()->getHitFxBoneName().isValid() )
        {
            m_hitFxBoneIndex =  m_animComponent->getBoneIndex(getTemplate()->getHitFxBoneName());

            if ( m_hitFxBoneIndex != U32_INVALID )
            {
                return getBonePos(m_hitFxBoneIndex,_default);
            }
        }
    }
    else
    {
        return getBonePos(m_hitFxBoneIndex,_default);
    }

    return _default;
}

Vec3d Ray_PlayerControllerComponent::getBonePos( u32 _index, const Vec3d& _default ) const
{
    AnimMeshScene* meshScene = m_animComponent->getAnimMeshScene();

    if ( meshScene )
    {
        SafeArray<AnimBoneDyn> & boneDynList = meshScene->m_AnimInfo.getWorkingBoneList();
        if ( _index < boneDynList.size() )
        {
            return boneDynList[_index].m_Pos.to3d(m_actor->getPos().m_z);
        }
    }

    return _default;
}

// Prepare the state for an air punch
bbool Ray_PlayerControllerComponent::setAirPunch()
{
    EMoveDir moveDirection = getMoveData().getMoveDirection();

    if ( moveDirection == MOVEDIR_DOWN && !isOnGeyser() )
    {
        if ( m_crushRetriggerCounter )
        {
            return bfalse;
        }

        setCurrentPunch(PUNCHTYPE_CRUSH,getTemplate()->getCrushHitLevel());
        cancelPhysicAirHit();
        changeState(&m_stateReleaseHit);
    }
    else if ( isSizeReduced() )
    {
        if ( m_dashAttackCooldown == 0.f )
        {
            if ( m_reducedHitUpDisabledUntilLand  && moveDirection == MOVEDIR_UP )
            {
                return bfalse;
            }
            else
            {
                setCurrentPunch(PUNCHTYPE_REDUCEDSIZE,getTemplate()->getReducedSizeHitLevel());
                changeState(&m_stateReleaseHit);
            }
        }
        else
        {
            return bfalse;
        }
    }
    else if ( moveDirection == MOVEDIR_UP )
    {
        setCurrentPunch(PUNCHTYPE_UPPERKICK,getTemplate()->getUpperKickHitLevel());
        changeState(&m_stateReleaseHit);
    }
    else if ( m_stance != STANCE_HELICO && isPerformingUTurn() )
    {
        setCurrentPunch(PUNCHTYPE_UTURNKICK,getTemplate()->getUTurnAirHitLevel());
        changeState(&m_stateReleaseHit);
    }
    else
    {
        setCurrentPunch(PUNCHTYPE_REPEATING,getTemplate()->getBurstAirHitLevel());
        changeState(&m_statePunchMitraille);
    }

    return btrue;
}

void Ray_PlayerControllerComponent::processInteractionQuery(EventInteractionQuery * _event)
{
    if ( m_currentState == &m_stateDeadSoul ||
         (RAY_GAMEMANAGER->getAllowPlayerCrush() && !isSupporting()) )
    {
        _event->setInteraction(CharacterInteractionType_Crush);
    }
    else
    {
        _event->setInteraction(CharacterInteractionType_None);
    }
}

bbool Ray_PlayerControllerComponent::isDetectable( const ActorRef _detector ) const
{
    return !( m_pageAirTravelling && ( m_pageCurActor != _detector ) )
#ifdef ITF_SUPPORT_CHEAT
            && (m_currentState != &m_stateFlyMode)  // not detectable in cheat mode
#endif
            ;
}

void Ray_PlayerControllerComponent::processStargateUse( Ray_EventStargateUse* _event )
{
    if ( m_currentState != &m_stateStargate )
    {
        changeState(&m_stateStargate);
        m_stateStargate.onEvent(_event);
    }
}

void Ray_PlayerControllerComponent::processCheckPedestalSupport( Ray_EventCheckPedestalSupport * _pedestalSupportCheck )
{
    if ( m_onPedestalDisableCounter > 0.f && m_onPedestalDisableRef == _pedestalSupportCheck->getSender() )
    {
        return;
    }

    if ( m_currentState != &m_stateJumping &&
         m_currentState != &m_stateFalling &&
         m_currentState != &m_stateHelicopter )
    {
        return;
    }

    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        return;
    }

    f32 dotProd = m_currentGravityDir.dot(m_characterPhys->getSpeed());

    if ( dotProd < 0.f )
    {
        return;
    }

    f32 feetCheckSQ = getTemplate()->getPedestalFeetDistanceCheck() * (m_actor->getScale().m_x/m_actor->getWorldInitialScale().m_x);

    feetCheckSQ *= feetCheckSQ;

    Vec2d feetPos = m_actor->get2DPos() + ( ( Vec2d::Right.Rotate(m_actor->getAngle()-MTH_PIBY2) ) * m_characterPhys->getRadius() );
    f32 distSq = ( feetPos - _pedestalSupportCheck->getPedestalPos() ).sqrnorm();

    if ( distSq > feetCheckSQ )
    {
        return;
    }

    _pedestalSupportCheck->setTriggerPedestal();
    startOnPedestal(_pedestalSupportCheck->getSender(),_pedestalSupportCheck->getPedestalPos());
}

void Ray_PlayerControllerComponent::processQueryPedestalInfo( Ray_EventQueryPedestalInfo * _pedestalSupportCheck )
{
    if ( !isSupporting() )
    {
        return;
    }

    if ( m_objectsOnPedestal.find(_pedestalSupportCheck->getSender()) == -1 )
    {
        return;
    }

    _pedestalSupportCheck->setValid(btrue);
    _pedestalSupportCheck->setPedestalPos(getPedestalPos());

    u32 numObjects = m_objectsOnPedestal.size();

    for ( u32 i = 0; i < numObjects; i++ )
    {
        _pedestalSupportCheck->addObjectOnPedestal(m_objectsOnPedestal[i]);
    }
}

void Ray_PlayerControllerComponent::processReleasePedestal( Ray_EventReleasePedestal * _releasePedestal )
{
    if ( !isSupporting() )
    {
        return;
    }

    u32 index = m_objectsOnPedestal.find(_releasePedestal->getSender());

    if ( index != -1 )
    {
        m_objectsOnPedestal.eraseNoOrder(index);
    }
}

void Ray_PlayerControllerComponent::processScaleFunnelTrigger( Ray_EventScaleFunnelTrigger * _scaleFunnelTrigger )
{
    if ( _scaleFunnelTrigger->getEnter() )
    {
        EventTrail showTrail(btrue);
        EventShow alphaChange( 0.0f, 0.25f );
        EventStopPlayer stopPlayer(btrue);

        m_actor->onEvent(&alphaChange);
        m_actor->onEvent(&showTrail);
        m_actor->onEvent(&stopPlayer);
    }
    else
    {
        EventTeleport evtTeleport( _scaleFunnelTrigger->getExitPoint().to3d(m_actor->getPos().m_z), 0.0f, bfalse );
        EventShow alphaChange( 1.0f, 0.2f );
        EventTrail showTrail(bfalse);
        EventStopPlayer stopPlayer(bfalse);
        Vec2d originalScale = m_actor->getWorldInitialScale();

        m_actor->setScale(originalScale*_scaleFunnelTrigger->getScale());
        m_actor->onEvent( &evtTeleport );
        m_actor->onEvent(&alphaChange);
        m_actor->onEvent(&showTrail);
        m_actor->onEvent(&stopPlayer);

        if ( _scaleFunnelTrigger->getScale() < 1.f )
        {
            m_fxController->playFX(ITF_GET_STRINGID_CRC(ScaleReducedFx,1232691098));
        }
        else
        {
            m_fxController->playFX(ITF_GET_STRINGID_CRC(ScaleNormalFx,1401567755));
        }
    }
}

void Ray_PlayerControllerComponent::processAINetworkWaypointEvent( class Ray_EventAINetworkWaypoint * _aiNetworkWaypoint )
{
    m_aiNetworkCurrentWaypoint = _aiNetworkWaypoint->getSender();

    if ( m_aiNetworkRegistered )
    {
        RAY_AINETWORKMANAGER->updateWaypoint(m_actor->getRef(),m_aiNetworkCurrentWaypoint);
    }
}

void Ray_PlayerControllerComponent::processSnapDataQuery(EventQuerySnapData * _event)
{
    AnimMeshScene* meshScene = m_animComponent->getAnimMeshScene();

    if ( meshScene )
    {
        u32 characterSnapBoneIndex = U32_INVALID;
        if ( getTemplate()->getCharacterSnapBone().isValid() )
        {
            characterSnapBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getCharacterSnapBone());
        }
        if (characterSnapBoneIndex != U32_INVALID)
        {
            SafeArray<AnimBoneDyn> & boneDynList = meshScene->m_AnimInfo.getWorkingBoneList();

            _event->setPos(boneDynList[characterSnapBoneIndex].m_Pos.to3d(m_actor->getPos().m_z));
            _event->setAngle(boneDynList[characterSnapBoneIndex].m_Angle);
        }
    }
    else
    {
        _event->setPos(m_actor->getPos());
        _event->setAngle(m_actor->getAngle());
    }

    _event->setLookRight(!m_actor->isFlipped());
}

void Ray_PlayerControllerComponent::actorLock()
{
    m_lockPrevPhysics = m_characterPhys->getDisabled();
    m_characterPhys->setDisabled(btrue,bfalse,btrue);
    m_characterPhys->resetForces();
    m_characterPhys->setSpeed(Vec2d::Zero);
    m_actorLocked = btrue;
    m_lockPrevBinding = m_actor->getBinding()->getParent();

    Actor * snapActor = AIUtils::getActor(m_lockSnapActor);

    if (snapActor)
    {
        if (m_actor->getBinding()->getParent() != m_lockSnapActor)
        {
            snapActor->getBinding()->bindChild(m_actor->getRef());
        }
    }

    if ( m_phantom && m_objectsRegistered && m_phantomEnabled )
    {
        PHYSWORLD->removePhantom(m_phantom);
        m_lockPhantomRemoved = btrue;
    }
    else
    {
        m_lockPhantomRemoved = bfalse;
    }
}

void Ray_PlayerControllerComponent::clearActorLock()
{
    m_animComponent->resetLockMagicBox();
    m_actor->getBinding()->unbindFromParent();
    m_characterPhys->setDisabled(m_lockPrevPhysics,bfalse,btrue);
    m_lockSnapActor = ITF_INVALID_OBJREF;
    m_actorLocked = bfalse;

    if ( m_lockPrevBinding != ITF_INVALID_OBJREF )
    {
        BaseObject* obj = GETOBJECT(m_lockPrevBinding);

        if ( obj )
        {
            if ( Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                actor->getBinding()->bindChild(m_actor->getRef());
            }
        }

        m_lockPrevBinding = ITF_INVALID_OBJREF;
    }

    if ( m_lockPhantomRemoved && m_phantom && m_objectsRegistered && m_phantomEnabled )
    {
        PHYSWORLD->insertPhantom(m_phantom, m_actor->getDepth());
    }

    m_lockPhantomRemoved = bfalse;
}

void Ray_PlayerControllerComponent::updateActorLock()
{
    ITF_ASSERT(m_actorLocked);

    if ( m_lockSnapActor != ITF_INVALID_OBJREF )
    {
        Actor * interactionCharacter = AIUtils::getActor(m_lockSnapActor);

        if (interactionCharacter)
        {
            EventQuerySnapData query;

            interactionCharacter->onEvent(&query);

            m_animComponent->lockMagicBox(query.getPos());
            m_actor->setAngle(query.getAngle());
        }
    }
    else if ( isHanging() )
    {
        Vec2d pos;
        f32 angle;

        getWorldHangPos(pos,angle);
        m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));
    }

    updateLookRightFromMoveDirection(getMoveData().getMoveDirection());

}

bbool Ray_PlayerControllerComponent::trySwim( bbool _checkSpeed )
{
    if ( _checkSpeed && m_currentGravityDir.dot(m_characterPhys->getSpeed()) < 0.f )
    {
        // Avoid getting in water right after a jump
        return bfalse;
    }

    u32 numRegions = m_currentRegions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        PolylineRegion& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_Swim )
        {
            PolyLine* poly = AIUtils::getPolyLine(data.m_polyRef);

            if ( poly )
            {
                setSwimingPolyline(poly);
                return btrue;
            }
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::setSwimingPolyline( const PolyLine* _polyLine )
{
    // Process swiming event
    PolyLine* prevPolyLine = AIUtils::getPolyLine(m_swimingPolyline);

    if (prevPolyLine && prevPolyLine->m_ownerFrise)
    {
        EventSwiming swiming(m_actor->getRef(), bfalse, m_characterPhys->getSpeed() + m_characterPhys->getForce()*LOGICDT, 0.f, m_characterPhys->getWaterMultiplier(), m_characterPhys->getWeight(),bfalse);
        prevPolyLine->m_ownerFrise->onEvent(&swiming);
    }

    if ( _polyLine )
    {
        m_swimingPolyline = _polyLine->getRef();

        const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(_polyLine->getEdgeAt(0).getGameMaterial()));

        if ( mat )
        {
            m_swimingWaterResistance = mat->getWaterResistance();
        }

        if ( m_stance != STANCE_SWIM && m_currentState != &m_stateReceiveHit )
        {
            changeState(&m_stateSwimIdle);
        }

        if (_polyLine->m_ownerFrise)
        {
            EventSwiming swiming(m_actor->getRef(), btrue, m_characterPhys->getSpeed() + m_characterPhys->getForce()*LOGICDT, m_actor->getRadius(), m_characterPhys->getWaterMultiplier(), m_characterPhys->getWeight(),bfalse);
            _polyLine->m_ownerFrise->onEvent(&swiming);
        }

        if( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
        {
            checkAchievementPiranha(prevPolyLine, _polyLine);
        }
    }
    else
    {
        if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
        {
            checkAchievementPiranha(prevPolyLine, NULL);
        }

        m_swimingPolyline = ITF_INVALID_OBJREF;
        m_swimingClosestEdge = U32_INVALID;
        m_swimingSurfaceEdge = U32_INVALID;
        m_swimingIsInside = bfalse;
        m_swimingIsOnSurface = bfalse;
        m_swimingIsOnJumpRange = bfalse;
        setStance(STANCE_STAND);
    }
}

void Ray_PlayerControllerComponent::swimingUpdateAirFriction()
{
    f32 waterResist = m_swimingWaterResistance;
    f32 speedSq = m_characterPhys->getSpeed().sqrnorm();

    if ( speedSq > (getTemplate()->getSwimingWaterResistMinSpeed()*getTemplate()->getSwimingWaterResistMinSpeed()) )
    {
        f32 speed = f32_Sqrt(speedSq);

        speed = Clamp(speed,getTemplate()->getSwimingWaterResistMinSpeed(),getTemplate()->getSwimingWaterResistMaxSpeed());

        f32 t = ( speed - getTemplate()->getSwimingWaterResistMinSpeed() ) / ( getTemplate()->getSwimingWaterResistMaxSpeed() - getTemplate()->getSwimingWaterResistMinSpeed() );
        f32 multiplier = Interpolate(1.f,getTemplate()->getSwimingWaterResistMaxMult(),t);

        waterResist *= multiplier;
    }

    if ( m_swimingApplyEnterWaterResistance )
    {
        if ( m_swimingTime > getTemplate()->getSwimingWaterResistEnterDuration() ||
             m_characterPhys->getSpeed().m_y >= 0.f )
        {
            m_swimingApplyEnterWaterResistance = bfalse;
        }
        else
        {
            f32 speedMultiplier = m_swimingEnterDown ? getTemplate()->getSwimingWaterResistEnterDownForceSpeedMultiplier() :
                                                       getTemplate()->getSwimingWaterResistEnterForceSpeedMultiplier();
            Vec2d force = Vec2d::Up * (-m_characterPhys->getSpeed().m_y) * speedMultiplier;

            m_characterPhys->addForce(force);
        }
    }

    m_characterPhys->setAirFrictionMultiplier(waterResist);
}

void Ray_PlayerControllerComponent::swimingUpdatePolylines( const PolyLine* _polyLine, f32 _dt )
{
    swimingUpdateAirFriction();

    AIUtils::getSwimingPolylineData(_polyLine,m_actor->getPos(),m_currentGravityDir,getTemplate()->getSwimingSurfaceRange(),
                                        m_swimingSurfaceEdge,m_swimingSurfaceT,m_swimingIsOnSurface,
                                        m_swimingClosestEdge,m_swimingClosestEdgeT,m_swimingIsInside);

    if ( !m_swimingIsInside )
    {
        u32 numRegions = m_currentRegions.size();

        for ( u32 i = 0; i < numRegions; i++ )
        {
            PolylineRegion& data = m_currentRegions[i];

            if ( data.m_regionId == s_RegionName_Swim )
            {
                PolyLine* poly = AIUtils::getPolyLine(data.m_polyRef);

                if ( poly )
                {
                    _polyLine = poly;
                    m_swimingPolyline = data.m_polyRef;

                    AIUtils::getSwimingPolylineData(_polyLine,m_actor->getPos(),m_currentGravityDir,getTemplate()->getSwimingSurfaceRange(),
                                                    m_swimingSurfaceEdge,m_swimingSurfaceT,m_swimingIsOnSurface,
                                                    m_swimingClosestEdge,m_swimingClosestEdgeT,m_swimingIsInside);
                    break;
                }
            }
        }
    }

    m_swimingIsOnJumpRange = bfalse;

    if ( m_swimingSurfaceEdge != U32_INVALID )
    {
        PolyLine* polyline;
        const PolyLineEdge* edge;

        AIUtils::getPolyLine(m_swimingPolyline,m_swimingSurfaceEdge,polyline,edge);

        if ( edge )
        {
            Vec2d surfacePoint = edge->getPos() + (edge->m_vector * m_swimingSurfaceT);
            f32 distToSurface = ( m_actor->get2DPos() - surfacePoint ).sqrnorm();

            if ( distToSurface <= ( getTemplate()->getSwimingJumpRange() * getTemplate()->getSwimingJumpRange() ) )
            {
                Vec2d checkPos = surfacePoint + Vec2d::Up;
                const RegionsManager::RegionDataList* regionList = REGIONS_MANAGER->getRegions(m_actor->getDepth());

                if ( regionList )
                {
                    for ( u32 i = 0; i < regionList->size(); i++ )
                    {
                        const RegionsManager::RegionData& regionData = (*regionList)[i];

                        if ( regionData.m_id != s_RegionName_Swim )
                        {
                            continue;
                        }

                        PolyLine* poly = AIUtils::getPolyLine(regionData.m_ref);

                        if ( !poly || poly == polyline )
                        {
                            continue;
                        }

                        if ( !poly->isPointInside(checkPos) )
                        {
                            continue;
                        }

                        return;
                    }
                }

                m_swimingIsOnJumpRange = btrue;
            }
        }
    }
}

bbool Ray_PlayerControllerComponent::handleSwimInsideOutside( f32 _dt, const PolyLineEdge* _closestEdge )
{
    if ( !m_swimingIsInside )
    {
        // stay inside the water region
        Vec2d closestPoint = _closestEdge->getPos() + (_closestEdge->m_vector * m_swimingClosestEdgeT);
        static const f32 s_maxDistanceSq = (0.5f*0.5f);

        f32 dist = ( m_actor->get2DPos() - closestPoint ).sqrnorm();

        if ( dist > s_maxDistanceSq )
        {
            setSwimingPolyline(NULL);
            return bfalse;
        }

        Vec2d adjPos = AIUtils::findFreeCollisionPosition(closestPoint,m_characterPhys->getRadius(),m_actor->getPos().m_z,btrue);

        m_actor->updateComponentPosAngle(adjPos.to3d(m_actor->getPos().m_z),m_actor->getAngle(),this);
    }

    return btrue;
}

void Ray_PlayerControllerComponent::UpdatePhysicSwim( f32 _dt )
{
    PolyLine* poly = NULL;
    const PolyLineEdge* surfaceEdge = NULL;
    const PolyLineEdge* closestEdge = NULL;

    AIUtils::getPolyLine(m_swimingPolyline,m_swimingSurfaceEdge,poly,surfaceEdge);
    AIUtils::getPolyLine(m_swimingPolyline,m_swimingClosestEdge,poly,closestEdge);

    if ( !poly || !surfaceEdge || !closestEdge )
    {
        return;
    }

    m_swimingFloatForce = Vec2d::Zero;

    setPathBlocked(bfalse);

    Vec2d startPos = m_actor->get2DPos();

    if ( !handleSwimInsideOutside(_dt,closestEdge) )
    {
        return;
    }

    bbool mustFloat = !isPowerUpEnabled(Ray_PowerUp_Dive);
    bbool performTwist = bfalse;

    if ( mustFloat )
    {
        swimUpdateFloat(_dt,*surfaceEdge,performTwist);
    }
    else
    {
        swimUpdateDive(_dt,*surfaceEdge,performTwist);
    }

    swimApplyForces(_dt,mustFloat);

    UpdatePhysicSwimRotation(_dt,performTwist);

    m_swimingTime += _dt;

    if ( m_moveInput != Vec2d::Zero )
    {
        m_swimingStandPose = 0.f;
    }
    else
    {
        if ( m_swimingStandPose <= getTemplate()->getSwimingTurnUpsideDownWait() )
        {
            m_swimingStandPose += _dt;
        }
    }
}

void Ray_PlayerControllerComponent::swimSetFloatForce( const PolyLineEdge& _surfaceEdge )
{
    Vec2d surfacePoint = _surfaceEdge.getPos() + (_surfaceEdge.m_vector * m_swimingSurfaceT);

    // Pull down the surface point a bit to make sure we are inside the water
    surfacePoint += m_currentGravityDir * 0.1f;

    // Apply the force to be pulled towards the surface
    f32 speedProj = m_currentGravityDir.dot(m_characterPhys->getSpeed());
    Vec2d ySpeed = m_currentGravityDir * speedProj;
    Vec2d dist = surfacePoint - m_actor->get2DPos();

    f32 distMult = getTemplate()->getSwimingSurfaceForceDistMultInside();
    f32 speedMult = getTemplate()->getSwimingSurfaceForceSpeedMultInside();

    Vec2d surfaceForce = ( dist * distMult ) - ( ySpeed * speedMult );

    m_swimingFloatForce = surfaceForce;

    Vec2d forceDir = surfaceForce;

    forceDir.normalize();

    f32 floatSign = forceDir.dot(m_currentGravityDir) > 0.f ? 1.f : -1.f;
    m_swimingFloatForce += m_currentGravityDir*m_swimingWaterResistance*floatSign;
}

void Ray_PlayerControllerComponent::swimProcessPush( Vec2d& _moveDir, Vec2d& _moveAxis )
{
    // First of all lets see if we are trying to move towards a contact. If so then correct the movement
    const PhysContactsContainer& contacts = m_characterPhys->getContacts();

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& c = contacts[i];

        f32 dotProd = _moveDir.dot(c.m_normal);

        if ( dotProd >= getTemplate()->getSwimingPushThreshold() )
        {
            // Pushing
            setPathBlocked(btrue);

            f32 targetRotation = getVec2Angle(_moveDir);
            setSwimTargetRotation(targetRotation);
            m_characterPhys->setSpeed(Vec2d::Zero);

            _moveDir = Vec2d::Zero;
            _moveAxis = Vec2d::Zero;
            break;
        }

        if ( dotProd <= 0.f )
        {
            continue;
        }

        if ( m_swimingIsOnSurface )
        {
            if ( c.m_normal.m_x > 0.f )
            {
                _moveDir = Vec2d::Right;
            }
            else
            {
                _moveDir = -Vec2d::Right;
            }
        }
        else
        {
            f32 crossProd = _moveDir.cross(c.m_normal);

            if ( crossProd < 0.f )
            {
                _moveDir = c.m_normal.getPerpendicular();
            }
            else
            {
                _moveDir = c.m_normal.getPerpendicular()*-1.f;
            }
        }

        break;
    }
}

void Ray_PlayerControllerComponent::swimUpdateDive( f32 _dt, const PolyLineEdge& _surfaceEdge, bbool& _performTwist )
{
    Vec2d moveAxis;
    Vec2d moveDir;

    if ( m_swimingIsOnSurface )
    {
        EMoveDir moveDirection = getMoveDataAnalog().getMoveDirection();

        if ( moveDirection == MOVEDIR_LEFT || moveDirection == MOVEDIR_RIGHT )
        {
            // While in the surface we move in the direction of the polyline to follow its shape
            moveDir = _surfaceEdge.m_normalizedVector;
            moveAxis = moveDir;
            moveAxis *= getMoveDataAnalog().getMoveReal().norm();

            if ( moveDirection == MOVEDIR_LEFT )
            {
                moveDir *= -1.f;
            }

            swimSetFloatForce(_surfaceEdge);
        }
        else if ( moveDirection == MOVEDIR_DOWN )
        {
            // If not on the surface or going down it's free diving
            moveAxis = getMoveDataAnalog().getMoveReal();
            moveDir = getMoveDataAnalog().getMoveRealNormalized();
        }
        else
        {
            swimSetFloatForce(_surfaceEdge);

            if ( m_swimingDashCounter )
            {
                m_moveInput = moveAxis = moveDir = AIUtils::getLookDir(m_actor);
            }
            else
            {
                moveAxis = Vec2d::Zero;
                moveDir = Vec2d::Zero;
            }
        }
    }
    else
    {
        // If not on the surface or going down it's free diving
        moveAxis = getMoveDataAnalog().getMoveReal();
        moveDir = getMoveDataAnalog().getMoveRealNormalized();

        if ( moveDir == Vec2d::Zero && m_swimingDashCounter )
        {
            m_moveInput = moveAxis = moveDir = AIUtils::getLookDir(m_actor);
        }
    }

    bbool canMove = swimCanMove();

    // Swim in direction
    if ( !canMove )
    {
        moveAxis = Vec2d::Zero;
    }

    if ( moveAxis != Vec2d::Zero && moveDir != Vec2d::Zero )
    {
        swimProcessPush(moveDir,moveAxis);
    }

    swimSetSwimForce(_dt,moveDir,moveAxis.norm(),bfalse);

    if ( moveDir != Vec2d::Zero )
    {
        f32 targetRotation = getVec2Angle(moveDir);
        setSwimTargetRotation(targetRotation);
    }
    else
    {
        if ( trySwimSprintJump() )
        {
            // we are about to perform a sprint jump, so let it be
            moveAxis = getMoveDataAnalog().getMoveReal();
            moveDir = getMoveDataAnalog().getMoveRealNormalized();
        }
        else
        {
            if ( !isPathBlocked() && canMove )
            {
                if ( m_characterPhys->getSpeed().sqrnorm() > 1.f &&
                   ( m_currentState == &m_stateReleaseHit || m_currentWindForces.size() > 0 ) )
                {
                    Vec2d speedDir = m_characterPhys->getSpeed();
                    speedDir.normalize();
                    f32 targetRotation = getVec2Angle(speedDir);
                    setSwimTargetRotation(targetRotation);
                }
                else
                {
                    resetSwimTargetRotation(_performTwist);
                }

                m_moveInput = Vec2d::Zero;
            }
        }
    }
}

bbool Ray_PlayerControllerComponent::swimCanMove() const
{
    return m_currentState == &m_stateSwimIdle || m_currentState->DynamicCast<StateHitBase>(ITF_GET_STRINGID_CRC(StateHitBase,676090084)) != NULL;
}

void Ray_PlayerControllerComponent::swimUpdateFloat( f32 _dt, const PolyLineEdge& _surfaceEdge, bbool& _performTwist )
{
    swimSetFloatForce(_surfaceEdge);

    EMoveDir moveDirection = getMoveData().getMoveDirection();

    // Calculate the swiming force
    if ( moveDirection == MOVEDIR_RIGHT ||
         moveDirection == MOVEDIR_LEFT ||
         m_swimingDashCounter )
    {
        Vec2d moveAxis;

        if ( m_swimingIsOnSurface || !m_swimingIsInside )
        {
            // Move along the edge
            moveAxis = _surfaceEdge.m_normalizedVector;
        }
        else
        {
            // Move freely
            moveAxis = m_currentGravityDir.getPerpendicular();
        }

        // Adjust the look direction of the player
        f32 targetRotation = getVec2Angle(_surfaceEdge.m_normalizedVector);

        Vec2d moveDir = moveAxis;

        // Adjust the look direction of the player
        if ( moveDirection == MOVEDIR_LEFT )
        {
            moveDir *= -1.f;
            targetRotation += MTH_PI;
        }
        else if ( moveDirection == MOVEDIR_NONE && m_swimingDashCounter )
        {
            if ( m_actor->isFlipped() )
            {
                moveDir *= -1.f;
                targetRotation += MTH_PI;
            }

            m_moveInput = moveDir;
        }

        if ( swimCanMove() )
        {
            swimSetSwimForce(_dt,moveDir,getMoveData().getMove().norm(),btrue);
        }

        setSwimTargetRotation(targetRotation);
    }
    else
    {
        resetSwimTargetRotation(_performTwist);
        m_moveInput.m_x = 0.f;
    }

    m_moveInput.m_y = 0.f;
}

void Ray_PlayerControllerComponent::swimSetSwimForce( f32 _dt, Vec2d& _moveDir, f32 _moveSize, bbool _float )
{
    Vec2d targetMove;
    f32 topSpeed = getTemplate()->getSwimingTopSpeed();
    f32 swimingForce = getTemplate()->getSwimingForce();

    if ( isSprinting() )
    {
        topSpeed *= getTemplate()->getSwimingSprintMaxSpeedMultiplier();
    }

    if ( _moveSize && _moveDir != Vec2d::Zero )
    {
        // Set the swiming force
        targetMove = _moveDir*_moveSize*topSpeed;

        f32 blendTime = getMoveTargetBlendTime();

        if ( m_moveTargetBlendTimer < blendTime )
        {
            targetMove *= m_moveTargetBlendTimer / blendTime;
        }
    }
    else
    {
        targetMove = Vec2d::Zero;
    }

    Vec2d desiredSpeed = targetMove;
    Vec2d currentSpeed = m_characterPhys->getSpeed();

    if ( m_currentState == &m_stateReleaseHit ||
         isSprinting() ||
         m_swimingDashCounter ||
         m_hitQueueTimer > m_dashAttackCooldown ||
         m_timeSinceDashAttack < getTemplate()->getSwimAttackSpamTime() )
    {
        // In this state we want to damp our rotation
        f32 targetSpeed = desiredSpeed.norm();
        Vec2d desiredDir;

        if ( targetSpeed == 0.f )
        {
            desiredDir = AIUtils::getLookDir(m_actor);
        }
        else
        {
            desiredDir = desiredSpeed;
            desiredDir.normalize();
        }

        Vec2d currentDir;

        if ( m_characterPhys->getSpeed().IsEqual(Vec2d::Zero,0.5f) )
        {
            currentDir = AIUtils::getLookDir(m_actor);
        }
        else
        {
            currentDir = m_characterPhys->getSpeed();
            currentDir.normalize();
        }

        f32 dotProd = desiredDir.dot(currentDir);
        f32 angle = f32_ACos(dotProd);

        if ( f32_Abs(angle) >= (MTH_PI*0.75f) && m_currentState != &m_stateReleaseHit && m_hitQueueTimer > m_dashAttackCooldown )
        {
            if ( !isPerformingUTurn() && !m_uTurnFlipped )
            {
                m_swimingTwist = bfalse;
                startUTurn(UTURNTYPE_SWIM);
            }
        }
        else
        {
            f32 sign = desiredDir.cross(currentDir) < 0.f ? 1.f : -1.f;
            f32 angle = f32_ACos(dotProd);

            angle *= getTemplate()->getSwimAttackMoveDamping();

            desiredDir = currentDir.Rotate(angle*sign);

            _moveDir = desiredDir;

            desiredSpeed = desiredDir * targetSpeed;
            targetMove = desiredSpeed;
        }
    }

    f32 xFactor, yFactor;

    if ( getSign(desiredSpeed.m_x) == getSign(currentSpeed.m_x) )
    {
        xFactor = 1.f - (currentSpeed.m_x/desiredSpeed.m_x);
        xFactor = Clamp(xFactor,0.f,1.f);
    }
    else
    {
        xFactor = 1.f;
    }

    if ( getSign(desiredSpeed.m_y) == getSign(currentSpeed.m_y) )
    {
        yFactor = 1.f - (currentSpeed.m_y/desiredSpeed.m_y);
        yFactor = Clamp(yFactor,0.f,1.f);
    }
    else
    {
        yFactor = 1.f;
    }

    m_currentTargetForce.m_x = targetMove.m_x * xFactor;
    m_currentTargetForce.m_y = targetMove.m_y * yFactor;
    m_currentTargetForce *= swimingForce;
}

void Ray_PlayerControllerComponent::swimApplyForces( f32 _dt, bbool _mustFloat )
{
    bbool floating = m_swimingFloatForce != Vec2d::Zero;
    Vec2d swimForce = m_currentTargetForce;

    if ( floating && !m_swimingIsOnSurface )
    {
        Vec2d rightDir = m_currentGravityDir.getPerpendicular();

        swimForce = rightDir * rightDir.dot(swimForce);
    }

    m_characterPhys->addForce(swimForce);

    if ( floating )
    {
        m_characterPhys->addForce(m_swimingFloatForce);
    }
}

void Ray_PlayerControllerComponent::UpdatePhysicSwimRotation( f32 _dt, bbool _performTwist )
{
    f32 currentAngle = m_actor->getAngle();

    if ( _performTwist )
    {
        currentAngle = m_swimingTargetRotation;
        m_actor->setFlipped(!m_actor->isFlipped());
        m_swimingTwist = btrue;
    }
    else
    {
        bbool lookRight = !m_actor->isFlipped();
        bbool performingUTurn = isPerformingUTurn() && m_performingUTurn;
        bbool canMove = swimCanMove();

        // First we transform the angle to "world" space, removing the flipping
        bbool piAdded = bfalse;

        if ( performingUTurn )
        {
            // While doing the uturn we assume the character is already in his final rotation
            if ( !m_uTurnFlipped )
            {
                lookRight = !lookRight;
            }
        }

        if ( !lookRight )
        {
            currentAngle += MTH_PI;
            piAdded = btrue;
        }

        // Then we calculate the current angular speed
        f32 angleDif = getShortestAngleDelta(currentAngle,m_swimingTargetRotation);
        f32 curveDif = angleDif;

        // Check if we need to do a Turn or a Roll
        if ( !performingUTurn )
        {
            if ( canMove && f32_Abs(angleDif) >= (MTH_PI*0.75f) && m_currentState != &m_stateReleaseHit )
            {
                m_swimingTwist = bfalse;
                startUTurn(UTURNTYPE_SWIM);

                curveDif = 0.f;
                m_swimingCurveSpeed = 0.f;
                m_swimingCurveCursor = 0.5f;
                m_swimingCurveTarget = 0.5f;
            }
            else
            {
                if ( m_moveInput == Vec2d::Zero )
                {
                    curveDif = 0.f;
                }
            }
        }
        else
        {
            curveDif = 0.f;
        }

        UpdatePhysicSwimingCurve(_dt,curveDif);

        f32 force = (angleDif*getTemplate()->getSwimingTurnDistMult()) - (m_swimingCurrentAngularSpeed*getTemplate()->getSwimingTurnSpeedMult());

        m_swimingCurrentAngularSpeed += force*_dt;

        currentAngle += m_swimingCurrentAngularSpeed * _dt;

        NormalizeAnglePI(currentAngle);

        if ( piAdded )
        {
            currentAngle -= MTH_PI;
        }
    }

    m_actor->updateComponentPosAngle(m_actor->getPos(),currentAngle,this);
}

void Ray_PlayerControllerComponent::UpdatePhysicSwimingCurve( f32 _dt, f32 targetDif )
{
    m_swimingCurveTarget = targetDif / getTemplate()->getSwimingCurveMaxDif();
    m_swimingCurveTarget = Clamp(m_swimingCurveTarget,-getTemplate()->getSwimingCurveMaxOffset(),getTemplate()->getSwimingCurveMaxOffset());
    m_swimingCurveTarget = 0.5f + m_swimingCurveTarget;

    bbool invert = ( m_actor->isFlipped() && !isPerformingUTurn() ) || ( !m_actor->isFlipped() && isPerformingUTurn() );

    if ( invert )
    {
        m_swimingCurveTarget = 1.f - m_swimingCurveTarget;
    }

    f32 force = ( ( m_swimingCurveTarget - m_swimingCurveCursor ) * getTemplate()->getSwimingCurveForceS() ) -
                ( m_swimingCurveSpeed * getTemplate()->getSwimingCurveForceD() );

    m_swimingCurveSpeed += force * _dt;
    m_swimingCurveCursor += m_swimingCurveSpeed * _dt;
    m_swimingCurveCursor = Clamp(m_swimingCurveCursor,0.f,1.f);
}

void Ray_PlayerControllerComponent::setSwimTargetRotation( f32 _target )
{
     m_swimingTargetRotation = _target;
     NormalizeAnglePI(m_swimingTargetRotation);
}

void Ray_PlayerControllerComponent::resetSwimTargetRotation( bbool& _performTwist )
{
    f32 targetRotation = getVec2Angle(m_currentGravityDir.getPerpendicular());
    f32 anglePi = m_actor->getAngle();
    bbool lookingRight = !m_actor->isFlipped();

    if ( isPerformingUTurn() && !m_uTurnFlipped )
    {
        // If we are doing a uturn we assume always we are already at the target rotation
        lookingRight = !lookingRight;
    }

    if ( !lookingRight )
    {
        anglePi += MTH_PI;
    }

    NormalizeAnglePI(anglePi);

    anglePi = f32_Abs(anglePi);

    bbool upsideDown = ( anglePi <= (MTH_PIBY2*0.5f) && !lookingRight ) ||
                       ( anglePi >= (MTH_PI*0.75f) && lookingRight );

    if ( upsideDown )
    {
        if ( m_swimingStandPose >= getTemplate()->getSwimingTurnUpsideDownWait() )
        {
            _performTwist = btrue;
        }
        else
        {
            if ( lookingRight )
            {
                targetRotation += MTH_PI;
            }
        }
    }
    else
    {
        if ( !lookingRight )
        {
            targetRotation += MTH_PI;
        }
    }

    m_swimingTargetRotation = targetRotation;
}

bbool Ray_PlayerControllerComponent::trySwimSprintJump()
{
    if ( !isSprinting() || getMoveData().getMoveDirection() != MOVEDIR_UP )
    {
        return bfalse;
    }

    PolyLine* poly;
    const PolyLineEdge* closestEdge;

    AIUtils::getPolyLine(m_swimingPolyline,m_swimingClosestEdge,poly,closestEdge);

    if ( !poly || !closestEdge )
    {
        return bfalse;
    }

    if ( !m_swimingIsOnSurface )
    {
        Vec2d vecToPos = m_actor->get2DPos() - closestEdge->getPos();
        f32 dist = closestEdge->m_normalizedVector.dot(vecToPos);
        Vec2d edgePos = closestEdge->getPos() + (closestEdge->m_normalizedVector*dist);
        f32 distFromEdgeSq = ( edgePos - m_actor->get2DPos() ).sqrnorm();
        bbool nearEdge = distFromEdgeSq < (getTemplate()->getSwimingSurfaceRange()*getTemplate()->getSwimingSurfaceRange());

        if ( !nearEdge )
        {
            return bfalse;
        }
    }

    f32 dotProd = Vec2d::Right.dot(closestEdge->m_normalizedVector);
    bbool horizontal = dotProd >= 0.5f;

    if ( !horizontal )
    {
        return bfalse;
    }

    Vec2d speed = m_characterPhys->getSpeed();

    f32 ySpeed = speed.dot(Vec2d::Up);

    if ( ySpeed < getTemplate()->getSwimingSprintJumpMinSpeed() )
    {
        return bfalse;
    }

    Vec2d vecToPos = m_actor->get2DPos() - closestEdge->getPos();
    f32 dist = closestEdge->m_normalizedVector.dot(vecToPos);
    Vec2d edgePos = closestEdge->getPos() + (closestEdge->m_normalizedVector*dist);
    Vec2d checkPos = edgePos + Vec2d::Up;

    const RegionsManager::RegionDataList* regionList = REGIONS_MANAGER->getRegions(m_actor->getDepth());

    if ( regionList )
    {
        for ( u32 i = 0; i < regionList->size(); i++ )
        {
            const RegionsManager::RegionData& regionData = (*regionList)[i];

            if ( regionData.m_id != s_RegionName_Swim )
            {
                continue;
            }

            PolyLine* polyRegion = AIUtils::getPolyLine(regionData.m_ref);

            if ( !polyRegion || polyRegion == poly )
            {
                continue;
            }

            if ( !polyRegion->isPointInside(checkPos) )
            {
                continue;
            }

            return bfalse;
        }
    }

    return btrue;
}

void Ray_PlayerControllerComponent::setupSwimDolphinJump()
{
    resetSwimUpsideDown();

    setGravityMode(GRAVITYMODE_SWIMDOLPHIN,getTemplate()->getSwimingSprintJumpGravityMult());
    changeState(&m_stateFalling);

    Vec2d currentSpeed = m_characterPhys->getSpeed();
    f32 xSpeed = currentSpeed.dot(m_currentGravityDir.getPerpendicular());
    f32 ySpeed = getTemplate()->getSwimingDolphinYSpeed();

    m_characterPhys->setSpeed(Vec2d(xSpeed,ySpeed));
    m_jumpQueueTimer = 0.f;
    m_wallSlideStickDelayCounter = m_wallJumpDisableTimer = getTemplate()->getSwimingDolphinWallJumpDisableTimer();
}

void Ray_PlayerControllerComponent::resetSwimUpsideDown()
{
    resetUTurn();

    f32 gravityAngle = getVec2Angle(m_currentGravityDir.getPerpendicular());
    f32 difWithGravity = getShortestAngleDelta(gravityAngle,m_actor->getAngle());

    if ( f32_Abs(difWithGravity) > MTH_PIBY2 )
    {
        // We are upside down, flip the character so that he is ok
        f32 targetAngle;

        if ( m_actor->getAngle() > 0.f )
        {
            targetAngle = m_actor->getAngle() - MTH_PI;
        }
        else
        {
            targetAngle = m_actor->getAngle() + MTH_PI;
        }

        if ( !m_actor->isFlipped() )
        {
            setLookRight(bfalse);
        }
        else
        {
            setLookRight(btrue);
        }

        m_actor->setAngle(targetAngle);
        m_swimingTargetRotation = targetAngle;
    }
}

bbool Ray_PlayerControllerComponent::shouldExitSwiming() const
{
    if ( !m_swimingIsInside )
    {
        const PhysContactsContainer& _contactList = m_characterPhys->getContacts();
        u32 numContacts = _contactList.size();
        PolyLine* poly;
        const PolyLineEdge* edge;
        f32 radiusSq = m_characterPhys->getRadius();

        radiusSq *= radiusSq;

        for ( u32 i = 0; i < numContacts; i++ )
        {
            const SCollidableContact& c = _contactList[i];

            if ( c.m_edgeIndex == U32_INVALID )
            {
                continue;
            }

            AIUtils::getPolyLine(c.m_collidableUserData,c.m_edgeIndex,poly,edge);

            if ( !edge )
            {
                continue;
            }

            StickToPolylinePhysComponent::EdgeOrientationType edgeOrientation = m_characterPhys->getEdgeOrientationType(edge->m_normalizedVector);

            if ( edgeOrientation != StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
            {
                continue;
            }

            Vec2d vecToPos = m_actor->get2DPos() - edge->getPos();
            f32 dist = edge->m_normalizedVector.dot(vecToPos);

            dist = Clamp(dist,0.f,edge->m_length);

            Vec2d edgePos = edge->getPos() + ( edge->m_normalizedVector * dist );

            vecToPos = edgePos - m_actor->get2DPos();

            f32 distToPosSq = vecToPos.sqrnorm();

            if ( distToPosSq < (radiusSq*0.8f) )
            {
                continue;
            }

            vecToPos.normalize();

            f32 dotProd = vecToPos.dot(m_currentGravityDir);

            if ( dotProd > 0.f )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::processSwingLatchEvent( Ray_EventSwingLatch* _event )
{
    m_swingOnActor = _event->getSender();
    m_swingLength = _event->getLength();
    m_swingMaxAngle = _event->getAngle();
    m_swingGravMultiplier = _event->getGravityMultiplier();
}

void Ray_PlayerControllerComponent::processQueryHangInfo( Ray_EventQueryHangInfo* _event )
{
    if ( m_coopActionMode != ACTIONCOOPMODE_VICTIM && m_coopActionMode != ACTIONCOOPMODE_MAIN_VICTIM )
    {
        if ( isClimbing() )
        {
            _event->setIsClimb(btrue);
            _event->setPoly(m_climbingPolyline);
            _event->setEdge(m_climbingEdge);
        }
        else if ( isHanging() )
        {
            _event->setIsHang(btrue);
            _event->setPoly(m_hangingPolyline);
            _event->setEdge(m_hangingEdge);
        }

        _event->setT(m_climbingT);
        _event->setOrientationPose(m_orientationPose);
    }
}

void Ray_PlayerControllerComponent::processHangOnCharacter( Ray_EventHangOnCharacter* _event )
{
    if ( _event->getHanging() )
    {
        if ( m_coopActionMain != ITF_INVALID_OBJREF )
        {
            m_coopActionMode = ACTIONCOOPMODE_MAIN_VICTIM;
        }
        else
        {
            m_coopActionMode = ACTIONCOOPMODE_MAIN;
        }

        m_coopActionVictim = _event->getSender();
    }
    else
    {
        m_coopActionVictim = ITF_INVALID_OBJREF;

        if ( m_coopActionMode == ACTIONCOOPMODE_MAIN_VICTIM )
        {
            m_coopActionMode = ACTIONCOOPMODE_VICTIM;
        }
        else
        {
            m_coopActionMode = ACTIONCOOPMODE_NONE;
        }
    }
}

void Ray_PlayerControllerComponent::processPlayerActivationChanged( EventPlayerActivationChanged* _eventActivation )
{
    if ( m_playerIndex != U32_INVALID )
    {
        Ray_Player* player = static_cast<Ray_Player*>(_eventActivation->getPlayer());
        bbool isSolePlayer = (GAMEMANAGER->getNumActivePlayers() == 1);

        if ( player && player->getIndex() == m_playerIndex )
        {
            if ( player->getActive() && player->getCurrentMode() == RAY_GAMEMODE_PLATFORMER)
            {
                // register data that may have been removed before
                registerDatas();

                // we just joined the game
                m_justActivated = btrue;
                m_justActivatedFrame = CURRENTFRAME;

                if ( _eventActivation->getStartLevel() )
                {
                    CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef());
                    changeState(&m_stateStartLevel);

                    if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
                    {
                        player->setWasHitInLevel(bfalse);

                        Ray_ShooterGameModeController* shooterController = RAY_GAMEMANAGER->getShooterGameModeController();

                        if ( shooterController && shooterController->getShooterGameModeParameters() != NULL )
                        {
                            player->setSprintReleased(btrue);
                        }
                        else
                        {
                            player->setSprintReleased(bfalse);
                        }
                    }
                }
                else
                {
                    // someone else was already playing
                    RAY_GAMEMANAGER->getCurrentScore().setLums(getPlayerIndex(), 0);

                    // restore control
                    EventStopPlayer stopPlayer(bfalse);
                    m_actor->onEvent(&stopPlayer);

                    bbool joinAlive = !_eventActivation->getStartDead();
                    teleportToCurrentSpawnPoint(joinAlive);

                    if (joinAlive)
                    {
                        // start alive
                        CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef());
                        changeState(&m_stateFalling);
                        player->setHitPoints(RAY_GAMEMANAGER->getStartHitPointsAfterDeath());
                    }
                    else
                    {
                        // start dead
                        changeState(&m_stateDeadSoul);
                        player->setHitPoints(0);
                    }
                }
            }
            else
            {
                if ( player->getActive() && ( player->getCurrentMode() == RAY_GAMEMODE_SHOOTER ) )
                {
                    m_justActivated = btrue;
                    m_justActivatedFrame = CURRENTFRAME;
                }

                CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());

                if ( m_coopActionMode == ACTIONCOOPMODE_MAIN )
                {
                    m_actor->getBinding()->unbindChild(m_coopActionVictim);
                    m_actor->getBinding()->forceRemovePending();
                }

                detachFromEverything();

                if ( !isDead() )
                {
                    changeState(&m_stateIdle);
                }
            }
        }
        else
        {
            Player* myPlayer = GAMEMANAGER->getPlayer(m_playerIndex);
            if (!myPlayer || (myPlayer && !myPlayer->getActive()) )
                return;

            if ( player && !player->getActive() )
            {
                // someone else left the game, if we are alone we trigger the death sequence
                if ( isSolePlayer && isDead() && m_playerIndex == GAMEMANAGER->getMainIndexPlayer() && !GAMEMANAGER->isLoadingMap() )
                {
                    RAY_GAMEMANAGER->triggerRestoreDeath(m_actor->getRef());
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::teleportToCurrentSpawnPoint( bbool& _joinAlive )
{
    // Get all live players, sorted along map direction, and get the most lagging one.
    // If he's inside the current checkpoint, spawn at the checkpoint. If not, at the other player position.

    Actor* otherPlayer(NULL);
    Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();
    DetectorComponent* detector = checkpoint && checkpoint->isActive() ? checkpoint->GetComponent<DetectorComponent>() : NULL;
    CheckpointComponent* checkpointComp = NULL;
    bbool useCheckpoint = bfalse;

	Actor* buffer[8];
	ActorList players(8,MemoryId::mId_Temporary,buffer);
	//static ActorList players;
	//players.clear();
    AIUtils::getLivePlayersForGameMode( players, RAY_GAMEMODE_PLATFORMER );      // => take only same game mode actors
    Vec2d mapDir;

    if ( checkpoint && CAMERACONTROLLERMANAGER->getMapDirection( mapDir, checkpoint->getPos() ))
    {
        AIUtils::sortOnDirection(mapDir, players);
    }
    else
    {
        ITF_WARNING(0, 0, "Map has no direction! Please put a camera modifier with the proper direction.");
    }

	for (u32 i = 0; i < players.size(); i++)
    {
        Actor* playerActor = players[i];

        if ( playerActor == m_actor )
        {
            continue;
        }

        otherPlayer = playerActor;

        if ( detector )
        {
            useCheckpoint = detector->getActorsInside().find(playerActor->getRef()) != -1;
        }

        break;
    }

    if ( !otherPlayer )
    {
        return;
    }

    f32 sceneZ;
    Scene* currentSceneReference = RAY_GAMEMANAGER->getCurrentSceneReference();
    const SubSceneActor* subSceneActorReference = currentSceneReference ? currentSceneReference->getSubSceneActorHolder() : NULL;

    if ( subSceneActorReference)
    {
        sceneZ = subSceneActorReference->getPos().m_z;
    }
    else if ( checkpoint )
    {
        checkpointComp = checkpoint->GetComponent<CheckpointComponent>();

        sceneZ = AIUtils::getSceneZFromPickable(checkpoint);
    }
    else
    {
        sceneZ = AIUtils::getSceneZFromPickable(otherPlayer);
    }

    Vec3d otherPos;

    if ( useCheckpoint && checkpointComp )
    {
        otherPos = checkpointComp->getPlayerSpawnPos(m_playerIndex).to3d();
        if (checkpointComp->getJoinAlive())
        {
            _joinAlive = btrue;
        }
    }
    else
    {
        otherPos = otherPlayer->getPos();

        f32 scale = otherPlayer->getScale().m_x/otherPlayer->getWorldInitialScale().m_x;

        m_actor->setScale(m_actor->getWorldInitialScale()*scale);
    }

    otherPos.m_z = sceneZ + m_actor->getTemplate()->getForcedZ();
    EventTeleport teleport(otherPos,m_actor->getAngle(),btrue);
    m_actor->onEvent(&teleport);
}

void Ray_PlayerControllerComponent::processPlayerHpChanged( EventPlayerHpChanged* _eventHP )
{
    if ( _eventHP->getPlayer()->getIndex() == m_playerIndex )
    {
        if ( _eventHP->getPlayer()->getCurrentMode() == RAY_GAMEMODE_SHOOTER ||
             static_cast<Ray_Player*>(_eventHP->getPlayer())->getIsLeaving() )
            return;

        if ( _eventHP->getJustRevived() )
        {
            changeState(&m_stateIdle);
        }
        else if ( _eventHP->getJustDied() )
        {
            changeState(&m_stateDeadSoul);
        }
    }
}

void Ray_PlayerControllerComponent::processRevertToLastCheckpoint( EventRevertToLastCheckpoint* _eventRevert )
{
    if ( m_playerIndex == U32_INVALID )
    {
        return;
    }

    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

    if ( !player || !player->getActive() )
    {
        return;
    }

    if ( isDead() )
    {
        cleanListener();
        m_runToggleActive = bfalse;  // Reset toggle when dead
        m_prevRunButtonPressed = bfalse;
        UpdateMoveDir();
        resetDesiredLookDir();
        changeState(&m_stateIdle);

        player->setHitPoints(RAY_GAMEMANAGER->getStartHitPointsAfterDeath());

        EventReset resetEvent;
        m_actor->onEvent(&resetEvent);
    }

    m_stateDeath.destroyFX();

    m_lastHitNature = HITNATURE_UNKNOWN;
    m_actor->setScale(m_actor->getWorldInitialScale()*RAY_GAMEMANAGER->getPlayersCurrentScale());

    detachFromEverything();
    enablePhantom(btrue);
    resetPendingEvents();

    // Reset toggle state on checkpoint reload (even if not dead)
    m_runToggleActive = bfalse;
    m_prevRunButtonPressed = bfalse;
    resetBounceEdgeCheck();
    m_pageActive = bfalse;
    m_pageAirTravelling = bfalse;
    m_forceFindGround = btrue;
    m_inputDisabled = bfalse;
    m_characterPhys->setFrictionMultiplier(1.f);
    m_characterPhys->setSpeedMultiplier(Vec2d::One);
    m_characterPhys->setGravityMultiplier(1.f);
    m_frictionMode = FRICTIONMODE_NORMAL;
    m_gravityMode = GRAVITYMODE_NORMAL;
    REWARD_MANAGER->Action_TimerStop(Ray_RewardManager::Timer_IsGhost, m_playerIndex);
    REWARD_MANAGER->Action_TimerStart(Ray_RewardManager::Timer_IsAlive, m_playerIndex);

    // Seb : 15-09-11 Fix just here to avoid potential unknown bugs...
    if ( player->getCurrentMode() != RAY_GAMEMODE_SHOOTER )
    {
        CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef());
    }

}

void Ray_PlayerControllerComponent::resetPendingEvents()
{
    m_enterDoorReceived = bfalse;
    m_changePageReceived = bfalse;
    m_stargateReceived = bfalse;
    m_scaleDoorReceived = bfalse;
    m_scaleTunnelReceived = bfalse;
    m_jumpToPosReceived = bfalse;
    m_snapDanceReceived = bfalse;
    m_throwTeethReceived = bfalse;
    m_playAnimReceived = bfalse;
    m_punchEventReceived = bfalse;
}

void Ray_PlayerControllerComponent::disablePlayer( bbool _disable )
{
    if ( _disable == bfalse )
    {
        m_animComponent->enableDraw(btrue);
        resetDesiredLookDir();
        if ( m_runToggleActive )
        {
            m_listenerSprint = btrue;
        }
    }
    else
    {
        detachFromEverything();
        m_moveInput = Vec2d::Zero;
        m_characterPhys->setSpeed(Vec2d::Zero);
        m_characterPhys->resetForces();

        if ( !isDead() )
        {
            if ( m_currentState == &m_stateCaughtInPipe )
            {
                Actor* pipeActor = AIUtils::getActor(m_pipeRef);

                if ( pipeActor )
                {
                    Ray_EventCaughtInPipe pipeEvent;
                    pipeEvent.setCatch(bfalse);
                    pipeEvent.setSender(m_actor->getRef());
                    pipeActor->onEvent(&pipeEvent);
                }
            }

            setIdleState();
        }
    }

    resetMoveToPoint();
    m_playerDisabled = _disable;
    m_inputDisabled = _disable;
    setSprinting(bfalse);
    setRolling(bfalse);
    enablePhantom(_disable == bfalse);
    m_characterPhys->setDisabled(_disable,bfalse,btrue);
}

void Ray_PlayerControllerComponent::setIdleState()
{
    if ( trySwim(bfalse) )
    {
        if ( m_currentState != &m_stateSwimIdle )
        {
            changeState(&m_stateSwimIdle);
        }
    }
    else
    {
        if ( m_currentState != &m_stateIdle )
        {
            changeState(&m_stateIdle);
        }
    }
}

void Ray_PlayerControllerComponent::processChangePage( Ray_EventChangePage* _eventChangePage )
{
	Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

	ITF_ASSERT( player != NULL );

	if ( !player->getActive() )
    {
		return;
    }

    if ( m_currentState == &m_stateCaught )
    {
        Actor* handsActors = AIUtils::getActor(m_handsRef);

        if ( handsActors )
        {
            Ray_EventHandsRelease handRelease;
            handRelease.setSender(m_actor->getRef());
            handsActors->onEvent(&handRelease);
            triggerDeath(DEATHTYPE_REGULARDEATH);
        }
    }

    if ( isOnPedestal() )
    {
        stopOnPedestal();
    }

    // store page actor
    m_pageCurActor = _eventChangePage->getPageActor();

	// Changing page (air travel)
	//----------------------------
	if ( _eventChangePage->isAirTravelActive() )
	{
        detachFromEverything();
        disableSoftCollision();
        disablePlayer(btrue);
        m_pageAirTravelling = btrue;
        m_pageActive = btrue;

        if ( !isDead() )
        {
            Actor* targetPage = _eventChangePage->getTargetPageActor().getActor();

            if ( targetPage )
            {
                if ( !isPosInWater(targetPage->getPos()) )
                {
                    if ( m_currentState != &m_stateIdle )
                    {
                        changeState(&m_stateIdle);
                    }
                }
            }
            else
            {
                Actor* currentPage = _eventChangePage->getPageActor().getActor();

                if ( currentPage )
                {
                    if ( !isPosInWater(currentPage->getPos()) )
                    {
                        if ( m_currentState != &m_stateIdle )
                        {
                            changeState(&m_stateIdle);
                        }
                    }
                }
            }
        }

        // remove binding
        // if the player is very far, parent object can be inactive then force unbind player
        if ( m_actor->getBinding()->isBound() )
        {
            m_actor->getBinding()->unbindFromParent();

            ObjBinding* parentBinding = ObjBinding::getBindingFromObj( m_actor->getBinding()->getParent() );
            if ( parentBinding != NULL )
            {
                parentBinding->forceRemovePending();
            }
        }

        if ( _eventChangePage->getHideActor() )
        {
		    EventShow alphaChange( 0.0f, 0.25f );
		    m_actor->onEvent(&alphaChange);
        }

        CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());

        if ( m_actor->getRef() != RAY_GAMEMANAGER->getChangePageTriggerer() )
        {
            m_fxController->playFX(ITF_GET_STRINGID_CRC(MRK_Vortex_Voyage_End,3558498901));

            // trail / alpha
            EventTrail showTrail(btrue);
            m_actor->onEvent( &showTrail );
        }
	}
	// Enter page : move to
	//-------------------------
	else if ( _eventChangePage->isEnterMovePosActive() )
	{
		// Trail / alpha
        if ( isDead() && m_currentState != &m_stateRevive )
        {
            if ( m_currentState != &m_stateDeadSoul )
            {
                changeState(&m_stateDeadSoul);
            }
        }
        else
        {
            if ( trySwim(bfalse) )
            {
                if ( m_currentState != &m_stateSwimIdle )
                {
                    changeState(&m_stateSwimIdle);
                }
            }
            else
            {
                if ( m_currentState != &m_stateIdle )
                {
                    changeState(&m_stateIdle);
                }
            }
        }

		EventTrail showTrail(bfalse,btrue);
		m_actor->onEvent( &showTrail );

        if ( _eventChangePage->getHideActor() )
        {
		    EventShow alphaChange( 1.0f, 0.2f );
		    m_actor->onEvent(&alphaChange);
        }

		disablePlayer(bfalse);
        m_pageAirTravelling = bfalse;

        if ( _eventChangePage->getEnterEjectForce() == Vec2d::Zero )
        {
            setMoveToPoint(_eventChangePage->getEnterMovePos().truncateTo2D(),0.1f);
            m_characterPhys->forceFindStickEdge();
            m_prevPos = m_actor->get2DPos();
        }
        else
        {
            m_characterPhys->addForce( _eventChangePage->getEnterEjectForce() );
        }

        // Check water case
        if ( m_swimingIsInside && ( AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim, m_actor->get2DPos() ) == NULL ) )
        {
            setSwimingPolyline(NULL);
        }

        if ( !isDead() )
        {
            CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef());
        }
	}
    else if ( _eventChangePage->getPlayFx() )
    {
        Actor* page = AIUtils::getActor(_eventChangePage->getSender());

        if ( page )
        {
            Ray_ChangePageComponent *changePageComponent = page->GetComponent<Ray_ChangePageComponent>();
            if (changePageComponent && !changePageComponent->isWarpZoneExit())
            {
                m_fxController->playFX(ITF_GET_STRINGID_CRC(ChangePageLastPlayerFx,2645763052));
            }
        }
    }
	else
	{
        m_pageAirTravelling = bfalse;
        m_pageActive = bfalse;
        restoreSoftCollision();
		disablePlayer(bfalse);

        EventTrail showTrail(bfalse,btrue);
        m_actor->onEvent( &showTrail );
	}
}

void Ray_PlayerControllerComponent::processPlayerModeChanged( EventPlayerModeChanged* _eventGameMode )
{
    Player* player = nullptr;
    if (_eventGameMode->getActivator().isValid())
        player = GAMEMANAGER->getPlayerFromActor(_eventGameMode->getActivator());

    if ( player && player->getActive() )
    {
        // Reset toggle state on game mode change
        m_runToggleActive = bfalse;
        m_prevRunButtonPressed = bfalse;

        switch ( _eventGameMode->getMode() )
        {
        case RAY_GAMEMODE_SHOOTER:
            {
                stopSlideFx();
                stopWallSlideFx();
                stopHelicoFx();
                stopLianaSlideFx();
                // deactivate platformer actor
                m_actor->disable();
                m_heartShield.show( bfalse );
                CAMERACONTROLLERMANAGER->unregisterSubject(m_actor->getRef());

                // remove data from AI manager
                unregisterDatas();
            }
            break;

        case RAY_GAMEMODE_PLATFORMER:
            {
                // register data
                registerDatas();

                // activate platformer actor
                m_actor->enable();

                // restore control
                EventStopPlayer stopPlayer(bfalse);
                m_actor->onEvent(&stopPlayer);

                bbool isAlive = !( player->isDead() );

                // has just been activated, teleport on spawn point
                if ( m_justActivated && ( m_justActivatedFrame == CURRENTFRAME ) )
                {
                    teleportToCurrentSpawnPoint( isAlive );
                }
                else
                {
                    // teleport to where the shooter actor was
                    Actor* shooterActor = (Actor*)GETOBJECT( player->getActorForMode(RAY_GAMEMODE_SHOOTER) );
                    //ITF_ASSERT_MSG(shooterActor, "shooter actor not found");
                    if (shooterActor)
                    {
                        Vec3d otherPos = shooterActor->getPos();
                        // consider the shooter actor pos on the good default Z
                        // (the shooter actor scene is always the player scene with Z=0, but in shooter start/stop shooter Z can be different from 0)
                        otherPos.m_z += /*AIUtils::getSceneZFromPickable(shooterActor) +*/ m_actor->getTemplate()->getForcedZ();

                        EventTeleport teleport(otherPos,m_actor->getAngle(),btrue);
                        m_actor->onEvent(&teleport);
                    }
                }

                // is dead, switch to deadsoul
                if ( !isAlive )
                {
                    changeState( &m_stateDeadSoul );
                    break;
                }
                else
                {
                    computeCurrentRegions();        // to recompute the current region from the new player actor position
                    setIdleState();
                }

                m_heartShield.show( btrue );

                CAMERACONTROLLERMANAGER->registerSubject(m_actor->getRef(), 0.0f, btrue, bfalse );

                if ( SHOOTERCAMERA != NULL )
                {
                    // teleport only the platform camera controller to have a good blend with the shooter cam
                    const SafeArray<CameraControllerData>& camControllerList = CAMERACONTROLLERMANAGER->getCameraControllerList();
                    ICameraController * camModifier = NULL;
                    for ( u32 i = 0; i < camControllerList.size(); ++i )
                    {
                        const CameraControllerData & camData = camControllerList[i];
                        if ( camData.m_controller != SHOOTERCAMERA && ( camData.m_controller != NULL ) )
                        {
                            if ( Actor * cam = AIUtils::getActor( camData.m_controller->getActorRef() ) )
                            {
                                if ( cam->GetComponent<Ray_PlatformerCameraComponent>() != NULL )
                                {
                                    camModifier = camData.m_controller;
                                    break;
                                }
                            }
                        }
                    }
                    if ( camModifier != NULL )
                    {
                        camModifier->teleport( m_actor->getPos() );
                    }
                    CAMERACONTROLLERMANAGER->m_controllerJustTeleported = btrue;
                }
            }
            break;
        }
    }
}

void Ray_PlayerControllerComponent::processPlayerModeSet( EventPlayerModeSet* _eventGameModeSet )
{
    RAY_GAMEMANAGER->changePlayerGameMode( GAMEMANAGER->getPlayerFromActor(m_actor->getRef()), _eventGameModeSet->getMode() );

    //EventPlayerModeChanged eventGameMode;
    //eventGameMode.setMode(_eventGameModeSet->getMode());
    //eventGameMode.setActivator(m_actor->getRef());

    //// Broadcast
    //GAMEMANAGER->onEvent(&eventGameMode);
    //EVENTMANAGER->broadcastEvent(&eventGameMode);
}

void Ray_PlayerControllerComponent::processTeleport( EventTeleport* _eventTeleport )
{
    Super::processTeleport(_eventTeleport);

    resetMoveToPoint();
    restoreSoftCollision();
    resetUTurn();
    resetDesiredLookDir();

    if ( _eventTeleport->getReset() )
    {
        if ( !isDead() && m_currentState != &m_stateStartLevel )
        {
            changeState(&m_stateFalling);
        }
    }
    else
    {
        Ray_DoorTeleport* doorTeleport = _eventTeleport->DynamicCast<Ray_DoorTeleport>(ITF_GET_STRINGID_CRC(Ray_DoorTeleport,3845549645));

        if ( doorTeleport )
        {
            const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

            if ( !edge )
            {
                f32 entryAngle = doorTeleport->getEntryAngle();
                f32 exitAngle = doorTeleport->getExitAngle();

                f32 angleDif = getShortestAngleDelta(entryAngle,exitAngle);
                Vec2d currentSpeed = m_characterPhys->getSpeed().Rotate(angleDif);

                m_characterPhys->setSpeed(currentSpeed);
            }
            else
            {
                f32 currentSpeed = m_characterPhys->getSpeed().norm();
                Vec2d newSpeed = edge->m_normalizedVector*currentSpeed;

                if ( _eventTeleport->getFlip() )
                {
                    newSpeed *= -1.f;
                }

                m_characterPhys->setSpeed(newSpeed);
            }
        }
    }

    if ( _eventTeleport->getOverrideFlip() )
    {
        if ( !m_actor->isFlipped() != !_eventTeleport->getFlip() )
        {
            setLookRight(!_eventTeleport->getFlip());
            m_desiredLookDir = !m_actor->isFlipped() ? MOVEDIR_RIGHT : MOVEDIR_LEFT;
            flipMoveData();
        }
    }

    m_prevCurrentRegion = ITF_INVALID_OBJREF;
    m_prevCurrentMove = Vec2d::Zero;
}

void Ray_PlayerControllerComponent::processPlayerIndexChange( EventPlayerIndexChange* _playerIndex )
{
    Super::processPlayerIndexChange(_playerIndex);
    m_heartShield.setPlayerIndex(m_playerIndex);
    m_superPunchBasicGauge.setPlayerIndex(m_playerIndex);
    m_superPunchSeekingGauge.setPlayerIndex(m_playerIndex);
    m_swarmRepellerPowerUp.setPlayerIndex(m_playerIndex);

    if ( m_playerIndex != U32_INVALID )
    {
        if ( !m_listenerAdded && m_actor->isEnabled() )
        {
            addListener();
        }
    }
    else
    {
        if ( m_listenerAdded )
        {
            removeListener();
        }
    }
}

void Ray_PlayerControllerComponent::flipMoveData()
{
    getMoveData().flip();
    getMoveDataAnalog().flip();
    getMoveDataGravity().flip();
    getMoveDataGravityAnalog().flip();
}

void Ray_PlayerControllerComponent::processMagnetTrigger( Ray_EventMagnetTrigger* _magnetTrigger )
{
    if ( _magnetTrigger->getActivated() )
    {
        m_currentMagnet = _magnetTrigger->getSender();
        m_magnetForce = _magnetTrigger->getMagnetForce();
    }
    else
    {
        m_currentMagnet = ITF_INVALID_OBJREF;
        m_magnetForce = 0;
    }
}

Vec2d Ray_PlayerControllerComponent::getMagnetForce( bbool _allowMagnetAttraction )
{
    if ( _allowMagnetAttraction && m_currentMagnet != ITF_INVALID_OBJREF )
    {
        Actor* magnet = AIUtils::getActor(m_currentMagnet);
        if (magnet)
        {
            Vec2d delta = magnet->get2DPos() - m_actor->get2DPos();
            return delta * m_magnetForce;
        }
    }

    return Vec2d::Zero;
}

void Ray_PlayerControllerComponent::prepareClimbingJump()
{
    Ray_State* newState;

    m_currentJumpForceSideways = bfalse;

    if ( m_coopActionMode == ACTIONCOOPMODE_VICTIM || m_coopActionMode == ACTIONCOOPMODE_MAIN_VICTIM )
    {
        // Take the data of the climbing or hanging from the master before jumping
        Actor* coopMaster = AIUtils::getActor(m_hangingActor);

        if ( coopMaster )
        {
            Ray_EventQueryHangInfo queryEvent;

            coopMaster->onEvent(&queryEvent);

            if ( queryEvent.getPoly() != ITF_INVALID_OBJREF )
            {
                Ray_EventHangOnCharacter hangEvent(m_actor->getRef(),bfalse);
                coopMaster->onEvent(&hangEvent);

                m_climbingT = queryEvent.getT();

                if ( queryEvent.getIsClimb() )
                {
                    m_climbActivationCounter = getTemplate()->getClimbEdgeActivationTime();
                    m_previousClimbingPolyline = queryEvent.getPoly();
                }
                else
                {
                    m_hangingPolyline = queryEvent.getPoly();
                }
            }
        }

        newState = &m_stateJumping;
    }
    else
    {
        if ( m_coopActionMode == ACTIONCOOPMODE_MAIN )
        {
            // Tell the victim to jump too
            Actor* coopVictim = AIUtils::getActor(m_coopActionVictim);

            if ( coopVictim )
            {
                Ray_EventHangCoopJump jumpEvent;
                coopVictim->onEvent(&jumpEvent);
            }
        }

        if ( isHanging() )
        {
            // If we were hanging check if we need to blend to the vertical axis first or not
            Vec2d hangPos;
            f32 hangAngle;

            getWorldHangPos(hangPos,hangAngle);

            f32 gravAngle = getVec2Angle(m_currentGravityDir.getPerpendicular());
            f32 angleDif = getShortestAngleDelta(hangAngle,gravAngle);

            if ( m_actor->isFlipped() )
            {
                angleDif *= -1.f;
            }

            if ( m_moveInput.m_x < 0.f )
            {
                startUTurn(UTURNTYPE_GROUND);
            }

            if ( angleDif >= 0.f )
            {
                newState = &m_stateJumping;
            }
            else
            {
                newState = &m_stateHangToJump;
            }
        }
        else
        {
            newState = &m_stateJumping;
        }
    }

    if ( isHanging() )
    {
        setJumpData(getTemplate()->getJumpMultiplierHang(),getTemplate()->getJumpMultiplierNoImpulseHang(),m_moveInput.m_x < 0.f ? getTemplate()->getJumpImpulseXHang() : 0.f);
    }
    else
    {
        if ( m_climbingVertical )
        {
            f32 xImpulse = getTemplate()->getJumpImpulseXClimbV();

            if ( trySprint() )
            {
                xImpulse *= 2.f;
            }

            m_currentJumpForceSideways = m_climbingForceSidewaysJump;

            setJumpData(getTemplate()->getJumpMultiplierClimbV(),getTemplate()->getJumpMultiplierNoImpulseClimbV(),xImpulse);
        }
        else
        {
            if ( m_climbingSlide )
            {
                m_currentJumpForceSideways = btrue;
                m_currentJumpKeepSpeedX = btrue;
                m_currentJumpForceDisableAirControl = getTemplate()->getClimbSlideDisableAirControl();
            }
            else
            {
                m_currentJumpForceSideways = m_climbingForceSidewaysJump;
            }

            setJumpData(getTemplate()->getJumpMultiplierClimbH(),getTemplate()->getJumpMultiplierNoImpulseClimbH(),getTemplate()->getJumpImpulseXClimbH());
        }
    }

    changeState(newState);
}

void Ray_PlayerControllerComponent::removeAllHealth()
{
    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));
    i32 hitPoints = static_cast<i32>(player->getHitPoints());

    if ( hitPoints > 0 )
    {
        player->setHeartTier(HeartTier_None);
        player->addHitPoints(-hitPoints,btrue);
    }
}

void Ray_PlayerControllerComponent::receiveDamage( i32 _points )
{
    PunchStim punchStim;
    punchStim.setSender(m_receivedHitInstigator);
    punchStim.setDirection(m_receivedHitDir);
    punchStim.setLevel(m_receivedHitLevel);
    punchStim.setReceivedHitType(m_receivedHitType);
    const StringID feedbackAction = punchStim.getActionFromHit();

    u32 fxHandle = m_fxController->playFeedback(m_receivedHitInstigator, feedbackAction, m_actor->getRef());
    m_fxController->setFXPosFromHandle(fxHandle, m_receivedHitFXPos);

    if ( isImmune() || CHEATMANAGER->isPlayerInvincible() )
    {
        return;
    }

    if (_points != 0)
    {
        // http://mtp-mk-server/trac/ticket/1392
        static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex))->addHitPoints(-1);

        m_inmunityCounter = getTemplate()->getInmunityTime();
        m_inmunityCounterReceiveHit = getTemplate()->getInmunityTimeReceiveHit();
    }
}

bbool Ray_PlayerControllerComponent::isDead() const
{
    Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

    ITF_ASSERT_MSG(player,"An actor with a player component is used as an NPC");

    if ( player && player->getHitPoints() <= 0 )
    {
        return btrue;
    }

    if ( m_currentState == &m_stateDeath || m_currentState == &m_stateDeadSoul )
    {
        return btrue;
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::isImmune() const
{
    return m_inmunityCounter != 0.f ||
           isDead();
}

void Ray_PlayerControllerComponent::UpdateSupport(f32 _dt)
{
    Vec2d supportPos = getPedestalPos();

    if ( supportPos == Vec2d::Zero )
    {
        return;
    }

    f32 scale = (m_actor->getScale().m_x/m_actor->getWorldInitialScale().m_x);
    f32 areaRadius = getTemplate()->getPedestalAreaRadius()*scale;
    PhysShapeCircle circle(areaRadius);
    PhysContactsContainer contacts;
    f32 distCheckSq = getTemplate()->getPedestalDistanceCheck() * scale;

    distCheckSq *= distCheckSq;

    PHYSWORLD->collidePhantoms(supportPos,supportPos,m_actor->getAngle(),&circle,m_actor->getDepth(),(ECollisionFilter)(ECOLLISIONFILTER_CHARACTERS),contacts);

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_collidableUserData == m_actor->getRef() )
        {
            continue;
        }

        Actor* actor = AIUtils::getActor(contact.m_collidableUserData);

        if ( !actor )
        {
            continue;
        }

        if ( m_objectsOnPedestal.find(contact.m_collidableUserData) != -1 )
        {
            continue;
        }

        f32 distSq = ( supportPos - actor->get2DPos() ).sqrnorm();

        if ( distSq > distCheckSq )
        {
            continue;
        }

        Ray_EventCheckPedestalSupport pedestalCheck(m_actor->getRef(),supportPos);

        actor->onEvent(&pedestalCheck);

        if ( pedestalCheck.getTriggerPedestal() )
        {
            m_objectsOnPedestal.push_back(actor->getRef());
        }
    }
}

void Ray_PlayerControllerComponent::UpdateSupported(f32 _dt)
{
    Actor* pedestalActor = AIUtils::getActor(m_onPedestal);

    if ( !pedestalActor )
    {
        m_onPedestalValid = bfalse;
        return;
    }

    Ray_EventQueryPedestalInfo pedestalInfo(m_actor->getRef());

    pedestalActor->onEvent(&pedestalInfo);

    if ( !pedestalInfo.getValid() )
    {
        m_onPedestalValid = bfalse;
        return;
    }

    m_onPedestalAllowFall = pedestalInfo.getAllowFallFromMoveDown();

    m_onPedestalPos = pedestalInfo.getPedestalPos();
    m_onPedestalPos -= m_currentGravityDir * m_characterPhys->getRadius();

    f32 targetAngle = getVec2Angle(m_currentGravityDir.getPerpendicular());

    if ( m_onPedestalInterpolateCounter == 0.f )
    {
        m_actor->set2DPos(m_onPedestalPos);
        m_actor->setAngle(targetAngle);
    }
    else
    {
        f32 bounceHeight = getTemplate()->getOnPedestalLandBounceHeight() * (m_actor->getScale().m_x/m_actor->getWorldInitialScale().m_x);
        f32 t = 1.f - (m_onPedestalInterpolateCounter/s_onPedestalInterpolateDuration);

        t = Clamp(t,0.f,1.f);

        f32 newPosX = Interpolate(m_onPedestalStartPos.m_x,m_onPedestalPos.m_x,t);
        f32 newPosY;

        if ( t < 0.5f )
        {
            f32 tInterp = 1.f - (t / 0.5f);
            tInterp = 1.f - (tInterp*tInterp);
            newPosY = m_onPedestalStartPos.m_y + Interpolate(0.f,bounceHeight,tInterp);
        }
        else
        {
            f32 tInterp = (t-0.5f) / 0.5f;
            tInterp = (tInterp*tInterp);
            newPosY = Interpolate(m_onPedestalStartPos.m_y + bounceHeight,m_onPedestalPos.m_y,tInterp);
        }

        f32 dif = getShortestAngleDelta(m_actor->getAngle(),targetAngle);
        f32 newAngle = m_actor->getAngle() + (dif*t);

        m_actor->set2DPos(Vec2d(newPosX,newPosY));
        m_actor->setAngle(newAngle);

        m_onPedestalInterpolateCounter = Max(m_onPedestalInterpolateCounter-_dt,0.f);
    }
}

Vec2d Ray_PlayerControllerComponent::getPedestalPos() const
{
    Vec2d pedestalPos = Vec2d::Zero;

    if ( m_pedestalBoneIndex == U32_INVALID )
    {
        m_pedestalBoneIndex = m_animComponent->getBoneIndex(ITF_GET_STRINGID_CRC(B_Snap_Character,453384700));

        if ( m_pedestalBoneIndex != U32_INVALID )
        {
            m_animComponent->getBonePos(m_pedestalBoneIndex,pedestalPos);
        }
    }
    else
    {
        m_animComponent->getBonePos(m_pedestalBoneIndex,pedestalPos);
    }

    return pedestalPos;
}

void Ray_PlayerControllerComponent::startOnPedestal( ObjectRef _pedestalActor, const Vec2d& _pedestalPos )
{
    m_onPedestal = _pedestalActor;
    m_onPedestalValid = btrue;
    m_onPedestalPos = _pedestalPos;
    m_onPedestalPos -= m_currentGravityDir * m_characterPhys->getRadius();
    m_onPedestalInterpolateCounter = s_onPedestalInterpolateDuration;
    m_onPedestalStartPos = m_actor->get2DPos();

    m_characterPhys->setDisabled(btrue);
    m_characterPhys->setSpeed(Vec2d::Zero);

    Actor* pedestalActor = AIUtils::getActor(m_onPedestal);

    if ( pedestalActor )
    {
        m_onPedestalLastPos = pedestalActor->get2DPos();
    }
    else
    {
        m_onPedestalLastPos = Vec2d::Zero;
    }
}

void Ray_PlayerControllerComponent::stopOnPedestal()
{
    if ( isOnPedestal() )
    {
        Actor* pedestalActor = AIUtils::getActor(m_onPedestal);

        if ( pedestalActor )
        {
            Ray_EventReleasePedestal releaseEvent(m_actor->getRef());

            pedestalActor->onEvent(&releaseEvent);
        }

        m_onPedestalDisableCounter = getTemplate()->getPedestalDisableTimer();
        m_onPedestalDisableRef = m_onPedestal;
        m_onPedestal = ITF_INVALID_OBJREF;
        m_onPedestalInterpolateCounter = 0.f;
        m_characterPhys->setDisabled(bfalse);
    }
}

void Ray_PlayerControllerComponent::checkSetPedestal( bbool& exitCornerPose, bbool& allowSupport, bbool& allowCrouch )
{
    bbool validInput = bfalse;
    EMoveDir moveDirection = getMoveData().getMoveDirection();

    if ( moveDirection == MOVEDIR_UP )
    {
        Vec2d moveDir = getMoveDataAnalog().getMoveReal();

        if ( !moveDir.IsEqual(Vec2d::Zero,MTH_EPSILON) )
        {
            moveDir.normalize();

            f32 moveAngle = getVec2Angle(moveDir);

            moveAngle -= MTH_PIBY2;

            if ( moveAngle < getTemplate()->getPedestalInputCone().ToRadians() &&
                 moveAngle > -getTemplate()->getPedestalInputCone().ToRadians() )
            {
                validInput = btrue;
            }
        }
    }

    if ( isCrouch() )
    {
        allowSupport = bfalse;
    }
    else
    {
        if ( !allowSupport )
        {
            allowSupport = !validInput;
        }
    }

    bbool supporting = bfalse;

    if ( allowSupport && !isLowRoof() && !isSprinting() )
    {
        if ( isSupporting())
        {
            allowCrouch = bfalse;

            if ( m_objectsOnPedestal.size() == 0 )
            {
                //no one on top
                if ( !validInput )
                {
                    supporting = bfalse;
                }
                else
                {
                    supporting = btrue;
                }
            }
            else
            {
                if ( moveDirection == MOVEDIR_DOWN && !isLayingDown())
                {
                    setLayingDown(btrue);
                }

                if ( isLayingDown() && m_animComponent->isMainNodeFinished() )
                {
                    setLayingDown(bfalse);
                    supporting = bfalse;
                }
                else
                {
                    supporting = btrue;
                }
            }
        }
        else
        {
            supporting = validInput;
        }
    }

    setSupporting(supporting);

    if ( supporting )
    {
        exitCornerPose = btrue;
    }
}

void Ray_PlayerControllerComponent::onBecomeActive()
{
    Super::onBecomeActive();

    m_squadData = RAY_AVOIDANCEMANAGER->addMember(m_actor->getRef());

    updateSquadData(0);

    if ( !m_listenerAdded )
    {
        addListener();
    }

    if ( !m_aiNetworkRegistered )
    {
        RAY_AINETWORKMANAGER->registerActor(m_actor->getRef());

        if ( m_aiNetworkCurrentWaypoint != ITF_INVALID_OBJREF )
        {
            RAY_AINETWORKMANAGER->updateWaypoint(m_actor->getRef(),m_aiNetworkCurrentWaypoint);
        }

        m_aiNetworkRegistered = btrue;
    }

    if ( RAY_GAMEMANAGER->getCurrentLevelName() == ITF_GET_STRINGID_CRC(FO_A1,856121799) )
    {
        m_checkRewardBounce = btrue;
    }
    else
    {
        m_checkRewardBounce = bfalse;
    }

    // restore power ups displays
    for (u32 i=0; i<m_powerUpDisplays.size(); ++i)
    {
        m_powerUpDisplays[i]->onBecomeActive();
    }
}

void Ray_PlayerControllerComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    RAY_AVOIDANCEMANAGER->removeMember(m_actor->getRef());
    m_squadData = NULL;

    if ( m_listenerAdded )
    {
        removeListener();
    }

    if ( m_aiNetworkRegistered )
    {
        RAY_AINETWORKMANAGER->unregisterActor(m_actor->getRef());
        m_aiNetworkRegistered = bfalse;
    }

    // kill power ups displays
    for (u32 i=0; i<m_powerUpDisplays.size(); ++i)
    {
        m_powerUpDisplays[i]->onBecomeInactive();
    }
}

void Ray_PlayerControllerComponent::updateSquadData( f32 _deltaTime )
{
    ITF_ASSERT(m_squadData);
    if (m_squadData)
    {
        m_squadData->setRadius( getTemplate()->getAvoidanceRadius() );
        m_squadData->setPosition( m_actor->get2DPos() );
        m_squadData->setDestination( m_actor->get2DPos() );   // TODO: add speed here?
        m_squadData->setIsBlocked( btrue );
    }
}

Ray_PlayerControllerComponent::ECornerPose Ray_PlayerControllerComponent::calculateCornerPose() const
{
    ECornerPose cornerPose = CORNERPOSE_NONE;

    if ( isPerformingUTurn() )
    {
        return cornerPose;
    }

    PolyLine* polyLine = m_characterPhys->getStickedPolyline();
    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

    if ( !edge || !polyLine )
    {
        return cornerPose;
    }

    const Ray_GameMaterial_Template * mat =  (const Ray_GameMaterial_Template*)World::getGameMaterial(edge->getGameMaterial());
    if (mat && mat->ignoreCorners())
    {
        return cornerPose;
    }

    f32 relDist = m_characterPhys->getRelativeEdgeDistance();
    u32 edgeIndex = m_characterPhys->getStickedEdgeIndex();
    static const f32 s_edgeLimit = 0.8f;
    static const f32 s_edgeAngle = -0.5f;

    if ( relDist <= getTemplate()->getCornerPoseExtraDist() )
    {
        bbool onCorner;
        bbool current;

        PolyLine* prevPoly;
        u32 prevIndex;

        AIUtils::getAdjacentEdge(polyLine,edgeIndex,bfalse,prevPoly,prevIndex);

        if ( prevIndex == U32_INVALID )
        {
            onCorner = btrue;
            current = bfalse;
        }
        else
        {
            const PolyLineEdge& prevEdge = prevPoly->getEdgeAt(prevIndex);
            const PolyLineEdge& currentEdge = polyLine->getEdgeAt(edgeIndex);

            f32 angleSign = prevEdge.m_normalizedVector.cross(currentEdge.m_normalizedVector);

            if ( angleSign < 0.f )
            {
                StickToPolylinePhysComponent::EdgeOrientationType prevEdgeOrientation = m_characterPhys->getEdgeOrientationType(prevEdge.m_normalizedVector);
                StickToPolylinePhysComponent::EdgeOrientationType currentEdgeOrientation = m_characterPhys->getEdgeOrientationType(currentEdge.m_normalizedVector);

                if ( prevEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL &&
                     currentEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
                {
                    onCorner = btrue;
                    current = bfalse;
                }
                else if ( prevEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND &&
                          currentEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
                {
                    onCorner = btrue;
                    current = btrue;
                }
                else
                {
                    onCorner = bfalse;
                    current = bfalse;
                }
            }
            else
            {
                onCorner = bfalse;
                current = bfalse;
            }
        }

        if ( onCorner )
        {
            if ( !m_actor->isFlipped() )
            {
                if ( current )
                {
                    cornerPose = CORNERPOSE_FRONT;
                }
                else
                {
                    cornerPose = CORNERPOSE_BACK;
                }
            }
            else
            {
                if ( current )
                {
                    cornerPose = CORNERPOSE_BACK;
                }
                else
                {
                    cornerPose = CORNERPOSE_FRONT;
                }
            }
        }
    }
    else if ( relDist >= edge->m_length - getTemplate()->getCornerPoseExtraDist() )
    {
        bbool onCorner;
        bbool current;

        PolyLine* nextPoly;
        u32 nextIndex;

        AIUtils::getAdjacentEdge(polyLine,edgeIndex,btrue,nextPoly,nextIndex);

        if ( nextIndex == U32_INVALID )
        {
            onCorner = btrue;
            current = bfalse;
        }
        else
        {
            const PolyLineEdge& nextEdge = nextPoly->getEdgeAt(nextIndex);
            const PolyLineEdge& currentEdge = polyLine->getEdgeAt(edgeIndex);

            f32 angleSign = currentEdge.m_normalizedVector.cross(nextEdge.m_normalizedVector);

            if ( angleSign < 0.f )
            {
                StickToPolylinePhysComponent::EdgeOrientationType nextEdgeOrientation = m_characterPhys->getEdgeOrientationType(nextEdge.m_normalizedVector);
                StickToPolylinePhysComponent::EdgeOrientationType currentEdgeOrientation = m_characterPhys->getEdgeOrientationType(currentEdge.m_normalizedVector);

                if ( nextEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL &&
                     currentEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
                {
                    onCorner = btrue;
                    current = bfalse;
                }
                else if ( nextEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND &&
                          currentEdgeOrientation == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL )
                {
                    onCorner = btrue;
                    current = btrue;
                }
                else
                {
                    onCorner = bfalse;
                    current = bfalse;
                }
            }
            else
            {
                onCorner = bfalse;
                current = bfalse;
            }
        }

        if ( onCorner )
        {
            if ( !m_actor->isFlipped() )
            {
                if ( current )
                {
                    cornerPose = CORNERPOSE_BACK;
                }
                else
                {
                    cornerPose = CORNERPOSE_FRONT;
                }
            }
            else
            {
                if ( current )
                {
                    cornerPose = CORNERPOSE_FRONT;
                }
                else
                {
                    cornerPose = CORNERPOSE_BACK;
                }
            }
        }
    }

    if ( cornerPose != CORNERPOSE_NONE )
    {
        if ( isPerformingUTurn() )
        {
            if ( cornerPose == CORNERPOSE_FRONT )
            {
                cornerPose = CORNERPOSE_BACK;
            }
            else
            {
                cornerPose = CORNERPOSE_FRONT;
            }
        }
    }

    return cornerPose;
}

void Ray_PlayerControllerComponent::exitCornerPose( ECornerPose _pose, f32 _dt )
{
    f32 edgeDir;

    if ( _pose == CORNERPOSE_BACK )
    {
        edgeDir = !m_actor->isFlipped() ? 1.f : -1.f;
    }
    else
    {
        edgeDir = !m_actor->isFlipped() ? -1.f : 1.f;
    }

    const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

    if ( edge )
    {
        Vec2d dir = edge->m_normalizedVector*edgeDir;
        Vec2d newPos = m_actor->get2DPos() + (dir*_dt);
        m_actor->updateComponentPosAngle(newPos.to3d(m_actor->getPos().m_z),m_actor->getAngle(),this);
    }
}

void Ray_PlayerControllerComponent::processBounceStim( BounceStim* _stim )
{
    if ( isDead() ||
         m_playerDisabled ||
         m_currentState == &m_stateBounceToLayer ||
#ifdef ITF_SUPPORT_CHEAT
         m_currentState == &m_stateFlyMode ||
#endif
         m_stance == STANCE_SWING ||
         m_stance == STANCE_HANG ||
         m_stance == STANCE_CLIMB )
    {
        _stim->setSkipped(btrue);
        return;
    }

    Vec2d bounceDir;

    if ( _stim->getIsRadial() )
    {
        bounceDir = m_actor->get2DPos() - _stim->getPos();
        bounceDir.normalize();
    }
    else
    {
        bounceDir = _stim->getBounceDir();
    }

    processBounce(0,bounceDir,_stim->getBounceType(), m_characterPhys->getSpeed(), StringID::Invalid);

    if ( m_bounceCurrentDir != Vec2d::Zero )
    {
        m_bounceCurrentOrigin = _stim->getPos();
        m_bounceMultiplier = _stim->getMultiplier();
        m_bounceSender = _stim->getSender();
    }
}

void Ray_PlayerControllerComponent::processBounce( u32 dangerLevel, Vec2d bounceDirection, BounceType _bounceType, const Vec2d _speedAtBounce, const StringID _bounceGameMaterial )
{
    if ( _bounceType == BOUNCETYPE_NONE )
    {
        return;
    }

    m_bouncePrevFramePoly = m_bounceCheckEdgePolyRef;

    StringID bounceGameMaterialCopy = _bounceGameMaterial;
    resetBounce();

    if ( dangerLevel )
    {
        // No delay for hurting bounce
        m_receivedHitPosition = m_actor->get2DPos();
        m_receivedHitFXPos = m_actor->getPos();
        m_receivedHitType = RECEIVEDHITTYPE_HURTBOUNCE;
        m_receivedHitLevel = dangerLevel-1;
        m_receivedHitDamage = dangerLevel;

        m_bounceCurrentDangerLevel = dangerLevel;
        m_bounceCurrentType = _bounceType;
        m_bounceCurrentDir = bounceDirection;
        m_bounceCurrentGameMaterial = bounceGameMaterialCopy;
        m_bounceSpeedAtTime = _speedAtBounce;
    }
    else
    {
        m_bounceCurrentDir = bounceDirection;

        bbool willJump = isJumpQueued();

        m_bounceCurrentType = _bounceType;
        m_bounceCurrentGameMaterial = bounceGameMaterialCopy;
        m_bounceJumpAllowTimer = willJump ? getTemplate()->getBounceJumpMargin() : 0.f;
        m_bounceSpeedAtTime = _speedAtBounce;

        if ( m_currentState == &m_stateReleaseHit &&
             m_currentPunchHitType == PUNCHTYPE_CRUSH )
        {
            m_bounceCurrentReactType = BOUNCEREACTTYPE_CRUSH;
        }
        else if ( willJump )
        {
            m_bounceCurrentReactType = BOUNCEREACTTYPE_SYNCHRO;
        }
        else
        {
            m_bounceCurrentReactType = BOUNCEREACTTYPE_AUTO;
        }

        m_jumpQueueTimer = 0.f;
        m_bounceTimer = m_bounceCurrentReactType == BOUNCEREACTTYPE_AUTO ? getTemplate()->getBounceDelay() : 0.f;
    }
}

void Ray_PlayerControllerComponent::processTriggerVictory( Ray_EventTriggerVictory* _victory )
{
    changeState(&m_stateExitLevel);
}

void Ray_PlayerControllerComponent::processSequenceActorActivate( EventSequenceActorActivate* _sequenceEvent )
{
    if ( m_playerIndex == U32_INVALID )
    {
        return;
    }

    Ray_Player* player = static_cast<Ray_Player*>(RAY_GAMEMANAGER->getPlayer(m_playerIndex));

    if ( player && player->getIsLeaving() )
    {
        return;
    }

    disablePlayer(_sequenceEvent->getActivate());

    if ( _sequenceEvent->getActivate())
    {
        if (m_currentState != &m_stateIdle )
            changeState(&m_stateIdle);
    }
    else
    {
        resetPendingEvents();

        enablePhantom(btrue);
        m_characterPhys->resetForces();
        m_characterPhys->setSpeed(Vec2d::Zero);
        m_characterPhys->forceFindStickEdge();
        m_jumpQueueTimer = 0.f;
        m_hitQueueTimer = 0.f;
        resetBounce();

        if ( m_characterPhys->getStickedEdge() )
            changeState(&m_stateIdle);
        else
            changeState(&m_stateFalling);

        m_animComponent->resetTree();
        m_animComponent->resetTransition();
    }

    setActionPose(ACTIONPOSE_STAND);
}

void Ray_PlayerControllerComponent::processSequenceActorPrepare( EventSequenceActorPrepare* _sequenceEvent )
{
    m_inputDisabled = _sequenceEvent->getLockInput();
    m_characterPhys->setFrictionMultiplier(_sequenceEvent->getFriction());
    if (m_inputDisabled)
    {
        m_characterPhys->setSpeed(Vec2d::Zero);

        if (_sequenceEvent->getRevive())
        {
            if (m_currentState == &m_stateDeadSoul)
            {
                m_stateDeadSoul.reviveOnFirstLivePlayer();
            }
            else if (m_currentState == &m_stateDeath)
            {
                m_stateDeath.reviveOnFirstLivePlayer();
            }
        }
    }
}

void Ray_PlayerControllerComponent::processSequenceActorReady( EventSequenceActorReady* _sequenceEvent )
{
    _sequenceEvent->setReady(m_currentState == &m_stateIdle || isDead());
}


void Ray_PlayerControllerComponent::processReset()
{
    resetMoveToPoint();
    disablePlayer(bfalse);
    m_handsCaught = bfalse;
    m_caughtInPipe = bfalse;
    m_squashed = bfalse;
    m_pipeRef = ITF_INVALID_OBJREF;
}

void Ray_PlayerControllerComponent::updateMoveToPoint()
{
    if ( m_currentState == &m_stateDeadSoul )
    {
        return;
    }

	Vec2d feetPos = !m_swimingIsInside ? AIUtils::getFeetPos(m_actor) : m_actor->get2DPos();
    Vec2d targetPoint = m_moveToPointTarget;
    Vec2d vecToPoint = targetPoint - feetPos;
    f32 dist = vecToPoint.sqrnorm();
    f32 radius = m_characterPhys->getRadius();

    if ( !isSwimming() && dist < (radius*radius) && m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        const PolyLineEdge* currentEdge = m_characterPhys->getStickedEdge();

        if ( currentEdge )
        {
            Vec2d vecToPos = m_moveToPointTarget - currentEdge->getPos();
            f32 dist = currentEdge->m_normalizedVector.dot(vecToPos);

            targetPoint = currentEdge->getPos() + ( currentEdge->m_normalizedVector * dist );
            vecToPoint = targetPoint - feetPos;
            dist = vecToPoint.sqrnorm();
        }
    }

    if ( ( m_swimingIsInside && dist < 0.5f ) || ( dist < (m_moveToPointFinishedToler*m_moveToPointFinishedToler) ) )
    {
        finishMoveToPoint(feetPos);
    }
    else
    {
        Vec2d dirToPoint = vecToPoint;

        dirToPoint.normalize();

        cleanListener();

	    // in water
	    if ( m_swimingIsInside )
	    {
            m_listenerMove.m_x = Clamp( Min(fabsf(dirToPoint.m_x), fabsf(vecToPoint.m_x)*getSign(dirToPoint.m_x)), -1.0f, 1.0f );
            m_listenerMove.m_y = Clamp( Min(fabsf(dirToPoint.m_y), fabsf(vecToPoint.m_y)*getSign(dirToPoint.m_y)), -1.0f, 1.0f );
	    }
	    else
	    {
            f32 distSq = vecToPoint.sqrnorm();

            if ( distSq > 1.f )
            {
		        f32 angleWithGrav = m_currentGravityDir.dot(dirToPoint);

		        if ( angleWithGrav < -0.8f )
		        {
			        updateMoveToPointUp();
		        }
            }

		    f32 angleWithGravNorm = m_currentGravityDir.getPerpendicular().dot(dirToPoint);

            if ( !m_moveToPointFirstTime )
            {
                if ( fabs(angleWithGravNorm) > 0.8f )
                {
                    Vec2d frameMoveDir = m_actor->get2DPos() - m_prevPos;
                    f32 dotProd = frameMoveDir.dot(dirToPoint);

                    if ( dotProd < -0.f )
                    {
                        dotProd = frameMoveDir.dot(m_currentGravityDir.getPerpendicular());

                        if ( fabs(dotProd) > 0.05f )
                        {
                            finishMoveToPoint(feetPos);
                            return;
                        }
                    }
                }
                else if ( angleWithGravNorm > -0.1f && angleWithGravNorm < 0.1f )
                {
                    f32 radius = m_characterPhys->getRadius()*2.f;

                    if ( distSq < (radius*radius) )
                    {
                        finishMoveToPoint(feetPos);
                        return;
                    }
                }
            }

		    if ( angleWithGravNorm > 0.f )
		    {
			    updateMoveToPointRight();
		    }
		    else if ( angleWithGravNorm < 0.f )
		    {
			    updateMoveToPointLeft();
		    }
	    }
    }

    m_moveToPointFirstTime = bfalse;
}

void Ray_PlayerControllerComponent::finishMoveToPoint( const Vec2d& _feetPos )
{
    if ( !m_swimingIsInside )
    {
        Vec2d targetPosWithFeet = m_moveToPointTarget + ( m_actor->get2DPos() - _feetPos );
        m_actor->updateComponentPosAngle( targetPosWithFeet.to3d(m_actor->getPos().m_z),m_actor->getAngle(),this);
    }

    m_moveToPointFinished = btrue;
}

void Ray_PlayerControllerComponent::updateMoveToPointRight()
{
    // Move to the right
    m_listenerMove.m_x = 1.f;

    if ( m_currentState != &m_stateIdle &&
        !isDead() )
    {
        changeState(&m_stateIdle);
    }
}

void Ray_PlayerControllerComponent::updateMoveToPointLeft()
{
    // Move to the left
    m_listenerMove.m_x = -1.f;

    if ( m_currentState != &m_stateIdle &&
         m_currentState != &m_stateDeadSoul )
    {
        changeState(&m_stateIdle);
    }
}

void Ray_PlayerControllerComponent::updateMoveToPointUp()
{
    // Need to jump, float in the air
    if ( m_currentState != &m_stateJumping &&
         m_currentState != &m_stateDeadSoul )
    {
        setJumpData();
        changeState(&m_stateJumping);
    }

    m_listenerJumpHold = btrue;
}

void Ray_PlayerControllerComponent::setMoveToPoint( const Vec2d& _pos, f32 _moveToThreshold )
{
    m_moveToPoint = btrue;
    m_moveToPointFirstTime = btrue;
    m_moveToPointFinished = bfalse;
    m_moveToPointTarget = _pos;
    m_moveToPointFinishedToler = _moveToThreshold;
    enablePhantom(bfalse);
}

void Ray_PlayerControllerComponent::resetMoveToPoint()
{
    if ( m_moveToPoint )
    {
        m_moveToPoint = bfalse;
        m_moveToPointFinished = bfalse;
        m_moveToPointTarget = Vec2d::Zero;
        m_moveToPointFirstTime = bfalse;
        enablePhantom(btrue);
    }
}

bbool Ray_PlayerControllerComponent::findWallSlidingPolyline( ObjectRef _currentPoly, u32 _currentEdge, PolyLine** _poly, u32* _edge, bbool _fromHang, bbool _speedTest ) const
{
    *_poly = NULL;
    *_edge = U32_INVALID;

    PolyLine* foundPoly = NULL;
    u32 foundEdge = U32_INVALID;
    bbool floorCollision = bfalse;
    PhysContactsContainer contacts;
    f32 myRadius = m_characterPhys->getRadius();
    f32 myRadiusSq = myRadius*myRadius;
    PhysShapeCircle circle(myRadius+getTemplate()->getWallSlideExtraRadius());

    PHYSWORLD->checkEncroachment(m_actor->get2DPos(),m_actor->get2DPos(),
        m_actor->getAngle(),
        &circle,
        ECOLLISIONFILTER_ENVIRONMENT,
        m_actor->getDepth(),
        contacts);

    f32 maxDist = 0.f;
    f32 currentDist;
    bbool lookFlipped = m_actor->isFlipped();

    if ( isPerformingUTurn() && !m_uTurnFlipped )
    {
        lookFlipped = !lookFlipped;
    }

    Vec2d lookDir = AIUtils::getLookDir(m_actor,lookFlipped);
    Vec2d edgeNormal;

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        PolyLine* poly = NULL;
        const PolyLineEdge* edge = NULL;

        AIUtils::getPolyLine(contact.m_collidableUserData,contact.m_edgeIndex,poly,edge);

        if ( !poly || !edge )
        {
            continue;
        }

        if (poly->getOwnerActor())
        {
            EventQueryCanStickOrCollide queryCanStick;
            queryCanStick.setSender(m_actor->getRef());
            poly->getOwnerActor()->onEvent(&queryCanStick);
            if (!queryCanStick.canStick())
                return bfalse;
        }

        // Now we need to check if we are inside the range to collide with this edge or not
        Vec2d edgeToPos = m_actor->get2DPos() - edge->getPos();
        f32 dist = edge->m_normalizedVector.dot(edgeToPos);

        if ( dist < 0.f || dist > edge->m_length )
        {
            f32 distToContact = ( m_actor->get2DPos() - contact.m_contactPoint ).sqrnorm();

            if ( distToContact > myRadiusSq )
            {
                continue;
            }

            // If we are outside, check if we are within range with the next normal
            AIUtils::AdjacentEdgeData adjacentEdgeData;

            AIUtils::getAdjacentEdgeData(poly,*edge,contact.m_edgeIndex,m_currentGravityDir,dist < 0.f,1.f,dist,adjacentEdgeData);

            if ( adjacentEdgeData.m_crossProdEdges > 0.f )
            {
                f32 angleBetweenNormals = f32_ACos(adjacentEdgeData.m_normalA.dot(adjacentEdgeData.m_normalB));

                // Find out what's our angle between ourselves and the normal from the edge from the left
                if ( angleBetweenNormals )
                {
                    Vec2d myNorm = m_actor->get2DPos() - adjacentEdgeData.m_edgesCommonPoint;
                    myNorm.normalize();

                    f32 angle = f32_ACos(adjacentEdgeData.m_normalA.dot(myNorm));

                    // With the angle we can check if we are within the boundaries of the edge
                    if ( dist < 0.f )
                    {
                        if ( angle < (angleBetweenNormals*0.5f) )
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if ( angle > (angleBetweenNormals*0.5f) )
                        {
                            continue;
                        }
                    }
                }
            }
            else
            {
                continue;
            }
        }

        if ( m_characterPhys->getEdgeOrientationType(edge->m_normalizedVector) == StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND )
        {
            floorCollision = btrue;
        }

        edgeNormal = edge->m_normalizedVector.getPerpendicular();

        if ( contact.m_normal.dot(edgeNormal) > 0.f )
        {
            continue;
        }

        if ( !isPolylineEdgeGoodForWallSlide(poly,edge,contact.m_edgeIndex,_fromHang) )
        {
            continue;
        }

        if ( _speedTest )
        {
            Vec2d speed;

            if ( m_wallSlideTestSpeed != Vec2d::Zero )
            {
                speed = m_wallSlideTestSpeed;
            }
            else
            {
                speed = m_characterPhys->getSpeed();
            }

            f32 speedProj = speed.dot(edgeNormal);
            bbool speedTowardsWall = speedProj < -0.5f;
            bbool stickTowardsWall;

            if ( m_airControlDisableDelay == 0.f )
            {
                if ( getMoveData().getMoveDirection() == MOVEDIR_RIGHT )
                {
                    stickTowardsWall = edgeNormal.dot(Vec2d::Right) < 0.f;
                }
                else if ( getMoveData().getMoveDirection() == MOVEDIR_LEFT )
                {
                    stickTowardsWall = edgeNormal.dot(-Vec2d::Right) < 0.f;
                }
                else
                {
                    stickTowardsWall = bfalse;
                }
            }
            else
            {
                stickTowardsWall = bfalse;
            }

            if ( !speedTowardsWall && !stickTowardsWall )
            {
                f32 dotProd = lookDir.dot(edgeNormal);
                if ( dotProd >= 0.f )
                {
                    continue;
                }
            }
        }

        currentDist = ( contact.m_contactPoint - m_actor->get2DPos() ).dot(m_currentGravityDir);

        if ( !foundPoly || currentDist > maxDist )
        {
            foundPoly = poly;
            foundEdge = contact.m_edgeIndex;
            maxDist = currentDist;
        }
    }

    if ( foundPoly )
    {
        *_poly = foundPoly;
        *_edge = foundEdge;
    }
    else
    {
        // See if we could be adjusted on the wall we were sliding on
        BaseObject* obj = GETOBJECT(_currentPoly);

        if ( obj )
        {
            PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

            if ( poly )
            {
                const PolyLineEdge& edge = poly->getEdgeAt(_currentEdge);
                Vec2d vecToPos = m_actor->get2DPos() - edge.getPos();
                f32 dist = edge.m_normalizedVector.dot(vecToPos);

                if ( dist >= 0.f && dist <= edge.m_length && isPolylineEdgeGoodForWallSlide(poly,&edge,_currentEdge,_fromHang) )
                {
                    *_poly = poly;
                    *_edge = _currentEdge;
                }
                else
                {
                    if ( dist < 0.f )
                    {
                        PolyLine* newPoly;
                        u32 newEdgeIndex;

                        AIUtils::getAdjacentEdge(poly,_currentEdge,bfalse,newPoly,newEdgeIndex);

                        if ( newPoly )
                        {
                            const PolyLineEdge& nextEdge = newPoly->getEdgeAt(newEdgeIndex);

                            if ( nextEdge.m_normalizedVector.dot(edge.m_normalizedVector) > 0.f )
                            {
                                // Make sure they go in the same direction
                                Vec2d vecToPos = m_actor->get2DPos() - nextEdge.getPos();
                                f32 dist = nextEdge.m_normalizedVector.dot(vecToPos);

                                if ( dist >= 0.f && dist <= nextEdge.m_length && isPolylineEdgeGoodForWallSlide(newPoly,&nextEdge,newEdgeIndex,_fromHang) )
                                {
                                    *_poly = newPoly;
                                    *_edge = newEdgeIndex;
                                }
                            }
                        }
                    }
                    else
                    {
                        PolyLine* newPoly;
                        u32 newEdgeIndex;

                        AIUtils::getAdjacentEdge(poly,_currentEdge,btrue,newPoly,newEdgeIndex);

                        if ( newPoly )
                        {
                            const PolyLineEdge& nextEdge = newPoly->getEdgeAt(newEdgeIndex);

                            if ( nextEdge.m_normalizedVector.dot(edge.m_normalizedVector) > 0.f )
                            {
                                Vec2d vecToPos = m_actor->get2DPos() - nextEdge.getPos();
                                f32 dist = nextEdge.m_normalizedVector.dot(vecToPos);

                                if ( dist >= 0.f && dist <= nextEdge.m_length && isPolylineEdgeGoodForWallSlide(newPoly,&nextEdge,newEdgeIndex,_fromHang) )
                                {
                                    *_poly = newPoly;
                                    *_edge = newEdgeIndex;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return floorCollision;
}

void Ray_PlayerControllerComponent::setPrevWallSlide( ObjectRef _poly, u32 _edge )
{
    m_wallSlidePrevPoly = _poly;
    m_wallSlidePrevEdge = _edge;
}

void Ray_PlayerControllerComponent::resetPrevWallSlide()
{
    m_wallSlidePrevPoly = ITF_INVALID_OBJREF;
    m_wallSlidePrevEdge = U32_INVALID;
}

bbool Ray_PlayerControllerComponent::isInDeadRegion() const
{
    if ( m_playerDisabled || m_moveToPoint )
    {
        return bfalse;
    }

    u32 numRegions = m_currentRegions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        const PolylineRegion& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_Death )
        {
           return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::isInNoFriendlyFireRegion() const
{
    u32 numRegions = m_currentRegions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        const PolylineRegion& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_NoFriendlyFire )
        {
            return btrue;
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::triggerDeath( EDeathType _type )
{
    ITF_ASSERT(_type!=DEATHTYPE_NONE);

    if ( _type != DEATHTYPE_NONE )
    {
        m_currentDeathType = _type;

        if ( !isDead() )
        {
            removeAllHealth();
        }

#ifdef ITF_SUPPORT_ONLINETRACKING
		RAY_GAMEMANAGER->getOnlineTrackingManager()->OnlineTrackingTrackDeath(m_playerIndex);
#endif // ITF_SUPPORT_ONLINETRACKING

        resetBounce();
        changeState(&m_stateDeath);

        REWARD_MANAGER->Action_New(Ray_RewardManager::Action_KilledByEnnemy, m_playerIndex);
        REWARD_MANAGER->Action_TimerStart(Ray_RewardManager::Timer_IsGhost, m_playerIndex);
        REWARD_MANAGER->Action_TimerStop(Ray_RewardManager::Timer_IsAlive, m_playerIndex);

        if ( m_playerIndex == RAY_GAMEMANAGER->getMainIndexPlayer() )
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));

            player->setWasHitInLevel(btrue);
        }
    }
}

void Ray_PlayerControllerComponent::resetDeath()
{
    m_currentDeathType = DEATHTYPE_NONE;
}

void Ray_PlayerControllerComponent::setLastHitNature( EHitNature _hitNature )
{
    if ( isDead() )
    {
        return;
    }

    m_lastHitNature = _hitNature;
}

bbool Ray_PlayerControllerComponent::shouldTriggerHelicopterAction() const
{
    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        return bfalse;
    }

    if ( isSwimming() )
    {
        return bfalse;
    }

    if ( !m_helicopterCooldown && isPowerUpEnabled(Ray_PowerUp_Helicopter) )
    {
        if ( isPressingHelicopterAction() ||
           ( m_helicopterCooldownJustFinished && isHoldingHelicopterAction() ) ||
           ( m_airControlJustFinished && isHoldingHelicopterAction() ) )
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::shouldTriggerHelicoFromHold() const
{
    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        return bfalse;
    }

    if ( isSwimming() )
    {
        return bfalse;
    }

    if ( !m_helicopterCooldown && isPowerUpEnabled(Ray_PowerUp_Helicopter) )
    {
        if ( isHoldingHelicopterAction()  )
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::isPressingHelicopterAction() const
{
    return m_listenerHelico;
}

bbool Ray_PlayerControllerComponent::isHoldingHelicopterAction() const
{
    return m_listenerHelicoHold;
}

void Ray_PlayerControllerComponent::startSlideFx()
{
    const GameMaterial_Template* mat = NULL;
    const PolyLineEdge * edge = m_characterPhys->getStickedEdge();
    if (edge)
    {
        mat = World::getGameMaterial(edge->getGameMaterial());
    }

    if (m_slideFxHandle == U32_INVALID)
        m_slideFxHandle = m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(SlideFX,652749163),mat);
}

void Ray_PlayerControllerComponent::stopSlideFx()
{
    if (m_slideFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_slideFxHandle);
        m_slideFxHandle = U32_INVALID;
    }
}
bbool Ray_PlayerControllerComponent::isSlideFx()
{
    return m_slideFxHandle != U32_INVALID;
}
void Ray_PlayerControllerComponent::startWallSlideFx(const class PolyLine* _poly, u32 _edge)
{
    const GameMaterial_Template* mat = NULL;

    if (_poly)
    {
        const PolyLineEdge& edge = _poly->getEdgeAt(_edge);
        mat = World::getGameMaterial(edge.getGameMaterial());
    }

    if (m_wallSlideFxHandle == U32_INVALID)
        m_wallSlideFxHandle = m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(WallSlideFx,3782089884),mat);
}
void Ray_PlayerControllerComponent::stopWallSlideFx()
{
    if (m_wallSlideFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_wallSlideFxHandle);
        m_wallSlideFxHandle = U32_INVALID;
    }
}

void Ray_PlayerControllerComponent::startJumpFx()
{
    stopJumpFx();

    if (getStance() == STANCE_WALLSLIDE)
    {
        m_fxController->playFX(ITF_GET_STRINGID_CRC(WallJumpFx,2963764456));
    }
    else if (m_jumpFxHandle == U32_INVALID)
    {
        m_jumpFxHandle = m_fxController->playFX(ITF_GET_STRINGID_CRC(JumpFx,315850038));
    }

}

void Ray_PlayerControllerComponent::stopJumpFx()
{
    if (m_jumpFxHandle != U32_INVALID)
    {
        // JP : FX launched when doing a jump are one shot FX. They stop by themselves.
        // the side effect of stopping it is that another effect can be played at the very same
        // handle, and the stop jump FX is stopping it, which is bad.
        //m_fxController->stopFX(m_jumpFxHandle);
        m_jumpFxHandle = U32_INVALID;
    }
}

void Ray_PlayerControllerComponent::startBounceOnLineFx(const GameMaterial_Template * _mat)
{
    m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(BounceOnLineFx,222755042), _mat);
}

void Ray_PlayerControllerComponent::startDiveFx()
{
    u32 fxHandle = m_fxController->playFX(ITF_GET_STRINGID_CRC(DiveFx,1187245914));
    m_fxController->setFXPosFromHandle(fxHandle,m_actor->getPos());
}

void Ray_PlayerControllerComponent::startDiveOutFx()
{
    u32 fxHandle = m_fxController->playFX(ITF_GET_STRINGID_CRC(DiveOutFx,2335027334));
    m_fxController->setFXPosFromHandle(fxHandle,m_actor->getPos());
}

void Ray_PlayerControllerComponent::startHelicoFx()
{
    if (m_helicoFxHandle == U32_INVALID)
        m_helicoFxHandle = m_fxController->playFX(ITF_GET_STRINGID_CRC(HelicoFx,3175983864));
}

void Ray_PlayerControllerComponent::stopHelicoFx()
{
    if (m_helicoFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_helicoFxHandle);
        m_helicoFxHandle = U32_INVALID;
    }
}
void Ray_PlayerControllerComponent::startDeadSoulFx()
{
    if (m_deadSoulFxHandle == U32_INVALID)
        m_deadSoulFxHandle = m_fxController->playFX(ITF_GET_STRINGID_CRC(DeadSoulFx,1249024881));
}
void Ray_PlayerControllerComponent::startLandFx(const GameMaterial_Template * _mat)
{
    m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(LandFx,741732833),_mat);
}
void Ray_PlayerControllerComponent::startWallLandFx(const GameMaterial_Template * _mat)
{
    m_fxController->playMaterialFX(ITF_GET_STRINGID_CRC(WallLandFx,888681957),_mat);
}
void Ray_PlayerControllerComponent::stopDeadSoulFx()
{
    if (m_deadSoulFxHandle != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_deadSoulFxHandle);
        m_deadSoulFxHandle = U32_INVALID;
    }
}


bbool Ray_PlayerControllerComponent::useCameraLimits() const
{
    if ( m_playerDisabled || m_moveToPoint )
    {
        return bfalse;
    }

    if ( !CAMERACONTROLLERMANAGER->isSubjectRegistered(m_actor->getRef()) )
    {
        return bfalse;
    }

    if ( isDead() )
    {
        return bfalse;
    }

#ifdef ITF_SUPPORT_CHEAT
    if ( m_currentState == &m_stateFlyMode )
    {
        return bfalse;
    }

#ifdef ITF_WII
    if ( Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex)) )
    {
        if (player->getInvincible())
        {
            return bfalse;
        }
    }
#endif // ITF_WII
#endif

    if ( LOADINGSCREEN->isVisible()  )
    {
        return bfalse;
    }

    if ( RAY_GAMEMANAGER->isPlayerInSequence(m_actor->getRef()) )
    {
        return bfalse;
    }

    if ( CINEMATICMANAGER->getLockPlayers() )
    {
        return bfalse;
    }

    if ( m_currentState == &m_stateStartLevel ||
         m_currentState == &m_stateStargate )
    {
        return bfalse;
    }

    return btrue;
}

bbool Ray_PlayerControllerComponent::isCameraLimited() const
{
    if ( !useCameraLimits() )
    {
        return bfalse;
    }


    const Ray_CameraLimiterData& limiterData = RAY_CAMERACONTROLLERMANAGER->getLimiterData();

    if ( limiterData.getUseEjectMargin() )
    {
        AABB ejectAABB;
        return limiterData.getEjectAABB(m_actor->getDepth(), ejectAABB) &&
               !ejectAABB.contains(m_actor->get2DPos());
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::updateCameraLimiter( f32 _dt )
{
    f32 lastCameraLimiterTimer = m_cameraLimiterTimer;
    m_cameraLimiterTimer = 0;

    f32 lastOffscreenTimer = m_offscreenTimer;
    m_offscreenTimer = 0;

    m_lastOnscreenPlayerKillsEveryone = btrue;  // default

    if ( !useCameraLimits() )
    {
        return;
    }


    const Ray_CameraLimiterData& limiterData = RAY_CAMERACONTROLLERMANAGER->getLimiterData();
    m_lastOnscreenPlayerKillsEveryone = limiterData.getLastOnscreenPlayerKillsEveryone();

    // push the player is he's outside the eject margin (soft col)
    if ( limiterData.getUseEjectMargin() &&
         m_stance != STANCE_SWING ) // RO-15790: don't push swinging players otherwise the force accumulates (because the physics is not updated)
    {
        AABB ejectAABB;
        if ( limiterData.getEjectAABB(m_actor->getDepth(), ejectAABB) )
        {
            const Vec2d& min = ejectAABB.getMin();
            const Vec2d& max = ejectAABB.getMax();
            const Vec2d& pos = m_actor->get2DPos();

            Vec2d penetration(Vec2d::Zero);
            if (pos.m_x < min.m_x)
                penetration.m_x = pos.m_x - min.m_x;
            else if (pos.m_x > max.m_x)
                penetration.m_x = pos.m_x - max.m_x;
            if (pos.m_y < min.m_y)
                penetration.m_y = pos.m_y - min.m_y;
            else if (pos.m_y > max.m_y)
                penetration.m_y = pos.m_y - max.m_y;
            //GFX_ADAPTER->drawDBGLine(pos, pos - penetration, Color::red());

            // apply a force to move away from the screen border
            //f32 penetrationCoeff = Min(penetration.norm(), 1.f);
            f32 penetrationCoeff = penetration.norm();
            if (penetrationCoeff > MTH_EPSILON)
            {
#ifndef ITF_FINAL
                //DebugDraw::text("eject %.3f", penetrationCoeff);
#endif // ITF_FINAL

                if (limiterData.getEjectMarginDetachesPlayer())
                {
                    if ( m_currentState == &m_stateHelicopter )
                    {
                        changeState(&m_stateFalling);
                    }
                    else
                    {
                        setStance(STANCE_STAND);
                    }
                }

                if (!m_characterPhys->getDisabled())   // don't push players when the physics is not updated, otherwise the force accumulates
                {
                    Vec2d ejectDir;

                    if (m_characterPhys->getStickedPolyline())
                    {
                        const Vec2d& groundDir = m_characterPhys->getCurrentGroundDir();
                        ejectDir = groundDir * (groundDir.dot(penetration) < 0 ? 1.f : -1.f);
                    }
                    else
                    {
                        ejectDir = -penetration;
                        ejectDir.normalize();
                    }

                    const f32 s_exitSpeed = getTemplate()->getSoftCollisionExitSpeed();

                    const Vec2d speed = m_characterPhys->getSpeed();
                    const f32 speedDotP = speed.dot(ejectDir);
                    const f32 speedRatio = speedDotP < 0 ? 1.f : 1.f - Clamp(speedDotP / s_exitSpeed, 0.f, 1.f);

                    const Vec2d force = Clamp(sqr(penetrationCoeff),0.0f,getTemplate()->getCameraLimiterMaxCoeff()) * speedRatio * ejectDir * limiterData.getEjectForce();
                    m_characterPhys->addForce(force);
                }
            }

            //GFX_ADAPTER->drawDBGAABB(AABB(min, max));
        }
    }


    bbool isPlayerOnScreen = m_cameraSubjectComponent->isSubjectOnScreen(btrue);

    // kill the player if he's outside the death margin
    if ( limiterData.getUseDeathMargin() &&
         !isPlayerOnScreen )    // never kill players on-screen: RO-16334
    {
        AABB deathAABB;

        if ( limiterData.getDeathAABB(m_actor->getDepth(), deathAABB) )
        {
            const AABB subjectAABB = m_cameraSubjectComponent->getAABB();
            if ( !deathAABB.checkOverlap(subjectAABB) )
            {
                m_cameraLimiterTimer = lastCameraLimiterTimer + _dt;
                if (m_cameraLimiterTimer > 0.1f)
                {
#ifdef ITF_SUPPORT_ONLINETRACKING
                    if (CONFIG->m_enableonlineTracking==btrue)
                    {
                        RAY_GAMEMANAGER->getOnlineTrackingManager()->m_lastOffscreen = 1;
                    }
#endif
                    triggerDeath(DEATHTYPE_INFINITEFALL);
                    return;
                }
            }
        }
    }

    // last resort: kill the player if he hasn't been visible for too long
    if (!isPlayerOnScreen)
    {
        m_offscreenTimer = lastOffscreenTimer + _dt;
        if (lastOffscreenTimer > limiterData.getTimeOut())
        {
            m_offscreenTimer = 0;
#ifdef ITF_SUPPORT_ONLINETRACKING
			if (CONFIG->m_enableonlineTracking==btrue)
			{
						RAY_GAMEMANAGER->getOnlineTrackingManager()->m_lastOffscreen = 1;
			}
#endif
            triggerDeath(DEATHTYPE_INFINITEFALL);
            return;
        }
    }
}

void Ray_PlayerControllerComponent::processPhysicsUpdated()
{
    if ( !isDead() && !m_playerDisabled && !m_moveToPoint && m_bounceRepositionCounter == 0.f )
    {
        checkSquash();
    }

    updatePhantom();
}

void Ray_PlayerControllerComponent::updateBaseInfo( f32 _dt )
{
    PolyLine* poly;
    const PolyLineEdge* edge;
    bbool speedIsValid = bfalse;

    if ( m_baseInfoPrevPoly != ITF_INVALID_OBJREF )
    {
        AIUtils::getPolyLine(m_baseInfoPrevPoly,m_baseInfoPrevEdgeIndex,poly,edge);

        if ( poly && edge )
        {
            Vec2d newPos = edge->getPos() + ( edge->m_normalizedVector * edge->m_length * m_baseInfoPrevRelDist );

            m_baseInfoSpeed = ( newPos - m_baseInfoPrevPos ) / _dt;
            speedIsValid = btrue;
        }
    }
    else if ( m_baseInfoPrevActor != ITF_INVALID_OBJREF )
    {
        Actor* hangingActor = AIUtils::getActor(m_baseInfoPrevActor);

        if ( hangingActor )
        {
            EventQuerySnapData snap;

            hangingActor->onEvent(&snap);

            m_baseInfoSpeed = ( snap.getPos().truncateTo2D() - m_baseInfoPrevPos ) / _dt;
            speedIsValid = btrue;
        }
    }

    if ( !speedIsValid && !m_jumpOnAirTimer )
    {
        m_baseInfoSpeed = Vec2d::Zero;
    }

    resetBaseInfo();

    poly = NULL;
    edge = NULL;

    if ( isHanging() )
    {
        if ( m_hangingActor != ITF_INVALID_OBJREF )
        {
            Actor* hangingActor = AIUtils::getActor(m_hangingActor);

            if ( hangingActor )
            {
                EventQuerySnapData snap;
                hangingActor->onEvent(&snap);
                m_baseInfoPrevPos = snap.getPos().truncateTo2D();
                m_baseInfoPrevPoly = ITF_INVALID_OBJREF;
                m_baseInfoPrevEdgeIndex = U32_INVALID;
                m_baseInfoPrevActor = m_hangingActor;
            }
        }
        else if ( m_hangingPolyline != ITF_INVALID_OBJREF )
        {
            AIUtils::getPolyLine(m_hangingPolyline,m_hangingEdge,poly,edge);

            if ( poly && edge )
            {
                m_baseInfoPrevRelDist = Clamp(m_climbingT,0.f,edge->m_length);
                m_baseInfoPrevPos = edge->getPos() + ( edge->m_normalizedVector * edge->m_length * m_baseInfoPrevRelDist );
                m_baseInfoPrevPoly = m_hangingPolyline;
                m_baseInfoPrevEdgeIndex = m_hangingEdge;
            }
        }
    }
    else if ( isClimbing() )
    {
        AIUtils::getPolyLine(m_climbingPolyline,m_climbingEdge,poly,edge);

        if ( poly && edge )
        {
            m_baseInfoPrevRelDist = Clamp(m_climbingT,0.f,edge->m_length);
            m_baseInfoPrevPos = edge->getPos() + ( edge->m_normalizedVector * edge->m_length * m_baseInfoPrevRelDist );
            m_baseInfoPrevPoly = m_hangingPolyline;
            m_baseInfoPrevEdgeIndex = m_hangingEdge;
        }
    }
    else
    {
        AIUtils::getPolyLine(m_characterPhys->getStickedPolylineRef(),m_characterPhys->getStickedEdgeIndex(),poly,edge);

        if ( poly && edge )
        {
            m_baseInfoPrevRelDist = Clamp(m_characterPhys->getRelativeEdgeDistance(),0.f,edge->m_length);
            m_baseInfoPrevPos = edge->getPos() + ( edge->m_normalizedVector * edge->m_length * m_baseInfoPrevRelDist );
            m_baseInfoPrevPoly = m_characterPhys->getStickedPolylineRef();
            m_baseInfoPrevEdgeIndex = m_characterPhys->getStickedEdgeIndex();
        }
    }

    if ( edge )
    {
        const Ray_GameMaterial_Template* mat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));

        if ( mat )
        {
            m_baseInfoPrevMultiplierX = mat->getMovePlatformSpeedXMult();
            m_baseInfoPrevMultiplierY = mat->getMovePlatformSpeedYMult();
        }
    }
}

void Ray_PlayerControllerComponent::resetBaseInfo()
{
    m_baseInfoPrevPoly = ITF_INVALID_OBJREF;
    m_baseInfoPrevEdgeIndex = U32_INVALID;
    m_baseInfoPrevActor = ITF_INVALID_OBJREF;
    m_baseInfoPrevMultiplierX = 0.f;
    m_baseInfoPrevMultiplierY = 0.f;
}

bbool Ray_PlayerControllerComponent::hasOtherLivePlayer()
{
    for (u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
    {
        if (i == m_playerIndex)
        {
            continue;
        }

        Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
        if (otherPlayer->getActive() && !otherPlayer->isDead())
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::hasOtherLivePlayerOnscreen()
{
    for (u32 i = 0; i < RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
    {
        if (i == m_playerIndex)
        {
            continue;
        }

        Ray_Player* otherPlayer = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(i));
        if (!otherPlayer->getActive() || otherPlayer->isDead())
        {
            continue;
        }

        if (Actor* otherActor = otherPlayer->getActor())
        {
            if (CameraSubjectComponent* otherSubject = otherActor->GetComponent<CameraSubjectComponent>())
            {
                if (otherSubject->isSubjectOnScreen())
                {
                    return btrue;
                }
            }
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::updateSoftCollisions( f32 _deltaTime )
{
    if ( m_softCollisionDisabled ||
         (m_softCollisionForceMultiplier * m_softCollisionRadiusMultiplier <= 0.f) )
    {
        return;
    }

	Actor* buffer[8];
	ActorList players(8,MemoryId::mId_Temporary,buffer);
    DepthRange depthRange(m_actor->getDepth());
    AIUtils::getPlayers(depthRange,players);

    if (players.size() == 0)
        return;

    u32 numRegions = m_currentRegions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        PolylineRegion& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_NoSoftColl )
        {
            return;
        }
    }

    bbool isAlive = !isDead();

#ifndef ITF_FINAL
    //DebugDraw::shape(m_actor->getPos(),m_actor->getAngle(),&thisPlayerShape);
#endif // ITF_FINAL

    for (u32 i = 0; i < players.size(); ++i)
    {
        Actor * otherPlayer = players[i];
        if (otherPlayer == m_actor)
            continue;

        if (Ray_ShooterControllerComponent * controller = otherPlayer->GetComponent<Ray_ShooterControllerComponent>())
            continue;

        Ray_PlayerControllerComponent * otherPlayerController = otherPlayer->GetComponent<Ray_PlayerControllerComponent>();
        StickToPolylinePhysComponent * otherPlayerPhysics = otherPlayer->GetComponent<StickToPolylinePhysComponent>();

        // live player only take forces from live players; dead players take forces from everyone
        if (isAlive)
        {
            if ( ( otherPlayerController == NULL ) || ( otherPlayerPhysics == NULL ) || otherPlayerController->isDead() )
                continue;
        }


        const f32 thisPlayerRadius = getSoftCollisionRadius() * m_softCollisionForceMultiplier;
        const f32 otherPlayerRadius = otherPlayerController->getSoftCollisionRadius() * otherPlayerController->m_softCollisionForceMultiplier;
        if ( otherPlayerRadius <= 0.f )
        {
            continue;
        }

        const f32 totalRadius = thisPlayerRadius + otherPlayerRadius;

        const f32 sqrDist = (otherPlayer->get2DPos() - m_actor->get2DPos()).sqrnorm();

#ifndef ITF_FINAL
        //DebugDraw::shape(otherPlayer->getPos(),otherPlayer->getAngle(),&otherPlayerShape);
#endif // ITF_FINAL
        if (sqrDist < totalRadius * totalRadius)
        {
            const Vec2d toPlayer = m_actor->get2DPos() - otherPlayer->get2DPos();
            Vec2d dirToPlayer = toPlayer;
            dirToPlayer.normalize();

            Vec2d dir = m_characterPhys->getCurrentGroundDir();

            if (dirToPlayer.dot(dir)<0.0f)
            {
                dir = -dir;
            }
            Vec2d speed = m_characterPhys->getSpeed() - otherPlayerPhysics->getSpeed();
            const f32 distToPlayer = toPlayer.norm();
            const f32 divDist = distToPlayer/totalRadius;
            const f32 distRatio = 1.0f - (divDist * divDist);
            const f32 speedDotP = speed.dot(dir);
            static f32 minClamp = 0.f;
            static f32 maxClamp = 0.1f;
            const f32 speedRatio = speedDotP < 0.0f ? 1.0f : 1.0f - Clamp(speedDotP/getTemplate()->getSoftCollisionExitSpeed(),minClamp,maxClamp);
            Vec2d force = dir * getTemplate()->getSoftCollisionExitForce() * distRatio;
            force *= speedRatio;
            force *= m_softCollisionForceMultiplier * otherPlayerController->m_softCollisionForceMultiplier;
            m_characterPhys->addForce(force);
            //GFX_ADAPTER->drawArrow(m_actor->getPos(), m_actor->getPos() + (force * 0.1f).to3d());
        }
    }
}

void Ray_PlayerControllerComponent::processQueryAIData( Ray_EventQueryAIData * aiQuery )
{
    aiQuery->setSoftCollisionRadius(getSoftCollisionRadius());
    aiQuery->setSoftCollisionForceMultiplier(m_softCollisionForceMultiplier);
    aiQuery->setReducedSizd(isSizeReduced());
    if (isSwimming())
    {
        aiQuery->setSwimmingPolyline(m_swimingPolyline);
    }
}

f32 Ray_PlayerControllerComponent::getSoftCollisionRadius() const
{
    return m_characterPhys->getRadius() * m_softCollisionRadiusMultiplier;
}

void Ray_PlayerControllerComponent::disableSoftCollision()
{
    m_softCollisionRadiusMultiplier = 0.f;
    m_softCollisionDisabled = btrue;
}

void Ray_PlayerControllerComponent::restoreSoftCollision()
{
    if ( !m_pageActive )
    {
        m_softCollisionRadiusMultiplier = getTemplate()->getSoftCollisionRadiusMultiplier();
        m_softCollisionRestoreForceTimer = getTemplate()->getSoftCollisionRestoreForceDelay() + getTemplate()->getSoftCollisionRestoreForceDuration();

        m_softCollisionForceMultiplier = (m_softCollisionRestoreForceTimer > 0.f) ? 0.f : 1.f;
        m_softCollisionDisabled = bfalse;
    }
}

void Ray_PlayerControllerComponent::processPowerUp(const Ray_EventPowerUp* _powerUp)
{
    // enable/disable power ups through events
    Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
    powerUpManager.processPowerUp(_powerUp, m_playerIndex);
}

bbool Ray_PlayerControllerComponent::isPowerUpEnabled(const StringID& _id) const
{
    // ease power up query from self and PC states
    Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
    if (powerUpManager.isGlobal(_id))
    {
        return powerUpManager.isEnabled(_id);
    }
    if (powerUpManager.isPerPlayer(_id))
    {
        return powerUpManager.isEnabled(_id, m_playerIndex);
    }
    ITF_WARNING(m_actor, bfalse, "Power-up not found");
    return bfalse;
}

void Ray_PlayerControllerComponent::updateLianaSlideFx( )
{
    PolyLine* polyLine;
    const PolyLineEdge* edge;

    AIUtils::getPolyLine(m_climbingPolyline,m_climbingEdge,polyLine,edge);

    if (polyLine && edge)
    {
        const Ray_GameMaterial_Template* gmat = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge->getGameMaterial()));
        if (gmat->isClimbSlide())
        {
            if (m_lianaSlideFx == U32_INVALID)
            {
                const StringID s_lianaSlideFx("lianaSlideFx");
                m_lianaSlideFx = m_fxController->playMaterialFX(s_lianaSlideFx,gmat);
            }

            Vec2d climbPos = edge->getPos() + ( edge->m_vector *  m_climbingT );
            const f32 depth = m_actor->getDepth();
            m_fxController->setFXPosFromHandle(m_lianaSlideFx,climbPos.to3d(depth));
        }
        else
        {
            stopLianaSlideFx();
        }
    }
    else
    {
        stopLianaSlideFx();
    }
}

void Ray_PlayerControllerComponent::stopLianaSlideFx()
{
    if (m_lianaSlideFx != U32_INVALID)
    {
        m_fxController->stopFXFromHandle(m_lianaSlideFx);
        m_lianaSlideFx = U32_INVALID;
    }
}

bbool Ray_PlayerControllerComponent::isSizeReduced() const
{
    return getCharacterSize() == CHARACTERSIZE_SMALL;
}

ECharacterSize Ray_PlayerControllerComponent::getCharacterSize() const
{
    return AIUtils::calculateCharacterSize(m_actor->getWorldInitialScale().m_x,m_actor->getScale().m_x);
}

void Ray_PlayerControllerComponent::playSnapFX()
{
    m_fxController->playFX(ITF_GET_STRINGID_CRC(Snap,1097265724));
}

void Ray_PlayerControllerComponent::Receive( u32 _playerIndex, f32 _axis, const StringID& _action )
{
    if ( m_playerIndex != _playerIndex || !m_actor->isEnabled() || m_inputDisabled )
    {
        return;
    }

    if ( _action == ITF_GET_STRINGID_CRC(JUMP,2374255179) )
    {
        m_listenerJump = _axis > 0.f;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(JUMP_HOLD,4023844934) )
    {
        m_listenerJumpHold = _axis > 0.f;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(HELICO,292076939) )
    {
        m_listenerHelico = _axis > 0.f;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(HELICO_HOLD,2945208220) )
    {
        m_listenerHelicoHold = _axis > 0.f;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(ATTACK,3643922889) )
    {
        m_listenerAttack = _axis > 0.f;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(ATTACK_HOLD,26671617) )
    {
        m_listenerAttackHold = _axis > 0.f;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(MOVEX,965019289) )
    {
        m_listenerMove.m_x = _axis;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(MOVEY,951160567) )
    {
        m_listenerMove.m_y = _axis;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(SPRINT,183769305) )
    {
        // Toggle/Hold for Run accessibility option
        bbool runButtonPressed = _axis > 0.f;
        i32 runMode = RAY_GAMEMANAGER->getRunButtonMode();

        if (runMode == RunButtonMode_Hold)
        {
            m_listenerSprint = runButtonPressed;
        }
        else if (runMode == RunButtonMode_Toggle)
        {
            if (runButtonPressed && !m_prevRunButtonPressed)
            {
                m_runToggleActive = !m_runToggleActive;
            }

            m_listenerSprint = m_runToggleActive;
        }

        m_prevRunButtonPressed = runButtonPressed;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(LEAVE,982417778) )
    {
        m_listenerLeave = _axis > 0.f;
    }
    else if ( _action == ITF_GET_STRINGID_CRC(SPRINT_TRIGGER,3559205352) )
    {
        m_listenerSprintTrigger = _axis > 0.f;
    }
#ifdef ITF_SUPPORT_CHEAT
    else if ( _action == ITF_GET_STRINGID_CRC(FLYCHEAT,3189998060) )
    {
        m_listenerFlyCheat = _axis > 0.f;
    }
#endif
}

void Ray_PlayerControllerComponent::cleanListener()
{
    m_listenerMove = Vec2d::Zero;
    m_listenerAttack = bfalse;
    m_listenerAttackHold = bfalse;
    m_listenerJump = bfalse;
    m_listenerJumpHold = bfalse;
    m_listenerHelico = bfalse;
    m_listenerHelicoHold = bfalse;
    m_listenerSprint = bfalse;
    m_listenerLeave = bfalse;
    m_listenerSprintTrigger = bfalse;

#ifdef ITF_SUPPORT_CHEAT
    m_listenerFlyCheat = bfalse;
#endif
    if (m_runToggleActive)
    {
        m_listenerSprint = btrue;
    }
}

void Ray_PlayerControllerComponent::addListener()
{
    GAMEMANAGER->addGamePlayInputListener(this);
    m_listenerAdded = btrue;
}

void Ray_PlayerControllerComponent::removeListener()
{
    GAMEMANAGER->removeInputListener(this);
    m_listenerAdded = bfalse;
}




void Ray_PlayerControllerComponent::checkAchievementPiranha(const PolyLine * const _previousWaterPolyline, const PolyLine * const _nextWaterPolyline)
{
    if (isDead())
    {
        return;
    }

    //if previous water polyline was valid (go out of a water polyline)
    if( _previousWaterPolyline && _previousWaterPolyline->m_ownerFrise )
    {
        if (m_survivePiranhaPondInside )
        {
            if( m_survivePiranhaPondTimer > SURVIVE_PIRANHAS_MIN_TIME_INSIDE && m_survivePiranhaPondLastHitTimer == 0.f )
            {
                m_survivePiranhaPondOutside = btrue;
                m_survivePiranhaPondTimer = 0.0f;
            }
            m_survivePiranhaPondInside = bfalse;
        }
    }

    //if next water polyline is valid (go into new water polyline)
    if( _nextWaterPolyline && _nextWaterPolyline->m_ownerFrise )
    {
        m_survivePiranhaPondInside = Ray_BabyPiranhaComponent::hasPiranha(_nextWaterPolyline->getRef());
        m_survivePiranhaPondOutside = bfalse;
        m_survivePiranhaPondTimer = 0.0f;
    }
}

//-------------------------------------------------------------------------------------
// Camera
Vec2d Ray_PlayerControllerComponent::getCurrentWindForce() const
{
    u32 numForces = m_currentWindForces.size();
    Vec2d totalForces = Vec2d::Zero;

    for ( u32 i = 0; i < numForces; i++ )
    {
        totalForces += m_currentWindForces[i].m_force;
    }

    return totalForces;
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_PlayerControllerComponent::drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    Super::drawEdit(drawInterface, _flags);

    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    Vec2d pos = m_actor->get2DPos();
    Angle angle(bfalse,m_actor->getAngle());

    if ( m_currentState )
    {
        m_currentState->drawDebug();
    }

    if ( isSupporting() )
    {
        Vec2d supportPos = getPedestalPos();

        if ( supportPos != Vec2d::Zero )
        {
            f32 scale = (m_actor->getScale().m_x/m_actor->getWorldInitialScale().m_x);
            f32 areaRadius = getTemplate()->getPedestalAreaRadius()*scale;
            PhysShapeCircle circle(areaRadius);

            DebugDraw::shape(supportPos, m_actor->getDepth(), m_actor->getAngle(), &circle,
                Color::white(), 0.f, "pedestal", 65 * MTH_DEGTORAD);
        }
    }

    if ( m_currentState == &m_stateJumping || m_currentState == &m_stateFalling )
    {
        Vec2d feetPos = m_actor->get2DPos() + ( ( Vec2d::Right.Rotate(m_actor->getAngle()-MTH_PIBY2) ) * m_characterPhys->getRadius() );

        PhysShapeCircle circle(getTemplate()->getPedestalFeetDistanceCheck());

        DebugDraw::shape(feetPos, m_actor->getDepth(), m_actor->getAngle(), &circle,
            Color::white(), 0.f, "feet pedestal check", 65 * MTH_DEGTORAD);
    }

    DebugDraw::circle(m_actor->getPos(), getSoftCollisionRadius(), Color::pink(), 0.f, "softCol", MTH_PIBY4 * 3);
}

void Ray_PlayerControllerComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    if ( isClimbing() )
    {
        UpdatePhysicClimbingEdge(0.f);
    }
    else if ( isHanging() )
    {
        Vec2d hangPos;
        f32 hangAngle;
        getWorldHangPos(hangPos,hangAngle);
        m_animComponent->lockMagicBox(hangPos.to3d(m_actor->getPos().m_z));
        m_actor->setAngle(hangAngle);
    }

    if ( m_currentState )
    {
        m_currentState->onEditorMove(_modifyInitialPos);
    }
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_PlayerControllerComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_PlayerControllerComponent_Template)

    SERIALIZE_MEMBER("controllerHeight",m_height);
    SERIALIZE_MEMBER("controllerFlyModeSpeed",m_flyModeSpeed);
    SERIALIZE_MEMBER("controllerWalkSpeed",m_walkForce);
    SERIALIZE_MEMBER("controllerWalkMiniJumpForce",m_walkMiniJumpForce);
    SERIALIZE_MEMBER("controllerWalkMiniJumpMaxStep",m_walkMiniJumpMaxStep);
    SERIALIZE_MEMBER("controllerWalkIceCompensationFrictionMultiplier",m_iceCompensateFrictionMultiplier);
    SERIALIZE_MEMBER("controllerAirSpeed",m_airForce);
    SERIALIZE_MEMBER("controllerAirSpeedWallJump",m_airForceWallJump);
    SERIALIZE_MEMBER("controllerAirControlDelayWallJump",m_airControlDelayWallJump);
    SERIALIZE_MEMBER("controllerAirControlDelayDelayFall",m_airControlDelayFall);
    SERIALIZE_MEMBER("controllerAirWallJumpExtraRadius",m_airWallJumpExtraRadius);
    SERIALIZE_MEMBER("controllerAirWallJumpCornerCheckDistance",m_airWallJumpCornerCheck);
    SERIALIZE_MEMBER("controllerAirTopXSpeed",m_airTopXSpeed);
    SERIALIZE_MEMBER("controllerAirNoMoveFrictionX",m_airNoMoveFrictionX);
    SERIALIZE_MEMBER("controllerWallRunJumpDuration",m_wallRunJumpDuration);
    SERIALIZE_MEMBER("controllerWallRunJumpAirControlDelay",m_wallRunJumpAirControlDelay);
    SERIALIZE_MEMBER("controllerWallRunJumpKeepSpeedFactor",m_wallRunJumpKeepSpeedFactor);
    SERIALIZE_MEMBER("controllerWallRunJumpKeepForce",m_wallRunJumpForce);
    SERIALIZE_MEMBER("controllerWallRunMinSpeed",m_wallRunMinSpeed);
    SERIALIZE_MEMBER("controllerWallRunUpThreshold",m_wallRunUpThreshold);
    SERIALIZE_MEMBER("controllerSwimWaterResistMinSpeed",m_swimingWaterResistMinSpeed);
    SERIALIZE_MEMBER("controllerSwimWaterResistMaxSpeed",m_swimingWaterResistMaxSpeed);
    SERIALIZE_MEMBER("controllerSwimWaterResistMaxMultiplier",m_swimingWaterResistMaxMult);
    SERIALIZE_MEMBER("controllerSwimWaterResistEnterDuration",m_swimingWaterResistEnterDuration);
    SERIALIZE_MEMBER("controllerSwimWaterResistEnterForceSpeedMultiplier",m_swimingWaterResistEnterForceSpeedMultiplier);
    SERIALIZE_MEMBER("controllerSwimWaterResistEnterDownForceSpeedMultiplier",m_swimingWaterResistEnterDownForceSpeedMultiplier);
    SERIALIZE_MEMBER("controllerSwimTopSpeed",m_swimingTopSpeed);
    SERIALIZE_MEMBER("controllerSwimForce",m_swimingForce);
    SERIALIZE_MEMBER("controllerSwimMinExitSpeed",m_swimingMinExitSpeed);
    SERIALIZE_MEMBER("controllerSwimSurfaceForceDistanceMultiplierInside",m_swimingSurfaceForceDistMultInside);
    SERIALIZE_MEMBER("controllerSwimSurfaceForceSpeedMultiplierInside",m_swimingSurfaceForceSpeedMultInside);
    SERIALIZE_MEMBER("controllerSwimSurfaceForceDistanceMultiplierOutside",m_swimingSurfaceForceDistMultOutside);
    SERIALIZE_MEMBER("controllerSwimSurfaceForceSpeedMultiplierOutisde",m_swimingSurfaceForceSpeedMultOutside);
    SERIALIZE_MEMBER("controllerSwimSurfaceRange",m_swimingSurfaceRange);
    SERIALIZE_MEMBER("controllerSwimJumpRange",m_swimingJumpRange);
    SERIALIZE_MEMBER("controllerSwimTimeAllowAction",m_swimingTimeAllowAction);
    SERIALIZE_MEMBER("controllerSwimJumpDelayAfterEnter",m_swimingRegainControl);
    SERIALIZE_MEMBER("controllerSwimTurnDistanceMultiplier",m_swimingTurnDistMult);
    SERIALIZE_MEMBER("controllerSwimTurnSpeedMultiplier",m_swimingTurnSpeedMult);
    SERIALIZE_MEMBER("controllerSwimSprintMaxSpeedMultiplier",m_swimingSprintMaxSpeedMultiplier);
    SERIALIZE_MEMBER("controllerSwimCurveMaxOffset",m_swimingCurveMaxOffset);
    SERIALIZE_MEMBER("controllerSwimCurveMaxDif",m_swimingCurveMaxDif);
    SERIALIZE_MEMBER("controllerSwimCurveForceS",m_swimingCurveForceS);
    SERIALIZE_MEMBER("controllerSwimCurveForceD",m_swimingCurveForceD);
    SERIALIZE_MEMBER("controllerSwimTurnUpsideDownWait",m_swimingTurnUpsideDownWait);
    SERIALIZE_MEMBER("controllerSwimSprintJumpMinSpeed",m_swimingSprintJumpMinSpeed);
    SERIALIZE_MEMBER("controllerSwimSprintJumpGravityMultiplier",m_swimingSprintJumpGravityMult);
    SERIALIZE_MEMBER("controllerSwimSprintJumpGravityDuration",m_swimingSprintJumpGravityDuration);
    SERIALIZE_MEMBER("controllerSwimSprintJumpAngularSpeedMultiplier",m_swimingSprintJumpAngularSpeedMult);
    SERIALIZE_MEMBER("controllerSwimAccelerationBlendTime",m_swimingMoveTargetMultiplierBlendTime);
    SERIALIZE_MEMBER("controllerSwimDolphinYSpeed",m_swimingDolphinYSpeed);
    SERIALIZE_MEMBER("controllerSwimDolphinWallJumpDisableTimer",m_swimingDolphinWallJumpDisableTimer);
    SERIALIZE_MEMBER("controllerSwimPushThreshold",m_swimingPushThreshold);
    SERIALIZE_MEMBER("controllerSwimDashDuration",m_swimingDashDuration);
    SERIALIZE_MEMBER("controllerMoveTargetMultiplierMin",m_moveTargetMultiplierMin);
    SERIALIZE_MEMBER("controllerMoveTargetMultiplierMax",m_moveTargetMultiplierMax);
    SERIALIZE_MEMBER("controllerMoveTargetBlendTime",m_moveTargetMultiplierBlendTime);
    SERIALIZE_MEMBER("controllerJumpQueueTime",m_jumpQueueTime);
    SERIALIZE_MEMBER("controllerHitQueueTime",m_hitQueueTime);
    SERIALIZE_MEMBER("controllerHitDisableTimeOnBounce",m_hitDisableOnBounce);
    SERIALIZE_MEMBER("controllerHitFilterNonFacingOpponentSpeedX",m_hitFilterNonFacingOpponentSpeedX);
    SERIALIZE_MEMBER("controllerHitQueueBlockingHitsMinDistance",m_hitQueueBlockingHitsMinDistance);
    SERIALIZE_MEMBER("controllerWindDisableOnBounce",m_windDisableOnBounce);
    SERIALIZE_MEMBER("controllerJumpOnAirTime",m_jumpOnAirTime);
    SERIALIZE_MEMBER("controllerJumpForceY",m_jumpForceY);
    SERIALIZE_MEMBER("controllerJumpForceYBounceMultiplier",m_jumpForceYBounceMultiplier);
    SERIALIZE_MEMBER("controllerJumpMultiplierClimbH",m_jumpMultiplierClimbH);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulseClimbH",m_jumpMultiplierNoImpulseClimbH);
    SERIALIZE_MEMBER("controllerJumpImpulseXClimbH",m_jumpImpulseXClimbH);
    SERIALIZE_MEMBER("controllerJumpMultiplierClimbV",m_jumpMultiplierClimbV);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulseClimbV",m_jumpMultiplierNoImpulseClimbV);
    SERIALIZE_MEMBER("controllerJumpImpulseXClimbV",m_jumpImpulseXClimbV);
    SERIALIZE_MEMBER("controllerJumpMultiplierHang",m_jumpMultiplierHang);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulseHang",m_jumpMultiplierNoImpulseHang);
    SERIALIZE_MEMBER("controllerJumpImpulseXHang",m_jumpImpulseXHang);
    SERIALIZE_MEMBER("controllerJumpMultiplierRolling",m_jumpMultiplierRolling);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulseRolling",m_jumpMultiplierNoImpulseRolling);
    SERIALIZE_MEMBER("controllerJumpImpulseXRolling",m_jumpImpulseXRolling);
    SERIALIZE_MEMBER("controllerJumpMultiplierCrouch",m_jumpMultiplierCrouch);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulseCrouch",m_jumpMultiplierNoImpulseCrouch);
    SERIALIZE_MEMBER("controllerJumpImpulseXCrouch",m_jumpImpulseXCrouch);
    SERIALIZE_MEMBER("controllerJumpMultiplierFallOnVictim",m_jumpMultiplierFallOnVictim);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulseFallOnVictim",m_jumpMultiplierNoImpulseFallOnVictim);
    SERIALIZE_MEMBER("controllerJumpImpulseXFallOnVictim",m_jumpImpulseXFallOnVictim);
    SERIALIZE_MEMBER("controllerJumpMultiplierPedestal",m_jumpMultiplierPedestal);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulsePedestal",m_jumpMultiplierNoImpulsePedestal);
    SERIALIZE_MEMBER("controllerJumpImpulseXPedestal",m_jumpImpulseXPedestal);
    SERIALIZE_MEMBER("controllerJumpMultiplierOnPedestal",m_jumpMultiplierOnPedestal);
    SERIALIZE_MEMBER("controllerJumpMultiplierNoImpulseOnPedestal",m_jumpMultiplierNoImpulseOnPedestal);
    SERIALIZE_MEMBER("controllerJumpImpulseXOnPedestal",m_jumpImpulseXOnPedestal);
    SERIALIZE_MEMBER("controllerDelayToAirSuspension",m_airSuspensionDelay);
    SERIALIZE_MEMBER("controllerAirSuspensionPushThreshold",m_airSuspensionPushThreshold);
    SERIALIZE_MEMBER("controllerAirSuspensionMinYSpeed",m_airSuspensionMinYSpeed);
    SERIALIZE_MEMBER("controllerAirSuspensionMaxYSpeed",m_airSuspensionMaxYSpeed);
    SERIALIZE_MEMBER("controllerAirSuspensionMinMultiplier",m_airSuspensionMinMultiplier);
    SERIALIZE_MEMBER("controllerAirSuspensionMaxMultiplier",m_airSuspensionMaxMultiplier);
    SERIALIZE_MEMBER("controllerEfficiencyMaxSpeed",m_efficiencyMaxSpeed);
    SERIALIZE_MEMBER("controllerAchievedSpeedFactorDefault",m_achievedSpeedFactorDefault);
    SERIALIZE_MEMBER("controllerAchievedSpeedS",m_achievedSpeedFactorS);
    SERIALIZE_MEMBER("controllerAchievedSpeedD",m_achievedSpeedFactorD);
    SERIALIZE_MEMBER("controllerEfficiencyMinGroundAngle",m_minGroundAngleEfficiencyAngle);
    SERIALIZE_MEMBER("controllerEfficiencyMaxGroundAngle",m_maxGroundAngleEfficiencyAngle);
    SERIALIZE_MEMBER("controllerEfficiencyMinAngleMultiplier",m_minGroundAngleEfficiencyMult);
    SERIALIZE_MEMBER("controllerEfficiencyMaxAngleMultiplier",m_maxGroundAngleEfficiencyMult);
    SERIALIZE_MEMBER("controllerCornerPoseExtraDistance",m_cornerPoseExtraDist);
    SERIALIZE_MEMBER("controllerHangingGroundMaxAngle",m_hangMaxGroundAngle);
    SERIALIZE_MEMBER("controllerHangingRadiusCheck",m_hangRadiusCheck);
    SERIALIZE_MEMBER("controllerHangingCornerMaxSpeed",m_hangCornerMaxSpeed);
    SERIALIZE_MEMBER("controllerHangingOffsetSolid",m_hangingCollisionCheckOffsetSolid);
    SERIALIZE_MEMBER("controllerHangingOffsetCoop",m_hangingCollisionCheckOffsetCoop);
    SERIALIZE_MEMBER("controllerHangCoopMaxDistance",m_hangCoopDistanceCheck);
    SERIALIZE_MEMBER("controllerHangingRectSize",m_hangRectSize);
    SERIALIZE_MEMBER("controllerHangingCollisionExtentSolid",m_hangCollisionExtentSolid);
    SERIALIZE_MEMBER("controllerHangingCollisionExtentCoop",m_hangCollisionExtentCoop);
    SERIALIZE_MEMBER("controllerHangingRectOffset",m_hangRectOffset);
    SERIALIZE_MEMBER("controllerHangingMaxYSpeed",m_hangMaxSpeed);
    SERIALIZE_MEMBER("controllerHangingActivationTime",m_hangEdgeActivationTime);
    SERIALIZE_MEMBER("helicopterForceX",m_helicopterForceX);
    SERIALIZE_MEMBER("helicopterUnstickMinFrictionMultiplier",m_helicopterUnstickMinFrictionMultiplier);
    SERIALIZE_MEMBER("helicopterMinDragSpeedX",m_helicopterMinDragSpeedX);
    SERIALIZE_MEMBER("helicopterAirFrictionMultiplier",m_helicopterAirFrictionMultiplier);
    SERIALIZE_MEMBER("helicopterDragForceX",m_helicopterDragForceX);
    SERIALIZE_MEMBER("helicopterWindForcesAngle",m_helicopterWindForcesAngle);
    SERIALIZE_MEMBER("helicopterWindForcesMultiplier",m_helicopterWindForcesMultiplier);
    SERIALIZE_MEMBER("helicopterWindSpeedAngle",m_helicopterWindSpeedAngle);
    SERIALIZE_MEMBER("helicopterWindSpeedMultiplier",m_helicopterWindSpeedMultiplier);
    SERIALIZE_MEMBER("helicopterReleaseDelay",m_helicopterReleaseDelay);
    SERIALIZE_MEMBER("helicopterMaxSpeedUp",m_helicopterMaxSpeedUp);
    SERIALIZE_MEMBER("helicopterMaxSpeedDown",m_helicopterMaxSpeedDown);
    SERIALIZE_MEMBER("helicopterGravityMultiplier",m_helicopterGravityMultiplier);
    SERIALIZE_MEMBER("helicopterCooldown",m_helicopterCooldown);
    SERIALIZE_MEMBER("helicopterSuspensionTime",m_helicopterSuspensionTime);
    SERIALIZE_MEMBER("helicopterSuspensionMultiplier",m_helicopterSuspensionMultiplier);
    SERIALIZE_MEMBER("airHitForceApplicationTime",m_airHitForceApplicationTime);
    SERIALIZE_MEMBER("airHitUnstickMinFrictionMultiplier",m_airHitUnstickMinFrictionMultiplier);
    SERIALIZE_MEMBER("airHitMinDragSpeedX",m_airHitMinDragSpeedX);
    SERIALIZE_MEMBER("airHitDragForceX",m_airHitDragForceX);
    SERIALIZE_MEMBER("airHitSpeedMultiplier",m_airHitSpeedMultiplier);
    SERIALIZE_MEMBER("airHitSpeedMultiplierReduced",m_airHitSpeedMultiplierReduced);
    SERIALIZE_MEMBER("airHitGravityMultiplier",m_airHitGravityMultiplier);
    SERIALIZE_MEMBER("rehitTimer",m_rehitTimer);
    SERIALIZE_MEMBER("actionPoseFightDuration",m_fightPoseDuration);
    SERIALIZE_MEMBER("minBounceAngle",m_minBounceAngle);
    SERIALIZE_MEMBER("bounceHeightPlatformLevel1",m_bounceHeightPlatformLevel1);
    SERIALIZE_MEMBER("bounceHeightPlatformLevel2",m_bounceHeightPlatformLevel2);
    SERIALIZE_MEMBER("bounceHeightPlatformLevel3",m_bounceHeightPlatformLevel3);
    SERIALIZE_MEMBER("bounceHeightEnemyLevel1",m_bounceHeightEnemyLevel1);
    SERIALIZE_MEMBER("bounceHeightEnemyLevel2",m_bounceHeightEnemyLevel2);
    SERIALIZE_MEMBER("bounceHeightEnemyLevel3",m_bounceHeightEnemyLevel3);
    SERIALIZE_MEMBER("bounceHeightTalkingHatLevel1",m_bounceHeightTalkingHatLevel1);
    SERIALIZE_MEMBER("bounceHeightTalkingHatLevel2",m_bounceHeightTalkingHatLevel2);
    SERIALIZE_MEMBER("bounceHeightTalkingHatLevel3",m_bounceHeightTalkingHatLevel3);
    SERIALIZE_MEMBER("bounceHeightPolylineLevel1",m_bounceHeightPolylineLevel1);
    SERIALIZE_MEMBER("bounceHeightPolylineLevel2",m_bounceHeightPolylineLevel2);
    SERIALIZE_MEMBER("bounceHeightPolylineLevel3",m_bounceHeightPolylineLevel3);
    SERIALIZE_MEMBER("bounceHeightWave",m_bounceHeightWave);
    SERIALIZE_MEMBER("bounceHeightWaveLevel3", m_bounceHeightWaveLevel3);
    SERIALIZE_MEMBER("bounceEnemySpeedXMultiplier",m_bounceEnemyXSpeedMultiplier);
    SERIALIZE_MEMBER("bouncePlatformAirControlDelayLevel1",m_bouncePlatformAirControlDelayLevel1);
    SERIALIZE_MEMBER("bouncePlatformAirControlDelayLevel2",m_bouncePlatformAirControlDelayLevel2);
    SERIALIZE_MEMBER("bouncePlatformAirControlDelayLevel3",m_bouncePlatformAirControlDelayLevel3);
    SERIALIZE_MEMBER("bounceEnemyAirControlDelayLevel1",m_bounceEnemyAirControlDelayLevel1);
    SERIALIZE_MEMBER("bounceEnemyAirControlDelayLevel2",m_bounceEnemyAirControlDelayLevel2);
    SERIALIZE_MEMBER("bounceEnemyAirControlDelayLevel3",m_bounceEnemyAirControlDelayLevel3);
    SERIALIZE_MEMBER("bounceTalkingHatAirControlDelayLevel1",m_bounceTalkingHatAirControlDelayLevel1);
    SERIALIZE_MEMBER("bounceTalkingHatAirControlDelayLevel2",m_bounceTalkingHatAirControlDelayLevel2);
    SERIALIZE_MEMBER("bounceTalkingHatAirControlDelayLevel3",m_bounceTalkingHatAirControlDelayLevel3);
    SERIALIZE_MEMBER("bouncePolylineMaxXSpeed",m_bouncePolylineMaxXSpeed);
    SERIALIZE_MEMBER("bounceDelay",m_bounceDelay);
    SERIALIZE_MEMBER("bounceRepositionDuration",m_bounceRepositionDuration);
    SERIALIZE_MEMBER("bounceToActorYFuncPoint0Dist",m_bounceToActorYFuncPoint0Dist);
    SERIALIZE_MEMBER("bounceToActorYFuncPoint1Dist",m_bounceToActorYFuncPoint1Dist);
    SERIALIZE_MEMBER("bounceToActorXZFuncPoint0T",m_bounceToActorXZFuncPoint0T);
    SERIALIZE_MEMBER("bounceToActorXZFuncPoint1T",m_bounceToActorXZFuncPoint1T);
    SERIALIZE_MEMBER("bounceToActorSpeed",m_bounceToActorSpeed);
    SERIALIZE_MEMBER("bounceToActorMinTime",m_bounceToActorMinTime);
    SERIALIZE_MEMBER("windTunnelExitSpeedLevel0",m_windTunnelExitSpeedLevel0);
    SERIALIZE_MEMBER("windTunnelExitSpeedLevel1",m_windTunnelExitSpeedLevel1);
    SERIALIZE_MEMBER("windTunnelExitSpeedLevel2",m_windTunnelExitSpeedLevel2);
    SERIALIZE_MEMBER("controllerClimbCollisionCheckExtent",m_climbingCollisionCheckExtent);
    SERIALIZE_MEMBER("controllerClimbVerticalSpeedUp",m_climbVerticalSpeedUp);
    SERIALIZE_MEMBER("controllerClimbVerticalAscendAcceleration",m_climbVerticalAscendAccel);
    SERIALIZE_MEMBER("controllerClimbVerticalDescendAcceleration",m_climbVerticalDescendAccel);
    SERIALIZE_MEMBER("controllerClimbVerticalDescendMaxSpeed",m_climbVerticalDescendMaxSpeed);
    SERIALIZE_MEMBER("controllerClimbVerticalDescendMaxSpeedSprint",m_climbVerticalDescendMaxSpeedSprint);
    SERIALIZE_MEMBER("controllerClimbHorizontalSpeed",m_climbHorizontalSpeed);
    SERIALIZE_MEMBER("controllerClimbHorizontalMoveAnimRate",m_climbHorizontalMoveAnimRate);
    SERIALIZE_MEMBER("controllerClimbSprintSpeedMultiplier",m_climbSprintSpeedMultiplier);
    SERIALIZE_MEMBER("controllerClimbHorizontalDescendMaxSpeed",m_climbHorizontalDescendMaxSpeed);
    SERIALIZE_MEMBER("controllerClimbHorizontalDescendMaxSpeedSprint",m_climbHorizontalDescendMaxSpeedSprint);
    SERIALIZE_MEMBER("controllerClimbSlideDisableAirControl",m_climbSlideDisableAirControl);
    SERIALIZE_MEMBER("controllerClimbEdgeInertiaBrake",m_climbEdgeInertiaBrake);
    SERIALIZE_MEMBER("controllerClimbEdgeDistanceRange",m_climbEdgeDistanceRange);
    SERIALIZE_MEMBER("controllerClimbEdgeActivationTime",m_climbEdgeActivationTime);
    SERIALIZE_MEMBER("controllerClimbEdgeSwingImpulse",m_climbEdgeSwingImpulse);
    SERIALIZE_MEMBER("controllerClimbEdgeSwingImpulseReduced",m_climbEdgeSwingImpulseReduced);
    SERIALIZE_MEMBER("controllerClimbEdgePunchImpulse",m_climbEdgePunchImpulse);
    SERIALIZE_MEMBER("controllerClimbEdgeMoveImpulse",m_climbEdgeMoveImpulse);
    SERIALIZE_MEMBER("controllerClimbEdgeAngularSpeed",m_climbEdgeAngularSpeed);
    SERIALIZE_MEMBER("controllerClimbEdgeCoopHangEdgeRange",m_climbEdgeCoopHangEdgeRange);
    SERIALIZE_MEMBER("controllerClimbMaxJumpSpeedTransfer",m_climbMaxJumpSpeedTransfer);
    SERIALIZE_MEMBER("controllerPunchShapeOffset",m_punchShapeOffset);
    SERIALIZE_MEMBER("controllerReceivePunchFrontWeakForce",m_receivePunchFrontWeakForce);
    SERIALIZE_MEMBER("controllerReceivePunchFrontStrongForce",m_receivePunchFrontStrongForce);
    SERIALIZE_MEMBER("controllerReceivePunchFrontMegaForce",m_receivePunchFrontMegaForce);
    SERIALIZE_MEMBER("controllerReceivePunchFrontWeakFrictionMult",m_receivePunchFrontWeakFrictionMult);
    SERIALIZE_MEMBER("controllerReceivePunchFrontStrongFrictionMult",m_receivePunchFrontStrongFrictionMult);
    SERIALIZE_MEMBER("controllerReceivePunchFrontMegaFrictionMult",m_receivePunchFrontMegaFrictionMult);
    SERIALIZE_MEMBER("controllerReceivePunchUpWeakForce",m_receivePunchUpWeakForce);
    SERIALIZE_MEMBER("controllerReceivePunchUpStrongForce",m_receivePunchUpStrongForce);
    SERIALIZE_MEMBER("controllerReceivePunchUpMegaForce",m_receivePunchUpMegaForce);
    SERIALIZE_MEMBER("controllerReceivePunchUpWeakGravityMultiplier",m_receivePunchUpWeakGravityMult);
    SERIALIZE_MEMBER("controllerReceivePunchUpStrongGravityMultiplier",m_receivePunchUpStrongGravityMult);
    SERIALIZE_MEMBER("controllerReceivePunchUpMegaGravityMultiplier",m_receivePunchUpMegaGravityMult);
    SERIALIZE_MEMBER("controllerReceiveEarthquakeWeakForce",m_receiveEarthquakeWeakForce);
    SERIALIZE_MEMBER("controllerReceiveEarthquakeStrongForce",m_receiveEarthquakeStrongForce);
    SERIALIZE_MEMBER("controllerReceiveEarthquakeMegaForce",m_receiveEarthquakeMegaForce);
    SERIALIZE_MEMBER("controllerReceiveBounceVWeakForce",m_receiveBounceVWeakForce);
    SERIALIZE_MEMBER("controllerReceiveBounceVStrongForce",m_receiveBounceVStrongForce);
    SERIALIZE_MEMBER("controllerReceiveBounceVMegaForce",m_receiveBounceVMegaForce);
    SERIALIZE_MEMBER("controllerReceiveBounceHWeakForce",m_receiveBounceHWeakForce);
    SERIALIZE_MEMBER("controllerReceiveBounceHStrongForce",m_receiveBounceHStrongForce);
    SERIALIZE_MEMBER("controllerReceiveBounceHMegaForce",m_receiveBounceHMegaForce);
    SERIALIZE_MEMBER("controllerReceiveBounceWeakFrictionMultiplier",m_receiveBounceWeakFrictionMult);
    SERIALIZE_MEMBER("controllerReceiveBounceStrongFrictionMultiplier",m_receiveBounceStrongFrictionMult);
    SERIALIZE_MEMBER("controllerReceiveBounceMegaFrictionMultiplier",m_receiveBounceMegaFrictionMult);
    SERIALIZE_MEMBER("controllerReceiveBounceWeakSwimingMult",m_receiveBounceWeakSwimingMult);
    SERIALIZE_MEMBER("controllerReceiveBounceStrongSwimingMult",m_receiveBounceStrongSwimingMult);
    SERIALIZE_MEMBER("controllerReceiveBounceMegaSwimingMult",m_receiveBounceMegaSwimingMult);
    SERIALIZE_MEMBER("controllerReceiveHitSwimGravityMultiplier",m_receiveHitSwimGravityMultiplier);
    SERIALIZE_MEMBER("controllerSlideFrictionMultiplier",m_slideFrictionMultiplier);
    SERIALIZE_MEMBER("controllerSlideMinFastSpeed",m_slideMinFastSpeed);
    SERIALIZE_MEMBER("controllerSlideHitLevel",m_slideHitLevel);
    SERIALIZE_MEMBER("controllerSlideMinSpeed",m_slideMinSpeedToAllow);
    SERIALIZE_MEMBER("controllerSlideMinSpeedMultiplierForMinFriction",m_slideMinSpeedMultMinFriction);
    SERIALIZE_MEMBER("controllerSlideMinSpeedMultiplierForMaxFriction",m_slideMinSpeedMultMaxFriction);
    SERIALIZE_MEMBER("controllerSlideFallDelay",m_slideFallDelay);
    SERIALIZE_MEMBER("controllerSurfingJumpTime",m_surfingJumpTime);
    SERIALIZE_MEMBER("controllerHoverMinYSpeed",m_hoverMinYSpeed);
    SERIALIZE_MEMBER("controllerMettrailleMaxHits",m_metrailleMaxHits);
    SERIALIZE_MEMBER("controllerMettrailleFinalHitLevel",m_metrailleFinalHitLevel);
    SERIALIZE_MEMBER("controllerMettrailleGroundVariation",m_metrailleGroundVariation);
    SERIALIZE_MEMBER("controllerMettrailleAirVariation",m_metrailleAirVariation);
    SERIALIZE_MEMBER("controllerMettrailleMaxBufferedHits",m_metrailleMaxBufferedHits);
    SERIALIZE_MEMBER("controllerAccrobaticStageKeepTime",m_accrobaticStageKeepTime);
    SERIALIZE_MEMBER("controllerSwingAdjustTime",m_swingSpeedAdjustTime);
    SERIALIZE_MEMBER("controllerSwingInterpolateTime",m_swingInterpolateTime);
    SERIALIZE_MEMBER("controllerSwingHitAreaHeight",m_swingHitAreaHeight);
    SERIALIZE_MEMBER("controllerSwingHitAreaOffset",m_swingHitAreaOffset);
    SERIALIZE_MEMBER("controllerSwingHitMinAngularSpeed",m_swingHitMinAngularSpeed);
    SERIALIZE_MEMBER("controllerSwingInitialBoost",m_swingInitialBoost);
    SERIALIZE_MEMBER("controllerSwingJumpXForceThreshold",m_swingJumpXForceThreshold);
    SERIALIZE_MEMBER("controllerSwingJumpXAngleThreshold",m_swingJumpXAngleThreshold);
    SERIALIZE_MEMBER("controllerSwingJumpForceX",m_swingJumpForceX);
    SERIALIZE_MEMBER("controllerSwingJumpDisableDelay",m_swingJumpDelay);
    SERIALIZE_MEMBER("controllerUTurnHitLevel",m_uTurnHitLevel);
    SERIALIZE_MEMBER("controllerUTurnAirHitLevel",m_uTurnAirHitLevel);
    SERIALIZE_MEMBER("controllerCrouchedHitLevel",m_crouchHitLevel);
    SERIALIZE_MEMBER("controllerTornadoHitLevel",m_tornadoHitLevel);
    SERIALIZE_MEMBER("controllerSpinHitLevel",m_spinHitLevel);
    SERIALIZE_MEMBER("controllerReducedSizeHitLevel",m_reducedSizeHitLevel);
    SERIALIZE_MEMBER("controllerUpperkickHitLevel",m_upperKickHitLevel);
    SERIALIZE_MEMBER("controllerCrushHitLevel",m_crushHitLevel);
    SERIALIZE_MEMBER("controllerCrushHitIncreaseFirstTime",m_crushHitLevelIncrTimeFirst);
    SERIALIZE_MEMBER("controllerCrushHitIncreaseSecondTime",m_crushHitLevelIncrTimeSecond);
    SERIALIZE_MEMBER("controllerCrushHitRetriggerDelay",m_crushRetriggerDelay);
    SERIALIZE_MEMBER("controllerSlideDownHitLevel",m_slideDownHitLevel);
    SERIALIZE_MEMBER("controllerSwingHitLevel",m_swingHitLevel);
    SERIALIZE_MEMBER("controllerHangCoopHitLevel",m_hangCoopHitLevel);
    SERIALIZE_MEMBER("controllerBurstHitLevel",m_burstHitLevel);
    SERIALIZE_MEMBER("controllerBurstAirHitLevel",m_burstAirHitLevel);
    SERIALIZE_MEMBER("controllerNormalPunchLevelForShakeCam",m_normalPunchLevelForShakeCam);
    SERIALIZE_MEMBER("switchHitBufferTime",m_switchHitBufferTime);
    SERIALIZE_MEMBER("UTurnComboTime",m_uTurnComboTime);
    SERIALIZE_MEMBER("crushHitSuspensionTime",m_crushHitSuspensionTime);
    SERIALIZE_MEMBER("crushHitMinYSpeed",m_crushHitMinYSpeed);
    SERIALIZE_MEMBER("crushHitYSpeedMultiplier",m_crushHitYMultiplier);
    SERIALIZE_MEMBER("crushHitDownForce",m_crushHitDownForce);
    SERIALIZE_MEMBER("crushHitFrictionMultiplier",m_crushHitFriction);
    SERIALIZE_MEMBER("slideDownMinSpeed",m_slideDownHitMinSpeed);
    SERIALIZE_MEMBER("swimAttackNormalDuration",m_swimAttackNormalDuration);
    SERIALIZE_MEMBER("swimAttackNormalSpeed",m_swimAttackNormalSpeed);
    SERIALIZE_MEMBER("swimAttackSprintDuration",m_swimAttackSprintDuration);
    SERIALIZE_MEMBER("swimAttackSprintSpeed",m_swimAttackSprintSpeed);
    SERIALIZE_MEMBER("swimAttackCooldown",m_swimAttackCooldown);
    SERIALIZE_MEMBER("swimAttackFriction",m_swimAttackFriction);
    SERIALIZE_MEMBER("swimAttackMinSpeed",m_swimAttackMinSpeed);
    SERIALIZE_MEMBER("swimAttackMoveDamping",m_swimAttackMoveDamping);
    SERIALIZE_MEMBER("swimAttackSpamTime",m_swimAttackSpamTime);
    SERIALIZE_MEMBER("swimAttackSpamImpulseMultiplier",m_swimAttackSpamImpulseMultiplier);
    SERIALIZE_MEMBER("swimAttackSpamImpulseMultiplierSprint",m_swimAttackSpamImpulseMultiplierSprint);
    SERIALIZE_MEMBER("tornadoDuration",m_tornadoDuration);
    SERIALIZE_MEMBER("tornadoSpeed",m_tornadoSpeed);
    SERIALIZE_MEMBER("tornadoCooldown",m_tornadoCooldown);
    SERIALIZE_MEMBER("tornadoMinSpeed",m_tornadoMinSpeed);
    SERIALIZE_MEMBER("tornadoFrictionMultiplier",m_tornadoFriction);
    SERIALIZE_MEMBER("tornadoAllowMoveTime",m_tornadoAllowMoveTime);
    SERIALIZE_MEMBER("reducedSizeHitDuration",m_reducedSizeHitDuration);
    SERIALIZE_MEMBER("reducedSizeHitAirDuration",m_reducedSizeHitAirDuration);
    SERIALIZE_MEMBER("reducedSizeHitUpDuration",m_reducedSizeHitUpDuration);
    SERIALIZE_MEMBER("reducedSizeHitFriction",m_reducedSizeHitFriction);
    SERIALIZE_MEMBER("reducedSizeHitGravity",m_reducedSizeHitGravity);
    SERIALIZE_MEMBER("reducedSizeHitSideForceGround",m_reducedSizeHitSideForceGround);
    SERIALIZE_MEMBER("reducedSizeHitSideForceAir",m_reducedSizeHitSideForceAir);
    SERIALIZE_MEMBER("reducedSizeHitUpForce",m_reducedSizeHitUpForce);
    SERIALIZE_MEMBER("reducedSizeHitMinSpeed",m_reducedSizeHitMinSpeed);
    SERIALIZE_MEMBER("reducedSizeHitCrushAttackQueueLength",m_reducedSizeHitCrushAttackQueueLength);
    SERIALIZE_MEMBER("reducedSizeHitCrushDistance",m_reducedSizeHitCrushDistance);
    SERIALIZE_MEMBER("reducedSizeHitCooldown",m_reducedSizeHitCooldown);
    SERIALIZE_MEMBER("reducedSizeHitGroundFrictionMultiplierMin",m_reducedSizeHitGroundFrictionMultiplierMin);
    SERIALIZE_MEMBER("reducedSizeHitGroundFrictionMultiplierMax",m_reducedSizeHitGroundFrictionMultiplierMax);
    SERIALIZE_MEMBER("reducedSizeHitUpCooldown",m_reducedSizeHitUpCooldown);
    SERIALIZE_MEMBER("hitFxBoneName",m_hitFxBoneName);
    SERIALIZE_MEMBER("hitPrepareLengthWeak",m_prepareHitLengthWeak);
    SERIALIZE_MEMBER("hitPrepareLengthStrong",m_prepareHitLengthStrong);
    SERIALIZE_MEMBER("hitPrepareLengthMega",m_prepareHitLengthMega);
    SERIALIZE_MEMBER("reHitDelayPermanentHit",m_rehitDelayPermanentHit);
    SERIALIZE_MEMBER("reHitDelayComboHit",m_rehitDelayComboHit);
    SERIALIZE_MEMBER("wallSlideStartDelay",m_wallSlideStartDelay);
    SERIALIZE_MEMBER("wallSlideStartDelaySpeedThreshold",m_wallSlideStartDelaySpeedThreshold);
    SERIALIZE_MEMBER("wallSlideMinLinearSpeed",m_wallSlideAngularSpeedMinLinear);
    SERIALIZE_MEMBER("wallSlideMaxLinearSpeed",m_wallSlideAngularSpeedMaxLinear);
    SERIALIZE_MEMBER("wallSlideMinAngularSpeed",m_wallSlideAngularSpeedMin);
    SERIALIZE_MEMBER("wallSlideMaxAngularSpeed",m_wallSlideAngularSpeedMax);
    SERIALIZE_MEMBER("wallSlideBrakeTime",m_wallSlideBrakeTime);
    SERIALIZE_MEMBER("wallSlideStartSpeed",m_wallSlideStartSpeed);
    SERIALIZE_MEMBER("wallSlideGravityMultiplier",m_wallSlideGravityMultiplier);
    SERIALIZE_MEMBER("wallSlideJumpImpulseX",m_wallSlideJumpXImpulse);
    SERIALIZE_MEMBER("wallSlideFrictionCoefCondition",m_wallSlideFrictionCoefCondition);
    SERIALIZE_MEMBER("wallSlideMinimumEdgeMargin",m_wallSlideMinEdgeMargin);
    SERIALIZE_MEMBER("wallPushMaxWallAngle",m_wallPushWallAngle);
    SERIALIZE_MEMBER("wallPushForce",m_wallPushForce);
    SERIALIZE_MEMBER("wallPushDelay",m_wallPushDelay);
    SERIALIZE_MEMBER("wallPushRepushDelay",m_wallPushRepushDelay);
    SERIALIZE_MEMBER("wallSlideMaxSpeed",m_wallSlideMaxSpeed);
    SERIALIZE_MEMBER("wallSlideUnstickDelay",m_wallSlideUnstickDelay);
    SERIALIZE_MEMBER("wallSlideRestickDelay",m_wallSlideRestickDelay);
    SERIALIZE_MEMBER("wallSlideExtraRadius",m_wallSlideExtraRadius);
    SERIALIZE_MEMBER("wallSlideYSpeedThresholdToHang",m_wallSlideYSpeedThresholdToHang);
    SERIALIZE_MEMBER("disableCrouchTime",m_disableCrouchTime);
    SERIALIZE_MEMBER("crouchUnhangTime",m_crouchUnhangTime);
    SERIALIZE_MEMBER("rollingFrictionMultiplier",m_rollingFrictionMultiplier);
    SERIALIZE_MEMBER("rollingMinSpeed",m_rollingMinSpeed);
    SERIALIZE_MEMBER("fallOnVictimMinSpeed",m_fallOnVictimMinSpeed);
    SERIALIZE_MEMBER("sprintActivationLandingDelay",m_sprintActivationLandingDelay);
    SERIALIZE_MEMBER("sprintWalkForceMultiplier",m_sprintForceMultiplier);
    SERIALIZE_MEMBER("sprintEfficiencyMaxSpeedMultiplier",m_sprintEfficiencyMultiplier);
    SERIALIZE_MEMBER("sprintAccelerationDuration",m_sprintMaxSpeedBlendDuration);
    SERIALIZE_MEMBER("sprintReleaseForceMultiplier",m_sprintReleaseForceMultiplier);
    SERIALIZE_MEMBER("sprintReleaseForceMax",m_sprintReleaseForceMax);
    SERIALIZE_MEMBER("bounceToLayerFXFile",m_bounceToLayerFXFile);
    SERIALIZE_MEMBER("tinyFXFile",m_tinyFXFile);
    SERIALIZE_MEMBER("tinyTrailSprintColor",m_tinyTrailSprintColor);
    SERIALIZE_MEMBER("tinyTrailWallrunColor",m_tinyTrailWallRunColor);
    SERIALIZE_MEMBER("tinyTrailWallrunJumpColor",m_tinyTrailWallRunJumpColor);
    SERIALIZE_MEMBER("tinyTrailNormalColor",m_tinyTrailNormalColor);
    SERIALIZE_MEMBER("controllerPunchPushbackRadius",m_punchPushbackRadius);
    SERIALIZE_OBJECT_WITH_FACTORY("phantomShapeCrouch", m_phantomShapeCrouch, PHYSWORLD->getShapesFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("phantomShapeSwim", m_phantomShapeSwim, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("damageInmunityTime", m_inmunityTime);
    SERIALIZE_MEMBER("damageInmunityTimeReceiveHit", m_inmunityTimeReceiveHit);
    SERIALIZE_MEMBER("damageInmunityTimeRevive", m_inmunityTimeRevive);
    SERIALIZE_MEMBER("damageInmunityTimeReceiveHitRevive", m_inmunityTimeReceiveHitRevive);
    SERIALIZE_MEMBER("squashBounceMargin", m_squashBounceMargin);
    SERIALIZE_MEMBER("squashHurtLevel", m_squashHurtLevel);
    SERIALIZE_MEMBER("squashDeathPenetration", m_squashDeathPenetration);
    SERIALIZE_MEMBER("characterSnapBone", m_characterSnapBone);
    SERIALIZE_OBJECT("stateDeadSoul",m_stateDeadSoul);
    SERIALIZE_OBJECT("stateRevive",m_stateRevive);
    SERIALIZE_MEMBER("avoidanceRadius",m_avoidanceRadius);
    SERIALIZE_MEMBER("darktoonificationPosOffset",m_darktoonificationPosOffset);
    SERIALIZE_MEMBER("controllerPedestalWalkMultiplier",m_pedestalWalkMultiplier);
    SERIALIZE_MEMBER("controllerPedestalAreaRadius",m_pedestalAreaRadius);
    SERIALIZE_MEMBER("controllerPedestalDistanceCheck",m_pedestalDistanceCheck);
    SERIALIZE_MEMBER("controllerPedestalFeetDistanceCheck",m_pedestalFeetDistanceCheck);
    SERIALIZE_MEMBER("controllerPedestalDisableTimer",m_pedestalDisableTimer);
    SERIALIZE_MEMBER("controllerPedestalInputCone",m_pedestalInputCone);
    SERIALIZE_MEMBER("controllerOnPedestalLandBounceHeight",m_onPedestalLandBounceHeight);
    SERIALIZE_MEMBER("controllerDeathSeqWait",m_deathNormalWait);
    SERIALIZE_MEMBER("controllerMoveCursorMinEfficiency",m_moveCursorMinEfficiency);
    SERIALIZE_MEMBER("controllerMoveCursorMaxEfficiency",m_moveCursorMaxEfficiency);
    SERIALIZE_MEMBER("controllerMoveCursorMinAngle",m_moveCursorMinAngle);
    SERIALIZE_MEMBER("controllerMoveCursorMaxAngle",m_moveCursorMaxAngle);
    SERIALIZE_MEMBER("controllerMoveCursorMinMultiplier",m_moveCursorMinMultiplier);
    SERIALIZE_MEMBER("controllerMoveCursorMaxMultiplier",m_moveCursorMaxMultiplier);
    SERIALIZE_MEMBER("controllerMoveCursorEfficiencyAngle",m_moveCursorEfficiencyAngle);
    SERIALIZE_MEMBER("controllerWalkOnWallsDisableDelay",m_walkOnWallsDisableDelay);
    SERIALIZE_MEMBER("controllerWalkOnWallsReleaseStickDuration",m_walkOnWallsReleaseStickDuration);
    SERIALIZE_MEMBER("controllerDeathActorFXSpawn",m_deathFXActorSpawn);
    SERIALIZE_OBJECT("offscreenDeathMargin",m_offscreenDeathMargin);
    SERIALIZE_MEMBER("offscreenDeathSmooth",m_offscreenDeathSmooth);
    SERIALIZE_OBJECT("offscreenDeathFxMargin",m_offscreenDeathFxMargin);
    SERIALIZE_MEMBER("applyWindSpeedLimitation",m_applyWindSpeedLimitation);
    SERIALIZE_MEMBER("clampMaxSpeedTotal",m_clampingMaxSpeedTotal);
    SERIALIZE_MEMBER("clampMaxSpeedX",m_clampingMaxSpeedX);
    SERIALIZE_MEMBER("clampMaxSpeedY",m_clampingMaxSpeedY);
    SERIALIZE_MEMBER("airBrakeMaxSpeedX",m_airBrakeMaxSpeedX);
    SERIALIZE_MEMBER("softCollisionRadiusMultiplier",m_softCollisionRadiusMultiplier);
    SERIALIZE_MEMBER("softCollisionExitSpeed",m_softCollisionExitSpeed);
    SERIALIZE_MEMBER("softCollisionExitForce",m_softCollisionExitForce);
    SERIALIZE_MEMBER("softCollisionRestoreForceDelay",m_softCollisionRestoreForceDelay);
    SERIALIZE_MEMBER("softCollisionRestoreForceDuration",m_softCollisionRestoreForceDuration);
    SERIALIZE_MEMBER("cameraLimiterMaxCoeff",m_cameraLimiterMaxCoeff);

	SERIALIZE_MEMBER("stargateSuckinForceK",m_stargateSuckinK);
	SERIALIZE_MEMBER("stargateSuckinForceD",m_stargateSuckinD);
    SERIALIZE_MEMBER("stargateNoiseAmplitude",m_stargateNoiseAmplitude);
	SERIALIZE_MEMBER("stargateNoiseTimeMultiplier",m_stargateNoiseTimeMultiplier);
    SERIALIZE_MEMBER("stargateTravelRotationSpeedMultiplier",m_stargateTravelRotationSpeedMultiplier);
    SERIALIZE_MEMBER("stargateTravelDecelerateTime",m_stargateDecelerateTime);
    SERIALIZE_MEMBER("stargateTravelDecelerateMultiplier",m_stargateDecelerateMultiplier);
    SERIALIZE_MEMBER("stargateTravelTrailPath",m_stargateTravelTrailPath);

    SERIALIZE_MEMBER("deadSoulInactiveOptOutTime",m_deadSoulInactiveOptOutTime);

    SERIALIZE_OBJECT("heartShield",m_heartShield);
    SERIALIZE_OBJECT("superPunchBasicGauge",m_superPunchBasicGauge);
    SERIALIZE_OBJECT("superPunchSeekingGauge",m_superPunchSeekingGauge);
	SERIALIZE_OBJECT("swarmRepeller",m_swarmRepellerPowerUp);
END_SERIALIZATION()

Ray_PlayerControllerComponent_Template::Ray_PlayerControllerComponent_Template()
: m_hangMaxGroundAngle(btrue,45.f)
, m_climbingCollisionCheckExtent(0.08f,0.36f)
, m_jumpMultiplierClimbH(1.2f)
, m_jumpMultiplierNoImpulseClimbH(1.2f)
, m_jumpImpulseXClimbH(0.f)
, m_jumpMultiplierClimbV(1.2f)
, m_jumpMultiplierNoImpulseClimbV(1.2f)
, m_jumpImpulseXClimbV(0.f)
, m_jumpMultiplierHang(1.2f)
, m_jumpMultiplierNoImpulseHang(1.2f)
, m_jumpImpulseXHang(0.f)
, m_jumpMultiplierRolling(1.f)
, m_jumpMultiplierNoImpulseRolling(1.f)
, m_jumpImpulseXRolling(0.f)
, m_jumpMultiplierCrouch(1.f)
, m_jumpMultiplierNoImpulseCrouch(1.f)
, m_jumpImpulseXCrouch(0.f)
, m_jumpMultiplierFallOnVictim(1.5f)
, m_jumpMultiplierNoImpulseFallOnVictim(1.5f)
, m_jumpImpulseXFallOnVictim(0.f)
, m_jumpMultiplierPedestal(0.5f)
, m_jumpMultiplierNoImpulsePedestal(0.5f)
, m_jumpImpulseXPedestal(0.f)
, m_jumpMultiplierOnPedestal(1.f)
, m_jumpMultiplierNoImpulseOnPedestal(1.f)
, m_jumpImpulseXOnPedestal(250.f)
, m_airSuspensionDelay(0.05f)
, m_airSuspensionPushThreshold(10.f)
, m_swimingWaterResistMinSpeed(4.f)
, m_swimingWaterResistMaxSpeed(10.f)
, m_swimingWaterResistMaxMult(10.f)
, m_swimingWaterResistEnterDuration(1.f)
, m_swimingWaterResistEnterForceSpeedMultiplier(5.f)
, m_swimingWaterResistEnterDownForceSpeedMultiplier(3.f)
, m_swimingTopSpeed(3.f)
, m_swimingForce(10.f)
, m_swimingMinExitSpeed(2.f)
, m_swimingSurfaceForceDistMultInside(20.f)
, m_swimingSurfaceForceSpeedMultInside(5.f)
, m_swimingSurfaceForceDistMultOutside(100.f)
, m_swimingSurfaceForceSpeedMultOutside(60.f)
, m_swimingSurfaceRange(0.3f)
, m_swimingJumpRange(2.f)
, m_swimingTimeAllowAction(0.5f)
, m_swimingRegainControl(1.f)
, m_swimingTurnDistMult(50.f)
, m_swimingTurnSpeedMult(10.f)
, m_swimingSprintMaxSpeedMultiplier(2.f)
, m_swimingCurveMaxOffset(0.5f)
, m_swimingCurveMaxDif(1.57f)
, m_swimingCurveForceS(40.f)
, m_swimingCurveForceD(20.f)
, m_swimingTurnUpsideDownWait(0.35f)
, m_swimingSprintJumpMinSpeed(4.f)
, m_swimingSprintJumpGravityMult(0.0f)
, m_swimingSprintJumpGravityDuration(2.f)
, m_swimingSprintJumpAngularSpeedMult(0.4f)
, m_swimingMoveTargetMultiplierBlendTime(0.5f)
, m_swimingDolphinYSpeed(10.f)
, m_swimingDolphinWallJumpDisableTimer(0.3f)
, m_swimingPushThreshold(0.90f)
, m_swimingDashDuration(0.5f)
, m_phantomShapeCrouch(NULL)
, m_phantomShapeSwim(NULL)
, m_height(1.8f)
, m_flyModeSpeed(15.f)
, m_walkForce(500.f)
, m_walkMiniJumpForce(400.f)
, m_walkMiniJumpMaxStep(0.132f)
, m_iceCompensateFrictionMultiplier(5.f)
, m_airForce(40.f)
, m_airForceWallJump(5.f)
, m_airControlDelayWallJump(0.5f)
, m_airControlDelayFall(0.5f)
, m_airWallJumpExtraRadius(0.04f)
, m_airWallJumpCornerCheck(0.4f)
, m_airTopXSpeed(5.f)
, m_airNoMoveFrictionX(2.f)
, m_wallRunJumpDuration(1.f)
, m_wallRunJumpAirControlDelay(1.2f)
, m_wallRunJumpKeepSpeedFactor(0.5f)
, m_wallRunJumpForce(600.f)
, m_wallRunMinSpeed(8.f)
, m_wallRunUpThreshold(10.f)
, m_climbVerticalSpeedUp(2.f)
, m_climbVerticalAscendAccel(40.f)
, m_climbVerticalDescendAccel(20.f)
, m_climbVerticalDescendMaxSpeed(10.f)
, m_climbVerticalDescendMaxSpeedSprint(15.f)
, m_climbHorizontalSpeed(4.f)
, m_climbSprintSpeedMultiplier(1.5f)
, m_climbHorizontalDescendMaxSpeed(8.f)
, m_climbHorizontalDescendMaxSpeedSprint(12.f)
, m_climbSlideDisableAirControl(0.5f)
, m_climbHorizontalMoveAnimRate(5.f)
, m_climbEdgeInertiaBrake(40.f)
, m_climbEdgeDistanceRange(0.5f)
, m_climbEdgeActivationTime(0.5f)
, m_hangEdgeActivationTime(0.25f)
, m_climbEdgeSwingImpulse(40.f)
, m_climbEdgeSwingImpulseReduced(10.f)
, m_climbEdgePunchImpulse(200.f)
, m_climbEdgeMoveImpulse(500.f)
, m_climbEdgeAngularSpeed(btrue,359.f)
, m_climbEdgeCoopHangEdgeRange(1.16f)
, m_climbMaxJumpSpeedTransfer(10.f,10.f)
, m_punchShapeOffset(0.2f,0.2f)
, m_wallSlideStartDelay(0.1f)
, m_wallSlideStartDelaySpeedThreshold(4.f)
, m_wallSlideAngularSpeedMinLinear(1.f)
, m_wallSlideAngularSpeedMaxLinear(5.f)
, m_wallSlideAngularSpeedMin(bfalse,MTH_2PI)
, m_wallSlideAngularSpeedMax(bfalse,MTH_2PI*2.f)
, m_wallSlideBrakeTime(0.2f)
, m_wallSlideStartSpeed(1.f)
, m_wallSlideGravityMultiplier(0.3f)
, m_wallSlideJumpXImpulse(250.f)
, m_wallSlideFrictionCoefCondition(0.36f)
, m_wallSlideMinEdgeMargin(1.f)
, m_wallPushWallAngle(btrue,112.5f)
, m_wallPushForce(100.f)
, m_wallPushDelay(0.5f)
, m_wallPushRepushDelay(0.3f)
, m_wallSlideMaxSpeed(10.f)
, m_wallSlideUnstickDelay(0.2f)
, m_wallSlideRestickDelay(0.2f)
, m_wallSlideExtraRadius(0.05f)
, m_wallSlideYSpeedThresholdToHang(4.f)
, m_disableCrouchTime(1.f)
, m_crouchUnhangTime(0.25f)
, m_rollingFrictionMultiplier(0.1f)
, m_rollingMinSpeed(3.f)
, m_fallOnVictimMinSpeed(8.f)
, m_sprintActivationLandingDelay(1.f)
, m_sprintForceMultiplier(1.f)
, m_sprintEfficiencyMultiplier(3.f)
, m_sprintMaxSpeedBlendDuration(1.2f)
, m_sprintReleaseForceMultiplier(0.8f)
, m_sprintReleaseForceMax(100.f)
, m_tinyTrailSprintColor(Color::blue())
, m_tinyTrailWallRunColor(Color::yellow())
, m_tinyTrailWallRunJumpColor(Color::yellow())
, m_tinyTrailNormalColor(Color::white())
, m_moveTargetMultiplierMin(0.f)
, m_moveTargetMultiplierMax(1.f)
, m_moveTargetMultiplierBlendTime(0.5f)
, m_jumpQueueTime(0.2f)
, m_hitQueueTime(0.2f)
, m_hitDisableOnBounce(0.05f)
, m_hitFilterNonFacingOpponentSpeedX(5.f)
, m_hitQueueBlockingHitsMinDistance(0.2f)   // 20 cm
, m_windDisableOnBounce(0.5f)
, m_jumpOnAirTime(0.2f)
, m_jumpForceY(600.f)
, m_jumpForceYBounceMultiplier(2.f)
, m_airSuspensionMinYSpeed(0.f)
, m_airSuspensionMaxYSpeed(8.f)
, m_airSuspensionMinMultiplier(0.25f)
, m_airSuspensionMaxMultiplier(0.5f)
, m_efficiencyMaxSpeed(5.43f)
, m_achievedSpeedFactorDefault(0.75f)
, m_achievedSpeedFactorS(10.f)
, m_achievedSpeedFactorD(4.f)
, m_minGroundAngleEfficiencyAngle(btrue,45.f)
, m_maxGroundAngleEfficiencyAngle(btrue,75.f)
, m_minGroundAngleEfficiencyMult(1.f)
, m_maxGroundAngleEfficiencyMult(0.f)
, m_cornerPoseExtraDist(0.2f)
, m_hangRadiusCheck(0.32f)
, m_hangCornerMaxSpeed(3.f)
, m_hangRectOffset(0.3f,1.f)
, m_hangRectSize(0.5f,1.5f)
, m_hangCollisionExtentSolid(0.2f,0.6f)
, m_hangCollisionExtentCoop(0.2f,0.8f)
, m_hangingCollisionCheckOffsetSolid(0.3f,0.2f)
, m_hangingCollisionCheckOffsetCoop(0.3f,0.9f)
, m_hangCoopDistanceCheck(0.5f)
, m_hangMaxSpeed(3.f)
, m_helicopterForceX(100.0f)
, m_helicopterUnstickMinFrictionMultiplier(0.1f)
, m_helicopterMinDragSpeedX(3.0f)
, m_helicopterDragForceX(20.0f)
, m_helicopterWindForcesAngle(btrue,45.f)
, m_helicopterWindForcesMultiplier(0.2f)
, m_helicopterWindSpeedAngle(btrue,45.f)
, m_helicopterWindSpeedMultiplier(1.f, 0.85f)
, m_helicopterReleaseDelay(0.2f)
, m_helicopterAirFrictionMultiplier(2.0f)
, m_helicopterMaxSpeedUp(10.f)
, m_helicopterMaxSpeedDown(4.f)
, m_helicopterGravityMultiplier(0.5f)
, m_helicopterCooldown(0.1f)
, m_helicopterSuspensionTime(0.5f)
, m_helicopterSuspensionMultiplier(0.5f)
, m_airHitForceApplicationTime(0.5f)
, m_airHitUnstickMinFrictionMultiplier(0.1f)
, m_airHitMinDragSpeedX(3.0f)
, m_airHitDragForceX(20.0f)
, m_airHitGravityMultiplier(0.3f)
, m_airHitSpeedMultiplier(Vec2d(0.3f,0.3f))
, m_airHitSpeedMultiplierReduced(Vec2d(0.3f,0.3f))
, m_minBounceAngle(btrue,45.0f)
, m_bounceJumpMargin(0.1f)
, m_bounceHeightPlatformLevel1(3.5f)
, m_bounceHeightPlatformLevel2(5.5f)
, m_bounceHeightPlatformLevel3(7.5f)
, m_bounceHeightEnemyLevel1(3.5f)
, m_bounceHeightEnemyLevel2(5.5f)
, m_bounceHeightEnemyLevel3(7.5f)
, m_bounceHeightTalkingHatLevel1(2.5f)
, m_bounceHeightTalkingHatLevel2(4.5f)
, m_bounceHeightTalkingHatLevel3(6.5f)
, m_bounceHeightPolylineLevel1(3.5f)
, m_bounceHeightPolylineLevel2(5.5f)
, m_bounceHeightPolylineLevel3(7.5f)
, m_bounceHeightWave(5.f)
, m_bounceHeightWaveLevel3(10.f)
, m_bounceEnemyXSpeedMultiplier(1.f)
, m_bouncePlatformAirControlDelayLevel1(0.5f)
, m_bouncePlatformAirControlDelayLevel2(0.5f)
, m_bouncePlatformAirControlDelayLevel3(0.5f)
, m_bounceEnemyAirControlDelayLevel1(0.5f)
, m_bounceEnemyAirControlDelayLevel2(0.5f)
, m_bounceEnemyAirControlDelayLevel3(0.5f)
, m_bounceTalkingHatAirControlDelayLevel1(0.5f)
, m_bounceTalkingHatAirControlDelayLevel2(0.5f)
, m_bounceTalkingHatAirControlDelayLevel3(0.5f)
, m_bouncePolylineMaxXSpeed(10.f)
, m_bounceDelay(0.15f)
, m_bounceRepositionDuration(0.2f)
, m_bounceToActorYFuncPoint0Dist(5.f)
, m_bounceToActorYFuncPoint1Dist(5.f)
, m_bounceToActorXZFuncPoint0T(0.3f)
, m_bounceToActorXZFuncPoint1T(0.85f)
, m_bounceToActorSpeed(20.f)
, m_bounceToActorMinTime(1.f)
, m_windTunnelExitSpeedLevel0(20.f)
, m_windTunnelExitSpeedLevel1(22.f)
, m_windTunnelExitSpeedLevel2(24.f)
, m_receivePunchFrontWeakForce(200.f)
, m_receivePunchFrontStrongForce(400.f)
, m_receivePunchFrontMegaForce(600.f)
, m_receivePunchFrontWeakFrictionMult(0.3f)
, m_receivePunchFrontStrongFrictionMult(0.2f)
, m_receivePunchFrontMegaFrictionMult(0.1f)
, m_receivePunchUpWeakForce(400.f)
, m_receivePunchUpStrongForce(800.f)
, m_receivePunchUpMegaForce(1200.f)
, m_receivePunchUpWeakGravityMult(0.5f)
, m_receivePunchUpStrongGravityMult(0.3f)
, m_receivePunchUpMegaGravityMult(0.2f)
, m_receiveEarthquakeWeakForce(5000.f)
, m_receiveEarthquakeStrongForce(5000.f)
, m_receiveEarthquakeMegaForce(5000.f)
, m_receiveBounceVWeakForce(400.f)
, m_receiveBounceVStrongForce(500.f)
, m_receiveBounceVMegaForce(600.f)
, m_receiveBounceHWeakForce(400.f)
, m_receiveBounceHStrongForce(500.f)
, m_receiveBounceHMegaForce(600.f)
, m_receiveBounceWeakFrictionMult(0.3f)
, m_receiveBounceStrongFrictionMult(0.3f)
, m_receiveBounceMegaFrictionMult(0.2f)
, m_receiveBounceWeakSwimingMult(0.5f)
, m_receiveBounceStrongSwimingMult(0.4f)
, m_receiveBounceMegaSwimingMult(0.3f)
, m_receiveHitSwimGravityMultiplier(0.8f)
, m_slideFrictionMultiplier(0.1f)
, m_slideMinFastSpeed(4.f)
, m_slideHitLevel(1)
, m_slideMinSpeedToAllow(3.f)
, m_slideMinSpeedMultMinFriction(0.1f)
, m_slideMinSpeedMultMaxFriction(1.f)
, m_slideFallDelay(0.3f)
, m_surfingJumpTime(0.1f)
, m_hoverMinYSpeed(4.f)
, m_metrailleMaxHits(3)
, m_metrailleFinalHitLevel(1)
, m_metrailleGroundVariation(2)
, m_metrailleAirVariation(7)
, m_metrailleMaxBufferedHits(4)
, m_accrobaticStageKeepTime(0.1f)
, m_swingSpeedAdjustTime(3.f)
, m_swingInterpolateTime(1.f)
, m_swingHitAreaHeight(1.f)
, m_swingHitAreaOffset(0.5f)
, m_swingHitMinAngularSpeed(3.f)
, m_swingInitialBoost(2.f)
, m_swingJumpXForceThreshold(2.f)
, m_swingJumpXAngleThreshold(0.5f)
, m_swingJumpForceX(200.f)
, m_swingJumpDelay(0.1f)
, m_inmunityTime(1.f)
, m_inmunityTimeReceiveHit(0.2f)
, m_inmunityTimeRevive(1.f)
, m_inmunityTimeReceiveHitRevive(0.2f)
, m_squashBounceMargin(0.5f)
, m_squashHurtLevel(0)
, m_squashDeathPenetration(0.6f)
, m_rehitTimer(0.2f)
, m_fightPoseDuration(5.f)
, m_uTurnHitLevel(1)
, m_uTurnAirHitLevel(2)
, m_crouchHitLevel(1)
, m_tornadoHitLevel(1)
, m_spinHitLevel(1)
, m_reducedSizeHitLevel(1)
, m_upperKickHitLevel(1)
, m_crushHitLevel(0)
, m_crushHitLevelIncrTimeFirst(0.25f)
, m_crushHitLevelIncrTimeSecond(0.33f)
, m_crushRetriggerDelay(1.f)
, m_slideDownHitLevel(0)
, m_swingHitLevel(0)
, m_hangCoopHitLevel(0)
, m_burstHitLevel(0)
, m_burstAirHitLevel(1)
, m_normalPunchLevelForShakeCam(2)
, m_switchHitBufferTime(0.2f)
, m_crushHitSuspensionTime(0.15f)
, m_crushHitMinYSpeed(4.f)
, m_crushHitYMultiplier(2.f)
, m_crushHitDownForce(1800.f)
, m_crushHitFriction(1.5f)
, m_slideDownHitMinSpeed(12.f)
, m_swimAttackNormalDuration(0.3f)
, m_swimAttackNormalSpeed(6.f)
, m_swimAttackSprintDuration(0.4f)
, m_swimAttackSprintSpeed(10.f)
, m_swimAttackCooldown(1.f)
, m_swimAttackFriction(0.5f)
, m_swimAttackMinSpeed(1.f)
, m_swimAttackMoveDamping(0.1f)
, m_swimAttackSpamTime(0.1f)
, m_swimAttackSpamImpulseMultiplier(0.7f)
, m_swimAttackSpamImpulseMultiplierSprint(0.8f)
, m_tornadoDuration(0.3f)
, m_tornadoSpeed(10.f)
, m_tornadoCooldown(1.f)
, m_tornadoMinSpeed(9.f)
, m_tornadoFriction(0.2f)
, m_tornadoAllowMoveTime(0.1f)
, m_reducedSizeHitDuration(0.4f)
, m_reducedSizeHitAirDuration(0.4f)
, m_reducedSizeHitUpDuration(0.4f)
, m_reducedSizeHitFriction(0.5f)
, m_reducedSizeHitGravity(0.2f)
, m_reducedSizeHitSideForceGround(600.f)
, m_reducedSizeHitSideForceAir(600.f)
, m_reducedSizeHitUpForce(800.f)
, m_reducedSizeHitMinSpeed(1.f)
, m_reducedSizeHitCrushAttackQueueLength(0.1f)
, m_reducedSizeHitCrushDistance(2.f)
, m_reducedSizeHitCooldown(0.2f)
, m_reducedSizeHitGroundFrictionMultiplierMin(0.5f)
, m_reducedSizeHitGroundFrictionMultiplierMax(0.1f)
, m_reducedSizeHitUpCooldown(0.3f)
, m_prepareHitLengthWeak(0.5f)
, m_prepareHitLengthStrong(0.66f)
, m_prepareHitLengthMega(0.16f)
, m_rehitDelayPermanentHit(1.f)
, m_rehitDelayComboHit(0.2f)
, m_uTurnComboTime(0.25f)
, m_avoidanceRadius(1.0f)
, m_punchPushbackRadius(1.5f)
, m_darktoonificationPosOffset(0.75f)
, m_pedestalWalkMultiplier(0.35f)
, m_pedestalAreaRadius(1.f)
, m_pedestalDistanceCheck(0.5f)
, m_pedestalFeetDistanceCheck(0.5f)
, m_pedestalDisableTimer(0.5f)
, m_pedestalInputCone(bfalse,MTH_PIBY4)
, m_onPedestalLandBounceHeight(1.f)
, m_deathNormalWait(1.5f)
, m_moveCursorMinEfficiency(0.f)
, m_moveCursorMaxEfficiency(3.f)
, m_moveCursorMinAngle(-MTH_PIBY4)
, m_moveCursorMaxAngle(MTH_PIBY4)
, m_moveCursorMinMultiplier(1.f)
, m_moveCursorMaxMultiplier(1.5f)
, m_moveCursorEfficiencyAngle(MTH_DEGTORAD*20.f)
, m_walkOnWallsDisableDelay(0.1f)
, m_walkOnWallsReleaseStickDuration(0.1f)
, m_offscreenDeathSmooth(0.1f)
, m_applyWindSpeedLimitation(btrue)
, m_clampingMaxSpeedTotal(15.f)
, m_clampingMaxSpeedX(15.f)
, m_clampingMaxSpeedY(0.f)
, m_airBrakeMaxSpeedX(10.f)
, m_softCollisionRadiusMultiplier(1.0f)
, m_softCollisionExitSpeed(5.0f)
, m_softCollisionExitForce(800.0f)
, m_softCollisionRestoreForceDelay(1.f)
, m_softCollisionRestoreForceDuration(2.f)
, m_cameraLimiterMaxCoeff(2.0f)
, m_stargateSuckinK(30.f)
, m_stargateSuckinD(10.f)
, m_stargateNoiseAmplitude(1.f)
, m_stargateTravelRotationSpeedMultiplier(1.f)
, m_stargateDecelerateTime(1.f)
, m_stargateDecelerateMultiplier(1.f)
, m_stargateNoiseTimeMultiplier(6.f)
, m_deadSoulInactiveOptOutTime(30.f)
, m_heartShield()
, m_superPunchBasicGauge()
, m_superPunchSeekingGauge()
, m_swarmRepellerPowerUp()
{
}

Ray_PlayerControllerComponent_Template::~Ray_PlayerControllerComponent_Template()
{
    SF_DEL(m_phantomShapeCrouch);
    SF_DEL(m_phantomShapeSwim);
}

} // namespace ITF
