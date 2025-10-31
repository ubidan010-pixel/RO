#ifndef _ITF_WAR_PCSTATECLIMBING_H_
#define _ITF_WAR_PCSTATECLIMBING_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateClimbing : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateClimbing, War_State,2859903667)
public:

    StateClimbing();
    virtual ~StateClimbing() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void    updateAnimInputs();
    virtual void    onEvent(Event * _event);

    void            setHangInfo(HangingInfo & _info) { m_hangInfo = _info;}
    void            setMoveInertia( f32 _speed ) { m_moveInertia = _speed; }
    void            setAllowExitHorizontal( bbool _val ) { m_allowExitHorizontal = _val; }
    bbool           getAllowExitHorizontal() const { return m_allowExitHorizontal; }

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("CLIMB");

        return strAction;
    }

private:

    void            updateMove( f32 _dt, const class PolyLine* _polyLine, const class PolyLineEdge* _edge, Vec2d& _moveInput );
    void            updateOrientation( f32 _dt );
    void            updateSwing( f32 _dt, const class PolyLine* _polyLine, Vec2d& _moveInput );
    void            updateLookDir();
    void            sendMoveImpulseEvent();

    // move
    void            getMoveData( const class PolyLineEdge* _edge,
                                 f32& _moveSign, EMoveDir& _climbDir ) const;
    f32             getMoveSize( f32 _deltaTime, f32 _moveDir, EMoveDir _climbDir ) const;
    void            updateInertia( f32 _dt, EMoveDir _climbDir );
    void            applyMove( f32 _dt, const class PolyLineEdge* _edge, f32& _move, f32& _moveSign, EMoveDir& _climbDir );
    void            adjustClimbT( const class PolyLine* _polyLine, const class PolyLineEdge* _edge, f32& _move );
    void            setAnimData( f32 _dt, const class PolyLineEdge* _edge, f32 _move, f32 _moveSign, Vec2d& _moveInput );

    EMoveDir        getClimbDirFromMoveSign( f32 _moveSign, bbool _vertical, const class PolyLineEdge* _edge ) const;
    f32             getClimbSignFromClimbDir( EMoveDir _moveDir, bbool _vertical, const class PolyLineEdge* _edge ) const;

    // jump
    void            processJump();

    // Slide damage
    void            doSlideDamage();

    enum ESwingDir
    {
        SWINGDIR_NONE,
        SWINGDIR_LEFT,
        SWINGDIR_RIGHT,
        ENUM_FORCE_SIZE_32(0)
    };

    static const f32 s_swingStopTimer;

    HangingInfo     m_hangInfo;
    f32             m_moveInertia;
    f32             m_prevMoveSign;
    f32             m_animRate;
    f32             m_stopSwingTimer;
    ESwingDir       m_swingDir;
    EMoveDir        m_prevClimbDir;
    bbool           m_inputVertical;
    bbool           m_animVertical;
    bbool           m_justEntered;
    bbool           m_allowDropVertical;    // We allow dropping when a player reached the end of the rope (for vertical)
                                            // We always allow dropping on horizontal
    bbool           m_allowExitHorizontal;  // Special flag to say if we allow unhanging from horizontal ropes. It's special
                                            // because it's handled from the idleToClimb state
};

#endif //_ITF_WAR_PCSTATECLIMBING_H_


