#include "precompiled_gameplay.h"

#ifndef _ITF_AISIMPLEPLAYANIMBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AISimplePlayAnimBehavior.h"
#endif //_ITF_AISIMPLEPLAYANIMBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AISimplePlayAnimBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(AISimplePlayAnimBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("playAnim", m_playAnim, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

        AISimplePlayAnimBehavior_Template::AISimplePlayAnimBehavior_Template()
        : m_playAnim(NULL)
    {

    }

    AISimplePlayAnimBehavior_Template::~AISimplePlayAnimBehavior_Template()
    {
        SF_DEL(m_playAnim);
    }
    //BEGIN_VALIDATE_BEHAVIOR(AISimplePlayAnimBehavior)
    //    VALIDATE_BEHAVIOR_PARAM("playAnim", m_playAnim, "this action is mandatory");
    //END_VALIDATE_BEHAVIOR()

    IMPLEMENT_OBJECT_RTTI(AISimplePlayAnimBehavior)
    AISimplePlayAnimBehavior::AISimplePlayAnimBehavior()
        : m_playAnim(NULL)
    {
    }

    AISimplePlayAnimBehavior::~AISimplePlayAnimBehavior()
    {
    }

    void AISimplePlayAnimBehavior::onActivate()
    {
        setAction(m_playAnim);
    }

    void AISimplePlayAnimBehavior::onActionFinished()
    {
        ITF_ASSERT(m_currentAction && m_currentAction == m_playAnim);
        m_aiComponent->onBehaviorFinished();
    }

    void AISimplePlayAnimBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_playAnim = static_cast<AIPlayAnimAction*>(createAiAction(getTemplate()->getPlayAnim()));
        //ALWAYS CALL THIS LAST
        Super::onActorLoaded( _hotReload );
    }
}

