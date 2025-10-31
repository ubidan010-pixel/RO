#ifndef _ITF_AIBALLISTICSACTION_H_
#define _ITF_AIBALLISTICSACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{

class AIBallisticsAction_Template;
class PhysComponent;
class StickToPolylinePhysComponent;

//------------------------------------------------------------------------------
/// Base action to send an actor to a given target in a given time, or at a
/// given "speed". For now, not using "real" physics, but this would be possible
/// to have an "AIBallisticsPhysAction" as well (send the impulse to physics at
/// startup, check for collision, reaching the target not guaranteed).
/// Reference: http://hyperphysics.phy-astr.gsu.edu/hbase/traj.html
class AIBallisticsAction : public AIAction
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIBallisticsAction,AIAction,3790773157);
    DECLARE_VALIDATE_ACTION()

public:
    AIBallisticsAction();
    virtual ~AIBallisticsAction();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate();
    virtual void onDeactivate();
    virtual void update(f32 _dt);
    virtual void onEvent(Event * _event);
    virtual void updateAnimInputs();

    ITF_INLINE void setTarget(const Vec3d& _target) { m_target = _target; }

protected:
    f32 getDuration() const;

    f32 m_gravity;
    Vec3d m_start;
    Vec3d m_impulse;
    Vec3d m_origin;
    Vec3d m_target;
    bbool m_physWasDisabled;
    PhysComponent* m_physComponent;
    StickToPolylinePhysComponent* m_stickToPolylineComponent;

private:
    ITF_INLINE const AIBallisticsAction_Template* getTemplate() const;

    virtual void setupTrajectory() = 0;

    f32 m_timer;
};

//------------------------------------------------------------------------------
class AIBallisticsAction_Template : public AIAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIBallisticsAction_Template,AIAction_Template,1617132579);
    DECLARE_SERIALIZE();

public:
    AIBallisticsAction_Template();
    ~AIBallisticsAction_Template();

    ITF_INLINE f32 getDuration() const { return m_duration; }
    ITF_INLINE f32 getSpeed() const { return m_speed; }

private:
    f32 m_duration;
    f32 m_speed;
};

//------------------------------------------------------------------------------
ITF_INLINE
const class AIBallisticsAction_Template*
AIBallisticsAction::getTemplate() const
{
    return static_cast<const AIBallisticsAction_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_AIBALLISTICSACTION_H_

