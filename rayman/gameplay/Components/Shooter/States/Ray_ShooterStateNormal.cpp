#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_ShooterControllerComponent,StateNormal)

	void Ray_ShooterControllerComponent::StateNormal::onEnter()
	{
		Super::onEnter();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateNormal::onExit()
	{
		Super::onExit();
	}

	//------------------------------------------------------------------------------------------------------------------
	u32 Ray_ShooterControllerComponent::StateNormal::onEvaluateNextStateConditions( u32 _wantedStateID )
	{
		u32 resState = U32_INVALID;
		
        //const f32 comboPressDuration = 0.3f;
		
		//--------------------
		// TWO BUTTON CHECK
		//--------------------
	    if ( ( _wantedStateID == STATE_NORMAL_CHOOSE_VACCUM ) || ( _wantedStateID == STATE_NORMAL_CHOOSE ) )
		{
			if ( ( getParent()->getVacuumSkill()->isFull() && !getParent()->getTemplate()->getDBG_CanVaccumIfFull() ) || ( getParent()->m_vacuumRestartTimer > 0.0f ) )
			{
				if ( getParent()->getVacuumListener() )
				{
					resState = STATE_NORMAL_CANTVACCUM;
				}
			}
            else if ( !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM ) )
            {
                if ( getParent()->getVacuumListener() || getParent()->getVacuumHoldListener()
                    /*( getParent()->getVacuumListener() ) 
                    || ( ( getParent()->m_vaccumButtonPressedDuration <= comboPressDuration ) 
                        && ( getParent()->getVacuumListener() || getParent()->getVacuumHoldListener() ) )*/
                    )
                {
                    resState = STATE_VACCUM_START;
                }
            }
			else if ( ( getParent()->getVacuumHoldListener() ) || ( getParent()->getVacuumListener() ) )
			{
				//else if ( !getParent()->getTemplate()->getDBG_VaccumAllowPressStop() )
				{
					resState = STATE_VACCUM_START;
				}
			}
			/*if ( ( resState != STATE_NORMAL_CANTVACCUM ) && getParent()->getTemplate()->getDBG_VaccumAllowPressStop() &&  getButtons()[getParent()->getVacuumButton()] == InputAdapter::Pressed )
			{
				resState = STATE_VACCUM_START;
			}*/
		}
		if ( ( _wantedStateID == STATE_NORMAL_CHOOSE_ATTACK ) || ( _wantedStateID == STATE_NORMAL_CHOOSE ) )
		{
			if (    getParent()->getShootListener() ||    // just pressed
                    ( getParent()->getShootHoldListener() && ( getParent()->m_shootButtonPressedDuration >= getParent()->getTemplate()->getAutoFireDelay() ) )
                )                        
			{
                if ( getParent()->hasAmmo() ||  getParent()->getVacuumSkill()->hasVacuumedActors() )
				{
					resState = STATE_SPIT_START;
				}
				else
				{
					resState = STATE_ATTACK_START;
				}
			}
		}

		return resState;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateNormal::changeStateCheck( f32 _deltaTime )
	{
		Super::changeStateCheck( _deltaTime );

		getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE, btrue );
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateNormal::update( f32 _dt )
	{
		Super::update( _dt );
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateNormal::changeStateCheckAnimFinished()
	{
		if ( IS_CURSTATE( STATE_NORMAL_CANTVACCUM ) ) 
		{
			CHANGE_STATE( STATE_NORMAL );
		}
	}
	
}