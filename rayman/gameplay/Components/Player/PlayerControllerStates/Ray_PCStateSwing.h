#ifndef _ITF_RAY_PCSTATESWING_H_
#define _ITF_RAY_PCSTATESWING_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateSwing : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateSwing,Ray_State,2263442092)

public:

    StateSwing();
    virtual ~StateSwing() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    updateAnimInputs();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(SWING,3833044906);
    }

private:

    Vec2d                   updateSwinging( f32 _dt, const Vec2d& _swingPos, f32 _swingActorAngle );
    void                    updateHitting( const Vec2d& _swingPos );
    void                    updateOrientation( f32 _targetAngle );

    void                    startSwing( const Vec2d& _swingPos );
    f32                     calculateInitialAngularVelocity( const Vec2d& _swingStart ) const;
    f32                     calculateAngle( const Vec2d& _swingPos, const Vec2d& _pos ) const;
    void                    calculateHitPos( const Vec2d& _swingPos, Vec2d& _posA, Vec2d& _posB ) const;
    void                    processHitSwing( const Vec2d& _swingPos );

    f32                     getAcceleration( f32 _gravity, f32 _angle ) const;
    f32                     getExpectedSpeed( f32 _gravity, f32 _angle, f32 _maxAngle ) const;

    void                    setJumpForceDir();
    void                    updateCurrentQuadrant( f32 _angle );

    bbool                   checkCollision() const;

    Vec2d                   m_characterSpeed;
    f32                     m_angularVel;
    f32                     m_adjustSpeedTimer;
    f32                     m_interpolateTimer;
    f32                     m_angle;
    f32                     m_disableJumpCounter;
    u32                     m_currentSwingQuadrant;
    Vec2d                   m_prevPos;
    Vec2d                   m_prevHitPosA;
    Vec2d                   m_prevHitPosB;
    FixedArray <Vec2d,4>    m_shapeHitPos;
    bbool                   m_gotPrevHitPos;
    bbool                   m_justEntered;
    bbool                   m_canUnhang;
};

#endif //_ITF_RAY_PCSTATESWING_H_


