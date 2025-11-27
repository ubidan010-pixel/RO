#include "precompiled_gameplay.h"

#ifndef _ITF_AIPLAYFXACTION_H_
#include "gameplay/ai/Actions/AIPlayFXAction.h"
#endif //_ITF_AIPLAYFXACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIPlayFXAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIPlayFXAction_Template)
        SERIALIZE_MEMBER("fxName", m_effectID);
    END_SERIALIZATION()



    IMPLEMENT_OBJECT_RTTI(AIPlayFXAction)

    AIPlayFXAction::AIPlayFXAction()
        : m_playIndex(U32_INVALID)
    {
    }

    AIPlayFXAction::~AIPlayFXAction()
    {
    }

    void AIPlayFXAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ITF_ASSERT_MSG(m_fxController, "AIPlayFXAction needs an FXControllerComponent");
    }

    void AIPlayFXAction::onActivate()
    {
        Super::onActivate();

        if ( m_fxController )
        {
            m_playIndex = m_fxController->playFX(getTemplate()->getEffectID());
            ITF_ASSERT_MSG(m_playIndex != U32_INVALID, "Invalid FX");
        }
    }

    void AIPlayFXAction::update( f32 _delta )
    {
        Super::update(_delta);

        if ( m_fxController && m_playIndex != U32_INVALID && !m_fxController->isPlayingFromHandle(m_playIndex) )
        {
            m_aiBehavior->onActionFinished();
        }
    }

} // namespace ITF
