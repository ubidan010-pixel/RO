

#ifndef _ITF_RAY_PCSTATESTARTLEVEL_H_
#define _ITF_RAY_PCSTATESTARTLEVEL_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateStartLevel : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateStartLevel,Ray_State,1497808111)

public:

    StateStartLevel() : m_finished(bfalse), m_allowMove(bfalse) {}
    virtual ~StateStartLevel() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual void    onEvent(Event * _event);
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(STARTLEVEL,4177648987);
    }

private:

    bbool           m_finished;
    bbool           m_allowMove;
};

#endif //_ITF_RAY_PCSTATESTARTLEVEL_H_


