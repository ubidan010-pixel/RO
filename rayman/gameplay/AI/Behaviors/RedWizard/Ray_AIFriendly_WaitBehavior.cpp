#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AINPC_WAITFORPLAYERBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIFriendly_WaitBehavior.h"
#endif //_ITF_RAY_AINPC_WAITFORPLAYERBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIFriendly_WaitBehavior)
    IMPLEMENT_OBJECT_RTTI(Ray_AIFriendly_WaitBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIFriendly_WaitBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("randomAction", m_randomAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("triggerTarget", m_triggerTarget);
        SERIALIZE_MEMBER("randomActionDelayMin", m_randomActionDelayMin);
        SERIALIZE_MEMBER("randomActionDelayMax", m_randomActionDelayMax);
    END_SERIALIZATION()

    Ray_AIFriendly_WaitBehavior_Template::Ray_AIFriendly_WaitBehavior_Template()
        : m_idle(NULL)
        , m_randomAction(NULL)
        , m_randomActionDelayMin(10.f)
        , m_randomActionDelayMax(15.f)
        , m_triggerTarget(bfalse)
    {
    }

    Ray_AIFriendly_WaitBehavior_Template::~Ray_AIFriendly_WaitBehavior_Template()
    {
        SF_DEL(m_idle);
        SF_DEL(m_randomAction);
    }


    BEGIN_VALIDATE_BEHAVIOR(Ray_AIFriendly_WaitBehavior)
        VALIDATE_BEHAVIOR_PARAM("targetReached", m_idle, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()

    Ray_AIFriendly_WaitBehavior::Ray_AIFriendly_WaitBehavior()
        : m_idle(NULL)
        , m_randomAction(NULL)
        , m_orderType(Ray_EventAIOrder::ORDERTYPE_Unknown)
        , m_playerDetectRadius(0.f)
        , m_randomActionCounter(0.f)
    {
    }

    Ray_AIFriendly_WaitBehavior::~Ray_AIFriendly_WaitBehavior()
    {
    }


    void Ray_AIFriendly_WaitBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idle = createAiAction(getTemplate()->getIdle());
        m_randomAction = createAiAction(getTemplate()->getRandomAction());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_aiComponent);
    }

    void Ray_AIFriendly_WaitBehavior::setData( Event* _event )
    {
        Ray_EventAIOrder* order = _event->DynamicCast<Ray_EventAIOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIOrder,1055366852));
        if (order)
        {
            m_target = _event->getSender();
            m_orderType = order->getOrderType();
            m_playerDetectRadius = order->getPlayerDetectRadius();
        }
        else
        {
            ITF_ASSERT_MSG(0, "wrong order type!");
            m_target = _event->getSender();
            m_orderType = Ray_EventAIOrder::ORDERTYPE_WaitForPlayer;
            m_playerDetectRadius = 3.f;
        }
    }

    void Ray_AIFriendly_WaitBehavior::onActivate()
    {
        //Super::onActivate();

        ITF_ASSERT(m_target.isValid());
        ITF_ASSERT(m_orderType != Ray_EventAIOrder::ORDERTYPE_Unknown);
        ITF_ASSERT(m_playerDetectRadius > 0.f);

        setAction(m_idle);
        triggerTarget();    // TODO: could be done with an anim event
        
        if ( m_randomAction )
        {
            setRandomActionCounter();
        }
    }

    void Ray_AIFriendly_WaitBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_target.invalidate();
        m_orderType = Ray_EventAIOrder::ORDERTYPE_Unknown;
        m_playerDetectRadius = 0.f;
    }

    void Ray_AIFriendly_WaitBehavior::onActionFinished()
    {
        if ( m_currentAction == m_randomAction )
        {
            setRandomActionCounter();
            setAction(m_idle);
        }
    }

    void Ray_AIFriendly_WaitBehavior::setRandomActionCounter()
    {
        m_randomActionCounter = Seeder::getSharedSeeder().GetFloat(getTemplate()->getRandomActionDelayMin(), getTemplate()->getRandomActionDelayMax());
    }

    void Ray_AIFriendly_WaitBehavior::update( f32 _dt )
    {
        Super::update(_dt);

        if (m_orderType == Ray_EventAIOrder::ORDERTYPE_WaitForPlayer)
        {
            ActorList players;
            AIUtils::getPlayers(m_actor->getDepth(), players);

            bbool hasPlayerInRange = AIUtils::hasActorInRange( players,
                m_actor, m_actor->isFlipped(),
                m_playerDetectRadius );

            if (hasPlayerInRange)
            {
                m_aiComponent->onBehaviorFinished();
                return;
            }
        }

        if ( m_currentAction == m_idle )
        {
            if ( m_randomActionCounter )
            {
                m_randomActionCounter -= _dt;

                if ( m_randomActionCounter <= 0.f )
                {
                    m_randomActionCounter = 0.f;
                    setAction(m_randomAction);
                }
            }
        }
    }

    void Ray_AIFriendly_WaitBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (m_orderType == Ray_EventAIOrder::ORDERTYPE_WaitForTrigger)
        {
            if (_event->IsClassCRC(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
            {
                m_aiComponent->onBehaviorFinished();
            }
        }
    }

    void Ray_AIFriendly_WaitBehavior::triggerTarget()
    {
        if (!getTemplate()->getTriggerTarget())
        {
            return;
        }

        Actor* target = m_target.getActor();
        if (!target)
        {
            return;
        }

        // send a special event: the order will then trigger its children with a standard EventTrigger
        Ray_EventAIOrderProcessed eventTrigger;
        eventTrigger.setSender(m_actor->getRef());
        target->onEvent(&eventTrigger);

        //EventTrigger eventTrigger;
        //eventTrigger.setSender(m_actor->getRef());
        //eventTrigger.setActivator(m_actor->getRef());
        //eventTrigger.setActivated(btrue);
        //target->onEvent(&eventTrigger);
    }

}