#ifndef _ITF_RAY_AIJANODROAMINGBEHAVIOR_H_
#define _ITF_RAY_AIJANODROAMINGBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Fruit/Ray_AIFruitRoamingBehavior.h"
#endif //_ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

#ifndef __RAY_AIJANODROAMINGBASEBEHAVIOR_H__
#include "rayman/gameplay/ai/Behaviors/Janod/Ray_AIJanodRoamingBaseBehavior.h"
#endif //__RAY_AIJANODROAMINGBASEBEHAVIOR_H__

#ifndef _ITF_RAY_AIBOUNCEUTILITY_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIBounceUtility.h"
#endif

namespace ITF
{
    class StickToPolylinePhysComponent;
    class AIPlayAnimAction;

    class Ray_AIJanodRoamingBehavior : public Ray_AIJanodRoamingBaseBehavior, public Ray_AIBounceUtility
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIJanodRoamingBehavior, AIBehavior,473575231);
        //DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIJanodRoamingBehavior();
        virtual ~Ray_AIJanodRoamingBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        update( f32 _delta  );
        virtual void                        onActionFinished();

    private:
        ITF_INLINE const class Ray_AIJanodRoamingBehavior_Template * getTemplate() const;

        enum State_t 
        {
            State_NA=0, 
            State_WaitTrigger, //this state is followed by waitdelay
            State_WaitDelayBeforeStarting,
            State_PreJump, 
            State_ReactingToTinyCrush,
            State_JumpOrFall, 
            State_Bristling, 
            State_FloatingBeforeFalling_Part1, 
            State_Sleeps,
            State_Count
        };

        virtual Vec2d bounceUtility_getBounceForce( Vec2d _edgeDir);
        void onEvent(Event * _event);
        void onActivate();
        void onDeactivate();
        bbool checkIfSwitchToBristle(bbool _affectBristlingPoint);
        void setState(State_t _state, const void *_params=NULL);
        void resetState();
        void updateJumpOrFall();        
        Actor *getVictim() const;
        const StringID &getCurrentAnim() const {return m_currentAnim;}
        void setCurrentAnim(const StringID &_newAnim);
        void setOutOfFloatSpeed();
        void reactToOtherJanods();
        void bounceUtility_UpdateJumpOrFall_onIsOnGround(const Vec2d &_normalizedEdge, bbool isFromActor);
        void bounceUtility_UpdateJumpOrFall_onNearTopOfJump();
        void mayReachForcedX();
        f32 computeJumpDuration(); //must be called in cycle-based janods
        f32 computeGravityMultiplierForCycleBased();
        void switchToStartState();

        Vec2d m_apexPoint, m_lastBristleContact;
        StringID m_currentAnim;
        bbool m_delayedSetPosAtApex;
        f32 m_jumpFallAnimPlayRate;
        f32 m_computedBounceForce;
        f32 m_normalGravityMultiplier;
        bbool m_signaledTinyCrush;


        State_t m_state;
        f32 m_timeInCurrentState;
        Vec2d m_previousSpeed;
        bbool m_bouncingOnWall;
        Vec2d m_bouncingOnWall_NextSpeed;
        Vec2d m_edgeWhenEnteredPreJump;
        StickToPolylinePhysComponent *m_physComponent;
    };

    class Ray_AIJanodRoamingBehavior_Template : public Ray_AIJanodRoamingBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIJanodRoamingBehavior_Template,Ray_AIJanodRoamingBaseBehavior_Template,1071796122);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIJanodRoamingBehavior);

    public:

        Ray_AIJanodRoamingBehavior_Template();
        ~Ray_AIJanodRoamingBehavior_Template() {}

        f32                 getSideForce() const { return m_sideForce; }
        f32                 getDetectionDistance() const { return m_detectionDistance; }
        f32                 getDelayBeforeHurts() const { return m_delayBeforeHurts; }
        f32                 getStartToFloatTime() const { return m_startToFloatTime; }
        f32                 getFloatTimeBeforeFalling() const { return m_floatTimeBeforeFalling; }
        f32                 getSpeedThatTriggersFloating() const { return m_speedThatTriggersFloating; }
        f32                 getBristlingAirMultiplier() const { return m_bristingAirMultiplier; }
        f32                 getFloatConstantForceDuration() const { return m_floatConstantForceDuration; }
        f32                 getBristlingConstantForceDuration() const { return m_bristlingConstantForceDuration; }
        f32                 getFloatMaxForce() const { return m_floatMaxForce; }
        f32                 getFallToVictimInitialSpeed() const { return m_fallToVictimInitialSpeed; }
        bbool               getMustTrackVictim() const { return m_mustTrackVictim; }
        f32                 getStimFeedback() const { return m_stimFeedback; }
        f32                 getStimFeedbackDist() const { return m_stimFeedbackDist; }
        f32                 getInterJanodForce() const { return m_interJanodForce; }
        const Angle&        getMaxBounceAngleFromVertical() const {return m_maxBounceAngleFromVertical;}
        const Angle&        getSmallAngleForBounceOnlyForce() const {return m_smallAngleForBounceOnlyForce;}
        bbool               getIsAsleep() const {return m_isAsleep;}
        bbool               getHasConstantX() const {return m_hasConstantX;}
        f32                 getHasContantCoordinate_Force() const {return m_hasContantCoordinate_Force;}
        f32                 getApexTypicalHeight() const {return m_apexTypicalHeight;}
        bbool               getIsCycleBased() const {return m_isCycleBased;}
        f32                 getTotalCycleDuration() const {return m_totalCycleDuration;}
        f32                 getAnticipationAnimationDuration() const {return m_anticipationAnimationDuration;}

        f32                 getDurationForNormalFallAnimSpeed() const {return m_durationForNormalFallAnimSpeed;}
        f32                 getMinFallAnimSpeedRatio() const {return m_minFallAnimSpeedRatio;}
        f32                 getMaxFallAnimSpeedRatio() const {return m_maxFallAnimSpeedRatio;}

        f32                 getFreeFallMinSpeed() const {return m_freeFallMinSpeed;}

    private:

        f32 m_detectionDistance;
        f32 m_delayBeforeHurts;
        f32 m_startToFloatTime;
        f32 m_floatTimeBeforeFalling;
        f32 m_speedThatTriggersFloating;
        f32 m_bristingAirMultiplier;
        f32 m_floatConstantForceDuration;
        f32 m_bristlingConstantForceDuration;
        f32 m_floatMaxForce;
        f32 m_fallToVictimInitialSpeed;
        bbool m_mustTrackVictim;
        f32 m_stimFeedback, m_stimFeedbackDist;
        f32 m_interJanodForce;
        f32 m_sideForce;
        Angle m_maxBounceAngleFromVertical;
        Angle m_smallAngleForBounceOnlyForce;
        bbool m_isAsleep;
        bbool m_hasConstantX;
        f32 m_hasContantCoordinate_Force;
        f32 m_apexTypicalHeight;
        bbool m_isCycleBased;
        f32 m_totalCycleDuration;
        f32 m_anticipationAnimationDuration;
        f32 m_durationForNormalFallAnimSpeed;
        f32 m_minFallAnimSpeedRatio;
        f32 m_maxFallAnimSpeedRatio;
        f32 m_freeFallMinSpeed;
    };

    ITF_INLINE const class Ray_AIJanodRoamingBehavior_Template * Ray_AIJanodRoamingBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIJanodRoamingBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIJANODROAMINGBEHAVIOR_H_

