#ifndef _ITF_BTACTIONJUMPTOTARGET_H_
#define _ITF_BTACTIONJUMPTOTARGET_H_

#ifndef _ITF_BTACTION_H_
#include "gameplay/ai/BTs/BTAction.h"
#endif // _ITF_BTACTION_H_

namespace ITF
{

class BTActionJumpToTarget : public BTAction
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionJumpToTarget,BTAction,2367817946);

public:

    BTActionJumpToTarget();
    ~BTActionJumpToTarget() {}

    virtual void                                init( class BehaviorTree* _tree, const class BTNode_Template* _template );
    virtual bbool                               validate( Actor* _actor ) const;
    bbool                                       decide( f32 _dt );
    virtual void                                onActivate();
    virtual bbool                               update( f32 _dt );

private:

    Vec3d                                       getTargetPos() const;
    const class BTActionJumpToTarget_Template*  getTemplate() const;

    class GroundAIControllerComponent*          m_aiController;
    f32                                         m_jumpCounter;
    Vec3d                                       m_prevTarget;
    Vec2d                                       m_prevPos;
};

class BTActionJumpToTarget_Template : public BTAction_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTActionJumpToTarget_Template,BTAction_Template,1525238737);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTActionJumpToTarget);

public:

    BTActionJumpToTarget_Template();
    ~BTActionJumpToTarget_Template() {}

    const StringID&             getAnim() const { return m_anim; }
    const StringID&             getFactTargetActor() const { return m_factTargetActor; }
    const StringID&             getFactTargetPos() const { return m_factTargetPos; }

private:

    StringID                    m_anim;
    StringID                    m_factTargetActor;
    StringID                    m_factTargetPos;
};

ITF_INLINE const BTActionJumpToTarget_Template*  BTActionJumpToTarget::getTemplate() const {return static_cast<const BTActionJumpToTarget_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTIONJUMPTOTARGET_H_