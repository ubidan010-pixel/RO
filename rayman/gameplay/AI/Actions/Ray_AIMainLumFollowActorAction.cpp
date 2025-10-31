#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIMAINLUMFOLLOWACTORACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIMainLumFollowActorAction.h"
#endif //_ITF_RAY_AIMAINLUMFOLLOWACTORACTION_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIMainLumFollowActorAction_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIMainLumFollowActorAction_Template)

        SERIALIZE_MEMBER("timeToFlip",m_timeToFlip);
        SERIALIZE_MEMBER("growStopMarker",m_growStopMarker);

    END_SERIALIZATION()

    Ray_AIMainLumFollowActorAction_Template::Ray_AIMainLumFollowActorAction_Template()
    : m_timeToFlip(0.3f)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIMainLumFollowActorAction)

    Ray_AIMainLumFollowActorAction::Ray_AIMainLumFollowActorAction() 
    : m_flipped(bfalse)
    , m_timer(0.0f)
    , m_growing(bfalse)
    , m_growCount(0)
    , m_growStop(bfalse)
    {
    }

    Ray_AIMainLumFollowActorAction::~Ray_AIMainLumFollowActorAction()
    {

    }

    void Ray_AIMainLumFollowActorAction::onActivate()
    {
        Super::onActivate();

        ITF_ASSERT(m_followAnimLight!=NULL);

        Actor * actor = ActorRef(m_followActor).getActor();

        m_flipped = actor && actor->isFlipped();
        m_currentRelativePos = m_relativPos;
        if (m_flipped )
            m_currentRelativePos.m_x = -m_currentRelativePos.m_x;
    }

    void Ray_AIMainLumFollowActorAction::update( f32 _delta )
    {
        if (!m_followAnimLight || !m_physComponent || _delta <= 0.f)
            return;

        Super::update(_delta);


        Actor * actor = ActorRef(m_followActor).getActor();

        bbool flip = actor && actor->isFlipped();

        if (m_flipped != flip)
        {
            m_flipped = flip;
            m_timer = getTemplate()->getTimeToFlip();
        }
        else
        {
            m_timer -= _delta;
        }

        m_timer = Max(m_timer,0.0f);
        f32 sign = m_flipped?1.0f:-1.0f;
        m_currentRelativePos.m_x += sign * (1.0f - m_timer/getTemplate()->getTimeToFlip()) * (2 * m_relativPos.m_x);
        m_currentRelativePos.m_x = Clamp(m_currentRelativePos.m_x ,m_relativPos.m_x,-m_relativPos.m_x);

        if (!m_growing && m_growCount > 0)
        {
            m_growing  = btrue;
            m_growCount--;
        }
        else if (m_growing && m_growStop)
        {
            m_growing = bfalse;
            m_growStop = bfalse;
        }

    }

    Vec3d Ray_AIMainLumFollowActorAction::getFollowPos() const 
    {
        Actor * followActor     = getFollowActor();

        ITF_ASSERT(followActor!=NULL);

        if ( followActor )
        {
            return followActor->getPos() + m_currentRelativePos;
        }
        else
        {
            return m_actor->getPos();
        }
    }

    void Ray_AIMainLumFollowActorAction::onEvent(Event * _event)
    {
        if (AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (animEvent->getName() == getTemplate()->getGrowStopMarker())
            {
                m_growStop = btrue;
            }
        }
    }


    void Ray_AIMainLumFollowActorAction::updateAnimInputs()
    {
        Super::updateAnimInputs();
        m_animatedComponent->setInput("Grow",(u32)m_growing);
    }

    void Ray_AIMainLumFollowActorAction::clear()
    {

        m_flipped  = bfalse;
        m_timer = 0.0f;
        m_growing = bfalse;
        m_growCount = 0;
        m_growStop = bfalse;

    }
}