#ifndef _ITF_RAY_PCSTATEHANGING_H_
#define _ITF_RAY_PCSTATEHANGING_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateHanging : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHanging,Ray_State,4261805009)

public:

    StateHanging();
    virtual ~StateHanging() {};

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    onEvent(Event * _event);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(HANG,2269038713);
    }

private:

    bbool           tryWallSlide();
    void            updateInput( f32 _dt );

    f32 m_blendCounter;
    f32 m_startRot;
    bbool m_canMoveAgain;
    bbool m_justEntered;
    bbool m_climbUp;
    bbool m_allowDrop;
    bbool m_coopJump;

    static const f32 s_blendTime;
};

#endif //_ITF_RAY_PCSTATEHANGING_H_


