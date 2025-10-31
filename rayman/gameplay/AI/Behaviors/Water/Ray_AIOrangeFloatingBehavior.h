#ifndef _ITF_RAY_AIORANGEFLOATINGBEHAVIOR_H_
#define _ITF_RAY_AIORANGEFLOATINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERLOATINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterFloatingBehavior.h"
#endif //_ITF_RAY_AIWATERLOATINGBEHAVIOR_H_

namespace ITF
{
    //forward
    class Ray_AIOrangeFloatingBehavior_Template;

    class Ray_AIOrangeFloatingBehavior : public Ray_AIWaterFloatingBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIOrangeFloatingBehavior, Ray_AIWaterFloatingBehavior,1060894626)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIOrangeFloatingBehavior();
        virtual ~Ray_AIOrangeFloatingBehavior();

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onActivate();
        virtual void update(f32 _dt);
        virtual void onEvent( Event * _event);

    protected:
        virtual void processStickOnPolyline(EventStickOnPolyline * _event);

    private:
        enum OrangeState
        {
            OrangeState_UNDEFINED = -1,
            OrangeState_IDLE = 0,
            OrangeState_MOVING,
            OrangeState_ARRIVAL
        };

        ITF_INLINE const Ray_AIOrangeFloatingBehavior_Template * getTemplate() const;

        //state management
        void changeState(const OrangeState _state, const Vec2d& _param = Vec2d::Zero);
        void updateState(const OrangeState _state, const f32 _dt);

        //idle state
        void updateIdle();

        //moving state
        void startMoving(const Vec2d & _impulsion, const OrangeState _previousState);
        void updateMoving(const f32 _dt);
        void toggleMoveDirection();
        ITF_INLINE bbool isAbleToToggleDirection() const;

        //arrival state
        void updateArrival(const f32 _dt);

        //event
        void processBlockedEvent(const EventBlockedByPolyline * const _event);
        void processOrangeContactEvent(const EventOrangeContact * const _event);

        //compute wanted speed from impulsion x comp
        void computeWantedSpeedAndArrivalFactor(const f32 _x);

        static const f32 TOGGLE_DIRECTION_DELAY;

        f32 m_initialX;
        f32 m_signedWantedSpeed;
        f32 m_previousDistance;
        f32 m_toggleDirectionTimer;
        OrangeState m_orangeState;
    };

    class Ray_AIOrangeFloatingBehavior_Template : public Ray_AIWaterFloatingBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIOrangeFloatingBehavior_Template,Ray_AIWaterFloatingBehavior_Template,46232455);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIOrangeFloatingBehavior);

    public:

        Ray_AIOrangeFloatingBehavior_Template();
        ~Ray_AIOrangeFloatingBehavior_Template();

        ITF_INLINE f32 getOrangeMinSpeed() const;
        ITF_INLINE f32 getOrangeMaxSpeed() const;
        ITF_INLINE f32 getLandingSpeedXThreshold() const;
        ITF_INLINE f32 getArrivalDistance() const;
        ITF_INLINE f32 getMoveThreshold() const;

    private:
        f32 m_orangeMinSpeed;
        f32 m_orangeMaxSpeed;
        f32 m_landingSpeedXThreshold;
        f32 m_arrivalDistance;
        f32 m_moveThreshold;
    };

    ITF_INLINE const Ray_AIOrangeFloatingBehavior_Template * Ray_AIOrangeFloatingBehavior::getTemplate() const
    {
        return static_cast<const Ray_AIOrangeFloatingBehavior_Template *>(m_template);
    }

    ITF_INLINE bbool Ray_AIOrangeFloatingBehavior::isAbleToToggleDirection() const
    {
        return (m_orangeState == OrangeState_MOVING) && (m_toggleDirectionTimer > Ray_AIOrangeFloatingBehavior::TOGGLE_DIRECTION_DELAY);
    }


    ITF_INLINE f32 Ray_AIOrangeFloatingBehavior_Template::getOrangeMinSpeed() const
    {
        return m_orangeMinSpeed;
    }

    ITF_INLINE f32 Ray_AIOrangeFloatingBehavior_Template::getOrangeMaxSpeed() const
    {
        return m_orangeMaxSpeed;
    }

    ITF_INLINE f32 Ray_AIOrangeFloatingBehavior_Template::getLandingSpeedXThreshold() const
    {
        return m_landingSpeedXThreshold;
    }

    ITF_INLINE f32 Ray_AIOrangeFloatingBehavior_Template::getArrivalDistance() const
    {
        return m_arrivalDistance;
    }

    ITF_INLINE f32 Ray_AIOrangeFloatingBehavior_Template::getMoveThreshold() const
    {
        return m_moveThreshold;
    }
}

#endif //_ITF_RAY_AIORANGEFLOATINGBEHAVIOR_H_

