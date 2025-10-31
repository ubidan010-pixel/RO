#ifndef _ITF_AIJUMPANGLEACTION_H_
#define _ITF_AIJUMPANGLEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{

class AIJumpAngleAction_Template;
class PhysComponent;

//------------------------------------------------------------------------------
/// Same as AIJumpInDirAction, except the direction is set via code, not data
class AIJumpAngleAction : public AIAction
{
    DECLARE_OBJECT_CHILD_RTTI(AIJumpAngleAction, AIAction,3963969904);
    DECLARE_VALIDATE_ACTION()

public:

    AIJumpAngleAction();
    virtual ~AIJumpAngleAction();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void onActivate();
    virtual void update(f32 _dt );

    ITF_INLINE void setJumpAngle(f32 _val) { m_jumpAngle = _val; }

private:
    ITF_INLINE const AIJumpAngleAction_Template* getTemplate() const;

    StickToPolylinePhysComponent* m_physComponent;
    bbool m_justStarted;
    f32 m_jumpAngle;
};

//------------------------------------------------------------------------------
class AIJumpAngleAction_Template : public AIAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(AIJumpAngleAction_Template,AIAction_Template,3098797069);
    DECLARE_SERIALIZE();
    DECLARE_AIACTION_TEMPLATE(AIJumpAngleAction);

public:
    AIJumpAngleAction_Template();
    ~AIJumpAngleAction_Template();

    ITF_INLINE f32 getJumpForce() const { return m_jumpForce; }

private:
    f32 m_jumpForce;
};

//------------------------------------------------------------------------------
ITF_INLINE
const AIJumpAngleAction_Template*
AIJumpAngleAction::getTemplate() const
{
    return static_cast<const AIJumpAngleAction_Template *>(m_template);
}
    
} // namespace ITF

#endif // _ITF_AIJUMPANGLEACTION_H_

