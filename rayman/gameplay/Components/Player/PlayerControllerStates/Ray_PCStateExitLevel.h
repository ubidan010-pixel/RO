

#ifndef _ITF_RAY_PCSTATEEXITLEVEL_H_
#define _ITF_RAY_PCSTATEEXITLEVEL_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateExitLevel : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateExitLevel,Ray_State,4225276183)

public:

    StateExitLevel() : m_finished(bfalse) {}
    virtual ~StateExitLevel() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual void    onEvent(Event * _event);
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(EXITLEVEL,1655151481);
    }

private:

    bbool           m_finished;
};

#endif //_ITF_RAY_PCSTATEEXITLEVEL_H_


