#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERPROJECTILEACTION_H_
#include "rayman/gameplay/ai/Actions/Shooter/Ray_AIShooterProjectileAction.h"
#endif //_ITF_RAY_AISHOOTERPROJECTILEACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_


#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterProjectileAction)

    Ray_AIShooterProjectileAction::Ray_AIShooterProjectileAction()
        : Super()
        , m_prevPositionOffset( Vec2d::Zero )
    {
    }

    Ray_AIShooterProjectileAction::~Ray_AIShooterProjectileAction()
    {
    }

	//--------------------------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterProjectileAction::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);
		m_bulletAbility.init( m_actor, getTemplate()->getBasicBulletTemplate() );
	}

	//--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterProjectileAction::onActivate()
    {
        Super::onActivate();
		m_bulletAbility.onBecomeActive();
		
		if ( m_physComponent != NULL )
		{
			m_physComponent->setDisabled( btrue );
		}

        // find eject phantom shape
        if ( PhantomComponent * phantomComp = m_actor->GetComponent<PhantomComponent>() )
        {
            phantomComp->setShape( ITF_GET_STRINGID_CRC(ejectPhantom,291858271) );
        }
        
        if ( m_animatedComponent != NULL )
        {
            m_prevPositionOffset = m_animatedComponent->getInstancePositionOffset();
            m_animatedComponent->setInstancePositionOffset( getTemplate()->getPosOffset() );        
        }

        // trail activation
        EventTrail showTrail(btrue);
        m_actor->onEvent( &showTrail );
    }
	//--------------------------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterProjectileAction::onDeactivate()
	{
		Super::onDeactivate();
		m_bulletAbility.onBecomeInactive();

		if ( m_physComponent != NULL )
		{
			m_physComponent->setDisabled( bfalse );
		}

        if ( m_animatedComponent != NULL )
        {
            m_animatedComponent->setInstancePositionOffset( m_prevPositionOffset );
        }

        // trail activation
        EventTrail showTrail(bfalse);
        m_actor->onEvent( &showTrail );
	}	

	//--------------------------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterProjectileAction::update( f32 _dt )
    {
        Super::update(_dt);
		
		// Update bullet
		//----------------
		m_bulletAbility.update(_dt);
		if ( m_bulletAbility.isObsolete() )
		{
			 m_aiBehavior->onActionFinished();
		}
    }

	//--------------------------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterProjectileAction::onEvent( Event * _event )
	{
		Super::onEvent( _event );

		// received at start
		if ( Ray_EventShooterEjectActor * ejectActorEvent = _event->DynamicCast<Ray_EventShooterEjectActor>(ITF_GET_STRINGID_CRC(Ray_EventShooterEjectActor,937196133)) )
		{
			m_bulletAbility.setMoveDir( ejectActorEvent->getEjectDir() );
            m_bulletAbility.setOwner( ejectActorEvent->getSender() );

            // apply the sender faction
            Actor * sender = AIUtils::getActor( ejectActorEvent->getSender() );
            if ( sender != NULL )
            {
                m_bulletAbility.setFaction( AIUtils::getFaction( sender ) );
            }            

            if ( m_bulletAbility.getDefaultMoveSpeed().IsEqual( Vec2d::Zero ) )
            {
			    m_bulletAbility.setMoveSpeed( ejectActorEvent->getEjectForce().norm() );
            }

            m_bulletAbility.setPosOutOfEnvCollisions();
		}
        else if ( EventQueryCanReceiveHit * canReceiveHit = _event->DynamicCast<EventQueryCanReceiveHit>(ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316)) )
        {
            if ( /*!m_bulletAbility.isEnvTouched() && */!m_bulletAbility.isObsolete() )
            {
                canReceiveHit->setCanReceiveHit( bfalse );
            }
        }
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(Ray_AIShooterProjectileAction_Template)

	BEGIN_SERIALIZATION_CHILD(Ray_AIShooterProjectileAction_Template)
		SERIALIZE_OBJECT( "basicBullet",	m_basicBullet );
        SERIALIZE_MEMBER( "posOffset",	    m_posOffset );
	END_SERIALIZATION()

	Ray_AIShooterProjectileAction_Template::Ray_AIShooterProjectileAction_Template()
    : Super()
    , m_posOffset( Vec2d::Zero )
	{
	}


}