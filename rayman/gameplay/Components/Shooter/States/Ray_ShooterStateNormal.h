#ifndef _ITF_RAY_SHOOTERSTATENORMAL_H_
#define _ITF_RAY_SHOOTERSTATENORMAL_H_

#ifndef _ITF_RAY_SHOOTERSTATE_H_
#include "rayman/gameplay/Components/Shooter/States/Ray_ShooterState.h"
#endif //_ITF_RAY_SHOOTERSTATE_H_

//#ifndef _ITF_BASICSTATEMACHINE_H_
//#include "rayman\gameplay\Components\Shooter\BasicStateMachine.h"
//#endif //_ITF_BASICSTATEMACHINE_H_

class StateNormal : public Ray_ShooterStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI(StateNormal, Ray_ShooterStateImplement,3249156578)

public:
	StateNormal() :Super(){}
	virtual ~StateNormal(){}

	virtual void onEnter();
	virtual void onExit();
	virtual void update( f32 _dt );

	virtual u32		onEvaluateNextStateConditions( u32 _wantedStateID );
	//virtual u32		onValidateStateChange( u32 _prevStateID, u32 _wantedStateID );
	virtual void	changeStateCheck( f32 _deltaTime );
	virtual void	changeStateCheckAnimFinished();

};


#endif //_ITF_RAY_SHOOTERSTATENORMAL_H_

