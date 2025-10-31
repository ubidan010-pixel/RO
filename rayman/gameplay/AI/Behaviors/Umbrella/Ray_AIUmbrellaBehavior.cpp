#include "precompiled_gameplay_rayman.h"

#ifndef _RAY_AI_UMBRELLABEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Umbrella/Ray_AIUmbrellaBehavior.h"
#endif //_RAY_AI_UMBRELLABEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIUmbrellaBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIUmbrellaBehavior);
        VALIDATE_BEHAVIOR_PARAM("openAction", m_openAction, "Action mandatory");
        VALIDATE_BEHAVIOR_PARAM("closedAction", m_closedAction, "Action mandatory");
        VALIDATE_BEHAVIOR_PARAM("openingAction", m_openingAction, "Action mandatory");
        VALIDATE_BEHAVIOR_PARAM("closingAction", m_closingAction, "Action mandatory");
        VALIDATE_BEHAVIOR_PARAM("polylineComponent", m_polylineComponent, "Component mandatory");
    END_VALIDATE_BEHAVIOR()

    //-------------------------------------------------------
    Ray_AIUmbrellaBehavior::Ray_AIUmbrellaBehavior()
    : m_openAction(NULL)
    , m_closedAction(NULL)
    , m_openingAction(NULL)
    , m_closingAction(NULL)
    , m_warningAction(NULL)
    , m_polylineComponent(NULL)
    , m_currentActionTimer(0.0f)
    {
    }

    //-------------------------------------------------------
    Ray_AIUmbrellaBehavior::~Ray_AIUmbrellaBehavior()
    {
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //create actions
        m_openAction = createAiAction(getTemplate()->getOpenAction());
        m_closedAction = createAiAction(getTemplate()->getClosedAction());
        m_openingAction = createAiAction(getTemplate()->getOpeningAction());
        m_closingAction = createAiAction(getTemplate()->getClosingAction());
        m_warningAction = createAiAction(getTemplate()->getWarningAction());
        ITF_WARNING( m_actor, (m_warningAction != NULL) || (getTemplate()->getCloseWithWeight() == 0), "Umbrella close with weight but do not have warning action, check your Ray_AIUmbrellaBehavior parameters!" );

        //get polyline component
        m_polylineComponent = m_actor->GetComponent<PolylineComponent>();

        //register event
        if( !getTemplate()->getCloseWithWeight() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_aiComponent);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), m_aiComponent);
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::onActivate()
    {
        //start behavior with the right action according to metronome current state
        initAction();
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::update(f32 _dt)
    {
        Super::update(_dt);

        if( m_currentAction == m_openAction )
        {
            updateOpenAction();
        }
        else if( m_currentAction == m_closedAction )
        {
            updateClosedAction();
        }
        if( m_currentAction == m_openingAction )
        {
            updateOpeningAction();
        }
        else if( m_currentAction == m_closingAction )
        {
            updateClosingAction();
        }
        else if( m_currentAction == m_warningAction )
        {
            updateWarningAction();
        }
        
        m_currentActionTimer += _dt;
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::initAction()
    {
        startOpenAction();
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::startAction()
    {
        m_currentActionTimer = 0.0f;
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::startClosedAction()
    {
        startAction();
        setAction( m_closedAction );
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::startOpenAction()
    {
        startAction();
        setAction( m_openAction );
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::startClosingAction()
    {
        startAction();
        setAction( m_closingAction );
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::startOpeningAction()
    {
        startAction();
        setAction( m_openingAction );
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::startWarningAction()
    {
        startAction();
        setAction( m_warningAction );
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::updateClosedAction()
    {
        if( getTemplate()->getCloseWithWeight() )
        {
            //if platform remained enough time closed
            if( m_currentActionTimer >= getTemplate()->getClosedStateTime() )
            {
                //start opening
                startOpeningAction();
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::updateOpenAction()
    {
        if( getTemplate()->getCloseWithWeight() )
        {
            if( m_polylineComponent->getMaxUserWeight() >= getTemplate()->getWeightThreshold() )
            {
                startWarningAction();
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::updateClosingAction()
    {
        if( m_animComponent != NULL )
        {
            if( !m_animComponent->isAnimLooped() )
            {
                if( m_animComponent->isSubAnimFinished() )
                {
                    startClosedAction();
                }
            }
        }
        else
        {
            startClosedAction();
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::updateOpeningAction()
    {
        if( m_animComponent != NULL )
        {
            if( !m_animComponent->isAnimLooped() )
            {
                if( m_animComponent->isSubAnimFinished() )
                {
                    startOpenAction();
                }
            }
        }
        else
        {
            startOpenAction();
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::updateWarningAction()
    {
        //if there is still enough weight on platform
        if( m_polylineComponent->getMaxUserWeight() >= getTemplate()->getWeightThreshold() )
        {
            if( m_currentActionTimer >= getTemplate()->getWarningStateTime() )
            {
                startClosingAction();
            }
        }
        else
        {
            //back to opened state
            startOpenAction();
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::onEvent(Event * _event)
    {
        if( const EventTrigger * const triggerEvent = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            onTriggerEvent(triggerEvent);
        }
        else if ( const EventGeneric * const genericEvent = _event->DynamicCast<EventGeneric>(ITF_GET_STRINGID_CRC(EventGeneric,4063838687)) )
        {
            onGenericEvent(genericEvent);
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::onTriggerEvent(const EventTrigger * const _event)
    {
        if( m_currentAction == m_openAction || m_currentAction == m_openingAction )
        {
            startClosingAction();
        }
        else if( m_currentAction == m_closedAction || m_currentAction == m_closingAction )
        {
            startOpeningAction();
        }
    }

    //-------------------------------------------------------
    void Ray_AIUmbrellaBehavior::onGenericEvent(const EventGeneric * const _event)
    {
        const StringID s_Open("Open");
        const StringID s_Close("Close");

        if (_event->getId() == s_Open)
        {
            if( m_currentAction == m_closedAction || m_currentAction == m_closingAction )
            {
                startOpeningAction();
            }
        }
        else if (_event->getId() == s_Close)
        {
            if( m_currentAction == m_openAction || m_currentAction == m_openingAction )
            {
                startClosingAction();
            }
        }
    }



    /**********************************************************************/
    //  template class
    /**********************************************************************/

    IMPLEMENT_OBJECT_RTTI(Ray_AIUmbrellaBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIUmbrellaBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("openAction", m_openAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closedAction", m_closedAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("openingAction", m_openingAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closingAction", m_closingAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("warningAction", m_warningAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("closedStateTime", m_closedStateTime);
        SERIALIZE_MEMBER("warningStateTime", m_warningStateTime);
        SERIALIZE_MEMBER("weightThreshold", m_weightThreshold);
        SERIALIZE_MEMBER("closeWithWeight", m_closeWithWeight);

    END_SERIALIZATION();

    Ray_AIUmbrellaBehavior_Template::Ray_AIUmbrellaBehavior_Template()
    : m_openAction(NULL)
    , m_closedAction(NULL)
    , m_openingAction(NULL)
    , m_closingAction(NULL)
    , m_warningAction(NULL)
    , m_closedStateTime(0.5f)
    , m_warningStateTime(1.0f)
    , m_weightThreshold(0.6f)
    , m_closeWithWeight(bfalse)
    {
    }

    Ray_AIUmbrellaBehavior_Template::~Ray_AIUmbrellaBehavior_Template()
    {
        SF_DEL(m_openAction);
        SF_DEL(m_closedAction);
        SF_DEL(m_openingAction);
        SF_DEL(m_closingAction);
        SF_DEL(m_warningAction);
    }
}