#include "precompiled_gameplay.h"

#ifndef _ITF_AIFADEACTION_H_
#include "gameplay/ai/Actions/AIFadeAction.h"
#endif //_ITF_AIFADEACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIFadeAction_Template)
    BEGIN_SERIALIZATION_CHILD(AIFadeAction_Template)
        SERIALIZE_MEMBER("visible", m_visible);
        SERIALIZE_MEMBER("fadeDuration", m_fadeDuration);
    END_SERIALIZATION()

    AIFadeAction_Template::AIFadeAction_Template()
    : m_visible(bfalse)
    , m_fadeDuration(1)
    {
    }

    AIFadeAction_Template::~AIFadeAction_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(AIFadeAction)

    AIFadeAction::AIFadeAction()
        : Super()
        , m_timer(0)
    {
    }

    AIFadeAction::~AIFadeAction()
    {
    }

    void AIFadeAction::onActivate()
    {
        Super::onActivate();

        EventShow evtShow(getTemplate()->getVisible()?1.f:0.f, getTemplate()->getFadeDuration());
        m_actor->onEvent(&evtShow);
        m_timer = 0;
    }

    void AIFadeAction::update( f32 _delta )
    {
        Super::update(_delta);

        m_timer += _delta;
        if (m_timer > getTemplate()->getFadeDuration())
        {
            m_aiBehavior->onActionFinished();
        }
    }

}