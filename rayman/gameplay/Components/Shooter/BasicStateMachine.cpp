#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_BASICSTATEMACHINE_H_
#include "rayman\gameplay\Components\Shooter\BasicStateMachine.h"
#endif //_ITF_BASICSTATEMACHINE_H_

namespace ITF
{
	IMPLEMENT_OBJECT_RTTI( BasicStateMachine )
	IMPLEMENT_OBJECT_RTTI( WithAnimStateMachine )
	IMPLEMENT_OBJECT_RTTI( PlayerStateMachine )

	IMPLEMENT_OBJECT_RTTI( BasicState )
	IMPLEMENT_OBJECT_RTTI( WithAnimState )
	
	IMPLEMENT_OBJECT_RTTI( BasicStateImplement )
	IMPLEMENT_OBJECT_RTTI( WithAnimStateImplement )
	IMPLEMENT_OBJECT_RTTI( PlayerStateImplement )


	//-------------------------------------------------------------------------------------------------------------
	//												STATES
	//-------------------------------------------------------------------------------------------------------------
	WithAnimState::WithAnimState()
	: Super()
	, m_restartAnim(bfalse)
	, m_endCheckByAnimEvent( bfalse )
	, m_endCheckByAnimEnd( btrue )
    , m_restartAnimIfSame( btrue )
	//, m_animFinished( bfalse )
	{
		m_defaultAnim.invalidate(); 
		m_currentAnim.invalidate();
		m_wantedAnim.invalidate();
		reset();
	}
	//-------------------------------------------------------------------------------------------------------------
	void WithAnimState::startAnimation( AnimatedComponent * _animComponent, StringID const & _defaultAnim )
	{
		if ( _animComponent == NULL )
			return;

		if ( !m_currentAnim.isValid() && !m_wantedAnim.isValid() )
		{
			if ( !_defaultAnim.isValid() )
			{
				return;
			}
			m_wantedAnim = _defaultAnim;
		}
		
		if ( ( m_currentAnim != m_wantedAnim ) || m_restartAnim )
		{
			_animComponent->setAnim( m_wantedAnim );
			if ( m_restartAnim )
			{
				_animComponent->resetTree();
			}
			m_currentAnim	= m_wantedAnim;
			m_restartAnim	= bfalse;
			//m_animFinished	= bfalse;
		}
	}

	//-------------------------------------------------------------------------------------------------------------
	f32 WithAnimState::getCurAnimDuration() const
	{
		if ( !m_currentAnim.isValid() )
		{
			return 0.0f;
		}
	
		WithAnimStateMachine * animStateMachine = getStateMachine();
		
		ITF_ASSERT( animStateMachine != NULL ); // anim state must have an animation state machine 
		if ( animStateMachine != NULL )
		{
			AnimatedComponent * animComponent = animStateMachine->getAnimComponent();
			ITF_ASSERT( animComponent != NULL ); // anim state machine must have an anim component
			if ( animComponent != NULL )
			{
				SubAnim *subAnim = animComponent->getSubAnim( m_currentAnim ); 
				return ( ( subAnim != NULL ) ? subAnim->getDuration() : 0.0f );
			}
		}
		return 0.0f;
	}

	//-------------------------------------------------------------------------------------------------------------
	bbool WithAnimState::isCurAnimFinished() const
	{
		WithAnimStateMachine * animStateMachine = getStateMachine();

		ITF_ASSERT( animStateMachine != NULL ); // anim state must have an animation state machine 
		if ( animStateMachine != NULL )
		{
			AnimatedComponent * animComponent = animStateMachine->getAnimComponent();
			ITF_ASSERT( animComponent != NULL ); // anim state machine must have an anim component
			if ( animComponent != NULL )
			{
				if (	( m_endCheckByAnimEnd && animComponent->isMainNodeFinished() ) || 
						( m_endCheckByAnimEvent && getStateMachine()->isCurAnimFinishedFormEvent() ) 
					)
				{
					return btrue;
				}
			}
		}
		return bfalse;
	}
	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------
	/*void WithAnimStateImplement::changeStateCheck( f32 _dt )
	{
		Super::changeStateCheck( _dt );

		// Check anim finished only of the state is still running
		//
		if ( !getStateMachine()->isCurStateHasJustChanged() )
		{
			AnimatedComponent * animComponent = getAnimComponent();
			if ( animComponent != NULL )
			{
				bbool checkEvent = bfalse;
				bbool checkAnimEnd = btrue;

				BasicState * curState = getStateMachine()->getCurState();
				WithAnimState * animState = curState->DynamicCast<WithAnimState>(ITF_GET_STRINGID_CRC(WithAnimState,3620460103));

				if ( animState != NULL )
				{
					checkEvent		= animState->getEndCheckByAnimEvent();
					checkAnimEnd	= animState->getEndCheckByAnimEnd();
				}

				if ( ( checkAnimEnd && animComponent->isMainNodeFinished() ) || 
					 ( checkEvent && static_cast<WithAnimStateMachine*>( getStateMachine())->isCurAnimFinishedFormEvent() ) ||
					 ( getCurState()->isAnimFinished() )
				   )
				{
					getCurState()->setAnimFinished();
					changeStateCheckAnimFinished();
				}
			}
		}
	}*/

	//-------------------------------------------------------------------------------------------------------------
	void WithAnimStateImplement::setOwner( BasicStateMachine * _owner )
	{
		Super::setOwner( _owner );
		ITF_ASSERT( _owner->DynamicCast<WithAnimStateMachine>(ITF_GET_STRINGID_CRC(WithAnimStateMachine,1042035092)) ); // owner state machine must be a "with anim state machine"
	}

	//-------------------------------------------------------------------------------------------------------------
	//											STATES MACHINES
	//-------------------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------------------
	BasicStateMachine::BasicStateMachine()
	: m_currentState( NULL )
	, m_currentStateImplement( NULL )
	, m_currentStateID( U32_INVALID )
	, m_previousStateID( U32_INVALID )
	, m_nextStateID( U32_INVALID )
	, m_stateHasJustChanged( bfalse )
	, m_isChangingImplement( bfalse )
	, m_currentStateStartTime( 0.f )
	, m_currentStateStartDuration( 0.f )
	, m_parent( NULL )
	{
	}

	//-------------------------------------------------------------------------------------------------------------
	BasicStateMachine::~BasicStateMachine()
	{
		for ( u32 i = 0 ; i < m_stateList.size(); ++i )
		{
			BasicState * stateToDel = m_stateList[i];
			if ( ( stateToDel != NULL ) && stateToDel->isAlloc() )
			{
				ITF_DELETE( stateToDel );
			}
		}
		m_stateList.clear();

		for ( u32 i = 0 ; i < m_stateImplements.size(); ++i )
		{
			ITF_DELETE( m_stateImplements[i] );
		}
		m_stateImplements.clear();
	}

	//-------------------------------------------------------------------------------------------------------------
	BasicState * BasicStateMachine::addState( u32 _state, StringID const & _name, BasicStateImplement * _basicStateImplement /*=NULL*/ )
	{
		ITF_ASSERT_CRASH( _state < m_stateList.capacity(), "## BasicStateMachine ## addState : Invalid state idx !" );
		// Do you have called reserveSize ?
		
		// Create the state
		//-------------------
		BasicState * _basicState = createState();
		if ( _basicState == NULL )
		{
			return NULL;
		}
		_basicState->setName( _name );
		_basicState->setOwner( this );
		_basicState->setImplement( _basicStateImplement );

		// Store state
		m_stateList[ _state ] = _basicState;

		// Store the state implement 
		//----------------------------
		if ( _basicStateImplement != NULL )
		{
			i32 implementIdx = m_stateImplements.find( _basicStateImplement );
			if ( implementIdx < 0 )
			{
				m_stateImplements.push_back( _basicStateImplement );
				_basicStateImplement->setOwner( this );
			}
		}

		return _basicState;
	}

	//-------------------------------------------------------------------------------------------------------------
	bbool BasicStateMachine::changeState( u32 _nextStateID )
	{
		ITF_ASSERT_CRASH( _nextStateID < m_stateList.size(),"## BasicStateMachine ## changeState : Invalid state idx !" );

	    m_nextStateID = _nextStateID;

		// validate change state
		//-----------------------
		u32 i;
		BasicState * nextState = NULL;
		for ( i = 0; i < 100; ++i ) 
		{
			u32 prevStateID = m_nextStateID;
			nextState = getState( m_nextStateID );
			ITF_ASSERT_MSG( nextState != NULL, "## BasicStateMachine ## changeState : given state is NULL !" );

			if ( ( nextState != NULL ) && ( nextState->getImplement() != NULL ) )
			{
				m_nextStateID = nextState->getImplement()->onValidateStateChange( ( prevStateID == m_nextStateID ? m_currentStateID : prevStateID ), m_nextStateID );
                
                // no changes wanted
                if ( m_nextStateID == U32_INVALID )
                {
                    m_nextStateID = U32_INVALID;
                    return bfalse;
                }
			}
			if ( prevStateID == m_nextStateID )
				break;
		}
		ITF_ASSERT_MSG( i < 100, "## BasicStateMachine ## changeState, onValidateStateChange : to many change state requested !");

		if ( nextState == NULL )
		{
            m_nextStateID = U32_INVALID;
			return bfalse;
		}		

		// Changing implement ?
		m_isChangingImplement = ( nextState->getImplement() != m_currentStateImplement );
		
		// Call current state exit
		if ( m_currentStateImplement != NULL )
		{
			m_currentStateImplement->onExit();
		}

		// Apply next state
		m_previousStateID		= m_currentStateID;
		m_currentStateID		= m_nextStateID;
		m_currentState			= nextState;
		m_currentState->reset();
		m_currentStateImplement = nextState->getImplement();
		m_currentStateStartTime	= SYSTEM_ADAPTER->getTime();
		m_currentStateStartDuration = 0.0f;

		// Call enter function
		if ( m_currentStateImplement != NULL )
		{
			m_currentStateImplement->onEnter();
		}

		m_nextStateID = U32_INVALID;
		m_stateHasJustChanged = btrue;
		m_isChangingImplement = bfalse;
        
        return btrue;
	}

