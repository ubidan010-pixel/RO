#ifndef _ITF_AIDISABLEACTION_H_
#define _ITF_AIDISABLEACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIDisableAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIDisableAction, AIAction,2100523428);

    public:
        AIDisableAction(){}
        virtual void        onActivate();
    };

    class AIDisableAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIDisableAction_Template,AIAction_Template,1941849167);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIDisableAction);

    public:

        AIDisableAction_Template(){};
        ~AIDisableAction_Template(){};
    };
}

#endif // _ITF_AIDISABLEACTION_H_

