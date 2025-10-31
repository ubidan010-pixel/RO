#ifndef _ITF_RAY_SHOOTERSTATESPIT_H_
#define _ITF_RAY_SHOOTERSTATESPIT_H_

#ifndef _ITF_RAY_SHOOTERSTATE_H_
#include "rayman/gameplay/Components/Shooter/States/Ray_ShooterState.h"
#endif //_ITF_RAY_SHOOTERSTATE_H_

class StateSpit : public Ray_ShooterStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI(StateSpit, Ray_ShooterStateImplement,2292276140)

public:
	StateSpit() :Super(), m_spitRequest(bfalse){}
	virtual ~StateSpit(){}

	virtual void onEnter();
	virtual void onExit();
	virtual void update( f32 _dt );

	virtual void changeStateCheck( f32 _deltaTime );
	virtual void changeStateCheckAnimFinished();
    virtual u32  onValidateStateChange( u32 _prevStateID, u32 _wantedStateID );

protected:
	bbool	m_spitRequest;

};


class StateAttack : public Ray_ShooterStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI(StateAttack, Ray_ShooterStateImplement,100754618)

public:
	StateAttack() :Super(), m_queueAttack( bfalse ) {}
	virtual ~StateAttack(){}

	virtual void onEnter();
	virtual void onExit();
	virtual void update( f32 _dt );

	virtual void changeStateCheck( f32 _deltaTime );
	virtual void changeStateCheckAnimFinished();

private :
	bbool m_queueAttack;

};

#endif //_ITF_RAY_SHOOTERSTATESPIT_H_

