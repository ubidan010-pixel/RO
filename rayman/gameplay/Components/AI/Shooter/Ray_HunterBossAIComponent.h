#ifndef _ITF_RAY_HUNTERBOSSAICOMPONENT_H_
#define _ITF_RAY_HUNTERBOSSAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    class Ray_AIHunterAttackBehavior;
    class AIReceiveHitBehavior;
    class Ray_AIGroundAttackBehavior;
    class Ray_AIShooterCloseAttackBehavior;
    class Ray_HunterBossNodeComponent;
    class Ray_AIDeathBehavior;

    class Ray_HunterBossAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_HunterBossAIComponent, Ray_AIComponent,3955300393)

    public:
        DECLARE_SERIALIZE()

        Ray_HunterBossAIComponent();
        ~Ray_HunterBossAIComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );

        virtual void                onBehaviorFinished();

        void onFinishedVoid();
        virtual void                onEvent( Event* _event);
        virtual void                updateAnimInput();
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface , u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_HunterBossAIComponent_Template * getTemplate() const;

        AIBehavior*                     m_idleBehavior;
        Ray_AIHunterAttackBehavior*     m_attackBehavior;
        Ray_AIHunterAttackBehavior*     m_holeAttackBehavior;
        Ray_AIHunterAttackBehavior*     m_currentAttackBehavior;
        AIReceiveHitBehavior*           m_receiveHitBehavior;
        Ray_AIDeathBehavior*            m_deathBehavior;
        Ray_AIShooterCloseAttackBehavior*     m_closeRangeBehavior;
        AIBehavior*                     m_crushedBehavior;
        AIBehavior*                     m_voidBehavior;
        Ray_HunterBossNodeComponent *   m_nodeComponent;
        StickToPolylinePhysComponent *  m_physComponent;
        void                        clear();
        void                        postLoadInit();
        void                        onCheckpointLoaded();

        void                        processHit( PunchStim* _hit );

        ObjectRef                   findEnemy(bbool & _closeRange) const;

        Actor *                     getClosestEnemy(f32 _radius) const ;
        Actor *                     getClosestEnemy(const AABB & _range) const ;

        void                        processInteractionQuery( EventInteractionQuery* interactionQuery );
        void                        processCrushed( EventCrushed* crushed );
        void                        onFinishedCrush();

        void                        startCrushed();
        void                        startIdle();
        void                        startAttack(Ray_AIHunterAttackBehavior * _attack, ObjectRef _victim);
        void                        startVoid();
        void                        updateIdle(f32 _dt);
        void                        updateCloseRange(f32 _dt);
        void                        updateAttack(f32 _dt);

        void                        getNextNode();
        Ray_EventQueryHunterNode    m_nodeData;
        u32                         m_currentNode;
        ActorRefList                m_children;
        u32                         m_isHole;
        f32                         m_finishRadius;
        u32                         m_healthCounter;
    };


    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_HunterBossAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_HunterBossAIComponent_Template, Ray_AIComponent_Template,3752367210);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_HunterBossAIComponent);

    public:

        Ray_HunterBossAIComponent_Template();
        ~Ray_HunterBossAIComponent_Template();


        ITF_INLINE f32              getFindEnemyRadius() const          {return m_findEnemyRadius;}
        ITF_INLINE f32              getCloseRadius() const              {return m_closeRadius;}
        ITF_INLINE const AABB &     getDetectionRange() const           {return m_detectionRange;}
        ITF_INLINE const AABB &     getDetectionCloseRange() const      {return m_detectionCloseRange;}
        ITF_INLINE bbool            getUseRadius() const                {return m_useRadius;}
        ITF_INLINE u32              getMaxNodes() const                 {return m_maxNodes;}
        ITF_INLINE u32              getHealthBeforeChange() const       {return m_healthBeforeChange;}

        ITF_INLINE const class TemplateAIBehavior* getIdleBehavior() const { return m_idleBehavior; }
        ITF_INLINE const class Ray_AIHunterAttackBehavior_Template* getAttackBehavior() const { return m_attackBehavior; }
        ITF_INLINE const class Ray_AIHunterAttackBehavior_Template* getHoleAttackBehavior() const { return m_holeAttackBehavior; }
        ITF_INLINE const class Ray_AIHunterAttackBehavior_Template* getCurrentAttackBehavior() const { return m_currentAttackBehavior; }
        ITF_INLINE const class AIReceiveHitBehavior_Template* getReceiveHitBehavior() const { return m_receiveHitBehavior; }
        ITF_INLINE const class Ray_AIDeathBehavior_Template* getDeathBehavior() const { return m_deathBehavior; }
        ITF_INLINE const class Ray_AIShooterCloseAttackBehavior_Template* getCloseRangeBehavior() const { return m_closeRangeBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getCrushedBehavior() const { return m_crushedBehavior; }
        ITF_INLINE const class TemplateAIBehavior* getVoidBehavior() const { return m_voidBehavior; }

    private:
        class TemplateAIBehavior*                         m_idleBehavior;
        class Ray_AIHunterAttackBehavior_Template*        m_attackBehavior;
        class Ray_AIHunterAttackBehavior_Template*        m_holeAttackBehavior;
        class Ray_AIHunterAttackBehavior_Template*        m_currentAttackBehavior;
        class AIReceiveHitBehavior_Template*              m_receiveHitBehavior;
        class Ray_AIDeathBehavior_Template*               m_deathBehavior;
        class Ray_AIShooterCloseAttackBehavior_Template*  m_closeRangeBehavior;
        class TemplateAIBehavior*                         m_crushedBehavior;
        class TemplateAIBehavior*                         m_voidBehavior;

        f32                         m_findEnemyRadius;
        f32                         m_closeRadius;
        AABB                        m_detectionRange;
        AABB                        m_detectionCloseRange;
        bbool                       m_useRadius;
        u32                         m_maxNodes;
        u32                         m_healthBeforeChange;
    };
    ITF_INLINE const class Ray_HunterBossAIComponent_Template * Ray_HunterBossAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_HunterBossAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_HUNTERBOSSAICOMPONENT_H_

