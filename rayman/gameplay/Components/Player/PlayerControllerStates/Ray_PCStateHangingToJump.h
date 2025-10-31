

#ifndef _ITF_RAY_PCSTATEHANGINGTOJUMP_H_
#define _ITF_RAY_PCSTATEHANGINGTOJUMP_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateHangToJump : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHangToJump,Ray_State,4200066351)

public:

    StateHangToJump() : m_timeCounter(0.f) {}
    virtual ~StateHangToJump() {}

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    updateAnimInputs();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(HANGTOJUMP,668554470);
    }

private:

    f32             m_timeCounter;

    static const f32 s_blendTime;
};

#endif //_ITF_RAY_PCSTATEHANGINGTOJUMP_H_