	//-------------------------------------------------------------------------------------------------------------
	void BasicStateMachine::update( f32 _dt )
	{
		// Update state start duration
		//
		if ( ( SYSTEM_ADAPTER->getTime() - m_currentStateStartTime ) > 0.0f )
		{
			m_currentStateStartDuration += _dt;
		}

		// Automatic changes
		//
		if ( ( m_currentState != NULL ) && m_currentState->isFinished() && ( m_currentState->getDefaultNextStateID() != U32_INVALID ) )
		{
            changeState( m_currentState->getDefaultNextStateID() );
		}

		// Update state update
		//
		if ( ( m_currentState != NULL ) && ( m_currentStateImplement != NULL ) )
		{
			u32 i;
			// Check state change
			//---------------------
			for ( i = 0; i < 100; ++i ) 
			{
				u32 prevStateID = m_currentStateID;
				if ( m_currentStateImplement != NULL )
				{
					m_currentStateImplement->changeStateCheck( _dt );
				}
				if ( prevStateID == m_currentStateID )
					break;
			}
			ITF_ASSERT_MSG( i < 100, "## BasicStateMachine ## changeStateCheck : to many change state !");

			// Update the state
			if ( m_currentStateImplement != NULL )
			{
				m_currentStateImplement->update( _dt );
			}			
		}
		m_stateHasJustChanged = bfalse;
	}

	//-------------------------------------------------------------------------------------------------------------
	u32 BasicStateMachine::evaluateNextStateConditions( u32 _wantedStateID, bbool _changeIfValid /*= bfalse*/ )
	{
		BasicState * state = getState( _wantedStateID );
		if ( state != NULL )
		{
			if ( state->getImplement() != NULL )
			{
				u32 resState = state->getImplement()->onEvaluateNextStateConditions( _wantedStateID );
				if ( ( resState != U32_INVALID ) && _changeIfValid )
				{
					changeState( resState );
				}
				return resState;
			}
		}
		return U32_INVALID;
	}

    //-------------------------------------------------------------------------------------------------------------
    void BasicStateMachine::onEvent( Event * _evt )
    {
        if ( m_currentStateImplement != NULL )
        {
            m_currentStateImplement->onEvent( _evt );
        }			
    }
	
	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------------------------------------------
	bbool WithAnimStateMachine::changeState( u32 _nextStateID )
	{
        bbool prevStateJustChanged = m_stateHasJustChanged;
		bbool res = Super::changeState( _nextStateID );

        if ( !res )
        {
            m_changeStateRestartAnim = bfalse;
            return res;
        }

		m_curAnimFinishedFromEvent = bfalse;

		if ( m_currentState != NULL && ( m_animComponent != NULL ) )
		{
            bbool restartAnim = btrue;
			WithAnimState * animState = static_cast<WithAnimState*>( m_currentState );
            WithAnimState * animPrevState = NULL;
            if ( m_previousStateID != U32_INVALID )
            {
                animPrevState = static_cast<WithAnimState*>( getState( m_previousStateID ) );
                if ( !m_changeStateRestartAnim && !prevStateJustChanged 
                    && ( animPrevState != NULL ) 
                    && !animState->getRestartAnimIfSame()
                    && ( animPrevState->getDefaultAnimation() == animState->getDefaultAnimation() ) )
                {
                    restartAnim = bfalse;
                }
                // update anim input
                if ( animPrevState != NULL )
                {
                    m_animComponent->setInput( getStateName( m_previousStateID ), 0 );
                }
            }
            // update anim input
            m_animComponent->setInput( m_currentState->getName(), 1 );
            
			animState->setWantedAnimation( animState->getDefaultAnimation(), restartAnim );
		}

        m_changeStateRestartAnim = bfalse;
        
        return res;
	}

