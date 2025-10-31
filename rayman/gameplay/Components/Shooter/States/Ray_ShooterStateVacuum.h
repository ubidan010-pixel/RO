#ifndef _ITF_RAY_SHOOTERSTATEVACUUM_H_
#define _ITF_RAY_SHOOTERSTATEVACUUM_H_

#ifndef _ITF_RAY_SHOOTERSTATE_H_
#include "rayman/gameplay/Components/Shooter/States/Ray_ShooterState.h"
#endif //_ITF_RAY_SHOOTERSTATE_H_

class StateVacuum : public Ray_ShooterStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI(StateSpit, Ray_ShooterStateImplement,2292276140)

public:
	StateVacuum() :Super(), m_vacuumTimer( 0.0f ) , m_startAnimTimer( 0.0f ), m_transitionTimer( 0.0f ), m_stopRequested( bfalse ), m_redTimer(0.0f) {}
	virtual ~StateVacuum(){}

	virtual void onEnter();
	virtual void onExit();
	virtual void update( f32 _dt );

    virtual u32		onEvaluateNextStateConditions( u32 _wantedStateID );
	virtual u32		onValidateStateChange( u32 _prevStateID, u32 _wantedStateID );
	virtual void	changeStateCheck( f32 _deltaTime );//, const InputAdapter::PressStatus* _buttons, f32* _axes );
	virtual void	changeStateCheckAnimFinished();

	virtual void	updateAnimation( f32 _dt );
	virtual void	chooseAnimation();

    f32 getAnimDuration( u32 _stateID );

private :
    f32 m_transitionTimer;
	f32 m_vacuumTimer;
	f32 m_startAnimTimer;
    f32 m_redTimer;
    bbool m_stopRequested;


};




#endif //_ITF_RAY_SHOOTERSTATEVACUUM_H_

