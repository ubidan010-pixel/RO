#ifndef _ITF_RAY_PCSTATEIDLE_H_
#define _ITF_RAY_PCSTATEIDLE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateIdle : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateIdle,Ray_State,671570437)

public:
    StateIdle();
    ~StateIdle() {}

    virtual void    onEnter();
    virtual void    onExit();
    virtual bbool   checkStateChange( f32 _dt );
    virtual void    update(f32 _dt);
    virtual void    updateAnimInputs();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(IDLE,1633200266);
    }

private:

    void            updateSpecialMoves( f32 _speedOnGround, bbool& exitCornerPose );
    void            updateAllowFlags();
    void            updatePathBlocked( f32 _dt );
    void            updateSupport( bbool& exitCornerPose );
    void            clearPush();
    bbool           canTryUnhangCorner() const;
    bbool           tryUnhangCorner();

    struct HitVictimData
    {
        ObjectRef m_victim;
        u32 m_edgeIndex;
    };

    typedef SafeArray <HitVictimData> HitVictimContainer;

    ObjectRef                       m_pushedObj;        // The object pushed
    f32                             m_pushCounter;
    f32                             m_repushCounter;
    f32                             m_keepPushDelay;

    bbool                           m_justEntered;      // We use this flag to delay the jump queue so that we can calculate the platform speed
    bbool                           m_allowUnhang;      // Pressing down from crouch to unhang from a polyline
    bbool                           m_allowCrouch;      // To allow the crouch we must stay for a frame not pressing down
    bbool                           m_allowSupport;     // If we allow support pressing up
};

#endif //_ITF_RAY_PCSTATEIDLE_H_

