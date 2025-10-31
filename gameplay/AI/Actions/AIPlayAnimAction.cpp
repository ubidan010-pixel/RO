#include "precompiled_gameplay.h"

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIPlayAnimAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIPlayAnimAction_Template)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(AIPlayAnimAction)
    AIPlayAnimAction::AIPlayAnimAction()
    {
    }

    AIPlayAnimAction::~AIPlayAnimAction()
    {
    }

    void AIPlayAnimAction::update( f32 _delta )
    {
        Super::update(_delta);

        if ( m_animComponent && !m_animComponent->isAnimLooped() && isAnimFinished() )
        {
            m_aiBehavior->onActionFinished();
        }
    }

} // namespace ITF