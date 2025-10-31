#ifndef _ITF_RAY_PCSTATECOOPHANGCLIMB_H_
#define _ITF_RAY_PCSTATECOOPHANGCLIMB_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateCoopHangClimb : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateCoopHangClimb,Ray_State,1473471928)

public:

    StateCoopHangClimb() : m_blendCounter(0.f), m_startRot(0.f) {}
    virtual ~StateCoopHangClimb() {};

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(COOPHANGCLIMB,2857071988);
    }

private:

    f32             m_blendCounter;
    f32             m_startRot;
    
    static const f32 s_blendTime;
};

#endif //_ITF_RAY_PCSTATECOOPHANGCLIMB_H_


