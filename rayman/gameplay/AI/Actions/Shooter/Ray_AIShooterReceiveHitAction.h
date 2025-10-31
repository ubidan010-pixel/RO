#ifndef _ITF_RAY_AISHOOTERRECEIVEHITACTION_H_
#define _ITF_RAY_AISHOOTERRECEIVEHITACTION_H_

#ifndef _ITF_RAY_AIRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIReceiveHitAction.h"
#endif //_ITF_RAY_AIRECEIVEHITACTION_H_

namespace ITF
{
    class Ray_AIShooterReceiveHitAction : public Ray_AIReceiveHitAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterReceiveHitAction, Ray_AIReceiveHitAction,2305689383);

    public:

        Ray_AIShooterReceiveHitAction();
        virtual ~Ray_AIShooterReceiveHitAction();

        virtual void                        onActivate();
        virtual void                        update( f32 _dt );
        virtual void                        updateAnimInputs();

    private:

        ITF_INLINE const class Ray_AIShooterReceiveHitAction_Template* getTemplate() const;

        void                                setupHit();
        void                                setNormalHit();
        void                                setupNormalHitReward();

        f32                                 m_timeCounter;
        bbool                               m_stunned;
    };

    class Ray_AIShooterReceiveHitAction_Template : public Ray_AIReceiveHitAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIShooterReceiveHitAction_Template,Ray_AIReceiveHitAction_Template,656935159);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIShooterReceiveHitAction);

    public:

        Ray_AIShooterReceiveHitAction_Template();
        ~Ray_AIShooterReceiveHitAction_Template() {}

        ITF_INLINE f32                      getMinStunTime() const { return m_minStunTime; }
        ITF_INLINE f32                      getMaxStunTime() const { return m_maxStunTime; }
        ITF_INLINE f32                      getLoopingAnimDuration() const { return m_loopingAnimDuration; }
        ITF_INLINE const StringID &         getStunAnimation() const { return m_stunAnimation; }
        

    private:

        f32                                 m_minStunTime;
        f32                                 m_maxStunTime;
        f32                                 m_loopingAnimDuration;
        StringID                            m_stunAnimation;
    };

    ITF_INLINE const class Ray_AIShooterReceiveHitAction_Template* Ray_AIShooterReceiveHitAction::getTemplate() const { return static_cast<const Ray_AIShooterReceiveHitAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AISHOOTERRECEIVEHITACTION_H_

