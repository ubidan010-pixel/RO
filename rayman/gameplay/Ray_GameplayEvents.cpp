#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_


#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_EventFruitAttachQuery)
IMPLEMENT_OBJECT_RTTI(Ray_EventFruitDetach)
IMPLEMENT_OBJECT_RTTI(Ray_EventPowerUp)
IMPLEMENT_OBJECT_RTTI(Ray_EventPlayLockAnim)
IMPLEMENT_OBJECT_RTTI(Ray_EventSwingLatch)
IMPLEMENT_OBJECT_RTTI(Ray_EventSwingDetach)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryIsAccrobatic)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryIsReceivingHit)
IMPLEMENT_OBJECT_RTTI(Ray_EventHangOnCharacter)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryHangInfo)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryStanceInfo)
IMPLEMENT_OBJECT_RTTI(Ray_EventHangCoopJump)
IMPLEMENT_OBJECT_RTTI(Ray_EventSetPickedUp)
IMPLEMENT_OBJECT_RTTI(Ray_EventReuse)
IMPLEMENT_OBJECT_RTTI(Ray_EventBossSpawnActor)
IMPLEMENT_OBJECT_RTTI(Ray_EventSpawnReward)
IMPLEMENT_OBJECT_RTTI(Ray_EventSpawnRewardLum)
IMPLEMENT_OBJECT_RTTI(Ray_EventSpawnRewardTun)
IMPLEMENT_OBJECT_RTTI(Ray_EventSpawnRewardHeart)
IMPLEMENT_OBJECT_RTTI(Ray_EventLumEjection);
IMPLEMENT_OBJECT_RTTI(Ray_EventRewardPickedUp)
IMPLEMENT_OBJECT_RTTI(Ray_EventSetDeathReward)
IMPLEMENT_OBJECT_RTTI(Ray_EventMagnetTrigger)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryCanBeAttacked)
IMPLEMENT_OBJECT_RTTI(Ray_EventAttackGranted)
IMPLEMENT_OBJECT_RTTI(Ray_EventMainLumDestroyed)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryDarktoonificationPosition)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryCanDarktoonify)
IMPLEMENT_OBJECT_RTTI(Ray_EventUndarktoonify)
IMPLEMENT_OBJECT_RTTI(Ray_EventAddHealth)
IMPLEMENT_OBJECT_RTTI(Ray_EventDance)
IMPLEMENT_OBJECT_RTTI(Ray_DoorTeleport)
IMPLEMENT_OBJECT_RTTI(Ray_EventSetInitialNode)
IMPLEMENT_OBJECT_RTTI(Ray_EventSetNodeSelector)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryCameraLimiter)
IMPLEMENT_OBJECT_RTTI(Ray_EventChestUpsideDown)
IMPLEMENT_OBJECT_RTTI(Ray_EventNodeReached)
IMPLEMENT_OBJECT_RTTI(Ray_EventBossMorayNodeReached)
IMPLEMENT_OBJECT_RTTI(Ray_EventBossMorayFireMissile)
IMPLEMENT_OBJECT_RTTI(Ray_EventTriggerVictory)
IMPLEMENT_OBJECT_RTTI(Ray_EventChangePage)
IMPLEMENT_OBJECT_RTTI(Ray_EventChangePageQueryPoints)
IMPLEMENT_OBJECT_RTTI(Ray_EventSequenceFinished)
IMPLEMENT_OBJECT_RTTI(Ray_EventMoveToPoint)
IMPLEMENT_OBJECT_RTTI(Ray_EventResetMoveToPoint)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryDoorData)
IMPLEMENT_OBJECT_RTTI(Ray_EventShowUI)
IMPLEMENT_OBJECT_RTTI(Ray_EventShowElectoonMedal)
IMPLEMENT_OBJECT_RTTI(Ray_EventMedalCinematicEnded)
IMPLEMENT_OBJECT_RTTI(Ray_EventMedalNonCinematicEnded)
IMPLEMENT_OBJECT_RTTI(Ray_EventReleaseChild)
IMPLEMENT_OBJECT_RTTI(Ray_EventUturn);
IMPLEMENT_OBJECT_RTTI(Ray_EventSpawn);
IMPLEMENT_OBJECT_RTTI(Ray_EventAddLum)
IMPLEMENT_OBJECT_RTTI(Ray_EventLumReachesScore)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryHunterNode)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryReachedMoveToPoint)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryLumStage)
IMPLEMENT_OBJECT_RTTI(Ray_EventBounceToLayer)
IMPLEMENT_OBJECT_RTTI(Ray_EventTriggerBounce)
IMPLEMENT_OBJECT_RTTI(Ray_EventJumpToPos)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterVacuumed)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterSwallow)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterVacuumReleased)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterEjectActor)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterCamera_TransitionNotify)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterLandingRequested)
IMPLEMENT_OBJECT_RTTI(Ray_EventCanBeVacuum)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryHasBeenVacuumed)
IMPLEMENT_OBJECT_RTTI(Ray_EventIsInfluencedByVacuum)
IMPLEMENT_OBJECT_RTTI(Ray_EventHasBeenVacuumed)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterVacuumBhvChangeState)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterInvulnerableActor)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterSpawnerDataSet)
IMPLEMENT_OBJECT_RTTI(Ray_EventShooterBossDataSet)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryChildLaunch)
IMPLEMENT_OBJECT_RTTI(Ray_EventChildLaunch)
IMPLEMENT_OBJECT_RTTI(Ray_EventChildLaunchAll)
IMPLEMENT_OBJECT_RTTI(Ray_EventBulletLaunch)
IMPLEMENT_OBJECT_RTTI(Ray_EventSetBubblePrizeRewardNumber)
IMPLEMENT_OBJECT_RTTI(Ray_EventHunterBirdBind)
IMPLEMENT_OBJECT_RTTI(Ray_EventHunterBirdUnbind)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryAIData)
IMPLEMENT_OBJECT_RTTI(Ray_EventSuperPunchLaunch)
IMPLEMENT_OBJECT_RTTI(Ray_EventAIOrder)
IMPLEMENT_OBJECT_RTTI(Ray_EventAIOrderProcessed)
IMPLEMENT_OBJECT_RTTI(Ray_EventAIQueryNextOrder)
IMPLEMENT_OBJECT_RTTI(Ray_EventInstantKill)
IMPLEMENT_OBJECT_RTTI(Ray_EventSnapDance)
IMPLEMENT_OBJECT_RTTI(Ray_EventHandsCaught)
IMPLEMENT_OBJECT_RTTI(Ray_EventHandsRelease)
IMPLEMENT_OBJECT_RTTI(Ray_EventCaughtInPipe)
IMPLEMENT_OBJECT_RTTI(Ray_EventTeleportUsed)
IMPLEMENT_OBJECT_RTTI(Ray_EventTriggerChangePage)
IMPLEMENT_OBJECT_RTTI(Ray_EventTriggerEnterDoor)
IMPLEMENT_OBJECT_RTTI(Ray_EventTriggerStargate)
IMPLEMENT_OBJECT_RTTI(Ray_EventCanDanceOnStone)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryEmptyStone)
IMPLEMENT_OBJECT_RTTI(Ray_EventStoneActivationFinished)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryIsInPedestal)
IMPLEMENT_OBJECT_RTTI(Ray_MusicScoreOpenEvent)
IMPLEMENT_OBJECT_RTTI(Ray_MusicScoreCloseEvent)
IMPLEMENT_OBJECT_RTTI(Ray_EventRestoreDeath)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryCanJumpFromCurrent)
IMPLEMENT_OBJECT_RTTI(Ray_EventActivateStone)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryCanActivateStone)
IMPLEMENT_OBJECT_RTTI(Ray_EventStargateUse)
IMPLEMENT_OBJECT_RTTI(Ray_EventStargateStageFinished)
IMPLEMENT_OBJECT_RTTI(Ray_EventBouncePropagate)
IMPLEMENT_OBJECT_RTTI(Ray_EventSpawnWave);
IMPLEMENT_OBJECT_RTTI(Ray_EventWaveGeneratorNotification);
IMPLEMENT_OBJECT_RTTI(Ray_EventDynamicFogParamSet);
IMPLEMENT_OBJECT_RTTI(Ray_EventDynamicFogParamQuery);
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryWindTunnelInfo);
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryGeyserBehavior);
IMPLEMENT_OBJECT_RTTI(Ray_EventRescueFriendly);
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryIsRescued);
IMPLEMENT_OBJECT_RTTI(Ray_EventBreakableBreak);
IMPLEMENT_OBJECT_RTTI(Ray_EventBreakableQuery);
IMPLEMENT_OBJECT_RTTI(Ray_EventSetSpikyFlower);
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryOpen);
IMPLEMENT_OBJECT_RTTI(Ray_EventAddSoftCollForce);
IMPLEMENT_OBJECT_RTTI(Ray_EventReinitializeNPC);
IMPLEMENT_OBJECT_RTTI(Ray_EventScaleDoorTrigger)
IMPLEMENT_OBJECT_RTTI(Ray_EventScaleFunnelTrigger)
IMPLEMENT_OBJECT_RTTI(Ray_EventScaleTunnelTrigger)
IMPLEMENT_OBJECT_RTTI(Ray_EventCheckPedestalSupport)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryPedestalInfo)
IMPLEMENT_OBJECT_RTTI(Ray_EventReleasePedestal)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryPaintTrail)
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryLumCount);
IMPLEMENT_OBJECT_RTTI(Ray_EventRegisterBubblePrizeHub);
IMPLEMENT_OBJECT_RTTI(Ray_EventNotifyBubblePrizeHub);
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryHitType);
IMPLEMENT_OBJECT_RTTI(Ray_EventScoreGuageAddLum);
IMPLEMENT_OBJECT_RTTI(Ray_EventScoreSetup);
IMPLEMENT_OBJECT_RTTI(Ray_EventGoToWorldMap);
IMPLEMENT_OBJECT_RTTI(Ray_EventStoneManLaunch);
IMPLEMENT_OBJECT_RTTI(Ray_EventTakeTooth);
IMPLEMENT_OBJECT_RTTI(Ray_EventQueryDisableAirControlInfo);
IMPLEMENT_OBJECT_RTTI(Ray_EventReturnTeeth);
IMPLEMENT_OBJECT_RTTI(Ray_EventHunterBulletLaunched);
IMPLEMENT_OBJECT_RTTI(Ray_EventPlayAnimState);
IMPLEMENT_OBJECT_RTTI(Ray_EventDyingStoneMan);
IMPLEMENT_OBJECT_RTTI(Ray_EventWorldMapUnlock);
IMPLEMENT_OBJECT_RTTI(Ray_EventShowElectoonToothScore);
IMPLEMENT_OBJECT_RTTI(Ray_EventEndSequence);
IMPLEMENT_OBJECT_RTTI(Ray_EventFxScreen);
IMPLEMENT_OBJECT_RTTI(Ray_EventFetchElectoonToothScoreInfos);
IMPLEMENT_OBJECT_RTTI(Ray_EventWMOpenPaths);
IMPLEMENT_OBJECT_RTTI(Ray_MoveOnPolylineFromNautilus);
IMPLEMENT_OBJECT_RTTI(Ray_EventFetchWMOpenPaths);
IMPLEMENT_OBJECT_RTTI(Ray_EventWMDisplayWorldRecap);
IMPLEMENT_OBJECT_RTTI(Ray_EventWMShowNewNotification);
IMPLEMENT_OBJECT_RTTI(Ray_EventWMShow);
IMPLEMENT_OBJECT_RTTI(Ray_EventTriggerStartTimeAttack);

