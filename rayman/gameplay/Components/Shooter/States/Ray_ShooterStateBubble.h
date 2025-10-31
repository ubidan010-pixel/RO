#ifndef _ITF_RAY_SHOOTERSTATEBUBBLE_H_
#define _ITF_RAY_SHOOTERSTATEBUBBLE_H_

#ifndef _ITF_RAY_SHOOTERSTATE_H_
#include "rayman/gameplay/Components/Shooter/States/Ray_ShooterState.h"
#endif //_ITF_RAY_SHOOTERSTATE_H_

class StateBubble : public Ray_ShooterStateImplement
{
	DECLARE_OBJECT_CHILD_RTTI(StateBubble, Ray_ShooterStateImplement,98144012)

public:
    StateBubble() :Super(), m_bezierTimer( 0.0f ), m_angle( 0.0f ), m_previousFaction(RAY_FACTION_FRIENDLY), m_previousWindMultiplier( 1.0f ) {}
	virtual ~StateBubble(){}

	virtual void onEnter();
	virtual void onExit();
	virtual void update( f32 _dt );

    virtual u32     onValidateStateChange( u32 _prevStateID, u32 _wantedStateID );
	virtual void	changeStateCheck( f32 _deltaTime );
    virtual void	changeStateCheckAnimFinished();
	virtual void    onEvent( Event * _evt );

    void        updateBubble(f32 _dt );

protected:
    
    void        updateBubbleExit(f32 _dt );

    Vec2d   m_moveVector;
    f32     m_angle;

    Faction m_previousFaction;
    f32     m_previousWindMultiplier;

    Vec2d   m_bezierPos0;
    Vec2d   m_bezierPos1;
    Vec2d   m_bezierPos2;
    Vec2d   m_bezierPos3;
    f32     m_bezierTimer;

    PhysShapeCircle m_shape;

};


#endif //_ITF_RAY_SHOOTERSTATEBUBBLE_H_