	//-------------------------------------------------------------------------------------------------------------
	void WithAnimStateMachine::update( f32 _dt  )
	{
		WithAnimState * animState = static_cast<WithAnimState*>( m_currentState );
		WithAnimStateImplement * animStateImplement = NULL;
		if ( m_currentStateImplement != NULL )
		{
			animStateImplement	= m_currentStateImplement->DynamicCast<WithAnimStateImplement>(ITF_GET_STRINGID_CRC(WithAnimStateImplement,858798109));
		}

		// Check animation end
		if ( ( animState != NULL ) && ( animState->isCurAnimFinished() || animState->isFinished() ) )
		{
            animState->setFinished();
			if ( animStateImplement != NULL )
			{
				animStateImplement->changeStateCheckAnimFinished();
			}
		}

		// Update basic state
		//
		Super::update( _dt );

		// get anim/imp state again because they could have changed in previous update
		animState = static_cast<WithAnimState*>( m_currentState );
		if ( m_currentStateImplement != NULL )
		{
			animStateImplement	= m_currentStateImplement->DynamicCast<WithAnimStateImplement>(ITF_GET_STRINGID_CRC(WithAnimStateImplement,858798109));
		}
		
		if ( animState != NULL )
		{
			// Choose animation
			//
			if ( animStateImplement != NULL )
			{
				animStateImplement->chooseAnimation();
			}
			// Start state animation
			//
			animState->startAnimation( m_animComponent, animState->getDefaultAnimation() );

			// update the current animation
			//
			if ( animStateImplement != NULL )
			{
				animStateImplement->updateAnimation( _dt );
			}
		}		
	}

	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------------------------------
	
	PlayerStateMachine::PlayerStateMachine()
		: Super()
		//, m_playerIndex( U32_INVALID )
	{
		//for ( u32 i = 0; i < JOY_MAX_BUT; ++i )
		//{
		//	m_buttons[i] = InputAdapter::Released;
		//}
		//for ( u32 i = 0; i < JOY_MAX_AXES; ++i )
		//{
		//	m_axes[i] = 0.0f;
		//}	
	}

    //-------------------------------------------------------------------------------------------------------------
    //void PlayerStateMachine::setAxesAndButtons( const f32 * _axes, const InputAdapter::PressStatus * _buttons )
    //{
    //    ITF_Memcpy( m_buttons, _buttons, sizeof(m_buttons) );
    //    ITF_Memcpy( m_axes, _axes, sizeof(m_axes) );
    //}

	//-------------------------------------------------------------------------------------------------------------
	void PlayerStateMachine::update( f32 _dt  )
	{
		// update buttons + axes
		/*INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, m_playerIndex, m_buttons, JOY_MAX_BUT);
		INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentEngine, m_playerIndex, m_axes, JOY_MAX_AXES);*/

		// Update states
		Super::update( _dt );
	}

	//-------------------------------------------------------------------------------------------------------------
	/*void PlayerStateMachine::changeStateCheck( f32 _deltaTime, const InputAdapter::PressStatus* _buttons, f32* _axes )
	{
		if ( ( m_currentState == NULL ) || ( m_currentStateImplement == NULL ) )
			return;

		u32 i;
		for ( i = 0; i < 100; ++i ) 
		{
			PlayerStateImplement * curPlayerStateImp = m_currentStateImplement->DynamicCast<PlayerStateImplement>(ITF_GET_STRINGID_CRC(PlayerStateImplement,1435752678));
			u32 prevStateID = m_currentStateID;
			if ( curPlayerStateImp != NULL )
			{
				curPlayerStateImp->changeStateCheck( _deltaTime, _buttons, _axes );
			}
			if ( prevStateID == m_currentStateID )
				break;
		}
		ITF_ASSERT_MSG( i < 100, "## PlayerStateMachine ## changeStateCheck : to many change state !");
	}*/

	

	//-------------------------------------------------------------------------------------------------------------
	//											   TEMPLATES
	//-------------------------------------------------------------------------------------------------------------
/*
	BEGIN_SERIALIZATION( BasicStateMachine_Template )
		SERIALIZE_CONTAINER_OBJECT( "stateDescriptionList",m_stateDescriptionList, ESerializeGroup_TemplateEditable );
	END_SERIALIZATION()

	BasicStateMachine_Template::BasicStateMachine_Template()
	{
	}
	
	BEGIN_SERIALIZATION_SUBCLASS( BasicStateMachine_Template, StateDescription )
		SERIALIZE_MEMBER				( "name",	m_name, ESerializeGroup_TemplateEditable );
		SERIALIZE_OBJECT_WITH_FACTORY	( "class",	m_class, NULL, ESerializeGroup_TemplateEditable );
	END_SERIALIZATION()
*/

}
