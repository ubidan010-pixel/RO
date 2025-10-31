#ifndef _ITF_RAY_PCSTATESTARGATE_H_
#define _ITF_RAY_PCSTATESTARGATE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateStargate : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateStargate, Ray_State,3086676344)

public:

    StateStargate();
    virtual ~StateStargate();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(STARGATE,4212359101);
    }

    virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update( f32 _dt );
    virtual void    onEvent( Event* _event );
    virtual void    updateAnimInputs();

private:

    void            setNextStage();

    void            updateSuckedIn( f32 _dt );
    void            updateWait( f32 _dt );
    void            updateTravel( f32 _dt );

    void            startTrail();
    void            stopTrail();
    void            updateTrail( f32 _dt );

    void            processStargateUse( class Ray_EventStargateUse* _stargateUseEvent );

    SpawneeGenerator    m_trailGenerator;
    ActorRef            m_trailRef;

    EStargateStage  m_currentStage;
    EStargateStage  m_nextStage;
    Spline		    m_spline;
    PerlinNoise     m_noiseGenX;
    PerlinNoise     m_noiseGenY;
    f32             m_currentTimer;
    f32             m_totalTime;
    Vec3d           m_suckedInPoint;
    Vec3d           m_stargateStart;
	Vec3d			m_speed;
    Vec2d           m_waitPos;
	f32				m_rotateStart;
	f32 			m_rotateTarget;
	f32				m_rotateCounter;
	f32				m_rotateTime;
    f32             m_decelerateTime;
    f32             m_decelerateCounter;
    bbool           m_justEntered;
    bbool           m_lastTrip;
};

#endif //_ITF_RAY_PCSTATESTARGATE_H_