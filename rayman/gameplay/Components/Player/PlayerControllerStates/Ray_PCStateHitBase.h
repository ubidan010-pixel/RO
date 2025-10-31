#ifndef _ITF_RAY_PCSTATEHITBASE_H_
#define _ITF_RAY_PCSTATEHITBASE_H_

#ifndef _ITF_RAY_PCSTATE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCState.h"
#endif //_ITF_RAY_PCSTATE_H_

class StateHitBase : public Ray_State
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(StateHitBase,Ray_State,676090084)

public:

    StateHitBase() {}

    virtual void                update(f32 _dt);

protected:
    virtual bbool               canUpdateLookDir() const { return btrue; }

    void                        updateStand(f32 _dt);
    void                        updateHang(f32 _dt);
    void                        updateClimb(f32 _dt);
    void                        updateSwim(f32 _dt);
    void                        updateLookDir();
    
};

#endif //_ITF_RAY_PCSTATEHITBASE_H_


