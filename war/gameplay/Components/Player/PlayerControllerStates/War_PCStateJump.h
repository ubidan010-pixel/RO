#ifndef _ITF_WAR_PCSTATEJUMP_H_
#define _ITF_WAR_PCSTATEJUMP_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateJump : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateJump,War_State,4001937309)
public:

    StateJump() : m_verticalSpeed(0.f), m_justJumped(btrue), m_setVerticalSpeed(bfalse) {}
    ~StateJump() {}

    virtual void                onEnter();
    virtual void                onExit();
    virtual bbool               checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void                update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void                updateAnimInputs();

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("JUMP");

        return strAction;
    }

private:

    void                        setupJump();

    f32                         m_verticalSpeed;                // The vertical speed at the start of the jump
    bbool                       m_justJumped;                   // True if we just entered the state
    bbool                       m_setVerticalSpeed;             // True if we need to set the vertical speed
};

#endif //_ITF_WAR_PCSTATEJUMP_H_


