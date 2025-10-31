#ifndef _ITF_RAY_PCSTATEHANGINGTOIDLE_H_
#define _ITF_RAY_PCSTATEHANGINGTOIDLE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateHangToIdle : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHangToIdle,Ray_State,1888865426)

public:

    StateHangToIdle() : m_timeCounter(0.f), m_startRot(0.f), m_canMove(bfalse), m_moving(bfalse) {}
    virtual ~StateHangToIdle() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual void    onEvent(Event * _event);
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(HANGTOIDLE,1219294316);
    }

private:

    void            finishHang();
    void            clearHanging();

    f32             m_timeCounter;
    f32             m_startRot;
    bbool           m_canMove;
    bbool           m_moving;

    static const f32 s_blendTime;
};

#endif //_ITF_RAY_PCSTATEHANGINGTOIDLE_H_


