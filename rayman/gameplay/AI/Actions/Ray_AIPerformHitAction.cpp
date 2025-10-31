#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIPERFORMHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitAction.h"
#endif //_ITF_RAY_AIPERFORMHITACTION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitAction_Template)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(Ray_AIPerformHitAction_Template)
        SERIALIZE_MEMBER("level", m_hitLevel);
        SERIALIZE_ENUM_BEGIN("type", m_hitType);
        SERIALIZE_ENUM_VAR(RECEIVEDHITTYPE_FRONTPUNCH);
        SERIALIZE_ENUM_VAR(RECEIVEDHITTYPE_UPPUNCH);
        SERIALIZE_ENUM_VAR(RECEIVEDHITTYPE_EJECTXY);
        SERIALIZE_ENUM_VAR(RECEIVEDHITTYPE_DARKTOONIFY);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("marker", m_performHitMarker);
        SERIALIZE_MEMBER("usesAdditive", m_usesAdditive);
    END_SERIALIZATION()

        Ray_AIPerformHitAction_Template::Ray_AIPerformHitAction_Template()
        : Super()
        , m_usesAdditive(bfalse)
        , m_hitLevel(0)
        , m_hitType(RECEIVEDHITTYPE_FRONTPUNCH)
        , m_canBeCrushed(bfalse)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(HIT,2856859763);
        m_performHitMarker = MRK_Perform_Hit;
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitAction)

        Ray_AIPerformHitAction::Ray_AIPerformHitAction()
        : Super()
        , m_performHit(bfalse)
    {

    }

    Ray_AIPerformHitAction::~Ray_AIPerformHitAction()
    {
    }


    void Ray_AIPerformHitAction::update( f32 _dt )
    {
        Super::update(_dt);
        if ( m_performHit )
        {
            performHit();
            m_performHit = bfalse;
        }

        if ( !getTemplate()->getUsesAdditive() && isAnimFinished() )
        {
            m_aiBehavior->onActionFinished();
        }
    }

    void Ray_AIPerformHitAction::onEvent( Event* _event)
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( animEvent->getName() == getTemplate()->getPerformHitMarker() )
            {
                m_performHit = btrue;
            }
        }
        /*
        else if (EventQueryCanReceiveHit * query = _event->DynamicCast<EventQueryCanReceiveHit>(ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316)))
        {
            if (query->getStim()->getLevel() <= getTemplate()->getHitLevel())
                query->setCanReceiveHit(bfalse);
            else
                query->setCanReceiveHit(btrue);
        }
        */
        else if (EventInteractionQuery * _query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)))
        {
            if (getTemplate()->getCanBeCrushed())
            {
                _query->setInteraction(CharacterInteractionType_Crush);
            }
        }
        else if (EventSetOriginalSender * setOriginalSenderEvt = _event->DynamicCast<EventSetOriginalSender>(ITF_GET_STRINGID_CRC(EventSetOriginalSender,1174287572)))
        {
            m_originalSender = setOriginalSenderEvt->get();
        }
    }

    void Ray_AIPerformHitAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetOriginalSender,1174287572),m_aiComponent);
    }
}