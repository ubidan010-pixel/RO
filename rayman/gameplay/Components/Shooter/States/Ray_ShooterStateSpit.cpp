#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

namespace ITF
{
  	IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_ShooterControllerComponent,StateSpit)

	void Ray_ShooterControllerComponent::StateSpit::onEnter()
	{
		Super::onEnter();

		// Start animation
		if (    IS_CURSTATE( STATE_SPIT_START ) ||
                IS_CURSTATE( STATE_SPIT_BASESHOOT ) || 
                IS_CURSTATE( STATE_SPIT_BASESHOOT_RESTART )
           )
		{
			m_spitRequest = bfalse;
            // reset the hold duration
            getParent()->m_shootButtonPressedDuration = 0.0f;
		}
		// Shoot
		else if ( IS_CURSTATE( STATE_SPIT_END ) )
		{
			getParent()->spit();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSpit::onExit()
	{
		Super::onExit();
	}
    //----------------------------------------------------------------------------------------------------------------------
    u32 Ray_ShooterControllerComponent::StateSpit::onValidateStateChange( u32 _prevStateID, u32 _wantedStateID )
    {
        // Call from different implement
        //if ( getStateMachine()->getCurImplement() != this )
        //    return _wantedStateID;

        u32 returnedState = _wantedStateID;

        if ( ( _wantedStateID == STATE_SPIT_START ) && 
             ( getParent()->getVacuumSkill()->getFillValue() == 0 ) && 
             !getParent()->getVacuumSkill()->hasVacuumedActors() )
        {
            returnedState = STATE_SPIT_BASESHOOT;
            if ( getStateMachine()->getPrevStateID() == STATE_SPIT_BASESHOOT )
            {
                returnedState = STATE_SPIT_BASESHOOT_RESTART;
            }
        }

        return returnedState;
    }

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSpit::changeStateCheck( f32 _deltaTime )
	{
		Super::changeStateCheck( _deltaTime );

		if ( !getStateMachine()->isCurStateHasJustChanged() )
		{
			if ( getParent()->getShootListener() ||
                ( getParent()->getShootHoldListener() && ( getParent()->m_shootButtonPressedDuration >= getParent()->getTemplate()->getAutoFireDelay() ) )
                )
			{
                getParent()->m_shootButtonPressedDuration = 0.0f;
				m_spitRequest = btrue;
			}

			if ( IS_CURSTATE( STATE_SPIT_END ) || IS_CURSTATE( STATE_SPIT_TO_STAND )  )
			{
				// shoot resquested
				if ( m_spitRequest && getParent()->hasAmmo() && getParent()->hasCapacityFlag( canMove ) )
				{
					CHANGE_STATE( STATE_SPIT_START );
				}	
				// No more ammo
				else if ( m_spitRequest && getParent()->hasCapacityFlag( canMove ) )
				{
					CHANGE_STATE( STATE_ATTACK_START );
				}
				else if ( getParent()->hasCapacityFlag( canMove ) )
				{
					getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE_VACCUM, btrue );
				}
			}
            else if ( IS_CURSTATE( STATE_SPIT_BASESHOOT_RESTART ) )
            {
                CHANGE_STATE( STATE_SPIT_BASESHOOT );
            }
            else if ( IS_CURSTATE( STATE_SPIT_BASESHOOT ) && getParent()->hasCapacityFlag(comboNext) && m_spitRequest )
            {
                m_spitRequest = bfalse;
                getParent()->removeCapacityFlag( comboNext );
                CHANGE_STATE( STATE_SPIT_BASESHOOT_RESTART );
            }
            else
            {
                getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE_VACCUM, btrue );
            }
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSpit::changeStateCheckAnimFinished()
	{
		if ( IS_CURSTATE( STATE_SPIT_END ) )
		{
			CHANGE_STATE( STATE_SPIT_TO_STAND );
		}
		else if ( IS_CURSTATE( STATE_SPIT_TO_STAND ) )
		{
			CHANGE_STATE( STATE_NORMAL );
		}
        // combo next flag is at the end of the animation, then before auto change of the state, check it
        else if ( IS_CURSTATE( STATE_SPIT_BASESHOOT ) && getParent()->hasCapacityFlag(comboNext) && m_spitRequest )
        {
            m_spitRequest = bfalse;
            getParent()->removeCapacityFlag( comboNext );
            CHANGE_STATE( STATE_SPIT_BASESHOOT_RESTART );
        }
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSpit::update( f32 _dt )
	{
		Super::update( _dt );
	}

	//------------------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_ShooterControllerComponent,StateAttack)

	void Ray_ShooterControllerComponent::StateAttack::onEnter()
	{
		Super::onEnter();
		
		// Start animation
		if ( IS_CURSTATE( STATE_ATTACK_START ) || IS_CURSTATE( STATE_ATTACK_COMBO ) )
		{
			m_queueAttack = bfalse;
            getParent()->setCapacityFlag( cannotMove );
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateAttack::onExit()
	{
		Super::onExit();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateAttack::changeStateCheck( f32 _deltaTime )
	{
		Super::changeStateCheck( _deltaTime );

		if ( !getStateMachine()->isCurStateHasJustChanged() )
		{
            if ( getParent()->getShootListener() )
            {
                m_queueAttack = btrue;
            }

            u32 resState = U32_INVALID;
            if ( getParent()->hasCapacityFlag( canMove ) )
            {
                resState = getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE_VACCUM, btrue );
            }
            if ( ( resState == U32_INVALID ) && m_queueAttack && getParent()->hasCapacityFlag( comboNext ) && !getParent()->hasAmmo() )
            {
                if ( IS_CURSTATE( STATE_ATTACK_START) )
                {
                    CHANGE_STATE( STATE_ATTACK_COMBO );
                }
                else
                {
                    CHANGE_STATE( STATE_ATTACK_START );
                }
            }            

            // --- prick
			/*if ( getButtons()[getParent()->getShootButton()] == InputAdapter::JustPressed )
			{
				m_queueAttack = btrue;
			}

            u32 resState = U32_INVALID;
            if ( getParent()->hasCapacityFlag( canMove ) )
            {
                resState = getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE_VACCUM, btrue );
            }
			if ( ( resState == U32_INVALID ) && m_queueAttack && getParent()->hasCapacityFlag( canMove ) && !getParent()->hasAmmo() )
			{
				CHANGE_STATE( STATE_ATTACK_START );
			}*/
            // --- prick end
//#ifndef TEST_MODE_NORMAL
//			else
//			{
//                if ( getParent()->hasCapacityFlag( canMove ) )
//                {
//				    getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE_VACCUM, btrue );
//                }
//			}
//#endif //TEST_MODE_NORMAL
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateAttack::changeStateCheckAnimFinished()
	{
		//if ( IS_CURSTATE( STATE_ATTACK_START ) )
		/*{
			CHANGE_STATE( STATE_NORMAL );
		}*/
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateAttack::update( f32 _dt )
	{
		Super::update( _dt );

	}

}