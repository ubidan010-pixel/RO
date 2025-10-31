#ifndef _ITF_RAY_PCSTATEWALLSLIDE_H_
#define _ITF_RAY_PCSTATEWALLSLIDE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateWallSlide : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateWallSlide,Ray_State,1378969567)

public:

    StateWallSlide();
    virtual ~StateWallSlide() {};

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(WALLSLIDE,185571511);
    }

    void            forceWallSlideEdge( ObjectRef _poly, u32 _edgeIndex ) { m_polyLine = _poly; m_edge = _edgeIndex; }

private:

    Vec2d           calculateSpeed( f32 _dt, const class PolyLineEdge* _edge );
    f32             calculateRotation( f32 _dt, const class PolyLineEdge* _edge );

    f32             getAngularSpeed() const;

    ObjectRef       m_polyLine;
    u32             m_edge;

    f32             m_targetAngle;
    f32             m_startSpeed;
    f32             m_brakeTimer;

    f32             m_unstickFromPushCounter;
    f32             m_wallSlideDelay;

    bbool           m_justEntered;
    bbool           m_keepImpulse;
    bbool           m_fromHang;
    bbool           m_delayApplied;
};

#endif //_ITF_RAY_PCSTATEWALLSLIDE_H_


