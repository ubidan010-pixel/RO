#ifndef _ITF_WAR_PCSTATEFALLING_H_
#define _ITF_WAR_PCSTATEFALLING_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateFalling: public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateFalling,War_State,4232906610)

public:

    StateFalling()
        : m_justEntered(bfalse)
        , m_animCursor(0)
    {}
    virtual ~StateFalling() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void    updateAnimInputs();

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("FALL");

        return strAction;
    }

private:

    bbool           m_justEntered;
    f32             m_animCursor;
};

#endif //_ITF_WAR_PCSTATEFALLING_H_


