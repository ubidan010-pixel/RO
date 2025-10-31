#include "precompiled_gameplay.h"

#ifndef _ITF_AIUTURNACTION_H_
#include "gameplay/ai/Actions/AIUTurnAction.h"
#endif //_ITF_AIUTURNACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIUTurnAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIUTurnAction_Template)
    END_SERIALIZATION()
    AIUTurnAction_Template::AIUTurnAction_Template()
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(UTURN,3271188893);
    }

    IMPLEMENT_OBJECT_RTTI(AIUTurnAction)

    AIUTurnAction::AIUTurnAction()
    {
        m_justEntered = btrue;
    }

    AIUTurnAction::~AIUTurnAction()
    {
    }

    void AIUTurnAction::update( f32 _dt )
    {
        if (m_justEntered)
        {
            m_justEntered = bfalse;
        }
        else
        {
            if ((getAnimAction().isValid() && isAnimFinished()) || !getAnimAction().isValid())
            {
                //            m_animComponent->setFlipped(!m_actor->isFlipped());
                m_aiBehavior->onActionFinished();
            }
        }
        
    }

    void AIUTurnAction::onActivate()
    {
        Super::onActivate();
        m_justEntered = btrue;
    }

}