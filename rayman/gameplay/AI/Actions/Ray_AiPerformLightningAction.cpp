#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIPERFORMLIGHTNINGACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AiPerformLightningAction.h"
#endif //_ITF_RAY_AIPERFORMLIGHTNINGACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


#ifndef _ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_LightningPatchAIComponent.h"
#endif //_ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AiPerformLightningAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AiPerformLightningAction_Template)
    END_SERIALIZATION()

        Ray_AiPerformLightningAction_Template::Ray_AiPerformLightningAction_Template()
        : Super()
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(HIT,2856859763);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AiPerformLightningAction)
    BEGIN_VALIDATE_ACTION(Ray_AiPerformLightningAction)
    VALIDATE_ACTION_PARAM("lightning", m_lightningPatchComponent, "requires Lightning Patch component");
    END_VALIDATE_ACTION()

        Ray_AiPerformLightningAction::Ray_AiPerformLightningAction()
        : Super()
        , m_lightningPatchComponent(NULL)
    {
    }

    Ray_AiPerformLightningAction::~Ray_AiPerformLightningAction()
    {
    }


    void Ray_AiPerformLightningAction::update( f32 _dt )
    {
        Super::update(_dt);

        if ( isAnimFinished() )
        {
            m_aiBehavior->onActionFinished();
        }
    }

    void Ray_AiPerformLightningAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent* gEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if (gEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Flame_Start,491367522))
            {
                m_lightningPatchComponent->open();
            }
            else if (gEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Flame_Stop,959055747))
            {
                m_lightningPatchComponent->close(bfalse);
            }
            else if (gEvent->getName() == ITF_GET_STRINGID_CRC(MRK_charge_Start,1682453774))
            {
                m_lightningPatchComponent->charge();
            }
        }
    }

    void Ray_AiPerformLightningAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);

        m_lightningPatchComponent = m_actor->GetComponent<Ray_LightningPatchAIComponent>();
    }


    void Ray_AiPerformLightningAction::onDeactivate()
    {
        Super::onDeactivate();
        m_lightningPatchComponent->close(btrue);

    }

}