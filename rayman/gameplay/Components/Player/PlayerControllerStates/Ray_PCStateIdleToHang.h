#ifndef _ITF_RAY_PCSTATEIDLETOHANG_H_
#define _ITF_RAY_PCSTATEIDLETOHANG_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateIdleToHang : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateIdleToHang,Ray_State,3098239164)

public:

    StateIdleToHang() : m_blendCounter(0.f), m_startRot(0.f), m_canMove(bfalse) {}
    virtual ~StateIdleToHang() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    onEvent(Event * _event);
    virtual void    updateAnimInputs();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(IDLETOHANG,725106680);
    }

private:

    f32                 m_blendCounter;
    f32                 m_startRot;
    bbool               m_canMove;

    static const f32 s_blendTime;
};

#endif //_ITF_RAY_PCSTATEIDLETOHANG_H_


