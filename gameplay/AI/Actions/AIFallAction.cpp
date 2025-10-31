#include "precompiled_gameplay.h"

#ifndef _ITF_AIFALLACTION_H_
#include "gameplay/ai/Actions/AIFallAction.h"
#endif //_ITF_AIFALLACTION_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIFallAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIFallAction_Template)
    END_SERIALIZATION()

    AIFallAction_Template::AIFallAction_Template()
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(FALL,100613701);
    }
    
        
    IMPLEMENT_OBJECT_RTTI(AIFallAction)

    BEGIN_VALIDATE_ACTION(AIFallAction)
        VALIDATE_ACTION_PARAM("", m_physComponent, "AIFallAction requires a StickToPolylinePhysComponent");
    END_VALIDATE_ACTION()

    AIFallAction::AIFallAction()
        : m_physComponent(NULL)
    {
    }

    AIFallAction::~AIFallAction()
    {
    }

    void AIFallAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    void AIFallAction::update( f32 _delta )
    {
        if ( m_physComponent->getStickedEdge() )
        {
            m_aiBehavior->onActionFinished();
        }
    }

}