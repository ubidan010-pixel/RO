#ifndef _ITF_RAY_AICOMPONENT_H_
#define _ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

namespace ITF
{
    class EventCrushed;
    class PolyLine;
    class PolyLineEdge;
    class Ray_GameMaterial_Template;
    class PunchStim;
    class Ray_EventSpawnReward;

    class Ray_AIComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_AIComponent,AIComponent,2143961386);
        DECLARE_SERIALIZE()

    public:

        Ray_AIComponent();
        virtual ~Ray_AIComponent();

        virtual void                    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                    Update( f32 _dt );

        virtual void                    receiveHit( HitStim* _hit, bbool _damageDependsOnLevel = btrue, u32 _forcedDamage = 0 );
        void                            receiveCrush( EventCrushed* _crushed );
        virtual void                    receiveHit( ActorRef _attacker, u32 _level, const Vec2d& _dir, const Vec3d& _fxPos, bbool _damageDependsOnLevel = btrue, u32 _forcedDamage = 0 );
        ITF_INLINE EReceivedHitType     getLastHitType() const { return m_lastHitType; }
        void                            playLastHitFx();

        virtual void                    onEvent(Event* _event);
        virtual void                    onCheckpointLoaded();

		// Change the vacuum data template
        virtual f32                     getSoftCollisionRadius() const;
        f32                             getSoftCollisionRadiusMulitiplier() const { return m_softCollisionRadiusMulitiplier; }
        void                            setSoftCollisionRadiusMulitiplier(f32 _multi)  { m_softCollisionRadiusMulitiplier = _multi; }
#ifdef ITF_SUPPORT_EDITOR
        virtual void                    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        ITF_INLINE void                 setHasBeenVacuumed( bbool _vacuumed )   { m_hasBeenVaccumed = _vacuumed; }
        ITF_INLINE bbool                hasBeenVacuumed() const                 { return m_hasBeenVaccumed; }

        ITF_INLINE ActorRef             getRewardReceiver() const { return m_rewardReceiver; }
        ITF_INLINE void                 setRewardReceiver( ActorRef _receiver ) { m_rewardReceiver = _receiver; }
    protected:

        const class Ray_AIComponent_Template* getTemplate() const;
		
#ifdef ITF_SUPPORT_EDITOR
        virtual void                    processEventQueryLumCount(Ray_EventQueryLumCount *_queryLumCount);
#endif

        void                            updateMaterials();
        bbool                           processMaterial( PolyLine* _poly, const PolyLineEdge* _edge, const Ray_GameMaterial_Template* _mat );

        StickToPolylinePhysComponent*   m_physComponent;

        EReceivedHitType                m_lastHitType;

        Ray_EventSpawnReward*           m_receiveHitReward;
        ActorRef                        m_rewardReceiver;
        bbool                           m_rewardGiven;

        //instigator->level->FXControl
		bbool							m_hasBeenVaccumed;
        f32                             m_softCollisionRadiusMulitiplier;
    };

    //////////////////////////////////////////////////////////////////////////
    class Ray_AIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIComponent_Template,AIComponent_Template,4151908991);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_AIComponent);

    public:

        Ray_AIComponent_Template();
        ~Ray_AIComponent_Template();

        virtual void onTemplateLoaded();

        ITF_INLINE bbool getReactivateOnCheckpoint()							    const { return m_reactivateOnCheckpoint; }
        ITF_INLINE bbool getCustomCheckpointHandling()                              const { return m_customCheckpointHandling; }
        ITF_INLINE f32 getSoftCollisionRadius() const { return m_softCollisionRadius; }
        ITF_INLINE const Ray_EventSpawnReward* getReceiveHitReward() const { return m_receiveHitReward; }
        ITF_INLINE bbool getInvincibleToDangerousMaterial() const { return m_invincibleToDangerousMaterial; }
        ITF_INLINE bbool alsoCheckEncroachedDangerousMaterials() const {return m_alsoCheckEncroachedDangerousMaterials;}

    private:
        bbool                       m_reactivateOnCheckpoint;
        bbool                       m_customCheckpointHandling;
        f32                         m_softCollisionRadius;
        Ray_EventSpawnReward*       m_receiveHitReward;
        bbool                       m_invincibleToDangerousMaterial;
        bbool                       m_alsoCheckEncroachedDangerousMaterials;
    };

    ITF_INLINE const Ray_AIComponent_Template* Ray_AIComponent::getTemplate() const
    {
        return static_cast<const Ray_AIComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_AICOMPONENT_H_
