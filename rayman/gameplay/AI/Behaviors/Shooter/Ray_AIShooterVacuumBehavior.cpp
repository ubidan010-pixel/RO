#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterVacuumBehavior.h"
#endif //_ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterVacuumBehavior)
	/*BEGIN_VALIDATE_BEHAVIOR(Ray_AIShooterVacuumBehavior)
    END_VALIDATE_BEHAVIOR()*/

    //---------------------------------------------------------------------------------------------------------------
    Ray_AIShooterVacuumBehavior::Ray_AIShooterVacuumBehavior()
        : Super()
        , m_externStateRequested( U32_INVALID )
        , m_startVacuumRequested( bfalse )
        , m_resourceReady( bfalse )
        , m_waitAction( NULL )
        , m_vacuumAction( NULL )
        , m_spitAction( NULL )
        , m_spitBoneIndex( U32_INVALID )
        , m_canReceiveHit( btrue )
        , m_spitProcessed( bfalse )
        , m_absorbedAnimCursor( 99.9f )
        , m_geyserPlatformAIComponent( NULL )
        , m_startVacuumTimer( 0.0f )
        , m_stoppingVacuum( bfalse )
        , m_stoppingVacuumReset( bfalse )
        , m_stopVacuumRequested( bfalse )
    {
    }

    Ray_AIShooterVacuumBehavior::~Ray_AIShooterVacuumBehavior()
    {
        m_vacuumSkill.removeVacuum();
    }

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterVacuumBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
        // Create actions
        //-------------------
        ITF_ASSERT( getTemplate()->getWaitActionTemplate() );   //actions needed
        ITF_ASSERT( getTemplate()->getVacuumActionTemplate() );
        ITF_ASSERT( getTemplate()->getSpitActionTemplate() );
        if ( getTemplate()->getWaitActionTemplate() )
        {
            m_waitAction = createAiAction( getTemplate()->getWaitActionTemplate() );
        }
        if ( getTemplate()->getVacuumActionTemplate() )
        {
            m_vacuumAction = createAiAction( getTemplate()->getVacuumActionTemplate() );
        }
        if ( getTemplate()->getSpitActionTemplate() )
        {
            m_spitAction = createAiAction( getTemplate()->getSpitActionTemplate() );
        }

		// after action add
		Super::onActorLoaded(_hotReload);
        
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumed,1513162250), m_aiComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumBhvChangeState,2152722158), m_aiComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_aiComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316), m_aiComponent );

        m_geyserPlatformAIComponent = m_actor->GetComponent<Ray_GeyserPlatformAIComponent>();
        
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::onResourceReady()
    {
        Super::onResourceReady();

        m_vacuumSkill.initialize( m_actor, m_animatedComponent, &(getTemplate()->getVacuumSkill()) );
        m_resourceReady = btrue;

        if ( m_animatedComponent != NULL )
        {
            if ( getTemplate()->getSpitBoneName().isValid() )
            {
                m_spitBoneIndex = m_animatedComponent->getBoneIndex( getTemplate()->getSpitBoneName() );
                ITF_ASSERT_MSG(m_spitBoneIndex != U32_INVALID, "you need to specify the valid spit bone");
            }
        }
    }    

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterVacuumBehavior::onActivate()
	{
        m_canReceiveHit = btrue;
        startWaitAction();
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::onDeactivate()
    {
        if ( !m_stoppingVacuum )            
        {
            stopVacuum();
            
            // Spit action not finished, save SPIT as next state
            /*if ( ( getCurrentAction() == m_spitAction ) && !m_spitProcessed )
            {
                m_externStateRequested = SPIT;
            }*/
            // remove all vacuumed objs
            m_vacuumSkill.reset( btrue );
        }
        else
        {
            m_stoppingVacuumReset = btrue;
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::onEvent( Event * _event )
    {
        Super::onEvent( _event );

        if (Ray_EventShooterVacuumed * onVacuumed = _event->DynamicCast<Ray_EventShooterVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumed,1513162250)))
        {
            m_vacuumSkill.processVacuumed(onVacuumed);
        }
        else if (Ray_EventShooterVacuumBhvChangeState * chgEvt = _event->DynamicCast<Ray_EventShooterVacuumBhvChangeState>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumBhvChangeState,2152722158)))
        {
            processEvtChangeState( chgEvt );
        }
        else if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( ( getCurrentAction() == m_spitAction ) && ( animEvent->getName() == getTemplate()->getSpitMarker() ) )
            {
                spit();
            }
            else if ( ( getCurrentAction() == m_vacuumAction ) && ( animEvent->getName() == getTemplate()->getStartVacuumMarker() ) )
            {
                startVacuum();
            }
        }
        else if ( EventQueryCanReceiveHit* receiveHitQuery = _event->DynamicCast<EventQueryCanReceiveHit>(ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316)) )
        {
            receiveHitQuery->setCanReceiveHit( m_canReceiveHit );
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::processEvtChangeState( Ray_EventShooterVacuumBhvChangeState * _evt )
    {
        ITF_ASSERT( _evt != NULL );
        if ( _evt == NULL )
            return;

        ITF_WARNING( m_actor, _evt->getState() < VACUUM_BHV_STATES_COUNT, "Invalid given state id !" );

        if ( _evt->getState() < VACUUM_BHV_STATES_COUNT )
        {
            m_externStateRequested = _evt->getState();
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::processChangeStateRequest()
    {
        if ( ( m_externStateRequested == U32_INVALID ) || ( m_animatedComponent == NULL ) )
            return;

        // wait action finish
        if (    m_animatedComponent->isInTransition() ||
                ( getCurrentAction() == m_spitAction )
            )                
        {
            return;
        }

        switch( m_externStateRequested )
        {
            case WAIT :
            {
                startWaitAction(); 
                break;                    
            }
            case VACUUM :
            {
                startVacuumAction(); 
                break;                    
            }
            case SPIT :
            {
                if ( m_vacuumSkill.hasAmmo() )
                {
                    startSpitAction();
                }
                else
                {
                    startWaitAction();                
                }
                break;                    
            }
            default:
            {
                ITF_WARNING( m_actor, m_externStateRequested < VACUUM_BHV_STATES_COUNT, "Invalid given state id !" );
            }
        }
        m_externStateRequested = U32_INVALID;
    }

    //---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterVacuumBehavior::startVacuum()
	{
        if ( m_resourceReady )
        {
            m_vacuumSkill.addVacuum();
            m_vacuumSkill.setPowerFactor( 0.0f );
            m_startVacuumRequested = bfalse;
            m_startVacuumTimer = 0.0f;

            if ( m_geyserPlatformAIComponent != NULL )
            {
                m_geyserPlatformAIComponent->open();
            }
        }
        else
        {
            m_startVacuumRequested = btrue;
        }
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::stopVacuumRequest()
    {
        if ( m_vacuumSkill.isActive() )
        {
            if ( !m_stopVacuumRequested )
            {
                if ( m_geyserPlatformAIComponent != NULL )
                {
                    m_geyserPlatformAIComponent->close();
                }
            }
            m_stopVacuumRequested = btrue;        
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::stopVacuum()
    {
        if ( !m_stopVacuumRequested && ( m_geyserPlatformAIComponent != NULL ) )
        {
            m_geyserPlatformAIComponent->close();
        }
        
        m_stopVacuumRequested = bfalse;
        m_stoppingVacuum = btrue;
        m_stoppingVacuumReset = bfalse;
        m_vacuumSkill.removeVacuum();
        m_vacuumSkill.aspirationFinished();
        
        m_stoppingVacuum = bfalse;
        
        // could arrive if a swallowed actor send a hit to the actor during aspiration finished
        // actor pass in hit bhv by calling cur bhv onbecomeinactive function
        if ( m_stoppingVacuumReset )
        {
            m_vacuumSkill.reset( btrue );
            m_stoppingVacuumReset = bfalse;
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::startWaitAction()
    {
        if ( m_waitAction != NULL )
        {
            stopVacuumRequest();
            if ( m_aiComponent->getCurrentBehavior() == this )
            {
                setAction( m_waitAction, btrue );        
            }
        }
    }
    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::startVacuumAction()
    {
        if ( ( m_vacuumAction != NULL ) && ( getCurrentAction() != m_vacuumAction ) )
        {
            setAction( m_vacuumAction );
            if ( !getTemplate()->getStartVacuumMarker().isValid() )
            {
                startVacuum();
            }
        }

    }
    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::startSpitAction()
    {
        if ( ( m_spitAction != NULL ) && ( getCurrentAction() != m_spitAction ) )
        {
            m_spitProcessed = bfalse;
            stopVacuum();
            if ( m_aiComponent->getCurrentBehavior() == this )
            {
                setAction( m_spitAction );        
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::spit()
    {
        if ( ( getCurrentAction() != m_spitAction ) || !m_vacuumSkill.hasAmmo() )
        {
            return;
        }
        
        Vec3d bonePos = m_actor->getPos();
        if ( m_spitBoneIndex != U32_INVALID )
        {
            m_animComponent->getBonePos( m_spitBoneIndex,bonePos );
        }
        Vec3d shootPos = bonePos;
        Vec2d shootDir = getTemplate()->getSpitForce().IsEqual( Vec2d::Zero ) ? Vec2d::Right : getTemplate()->getSpitForce();
        shootDir.normalize();
        shootDir = shootDir.Rotate( m_actor->getAngle() );
       	
        Actor * ejectedActor = m_vacuumSkill.Shoot( shootPos, shootDir );
		if ( ejectedActor != NULL )
		{
			ejectedActor->enable();
			ejectedActor->setPos( shootPos );
			Ray_EventShooterEjectActor ejectEvent( shootDir, getTemplate()->getSpitForce() );
            ejectEvent.setSender( m_actor->getRef() );
			ejectedActor->onEvent(&ejectEvent);

			EventShow show(1.0f,0.1f);
			ejectedActor->onEvent(&show);
		}    

        m_spitProcessed = btrue;
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::onActionFinished()
    {
        if ( getCurrentAction() == m_spitAction )
        {
            startWaitAction();

            if ( ( m_externStateRequested == U32_INVALID ) && m_vacuumSkill.hasAmmo() )
            {
                m_externStateRequested = SPIT;
            }
        }
    }

	//---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::update( f32 _dt  )
    {
        // check state request
        processChangeStateRequest();

        Super::update( _dt );

        // Get vacuum Pos
        Vec3d vacuumPos = m_actor->getPos();
        if ( m_spitBoneIndex != U32_INVALID )
        {
            m_animComponent->getBonePos( m_spitBoneIndex, vacuumPos );
        }

        // check vacuum state
        if ( m_startVacuumRequested )
        {
            startVacuum();
        }
        if ( m_vacuumSkill.isActive() )
        {
            m_startVacuumTimer += _dt;
            f32 factor = ( getTemplate()->getVacuumSkill().getStateStartDuration() != 0.0f ) ? f32_Min( 1.0f, m_startVacuumTimer / getTemplate()->getVacuumSkill().getStateStartDuration() ) : 1.0f;
                        
            m_vacuumSkill.setPowerFactor( factor );
            
            if ( !m_stopVacuumRequested )
            {
                m_vacuumSkill.aspirate();
            }
            m_vacuumSkill.aspirationUpdate( _dt, m_actor->getPos() );

            // Update FX
            if ( m_geyserPlatformAIComponent != NULL )
            {
                Vec2d startPos;
                Vec3d dir = ( m_vacuumSkill.getVacuumDirectionAndPos(startPos) ).to3d( m_actor->getDepth()) * m_vacuumSkill.getVacuumWidth();
                m_geyserPlatformAIComponent->setTarget( startPos.to3d( m_actor->getDepth() ) + dir );
            }

            // stop requested, check aspirated actors
            if ( m_stopVacuumRequested && !m_vacuumSkill.hasVacuumedActors() )
            {
                stopVacuum();
            }
        }

        if ( m_vacuumSkill.isFillValueHasChanged() )
        {
            m_absorbedAnimCursor = 0.f;
        }

        // update vacuumed obj pos
        m_vacuumSkill.updateSpecificProjectiles( vacuumPos );

        m_absorbedAnimCursor = f32_Min( m_absorbedAnimCursor + _dt, 99.9f );
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::updateAnimInputs()
    {
        if ( m_animatedComponent == NULL )
            return;
        
        m_animatedComponent->setInput( ITF_GET_STRINGID_CRC(Full,4184752541), m_vacuumSkill.hasAmmo() ? 1 : 0 );
        m_animatedComponent->setInput( ITF_GET_STRINGID_CRC(AbsorbAddAnimCursor,1951647409), m_absorbedAnimCursor );        
    }

#ifdef ITF_SUPPORT_EDITOR
    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterVacuumBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const
    {
        m_vacuumSkill.debugDraw();
    }
#endif // ITF_SUPPORT_EDITOR
    
	//---------------------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(Ray_AIShooterVacuumBehavior_Template)
	BEGIN_SERIALIZATION_CHILD(Ray_AIShooterVacuumBehavior_Template)
        SERIALIZE_OBJECT( "vacuumSkill", m_vacuumSkill );
        SERIALIZE_OBJECT_WITH_FACTORY("waitAction",	    m_waitAction,   ACTORSMANAGER->getAIActionsFactory() );
        SERIALIZE_OBJECT_WITH_FACTORY("vacuumAction",	m_vacuumAction, ACTORSMANAGER->getAIActionsFactory() );
        SERIALIZE_OBJECT_WITH_FACTORY("spitAction",	    m_spitAction,   ACTORSMANAGER->getAIActionsFactory() );
        SERIALIZE_MEMBER("spitMarker",                  m_spitMarker );
        SERIALIZE_MEMBER("spitBoneName",                m_spitBoneName );
        SERIALIZE_MEMBER("spitForce",                   m_spitForce );
        SERIALIZE_MEMBER("startVacuumMarker",           m_startVacuumMarker );
	END_SERIALIZATION()

	Ray_AIShooterVacuumBehavior_Template::Ray_AIShooterVacuumBehavior_Template()
	: Super()
    , m_waitAction( NULL )
    , m_vacuumAction ( NULL )
    , m_spitAction( NULL )
	{
	}

	Ray_AIShooterVacuumBehavior_Template::~Ray_AIShooterVacuumBehavior_Template()
	{
        SF_DEL( m_waitAction );
        SF_DEL( m_vacuumAction );
        SF_DEL( m_spitAction );
        
	}
}