// HUD
IMPLEMENT_OBJECT_RTTI(Ray_EventHUDDisplayElem);
IMPLEMENT_OBJECT_RTTI(Ray_EventHUDSetText);


// CASA::CTR::SFS : Add For Arena   
IMPLEMENT_OBJECT_RTTI(EventHurtNotification)
IMPLEMENT_OBJECT_RTTI(EventPickRewardNotification)
// CASA>

// CASA::CTR::SFS : Add For Arena 
BEGIN_SERIALIZATION_CHILD(EventPickRewardNotification)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventHurtNotification)
END_SERIALIZATION()
// CASA>

BEGIN_SERIALIZATION_CHILD(Ray_EventFruitAttachQuery)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventFruitDetach)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventPowerUp)
    SERIALIZE_MEMBER("id", m_id);
    SERIALIZE_MEMBER("enable", m_enable);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventPlayLockAnim)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSwingLatch)
END_SERIALIZATION()

Ray_EventSwingLatch::Ray_EventSwingLatch()
: m_length(0.f)
, m_angle(MTH_PIBY2)
, m_gravMultiplier(1.f)
{
}

Ray_EventSwingLatch::Ray_EventSwingLatch( ObjectRef _sender, f32 _length, f32 _angle, f32 _gravityMultiplier )
: Super(_sender)
, m_length(_length)
, m_angle(_angle)
, m_gravMultiplier(_gravityMultiplier)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventSwingDetach)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryIsAccrobatic)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryIsReceivingHit)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHangOnCharacter)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryHangInfo)
END_SERIALIZATION()

