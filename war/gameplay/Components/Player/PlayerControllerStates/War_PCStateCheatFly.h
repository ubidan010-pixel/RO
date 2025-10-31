#ifndef _ITF_WAR_PCSTATECHEATFLY_H_
#define _ITF_WAR_PCSTATECHEATFLY_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateCheatFlyMode  : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateCheatFlyMode,War_State,2457489997)

public:

    StateCheatFlyMode() {}
    virtual ~StateCheatFlyMode() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update( f32 _dt, const InputAdapter::PressStatus* _buttons, f32 * _axes );

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("FALL");
        return strAction;
    }

private:

};

#endif //_ITF_WAR_PCSTATECHEATFLY_H_


