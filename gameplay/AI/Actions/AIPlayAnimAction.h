#ifndef _ITF_AIPLAYANIMACTION_H_
#define _ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIPlayAnimAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIPlayAnimAction, AIAction,776382385);
        DECLARE_SERIALIZE()

    public:

        AIPlayAnimAction();
        virtual ~AIPlayAnimAction();

        virtual void      update( f32 _delta );

    };

    class AIPlayAnimAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(AIPlayAnimAction_Template,AIAction_Template,3016991921);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(AIPlayAnimAction);

    public:

        AIPlayAnimAction_Template(){}
        ~AIPlayAnimAction_Template(){}

    private:
    };
}

#endif // _ITF_AIPLAYANIMACTION_H_

