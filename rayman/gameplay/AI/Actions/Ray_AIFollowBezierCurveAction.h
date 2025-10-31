#ifndef _ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_
#define _ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF {

class BezierCurveComponent;
class PhysComponent;
class Ray_AIFollowBezierCurveAction_Template;

//------------------------------------------------------------------------------
class Ray_AIFollowBezierCurveAction : public AIAction
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIFollowBezierCurveAction, AIAction,199603799);
    DECLARE_VALIDATE_ACTION()

public:
    Ray_AIFollowBezierCurveAction();
    virtual ~Ray_AIFollowBezierCurveAction();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void update(f32 _dt);

private:
    ITF_INLINE const Ray_AIFollowBezierCurveAction_Template* getTemplate() const;

    f32 m_speed;
    f32 m_distance;
    PhysComponent* m_physComponent;
    BezierCurveComponent* m_curveComponent;
};

//------------------------------------------------------------------------------
class Ray_AIFollowBezierCurveAction_Template : public AIAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_AIFollowBezierCurveAction_Template,AIAction_Template,1031333392);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(Ray_AIFollowBezierCurveAction);

public:
    Ray_AIFollowBezierCurveAction_Template();
    ~Ray_AIFollowBezierCurveAction_Template(){}

    ITF_INLINE f32 getSpeed() const { return m_speed; }

private:
    f32 m_speed;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_AIFollowBezierCurveAction_Template*
Ray_AIFollowBezierCurveAction::getTemplate() const
{
    return static_cast<const Ray_AIFollowBezierCurveAction_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_

