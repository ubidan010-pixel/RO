#ifndef _ITF_RAY_PCSTATEFALLING_H_
#define _ITF_RAY_PCSTATEFALLING_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateFalling: public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateFalling,Ray_State,4232906610)

public:

    StateFalling();
    virtual ~StateFalling() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    updateAnimInputs();

    void            setSkitResetUTurn() { m_skipResetUTurn = btrue; }
    void            setJumpReleased( bbool _val ) { m_jumpReleased = _val; }

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(FALL,100613701);
    }

private:

    void                        updateSurfing();

    AirPose                     m_airPose;                     // In what pose we are while falling
    f32                         m_fallAngleCursor;
    f32                         m_initialDif;
    f32                         m_fallCounter;
    bbool                       m_keepMiddlePose;
    bbool                       m_justEntered;
    bbool                       m_skipResetUTurn;
    bbool                       m_jumpReleased;
};

#endif //_ITF_RAY_PCSTATEFALLING_H_


