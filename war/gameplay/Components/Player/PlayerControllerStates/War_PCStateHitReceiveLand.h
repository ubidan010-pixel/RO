#ifndef _ITF_WAR_PCSTATEHITRECEIVELAND_H_
#define _ITF_WAR_PCSTATEHITRECEIVELAND_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHitReceiveLand : public War_State
{

    DECLARE_OBJECT_CHILD_RTTI(StateHitReceiveLand,War_State,818901218)

public:

    StateHitReceiveLand() : m_prevFrictionMultiplier(1.f) {}
    virtual ~StateHitReceiveLand() {}

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("RECEIVEHITLAND");

        return strAction;
    }

private:

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );

    f32             m_prevFrictionMultiplier;
};

#endif //_ITF_WAR_PCSTATEHITRECEIVELAND_H_


