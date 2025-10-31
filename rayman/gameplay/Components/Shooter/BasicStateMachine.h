#ifndef _ITF_BASICSTATEMACHINE_H_
#define _ITF_BASICSTATEMACHINE_H_

namespace ITF
{
	//-------------------------------------------------------------------------------------------------------------
	class BasicStateMachine;
	class BasicStateImplement;
	class BasicState : public IRTTIObject
	{
		DECLARE_OBJECT_CHILD_RTTI( BasicState, IRTTIObject ,1175823631)
	public:
		BasicState() 
			: m_isAlloc( bfalse )
			, m_implement( NULL )
			, m_owner( NULL )
			, m_defaultNextStateID( U32_INVALID )
			, m_isFinished( bfalse ) 
		{ 
			m_name.invalidate(); 
		}
		virtual ~BasicState(){}

		ITF_INLINE void						setName( StringID const & _name )				{ m_name = _name; }
		ITF_INLINE StringID const &			getName() const									{ return m_name; }
		ITF_INLINE void						setIsAlloc()									{ m_isAlloc = btrue; }
		ITF_INLINE bbool					isAlloc() const									{ return m_isAlloc; }
		ITF_INLINE void						setImplement( BasicStateImplement * _implement ){ m_implement = _implement; }
		ITF_INLINE BasicStateImplement *	getImplement() const							{ return m_implement; }
		ITF_INLINE void						setOwner( BasicStateMachine * _owner )			{ m_owner = _owner; }
		ITF_INLINE BasicStateMachine*		getStateMachine() const							{ return m_owner; }

		ITF_INLINE void						setFinished()									{ m_isFinished = btrue; }
		ITF_INLINE bbool					isFinished() const								{ return m_isFinished; }
		
		ITF_INLINE void						setDefaultNextStateID( u32 _nextStateID )		{ m_defaultNextStateID = _nextStateID; }
		ITF_INLINE u32						getDefaultNextStateID() const					{ return m_defaultNextStateID; }

		virtual void						reset()											{ m_isFinished = bfalse; }

	private:
		StringID				m_name;
		bbool					m_isAlloc;

	protected:
		BasicStateImplement *	m_implement;
		BasicStateMachine *		m_owner;

		bbool					m_isFinished;
		u32						m_defaultNextStateID;
	};


	//-------------------------------------------------------------------------------------------------------------
	class WithAnimStateMachine;
	class WithAnimState : public BasicState
	{
		DECLARE_OBJECT_CHILD_RTTI( WithAnimState, BasicState ,3620460103)

	public:
		WithAnimState();
		virtual ~WithAnimState(){}

		//virtual void					reset()								{ Super::reset(); m_animFinished = bfalse; }

		// function called after chooseAnimation of implement to start the choosen animation else the default anim is played
		virtual void					startAnimation( AnimatedComponent * _animComponent, StringID const & _defaultAnim );

		f32  							getCurAnimDuration()	const;
		bbool							isCurAnimFinished()		const;

		// Accessors inline
		//
		ITF_INLINE WithAnimStateMachine* getStateMachine() const;
		
		ITF_INLINE const StringID &  	getDefaultAnimation()	const		{ return m_defaultAnim; }
		ITF_INLINE const StringID &  	getCurrentAnimation()	const		{ return m_currentAnim; }
		ITF_INLINE const StringID &  	getWantedAnimation()	const		{ return m_wantedAnim; }
		ITF_INLINE bbool  				getEndCheckByAnimEvent() const		{ return m_endCheckByAnimEvent; }
		ITF_INLINE bbool  				getEndCheckByAnimEnd()	const		{ return m_endCheckByAnimEnd; }
        ITF_INLINE bbool  				getRestartAnimIfSame()	const		{ return m_restartAnimIfSame; }
		

		//ITF_INLINE bbool  				isAnimFinished()		const		{ return m_animFinished; }
		
