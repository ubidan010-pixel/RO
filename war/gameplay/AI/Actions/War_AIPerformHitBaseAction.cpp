#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIPERFORMHITBASEACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitBaseAction.h"
#endif //_ITF_WAR_AIPERFORMHITBASEACTION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_WAR_AITARGETCOMPONENT_H_
#include "war/gameplay/Components/Misc/War_AITargetComponent.h"
#endif //_ITF_WAR_AITARGETCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIPerformHitBaseAction)

    War_AIPerformHitBaseAction::War_AIPerformHitBaseAction()
        : Super()
    {
    }

    War_AIPerformHitBaseAction::~War_AIPerformHitBaseAction()
    {
    }

    void War_AIPerformHitBaseAction::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (animEvent->getName() == War_MRK_Perform_Hit)
            {
                performHit(animEvent);
                m_target->attackPerformed(m_actor);
            }
        }
    }

}