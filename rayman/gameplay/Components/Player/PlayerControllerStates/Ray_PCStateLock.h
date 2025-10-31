#ifndef _ITF_RAY_PCSTATELOCK_H_
#define _ITF_RAY_PCSTATELOCK_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateLock : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateLock,Ray_State,1305888277)

public:
    StateLock();
    ~StateLock() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual void    onEvent(Event * _event);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(LOCK,3199059628);
    }

    void setData( bbool _lookRight );

private:

    PlayerControllerState* m_prevState;
    bbool m_clearLockOnExit;
    bbool m_exit;
};

#endif //_ITF_RAY_PCSTATELOCK_H_

