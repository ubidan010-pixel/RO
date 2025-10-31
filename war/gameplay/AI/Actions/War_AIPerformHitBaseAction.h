#ifndef _ITF_WAR_AIPERFORMHITBASEACTION_H_
#define _ITF_WAR_AIPERFORMHITBASEACTION_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

namespace ITF
{
    class War_AIPerformHitBaseAction : public AIPlayAnimAction
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(War_AIPerformHitBaseAction, AIPlayAnimAction,1296144142)

    public:

        War_AIPerformHitBaseAction();
        virtual ~War_AIPerformHitBaseAction();

        virtual void        onEvent( Event* _event);
                void        setTarget( class War_AITargetComponent* _target ) { m_target = _target; }

    protected:
        virtual void        performHit( class AnimGameplayEvent* _animEvent ) = 0;

        War_AITargetComponent*  m_target;
    };

}

#endif //_ITF_WAR_AIPERFORMHITBASEACTION_H_

