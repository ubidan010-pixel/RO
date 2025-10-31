#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#define _ITF_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    class AIReceiveHitBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIReceiveHitBehavior, AIBehavior,3669985857);

    public:

        virtual void        setData( HitStim* _hit ) = 0;
        virtual bbool       canReceiveHit(HitStim * _hit) = 0;

    };

    class AIReceiveHitBehavior_Template : public TemplateAIBehavior
    {
    };
}

#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_