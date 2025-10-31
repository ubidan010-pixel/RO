#ifndef _ITF_RAY_PCSTATECLIMBING_H_
#define _ITF_RAY_PCSTATECLIMBING_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateClimbing : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateClimbing, Ray_State,2859903667)
public:

    StateClimbing();
    virtual ~StateClimbing() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update( f32 _dt );
    virtual void    updateAnimInputs();
    virtual void    onEvent(Event * _event);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(CLIMB,3278228674);
    }

private:

    void            sendMoveImpulseEvent();
    void            exitToFall();

    bbool           m_allowMove;            // If the marker allow move was reached
};

#endif //_ITF_RAY_PCSTATECLIMBING_H_


