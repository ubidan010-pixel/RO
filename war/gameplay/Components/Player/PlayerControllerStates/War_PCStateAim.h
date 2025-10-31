#ifndef _ITF_WAR_PCSTATEAIM_H_
#define _ITF_WAR_PCSTATEAIM_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateAim : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateAim,War_State,865372951)

public:

    StateAim();
    virtual ~StateAim() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void    updateAnimInputs();
    virtual void    onBecomeActive();
    virtual void    onBecomeInactive();

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("AIM");
        return strAction;
    }

private:

    void            updateAimAngle( f32 _dt );

    f32     m_aimAngle;
    f32     m_targetAimAngle;
    u32     m_fxHandle;
};

#endif //_ITF_WAR_PCSTATEAIM_H_


