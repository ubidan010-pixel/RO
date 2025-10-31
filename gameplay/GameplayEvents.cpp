#include "precompiled_gameplay.h"

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(EventHanging)
IMPLEMENT_OBJECT_RTTI(EventHangUpdate)
IMPLEMENT_OBJECT_RTTI(EventStickOnPolyline)
IMPLEMENT_OBJECT_RTTI(EventStickOnPolylineChangeEdge)
IMPLEMENT_OBJECT_RTTI(EventStickOnPolylineUpdate)
IMPLEMENT_OBJECT_RTTI(EventBlockedByPolyline)
IMPLEMENT_OBJECT_RTTI(EventPolylineBlockingContact)
IMPLEMENT_OBJECT_RTTI(EventWallJump)
IMPLEMENT_OBJECT_RTTI(EventRopeSwingImpulse)
IMPLEMENT_OBJECT_RTTI(EventSwiming)
IMPLEMENT_OBJECT_RTTI(EventSwimingPlouf)
IMPLEMENT_OBJECT_RTTI(EventCollide)
IMPLEMENT_OBJECT_RTTI(EventInteractionQuery)
IMPLEMENT_OBJECT_RTTI(EventCrushed)
IMPLEMENT_OBJECT_RTTI(EventQueryPhysicsData)
IMPLEMENT_OBJECT_RTTI(EventDetach)
IMPLEMENT_OBJECT_RTTI(EventTrail)
IMPLEMENT_OBJECT_RTTI(EventActivate)
IMPLEMENT_OBJECT_RTTI(EventShow)
IMPLEMENT_OBJECT_RTTI(EventQuerySnapData)
IMPLEMENT_OBJECT_RTTI(EventQueryFaction)
IMPLEMENT_OBJECT_RTTI(EventQueryIsDead)
IMPLEMENT_OBJECT_RTTI(EventQueryIsDisabled)
IMPLEMENT_OBJECT_RTTI(EventHitSuccessful)
IMPLEMENT_OBJECT_RTTI(EventForceOnActor)
IMPLEMENT_OBJECT_RTTI(EventForceTransfer)
IMPLEMENT_OBJECT_RTTI(EventAnimUpdated)
IMPLEMENT_OBJECT_RTTI(EventPhysicsUpdated)
IMPLEMENT_OBJECT_RTTI(EventPlayerActivationChanged)
IMPLEMENT_OBJECT_RTTI(EventPlayerIndexChange)
IMPLEMENT_OBJECT_RTTI(EventPlayerModeChanged)
IMPLEMENT_OBJECT_RTTI(EventPlayerModeSet)
IMPLEMENT_OBJECT_RTTI(EventPlayerHpChanged)
IMPLEMENT_OBJECT_RTTI(EventCheckpointReached)
IMPLEMENT_OBJECT_RTTI(EventRevertToLastCheckpoint)
IMPLEMENT_OBJECT_RTTI(EventPushForce)
IMPLEMENT_OBJECT_RTTI(EventSetPlayer)
IMPLEMENT_OBJECT_RTTI(EventLockPlayers)
IMPLEMENT_OBJECT_RTTI(EventZoneTrigger)
IMPLEMENT_OBJECT_RTTI(EventQueryCanStickOrCollide)
IMPLEMENT_OBJECT_RTTI(EventSetDirection)
IMPLEMENT_OBJECT_RTTI(EventSetTarget)
IMPLEMENT_OBJECT_RTTI(EventSetTargetPos)
IMPLEMENT_OBJECT_RTTI(EventDead)
IMPLEMENT_OBJECT_RTTI(EventStopPlayer)
IMPLEMENT_OBJECT_RTTI(EventBouncedOnPolyline)
IMPLEMENT_OBJECT_RTTI(EventQueryBlocksHits)
IMPLEMENT_OBJECT_RTTI(EventQueryCanReceiveHit)
IMPLEMENT_OBJECT_RTTI(EventQueryPhysShape)
IMPLEMENT_OBJECT_RTTI(EventChangeBehaviourQuery)
IMPLEMENT_OBJECT_RTTI(EventChangeExternBhvValidationQuery)
IMPLEMENT_OBJECT_RTTI(EventQueryIsCaught)
IMPLEMENT_OBJECT_RTTI(EventSetupBezier)
IMPLEMENT_OBJECT_RTTI(EventVirtualLinkBroadcast)
IMPLEMENT_OBJECT_RTTI(EventVirtualLinkChildQuery)
IMPLEMENT_OBJECT_RTTI(EventDisableCollision)
IMPLEMENT_OBJECT_RTTI(EventQueryCanZoomOnTarget);
IMPLEMENT_OBJECT_RTTI(EventForceStickOnEdge);
IMPLEMENT_OBJECT_RTTI(EventQueryWaterInfluence);
IMPLEMENT_OBJECT_RTTI(EventTrajectorySpawn);
IMPLEMENT_OBJECT_RTTI(EventTriggerBehaviour);
IMPLEMENT_OBJECT_RTTI(EventOrangeContact);
IMPLEMENT_OBJECT_RTTI(EventPoolActorRegistration);
IMPLEMENT_OBJECT_RTTI(EventPoolActorRecycle);
IMPLEMENT_OBJECT_RTTI(EventTimedSpawnerDataSet);
IMPLEMENT_OBJECT_RTTI(EventQueryIsSprinting)
IMPLEMENT_OBJECT_RTTI(EventSetOriginalSender);
IMPLEMENT_OBJECT_RTTI(EventGetPolylineConstrainedPosition)
IMPLEMENT_OBJECT_RTTI(EventDisplayText)
IMPLEMENT_OBJECT_RTTI(EventPreCheckpointSave);

