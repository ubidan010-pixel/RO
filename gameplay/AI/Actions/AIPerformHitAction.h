#ifndef _ITF_AIPERFORMHITACTION_H_
#define _ITF_AIPERFORMHITACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class AIPerformHitAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(AIPerformHitAction,AIAction,2299060822);

    public:
        AIPerformHitAction() : m_targetVictim(ITF_INVALID_OBJREF) {}
        void                setData(ObjectRef _victim) { m_targetVictim = _victim; }
        virtual bbool       isValid(Actor * _actor, Actor * _target) const
        {
            return btrue;
        }
    protected:

        ObjectRef           m_targetVictim;

    };
    class AIPerformHitAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(AIPerformHitAction_Template,AIAction_Template,4146815387);

    public:

    private:

    };
}

#endif //_ITF_AIPERFORMHITACTION_H_

