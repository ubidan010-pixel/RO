#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGEYSERACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIGeyserAction.h"
#endif //_ITF_RAY_AIGEYSERACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGeyserAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGeyserAction_Template)
    END_SERIALIZATION()

        Ray_AIGeyserAction_Template::Ray_AIGeyserAction_Template()
        : Super()
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AiGeyserAction)

        Ray_AiGeyserAction::Ray_AiGeyserAction()
        : Super()
        , m_performHit(bfalse)
        , m_geyserComponent(NULL)
    {
    }

    Ray_AiGeyserAction::~Ray_AiGeyserAction()
    {
    }


    void Ray_AiGeyserAction::update( f32 _dt )
    {
        Super::update(_dt);
        if ( isAnimFinished() )
        {
            m_aiBehavior->onActionFinished();
            return;

        }

        if (m_performHit)
        {
        }
    }

    void Ray_AiGeyserAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Wind_Start,2200866592) )
            {
                m_performHit = btrue;
                m_geyserComponent->open();
            }
            else if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Wind_Stop,2638136289) )
            {
                m_performHit = bfalse;
                m_geyserComponent->close();
            }
        }
    }

    void Ray_AiGeyserAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);

        m_geyserComponent = m_actor->GetComponent<Ray_GeyserPlatformAIComponent>();
        m_geyserComponent->close();
    }

    void Ray_AiGeyserAction::onActivate()
    {
        Super::onActivate(  );

        m_performHit = bfalse;
    }

    void Ray_AiGeyserAction::onDeactivate()
    {
        Super::onDeactivate(  );
        m_geyserComponent->close();
    }

}