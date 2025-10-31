#ifndef _ITF_WAR_PCSTATEHITBASE_H_
#define _ITF_WAR_PCSTATEHITBASE_H_

#ifndef _ITF_WAR_PCSTATE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCState.h"
#endif //_ITF_WAR_PCSTATE_H_

class StateHitBase : public War_State
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(StateHitBase,War_State,676090084)

public:

    virtual void                onEnter();
    virtual void                update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void                onEvent(Event * _event);

private:

    virtual bbool               canUpdateLookDir() const { return btrue; }

    void                        updateStand(f32 _dt,const InputAdapter::PressStatus* _buttons);
    void                        updateHang(f32 _dt,const InputAdapter::PressStatus* _buttons);
    void                        updateClimb(f32 _dt,const InputAdapter::PressStatus* _buttons);
    void                        updateLookDir();

    void                        performHit();
    void                        setupEarthquake();
    void                        sendClimbImpulse( const Vec2d& _dir ) const;
    War_HitStim*                setupHitStim( bbool _flipped ) const;
    void                        setupHitStimReceivedHitType( War_HitStim* _stim ) const;
};

#endif //_ITF_WAR_PCSTATEHITBASE_H_


