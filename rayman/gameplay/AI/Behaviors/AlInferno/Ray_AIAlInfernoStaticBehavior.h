#ifndef _ITF_RAY_AIALINFERNOSTATICBEHAVIOR_H_
#define _ITF_RAY_AIALINFERNOSTATICBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

namespace ITF
{
    class Ray_AIAlInfernoStaticBehavior : public Ray_AIGroundBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIAlInfernoStaticBehavior, Ray_AIGroundBaseBehavior,1719286819);

    public:

        Ray_AIAlInfernoStaticBehavior();
        virtual ~Ray_AIAlInfernoStaticBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );

        virtual void                        onActivate();

        virtual void                        onDeactivate();
        virtual void                        onActionFinished();

        virtual void                        onEvent(Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:
        ITF_INLINE const class Ray_AIAlInfernoStaticBehavior_Template * getTemplate() const;

        void startUturn();
        void onFinishedUturn();
        void startCycleUturn();
        void onFinishedCycleUturn();

        void startIdle();
        void updateIdle(f32 _dt);

        bbool checkAttack(Actor * player);
        void startAttack();
        void onFinishedAttack();
    private:
        AIAction *  m_idle;
        AIAction *  m_uturn;
        AIAction *  m_attack;
        AIAction *  m_cycleUturn;
        f32         m_idleTimer;
        u32         m_flameHandle;
        bbool       m_active;
    };

    class Ray_AIAlInfernoStaticBehavior_Template : public Ray_AIGroundBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIAlInfernoStaticBehavior_Template,Ray_AIGroundBaseBehavior_Template,190919790);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIAlInfernoStaticBehavior);

    public:

        Ray_AIAlInfernoStaticBehavior_Template();
        ~Ray_AIAlInfernoStaticBehavior_Template();

        ITF_INLINE const AIAction_Template * getIdle() const { return m_idle; }
        ITF_INLINE const AIAction_Template * getUturn() const { return m_uturn; }
        ITF_INLINE const AIAction_Template * getAttack() const { return m_attack; }
        ITF_INLINE AIAction_Template * getCycleUturn() const { return m_cycleUturn; }
        ITF_INLINE f32 getIdleTime() const { return m_idleTime; }
        ITF_INLINE bbool getAttackOnDetection() const { return m_attackOnDetection; }
        ITF_INLINE const AABB & getDetectionRange() const { return m_detectionRange; }
        ITF_INLINE bbool getStickOnWalls() const { return m_stickOnWalls; }
    private:

        AIAction_Template * m_idle;
        AIAction_Template * m_uturn;
        AIAction_Template * m_attack;
        AIAction_Template * m_cycleUturn;
        f32                 m_idleTime;
        bbool               m_attackOnDetection;
        AABB                m_detectionRange;
        bbool               m_stickOnWalls;
    };
    ITF_INLINE const Ray_AIAlInfernoStaticBehavior_Template * Ray_AIAlInfernoStaticBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIAlInfernoStaticBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIALINFERNOSTATICBEHAVIOR_H_