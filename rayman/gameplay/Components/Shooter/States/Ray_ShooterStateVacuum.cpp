#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#define NO_COLOR    1

namespace ITF
{
   	IMPLEMENT_OBJECT_RTTI_SUBCLASS( Ray_ShooterControllerComponent,StateVacuum )

	//----------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateVacuum::onEnter()
	{
		Super::onEnter();

		// INIT STATE
		if ( getStateMachine()->isChangingImplement() )
		{
			m_vacuumTimer = 0.0f;
			m_startAnimTimer = 0.0f;
            m_transitionTimer = 0.0f;
            m_redTimer = 0.0f;
            m_stopRequested = bfalse;
		}
		
		/*if ( IS_CURSTATE( STATE_VACCUM_QUICK ) )
		{
			getParent()->getVacuumSkill()->setInstantVacuum( btrue );
			m_vacuumTimer = 0.0f;
		}*/

        if ( IS_CURSTATE( STATE_VACCUM_START ) )
        {
            getParent()->getVacuumSkill()->addVacuum();
            getParent()->getVacuumSkill()->setPowerFactor( 0.0f );
            m_transitionTimer = 0.0f;
        }
		else if ( IS_CURSTATE( STATE_VACCUM_LOOP ) )
		{
			getParent()->m_vacuumRestartTimer = getParent()->getTemplate()->getVacuumSkill().getStateBeforeRestartDuration();
            // max power !
            getParent()->getVacuumSkill()->setPowerFactor( 1.0f );
		}
        else if ( IS_CURSTATE( STATE_VACCUM_FINISHED_FROM_START ) )
        {
            //m_startAnimTimer = ( m_transitionTimer / getAnimDuration( STATE_VACCUM_START ) ) * getAnimDuration( STATE_VACCUM_FINISHED_FROM_START );
            m_startAnimTimer = m_transitionTimer;
        }
	}

	//----------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateVacuum::onExit()
	{
		Super::onExit();

		// STOP STATE
		if ( getStateMachine()->isChangingImplement() )
		{
			getParent()->getVacuumSkill()->removeVacuum();
            getParent()->getVacuumSkill()->aspirationFinished();                // swallowed all vaccumed actors 

#ifndef NO_COLOR
			EventShow colorChange( 1.0f, getParent()->m_vacuumRestartTimer );
			colorChange.setOverrideColor( Color::white() );
			getParent()->GetActor()->onEvent(&colorChange);
#endif //NO_COLOR
		}

		if ( IS_CURSTATE( STATE_VACCUM_START ) )
		{
			m_vacuumTimer = 0.0f;
		}
	}

