#ifndef _ITF_RAY_AIDARKTOONIFICATIONACTION_H_
#define _ITF_RAY_AIDARKTOONIFICATIONACTION_H_

#ifndef _ITF_RAY_AIPERFORMHITPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPUNCHACTION_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

namespace ITF
{
    class StickToPolylinePhysComponent;

    class Ray_AIDarktoonificationAction : public Ray_AIPerformHitPunchAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonificationAction, Ray_AIPerformHitPunchAction,31283770);

    public:

        Ray_AIDarktoonificationAction();

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActivate();
        virtual void        onDeactivate();
        virtual void        update( f32 _delta );
        virtual void        onEvent( Event* _event );
        virtual void        updateAnimInputs();
        void        setVictimPos(const Vec2d & _pos) { m_victimPos = _pos;m_victimPosSet = btrue;}
        virtual bbool       isValid(Actor * _actor, Actor * _target) const;
    private:
        ITF_INLINE const class Ray_AIDarktoonificationAction_Template * getTemplate() const;

        void jump();
        void updateJump( f32 _dt );

        void processAnimEvent( AnimGameplayEvent* _animEvent );
        void onDarktoonify( EventHitSuccessful* _eventDT);
        void onUndarktoonify( Ray_EventUndarktoonify* _eventUnDT );

        StickToPolylinePhysComponent* m_physComponent;

        f32 m_trajectoryTimer;

        f32 m_previousGravityMultiplier;
        bbool m_justEntered;

        Vec2d m_lastPosition;
        bbool m_hasJumped;
        f32 m_time;
        bbool m_hitPerformed;
        bbool m_falling;
        bbool m_landed;
        Vec2d m_vecToVictim;
        Vec2d m_victimPos;
        bbool m_victimPosSet;
        bbool m_hitVictim;
    };
    class Ray_AIDarktoonificationAction_Template : public Ray_AIPerformHitPunchAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIDarktoonificationAction_Template,Ray_AIPerformHitPunchAction_Template,347652341);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIDarktoonificationAction);

    public:

        Ray_AIDarktoonificationAction_Template();
        ~Ray_AIDarktoonificationAction_Template(){}

        ITF_INLINE f32 getTrajectoryDuration() const { return m_trajectoryDuration; }
        ITF_INLINE f32 getGravityMultiplier() const { return m_gravityMultiplier; }
        ITF_INLINE f32 getGravityMultiplierAfterFail() const { return m_gravityMultiplierAfterFail; }
        ITF_INLINE f32 getInitialSpeedAfterFail() const { return m_initialSpeedAfterFail; }
        ITF_INLINE f32 getTangentPhaseLimit() const { return m_tangentPhaseLimit; }
        ITF_INLINE f32 getMaxTargetHeightDifference() const { return m_maxTargetHeightDifference; }
        ITF_INLINE f32 getMaxTargetDistance() const { return m_maxTargetDistance; }

    private:
        f32 m_trajectoryDuration;
        f32 m_gravityMultiplier;
        f32 m_gravityMultiplierAfterFail;
        f32 m_initialSpeedAfterFail;
        f32 m_tangentPhaseLimit;
        f32 m_maxTargetHeightDifference;
        f32 m_maxTargetDistance;
    };
    ITF_INLINE const class Ray_AIDarktoonificationAction_Template * Ray_AIDarktoonificationAction::getTemplate() const
    {
        return static_cast<const class Ray_AIDarktoonificationAction_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIDARKTOONIFICATIONACTION_H_

