#ifndef _ITF_WAR_PCSTATEHANGING_H_
#define _ITF_WAR_PCSTATEHANGING_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHanging : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHanging,War_State,4261805009)

public:

    StateHanging() : m_climbTimer(0.f), m_canMoveAgain(bfalse), m_justEntered(bfalse) {}
    virtual ~StateHanging() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void    onEvent(Event * _event);
    virtual void    updateAnimInputs();

    void            setHangInfo(HangingInfo & _info) { m_hangInfo = _info;}

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("HANG");

        return strAction;
    }

private:

    bbool           tryWallSlide();

    HangingInfo m_hangInfo;
    f32 m_climbTimer;
    bbool m_canMoveAgain;
    bbool m_justEntered;
};

#endif //_ITF_WAR_PCSTATEHANGING_H_