		ITF_INLINE void  				setDefaultAnimation( StringID const & _defaultAnim )						{ m_defaultAnim = _defaultAnim; }
		ITF_INLINE void  				setWantedAnimation( StringID const & _wantedAnim, bbool _restart = bfalse )	{ m_wantedAnim = _wantedAnim; m_restartAnim = _restart; }
		ITF_INLINE void  				setEndCheckByAnimEvent( bbool _keepAnimEndCheck = bfalse )					{ m_endCheckByAnimEvent = btrue; m_endCheckByAnimEnd = _keepAnimEndCheck; }
        ITF_INLINE void  				setEndCheckByAnimEnd( bbool _AnimEndCheck )					                { m_endCheckByAnimEnd = _AnimEndCheck; }
        ITF_INLINE void  				setRestartAnimIfSame( bbool _restart = btrue )					            { m_restartAnimIfSame = _restart;  }
		//ITF_INLINE void  				setAnimFinished()															{ m_animFinished = btrue; }

	private:
		StringID				m_defaultAnim;
		StringID				m_currentAnim;
		StringID				m_wantedAnim;
		bbool					m_restartAnim;
		//bbool					m_animFinished;
		
		bbool					m_endCheckByAnimEvent;
		bbool					m_endCheckByAnimEnd;
        bbool					m_restartAnimIfSame;
	};
	
	//-------------------------------------------------------------------------------------------------------------
	//										STATE IMPLEMENT
	//-------------------------------------------------------------------------------------------------------------
	class BasicStateImplement : public IRTTIObject
	{
		DECLARE_OBJECT_CHILD_RTTI( BasicStateImplement, IRTTIObject ,1467857618)

	public:
		BasicStateImplement() : m_owner(NULL)	{}
		virtual ~BasicStateImplement()			{}

		// Function called at state enter
		virtual void onEnter(){}
		// Function called at state exit
		virtual void onExit(){}
		// Allow the user to validate the state change request; is called when the stateChange function is called.
		// Warring ! This function is called on the "state implement" of the requested state !
		// returns the final state which will be applied
		virtual u32	 onValidateStateChange( u32 _prevStateID, u32 _wantedStateID )	{ return _wantedStateID; }
		// Function called before state update to allow state change
		virtual void changeStateCheck( f32 _dt ){}
		// Function called to update the state
		virtual void update( f32 _dt ){}
		// This function can be used to centralized state change condition tests.
		// It is generally called by the state machine when it's requested.
		// Returns the result next state or U32_INVALID if conditions are not validated
		virtual u32 onEvaluateNextStateConditions( u32 _wantedStateID ){ return U32_INVALID; }
        // Is called when an event is received
        virtual void onEvent( Event * _evt )    {}

		ITF_INLINE virtual void			setOwner( BasicStateMachine * _owner )	{ ITF_ASSERT( _owner != NULL ); m_owner = _owner; }
		ITF_INLINE IRTTIObject *		getParent()	const;
		ITF_INLINE BasicStateMachine *	getStateMachine() const					{ return m_owner; }
		ITF_INLINE BasicState *			getCurState() const;
		ITF_INLINE BasicState *			getState( u32 _stateId ) const;

	protected:
		BasicStateMachine * m_owner;
	};

	//-------------------------------------------------------------------------------------------------------------
	class WithAnimStateImplement : public BasicStateImplement
	{
		DECLARE_OBJECT_CHILD_RTTI( WithAnimStateImplement, BasicStateImplement ,858798109)

	public:
		WithAnimStateImplement() :Super()	{}
		virtual ~WithAnimStateImplement()	{}

		// Function called before state update to allow state change
		//virtual void					changeStateCheck( f32 _dt );
		// Function called by changeStateCheck to manage animation finished state changement
		virtual void					changeStateCheckAnimFinished()	{}
		// function called after state update to allow changing animation
		virtual void					chooseAnimation()	{}
		// function called after startAnimation to allow updating animation inputs
		virtual void					updateAnimation( f32 _dt )		{}

		// Accessors
		//
		virtual void							setOwner( BasicStateMachine * _owner );
		ITF_INLINE AnimatedComponent *			getAnimComponent()	const;
		ITF_INLINE class WithAnimStateMachine *	getStateMachine()	const;
		ITF_INLINE WithAnimState*				getCurState()		const;
	};

