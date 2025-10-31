#ifndef _ITF_RAY_SHOOTERSTATEHIT_H_
#define _ITF_RAY_SHOOTERSTATEHIT_H_

#ifndef _ITF_RAY_SHOOTERSTATE_H_
#include "rayman/gameplay/Components/Shooter/States/Ray_ShooterState.h"
#endif //_ITF_RAY_SHOOTERSTATE_H_

class StateHit : public Ray_ShooterStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI(StateHit, Ray_ShooterStateImplement,619179744)

public:
	StateHit() :Super(), m_isdead(bfalse), m_disableAtEnd(bfalse), m_restartTriggered( bfalse ), m_spawnDeathFX( bfalse ) {}
	virtual ~StateHit();

	virtual void onEnter();
	virtual void onExit();
	virtual void update( f32 _dt );
    
    virtual u32     onValidateStateChange( u32 _prevStateID, u32 _wantedStateID );
	virtual void	changeStateCheck( f32 _deltaTime );
    virtual void	changeStateCheckAnimFinished();
	virtual void    onEvent( Event * _evt );

    void destroyFX();

private :
    Vec2d   m_InitScale;
    bbool   m_isdead; 
    bbool   m_disableAtEnd;
    bbool   m_restartTriggered;
    bbool   m_spawnDeathFX;

    ObjectRef m_bounceToTarget;
    SafeArray<ActorRef> m_deathFXActors;

};


#endif //_ITF_RAY_SHOOTERSTATEHIT_H_

