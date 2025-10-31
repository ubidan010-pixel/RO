#ifndef _ITF_AIRECEIVEHITACTION_H_
#define _ITF_AIRECEIVEHITACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIReceiveHitAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIReceiveHitAction, AIAction,260052035);

    public:

        virtual void        setData( HitStim* _hit ) = 0;

    };

    class AIReceiveHitAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIReceiveHitAction_Template, AIAction_Template,1731564983)
    };
}

#endif //_ITF_AIRECEIVEHITACTION_H_