Ray_EventQueryHangInfo::Ray_EventQueryHangInfo() 
: m_polyRef(ITF_INVALID_OBJREF)
, m_edgeIndex(U32_INVALID)
, m_t(0.0f)
, m_orientationPose(ORIENTATION_HORIZONTAL)
, m_climb(bfalse)
, m_hang(bfalse)
{
}

Ray_EventQueryHangInfo::Ray_EventQueryHangInfo( ObjectRef _poly, u32 _edge, f32 _t, EOrientationPose _orientationPose, bbool _climb, bbool _hang )
: m_polyRef(_poly)
, m_edgeIndex(_edge)
, m_t(_t)
, m_orientationPose(_orientationPose)
, m_climb(_climb)
, m_hang(_hang)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryStanceInfo)
END_SERIALIZATION()

Ray_EventQueryStanceInfo::Ray_EventQueryStanceInfo() 
: m_stance(STANCE_STAND)
, m_orientationPose(ORIENTATION_HORIZONTAL)
{
}

Ray_EventQueryStanceInfo::Ray_EventQueryStanceInfo( EStance _stance, EOrientationPose _orientationPose )
: m_stance(_stance)
, m_orientationPose(_orientationPose)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventHangCoopJump)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSetPickedUp)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventReuse)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBossSpawnActor)
    SERIALIZE_MEMBER("actorIndex",  m_actorIndex);
    SERIALIZE_MEMBER("offset",      m_offset);
    SERIALIZE_MEMBER("flipped",     m_flipped);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSpawnReward) 
    SERIALIZE_MEMBER("numRewards",m_numRewards);
    SERIALIZE_MEMBER("autoPickup",m_autoPickup);
    SERIALIZE_MEMBER("ejectionRandomForce",m_ejectionRandomForce);
    SERIALIZE_MEMBER("ejectionRandomAngle",m_ejectionRandomAngle);
    SERIALIZE_MEMBER("ejectionDuration",m_ejectionDuration);
    SERIALIZE_MEMBER("ejectionForce",m_ejectionForce);
    SERIALIZE_MEMBER("ejectionGravityAngle", m_ejectionGravityAngle);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSpawnRewardLum)
    SERIALIZE_MEMBER("forceYellow", m_forceYellow);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSpawnRewardTun)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSpawnRewardHeart)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventLumEjection)
