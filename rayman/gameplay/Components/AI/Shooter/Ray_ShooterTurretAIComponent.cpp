#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERTURRETAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterTurretAIComponent.h"
#endif //_ITF_RAY_SHOOTERTURRETAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterAttackBehavior.h"
#endif //_ITF_RAY_AISHOOTERATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIReceiveHitBehavior.h"
#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIDeathBehavior.h"
#endif //_ITF_RAY_AIDEATHBEHAVIOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterTurretAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterTurretAIComponent)
        SERIALIZE_OBJECT( "timedSpawnerData",   m_timedSpawnerData );
        SERIALIZE_MEMBER( "fixedAngle",         m_fixedAngle );
    END_SERIALIZATION()

    Ray_ShooterTurretAIComponent::Ray_ShooterTurretAIComponent() 
        : Super()
        , m_idleBehavior(NULL)
        , m_attackBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_activateBehavior(NULL)
        , m_desactivateBehavior(NULL)
        , m_closedBehavior(NULL)
        , m_open( bfalse )
        , m_fixedAngle( 0.0f )
        , m_sendHitToPieces( bfalse )
    {
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idleBehavior          = createAiBehavior(getTemplate()->getIdle());
        m_attackBehavior        = createAiBehaviorAndCast<Ray_AIShooterAttackBehavior>(getTemplate()->getAttack());
        m_receiveHitBehavior    = createAiBehaviorAndCast<AIReceiveHitBehavior>(getTemplate()->getReceiveHit());
        m_deathBehavior         = createAiBehavior(getTemplate()->getDeath());
        m_crushedBehavior       = createAiBehavior(getTemplate()->getCrushed());

        m_activateBehavior      = createAiBehavior( getTemplate()->getActivateBhv() );
        m_desactivateBehavior   = createAiBehavior( getTemplate()->getDesactivateBhv() );
        m_closedBehavior        = createAiBehavior( getTemplate()->getClosedBhv() );
                
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        if (m_crushedBehavior)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
        }

        if ( getTemplate()->isTriggerable() )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        }

        if ( getTemplate()->getForceLookDir() )
        {
            bbool flipped = !getTemplate()->getForceLookDirRight();
            if ( Vec2d::Right.Rotate( m_actor->getAngle() ).dot(  Vec2d::Right ) < 0.f )
            {
                flipped = !flipped;            
            }
            m_actor->setFlipped( flipped  );
        }
        
       start();
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::start()
    {
        m_open = bfalse;

        if ( getTemplate()->isTriggerable() )
        {
            setBehavior( m_closedBehavior, btrue );
        }
        else
        {
            if ( !m_activateBehavior )
            {
                m_open = btrue;
                setBehavior(m_idleBehavior, btrue);
            }
            else
            {
                setBehavior(m_activateBehavior, btrue);
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::stop()
    {
        if ( m_open )
        {
            m_open = bfalse;
            if ( m_desactivateBehavior != NULL )
            {
                setBehavior( m_desactivateBehavior );
            }
            else
            {
                setBehavior( m_closedBehavior );
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        unpause();

        start();
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        // check my pieces nb
        //--------------------
        if ( m_open && ( m_initPieceCount > 0 ) && ( m_piecesList.size() == 0 ) )
        {
            stop();
        }

        if ( getHealth() <= 0 )
        {
            if (m_currentBehavior != m_deathBehavior)
            {
                if ( Ray_StimComponent * stimcomp = m_actor->GetComponent<Ray_StimComponent>() )
                {
                    stimcomp->setDisabled( btrue );
                }

                // die!
                setBehavior(m_deathBehavior);
                unregisterObjects();    // if the object is recycled, will re-register on unpause
            }
        }
        else if (m_currentBehavior == m_receiveHitBehavior)
        {
            // nothing to do...
        }
        else if ( m_crushedBehavior && m_currentBehavior == m_crushedBehavior )
        {
            // nothing to do ATM...
        }
        else if ( m_open )
        {
            /*if ( getTemplate()->getForceLookDir() )
            {
                m_attackBehavior->setFixedAngle( m_fixedAngle );
                m_attackBehavior->setVictim( ITF_INVALID_OBJREF );
                m_attackBehavior->setTimedSpawnerData( &m_timedSpawnerData );
            }
            else
            {*/

            bbool closeRange = bfalse;
            ObjectRef enemy = findEnemy(closeRange);
            if (enemy != ITF_INVALID_OBJREF)
            {
                if (!closeRange)
                {
                    m_attackBehavior->setFixedAngle( m_fixedAngle );
                    m_attackBehavior->setVictim(enemy);
                    m_attackBehavior->setTimedSpawnerData( &m_timedSpawnerData );
                    setBehavior(m_attackBehavior);            
                }
            }
            else if ( m_currentBehavior != m_idleBehavior )
            {
                setBehavior(m_idleBehavior);
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    ObjectRef Ray_ShooterTurretAIComponent::findEnemy(bbool & _closeRange) const
    {
        ObjectRef enemy = ITF_INVALID_OBJREF;
        if (getTemplate()->getUseRadius())
        {
            Actor * actor =  AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), getTemplate()->getFindEnemyRadius());
            if (actor)
            {
                if ((actor->get2DPos() - m_actor->get2DPos()).sqrnorm() < (getTemplate()->getCloseRadius()*getTemplate()->getCloseRadius()))
                    _closeRange = btrue;
                else
                    _closeRange = bfalse;
                enemy = actor->getRef();
            }

        }
        else
        {
            u32 enemyCount;
            Actor * closeActor =  AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), getTemplate()->getDetectionCloseRange(), &enemyCount);
            if (closeActor)
            {
                _closeRange = btrue;
                enemy = closeActor->getRef();
            }
            else
            {
                Actor * actor =  AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), getTemplate()->getDetectionRange(), &enemyCount);
                if (actor)
                {
                    _closeRange = bfalse;
                    enemy = actor->getRef();
                }
            }

        }
        return enemy;
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        /*if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            receiveHit(hit);
        }
        else */if ( EventInteractionQuery* interactionQuery = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
        {
            processInteractionQuery(interactionQuery);
        }
        else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
        {
            processCrushed(crushed);
        }
        else if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processTrigger(trigger);
        }
        
    }
   
    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::processHit( HitStim* _hit )
    {
        if ( m_sendHitToPieces )
            return;

        if ( !AIUtils::isEnemyHit(_hit, getFaction()) )
            return;

        if ( ( m_receiveHitBehavior != NULL ) && ( m_currentBehavior != m_deathBehavior ) )
        {
            // notify the sender that he touched us
            Actor* sender = GETOBJECT(_hit->getSender())->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if (sender)
            {
                EventHitSuccessful hitSuccessful;
                hitSuccessful .setSender(m_actor->getRef());
                sender->onEvent(&hitSuccessful);
            }

            setBehavior(m_receiveHitBehavior);
            m_receiveHitBehavior->setData(_hit);
            
            m_sendHitToPieces = btrue;
            // Send stim to pieces
            if ( m_piecesList.size() > 0 )
            {
                for ( u32 i = 0; i < m_piecesList.size(); i++ )
                {
                    Actor * piece = AIUtils::getActor( m_piecesList[i].m_actor );
                    if ( piece != NULL )
                    {
                        piece->onEvent( _hit );
                    }
                }
            }
            m_sendHitToPieces = bfalse;
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if (m_currentBehavior == m_receiveHitBehavior || m_currentBehavior == m_attackBehavior)
        {
            // let Update decide what to do next
            setBehavior(m_idleBehavior);
        }
        else if ( m_currentBehavior == m_crushedBehavior )
        {
            onFinishedCrush();
        }
        // Activated, start idle 
        else if ( m_currentBehavior == m_activateBehavior )
        {
            setBehavior(m_idleBehavior);
            m_open = btrue;
        }
        else if ( m_currentBehavior == m_desactivateBehavior )
        {
            setBehavior(m_closedBehavior);
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        PhysActorForceArray windForces;
        Vec2d windForce = Vec2d::Zero;

        PHYSWORLD->getWindForces(m_actor->get2DPos(), Vec2d::Zero, m_actor->getDepth(), windForces);

        for ( u32 i = 0; i < windForces.size(); i++ )
        {
            windForce += windForces[i].m_force;
        }

        StringID s_windForce( ITF_GET_STRINGID_CRC(windForce,2257032332) );
        f32 force = windForce.dot(Vec2d::Right.Rotate(m_actor->isFlipped()?MTH_PI-m_actor->getAngle():m_actor->getAngle()));
        m_animatedComponent->setInput(s_windForce,force);
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::startCrushed()
    {
        setBehavior(m_crushedBehavior);
    }
    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::processInteractionQuery( EventInteractionQuery* interactionQuery )
    {
        if ( getHealth() <= 0 )
        {
            ITF_ASSERT_MSG(0, "dead AIs should be unregistered!");
            return;
        }

        if ( RAY_GAMEMANAGER->getAllowNpcCrush() )
        {
            if ( m_currentBehavior != m_crushedBehavior )
            {
                interactionQuery->setInteraction(CharacterInteractionType_Crush);
            }
        }
    }
    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::processCrushed( EventCrushed* crushed )
    {
        ITF_ASSERT_MSG(m_crushedBehavior, "got crushed but we don't have a crushed behavior!");
        if (m_crushedBehavior)
        {
            receiveCrush(crushed);
            playLastHitFx();

            startCrushed();
        }
    }
    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::onFinishedCrush()
    {
        setBehavior(m_idleBehavior);
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterTurretAIComponent::processTrigger( EventTrigger* _triggerEvt )
    {
        if ( ( _triggerEvt == NULL ) || !getTemplate()->isTriggerable() )
            return;

        if ( _triggerEvt->getActivated() )
        {
            if ( m_activateBehavior != NULL )
            {
                setBehavior( m_activateBehavior );
            }
            else
            {
                m_open = btrue;
                setBehavior( m_idleBehavior );
            }
        }
        else if ( m_open || ( m_currentBehavior == m_activateBehavior ) )
        {
            stop();
        }
    }   

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_ShooterTurretAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterTurretAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idleBehavior",       m_idleBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("attackBehavior",     m_attackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("receiveHitBehavior", m_receiveHitBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("deathBehavior",      m_deathBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("crushedBehavior",    m_crushedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("activateBehavior",   m_activateBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("desactivateBehavior",m_desactivateBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("closedBehavior",     m_closedBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("findEnemyRadius",     m_findEnemyRadius);
        SERIALIZE_MEMBER("closeRadius",         m_closeRadius);
        SERIALIZE_MEMBER("useRadius",           m_useRadius);
        SERIALIZE_OBJECT("detectionRange",      m_detectionRange);
        SERIALIZE_OBJECT("detectionCloseRange", m_detectionCloseRange);
        SERIALIZE_MEMBER("triggerable",         m_triggerable);
        SERIALIZE_MEMBER("forceLookDir",        m_forceLookDir);
        SERIALIZE_MEMBER("forceLookDirRight",   m_forceLookDirRight);
    END_SERIALIZATION()

        Ray_ShooterTurretAIComponent_Template::Ray_ShooterTurretAIComponent_Template()
        : m_findEnemyRadius(4.f)
        , m_closeRadius(1.0f)
        , m_detectionRange(Vec2d(-2, -2), Vec2d(5, 2))     // 5 meters front, 2 behind, 2 above & below
        , m_detectionCloseRange(Vec2d(-2, -2), Vec2d(5, 2))     // 5 meters front, 2 behind, 2 above & below
        , m_useRadius(btrue)
        , m_idleBehavior(NULL)
        , m_attackBehavior(NULL)
        , m_receiveHitBehavior(NULL)
        , m_deathBehavior(NULL)
        , m_crushedBehavior(NULL)
        , m_activateBehavior(NULL)
        , m_desactivateBehavior(NULL)
        , m_closedBehavior(NULL)
        , m_triggerable( bfalse )
        , m_forceLookDir( bfalse )
        , m_forceLookDirRight( bfalse )
    {
    }

        Ray_ShooterTurretAIComponent_Template::~Ray_ShooterTurretAIComponent_Template()
        {
            SF_DEL(m_idleBehavior);
            SF_DEL(m_attackBehavior);
            SF_DEL(m_receiveHitBehavior);
            SF_DEL(m_deathBehavior);
            SF_DEL(m_crushedBehavior);
            SF_DEL(m_activateBehavior);
            SF_DEL(m_desactivateBehavior);
            SF_DEL(m_closedBehavior);            
        }

} // namespace ITF

