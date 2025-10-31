#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISLEEPBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AISleepBehavior.h"
#endif //_ITF_RAY_AISLEEPBEHAVIOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AISleepBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AISleepBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("wakeUp", m_wakeUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("deactivatePhysics",m_deactivatePhysics);
        SERIALIZE_MEMBER("wakeUpOnTrigger",m_wakeUpOnTrigger);
    END_SERIALIZATION()

    Ray_AISleepBehavior_Template::Ray_AISleepBehavior_Template()
    : m_idle(NULL)
    , m_wakeUp(NULL)
    , m_deactivatePhysics(bfalse)
    , m_wakeUpOnTrigger(btrue)
    {

    }

    Ray_AISleepBehavior_Template::~Ray_AISleepBehavior_Template()
    {
        SF_DEL(m_idle);
        SF_DEL(m_wakeUp);

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AISleepBehavior)

    Ray_AISleepBehavior::Ray_AISleepBehavior()
        : Super()
        , m_idle(NULL)
        , m_wakeUp(NULL)
        , m_physComponent(NULL)
    {
    }

    Ray_AISleepBehavior::~Ray_AISleepBehavior()
    {
    }

    void Ray_AISleepBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idle = createAiAction(getTemplate()->getIdle());
        m_wakeUp = createAiAction(getTemplate()->getWakeUp());

        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // not mandatory

        if (getTemplate()->getWakeUpOnTrigger())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_aiComponent);
        }
    }

    void Ray_AISleepBehavior::onActivate()
    {
        if (m_idle)
        {
            if (getTemplate()->getDeactivatePhysics() && m_physComponent)
            {
                m_physComponent->setDisabled(btrue,bfalse, btrue);
            }
            setAction(m_idle);
        }
    }

    void Ray_AISleepBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_currentAction = NULL;
    }

    void Ray_AISleepBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( getTemplate()->getWakeUpOnTrigger() &&
             _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            wakeUp();
        }
    }

    void Ray_AISleepBehavior::wakeUp()
    {
        if (m_wakeUp)
        {
            if (m_currentAction && m_currentAction != m_wakeUp)
            {
                setAction(m_wakeUp);
                if (getTemplate()->getDeactivatePhysics() && m_physComponent)
                {
                    m_physComponent->setDisabled(bfalse,bfalse, btrue);
                }
            }
        }
        else
        {
            m_aiComponent->onBehaviorFinished();
        }
    }

    void Ray_AISleepBehavior::onActionFinished()
    {
        Super::onActionFinished();

        if (m_currentAction && m_currentAction == m_wakeUp)
        {
            m_aiComponent->onBehaviorFinished();
        }
    }

}