SERIALIZE_MEMBER("ejectionForce", m_ejectionForce);
SERIALIZE_MEMBER("ejectionDuration", m_ejectionDuration);
SERIALIZE_MEMBER("isAutoPickup", m_isAutoPickup);
SERIALIZE_MEMBER("ejectionGravityAngle", m_ejectionGravityAngle);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventRewardPickedUp)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventMagnetTrigger)
SERIALIZE_MEMBER("magnetForce", m_magnetForce);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSetDeathReward)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventAddHealth)
SERIALIZE_MEMBER("health",m_health);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventDance)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_DoorTeleport)
END_SERIALIZATION()

Ray_DoorTeleport::Ray_DoorTeleport( const Vec3d& _pos, f32 _angle, f32 _entryAngle, f32 _exitAngle )
: Super(_pos,_angle,bfalse)
, m_entryAngle(_entryAngle)
, m_exitAngle(_exitAngle)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventSetInitialNode)
    SERIALIZE_MEMBER("getNodeFromSender", m_getNodeFromSender);
    SERIALIZE_MEMBER("interruptCurrentNode", m_interruptCurrentNode);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSetNodeSelector)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventChestUpsideDown)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventNodeReached)
    SERIALIZE_MEMBER("cameraOn", m_cameraOn);
    SERIALIZE_MEMBER("cameraOff", m_cameraOff);
    SERIALIZE_MEMBER("cameraZOffset", m_cameraZOffset);
    SERIALIZE_MEMBER("cameraOffset", m_cameraOffset);
    SERIALIZE_MEMBER("cameraZOffsetDuration", m_cameraZOffsetDuration);
    SERIALIZE_MEMBER("cameraOffsetDuration", m_cameraOffsetDuration);
    SERIALIZE_OBJECT("cameraEjectMargin", m_cameraEjectMargin);
    SERIALIZE_OBJECT("cameraDeathMargin", m_cameraDeathMargin);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBossMorayNodeReached)
    SERIALIZE_MEMBER("speed", m_speed);
    SERIALIZE_MEMBER("acceleration", m_acceleration);
    SERIALIZE_MEMBER("disableSpeedMultiplier", m_disableSpeedMultiplier);
    SERIALIZE_MEMBER("startDash", m_startDash);
    SERIALIZE_MEMBER("stopDash", m_stopDash);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBossMorayFireMissile)
    SERIALIZE_MEMBER("bodyPartIndex", m_bodyPartIndex);
