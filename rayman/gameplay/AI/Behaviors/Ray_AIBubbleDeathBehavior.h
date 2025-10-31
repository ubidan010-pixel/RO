#ifndef _ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_
#define _ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{
    class Ray_EventSpawnReward;

    class Ray_AIBubbleDeathBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBubbleDeathBehavior, AIBehavior,3031963609)

    public:

        Ray_AIBubbleDeathBehavior();
        virtual ~Ray_AIBubbleDeathBehavior();

        virtual void            onActivate();
        virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onEvent(Event * _event);
        virtual void            onActionFinished();
        virtual void            update( f32 _dt );

        // for Ray_alTranquiloAiComponent
        ITF_INLINE void release() { startFloat(); }
        ITF_INLINE void setLifetime( const f32 _value ) { m_floatTime = _value; }
        ITF_INLINE void setFloatForceX( const f32 _value ) { m_floatForceX = _value; }


    protected:
        void initRewardAndFx();
        void spawnDeathFx();

    private:
        ITF_INLINE const class Ray_AIBubbleDeathBehavior_Template * getTemplate() const;

        void startFloat();
        void startExplode();
        void processBlockedByPolyline( EventBlockedByPolyline * _onBlocked );
        void spawnReward( const Vec2d & _dir, ObjectRef _receiver, bbool _achievement = btrue );
        void updateSwell();
        void updateFloatForce();
        void updateFloat( f32 _dt );
        void updateAngle( f32 _dt );
        void updateSoftColl();
        bbool checkSquash();
        bbool checkWater();
        Vec2d getPedestalPos( u32 _slotIndex );
        i32 getPedestalFreeSlotIndex();
        u32 getPedestalUserCount();
        void updatePedestal();
        bbool canSupport();
        f32 getSoftCollRadius()const;
        void processReleasePedestal( Ray_EventReleasePedestal * onReleasePedestal );
        void processQueryPedestalInfo( Ray_EventQueryPedestalInfo * onQueryPedestal );
        void processInteractionQuery( EventInteractionQuery * onQueryInteraction );
        void processCrush( EventCrushed * onCrush );
        void processStim( PunchStim * stim );

        class StickToPolylinePhysComponent* m_physComponent;
        class PhantomComponent*             m_phantomComponent;
        SpawneeGenerator        m_fxSpawner;
        Ray_EventSpawnReward*   m_reward;
        Ray_EventSpawnReward*   m_rewardAtStart;
        AIAction*      m_swellAction;
        AIAction*      m_floatAction;
        AIAction*      m_explodeAction;
        f32            m_timer;
        f32            m_initialAngle;
        f32            m_accumulatedTime;
        bbool          m_wasHit;
        Vec2d          m_hitDir;
        f32            m_floatForceTimer;
        bbool          m_firstUpdate;
        ActorRefList   m_supportedActors; 
        ITF_VECTOR<std::pair< ActorRef, f32  > > m_recentlySupportedActors;
        f32            m_floatForceExtraTimer;
        f32            m_stimPossibilityTimer;

        // copied from template, overriable by spawner
        f32            m_floatTime;
        f32            m_floatForceX;
    };




    class Ray_AIBubbleDeathBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBubbleDeathBehavior_Template,TemplateAIBehavior,4112732321);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBubbleDeathBehavior);

    public:

        Ray_AIBubbleDeathBehavior_Template();
        ~Ray_AIBubbleDeathBehavior_Template();

        ITF_INLINE const Ray_EventSpawnReward* getReward() const { return m_reward; }
        ITF_INLINE const Ray_EventSpawnReward* getRewardAtStart() const { return m_rewardAtStart; }
        ITF_INLINE const Path& getFxPath() const { return m_fxPath; }
        ITF_INLINE bbool getSpawnOnMarker() const { return m_spawnOnMarker; }
        ITF_INLINE const AIAction_Template* getSwellAction() const { return m_swellAction; }
        ITF_INLINE const AIAction_Template* getFloatAction() const { return m_floatAction; }
        ITF_INLINE const AIAction_Template* getExplodeAction() const { return m_explodeAction; }
        ITF_INLINE f32 getFloatTime() const { return m_floatTime; }
        ITF_INLINE f32 getFloatForce() const { return m_floatForce; }
        ITF_INLINE Angle getFloatAngleOffset() const { return m_floatAngleOffset; }
        ITF_INLINE f32 getFloatAngleFrequency() const { return m_floatAngleFrequency; }
        ITF_INLINE f32 getFloatAirFrictionMultiplier() const { return m_floatAirFrictionMultiplier; }
        ITF_INLINE f32 getHitForce() const { return m_hitForce; }
        ITF_INLINE f32 getAvoidanceForce() const { return m_avoidanceForce; }
        ITF_INLINE f32 getAvoidanceRadius() const { return m_avoidanceRadius; }
        ITF_INLINE f32 getMaxFloatSpeed() const { return m_maxFloatSpeed; }
        ITF_INLINE f32 getMinFloatSpeed() const { return m_minFloatSpeed; }
        ITF_INLINE f32 getFloatForceExtraTime() const { return m_floatForceExtraTime; }
        ITF_INLINE f32 getMaxFloatExtraSpeed() const { return m_maxFloatExtraSpeed; }
        ITF_INLINE f32 getMinFloatExtraSpeed() const { return m_minFloatExtraSpeed; }
        ITF_INLINE f32 getFloatForceTime() const { return m_floatForceTime; }
        ITF_INLINE f32 getSoftCollisionRadiusMultiplier() const { return m_softCollisionRadiusMultiplier; }
        ITF_INLINE f32 getSquashPenetrationRadius() const { return m_squashPenetrationRadius; }
        ITF_INLINE bbool getExplodeOnPlayer() const { return m_explodeOnPlayer; }
        ITF_INLINE bbool getUsePhysRadiusAsSoftCollRadius() const { return m_usePhysRadiusAsSoftCollRadius; }
        ITF_INLINE f32 getFloatForceX() const { return m_floatForceX; }
        ITF_INLINE f32 getMaxSinkSpeed() const { return m_maxSinkSpeed; }
        ITF_INLINE f32 getMinSinkSpeed() const { return m_minSinkSpeed; }
        ITF_INLINE f32 getSinkForce() const { return m_sinkForce; }
        ITF_INLINE f32 getMaxXSpeed() const { return m_maxXSpeed; }
        ITF_INLINE f32 getMinXSpeed() const { return m_minXSpeed; }
        ITF_INLINE f32 getExplosionFeedBackTime() const { return m_explosionFeedBackTime; }
        ITF_INLINE f32 getExplosionFeedBackFreq() const { return m_explosionFeedBackFreq; }
        ITF_INLINE f32 getExplosionFeedBackAmplitude() const { return m_explosionFeedBackAmplitude; }
        ITF_INLINE u32 getPedestalMaxUserCount() const { return m_pedestalMaxUserCount; }
        ITF_INLINE f32 getPedestalOffset() const { return m_pedestalOffset; }
        ITF_INLINE ITF::bbool getCheckWater() const { return m_checkWater; }
        ITF_INLINE f32 getSupportedActorInvincibilityTimer() const { return m_supportedActorInvincibilityTimer; }
        ITF_INLINE f32 getWaitDurationBeforeStimPossibility() const { return m_waitDurationBeforeStimPossibility; }

    private:
        Ray_EventSpawnReward*   m_reward;
        Ray_EventSpawnReward*   m_rewardAtStart;
        Path                    m_fxPath;
        bbool                   m_spawnOnMarker;
        AIAction_Template*      m_swellAction;
        AIAction_Template*      m_floatAction;
        AIAction_Template*      m_explodeAction;
        f32                     m_floatTime;
        f32                     m_floatForce;
        f32                     m_sinkForce;
        f32                     m_floatForceX;
        Angle                   m_floatAngleOffset;
        f32                     m_floatAngleFrequency;
        f32                     m_floatAirFrictionMultiplier;
        f32                     m_hitForce;
        f32                     m_avoidanceForce;
        f32                     m_avoidanceRadius;
        f32                     m_maxFloatSpeed;
        f32                     m_minFloatSpeed;
        f32                     m_floatForceExtraTime;  // extra boost when jumping off a bubble
        f32                     m_maxFloatExtraSpeed;
        f32                     m_minFloatExtraSpeed;
        f32                     m_maxSinkSpeed;
        f32                     m_minSinkSpeed;
        f32                     m_maxXSpeed;
        f32                     m_minXSpeed;
        f32                     m_floatForceTime;
        f32                     m_softCollisionRadiusMultiplier;
        f32                     m_squashPenetrationRadius;
        bbool                   m_explodeOnPlayer;
        u32                     m_pedestalMaxUserCount;
        f32                     m_pedestalOffset;
        bbool                   m_usePhysRadiusAsSoftCollRadius;
        f32                     m_explosionFeedBackTime;
        f32                     m_explosionFeedBackFreq;
        f32                     m_explosionFeedBackAmplitude;
        bbool                   m_checkWater;
        f32                     m_supportedActorInvincibilityTimer;
        f32                     m_waitDurationBeforeStimPossibility;
    };

    ITF_INLINE const class Ray_AIBubbleDeathBehavior_Template * Ray_AIBubbleDeathBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIBubbleDeathBehavior_Template *>(m_template);
    }
}

#endif // _ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_

