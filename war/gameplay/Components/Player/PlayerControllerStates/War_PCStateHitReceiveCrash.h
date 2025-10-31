#ifndef _ITF_WAR_PCSTATEHITRECEIVECRASH_H_
#define _ITF_WAR_PCSTATEHITRECEIVECRASH_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHitReceiveCrash : public War_State
{

    DECLARE_OBJECT_CHILD_RTTI(StateHitReceiveCrash,War_State,2079623441)

public:

    StateHitReceiveCrash() : m_crashPoly(ITF_INVALID_OBJREF), m_crashDist(0.f), m_crashTargetAngle(0.f),
        m_crashEdge(U32_INVALID) {}
    virtual ~StateHitReceiveCrash() {}

    void            setCrashData( ObjectRef _crashPoly, u32 _crashEdge, f32 _crashDist, f32 _crashTargetAngle );

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("RECEIVEHITCRASH");

        return strAction;
    }

private:

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);

    ObjectRef       m_crashPoly;
    f32             m_crashDist;
    f32             m_crashTargetAngle;
    u32             m_crashEdge;
};

#endif //_ITF_WAR_PCSTATEHITRECEIVECRASH_H_


