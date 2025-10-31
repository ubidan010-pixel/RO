#ifndef _ITF_WAR_PCSTATECLIMBINGTOIDLE_H_
#define _ITF_WAR_PCSTATECLIMBINGTOIDLE_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateClimbingToIdle : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateClimbingToIdle,War_State,4189549355)

public:

    StateClimbingToIdle();
    virtual ~StateClimbingToIdle() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual void    onEvent(Event * _event);
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("CLIMBTOIDLE");

        return strAction;
    }

private:

    void            finishClimb();
    void            clearClimbing();

    f32             m_blendCounter;
    f32             m_startRot;

    bbool           m_canMove;
    bbool           m_moving;

    static const f32 s_blendTime;
};

#endif //_ITF_WAR_PCSTATECLIMBINGTOIDLE_H_


