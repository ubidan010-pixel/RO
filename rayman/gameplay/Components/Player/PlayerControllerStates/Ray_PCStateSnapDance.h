#ifndef _ITF_RAY_PCSTATESNAPDANCE_H_
#define _ITF_RAY_PCSTATESNAPDANCE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateSnapDance : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateSnapDance,Ray_State,993666460)

public:
    StateSnapDance();
    ~StateSnapDance() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    onEvent(Event * _event);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(IDLE,1633200266);
    }

private:

    void            updateAnimInputs();

    bbool           m_exit;

};

#endif //_ITF_RAY_PCSTATESNAPDANCE_H_

