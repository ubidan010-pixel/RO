#ifndef _ITF_RAY_AIWATERLOATINGBEHAVIOR_H_
#define _ITF_RAY_AIWATERLOATINGBEHAVIOR_H_

#ifndef _ITF_RAY_AIWATERBASEBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Water/Ray_AIWaterBaseBehavior.h"
#endif //_ITF_RAY_AIWATERBASEBEHAVIOR_H_

namespace ITF
{
    class Ray_AIWaterFloatingBehavior : public Ray_AIWaterBaseBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIWaterFloatingBehavior, Ray_AIWaterBaseBehavior,3809035055)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIWaterFloatingBehavior();
        virtual ~Ray_AIWaterFloatingBehavior();

        virtual void                            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                            update( f32 _delta  );
        virtual void                            onActivate();
        virtual void                            onDeactivate();
        virtual void                            onEvent( Event * _event);

    protected:
        virtual void                            processBlockingEvent( class EventPolylineBlockingContact* _pushEvent );
        virtual void                            processStickOnPolyline( class EventStickOnPolyline* _stickEvent );

    private:

        ITF_INLINE const class Ray_AIWaterFloatingBehavior_Template * getTemplate() const;


    protected:
        class StickToPolylinePhysComponent*     m_physComponent;

    private:
        class Ray_AIWaterFloatAction*           m_float;
    };

    class Ray_AIWaterFloatingBehavior_Template : public Ray_AIWaterBaseBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIWaterFloatingBehavior_Template,Ray_AIWaterBaseBehavior_Template,4149133277);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIWaterFloatingBehavior);

    public:

        Ray_AIWaterFloatingBehavior_Template();
        ~Ray_AIWaterFloatingBehavior_Template();

        const class Ray_AIWaterFloatAction_Template*    getFloat() const { return m_float; }
        f32                                             getPushMinSpeed() const { return m_pushMinSpeed; }
        f32                                             getPushMaxSpeed() const { return m_pushMaxSpeed; }
        f32                                             getPushMinForce() const { return m_pushMinForce; }
        f32                                             getPushMaxForce() const { return m_pushMaxForce; }
        
        f32                                             getPushSpeedScaleMin() const { return m_pushSpeedScaleMin; }
        f32                                             getPushSpeedScaleMax() const { return m_pushSpeedScaleMax; }

        f32                                             getLandForceMultiplier() const { return m_landForceMultiplier; }
        f32                                             getLandXForceMultiplier() const { return m_landXForceMultiplier; }

    private:

        class Ray_AIWaterFloatAction_Template*          m_float;
        f32                                             m_pushMinSpeed;
        f32                                             m_pushMaxSpeed;
        f32                                             m_pushMinForce;
        f32                                             m_pushMaxForce;

        f32                                             m_pushSpeedScaleMin;
        f32                                             m_pushSpeedScaleMax;

        f32                                             m_landForceMultiplier;
        f32                                             m_landXForceMultiplier;

    };

    ITF_INLINE const class Ray_AIWaterFloatingBehavior_Template * Ray_AIWaterFloatingBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIWaterFloatingBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIWATERLOATINGBEHAVIOR_H_

