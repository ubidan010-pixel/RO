#ifndef _ITF_WAR_AIPERFORMHITSTIMACTION_H_
#define _ITF_WAR_AIPERFORMHITSTIMACTION_H_

#ifndef _ITF_WAR_AIPERFORMHITBASEACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitBaseAction.h"
#endif //_ITF_WAR_AIPERFORMHITBASEACTION_H_

namespace ITF
{
    class War_AIPerformHitStimAction : public War_AIPerformHitBaseAction
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIPerformHitStimAction, War_AIPerformHitBaseAction,582620550)
        DECLARE_SERIALIZE()

    public:

        War_AIPerformHitStimAction();
        virtual ~War_AIPerformHitStimAction();

    protected:
        virtual void        performHit( class AnimGameplayEvent* _animEvent );

    private:
        void                clear();
        PhysShape*          m_stimShape;
    };

}

#endif //_ITF_WAR_AIPERFORMHITSTIMACTION_H_

