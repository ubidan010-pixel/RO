#ifndef _ITF_WAR_AIWAITFORATTACKWINDOWACTION_H_
#define _ITF_WAR_AIWAITFORATTACKWINDOWACTION_H_

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

namespace ITF
{
    class War_AIWaitForAttackWindowAction : public AIIdleAction
    {
        DECLARE_OBJECT_CHILD_RTTI(War_AIWaitForAttackWindowAction, AIIdleAction,3921129388)
        DECLARE_SERIALIZE()

    public:

        War_AIWaitForAttackWindowAction();
        virtual ~War_AIWaitForAttackWindowAction();

        virtual void            onActivate();
        virtual void            onEvent( Event* _event);
        virtual void            update( f32 _delta );

                void            setTarget( class War_AITargetComponent* _target ) { m_target = _target; }
    private:

        War_AITargetComponent*  m_target;
        bbool                   m_attackRequested;
        bbool                   m_attackGranted;
    };

}

#endif // _ITF_WAR_AIWAITFORATTACKWINDOWACTION_H_