	//-------------------------------------------------------------------------------------------------------------
	class PlayerStateImplement : public WithAnimStateImplement
	{
		DECLARE_OBJECT_CHILD_RTTI( PlayerStateImplement, WithAnimStateImplement ,1435752678)

	public:
		PlayerStateImplement():Super(){};
		virtual ~PlayerStateImplement(){}

		ITF_INLINE class PlayerStateMachine *			getStateMachine()	const;
		ITF_INLINE const  InputAdapter::PressStatus *	getButtons()		const;
		ITF_INLINE const  f32 *							getAxes()			const;

	};
	
	//-------------------------------------------------------------------------------------------------------------
	//											STATE MACHINE
	//-------------------------------------------------------------------------------------------------------------
	class BasicStateMachine : public IRTTIObject
	{
		DECLARE_OBJECT_CHILD_RTTI(BasicStateMachine, IRTTIObject,3161180923)

	public:
		BasicStateMachine();
		virtual ~BasicStateMachine();

		template <class T> static T* allocStateImplement()
		{
			T * basicStateImplement  = newAlloc( mId_Gameplay, T() );
			return basicStateImplement;
		}
		
		void						reserveSize( u32 _stateCount )		{ m_stateList.resize( _stateCount ); m_stateList.fill( NULL ); }
		void						initialize( IRTTIObject * _parent )	{ m_parent = _parent; }
		
		virtual BasicState *		addState( u32 _state, StringID const & _name, BasicStateImplement * _basicStateImplement = NULL );
		virtual bbool				changeState( u32 _nextStateID );
		virtual void				update( f32 _dt );
		// this function call the onEvaluateNextStateConditions of given state ID implement.
		// Returns the result next state 
		virtual u32					evaluateNextStateConditions( u32 _wantedStateID, bbool _changeIfValid = bfalse );
		// Function can be called when the cur state is finished
		virtual void				setCurStateFinished()			{ if ( m_currentState ) m_currentState->setFinished(); }
        // call "on event" function on current state implement
        virtual void                onEvent( Event * _evt );

		ITF_INLINE IRTTIObject *	getParent()	const				{ return m_parent; }
		ITF_INLINE u32				getCurStateID() const   		{ return m_currentStateID; }
		ITF_INLINE u32				getPrevStateID() const			{ return m_previousStateID; }
        ITF_INLINE u32				getNextStateID() const			{ return m_nextStateID; }
        ITF_INLINE bbool			isChangingImplement()const		{ return m_isChangingImplement; }
		ITF_INLINE bbool			isCurStateHasJustChanged()const	{ return m_stateHasJustChanged; }
		ITF_INLINE f32				getCurStateStartDuration()const	{ return m_currentStateStartDuration; }
		
		ITF_INLINE BasicState *		getState( u32 _stateID ) const			{ ITF_ASSERT_CRASH( _stateID < m_stateList.size(), "Invalid state idx !" ); return m_stateList[_stateID]; }
		ITF_INLINE BasicState *		getCurState() const						{ return m_currentState; }
		ITF_INLINE const StringID &	getStateName( u32 _stateID ) const		{ ITF_ASSERT_CRASH( _stateID < m_stateList.size(), "Invalid state idx !" ); ITF_ASSERT_CRASH( m_stateList[_stateID] != NULL, "NULL state !" ); return m_stateList[_stateID]->getName(); }
		ITF_INLINE const StringID &	getCurStateName() const					{ return getStateName( m_currentStateID ); }

		ITF_INLINE BasicStateImplement *getCurImplement() const				{ return m_currentStateImplement; }

		ITF_INLINE bbool			isCurStateInGroupRange( u32 _startRangeId, u32 _stopRangeId ) const				{ return isStateInGroupRange( m_currentStateID, _startRangeId, _stopRangeId ); }
        ITF_INLINE static bbool	    isStateInGroupRange( u32 _stateId, u32 _startRangeId, u32 _stopRangeId )	    { if ( ( _stateId >= _startRangeId )&&( _stateId <= _stopRangeId )) return btrue; else return bfalse; }

