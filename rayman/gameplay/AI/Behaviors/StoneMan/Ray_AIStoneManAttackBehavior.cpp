#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISTONEMANATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/StoneMan/Ray_AIStoneManAttackBehavior.h"
#endif //_ITF_RAY_AISTONEMANATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_
#include "rayman/gameplay/AI/Actions/Shooter/Ray_AIHunterLaunchBulletAction.h"
#endif //_ITF_RAY_AIHUNTERLAUNCHBULLETACTION_H_

#ifndef _ITF_RAY_STONEMANAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_StoneManAIComponent.h"
#endif //_ITF_RAY_STONEMANAICOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIStoneManAttackBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIStoneManAttackBehavior)
        VALIDATE_BEHAVIOR_PARAM("respawnBoulderAction", m_actionRespawnBoulder, "Action mandatory");
        VALIDATE_BEHAVIOR_PARAM("idleAction", m_actionIdle, "Action mandatory");
        VALIDATE_BEHAVIOR_PARAM("anticipationAction", m_actionAnticipation, "Action mandatory");
        VALIDATE_BEHAVIOR_PARAM("stoneManAIComponent", m_stoneManAIComponent, "Behavior mandatory");
    END_VALIDATE_BEHAVIOR()


    //-------------------------------------------------------
    Ray_AIStoneManAttackBehavior::Ray_AIStoneManAttackBehavior()
    : m_actionRespawnBoulder(NULL)
    , m_actionIdle(NULL)
    , m_actionAnticipation(NULL)
    , m_actionSleep(NULL)
    , m_actionWakeUp(NULL)
    , m_launchActionList()
    , m_stoneManAIComponent(NULL)
    , m_launchActionIndex(0)
    , m_enabled(bfalse)
    , m_linkedToActivator(bfalse)
    , m_launchRequested(bfalse)
    {
    }

    //-------------------------------------------------------
    Ray_AIStoneManAttackBehavior::~Ray_AIStoneManAttackBehavior()
    {
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_actionRespawnBoulder = createAiAction(getTemplate()->getActionRespawnBoulder());
        m_actionIdle = createAiAction(getTemplate()->getActionIdle());
        m_actionAnticipation = createAiAction(getTemplate()->getActionAnticipation());
        m_actionSleep = createAiAction(getTemplate()->getActionSleep());
        m_actionWakeUp = createAiAction(getTemplate()->getActionWakeUp());

        //create launch action list
        Ray_AIStoneManAttackBehavior_Template::LaunchActionList::const_iterator it = getTemplate()->getLaunchActionList().begin();
        while( it != getTemplate()->getLaunchActionList().end() )
        {
            Ray_AIHunterLaunchBulletAction * const launchAction = static_cast<Ray_AIHunterLaunchBulletAction*>(createAiAction((*it)));
            ITF_ASSERT(launchAction);
            if( launchAction )
            {
                m_launchActionList.push_back(launchAction);
            }
            ++it;
        }

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventStoneManLaunch,3722106216), m_aiComponent);        

        m_stoneManAIComponent = m_actor->GetComponent<Ray_StoneManAIComponent>();
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onActivate()
    {
        if( !m_linkedToActivator )
        {
            if( m_stoneManAIComponent->getBoulderPresence() )
            {
                startIdle();
            }
            else
            {
                startRespawnBoulder();
            }
        }
        else
        {
            startSleep();
        }
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::update(f32 _dt)
    {
        if( !m_linkedToActivator || m_enabled )
        {
            if( isCurrentAction(m_actionSleep) )
            {
                updateSleep(_dt);
            }
            else if( isCurrentAction(m_actionIdle) )
            {
                updateIdle(_dt);
            }
        }
        else
        {
            if( !isCurrentAction(m_actionSleep) )
            {
                startSleep();
            }
        }

        Super::update(_dt);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (EventActivate * activate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            m_enabled = activate->getActivated();
            m_linkedToActivator = btrue;
        }
        else if( EventTrigger * trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if( trigger->getActivated() )
            {
                tryToStartLaunch(0);
            }
        }
        else if ( Ray_EventStoneManLaunch * launchEvent = _event->DynamicCast<Ray_EventStoneManLaunch>(ITF_GET_STRINGID_CRC(Ray_EventStoneManLaunch,3722106216)) )
        {
            tryToStartLaunch(launchEvent->getActionIndex());
        }
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onActionFinished()
    {
        if( isCurrentAction(m_actionWakeUp) )
        {
            onFinishedWakeUp();
        }
        else if( isCurrentAction(m_actionRespawnBoulder) )
        {
            onFinishedRespawnBoulder();
        }
        else if( isCurrentAction(m_actionAnticipation) )
        {
            onFinishedAnticipation();
        }
        else if( isCurrentActionLaunch() )
        {
            onFinishedLaunch();
        }
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::startSleep()
    {
        setAction(m_actionSleep);
        m_stoneManAIComponent->setBoulderPresence(bfalse);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::updateSleep(const f32 _dt)
    {
        if( m_linkedToActivator && m_enabled )
        {
            startWakeUp();
        }
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::startWakeUp()
    {
        setAction(m_actionWakeUp);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onFinishedWakeUp()
    {
        startRespawnBoulder();
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::startRespawnBoulder()
    {
        setAction(m_actionRespawnBoulder);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onFinishedRespawnBoulder()
    {
        startIdle();
        m_stoneManAIComponent->setBoulderPresence(btrue);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::startIdle()
    {
        setAction(m_actionIdle);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::updateIdle(const f32 _dt)
    {
        if( m_launchRequested )
        {
            startAnticipation();
            m_launchRequested = bfalse;
        }
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::startAnticipation()
    {
        setAction(m_actionAnticipation);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onFinishedAnticipation()
    {
        startLaunch();
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::startLaunch()
    {
        setAction(m_launchActionList[m_launchActionIndex]);
        m_stoneManAIComponent->setBoulderPresence(bfalse);
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::onFinishedLaunch()
    {
        startRespawnBoulder();
    }

    //-------------------------------------------------------
    bbool Ray_AIStoneManAttackBehavior::isCurrentActionLaunch() const
    {
        return ( (m_currentAction != NULL) && (m_currentAction == m_launchActionList[m_launchActionIndex]) );
    }

    //-------------------------------------------------------
    void Ray_AIStoneManAttackBehavior::tryToStartLaunch(const u32 _actionIndex)
    {
        m_launchActionIndex = _actionIndex;
        if( isCurrentAction(m_actionIdle) )
        {                
            startAnticipation();
        }
        else
        {
            m_launchRequested = btrue;
        }
    }



    //-------------------------------------------------------
    // Template
    //-------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_AIStoneManAttackBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIStoneManAttackBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("respawnBoulder", m_actionRespawnBoulder, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_actionIdle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("anticipation", m_actionAnticipation, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("sleep", m_actionSleep, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("wakeUp", m_actionWakeUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_CONTAINER_WITH_FACTORY("launchActions", m_launchActionList, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    Ray_AIStoneManAttackBehavior_Template::Ray_AIStoneManAttackBehavior_Template()
    : m_actionRespawnBoulder(NULL)
    , m_actionIdle(NULL)
    , m_actionAnticipation(NULL)
    , m_actionSleep(NULL)
    , m_actionWakeUp(NULL)
    {
    }

    Ray_AIStoneManAttackBehavior_Template::~Ray_AIStoneManAttackBehavior_Template()
    {
        SF_DEL(m_actionRespawnBoulder);
        SF_DEL(m_actionIdle);
        SF_DEL(m_actionAnticipation);
        SF_DEL(m_actionSleep);
        SF_DEL(m_actionWakeUp);
        LaunchActionList::iterator it = m_launchActionList.begin();
        while( it != m_launchActionList.end() )
        {
            SF_DEL(*it);
            ++it;
        }
    }
}
