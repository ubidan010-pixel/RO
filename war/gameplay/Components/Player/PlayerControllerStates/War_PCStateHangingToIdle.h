#ifndef _ITF_WAR_PCSTATEHANGINGTOIDLE_H_
#define _ITF_WAR_PCSTATEHANGINGTOIDLE_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHangToIdle : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHangToIdle,War_State,1888865426)

public:

    StateHangToIdle() : m_timeCounter(0.f), m_canMove(bfalse), m_moving(bfalse) {}
    virtual ~StateHangToIdle() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual void    onEvent(Event * _event);
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("HANGTOIDLE");

        return strAction;
    }

private:

    void            finishHang();
    void            clearHanging();

    f32             m_timeCounter;
    bbool           m_canMove;
    bbool           m_moving;

    static const f32 s_blendTime;
};

#endif //_ITF_WAR_PCSTATEHANGINGTOIDLE_H_


