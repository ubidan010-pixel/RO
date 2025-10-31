

#ifndef _ITF_AIUTURNACTION_H_
#define _ITF_AIUTURNACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{

    class AIUTurnAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIUTurnAction, AIAction,228338847);
        DECLARE_SERIALIZE()

    public:

        AIUTurnAction();
        virtual ~AIUTurnAction();

        virtual void update( f32 _delta );
        virtual void onActivate();
    private:
        bbool m_justEntered;

    };
    class AIUTurnAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIUTurnAction_Template,AIAction_Template,1794956752);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIUTurnAction);

    public:

        AIUTurnAction_Template();
        ~AIUTurnAction_Template(){}

    private:
    };
}

#endif // _ITF_AIUTURNACTION_H_

