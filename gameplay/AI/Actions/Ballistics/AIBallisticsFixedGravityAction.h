#ifndef _ITF_AIBALLISTICSFIXEDGRAVITYACTION_H_
#define _ITF_AIBALLISTICSFIXEDGRAVITYACTION_H_

#ifndef _ITF_AIBALLISTICSACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsAction.h"
#endif //_ITF_AIBALLISTICSACTION_H_

namespace ITF
{

class AIBallisticsFixedGravityAction_Template;

//------------------------------------------------------------------------------
/// Use fixed gravity to determine impulse. Optionally, gravity can be retrieved
/// from a PhysComponent.
class AIBallisticsFixedGravityAction : public AIBallisticsAction
{
    DECLARE_OBJECT_CHILD_RTTI(AIBallisticsFixedGravityAction,AIBallisticsAction,1463863728);
    DECLARE_VALIDATE_ACTION()

public:
    AIBallisticsFixedGravityAction();
    virtual ~AIBallisticsFixedGravityAction();

private:
    ITF_INLINE const AIBallisticsFixedGravityAction_Template* getTemplate() const;

    virtual void setupTrajectory();
};

//------------------------------------------------------------------------------
class AIBallisticsFixedGravityAction_Template : public AIBallisticsAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AIBallisticsFixedGravityAction_Template,AIBallisticsAction_Template,837751950);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(AIBallisticsFixedGravityAction);

public:
    AIBallisticsFixedGravityAction_Template();
    ~AIBallisticsFixedGravityAction_Template();

    bbool getUsePhysicsGravity() const { return m_usePhysicsGravity; }
    f32 getFixedGravity() const { return m_fixedGravity; }

private:
    bbool m_usePhysicsGravity;
    f32 m_fixedGravity;
};

//------------------------------------------------------------------------------
ITF_INLINE
const class AIBallisticsFixedGravityAction_Template*
AIBallisticsFixedGravityAction::getTemplate() const
{
    return static_cast<const AIBallisticsFixedGravityAction_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_AIBALLISTICSFIXEDGRAVITYACTION_H_
