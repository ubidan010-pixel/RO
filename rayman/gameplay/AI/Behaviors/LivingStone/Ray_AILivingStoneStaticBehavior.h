#ifndef _ITF_RAY_AILIVINGSTONESTATICBEHAVIOR_H_
#define _ITF_RAY_AILIVINGSTONESTATICBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEBEHAVIOR_H_

namespace ITF
{
    class Ray_AILivingStoneStaticBehavior : public Ray_AIGroundBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneStaticBehavior, Ray_AIGroundBaseBehavior,3233229791)

    public:

        Ray_AILivingStoneStaticBehavior();
        virtual ~Ray_AILivingStoneStaticBehavior();

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
        ITF_INLINE const class Ray_AILivingStoneStaticBehavior_Template * getTemplate() const;

        void checkAggro(Actor * _player);
        void startAggro();

        bbool checkUturn(Actor * _player);
        void startUturn();
        void onFinishedUturn();

        bbool checkIdle( Actor * player, f32 _dt );
        void startIdle();

        bbool checkAttack(Actor * player);
        void startAttack();
        void onFinishedAttack();

        void getAttackRange(AABB & _range) const;
    private:
        AIAction *  m_idle;
        AIAction *  m_uturn;
        AIAction *  m_aggro;
        AIAction *  m_attack;
        f32         m_timer;
    };

    class Ray_AILivingStoneStaticBehavior_Template : public Ray_AIGroundBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILivingStoneStaticBehavior_Template,Ray_AIGroundBaseBehavior_Template,2904103013);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AILivingStoneStaticBehavior);

    public:

        Ray_AILivingStoneStaticBehavior_Template();
        ~Ray_AILivingStoneStaticBehavior_Template();

        ITF_INLINE const AIAction_Template * getIdle() const { return m_idle; }
        ITF_INLINE const AIAction_Template * getUturn() const { return m_uturn; }
        ITF_INLINE const AIAction_Template * getAggro() const { return m_aggro; }
        ITF_INLINE const AIAction_Template * getAttack() const { return m_attack; }
        ITF_INLINE f32 getDetectionRadius() const { return m_detectionRadius; }
        ITF_INLINE f32 getDetectionHysteresisTime() const { return m_detectionHysteresisTime; }
        ITF_INLINE const AABB & getAttackRange() const { return m_attackRange; }
    private:

        AIAction_Template * m_idle;
        AIAction_Template * m_uturn;
        AIAction_Template * m_aggro;
        AIAction_Template * m_attack;

        f32                 m_detectionRadius;
        f32                 m_detectionHysteresisTime;
        AABB                m_attackRange;
    };
    ITF_INLINE const Ray_AILivingStoneStaticBehavior_Template * Ray_AILivingStoneStaticBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AILivingStoneStaticBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AILIVINGSTONESTATICBEHAVIOR_H_