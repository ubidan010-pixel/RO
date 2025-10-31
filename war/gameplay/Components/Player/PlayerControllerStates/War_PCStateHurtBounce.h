#ifndef _ITF_WAR_PCSTATEHURTBOUNCE_H_
#define _ITF_WAR_PCSTATEHURTBOUNCE_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHurtBounce  : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHurtBounce,War_State,3896982778)

public:

    StateHurtBounce() {}
    virtual ~StateHurtBounce() {};

    virtual void    onEnter();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("HURTBOUNCE");

        return strAction;
    }

private:

};

#endif //_ITF_WAR_PCSTATEHURTBOUNCE_H_


