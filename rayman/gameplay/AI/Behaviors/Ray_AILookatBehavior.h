#ifndef _ITF_RAY_AILOOKATBEHAVIOR_H_
#define _ITF_RAY_AILOOKATBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class Ray_AILookatBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILookatBehavior, AIBehavior,3419701131)

    public:

        Ray_AILookatBehavior();
        virtual ~Ray_AILookatBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );

        virtual void                        onActivate();

        virtual void                        onDeactivate();
        virtual void                        onActionFinished();

        virtual void                        onEvent(Event * _event);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        virtual void                        updateAnimInputs();

    protected:
        ITF_INLINE const class Ray_AILookatBehavior_Template * getTemplate() const;

        bbool checkAggro(Actor * _player);
        void updateAggro( Actor * _player, f32 _delta );
        void startAggro();

        bbool checkUturn(Actor * _player);
        void startUturn();
        void onFinishedUturn();

        bbool checkIdle( Actor * _player, f32 _dt );
        void updateIdle( Actor * _player );
        void startIdle();

        bbool checkAttack(Actor * _player);
        void startAttack();
        void updateAttack(Actor * _player, f32 _delta);
        void onFinishedAttack();

        void    updateLookAt(f32 _dt, Actor * _player);
    private:
        AIAction *  m_idle;
        AIAction *  m_uturn;
        AIAction *  m_aggro;
        AIAction *  m_attack;
        f32         m_detectionHysteresisTimer;
        Angle       m_lookAtAngle;
        ActorRef    m_target;
        f32         m_attackDelayTimer;
        u32         m_flameHandle;
        bbool       m_updateLookAt;
        class StickToPolylinePhysComponent * m_physComponent;
    };

    class Ray_AILookatBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILookatBehavior_Template,TemplateAIBehavior,1462428637);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AILookatBehavior);

    public:

        Ray_AILookatBehavior_Template();
        ~Ray_AILookatBehavior_Template();

        ITF_INLINE const AIAction_Template * getIdle() const { return m_idle; }
        ITF_INLINE const AIAction_Template * getUturn() const { return m_uturn; }
        ITF_INLINE const AIAction_Template * getAggro() const { return m_aggro; }
        ITF_INLINE const AIAction_Template * getAttack() const { return m_attack; }
        ITF_INLINE f32 getDetectionRadius() const { return m_detectionRadius; }
        ITF_INLINE f32 getDetectionHysteresisTime() const { return m_detectionHysteresisTime; }
        ITF_INLINE f32 getAttackRadius() const { return m_attackRadius; }
        ITF_INLINE f32 getAttackDelay() const { return m_attackDelay; }
        ITF_INLINE bbool getAttackLookAt() const { return m_attackLookAt; }
        ITF_INLINE f32 getLookAtSmoothFactor() const { return m_lookAtSmoothFactor; }
        ITF_INLINE bbool getStickOnWalls() const { return m_stickOnWalls; }

    private:

        AIAction_Template * m_idle;
        AIAction_Template * m_uturn;
        AIAction_Template * m_aggro;
        AIAction_Template * m_attack;

        f32                 m_detectionRadius;
        f32                 m_detectionHysteresisTime;
        f32                 m_attackRadius;
        f32                 m_attackDelay;
        bbool               m_attackLookAt;
        f32                 m_lookAtSmoothFactor;
        bbool               m_stickOnWalls;
    };
    ITF_INLINE const Ray_AILookatBehavior_Template * Ray_AILookatBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AILookatBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AILOOKATBEHAVIOR_H_