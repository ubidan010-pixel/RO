

#ifndef _ITF_WAR_PCSTATEHANGINGTOJUMP_H_
#define _ITF_WAR_PCSTATEHANGINGTOJUMP_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHangToJump : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHangToJump,War_State,4200066351)

public:

    StateHangToJump() : m_timeCounter(0.f), m_startAngle(0.f), m_targetAngle(0.f), m_angleDif(0.f) {}
    virtual ~StateHangToJump() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("HANGTOJUMP");

        return strAction;
    }

private:

    f32             m_timeCounter;
    f32             m_startAngle;
    f32             m_targetAngle;
    f32             m_angleDif;

    static const f32 s_blendTime;
};

#endif //_ITF_WAR_PCSTATEHANGINGTOJUMP_H_


