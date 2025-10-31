#ifndef _ITF_RAY_PCSTATEFLY_H_
#define _ITF_RAY_PCSTATEFLY_H_

#ifdef ITF_SUPPORT_CHEAT

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateFlyMode  : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateFlyMode,Ray_State,2345466242)

public:

    StateFlyMode() {}
    virtual ~StateFlyMode() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(FALL,100613701);
    }

private:

};

#endif

#endif //_ITF_RAY_PCSTATEFLY_H_


