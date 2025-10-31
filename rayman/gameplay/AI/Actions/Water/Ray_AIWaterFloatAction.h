#ifndef _ITF_RAY_AIWATERFLOATACTION_H_
#define _ITF_RAY_AIWATERFLOATACTION_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

namespace ITF
{
    class Ray_AIWaterFloatAction : public AIPlayAnimAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIWaterFloatAction, AIPlayAnimAction,3288751797);

    public:

        Ray_AIWaterFloatAction();
        virtual ~Ray_AIWaterFloatAction();

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onActivate();
        virtual void    update( f32 _dt );
        virtual void    validate( bbool& _isActionValidated );
        virtual void    onEvent( Event * _event );

    private:

        ITF_INLINE const class Ray_AIWaterFloatAction_Template* getTemplate() const;

        void            processStick( EventStickOnPolyline* _stickEvent );

        class Ray_AIWaterBaseBehavior*          m_waterBehavior;
        StickToPolylinePhysComponent*           m_physComponent;
        f32                                     m_orientSpeed;
        Vec2d                                   m_forces;
    };

    class Ray_AIWaterFloatAction_Template : public AIPlayAnimAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIWaterFloatAction_Template,AIPlayAnimAction_Template,1089754944);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AIWaterFloatAction);

    public:

        Ray_AIWaterFloatAction_Template();
        ~Ray_AIWaterFloatAction_Template(){}

        f32                                     getFloatForceDistMult() const { return m_floatForceDistMult; }
        f32                                     getFloatForceSpeedMult() const { return m_floatForceSpeedMult; }
        f32                                     getOrientForceDistMult() const { return m_orientForceDistMult; }
        f32                                     getOrientForceSpeedMult() const { return m_orientForceSpeedMult; }
        f32                                     getWeightMultiplier() const { return m_weightMultiplier; }
        f32                                     getSpeedMultiplier() const { return m_speedMultiplier; }

    private:

        f32                                     m_floatForceDistMult;
        f32                                     m_floatForceSpeedMult;
        f32                                     m_orientForceDistMult;
        f32                                     m_orientForceSpeedMult;
        f32                                     m_weightMultiplier;
        f32                                     m_speedMultiplier;
    };

    ITF_INLINE const class Ray_AIWaterFloatAction_Template* Ray_AIWaterFloatAction::getTemplate() const { return static_cast<const Ray_AIWaterFloatAction_Template*>(m_template); }
}

#endif // _ITF_RAY_AIWATERFLOATACTION_H_