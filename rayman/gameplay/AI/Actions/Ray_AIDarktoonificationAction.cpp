#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIDARKTOONIFICATIONACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIDarktoonificationAction.h"
#endif //_ITF_RAY_AIDARKTOONIFICATIONACTION_H_

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonificationAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIDarktoonificationAction_Template)
        SERIALIZE_MEMBER("trajectoryDuration", m_trajectoryDuration);
        SERIALIZE_MEMBER("gravityMultiplier", m_gravityMultiplier);
        SERIALIZE_MEMBER("gravityMultiplierAfterFail", m_gravityMultiplierAfterFail);
        SERIALIZE_MEMBER("initialSpeedAfterFail", m_initialSpeedAfterFail);
        SERIALIZE_MEMBER("tangentPhaseLimit", m_tangentPhaseLimit);
        SERIALIZE_MEMBER("maxTargetHeightDifference", m_maxTargetHeightDifference);
        SERIALIZE_MEMBER("maxTargetDistance", m_maxTargetDistance);
    END_SERIALIZATION()

        Ray_AIDarktoonificationAction_Template::Ray_AIDarktoonificationAction_Template()
        : m_trajectoryDuration(0.83f)
        , m_gravityMultiplier(0.5f)
        , m_gravityMultiplierAfterFail(2.f)
        , m_initialSpeedAfterFail(10.f)
        , m_tangentPhaseLimit(0.5f)
        , m_maxTargetHeightDifference(2.99f)
        , m_maxTargetDistance(3.0f)
    {

    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonificationAction)

        Ray_AIDarktoonificationAction::Ray_AIDarktoonificationAction()
        : m_physComponent(NULL)
        , m_trajectoryTimer(0.0f)
        , m_justEntered(btrue)
        , m_previousGravityMultiplier(0.0f)
        , m_hasJumped(bfalse)
        , m_time(0.0f)
        , m_hitPerformed(bfalse)
        , m_falling(bfalse)
        , m_landed(bfalse)
        , m_victimPosSet(bfalse)
        , m_hitVictim (bfalse)
    {
    }

    void Ray_AIDarktoonificationAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventUndarktoonify,2297042636),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608),m_aiComponent);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        ITF_ASSERT(m_physComponent);
    }

    void Ray_AIDarktoonificationAction::onActivate()
    {
        Super::onActivate();

        m_justEntered = btrue;
        m_time = 0.0f;
        m_hasJumped = bfalse;
        m_hitPerformed = bfalse;
        m_falling = bfalse;
        m_landed = bfalse;
        m_hitVictim =bfalse;

        m_previousGravityMultiplier = m_physComponent->getGravityMultiplier();

        if (!m_victimPosSet)
        {
            Ray_EventQueryDarktoonificationPosition query;
            Actor * victim = static_cast<ActorRef>(m_targetVictim).getActor();
            if (!victim )
            {
                //m_aiBehavior->onActionFinished();
                //return;

                m_victimPos = m_actor->get2DPos() + Vec2d::Up;
            }
            else
            {
                victim->onEvent(&query);

                //GFX_ADAPTER->drawDBGCircle(query.getPos().m_x, query.getPos().m_y, 0.1f, 1,1,1,0.5);
                m_victimPos = query.getPos().truncateTo2D();
                Vec2d diff = m_victimPos - m_actor->getBase().truncateTo2D();
                if ( diff.m_y > getTemplate()->getMaxTargetHeightDifference() || f32_Abs(diff.m_y) > getTemplate()->getMaxTargetDistance())
                    m_victimPos = m_actor->get2DPos() + Vec2d::Up;
            }

        }
        m_vecToVictim = m_victimPos - m_actor->get2DPos();

        if (m_victimPosSet)
        {
            jump();
        }


    }

    void Ray_AIDarktoonificationAction::onDeactivate()
    {
        Super::onDeactivate();

        m_physComponent->setGravityMultiplier(m_previousGravityMultiplier);
        m_victimPosSet = bfalse;
    }

    void Ray_AIDarktoonificationAction::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            processAnimEvent(animEvent);
        }
        else if ( EventHitSuccessful* eventDT = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)) )
        {
            onDarktoonify(eventDT);
        }
        else if ( Ray_EventUndarktoonify* eventUnDT = _event->DynamicCast<Ray_EventUndarktoonify>(ITF_GET_STRINGID_CRC(Ray_EventUndarktoonify,2297042636)) )
        {
            onUndarktoonify(eventUnDT);
        }
        else if ( EventStimNotify * _onStimNotify = _event->DynamicCast<EventStimNotify>(ITF_GET_STRINGID_CRC(EventStimNotify,1337973749)))
        {
            Actor * victim = AIUtils::getActor(_onStimNotify->getSender());
            if (victim && AIUtils::isEnemy(m_aiComponent->getFaction(),AIUtils::getFaction(victim)))
                m_hitVictim = btrue;
        }
        else if (EventInteractionQuery * query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)))
        {
            query->setInteraction(CharacterInteractionType_None);
        }
    }

    void Ray_AIDarktoonificationAction::update( f32 _dt )
    {
        if (m_justEntered)
        {
            m_justEntered = bfalse;
            return;
        }

        //Super::update(_dt);

        if (m_landed)
        {
            if ( isAnimFinished() )
            {
                m_aiBehavior->onActionFinished();
            }
        }
        else if (m_falling)
        {
            if (m_physComponent->getStickedEdge())
            {
                m_landed = btrue;
                //m_falling = bfalse;
            }
        }
        else if (m_hasJumped)
        {
            updateJump(_dt);
        }
        //else
        //{
        //    // until we jump, we can still cancel if the victim disappears
        //    Actor * victim = AIUtils::getActor(m_targetVictim);
        //    if (victim == NULL)
        //        m_aiBehavior->onActionFinished();
        //}
    }

    void Ray_AIDarktoonificationAction::processAnimEvent( AnimGameplayEvent* _animEvent )
    {
        if ( _animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Jump,868678677) )
        {
            ITF_ASSERT(!m_hasJumped);
            if (!m_hasJumped)
            {
                jump();
            }
        }
        else if ( _animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Antigrav_Start,2483614339) )
        {
            m_physComponent->setSpeed(Vec2d::Zero);
            m_physComponent->setGravityMultiplier(0);
        }
        else if ( _animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Antigrav_Stop,3588802278) )
        {
            m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplierAfterFail());
            m_physComponent->setSpeed(getTemplate()->getInitialSpeedAfterFail()* m_aiComponent->getCurrentGravityDir());
            m_falling = btrue;
        }
    }

    void Ray_AIDarktoonificationAction::jump()
    {
        //Actor* victim = AIUtils::getActor(m_targetVictim);
        //if (victim == NULL)
        //{
        //    m_aiBehavior->onActionFinished();
        //    return;
        //}

        //Ray_AIGroundAttackBehavior* groundBehavior = m_aiBehavior->DynamicCast<Ray_AIGroundAttackBehavior>(ITF_GET_STRINGID_CRC(Ray_AIGroundAttackBehavior,1320632199));
        //ITF_ASSERT(groundBehavior);
        //if (!groundBehavior || !groundBehavior->isVictimInRange())
        //{
        //    m_aiBehavior->onActionFinished();
        //    return;
        //}


        Vec2d impulse;
        impulse.m_x =  m_vecToVictim.m_x / getTemplate()->getTrajectoryDuration();
        impulse.m_y = (m_vecToVictim.m_y / getTemplate()->getTrajectoryDuration() ) - 0.5f * getTemplate()->getGravityMultiplier() * PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth()).m_y * getTemplate()->getTrajectoryDuration();


        m_hasJumped = btrue;

        m_trajectoryTimer = getTemplate()->getTrajectoryDuration();
        m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplier());

        m_physComponent->unstick();
        m_physComponent->setSpeed(impulse);

        m_lastPosition = m_actor->get2DPos();
    }

    void Ray_AIDarktoonificationAction::updateJump( f32 _dt )
    {
        m_time = 1.0f - (m_trajectoryTimer / getTemplate()->getTrajectoryDuration());
        m_trajectoryTimer = Max(0.0f, m_trajectoryTimer - _dt);

        if (m_time > getTemplate()->getTangentPhaseLimit() && !m_hitVictim)
        {
            performHit();
        }

        if (m_trajectoryTimer == 0.0f)
        {
            m_hitPerformed = btrue;
        }

        if (m_time < getTemplate()->getTangentPhaseLimit())
        {
            const Vec2d& pos = m_actor->get2DPos();
            Vec2d dir = (m_lastPosition - pos).normalize();
            m_actor->setAngle( dir.getAngle() + (m_actor->isFlipped() ? 0.f : MTH_PI) );
            //GFX_ADAPTER->drawDBGLine(m_lastPosition, pos, Color::white(), 2.f);
            m_lastPosition = pos;
        }
        else
        {
            m_actor->setAngle(0);
        }
    }


    void Ray_AIDarktoonificationAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(HasJumped,837498456), m_hasJumped);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(JumpCursor,3071503411), m_time);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(HitPerformed,2337862358), m_hitPerformed);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Falling,2990802452), m_falling);
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Landed,3584962097), m_landed);
    }

    void Ray_AIDarktoonificationAction::onDarktoonify( EventHitSuccessful* _eventDT )
    {
        // we might have hit someone else than who we were targeting...
        Actor * actor = ActorRef(_eventDT->getSender()).getActor();
        if (actor && isValid(actor,actor))
            m_actor->disable();
    }

    void Ray_AIDarktoonificationAction::onUndarktoonify( Ray_EventUndarktoonify* _eventUnDT )
    {
        m_actor->enable();

        if (m_actor->getBinding()->isBound())
        {
            m_actor->getBinding()->unbindFromParent();
        }

        Actor* victim = AIUtils::getActor(_eventUnDT->getSender());
        ITF_ASSERT_MSG(victim, "darktoon being kicked off but can't find its victim");
        if (victim)
        {
            m_aiBehavior->onActionFinished();   // this will re-enable physics

            // teleport to our ex-victim's head
            Ray_EventQueryDarktoonificationPosition queryPos;
            victim->onEvent(&queryPos);
            m_actor->setPos(queryPos.getPos());

            //// process the hit that kicked us off
            //HitStim* _hit = static_cast<HitStim*>(_eventUnDT->m_innerEvent);
            //_hit->setLevel(2);  // HACK
            //m_aiComponent->onEvent(_hit);

            //GFX_ADAPTER->drawDBGCircle(queryPos.getPos(), 0.3f, Color::red(), 3.f);
            //CHEATMANAGER->togglePause();
        }
    }

    bbool Ray_AIDarktoonificationAction::isValid(Actor * _actor, Actor * _target) const
    {
        ITF_ASSERT(_actor);
        ITF_ASSERT(_target);
        Ray_EventQueryCanDarktoonify query;
        _target->onEvent(&query);
        return query.getCanDarktoonify();
    }
}