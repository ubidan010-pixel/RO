#ifndef _ITF_RAY_PCSTATEJUMP_H_
#define _ITF_RAY_PCSTATEJUMP_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateJump : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI(StateJump,Ray_State,4001937309)
public:

    StateJump();
    ~StateJump() {}

    virtual void                onEnter();
    virtual void                onExit();
    virtual bbool               checkStateChange( f32 _dt );
    virtual void                update(f32 _dt);
    virtual void                updateAnimInputs();

    void                        restartJump();
    void                        setSkipForce( bbool _val ) { m_skipForce = _val; }
    bbool                       allowSuspension() const { return m_allowSuspension; }

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(JUMP,2374255179);
    }

private:

    void                        setupJump();
    bbool                       checkExitFromCurrent() const;

    AirPose                     m_airPose;                     // In what pose we are while jumping
    f32                         m_suspensionDelay;              // Delay to start applying suspension
    f32                         m_wallRunJumpCounter;           // For how long we keep the wall run jump
    f32                         m_suspensionPushCounter;        // Time the push button is held and suspension added
    f32                         m_queueCrushCounter;            // Counter to queue a crush attack
    bbool                       m_justJumped;                   // True if we just entered the state
    bbool                       m_skipForce;                    // If we don't want to apply forces
    bbool                       m_allowSuspension;              // If we allow suspension while jumping
    bbool                       m_jumpReleased;                 // If the jump button was released at any time
    bbool                       m_bouncing;                     // If we are in the middle of a bounce
};

#endif //_ITF_RAY_PCSTATEJUMP_H_


