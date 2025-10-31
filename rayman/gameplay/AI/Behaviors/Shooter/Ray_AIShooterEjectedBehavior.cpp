#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTEREJECTEDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterEjectedBehavior.h"
#endif //_ITF_RAY_AISHOOTEREJECTEDBEHAVIOR_H_

#ifndef _ITF_RAY_AISHOOTERPROJECTILEACTION_H_
#include "rayman/gameplay/ai/Actions/Shooter/Ray_AIShooterProjectileAction.h"
#endif //_ITF_RAY_AISHOOTERPROJECTILEACTION_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterEjectedBehavior)
		BEGIN_VALIDATE_BEHAVIOR(Ray_AIShooterEjectedBehavior)
    END_VALIDATE_BEHAVIOR()


    Ray_AIShooterEjectedBehavior::Ray_AIShooterEjectedBehavior()
        : Super()
		, m_ejectAction( NULL )
        , m_ejectProjectileAction( NULL )
        , m_hitCount( 0 )
    {
    }

    Ray_AIShooterEjectedBehavior::~Ray_AIShooterEjectedBehavior()
    {
    }

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterEjectedBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		// Create actions
		if ( getTemplate()->getEjectActionTemplate() )
		{
			m_ejectAction = createAiAction( getTemplate()->getEjectActionTemplate() );
            if ( m_ejectAction != NULL )
            {
                m_ejectProjectileAction = m_ejectAction->DynamicCast<Ray_AIShooterProjectileAction>(ITF_GET_STRINGID_CRC(Ray_AIShooterProjectileAction,2250286364));
            }
        }

        // after action add
		Super::onActorLoaded(_hotReload);
        
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013), m_aiComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(HitStim,2121727796), m_aiComponent );
	}

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterEjectedBehavior::onActivate()
	{
		if ( m_ejectAction != NULL )
		{
			setAction( m_ejectAction );
		}
	}

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterEjectedBehavior::onEvent( Event * _event)
	{
		Super::onEvent( _event );

		// Is received only at the behavior start
		//------------------------------------------
		if ( Ray_EventShooterEjectActor * ejectActorEvent = _event->DynamicCast<Ray_EventShooterEjectActor>(ITF_GET_STRINGID_CRC(Ray_EventShooterEjectActor,937196133)) )
		{
			m_curEjectDir	= ejectActorEvent->getEjectDir();
			m_curEjectForce = ejectActorEvent->getEjectForce();
		}
        else if ( EventHitSuccessful* hitSuccessful = _event->DynamicCast<EventHitSuccessful>(ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013)) )
        {
            onHitSuccessful(hitSuccessful);
        }
        // if the ejected actor is from an enemy, a player stim can stop eject action
        //
        else if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
        {
            if ( AIUtils::isDamageHit( hit, m_aiComponent->getFaction() ) )
            {
                ActorRef senderRef = hit->getOriginalSender().isValid() ? hit->getOriginalSender() : hit->getSender();
                if ( GAMEMANAGER->isPlayerActor( senderRef ) )
                {
                    ejectActionFinished( btrue );

                    if ( Actor * sender = AIUtils::getActor( hit->getSender() ) )
                    {
                        EventHitSuccessful success;
                        success.setSender( m_actor->getRef() );
                        sender->onEvent( &success );
                    }
                }
            }
        }
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterEjectedBehavior::onHitSuccessful( EventHitSuccessful * _hitSuccessfulEvt )
    {
        if ( getTemplate()->getHitNumber() > 0 )
        {
            m_hitCount++; 
            if ( m_hitCount >= getTemplate()->getHitNumber() )
            {
                ejectActionFinished( bfalse );
            }
        }
    }

	//---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterEjectedBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
    }

	//---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterEjectedBehavior::onActionFinished()
    {
        bbool isEjectAction = isCurrentAction( m_ejectAction );
        bbool aiBhvFinished = btrue;
        
        if ( getTemplate()->needDestroyOnEjectActionsEnd() )
        {
            aiBhvFinished = bfalse;
            if ( m_actor->isSerializable() )
            {
                m_actor->disable();
            }
            else
            {
                m_actor->requestDestruction();
            }
        }
        else 
        {
            if ( isEjectAction )
            {
                ejectActionFinished( btrue );
                aiBhvFinished = bfalse;
            }
        }

        if ( aiBhvFinished )
        {
            m_aiComponent->onBehaviorExternFinished();
        }
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterEjectedBehavior::ejectActionFinished( bbool _fromAction )
    {
        if ( _fromAction )
        {
            // is projectile action and exit screen ?
            if ( ( m_ejectProjectileAction != NULL ) && m_ejectProjectileAction->hasExitScreen() )
            {
                if ( m_actor->isSerializable() )
                {
                    m_actor->disable();
                }
                else
                {
                    m_actor->requestDestruction();
                }
            }
            // Check behavior
            else if ( getTemplate()->getEjectActionNextBhvName().isValid() )
            {   
                EventChangeBehaviourQuery bhvEvt;
                bhvEvt.setWantedBehaviourName( getTemplate()->getEjectActionNextBhvName() );
                m_actor->onEvent( &bhvEvt );                
            }
        }
        else
        {
            // Check behavior
            if ( getTemplate()->getHitNumberNextBhvName().isValid() )
            {
                EventChangeBehaviourQuery bhvEvt;
                bhvEvt.setWantedBehaviourName( getTemplate()->getHitNumberNextBhvName() );
                m_actor->onEvent( &bhvEvt );
            }
        }

        // kill actor ?
        if ( getTemplate()->needKillOnEnd() )
        {
            m_aiComponent->setHealth( 0 );
        }    

    }

	//---------------------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(Ray_AIShooterEjectedBehavior_Template)
	BEGIN_SERIALIZATION_CHILD(Ray_AIShooterEjectedBehavior_Template)
		SERIALIZE_OBJECT_WITH_FACTORY("ejectAction",	m_ejectAction, ACTORSMANAGER->getAIActionsFactory() );
		SERIALIZE_MEMBER( "destroyOnEjectActionsEnd",	m_destroyOnEjectActionsEnd );
        SERIALIZE_MEMBER( "killOnEnd",	                m_killOnEnd );
        SERIALIZE_MEMBER( "hitNumber",	                m_hitNumber );
        SERIALIZE_MEMBER( "hitNumberNextBhvName",	    m_hitNumberNextBhvName );
        SERIALIZE_MEMBER( "ejectActionNextBhvName",	    m_ejectActionNextBhvName );
        //SERIALIZE_MEMBER( "onEjectActionEndEvent",	    m_onEjectActionEndEvent );
	END_SERIALIZATION()

	Ray_AIShooterEjectedBehavior_Template::Ray_AIShooterEjectedBehavior_Template()
	: Super()
	, m_ejectAction( NULL )
    , m_hitNumber( 0 )
	, m_destroyOnEjectActionsEnd( btrue )
    , m_killOnEnd( bfalse )
    //, m_onEjectActionEndEvent( NULL )
	{
	}

	Ray_AIShooterEjectedBehavior_Template::~Ray_AIShooterEjectedBehavior_Template()
	{
        SF_DEL(m_ejectAction);
	}

}
