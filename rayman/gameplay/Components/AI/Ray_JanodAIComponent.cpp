#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_JANODAICOMPONENT_H__
#include "rayman/gameplay/Components/AI/Ray_JanodAIComponent.h"
#endif //_ITF_RAY_JANODAICOMPONENT_H__

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIDeathBehavior.h"
#endif //_ITF_RAY_AIDEATHBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_JanodAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_JanodAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("startsAtApex", m_startsAtApex);
            SERIALIZE_MEMBER("delayBeforeStarting", m_delayBeforeStarting);
            SERIALIZE_MEMBER("mustWaitForTrigger", m_mustWaitForTrigger);
            SERIALIZE_MEMBER("canFallWhenSleeping", m_canFallWhenSleeping);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_JanodAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_JanodAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior", m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("crushedBehavior", m_crushedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("squashPenetrationRadius", m_squashPenetrationRadius);
        SERIALIZE_MEMBER("canRehit", m_canRehit );
    END_SERIALIZATION();

    Ray_JanodAIComponent_Template::Ray_JanodAIComponent_Template() 
        : m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_squashPenetrationRadius(0.6f)
        , m_canRehit( bfalse )
    {}

    Ray_JanodAIComponent_Template::~Ray_JanodAIComponent_Template()
    {
        SF_DEL(m_receiveHitBehavior);
        SF_DEL(m_deathBehavior);
        SF_DEL(m_crushedBehavior);

    }
 
    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_JanodAIComponent::Ray_JanodAIComponent() : 
        Ray_FixedAIComponent() 
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_startsAtApex(bfalse)
        , m_delayBeforeStarting(0)
        , m_mustWaitForTrigger(bfalse)
        , m_canFallWhenSleeping(bfalse)
        , m_howLongWasCrushed(FLT_MAX)
        , m_isInBristleMode(bfalse)
        {}

    Ray_JanodAIComponent::~Ray_JanodAIComponent()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_JanodAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_receiveHitBehavior = static_cast<Ray_AIReceiveHitBehavior *>(createAiBehavior(getTemplate()->getReceiveHitBehavior()));
        m_deathBehavior = createAiBehavior(getTemplate()->getDeathBehavior());
        m_crushedBehavior = createAiBehavior(getTemplate()->getCrushedBehavior());

        Super::onActorLoaded(_hotReload);

        if (m_physComponent)
        {
            m_physComponent->setDisableWindForces(btrue);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_JanodAIComponent::startDeath()
    {
        setHealth(0);
        setBehavior(m_deathBehavior);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void  Ray_JanodAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if (m_currentBehavior == m_crushedBehavior)
        {
            startDeath();
        }
        else if (m_currentBehavior == m_receiveHitBehavior)
        {
            if (getHealth()<=0)
            {
                startDeath();
            }
            else
            {
                setBehavior(m_behavior);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_JanodAIComponent::Update( f32 _dt )
    {

        m_howLongWasCrushed += _dt;

        if (m_currentBehavior != m_deathBehavior)
        {

            //water death
            PolyLine* waterPoly = AIUtils::getPolylineRegion(m_actor->getDepth(),s_RegionName_Swim,m_actor->get2DPos());
            if ( waterPoly )
            {
                if (m_physComponent)
                {
                    Vec2d   pos         = m_actor->get2DPos();
                    f32     radius      = m_physComponent->getRadius();
                    f32     perturbation= m_physComponent->getWaterMultiplier()*m_physComponent->getSpeed().norm()*10;

                    EventSwimingPlouf plouf(pos, perturbation, radius, bfalse);
                    Pickable *destination = (Pickable*)waterPoly->m_ownerFrise;
                    if (destination)
                    {                        
                        destination->onEvent(&plouf);
                    }
                }

                startDeath();
                return;
            }
            //squash death
            if (isSquashAuthorized())
            {
                if (Ray_AIUtils::getSquashingPolyline(m_actor,m_physComponent->getStickedPolylineRef(),m_physComponent->getStickedEdge(),m_physComponent->getRadius(),getTemplate()->getSquashPenetrationRadius() * m_physComponent->getRadius()))
                {
                    startDeath();
                return;
            }
        }


        }
        Super::Update(_dt);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_JanodAIComponent::processCrush(EventCrushed *crushed)
    {
        receiveCrush(crushed);
        playLastHitFx();

        if (!m_crushedBehavior)
        {
            return;
        }
        if (crushed->getCharacterSize() != CHARACTERSIZE_SMALL)
        {
            if ( (m_currentBehavior != m_deathBehavior) 
                && (m_currentBehavior!=m_receiveHitBehavior) 
                && (m_currentBehavior!=m_crushedBehavior) )
            {
                if (m_physComponent)
                {
                    m_physComponent->setSpeed(Vec2d::Zero);
                }
                setBehavior(m_crushedBehavior);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_JanodAIComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        if (m_currentBehavior == m_behavior) //WHEN IN STD BEHAVIOR
        {
            //cannot happen in bristle mode
            if (!m_isInBristleMode)
            {
                if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
                {
                    processHit(hit);
                }
                else if ( EventInteractionQuery* interactionQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
                {
                    if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
                    {
                        Actor *sender = IRTTIObject::SafeDynamicCast<Actor>(interactionQuery->getSender().getObject(), ITF_GET_STRINGID_CRC(Actor,2546623115));
                        if (sender && (sender->getPos().m_y>m_actor->getPos().m_y) )
                        {
                            interactionQuery->setInteraction(CharacterInteractionType_Crush);
                        }
                    }
                }
                else if (EventCrushed *crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)))
                {
                    m_howLongWasCrushed = 0;

                    processCrush(crushed);
                }
            }
            //End of condition in bristling mode
        }
        else if (
                    (m_currentBehavior == m_deathBehavior) 
                    || (m_currentBehavior==m_receiveHitBehavior) 
                    || (m_currentBehavior==m_crushedBehavior)
                 ) ////WHEN IN DEATH OR CLOSE TO
        {
            if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
            {
                if (m_currentBehavior==m_receiveHitBehavior)
                {
                    processHit(hit);
                }
            }
            else if ( EventInteractionQuery* interactionQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
            {
                //NB : special crush case is handled by death behavior
                if ( RAY_GAMEMANAGER->getAllowNpcCrush() && (m_currentBehavior != m_deathBehavior))
                {
                    interactionQuery->setInteraction(CharacterInteractionType_Crush);
                }
            }
            else if (EventCrushed *crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)))
            {
                receiveCrush(crushed);
                playLastHitFx();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_JanodAIComponent::processHit(PunchStim * _hit)
    {
        if (!m_receiveHitBehavior)
            return;
        
        if (m_currentBehavior == m_receiveHitBehavior)
        {
            if ( !m_receiveHitBehavior->canReceiveHit(_hit))
            {
                bbool isDominoHit = _hit->IsClassCRC(ITF_GET_STRINGID_CRC(RehitStim,1008690800));
                if (isDominoHit)
                    return;
                else
                {
                    startDeath();
                    return;
                }
            }
            else if ( getTemplate()->getCanRehit() )
            {
                setBehavior(m_receiveHitBehavior);
                m_receiveHitBehavior->setData(_hit);
            }
        }
        else
        {
            setBehavior(m_receiveHitBehavior);
            m_receiveHitBehavior->setData(_hit);
        }
        
        // notify the sender that he touched us
        Actor* sender = static_cast<Actor*>(_hit->getSender().getObject());
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender( m_actor->getRef() );
            hitSuccessful.setHasBeenDamaged( btrue );
            sender->onEvent(&hitSuccessful);
        }
    }
}