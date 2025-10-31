#ifndef _ITF_RAY_PCSTATEHELICOPTER_H_
#define _ITF_RAY_PCSTATEHELICOPTER_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateHelicopter: public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHelicopter,Ray_State,2558717036)

public:
    StateHelicopter() : m_exitCounter(0.f), m_justEntered(bfalse), m_exit(bfalse) {}
    virtual ~StateHelicopter() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(HELICO,292076939);
    }

private:

    f32             m_exitCounter;
    bbool           m_justEntered;
    bbool           m_exit;
};

#endif //_ITF_RAY_PCSTATEHELICOPTER_H_


