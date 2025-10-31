#ifndef _ITF_BTACTION_H_
#define _ITF_BTACTION_H_

#ifndef _ITF_BTNODE_H_
#include "gameplay/ai/BTs/BTNode.h"
#endif // _ITF_BTNODE_H_

namespace ITF
{

class BTAction : public BTNode
{
    DECLARE_OBJECT_CHILD_RTTI(BTAction,BTNode,744714947);

public:

    BTAction();
    ~BTAction() {}

private:

    const class BTAction_Template* getTemplate() const;
};

class BTAction_Template : public BTNode_Template
{
    DECLARE_OBJECT_CHILD_RTTI(BTAction_Template,BTNode_Template,2337411787);
    DECLARE_SERIALIZE();
    DECLARE_BTNODE_TEMPLATE(BTAction);

public:

    BTAction_Template() {}
    ~BTAction_Template() {}

private:

};

ITF_INLINE const BTAction_Template*  BTAction::getTemplate() const {return static_cast<const BTAction_Template*>(m_template);}

} // namespace ITF

#endif // _ITF_BTACTION_H_