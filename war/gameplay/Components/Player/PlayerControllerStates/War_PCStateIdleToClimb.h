#ifndef _ITF_WAR_PCSTATEIDLETOCLIMB_H_
#define _ITF_WAR_PCSTATEIDLETOCLIMB_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateIdleToClimb : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateIdleToClimb,War_State,3207193240)

public:

    StateIdleToClimb() : m_blendCounter(0.f), m_startRot(0.f), m_canMove(bfalse) {}
    virtual ~StateIdleToClimb() {};

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void    onEvent(Event * _event);

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("IDLETOCLIMB");

        return strAction;
    }

private:

    f32             m_blendCounter;
    f32             m_startRot;
    bbool           m_canMove;
    
    static const f32 s_blendTime;
};

#endif //_ITF_WAR_PCSTATEIDLETOCLIMB_H_


