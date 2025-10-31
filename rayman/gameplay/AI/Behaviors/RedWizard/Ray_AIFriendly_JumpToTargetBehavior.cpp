#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIFRIENDLY_JUMPTOTARGETBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIFriendly_JumpToTargetBehavior.h"
#endif //_ITF_RAY_AIFRIENDLY_JUMPTOTARGETBEHAVIOR_H_

#ifndef _ITF_AIJUMPTOTARGETACTION_H_
#include "gameplay/ai/Actions/AIJumpToTargetAction.h"
#endif //_ITF_AIJUMPTOTARGETACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIFriendly_JumpToTargetBehavior)
    IMPLEMENT_OBJECT_RTTI(Ray_AIFriendly_JumpToTargetBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIFriendly_JumpToTargetBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("jump", m_jump, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    Ray_AIFriendly_JumpToTargetBehavior_Template::Ray_AIFriendly_JumpToTargetBehavior_Template()
        : m_jump(NULL)
    {
    }

    Ray_AIFriendly_JumpToTargetBehavior_Template::~Ray_AIFriendly_JumpToTargetBehavior_Template()
    {
        SF_DEL(m_jump);
    }


    BEGIN_VALIDATE_BEHAVIOR(Ray_AIFriendly_JumpToTargetBehavior)
        VALIDATE_BEHAVIOR_PARAM("jump", m_jump, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()

    Ray_AIFriendly_JumpToTargetBehavior::Ray_AIFriendly_JumpToTargetBehavior()
        : m_jump(NULL)
    {
    }

    Ray_AIFriendly_JumpToTargetBehavior::~Ray_AIFriendly_JumpToTargetBehavior()
    {
    }


    void Ray_AIFriendly_JumpToTargetBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_jump = createAiActionAndCast<AIJumpToTargetAction>(getTemplate()->getJump());

        Super::onActorLoaded(_hotReload);
    }

    void Ray_AIFriendly_JumpToTargetBehavior::setData( Event* _event )
    {
        Ray_EventAIOrder* order = _event->DynamicCast<Ray_EventAIOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIOrder,1055366852));
        if (order)
        {
            m_target = _event->getSender();
            // TODO: get jumpTarget from event
        }
        else
        {
            ITF_ASSERT_MSG(0, "wrong order type!");
            m_target = _event->getSender();
        }
    }

    void Ray_AIFriendly_JumpToTargetBehavior::onActivate()
    {
        //Super::onActivate();

        ITF_ASSERT(m_target.isValid());

        startJump();
    }

    void Ray_AIFriendly_JumpToTargetBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_target.invalidate();
    }


    Actor* getNextNode( ActorRef _parent )
    {
        if ( Actor* parent = AIUtils::getActor(_parent) )
        {
            if ( LinkComponent* parentLink = parent->GetComponent<LinkComponent>() )
            {
                AIUtils::LinkIterator links(parentLink, btrue);
                if ( Actor* firstChild = links.getNextRelativeActor() )
                {
                    return firstChild;
                }
            }
        }

        return NULL;
    }

    void Ray_AIFriendly_JumpToTargetBehavior::startJump()
    {
        if ( Actor* next = getNextNode(m_target) )
        {
            m_jump->setTargetActor(next->getRef());
        }

        setAction(m_jump,btrue);
    }

    void Ray_AIFriendly_JumpToTargetBehavior::onActionFinished()
    {
        Super::onActionFinished();

        if (m_currentAction == m_jump)
        {
            m_aiComponent->onBehaviorFinished();
        }
    }

}