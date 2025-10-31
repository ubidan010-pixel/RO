#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

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

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_FILESERVER_H_
#include "engine/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

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

#ifndef _ITF_LAYER_MANAGER_H_
#include "engine/scene/layerManager.h"
#endif //_ITF_LAYER_MANAGER_H_

#ifndef _ITF_WAR_GAMEMATERIAL_H_
#include "war/gameplay/War_GameMaterial.h"
#endif //_ITF_WAR_GAMEMATERIAL_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent)

BEGIN_SERIALIZATION_CHILD(War_PlayerControllerComponent)

    SERIALIZE_FUNCTION(clear,ESerialize_Template_Load);
    SERIALIZE_MEMBER("controllerHeight",m_height,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerFlyModeSpeed",m_cheatFlyModeSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerWalkSpeed",m_walkForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpMultiplierWithChains",m_jumpMultiplierWithChains,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAirSpeed",m_airForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAirTopXSpeed",m_airTopXSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerMoveTargetMultiplierMin",m_moveTargetMultiplierMin,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerMoveTargetMultiplierMax",m_moveTargetMultiplierMax,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerMoveTargetBlendTime",m_moveTargetMultiplierBlendTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpQueueTime",m_jumpQueueTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpOnAirTime",m_jumpOnAirTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpForceMinXSpeed",m_jumpForceMinXSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpForceMaxXSpeed",m_jumpForceMaxXSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpForceMinXSpeedForce",m_jumpForceMinXSpeedForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpForceMaxXSpeedForce",m_jumpForceMaxXSpeedForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpExtraForceMinYSpeed",m_jumpExtraForceMinYSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpExtraForceMaxYSpeed",m_jumpExtraForceMaxYSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpExtraForceMinYSpeedForce",m_jumpExtraForceMinYSpeedForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpExtraForceMaxYSpeedForce",m_jumpExtraForceMaxYSpeedForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpForceFromHangMultiplier",m_jumpForceFromHangMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJumpEdgeSpeedMultiplier",m_jumpEdgeSpeedMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAirSuspensionMinYSpeed",m_airSuspensionMinYSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAirSuspensionMaxYSpeed",m_airSuspensionMaxYSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAirSuspensionMinMultiplier",m_airSuspensionMinMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAirSuspensionMaxMultiplier",m_airSuspensionMaxMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAirSuspensionTime",m_airSuspensionTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerEfficiencyMinSpeed",m_efficiencyMinSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerEfficiencyMaxSpeed",m_efficiencyMaxSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerEfficiencyMaxSpeedWithChains",m_efficiencyMaxSpeedWithChains,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerEfficiencyMinSpeedValue",m_efficiencyMinSpeedValue,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerEfficiencyMaxSpeedValue",m_efficiencyMaxSpeedValue,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerFrictionModulator",m_frictionModulator,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingExtraRadius",m_hangRadiusExtra,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingOffsetSolid",m_hangingCollisionCheckOffsetSolid,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingOffsetNonSolid",m_hangingCollisionCheckOffsetNonSolid,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingRectSize",m_hangRectSize,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingCollisionExtentSolid",m_hangCollisionExtentSolid,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingCollisionExtentNonSolid",m_hangCollisionExtentNonSolid,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingRectOffset",m_hangRectOffset,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingMaxYSpeed",m_hangMaxSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerHangingActivationTime",m_hangEdgeActivationTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("airHitForceX",m_airHitForceX,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("airHitForceY",m_airHitForceY,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("airHitAntiGravfactorMin",m_airHitAntiGravfactor,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("airHitForceApplicationTime",m_airHitForceApplicationTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("airHitUnstickMinFrictionMultiplier",m_airHitUnstickMinFrictionMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("airHitMinDragSpeedX",m_airHitMinDragSpeedX,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("airHitDragForceX",m_airHitDragForceX,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("rehitTimer",m_rehitTimer,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("minBounceSpeed",m_minBounceSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("maxBounceSpeed",m_maxBounceSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("minBounceAngle",m_minBounceAngle,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("maxBounceAngle",m_maxBounceAngle,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("bounceSpeedMultiplier",m_bounceSpeedMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("bounceJumpMultiplier",m_bounceJumpMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("bounceDelay",m_bounceDelay,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("freeFallCurveRange",m_freeFallCurveRange,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbLandSpeedImpulseMultiplier",m_climbLandSpeedImpulseMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbLandSpeedImpulseTime",m_climbLandSpeedImpulseTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbLandSpeedImpulseDecreaseTime",m_climbLandSpeedImpulseDecreaseTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbVerticalSpeedUp",m_climbVerticalSpeedUp,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbVerticalDescendAcceleration",m_climbVerticalDescendAccel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbVerticalDescendMaxSpeed",m_climbVerticalDescendMaxSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbVerticalMoveAnimRate",m_climbVerticalUpAnimRate,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbVerticalJumpImpulse",m_climbVerticalJumpImpulse,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbHorizontalSpeed",m_climbHorizontalSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbHorizontalMoveAnimRate",m_climbHorizontalMoveAnimRate,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgeInertiaBrake",m_climbEdgeInertiaBrake,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgeDistanceRange",m_climbEdgeDistanceRange,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgeActivationTime",m_climbEdgeActivationTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgeSwingImpulse",m_climbEdgeSwingImpulse,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgePunchImpulse",m_climbEdgePunchImpulse,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgeMoveImpulse",m_climbEdgeMoveImpulse,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgeAngularSpeed",m_climbEdgeAngularSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbEdgeJumpMultiplier",m_climbEdgeJumpMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerClimbTimerThreshold",m_climbTimerThreshold,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerPunchShapeOffset",m_punchShapeOffset,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerReceiveHitForce",m_receiveHitForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerReceiveLowHitEjectForce",m_receiveLowHitEjectForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerReceiveLowHitEjectAnimSpeed",m_receiveLowHitEjectAnimSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerReceiveHitGravityMultiplier",m_receiveHitGravityMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerReceiveHitFrictionMultiplier",m_receiveHitFrictionMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerSlideFrictionMultiplier",m_slideFrictionMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerSlideMinFastSpeed",m_slideMinFastSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerSlideHitLevel",m_slideHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerSlideMinSpeed",m_minSpeedForSlide,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerMaxAccrobaticPunchCombo",m_numAccrobaticPunchCombo,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerBurstPunchInputFrequency",m_burstPunchInputFrequency,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAccrobaticPunchInputFrequency",m_accrobaticPunchInputFrequency,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerMinSpeedForAccrobaticCombo",m_minSpeedForAccrobaticCombo,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerUTurnHitLevel",m_uTurnHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerCrouchedHitLevel",m_crouchHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerTornadoHitLevel",m_tornadoHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerAccrobaticTornadoHitLevel",m_accrobaticTornadoHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerJCVDHitLevel",m_jcvdHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerEarthquakeHitLevel",m_earthQuakeHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerUTurnJumpHitLevel",m_uTurnJumpKickHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("controllerSlideDownHitLevel",m_slideDownHitLevel,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("allowUTurnHitTime",m_uTurnHitAllowTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("earthquakeSuspensionTime",m_earthquakeSuspensionTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("earthquakeMinYSpeed",m_earthquakeMinYSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("earthquakeYSpeedMultiplier",m_earthquakeYMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("earthquakeDownForce",m_earthquakeDownForce,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("slideDownMinSpeed",m_slideDownHitMinSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("tornadoMinSpeed",m_tornadoMinSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("tornadoFrictionMultiplier",m_tornadoFriction,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("hitFxBoneName",m_hitFxBoneName,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("disableCrouchTime",m_disableCrouchTime,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("rollingJumpMultiplier",m_rollingJumpMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("rollingFrictionMultiplier",m_rollingFrictionMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("crouchJumpMultiplier",m_crouchJumpMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("fallOnVictimMinSpeed",m_fallOnVictimMinSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("fallOnVictimJumpMultiplier",m_fallOnVictimJumpMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintWalkForceMultiplier",m_sprintForceMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintEfficiencyMaxSpeedMultiplier",m_sprintEfficiencyMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintStickRangeMinAngleMinSpeed",m_sprintStickRangeMinAngleMinSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintStickRangeMaxAngleMinSpeed",m_sprintStickRangeMaxAngleMinSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintStickRangeMinAngleMaxSpeed",m_sprintStickRangeMinAngleMaxSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintStickRangeMaxAngleMaxSpeed",m_sprintStickRangeMaxAngleMaxSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintStickRangeMinSpeed",m_sprintStickRangeMinSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("sprintStickRangeMaxSpeed",m_sprintStickRangeMaxSpeed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("doubleJumpForceMultiplier",m_doubleJumpForceMultiplier,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerPunchLowArea",m_punchShapeLow,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerPunchMedArea",m_punchShapeMed,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerPunchHighArea",m_punchShapeHigh,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerPunchCrouchKick",m_crouchKickShape,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerPunchJCVD",m_jcvdKickShape,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerPunchUTurnKick",m_uturnKickShape,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerPunchUTurnJumpKick",m_uturnJumpKickShape,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerSlideHitArea",m_slideHitShape,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerEarthquakeHitArea",m_earthquakeHitShape,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("controllerSlideDownHitArea",m_slideDownHitShape,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT_WITH_FACTORY("phantomShapeCrouch", m_phantomShapeCrouch, PHYSWORLD->getShapesFactory(), ESerialize_Template|ESerialize_PropertyEdit);
    SERIALIZE_MEMBER("displayHangDebug",m_displayHangDebug,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("drawAnimInputs",m_drawAnimInputs,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_FUNCTION(postLoadProperties, ESerialize_PropertyEdit_Load);
    SERIALIZE_MEMBER("hasChains",m_hasChains,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_MEMBER("hasSoupiere",m_hasSoupiere,ESerialize_PropertyEdit|ESerialize_Template);

END_SERIALIZATION()

void War_PlayerControllerComponent::postLoadProperties()
{
    m_hangingCollisionShapeSolid.setExtent(m_hangCollisionExtentSolid);
    m_hangingCollisionShapeNonSolid.setExtent(m_hangCollisionExtentNonSolid);
}

const f32 War_PlayerControllerComponent::s_efficiencyCalculationDelay = 0.5f;

War_PlayerControllerComponent::War_PlayerControllerComponent()
: Super()
, m_hangingPolyline(ITF_INVALID_OBJREF)
, m_hangingEdge(U32_INVALID)
, m_hangingSolid(bfalse)
, m_climbingPolyline(ITF_INVALID_OBJREF)
, m_climbingEdge(U32_INVALID)
, m_climbingT(0.f)
, m_climbingVerticalAnim(bfalse)
, m_phantomShapeCrouch(NULL)
, m_height(1.8f)
, m_cheatFlyModeSpeed(15.f)
, m_walkForce(6.f)
, m_jumpMultiplierWithChains(0.5f)
, m_airForce(40.f)
, m_airTopXSpeed(5.f)
, m_climbLandSpeedImpulseMultiplier(10.f)
, m_climbLandSpeedImpulseTime(1.f)
, m_climbLandSpeedImpulseDecreaseTime(0.8f)
, m_climbLandSpeedImpulseTimer(0.f)
, m_climbLandSpeedImpulseValue(Vec2d::Zero)
, m_climbVerticalSpeedUp(2.f)
, m_climbVerticalDescendAccel(20.f)
, m_climbVerticalDescendMaxSpeed(20.f)
, m_climbVerticalUpAnimRate(2.f)
, m_climbVerticalJumpImpulse(0.f)
, m_climbHorizontalSpeed(4.f)
, m_climbHorizontalMoveAnimRate(5.f)
, m_climbEdgeInertiaBrake(40.f)
, m_climbEdgeDistanceRange(0.5f)
, m_climbEdgeActivationTime(0.5f)
, m_hangEdgeActivationTime(0.25f)
, m_climbEdgeActivationCounter(0.f)
, m_climbEdgeSwingImpulse(10.f)
, m_climbEdgePunchImpulse(200.f)
, m_climbEdgeMoveImpulse(500.f)
, m_climbEdgeAngularSpeed(btrue,359.f)
, m_climbEdgeJumpMultiplier(1.2f)
, m_climbTimerThreshold(0.3f)
, m_punchShapeOffset(0.2f,0.2f)
, m_playWorldSpeed(Vec2d::Zero)
, m_prevPos(Vec2d::Zero)
, m_disableCrouchTimer(0.f)
, m_disableCrouchTime(1.f)
, m_rollingJumpMultiplier(1.3f)
, m_rollingFrictionMultiplier(0.1f)
, m_rollingPrevFrictionMultiplier(1.f)
, m_crouchJumpMultiplier(1.3f)
, m_fallOnVictimMinSpeed(8.f)
, m_fallOnVictimJumpMultiplier(1.5f)
, m_sprintForceMultiplier(1.f)
, m_sprintEfficiencyMultiplier(2.f)
, m_sprintStickRangeMinAngleMinSpeed(btrue,190.f)
, m_sprintStickRangeMaxAngleMinSpeed(btrue,345.f)
, m_sprintStickRangeMinAngleMaxSpeed(btrue,160.f)
, m_sprintStickRangeMaxAngleMaxSpeed(btrue,20.f)
, m_sprintStickRangeMinSpeed(8.f)
, m_sprintStickRangeMaxSpeed(12.f)
, m_sprintStickRangeIndex(U32_INVALID)
, m_doubleJumpForceMultiplier(1.2f)
, m_moveTargetMultiplierMin(8.f)
, m_moveTargetMultiplierMax(11.f)
, m_moveTargetMultiplierBlendTime(0.35f)
, m_jumpQueueTime(0.2f)
, m_jumpOnAirTime(0.2f)
, m_moveTargetBlendTimer(0.f)
, m_currentMoveTargetMultiplier(1.f)
, m_jumpQueueTimer(0.f)
, m_jumpOnAirTimer(0.f)
, m_jumpForceMinXSpeed(8.f)
, m_jumpForceMaxXSpeed(13.f)
, m_jumpForceMinXSpeedForce(600.f)
, m_jumpForceMaxXSpeedForce(600.f)
, m_jumpExtraForceMinYSpeed(5.f)
, m_jumpExtraForceMaxYSpeed(15.f)
, m_jumpExtraForceMinYSpeedForce(1.1f)
, m_jumpExtraForceMaxYSpeedForce(1.6f)
, m_jumpForceFromHangMultiplier(1.2f)
, m_jumpForceMultiplier(1.0f)
, m_jumpEdgeSpeedMultiplier(50.f)
, m_airSuspensionMinYSpeed(0.f)
, m_airSuspensionMaxYSpeed(8.f)
, m_airSuspensionMinMultiplier(0.25f)
, m_airSuspensionMaxMultiplier(0.5f)
, m_airSuspensionTime(2.f)
, m_airSuspensionCounter(0.f)
, m_efficiencyMinSpeed(0.f)
, m_efficiencyMaxSpeed(10.f)
, m_efficiencyMaxSpeedWithChains(5.f)
, m_efficiencyMinSpeedValue(1.f)
, m_efficiencyMaxSpeedValue(0.f)
, m_currentEfficiency(1.f)
, m_efficiencyCalculationTimer(s_efficiencyCalculationDelay)
, m_targetMoveEfficiency(1.f)
, m_frictionModulator(30.f)
, m_hangRadiusExtra(1.f)
, m_hangRectOffset(0.3f,1.f)
, m_hangRectSize(0.5f,1.5f)
, m_hangCollisionExtentSolid(0.2f,0.6f)
, m_hangCollisionExtentNonSolid(0.2f,0.8f)
, m_hangingCollisionCheckOffsetSolid(0.3f,0.2f)
, m_hangingCollisionCheckOffsetNonSolid(0.3f,0.9f)
, m_hangMaxSpeed(3.f)
, m_hangEdgeActivationCounter(0.f)
, m_receivedHitLevel(0)
, m_receivedHitDir(Vec2d::Right)
, m_receivedHitType(WAR_RECEIVEDHITTYPE_NORMALHIT)
, m_currentTargetForce(Vec2d::Zero)
, m_currentGravity(Vec2d::Zero)
, m_currentGravityDir(Vec2d::Zero)
, m_airHitForceX(10.0f)
, m_airHitForceY(10.0f)
, m_airHitAntiGravfactor(0.9f)
, m_airHitForceApplicationTime(0.5f)
, m_airHitUnstickMinFrictionMultiplier(0.1f)
, m_airHitMinDragSpeedX(3.0f)
, m_airHitDragForceX(20.0f)
, m_minBounceSpeed(20.0f)
, m_maxBounceSpeed(25.0f)
, m_minBounceAngle(30.0f)
, m_maxBounceAngle(80.0f)
, m_bounceSpeedMultiplier(100.0f)
, m_bounceJumpMultiplier(1.1f)
, m_bounceDelay(0.15f)
, m_bounceTimer(0.f)
, m_freeFallCurveRange(15.f)
, m_receiveHitForce(400.f)
, m_receiveLowHitEjectForce(400.f)
, m_receiveLowHitEjectAnimSpeed(3.f)
, m_receiveHitGravityMultiplier(0.8f)
, m_receiveHitFrictionMultiplier(0.7f)
, m_slideFrictionMultiplier(0.1f)
, m_slideMinFastSpeed(4.f)
, m_slideHitLevel(1)
, m_minSpeedForSlide(3.f)
, m_slidePrevFrictionMultiplier(1.f)
, m_punchType(0)
, m_numAccrobaticPunchCombo(4)
, m_burstPunchInputFrequency(0.2f)
, m_accrobaticPunchInputFrequency(0.f)
, m_currentAccrobaticStage(0)
, m_minSpeedForAccrobaticCombo(3.f)
, m_lookRight(btrue)
, m_moveSameDir(bfalse)
, m_performingSlide(bfalse)
, m_lockMoveInput(bfalse)
, m_lockLookRight(bfalse)
, m_queueJump(bfalse)
, m_queueHit(bfalse)
, m_displayHangDebug(bfalse)
, m_drawAnimInputs(bfalse)
, m_currentPunchHitType(WAR_PUNCHTYPE_NORMAL)
, m_currentPunchHitLevel(0)
, m_hurtBounceLevel(0)
, m_rehitTimer(0.2f)
, m_lastJumpTimer(0.f)
, m_stance(STANCE_STAND)
, m_prevStance(STANCE_STAND)
, m_orientationPose(ORIENTATION_HORIZONTAL)
, m_uTurnHitLevel(1)
, m_crouchHitLevel(1)
, m_tornadoHitLevel(1)
, m_accrobaticTornadoHitLevel(1)
, m_jcvdHitLevel(1)
, m_earthQuakeHitLevel(1)
, m_uTurnJumpKickHitLevel(1)
, m_slideDownHitLevel(0)
, m_uTurnHitAllowTime(0.2f)
, m_earthquakeSuspensionTime(0.15f)
, m_earthquakeSuspensionCounter(0.f)
, m_earthquakeMinYSpeed(4.f)
, m_earthquakeYMultiplier(2.f)
, m_earthquakeDownForce(1800.f)
, m_slideDownHitMinSpeed(12.f)
, m_tornadoMinSpeed(9.f)
, m_tornadoFriction(0.2f)
, m_hitFxBoneIndex(U32_INVALID)
, m_allowEarthquakePunch(bfalse)
, m_performingUTurn(bfalse)
, m_uTurnFinished(bfalse)
, m_currentMove(Vec2d::Zero)
, m_currentMoveDir(Vec2d::Zero)
, m_prevMoveDir(Vec2d::Zero)
, m_moveDirection(MOVEDIR_NONE)
, m_desiredLookDir(MOVEDIR_NONE)
, m_currentMoveGravity(Vec2d::Zero)
, m_currentMoveDirGravity(Vec2d::Zero)
, m_prevMoveGravity(Vec2d::Zero)
, m_prevMoveDirGravity(Vec2d::Zero)
, m_moveDirectionGravity(MOVEDIR_NONE)
, m_moveInput(Vec2d::Zero)
, m_actionFlags(0)
, m_helicoFxHandle(U32_INVALID)
, m_hasChains(btrue)
, m_hasSoupiere(bfalse)
, m_receivedHitInstigator(ITF_INVALID_OBJREF)
, m_previousClimbingPolyline(ITF_INVALID_OBJREF)
, m_previousHangingPolyline(ITF_INVALID_OBJREF)
, m_punchLevel(U32_INVALID)
{
    m_stateList.push_back(&m_stateIdle);
    m_stateList.push_back(&m_statePrepareHit);
    m_stateList.push_back(&m_stateReleaseHit);
    m_stateList.push_back(&m_stateJumping);
    m_stateList.push_back(&m_stateAim);
    m_stateList.push_back(&m_stateFalling);
    m_stateList.push_back(&m_stateHanging);
    m_stateList.push_back(&m_stateHangToIdle);
    m_stateList.push_back(&m_stateHangToJump);
    m_stateList.push_back(&m_stateReceiveHit);
    m_stateList.push_back(&m_stateReceiveHitLand);
    m_stateList.push_back(&m_stateReceiveHitCrash);
    m_stateList.push_back(&m_stateHurtBounce);
    m_stateList.push_back(&m_stateClimbing);
    m_stateList.push_back(&m_stateClimbingToIdle);
    m_stateList.push_back(&m_stateIdleToClimb);
    m_stateList.push_back(&m_stateCheatFlyMode);
    m_stateList.push_back(&m_statePunchAccrobatic);
    m_stateList.push_back(&m_statePunchBurst);
    m_stateList.push_back(&m_stateCrushedByCharacter);
    m_stateList.push_back(&m_stateHangToStand);


    SafeArray <Vec2d> punchShape;

    punchShape.push_back(Vec2d(0.f,0.1f));
    punchShape.push_back(Vec2d(1.2f,0.7f));
    punchShape.push_back(Vec2d(1.2f,-0.7f));
    punchShape.push_back(Vec2d(0.f,-0.1f));

    m_punchShapeLow.setPoints(punchShape);

    punchShape.clear();
    punchShape.push_back(Vec2d(0.f,0.1f));
    punchShape.push_back(Vec2d(1.5f,0.7f));
    punchShape.push_back(Vec2d(1.5f,-0.7f));
    punchShape.push_back(Vec2d(0.f,-0.1f));

    m_punchShapeMed.setPoints(punchShape);

    punchShape.clear();
    punchShape.push_back(Vec2d(0.f,0.1f));
    punchShape.push_back(Vec2d(2.0f,0.7f));
    punchShape.push_back(Vec2d(2.0f,-0.7f));
    punchShape.push_back(Vec2d(0.f,-0.1f));

    m_punchShapeHigh.setPoints(punchShape);

    punchShape.clear();

    punchShape.push_back(Vec2d(0.f,0.1f));
    punchShape.push_back(Vec2d(1.2f,0.7f));
    punchShape.push_back(Vec2d(1.2f,-0.7f));
    punchShape.push_back(Vec2d(0.f,-0.1f));

    m_crouchKickShape.setPoints(punchShape);
    m_jcvdKickShape.setPoints(punchShape);
    m_uturnKickShape.setPoints(punchShape);
    m_uturnJumpKickShape.setPoints(punchShape);
    m_slideHitShape.setPoints(punchShape);
    m_earthquakeHitShape.setPoints(punchShape);
    m_slideDownHitShape.setPoints(punchShape);
}

War_PlayerControllerComponent::~War_PlayerControllerComponent()
{
    clear();
}

void War_PlayerControllerComponent::clear()
{
    m_hitFxBoneIndex = U32_INVALID;
}

void War_PlayerControllerComponent::rotateAim(f32 angle)
{
    // HACK
    //Vec2d offset(0.f,m_fanHeight*0.5f + 0.1f);
    //offset = offset.RotateAround(Vec2d::Zero,angle);
    //m_fanForceModifiers[0].setOffset(offset);
    //m_fanForceModifiers[0].setRotation(Angle(bfalse,angle-MTH_PIBY2));
}

void War_PlayerControllerComponent::onActorLoaded()
{
    Super::onActorLoaded();

    m_prevPos = m_actor->get2DPos();
    m_playWorldSpeed = Vec2d::Zero;
    m_currentMoveTargetMultiplier = m_moveTargetMultiplierMin;

    postLoadProperties();

    Player* pPlayer = GAMEMANAGER->getPlayerFromActor(m_actor->getRef());
    if(pPlayer)
        m_playerIndex = pPlayer->getIndex();

    changeState(&m_stateIdle);

    m_actor->registerEvent(EventTeleport::GetClassNameStatic(),this);
    m_actor->registerEvent(War_HitStim::GetClassNameStatic(),this);
    m_actor->registerEvent(EventStimNotify::GetClassNameStatic(),this);
    m_actor->registerEvent(EventStickOnPolylineUpdate::GetClassNameStatic(),this);
    m_actor->registerEvent(AnimGameplayEvent::GetClassNameStatic(),this);
    m_actor->registerEvent(EventPolylineMaterialChange::GetClassNameStatic(),this);
    m_actor->registerEvent(EventInteractionQuery::GetClassNameStatic(),this);
    m_actor->registerEvent(EventCrushed::GetClassNameStatic(),this);
    m_actor->registerEvent(EventDetach::GetClassNameStatic(), this);
    m_actor->registerEvent(War_EventPickup::GetClassNameStatic(),this);
    m_actor->registerEvent(EventTrigger::GetClassNameStatic(),this);
}

void War_PlayerControllerComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    if (_deltaTime == 0.f) // do nothing on pause
        return;

    measurePlayWorldSpeed(_deltaTime);

    m_currentGravityDir = m_currentGravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
    m_currentGravityDir.normalize();

    checkSquash();

    updateStates(_deltaTime);

    //FX CONTROLLER
    updateFx(_deltaTime);

    m_climbEdgeActivationCounter = Max(m_climbEdgeActivationCounter-_deltaTime,0.f);
    m_hangEdgeActivationCounter = Max(m_hangEdgeActivationCounter-_deltaTime,0.f);
    m_airSuspensionCounter = Max(m_airSuspensionCounter-_deltaTime,0.f);
    m_jumpOnAirTimer = Max(m_jumpOnAirTimer-_deltaTime,0.f);
    m_jumpQueueTimer = Max(m_jumpQueueTimer-_deltaTime,0.f);
    m_disableCrouchTimer = Max(m_disableCrouchTimer-_deltaTime,0.f);
    m_bounceTimer = Max(m_bounceTimer-_deltaTime,0.f);
    m_lastJumpTimer += _deltaTime;
}


void War_PlayerControllerComponent::measurePlayWorldSpeed( f32 _dt )
{
    Vec2d currentPos = m_actor->get2DPos();

    if ( m_currentState != &m_stateIdleToClimb &&
         m_currentState != &m_stateHangToIdle )
    {
        Vec2d dif = currentPos - m_prevPos;
        m_playWorldSpeed = dif / _dt;
    }
    else
    {
        m_playWorldSpeed = Vec2d::Zero;
    }

    m_prevPos = currentPos;
}

void War_PlayerControllerComponent::updateStates( f32 _deltaTime )
{
    f32 axes[ JOY_MAX_AXES ];
    InputAdapter::PressStatus buttons[JOY_MAX_BUT];
    StickToPolylinePhysComponent* characterPhys = m_actor->GetComponent<StickToPolylinePhysComponent>();
    AnimatedComponent* anim = m_actor->GetComponent<AnimatedComponent>();

    INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine,m_playerIndex, buttons, JOY_MAX_BUT);
    INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentEngine,m_playerIndex, axes, JOY_MAX_AXES);

    if ( fabs(axes[m_joyTrigger_Right]) < 0.05f)
    {
        axes[m_joyTrigger_Right] = 0.0f;
    }

    if ( fabs(axes[m_joyTrigger_Left]) < 0.05f)
    {
        axes[m_joyTrigger_Left] = 0.0f;
    }

    UpdateMoveInput(_deltaTime,buttons,characterPhys,axes);

    //special case for fly mode

    if ( CHEATMANAGER->getActive() )
    {
        updateCheats(buttons,axes,characterPhys,anim);
    }

    if ( !m_queueJump )
    {
        m_queueJump = buttons[m_joyButton_A]==InputAdapter::JustPressed;
    }

    if ( !m_queueHit)
    {
        m_queueHit = buttons[m_joyButton_X]==InputAdapter::JustPressed;
    }

    updateCurrentState(_deltaTime, buttons, axes);

    m_previousSpeed = characterPhys->getSpeed();

    updateSprint(characterPhys);
}

void War_PlayerControllerComponent::updateAnimInput()
{
    if ( m_currentState && !m_pause )
    {
        static const StringID s_Speed = "Speed";                                // Absolute speed value
        static const StringID s_SpeedX = "SpeedX";                              // Speed on X (positive in the direction of the character)
        static const StringID s_SpeedY = "SpeedY";                              // Speed on Y (positive in the opposite direction of the gravity)
        static const StringID s_MoveX = "MoveX";                                // Stick Push on X (positive in the direction of the character)
        static const StringID s_MoveY = "MoveY";                                // Stick Push on Y (positive in the opposite direction of the gravity)
        static const StringID s_Stance = "Stance";                              // Standing, Hanging, Climbing mode
        static const StringID s_PrevStance = "PrevStance";                      // The previous stance
        static const StringID s_InAir = "InAir";                                // If the character is not sticked to anything
        static const StringID s_PunchLevel = "PunchLevel";                      // Level or stage of the current punch
        static const StringID s_ReceiveHitLevel = "ReceivedHitLevel";           // Vertical or Horizontal
        static const StringID s_OrientationPose = "OrientationPose";            // Angle of the character
        static const StringID s_Angle = "Angle";                                // Level of the last received hit
        static const StringID s_UTurn = "UTurn";                                // If the character is performing a UTurn
        static const StringID s_TargetMoveEfficiency = "TargetMoveEfficiency";  // How close am I to the speed I want to achieve
        static const StringID s_PunchHitType = "PunchHitType";                  // What type of punch we are performing
        static const StringID s_ReceivedHitType = "ReceivedHitType";            // The type of the received hit
        static const StringID s_HasChains = "HasChains";                        // Are Emile's feet chained?
        static const StringID s_HasSoupiere = "HasSoupiere";                    // Is Emile carrying the soupiere?

        f32 totalSpeed = 0.f;
        f32 speedX = 0.f;
        f32 speedY = 0.f;
        u32 inAir = 0;

        const Vec2d& speed = m_characterPhys->getSpeed();
        const PolyLineEdge* edge = m_characterPhys->getStickedEdge();
        Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

        if ( !m_lookRight )
        {
            dir = -dir;
        }

        totalSpeed = speed.norm();

        speedX = speed.dot(dir);
        speedY = speed.dot(-m_currentGravityDir);
        inAir = edge == NULL;

        if ( m_uTurnFinished )
        {
            resetUTurn();
        }

        m_animComponent->setInput(s_Speed,totalSpeed);
        m_animComponent->setInput(s_SpeedX,speedX);
        m_animComponent->setInput(s_SpeedY,speedY);
        m_animComponent->setInput(s_MoveX,m_moveInput.m_x);
        m_animComponent->setInput(s_MoveY,m_moveInput.m_y);
        m_animComponent->setInput(s_Stance,static_cast<u32>(m_stance));
        m_animComponent->setInput(s_PrevStance,static_cast<u32>(m_prevStance));
        m_animComponent->setInput(s_InAir,inAir);
        m_animComponent->setInput(s_PunchLevel,m_currentPunchHitLevel);
        m_animComponent->setInput(s_ReceiveHitLevel,m_receivedHitLevel);
        m_animComponent->setInput(s_OrientationPose,static_cast<u32>(m_orientationPose));
        m_animComponent->setInput(s_Angle,MTH_RADTODEG*m_actor->getAngle());
        m_animComponent->setInput(s_UTurn,static_cast<u32>(m_performingUTurn));
        m_animComponent->setInput(s_TargetMoveEfficiency,m_targetMoveEfficiency);
        m_animComponent->setInput(s_PunchHitType,static_cast<u32>(m_currentPunchHitType));
        m_animComponent->setInput(s_ReceivedHitType,static_cast<u32>(m_receivedHitType));
        m_animComponent->setInput(s_HasChains,static_cast<u32>(m_hasChains));
        m_animComponent->setInput(s_HasSoupiere,static_cast<u32>(m_hasSoupiere));

        m_animComponent->setFlipped(!m_lookRight);

        m_currentState->updateAnimInputs();
    }
}

void War_PlayerControllerComponent::updateCheats( const InputAdapter::PressStatus* _buttons,
                                                 const f32 * _axes,
                                                  StickToPolylinePhysComponent* _phys,
                                                  AnimatedComponent* _anim )
{
    if ( _buttons[m_joyButton_LB] == InputAdapter::Pressed &&
         _buttons[m_joyButton_RB] == InputAdapter::Pressed &&
         _axes[m_joyTrigger_Left] != 0.0f &&
         _axes[m_joyTrigger_Right] != 0.0f)
    {
        detachFromEverything(_phys,_anim);
    }
}

void War_PlayerControllerComponent::detachFromEverything( StickToPolylinePhysComponent* _phys,
                                                          AnimatedComponent* _anim )
{
    clearHanging(_phys,_anim);
    clearClimbing(_phys,_anim);
    changeState(&m_stateCheatFlyMode);
}

void War_PlayerControllerComponent::updateFx(f32 _deltaTime)
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

    m_fxController->setPolyLine(polyLine, polyLineEdge);
}



void War_PlayerControllerComponent::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( _event->IsClass(EventTeleport::GetClassNameStatic()) )
    {
        changeState(&m_stateFalling);
    }
    else if ( War_HitStim* hit = _event->DynamicCast<War_HitStim>(ITF_GET_STRINGID_CRC(War_HitStim,3270006373)) )
    {
        receiveHit(hit);
    }
    else if ( _event->IsClass(EventStickOnPolylineUpdate::GetClassNameStatic()) || 
              _event->IsClass(EventPolylineMaterialChange::GetClassNameStatic()))
    {
        processPolyline(_event);
    }
    else if (_event->IsClass(AnimGameplayEvent::GetClassNameStatic()))
    {
        handleAnimTrackEvent(static_cast<AnimGameplayEvent*>(_event));
    }
    else if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
    {
        // HACK
        //query->setCanGetCrushed(m_currentState != &m_stateBubble);
        query->setInteraction(CharacterInteractionType_Crush);
    }
    else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
    {
        processCrush(crushed);
    }
    else if ( EventDetach* detachEvent = _event->DynamicCast<EventDetach>(ITF_GET_STRINGID_CRC(EventDetach,889341216)) )
    {
        StickToPolylinePhysComponent* characterPhys = m_actor->GetComponent<StickToPolylinePhysComponent>();
        AnimatedComponent* animComponent = m_actor->GetComponent<AnimatedComponent>();

        detachFromEverything(characterPhys,animComponent);
    }
    else if ( War_EventPickup* evtPickup = _event->DynamicCast<War_EventPickup>(ITF_GET_STRINGID_CRC(War_EventPickup,3375386123)) )
    {
        processPickup(evtPickup);
    }
    else if ( EventTrigger* evtTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
    {
        if (evtTrigger->getActivated())
            processTrigger(evtTrigger);
    }
    else if ( EventPlayerActivationChanged* eventActivation = _event->DynamicCast<EventPlayerActivationChanged>(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208)) )
    {
        processPlayerActivationChanged(eventActivation);
    }
    else if ( EventCheckpointReached* eventCheckpoint = _event->DynamicCast<EventCheckpointReached>(ITF_GET_STRINGID_CRC(EventCheckpointReached,2346783043)) )
    {
        processCheckpointReached(eventCheckpoint);
    }
    else if ( EventRevertToLastCheckpoint* eventRevert = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)) )
    {
        processRevertToLastCheckpoint(eventRevert);
    }
    else if ( EventTeleport* teleport = _event->DynamicCast<EventTeleport>(ITF_GET_STRINGID_CRC(EventTeleport,1075308886)) )
    {
        processTeleport(teleport);
    }

    m_currentState->onEvent(_event);
}

void War_PlayerControllerComponent::processPlayerActivationChanged( EventPlayerActivationChanged* _eventActivation )
{
    Player* player = _eventActivation->getPlayer();
    bbool isSolePlayer = (GAMEMANAGER->getNumActivePlayers() == 1);

    if ( player->getIndex() == m_playerIndex )
    {
        if ( player->getActive() )
        {
            // we just joined the game

            if ( isSolePlayer )
            {
                // we are the first player to join: spawn on checkpoint

                changeState(&m_stateIdle);

                World* world = CURRENTWORLD;
                Scene* scene = world ? world->getRootScene() : NULL;
                if (scene)
                {
                    Actor* checkpoint = GAMEMANAGER->getFirstCheckpoint();
                    if (checkpoint)
                    {
                        EventTeleport teleport(checkpoint->getPos(),m_actor->getAngle(),btrue);
                        m_actor->onEvent(&teleport);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(0, "no checkpoint found");
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(0, "no scene found");
                }
            }
            else
            {
                // someone else was already playing: start close to him

                changeState(&m_stateIdle);

                // get other player
                Actor* otherActor(NULL);
                for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    if (i != player->getIndex())
                    {
                        Player* otherPlayer = GAMEMANAGER->getPlayer(i);
                        if (otherPlayer->getActive())
                        {
                            otherActor = otherPlayer->getActor();
                            break;
                        }
                    }
                }

                if (otherActor) // checked above but... Coverity
                {
                    // move to the other player's location
                    EventTeleport teleport(otherActor->getPos(),m_actor->getAngle(),btrue);
                    m_actor->onEvent(&teleport);
                }
                else
                {
                    ITF_ASSERT_MSG(0, "other player not found");
                }
            }
        }
        else
        {
            // we just left the game

            // TODO: play bye-bye anim here?
        }
    }
    /*else
    {
        Player* myPlayer = GAMEMANAGER->getPlayer(m_playerIndex);
        if ( !myPlayer->getActive() )
            return;

        if ( !player->getActive() )
        {
            // someone else left the game

        }
        // else: someone else joined the game (and we don't give a tiny rat's ass)
    }*/
}

void War_PlayerControllerComponent::processCheckpointReached( EventCheckpointReached* _eventCheckpoint )
{
    //// extract the checkpoint
    //Actor* checkpointActor = AIUtils::getActor(_eventCheckpoint->getSender());
    //CheckpointComponent* checkpoint = checkpointActor ? checkpointActor->GetComponent<CheckpointComponent>() : NULL;
    //ITF_ASSERT(checkpoint);
    //if (!checkpoint) return;
}

void War_PlayerControllerComponent::processRevertToLastCheckpoint( EventRevertToLastCheckpoint* _eventRevert )
{
    // extract the checkpoint
    Actor* checkpointActor = AIUtils::getActor(_eventRevert->getSender());
    ITF_ASSERT(checkpointActor);
    if (!checkpointActor) return;

    // teleport to checkpoint
    EventTeleport teleport(checkpointActor->getPos(),m_actor->getAngle(),btrue);
    m_actor->onEvent(&teleport);
}

void War_PlayerControllerComponent::processTeleport( EventTeleport* _eventTeleport )
{
    m_characterPhys->setDisabled(bfalse);
    m_characterPhys->setSpeed(Vec2d::Zero);
    changeState(&m_stateFalling);
}

void War_PlayerControllerComponent::processCrush( EventCrushed* _crushed )
{
    if ( m_currentState != &m_stateCrushedByCharacter )
    {
        changeState(&m_stateCrushedByCharacter);
    }
    else
    {
        m_stateCrushedByCharacter.restart();
    }
}

void War_PlayerControllerComponent::receiveHit( const War_HitStim* _hit )
{
    if ( _hit->getSender() == m_actor->getRef() )
        return;

    if ( m_currentState != &m_stateReceiveHit || m_stateReceiveHit.canRestart(_hit->getReceivedHitType()) )
    {
        m_receivedHitInstigator = _hit->getSender();
        m_receivedHitLevel = _hit->getLevel(m_actor->get2DPos());
        m_receivedHitDir = _hit->getDirection(m_actor->get2DPos());
        m_receivedHitType = _hit->getReceivedHitType();

        if ( m_currentState == &m_stateReceiveHit )
        {
            m_stateReceiveHit.restart();
        }
        else
        {
            changeState(&m_stateReceiveHit);
        }
    }
}

void War_PlayerControllerComponent::onForceMove( )
{
    if(m_actor)
    {
        if ( m_currentState == &m_stateHanging )
        {
            StickToPolylinePhysComponent* characterPhys = m_actor->GetComponent<StickToPolylinePhysComponent>();
            AnimatedComponent* animComponent = m_actor->GetComponent<AnimatedComponent>();

            detachFromEverything(characterPhys,animComponent);
        }

        m_prevPos = m_actor->get2DPos();
    }
}

void War_PlayerControllerComponent::UpdateMoveInput( f32 _deltaTime, const InputAdapter::PressStatus* _buttons,
                                                  StickToPolylinePhysComponent* _characterPhys,
                                                  f32 * _axes )
{
    UpdateMoveDir(_characterPhys,_axes);
    UpdateMoveTargetMultiplier(_deltaTime);
    UpdateDesiredLookDir();
}


void War_PlayerControllerComponent::PerformJump( StickToPolylinePhysComponent* _phys )
{
    if (m_bounceSpeed!= Vec2d::Zero)
    {
        _phys->addForce(m_bounceSpeed * m_bounceSpeedMultiplier * m_bounceJumpMultiplier);
        m_bounceSpeed = Vec2d::Zero;
    }
    else
    {
        // Set the average speed horizontally
        Vec2d originalSpeed = _phys->getSpeed();
        f32 xWorldSpeed = 0.f;
        f32 slopePropulsion = 0.f;

        if ( m_prevStance == STANCE_HANG )
        {
            // If we were hanging we don't want any xSpeed
            _phys->setSpeed(Vec2d::Zero);
        }
        else
        {
            // In normal conditions we keep our X speed
            Vec2d gravNormal = m_currentGravityDir.getPerpendicular();
            xWorldSpeed = _phys->getWorldSpeedAverageX();

            // If we are moving at a high speed on a slope upwards
            // then we want to reduce our X speed and increase our Y force
            Vec2d worldSpeed = Vec2d(xWorldSpeed,_phys->getWorldSpeedAverageY());
            f32 gravAngle = getVec2Angle(m_currentGravityDir.getPerpendicular());

            if ( gravAngle )
            {
                worldSpeed = worldSpeed.Rotate(-gravAngle);
            }

            if ( worldSpeed.m_y >= m_jumpExtraForceMinYSpeed )
            {
                if ( worldSpeed.m_y > 0.f )
                {
                    xWorldSpeed = Clamp(xWorldSpeed, -m_jumpForceMaxXSpeed, m_jumpForceMaxXSpeed);
                }

                f32 ySpeed = Clamp(worldSpeed.m_y,m_jumpExtraForceMinYSpeed,m_jumpExtraForceMaxYSpeed);
                f32 t = (ySpeed-m_jumpExtraForceMinYSpeed)/(m_jumpExtraForceMaxYSpeed-m_jumpExtraForceMinYSpeed);
                slopePropulsion = Interpolate(m_jumpExtraForceMinYSpeedForce,m_jumpExtraForceMaxYSpeedForce,t);
            }
            
            _phys->setSpeed(gravNormal*xWorldSpeed);
        }

        // Calculate how much impulse upwards we will apply based on the average horizontal speed
        f32 ySpeed = Clamp(fabsf(xWorldSpeed),m_jumpForceMinXSpeed,m_jumpForceMaxXSpeed);
        f32 t = ( ySpeed - m_jumpForceMinXSpeed ) / ( m_jumpForceMaxXSpeed - m_jumpForceMinXSpeed );
        f32 force = Interpolate(m_jumpForceMinXSpeedForce * m_jumpForceMultiplier,m_jumpForceMaxXSpeedForce * m_jumpForceMultiplier,t);
        
        // If our world speed is way bigger than our current speed it means we are on a moving platform
        // We add an extra force to counter the movement of this platform
        static const f32 PlatformSpeedMargin = 2.f;
        f32 yWorldSpeed = (-m_currentGravityDir).dot(m_playWorldSpeed);
        bbool addPlatformForce = yWorldSpeed > (originalSpeed.m_y+PlatformSpeedMargin);

        if ( slopePropulsion && !addPlatformForce )
        {
            force *= slopePropulsion;
        }

        _phys->addForce((-m_currentGravityDir)*force);

        if ( addPlatformForce )
        {
            f32 ySpeed = yWorldSpeed - originalSpeed.m_y;

            _phys->addForce(Vec2d(0.f,ySpeed)*m_jumpEdgeSpeedMultiplier);
        }
    }

    m_jumpQueueTimer = 0.f;
    m_airSuspensionCounter = m_airSuspensionTime;
}

void War_PlayerControllerComponent::UpdatePhysicFlyMode( f32 _deltaTime )
{
    const Vec3d newPos = m_actor->getPos() + (getCurrentMove() * _deltaTime * m_cheatFlyModeSpeed).to3d();

    m_actor->setPos(newPos);
    m_actor->onForceMove();
    m_currentTargetForce = Vec2d::Zero;
}

void War_PlayerControllerComponent::UpdatePhysicClimbingEdge( f32 _dt, AnimatedComponent* _animComponent, StickToPolylinePhysComponent* _phys )
{
    const BaseObject* pObj = GETOBJECT(m_climbingPolyline);
    const PolyLine* polyLine;

    if ( pObj )
    {
        ITF_ASSERT(pObj->IsClass(PolyLine::GetClassNameStatic()));

        polyLine = static_cast<const PolyLine*>(pObj);

        if ( m_climbingEdge < polyLine->getPosCount() )
        {
            const PolyLineEdge& edge = polyLine->getEdgeAt(m_climbingEdge);
            Vec2d climbPos = edge.getPos() + ( edge.m_vector *  m_climbingT );

            _animComponent->lockMagicBox(climbPos.to3d(&m_actor->getPos().m_z));

            if ( m_climbLandSpeedImpulseTimer )
            {
                m_climbLandSpeedImpulseTimer = Max(m_climbLandSpeedImpulseTimer-_dt,0.f);

                if ( m_climbLandSpeedImpulseTimer && polyLine->getOwnerActor() )
                {
                    f32 multiplier;

                    if ( m_climbLandSpeedImpulseTimer > m_climbLandSpeedImpulseDecreaseTime )
                    {
                        multiplier = m_climbLandSpeedImpulseMultiplier;
                    }
                    else
                    {
                        f32 t = m_climbLandSpeedImpulseTimer / m_climbLandSpeedImpulseDecreaseTime;
                        multiplier = m_climbLandSpeedImpulseMultiplier*t;
                    }

                    Vec2d impulse = m_climbLandSpeedImpulseValue*multiplier;

                    EventPolylineImpulse eventImpulse(polyLine->getRef(),m_climbingEdge,
                        m_climbingT,_phys->getWeight(),impulse);

                    polyLine->getOwnerActor()->onEvent(&eventImpulse);
                }
            }
        }
    }
}

void War_PlayerControllerComponent::UpdateMoveDir( StickToPolylinePhysComponent* _phys, f32* _axes )
{
    if (fabs(_axes[m_joyStickLeft_X]) < 0.6f)
        _axes[m_joyStickLeft_X] = 0.f;
    if (fabs(_axes[m_joyStickLeft_Y]) < 0.6f)
        _axes[m_joyStickLeft_Y] = 0.f;

    m_currentMove.m_x = _axes[m_joyStickLeft_X];
    m_currentMove.m_y = _axes[m_joyStickLeft_Y];

    m_currentMoveDir = m_currentMove;
    m_currentMoveDir.normalize();

    bbool isMoving = m_currentMoveDir != Vec2d::Zero;

    m_moveSameDir = isMoving &&
                    m_currentMoveDir.dot(m_prevMoveDir) > 0.98f &&
                  ( m_currentState == &m_stateIdle || m_currentState == &m_stateClimbing );

    if ( !isMoving || !m_moveSameDir )
    {
        m_prevMove = m_currentMove;
        m_prevMoveDir = m_currentMoveDir;

        if ( isMoving )
        {
            f32 moveAngle = getVec2Angle(m_currentMoveDir);

            NormalizeAngle2PI(moveAngle);

            m_moveDirection = getMoveDirectionFromAngle(moveAngle);
        }
        else
        {
            m_moveDirection = MOVEDIR_NONE;
        }
    }
    else
    {
        f32 size = m_currentMove.norm();
        m_currentMove = m_prevMoveDir*size;
        m_currentMoveDir = m_prevMoveDir;
    }

    // Now calculate the gravity-relative values
    f32 gravityAngle = getVec2Angle(m_currentGravityDir.getPerpendicular());

    m_currentMoveGravity = m_currentMove.Rotate(gravityAngle);
    m_currentMoveDirGravity = m_currentMoveGravity;
    m_currentMoveDirGravity.normalize();
    m_prevMoveGravity = m_prevMove.Rotate(gravityAngle);
    m_prevMoveDirGravity = m_prevMoveGravity;
    m_prevMoveDirGravity.normalize();

    if ( !isMoving || !m_moveSameDir )
    {
        if ( isMoving )
        {
            f32 moveAngle = getVec2Angle(m_currentMoveDirGravity);

            NormalizeAngle2PI(moveAngle);

            m_moveDirectionGravity = getMoveDirectionFromAngle(moveAngle);
        }
        else
        {
            m_moveDirectionGravity = MOVEDIR_NONE;
        }
    }
    else
    {
        m_moveDirectionGravity = MOVEDIR_NONE;
    }

    Vec2d dir = Vec2d::Right;

    if ( !m_lookRight )
    {
        dir = -dir;
    }

    Vec2d moveInput;
    
    moveInput.m_x = m_currentMoveDir.dot(dir);
    moveInput.m_y = m_currentMoveDir.m_y;

    setMoveInput(moveInput);
}

// This transforms an angle into a move direction. The angle has to be normalized around 2PI
War_PlayerControllerComponent::EMoveDir War_PlayerControllerComponent::getMoveDirectionFromAngle( f32 _angle ) const
{
    EMoveDir dir;

    if ( _angle <= (45.f*MTH_DEGTORAD) || _angle >= (315*MTH_DEGTORAD) )
    {
        dir = MOVEDIR_RIGHT;
    }
    else if ( _angle >= (45.f*MTH_DEGTORAD) && _angle <= (135.f*MTH_DEGTORAD) )
    {
        dir = MOVEDIR_UP;
    }
    else if ( _angle >= (135.f*MTH_DEGTORAD) && _angle <= (225.f*MTH_DEGTORAD) )
    {
        dir = MOVEDIR_LEFT;
    }
    else
    {
        dir = MOVEDIR_DOWN;
    }

    return dir;
}

void War_PlayerControllerComponent::UpdateMoveTargetMultiplier( f32 _deltaTime )
{
    bbool moving = getCurrentMove().m_x!=0.f && !isHanging();

    if ( moving || isSprinting() )
    {
        m_moveTargetBlendTimer = Min(m_moveTargetBlendTimer+_deltaTime,m_moveTargetMultiplierBlendTime);
    }
    else
    {
        m_moveTargetBlendTimer = 0.f;
    }

    if ( m_moveTargetMultiplierBlendTime )
    {
        m_currentMoveTargetMultiplier = Interpolate(m_moveTargetMultiplierMin,m_moveTargetMultiplierMax,m_moveTargetBlendTimer/m_moveTargetMultiplierBlendTime);
    }
    else
    {
        m_currentMoveTargetMultiplier = m_moveTargetMultiplierMax;
    }
}

// If we press left or right, memorize it so that we end up looking in that direction
void War_PlayerControllerComponent::UpdateDesiredLookDir()
{
    // While hanging we don't update the look direction because the player can turn his head
    if ( isHanging() )
    {
        return;
    }

    if ( m_moveDirection == MOVEDIR_LEFT ||
         m_moveDirection == MOVEDIR_RIGHT )
    {
        m_desiredLookDir = m_moveDirection;
    }
}

// Try to end up looking in the direction where we were trying to look
void War_PlayerControllerComponent::setDesiredLookDir()
{
    if ( m_desiredLookDir != MOVEDIR_NONE && !isPerformingUTurn() )
    {
        if ( m_desiredLookDir == MOVEDIR_RIGHT )
        {
            if ( !getIsLookingRight() )
            {
                startUTurn();
            }
        }
        else if ( m_desiredLookDir == MOVEDIR_LEFT )
        {
            if ( getIsLookingRight() )
            {
                startUTurn();
            }
        }
    }
}

void War_PlayerControllerComponent::updateLookRightFromMoveDirection( EMoveDir _moveDir )
{
    if ( _moveDir == MOVEDIR_RIGHT )
    {
        setLookRight(btrue);
    }
    else if ( _moveDir == MOVEDIR_LEFT )
    {
        setLookRight(bfalse);
    }
}

void War_PlayerControllerComponent::UpdatePhysicWalk( StickToPolylinePhysComponent* _phys, f32 _deltaTime )
{
    calculateEfficiency(_phys);

    Vec2d moveInput = m_moveInput;

    if ( m_performingSlide ||
       ( m_moveDirection != MOVEDIR_LEFT &&
         m_moveDirection != MOVEDIR_RIGHT ) )
    {
        m_targetMoveEfficiency = 1.f;
        m_currentTargetForce = Vec2d::Zero;
        m_efficiencyCalculationTimer = s_efficiencyCalculationDelay;
        moveInput.m_x = 0.f;
        setMoveInput(moveInput);
        return;
    }

    const PolyLineEdge* edge = _phys->getStickedEdge();

    if ( edge )
    {
        Vec2d worldTargetSpeed;

        f32 moveSize = getCurrentMove().norm();
        moveSize = Clamp(moveSize,0.f,1.f);
        moveInput.m_x = fabs(moveSize);

        // Check in which direction of the edge we are moving
        f32 moveSign = m_moveDirection == MOVEDIR_RIGHT ? 1.f : -1.f;

        // If we keep the stick in the same direction, move along the edge in the same direction
        if ( m_moveSameDir )
        {
            worldTargetSpeed = _phys->getCurrentGroundDir() * moveSize * moveSign * m_walkForce;
        }
        else
        {
            // Calculate the total target speed
            worldTargetSpeed = _phys->getCurrentGroundDir() * moveSize * moveSign * m_walkForce;

            bbool lookRight = moveSign >= 0.f;

            if ( !isPerformingUTurn() )
            {
                if ( m_lookRight != lookRight )
                {
                    startUTurn();
                }
            }
            else
            {
                // If we are trying to uturn while doing a uturn, then reset and continue doing it
                if ( m_lookRight == lookRight )
                {
                    resetUTurn();
                    startUTurn();
                }
            }
        }

        calculateWalkTargetMoveEfficiency(_phys,worldTargetSpeed,_deltaTime);

        // basic force
        m_currentTargetForce = worldTargetSpeed;
        // multiplied by engine force
        m_currentTargetForce *= m_currentMoveTargetMultiplier;
        // multiplied by efficiency
        m_currentTargetForce *= m_currentEfficiency;

        if ( isSprinting() )
        {
            m_currentTargetForce *= m_sprintForceMultiplier;
        }

        _phys->addForce(m_currentTargetForce);

        setMoveInput(moveInput);
    }

    if ( isPerformingUTurn() )
    {
        m_moveSameDir = bfalse;
    }
    else
    {
        updateLookRightFromMoveDirection(m_moveDirection);
    }
}

void War_PlayerControllerComponent::calculateWalkTargetMoveEfficiency( StickToPolylinePhysComponent* _phys,
                                                                   const Vec2d& _currentTarget,
                                                                   f32 _deltaTime )
{
    if ( !m_performingUTurn )
    {
        m_targetMoveEfficiency = 1.f;
        m_efficiencyCalculationTimer = s_efficiencyCalculationDelay;
        return;
    }

    Vec2d speedDir = _phys->getSpeed();

    speedDir.normalize();

    bbool sameDir;
    
    if ( _currentTarget == Vec2d::Zero )
    {
        Vec2d myDir = Vec2d::Right.Rotate(m_actor->getAngle());

        if ( !m_lookRight )
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

    f32 desiredSpeed = m_moveInput.m_x * getEfficiencyMaxSpeed() * m_currentMoveTargetMultiplier;
    f32 currentSpeed = fabs(_phys->getCurrentGroundDir().dot(_phys->getSpeed()));
    f32 speedDif;

    if ( sameDir )
    {
        speedDif = desiredSpeed - currentSpeed;
    }
    else
    {
        speedDif = currentSpeed + desiredSpeed;
    }

    if ( m_efficiencyCalculationTimer )
    {
        m_efficiencyCalculationTimer = Max(m_efficiencyCalculationTimer-_deltaTime,0.f);
    }
    else
    {
        if ( speedDif > 0.f )
        {
            // We want to go faster than what we are going. We are not very efficient
            m_targetMoveEfficiency = 1.f - (speedDif / getEfficiencyMaxSpeed());
            m_targetMoveEfficiency = Clamp(m_targetMoveEfficiency,0.f,1.f);
        }
        else
        {
            // We want to go slower than what we are going. We are over-efficient
            m_targetMoveEfficiency = 1.f + fabs(speedDif / getEfficiencyMaxSpeed());
            m_targetMoveEfficiency = Clamp(m_targetMoveEfficiency,1.f,2.f);
        }
    }
}

void War_PlayerControllerComponent::UpdatePhysicHanging( StickToPolylinePhysComponent* _phys )
{
    Vec2d pos;
    f32 angle;

    getWorldHangPosFromLocal(0.f,pos,angle);

    AnimatedComponent* animComponent = m_actor->GetComponent<AnimatedComponent>();

    animComponent->lockMagicBox(pos.to3d(&m_actor->getPos().m_z));
    m_actor->setAngle(angle);
}

void War_PlayerControllerComponent::UpdatePhysicAir( StickToPolylinePhysComponent* _phys )
{
    Vec2d worldDir = getCurrentMove();
    Vec2d speedDir = _phys->getSpeed();
    Vec2d gravXaxis = m_currentGravityDir.getPerpendicular();
    f32 moveSize = gravXaxis.dot(getCurrentMove());
    Vec2d move = gravXaxis*moveSize;
    f32 currentSpeed = Min(fabsf(gravXaxis.dot(_phys->getSpeed())),m_airTopXSpeed);

    worldDir.normalize();
    speedDir.normalize();

    // In air our force will be proportionally inverted to the top speed in air
    // The faster we go horizontally the less force we will be able to apply
    bbool sameDir = move.dot(speedDir) >= 0.f;
    f32 airForce = m_airForce;

    if ( sameDir )
    {
        m_currentTargetForce = move * (airForce*(1.f-(currentSpeed/m_airTopXSpeed)));
    }
    // If we are trying to move in the opposite direction then we apply the force normally
    else
    {
        m_currentTargetForce = move * airForce;
    }

    updateLookRightFromMoveDirection(m_moveDirection);

    _phys->addForce(m_currentTargetForce);
}

void War_PlayerControllerComponent::UpdatePhysicAirHit( StickToPolylinePhysComponent* _phys )
{
    Vec2d worldDir = getCurrentMove();
    Vec2d speedDir = _phys->getSpeed();
    Vec2d gravXaxis = m_currentGravityDir.getPerpendicular();
    f32 moveSize = gravXaxis.dot(getCurrentMove());
    Vec2d move = gravXaxis*moveSize;

    worldDir.normalize();
    speedDir.normalize();

    const Vec2d& currentSpeed = _phys->getSpeed();
    f32 speedProjectedX = gravXaxis.dot(currentSpeed);
    f32 speedProjectedY = -m_currentGravityDir.dot(currentSpeed);

    //pushing in same direction as direction of flight
    bbool sameDir = move.dot(speedDir) >= 0.f;


    // X FORCE
    m_currentTargetForce = Vec2d::Zero;
    f32 currentSpeedX = fabs(speedProjectedX);

    f32 xComponent = 0.0f;
    if (sameDir)
    {
        //ADD DRAG
        if (currentSpeedX > m_airHitMinDragSpeedX)
        {
            xComponent = (speedProjectedX>0.0f?-1.0f:1.0f) * m_airHitDragForceX * (currentSpeedX -m_airHitMinDragSpeedX);
        }
        //AIR CONTROL
        else
        {
            xComponent += moveSize * (m_airHitForceX*(1.f-(currentSpeedX/m_airHitMinDragSpeedX)));
        }
    }
    else
    {
        //ADD DRAG
        if (currentSpeedX > m_airHitMinDragSpeedX)
        {
            xComponent = (speedProjectedX>0.0f?-1.0f:1.0f) * m_airHitDragForceX * (currentSpeedX -m_airHitMinDragSpeedX);
        }
        //AIR CONTROL
        xComponent += moveSize * m_airHitForceX;
    }
    m_currentTargetForce = gravXaxis * xComponent;

    //// Y FORCE
    _phys->addForce(m_currentTargetForce);

    bbool hitGround = bfalse;
    const PhysContactsContainer& contacts = _phys->getContacts();

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

    }

    if ( !hitGround )
    {
        Vec2d force = -m_currentGravity * m_airHitAntiGravfactor;
        Vec2d yForce = -m_currentGravity;
        yForce.normalize();

        force += yForce * (m_airHitForceY * -speedProjectedY);
        _phys->addForce(force);
    }
}

void War_PlayerControllerComponent::UpdatePhysicEarthquake( f32 _dt, StickToPolylinePhysComponent* _phys )
{
    if ( m_earthquakeSuspensionCounter )
    {
        m_earthquakeSuspensionCounter = Max(m_earthquakeSuspensionCounter-_dt,0.f);

        f32 ySpeed = m_currentGravityDir.dot(_phys->getSpeed());

        if ( !m_earthquakeSuspensionCounter || ySpeed > 0.f )
        {
            // Apply extra force down
            Vec2d force = m_currentGravityDir * m_earthquakeDownForce;
            Vec2d gravityNormal = m_currentGravityDir.getPerpendicular();
            f32 xSpeed = gravityNormal.dot(_phys->getSpeed());
            Vec2d speed = gravityNormal * xSpeed;
            _phys->setSpeed(speed);
            _phys->addForce(force);
            m_earthquakeSuspensionCounter = 0.f;
        }
    }

    /*
    if ( m_earthquakeSuspensionCounter )
    {
        UpdatePhysicAirHit(_phys);
    }
    */
}

void War_PlayerControllerComponent::setHangState( struct HangingInfo& _info )
{
    if ( _info.m_climb )
    {
        m_stateClimbing.setHangInfo(_info);
        changeState(&m_stateClimbing);
    }
    else
    {
        m_stateHanging.setHangInfo(_info);
        changeState(&m_stateHanging);
    }
}

// With this we try to hang or climb a polyline while we are in the air
bbool War_PlayerControllerComponent::tryHanging( StickToPolylinePhysComponent* _phys, HangingInfo & _info ) const
{
    PhysContactsContainer contacts;
    f32 radius = m_actor->getRadius();

    const Vec2d& pos2D = m_actor->get2DPos();
    Vec2d upAxis = Vec2d::Right.Rotate(m_actor->getAngle()+MTH_PIBY2);
    Vec2d feetPos = pos2D - (upAxis*radius);
    Vec2d headPos = feetPos + (upAxis*m_height);

    if ( m_displayHangDebug )
    {
        GFX_ADAPTER->drawDBGCircle(headPos.m_x,headPos.m_y,0.2f,0.f,0.f,1.f);
        GFX_ADAPTER->drawDBGCircle(headPos.m_x,headPos.m_y,radius+m_hangRadiusExtra,1.f,1.f,0.f);
    }

    PhysShapeCircle shape(radius+m_hangRadiusExtra);

    PHYSWORLD->checkEncroachment(headPos,headPos,m_actor->getAngle(),&shape,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& c = contacts[i];

        if ( processContactHang(_phys,c,headPos,_info) )
        {
            return btrue;
        }
    }

    contacts.clear();

    // If there is no polyline to grab in the environment maybe we can grab a phantom, polylines that we don't
    // collide with (like ropes)
    PHYSWORLD->collidePhantoms(headPos,headPos,m_actor->getAngle(),&shape,m_actor->getDepth(),ECOLLISIONFILTER_ENVIRONMENT,contacts);

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& c = contacts[i];

        if ( processContactHang(_phys,c,headPos,_info) )
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool War_PlayerControllerComponent::processContactHang( StickToPolylinePhysComponent* _phys,
                                                     const SCollidableContact& _contact,
                                                     const Vec2d& _headPos,
                                                     HangingInfo& _info ) const
{
    if ( _contact.m_edgeIndex == U32_INVALID )
    {
        return bfalse;
    }

    if ( m_hangEdgeActivationCounter )
    {
        if ( m_previousHangingPolyline == _contact.m_collidableUserData )
        {
            return bfalse;
        }
    }

    BaseObject* pObj = GETOBJECT(_contact.m_collidableUserData);

    if (!pObj->IsClass(PolyLine::GetClassNameStatic()))
        return bfalse;

    PolyLine* polyLine = static_cast<PolyLine*>(pObj);

    if ( _contact.m_edgeIndex >= polyLine->getPosCount() )
    {
        return bfalse;
    }

    const PolyLineEdge& edge = polyLine->getEdgeAt(_contact.m_edgeIndex);
    const War_GameMaterial* mat = (const War_GameMaterial*)(World::getGameMaterial(edge.getGameMaterial()));

    if ( mat && mat->getCanClimb() )
    {
        return processContactClimb(_phys,_contact,polyLine,edge,_headPos,_info);
    }
    else
    {
        return processContactHangSide(_phys,_contact,polyLine,edge,_contact.m_edgeIndex,_headPos,_info);
    }
}

void War_PlayerControllerComponent::calculateHangPosCheck( const PolyLine* _polyLine,
                                                       const PolyLineEdge& _edge, u32 _edgeIndex,
                                                       bbool _isSolid, bbool _rightSide,
                                                       Vec2d& _pos, f32& _angle ) const
{
    if ( _isSolid )
    {
        if ( !_rightSide )
        {
            if ( _edgeIndex == 0 )
            {
                _angle = getVec2Angle(_edge.m_normalizedVector);
            }
            else
            {
                const PolyLineEdge& prevEdge = _polyLine->getEdgeAt(_edgeIndex-1);
                _angle = getVec2Angle(-prevEdge.m_normalizedVector.getPerpendicular());
            }
        }
        else
        {
            if ( _edgeIndex == (_polyLine->getVectorsCount()-1) )
            {
                _angle = getVec2Angle(_edge.m_normalizedVector);
            }
            else
            {
                const PolyLineEdge& nextEdge = _polyLine->getEdgeAt(_edgeIndex+1);
                _angle = getVec2Angle(nextEdge.m_normalizedVector.getPerpendicular());
            }
        }
    }
    else
    {
        _angle = getVec2Angle(m_currentGravityDir) + MTH_PIBY2;
    }

    Vec2d dir = Vec2d::Right.Rotate(_angle);

    if ( !_rightSide )
    {
        _pos = _edge.getPos();

        if ( _isSolid )
        {
            _pos -= dir * m_hangingCollisionCheckOffsetSolid.m_x;
            _pos -= dir.getPerpendicular() * m_hangingCollisionCheckOffsetSolid.m_y;
        }
        else
        {
            _pos -= dir * m_hangingCollisionCheckOffsetNonSolid.m_x;
            _pos -= dir.getPerpendicular() * m_hangingCollisionCheckOffsetNonSolid.m_y;
        }
    }
    else
    {
        _pos = _edge.getPos() + _edge.m_vector;

        if ( _isSolid )
        {
            _pos += dir * m_hangingCollisionCheckOffsetSolid.m_x;
            _pos -= dir.getPerpendicular() * m_hangingCollisionCheckOffsetSolid.m_y;
        }
        else
        {
            _pos += dir * m_hangingCollisionCheckOffsetNonSolid.m_x;
            _pos -= dir.getPerpendicular() * m_hangingCollisionCheckOffsetNonSolid.m_y;
        }
    }
}

bbool War_PlayerControllerComponent::isEdgeDirValidToHang( const PolyLineEdge& _edge, bbool _isSolid ) const
{
    if ( _isSolid )
    {
        Vec2d gravDir = m_currentGravityDir.getPerpendicular();
        Vec2d myDir = _edge.m_normalizedVector;
        f32 dotProd = Clamp(gravDir.dot(myDir),-1.f,1.f);
        f32 angle = acosf(dotProd);

        return angle < MTH_PIBY2*0.5f;
    }
    else
    {
        return m_currentGravityDir.dot(_edge.m_normalizedVector.getPerpendicular()) < 0.f;
    }
}

bbool War_PlayerControllerComponent::isCurrentHangDirValid() const
{
    BaseObject* obj = GETOBJECT(m_hangingPolyline);

    if ( obj )
    {
        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly && m_hangingEdge != U32_INVALID && m_hangingEdge < poly->getPosCount() )
        {
            const PolyLineEdge& edge = poly->getEdgeAt(m_hangingEdge);

            return isEdgeDirValidToHang(edge,m_hangingSolid);
        }
    }

    return bfalse;
}

bbool War_PlayerControllerComponent::processContactHangSide( StickToPolylinePhysComponent* _phys,
                                                         const struct SCollidableContact& _contact,
                                                         PolyLine* _polyLine, const PolyLineEdge& _edge,
                                                         u32 _edgeIndex, const Vec2d& _headPos,
                                                         HangingInfo& _info ) const
{
    const War_GameMaterial* mat = (const War_GameMaterial*)(World::getGameMaterial(_edge.getGameMaterial()));
    bbool isSolid = !mat || mat->isSolidHanging();

    // The platform must be at a certain angle with the gravity
    if ( !isEdgeDirValidToHang(_edge,isSolid) )
    {
        return bfalse;
    }

    f32 speedProj = m_currentGravityDir.dot(_phys->getSpeed());

    if ( speedProj < -m_hangMaxSpeed )
    {
        return bfalse;
    }

    // At this point we calculate the axes where we are going to hang and a axes where we should
    // check for collisions, plus set if we are hanging from the left or the right
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

    Vec2d hangPointTest;
    f32 hangAngleTest;
    
    calculateHangPosCheck(_polyLine,_edge,_edgeIndex,isSolid,rightSide,hangPointTest,hangAngleTest);

    if ( isValidPointForHanging(hangPoint,hangPointTest,hangAngleTest,_headPos,_polyLine,_contact.m_edgeIndex,rightSide,isSolid) )
    {
        _info.m_hangPos = hangPoint;
        _info.m_polyline = _polyLine;
        _info.m_edge = _contact.m_edgeIndex;
        _info.m_right = rightSide;
        _info.m_climb = bfalse;
        _info.m_solid = isSolid;
        return btrue;
    }

    return bfalse;
}

// This function checks if a specific point is valid for hanging
bbool War_PlayerControllerComponent::isValidPointForHanging( const Vec2d& _hangPoint, const Vec2d& _collisionCheckPos,
                                                         f32 _collisionCheckAngle,
                                                         const Vec2d& _headPos, const PolyLine* _poly,
                                                         u32 _edgeIndex, bbool _right, bbool _isSolid ) const
{
    // Check that the point is in direction to the head, not the feet
    const Vec2d& pos2D = m_actor->getPos().truncateTo2D();
    Vec2d dirToHead = _headPos - pos2D;
    Vec2d dirToEdge = _hangPoint - pos2D;

    dirToHead.normalize();
    dirToEdge.normalize();

    f32 dotProd = dirToHead.dot(dirToEdge);

    if ( dotProd < 0.f )
    {
        return bfalse;
    }

    // Make sure there is a certain angle with the next edge so that we don't hang in the middle of a flat ground
    if ( _right && _edgeIndex < _poly->getVectorsCount()-1 )
    {
        const PolyLineEdge& edge = _poly->getEdgeAt(_edgeIndex);
        const PolyLineEdge& nextEdge = _poly->getEdgeAt(_edgeIndex+1);
        f32 crossP = edge.m_normalizedVector.cross(nextEdge.m_normalizedVector);

        if ( crossP > 0.f )
        {
            return bfalse;
        }

        f32 dotP = edge.m_normalizedVector.dot(nextEdge.m_normalizedVector);

        if ( dotP > 0.8f )
        {
            return bfalse;
        }
    }
    else if ( !_right && _edgeIndex > 0 )
    {
        const PolyLineEdge& edge = _poly->getEdgeAt(_edgeIndex);
        const PolyLineEdge& prevEdge = _poly->getEdgeAt(_edgeIndex-1);
        f32 crossP = prevEdge.m_normalizedVector.cross(edge.m_normalizedVector);

        if ( crossP > 0.f )
        {
            return bfalse;
        }

        f32 dotP = edge.m_normalizedVector.dot(prevEdge.m_normalizedVector);

        if ( dotP > 0.8f )
        {
            return bfalse;
        }
    }

    // Now we check if the hang point is inside a rectangle because we want to have a big radius vertically
    // but a smaller radius horizontally
    Vec2d localHangPos = _hangPoint;

    localHangPos -= pos2D;
    localHangPos = localHangPos.Rotate(-m_actor->getAngle());

    Vec2d rectOffset = m_hangRectOffset;
    bbool lookRight = isPerformingUTurn() ? !m_lookRight : m_lookRight;

    if ( !lookRight )
    {
        rectOffset.m_x *= -1.f;
    }

    if ( m_displayHangDebug )
    {
        Vec2d boxPos = rectOffset.Rotate(m_actor->getAngle()) + pos2D;

        GFX_ADAPTER->drawDBGBox(boxPos,m_actor->getAngle(),m_hangRectSize,1.f,0.f,1.f,1.f);
    }

    if ( localHangPos.m_x < rectOffset.m_x - m_hangRectSize.m_x )
    {
        return bfalse;
    }

    if ( localHangPos.m_x > rectOffset.m_x + m_hangRectSize.m_x )
    {
        return bfalse;
    }

    if ( localHangPos.m_y < rectOffset.m_y - m_hangRectSize.m_y )
    {
        return bfalse;
    }

    if ( localHangPos.m_y > rectOffset.m_y + m_hangRectSize.m_y )
    {
        return bfalse;
    }

    if ( isHangingPosColliding(_collisionCheckPos,_collisionCheckAngle,_isSolid) )
    {
        return bfalse;
    }

    return btrue;
}

bbool War_PlayerControllerComponent::isCurrentHangColliding() const
{
    if ( m_hangingPolyline == ITF_INVALID_OBJREF )
    {
        return bfalse;
    }

    BaseObject* obj = GETOBJECT(m_hangingPolyline);

    if ( obj )
    {
        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly && m_hangingEdge != U32_INVALID && m_hangingEdge < poly->getPosCount() )
        {
            const PolyLineEdge& edge = poly->getEdgeAt(m_hangingEdge);
            PhysContactsContainer contacts;
            Vec2d hangingPos;
            f32 hangingAngle;

            calculateHangPosCheck(poly,edge,m_hangingEdge,m_hangingSolid,!m_lookRight,hangingPos,hangingAngle);

            if ( isHangingPosColliding(hangingPos,hangingAngle,m_hangingSolid) )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

bbool War_PlayerControllerComponent::isHangingPosColliding( const Vec2d& _collisionCheckPos,
                                                        f32 _collisionCheckAngle,
                                                        bbool _isSolid ) const
{
    // Lastly, we check that we are not colliding with anything else inside an area around _collisionCheckPos
    PhysContactsContainer contacts;
    const PhysShapeBox* shape;

    if ( _isSolid )
    {
        shape = &m_hangingCollisionShapeSolid;
    }
    else
    {
        shape = &m_hangingCollisionShapeNonSolid;
    }

    if ( m_displayHangDebug )
    {
        GFX_ADAPTER->drawDBGBox(_collisionCheckPos,_collisionCheckAngle,shape->getExtent(),1.0f,1.0f,1.f,1.f);
    }

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
            return btrue;
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::hangFromPoint( HangingInfo & _info )
{
    Vec2d speed = m_characterPhys->getSpeed();
    m_characterPhys->setDisabled(btrue);
    m_characterPhys->resetForces();
    m_characterPhys->setSpeed(Vec2d::Zero);

    if ( _info.m_polyline->getOwnerActor() )
    {
        _info.m_polyline->getOwnerActor()->getBinding()->bindChild(m_actor->getRef());
    }
    else
    {
        _info.m_polyline->getBinding()->bindChild(m_actor->getRef());
    }

    setLookRight(!_info.m_right);
    m_hangingPolyline = _info.m_polyline->getRef();
    m_hangingEdge = _info.m_edge;
    m_hangingSolid = _info.m_solid;
    m_currentTargetForce = Vec2d::Zero;

    m_animComponent->lockMagicBox(_info.m_hangPos.to3d(&m_actor->getPos().m_z));

    if ( _info.m_polyline->getOwnerActor() )
    {
        const PolyLineEdge& edge = _info.m_polyline->getEdgeAt(_info.m_edge);
        bbool left = ( _info.m_hangPos - edge.getPos() ).sqrnorm() < ( _info.m_hangPos - ( edge.getPos() + edge.m_vector ) ).sqrnorm();
        EventHanging event(m_actor->getRef(), m_hangingPolyline, m_hangingEdge, m_characterPhys->getWeight(), speed, left?0.f:1.f, btrue);
        _info.m_polyline->getOwnerActor()->onEvent(&event);
    }
}

void War_PlayerControllerComponent::clearHanging( StickToPolylinePhysComponent* _phys, AnimatedComponent* _anim )
{
    if ( m_hangingPolyline != ITF_INVALID_OBJREF )
    {
        const BaseObject* pObj = GETOBJECT(m_hangingPolyline);

        if ( pObj )
        {
            ITF_ASSERT(pObj->IsClass(PolyLine::GetClassNameStatic()));

            const PolyLine* polyLine = static_cast<const PolyLine*>(pObj);

            if ( polyLine->getOwnerActor() )
            {
                EventHanging event(m_actor->getRef(),m_hangingPolyline,m_hangingEdge,_phys->getWeight(),_phys->getSpeed(),0.f,bfalse);
                polyLine->getOwnerActor()->onEvent(&event);
            }
        }

        m_hangEdgeActivationCounter = m_hangEdgeActivationTime;
        m_previousHangingPolyline = m_hangingPolyline;
        m_hangingPolyline = ITF_INVALID_OBJREF;
        m_hangingEdge = U32_INVALID;
        m_actor->getBinding()->unbindFromParent();
        setStance(STANCE_STAND);

        _phys->setDisabled(bfalse);

        _anim->resetLockMagicBox();
    }
}

// _edgeBlend is how much we blend from the edge we come from to the surface where we would climb
bbool War_PlayerControllerComponent::getWorldHangPosFromLocal( f32 _edgeBlend, Vec2d& _ret, f32& _angle ) const
{
    const BaseObject* pObj = GETOBJECT(m_hangingPolyline);

    if ( !pObj )
    {
        return bfalse;
    }

    ITF_ASSERT(pObj->IsClass(PolyLine::GetClassNameStatic()));

    const PolyLine* polyLine = static_cast<const PolyLine*>(pObj);

    if ( m_hangingEdge >= polyLine->getPosCount() )
    {
        return bfalse;
    }

    const PolyLineEdge& edge = polyLine->getEdgeAt(m_hangingEdge);

    if ( getIsLookingRight() )
    {
        _ret = edge.getPos();
    }
    else
    {
        _ret = edge.getPos() + edge.m_vector;
    }

    f32 wallAngle;
    f32 surfaceAngle = getVec2Angle(edge.m_normalizedVector);

    if ( m_hangingSolid )
    {
        if ( getIsLookingRight() )
        {
            if ( m_hangingEdge == 0 )
            {
                wallAngle = getVec2Angle(edge.m_normalizedVector);
            }
            else
            {
                const PolyLineEdge& prevEdge = polyLine->getEdgeAt(m_hangingEdge-1);
                wallAngle = getVec2Angle(-prevEdge.m_normalizedVector.getPerpendicular());
            }
        }
        else
        {
            if ( m_hangingEdge == (polyLine->getVectorsCount()-1) )
            {
                wallAngle = getVec2Angle(edge.m_normalizedVector);
            }
            else
            {
                const PolyLineEdge& nextEdge = polyLine->getEdgeAt(m_hangingEdge+1);
                wallAngle = getVec2Angle(nextEdge.m_normalizedVector.getPerpendicular());
            }
        }
    }
    else
    {
        wallAngle = getVec2Angle(m_currentGravityDir) + MTH_PIBY2;
    }

    f32 dif = getShortestAngleDelta(wallAngle,surfaceAngle);

    _angle = wallAngle + (dif*_edgeBlend);

    return btrue;
}

bbool War_PlayerControllerComponent::checkClimbCollision( StickToPolylinePhysComponent* _phys, ObjectRef _skipPolyline ) const
{
    const PhysContactsContainer& contacts = _phys->getContacts();

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        if ( contact.m_collidableUserData == _skipPolyline )
        {
            continue;
        }

    }

    return bfalse;
}

bbool War_PlayerControllerComponent::processContactClimb( StickToPolylinePhysComponent* _phys,
                                                      const SCollidableContact& _contact,
                                                      PolyLine* _polyLine,
                                                      const PolyLineEdge& _edge, const Vec2d& _headPos,
                                                      HangingInfo& _info ) const
{
    if ( checkClimbCollision(_phys,_polyLine->getRef()) )
    {
        return bfalse;
    }

    if ( m_climbEdgeActivationCounter )
    {
        if ( m_previousClimbingPolyline == _polyLine->getRef() )
        {
            return bfalse;
        }
    }

    bbool checkSpeed = _polyLine->m_isCollisionActive;

    if ( checkSpeed )
    {
        f32 speedProj = m_currentGravityDir.dot(_phys->getSpeed());

        if ( speedProj < -m_hangMaxSpeed )
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

    if ( dist > (m_climbEdgeDistanceRange*m_climbEdgeDistanceRange) )
    {
        return bfalse;
    }

    _info.m_hangPos = closestPoint;
    _info.m_polyline = _polyLine;
    _info.m_edge = _contact.m_edgeIndex;
    _info.m_right = m_lookRight;
    _info.m_climb = btrue;
    _info.m_solid = bfalse;

    return btrue;
}

void War_PlayerControllerComponent::climbFromPoint( HangingInfo & _info )
{
    StickToPolylinePhysComponent* characterPhys = m_actor->GetComponent<StickToPolylinePhysComponent>();
    const PolyLineEdge& edge = _info.m_polyline->getEdgeAt(_info.m_edge);

    Vec2d speed = characterPhys->getSpeed();

    characterPhys->setDisabled(btrue);
    characterPhys->resetForces();
    characterPhys->setSpeed(Vec2d::Zero);

    if ( _info.m_polyline->getOwnerActor() )
    {
        _info.m_polyline->getOwnerActor()->getBinding()->bindChild(m_actor->getRef());
        m_climbLandSpeedImpulseTimer = m_climbLandSpeedImpulseTime;
        m_climbLandSpeedImpulseValue = characterPhys->getWorldSpeedAverage();
    }
    else
    {
        _info.m_polyline->getBinding()->bindChild(m_actor->getRef());
        m_climbLandSpeedImpulseTimer = 0.f;
    }

    Vec2d dirToTarget = _info.m_hangPos - edge.getPos();
    m_climbingT = edge.m_normalizedVector.dot(dirToTarget) / edge.m_length;
    m_climbingT = Clamp(m_climbingT,0.f,1.f);

    m_climbingPolyline = _info.m_polyline->getRef();
    m_climbingEdge = _info.m_edge;
    m_currentTargetForce = Vec2d::Zero;

    AnimatedComponent* animComponent = m_actor->GetComponent<AnimatedComponent>();

    Vec2d climbPos = edge.getPos() + ( edge.m_vector *  m_climbingT );

    animComponent->lockMagicBox(climbPos.to3d(&m_actor->getPos().m_z));

    f32 speedProd = edge.m_normalizedVector.dot(speed);

    m_stateClimbing.setMoveInertia(speedProd);

    if ( _info.m_polyline->getOwnerActor() )
    {
        EventHanging event(m_actor->getRef(),m_climbingPolyline,m_climbingEdge,
                           characterPhys->getWeight(),speed,m_climbingT,btrue);
        _info.m_polyline->getOwnerActor()->onEvent(&event);
    }
}

void War_PlayerControllerComponent::clearClimbing( StickToPolylinePhysComponent* _phys, class AnimatedComponent* _anim )
{
    if ( m_climbingPolyline != ITF_INVALID_OBJREF )
    {
        const BaseObject* pObj = GETOBJECT(m_climbingPolyline);

        if ( pObj )
        {
            ITF_ASSERT(pObj->IsClass(PolyLine::GetClassNameStatic()));

            const PolyLine* polyLine = static_cast<const PolyLine*>(pObj);

            if ( polyLine->getOwnerActor() )
            {
                EventHanging event(m_actor->getRef(),m_climbingPolyline,m_climbingEdge,
                                   _phys->getWeight(),_phys->getSpeed(),m_climbingT,bfalse);
                polyLine->getOwnerActor()->onEvent(&event);
            }
        }

        m_climbEdgeActivationCounter = m_climbEdgeActivationTime;
        m_previousClimbingPolyline = m_climbingPolyline;
        m_actor->getBinding()->unbindFromParent();
        setStance(STANCE_STAND);
        m_climbingPolyline = ITF_INVALID_OBJREF;
        m_climbingEdge = U32_INVALID;
        _anim->resetLockMagicBox();
        _anim->setPlayRate(1.f);
        _phys->setDisabled(bfalse);
        _phys->setSpeed(m_playWorldSpeed);
    }
}

void War_PlayerControllerComponent::getClimbEdgePolyline( class PolyLine** _polyLine,
                                                      const class PolyLineEdge** _edge ) const
{
    *_polyLine = NULL;
    *_edge = NULL;

    BaseObject* pObj = GETOBJECT(m_climbingPolyline);

    if ( pObj )
    {
        ITF_ASSERT(pObj->IsClass(PolyLine::GetClassNameStatic()));

        *_polyLine = static_cast<PolyLine*>(pObj);

        if ( m_climbingEdge < (*_polyLine)->getPosCount() )
        {
            *_edge = &(*_polyLine)->getEdgeAt(m_climbingEdge);
        }
    }
}

bbool War_PlayerControllerComponent::isVerticalClimb( const PolyLineEdge* _edge  ) const
{
    const War_GameMaterial* mat = (const War_GameMaterial*)(World::getGameMaterial(_edge->getGameMaterial()));

    return mat && mat->isClimbVertical();
}

// With this we test if we can put the character hanging from the polyline where he is standing on
bbool War_PlayerControllerComponent::tryHangFromPolyline( PolyLine* _polyLine, const PolyLineEdge* _edge, u32 _edgeIndex )
{
    const War_GameMaterial* mat = (const War_GameMaterial*)(World::getGameMaterial(_edge->getGameMaterial()));

    if ( mat && mat->getCanClimb() )
    {
        // Test to see if we are pushing down
        if ( m_moveDirection == MOVEDIR_DOWN )
        {
            m_characterPhys->getSpeed();
            m_characterPhys->setDisabled(btrue);
            m_characterPhys->resetForces();
            m_characterPhys->setSpeed(Vec2d::Zero);

            if ( _polyLine->getOwnerActor() )
            {
                _polyLine->getOwnerActor()->getBinding()->bindChild(m_actor->getRef());
            }
            else
            {
                _polyLine->getBinding()->bindChild(m_actor->getRef());
            }

            Vec2d dirToTarget = m_actor->getPos().truncateTo2D() - _edge->getPos();
            m_climbingT = _edge->m_normalizedVector.dot(dirToTarget) / _edge->m_length;
            m_climbingT = Clamp(m_climbingT,0.f,1.f);

            m_climbingPolyline = _polyLine->getRef();
            m_climbingEdge = _edgeIndex;
            m_currentTargetForce = Vec2d::Zero;

            AnimatedComponent* animComponent = m_actor->GetComponent<AnimatedComponent>();

            Vec2d climbPos = _edge->getPos() + ( _edge->m_vector *  m_climbingT );

            animComponent->lockMagicBox(climbPos.to3d(&m_actor->getPos().m_z));

            if ( _polyLine->getOwnerActor() )
            {
                EventHanging event(m_actor->getRef(), m_climbingPolyline, m_climbingEdge,
                                   m_characterPhys->getWeight(), Vec2d::Zero, m_climbingT, btrue);
                _polyLine->getOwnerActor()->onEvent(&event);
            }

            changeState(&m_stateIdleToClimb);

            return btrue;
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::calculateEfficiency( class StickToPolylinePhysComponent* _phys )
{
    f32 speed = _phys->getSpeed().norm();
    f32 maxSpeed = getEfficiencyMaxSpeed();

    if ( isSprinting() )
    {
        maxSpeed *= m_sprintEfficiencyMultiplier;
    }

    speed = Clamp(speed,m_efficiencyMinSpeed,maxSpeed);

    f32 t = ( speed - m_efficiencyMinSpeed ) / ( maxSpeed - m_efficiencyMinSpeed );

    // At less speed, best efficiency
    f32 efficiency = Interpolate(m_efficiencyMinSpeedValue,m_efficiencyMaxSpeedValue,t);

    // finally multiply it by the friction of the ground
    f32 frictionFactor = _phys->getFrictionCoef()/m_frictionModulator;
    efficiency *= Clamp(frictionFactor,0.f,1.f);

    m_currentEfficiency = efficiency;
}

void War_PlayerControllerComponent::changeState( War_State* _newState)
{
    m_prevState = m_currentState;
    m_nextState = _newState;

    if (m_currentState)
    {
        m_currentState->onExit();
    }

    m_currentState = _newState;
    _newState->onEnter();
}



void War_PlayerControllerComponent::handleAnimTrackEvent(AnimGameplayEvent* _event)
{
    if ( _event->getName() == War_MRK_UTurnIntro_Stop )
    {
        setLockMoveInput(btrue);
    }
    else if ( _event->getName() == War_MRK_UTurn_Stop )
    {
        m_uTurnFinished = btrue;
    }
}

void War_PlayerControllerComponent::processPolyline(Event * _event)
{
    if ( m_bounceSpeed != Vec2d::Zero )
    {
        // If we are about to bounce skip it
        return;
    }

    const PolyLineEdge* myEdge = NULL;
    const PolyLine * myPoly = NULL;
    Vec2d speed;
    
    if (!myPoly)
    {
        EventStickOnPolylineUpdate * event = _event->DynamicCast<EventStickOnPolylineUpdate>(ITF_GET_STRINGID_CRC(EventStickOnPolylineUpdate,2702146949));
        if (event)
        {
            BaseObject* obj = GETOBJECT(event->getPolyline());
            if ( !obj )  return;
            myPoly = static_cast<PolyLine*>(obj);
            myEdge = &myPoly->getEdgeAt(event->getEdge());
            speed = event->getSpeed();
        }
    }

    if (!myPoly)
    {
        EventPolylineMaterialChange * event = _event->DynamicCast<EventPolylineMaterialChange>(ITF_GET_STRINGID_CRC(EventPolylineMaterialChange,2134780340));
        if (event)
        {
            BaseObject* obj = GETOBJECT(event->getPolyline());
            if ( !obj )  return;
            myPoly = static_cast<PolyLine*>(obj);
            myEdge = &myPoly->getEdgeAt(event->getEdge());
            speed = Vec2d::Zero;
        }
    }

    if (myPoly && myEdge)
    {
        const PolyLineEdge * edge = myEdge;
        const War_GameMaterial* mat = (const War_GameMaterial*)(World::getGameMaterial(edge->getGameMaterial()));
        if ( mat && mat->getBounceFactor() > 0.0f )
        {
            m_bounceSpeed = Vec2d::Zero;
            m_hurtBounceLevel = mat->getDangerousLevel(); 

            Vec2d prevSpeed;

            if ( m_hurtBounceLevel )
            {
                // No delay for hurting bounce
                m_bounceTimer = 0.f;
            }
            else
            {
                m_bounceTimer = m_bounceDelay;
            }

            if ( m_previousSpeed != Vec2d::Zero )
            {
                // Use the previous speed
                prevSpeed = m_previousSpeed;
            }
            else
            {
                // If we don't have it just use the gravity
                prevSpeed = m_currentGravityDir;
            }

            if (mat->getBounceUseObjectNormal())
            {
                f32 angle = myPoly->getOwnerActor()->getAngle();
                Vec2d direction = Vec2d::XAxis.Rotate(angle);
                direction = direction.getPerpendicular();

                f32 norm = prevSpeed.norm() * mat->getBounceFactor();

                norm = Clamp(norm,m_minBounceSpeed,m_maxBounceSpeed);
                m_bounceSpeed  = direction * norm;
            }
            else
            {
                //determine direction
                bbool comingFromLeft = prevSpeed.dot(myEdge->m_normalizedVector) > 0.0f;

                bbool opposite = btrue;
                if (mat->getBounceDirection() == War_GameMaterial::BounceDirection_Opposite)
                    opposite = btrue;
                else if (mat->getBounceDirection() == War_GameMaterial::BounceDirection_Same)
                    opposite = bfalse;
                else if (mat->getBounceDirection() == War_GameMaterial::BounceDirection_Left)
                    opposite = !comingFromLeft;
                else if (mat->getBounceDirection() == War_GameMaterial::BounceDirection_Right)
                    opposite = comingFromLeft;

                if (opposite)
                {
                    Vec2d previousSpeed = prevSpeed;

                    const Vec2d normlizePreviousdSpeed = previousSpeed.normalize();

                    const Vec2d normal = edge->m_normalizedVector.getPerpendicular();

                    const Vec2d u = normal * normlizePreviousdSpeed.dot(normal);
                    const Vec2d w = edge->m_normalizedVector * normlizePreviousdSpeed.dot(edge->m_normalizedVector);

                    Vec2d newspeed = (w-u) * prevSpeed.norm() * mat->getBounceFactor();

                    f32 norm = newspeed.norm();

                    Vec2d normlizedSpeed = newspeed;
                    normlizedSpeed.normalize();

                    norm = Clamp(norm,m_minBounceSpeed,m_maxBounceSpeed);
                    newspeed = normlizedSpeed * norm;

                    const f32 sign = newspeed.dot(normal.getPerpendicular()) >= 0?1.0f:-1.0f;

                    const Vec2d limitAngleMin = normal.Rotate(sign * (MTH_PIBY2-m_minBounceAngle));
                    const Vec2d limitAngleMax = normal.Rotate(sign * (MTH_PIBY2-m_maxBounceAngle));
                    const f32 dotLimitAngleMin = limitAngleMin.dot(normal.getPerpendicular());
                    const f32 dotLimitAngleMax = limitAngleMax.dot(normal.getPerpendicular());
                    const f32 dotSpeed = normlizedSpeed.dot(normal.getPerpendicular());

                    if (fabsf(dotSpeed) > fabsf(dotLimitAngleMin))
                    {
                        newspeed = limitAngleMin * newspeed.norm();
                    }
                    else if (fabsf(dotSpeed) < fabsf(dotLimitAngleMax))
                    {
                        newspeed = limitAngleMax * newspeed.norm();
                    }
                    m_bounceSpeed  = newspeed;
                }
                else
                {
                    Vec2d newspeed = -prevSpeed * mat->getBounceFactor();

                    f32 norm = newspeed.norm();

                    Vec2d normlizedSpeed = newspeed;
                    normlizedSpeed.normalize();

                    norm = Clamp(norm,m_minBounceSpeed,m_maxBounceSpeed);
                    newspeed = normlizedSpeed * norm;

                    Vec2d normlizePreviousdSpeed = prevSpeed;
                    normlizePreviousdSpeed.normalize();

                    const Vec2d normal = edge->m_normalizedVector.getPerpendicular();

                    const f32 sign = newspeed.dot(normal.getPerpendicular()) >= 0?1.0f:-1.0f;

                    const Vec2d limitAngleMin = normal.Rotate(sign * (MTH_PIBY2-m_minBounceAngle));
                    const Vec2d limitAngleMax = normal.Rotate(sign * (MTH_PIBY2-m_maxBounceAngle));
                    const f32 dotLimitAngleMin = limitAngleMin.dot(normal.getPerpendicular());
                    const f32 dotLimitAngleMax = limitAngleMax.dot(normal.getPerpendicular());
                    const f32 dotSpeed = normlizedSpeed.dot(normal.getPerpendicular());

                    if (fabsf(dotSpeed) > fabsf(dotLimitAngleMin))
                    {
                        newspeed = limitAngleMin * newspeed.norm();
                    }
                    else if (fabsf(dotSpeed) < fabsf(dotLimitAngleMax))
                    {
                        newspeed = limitAngleMax * newspeed.norm();
                    }
                    m_bounceSpeed  = newspeed;
                }
            }
        }
        else
        {
            m_hurtBounceLevel = 0;
        }
    }  
}


void War_PlayerControllerComponent::setCurrentPunch( u32 _type, u32 _level )
{
    m_currentPunchHitType = _type;
    m_currentPunchHitLevel = _level;
}

PhysShapePolygon* War_PlayerControllerComponent::getCurrentPunchShape()
{
    switch(m_currentPunchHitType)
    {
        case WAR_PUNCHTYPE_NORMAL:
            {
                u32 level = Clamp(m_currentPunchHitLevel,static_cast<u32>(0),static_cast<u32>(2));

                switch(level)
                {
                case 0:
                    return &m_punchShapeLow;
                case 1:
                    return &m_punchShapeMed;
                case 2:
                    return &m_punchShapeHigh;
                default:
                    return NULL;
                }
            }
            break;
        case WAR_PUNCHTYPE_CROUCHKICK:
            return &m_crouchKickShape;
        case WAR_PUNCHTYPE_JCVD:
            return &m_jcvdKickShape;
        case WAR_PUNCHTYPE_ACCROBATIC_TORNADO:
        case WAR_PUNCHTYPE_UTURNKICK:
            return &m_uturnKickShape;
        case WAR_PUNCHTYPE_UTURNJUMPKICK:
            return &m_uturnJumpKickShape;
        case WAR_PUNCHTYPE_EARTHQUAKE:
            return &m_earthquakeHitShape;
        default:
            return NULL;
    }
}

void War_PlayerControllerComponent::resetStance(EStance _stance )
{
    m_prevStance = m_stance = _stance;
    if (m_helicoFxHandle != U32_INVALID)
    {
        m_fxController->stopFX(m_helicoFxHandle);
        m_helicoFxHandle = U32_INVALID;
    }
}

void War_PlayerControllerComponent::setStance( EStance _stance )
{
    if ( m_stance != _stance )
    {
        m_prevStance = m_stance;
        m_stance = _stance;

        // If we are not on a stand stance anymore we reset the crouch status
        if ( m_stance != STANCE_STAND )
        {
            if ( isCrouching() )
            {
                setCrouching(bfalse);
            }

            if ( isRolling() )
            {
                setRolling(bfalse);
            }

            if ( isSprinting() )
            {
                setSprinting(bfalse);
            }
        }
    }
}

void War_PlayerControllerComponent::startUTurn()
{
    if ( !m_performingUTurn )
    {
        setRolling(bfalse);
        m_performingUTurn = btrue;
        m_uTurnFinished = bfalse;
        setLockMoveInput(bfalse);
        setLockLookRight(btrue);
    }

    // HACK: disable u-turn entirely
    resetUTurn();
}

void War_PlayerControllerComponent::resetUTurn()
{
    if ( m_performingUTurn )
    {
        if ( m_moveInput.m_x != 0.f )
        {
            setCrouching(bfalse);
        }

        m_performingUTurn = bfalse;
        m_uTurnFinished = bfalse;
        setLockMoveInput(bfalse);
        setLockLookRight(bfalse);
        setLookRight(!m_lookRight);
    }
}

void War_PlayerControllerComponent::setMoveInput( const Vec2d& _moveInput )
{
    if ( !m_lockMoveInput )
    {
        m_moveInput = _moveInput;
    }
}

void War_PlayerControllerComponent::resetDesiredLookDir()
{
    if ( m_lookRight )
    {
        m_desiredLookDir = MOVEDIR_RIGHT;
    }
    else
    {
        m_desiredLookDir = MOVEDIR_LEFT;
    }
}

bbool War_PlayerControllerComponent::playerIsIdle( const InputAdapter::PressStatus* _buttons, f32 * _axes ) const
{
    return m_moveInput == Vec2d::Zero &&
           m_jumpQueueTimer == 0.f &&
           _buttons[m_joyButton_A]!=InputAdapter::JustPressed &&
           _buttons[m_joyButton_X]!=InputAdapter::JustPressed &&
           _axes[m_joyTrigger_Left] == 0.0f &&
           !m_queueHit &&
           !m_queueJump;
}

// Check if we die from being crushed by polylines
void War_PlayerControllerComponent::checkSquash()
{
    if ( m_currentState == &m_stateCheatFlyMode ||
         isHanging() )
    {
        return;
    }

    const PhysContactsContainer& contacts = m_characterPhys->getContacts();

    if ( contacts.size() < 2 )
    {
        return;
    }

    u32 collidingPolylines = 0;
    Vec2d hitNormal = Vec2d::Right;
    bbool solidPenetrating = bfalse;
    const f32 penetratingRadius = m_characterPhys->getRadius()*0.9f;

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            // If it's not a polyline we pass
            continue;
        }

        const BaseObject* pObj = GETOBJECT(contact.m_collidableUserData);

        if ( !pObj )
        {
            continue;
        }

        const PolyLine* polyLine = pObj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( !polyLine )
        {
            continue;
        }

        if ( contact.m_edgeIndex >= polyLine->getPosCount() )
        {
            continue;
        }

        const PolyLineEdge& edge = polyLine->getEdgeAt(contact.m_edgeIndex);
        Vec2d edgeNormal = edge.m_normalizedVector.getPerpendicular();

        if ( contact.m_normal.dot(edgeNormal) > 0 )
        {
            // If the normal of the edge goes in the direction of the normal then we skip it
            continue;
        }

        if ( !collidingPolylines )
        {
            // We found at least a polyline colliding with us, and/or other that is inside already
            hitNormal = contact.m_normal;
            collidingPolylines++;
        }
        else
        {
            // If we found already a collision we have to be sure that we get squashed in the opposite
            // direction of the first collision we found
            if ( hitNormal.dot(contact.m_normal) < -0.5f )
            {
                collidingPolylines++;
            }
            else
            {
                continue;
            }
        }

        // A solid polyline must be penetrating our collision circle
        if ( !solidPenetrating )
        {
            const War_GameMaterial* mat = (const War_GameMaterial*)(World::getGameMaterial(edge.getGameMaterial()));
            bbool isSolid = !mat || mat->isSolidHanging();

            if ( isSolid )
            {
                f32 distSq = ( m_actor->get2DPos() - contact.m_contactPoint ).sqrnorm();

                if ( distSq <= (penetratingRadius*penetratingRadius) )
                {
                    solidPenetrating = btrue;
                }
            }
        }
    }

    if ( collidingPolylines > 1 && solidPenetrating )
    {
        // For now we just play a receive hit
        if ( m_currentState != &m_stateReceiveHit )
        {
            m_receivedHitInstigator = ITF_INVALID_OBJREF;
            m_receivedHitLevel = 0;
            m_receivedHitDir = hitNormal;

            changeState(&m_stateReceiveHit);
        }
    }
}

void War_PlayerControllerComponent::setCrouching( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_CROUCH) != _val )
    {
        toggleActionFlag(ACTIONFLAG_CROUCH);

        if ( _val )
        {
            m_phantom->setShape(m_phantomShapeCrouch);
        }
        else
        {
            m_phantom->setShape(m_phantomShape);
        }
    }
}

void War_PlayerControllerComponent::setRolling( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_ROLLING) != _val )
    {
        toggleActionFlag(ACTIONFLAG_ROLLING);
        
        if ( _val )
        {
            m_rollingPrevFrictionMultiplier = m_characterPhys->getFrictionMultiplier();
            m_characterPhys->setFrictionMultiplier(m_rollingFrictionMultiplier);
            setCrouching(btrue);
        }
        else
        {
            m_characterPhys->setFrictionMultiplier(m_rollingPrevFrictionMultiplier);
        }
    }
}

void War_PlayerControllerComponent::setSprinting( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_SPRINTING) != _val )
    {
        toggleActionFlag(ACTIONFLAG_SPRINTING);

        if ( _val )
        {
            setCrouching(bfalse);
        }
    }
}

void War_PlayerControllerComponent::updateSprint( StickToPolylinePhysComponent* _phys )
{
    if ( m_sprintStickRangeIndex == U32_INVALID )
    {
        return;
    }

}

void War_PlayerControllerComponent::setDoubleJumping( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_DOUBLEJUMP) != _val )
    {
        toggleActionFlag(ACTIONFLAG_DOUBLEJUMP);
    }
}

void War_PlayerControllerComponent::setLowRoof( bbool _val )
{
    if ( isActionFlagSet(ACTIONFLAG_LOWROOF) != _val )
    {
        toggleActionFlag(ACTIONFLAG_LOWROOF);
    }
}

bbool War_PlayerControllerComponent::checkFallOnCharacter( StickToPolylinePhysComponent* _phys, Actor** _actor ) const
{
    //// early exit if crush was disabled in cheats menu
    //if (!GAMEMANAGER->getAllowPlayerCrush())
    //    return bfalse;


    // We don't do anything if we are not falling
    if ( m_currentGravityDir.dot(_phys->getSpeed()) < m_fallOnVictimMinSpeed )
    {
        return bfalse;
    }

    PhysContactsContainer contacts;

    *_actor = NULL;

    PHYSWORLD->collidePhantoms(m_actor->get2DPos(),m_actor->get2DPos(),
                               m_actor->getAngle(),m_phantomShape,m_actor->getDepth(),
                               ECOLLISIONFILTER_CHARACTERS,contacts);

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_collidableUserData != m_actor->getRef() )
        {
            BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

            if ( obj )
            {
                Actor* victim = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if ( victim )
                {
                    // query the victim to see if it can get crushed
                    EventInteractionQuery query;
                    victim->onEvent(&query);

                    if (query.getInteraction() == CharacterInteractionType_Crush)
                    {
                        *_actor = victim;
                        return btrue;
                    }
                }
            }
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::checkLowRoof( StickToPolylinePhysComponent* _phys )
{
    const PhysContactsContainer& contacts = _phys->getContacts();

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        Vec2d contactLocal = contact.m_contactPoint - m_actor->get2DPos();

        contactLocal = contactLocal.Rotate(-m_actor->getAngle());

        if ( contactLocal.m_y <= 0.1f )
        {
            continue;
        }

        BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

        if ( !obj )
        {
            continue;
        }

        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( !poly )
        {
            continue;
        }

        if ( contact.m_edgeIndex >= poly->getPosCount() )
        {
            continue;
        }

        const PolyLineEdge& edge = poly->getEdgeAt(contact.m_edgeIndex);

        f32 dotP = edge.m_normalizedVector.dot(m_currentGravityDir.getPerpendicular());

        if ( fabs(dotP) < 0.5f )
        {
            continue;
        }

        setLowRoof(btrue);
        return;
    }

    setLowRoof(bfalse);
}

// Prepare the state for a standing punch
void War_PlayerControllerComponent::setIdlePunch( StickToPolylinePhysComponent* _phys )
{
    setDesiredLookDir();

    if ( isPerformingUTurn() )
    {
        setCurrentPunch(WAR_PUNCHTYPE_UTURNKICK,m_uTurnHitLevel);
        changeState(&m_stateReleaseHit);
    }
    else if ( isCrouching() || isLowRoof() )
    {
        setCurrentPunch(WAR_PUNCHTYPE_CROUCHKICK,m_crouchHitLevel);
        changeState(&m_stateReleaseHit);
    }
    else
    {
        changeState(&m_statePrepareHit);
    }
}

void War_PlayerControllerComponent::updateAllowEarthquakePunch()
{
    if ( !m_allowEarthquakePunch && m_moveDirection != MOVEDIR_DOWN )
    {
        m_allowEarthquakePunch = btrue;
    }
}

Vec3d War_PlayerControllerComponent::getHitFxPos( AnimLightComponent* _anim, const Vec3d& _default )
{
    if ( m_hitFxBoneIndex == U32_INVALID )
    {
        if ( m_hitFxBoneName.isValid() )
        {
            if ( _anim->isLoaded() )
            {
                m_hitFxBoneIndex = _anim->getBoneIndex(m_hitFxBoneName);

                if ( m_hitFxBoneIndex != U32_INVALID )
                {
                    return getBonePos(_anim,m_hitFxBoneIndex,_default);
                }
            }
        }
    }
    else
    {
        return getBonePos(_anim,m_hitFxBoneIndex,_default);
    }

    return _default;
}

Vec3d War_PlayerControllerComponent::getBonePos( AnimLightComponent* _anim, u32 _index, const Vec3d& _default ) const
{
    AnimMeshScene* meshScene = _anim->getAnimMeshScene();

    if ( meshScene )
    {
        SafeArray<AnimBoneDyn> & boneList = meshScene->m_AnimInfo.getCurrentBoneList();
        if ( _index < boneList.size() )
        {
            return boneList[_index].m_Pos.to3d(&m_actor->getPos().m_z);
        }
    }

    return _default;
}

// Prepare the state for an air punch
void War_PlayerControllerComponent::setAirPunch()
{
    if ( m_moveDirection == MOVEDIR_DOWN && m_allowEarthquakePunch )
    {
        if ( isPerformingUTurn() )
        {
            resetUTurn();
        }

        setCurrentPunch(WAR_PUNCHTYPE_EARTHQUAKE,m_earthQuakeHitLevel);
        changeState(&m_stateReleaseHit);
    }
    else if ( isCrouching() &&
             !m_currentAccrobaticStage )
    {
        setCurrentPunch(WAR_PUNCHTYPE_JCVD,m_jcvdHitLevel);
        changeState(&m_stateReleaseHit);
    }
    else if ( isPerformingUTurn() )
    {
        setCurrentPunch(WAR_PUNCHTYPE_UTURNJUMPKICK,m_uTurnJumpKickHitLevel);
        changeState(&m_stateReleaseHit);
    }
    else if ( m_currentAccrobaticStage )
    {
        setCurrentPunch(WAR_PUNCHTYPE_ACCROBATIC_TORNADO,m_accrobaticTornadoHitLevel);
        changeState(&m_stateReleaseHit);
    }
    else
    {
        changeState(&m_statePrepareHit);
    }
}


void War_PlayerControllerComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    if ( m_currentState )
    {
        m_currentState->drawDebug();
    }
}



void War_PlayerControllerComponent::processPickup( War_EventPickup* _evtPickup )
{
    m_hasSoupiere = btrue;
}

void War_PlayerControllerComponent::processTrigger( EventTrigger* _evtTrigger )
{
    // get free
    m_hasChains = bfalse;
    m_hasSoupiere = bfalse;
}

} // namespace ITF
