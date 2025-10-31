#include "precompiled_gameplay.h"

#ifndef _ITF_AIDISABLEACTION_H_
#include "gameplay/ai/Actions/AIDisableAction.h"
#endif //_ITF_AIDISABLEACTION_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIDisableAction_Template)
    IMPLEMENT_OBJECT_RTTI(AIDisableAction)

    BEGIN_SERIALIZATION(AIDisableAction_Template)
    END_SERIALIZATION()
   
    void AIDisableAction::onActivate()
    {
        m_actor->disable();
    }    
}