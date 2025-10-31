#ifndef _ITF_WAR_PCSTATEHITRELEASE_H_
#define _ITF_WAR_PCSTATEHITRELEASE_H_

#ifndef _ITF_WAR_PCSTATEHITBASE_H_
#include "war/gameplay/Components/Player/PlayerControllerStates/War_PCStateHitBase.h"
#endif //_ITF_WAR_PCSTATEHITBASE_H_

class StateHitRelease : public StateHitBase
{
    DECLARE_OBJECT_CHILD_RTTI(StateHitRelease,StateHitBase,1800515954)
public:
    StateHitRelease();
    ~StateHitRelease() {}

    virtual void                onEnter();
    virtual void                onExit();
    virtual bbool               checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes );
    virtual void                update(f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes);
    virtual void                onEvent(Event * _event);

    void                        setUTurnTimer( f32 _timer ) { m_uturnTimer = _timer; }

    ITF_INLINE virtual StringID getAnimAction() const
    {
        static const StringID strAction("RELEASEHIT");
        return strAction;
    }

protected:

    virtual bbool               canUpdateLookDir() const { return bfalse; }

    bbool                       m_justEntered;              // If we just entered the state
    f32                         m_uturnTimer;               // We allow a small time to transform the hit into a uturn hit
    bbool                       m_canMove;                  // This flag is true if the anim marker that tell us that we can move again is reached
    bbool                       m_hitPerformed;             // If we performed the hit

    u32                         m_consecutivePunchCount;    // The number of consecutive punch (combo'ed or not)

    bbool                       m_comboWindowOpen;          // only used for debugging
    bbool                       m_canPlaceCombo;
    bbool                       m_comboQueued;
    u32                         m_comboStage;               // The number of consecutive combo punch
};

#endif //_ITF_WAR_PCSTATEHITRELEASE_H_