END_SERIALIZATION()


BEGIN_SERIALIZATION_CHILD(Ray_EventTriggerVictory)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventChangePage)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventChangePageQueryPoints)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSequenceFinished)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventMoveToPoint)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventResetMoveToPoint)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryDoorData)
END_SERIALIZATION()


Ray_EventQueryDoorData::Ray_EventQueryDoorData()
: m_entry(Vec2d::Zero)
, m_exit(Vec2d::Zero)
{
}

Ray_EventQueryDoorData::Ray_EventQueryDoorData( const Vec2d& _entry, const Vec2d& _exit, bbool _noEntry )
: m_entry(_entry)
, m_exit(_exit)
, m_noEntry(_noEntry)
{
}

void Ray_EventQueryDoorData::setData( const Vec2d& _entry, const Vec2d& _exit, bbool _noEntry )
{
    m_entry = _entry;
    m_exit = _exit;
    m_noEntry = _noEntry;
}

//BEGIN_SERIALIZATION_CHILD(Ray_EventShowUI)
//END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventMedalCinematicEnded)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventMedalNonCinematicEnded)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventReleaseChild)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventUturn)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSpawn)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventAddLum)
SERIALIZE_MEMBER("isAccrobatic", m_isAccrobatic);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventLumReachesScore)
SERIALIZE_MEMBER("isAccrobatic", m_isAccrobatic);
SERIALIZE_MEMBER("valueToAdd", m_valueToAdd);
SERIALIZE_MEMBER("playerIndex", m_playerIndex);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryHunterNode)
SERIALIZE_MEMBER("flip", m_flip);
SERIALIZE_MEMBER("isHole", m_isHole);
SERIALIZE_MEMBER("numBulletsPerCycle", m_numBulletsPerCycle);
SERIALIZE_MEMBER("numCycles", m_numCycles);
SERIALIZE_MEMBER("index", m_index);
SERIALIZE_MEMBER("finishRadius", m_finishRadius);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryReachedMoveToPoint)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryLumStage)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBounceToLayer)
END_SERIALIZATION()

Ray_EventBounceToLayer::Ray_EventBounceToLayer()
: m_bounceHeight(5.f)
, m_bounceHeight2(1.f)
, m_bounceSpeed(2.f)
, m_targetActor(ITF_INVALID_OBJREF)
, m_targetOffset(Vec2d::Zero)
, m_hurt(0)
, m_skipped(bfalse)
{
}

Ray_EventBounceToLayer::Ray_EventBounceToLayer( f32 _bounceHeight, f32 _bounceHeight2, f32 _bounceSpeed, ObjectRef _targetActor, const Vec2d& _offset, u32 _hurt )
: m_bounceHeight(_bounceHeight)
, m_bounceHeight2(_bounceHeight2)
, m_bounceSpeed(_bounceSpeed)
, m_targetActor(_targetActor)
, m_targetOffset(_offset)
, m_hurt(_hurt)
, m_skipped(bfalse)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventJumpToPos)
END_SERIALIZATION()

Ray_EventJumpToPos::Ray_EventJumpToPos()
: m_pos(Vec2d::Zero)
, m_objRef(ITF_INVALID_OBJREF)
, m_polyRef(ITF_INVALID_OBJREF)
, m_edgeIndex(U32_INVALID)
, m_edgeT(0.f)
, m_stop(bfalse)
{
}

Ray_EventJumpToPos::Ray_EventJumpToPos( const Vec2d& _pos )
: m_pos(_pos)
, m_objRef(ITF_INVALID_OBJREF)
, m_polyRef(ITF_INVALID_OBJREF)
, m_edgeIndex(U32_INVALID)
, m_edgeT(0.f)
, m_stop(bfalse)
{
}