BEGIN_SERIALIZATION_CHILD(EventHanging)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventHangUpdate)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventStickOnPolyline)
    SERIALIZE_MEMBER("sticked", m_sticked);   // serialized for matching event listeners
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventStickOnPolylineChangeEdge)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventStickOnPolylineUpdate)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventBlockedByPolyline)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventPolylineBlockingContact)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventWallJump)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventRopeSwingImpulse)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventCollide)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventQueryPhysShape)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventSwiming)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventSwimingPlouf)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventInteractionQuery)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventCrushed)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventQueryPhysicsData)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventDetach)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventTrail)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventActivate)
    SERIALIZE_MEMBER("activated", m_activated);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventShow)
    SERIALIZE_MEMBER("alpha",m_alpha);
    SERIALIZE_MEMBER("transitionTime",m_transitionTime);
    SERIALIZE_MEMBER("overrideColor",m_overrideColor);
    SERIALIZE_MEMBER("color",m_newColor);
    SERIALIZE_MEMBER("pauseOnEnd",m_pauseOnEnd);
    SERIALIZE_MEMBER("destroyOnEnd",m_destroyOnEnd);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventQuerySnapData)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventQueryFaction)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventHitSuccessful)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventForceOnActor)
END_SERIALIZATION()

EventForceOnActor::EventForceOnActor()
: m_actor(ITF_INVALID_OBJREF)
, m_pos(Vec2d::Zero)
, m_force(Vec2d::Zero)
{

}

EventForceOnActor::EventForceOnActor( ObjectRef _actor, const Vec2d _pos, const Vec2d& _force )
: m_actor(_actor)
, m_pos(_pos)
, m_force(_force)
{
}

BEGIN_SERIALIZATION_CHILD(EventForceTransfer)
END_SERIALIZATION()

EventForceTransfer::EventForceTransfer()
: m_pos(Vec2d::Zero)
, m_force(Vec2d::Zero)
{

}

EventForceTransfer::EventForceTransfer( const StringID& _polyID, const StringID& _polyPointID,
                                        const Vec2d& _pos, const Vec2d& _force )
: m_polyID(_polyID)
, m_polyPointID(_polyPointID)
, m_pos(_pos)
, m_force(_force)
{
}

BEGIN_SERIALIZATION_CHILD(EventAnimUpdated)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventPhysicsUpdated)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventPlayerModeChanged)
SERIALIZE_MEMBER("mode", m_mode);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventPlayerModeSet)
SERIALIZE_MEMBER("mode", m_mode);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventPushForce)
SERIALIZE_MEMBER("force", m_force);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventLockPlayers)
SERIALIZE_MEMBER("force", m_lock);
SERIALIZE_MEMBER("stopPlayers", m_stopPlayers);
SERIALIZE_MEMBER("stopInputs", m_stopInputs);
SERIALIZE_MEMBER("forceRevive", m_forceRevive);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventZoneTrigger)
    SERIALIZE_MEMBER("zoneID", m_zoneID);
    SERIALIZE_MEMBER("radius", m_radius);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventSetDirection)
    SERIALIZE_MEMBER("direction", m_direction);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventSetTarget)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventDead)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventStopPlayer)
    SERIALIZE_MEMBER("stop", m_stop);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventBouncedOnPolyline)
END_SERIALIZATION()

EventBouncedOnPolyline::EventBouncedOnPolyline()
: m_polyRef(ITF_INVALID_OBJREF)
, m_edgeIndex(U32_INVALID)
, m_dir(Vec2d::Zero)
{
}

EventBouncedOnPolyline::EventBouncedOnPolyline( ObjectRef _poly, u32 _edgeIndex, const Vec2d& _dir )
: m_polyRef(_poly)
, m_edgeIndex(_edgeIndex)
, m_dir(_dir)
{
}

BEGIN_SERIALIZATION_CHILD(EventQueryBlocksHits)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventQueryCanReceiveHit)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventChangeBehaviourQuery)
    SERIALIZE_MEMBER( "wantedBehaviourName", m_wantedBehaviourName );
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventChangeExternBhvValidationQuery)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventVirtualLinkBroadcast)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventVirtualLinkChildQuery)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventDisableCollision)
SERIALIZE_MEMBER("disable", m_disable);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventForceStickOnEdge)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventQueryWaterInfluence)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventTrajectorySpawn)
    SERIALIZE_MEMBER("spawneeIndex", m_spawneeIndex);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventTriggerBehaviour)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventOrangeContact)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventPoolActorRegistration)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventPoolActorRecycle)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventTimedSpawnerDataSet)
    SERIALIZE_MEMBER("spawnDelay", m_spawnDelay);
    SERIALIZE_MEMBER("spawnRate", m_spawnRate);
    SERIALIZE_MEMBER("burstElementsCount", m_burstElementsCount);
    SERIALIZE_MEMBER("burstCount", m_burstCount);
    SERIALIZE_MEMBER("burstDelay", m_burstDelay);
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventQueryIsSprinting)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventGetPolylineConstrainedPosition)
END_SERIALIZATION()

BEGIN_SERIALIZATION_CHILD(EventDisplayText)
    SERIALIZE_MEMBER("lineId", m_lineId);
END_SERIALIZATION()

}
