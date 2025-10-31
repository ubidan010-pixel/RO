#ifndef _ITF_RAY_AICORNFLOATACTION_H_
#define _ITF_RAY_AICORNFLOATACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF {

    class PhysComponent;
    class PolylineComponent;
    class Ray_SubAnchorComponent;

    class Ray_AICornFloatAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AICornFloatAction, AIAction,2294331675);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_ACTION()

    public:
        Ray_AICornFloatAction();
        virtual ~Ray_AICornFloatAction();

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onActivate();
        virtual void update(f32 _dt);
        virtual void updateAnimInputs();

    private:
        ITF_INLINE const class Ray_AICornFloatAction_Template * getTemplate() const;

        bbool m_crouched;
        bbool m_lowerPassed;
        f32 m_floatCountdown;
        f32 m_floatInterval;
        f32 m_floatHeight;

        PhysComponent* m_physComponent;
        PolylineComponent* m_polylineComponent;
        Ray_SubAnchorComponent* m_subAnchorComponent;
    };

    class Ray_AICornFloatAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AICornFloatAction_Template,AIAction_Template,3649626779);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AICornFloatAction);

    public:

        Ray_AICornFloatAction_Template();
        ~Ray_AICornFloatAction_Template(){}

        ITF_INLINE f32 getFloatIntervalMin() const { return m_floatIntervalMin; }
        ITF_INLINE f32 getFloatIntervalMax() const { return m_floatIntervalMax; }
        ITF_INLINE f32 getFloatHeightMin() const { return m_floatHeightMin; }
        ITF_INLINE f32 getFloatHeightMax() const { return m_floatHeightMax; }
        
        ITF_INLINE f32 getSpringConstant() const { return m_springConstant; }
        ITF_INLINE f32 getFrictionConstant() const { return m_frictionConstant; }
        ITF_INLINE f32 getWeightThreshold() const { return m_weightThreshold; }
        
        ITF_INLINE f32 getLowerSpringConstant() const { return m_lowerSpringConstant; }
        ITF_INLINE f32 getLowerFrictionConstant() const { return m_lowerFrictionConstant; }

    private:
        f32 m_floatHeightMin;
        f32 m_floatHeightMax;
        f32 m_floatIntervalMin;
        f32 m_floatIntervalMax;
        f32 m_springConstant;
        f32 m_frictionConstant;
        f32 m_weightThreshold;
        f32 m_lowerSpringConstant;
        f32 m_lowerFrictionConstant;
    };

    ITF_INLINE const class Ray_AICornFloatAction_Template * Ray_AICornFloatAction::getTemplate() const
    {
        return static_cast<const class Ray_AICornFloatAction_Template *>(m_template);
    }

} // namespace ITF

#endif //_ITF_RAY_AICORNFLOATACTION_H_
