#ifndef _ITF_WAR_PCSTATEHANGINGTOSTANDING_H_
#define _ITF_WAR_PCSTATEHANGINGTOSTANDING_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

// This state is used when we are hanging on an edge, and suddenly we have to drop from it because the
// direction of the edge is not good anymore (moving platform). In this case we may drop the edge to stand
// on the ground perpendicular to the edge
class StateHangToStanding : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHangToStanding,War_State,4278749346)

public:

    StateHangToStanding() : m_canMove(bfalse) {}
    virtual ~StateHangToStanding() {}

    virtual void    onEnter();
    virtual void    onEvent(Event * _event);
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("HANGTOSTANDGROUND");

        return strAction;
    }

private:

    bbool           m_canMove;
};

#endif //_ITF_WAR_PCSTATEHANGINGTOSTANDING_H_


