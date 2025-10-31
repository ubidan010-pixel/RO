#ifndef _ITF_RAY_PCSTATEHANGINGTOSTANDING_H_
#define _ITF_RAY_PCSTATEHANGINGTOSTANDING_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

// This state is used when we are hanging on an edge, and suddenly we have to drop from it because the
// direction of the edge is not good anymore (moving platform). In this case we may drop the edge to stand
// on the ground perpendicular to the edge
class StateHangToStanding : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHangToStanding,Ray_State,4278749346)

public:

    StateHangToStanding() : m_canMove(bfalse) {}
    virtual ~StateHangToStanding() {}

    virtual void    onEnter();
    virtual void    onEvent(Event * _event);
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(HANGTOSTANDGROUND,3395375245);
    }

private:

    bbool           m_canMove;
};

#endif //_ITF_RAY_PCSTATEHANGINGTOSTANDING_H_


