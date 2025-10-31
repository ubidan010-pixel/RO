#ifndef _ITF_RAY_PCSTATEHITPREPARE_H_
#define _ITF_RAY_PCSTATEHITPREPARE_H_

#ifndef _ITF_RAY_PCSTATEHITBASE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCStateHitBase.h"
#endif //_ITF_RAY_PCSTATEHITBASE_H_

class StateHitPrepare : public StateHitBase
{
    DECLARE_OBJECT_CHILD_RTTI(StateHitPrepare,StateHitBase,2530625326)
public:
    StateHitPrepare();
    ~StateHitPrepare() {}

    virtual void                onEnter();
    virtual bbool               checkStateChange( f32 _dt );
    virtual void                update(f32 _dt);
    virtual void                updateAnimInputs();

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(PREPAREHIT,213790770);
    }

protected:

    f32                         getStageLength() const;

    u32                         m_punchLevel;                   // Level of the punch we will send
    f32                         m_timeCounter;                  // The current time of the preparing
    bbool                       m_justEntered;                  // If we just entered the state
    bbool                       m_fromUTurn;                    // If we come from a uturn
};

#endif //_ITF_RAY_PCSTATEHITPREPARE_H_


