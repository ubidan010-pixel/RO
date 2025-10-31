#ifndef _ITF_RAY_PCSTATEPLAYANIM_H_
#define _ITF_RAY_PCSTATEPLAYANIM_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StatePlayAnim : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StatePlayAnim,Ray_State,2381756246)

public:
    StatePlayAnim();
    ~StatePlayAnim() {}

    virtual void     onExit();
    virtual bbool    checkStateChange( f32 _dt );

    ITF_INLINE virtual StringID getAnimAction() const
    {
        ITF_ASSERT(m_playAnim.isValid());
        return m_playAnim;
    }

    void            setAnim( const StringID& _anim ) { m_playAnim = _anim; }

private:

    StringID                m_playAnim;
};

#endif //_ITF_RAY_PCSTATEPLAYANIM_H_

