#ifndef _ITF_RAY_PCSTATESWIMIDLE_H_
#define _ITF_RAY_PCSTATESWIMIDLE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateSwimIdle : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateSwimIdle,Ray_State,2840542492)

public:

    StateSwimIdle();
    virtual ~StateSwimIdle() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    onEvent(Event * _event);
    virtual void    updateAnimInputs();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(SWIM,1258662562);
    }

private:

    bbool           tryExitFromCurrent() const;
    bbool           shouldJumpOutOfWater() const;

    f32             m_prevUnstickMinFrictionMultiplier;
    bbool           m_canDive;
    bbool           m_justEntered;
};

#endif //_ITF_RAY_PCSTATESWIMIDLE_H_


