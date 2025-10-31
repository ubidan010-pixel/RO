#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOULDERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BoulderAIComponent.h"
#endif //_ITF_RAY_BOULDERAICOMPONENT_H_

#ifndef _ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Boulder/Ray_AIBoulderSentBackBehavior.h"
#endif //_ITF_RAY_AIBOULDERSENTBACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Fruit/Ray_AIFruitRoamingBehavior.h"
#endif //_ITF_RAY_AIFRUITROAMINGBEHAVIOR_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_BoulderAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BoulderAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BoulderAIComponent)
        VALIDATE_COMPONENT_PARAM("Ray_StimComponent", m_stimComponent, "Component mandatory");
    END_VALIDATE_COMPONENT()

    //------------------------------------------------------------------
    Ray_BoulderAIComponent::Ray_BoulderAIComponent()
    : m_sentBackBehavior(NULL)
    , m_stimComponent(NULL)
    , m_timeSinceLastHitFX(F32_INFINITY)
    , m_lifeTime(0.0f)
    , m_destructionCountDown(0.0f)
    , m_senderDead(bfalse)
    , m_hitSent(bfalse)
    , m_hitEnvironment(bfalse)
    , m_destroyingHit(bfalse)
    {
    }

    //------------------------------------------------------------------
    Ray_BoulderAIComponent::~Ray_BoulderAIComponent()
    {
    }

    //------------------------------------------------------------------
    void Ray_BoulderAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_sentBackBehavior = static_cast<Ray_AIBoulderSentBackBehavior *>(createAiBehavior(getTemplate()->getSentBackBehavior()));

        Super::onActorLoaded(_hotReload);

        //register to events
        ACTOR_REGISTER_EVENT_COMPONENT(GetActor(), ITF_GET_STRINGID_CRC(Ray_EventDyingStoneMan,1834526668), this );
        ACTOR_REGISTER_EVENT_COMPONENT(GetActor(), ITF_GET_STRINGID_CRC(EventStimNotify,1337973749), this );

        //get components
        m_stimComponent = GetActor()->GetComponent<Ray_StimComponent>();
    }

    //------------------------------------------------------------------
    void Ray_BoulderAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        m_stimComponent->setDisabled(btrue);
    }

    //------------------------------------------------------------------
    void Ray_BoulderAIComponent::Update(f32 _dt)
    {
        Super::Update(_dt);

        //check stim component reactivation
        if( m_lifeTime > getTemplate()->getNoStimPeriod() && m_stimComponent->getDisabled() )
        {
            m_stimComponent->setDisabled(bfalse);
        }

        //update sent back behavior
        if( isCurrentBehavior(m_sentBackBehavior) )
        {
            updateSentBack();
        }

        //update timers
        m_timeSinceLastHitFX += _dt;
        m_lifeTime += _dt;
        if( m_senderDead )
        {
            m_destructionCountDown -= _dt;
        }
    }

    //------------------------------------------------------------------
    void Ray_BoulderAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);
        
        if(const Ray_EventDyingStoneMan * const dyingStoneManEvent = _event->DynamicCast<Ray_EventDyingStoneMan>(ITF_GET_STRINGID_CRC(Ray_EventDyingStoneMan,1834526668)))
        {
            m_senderDead = btrue;
            m_destructionCountDown = getTemplate()->getMaxLifeTimeAfterSenderDeath();
        }
        else if(const EventStimNotify * const stimSentEvent = _event->DynamicCast<EventStimNotify>(ITF_GET_STRINGID_CRC(EventStimNotify,1337973749)))
        {
            //check if stim notifier is not dead
            Actor * const sender = stimSentEvent->getSender().getObject()->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if( sender )
            {
                EventQueryIsDead query;
                query.setSender(GetActor()->getRef());
                sender->onEvent(&query);
                m_hitSent = !query.getIsDead();
            }
        }
    }

    //------------------------------------------------------------------
    void Ray_BoulderAIComponent::processHit(PunchStim *_hit)
    {
        ITF_ASSERT(_hit);

        if( m_fxController && (m_timeSinceLastHitFX > 0.5f) )
        {
            const StringID feedbackAction = _hit->getActionFromHit();
            u32 fxHandle = m_fxController->playFeedback(_hit->getSender(), feedbackAction, m_actor->getRef());
            m_fxController->setFXPos(fxHandle, _hit->getFxPos());
            m_timeSinceLastHitFX = 0.0f;
        }

        if( _hit->getFaction() == AIUtils::getFaction(GetActor()))
        {
            m_destroyingHit = btrue;
        }
        else
        {
            startSentBack(_hit);
        }
    }

    //------------------------------------------------------------------
    void Ray_BoulderAIComponent::startSentBack(const PunchStim * const _hit)
    {
        ITF_ASSERT(_hit);

        //default hit direction is direction found on hit
        Vec2d hitDirection = _hit->getDirection();
        bbool tornadoAttack = bfalse;
        //compute pos difference based direction if we are able to (it solves some hit direction strange values?)
        Actor * const sender = _hit->getSender().getObject()->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        if( sender )
        {
            const Vec2d difference = (GetActor()->get2DPos() - sender->get2DPos());
            const f32 distance = difference.norm();
            if( distance > 0.0f )
            {
                hitDirection = difference / distance;
            }

            Ray_EventQueryHitType hitTypeQuery;
            sender->onEvent(&hitTypeQuery);
            tornadoAttack = (hitTypeQuery.getHitType() == PUNCHTYPE_TORNADO);
        }
        if( !tornadoAttack )
        {
            m_sentBackBehavior->setSpeedValue(getTemplate()->getSentBackNormalSpeed());
        }
        else
        {
            m_sentBackBehavior->setSpeedValue(getTemplate()->getSentBackTornadoSpeed());
        }
        m_sentBackBehavior->setDirection(hitDirection);
        setBehavior(m_sentBackBehavior, btrue);
    }

    //------------------------------------------------------------------
    void Ray_BoulderAIComponent::updateSentBack()
    {
        const PhysShape * const shape = m_phantomComponent->getShape();
        if( shape )
        {
            const Vec2d & position = m_actor->get2DPos();
            const f32 angle = m_actor->getAngle();
            const f32 depth = m_actor->getDepth();

            PhysContactsContainer contacts;
            m_hitEnvironment = PHYSWORLD->checkEncroachment(position, position, angle, shape, ECOLLISIONFILTER_ENVIRONMENT, depth, contacts);
        }
    }

    //------------------------------------------------------------------
    bbool Ray_BoulderAIComponent::isInDeathCondition() const
    {
        //check destroying hit
        if( m_destroyingHit )
        {
            return btrue;
        }
        //check time in sent back behavior
        if( m_currentBehavior == m_sentBackBehavior )
        {
            const f32 maxTime = getTemplate()->getMaxSentBackDuration();
            if( maxTime >= 0.0f && m_sentBackBehavior->getTimer() > maxTime )
            {
                return btrue;
            }
            else
            {
                return m_hitEnvironment;
            }
        }
        if( m_currentBehavior == m_roamBehavior )
        {
            if( m_roamBehavior->getBouncesCount() == getTemplate()->getMaxBouncesCount() )
            {
                if( m_roamBehavior->getTimeSinceLastBounce() >= getTemplate()->getTimeAfterLastBounce() )
                {
                    return btrue;
                }
            }
            else if( m_roamBehavior->getBouncesCount() > getTemplate()->getMaxBouncesCount() )
            {
                return btrue;
            }
            else if( m_roamBehavior->getTimeSinceLastBounce() > getTemplate()->getMaxTimeWithoutBounce() )
            {
                return btrue;
            }
        }
        if( m_senderDead )
        {
            if( m_lifeTime < getTemplate()->getImmediateExplosionOnSenderDeathPeriod() )
            {
                return btrue;
            }
            else if ( m_destructionCountDown <= 0.0f )
            {
                return btrue;
            }
        }
        if( m_hitSent )
        {
            return btrue;
        }
        return Super::isInDeathCondition();
    }





    //------------------------------------------------------------------
    // Template
    //------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BoulderAIComponent_Template)
    
    BEGIN_SERIALIZATION_CHILD(Ray_BoulderAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("sentBackBehavior", m_sentBackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("maxSentBackDuration", m_maxSentBackDuration);
        SERIALIZE_MEMBER("maxBouncesCount", m_maxBouncesCount);
        SERIALIZE_MEMBER("timeAfterLastBounce", m_timeAfterLastBounce);
        SERIALIZE_MEMBER("maxTimeWithoutBounce", m_maxTimeWithoutBounce);
        SERIALIZE_MEMBER("maxTimeAfterSenderDeath", m_maxTimeAfterSenderDeath);
        SERIALIZE_MEMBER("immediateExplosionOnSenderDeathPeriod", m_immediateExplosionOnSenderDeathPeriod);
        SERIALIZE_MEMBER("noStimPeriod", m_noStimPeriod);
        SERIALIZE_MEMBER("sentbackNormalSpeed", m_sentbackNormalSpeed);
        SERIALIZE_MEMBER("sentbackTornadoSpeed", m_sentbackTornadoSpeed);
    END_SERIALIZATION()

    //------------------------------------------------------------------
    Ray_BoulderAIComponent_Template::Ray_BoulderAIComponent_Template()
    : m_sentBackBehavior(NULL)
    , m_maxSentBackDuration(-1.0f)
    , m_maxBouncesCount(3)
    , m_timeAfterLastBounce(0.5f)
    , m_maxTimeWithoutBounce(3.0f)
    , m_maxTimeAfterSenderDeath(2.0f)
    , m_immediateExplosionOnSenderDeathPeriod(0.5f)
    , m_noStimPeriod(0.5f)
    , m_sentbackNormalSpeed(10.0f)
    , m_sentbackTornadoSpeed(20.0f)
    {
    }

    //------------------------------------------------------------------
    Ray_BoulderAIComponent_Template::~Ray_BoulderAIComponent_Template()
    {
        SF_DEL(m_sentBackBehavior);
    }

} // namespace ITF

