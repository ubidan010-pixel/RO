#ifndef _ITF_RAY_SHOOTERSTATESEQUENCES_H_
#define _ITF_RAY_SHOOTERSTATESEQUENCES_H_

#ifndef _ITF_RAY_SHOOTERSTATE_H_
#include "rayman/gameplay/Components/Shooter/States/Ray_ShooterState.h"
#endif //_ITF_RAY_SHOOTERSTATE_H_

class StateSequences : public Ray_ShooterStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI(StateSequences, Ray_ShooterStateImplement,3915656743)

public:
	StateSequences();
	virtual ~StateSequences(){}

	virtual void onEnter();
	virtual void onExit();
	virtual void update( f32 _dt );

	virtual void    changeStateCheck( f32 _deltaTime );
    virtual void    onEvent( Event * _evt );
    virtual void	chooseAnimation();

private :

    void    computeTravel( const Vec2d & _destPos );

    Vec2d   m_travelPos0;
    Vec2d   m_travelPos1;
    Vec2d   m_travelPos2;
    Vec2d   m_travelPos3;
    
    f32     m_travelTimer;
    f32     m_travelDuration;
    bbool   m_travelInitialized;

    //ShooterCameraModifier_Transition                        m_changeDepthTransition;
    //f32                                                     m_changeDepthDest;
    //f32                                                     m_changeDepthStartValue;

};


#endif //_ITF_RAY_SHOOTERSTATESEQUENCES_H_

