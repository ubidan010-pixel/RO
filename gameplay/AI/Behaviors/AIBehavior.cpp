#include "precompiled_gameplay.h"

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(TemplateAIBehavior)
    BEGIN_SERIALIZATION_ABSTRACT(TemplateAIBehavior)
		SERIALIZE_MEMBER( "name", m_name );
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(AIBehavior)
	
    AIBehavior::AIBehavior()
        : Super()
        , m_template(NULL)
        , m_actor(NULL)
        , m_aiComponent(NULL)
        , m_currentAction(NULL)
        , m_animComponent(NULL)
        , m_animatedComponent(NULL)
        , m_soundComponent(NULL)
        , m_fxController(NULL)
        , m_destroyRegisteredActions(btrue)
		, m_isExtern( bfalse )
    {
    }

    AIBehavior::~AIBehavior()
    {
        // destroy registered actions
        const u32 actionCount = m_actions.size();
        for (u32 i = 0; i < actionCount; i++)
        {
            SF_DEL(m_actions[i]);
        }
    }



    void AIBehavior::addAction( AIAction* _action )
    {
        ITF_ASSERT_MSG(_action, "Do not add an invalid action !");
        if (_action)
            m_actions.push_back(_action);
    }

    void AIBehavior::init( Actor* _actor, AIComponent* _aiComponent )
    {
        m_actor = _actor;
        m_aiComponent = _aiComponent;

        m_animComponent     = m_actor->GetComponent<AnimLightComponent>();
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_fxController      = m_actor->GetComponent<FXControllerComponent>();
        m_soundComponent    = m_actor->GetComponent<SoundComponent>();

        onActorLoaded(Pickable::HotReloadType_None);

        const u32 actionCount = m_actions.size();
        for (u32 i = 0; i < actionCount; i++)
        {
            m_actions[i]->init(m_actor, m_aiComponent, this);
        }
    }

    void AIBehavior::onResourceReady()
    {
        const u32 actionCount = m_actions.size();
        for (u32 i = 0; i < actionCount; i++)
        {
            m_actions[i]->onResourceReady();
        }
    }

    void AIBehavior::validate( bbool& _isBehaviorValidated )
    {
        const u32 actionCount = m_actions.size();
        for (u32 i = 0; i < actionCount; i++)
        {
            m_actions[i]->validate(_isBehaviorValidated);
        }
    }

    void AIBehavior::setAction( AIAction* _action, bbool _restart /*= bfalse*/ )
    {
        if ( _action == NULL )
        {
            ITF_ASSERT_MSG(0, "trying to set a NULL action! if this was intentional, use setNullAction() instead");
            return;
        }

        if ( _action == m_currentAction && !_restart )
        {
            return;
        }

        if ( m_currentAction )
        {
            m_currentAction->onDeactivate();
        }

        m_currentAction = _action;

        if ( m_currentAction )
        {
            ITF_ASSERT(m_aiComponent);
            if (m_aiComponent)  // Happened on 2010/11/5th. TRAC #923
                m_aiComponent->computeRandomAnimNumber();
            m_currentAction->onActivate();
        }
    }

    void AIBehavior::setNullAction()
    {
        if ( m_currentAction )
        {
            m_currentAction->onDeactivate();
        }

        m_currentAction = NULL;
    }


    void AIBehavior::update( f32 _dt )
    {
        if ( m_currentAction )
        {
            m_currentAction->update(_dt);
        }
    }

    void AIBehavior::onActorBecomeActive()
    {
        if ( m_currentAction )
        {
            m_currentAction->onActorBecomeActive();
        }
    }

    void AIBehavior::onActorBecomeInactive()
    {
        if ( m_currentAction )
        {
            m_currentAction->onActorBecomeInactive();
        }
    }

    void AIBehavior::onForceMove()
    {
        if ( m_currentAction )
        {
            m_currentAction->onForceMove();
        }
    }

    void AIBehavior::onDeactivate()
    {
        if ( m_currentAction )
        {
            m_currentAction->onDeactivate();
            m_currentAction = NULL;
        }
    }

    void AIBehavior::onEvent( Event* _event)
    {
        if (EventSetOriginalSender* setOriginalSenderEvt = _event->DynamicCast<EventSetOriginalSender>(ITF_GET_STRINGID_CRC(EventSetOriginalSender,1174287572)))
        {
            // all actions, even inactive, need to know about the original sender 
            // NB: for rewards' Action_KillHunterWithOwnBullet
            const u32 actionCount = m_actions.size();
            for (u32 i=0; i<actionCount; ++i)
            {
                m_actions[i]->onEvent(_event);
            }
        }
        else if ( m_currentAction )
        {
            m_currentAction->onEvent(_event);
        }
    }

    void AIBehavior::updateAnimInputs()
    {
        if ( m_currentAction )
        {
            m_currentAction->updateAnimInputs();
        }
    }
    void AIBehavior::updateSoundInputs()
    {
        if ( m_currentAction )
        {
            m_currentAction->updateSoundInputs();
        }
    }

    void AIBehavior::updateFxInputs()
    {
        if ( m_currentAction )
        {
            m_currentAction->updateFxInputs();
        }
    }

    AIAction * AIBehavior::createAiAction( const AIAction_Template * _template )
    {
        if (_template)
        {
            AIAction * action = _template->createAIAction();
            addAction(action);
            action->setTemplate(_template);
            return action;
        }
        return NULL;
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void AIBehavior::getDebugText( String8& _text, const String8& _separator /*= " - "*/ )
    {
        _text += StringID(GetObjectClassCRC()).getDebugString();
        if (m_currentAction)
        {
            _text += _separator;
            m_currentAction->getDebugText(_text);
        }
        else
        {
            _text += _separator + "NO ACTION";
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

#ifdef ITF_SUPPORT_EDITOR
    void AIBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (m_currentAction)
        {
            m_currentAction->drawEdit(_drawInterface, _flags);
        }
    }
#endif // ITF_SUPPORT_EDITOR
}