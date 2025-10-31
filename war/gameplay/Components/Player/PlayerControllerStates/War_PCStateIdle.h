#ifndef _ITF_WAR_PCSTATEIDLE_H_
#define _ITF_WAR_PCSTATEIDLE_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateIdle : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateIdle,War_State,671570437)

    enum ECornerPos
    {
        CORNERPOS_NONE,
        CORNERPOS_BACK,
        CORNERPOS_FRONT,
        ENUM_FORCE_SIZE_32(1)
    };

public:
    StateIdle();
    ~StateIdle() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void    update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void    updateAnimInputs();

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("IDLE");

        return strAction;
    }

private:

    void            updateOnCorner();
    void            updateSpecialMoves( f32 _speedOnGround );
    void            updateAllowFlags();
    void            updatePathBlocked();

    ECornerPos      m_onCorner;         // The character is on the corner of a polyline
    bbool           m_justEntered;      // We use this flag to delay the jump queue so that we can calculate the platform speed
    bbool           m_allowUnhang;      // Pressing down from crouch to unhang from a polyline
    bbool           m_allowCrouch;      // To allow the crouch we must stay for a frame not pressing down
    bbool           m_isPathBlocked;    // If we can't walk because of an obstacle
};

#endif //_ITF_WAR_PCSTATEIDLE_H_

