#ifndef _ITF_AIWALKINDIRACTION_H_
#define _ITF_AIWALKINDIRACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIWalkInDirAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIWalkInDirAction, AIAction,2047489259);

    public:

        AIWalkInDirAction();
        virtual ~AIWalkInDirAction();

        virtual void                            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                            update( f32 _delta );
        virtual void                            onActivate();
        virtual void                            onDeactivate();

        ITF_INLINE void                         setMaxSpeed( f32 _value ) { m_efficiencyMaxSpeed = _value; }
        void                                    setWalkForce( f32 _force ) { m_walkForce = _force; }
        void                                    setWalkLeft( bbool _left ) { m_walkLeft = _left; }
        bbool                                   isWalkingLeft() const { return m_walkLeft; }

    protected:
        ITF_INLINE const class AIWalkInDirAction_Template * getTemplate() const;

        void                                    updateMoveTargetMultiplier( f32 _deltaTime );
        f32                                     calculateEfficiency() const;

        class StickToPolylinePhysComponent*     m_physComponent;
        f32                                     m_moveTargetBlendTimer;
        f32                                     m_currentMoveTargetMultiplier;
        bbool                                   m_walkLeft;

        f32                                     m_walkForce;
        f32                                     m_efficiencyMaxSpeed;

        // if specified, walk for a limited time and exit (otherwise walk forever)
        f32                                     m_walkTime;
        f32                                     m_timer;
    };

    class AIWalkInDirAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIWalkInDirAction_Template,AIAction_Template,3736858575);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIWalkInDirAction);

    public:
        AIWalkInDirAction_Template();
        ~AIWalkInDirAction_Template(){}

        ITF_INLINE f32 getWalkForce() const { return m_walkForce; }
        ITF_INLINE f32 getWalkForceRandomFactor() const { return m_walkForceRandomFactor; }
        ITF_INLINE f32 getEfficiencyMaxSpeed() const { return m_efficiencyMaxSpeed; }
        ITF_INLINE f32 getEfficiencyMaxSpeedRandomFactor() const { return m_efficiencyMaxSpeedRandomFactor; }
        ITF_INLINE Angle getMinGroundAngleEfficiencyAngle() const { return m_minGroundAngleEfficiencyAngle; }
        ITF_INLINE Angle getMaxGroundAngleEfficiencyAngle() const { return m_maxGroundAngleEfficiencyAngle; }
        ITF_INLINE f32 getMinGroundAngleEfficiencyMult() const { return m_minGroundAngleEfficiencyMult; }
        ITF_INLINE f32 getMaxGroundAngleEfficiencyMult() const { return m_maxGroundAngleEfficiencyMult; }
        ITF_INLINE f32 getMoveTargetMultiplierMin() const { return m_moveTargetMultiplierMin; }
        ITF_INLINE f32 getMoveTargetMultiplierMax() const { return m_moveTargetMultiplierMax; }
        ITF_INLINE f32 getMoveTargetMultiplierBlendTime() const { return m_moveTargetMultiplierBlendTime; }
        ITF_INLINE f32 getWalkAnimRate() const { return m_walkAnimRate; }
        ITF_INLINE f32 getMinWalkTime() const { return m_minWalkTime; }
        ITF_INLINE f32 getMaxWalkTime() const { return m_maxWalkTime; }
        ITF_INLINE bbool getWalkLeftFlipsAnim() const { return m_walkLeftFlipsAnim; }
        ITF_INLINE ITF::bbool getUseGroundAngle() const { return m_useGroundAngle; }

    private:
        f32                                     m_walkForce;
        f32                                     m_walkForceRandomFactor;
        f32                                     m_efficiencyMaxSpeed;
        f32                                     m_efficiencyMaxSpeedRandomFactor;
        Angle                                   m_minGroundAngleEfficiencyAngle;
        Angle                                   m_maxGroundAngleEfficiencyAngle;
        f32                                     m_minGroundAngleEfficiencyMult;
        f32                                     m_maxGroundAngleEfficiencyMult;
        f32                                     m_moveTargetMultiplierMin;
        f32                                     m_moveTargetMultiplierMax;
        f32                                     m_moveTargetMultiplierBlendTime;
        f32                                     m_walkAnimRate;
        f32                                     m_minWalkTime;
        f32                                     m_maxWalkTime;
        bbool                                   m_walkLeftFlipsAnim;
        bbool                                   m_useGroundAngle;
    };

    ITF_INLINE const class AIWalkInDirAction_Template * AIWalkInDirAction::getTemplate() const
    {
        return static_cast<const class AIWalkInDirAction_Template *>(m_template);
    }
}

#endif // _ITF_AIWALKINDIRACTION_H_
