#ifndef _ITF_RAY_REWARDAICOMPONENT_H_
#define _ITF_RAY_REWARDAICOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    class AIBehavior;
    class StickToPolylinePhysComponent;
    class PlayerDetectorComponent;
    class Ray_EventSpawnReward;
    class Ray_Player;
    class Trail3DComponent;
    class TriggerComponent;

    class Ray_RewardAIComponent : public AIComponent
    {

    public:

        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardAIComponent, AIComponent,4028072561)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        Ray_RewardAIComponent();
        ~Ray_RewardAIComponent();

        virtual bbool       needsUpdate() const     { return btrue; }
        virtual bbool       needsDraw() const       { return bfalse; }
        virtual bbool       needsDraw2D() const     { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        Update( f32 _deltaTime );
        virtual void        onEvent( Event* _event );
        
    private:
                void        snapOrPickup(ActorRef _playerRef);
                Vec2d       getCameraDirection();
                bbool       findSnapPlayer();
                void        onCarried();
                void        onDropped();
                void        pickup(ActorRef _playerRef);
    private:

        ITF_INLINE const class Ray_RewardAIComponent_Template * getTemplate() const;

        Ray_Player*   m_snapPlayer;

        AIBehavior*   m_idleBehavior;
        AIBehavior*   m_carriedBehavior;
        AIBehavior*   m_pickupBehavior;
        AIBehavior*   m_snapBehavior;

        Ray_EventSpawnReward* m_fallbackReward;

        StickToPolylinePhysComponent*   m_physComponent;
        PlayerDetectorComponent*        m_playerDetectorComponent;
        TriggerComponent*               m_triggerComponent;
        Trail3DComponent*               m_trailComponent;
    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class TemplateAIBehavior;

    class Ray_RewardAIComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardAIComponent_Template, AIComponent_Template,281197817);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_RewardAIComponent);

    public:

        Ray_RewardAIComponent_Template();
        ~Ray_RewardAIComponent_Template();

        ITF_INLINE const TemplateAIBehavior* getIdleBehavior() const        { return m_idleBehavior; }
        ITF_INLINE const TemplateAIBehavior* getCarriedBehavior() const     { return m_carriedBehavior; }
        ITF_INLINE const TemplateAIBehavior* getPickupBehavior() const      { return m_pickupBehavior; }
        ITF_INLINE const TemplateAIBehavior* getSnapBehavior() const        { return m_snapBehavior; }

        ITF_INLINE Ray_EventSpawnReward* getFallbackReward() const { return m_fallbackReward; }

        ITF_INLINE u32 getAddHp() const    { return m_addHp; }
        ITF_INLINE u32 getAddMaxHp() const { return m_addMaxHp; }

        ITF_INLINE f32 getSnapFactor()      const { return m_snapFactor; }
        ITF_INLINE f32 getSnapMinDistance() const { return m_snapMinDistance; }

    private:

        TemplateAIBehavior*   m_idleBehavior;
        TemplateAIBehavior*   m_carriedBehavior;
        TemplateAIBehavior*   m_pickupBehavior;
        TemplateAIBehavior*   m_snapBehavior; // if picked up a player that doesn't need a heart
        
        Ray_EventSpawnReward* m_fallbackReward;

        u32 m_addHp;
        u32 m_addMaxHp;

        f32 m_snapFactor;
        f32 m_snapMinDistance;
    };
    ITF_INLINE const class Ray_RewardAIComponent_Template * Ray_RewardAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_RewardAIComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_REWARDAICOMPONENT_H_

