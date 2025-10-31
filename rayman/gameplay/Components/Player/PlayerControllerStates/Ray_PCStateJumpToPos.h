#ifndef _ITF_RAY_PCSTATEJUMPTOPOS_H_
#define _ITF_RAY_PCSTATEJUMPTOPOS_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateJumpToPos : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateJumpToPos,Ray_State,1767736151)

public:

    StateJumpToPos();
    virtual ~StateJumpToPos() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    void            setTargetPos( const Vec2d& _pos );
    void            setTargetObj( ObjectRef _ref );
    void            setTargetPoly( ObjectRef _ref, u32 _edgeIndex, f32 _edgeT );

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(FALL,100613701);
    }

private:

    void            updateX( f32 _dt, f32& _x );
    void            updateY( f32 _dt, f32 _currentX, f32& _y );

    void            resetTarget();
    Vec2d           getTargetPos() const;

    void            exitState();

    Vec2d           m_posThisFrame;

    Vec2d           m_targetPos;
    ObjectRef       m_refObj;
    ObjectRef       m_refPoly;
    u32             m_edgeIndex;
    f32             m_edgeT;
};

#endif //_ITF_RAY_PCSTATEJUMPTOPOS_H_