	protected:
		template <class T> T* allocState()
		{
			T * basicState  = newAlloc( mId_GameplayMisc, T() );
			if ( basicState != NULL )
			{
				basicState->setIsAlloc();
			}
			return basicState;
		}
		virtual BasicState *		createState()					{ return allocState<BasicState>(); }

	protected:

		BasicStateImplement	*	m_currentStateImplement;
		BasicState *  			m_currentState;
		u32  					m_currentStateID;
		u32  					m_previousStateID;
		u32 					m_nextStateID;				// Valid only during enter/exit function

		SafeArray<BasicState*>	m_stateList;
		SafeArray<BasicStateImplement*>	m_stateImplements;

		bbool					m_stateHasJustChanged;
		bbool					m_isChangingImplement;
		f64						m_currentStateStartTime;
		f32						m_currentStateStartDuration;

		IRTTIObject *			m_parent;
	};

	//-------------------------------------------------------------------------------------------------------------
	class WithAnimStateMachine : public BasicStateMachine
	{
		DECLARE_OBJECT_CHILD_RTTI( WithAnimStateMachine, BasicStateMachine,1042035092)

	public:
		WithAnimStateMachine() : Super(), m_animComponent( NULL ), m_curAnimFinishedFromEvent( bfalse ), m_changeStateRestartAnim( bfalse ) {}
		virtual ~WithAnimStateMachine(){}

		virtual void					update( f32 _dt );

		void							initialize( IRTTIObject * _parent, AnimatedComponent * _animComponent )	{ BasicStateMachine::initialize( _parent ); m_animComponent = _animComponent; }
		virtual bbool					changeState( u32 _nextStateID );
        virtual bbool					changeStateRestartAnim( u32 _nextStateID )      { m_changeStateRestartAnim = btrue; return changeState( _nextStateID ); }

		ITF_INLINE AnimatedComponent *	getAnimComponent()	const						{ return m_animComponent; }
		ITF_INLINE void					setAnimFinishedFormEvent()						{ m_curAnimFinishedFromEvent = btrue; }
		ITF_INLINE bbool				isCurAnimFinishedFormEvent() const				{ return m_curAnimFinishedFromEvent; }

		ITF_INLINE WithAnimState *		getAnimState( u32 _stateID ) const				{ return static_cast<WithAnimState *>( getState( _stateID ) ); }
		ITF_INLINE WithAnimState *		getAnimCurState() const							{ return static_cast<WithAnimState *>( getCurState() ); }
	
	protected:

		virtual BasicState *			createState()													{ return allocState<WithAnimState>(); }

		AnimatedComponent * m_animComponent;
		bbool				m_curAnimFinishedFromEvent;
        bbool               m_changeStateRestartAnim;
		
	};


	//-------------------------------------------------------------------------------------------------------------
	class PlayerStateMachine : public WithAnimStateMachine
	{
		DECLARE_OBJECT_CHILD_RTTI( PlayerStateMachine, WithAnimStateMachine ,1620634591)

	public:
		PlayerStateMachine();
		virtual ~PlayerStateMachine(){};

		void						initialize( IRTTIObject * _parent, AnimatedComponent * _animComponent/*, u32 _playerIndex*/ )	{ WithAnimStateMachine::initialize( _parent, _animComponent );/* m_playerIndex = _playerIndex;*/ }
		virtual void				update( f32 _dt );

		//ITF_INLINE const  InputAdapter::PressStatus *	getButtons()		const	{ return m_buttons; }
		//ITF_INLINE const  f32 *							getAxes()			const	{ return m_axes; }
        //ITF_INLINE void  							    setPlayerIndex( u32 _idx )	{ m_playerIndex = _idx; }
        //void							                setAxesAndButtons( const f32 * _axes, const InputAdapter::PressStatus * _buttons );

	//private :
	//	u32							m_playerIndex;
	//	InputAdapter::PressStatus	m_buttons[JOY_MAX_BUT];
	//	f32							m_axes[JOY_MAX_AXES];
	};

	
	//-------------------------------------------------------------------------------------------------------------
	
	ITF_INLINE WithAnimStateMachine*	WithAnimState::getStateMachine()			const		{ return m_owner->DynamicCast<WithAnimStateMachine>(ITF_GET_STRINGID_CRC(WithAnimStateMachine,1042035092)); }
	
