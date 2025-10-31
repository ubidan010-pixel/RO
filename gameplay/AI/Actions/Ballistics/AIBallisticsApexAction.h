#ifndef _ITF_AIBALLISTICSAPEXACTION_H_
#define _ITF_AIBALLISTICSAPEXACTION_H_

#ifndef _ITF_AIBALLISTICSACTION_H_
#include "gameplay/ai/Actions/Ballistics/AIBallisticsAction.h"
#endif //_ITF_AIBALLISTICSACTION_H_

namespace ITF
{

class AIBallisticsApexAction_Template;

//------------------------------------------------------------------------------
/// Target = apex of the ballistic curve.
class AIBallisticsApexAction : public AIBallisticsAction
{
    DECLARE_OBJECT_CHILD_RTTI(AIBallisticsApexAction, AIBallisticsAction,211358248);
    DECLARE_VALIDATE_ACTION()

public:
    AIBallisticsApexAction();
    virtual ~AIBallisticsApexAction();

private:
    ITF_INLINE const AIBallisticsApexAction_Template* getTemplate() const;

    virtual void setupTrajectory();
};

//------------------------------------------------------------------------------
class AIBallisticsApexAction_Template : public AIBallisticsAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AIBallisticsApexAction_Template,AIBallisticsAction_Template,615420746);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(AIBallisticsApexAction);

public:
    AIBallisticsApexAction_Template();
    ~AIBallisticsApexAction_Template();
};

//------------------------------------------------------------------------------
ITF_INLINE
const class AIBallisticsApexAction_Template*
AIBallisticsApexAction::getTemplate() const
{
    return static_cast<const AIBallisticsApexAction_Template*>(m_template);
}

} // namespace ITF

#endif // _ITF_AIBALLISTICSAPEXACTION_H_
