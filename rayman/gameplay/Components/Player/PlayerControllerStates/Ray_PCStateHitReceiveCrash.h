#ifndef _ITF_RAY_PCSTATEHITRECEIVECRASH_H_
#define _ITF_RAY_PCSTATEHITRECEIVECRASH_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateHitReceiveCrash : public Ray_State
{

    DECLARE_OBJECT_CHILD_RTTI(StateHitReceiveCrash,Ray_State,2079623441)

public:

    StateHitReceiveCrash();
    virtual ~StateHitReceiveCrash() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    onEvent(Event * _event);

    void            setCrashData( ObjectRef _crashPoly, u32 _crashEdge );

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(RECEIVEHITCRASH,3219639338);
    }

private:

    void            updateStickEdge( const Vec2d& _newPos );
    void            unstick();
    bbool           isSticked() const { return m_crashPoly != ITF_INVALID_OBJREF; }

    ObjectRef       m_crashPoly;
    Vec2d           m_prevPos;
    f32             m_crashDist;
    f32             m_crashTargetAngle;
    u32             m_crashEdge;
    bbool           m_justEntered;
    bbool           m_triggerDeath;
};

#endif //_ITF_RAY_PCSTATEHITRECEIVECRASH_H_


