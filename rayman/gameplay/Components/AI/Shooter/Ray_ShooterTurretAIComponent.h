#ifndef _ITF_RAY_SHOOTERTURRETAICOMPONENT_H_
#define _ITF_RAY_SHOOTERTURRETAICOMPONENT_H_

//#ifndef _ITF_RAY_AICOMPONENT_H_
//#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
//#endif //_ITF_RAY_AICOMPONENT_H_
#ifndef _ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MultiPiecesActorAIComponent.h"
#endif //_ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_

#ifndef _ITF_TIMEDSPAWNERCOMPONENT_H_
#include "gameplay/Components/Common/TimedSpawnerComponent.h"
#endif //_ITF_TIMEDSPAWNERCOMPONENT_H_

namespace ITF
{
    class Ray_AIShooterAttackBehavior;
    class AIReceiveHitBehavior;
    class Ray_AIGroundAttackBehavior;

    class Ray_ShooterTurretAIComponent : public Ray_MultiPiecesActorAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterTurretAIComponent, Ray_MultiPiecesActorAIComponent ,3379650764)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterTurretAIComponent();
        ~Ray_ShooterTurretAIComponent() {}

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                Update( f32 _deltaTime );
        virtual void                onBehaviorFinished();
        virtual void                onEvent( Event* _event);
        virtual void                updateAnimInput();
    protected:

    private:

        const class Ray_ShooterTurretAIComponent_Template* getTemplate() const;
        void                        onCheckpointLoaded();

        //void                        receiveHit( PunchStim* _hit );
        virtual void                processHit( HitStim* _hit );

        ObjectRef                   findEnemy(bbool & _closeRange) const;
        void                        processInteractionQuery( EventInteractionQuery* interactionQuery );
        void                        processCrushed( EventCrushed* crushed );
        void                        processTrigger( EventTrigger* _triggerEvt );
        void                        onFinishedCrush();
        void                        startCrushed();

        void                        start();
        void                        stop();

        AIBehavior*                     m_idleBehavior;
        Ray_AIShooterAttackBehavior*    m_attackBehavior;
        AIReceiveHitBehavior*           m_receiveHitBehavior;
        AIBehavior*                     m_deathBehavior;
        AIBehavior*                     m_crushedBehavior;

        AIBehavior*                     m_activateBehavior;
        AIBehavior*                     m_desactivateBehavior;
        AIBehavior*                     m_closedBehavior;
        
        bbool                           m_open;
        bbool                           m_sendHitToPieces;

        // Editable params
        TimedSpawnerData                m_timedSpawnerData;
        f32                             m_fixedAngle;        
      
    };



    //---------------------------------------------------------------------------------------------------------------------------------------

    class Ray_AIShooterAttackBehavior_Template;
    class AIReceiveHitBehavior_Template;
    class Ray_AIGroundAttackBehavior_Template;
    class Ray_AIShooterCloseAttackBehavior_Template;

    class Ray_ShooterTurretAIComponent_Template : public Ray_MultiPiecesActorAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterTurretAIComponent_Template, Ray_MultiPiecesActorAIComponent_Template,3392503840);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterTurretAIComponent);

    public:

        Ray_ShooterTurretAIComponent_Template();
        ~Ray_ShooterTurretAIComponent_Template();

        ITF_INLINE const TemplateAIBehavior*                             getIdle() const { return m_idleBehavior; }
        ITF_INLINE const Ray_AIShooterAttackBehavior_Template*           getAttack() const { return m_attackBehavior; }
        ITF_INLINE const AIReceiveHitBehavior_Template*                  getReceiveHit() const { return m_receiveHitBehavior; }
        ITF_INLINE const TemplateAIBehavior*                             getDeath() const { return m_deathBehavior; }
        ITF_INLINE const TemplateAIBehavior*                             getCrushed() const { return m_crushedBehavior; }
        ITF_INLINE const TemplateAIBehavior*                             getActivateBhv() const { return m_activateBehavior; }
        ITF_INLINE const TemplateAIBehavior*                             getDesactivateBhv() const { return m_desactivateBehavior; }
        ITF_INLINE const TemplateAIBehavior*                             getClosedBhv() const { return m_closedBehavior; }

        ITF_INLINE f32              getFindEnemyRadius() const          {return m_findEnemyRadius;}
        ITF_INLINE f32              getCloseRadius() const              {return m_closeRadius;}
        ITF_INLINE const AABB&      getDetectionRange() const           {return m_detectionRange;}
        ITF_INLINE const AABB&      getDetectionCloseRange() const      {return m_detectionCloseRange;}
        ITF_INLINE bbool            getUseRadius() const                {return m_useRadius;}
        ITF_INLINE bbool            isTriggerable() const               {return m_triggerable;}
        ITF_INLINE bbool            getForceLookDir() const             {return m_forceLookDir;}
        ITF_INLINE bbool            getForceLookDirRight() const        {return m_forceLookDirRight;}

    private:

        TemplateAIBehavior*                             m_idleBehavior;
        Ray_AIShooterAttackBehavior_Template*           m_attackBehavior;
        AIReceiveHitBehavior_Template*                  m_receiveHitBehavior;
        TemplateAIBehavior*                             m_deathBehavior;
        TemplateAIBehavior*                             m_crushedBehavior;
        TemplateAIBehavior*                             m_activateBehavior;
        TemplateAIBehavior*                             m_desactivateBehavior;
        TemplateAIBehavior*                             m_closedBehavior;

        f32                                             m_findEnemyRadius;
        f32                                             m_closeRadius;
        AABB                                            m_detectionRange;
        AABB                                            m_detectionCloseRange;
        bbool                                           m_useRadius;
        bbool                                           m_triggerable;
        bbool                                           m_forceLookDir;
        bbool                                           m_forceLookDirRight;
       
    };

    ITF_INLINE const Ray_ShooterTurretAIComponent_Template* Ray_ShooterTurretAIComponent::getTemplate() const
    {
        return static_cast<const Ray_ShooterTurretAIComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_SHOOTERTURRETAICOMPONENT_H_


