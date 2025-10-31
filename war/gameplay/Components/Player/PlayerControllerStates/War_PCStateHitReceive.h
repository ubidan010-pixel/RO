#ifndef _ITF_WAR_PCSTATEHITRECEIVE_H_
#define _ITF_WAR_PCSTATEHITRECEIVE_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHitReceive : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateHitReceive,War_State,3755744405)

public:

    StateHitReceive();
    virtual ~StateHitReceive() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update( f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes );
    virtual void    updateAnimInputs();

    void            restart();
    bbool           canRestart( War_EReceivedHitType _receiveHitType ) const;

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("RECEIVEHIT");

        return strAction;
    }

private:

    void            setupHit();
    void            setupNormalHit();
    void            setupEjectHit();
    void            setLandAnim( const PolyLineEdge* _edge );
    void            setCrashAnim( const PolyLine* _poly, const class PolyLineEdge* _edge, u32 _edgeIndex );
    bbool           checkExitOnAir();

    void            updateNormalPunch();
    bbool           checkFinishedNormalPunch();

    Vec2d           m_prevSpeedDir;
    f32             m_prevGravityMultiplier;
    f32             m_airMoveInitialSpeed;
    f32             m_receivedHitDirX;
    f32             m_animCursor;
    bbool           m_hitAirMove;
    bbool           m_hitOnAir;
    bbool           m_justEntered;
};

#endif //_ITF_WAR_PCSTATEHITRECEIVE_H_