Ray_EventJumpToPos::Ray_EventJumpToPos( ObjectRef _objRef )
: m_pos(Vec2d::Zero)
, m_objRef(_objRef)
, m_polyRef(ITF_INVALID_OBJREF)
, m_edgeIndex(U32_INVALID)
, m_edgeT(0.f)
, m_stop(bfalse)
{
}

Ray_EventJumpToPos::Ray_EventJumpToPos( ObjectRef _polyRef, u32 _edgeIndex, f32 _edgeT )
: m_pos(Vec2d::Zero)
, m_objRef(ITF_INVALID_OBJREF)
, m_polyRef(_polyRef)
, m_edgeIndex(_edgeIndex)
, m_edgeT(_edgeT)
, m_stop(bfalse)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventShooterVacuumed)
END_SERIALIZATION()
BEGIN_SERIALIZATION_CHILD(Ray_EventShooterSwallow)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventShooterEjectActor)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventCanBeVacuum)
SERIALIZE_MEMBER( "canBeVaccumed", m_canBeVaccumed );
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryHasBeenVacuumed)
SERIALIZE_MEMBER( "hasBeenVacuumed", m_hasBeenVacuumed );
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventIsInfluencedByVacuum)
SERIALIZE_MEMBER( "influenced", m_influenced );
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHasBeenVacuumed)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventShooterVacuumBhvChangeState)
SERIALIZE_MEMBER( "state", m_state );
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventShooterInvulnerableActor)
SERIALIZE_MEMBER( "invulnerable", m_invulnerable );
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventShooterSpawnerDataSet)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventShooterBossDataSet)
SERIALIZE_MEMBER( "dataInt",    m_dataInt );
SERIALIZE_MEMBER( "dataFloat",  m_dataFloat );
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryChildLaunch)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventChildLaunchAll)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBulletLaunch)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSetBubblePrizeRewardNumber)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHunterBirdBind)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHunterBirdUnbind)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryAIData)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSuperPunchLaunch)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventAIOrder)
    SERIALIZE_MEMBER("targetReachedRadius", m_targetReachedRadius);
    SERIALIZE_MEMBER("playerDetectRadius", m_playerDetectRadius);
    SERIALIZE_ORDERTYPE("orderType", m_orderType);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventAIOrderProcessed)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventAIQueryNextOrder)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventInstantKill)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSnapDance)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHandsCaught)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHandsRelease)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventCaughtInPipe)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventTeleportUsed)
END_SERIALIZATION()

Ray_EventTeleportUsed::Ray_EventTeleportUsed( ObjectRef _sender, ObjectRef _teleportTarget, const Vec2d& _position )
: Super(_sender)
, m_teleportTarget(_teleportTarget)
, m_position(_position)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventTriggerChangePage)

    SERIALIZE_MEMBER("enterPoint",m_enterPoint);
    SERIALIZE_MEMBER("exitPoint",m_exitPoint);
    SERIALIZE_MEMBER("finalPoint",m_finalPoint);
    SERIALIZE_MEMBER("verticalEject",m_verticalEject);
    SERIALIZE_MEMBER("destinationPage",m_destinationPage);
    SERIALIZE_MEMBER("playerDuration",m_waitPlayerDuration);
    SERIALIZE_MEMBER("useFade",m_useFade);
    SERIALIZE_MEMBER("isCageDoor", m_isCageDoor);

END_SERIALIZATION()

Ray_EventTriggerChangePage::Ray_EventTriggerChangePage( ObjectRef _sender, const Vec3d& _enterPoint, const Vec3d& _exitPoint, const Vec3d& _finalPoint,
                                                        bbool _verticalEject, ActorRef _destinationPage, ActorRef _activateCheckpoint,
                                                        f32 _waitPlayerDuration, bbool _useFade, bbool _isCageDoor )
: Super(_sender)
, m_enterPoint(_enterPoint)
, m_exitPoint(_exitPoint)
, m_finalPoint(_finalPoint)
, m_verticalEject(_verticalEject)
, m_destinationPage(_destinationPage)
, m_activateCheckpoint(_activateCheckpoint)
, m_waitPlayerDuration(_waitPlayerDuration)
, m_useFade(_useFade)
, m_isCageDoor(_isCageDoor)
{
}

