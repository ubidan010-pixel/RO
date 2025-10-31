#ifndef _ITF_RAY_PCSTATECRUSHEDBYCHARACTER_H_
#define _ITF_RAY_PCSTATECRUSHEDBYCHARACTER_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateCrushedByCharacter : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateCrushedByCharacter,Ray_State,1381896961)

public:

    StateCrushedByCharacter() : m_canMove(bfalse) {}
    virtual ~StateCrushedByCharacter() {};

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    onEvent(Event * _event);
    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(CRUSHEDBYCHARACTER,17799211);
    }

    void            restart();

private:

    bbool           m_canMove;

};

#endif //_ITF_RAY_PCSTATECRUSHEDBYCHARACTER_H_


