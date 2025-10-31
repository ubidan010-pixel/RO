#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_ShooterControllerComponent,StateHit)

    //------------------------------------------------------------------------------------------------------------------
    Ray_ShooterControllerComponent::StateHit::~StateHit()
    {
        destroyFX();
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateHit::destroyFX()
    {
        for ( u32 i = 0; i < m_deathFXActors.size(); ++i )
        {
            if ( Actor * fx = AIUtils::getActor( m_deathFXActors[i] ) )
            {
                fx->requestDestruction();
            }
        }
        m_deathFXActors.clear();
    }    

    //------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateHit::onEnter()
	{
		Super::onEnter();

        m_InitScale = getParent()->GetActor()->getScale();
        
        m_bounceToTarget.invalidate();
      
        // no move possible at start
        getParent()->setCapacityFlag( cannotMove );

        // Check is dead
        m_isdead =  static_cast<Ray_Player*>( RAY_GAMEMANAGER->getPlayer( getParent()->getPlayerIndex() ) )->isDead();

        m_restartTriggered = bfalse;
        m_disableAtEnd = bfalse;
        m_spawnDeathFX = bfalse;

        if ( IS_CURSTATE( STATE_DEATH_BUBBLE ) && !RAY_GAMEMANAGER->getShooterGameModeController()->isAllPlayerDead() )
        {
            m_disableAtEnd = btrue;
        }	
    }

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateHit::onExit()
	{
		Super::onExit();
     
        if ( IS_CURSTATE( STATE_HIT ) )
        {
            getParent()->GetActor()->setScale( m_InitScale );
        }
	}

    //----------------------------------------------------------------------------------------------------------------------
    u32 Ray_ShooterControllerComponent::StateHit::onValidateStateChange( u32 _prevStateID, u32 _wantedStateID )
    {
        if ( getStateMachine()->getCurImplement() != this )
            return _wantedStateID;

        // is dead don't restart the state
        if ( m_isdead && !getStateMachine()->isStateInGroupRange( _wantedStateID, STATE_GROUP_DEATH ) )
        {
            return U32_INVALID;
        }
        return _wantedStateID;
    }

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateHit::changeStateCheck( f32 _deltaTime )
	{
		Super::changeStateCheck( _deltaTime );

        if ( getParent()->hasCapacityFlag( canMove ) && !m_isdead )
        {
            getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE, btrue );
        }
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateHit::update( f32 _dt )
	{
		Super::update( _dt );

        //***************
        //      HIT
        //***************
        if ( IS_CURSTATE( STATE_HIT ) )
        {
            f32 depth = 0.0f;
            f32 duration = 0.3f;// 0.64f;//getStateMachine()->getAnimState( STATE_HIT )->getCurAnimDuration();
            f32 time = getStateMachine()->getCurStateStartDuration();
            f32 cursor = Min( time / duration, 1.0f );	

            if ( time > duration * 0.5f )
            {
                cursor = ( 1.0f - cursor );
            }
            depth += cursor * 5.0f;
            getParent()->GetActor()->setScale( m_InitScale + Vec2d( depth, depth ) );

            // check bounce to target
            //-------------------------
            if ( m_bounceToTarget.isValid() && !getParent()->hasCapacityFlag( canMove ) )
            {
                Actor * targetActor = AIUtils::getActor( m_bounceToTarget );
                if ( targetActor != NULL )
                {
                    getParent()->m_stateWantedMove = btrue;
                    Vec2d vecToTarget = ( targetActor->get2DPos() - getParent()->GetActor()->get2DPos() );
                    f32 distToTarget = vecToTarget.norm();
                    vecToTarget = ( distToTarget != 0.0f ) ? vecToTarget / distToTarget : Vec2d::Zero;
                    
                    getParent()->m_stateWantedLocalSpeedMax = distToTarget / 0.31f; // is time between the anim start and can move flag

                    Vec2d camVelocity = SHOOTERCAMERA ? SHOOTERCAMERA->getSpeed() : Vec2d::Zero;

                    getParent()->m_stateWantedMoveVector = ( vecToTarget * getParent()->m_stateWantedLocalSpeedMax ) - camVelocity;
                }
            }
        }
        //************************
        //      BUBBLE DEATH
        //************************
        if ( IS_CURSTATE( STATE_DEATH_BUBBLE ) || IS_CURSTATE( STATE_DEATH ) )
        {
            if ( m_spawnDeathFX && getParent()->m_deathFXGenerator.isInitialized() )
            {
                m_spawnDeathFX = bfalse;

                Actor * FXActor = getParent()->m_deathFXGenerator.getSpawnee( getParent()->m_actor->getScene(), getParent()->m_actor->getPos(), getParent()->m_actor->getAngle() );
                if ( FXActor != NULL )
                {
                    FXActor->loadResources();
                    FXActor->setFlipped( getParent()->m_actor->isFlipped() );
                    
                    m_deathFXActors.push_back( FXActor->getRef() );
                }                
            }
        }
	}

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateHit::changeStateCheckAnimFinished()
    {
        // Dead by hit
        if ( m_isdead )
        {
            if ( IS_CURSTATE( STATE_HIT ) )
            {
                getParent()->die();
            }
            else if ( m_disableAtEnd && IS_CURSTATE( STATE_DEATH_BUBBLE ) )
            {
                RAY_GAMEMANAGER->changePlayerActivation( getParent()->getPlayerIndex(), bfalse );
            }
            else if ( !m_restartTriggered && ( IS_CURSTATE( STATE_DEATH_BUBBLE ) ||  IS_CURSTATE( STATE_DEATH ) ) )
            {
                m_restartTriggered = btrue;
                RAY_GAMEMANAGER->getShooterGameModeController()->setPlayerReadyToRestart( RAY_GAMEMANAGER->getPlayer( getParent()->getPlayerIndex() ) );
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateHit::onEvent( Event * _evt )
    {
        if (AnimGameplayEvent * animEvent = _evt->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if ( m_isdead )
            {
                if ( animEvent->getName() == MRK_Allow_Move )
                {
                    getParent()->die();
                    getParent()->setCapacityFlag( cannotMove );
                }
                else if ( !m_disableAtEnd && ( animEvent->getName() == MRK_Death ) )
                {
                    RAY_GAMEMANAGER->getShooterGameModeController()->setPlayerReadyToRestart( RAY_GAMEMANAGER->getPlayer( getParent()->getPlayerIndex() ) );
                    m_restartTriggered = btrue;
                    //RAY_GAMEMANAGER->triggerRestoreDeath( getParent()->GetActor()->getRef(),RAY_GAMEMANAGER->getDeathFadeDuration());
                }
                else if ( animEvent->getName() == MRK_SpawnFX ) 
                {
                    m_spawnDeathFX = btrue;
                }
            }
        }
        else if (Ray_EventBounceToLayer* bounceToLayer = _evt->DynamicCast<Ray_EventBounceToLayer>(ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527)) )
        {
            if ( IS_CURSTATE( STATE_HIT ) && bounceToLayer->getHurt() )
            {
                m_bounceToTarget = bounceToLayer->getTarget();
            }
        }
    }
}