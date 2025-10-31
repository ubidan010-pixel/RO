#include "precompiled_gameplay_rayman.h"

#ifndef _RAY_TEXTBUBBLEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/TextBubble/Ray_TextBubbleBehavior.h"
#endif //_RAY_TEXTBUBBLEBEHAVIOR_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_TextBubbleBehavior);

    BEGIN_VALIDATE_BEHAVIOR(Ray_TextBubbleBehavior)
        VALIDATE_BEHAVIOR_PARAM("SimpleTextComponent", m_textComponent != NULL, "Component mandatory");
        VALIDATE_BEHAVIOR_PARAM("AnimLightComponent", m_animLightComponent != NULL, "Component mandatory");
    END_VALIDATE_BEHAVIOR()

    Ray_TextBubbleBehavior::Ray_TextBubbleBehavior()
    : m_textComponent(NULL)
    , m_animLightComponent(NULL)
    , m_sleepAction(NULL)
    , m_openAction(NULL)
    , m_idleAction(NULL)
    , m_closeAction(NULL)
    , m_activated(bfalse)
    , m_linkToActivator(bfalse)
    , m_textRefreshRequested(bfalse)
    , m_bubbleClosingRequested(bfalse)
    {
    }


    Ray_TextBubbleBehavior::~Ray_TextBubbleBehavior()
    {
    }

    void Ray_TextBubbleBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //get actor's components
        m_textComponent = m_actor->GetComponent<UITextBox>();
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();

        //create actions
        m_sleepAction = createAiAction(getTemplate()->getSleepActionTemplate());
        m_openAction = createAiAction(getTemplate()->getOpenActionTemplate());
        m_idleAction = createAiAction(getTemplate()->getIdleActionTemplate());
        m_closeAction = createAiAction(getTemplate()->getCloseActionTemplate());

        //register actor to events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDisplayText,3103222744), m_aiComponent);
    }

    void Ray_TextBubbleBehavior::onActivate()
    {
        //Super::onActivate();

        startSleep();
    }

    void Ray_TextBubbleBehavior::update(f32 _dt)
    {
        Super::update(_dt);

        updateAction();
    }

    void Ray_TextBubbleBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        
        if(const EventActivate * const activateEvent = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
        {
            m_activated = activateEvent->getActivated();
            m_linkToActivator = btrue;
        }
        else if (const EventDisplayText * const displayTextEvent = _event->DynamicCast<EventDisplayText>(ITF_GET_STRINGID_CRC(EventDisplayText,3103222744)))
        {
            const u32 lineId = displayTextEvent->getLineId();
            if( lineId != U32_INVALID )
            {
                m_textRefreshRequested = btrue;
                m_lineId = displayTextEvent->getLineId();
            }
            else
            {
                m_bubbleClosingRequested = btrue;
            }
        }
    }

    //actions management

    void Ray_TextBubbleBehavior::updateAction()
    {
        if( isCurrentAction(m_sleepAction) )
        {
            updateSleep();
        }
        else if( isCurrentAction(m_idleAction) )
        {
            updateIdle();
        }
    }

    void Ray_TextBubbleBehavior::onActionFinished()
    {
        Super::onActionFinished();

        if( isCurrentAction(m_openAction) )
        {
            startIdle();
        }
        else if( isCurrentAction(m_closeAction) )
        {
            if( (m_linkToActivator && !m_activated) || m_bubbleClosingRequested )
            {
                startSleep();
            }
            else
            {
                startOpen();
            }
        }
    }

    void Ray_TextBubbleBehavior::startSleep()
    {
        setAction(m_sleepAction);
        m_textComponent->hide();
        m_bubbleClosingRequested = bfalse;
    }

    void Ray_TextBubbleBehavior::updateSleep()
    {
        if( m_linkToActivator )
        {
            if( m_activated && m_textRefreshRequested )
            {
                startOpen();
            }
        }
        else if( m_textRefreshRequested )
        {
            startOpen();
        }
    }

    void Ray_TextBubbleBehavior::startOpen()
    {
        setAction(m_openAction);
        m_textComponent->hide();
        m_textRefreshRequested = bfalse;
    }

    void Ray_TextBubbleBehavior::startIdle()
    {
        setAction(m_idleAction);

        m_textComponent->clearText();
        m_textComponent->addTextFromLineId(m_lineId,Color::black());
        m_textComponent->show();
    }

    void Ray_TextBubbleBehavior::updateIdle()
    {
        if( !m_linkToActivator || m_activated )
        {
            if( !m_textRefreshRequested )
            {
                if( !m_bubbleClosingRequested )
                {
                    startClose();
                }
            }
            else
            {
                startClose();
                m_textRefreshRequested = bfalse;
            }
        }
        else
        {
            startClose();
        }

    }

    void Ray_TextBubbleBehavior::startClose()
    {
        setAction(m_closeAction);
        m_textComponent->hide();
    }

    //text management

    Vec3d Ray_TextBubbleBehavior::getTextPosition() const
    {
        Vec3d position = m_actor->getPos();
        const StringID & boneName = getTemplate()->getTextBoneName();
        if( m_animLightComponent && boneName.isValid() )
        {
            const i32 boneIndex = m_animLightComponent->getBoneIndex(boneName);
            if( boneIndex != U32_INVALID )
            {
                m_animLightComponent->getBonePos(boneIndex, position);
            }
        }
        return position;
    }



    IMPLEMENT_OBJECT_RTTI(Ray_TextBubbleBehavior_Template)
    BEGIN_SERIALIZATION(Ray_TextBubbleBehavior_Template)
        SERIALIZE_MEMBER("textBoneName", m_textBoneName);
        SERIALIZE_OBJECT_WITH_FACTORY("sleepAction", m_sleepActionTemplate, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("openAction", m_openActionTemplate, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("idleAction", m_idleActionTemplate, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closeAction", m_closeActionTemplate, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    Ray_TextBubbleBehavior_Template::Ray_TextBubbleBehavior_Template()
    : m_textBoneName(StringID::Invalid)
    , m_sleepActionTemplate(NULL)
    , m_openActionTemplate(NULL)
    , m_idleActionTemplate(NULL)
    , m_closeActionTemplate(NULL)
    {
    }

    Ray_TextBubbleBehavior_Template::~Ray_TextBubbleBehavior_Template()
    {
        SF_DEL(m_sleepActionTemplate);
        SF_DEL(m_openActionTemplate);
        SF_DEL(m_idleActionTemplate);
        SF_DEL(m_closeActionTemplate);
    }
}

