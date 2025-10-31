#ifndef _ITF_RAY_PCSTATEHITRECEIVE_H_
#define _ITF_RAY_PCSTATEHITRECEIVE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateHitReceive : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHitReceive,Ray_State,3755744405)

public:

    StateHitReceive();
    virtual ~StateHitReceive() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update( f32 _dt );
    virtual void    updateAnimInputs();
    virtual void    onEvent(Event * _event);

    void            restart();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(RECEIVEHIT,848873176);
    }

private:

    void            setupHit( bbool _restart );

    void            setupPunchFront( bbool _restart );
    void            setupPunchUp( bbool _restart );
    void            setupHurtBounce( bbool _restart );
    void            setupEarthquake( bbool _restart );

    void            updatePunchFront( f32 _dt );
    void            updatePunchUp( f32 _dt );
    void            updateHurtBounce( f32 _dt );
    void            updateEarthquake( f32 _dt );

    void            updateSwimStance( f32 _dt );

    bbool           checkFinishedPunchFront();
    bbool           checkFinishedPunchUp();
    bbool           checkFinishedHurtBounce();
    bbool           checkFinishedEarthquake();

    void            setCrashAnim( const PolyLine* _poly, const class PolyLineEdge* _edge, u32 _edgeIndex );
    bbool           checkCrashedOnWall();

    bbool           exitFromAction();
    void            exitState();

    ObjectRef       m_blockedPolyline;
    u32             m_blockedEdgeIndex;
    Vec2d           m_currentSpeed;

    bbool           m_justEntered;
    bbool           m_canMove;
    bbool           m_triggerDeath;
    bbool           m_wasBlocked;
    bbool           m_appexStart;
    bbool           m_appexStop;
    bbool           m_appex;
};

#endif //_ITF_RAY_PCSTATEHITRECEIVE_H_


