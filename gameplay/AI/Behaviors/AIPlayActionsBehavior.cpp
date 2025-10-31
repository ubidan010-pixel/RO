#include "precompiled_gameplay.h"

#ifndef _ITF_AIPLAYACTIONSBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIPlayActionsBehavior.h"
#endif //_ITF_AIPLAYACTIONSBEHAVIOR_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIPlayActionsBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(AIPlayActionsBehavior_Template)
        SERIALIZE_CONTAINER_WITH_FACTORY("actions", m_playActions, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    AIPlayActionsBehavior_Template::AIPlayActionsBehavior_Template()
    {
    }

    AIPlayActionsBehavior_Template::~AIPlayActionsBehavior_Template()
    {
        for ( ITF_VECTOR <AIAction_Template*>::iterator it = m_playActions.begin(); it != m_playActions.end(); ++it )
        {
            delete (*it);
        }

    }
    BEGIN_VALIDATE_BEHAVIOR(AIPlayActionsBehavior)
        VALIDATE_BEHAVIOR_PARAM("actions", m_actions.size() > 0, "AIPlayActionsBehavior needs at least one action!");
    END_VALIDATE_BEHAVIOR()

    IMPLEMENT_OBJECT_RTTI(AIPlayActionsBehavior)


    AIPlayActionsBehavior::AIPlayActionsBehavior()
        : Super()
        , m_currentActionIndex(U32_INVALID)
    {
    }

    AIPlayActionsBehavior::~AIPlayActionsBehavior()
    {
    }

    void AIPlayActionsBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventReset,2551104629)) )
        {
            // TODO: I'm not sure we should need to handle EventReset in behaviors/actions;
            // if the AIComponent stops/restarts the current behavior, we should just go through the normal onActivate/onDeactivate
            m_currentActionIndex = 0;
        }
    }


    void AIPlayActionsBehavior::onActivate()
    {
        m_currentActionIndex = 0;
        if ( m_animComponent )
        {
            m_animComponent->resetCurTime();
        }
        setAction(m_actions[0]);
    }

    void AIPlayActionsBehavior::onActionFinished()
    {
        Super::onActionFinished();

        m_currentActionIndex++;
        if ( m_currentActionIndex < m_actions.size() )
        {
            // play next action
            if ( m_animComponent )
            {
                m_animComponent->resetCurTime();
            }
            setAction(m_actions[m_currentActionIndex]);
        }
        else
        {
            // notify component
            m_aiComponent->onBehaviorFinished();
        }
    }

    void AIPlayActionsBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        const AIPlayActionsBehavior_Template::ActionsArray & playActions = getTemplate()->getPlayActions();
        for (u32 i = 0; i < playActions.size(); ++i)
        {
            createAiAction(playActions[i]);
        }

        //ALWAYS CALL THIS LAST
        Super::onActorLoaded( _hotReload );
    }
}