Ray_EventTriggerChangePage::Ray_EventTriggerChangePage()
: m_enterPoint(Vec3d::Zero)
, m_exitPoint(Vec3d::Zero)
, m_finalPoint(Vec3d::Zero)
, m_verticalEject(bfalse)
, m_destinationPage(ITF_INVALID_OBJREF)
, m_waitPlayerDuration(0.f)
, m_useFade(bfalse)
, m_isCageDoor(bfalse)
{

}

BEGIN_SERIALIZATION_CHILD(Ray_EventTriggerEnterDoor)

    SERIALIZE_MEMBER("destination",m_destination);

END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventTriggerStargate)

    SERIALIZE_MEMBER("start",m_start);
    SERIALIZE_MEMBER("speed",m_speed);

END_SERIALIZATION()

Ray_EventTriggerStargate::Ray_EventTriggerStargate( ObjectRef _sender, const Vec3d& _start, f32 _speed )
: Super(_sender)
, m_start(_start)
, m_speed(_speed)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventCanDanceOnStone)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryEmptyStone)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventStoneActivationFinished)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryIsInPedestal)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_MusicScoreOpenEvent)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_MusicScoreCloseEvent)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventRestoreDeath)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryCanJumpFromCurrent)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventActivateStone)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryCanActivateStone)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventStargateUse)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventStargateStageFinished)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBouncePropagate)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSpawnWave)
SERIALIZE_MEMBER("waveType", m_waveType);
SERIALIZE_MEMBER("speed", m_speed);
SERIALIZE_MEMBER("delayBeforeMoving", m_delayBeforeMoving);
SERIALIZE_MEMBER("mustOffsetByRadius", m_mustOffsetByRadius);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventDynamicFogParamSet)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryWindTunnelInfo)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryGeyserBehavior)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventRescueFriendly)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryIsRescued)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBreakableBreak)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventBreakableQuery)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHUDDisplayElem)
SERIALIZE_MEMBER("index", m_index);
SERIALIZE_MEMBER("bShow", m_bShow);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHUDSetText)
SERIALIZE_MEMBER("friendlyName", m_friendlyName);
SERIALIZE_MEMBER("text", m_text);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventSetSpikyFlower)
SERIALIZE_MEMBER("circleIndex", m_circleIndex);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryOpen)
END_SERIALIZATION()
BEGIN_SERIALIZATION_CHILD(Ray_EventAddSoftCollForce)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventReinitializeNPC)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventScaleDoorTrigger)
END_SERIALIZATION()

Ray_EventScaleDoorTrigger::Ray_EventScaleDoorTrigger( ObjectRef _sender, ActorRef _targetActor, const Vec2d& _targetPos, f32 _scale, bbool _applyAll )
: Super(_sender)
, m_targetActor(_targetActor)
, m_targetPos(_targetPos)
, m_scale(_scale)
, m_applyAll(_applyAll)
{
}

BEGIN_SERIALIZATION_CHILD(Ray_EventScaleFunnelTrigger)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventScaleTunnelTrigger)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryPaintTrail)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventCheckPedestalSupport)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryPedestalInfo)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventReleasePedestal)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventGoToWorldMap)
    SERIALIZE_MEMBER("saveGameState", m_saveGameState);
    SERIALIZE_MEMBER("markCurrentLevelAsCompleted", m_markCurrentLevelAsCompleted);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventStoneManLaunch)
    SERIALIZE_MEMBER("actionIndex", m_actionIndex);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventTakeTooth)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventQueryDisableAirControlInfo)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventReturnTeeth)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventHunterBulletLaunched)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventPlayAnimState)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventWorldMapUnlock)
    SERIALIZE_CONTAINER("unlocks",m_unlocks);
    SERIALIZE_MEMBER("changeCurrentLevelName",m_changeCurrentLevelName);
    SERIALIZE_MEMBER("levelName",m_levelName);
    SERIALIZE_MEMBER("saveGameState",m_saveGameState);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventEndSequence)
SERIALIZE_MEMBER("playMovie", m_playMovie);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventFxScreen)
    SERIALIZE_MEMBER("isStart", m_isStart);
    SERIALIZE_MEMBER("fadeType", m_fadeType);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(Ray_EventTriggerStartTimeAttack)
END_SERIALIZATION()

}