	ITF_INLINE IRTTIObject *			BasicStateImplement::getParent()			const		{ ITF_ASSERT( m_owner != NULL ); return ( m_owner ? m_owner->getParent() : NULL ); }
	ITF_INLINE BasicState *				BasicStateImplement::getCurState()			const		{ return getStateMachine()->getCurState(); }
	ITF_INLINE BasicState *				BasicStateImplement::getState ( u32 _stateId )const		{ return getStateMachine()->getState( _stateId ); }
	
	ITF_INLINE AnimatedComponent *		WithAnimStateImplement::getAnimComponent()	const		{ WithAnimStateMachine * animSM = m_owner->DynamicCast<WithAnimStateMachine>(ITF_GET_STRINGID_CRC(WithAnimStateMachine,1042035092)); return ( animSM ? animSM->getAnimComponent() : NULL ); }
	ITF_INLINE WithAnimStateMachine *	WithAnimStateImplement::getStateMachine()	const		{ return m_owner->DynamicCast<WithAnimStateMachine>(ITF_GET_STRINGID_CRC(WithAnimStateMachine,1042035092)); }
	ITF_INLINE WithAnimState*			WithAnimStateImplement::getCurState()		const		{ return getStateMachine()->getAnimCurState(); }

	ITF_INLINE PlayerStateMachine *		PlayerStateImplement::getStateMachine()		const		{ return m_owner->DynamicCast<PlayerStateMachine>(ITF_GET_STRINGID_CRC(PlayerStateMachine,1620634591)); }
	/*ITF_INLINE const  InputAdapter::PressStatus *	PlayerStateImplement::getButtons()const		{ return getStateMachine()->getButtons(); }
	ITF_INLINE const  f32 *							PlayerStateImplement::getAxes()		const	{ return getStateMachine()->getAxes(); }*/



//-------------------------------------------------------------------------------------------------------------
//												MACROS
//-------------------------------------------------------------------------------------------------------------
#define _STATE_U32_TO_STRINGID( _stateId ) StringID( #_stateId )

#define CREATE_STATE_IMPLEMENT( _implementClass ) BasicStateMachine::allocStateImplement< _implementClass >();

#define ADD_ANIM_STATE( _stateId, _implementState, _defaultAnim )	{ WithAnimState* newState = static_cast<WithAnimState*>( getStateMachine()->addState( _stateId, _STATE_U32_TO_STRINGID( _stateId ), _implementState ) ); \
																	newState->setDefaultAnimation( _defaultAnim ); }

#define ADD_STATE( _stateId, _implementState )				getStateMachine()->addState(	_stateId, _STATE_U32_TO_STRINGID( _stateId ), _implementState )

#define CHANGE_STATE( _stateId )							getStateMachine()->changeState( _stateId )
#define IS_CURSTATE( _stateId )								( ( getStateMachine()->getCurStateID() == _stateId ) ? btrue : bfalse )
#define IS_PREVSTATE( _stateId )							( ( getStateMachine()->getPrevStateID() == _stateId ) ? btrue : bfalse )

#define GET_STATE( _stateId )								getStateMachine()->getState( _stateId )

#define START_STATE_GROUP( _groupId )						getStateMachine()->startGroupDeclaration( _groupId );
#define STOP_STATE_GROUP()									getStateMachine()->stopGroupDeclaration();


//-------------------------------------------------------------------------------------------------------------
//											   TEMPLATES
//-------------------------------------------------------------------------------------------------------------
/*	class BasicState_Template
	{
		DECLARE_SERIALIZE();
	public:

		BasicState_Template();
		~BasicState_Template();
	private:
	};
	

	class BasicStateMachine_Template
	{
		DECLARE_SERIALIZE();

	public:
		BasicStateMachine_Template();
		~BasicStateMachine_Template(){}

		struct StateDescription
		{
			DECLARE_SERIALIZE();
			StringID		m_name;
			BasicState *	m_class;
		};
	
	private:

		SafeArray<StateDescription> m_stateDescriptionList;
	};*/
}

#endif //_ITF_BASICSTATEMACHINE_H_