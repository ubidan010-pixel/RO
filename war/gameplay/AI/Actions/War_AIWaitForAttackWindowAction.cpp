#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIWAITFORATTACKWINDOWACTION_H_
#include "war/gameplay/AI/Actions/War_AIWaitForAttackWindowAction.h"
#endif //_ITF_WAR_AIWAITFORATTACKWINDOWACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_WAR_AITARGETCOMPONENT_H_
#include "war/gameplay/Components/Misc/War_AITargetComponent.h"
#endif //_ITF_WAR_AITARGETCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIWaitForAttackWindowAction)

    BEGIN_SERIALIZATION_CHILD(War_AIWaitForAttackWindowAction)
    END_SERIALIZATION()

    War_AIWaitForAttackWindowAction::War_AIWaitForAttackWindowAction()
        : Super()
        , m_target(NULL)
        , m_attackRequested(bfalse)
        , m_attackGranted(bfalse)
    {
    }

    War_AIWaitForAttackWindowAction::~War_AIWaitForAttackWindowAction()
    {
    }

    void War_AIWaitForAttackWindowAction::onActivate()
    {
        Super::onActivate();

        m_actor->registerEvent(War_EventAttackRequestGranted::GetClassNameStatic(), m_aiComponent);

        m_attackGranted = bfalse;
        m_target->requestAttack(m_actor);
    }

    void War_AIWaitForAttackWindowAction::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( _event->IsClass(War_EventAttackRequestGranted::GetClassNameStatic()))
        {
            m_attackGranted = btrue;
        }
    }

    void War_AIWaitForAttackWindowAction::update( f32 _delta )
    {
        if (m_attackGranted)
        {
            Super::update(_delta);
        }
    }

}