    //----------------------------------------------------------------------------------------------------------------------
    u32 Ray_ShooterControllerComponent::StateVacuum::onEvaluateNextStateConditions( u32 _wantedStateID )
    {
        u32 returnedState = U32_INVALID;

        if ( ( _wantedStateID == STATE_VACCUM_START ) || ( _wantedStateID == STATE_VACCUM_LOOP ) )
        {
             if ( getParent()->getTemplate()->getDBG_VaccumAllowPressStop() )
             {
                 u32 nextState = getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE_VACCUM );
                 // Vaccum no more valid ?
                 if ( nextState == U32_INVALID )
                 {
                     returnedState = STATE_VACCUM_FINISHED;
                 }				
             }
             // stick test
             /*if ( !IS_CURSTATE( STATE_VACCUM_START ) && ( m_vacuumTimer >= getParent()->getTemplate()->getVacuumSkill().getStateLoopDuration() ) )
             {
                 returnedState = STATE_VACCUM_FINISHED;
             }*/
         }
        return returnedState;
    }

	//----------------------------------------------------------------------------------------------------------------------
	u32 Ray_ShooterControllerComponent::StateVacuum::onValidateStateChange( u32 _prevStateID, u32 _wantedStateID )
	{
		// Call from different implement
		if ( getStateMachine()->getCurImplement() != this )
			return _wantedStateID;

		u32 returnedState = _wantedStateID;
		
		switch( _wantedStateID )
		{
			case STATE_VACCUM_FINISHED :
			{
				//if ( ( _prevStateID == STATE_VACCUM_START ) || ( getParent()->getVacuumSkill()->getFillValue() == 0 ) )
                if ( getParent()->getVacuumSkill()->isFull() )
                {
                    returnedState = STATE_VACCUM_FINISHED_FULL;
                }
                else
                {
                    returnedState = STATE_VACCUM_FINISHED_FROM_START;
                }
				/*else
				{
					returnedState = STATE_VACCUM_FINISHED_NORMAL;
				}*/
				break;
			}
            case STATE_VACCUM_CATCH :
            {
                //u32 nextState = getStateMachine()->evaluateNextStateConditions( STATE_VACCUM_LOOP );
                if ( getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM_FINISHED ) || m_stopRequested )
                {
                    returnedState = STATE_VACCUM_FINISHED_CATCH;
                }
                else
                {
                    returnedState = STATE_VACCUM_LOOP_CATCH;
                }
                break;
            }
		}

		return returnedState;
	}

	//----------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateVacuum::changeStateCheck( f32 _deltaTime )
	{
		Super::changeStateCheck( _deltaTime );

        u32 currentStateID = getStateMachine()->getCurStateID();

        // reset the shoot press duration => holding 2 buttons keep vacuum prio
        if ( !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM_FINISHED ) )
        {
            getParent()->m_shootButtonPressedDuration = 0.0f;
        }

        if ( !m_stopRequested && ( onEvaluateNextStateConditions( STATE_VACCUM_LOOP ) != U32_INVALID ) )
        {
            m_stopRequested = btrue;
            m_transitionTimer = ( m_transitionTimer / getAnimDuration( STATE_VACCUM_START ) ) * getAnimDuration( STATE_VACCUM_FINISHED_FROM_START );

            f32 oldMaxVal = getParent()->getTemplate()->getVacuumSkill().getStateLoopDuration() + getAnimDuration( STATE_VACCUM_START );
            f32 newMaxVal = getAnimDuration( STATE_VACCUM_FINISHED_FROM_START );
            m_redTimer = ( m_redTimer / oldMaxVal ) * newMaxVal;
        }

        // Shoot ?
        //-----------
        u32 nextState = getStateMachine()->evaluateNextStateConditions( STATE_NORMAL_CHOOSE_ATTACK, bfalse );
        // vacuum hold button is prio
        if ( ( nextState != U32_INVALID ) && ( ( getParent()->m_shootButtonPressedDuration > 0.0f ) || m_stopRequested ) )
        {
            CHANGE_STATE( nextState );
            return;
        }

		switch( currentStateID )
		{
			case STATE_VACCUM_START :
			case STATE_VACCUM_LOOP :
			{
                if ( m_stopRequested )
                {
                    CHANGE_STATE( STATE_VACCUM_FINISHED );
                }
                break;
            }
   		}
	}

	//----------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateVacuum::update( f32 _dt )
	{
		Super::update( _dt );
		
         m_vacuumTimer += _dt;

        // update vaccum power
        f32 powerFactor = 1.0f;
        f32 powerRefAnimDuration = 0.0f;
        f32 redMaxVal = 1.0f;
        
        if ( !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM_FINISHED ) && !m_stopRequested )
        {
            m_transitionTimer += _dt;
            powerRefAnimDuration = getAnimDuration( STATE_VACCUM_START );
            m_transitionTimer = Min( m_transitionTimer, powerRefAnimDuration );
            powerFactor = m_transitionTimer / powerRefAnimDuration;

            m_redTimer +=  _dt;
            redMaxVal = getParent()->getTemplate()->getVacuumSkill().getStateLoopDuration() + powerRefAnimDuration;
        }
        else
        {
            m_transitionTimer -= _dt;
            powerRefAnimDuration = getAnimDuration( STATE_VACCUM_FINISHED_FROM_START );
            m_transitionTimer = Max( m_transitionTimer, 0.0f );
            powerFactor = m_transitionTimer / powerRefAnimDuration;

            m_redTimer -=  _dt;
            redMaxVal = powerRefAnimDuration;
        }
        getParent()->getVacuumSkill()->setPowerFactor( Clamp( powerFactor, 0.0f, 1.0f ) );
       
		//if ( IS_CURSTATE( STATE_VACCUM_LOOP ) )
		{
#ifndef NO_COLOR
			EventShow colorChange( 1.0f, 0.0f );
            f32 colorBlendFactor = m_redTimer / redMaxVal;
			colorChange.setOverrideColor( Color::Interpolate( Color::white(), Color::red(), Min( 1.0f, colorBlendFactor ) ) );
			getParent()->GetActor()->onEvent(&colorChange);
#endif //NO_COLOR
		}

		{
			//u32 currentStateID = getStateMachine()->getCurStateID();
			//u32 vaccumSize = getParent()->getVacuumSkill()->getTotalVaccumedSizeFromStart();
			
            // Not finished, aspirate actors
            //
            //if ( !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM_FINISHED ) )
            if ( !IS_CURSTATE( STATE_VACCUM_FINISHED_FULL ) )
			{
				getParent()->getVacuumSkill()->aspirate();
			}
			
			Vec3d pos;
			getParent()->getSwallowPos(pos);
			getParent()->getVacuumSkill()->aspirationUpdate( _dt, pos );

            // may have changed state during aspiration finished ( hit )
            if ( !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM ) )
            {
                return;            
            }

			// Check if the fill factor has changed to change to catch state
			//
			//if ( vaccumSize != getParent()->getVacuumSkill()->getTotalVaccumedSizeFromStart() )
			if ( getParent()->getVacuumSkill()->isFillValueHasChanged() )
			{
                // Add reward
                REWARD_MANAGER->Action_New(Ray_RewardManager::Action_VacuumKnack, getParent()->getPlayerIndex() );

				if ( ( getParent()->getTemplate()->getDBG_CanVaccumIfFull() && ( getParent()->getVacuumSkill()->getTotalVaccumedSizeFromStart() >= getParent()->getTemplate()->getVacuumSkill().getStackFullSize() ) ) ||
					 ( !getParent()->getTemplate()->getDBG_CanVaccumIfFull() && getParent()->getVacuumSkill()->isFull() ) 
					)
				{
					CHANGE_STATE( STATE_VACCUM_FINISHED_FULL );
				}
				else
				{
                    CHANGE_STATE( STATE_VACCUM_CATCH );
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateVacuum::changeStateCheckAnimFinished()
	{
        //if ( IS_CURSTATE( STATE_VACCUM_LOOP_CATCH ) )
        //{
        //    CHANGE_STATE( STATE_VACCUM_FINISHED );        
        //}

		if (	IS_CURSTATE( STATE_VACCUM_START ) )// ||
				//IS_CURSTATE( STATE_VACCUM_CATCH ) )
		{
			CHANGE_STATE( STATE_VACCUM_LOOP );
		}
		else if ( ( IS_CURSTATE( STATE_VACCUM_FINISHED_FULL ) || 
					//IS_CURSTATE( STATE_VACCUM_FINISHED_NORMAL ) //|| 
					IS_CURSTATE(  STATE_VACCUM_FINISHED_FROM_START ) ) && !getParent()->getVacuumSkill()->hasVacuumedActors() )
		{
			CHANGE_STATE( STATE_NORMAL );
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateVacuum::chooseAnimation()
	{	

	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateVacuum::updateAnimation( f32 _dt )
	{	
        const StringID s_ProceduralCursor = ITF_GET_STRINGID_CRC(ProceduralCursor,321505276);

		if ( IS_CURSTATE( STATE_VACCUM_FINISHED_FROM_START ) )
		{
			m_startAnimTimer -= _dt;
			f32 cursor = m_startAnimTimer / getAnimDuration( getStateMachine()->getCurStateID() );
			if ( cursor <= 0.0f )
			{
				cursor = 0.0f;
				m_startAnimTimer = 0.0f;
				getCurState()->setFinished();
			}
			getAnimComponent()->setInput( s_ProceduralCursor, 1.0f - cursor );
		}
		else if ( IS_CURSTATE( STATE_VACCUM_START ) )
		{
			m_startAnimTimer += _dt;
			f32 animDuration = getAnimDuration( getStateMachine()->getCurStateID() );
			f32 cursor = m_startAnimTimer / animDuration;
			if ( cursor >= 1.0f )
			{
				cursor = 1.0f;
				m_startAnimTimer = animDuration;//getCurState()->getCurAnimDuration();
				getCurState()->setFinished();
			}
			getAnimComponent()->setInput( s_ProceduralCursor, cursor );
		}
	}	

    //------------------------------------------------------------------------------------------------------------------
    f32  Ray_ShooterControllerComponent::StateVacuum::getAnimDuration( u32 _stateID )
    {
        f32 animDuration = getCurState()->getCurAnimDuration();
        if ( _stateID == STATE_VACCUM_START )
        {
            animDuration = getParent()->getTemplate()->getVacuumSkill().getStateStartDuration();
        }
        else if ( _stateID == STATE_VACCUM_FINISHED_FROM_START )
        {
            animDuration = getParent()->getTemplate()->getVacuumSkill().getStateStopDuration();
        }
        return animDuration;
    }


}